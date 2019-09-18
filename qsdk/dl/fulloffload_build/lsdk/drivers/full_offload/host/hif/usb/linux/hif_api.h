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

/*
 * @File: HIF_api.h
 * 
 * @Abstract: HIF interface api
 * 
 */

#ifndef _HIF_API_H
#define _HIF_API_H


typedef void* hif_handle_t;
typedef enum{
    HIF_STATUS_ERROR = -1,
    HIF_STATUS_OK = 0,
} hif_status_t;


typedef struct _hif_os_callback{
    hif_status_t (*device_detect)(hif_handle_t hif_handle);
    hif_status_t (*device_disconnect)  (void *ctx);
    hif_status_t (*device_suspend) (void *ctx);
    hif_status_t (*device_resume ) (void *ctx);
    hif_status_t (*device_bootinit ) (void);
}hif_os_callback_t;


typedef struct _hif_callback {
    hif_status_t (*recv_buf)(void *ctx,struct sk_buff *netbuf, 
                             uint8_t pipeid);
    hif_status_t (*send_buf_done)(void *ctx, struct sk_buff *netbuf);
    void *context;
} hif_callback_t;

typedef enum{
    HIF_CONTROL_PIPE = 0,
    HIF_COMMAND_PIPE = 1,
    HIF_DATA_LP_PIPE = 2,
    HIF_WSUPP_PIPE = 3,
}hif_pipetype_t;


hif_status_t  hif_init(hif_os_callback_t *cb);
void          hif_claim_device(hif_handle_t hif_handle, void *ctx);
void          hif_register(hif_handle_t hif_handle, hif_callback_t *cb);
void          hif_deregister(hif_handle_t hif_handle);
hif_status_t  hif_start(hif_handle_t hif_handle);
hif_status_t  hif_cleanup(hif_handle_t hif_handle);
uint32_t    hif_get_reserveheadroom(hif_handle_t hif_handle);
uint32_t    hif_get_dlpipe(hif_pipetype_t type);
uint32_t    hif_get_ulpipe(hif_pipetype_t type);
hif_status_t  hif_send(hif_handle_t hif_handle ,uint32_t, struct sk_buff *);

/* The APIs exposed for HIF USB only */
uint32_t      usb_get_devid(hif_handle_t hif_handle);
hif_status_t  usb_download_image(hif_handle_t hif_handle, uint8_t cmd, 
                                    uint32_t tgt_add, uint8_t* image, 
                                    uint32_t image_len);


#endif /* #ifndef _HIF_API_H */
