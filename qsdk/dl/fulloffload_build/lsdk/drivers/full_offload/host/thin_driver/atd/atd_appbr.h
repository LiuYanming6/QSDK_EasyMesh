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


#ifndef __APP_BR_H
#define __APP_BR_H

#include <appbr_types.h>


struct atd_host_dev;
struct atd_host_vap;

#define ATD_APPBR_HDL(x)  (appbr_sc_t  *)((atd_host_dev_t  *)(x))->appbr_handle

typedef a_status_t (*appbr_os_fn)(void * ctx, adf_nbuf_t netbuf);

#define appbr_dbg_print(x...)     atd_trace(ATD_DEBUG_FUNCTRACE, (x)) 
/* #define appbr_dbg_print(x...)  adf_os_print(x)                  */

/** 
 * @brief  ApplicationBridge Kernel Callback structure
 */
typedef struct  appbr_cb_s {
    void            *ctx;
    appbr_os_fn     fn;
} appbr_cb_t;

typedef void * appbr_handle_t;

/** 
 * @brief APPBR module Softc
 */
typedef struct appbr_sc_s {
    struct atd_host_dev     *atd_handle;
    adf_netlink_handle_t    netlink_handle;          /** < Netlink Handle */
    appbr_cb_t              cb_list[APPBR_MAX_APPS]; /** < Callback Fn Table */
} appbr_sc_t;

/** 
 * @brief 
 */
typedef struct atd_appbr_hdr {
        a_uint8_t data[2];
} atd_appbr_hdr_t;

#define ATD_HDR_APPID_MASK      0xF

#if defined(OFFLOAD_APPBR)
/** 
 * @brief   ATD Application Bridge Module Init
 * 
 * @param atd_handle - Parent ATD Handle
 * @param atd_netlink_num
 * 
 * @return 
 */
appbr_handle_t
atd_appbr_init(void *atd_handle, a_uint32_t atd_netlink_num);

/** 
 * @brief Register Application Specific Callback
 * 
 * @param ctx
 * @param cb
 * @param app_id
 * 
 * @return 
 */
a_status_t
atd_appbr_register_cb(void * ctx,appbr_cb_t * cb, a_uint32_t app_id);

/** 
 * @brief  Send WMI event from Target to User Space APP as a Netlink Message
 * 
 * @param ctx
 * @param netbuf
 * @param addr
 *
 * @todo    Need to link this function to WMI event handler
 *
 */
void
atd_appbr_netlink_output(void *ctx, adf_nbuf_t netbuf, adf_netlink_addr_t addr);

/** 
 * @brief Cleanup allocated resources
 * 
 * @param ctx
 */
void
atd_appbr_cleanup(void *ctx);

#else

#define     atd_appbr_init(x, y)                    (x)
#define     atd_appbr_register_cb(x, y, z)          (A_STATUS_OK)
#define     atd_appbr_netlink_output(x, y, z)       ()
#define     atd_appbr_cleanup(x)                    do {} while(0)

#endif

#endif
