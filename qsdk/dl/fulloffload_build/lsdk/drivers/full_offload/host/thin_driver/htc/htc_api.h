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

#ifndef __HTC_API_H
#define __HTC_API_H

typedef void * htc_handle_t;
typedef void   (*htc_ep_txcomp_fn)(void * , adf_nbuf_t  , htc_endpointid_t);
typedef void   (*htc_ep_recv_fn)  (void * , adf_nbuf_t  , htc_endpointid_t);

typedef struct _htc_ep_cb {
    void                     *ep_ctx  ;      /* context for each callback */
    htc_ep_txcomp_fn          ep_txcomp;     /* tx completion callback for
                                                connected endpoint */
    htc_ep_recv_fn            ep_rx;         /* receive callback for 
                                                connected endpoint */
} htc_epcallback_t;

typedef struct _htc_connect_service_req {
    htc_epcallback_t cb;
    a_uint8_t svcid;
} htc_connect_service_req_t ;

typedef struct _htc_connect_service_resp {
    htc_endpointid_t epid;
} htc_connect_service_resp_t;



typedef struct _htc_config{
    hif_handle_t hif_handle;
    a_status_t (*htc_ready)(void *ctx);
    void *ctx;
}htc_config_t;

htc_handle_t htc_create(htc_config_t *htc_conf);
a_status_t   htc_wait_for_target_ready(htc_handle_t htc_handle);
void htc_start(htc_handle_t );
void htc_cleanup(htc_handle_t htc_handle);
a_status_t htc_connect_service (htc_handle_t htc ,
                                htc_connect_service_req_t *creq,
                                htc_connect_service_resp_t  *cresp);
a_status_t htc_send(htc_handle_t htc_handle, adf_nbuf_t netbuf, 
                       htc_endpointid_t epid);
a_uint32_t htc_get_reserveheadroom(htc_handle_t htc_handle);

#endif /* __HTC_API_H */
