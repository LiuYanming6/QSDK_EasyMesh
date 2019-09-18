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

#define MAX_HTC_MSG_SIZE 1514
typedef struct _htc_endpoint {
    htc_epcallback_t              ep_cb;  /* callbacks associated with 
                                                   this endpoint */
    a_uint8_t                     ul_pipeid;
    a_uint8_t                     dl_pipeid;
} htc_endpoint_t;


/* our HTC target state */

typedef struct _htc_softc {
    hif_handle_t                hif_handle;
    void                       *host_handle;
    htc_endpoint_t              ep_list[ENDPOINT_MAX];
    a_uint8_t                   htc_stop_flag;  /* 0:non-stop, 1:wait to stop, 2:stopped */
} htc_softc_t;
/* internal function */
static hif_status_t 
htc_send_comp(void *ctx, struct sk_buff * netbuf);
static hif_status_t 
htc_recv(void *ctx, struct sk_buff *netbuf, a_uint8_t pipeid);


static a_uint8_t htc_map_ep2ulpipe(htc_endpointid_t epid );
static a_uint8_t htc_map_ep2dlpipe(htc_endpointid_t epid );
static a_status_t  
htc_issue_send(htc_softc_t *sc, struct sk_buff * netbuf, 
              a_uint8_t flag, a_uint16_t len, a_uint8_t epid);
static void
htc_control_msg_recv(void *ctx, struct sk_buff * netbuf, 
                     htc_endpointid_t epid);
static a_status_t  
htc_issue_send(htc_softc_t *sc, struct sk_buff * netbuf, 
              a_uint8_t flag, a_uint16_t len, a_uint8_t epid);





#define htc_trc_error printk



