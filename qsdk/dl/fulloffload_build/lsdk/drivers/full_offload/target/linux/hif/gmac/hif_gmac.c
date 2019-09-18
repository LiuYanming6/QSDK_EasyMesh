/*
 * Copyright (c) 2013 Qualcomm Atheros, Inc. 
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

#include <linux/version.h>
#include <linux/skbuff.h>
#include <linux/netdevice.h>
#include <linux/if_ether.h>
#include <linux/etherdevice.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/timer.h>
#include <linux/if_vlan.h>
#include <linux/list.h>

#include "a_base_types.h"
#include "hif_api.h"

#include <asm/byteorder.h>

#ifndef BUILD_PARTNER_PLATFORM
#include <../arch/mips/include/asm/mach-ar7240/ar7240.h>
#include <atheros.h>
#endif

#define ETH_P_ATH               0x88BD
#define ATH_P_MAGBOOT           0x12 /*Magpie is booting*/
#define ATH_P_MAGNORM           0x13 /*Magpie is in default state*/
#define ATH_P_MAGDISC           0x14 /*Magpie is booting*/
#define ATH_P_MAGDISCRESP       0x15 /*Magpie is booting*/


#ifdef GMAC_FRAGMENT_SUPPORT
#define ATH_P_MAGMORE_FRAGMENT  0x16 /*Magpie normal packet has more fragment*/
#define ATH_P_MAGLAST_FRAGMENT  0x17 /*Magpie normal packet last fragment */
#define ATH_GMAC_FRAG_THRESH    1400 /*Maximum lenght of the fragment */
#endif
#define ATH_P_RESET             0x18
#define ATH_P_DBGLOG            0x20 /*Debug Log Packet*/

#define gmac_trc(x, y)          
#define dump_mac(x)             

int gmac_dbg_mask = 0x0;
struct gmac_softc ;
static char *hst_if = "eth0";

int __gmac_dev_event(struct notifier_block *nb, unsigned long event, 
                     void *dev);
int __gmac_recv(struct sk_buff  *skb,  struct net_device  *dev,
                struct packet_type  *type, struct net_device   *orig_dev);


#ifdef GMAC_8021Q_SUPPORT
int __gmac_vlan_recv(struct sk_buff  *skb,  struct net_device  *dev,
                struct packet_type  *type, struct net_device   *orig_dev);
#endif


typedef struct athtype{
#if defined (__LITTLE_ENDIAN_BITFIELD)
        a_uint8_t    proto:6,
                     res :2;
#elif defined (__BIG_ENDIAN_BITFIELD)
        a_uint8_t    res :  2,
                     proto : 6;
#else
#error  "Please fix"
#endif
        a_uint8_t   res_lo;
        a_uint16_t  res_hi;
    }__attribute__((packed))  athtype_t;

typedef struct athhdr{
    athtype_t   type;
    a_uint16_t  align_pad;
}athhdr_t;

typedef struct gmac_hdr{
    struct ethhdr    eth; /*14 bytes*/
    athhdr_t    ath;      /*6 byte*/ 
}gmac_hdr_t;

typedef struct hif_gmac_node{
    struct net_device      *dev; /* Netdev associated with this device */
    gmac_hdr_t              hdr; /* GMAC header to insert per packet */
    struct gmac_softc      *sc;
#ifdef GMAC_FRAGMENT_SUPPORT
    struct sk_buff         *fragment;
    spinlock_t             frag_lock;
#endif
    a_uint32_t             ready;
}gmac_node_t;


#define ATH_HLEN                sizeof(struct athhdr)
#define GMAC_HLEN               sizeof(struct gmac_hdr)

typedef struct _hif_gmac_dbglog {
    hif_os_callback_t     oscb;
    hif_callback_t        cb;
}hif_gmac_dbglog_t;

typedef struct gmac_softc {
    hif_os_callback_t     oscb;
    hif_callback_t        cb;
    hif_gmac_dbglog_t     dbglog;
    void * context;
    gmac_node_t           node ;
    struct timer_list     disc_timer;
#ifdef GMAC_8021Q_SUPPORT
    struct hlist_head     vlan_dev_list_head;
#endif
}gmac_softc_t ;

struct packet_type       __gmac_pkt = {
    .type = __constant_htons(ETH_P_ATH),
    .func = __gmac_recv, /* GMAC receive method */
};

#ifdef GMAC_8021Q_SUPPORT
typedef struct gmac_vlan_dev {
    struct net_device           *dev;     /* Back reference to net_device */
    struct net_device_stats     dev_stats;
    a_uint8_t                   nr_vaps; /* No. of VAPs associated with VLAN */
    a_uint8_t                   flags;
    a_uint16_t                  vlan_id;
    struct net_device           *vap_dev;
    struct hlist_node           vlan_dev_hlist;
} gmac_vlan_dev_t;

struct packet_type       __gmac_vlan_pkt = {
    .type = __constant_htons(ETH_P_8021Q),
    .func = __gmac_vlan_recv, /* GMAC receive method */
};

int
gmac_vlan_dev_xmit(struct sk_buff   *skb, struct net_device    *dev);

int gmac_vlan_dev_open(struct net_device     *dev);

int gmac_vlan_dev_stop(struct net_device     *dev);

struct net_device_stats *
gmac_vlan_dev_get_stats(struct net_device *dev);

struct net_device_ops  gmac_vlan_dev_ops = {
    .ndo_open               = gmac_vlan_dev_open,
    .ndo_stop               = gmac_vlan_dev_stop,
    .ndo_start_xmit         = gmac_vlan_dev_xmit,
    .ndo_get_stats          = gmac_vlan_dev_get_stats, 
};

#endif

struct notifier_block    __gmac_notifier = {
    .notifier_call = __gmac_dev_event,

};

hif_handle_t glbl_hif_handle = NULL;

hif_handle_t gmac_init(void ) ;

static a_status_t  gmac_send_pkt(gmac_node_t * node ,struct sk_buff * skb);
static inline int __os_gmac_xmit(struct sk_buff   *skb, 
                                struct net_device *dev);
static void   hif_gmac_header(struct sk_buff *  skb, gmac_hdr_t  *hdr);
static void hif_gmac_proto_header(struct sk_buff *  skb, gmac_hdr_t  *hdr, 
                               a_uint8_t proto);
static void   gmac_recv_default(gmac_node_t * node ,int pipeid, 
                              struct sk_buff *  skb );
static void   gmac_process_discresp(struct sk_buff   *skb, gmac_softc_t * sc );
static void   gmac_send_discovery(gmac_softc_t * sc);
static void   gmac_disc_timer (unsigned long ctx);
static void gmac_recv_dbglog(gmac_node_t *node, int pipeid, struct sk_buff *skb);


hif_status_t
hif_init(hif_os_callback_t *oscb)
{
    gmac_softc_t *sc = (gmac_softc_t *)glbl_hif_handle;

    gmac_trc(GMAC_FUNCTRACE,("Start"));

    memcpy(&sc->oscb,oscb,sizeof(hif_os_callback_t));

    if(sc->node.ready)
        sc->oscb.device_detect((hif_handle_t)sc);

    gmac_trc(GMAC_FUNCTRACE,("End"));
    return HIF_STATUS_OK;
} 

hif_handle_t    
gmac_init(void ) 
{
    gmac_softc_t  *sc =  NULL;
    gmac_node_t   *node;

    gmac_trc(GMAC_FUNCTRACE,("Start"));

    sc = kzalloc(sizeof(gmac_softc_t),GFP_KERNEL);
    if(sc == NULL){
        printk("Memalloc fail hif \n");
        goto out;
    }

    node = &sc->node;

    printk("hst_if =%s \n",hst_if);
     /*For TARGET_NULL only */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,30)    
    node->dev = dev_get_by_name(hst_if);
#else
    node->dev = dev_get_by_name(&init_net, hst_if);
#endif    

#ifdef GMAC_FRAGMENT_SUPPORT
    spin_lock_init(&node->frag_lock);
#endif

    node->sc = sc; 
    if(node->dev == NULL){
        printk("no eth0 present \n");
        goto out;
    }
    
    memcpy(&node->hdr.eth.h_source[0],&node->dev->dev_addr[0],6);

    {
        unsigned char * mac = &node->hdr.eth.h_source[0];
        int i=0;
        printk("mac address  dest --> %2x:%2x:%2x:%2x:%2x:%2x   \n ", \
                mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);

        mac = &node->hdr.eth.h_dest[0];
        while(i < 6)
            mac[i++]=0xff;
      
        printk("mac address  src--> %2x:%2x:%2x:%2x:%2x:%2x   \n ", \
                mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);

    }
    node->hdr.eth.h_proto  = htons(ETH_P_ATH);
    node->hdr.ath.type.proto = ATH_P_MAGDISC;


    /*For recive */
    __gmac_pkt.af_packet_priv  = sc ;
    dev_add_pack(&__gmac_pkt);
    register_netdevice_notifier(&__gmac_notifier);

#ifdef GMAC_8021Q_SUPPORT
    INIT_HLIST_HEAD(&sc->vlan_dev_list_head);
    dev_add_pack(&__gmac_vlan_pkt);
#endif

    /*For TARGET_NULL only */
    /*Target send broadcast */
    gmac_send_discovery(sc);
out :    

    gmac_trc(GMAC_FUNCTRACE,("End"));
    return sc;
}

hif_status_t
hif_cleanup(hif_handle_t  hif_handle)
{
    gmac_softc_t *sc = (gmac_softc_t *)hif_handle;

    gmac_trc(GMAC_FUNCTRACE,("x"));

    del_timer(&sc->disc_timer);

    dev_remove_pack(&__gmac_pkt);
#ifdef GMAC_8021Q_SUPPORT
    dev_remove_pack(&__gmac_vlan_pkt);
#endif
    unregister_netdevice_notifier(&__gmac_notifier);

    if(sc != NULL)
        kfree(sc);
    return HIF_STATUS_OK ;

}

void 
hif_claim_device(hif_handle_t hif_handle, void *ctx)
{
    gmac_softc_t *sc = (gmac_softc_t *)hif_handle;

    gmac_trc(GMAC_FUNCTRACE,("x"));

    sc->context = ctx;
}

void          
hif_register(hif_handle_t hif_handle , hif_callback_t *cb)
{
    gmac_softc_t *sc = (gmac_softc_t *) hif_handle;

    gmac_trc(GMAC_FUNCTRACE,("x"));

    memcpy(&sc->cb,cb,sizeof(hif_callback_t));
}

hif_status_t  
hif_start(hif_handle_t hif_handle)
{
    hif_status_t status = HIF_STATUS_OK;

    gmac_trc(GMAC_FUNCTRACE,("x"));

    return status;
}

a_uint32_t 
hif_get_reserveheadroom(hif_handle_t hif_handle)
{
    gmac_trc(GMAC_FUNCTRACE,("x"));
    return GMAC_HLEN;
}

a_uint32_t 
hif_get_dlpipe(hif_pipetype_t type)
{
    gmac_trc(GMAC_FUNCTRACE,("x"));
    return 1;
}

a_uint32_t 
hif_get_ulpipe(hif_pipetype_t type)
{
    gmac_trc(GMAC_FUNCTRACE,("x"));
    return 1;
}

hif_status_t  
hif_send(hif_handle_t hif_handle ,a_uint32_t pipeid , struct sk_buff * skb )
{
    gmac_softc_t *sc = (gmac_softc_t *) hif_handle;
    gmac_node_t *node = &sc->node;

    gmac_trc(GMAC_FUNCTRACE,("Start"));

    if(!node->ready){
        printk("Node is not ready \n");
        dev_kfree_skb_any(skb);
        return HIF_STATUS_OK;
    }

    if(gmac_send_pkt(node,skb)){
        printk("GMAC send pkt fail \n");
    }

    gmac_trc(GMAC_FUNCTRACE,("End"));
    return HIF_STATUS_OK;
}

static a_status_t 
gmac_send_pkt(gmac_node_t * node,struct sk_buff * skb){
    a_uint32_t  headroom;
    a_status_t result;

    gmac_trc(GMAC_FUNCTRACE,(" Start"));

    headroom = (skb_headroom(skb) < GMAC_HLEN);

    if(headroom){
        if(pskb_expand_head(skb, headroom, 0, GFP_ATOMIC)){
            printk("unable to expand heade");
            dev_kfree_skb_any(skb);
            return A_STATUS_OK;
        }
        gmac_trc(GMAC_DBGLVL0,("skb reallocated "));
    }
#ifdef GMAC_FRAGMENT_SUPPORT
    if(skb->len > ATH_GMAC_FRAG_THRESH ){
        /*Allocate a new skb for the 2nd fragment and send it */
        struct sk_buff* last_frag;
        a_uint32_t rem_len = skb->len - ATH_GMAC_FRAG_THRESH;
        last_frag = dev_alloc_skb(rem_len + GMAC_HLEN);
        if(last_frag == NULL){
            printk("unable to allocate fragment \n");
            dev_kfree_skb_any(skb);
            return A_STATUS_OK;
        }
        skb_reserve(last_frag, GMAC_HLEN);
        skb_put(last_frag, rem_len);
        skb_trim(skb, ATH_GMAC_FRAG_THRESH); 
        memcpy(last_frag->data, skb->data + ATH_GMAC_FRAG_THRESH, rem_len);
        hif_gmac_proto_header(skb, &node->hdr, ATH_P_MAGMORE_FRAGMENT);
        result = __os_gmac_xmit(skb, node->dev);
        if(result == A_STATUS_OK){
            hif_gmac_proto_header(last_frag, &node->hdr, 
                                      ATH_P_MAGLAST_FRAGMENT);
            result = __os_gmac_xmit(last_frag, node->dev);
            if(result != A_STATUS_OK){
                dev_kfree_skb_any(last_frag);
                result = A_STATUS_OK;
            }
        }
        else{
            printk("Error isn sending first fragment discarding pkt\n");
            dev_kfree_skb_any(skb);
            result = A_STATUS_OK;
        }
    }
    else{
#endif

    hif_gmac_header(skb, &node->hdr);

    gmac_trc(GMAC_FUNCTRACE,(" End"));

    result = __os_gmac_xmit(skb, node->dev);
#ifdef GMAC_FRAGMENT_SUPPORT
    }
#endif
    return result;
}

void
hif_gmac_header(struct sk_buff *  skb, gmac_hdr_t  *hdr)
{
    gmac_hdr_t  *gmac;

    gmac = (gmac_hdr_t *)skb_push(skb, GMAC_HLEN);

    memcpy(gmac, hdr, GMAC_HLEN);
}

void 
hif_gmac_proto_header(struct sk_buff *  skb, gmac_hdr_t  *hdr, 
                         a_uint8_t proto)
{
    gmac_hdr_t  *gmac;
    athhdr_t   *ath;

    gmac = (gmac_hdr_t *)skb_push(skb, GMAC_HLEN);

    memcpy(gmac, hdr, GMAC_HLEN);
    ath = &gmac->ath;
    ath->type.proto = proto; 
}

static inline int
__os_gmac_xmit(struct sk_buff   *skb, struct net_device  *dev)
{
    skb->dev = dev;

    return (dev_queue_xmit(skb)==0) ? A_STATUS_OK: A_STATUS_FAILED;
}


int
__gmac_recv(struct sk_buff  *skb,  struct net_device  *dev,
            struct packet_type  *type, struct net_device   *orig_dev)
{

    gmac_softc_t * sc  = __gmac_pkt.af_packet_priv;

    athhdr_t                *ath;

    gmac_trc(GMAC_FUNCTRACE,("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"));

    if (skb_shared(skb))
        skb = skb_unshare(skb, GFP_ATOMIC);

    ath = (athhdr_t *) skb->data;

    gmac_trc(GMAC_DBGLVL0,("proto %x ", ath->type.proto));
    switch(ath->type.proto)
    {
#ifdef GMAC_HOST
        case ATH_P_MAGDISC:

            gmac_trc(GMAC_DBGLVL0,("discover packet "));
            gmac_process_disc(skb,sc);
            return 0 ;
     
#else
        case ATH_P_MAGDISCRESP:

            gmac_trc(GMAC_DBGLVL0,("discover packet "));
            gmac_process_discresp(skb,sc);
            return 0;

        case ATH_P_RESET:

            printk("Received Reset command from host \n");
            for(;;) {
                printk("Resetting tgt by enabling full chip reset bit\n");
             //   printk("ROM  boot reset bit 8 \n");
              //  ar7240_reg_wr (AR7240_GPIO_OE,
               //         ar7240_reg_rd(AR7240_GPIO_OE) & (~(1 <<  8)));

#ifndef BUILD_PARTNER_PLATFORM
                ar7240_reg_wr(AR7240_RESET, AR7240_RESET_FULL_CHIP);
#endif
            }
            
#endif
        case ATH_P_MAGNORM:

            break;

        case ATH_P_DBGLOG:
        {
            gmac_recv_dbglog(&sc->node, HIF_DBGLOG_PIPE, skb);
            return 0;
        }

#ifdef GMAC_FRAGMENT_SUPPORT
        case ATH_P_MAGMORE_FRAGMENT:
        {
            unsigned long flag;
            /*Store this fragment in the node and when last fragment 
              comes make one pkt from fragments and send it */
            struct sk_buff* old_frag;
            spin_lock_irqsave(&sc->node.frag_lock, flag);
            old_frag = sc->node.fragment;
            sc->node.fragment = skb;
            spin_unlock_irqrestore(&sc->node.frag_lock, flag);
            if(old_frag != NULL){
                printk("1st frag already rcvd overriding stored fragment \n"); 
                dev_kfree_skb_any(old_frag);
            }

            return 0;
        }

        case ATH_P_MAGLAST_FRAGMENT:
        {
            unsigned long flag;
            struct sk_buff* old_frag;
            spin_lock_irqsave(&sc->node.frag_lock, flag);
            old_frag = sc->node.fragment;
            sc->node.fragment = NULL;
            spin_unlock_irqrestore(&sc->node.frag_lock, flag);
            if(old_frag == NULL){
                printk("Invalid last frag rcvd. No earlier fragment stored\n");
                dev_kfree_skb_any(skb);
                return 0;

            }
            else{
                a_uint32_t first_fraglen,totallen;
                struct sk_buff* new_skb;
                skb_pull(skb, ATH_HLEN);
                skb_pull(old_frag, ATH_HLEN);
                first_fraglen = old_frag->len;
                totallen = first_fraglen + skb->len;
                new_skb = dev_alloc_skb(totallen+100);
                skb_reserve(new_skb, 50);
                skb_put(new_skb, totallen);
                memcpy(new_skb->data, old_frag->data, 
                          first_fraglen);
                memcpy(new_skb->data + first_fraglen, skb->data, skb->len);
                dev_kfree_skb_any(old_frag);
                dev_kfree_skb_any(skb);
                //assert(sc->node.ready != 1)
                gmac_recv_default(&sc->node, 1, new_skb);
                return 0;

            }
            break;
        }
#endif

        default:
            printk("unsupoorted magic proto %x \n",ath->type.proto);
    }

    skb_pull(skb, ATH_HLEN);
    if (sc->node.ready != 1 ){
        printk("Node is stil not ready \n");
        dev_kfree_skb_any(skb);
        return 0;
    }
    
    gmac_recv_default(&sc->node ,1, skb );

    return 0;
}

static void 
gmac_recv_default(gmac_node_t * node ,int pipeid, struct sk_buff *  skb )
{
    gmac_softc_t  *sc = node->sc ;

    if(sc->cb.recv_buf == NULL){
        printk(" recv_buf null \n");
        dev_kfree_skb_any(skb);
        return ;
    }
    sc->cb.recv_buf(sc->cb.context,skb,1);

}

int
__gmac_dev_event(struct notifier_block  *nb, unsigned long event, void  *dev)
{
    struct net_device * netdev = (struct net_device *)dev;
    printk(" __gmac_dev_event *************************************\n");
    printk(" event %lu name %s \n",event,netdev->name);

    switch (event) {
        case NETDEV_DOWN:
            /**
             * XXX: how to handle multiple magpies*/
            printk("%s : NETDEV_DOWN:",__func__);
            break;
        default:
            break;
    }
    return NOTIFY_DONE;
}

static void 
gmac_process_discresp(struct sk_buff   *skb, gmac_softc_t *sc )
{
    struct ethhdr * eh = eth_hdr(skb);
    gmac_node_t *node = &sc->node;
    athhdr_t    *ath = &node->hdr.ath;
    
    del_timer(&sc->disc_timer);
    gmac_trc(GMAC_FUNCTRACE,("Start"));

    if(node->dev == skb->dev)
        printk("exisiting node  %s \n",node->dev->name);

    memcpy(node->hdr.eth.h_dest  , eh->h_source, ETH_ALEN);
    memcpy(node->hdr.eth.h_source, skb->dev->dev_addr, ETH_ALEN);
    dump_mac(node->hdr.eth.h_dest);
    dump_mac(node->hdr.eth.h_source);

    node->hdr.eth.h_proto  = htons(ETH_P_ATH);

    ath->type.proto = ATH_P_MAGNORM;
    node->dev = skb->dev;
    node->sc = sc;


    dev_kfree_skb_any(skb);
    /*call back applicatin */
    if(!node->ready)
    {
        node->ready=1;
        if(sc->oscb.device_detect != NULL) {

            gmac_trc(GMAC_FUNCTRACE,("Calling application "));
            sc->oscb.device_detect((hif_handle_t)sc);
        }
        else
            printk("applicatin ready handle null");

        if(sc->dbglog.oscb.device_detect != NULL) {
           sc->dbglog.oscb.device_detect((hif_handle_t)sc);
        }
    }
    else
        printk("Repeated Discovery Response \n");

    gmac_trc(GMAC_FUNCTRACE,("End"));

}

static void     
gmac_send_discovery(gmac_softc_t * sc)
{
    struct timer_list *timer;
    unsigned int delay =1000;

    gmac_trc(GMAC_FUNCTRACE,("x"));

    timer = &sc->disc_timer;
    init_timer(timer);
    timer->function = gmac_disc_timer;
    timer->data = (unsigned long)sc;

    timer->expires = jiffies + msecs_to_jiffies(delay);
    add_timer(timer);
    printk("Timer started\n");
}

static void 
gmac_disc_timer (unsigned long ctx)
{
    gmac_softc_t *sc  = (gmac_softc_t *)ctx;
    struct sk_buff *skb;

    gmac_trc(GMAC_FUNCTRACE,("x"));

    skb = dev_alloc_skb(500);
    if(skb == NULL){
        printk("Unable to allocate skb");
        return;
    }

    skb_reserve(skb,GMAC_HLEN);
    skb_put( skb,500 - GMAC_HLEN);

    gmac_send_pkt(&sc->node,skb);
    sc->disc_timer.expires = jiffies + msecs_to_jiffies(1000);
    add_timer(&sc->disc_timer);

}


#ifdef GMAC_8021Q_SUPPORT
int
gmac_vlan_dev_open(struct net_device     *dev)
{
    netif_start_queue(dev);
    return 0;
}

int
gmac_vlan_dev_stop(struct net_device     *dev)
{

    netif_stop_queue(dev);
    dev->flags &= ~IFF_RUNNING;
    return 0;
}

/* TODO - Just a place holder. the stats will show 0s for now  */
struct net_device_stats *
gmac_vlan_dev_get_stats(struct net_device *dev)
{
        gmac_vlan_dev_t   *vlan_dev = netdev_priv(dev);

        return &vlan_dev->dev_stats;
}

/** 
 * @brief 
 * 
 * @param context
 * @param vlan_id
 * 
 * @return 
 */
static struct gmac_vlan_dev *
hif_gmac_vlan_finddev(gmac_softc_t  *context,a_uint16_t vlan_id) 
{

    struct hlist_head *head;
    struct hlist_node *node;

    struct gmac_vlan_dev *vlan_dev;

    gmac_softc_t *sc    = (gmac_softc_t *)glbl_hif_handle;
    head                = &sc->vlan_dev_list_head;

    if(sc == NULL)
    {
        printk("%s hif_handle NULL \n",__func__);
        return NULL;
    }

    hlist_for_each_entry(vlan_dev , node, head , vlan_dev_hlist) {
        if(vlan_dev->vlan_id == vlan_id)
            return vlan_dev;
    }

    return NULL;
}


int
hif_gmac_vlan_addvap(void  *handle,a_uint16_t vlan_id,struct net_device *vap_netdev) 
{
    gmac_vlan_dev_t *vlan_dev;
    gmac_softc_t *sc;
    sc =  (gmac_softc_t *) glbl_hif_handle;

    printk(" %s adding %d\n",__func__, vlan_id);

    vlan_dev   =  hif_gmac_vlan_finddev(sc, vlan_id);

    if(vlan_dev == NULL)
    {
        printk(" %s Invalid VLAN group %d \n ",__func__, vlan_id);
        return 0;
    }

    vlan_dev->nr_vaps++;

    /* 
     * If there is only one VAP associated with VLAN, no bridging is required,
     * we dont pass the packets up the stack, we use the vap context to 
     * directly send or recieve the packets to or from wlan driver.
     */ 

    if(vlan_dev->nr_vaps == 1)
    {
        vlan_dev->vap_dev = vap_netdev;
    }

    return 1;
}

int
hif_gmac_vlan_delvap(void  *handle,a_uint16_t vlan_id,struct net_device *vap_netdev) 
{
    gmac_vlan_dev_t *vlan_dev;
    gmac_softc_t *sc;
    sc =  (gmac_softc_t *) glbl_hif_handle;

    printk(" %s deleting %d\n",__func__, vlan_id);

    vlan_dev   =  hif_gmac_vlan_finddev(sc, vlan_id);

    if(vlan_dev == NULL)
    {
        printk(" %s Invalid VLAN group %d \n ",__func__, vlan_id);
        return 0;
    }

    vlan_dev->nr_vaps--;

    /* 
     * If there are no VAPs associated with this VLAN group, 
     * just send the packet up.
     */ 

    if(vlan_dev->nr_vaps == 0)
    {
        vlan_dev->vap_dev = NULL;
    }

    return 1;
}

void gmac_vlan_setup(struct net_device *dev) 
{
    ether_setup(dev);

    /* dev->tx_queue_len   =   0; */
    dev->destructor         = free_netdev;
    dev->hard_header_len    += VLAN_HLEN;

    return;
}

int
hif_gmac_vlan_create(void  *handle,a_uint16_t vlan_id)
{
    gmac_vlan_dev_t *vlan_dev;
    struct net_device *dev;
    gmac_softc_t *sc;
    char name[IFNAMSIZ] = {0};
    sc =  (gmac_softc_t *) glbl_hif_handle;

    printk(" %s creating %d\n",__func__, vlan_id);

    /* Check if the VLAN device already exists for this vlan_id;
     * If yes, ignore this request and return */
    vlan_dev         = hif_gmac_vlan_finddev(glbl_hif_handle, vlan_id);  
    if(vlan_dev)
    {
        goto out;
    }

    /* Name will look line eth0.5. Kept the naming convention same so that all 
     * existing scripts could be reused */
    snprintf(name, IFNAMSIZ, "%s.%i", hst_if, vlan_id);

    dev = alloc_netdev(sizeof(struct gmac_vlan_dev), name, gmac_vlan_setup);

    dev->netdev_ops         = &gmac_vlan_dev_ops;

    memcpy(dev->dev_addr, sc->node.dev->dev_addr, 6);
    memcpy(dev->perm_addr, sc->node.dev->dev_addr, 6);

    vlan_dev   =  netdev_priv(dev);

    vlan_dev->vlan_id  =   vlan_id;
    vlan_dev->nr_vaps  =   0;
    vlan_dev->dev      =   dev;
    vlan_dev->vap_dev  =   NULL;
    vlan_dev->flags    =   0;

    hlist_add_head(&vlan_dev->vlan_dev_hlist, &sc->vlan_dev_list_head);

    register_netdev(dev);

out:
    return 1;

}

int
__gmac_vlan_recv(struct sk_buff  *skb,  struct net_device  *dev,
                 struct packet_type  *type, struct net_device   *orig_dev)
{
    struct             vlan_hdr     *vhdr;
    a_uint16_t         vlan_id;
    a_uint16_t         vlan_tci;
    a_uint16_t         proto;
    gmac_vlan_dev_t    *vlan_dev;

    vhdr        = (struct vlan_hdr *)skb->data;
    vlan_tci    = ntohs(vhdr->h_vlan_TCI);
    vlan_id     = vlan_tci & VLAN_VID_MASK;


    /* remove VLAN header  */
    skb_pull_rcsum(skb, VLAN_HLEN);

    if (skb_cow(skb, skb_headroom(skb)) < 0)
    {
        printk("No headroom \n");
        goto fail;
    }

    /* Reorder Ethernet Header  */
    memmove(skb->data - ETH_HLEN,
            skb->data - VLAN_ETH_HLEN, 12);
    skb->mac_header += VLAN_HLEN;

    vlan_dev         = hif_gmac_vlan_finddev(glbl_hif_handle, vlan_id);  

#if 0 /* Enable for debug */
    a_uint8_t i;
    a_uint8_t *data;
    data = skb->data - ETH_HLEN;
    printk("gmac_vlan_recv vlan %d",vlan_id);
    i = 0;
    while(i < 20)
    {
        printk(",%x",data[i++]);
    }
    printk("\n");
#endif


    /* If there are no associated VAPs with VLAN , or if there are more than 
     * one VAPs associated with VLAN (bridging required), send the packet up
     */

    /* XXX - Replace these if checks with a function indirection for better 
     * performance, since this is more or less static conditions. 
     *
     * eg - 2 Rx functions - gmac_vlan_rx_send_to_stack
     *                     - gmac_vlan_rx_send_to_wlan
     */
    if(vlan_dev == NULL || vlan_dev->nr_vaps != 1)
    {
        if(vlan_dev)
        {
            skb->dev    =   vlan_dev->dev;

            vlan_dev->dev_stats.rx_bytes  += skb->len;
            vlan_dev->dev_stats.rx_packets++;

        }


        proto = vhdr->h_vlan_encapsulated_proto;
        if (ntohs(proto) < 1536) 
        {
            proto = htons(ETH_P_802_3);

            if((*(unsigned short *) skb->data) != 0xffff)
                proto = htons(ETH_P_802_2);
        }
        skb->protocol = proto;

        /* printk("sending pkt up \n"); */
        /* skb->protocol = eth_type_trans(skb, dev); */
        if(in_irq())
            netif_receive_skb(skb);
        else
            netif_rx(skb);

        if(vlan_dev)
            vlan_dev->dev->last_rx = jiffies;

        goto out;
    }

    /* Else if nr_vaps == 1, Transmit to WLAN */
    skb_push(skb, ETH_HLEN);
    skb->dev    = vlan_dev->vap_dev;
    dev_queue_xmit(skb);

out:
    return 0;

fail:
    dev_kfree_skb_any(skb);
    return 1;

}

/** 
 * @brief 
 * 
 * @param skb
 * @param vlan_tci
 * 
 * @return 0 - Tag success
 *         1 - Fail 
 */
static int __gmac_vlan_put_tag(struct sk_buff *skb, a_uint16_t vlan_tci)
{
    struct vlan_ethhdr *veth;

    if (skb_cow_head(skb, VLAN_HLEN) < 0) {
        goto fail;
    }

    veth = (struct vlan_ethhdr *)skb_push(skb, VLAN_HLEN);

    /* Move the mac addresses to the beginning of the new header. */
    memmove(skb->data, skb->data + VLAN_HLEN, 2 * VLAN_ETH_ALEN);
    skb->mac_header -= VLAN_HLEN;

    /* first, the ethernet type */
    veth->h_vlan_proto = htons(ETH_P_8021Q);

    /* now, the TCI */
    veth->h_vlan_TCI = htons(vlan_tci);

    skb->protocol = htons(ETH_P_8021Q);

    return 0;

fail:
    /* dev_kfree_skb_any(skb); */
    return 1;
}

/** 
 * @brief 
 * 
 * @param handle
 * @param skb
 * @param vap_netdev
 * 
 * @return 0 - Processed packet successfully
 *         1 - Fail 
 */
int
hif_gmac_vlan_wlan_rx(void  *handle, struct sk_buff *skb, a_uint16_t vlan_id)
{
    gmac_softc_t *sc;
    struct gmac_vlan_dev * vlan_dev;
    struct net_device * dev;

    dev =  skb->dev;
    sc  =  (gmac_softc_t *)glbl_hif_handle ;// handle;

    vlan_dev = hif_gmac_vlan_finddev(sc, vlan_id);

    if(vlan_dev == NULL)
    {
        printk("Invalid VLAN Group  \n");
        goto fail;
    }

    /* If there are more than one VAPs associated with VLAN (bridging required),
     * send the packet up
     */
    if(vlan_dev->nr_vaps > 1)
    {
        skb->protocol   =   eth_type_trans(skb, dev);
        netif_rx(skb);
        dev->last_rx    =   jiffies;
    } 
    else /* Send packet through ethernet VLAN interface directly */
    {
        skb->dev    =   sc->node.dev;

        if(__gmac_vlan_put_tag(skb, vlan_id))
            goto fail;

        dev_queue_xmit(skb);
    }
out:
    return 0;

fail:
    dev_kfree_skb_any(skb);
    return 1;

}

/** 
 * @brief 
 * 
 * @param skb
 * @param dev
 * 
 * @return 0 =  NETDEV_TX_OK = driver took care of packet
 *         1 =  Fail 
 */
int
gmac_vlan_dev_xmit(struct sk_buff   *skb, struct net_device    *dev)
{
    gmac_softc_t *sc;

    struct gmac_vlan_dev * vlan_dev;

    sc =  (gmac_softc_t *)glbl_hif_handle ;// handle;

    vlan_dev    = netdev_priv(dev);
    skb->dev    = sc->node.dev;

    vlan_dev->dev_stats.tx_bytes  += skb->len;
    vlan_dev->dev_stats.tx_packets++;

    if(__gmac_vlan_put_tag(skb, vlan_dev->vlan_id))
        goto fail;

    dev_queue_xmit(skb);
    return 0;

fail:
    dev_kfree_skb_any(skb);
    return 1;
}
#endif


hif_status_t
hif_init_dbglog(hif_os_callback_t *oscb)
{
    gmac_softc_t *sc = (gmac_softc_t *) glbl_hif_handle;

    memcpy(&sc->dbglog.oscb, oscb,sizeof(hif_os_callback_t));

    if(sc->node.ready)
        sc->dbglog.oscb.device_detect((hif_handle_t)sc);

    return HIF_STATUS_OK;
}

void
hif_register_for_dbglog(hif_handle_t hif_handle , hif_callback_t *cb)
{
    gmac_softc_t *sc = (gmac_softc_t *) hif_handle;

    memcpy(&sc->dbglog.cb,cb,sizeof(hif_callback_t));
}

static void
gmac_recv_dbglog(gmac_node_t *node, int pipeid, struct sk_buff *skb)
{
    gmac_softc_t  *sc = node->sc;

    if(sc->dbglog.cb.recv_buf == NULL) {
        dev_kfree_skb_any(skb);
        return;
    }
    skb_pull(skb, ATH_HLEN);
    sc->dbglog.cb.recv_buf(sc->dbglog.cb.context, skb, pipeid);
}

hif_status_t  
hif_send_dbglog(hif_handle_t hif_handle ,a_uint32_t pipeid , struct sk_buff * skb )
{
    gmac_softc_t *sc = (gmac_softc_t *) hif_handle;
    gmac_node_t *node = &sc->node;
    a_uint32_t  headroom;

    if(!node->ready){
        dev_kfree_skb_any(skb);
        return HIF_STATUS_ERROR;
    }

    headroom = (skb_headroom(skb) < GMAC_HLEN);
    if(headroom){
        if(pskb_expand_head(skb, headroom, 0, GFP_ATOMIC)){
            dev_kfree_skb_any(skb);
            return HIF_STATUS_ERROR;
        }
    }

    hif_gmac_proto_header(skb, &node->hdr, ATH_P_DBGLOG);

    /*TODO: Looks like interrupts must be enabled before calling this. 
      which is not a case for crash log send */
    if(A_STATUS_OK != __os_gmac_xmit(skb, node->dev))
        return HIF_STATUS_ERROR;

    return HIF_STATUS_OK;
}

int gmac_module_init(void )
{
    hif_os_callback_t oscb;
    hif_callback_t cb;

#ifndef BUILD_PARTNER_PLATFORM
    ath_config_slave_mdio_gpios();
#endif

    gmac_trc(GMAC_FUNCTRACE,("x"));

    cb.context = NULL;
    cb.send_buf_done = NULL;
    cb.recv_buf    = NULL;

    memset(&oscb,0,sizeof(hif_os_callback_t));
    glbl_hif_handle = gmac_init();

    return 0;

}

void gmac_module_exit(void ){

    gmac_trc(GMAC_FUNCTRACE,("x"));

    if(glbl_hif_handle != NULL)
        hif_cleanup(glbl_hif_handle);

    return; 
}

EXPORT_SYMBOL(hif_init);
EXPORT_SYMBOL(hif_cleanup);
EXPORT_SYMBOL(hif_start);
EXPORT_SYMBOL(hif_claim_device);
EXPORT_SYMBOL(hif_send);
EXPORT_SYMBOL(hif_register);
EXPORT_SYMBOL(hif_get_reserveheadroom);
EXPORT_SYMBOL(hif_get_ulpipe);
EXPORT_SYMBOL(hif_get_dlpipe);
#ifdef GMAC_8021Q_SUPPORT 
EXPORT_SYMBOL(hif_gmac_vlan_addvap);
EXPORT_SYMBOL(hif_gmac_vlan_delvap);
EXPORT_SYMBOL(hif_gmac_vlan_create);
EXPORT_SYMBOL(hif_gmac_vlan_wlan_rx);
#endif
EXPORT_SYMBOL(hif_init_dbglog);
EXPORT_SYMBOL(hif_register_for_dbglog);
EXPORT_SYMBOL(hif_send_dbglog);

module_init(gmac_module_init);
module_exit(gmac_module_exit);
module_param(hst_if, charp, 0600);
MODULE_PARM_DESC(hst_if, "Interface used to connect host ");
MODULE_AUTHOR("Atheros Communications, Inc.");
MODULE_DESCRIPTION("Atheros Device Module");
MODULE_LICENSE("Dual BSD/GPL");
