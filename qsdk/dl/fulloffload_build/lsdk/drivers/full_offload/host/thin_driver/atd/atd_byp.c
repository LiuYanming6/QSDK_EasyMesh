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


#include "atd_internal.h"
#include <atd_appbr.h>
#include "atd_byp.h"
#include "adf_net.h"


/* Placeholder if any Tx filter is required in VLAN mode */
/* static adf_nbuf_t */
/* atd_byp_filter_vlan(void *ctx, struct atd_host_vap *vap, adf_nbuf_t skb); */

static adf_nbuf_t
atd_byp_filter_tunnel(void *ctx, struct atd_host_vap *vap, adf_nbuf_t skb);

/** 
 * @brief 
 * 
 * @return 
 */
byp_handle_t
atd_byp_init(void *atd_handle)
{

    appbr_sc_t      *appbr_sc    =   NULL;
    byp_sc_t        *byp_sc;
    appbr_cb_t      *cb;
    host_br_addr_t  *addr_list;

    appbr_sc    =   ATD_APPBR_HDL(atd_handle);
    byp_sc = adf_os_mem_alloc(NULL, sizeof(struct byp_sc_s));
    if(byp_sc == NULL) {
        adf_os_print("APPBR device not created\n");
        return NULL;
    }
    adf_os_mem_zero(byp_sc, sizeof(struct byp_sc_s));

    BYPSC_ATDHDL(byp_sc)    =   atd_handle;

    cb                      =   adf_os_mem_alloc(NULL, sizeof(struct appbr_cb_s));
    if(cb == NULL) {
        adf_os_print("atd_byp_init cb not created\n");
        adf_os_mem_free(byp_sc);
        return NULL;
    }
    adf_os_mem_zero(cb, sizeof(struct appbr_cb_s));

    BYPSC_CB(byp_sc)  =   cb;

    cb->fn      =   atd_byp_cb;
    cb->ctx     =   byp_sc;

    atd_appbr_register_cb(appbr_sc, cb, APPBR_BYP);

    addr_list               =   adf_os_mem_alloc(NULL, sizeof(struct host_br_addr_s));

    if(cb == NULL) {
        adf_os_print("atd_byp_init host_addr_list not created\n");
        adf_os_mem_free(BYPSC_CB(byp_sc));
        adf_os_mem_free(byp_sc);
        return NULL;
    }

    adf_os_mem_zero(addr_list, sizeof(struct host_br_addr_s));
    BYPSC_HSTADDR_TBL(byp_sc)  =   addr_list; 

    return (byp_sc);
}

/** 
 * @brief   
 * 
 * @param ctx
 */
void
atd_byp_cleanup(void *ctx)
{
    byp_sc_t        *byp_sc;

    appbr_sc_t      *appbr_sc = NULL;


    byp_sc = (byp_sc_t *) ctx;

    if(BYPSC_ATDHDL(byp_sc))
        appbr_sc    =   (BYPSC_ATDHDL(byp_sc))->appbr_handle;

    if(appbr_sc)
    {
        /** 
         * Deregister and Free the Callback Function
         */
        BYPSC_CB(byp_sc)->fn      =   NULL;
        BYPSC_CB(byp_sc)->ctx     =   NULL;
        atd_appbr_register_cb(appbr_sc, BYPSC_CB(byp_sc), APPBR_BYP);
    }

    adf_os_mem_free(BYPSC_HSTADDR_TBL(byp_sc));
    adf_os_mem_free(BYPSC_CB(byp_sc));
    adf_os_mem_free(byp_sc);

    return;
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
atd_byp_seten(void *ctx, byp_stat_t *buf)
{

    byp_sc_t * byp_sc       = (byp_sc_t *) ctx;

    BYPSC_BYPMODE(byp_sc)   =   buf->byp_status;

    switch(buf->byp_status) {
        case ATD_LANBYP_DIS:
            BYPSC_FILTER(byp_sc) =   NULL;
            break;
        case ATD_LANBYP_TUNNEL:
            BYPSC_FILTER(byp_sc) =   atd_byp_filter_tunnel;
            break;
        case ATD_LANBYP_VLAN:
            /* BYPSC_FILTER(byp_sc) =   atd_byp_filter_vlan; */
            BYPSC_FILTER(byp_sc) =   NULL;
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
static  a_status_t
atd_byp_addmac(void *ctx, byp_br_t *buf)
{

    byp_sc_t        *byp_sc         = (byp_sc_t * ) ctx;
    host_br_addr_t  *addr_list      = (host_br_addr_t  *) BYPSC_HSTADDR_TBL(byp_sc);

    a_uint32_t  i       =   0;

    for(i = 0; i < MAX_HOST_ADDR; i++) 
    {
        if(!(addr_list->vld_mask & (1 << i)))
            break;
    }

    if(i == MAX_HOST_ADDR)
    {
        adf_os_print("Host Entry Limit Max Limit Reached %d\n",i);
        return A_STATUS_FAILED;
    }

    addr_list->vld_mask |= (1 << i);

    adf_os_mem_copy(addr_list->addr[i], buf->br_addr, ACFG_MACADDR_LEN);

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
atd_byp_delmac(void *ctx, byp_br_t *buf)
{

    a_uint32_t  i       =   0;
                

    byp_sc_t        *byp_sc     =   (byp_sc_t * ) ctx;
    host_br_addr_t  *addr_list  =   (host_br_addr_t  *) BYPSC_HSTADDR_TBL(byp_sc);


    for(i = 0;i < MAX_HOST_ADDR; i++)
    {
        if((adf_net_cmp_ether_addr(addr_list->addr[i], buf->br_addr) 
                    == A_TRUE))
            break;
    }

    if(i == MAX_HOST_ADDR)
    {
        adf_os_print("atd_by_del_mac entry not found \n");
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
atd_byp_cb(void * ctx, adf_nbuf_t netbuf)
{

    a_uint8_t   cmd_id, node_id, *netbuf_data;
    a_uint32_t  netbuf_len; 
    a_status_t  status = A_STATUS_OK;

    byp_common_t *cmd_hdr;

    /** 
     * Note that this function only reads the SKB.
     * So we do not clone/copy SKB, we just read the necessary data
     * and pass the same SKB further
     */
    adf_nbuf_peek_header(netbuf, &netbuf_data, &netbuf_len);

    cmd_hdr     =   (byp_common_t *) netbuf_data;

    cmd_id      =   cmd_hdr->data[1];
    node_id     =   cmd_hdr->data[0] & BYP_CMD_NODE_MASK;

    /* Check if kernel needs to process this command  */
    if(node_id != BYP_NODE_ALL)
        return A_STATUS_OK;

    switch(cmd_id) {
        case  BYP_CMD_VAPCTRL:
            status  =   A_STATUS_OK;
            break;

        case  BYP_CMD_BYPEN:
            status  =   atd_byp_seten(ctx, (byp_stat_t *) netbuf_data);
            break;

        case  BYP_CMD_ADDMAC:
            status  =   atd_byp_addmac(ctx, (byp_br_t *) netbuf_data);
            break;

        case  BYP_CMD_DELMAC:
            status  =   atd_byp_delmac(ctx, (byp_br_t *) netbuf_data);
            break;

        case BYP_CMD_VLGRPCREATE:
        case BYP_CMD_VLGRPADDVAP:
        case BYP_CMD_VLGRPDELVAP:
        case BYP_CMD_BRLISTMAC:
            break;

        default:
            adf_os_print("%d:cmd , illegal node id \n",cmd_id); 
            status = A_STATUS_FAILED;
    }

    return status;
}

/** 
 * @brief MAC Addr Filter to check if the Packet has originated from Host 
 * 
 * @param skb
 * 
 * @return 
 */
a_uint32_t 
atd_byp_from_host(void *ctx, struct atd_host_vap *vap, adf_nbuf_t skb)
{

    adf_net_ethhdr_t *eh;

    byp_sc_t         *byp_sc    = (byp_sc_t * ) ctx;
    host_br_addr_t   *addr_list = (host_br_addr_t  *) BYPSC_HSTADDR_TBL(byp_sc);

    a_uint32_t          i           =   0,
                        mask        =   0;

    eh      =   (adf_net_ethhdr_t *) skb->data;

    mask    =   addr_list->vld_mask;

    /* Check if it is from VAP Gateway */
    if(!adf_net_cmp_ether_addr(skb->dev->dev_addr, eh->ether_shost)) 
        return 1;

    for(i = 0;  (i < MAX_HOST_ADDR) || (mask != 0); i++)
    {
        if(mask & 1)
        {
            if(!adf_net_cmp_ether_addr(addr_list->addr[i], eh->ether_shost)) 
                return 1;
        }

        mask>>=1;
    }

    return 0;
}

a_uint32_t 
atd_byp_mode(void *ctx) {

    byp_sc_t        *byp_sc     =   (byp_sc_t * ) ctx;

    if(!byp_sc)
        return  ATD_LANBYP_DIS;
    else
        return  BYPSC_BYPMODE(byp_sc);

}

/* 
 * @brief 
 */
static adf_nbuf_t
atd_byp_filter_tunnel(void *ctx, struct atd_host_vap *vap, adf_nbuf_t skb)
{
    if(atd_byp_is_mcast(skb)) 
    {
        /** Block all multicast packets in ATD path , 
         * if VAP is part of LAN Bridge */
        if(vap->lan_en){
	    adf_nbuf_free(skb);
            return NULL;
        }
        else {
            /** If VAP is not part of LAN Bridge ,block only multicasts 
             * not originating on Host*/
            if(!atd_byp_from_host(ctx, vap, skb)){
		adf_nbuf_free(skb);
                return NULL;
            }
        }
    }

    return skb;
}

/* 
 * @brief  Placeholder if any Tx filter is required in VLAN mode 
 */
/* static adf_nbuf_t */
/* atd_byp_filter_vlan(void *ctx, struct atd_host_vap *vap, adf_nbuf_t skb) */
/* { */
/* return skb; */
/* } */
