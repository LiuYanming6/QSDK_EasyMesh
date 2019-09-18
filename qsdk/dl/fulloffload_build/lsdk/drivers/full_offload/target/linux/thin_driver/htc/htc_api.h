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

#include <htc.h>
#include <hif_api.h>

typedef void* htc_handle_t;
typedef void   (*htc_ep_txcomp_fn)(void *, struct sk_buff *, htc_endpointid_t);
typedef void   (*htc_ep_recv_fn)  (void *, struct sk_buff *, htc_endpointid_t);
typedef struct _htc_ep_cb {
    void                     *ep_ctx  ;      /* context for each callback */
    htc_ep_txcomp_fn          ep_txcomp;     /* tx completion callback for
                                                connected endpoint */
    htc_ep_recv_fn            ep_rx;         /* receive callback for 
                                                connected endpoint */
} htc_epcallback_t;


typedef struct _htc_config{
    hif_handle_t hif_handle;
    void *ctx;
}htc_config_t;

htc_handle_t htc_create(htc_config_t *htc_conf);
void htc_start(htc_handle_t );
void htc_cleanup(htc_handle_t htc_handle);
a_status_t htc_send(htc_handle_t htc_handle, struct sk_buff * netbuf, 
                       htc_endpointid_t epid);
void htc_set_stopflag(htc_handle_t htc_handle, a_uint8_t val);                       
htc_endpointid_t  htc_register_service (htc_handle_t htc_handle ,
                                        htc_epcallback_t *cb,
                                        htc_service_t  svcid);
a_uint32_t htc_get_reserveheadroom(htc_handle_t htc_handle);
void htc_send_targe_tready(htc_handle_t htc_handle);















