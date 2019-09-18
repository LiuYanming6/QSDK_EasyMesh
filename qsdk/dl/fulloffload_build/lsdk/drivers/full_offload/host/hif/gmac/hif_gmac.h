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

#ifndef __HIF_GMAC_H
#define __HIF_GMAC_H

#include <linux/if_ether.h>
#include <adf_os_io.h>
#include <adf_net_types.h>
#include <adf_nbuf.h>
#include <adf_os_lock.h>
#include <adf_os_defer.h>
#include <if_ath_gmac.h>
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
#define GMAC_HASH_SHIFT     2 
#define GMAC_TBL_SIZE       (1 << GMAC_HASH_SHIFT)


typedef struct ethhdr     ethhdr_t;

typedef struct athtype{
#if defined (ADF_LITTLE_ENDIAN_MACHINE)
    a_uint8_t    proto:6,
                 res :2;
#elif defined (ADF_BIG_ENDIAN_MACHINE)
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


typedef void (*__gmac_recv_fn_t)(adf_nbuf_t  skb, struct hif_gmac_node  *node, 
                                a_uint8_t proto);

typedef enum {
    RECV_EXCEP,
    RECV_DEF,
    NUM_RECV
}hif_gmac_recv_idx_t;

/**
 * Per device structure
 */
typedef struct hif_gmac_node{
    os_gmac_dev_t          *dev; /* Netdev associated with this device */
    gmac_hdr_t              hdr; /* GMAC header to insert per packet */
    a_uint32_t              host_is_rdy; /* State of the HOST APP */
    adf_nbuf_t              tgt_rdy_skb; /* State of the TGT FW */
    adf_os_handle_t         os_hdl; /* OS handle to be used during attach */
    adf_os_work_t           work;
    a_uint32_t              device_detected;
}hif_gmac_node_t;

/* Per APP structure */
typedef struct hif_gmac_app{
    hif_os_callback_t      dev_cb;
    hif_callback_t         pkt_cb; /* Device specific callbacks */
}hif_gmac_app_t;

typedef void (* os_dev_fn_t)(hif_gmac_node_t  *node);
typedef int (* os_dev_discv_t)(adf_nbuf_t  skb, hif_gmac_node_t   *node, 
                               int disc_resp);

/* HIF GMAC softc */
typedef struct hif_gmac_softc{
    os_dev_discv_t          discv;
    hif_gmac_app_t          app_reg;
    void*                   app_context;
    __gmac_recv_fn_t        recv_fn[NUM_RECV_FNS];
    adf_os_spinlock_t       lock_bh;
    hif_gmac_node_t         node_tbl[GMAC_TBL_SIZE]; /* XXX: size should be 
                                                        programmable */
}hif_gmac_softc_t;

hif_gmac_softc_t  * hif_gmac_init(void);
void                hif_gmac_exit(hif_gmac_softc_t *sc);
void                hif_gmac_dev_attach(void  *node);
void                hif_gmac_dev_detach(hif_gmac_node_t  *node);


static inline athhdr_t *
ath_hdr(adf_nbuf_t skb)
{
    a_uint8_t  *data = NULL;
    a_uint32_t  len = 0;

    adf_nbuf_peek_header(skb, &data, &len);

    return (athhdr_t *)data;
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

    ath  = (athhdr_t *)adf_nbuf_push_head(skb, ATH_HLEN);
    ath->type.proto = hdr->type.proto;
}
#endif

