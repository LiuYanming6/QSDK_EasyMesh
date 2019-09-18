/*
 * Copyright (c) 2010, Atheros Communications Inc.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *  
 */ 


#include <linux/skbuff.h>
#include <linux/netdevice.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/rtnetlink.h>
#include <linux/wireless.h>
#include <linux/netlink.h>
#include <net/iw_handler.h>
#include <linux/etherdevice.h>
#include <net/sock.h>

#include <a_base_types.h>
#include <hif_api.h>
#include <htc_api.h>
#include <wmi_api.h>
#include <atd_wsupp.h>
#include <atd_hdr.h>
#include <atd_cmd.h>
#include <atd_internal.h>
#include <atd_appbr.h>
#include <atd_byp.h>

#ifdef GMAC_8021Q_SUPPORT
int
hif_gmac_vlan_addvap(void  *handle,a_uint16_t vlan_id, 
                     struct net_device *vap_netdev);
int
hif_gmac_vlan_delvap(void  *handle,a_uint16_t vlan_id, 
                     struct net_device *vap_netdev);
int
hif_gmac_vlan_wlan_rx(void  *handle, struct sk_buff *skb, a_uint16_t vlan_id);

int
hif_gmac_vlan_create(void  *handle,a_uint16_t vlan_id);
#endif

static struct sk_buff *
atd_byp_filter_tunnel(void *ctx, struct atd_tgt_vap *vap, struct sk_buff *skb);

static struct sk_buff *
atd_byp_filter_vlan(void *ctx, struct atd_tgt_vap *vap, struct sk_buff *skb);
/** 
 * @brief 
 * 
 * @return 
 */
byp_handle_t
atd_tgt_byp_init(void *atd_handle)
{

    appbr_sc_t      *appbr_sc    =  ATD_APPBR_HDL(atd_handle);
    byp_sc_t        *byp_sc;
    appbr_cb_t      *cb;
    host_br_addr_t  *addr_list;

    byp_sc  = kzalloc(sizeof(byp_sc_t), GFP_KERNEL);

    if(byp_sc == NULL) {
        printk("BYP device not created\n");
        return NULL;
    }

    cb                  =   kzalloc(sizeof(appbr_cb_t), GFP_KERNEL);

    if(cb == NULL) {
        kfree(byp_sc);
        printk("BYP CB not created\n");
        return NULL;
    }

    cb->fn              =   atd_tgt_byp_cb;
    cb->ctx             =   byp_sc;

    BYPSC_CB(byp_sc)      =   cb;
    BYPSC_ATDHDL(byp_sc)  =   atd_handle;

    BYPSC_BYPMODE(byp_sc)   =   ATD_LANBYP_DIS;
    BYPSC_FILTER(byp_sc)    =   NULL;

    atd_tgt_appbr_register_cb(appbr_sc, cb, APPBR_BYP);

    addr_list   =   kzalloc(sizeof(struct host_br_addr_s), GFP_KERNEL);

    if(addr_list == NULL) {
        kfree(byp_sc);
        kfree(cb);
        printk("BYP Host Address Table not created\n");
        return NULL;
    }

    BYPSC_HSTADDR_TBL(byp_sc)  =   addr_list;

    return (byp_sc);
}

void
atd_tgt_byp_cleanup(void *ctx)
{

    byp_sc_t * byp_sc = (byp_sc_t * ) ctx;

    appbr_sc_t      *appbr_sc = NULL;

    if(BYPSC_ATDHDL(byp_sc))
        appbr_sc    =   (BYPSC_ATDHDL(byp_sc))->appbr_handle;

    /* free the memory allocated for Host Address Table */
    kfree(BYPSC_HSTADDR_TBL(byp_sc));

    if(appbr_sc)
    {
        BYPSC_CB(byp_sc)->fn      = NULL;
        BYPSC_CB(byp_sc)->ctx     = NULL;

        /* Deregister the APPBR Callback */
        atd_tgt_appbr_register_cb(appbr_sc, BYPSC_CB(byp_sc), APPBR_BYP);
    }

    /* free the memory allocated for Callback */
    kfree(BYPSC_CB(byp_sc));

    /* Atlast free the memory allocated for the softc */
    kfree(byp_sc);

    return;
}

/** 
 * @brief 
 * 
 * @param ctx
 * @param buf
 * 
 * @return 
 */
static a_status_t
atd_tgt_byp_vapctrl(void *ctx, byp_vapctrl_t *buf)
{

    struct net_device   *dev;
    atd_tgt_vap_t       *vap;

    byp_sc_t *byp_sc    =   (byp_sc_t *) ctx;

    dev = atd_netdev_get_by_name(buf->if_name);
    vap = atd_tgt_find_vap(BYPSC_ATDHDL(byp_sc), dev);

    if(vap == NULL)
        return A_STATUS_FAILED;

    if(buf->ctrl & BYP_VAPCTRL_SETMASK)
    {
        if((buf->ctrl & BYP_VAPCTRL_ACCMASK) == BYP_VAPCTRL_BR_EN)
            vap->byp_flags  =  BYP_VAPCTRL_BR_EN;       

        if((buf->ctrl & BYP_VAPCTRL_ACCMASK) == BYP_VAPCTRL_LAN_EN)
        {
            vap->byp_flags  =  BYP_VAPCTRL_LAN_EN;       
            /** Send WMI event to Host to set LAN Access Flag on Host Side */
            atd_tgt_vap_event(vap, WMI_EVENT_BYP_VAP_LANEN);
        }
    }
    else 
    {
        vap->byp_flags =  0;

        if((buf->ctrl & BYP_VAPCTRL_ACCMASK) == BYP_VAPCTRL_LAN_EN)
        {
            /** Send WMI event to Host to reset LAN Access Flag on Host Side */
            atd_tgt_vap_event(vap, WMI_EVENT_BYP_VAP_LANDIS);
        }
    }

    dev_put(dev);
    return A_STATUS_OK;

}

#ifdef GMAC_8021Q_SUPPORT
static int
atd_atoi(char c)
{
    if ( c >= '0' && c <= '9' )
        return ( c - '0');
    else
        return 0;
}


/** 
 * @brief 
 * 
 * @param ctx
 * @param buf
 * 
 * @return 
 */
static a_status_t
atd_tgt_byp_vlgrpcreate(void *ctx, byp_vlan_t* buf)
{

    struct net_device   *dev;
    atd_tgt_vap_t       *vap;
    a_uint16_t          vlan_id;
    char                *p_vlid;


    byp_sc_t *byp_sc    = (byp_sc_t *) ctx;

    p_vlid              = buf->vlan_id;
    vlan_id             = 0;

    while(*p_vlid)
    {
        vlan_id = (10*vlan_id) + atd_atoi(*p_vlid);
        *p_vlid++;
    }
   
    hif_gmac_vlan_create(BYPSC_ATDHDL(byp_sc)->hif_handle, vlan_id);

    return A_STATUS_OK;

}

#endif
/** 
 * @brief 
 * 
 * @param ctx
 * @param buf
 * 
 * @return 
 */
static a_status_t
atd_tgt_byp_vlgrpadd(void *ctx, byp_vlan_t* buf)
{

    struct net_device   *dev;
    atd_tgt_vap_t       *vap;
#ifdef GMAC_8021Q_SUPPORT
    a_uint16_t          vlan_id;
    char                *p_vlid;
#endif

    byp_sc_t *byp_sc    =   (byp_sc_t *) ctx;

    dev = atd_netdev_get_by_name(buf->if_name);
    vap = atd_tgt_find_vap(BYPSC_ATDHDL(byp_sc), dev);

    if(vap == NULL)
        return A_STATUS_FAILED;

#ifdef GMAC_8021Q_SUPPORT
    p_vlid              = buf->vlan_id;
    vlan_id             = 0;

    while(*p_vlid)
    {
        vlan_id = (10*vlan_id) + atd_atoi(*p_vlid);
        *p_vlid++;
    }
    vap->vlan_id = vlan_id;

    hif_gmac_vlan_addvap(BYPSC_ATDHDL(byp_sc)->hif_handle, vlan_id, dev);
#endif

    /* vap->byp_flags  =  BYP_VAPCTRL_LAN_EN; */
    /** Send WMI event to Host to set LAN Access Flag on Host Side */
    /* atd_tgt_vap_event(vap, WMI_EVENT_BYP_VAP_LANEN); */

    dev_put(dev);
    return A_STATUS_OK;

}

/** 
 * @brief 
 * 
 * @param ctx
 * @param buf
 * 
 * @return 
 */
static a_status_t
atd_tgt_byp_vlgrpdel(void *ctx, byp_vlan_t* buf)
{
    struct net_device   *dev;
    atd_tgt_vap_t       *vap;
#ifdef GMAC_8021Q_SUPPORT
    a_uint16_t          vlan_id;
    char                *p_vlid;
#endif

    byp_sc_t *byp_sc    =   (byp_sc_t *) ctx;

    dev = atd_netdev_get_by_name(buf->if_name);
    vap = atd_tgt_find_vap(BYPSC_ATDHDL(byp_sc), dev);

    if(vap == NULL)
        return A_STATUS_FAILED;

#ifdef GMAC_8021Q_SUPPORT
    p_vlid              = buf->vlan_id;
    vlan_id             = 0;

    while(*p_vlid)
    {
        vlan_id = (10*vlan_id) + atd_atoi(*p_vlid);
        *p_vlid++;
    }
    vap->vlan_id = vlan_id;

    hif_gmac_vlan_delvap(BYPSC_ATDHDL(byp_sc)->hif_handle, vlan_id, dev);
#endif

    vap->byp_flags =  0;

    /** Send WMI event to Host to reset LAN Access Flag on Host Side */
    atd_tgt_vap_event(vap, WMI_EVENT_BYP_VAP_LANDIS);

    dev_put(dev);
    return A_STATUS_OK;
}

/** 
 * @brief 
 * 
 * @param ctx
 * @param bypstatus
 * 
 * @return 
 */
static a_status_t
atd_tgt_byp_seten(void *ctx, byp_stat_t *buf)
{

    byp_sc_t * byp_sc       = (byp_sc_t *) ctx;

    BYPSC_BYPMODE(byp_sc)   =   buf->byp_status;

    switch(buf->byp_status) {
        case ATD_LANBYP_DIS:
            BYPSC_FILTER(byp_sc) =   NULL;
            byp_dbg_printk("LAN Bypass Mode disabled on Tgt \n");
            break;
        case ATD_LANBYP_TUNNEL:
            BYPSC_FILTER(byp_sc) =   atd_byp_filter_tunnel;
            byp_dbg_printk("LAN Bypass Mode set to TUNNEL on Tgt \n");
            break;
        case ATD_LANBYP_VLAN:
            BYPSC_FILTER(byp_sc) =   atd_byp_filter_vlan;
            byp_dbg_printk("LAN Bypass Mode set to VLAN on Tgt \n");
            break;
    }

    return A_STATUS_OK;

}

/** 
 * @brief 
 * 
 * @param ctx
 * @param byp_br
 * 
 * @return 
 */
static a_status_t 
atd_tgt_byp_addmac(void *ctx, byp_br_t *buf)
{

    byp_sc_t        *byp_sc       = (byp_sc_t * ) ctx;
    host_br_addr_t  *addr_list    = (host_br_addr_t  *) BYPSC_HSTADDR_TBL(byp_sc);

    a_uint32_t  i       =   0;

    for(i = 0;i < MAX_HOST_ADDR; i++)
    {
        if(!(addr_list->vld_mask & (1 << i)))
            break;
    }

    if(i == MAX_HOST_ADDR)
    {
        printk("Host Entry Limit Max Limit Reached %d\n",i);
        return A_STATUS_FAILED;
    }

    addr_list->vld_mask |= (1 << i);

    memcpy(addr_list->addr[i], buf->br_addr, ACFG_MACADDR_LEN);

    return A_STATUS_OK;
}

/** 
 * @brief 
 * 
 * @param ctx
 * @param byp_br
 * 
 * @return 
 */
static a_status_t
atd_tgt_byp_delmac(void *ctx, byp_br_t *buf)
{

    a_uint32_t  i       =   0;
                

    byp_sc_t        *byp_sc     =   (byp_sc_t * ) ctx;
    host_br_addr_t  *addr_list  =   (host_br_addr_t  *) BYPSC_HSTADDR_TBL(byp_sc);


    for(i = 0;i < MAX_HOST_ADDR; i++)
    {
        if((compare_ether_addr(addr_list->addr[i], buf->br_addr) == 0))
            break;
    }

    if(i == MAX_HOST_ADDR)
    {
        printk("atd_by_del_mac entry not found \n");
        return A_STATUS_FAILED;
    }

    addr_list->vld_mask &= ~(1 << i);

    return A_STATUS_OK;
}



/** 
 * @brief 
 * 
 * @param ctx
 * @param netbuf
 * 
 * @return 
 */
a_status_t
atd_tgt_byp_cb(void * ctx, struct sk_buff *skb)
{

    a_uint8_t   cmd_id, node_id, *netbuf_data, netbuf_len;

    a_uint32_t  status = A_STATUS_OK;

    byp_common_t *cmd_hdr;

    /** 
     * Note that this function only reads the SKB.
     * So we do not clone/copy SKB, we just read the necessary data
     * and pass the same SKB further
     */
    netbuf_data = skb->data;
    netbuf_len  = skb->len;


    cmd_hdr     =   (byp_common_t *) netbuf_data;

    cmd_id      =   cmd_hdr->data[1];
    node_id     =   cmd_hdr->data[0] & BYP_CMD_NODE_MASK;

    /* Check if kernel needs to process this command  */
    if(node_id != BYP_NODE_ALL)
        return A_STATUS_OK;

    byp_dbg_printk("Inside atd_tgt_byp_cb:");
    byp_dbg_printk("cmd = %x, node = %x \n",cmd_id, node_id);

    switch(cmd_id) 
    {

        case  BYP_CMD_VAPCTRL:
            status  =   atd_tgt_byp_vapctrl(ctx, (byp_vapctrl_t*) netbuf_data);
            break;

        case  BYP_CMD_BYPEN:
            status  =   atd_tgt_byp_seten(ctx, (byp_stat_t*) netbuf_data);
            break;

        case  BYP_CMD_ADDMAC:
            status  =   atd_tgt_byp_addmac(ctx, (byp_br_t *) netbuf_data);
            break;

        case  BYP_CMD_DELMAC:
            status  =   atd_tgt_byp_delmac(ctx, (byp_br_t *) netbuf_data);
            break;

        case BYP_CMD_VLGRPCREATE:
#ifdef GMAC_8021Q_SUPPORT
            status  =   atd_tgt_byp_vlgrpcreate(ctx, (byp_vlan_t*) netbuf_data);
#else
            status = A_STATUS_OK;
#endif
            break;

        case  BYP_CMD_VLGRPADDVAP:
            status  =   atd_tgt_byp_vlgrpadd(ctx, (byp_vlan_t*) netbuf_data);
            break;

        case  BYP_CMD_VLGRPDELVAP:
            status  =   atd_tgt_byp_vlgrpdel(ctx, (byp_vlan_t*) netbuf_data);
            break;

        case  BYP_CMD_BRLISTMAC:
            /* Dont do anything. Host Prints all the MAC Addresses */
            break;

        default:
            printk("%d:cmd , illegal node id \n",cmd_id); 
            return A_STATUS_FAILED;
    }

    return status;
}

a_uint32_t 
atd_tgt_byp_from_host(struct sk_buff *skb, struct net_device   *vap_netdev)
{
    return 0;
}

a_uint32_t 
atd_tgt_byp_for_host(void *ctx, struct atd_tgt_vap   *vap, struct sk_buff *skb)
{

    struct ethhdr   *eh;
    byp_sc_t        *byp_sc      = (byp_sc_t * ) ctx;
    host_br_addr_t  *addr_list   = (host_br_addr_t  *) BYPSC_HSTADDR_TBL(byp_sc);

    a_uint32_t              i       =   0,
                            mask    =   0;

    eh      =   (struct ethhdr *) skb->data;
    mask    =   addr_list->vld_mask;

    /* Check if it is destined for VAP  */
    /* this is not needed now that we explicitly check if VAP is part of any 
     * bridge */
    /* if(compare_ether_addr(vap->mac_addr,eh->h_dest) == 0) */
    /* return 1; */

    for(i   =   0;  i <   MAX_HOST_ADDR;    i++)
    {
        if(!mask)
            return 0;

        if(mask & 1)
        {
            if(compare_ether_addr(addr_list->addr[i],   eh->h_dest) == 0)
                return 1;
        }

        mask>>=1;
    }

    return 0;
}



/** 
 * @brief 
 * 
 * @return 1, if LAN Bypass Mode is enabled, 
 *         0, if disabled 
 */
a_uint32_t 
atd_tgt_byp_mode(void *ctx) 
{

    byp_sc_t    *byp_sc =   (byp_sc_t * ) ctx;

    if(!byp_sc)
        return  ATD_LANBYP_DIS;
    else
        return  BYPSC_BYPMODE(byp_sc);

}

/** 
 * @brief 
 * 
 * @param ctx
 * @param vap
 * @param skb
 * 
 * @return 
 */
static struct sk_buff *
atd_byp_filter_vlan(void *ctx, struct atd_tgt_vap *vap, struct sk_buff *skb)
{

    struct ethhdr       *eh;
    struct net_device   *dev    =  skb->dev;
    byp_sc_t        *byp_sc      = (byp_sc_t * ) ctx;

    /* 
    if (vap->byp_flags == 0)
    {
        byp_dbg_printk("nbr");
        return  skb;
    } 
    */
#ifndef ETH_P_WAI
#define ETH_P_WAI    0x88b4 /* WAI/WAPI */
#endif

    eh      =   (struct ethhdr *) skb->data;

    if(eh->h_proto != htons(ETH_P_PAE) && eh->h_proto != htons(ETH_P_WAI))
    {
        byp_dbg_printk("n");
#ifdef GMAC_8021Q_SUPPORT
        hif_gmac_vlan_wlan_rx(BYPSC_ATDHDL(byp_sc)->hif_handle, skb, vap->vlan_id);
#else
        skb->protocol = eth_type_trans(skb, dev); 
        netif_rx(skb); 
        dev->last_rx = jiffies; 
#endif

        return NULL;
    }

    byp_dbg_printk("eap");
    return  skb;
}

static struct sk_buff *
atd_byp_filter_tunnel(void *ctx, struct atd_tgt_vap *vap, struct sk_buff *skb)
{

    struct sk_buff      *skb2   =   NULL;

    struct net_device   *dev     =  skb->dev;

    /* If VAP is not part of any bridge , do not deliver to OS */
    if (vap->byp_flags == 0)
    {
        byp_dbg_printk("nbr");
        return  skb;
    }

    /* Give the packet back to ATD, if the packet destination is Host */
    if(atd_tgt_byp_for_host(ctx, vap, skb)) 
    {
        byp_dbg_printk("h");
        return skb;
    }

    /* If VAP is part of VAP bridge , 
     * deliver to both OS (skb2) and ATD (skb) */
    if (vap->byp_flags == BYP_VAPCTRL_BR_EN)
    {
        if(atd_tgt_byp_is_mcast(skb)) /*Will take care of both MCAST & BCAST */
        {
            if ((skb2 = skb_copy(skb, GFP_ATOMIC)) == NULL) 
            { 
                /* If SKB Allocation Fails , print a msg and 
                 * give packet back to ATD*/
                printk("atd_byp_deliver_bypass: skb_copy fail \n");
                return  skb;
            }

            /* Send the copy to OS Network Stack, 
             * and give the original Packet back to ATD */
            byp_dbg_printk("B");
            skb2->protocol = eth_type_trans(skb2, dev);
            netif_rx(skb2);
            dev->last_rx = jiffies;
            return  skb;
        }
    }

    /* Indicate the packet to OS N/W Stack */
    byp_dbg_printk("b");
    skb->protocol = eth_type_trans(skb, dev);
    netif_rx(skb);
    dev->last_rx = jiffies;

    return NULL;
}
