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


#ifndef __ATD_NETLINK_H
#define __ATD_NETLINK_H
/* netlink library prototypes/structures */
typedef void * atd_tgt_netlink_handle_t;
typedef void (*atd_tgt_netlink_cb_t)(void *ctx, struct sk_buff *skb, int addr);
typedef struct atd_tgt_netlink_softc {
    struct sock *sock;
    atd_tgt_netlink_cb_t input;
    void *ctx;
} atd_tgt_netlink_softc_t;

/* netlink functions used by wsupp helper */
atd_tgt_netlink_handle_t atd_tgt_netlink_create(atd_tgt_netlink_cb_t input,
        void *ctx, uint32_t unit, uint32_t groups);

void atd_tgt_netlink_delete(atd_tgt_netlink_handle_t nlhdl);

struct sk_buff *atd_tgt_netlink_alloc(uint32_t len);

a_status_t atd_tgt_netlink_broadcast(atd_tgt_netlink_handle_t nlhdl,
        struct sk_buff *skb, int groups);
 
a_status_t
atd_tgt_netlink_unicast(atd_tgt_netlink_handle_t nlhdl,
                struct sk_buff *skb,
                        int addr);


/* ATD TGT Netlink Library */

#ifdef NL_DEBUG
#define nldebug printk
#else
#define nldebug(...)
#endif

#endif
