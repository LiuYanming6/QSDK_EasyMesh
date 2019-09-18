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

/**
 * HIF_TARGET_BOOT_DELAY_MS: 
 * Host Initial *wait* for the target's complete boot.
 * after this timer, host can start target state detection algorithm.
 */
#define HIF_TARGET_BOOT_DELAY_MS    (10000) 

/** 
 * HIF_TARGET_POLL_TIMER_MS: 
 * Host Poll timer to poll target's MDIO registers.
 * Used to detect target's state 
 */
#define HIF_TARGET_POLL_TIMER_MS    (1000)

/**
 * HIF_TARGET_REBOOT_DELAY_MS: 
 * When Host detected target reset, This is the time, 
 * Host should *wait* for the target related data structure cleanup 
 */
#define HIF_TARGET_REBOOT_DELAY_MS  (5000)
/** 
 * @brief Target Status Flags
 */
typedef enum{
    HIF_DEVICE_STATE_OK         = 0x00,
    HIF_DEVICE_STATE_RESET      = 0x01,
    HIF_DEVICE_STATE_BOOTED     = 0x02,
    HIF_DEVICE_STATE_UNKNOWN    = 0x03,
    HIF_DEVICE_STATE_ERROR      = 0xFF,
}hif_device_state_t;

typedef struct _hif_os_callback{
    hif_status_t (*device_detect)(hif_handle_t hif_handle);
    hif_status_t (*device_disconnect)  (void *ctx);
    hif_status_t (*device_suspend) (void *ctx);
    hif_status_t (*device_resume ) (void *ctx);
    hif_status_t (*device_bootinit ) (void);
}hif_os_callback_t;


typedef struct _hif_callback {
    hif_status_t (*recv_buf)(void *ctx, struct sk_buff *netbuf, 
                             a_uint8_t pipeid);
    hif_status_t (*send_buf_done)(void *ctx, struct sk_buff *netbuf);
    void *context;
} hif_callback_t;

typedef enum{
    HIF_CONTROL_PIPE = 1,
    HIF_COMMAND_PIPE = 1,
    HIF_DATA_LP_PIPE = 1,
    HIF_WSUPP_PIPE = 1,
    HIF_DBGLOG_PIPE = 2,
}hif_pipetype_t;


hif_status_t  hif_init(hif_os_callback_t *cb);
void          hif_claim_device(hif_handle_t hif_handle, void *ctx);
void          hif_register(hif_handle_t hif_handle, hif_callback_t *cb);
void          hif_deregister(hif_handle_t hif_handle);
hif_status_t  hif_start(hif_handle_t hif_handle);
hif_status_t  hif_cleanup(hif_handle_t hif_handle);
a_uint32_t    hif_get_reserveheadroom(hif_handle_t hif_handle);
a_uint32_t    hif_get_dlpipe(hif_pipetype_t type);
a_uint32_t    hif_get_ulpipe(hif_pipetype_t type);
hif_status_t  hif_send(hif_handle_t hif_handle ,a_uint32_t, struct sk_buff *);

hif_status_t  hif_init_dbglog(hif_os_callback_t *cb);
void          hif_register_for_dbglog(hif_handle_t hif_handle, hif_callback_t *cb);
hif_status_t  hif_send_dbglog(hif_handle_t hif_handle ,a_uint32_t, struct sk_buff *);

#endif /* #ifndef _HIF_API_H */
