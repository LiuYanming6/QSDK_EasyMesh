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

#include <linux/version.h>
#include <linux/skbuff.h>
#include <linux/netdevice.h>
#include <linux/if_ether.h>
#include <linux/etherdevice.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/timer.h>

#include "a_base_types.h"
#include "hif_api.h"

#include <asm/byteorder.h>

#ifdef ROM_MODE
#define MDIO_BOOT_LOAD
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
a_uint32_t hif_tgt_pwrdown = 0x0;
static char *tgt_if = "eth0";

struct gmac_softc ;

int __gmac_dev_event(struct notifier_block *nb, unsigned long event, 
                     void *dev);
int __gmac_recv(struct sk_buff  *skb,  struct net_device  *dev,
                struct packet_type  *type, struct net_device   *orig_dev);


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
    struct work_struct     defer_work;
    struct work_struct     disconnect_work;
    struct work_struct     tgt_reset_work; /*for target reboot*/
    struct sk_buff*        tgt_rdy_skb;
#ifdef GMAC_FRAGMENT_SUPPORT
    struct sk_buff         *fragment;
    spinlock_t             frag_lock;
#endif
    a_uint32_t             ready;
    a_uint32_t             status;
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
    gmac_node_t  node ;
    struct timer_list reboot_timer;
    struct timer_list remove_timer;
}gmac_softc_t ;

struct packet_type       __gmac_pkt = {
    .type = __constant_htons(ETH_P_ATH),
    .func = __gmac_recv, /* GMAC receive method */
};
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
static void   gmac_process_disc(struct sk_buff   *skb, gmac_softc_t * sc, 
                                int send_resp );
static void   gmac_defer_detect(struct work_struct *work );

#ifdef MDIO_BOOT_LOAD
static void   gmac_defer_disconnect(struct work_struct *work );
#ifdef OFFLOAD_TGT_RECOVERY
static void   gmac_tgt_poll_timer(unsigned long ctx);
static void   gmac_start_tgt_poll_timer(gmac_softc_t * sc);
extern int    mdio_boot_init(int unit);
extern int    mdio_reg_read(int unit , int reg);
extern int    mdio_reg_write(int unit , int reg, int val);
static void   tgt_recovery_bootinit(struct work_struct *work );

#define MDIO_REG_INDEX_START   0
#define MDIO_REG_INDEX_END     7
/* on host pb44 we need to access by indexing with 2, on magpie it is 4 */
#define MDIO_REG_WIDTH         2   /* 2 bytes */
#define MAX_MDIO_IO_LEN        14
#define MDIO_REG_TO_OFFSET(__reg_num__)\
            (0x7 << 8)+(MDIO_REG_WIDTH*(__reg_num__))

uint8_t mac_id = 0;
module_param(mac_id, byte, 0600);
MODULE_PARM_DESC(mac_id, "MAC(mac0/mac1) Interface to which target's MDC/MDIO lines are connected");

static unsigned int hif_tgt_reset_pattern_invalid = 1;
const uint16_t TGT_RESET_MAGIC_PATTERN[MDIO_REG_INDEX_END+1] = 
                 {0x3A3B,0x3C3D,0x3E3F,0x4A4B,0x4C4D,0x4E4F,0x5A5B,0x5C5D};

static hif_status_t hif_write_mdio_patten(void)
{
    unsigned int mdio_reg_num = 0;
    volatile uint16_t mdio_reg_val = 0;

    /**
     * NOTE: thre is a issue in writing if target is already in reset state. 
     * if in reset state, target expects only 1st stage FW. Right now assumption 
     * is target can't crash before the (HIF_TARGET_BOOT_DELAY_MS + 1) sec. 
     * Otherwise we need to move the pattern writing to FWD module */

    /* Write the Magic Pattern */
    printk("\nTarget Reset Magic Patteren set in MDIO_REGS[]={");
    for (mdio_reg_num = MDIO_REG_INDEX_START;
              mdio_reg_num <= MDIO_REG_INDEX_END ; mdio_reg_num++) {
              mdio_reg_val = TGT_RESET_MAGIC_PATTERN[mdio_reg_num];
              mdio_reg_write(mac_id, MDIO_REG_TO_OFFSET(mdio_reg_num), mdio_reg_val);
              printk("0x%x, ",mdio_reg_val);
    }
    printk("}\n");

    return HIF_STATUS_OK;
}

static hif_status_t hif_validate_mdio_pattern(void)
{
    unsigned int mdio_reg_num = 0;
    volatile uint16_t mdio_reg_val = 0;

    /* Read-back the Magic Pattern and verify */
    for (mdio_reg_num = (MDIO_REG_INDEX_START);
              mdio_reg_num <= (MDIO_REG_INDEX_END) ; mdio_reg_num++) {
          mdio_reg_val = mdio_reg_read(mac_id, MDIO_REG_TO_OFFSET(mdio_reg_num));
          if(mdio_reg_val != TGT_RESET_MAGIC_PATTERN[mdio_reg_num]) {
               printk("\nTarget Reset Magic Patteren MISMATCH!!");
               printk(" MDIO_REGS[%d] values read:0x%x, expected:0x%x \n", 
                         mdio_reg_num, mdio_reg_val, 
                         TGT_RESET_MAGIC_PATTERN[mdio_reg_num]);
               return HIF_STATUS_ERROR;
          }
    }

    return HIF_STATUS_OK;
}

static void hif_check_target_status(gmac_softc_t *sc)
{
    hif_status_t status;

    if(hif_tgt_reset_pattern_invalid){
        /**
         * Write MAGIC Pattern to detect Chip Reset.
         * Idea is when target is hang, watchdog timer 
         * will reset the Chip and MAGIC pattern is earased 
         */
           hif_write_mdio_patten();
           hif_tgt_reset_pattern_invalid = 0;
    }

    sc->node.status = HIF_DEVICE_STATE_UNKNOWN;

    status = hif_validate_mdio_pattern();
    if(status != HIF_STATUS_OK) {
             hif_tgt_reset_pattern_invalid = 1;
             sc->node.status = HIF_DEVICE_STATE_RESET;
             printk("\n%s Target Reset Detected...\n", __func__);
             return;
    }
    sc->node.status = HIF_DEVICE_STATE_BOOTED;
}

static void
gmac_start_tgt_poll_timer(gmac_softc_t * sc)
{
    struct timer_list *timer;
    unsigned int init_delay = HIF_TARGET_BOOT_DELAY_MS;

    gmac_trc(GMAC_FUNCTRACE,("Start"));

    timer = &sc->remove_timer;
    init_timer(timer);
    timer->function = gmac_tgt_poll_timer;
    timer->data = (unsigned long)sc;

    timer->expires = jiffies + msecs_to_jiffies(init_delay);
    add_timer(timer);
    gmac_trc(GMAC_FUNCTRACE,("End"));
}

static void
gmac_tgt_poll_timer(unsigned long ctx)
{
    gmac_softc_t *sc  = (gmac_softc_t *)ctx;

    gmac_trc(GMAC_FUNCTRACE,(".Start"));

    /* if target is alive, re-start timer. 
     * otherwise, intimate about target's dosconnect state.
     */
    hif_check_target_status(sc);
    if(sc->node.status == HIF_DEVICE_STATE_BOOTED) {
       sc->remove_timer.expires = jiffies + 
                        msecs_to_jiffies(HIF_TARGET_POLL_TIMER_MS);
       add_timer(&sc->remove_timer);
    }
    else {
       gmac_node_t* node = &sc->node;
       del_timer(&sc->remove_timer);
       schedule_work(&node->disconnect_work);
       sc->node.ready = 0;
    }
    gmac_trc(GMAC_FUNCTRACE,(".End"));
}

static void
gmac_tgt_reboot_timer(unsigned long ctx)
{
    gmac_softc_t *sc  = (gmac_softc_t *)ctx;
    gmac_node_t* node = NULL;

    gmac_trc(GMAC_FUNCTRACE,(".Start"));

    /* if target is alive, re-start timer. 
     * otherwise, intimate about target's dosconnect state.
     */
    del_timer(&sc->reboot_timer);
   
    /*schedule worker thread here*/
    node = &sc->node;
    schedule_work(&node->tgt_reset_work);
 
    gmac_trc(GMAC_FUNCTRACE,(".End"));
}

static void
gmac_start_tgt_reboot_timer(gmac_softc_t * sc)
{
    struct timer_list *timer;

    gmac_trc(GMAC_FUNCTRACE,("Start"));

    timer = &sc->reboot_timer;
    init_timer(timer);
    timer->function = gmac_tgt_reboot_timer;
    timer->data = (unsigned long)sc;

    timer->expires = jiffies + msecs_to_jiffies(HIF_TARGET_REBOOT_DELAY_MS);
    add_timer(timer);
    gmac_trc(GMAC_FUNCTRACE,("End"));
}

static void 
tgt_recovery_bootinit(struct work_struct *work )
{
    gmac_node_t *node = container_of(work, gmac_node_t, tgt_reset_work);
    gmac_softc_t *sc  = node->sc;

    if(sc->oscb.device_bootinit) {
            sc->oscb.device_bootinit();
    }
}
#endif

static void
gmac_defer_disconnect(struct work_struct *work )
{
    gmac_node_t *node = container_of(work, gmac_node_t, disconnect_work);
    gmac_softc_t *sc  = node->sc;

    if (sc->oscb.device_disconnect) {
        sc->oscb.device_disconnect(sc->context);
    }

    if(sc->dbglog.oscb.device_disconnect) {
        sc->dbglog.oscb.device_disconnect(sc->context);
    }
}
#endif

static void gmac_recv_dbglog(gmac_node_t *node, int pipeid, struct sk_buff *skb);

hif_status_t
hif_init(hif_os_callback_t *oscb)
{
    gmac_softc_t *sc = (gmac_softc_t *)glbl_hif_handle;

    gmac_trc(GMAC_FUNCTRACE,("Start"));

    {
        /* TODO - Customer to add Host GPIO toggle code here */
        /* host_gpio_reset_assert(); */
        /* host_gpio_reset_deassert(); */
    }

    memcpy(&sc->oscb,oscb,sizeof(hif_os_callback_t));

    if(sc->node.ready){
            gmac_node_t* node = &sc->node;
            INIT_WORK(&node->defer_work, gmac_defer_detect);
            schedule_work(&node->defer_work);
    }else if(sc->oscb.device_bootinit) {
            sc->oscb.device_bootinit();
    }

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

    printk("tgt_if =%s \n",tgt_if);
     /*For TARGET_NULL only */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,30)    
    node->dev = dev_get_by_name(tgt_if);
#else
    node->dev = dev_get_by_name(&init_net, tgt_if);
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
        while(i <6 )
            mac[i++]=0xff;
      
        printk("mac address  src--> %2x:%2x:%2x:%2x:%2x:%2x   \n ", \
                mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);

    }
    node->hdr.eth.h_proto  = htons(ETH_P_ATH);
    node->hdr.ath.type.proto = ATH_P_MAGBOOT;

    /*For recive */
    __gmac_pkt.af_packet_priv  = sc ;
    dev_add_pack(&__gmac_pkt);
    register_netdevice_notifier(&__gmac_notifier);

out :    

    gmac_trc(GMAC_FUNCTRACE,("End"));
    return sc;
}

hif_status_t
hif_target_reset(hif_handle_t hif_handle)
{
    struct sk_buff* skb;
    gmac_softc_t *sc = (gmac_softc_t *)hif_handle;
    gmac_node_t *node = &sc->node;
    gmac_hdr_t *gmac;

    skb = dev_alloc_skb(500);
    if(!skb){
        printk("Reset Command Allocation Error \n");
        return HIF_STATUS_ERROR;
    }
    skb_reserve(skb, GMAC_HLEN);
    skb_put(skb, 500 - GMAC_HLEN);
    hif_gmac_header(skb, &node->hdr);
    gmac = (gmac_hdr_t*)skb->data;
    gmac->ath.type.proto = ATH_P_RESET;

    printk("Sending Reset command to target \n");
    return __os_gmac_xmit(skb, node->dev);

}

hif_status_t
hif_cleanup(hif_handle_t  hif_handle)
{
    gmac_softc_t *sc = (gmac_softc_t *)hif_handle;

    gmac_trc(GMAC_FUNCTRACE,("GMAC.Start"));
   
    /*reset target */
    if(!hif_tgt_pwrdown)
        hif_target_reset(sc);

#ifdef MDIO_BOOT_LOAD
#ifdef OFFLOAD_TGT_RECOVERY
    /* Don't remove gmac handles on cleanup. do it on module exit */
    /* this function should return so that atd cleanup is complete */
    /* start a HIF Timer to start the download again */
    gmac_start_tgt_reboot_timer(sc); 
#endif
#else
    dev_remove_pack(&__gmac_pkt);
    unregister_netdevice_notifier(&__gmac_notifier);
    if(sc != NULL)
       kfree(sc);
    glbl_hif_handle = NULL;
#endif

    gmac_trc(GMAC_FUNCTRACE,("GMAC.End"));

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
    gmac_node_t* node = &sc->node;

    gmac_trc(GMAC_FUNCTRACE,("x"));

    memcpy(&sc->cb,cb,sizeof(hif_callback_t));
    if(node->tgt_rdy_skb != NULL){
        printk("sending tgt rdy msg to upper layer \n");
        sc->cb.recv_buf(sc->cb.context,node->tgt_rdy_skb,1);
        node->tgt_rdy_skb = NULL;
    }
}

hif_status_t  
hif_start(hif_handle_t hif_handle)
{
    hif_status_t status = HIF_STATUS_OK;

#ifdef MDIO_BOOT_LOAD
    gmac_softc_t *sc = (gmac_softc_t *) hif_handle;
    gmac_node_t *node = &sc->node;

    INIT_WORK(&node->disconnect_work, gmac_defer_disconnect);

#ifdef OFFLOAD_TGT_RECOVERY
    /*to take care of firmware download in case of target reboot*/
    INIT_WORK(&node->tgt_reset_work, tgt_recovery_bootinit);
    gmac_start_tgt_poll_timer(sc);
#endif
#endif

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

    gmac_trc(GMAC_FUNCTRACE,("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"));

    if (skb_shared(skb))
        skb = skb_unshare(skb, GFP_ATOMIC);

    ath = (athhdr_t *) skb->data;

    gmac_trc(GMAC_DBGLVL0,("proto %x ", ath->type.proto));
    switch(ath->type.proto)
    {
        case ATH_P_MAGDISC:
            gmac_trc(GMAC_DBGLVL0,("discover packet "));
            gmac_process_disc(skb, sc, 1);
            return 0 ;
     
        case ATH_P_MAGBOOT:
        {
            struct ethhdr           *eth;
            eth = eth_hdr(skb); /* Linux specific */
            if(is_broadcast_ether_addr(&eth->h_dest[0])){
                gmac_trc(GMAC_DBGLVL0,("boot packet "));
                gmac_process_disc(skb, sc, 0);
                return 0 ;
            }
            break;
        }

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
                printk("Invalid last frag.Len%d first frag not present\n",skb->len);
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
                new_skb = dev_alloc_skb(totallen + 40);
                skb_reserve(new_skb,40);
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
        printk("Recv Buff Null Storing tgt rdy msg\n");
        node->tgt_rdy_skb = skb;
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
gmac_defer_detect(struct work_struct *work )
{
    gmac_node_t *node = container_of(work, gmac_node_t, defer_work);
    gmac_softc_t *sc  = node->sc;

    sc->oscb.device_detect((hif_handle_t)node->sc);

    if(sc->dbglog.oscb.device_detect)
        sc->dbglog.oscb.device_detect((hif_handle_t)node->sc);
}

static void 
gmac_process_disc(struct sk_buff   *skb, gmac_softc_t *sc, int send_resp)
{
    struct ethhdr * eh = eth_hdr(skb);
    gmac_node_t *node = &sc->node;
    athhdr_t    *ath = &node->hdr.ath;
    

    gmac_trc(GMAC_FUNCTRACE,("x"));

    if(node->dev == skb->dev)
        printk("Discovery pkt from  %s \n",node->dev->name);
    else
        printk("Discovery from %s tgt if = %s\n",skb->dev->name,node->dev->name);

    memcpy(node->hdr.eth.h_dest  , eh->h_source, ETH_ALEN);
    memcpy(node->hdr.eth.h_source, skb->dev->dev_addr, ETH_ALEN);
    dump_mac(node->hdr.eth.h_dest);
    dump_mac(node->hdr.eth.h_source);

    node->hdr.eth.h_proto  = htons(ETH_P_ATH);

    if(send_resp){
        if(sc->oscb.device_detect != NULL)
        {
            ath->type.proto = ATH_P_MAGDISCRESP;
            skb_pull(skb,ATH_HLEN);
            /*Send response packet */
            if(gmac_send_pkt(node,skb) != A_STATUS_OK){
                printk("GMAC ATH_P_MAGDISCRESP pkt send failed \n");
            }
            ath->type.proto = ATH_P_MAGNORM;
        }
        else{
            printk("Host not Ready skipping discovery response\n");
        }
    }
    else{
        ath->type.proto = ATH_P_MAGBOOT;
    }

    node->sc = sc;
    /*call back applicatin */
    if(!node->ready){
        node->ready = 1;
        if(sc->oscb.device_detect != NULL) {
            INIT_WORK(&node->defer_work, gmac_defer_detect);
            schedule_work(&node->defer_work);
        }
        else
            printk("applicatin ready handle null");

    }

}

hif_status_t
hif_init_dbglog(hif_os_callback_t *oscb)
{
    gmac_softc_t *sc = (gmac_softc_t *) glbl_hif_handle;

    memcpy(&sc->dbglog.oscb, oscb,sizeof(hif_os_callback_t));

    if(sc->dbglog.oscb.device_detect && sc->node.ready)
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

    gmac_trc(GMAC_FUNCTRACE,("x"));

    cb.context = NULL;
    cb.send_buf_done = NULL;
    cb.recv_buf    = NULL;

    memset(&oscb,0,sizeof(hif_os_callback_t));
    glbl_hif_handle = gmac_init();

    return 0;
}

void gmac_module_exit(void )
{
    gmac_softc_t *sc = (gmac_softc_t *)glbl_hif_handle;
    gmac_trc(GMAC_FUNCTRACE,("x"));

    if(sc != NULL){
        del_timer(&sc->reboot_timer);
        del_timer(&sc->remove_timer);
        /*reset target */
        if(!hif_tgt_pwrdown)
            hif_target_reset(sc);
        dev_remove_pack(&__gmac_pkt);
        unregister_netdevice_notifier(&__gmac_notifier);
        kfree(sc);
    }
    glbl_hif_handle = NULL;

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
EXPORT_SYMBOL(hif_init_dbglog);
EXPORT_SYMBOL(hif_register_for_dbglog);
EXPORT_SYMBOL(hif_send_dbglog);

module_init(gmac_module_init);
module_exit(gmac_module_exit);
module_param(hif_tgt_pwrdown, uint, 0);
module_param(tgt_if, charp, 0600);
MODULE_PARM_DESC(tgt_if, "Interface used to connect target ");
MODULE_PARM_DESC(hif_tgt_pwrdown, "If enabled, target reset command is not issued while unloading ");


MODULE_AUTHOR("Atheros Communications, Inc.");
MODULE_DESCRIPTION("Atheros Device Module");
MODULE_LICENSE("Dual BSD/GPL");
