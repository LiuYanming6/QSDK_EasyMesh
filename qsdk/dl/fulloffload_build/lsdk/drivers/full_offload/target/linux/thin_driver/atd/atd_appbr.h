#ifndef __APP_BR_H
#define __APP_BR_H

#include <appbr_types.h>
#include <atd_netlink.h>

#define appbr_dbg_printk(x...)      atd_trace(ATD_DEBUG_FUNCTRACE, (x))    
/* #define appbr_dbg_printk(x...)         printk(x)   */

struct atd_tgt_dev;

#define ATD_APPBR_HDL(x)   (appbr_sc_t  *)((atd_tgt_dev_t  *)(x))->appbr_handle

typedef a_status_t (*appbr_os_fn)(void * ctx, struct sk_buff *skb);

/** 
 * @brief  ApplicationBridge Kernel Callback structure
 */
typedef struct  appbr_cb_s {
    void        *ctx;
    appbr_os_fn  fn;
} appbr_cb_t;

typedef void * appbr_handle_t;

/** 
 * @brief APPBR module Softc
 */
typedef struct appbr_sc_s {
    struct atd_tgt_dev          *atd_handle;            /**< Parent ATD Hdl*/  
    atd_tgt_netlink_handle_t    netlink_handle;         /**< Netlink Hdl */
    appbr_cb_t                  appbr_cb_list[APPBR_MAX_APPS];/**<CB Fn Table*/
} appbr_sc_t;

/** 
 * @brief 
 */
typedef struct atd_appbr_hdr {
    a_uint8_t     data[2];
} atd_appbr_hdr_t;

#define ATD_HDR_APPID_MASK      0xF

#if defined(OFFLOAD_APPBR)

/** 
 * @brief Initialize Netlink interface for ATD APPBR
 * 
 * @param ctx
 * 
 * @return 
 */
appbr_handle_t
atd_tgt_appbr_init(void *atd_handle, u_int32_t atd_netlink_num);

/** 
 * @brief 
 * 
 * @param ctx
 * @param cb
 * @param app_id
 * 
 * @return 
 */
a_status_t
atd_tgt_appbr_register_cb(void *ctx, appbr_cb_t *cb, u_int32_t app_id);

/** 
 * @brief 
 * 
 * @param ctx
 * @param cmdid
 * @param buffer
 * @param len
 */
void
atd_tgt_appbr_input(void *ctx, a_uint16_t cmdid,
        a_uint8_t *buffer, a_int32_t len);

/** 
 * @brief 
 * 
 * @param ctx
 */
void
atd_tgt_appbr_cleanup(void *ctx);

#else /** OFFLOAD_APP_BR */

#define     atd_tgt_appbr_init(x, y)           (x)
#define     atd_tgt_appbr_register_cb(x, y, z) (A_STATUS_OK)
#define     atd_tgt_appbr_input                NULL
#define     atd_tgt_appbr_cleanup(x)           do {} while(0)

#endif


#endif
