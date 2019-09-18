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

#include <a_base_types.h>
#include <linux/version.h>
#include <linux/if.h>
#include <linux/workqueue.h>
#include <linux/skbuff.h>
#include <linux/netdevice.h>
#include <htc_api.h>
#include <wmi_api.h>
#include "wmi_internal.h"
#include <atd_trc.h>
#define adf_os_print printk
#define adf_os_function __func__


static void wmi_recv_worker( void * arg);
static void wmi_send_comp(void *ctx ,struct sk_buff * netbuf, 
                          htc_endpointid_t);
static void wmi_recv(void *ctx, struct sk_buff * netbuf, 
                      htc_endpointid_t epid);
static void wmi_recv_worker( void * arg);
static void _wmi_recv(void * ctx  ,htc_endpointid_t epid , 
                      struct sk_buff * netbuf );

struct sk_buff * 
wmi_alloc_rsp(wmi_handle_t handle, a_uint32_t len);


wmi_handle_t 
wmi_init (void * htcctx)
{
    wmi_softc_t *sc = NULL;
    htc_epcallback_t cb;
    
    atd_trace(ATD_DEBUG_FUNCTRACE,(" Start ."));

    sc = (wmi_softc_t *)kzalloc(sizeof(wmi_softc_t),GFP_KERNEL);
    if (sc == NULL) {
        return NULL;    
    }

    sc->htc_handle = (htc_handle_t *)htcctx;    
    sc->wmi_stop_flag = 0;    
#ifdef WMI_RETRY
    sc->wmi_tgtseq =1;
    sc->wmi_retrydata = NULL;
    sc->wmi_retrycmdid = 0;
    sc->wmi_retrylen   = 0;
#endif    
       
     
    /* register the WMI control service */
    cb.ep_rx       = wmi_recv ;
    cb.ep_txcomp   = wmi_send_comp ;
    cb.ep_ctx      = (void *) sc;
    sc->wmi_ep= htc_register_service(sc->htc_handle,&cb,WMI_CONTROL_SVC);

    wmi_os_init_work(&sc->wmi_recvwork.wmiwork, wmi_recv_worker, (void *)sc);
    spin_lock_init(&sc->wmi_recvwork.lock);

    atd_trace(ATD_DEBUG_FUNCTRACE,(" End ."));
    return sc;
}
void 
wmi_set_stopflag(wmi_handle_t handle, a_uint8_t val)
{
    wmi_softc_t *sc = (wmi_softc_t *)handle;
    
    sc->wmi_stop_flag = val;
}
void 
wmi_register_disp_table(wmi_handle_t handle, wmi_dispatch_table_t *table)
{
    wmi_softc_t *sc = (wmi_softc_t *)handle;

    atd_trace(ATD_DEBUG_FUNCTRACE,(" Start ."));

    sc->wmi_disp_table =  table;


    atd_trace(ATD_DEBUG_FUNCTRACE,(" End ."));
}
void 
wmi_send_event(wmi_handle_t handle, struct sk_buff * netbuf, wmi_id_t  eventid)
{
    wmi_softc_t *sc = (wmi_softc_t *)handle;
	wmi_hdr_t *evnthdr;
    a_status_t status;
        

    atd_trace(ATD_DEBUG_FUNCTRACE,(" Start ."));

    if (sc->wmi_stop_flag) {
        status = A_STATUS_FAILED;
        goto out;
    }

    evnthdr = (wmi_hdr_t *)skb_push(netbuf, sizeof(wmi_hdr_t));

    evnthdr->id        = htons(eventid) ;
    evnthdr->seqno     = 0 ;
    evnthdr->flag      = 0 ;
	
    status = htc_send(sc->htc_handle, netbuf, sc->wmi_ep);
out:

    if(status != A_STATUS_OK){
        printk(" wmi_send_event :htc_send failed \n");
        dev_kfree_skb_any(netbuf);
    }


    atd_trace(ATD_DEBUG_FUNCTRACE,(" End ."));

}

void wmi_cleanup(wmi_handle_t handle)
{
    wmi_softc_t *sc = (wmi_softc_t *)handle;

    atd_trace(ATD_DEBUG_FUNCTRACE,(" Start ."));

#ifdef WMI_RETRY
    if (sc->wmi_retrydata != NULL)
    {
        kfree(sc->wmi_retrydata);
        sc->wmi_retrydata = NULL;
    }
#endif
    
    kfree(sc);

    atd_trace(ATD_DEBUG_FUNCTRACE,(" End ."));

}


struct sk_buff * 
wmi_alloc_rsp(wmi_handle_t handle, a_uint32_t len)
{     
    wmi_softc_t *sc = (wmi_softc_t *)handle;
    struct sk_buff *netbuf = NULL ;
    a_uint32_t headroom =  sizeof(wmi_hdr_t) + sizeof(wmi_rsp_hdr_t)
                           +htc_get_reserveheadroom(sc->htc_handle);


    atd_trace(ATD_DEBUG_FUNCTRACE,(" Start ."));

    len +=headroom; 
    netbuf = dev_alloc_skb(len);

    if(netbuf == NULL ){
        printk("%s : netbuf alloc fale \n",__func__);
        return NULL ;
    }

    skb_reserve(netbuf, headroom);

    atd_trace(ATD_DEBUG_FUNCTRACE,(" End ."));

    return netbuf;
}


struct sk_buff * 
wmi_alloc_event(wmi_handle_t handle, a_uint32_t len)
{     
    wmi_softc_t *sc = (wmi_softc_t *)handle;
    struct sk_buff *netbuf = NULL ;
    a_uint32_t headroom =  sizeof(wmi_hdr_t) + 
                           htc_get_reserveheadroom(sc->htc_handle);


    atd_trace(ATD_DEBUG_FUNCTRACE,(" Start ."));

    len +=headroom; 
    netbuf = dev_alloc_skb(len);

    if(netbuf == NULL ){
        printk("%s : netbuf alloc fale \n",__func__);
        return NULL ;
    }

    skb_reserve(netbuf, headroom);

    atd_trace(ATD_DEBUG_FUNCTRACE,(" End ."));

    return netbuf;
}
static void wmi_cmd_rsp_send(wmi_softc_t *sc, wmi_id_t cmd_id, a_uint8_t seqno, 
                             a_status_t status, a_uint8_t *buffer, a_int32_t rsplen)
{
    wmi_hdr_t      *wmih;
    wmi_rsp_hdr_t  *rsph; 
    a_uint8_t        evtflag;
    a_uint32_t       length;
    struct sk_buff  *netbuf ;
    a_status_t stat;
    a_uint8_t       *data;
    a_uint8_t        fragseq=0;
    
    atd_trace(ATD_DEBUG_FUNCTRACE,(" Start ."));

    do {
        if((rsplen )  > WMI_MAX_EVENT_BUFFER_SIZE )
        {
            length  = WMI_MAX_EVENT_BUFFER_SIZE;
            evtflag = WMIRSP_FLAG_MORE;
        }
        else
        {
            length  =  rsplen;
            evtflag = 0;
       }

        netbuf = wmi_alloc_rsp(sc, length);

        if (netbuf == NULL) {
            printk("%s: buffer allocation for event_id %x failed!\n", 
                    __FUNCTION__, cmd_id);
            dump_stack();                                     
            panic("Take care of the assert first\n"); 
            return;
        }
        
        if (rsplen != 0 && buffer != NULL) {
            data = (a_uint8_t *)skb_put(netbuf,length);
            memcpy(data, buffer, length);
        } 
        
        rsplen -= length;
        buffer += length;

        wmih = (wmi_hdr_t *)skb_push(netbuf, sizeof(wmi_hdr_t) +
                sizeof(wmi_rsp_hdr_t));

        wmih->id        = htons(cmd_id);
        wmih->seqno     = seqno;
        wmih->flag      = evtflag ;
        wmih->fragseq   = fragseq++;
        rsph            = (wmi_rsp_hdr_t *)(wmih + 1);
        rsph->status    = htonl(status);

        stat = htc_send(sc->htc_handle, netbuf, sc->wmi_ep);

        if(stat != A_STATUS_OK){
            printk("%s : htc_send failed \n", __func__);
            dev_kfree_skb_any(netbuf);
        }

        atd_trace(ATD_DEBUG_LEVEL0, ("%s: htc_send cmdid/seqid/flag %d/%d/%d  length %d\n", \
               __func__, cmd_id, seqno, evtflag, length));
        atd_trace(ATD_DEBUG_LEVEL0,("cmdid %d  seqno %d flag %d  length %d ", \
                                 cmd_id, seqno, evtflag, length));

    }while(rsplen > 0 );
       
    atd_trace(ATD_DEBUG_FUNCTRACE,(" End ."));
}

#ifdef WMI_RETRY
void 
wmi_replay_rsp(wmi_softc_t *sc)
{
    a_uint32_t rsplen;
    a_uint8_t *buffer;

    atd_trace(ATD_DEBUG_FUNCTRACE,(" Start ."));

    rsplen = sc->wmi_retrylen;
    buffer = sc->wmi_retrydata; 

    wmi_cmd_rsp_send(sc, sc->wmi_retrycmdid, (sc->wmi_tgtseq-1),
                     sc->wmi_retrystatus, buffer, rsplen);
                    
    atd_trace(ATD_DEBUG_FUNCTRACE,(" End ."));

}
#endif


void 
wmi_cmd_rsp(wmi_handle_t wmi_handle, wmi_id_t cmd_id, a_status_t status, 
        a_uint8_t *buffer, a_int32_t rsplen)
{
    wmi_softc_t  * sc = (wmi_softc_t *) wmi_handle;
    
    atd_trace(ATD_DEBUG_FUNCTRACE,(" Start ."));

#ifdef WMI_RETRY
    sc->wmi_retrydata = kzalloc(rsplen, GFP_KERNEL);
    if (sc->wmi_retrydata == NULL)
    {
        printk("%s: Failed to allocate retry buffer!!\n", __func__);
        dump_stack();                                     
        panic("Take care of the assert first\n"); 
    }
    else
    { 
        memcpy(sc->wmi_retrydata, buffer, rsplen);
    }
    sc->wmi_retrycmdid = cmd_id;
    sc->wmi_retrylen   = rsplen;
    sc->wmi_retrystatus = status;
#endif 

    wmi_cmd_rsp_send(sc, cmd_id, sc->wmi_lastcmdseq,
                     status, buffer, rsplen);

    atd_trace(ATD_DEBUG_FUNCTRACE,(" End ."));
}

void 
wmi_send_comp(void *ctx ,struct sk_buff * netbuf, htc_endpointid_t id)
{

    atd_trace(ATD_DEBUG_FUNCTRACE,(" Start ."));

    dev_kfree_skb_any(netbuf);


    atd_trace(ATD_DEBUG_FUNCTRACE,(" End ."));
}



static void 
wmi_recv(void *ctx, struct sk_buff * netbuf, htc_endpointid_t epid)
{
    wmi_softc_t *sc = (wmi_softc_t *)ctx;
    unsigned long  flags;
    a_uint8_t tail;

    atd_trace(ATD_DEBUG_FUNCTRACE,(" Start ."));

    spin_lock_irqsave(&sc->wmi_recvwork.lock, flags);

    if (sc->wmi_recvwork.count >= WMI_MAX_RECVQUEUE_LEN) {
        printk("%s: event queue is full\n", __func__);
        spin_unlock_irqrestore(&sc->wmi_recvwork.lock, flags);
        return ;
    }

    tail = sc->wmi_recvwork.tail;
    sc->wmi_recvwork.wmirecvarg[tail].epid   = epid;
    sc->wmi_recvwork.wmirecvarg[tail].netbuf = netbuf;
    sc->wmi_recvwork.wmirecvarg[tail].ctx    = ctx;


    sc->wmi_recvwork.tail = (tail + 1) & (WMI_MAX_RECVQUEUE_LEN-1);
    sc->wmi_recvwork.count++;
    spin_unlock_irqrestore(&sc->wmi_recvwork.lock, flags);

    atd_trace(ATD_DEBUG_LEVEL0,(" tail  %d head %d \n ", \
                sc->wmi_recvwork.tail,sc->wmi_recvwork.head));

    wmi_os_sched_work(sc, &sc->wmi_recvwork.wmiwork);


    atd_trace(ATD_DEBUG_FUNCTRACE,(" End ."));
}

static void 
wmi_recv_worker( void * arg)    
{
    wmi_softc_t *sc = (wmi_softc_t *)arg;
    wmi_recv_arg_t   wmirecvarg;
    unsigned long flags ;
    a_uint8_t head;

    atd_trace(ATD_DEBUG_FUNCTRACE,(" Start ."));

    spin_lock_irqsave(&sc->wmi_recvwork.lock, flags);

    if(sc->wmi_recvwork.count < 1){
        printk("Schedule but no instance \n");
        spin_unlock_irqrestore(&sc->wmi_recvwork.lock, flags);
        return;
    }

    head = sc->wmi_recvwork.head;
    sc->wmi_recvwork.head = (head + 1) & (WMI_MAX_RECVQUEUE_LEN-1);
    sc->wmi_recvwork.count--;
    memcpy(&wmirecvarg,&sc->wmi_recvwork.wmirecvarg[head],
            sizeof (wmi_recv_arg_t));

    spin_unlock_irqrestore(&sc->wmi_recvwork.lock, flags);

    atd_trace(ATD_DEBUG_LEVEL0,(" tail  %d head %d \n ", \
                sc->wmi_recvwork.tail,sc->wmi_recvwork.head));

 
     _wmi_recv(wmirecvarg.ctx, wmirecvarg.epid, wmirecvarg.netbuf);


    atd_trace(ATD_DEBUG_FUNCTRACE,(" end ."));


}

static void 
_wmi_recv( void * ctx  ,htc_endpointid_t epid , struct sk_buff * netbuf)
{
    wmi_softc_t *sc = (wmi_softc_t *)ctx ;
    wmi_hdr_t   *cmdhdr;
    a_uint8_t      *cmdbuffer; 
    a_uint16_t      cmd;
    a_uint32_t      len;
    wmi_dispatch_entry_t *wmi_entry;

    atd_trace(ATD_DEBUG_FUNCTRACE,(" Start ."));
            
    if (netbuf->len < sizeof(wmi_hdr_t)) {
        printk("Invalid length \n");
        goto out;
    }
    cmdhdr = (wmi_hdr_t *) netbuf->data;
    len    =  netbuf->len;

    cmd = ntohs(cmdhdr->id);
    sc->wmi_lastcmdseq = cmdhdr->seqno;

#ifdef WMI_RETRY        
    if(sc->wmi_lastcmdseq == sc->wmi_tgtseq)
    {
        sc->wmi_tgtseq++;
        if (sc->wmi_retrydata != NULL)
        {
            kfree(sc->wmi_retrydata);
            sc->wmi_retrydata = NULL;
        }
        sc->wmi_retrycmdid = 0;
        sc->wmi_retrylen   = 0;
    }
    else if(sc->wmi_lastcmdseq == (sc->wmi_tgtseq -1 )){
        printk("Target Has Recived but host did not recv the resp REXEC   \
                %d %d   \n",sc->wmi_lastcmdseq,sc->wmi_tgtseq);
        wmi_replay_rsp(sc);
        goto out;
    }else
        printk("Unhandled seq %d  WMI->wmi_tgtseq %d \n",
                sc->wmi_lastcmdseq, sc->wmi_tgtseq);
#endif

    cmdbuffer =  netbuf->data + sizeof(wmi_hdr_t);
    len = len - sizeof (wmi_hdr_t);

    wmi_entry  = sc->wmi_disp_table->disp_entry;
    wmi_entry  = wmi_entry + cmd ;
    
    
    wmi_entry->cmd_handler(sc->wmi_disp_table->ctx, cmd, cmdbuffer,len);
out:

    dev_kfree_skb_any(netbuf);


    atd_trace(ATD_DEBUG_FUNCTRACE,(" End ."));
}

void 
wmi_os_defer_func(struct work_struct *work)
{
#if LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,19)
    return;
#else
    wmi_os_work_t  *ctx = container_of(work, wmi_os_work_t, work);
    ctx->fn(ctx->arg);
#endif
}



