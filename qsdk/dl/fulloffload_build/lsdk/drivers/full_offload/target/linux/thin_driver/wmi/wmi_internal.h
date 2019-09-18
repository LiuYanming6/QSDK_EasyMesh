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
 * @File: 
 * 
 * @Abstract: internal data and structure definitions for WMI service
 * 
 */
#ifndef WMI_INTERNAL_H_
#define WMI_INTERNAL_H_




#if LINUX_VERSION_CODE  <= KERNEL_VERSION(2,6,19)
typedef struct work_struct     wmi_os_work_t;
#else
/**
 * wrapper around the real task func
 */
typedef void (*wmi_os_defer_fn_t)(void *);


typedef struct {    struct work_struct   work;
    wmi_os_defer_fn_t    fn;
    void                 *arg;
} wmi_os_work_t;

void wmi_os_defer_func(struct work_struct *work);
#endif


typedef struct _wmi_recv_arg {
    htc_endpointid_t  epid;
    struct sk_buff * netbuf;
    void *ctx;
}wmi_recv_arg_t ;

typedef struct _wmi_recv_work {
    a_uint8_t           tail;
    a_uint8_t           head;
    a_int16_t           count;
    wmi_recv_arg_t       wmirecvarg[WMI_MAX_RECVQUEUE_LEN];
    spinlock_t          lock;
    wmi_os_work_t       wmiwork;
}wmi_work_t;


typedef struct _wmi_softc {
    htc_handle_t    htc_handle;
    htc_endpointid_t wmi_ep;
    wmi_dispatch_table_t *wmi_disp_table;
    wmi_work_t      wmi_recvwork ;
    a_int32_t       wmi_lastcmdseq;
#ifdef WMI_RETRY    
    wmi_id_t        wmi_retrycmdid;
    a_uint8_t       wmi_tgtseq;
    a_int32_t       wmi_retrylen;
    a_uint8_t       *wmi_retrydata;
    a_status_t      wmi_retrystatus;
#endif 
    a_uint8_t       wmi_stop_flag;

} wmi_softc_t;



/**********WMI worker thread API*****************/

static inline a_status_t
wmi_os_sched_work(wmi_softc_t *sc,wmi_os_work_t *work)
{
#if LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,19)
    schedule_work(work);
#else
    schedule_work(&work->work);
#endif
    return A_STATUS_OK;

}
    static inline a_status_t
wmi_os_init_work(wmi_os_work_t *work,wmi_os_defer_fn_t func, void *arg)
{
#if LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,19)
    INIT_WORK(work, func, arg);
#else
    work->fn = func ;
    work->arg  = arg  ;
    INIT_WORK(&work->work, wmi_os_defer_func);
#endif

    return A_STATUS_OK;
}

#endif /*WMI_INTERNAL_H_*/
