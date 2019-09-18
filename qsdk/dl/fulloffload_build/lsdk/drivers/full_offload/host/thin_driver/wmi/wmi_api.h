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

#ifndef __WMI_API_H
#define __WMI_API_H


#define WMI_DEF_MSF_LEN 512
/* WMI command helper function */


struct atd_host_dev;

typedef void * wmi_handle_t;


typedef adf_nbuf_queue_t    wmi_eventq_t;

typedef void (*wmi_detach_fn_t)(void *);

typedef void (*wmi_event_cb)(struct atd_host_dev *dev_sc, wmi_id_t  event,
                             adf_nbuf_t     buf);

typedef struct wmi {
    void                   *devt;
    htc_handle_t            htc_handle;
    adf_os_spinlock_t       wmi_lock;
    a_uint32_t              wmi_flags;

    adf_os_sem_t            wmi_cmd_sem;
    adf_os_sem_t            wmi_op_sem;
    adf_os_spinlock_t       wmi_op_lock;

    a_uint8_t               wmi_stop_flag;
    a_uint32_t              wmi_in_progress;

    htc_endpointid_t        wmi_endpoint_id;

    a_uint8_t              *cmd_rsp_buf;
    a_uint32_t              cmd_rsp_len;
    a_uint8_t              *cmd_rsp_bufinit;
    a_uint32_t              cmd_rsp_leninit;

    a_uint32_t              cmd_rsp_status;
    wmi_id_t                wmi_last_sent_cmd;
    a_uint8_t               tx_seq_id;
    a_uint8_t               tx_frags_seqno;

    wmi_detach_fn_t         detach_fn;
    void                    *detach_arg;
    a_uint32_t              wmi_detach_pending;
    
    wmi_event_cb            event_cb;   /**< Event Callback */
    adf_os_work_t           event_loop; /**< Event Callback loop */
    wmi_eventq_t            eventq;     /**< Event queue head */

#ifdef WMI_RETRY
    a_uint8_t               wmi_data[WMI_DEF_MSG_LEN];
    adf_os_timer_t          wmi_retry_timer_instance;
    adf_nbuf_t              wmi_retryBuf;
    a_uint16_t              wmi_retrycmdLen;
    a_uint16_t              wmi_retrycnt;
    a_uint8_t               last_seq_id;
    a_uint8_t               last_more_flag;
    a_uint8_t               wmi_retry_timer_del_fail;
    a_uint8_t               recvd_seqno;
#endif
} wmi_t;

/** 
 * @brief Initialize the WMI
 * 
 * @param context
 * @param htcinstance
 * @param event_cb      (Event Callback)
 * 
 * @return 
 */
wmi_handle_t 
wmi_init (void * context , htc_handle_t htcinstance, wmi_event_cb  event_cb);

/** 
 * @brief Cleanup the WMI
 * 
 * @param wmi_handle
 */
void 
wmi_cleanup(wmi_handle_t wmi_handle);

/** 
 * @brief Bind the WMI service with HTC endpoint number
 * 
 * @param hHTC
 * @param hWMI
 * @param wmi_epid
 * 
 * @return 
 */
a_status_t  
wmi_connect(htc_handle_t hHTC, void *hWMI, htc_endpointid_t  *wmi_epid);

/** 
 * @brief Allocate a WMI message
 * 
 * @param wmi_handle
 * @param len
 * 
 * @return 
 */
adf_nbuf_t 
wmi_msg_alloc( wmi_handle_t wmi_handle, a_uint32_t len );

/** 
 * @brief Issue a WMI command to the target 
 * 
 * @param wmi_handle_t      (WMI handle)
 * @param cmdId             (WMI Command)
 * @param nbuf              (Actual to data to send)
 * @param respbuf           (Response buffer)
 * @param rspLen            (Length of the response buffer)
 * 
 * @return 
 */
a_status_t 
wmi_cmd_send ( void *wmi_handle_t, wmi_id_t cmdId, 
                          adf_nbuf_t nbuf, a_uint8_t *respbuf,
                          a_uint32_t rspLen);


/** 
 * @brief Stop the WMI and wake all pending WMI commands
 * 
 * @param wmi_handle
 */
void
wmi_stop(void *wmi_handle);


/** 
 * @brief Drain the WMI Event queue & destroy the Event loop
 * 
 * @param wmip
 */
void
wmi_drain_eventq(wmi_t   *wmip);


#endif /* __WMI_API_H */
