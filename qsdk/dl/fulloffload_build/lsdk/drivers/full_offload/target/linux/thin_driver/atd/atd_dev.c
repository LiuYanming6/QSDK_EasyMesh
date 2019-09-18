/*
 * Copyright (c) 2008-2010, Atheros Communications Inc.
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
 */

#include <linux/skbuff.h>
#include <linux/netdevice.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/rtnetlink.h>
#include <linux/wireless.h>
#include <linux/etherdevice.h>
#include <net/iw_handler.h>

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



#ifdef ATD_DEBUG_ENABLE
int atd_dbg_mask ;
module_param(atd_dbg_mask, int , 0600);
#endif

atd_tgt_dev_t *g_atd_dev ;
int  max_num_radio = 1;

module_param(max_num_radio, int , 0600);

MODULE_AUTHOR("Atheros Communications, Inc.");
MODULE_DESCRIPTION("Atheros Device Module");
MODULE_LICENSE("Dual BSD/GPL");

int
hif_gmac_vlan_delvap(void  *handle,a_uint16_t vlan_id,
                     struct net_device *vap_netdev);

int     atd_netdev_notifier(struct notifier_block  *nb, unsigned long event, 
                         void  *dev);

struct notifier_block    atd_netdev_notify = {
        .notifier_call = atd_netdev_notifier,
};

static a_int32_t   atd_tgt_module_init( void );
static void        atd_tgt_module_exit(void );


static a_status_t atd_tgt_wifi_avail_event(atd_tgt_dev_t *atsc,
                                           struct net_device *netdev);

static void      atd_tgt_register_services(atd_tgt_dev_t * atsc );
static void      atd_tgt_htc_sendcomplete(void *ServiceCtx, 
                                          struct sk_buff * buf,
                                          htc_endpointid_t Endpt );
static void      atd_tgt_htc_recv_message(void *ServiceCtx, 
                                          struct sk_buff * buf,
                                          htc_endpointid_t Endpt );
static a_int32_t atd_tgt_device_detect( hif_handle_t hif_handle);
static a_int32_t atd_tgt_device_disconnect( void * context);



static a_int32_t 
atd_tgt_module_init( void )
{
    hif_os_callback_t cb;
    printk("Atheros Fulloffload Target Loaded \n");
    cb.device_detect      = atd_tgt_device_detect;
    cb.device_disconnect = atd_tgt_device_disconnect;
    cb.device_suspend    = NULL;
    cb.device_resume     = NULL;
    hif_init(&cb);
    return 0;
}


static a_int32_t
atd_tgt_device_detect( hif_handle_t hif_handle)
{
 
    atd_tgt_dev_t * atsc =NULL;
    htc_config_t htc_conf;
    a_status_t status = A_STATUS_OK;

    atd_trace(ATD_DEBUG_FUNCTRACE,(" Start .."));


    printk("module init ");

    atsc = kzalloc(sizeof(atd_tgt_dev_t), GFP_KERNEL);
    g_atd_dev = atsc ;

    if(atsc ==  NULL ){
        printk(" Soft C allocation Failed \n");
        status = A_STATUS_FAILED;
        goto fail;
    }


    g_atd_dev = atsc ;
    atsc->hif_handle = hif_handle;
    hif_claim_device(hif_handle, (void *)atsc);
    atsc->wifi_cnt =0;
    atsc->wifiphy_max_cnt =  max_num_radio;

    htc_conf.hif_handle = atsc->hif_handle;
    htc_conf.ctx   = (void *)atsc;

    atsc->htc_handle = htc_create(&htc_conf);

    /*Register The Allowed service  we can register all service out of i
      which WASP uses few */

    atd_tgt_register_services(atsc);

    atd_tgt_wmi_init(atsc);

    /* register wsupp message helper */
    atsc->wsupp_handle = atd_tgt_wsupp_helper_init(atsc, atsc->htc_handle);
    if (atsc->wsupp_handle == NULL) {
        printk("wsupp helper init failed\n");
        status  =   A_STATUS_FAILED;
        goto fail;
    }

    register_netdevice_notifier(&atd_netdev_notify);

    atsc->appbr_handle  =   atd_tgt_appbr_init(atsc, APPBR_NETLINK_NUM);

    if (atsc->appbr_handle == NULL) {
        printk("Application Bridge Module init failed\n");
        status  =   A_STATUS_FAILED;
        goto fail;
    }

    atsc->byp_handle    =   atd_tgt_byp_init(atsc);

    if (atsc->byp_handle == NULL) {
        printk("Bypass Module Init failed\n");
        status  =   A_STATUS_FAILED;
        goto fail;
    }
    printk("Target Iniitialized \n");
fail:

    atd_trace(ATD_DEBUG_FUNCTRACE,(" End .."));
    return status;

}
static a_int32_t
atd_tgt_device_disconnect( void * context)
{
   printk("hif device disconnect event \n");
   return 0;
}

/** 
 * @brief Wifi netdevice notification handler
 * 
 * @param dev
 * @param dev_event
 */
static int
atd_wifi_notifier(struct net_device     *dev, int  dev_event)
{
    int status = NOTIFY_OK;
    atd_tgt_wifi_t *wifi;

    atd_assert(g_atd_dev);

    atd_trace(ATD_DEBUG_FUNCTRACE, (".START"));
    switch (dev_event) {
        case NETDEV_REGISTER:
            atd_tgt_wifi_avail_event(g_atd_dev, dev);
            printk ("%s register notification\n", dev->name);
            break;
        case NETDEV_UNREGISTER:
            wifi = atd_tgt_find_wifi(g_atd_dev, dev);
	    atd_assert(wifi);
 	
            atd_tgt_wifi_event(wifi, WMI_EVENT_DELETE_WIFI);
            g_atd_dev->wifi_cnt--;
            g_atd_dev->wifi_unregister = 1;
            kfree(wifi);
            printk("%s unregister notification\n", dev->name);
            break;
        default:
            printk("Ignoring %s notification %d\n", 
                    dev->name, dev_event);
            status = NOTIFY_DONE;
            break;
    }

    atd_trace(ATD_DEBUG_FUNCTRACE, (".END"));

    return status;
}

static a_status_t 
atd_tgt_vap_register(atd_tgt_dev_t *atsc,struct net_device *netdev)
{
    a_status_t status = A_STATUS_OK;
    atd_tgt_vap_t    *vap;
    atd_tgt_wifi_t     *wifisc = NULL;
    int i;

    atd_trace(ATD_DEBUG_FUNCTRACE,(" Start .."));
    atd_trace(ATD_DEBUG_FUNCTRACE, ("%s : : %s max %d "\
                "detected %d \n ",__func__,netdev->name\
                ,atsc->wifiphy_max_cnt,atsc->wifi_cnt));

    vap = kzalloc(sizeof(atd_tgt_vap_t), GFP_KERNEL);
    if(vap == NULL ){
        atd_trace(ATD_DEBUG_CFG, ("Unable to allocate memory \n"));
        status = A_STATUS_ENOMEM;
        return status;
    }
    for (i = 0; i< MAX_NUM_RADIO; i++) {
        if (atsc->wifi_list[i] != NULL &&
            (!memcmp(&(atsc->wifi_list[i]->mac_addr[1]), &(netdev->dev_addr[1]), ACFG_MACADDR_LEN-1)))
	{
                wifisc = atsc->wifi_list[i];
		if(i)
	            netdev->irq=0x80;
		else
	            netdev->irq=0;
        }
    }
    if(wifisc == NULL ){
        atd_trace(ATD_DEBUG_CFG, ("Unable to find wifi\n"));
        status = A_STATUS_FAILED;
        return status;
    }
    vap->vap_netdev = netdev;
    vap->wifisc      = wifisc ;
    memcpy(&vap->mac_addr[0], &vap->vap_netdev->dev_addr[0], ACFG_MACADDR_LEN);
    status = atd_tgt_set_vap_index(wifisc, vap);
    if(status != A_STATUS_OK){
        status = A_STATUS_ENXIO;
        return status;
    }
    atd_print_mac_addr(&vap->vap_netdev->dev_addr[0], __func__);

    atd_trace(ATD_DEBUG_FUNCTRACE,(" End .."));
    return status;

}

/** 
 * @brief VAP netdevice notification handler
 * 
 * @param dev
 * @param dev_event
 */
static int
atd_vap_notifier(struct net_device     *dev, int  dev_event)
{
    int             status = NOTIFY_OK;
    atd_tgt_vap_t   *vap;

    atd_trace(ATD_DEBUG_FUNCTRACE, (".START, Event = %d", dev_event));

    switch (dev_event) {
        case NETDEV_REGISTER:
            printk ("%s register notification\n", dev->name);
            vap = atd_tgt_find_vap(g_atd_dev, dev);
            if (vap==NULL){
                printk ("register new vap %s\n", dev->name);
                if(g_atd_dev->dont_send_create_event == 0 &&
                    atd_tgt_vap_register(g_atd_dev, dev) == A_STATUS_OK){
                    atd_netdev_get_by_name(dev->name);
                    vap = atd_tgt_find_vap(g_atd_dev, dev);
                    if (vap!=NULL)
                        atd_tgt_create_vap(vap, WMI_EVENT_CREATE_VAP);
                }
            }
            break;

        case NETDEV_GOING_DOWN:
            printk("%s: interface closing\n", dev->name);
            break;

        case NETDEV_UNREGISTER:
            vap = atd_tgt_find_vap(g_atd_dev, dev);
            if (!vap) break;
            
            atd_trace(ATD_DEBUG_CFG, ("vap = %p", vap));

#ifdef GMAC_8021Q_SUPPORT
            if(vap->vlan_id) {
                hif_gmac_vlan_delvap(g_atd_dev->hif_handle, vap->vlan_id, dev);
                vap->byp_flags = 0;
            }
#endif
            atd_tgt_vap_unref(vap);
            atd_tgt_vap_event(vap, WMI_EVENT_DELETE_VAP);
            break;

        default:
            printk("Ignoring %s notification %d\n", 
                    dev->name, dev_event);
            status = NOTIFY_DONE;
            break;
    }

    atd_trace(ATD_DEBUG_FUNCTRACE, (".END"));

    return status;
}

int
atd_netdev_notifier(struct notifier_block  *nb, unsigned long event, 
                    void  *dev)
{
    struct net_device  *netdev = (struct net_device *)dev;
    int                 status = NOTIFY_OK;
    
    atd_assert(dev);

    atd_trace(ATD_DEBUG_FUNCTRACE ,(".START"));
    
    atd_trace(ATD_DEBUG_FUNCTRACE ,("netdev(%s)", netdev->name));

    if ((strncmp(netdev->name, "wifi", 4)) == 0)
        status = atd_wifi_notifier(netdev, event);
    else if (strcmp(netdev->name, "lo"))
        status = atd_vap_notifier(netdev, event);

    atd_trace(ATD_DEBUG_FUNCTRACE ,(".END"));

    return status;
}


static void   atd_tgt_register_services(atd_tgt_dev_t * sc  ) 
{

    htc_epcallback_t cb;

    atd_trace(ATD_DEBUG_FUNCTRACE,(" Start"));

    cb.ep_rx       = atd_tgt_htc_recv_message ;
    cb.ep_txcomp   = atd_tgt_htc_sendcomplete ;
    cb.ep_ctx      = (void *) sc;
 
    sc->data_ep= htc_register_service(sc->htc_handle,&cb,WMI_DATA_BE_SVC);

    atd_trace(ATD_DEBUG_FUNCTRACE,(" End .."));


}


static a_status_t 
atd_tgt_wifi_avail_event(atd_tgt_dev_t *atsc,struct net_device *netdev)
{
    int index = atsc->wifi_cnt;
    a_status_t status = A_STATUS_OK;
    atd_trace(ATD_DEBUG_FUNCTRACE, ("%s : : %s max %d "\
                "detected %d \n ",__func__,netdev->name\
                ,atsc->wifiphy_max_cnt,atsc->wifi_cnt));

    atd_trace(ATD_DEBUG_FUNCTRACE,(" Start .."));
    atsc->wifi_list[index] =  (atd_tgt_wifi_t *) 
                         kzalloc(sizeof(atd_tgt_wifi_t),GFP_KERNEL);
    if(atsc->wifi_list[index] == NULL){
        printk("Error in allocationg wifisc");
        return A_STATUS_FAILED;
    }

    memset(atsc->wifi_list[index],0,sizeof(atd_tgt_wifi_t));

    atsc->wifi_list[index]->netdev = netdev ;
    atsc->wifi_list[index]->wifi_index = atsc->wifi_cnt++;
    atsc->wifi_list[index]->rx_endpoint = atsc->data_ep;

    memcpy(&atsc->wifi_list[index]->mac_addr[0],netdev->dev_addr, 
           ACFG_MACADDR_LEN);

    if(atsc->wifi_unregister) {
        printk("send an event to HOST about the particular WIFI radio\n");
        atd_tgt_restore_wifi(atsc->wifi_list[index], WMI_EVENT_RESTORE_WIFI);
        atsc->wifi_unregister = 0;
    }
    else if(atsc->wifi_cnt == atsc->wifiphy_max_cnt){
        printk("All the wifi detected %d:%d  Send HTC Ready \n",
                atsc->wifi_cnt,atsc->wifiphy_max_cnt);
        printk("Wifi Detected Send HTC ready \n");
        htc_send_targe_tready(atsc->htc_handle);

    }

    atd_trace(ATD_DEBUG_FUNCTRACE,(" End .."));
   return status;

}

void
atd_tgt_vap_unref(atd_tgt_vap_t     *vap)
{
    vap->wifisc->vap_list[vap->vap_index] =  NULL;

    dev_put(vap->vap_netdev);
}

void atd_tgt_module_exit(void )
{
    printk("%s \n",__func__);
    if (g_atd_dev->wsupp_handle)
        atd_tgt_wsupp_helper_cleanup(g_atd_dev->wsupp_handle);

    if (g_atd_dev->byp_handle)
        atd_tgt_byp_cleanup(g_atd_dev->byp_handle);

    if (g_atd_dev->appbr_handle)
        atd_tgt_appbr_cleanup(g_atd_dev->appbr_handle);

    unregister_netdevice_notifier(&atd_netdev_notify);
    if(g_atd_dev != NULL){
	    htc_cleanup(g_atd_dev->htc_handle);
	    hif_cleanup(g_atd_dev->hif_handle);
	    kfree(g_atd_dev);
    }
    //todo

}
/*************************Tx Rx ***********************************/

static inline struct net_device * 
atd_tgt_find_netdev(atd_tgt_dev_t * atsc,a_uint32_t vap_index ,
                    a_uint32_t wifi_index)
{
  if(atsc->wifi_list[wifi_index]!= NULL){
      if(atsc->wifi_list[wifi_index]->vap_list[vap_index] != NULL)
          return atsc->wifi_list[wifi_index]->vap_list[vap_index]->vap_netdev;
  }
  return NULL;
}
static void 
atd_tgt_htc_recv_message(void *ServiceCtx, struct sk_buff * buf, 
                         htc_endpointid_t EndPt )
{

    atd_tgt_dev_t  *atsc = (atd_tgt_dev_t *)ServiceCtx;
    atd_tx_hdr_t * DmesgHdr;
    struct net_device *vap_netdev;
    a_uint8_t *data;
    a_uint32_t len;
    a_int32_t  vap_index,wifi_index;

    atd_trace(ATD_DEBUG_FUNCTRACE,(" Start .."));

    data = buf->data ; 
    len  = buf->len  ;
    DmesgHdr  =  (atd_tx_hdr_t *)data;

    wifi_index = DmesgHdr->wifi_index;
    vap_index   = DmesgHdr->vap_index;

    atd_trace(ATD_DEBUG_FUNCTRACE, ("vap_index %d  wifi_index %d ", \
                vap_index,wifi_index));
    /*    check for bad index */
    if(wifi_index < 0 && wifi_index > atsc->wifi_cnt){
        printk("bad Radio Index %d \n",wifi_index);
        dev_kfree_skb_any(buf);
        return;
    }
    if(vap_index < 0 && vap_index > MAX_VAP_SUPPORTED){
        printk("bad vap Index %d \n",vap_index);
        dev_kfree_skb_any(buf);
        return;
    }
    vap_netdev = atd_tgt_find_netdev(atsc,vap_index,wifi_index);
    if(vap_netdev == NULL)
        goto fail;
    skb_pull(buf, sizeof(atd_tx_hdr_t));

    atd_dump_ether_header(buf);
    
    /* 
     * EAP-Request Short Packet Handling :
     * Workround for Ethernet MAC Padding for short packets
     * EV #87424
     */
    {
        struct ethhdr   *eh = (struct ethhdr  *)buf->data;
        struct ieee802_1x_hdr {
            a_uint8_t  version;
            a_uint8_t  type;
            a_uint16_t length;
            /* followed by length octets of data */
        }__packed;
        struct ieee802_1x_hdr *hdr = (struct ieee802_1x_hdr *)((char *)buf->data + sizeof(struct ethhdr));
        int true_len = 0;
#ifndef ETHERTYPE_PAE
#define ETHERTYPE_PAE   0x888e      /* EAPOL PAE/802.1x */
#endif
        if((eh->h_proto == htons(ETHERTYPE_PAE)) && (buf->len < 40))
        {
            true_len = (sizeof(struct ethhdr) + sizeof(struct ieee802_1x_hdr) + hdr->length);
            skb_trim(buf, true_len);
        }
#undef ETHERTYPE_PAE
    }

    atd_tx_to_wlan(buf, vap_netdev);

    atd_trace(ATD_DEBUG_FUNCTRACE,(" End .."));
    return;
fail :
     atd_trace(ATD_DEBUG_FUNCTRACE, ("%s Failure ...\n",__func__));
     dev_kfree_skb_any(buf);

}

/** 
 * @brief Find a VAP from a netdevice
 * 
 * @param atsc  (ATSC global pointer)
 * @param dev   (Netdevice pointer)
 * 
 * @return      (VAP pointer)
 */
atd_tgt_vap_t *
atd_tgt_find_vap(atd_tgt_dev_t *atsc, struct net_device * dev)
{
    atd_tgt_vap_t   *vap;
    atd_tgt_wifi_t *wifi;
    uint8_t vap_index=dev->irq & 0x3f;

    if(!atsc->wifi_cnt)
	return NULL;

    if(dev->irq & 0x80)
         wifi=atsc->wifi_list[1];	
    else
	wifi=atsc->wifi_list[0];
	
    vap=wifi->vap_list[vap_index];

    if(vap && (vap->vap_netdev == dev))
        return vap; 

    return NULL;
}

/** 
 * @brief Find Wifi from the Netdevice 
 * 
 * @param atsc
 * @param dev
 * 
 * @return  (Wifi Softc or NULL)
 */
atd_tgt_wifi_t *
atd_tgt_find_wifi(atd_tgt_dev_t  *atsc, struct net_device     *dev)
{
    int r;

    for(r = 0 ; r < atsc->wifi_cnt; r++) 
        if(atsc->wifi_list[r]->netdev == dev) return atsc->wifi_list[r];
    
    return NULL;
}

void atd_rx_deliver_eapol(struct sk_buff *skb)
{
#ifndef ETHERTYPE_PAE
#define ETHERTYPE_PAE   0x888e      /* EAPOL PAE/802.1x */
#endif
    struct sk_buff *nskb;
    struct ethhdr *eh;

    eh = (struct ethhdr *) skb->data;
    if (eh->h_proto == htons(ETHERTYPE_PAE)) {
        nskb = skb_copy(skb, GFP_ATOMIC);
        if (nskb) {
            struct net_device *dev = skb->dev;
            printk("eapol delivered: %s\n", dev->name);
            nskb->protocol = eth_type_trans(nskb, dev);
            netif_rx(nskb);
            dev->last_rx = jiffies;
        } else {
            printk("alloc eapol failed!\n");
        }
    }
#undef ETHERTYPE_PAE
}

/* TGf l2_update_frame  format */
struct l2_update_frame
{
    struct ethhdr eh;
    u_int8_t dsap;
    u_int8_t ssap;
    u_int8_t control;
    u_int8_t xid[3];
}  __packed;

#define IEEE80211_L2UPDATE_CONTROL 0xf5
#define IEEE80211_L2UPDATE_XID_0 0x81
#define IEEE80211_L2UPDATE_XID_1 0x80
#define IEEE80211_L2UPDATE_XID_2 0x0

void atd_rx_deliver_local(struct sk_buff *skb)
{
#ifndef ETHERTYPE_PAE
#define ETHERTYPE_PAE   0x888e      /* EAPOL PAE/802.1x */
#endif
#ifndef ETHERTYPE_ENCAP
#define ETHERTYPE_ENCAP 0x890d      /* TDLS /802.11z */
#endif

    struct sk_buff *nskb;
    struct ethhdr *eh;
    struct l2_update_frame *l2uf;

    eh = (struct ethhdr *) skb->data;
    l2uf = (struct l2_update_frame *)(skb->data);
    if (
#if (TARGET_STANDALONE == 1)
		1 || 
#endif
		eh->h_proto == htons(ETHERTYPE_PAE) ||
        eh->h_proto == htons(ETHERTYPE_ENCAP) ||
        (l2uf->control == IEEE80211_L2UPDATE_CONTROL &&
         l2uf->xid[0] == IEEE80211_L2UPDATE_XID_0 &&
         l2uf->xid[1] == IEEE80211_L2UPDATE_XID_1 &&
         l2uf->xid[2] == IEEE80211_L2UPDATE_XID_2)) {
        nskb = skb_copy(skb, GFP_ATOMIC);
        if (nskb) {
            struct net_device *dev = skb->dev;
#if (TARGET_STANDALONE != 1 )
            printk("packet delivered local: %s\n", dev->name);
#endif
            nskb->protocol = eth_type_trans(nskb, dev);
            netif_rx(nskb);
            dev->last_rx = jiffies;
        } else {
            printk("alloc skb failed!\n");
        }
    }
#undef ETHERTYPE_PAE
}

/* static struct sk_buff * */
/* atd_vlan_offload_filter(struct sk_buff *skb) */
/* { */
/* struct ethhdr       *eh; */
/* struct net_device   *dev    =  skb->dev; */

/* eh      =   (struct ethhdr *) skb->data; */

/* if(eh->h_proto != ETH_P_PAE) */
/* { */

/* skb->protocol = eth_type_trans(skb, dev); */
/* netif_rx(skb); */
/* dev->last_rx = jiffies; */
/* return NULL; */
/* } */

/* return  skb; */

/* } */

/** 
 * @brief The call back function from NEWMA to send packets to host
 * 
 * @param nbuf
 */
void   atd_rx_from_wlan(struct sk_buff * nbuf)
{
    a_status_t status ;
    atd_rx_hdr_t * rxdatahdr ;
    struct net_device * vap_netdev = NULL;
    htc_handle_t htc_handle ;
    byp_sc_t * byp_handle ;
    atd_tgt_vap_t *vap = NULL ;
    atd_tgt_wifi_t * wifisc = NULL;

    byp_filter_fn_t   byp_filter;
    atd_trace(ATD_DEBUG_FUNCTRACE,(" Start .."));

    if(g_atd_dev == NULL){
#if (TARGET_STANDALONE ==1 )
		atd_rx_deliver_local(nbuf);
#else
        printk("Radio is not detected so far \n");
#endif
        dev_kfree_skb_any(nbuf);
        goto out;
    }
    htc_handle = g_atd_dev->htc_handle;
    vap_netdev =  ((struct sk_buff *)nbuf)->dev;
    
    /* deliver packet locally as well if it is a selected packet */
    atd_rx_deliver_local(nbuf);

    vap = atd_tgt_find_vap(g_atd_dev,vap_netdev);
    if(vap == NULL ){
        printk("skb does not belong to any vap \n");
        dev_kfree_skb_any(nbuf);
        goto out;
    }

    /* check if it is a LAN packet which should be bypassed from ATD path*/
    byp_handle  = (byp_sc_t *) g_atd_dev->byp_handle;
    byp_filter  = BYPSC_FILTER(byp_handle);

    if(byp_filter)
    {
        nbuf = byp_filter(byp_handle, vap, nbuf);

        if(!nbuf)
            goto out;
    }

    /* nbuf = atd_vlan_offload_filter(nbuf); */
    /* if(!nbuf) */
    /* goto out; */

    wifisc = vap->wifisc;

    atd_trace(ATD_DEBUG_FUNCTRACE,("len %d vap %d radio %d \n",\
                 nbuf->len,vap->vap_index,wifisc->wifi_index));

    atd_dump_ether_header(nbuf);

    /* deliver packet locally as well if it is an EAPOL packet */
    /* atd_rx_deliver_eapol(nbuf); */

    rxdatahdr  = (atd_rx_hdr_t *)skb_push(nbuf,
                                                 sizeof(atd_rx_hdr_t));

    rxdatahdr->wifi_index = wifisc->wifi_index;
    rxdatahdr->vap_index = vap->vap_index;

    status = htc_send(htc_handle, nbuf, wifisc->rx_endpoint);
    if(status != A_STATUS_OK){
        /* printk(" atd_rx_from_wlan :htc_send failed \n"); */
        dev_kfree_skb_any(nbuf);
    }


    atd_trace(ATD_DEBUG_FUNCTRACE,(" End .."));
out:
    return ;
}
/* Tx done call back from HTC to Higher layer */

static void 
atd_tgt_htc_sendcomplete(void *ServiceCtx, struct sk_buff * buf,
                         htc_endpointid_t Endpt )
{
    dev_kfree_skb_any(buf);

}

void
atd_tgt_htc_set_stopflag(a_uint8_t val)
{
    atd_tgt_wmi_set_stopflag(g_atd_dev->wmi_handle, val);
    htc_set_stopflag(g_atd_dev->htc_handle, val);
}


EXPORT_SYMBOL(atd_rx_from_wlan);
EXPORT_SYMBOL(atd_tgt_htc_set_stopflag);


/*----------------------------------------------------------------*/

module_init(atd_tgt_module_init);
module_exit(atd_tgt_module_exit);
