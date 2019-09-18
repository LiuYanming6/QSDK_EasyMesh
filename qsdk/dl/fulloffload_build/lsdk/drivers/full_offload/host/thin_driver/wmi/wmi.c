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

#include <adf_os_types.h>
#include <adf_os_pci.h>
#include <adf_os_dma.h>
#include <adf_os_timer.h>
#include <adf_os_lock.h>
#include <adf_os_io.h>
#include <adf_os_mem.h>
#include <adf_os_module.h>
#include <adf_os_util.h>
#include <adf_os_defer.h>
#include <adf_os_atomic.h>
#include <adf_net_types.h>
#include <adf_nbuf.h>
#include <adf_net.h>


#include <hif_api.h>
#include <htc.h>
#include <htc_api.h>

#include <wmi.h>
#include <wmi_api.h>
#include <atd_internal.h>

// Remove this debug after good amount of test
#define WMI_RETRY_DBG

/** 
 * @brief 
 * 
 * @param wmi_handle
 */
void 
wmi_stop(void *wmi_handle);

/** 
 * @brief 
 * 
 * @param ctx
 * @param nbuf
 * @param htc_endpointid_t
 */
static void 
wmi_ctrlep_txcomp( void *ctx, adf_nbuf_t nbuf, htc_endpointid_t );

/** 
 * @brief 
 * 
 * @param wmip
 * @param netbuf
 * 
 * @return 
 */
static a_status_t 
wmi_control_rx( wmi_t *wmip, adf_nbuf_t netbuf);

/** 
 * @brief 
 * 
 * @param ctx
 * @param netbuf
 * @param Ep
 */
static void 
wmi_ctrlep_recv( void *ctx, adf_nbuf_t netbuf, htc_endpointid_t Ep);

/** 
 * @brief 
 * 
 * @param ctx
 * @param netbuf
 */
static void 
wmi_rsp_callback(void *ctx, adf_nbuf_t netbuf );

/** 
 * @brief 
 * 
 * @param wmip
 * @param netbuf
 * @param cmdId
 * @param retry
 * 
 * @return 
 */
static a_status_t 
wmi_cmd_issue(wmi_t *wmip, adf_nbuf_t netbuf, wmi_id_t cmdId, 
              a_uint16_t retry );

/** 
 * @brief Enqueue a event in the Device event queue
 * 
 * @param dev_sc
 * @param id
 * @param 
 * 
 * @return 
 */
a_status_t
wmi_enqueue_event(wmi_t     *wmip, wmi_id_t  id, adf_nbuf_t     buf);

/** 
 * @brief 
 * 
 * @param arg
 */
void
wmi_event_run(void  *arg);


#ifdef WMI_RETRY

/** 
 * @brief 
 * 
 * @param wmi_handle
 */
void wmi_retry_timer( void * wmi_handle);
#endif




/* Initialize the WMI module */
wmi_handle_t  
wmi_init( void * devinstance, htc_handle_t htcinstance, wmi_event_cb  event_cb)
{
    wmi_t *wmip;

    wmip = adf_os_mem_alloc(NULL, sizeof(wmi_t));
    if (!wmip) 
        return NULL;
    
    wmip->devt          = devinstance;
    wmip->htc_handle    = htcinstance ;
    wmip->event_cb      = event_cb;

    /* WMI Command specific semaphores */
    adf_os_sem_init(&wmip->wmi_cmd_sem);
    adf_os_sem_init(&wmip->wmi_op_sem);
    adf_os_sem_acquire(NULL, &wmip->wmi_cmd_sem);

    wmip->wmi_stop_flag = 1;
    wmip->wmi_in_progress = 0;
    wmip->tx_seq_id = 1;

    adf_nbuf_queue_init(&wmip->eventq);    
    adf_os_create_work(NULL, &wmip->event_loop, wmi_event_run, wmip);

#ifdef WMI_RETRY          
    adf_os_timer_init( NULL, &wmip->wmi_retry_timer_instance, 
                       wmi_retry_timer,wmip);
    wmip->last_more_flag = 0;
    wmip->wmi_retry_timer_del_fail = 0;
#endif
    return (wmip); 
}

/* De-init the WMI module */
void 
wmi_cleanup(wmi_handle_t wmi_handle)
{
    wmi_t *wmip = (wmi_t *)wmi_handle;
    
    adf_os_destroy_work(NULL, &wmip->event_loop);
    
    adf_os_mem_free(wmip);
}

/* Bind endpoint */
a_status_t 
wmi_connect(htc_handle_t htc_handle, void *hWMI, htc_endpointid_t *wmi_epid)
{
    a_status_t                  status  = A_STATUS_OK;
    wmi_t                       *wmip   = (wmi_t *)hWMI;
    htc_connect_service_req_t   creq    = {{0}};
    htc_connect_service_resp_t  cresp   = {0};

    /* store the HTC handle */
    wmip->htc_handle = htc_handle;

    /* It is the 'ctx' parameter when EP callbacks are invoked. */
    creq.cb.ep_ctx      = wmip;
    creq.cb.ep_txcomp   = wmi_ctrlep_txcomp;
    creq.cb.ep_rx       = wmi_ctrlep_recv;
    creq.svcid          = WMI_CONTROL_SVC;

    status = htc_connect_service(htc_handle, &creq, &cresp);
    if (status != A_STATUS_OK){
        adf_os_print(" Failed to connect to wmi \n");
        return status; 
    }

    wmip->wmi_endpoint_id   = cresp.epid;
    *wmi_epid               = wmip->wmi_endpoint_id;

    return status;
}

/* Allocate the WMI Message */
adf_nbuf_t 
wmi_msg_alloc (wmi_handle_t wmi_handle, a_uint32_t length)
{

    wmi_t *wmip         = (wmi_t *)wmi_handle;
    adf_nbuf_t netbuf   = NULL;
    a_uint16_t headroom = htc_get_reserveheadroom(wmip->htc_handle) 
                          + sizeof(wmi_hdr_t);

    /* allocate memory for command */
    netbuf = adf_nbuf_alloc(NULL, headroom + length, headroom, 0, 0);

    return netbuf;
}

/* Send a WMI command */
a_status_t
wmi_cmd_send( void         *wmi_handle, 
              wmi_id_t      cmdId, 
              adf_nbuf_t    netbuf,
              a_uint8_t    *respbuf, 
              a_uint32_t    rspLen)
{

    wmi_t *wmip = (wmi_t *)wmi_handle;
    a_status_t status = A_STATUS_OK;

#ifdef WMI_RETRY
    a_uint8_t *pData;
    a_uint32_t length;
#endif

    if (netbuf == ADF_NBUF_NULL) {
        adf_os_print("nbuf allocation for WMI echo cmd failed!");
        status = A_STATUS_ENOMEM;
        return status;
    }
    if (wmip == NULL) {
        adf_os_print("WMI handle is NULL!");
        status = A_STATUS_ENOMEM;
        return status;
    }

    /* Acquire the semaphore. Mutex is not used as it disables 
       the kernel premption and this would cause issues on premptive kernels.
     */
    adf_os_sem_acquire(NULL, &wmip->wmi_op_sem);

    adf_os_assert(wmip->wmi_in_progress == 0);

    wmip->wmi_in_progress=1;

    if (wmip->wmi_endpoint_id == 0) {
        adf_os_print("WMI endpoint ID is invalid!");
        status = A_STATUS_FAILED;
        goto _err_tx_out2;
    }

    /* check if wmi_stop_flag is set */
    if (wmip->wmi_stop_flag == 0) {
        status = A_STATUS_FAILED;
        goto _err_tx_out2;
    }

    /* record the rsp buffer and length */
    wmip->cmd_rsp_buf = respbuf;
    wmip->cmd_rsp_len = rspLen;

#ifdef WMI_RETRY
    adf_nbuf_peek_header(netbuf, &pData, &length);
    if(length > WMI_DEF_MSG_LEN){
        adf_os_print(" Message len %d > WMI_DEF_MSG_LEN\n", length);
        status = A_STATUS_FAILED;
        goto _err_tx_out2;
    }
    adf_os_mem_copy(wmip->wmi_data, pData, length);
    wmip->wmi_retrycmdLen = length;


    /*Storing the response buffer*/
    /* record the rsp buffer and length */
    wmip->cmd_rsp_bufinit = respbuf;
    wmip->cmd_rsp_leninit = rspLen;
    wmip->tx_frags_seqno=0;
    /*seq id taken care instid */
    adf_os_timer_start(&wmip->wmi_retry_timer_instance, 5000);

#endif

    status = wmi_cmd_issue(wmip, netbuf, cmdId, 0);

    if (status == A_STATUS_FAILED)  goto _err_tx_out2;

    adf_os_sem_acquire(NULL, &wmip->wmi_cmd_sem);

    wmip->wmi_in_progress=0;
    if(wmip->wmi_detach_pending) {
        adf_os_print(" Scheduling the detatch function \n");
        wmip->wmi_detach_pending = 0;
        wmip->detach_fn((void *)wmip->detach_arg);
    }

    adf_os_sem_release(NULL, &wmip->wmi_op_sem);
    status |= wmip->cmd_rsp_status;
    return status;

_err_tx_out2:

    adf_nbuf_free(netbuf);
    wmip->wmi_in_progress=0; 
    adf_os_sem_release(NULL, &wmip->wmi_op_sem); 

    return status;
}

/** 
 * @brief  Issue a WMI Command
 */
static a_status_t 
wmi_cmd_issue( wmi_t *wmip, adf_nbuf_t netbuf, wmi_id_t cmdId,
               a_uint16_t retry )
{
    a_status_t status = A_STATUS_OK;
    wmi_hdr_t *wmi_cmd_hdr;

    wmip->wmi_last_sent_cmd = cmdId;
    /* wmi header */
    wmi_cmd_hdr =(wmi_hdr_t *)adf_nbuf_push_head(netbuf, sizeof(wmi_hdr_t));
    wmi_cmd_hdr->id = adf_os_htons(cmdId);
#ifdef WMI_RETRY
    if(retry)
        wmi_cmd_hdr->seqno = wmip->tx_seq_id;
    else
#endif
    wmi_cmd_hdr->seqno = wmip->tx_seq_id;
    wmi_cmd_hdr->flag = 0;
    wmip->tx_frags_seqno=0;

    /*adf_os_print("%s: htc_send cmid/seqid/flag %d/%d/%d\n",
                  __func__,wmip->wmi_last_sent_cmd,wmi_cmd_hdr->seqno,wmi_cmd_hdr->flag); */
    /* invoke HTC API to send the WMI command packet */
    status = htc_send(wmip->htc_handle, netbuf, wmip->wmi_endpoint_id);
    return status;
}

static void 
wmi_ctrlep_txcomp(void *ctx, adf_nbuf_t netbuf, htc_endpointid_t ep)
{
    adf_nbuf_free(netbuf);
}


static void 
wmi_ctrlep_recv(void *ctx, adf_nbuf_t netbuf, htc_endpointid_t Ep) 
{
    a_status_t status = A_STATUS_OK;
    wmi_t *wmip = (wmi_t *)ctx;

    /* if wmi_stop_flag is set, don't handle this late 
       wmi event or wmi cmd response */
    if (wmip->wmi_stop_flag == 0) {
        adf_nbuf_free(netbuf);
        return;
    }
    status = wmi_control_rx(wmip, netbuf);
    if (status == A_STATUS_FAILED){
        adf_nbuf_free(netbuf);
    }
}

/** 
 * @brief Receive WMI packet on Control endpoint
 */
static a_status_t 
wmi_control_rx(wmi_t *wmip, adf_nbuf_t netbuf)
{
    a_status_t  status = A_STATUS_OK;
    a_uint16_t  cmd_evt_id ;
    wmi_hdr_t  *wmih;
    a_uint8_t  *netdata;
    a_uint32_t  netlen;

    adf_nbuf_peek_header(netbuf, &netdata, &netlen);

    wmih        = (wmi_hdr_t *)netdata;
    cmd_evt_id  = adf_os_ntohs(wmih->id);

    switch (cmd_evt_id & WMI_EVENT_START) {
        case WMI_EVENT_START:
            wmi_enqueue_event(wmip, cmd_evt_id, netbuf);
            break;

        default:
            /* 
             * WMI command response 
             */
            wmi_rsp_callback(wmip, netbuf);
            adf_nbuf_free(netbuf);
            break;
    }
    return status;
}

/** 
 * @brief Enqueue an WMI Event in the eventq
 */
a_status_t
wmi_enqueue_event(wmi_t  *wmip, wmi_id_t  id, adf_nbuf_t     buf)
{
    atd_host_dev_t  *dev_sc = wmip->devt;

    /* Protect the Event queue */
    atd_spin_lock(dev_sc);   /* Lock */

    adf_nbuf_queue_add(&wmip->eventq, buf);

    atd_spin_unlock(dev_sc); /* Unlock */
   
    /* Increment the ref count for safe schedule: take 1 for each event */
    atd_inc_ref(dev_sc);

    /* Schedule the Event loop */
    adf_os_sched_work(NULL, &wmip->event_loop);
    
    return A_STATUS_OK;
}

void
wmi_drain_eventq(wmi_t     *wmip)
{
    atd_host_dev_t     *dev_sc = wmip->devt;
    adf_nbuf_t          buf;

    /* Protect the EventQ  */
    atd_spin_lock_bh(dev_sc);
 
    for (;;) {
        buf = adf_nbuf_queue_remove(&wmip->eventq);

        if (!buf) break;

        adf_nbuf_free(buf);

        /* free 1 ref for each event processed */
        atd_dec_ref(dev_sc);
    }

    atd_spin_unlock_bh(dev_sc);
}

/** 
 * @brief Main WMI Event Loop
 */
void
wmi_event_run(void  *arg)
{
    wmi_t          *wmip = (wmi_t *)arg;
    atd_host_dev_t   *dev_sc = wmip->devt;
    adf_nbuf_t      buf;
    wmi_hdr_t      *wmih = 0;
    a_uint32_t      len = 0;

    /* Protect the Event thread from detach */
    atd_mutex_lock(dev_sc);

    do {
        /* Protect the EventQ  */
        atd_spin_lock_bh(dev_sc);

        buf = adf_nbuf_queue_remove(&wmip->eventq);

        atd_spin_unlock_bh(dev_sc);
        
        if(!buf) break;
        
        adf_nbuf_peek_header(buf, (a_uint8_t **)&wmih, &len);
        adf_nbuf_pull_head(buf, sizeof(wmi_hdr_t));
    
        /* Note: Events can be Mux'ed here based on id*/
        wmip->event_cb(dev_sc, adf_os_ntohs(wmih->id), buf);

        /* Wake people sleeping on the ref count  */
        /* free 1 ref for each event processed */
        atd_dec_ref(dev_sc);

    }while(1);

    atd_mutex_unlock(dev_sc);
}

/** 
 * @brief WMI response Callback function
 */
static void
wmi_rsp_callback(void *ctx, adf_nbuf_t netbuf)
{
    wmi_t *wmip = (wmi_t *)ctx;
    a_uint8_t       *netdata;
    a_uint32_t       netlen;
    wmi_hdr_t       *wmih;
    wmi_rsp_hdr_t   *rsph;
    a_uint16_t       rspid;
    a_uint8_t        seqno;

    adf_nbuf_peek_header(netbuf, &netdata, &netlen);

    wmih        =  (wmi_hdr_t *)netdata;
    rspid       =  adf_os_ntohs(wmih->id);
    seqno       =  wmih->seqno;

    rsph        =  (wmi_rsp_hdr_t *)(wmih + 1);

    wmip->cmd_rsp_status = ntohl(rsph->status);
 
    adf_nbuf_pull_head(netbuf, sizeof(wmi_hdr_t) + sizeof(wmi_rsp_hdr_t));
    
    adf_nbuf_peek_header(netbuf, &netdata, &netlen);




#ifndef WMI_RETRY
  if ( rspid != wmip->wmi_last_sent_cmd  || seqno != wmip->tx_seq_id ) 
    {
        int i ;
        adf_os_print("Last sent WMI command is %d but response cmd id is %d\n",
                wmip->wmi_last_sent_cmd, rspid);
        //adf_os_assert(0);
        adf_os_print("%s: last cmid/seqid/ %d/%d rx cmdid/seqid/flag %d/%d/%d  length %d\n",
                __func__,wmip->wmi_last_sent_cmd, wmip->tx_seq_id,
                rspid, seqno, more_flag, netlen);

        adf_os_print("Dump Data ... \n");
        /*14+4+8+8*/

        for(i=0;i< netlen + 36 ; i++){
            if(i % 16 == 0) printk("\n");
            printk(" %2x ",netdata[i-36]);
        }
        printk("Dump done \n");
//        return;

    }
  if(wmip->lastseqid == seqno){
      adf_os_print("Already recieved the response once  \n");
      return;

    }

#endif

    //adf_os_print(" wmip->tx_seq_id %d  seqno %d  %d %d flag %d \n", wmip->tx_seq_id ,seqno,wmip->tx_frags_seqno,wmih->fragseq,wmih->flag);
    if(wmip->tx_seq_id == seqno){
#ifndef WMI_RETRY
        wmip->last_seq_id =seqno;
#endif
        /*Expected Seq */
        if(wmip->tx_frags_seqno == wmih->fragseq ){

            /*Expected frags ID*/

            if (wmip->cmd_rsp_buf != NULL && wmip->cmd_rsp_len > 0) {
                adf_os_mem_copy(wmip->cmd_rsp_buf, netdata, netlen);
                wmip->cmd_rsp_buf += netlen;
                wmip->cmd_rsp_len -= netlen;
            }

            /*Next expected frag ID*/
            wmip->tx_frags_seqno++;

            if(wmih->flag ==0){

#ifdef WMI_RETRY
                if (adf_os_timer_cancel(&wmip->wmi_retry_timer_instance)) {
                    wmip->recvd_seqno = seqno;
                    wmip->wmi_retrycnt =0;
                    wmip->wmi_retrycmdLen=0;
                    wmip->tx_frags_seqno=0;
                    adf_os_sem_release(NULL, &wmip->wmi_cmd_sem); 
                } else {
#ifdef WMI_RETRY_DBG
                    printk(KERN_ERR "del timer failure!! current seqno %d, last seqno %d\n", seqno, wmip->recvd_seqno);
#endif
                    wmip->wmi_retry_timer_del_fail = seqno;
                }
#else
                wmip->tx_frags_seqno=0;
                adf_os_sem_release(NULL, &wmip->wmi_cmd_sem); 
#endif       
                wmip->tx_seq_id++;
            }
        }else{

            adf_os_print(" Drop tx_seq_id  %d seqno %d tx_frags_seqno %d  fragseq %d \n ",wmip->tx_seq_id,seqno, wmip->tx_frags_seqno,wmih->fragseq );
        }

    }
    else if( (wmip->tx_seq_id -1) == seqno){
        adf_os_print("Duplicate Or retry Resp reject  %d %d \n",wmip->tx_seq_id ,seqno);
        return ;
    }
    else{
        adf_os_print("Unhandled case expexted %d recv %d txseq %d  \n",
                wmip->last_seq_id,seqno,wmip->tx_seq_id);
        return ;
    }
}


/** 
 * @brief Stop the WMI
 * 
 * @param wmi_handle
 */
void
wmi_stop(void *wmi_handle)
{
    wmi_t* wmip = (wmi_t *)wmi_handle;

    wmip->wmi_stop_flag = 0;

    adf_os_sem_release(NULL, &wmip->wmi_cmd_sem);

}

#ifdef WMI_RETRY
void wmi_retry_timer( void * wmi_handle)
{
    wmi_t *wmip = (wmi_t *)wmi_handle;
    wmi_id_t cmdId = wmip->wmi_last_sent_cmd;
    adf_nbuf_t netbuf = NULL;
    a_uint8_t *pData;
    a_status_t status = A_STATUS_OK;

    if (wmip->wmi_retry_timer_del_fail) {
#ifdef WMI_RETRY_DBG
        printk(KERN_ERR "del timer had failed for seqno %d and last seqno %d..\n", wmip->wmi_retry_timer_del_fail, wmip->recvd_seqno);
#endif
        wmip->recvd_seqno = wmip->wmi_retry_timer_del_fail;
        wmip->wmi_retry_timer_del_fail = 0;
        wmip->wmi_retrycnt =0;
        wmip->wmi_retrycmdLen=0;
        wmip->tx_frags_seqno=0;
        adf_os_sem_release(NULL, &wmip->wmi_cmd_sem); 
        return;
    }
 
    netbuf = wmi_msg_alloc(wmip, WMI_DEF_MSG_LEN);

    if (netbuf == ADF_NBUF_NULL) {
        adf_os_timer_start(&wmip->wmi_retry_timer_instance, 5000);
        return;
    }
    pData = (a_uint8_t *)adf_nbuf_put_tail(netbuf,  wmip->wmi_retrycmdLen);

    adf_os_mem_copy(pData, wmip->wmi_data,  wmip->wmi_retrycmdLen);

    adf_os_print("%x wmi retry %d seq %d\n",
            (int)wmip ,wmip->wmi_retrycnt, wmip->tx_seq_id);

    wmip->wmi_retrycnt++;

    adf_os_timer_start(&wmip->wmi_retry_timer_instance, 5000);


    wmip->cmd_rsp_buf = wmip->cmd_rsp_bufinit;
    wmip->cmd_rsp_len = wmip->cmd_rsp_leninit;
    wmip->tx_frags_seqno=0;

    status = wmi_cmd_issue(wmip, netbuf, cmdId,1);
    if (status == A_STATUS_FAILED){
        adf_os_print("wmi cmd issue fail for retyr");

    }
    return;
}
#endif

adf_os_export_symbol( wmi_init);
adf_os_export_symbol( wmi_msg_alloc );
adf_os_export_symbol( wmi_cmd_send );
adf_os_export_symbol( wmi_connect);
adf_os_export_symbol( wmi_cleanup );

