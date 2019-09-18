#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/netdevice.h>
#include <linux/skbuff.h>
#include <linux/delay.h>
#include <linux/timer.h>
#include <linux/jiffies.h>
#include <a_base_types.h>
#include <hif_api.h>

typedef struct loop_stats{
    uint32_t   tx_pkts;
    uint32_t   rx_pkts;
    uint32_t   tx_bytes;
    uint32_t   rx_bytes;
}loop_stats_t;

typedef struct loop_sc{
    struct net_device          *dev;
    struct net_device_stats     dev_stats;
    hif_handle_t                handle;

    uint8_t                     tx_pipe;
    uint8_t                     rx_pipe;

    spinlock_t                  lock_irq;
    loop_stats_t                stats;

    struct timer_list           timer;
}loop_sc_t;


#define LOOP_TIMER_GRAN     1000 /* 1 sec */

uint8_t loop_drop_rx  = 1;


#define KEGS                    1024
#define MEGS                    (1024 * KEGS)
#define BYTES_TO_MBITS(bytes)   (((bytes) * 8)/ MEGS)
#define BYTES_TO_KBITS(bytes)   (((bytes) * 8)/ KEGS)

#define handle_to_sc(hdl)       (struct loop_sc *)hdl

struct loop_sc  *G_sc;

void
loop_mod_timer(struct timer_list    *timer)
{
    mod_timer(timer, (jiffies + msecs_to_jiffies(LOOP_TIMER_GRAN)));
}

/******************************** TX & RX handlers *****************/
#if 0
a_uint8_t
test_swap_ip(adf_nbuf_t    buf)
{
    adf_os_sglist_t    sg   = {0};
    adf_net_iphdr_t   *ih  = {0};
    a_uint32_t        *src;
    a_uint32_t        *dst;
    int               i = 0, len;

    adf_nbuf_frag_info(buf, &sg);

    for(i = 0, len = 0; i < sg.nsegs; i++){
        if((len + sg.sg_segs[i].len) > sizeof(adf_net_ethhdr_t))
            break;
        len += sg.sg_segs[i].len;
    }
    /**
     * No segements found fatal
     */
    adf_os_assert(i < sg.nsegs);
    /**
     * The segment should atleast contain an IP header
     */
    adf_os_assert((sg.sg_segs[i].len - len) >= 
                  sizeof(adf_net_iphdr_t)); 

    ih = (adf_net_iphdr_t *)(sg.sg_segs[i].vaddr + 
                             sizeof(adf_net_ethhdr_t) - len);

    /**
     * Toggle the third octet (Class C)
     */
    src = &ih->ip_saddr;
    dst = &ih->ip_daddr;
/*  */
/*     ((a_uint8_t *)src)[2] ^= 1; */
/*     ((a_uint8_t *)dst)[2] ^= 1; */
/*  */

    return (((a_uint8_t *)dst)[3] & 1); 
}
void
test_fix_cksum(adf_nbuf_t  buf)
{
    adf_nbuf_rx_cksum_t  cksum = {0};

    cksum.result    = ADF_NBUF_RX_CKSUM_UNNECESSARY;
    
    adf_nbuf_set_rx_cksum(buf, &cksum);
}
#endif

/**
 * @brief Test Receive Function
 * 
 * @param context
 * @param buf
 * @param pipe
 * 
 * @return A_STATUS
 */
hif_status_t
loop_recv(void *context, struct sk_buff     *skb, uint8_t pipe)
{
    loop_sc_t   *sc = (loop_sc_t *)context;

    sc->stats.rx_pkts++;
    sc->stats.rx_bytes      += skb->len;
    sc->dev_stats.rx_bytes  += skb->len;

    if (likely(loop_drop_rx)) {
        dev_kfree_skb_any(skb);
        goto done;
    }

    netif_rx(skb);

done:
    return HIF_STATUS_OK;
}   

int
loop_xmit(struct sk_buff   *skb, struct net_device    *dev)
{
    loop_sc_t      *sc = netdev_priv(dev);
    hif_status_t    status = HIF_STATUS_ERROR;

    sc->stats.tx_pkts ++;
    sc->stats.tx_bytes      += skb->len;
    sc->dev_stats.tx_bytes  += skb->len;
    
    if (skb_headroom(skb) < hif_get_reserveheadroom(sc->handle))
        printk("<LOOP>:Headroom less\n");

    status = hif_send(sc->handle, sc->tx_pipe, skb); 

    if (status != HIF_STATUS_OK)
        goto fail;
    
    return NETDEV_TX_OK;

fail:
    dev_kfree_skb_any(skb);

    return NETDEV_TX_OK;
}

hif_status_t
loop_tx_done(void *context, struct sk_buff  *skb)
{
    dev_kfree_skb_any(skb);

    return HIF_STATUS_OK;
}
/******************************* Open & Close *************************/
int
loop_open(struct net_device     *dev)
{
    loop_sc_t   *sc = netdev_priv(dev);

    netif_start_queue(dev);

    loop_mod_timer(&sc->timer);

    return 0;
}

int
loop_stop(struct net_device    *dev)
{  
    loop_sc_t   *sc = netdev_priv(dev);

    dev->flags &= ~IFF_RUNNING;

    netif_stop_queue(dev);

    del_timer(&sc->timer);

    return 0;
}


void
loop_timer_run(unsigned long  arg)
{
    loop_sc_t   *sc = (loop_sc_t *)arg;
    uint32_t   drop;

    drop = sc->stats.tx_pkts - sc->stats.rx_pkts;

    printk("tx( %d, %d ), rx( %d, %d ), drop( %d )\n",
            sc->stats.tx_pkts, BYTES_TO_MBITS(sc->stats.tx_bytes),
            sc->stats.rx_pkts, BYTES_TO_MBITS(sc->stats.rx_bytes),
            drop);

    memset(&sc->stats, 0, sizeof(struct loop_stats));

    loop_mod_timer(&sc->timer);
}

struct net_device_stats *
loop_get_stats(struct net_device *dev)
{
    loop_sc_t   *sc = netdev_priv(dev);

    return &sc->dev_stats;
}

/*************************** Device Inserted & Removed handler ********/


struct net_device_ops  loop_dev_ops = {
    .ndo_open               = loop_open,
    .ndo_stop               = loop_stop,
    .ndo_start_xmit         = loop_xmit,
    .ndo_get_stats          = loop_get_stats,
};

loop_sc_t *
loop_dev_create(hif_handle_t    handle)
{
    struct net_device   *dev;
    loop_sc_t           *sc;

    dev = alloc_netdev(sizeof(struct loop_sc), "ld%d", ether_setup);
    
    dev->netdev_ops         = &loop_dev_ops;
    dev->destructor         = free_netdev;
    dev->hard_header_len   += hif_get_reserveheadroom(handle);

    sc = netdev_priv(dev);

    sc->dev     = dev;
    sc->handle  = handle;

    sc->tx_pipe  = hif_get_dlpipe(HIF_CONTROL_PIPE); /* Load the tx pipes*/
    sc->rx_pipe  = hif_get_ulpipe(HIF_CONTROL_PIPE); /* Load the rx pipes*/

    spin_lock_init(&sc->lock_irq);

    init_timer(&sc->timer);
    sc->timer.function  = loop_timer_run;
    sc->timer.data      = (unsigned long)sc;
    
    memcpy(dev->dev_addr, "LD0\0", 6);
    memcpy(dev->perm_addr, "LD\0", 6);
    
    register_netdev(dev);

    return 0;
}

hif_status_t
loop_insert_dev(hif_handle_t    handle)
{
    hif_callback_t      pkt_cb = {0};
    loop_sc_t          *sc;

    sc = loop_dev_create(handle);

    if (!sc) return HIF_STATUS_ERROR;
    
    G_sc = sc;

    hif_claim_device(handle, sc);

    pkt_cb.context          = sc;
    pkt_cb.recv_buf         = loop_recv;
    pkt_cb.send_buf_done    = loop_tx_done;

    hif_register(handle, &pkt_cb);
              
    hif_start(handle);

    return HIF_STATUS_OK;    
}

hif_status_t
loop_remove_dev(void    *ctx)
{   
    loop_sc_t   *sc = handle_to_sc(ctx);

    hif_cleanup(sc->handle);

    unregister_netdev(sc->dev);

    return HIF_STATUS_OK;
}
/************************** Module Init & Exit ********************/
int
loop_mod_init(void)
{
    hif_os_callback_t cb = {0};

    cb.device_detect        = loop_insert_dev;
    cb.device_disconnect    = loop_remove_dev;

    hif_init(&cb);
        
    return 0;
}
void
loop_mod_exit(void)
{
    loop_remove_dev(G_sc);
}

module_init(loop_mod_init);
module_exit(loop_mod_exit);
