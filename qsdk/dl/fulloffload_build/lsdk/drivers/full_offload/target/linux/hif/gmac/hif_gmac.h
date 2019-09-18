#ifndef __HIF_GMAC_H
#define __HIF_GMAC_H

#include <linux/if_ether.h>
#include <linux/skbuff.h>
#include <linux/netdevice.h>

#include <a_base_types.h>
#include <hif_api.h>

#define ETH_P_ATH               0x88BD
#define ATH_P_MAGBOOT           0x12 /*Magpie is booting*/
#define ATH_P_MAGNORM           0x13 /*Magpie is in default state*/
#define ATH_P_MAGDISC           0x14
#define ATH_P_MAGDISCRESP       0x15
#define ATH_HLEN                sizeof(struct athhdr)
#define GMAC_HLEN               sizeof(struct gmac_hdr)
#define NUM_RECV_FNS            2
/**
 * Change this for handling multiple magpies
 */
#define GMAC_HASH_SHIFT     1 
#define GMAC_TBL_SIZE       (1 << GMAC_HASH_SHIFT)


typedef struct ethhdr     ethhdr_t;

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
    ethhdr_t    eth;
    athhdr_t    ath;
}gmac_hdr_t;

struct hif_gmac_softc;
struct hif_gmac_node;


/**
 * Per device structure
 */
typedef struct hif_gmac_node{
    struct net_device      *dev; /* Netdev associated with this device */
    gmac_hdr_t              hdr; /* GMAC header to insert per packet */
}hif_gmac_node_t;

/* Per APP structure */
typedef struct hif_gmac_app{
    hif_os_callback_t      dev_cb;
    hif_callback_t         pkt_cb; /* Device specific callbacks */
}hif_gmac_app_t;


/* HIF GMAC softc */
typedef struct hif_gmac_softc{
    hif_gmac_app_t          app_reg;
    void*                   app_context;
    spinlock_t              lock_bh;
    hif_gmac_node_t         node_tbl[GMAC_TBL_SIZE]; /* XXX: size should be 
                                                        programmable */
    struct timer_list       disc_timer;
    hif_gmac_node_t         *active_node;
}hif_gmac_softc_t;

hif_gmac_softc_t  * hif_gmac_init(void);
void                hif_gmac_exit(hif_gmac_softc_t *sc);
void                hif_gmac_dev_attach(hif_gmac_node_t  *node);
void                hif_gmac_dev_detach(hif_gmac_node_t  *node);


static inline athhdr_t *
ath_hdr(struct sk_buff *skb)
{
    return (athhdr_t *)skb->data;
}
static inline a_bool_t
is_ath_header(athhdr_t  *ath, a_uint8_t  sub_type)
{
    return ( ath->type.proto == sub_type);
}

static inline void
ath_put_hdr(struct sk_buff *skb, athhdr_t  *hdr)
{
    athhdr_t  *ath;

    ath  = (athhdr_t *)skb_push(skb, ATH_HLEN);
    ath->type.proto = hdr->type.proto;
}
static inline int
os_gmac_xmit(struct sk_buff* skb, struct net_device *dev)
{
    skb->dev = dev;
    return (dev_queue_xmit(skb) == 0) ? A_STATUS_OK: A_STATUS_FAILED;
}
#endif

