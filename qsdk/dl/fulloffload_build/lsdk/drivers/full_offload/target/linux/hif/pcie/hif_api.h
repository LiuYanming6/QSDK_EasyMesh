/*
* Copyright (c) 2012 Qualcomm Atheros, Inc..
* All Rights Reserved.
* Qualcomm Atheros Confidential and Proprietary.
*
* @File: hif_api.h
* 
* @Abstract: HIF interface api
* 
* @Notes:
*/


#ifndef _HIF_API_H
#define _HIF_API_H


/** 
 * @brief HIF handle
 */
typedef void* hif_handle_t;


/** 
 * @brief HIF status codes
 */
typedef enum{
    HIF_STATUS_ERROR = -1,
    HIF_STATUS_OK = 0,
} hif_status_t;


/** 
 * @brief OS callbacks
 */
typedef struct _hif_os_callback{
    hif_status_t (*device_detect)(hif_handle_t hif_handle);
    hif_status_t (*device_disconnect)  (void *ctx);
    hif_status_t (*device_suspend) (void *ctx);
    hif_status_t (*device_resume ) (void *ctx);
}hif_os_callback_t;


/** 
 * @brief HIF callback
 */
typedef struct _hif_callback {
    hif_status_t (*recv_buf)(void *ctx, struct sk_buff *netbuf, 
                             a_uint8_t pipeid);
    hif_status_t (*send_buf_done)(void *ctx, struct sk_buff *netbuf);
    void *context;
} hif_callback_t;

/** 
 * @brief 
 */
typedef enum{
    HIF_CONTROL_PIPE = 0,
    HIF_COMMAND_PIPE = 1,
    HIF_DATA_LP_PIPE = 2,
    HIF_WSUPP_PIPE = 3
}hif_pipetype_t;


/** 
 * @brief 
 * 
 * @param cb
 * 
 * @return 
 */
hif_status_t  
hif_init(hif_os_callback_t *cb);


/** 
 * @brief 
 * 
 * @param hif_handle
 * @param ctx
 */
void 
hif_claim_device(hif_handle_t hif_handle, void *ctx);

/** 
 * @brief 
 * 
 * @param hif_handle
 * @param cb
 */
void 
hif_register(hif_handle_t hif_handle, hif_callback_t *cb);


/** 
 * @brief 
 * 
 * @param hif_handle
 * 
 * @return 
 */
hif_status_t  
hif_start(hif_handle_t hif_handle);


/** 
 * @brief 
 * 
 * @param hif_handle
 * 
 * @return 
 */
hif_status_t  
hif_cleanup(hif_handle_t hif_handle);


/** 
 * @brief 
 * 
 * @param hif_handle
 * 
 * @return 
 */
a_uint32_t 
hif_get_reserveheadroom(hif_handle_t hif_handle);


/** 
 * @brief 
 * 
 * @param type
 * 
 * @return 
 */
a_uint32_t  
hif_get_dlpipe(hif_pipetype_t type);


/** 
 * @brief 
 * 
 * @param type
 * 
 * @return 
 */
a_uint32_t  
hif_get_ulpipe(hif_pipetype_t type);


/** 
 * @brief 
 * 
 * @param hif_handle
 * @param a_uint32_t
 * @param 
 * 
 * @return 
 */
hif_status_t  
hif_send(hif_handle_t hif_handle ,a_uint32_t, struct sk_buff *);


#endif /* #ifndef _HIF_API_H */
