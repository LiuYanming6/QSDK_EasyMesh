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


#include <htc.h>
#include <hif_api.h>
#include <htc_api.h>
#include <htc_internal.h>
#include <atd_trc.h>

/* internal function */
static a_uint8_t htc_map_ep2ulpipe(htc_endpointid_t epid );
static a_uint8_t htc_map_ep2dlpipe(htc_endpointid_t epid );
static void htc_control_msg_recv(void *ctx , adf_nbuf_t netbuf, 
                                 htc_endpointid_t epid);
static hif_status_t htc_send_comp(void *ctx, adf_nbuf_t netbuf);
static hif_status_t htc_recv(void *ctx, adf_nbuf_t netbuf, a_uint8_t pipeid);
static a_status_t  htc_issue_send(htc_softc_t *sc, adf_nbuf_t netbuf, 
                                  a_uint8_t flag, a_uint16_t len, 
                                  a_uint8_t epid);
static a_status_t htc_process_tgt_ready(htc_softc_t * sc );





htc_handle_t 
htc_create(htc_config_t *htc_conf)
{
    a_status_t    status = A_STATUS_OK;
    htc_softc_t       *sc = NULL ;
    htc_endpoint_t   *ep ;
    hif_callback_t    hifcb ;


    atd_trace(ATD_DEBUG_FUNCTRACE,("Start "));

    /* allocate HTC instance */

    if ((sc = (htc_softc_t *)adf_os_mem_alloc(NULL, sizeof(htc_softc_t))) == NULL) {
        htc_trc_error("Unable To allocate HTC Instance \n");
        status = A_STATUS_FAILED;
        goto out;
    }
    adf_os_mem_zero(sc, sizeof(htc_softc_t));



    sc->hif_handle  = htc_conf->hif_handle;
    sc->host_handle = htc_conf->ctx;
    sc->htc_ready   = htc_conf->htc_ready;
    

    ep = &sc->ep_list[HTC_CONTROL_EPID];
    ep->ul_pipeid = htc_map_ep2ulpipe(HTC_CONTROL_EPID);
    ep->dl_pipeid = htc_map_ep2dlpipe(HTC_CONTROL_EPID);

    ep->ep_cb.ep_rx  = htc_control_msg_recv;
    ep->ep_cb.ep_ctx = (void *)sc;



    hifcb.context = sc;
    hifcb.send_buf_done  = htc_send_comp;
    hifcb.recv_buf  = htc_recv;

    hif_register(sc->hif_handle,&hifcb);

    hif_start(sc->hif_handle);
out :
    return sc;

}

void 
htc_start(htc_handle_t htc_handle){

//    htc_softc_t *sc = (htc_softc_t *)htc_handle;

    /*Does not do anything */

    atd_trace(ATD_DEBUG_FUNCTRACE,("Start "));
    atd_trace(ATD_DEBUG_FUNCTRACE,("end "));
    atd_trace(ATD_DEBUG_FUNCTRACE,("htc_start \n"));
}

void *
htc_get_hifhandle(htc_handle_t htc_handle)
{
    htc_softc_t *sc = (htc_softc_t *)htc_handle;

    atd_trace(ATD_DEBUG_FUNCTRACE,("Start "));
    atd_trace(ATD_DEBUG_FUNCTRACE,("end "));
    return sc->hif_handle;
}

a_uint32_t 
htc_get_reserveheadroom(htc_handle_t htc_handle)
{
    htc_softc_t *sc = (htc_softc_t *)htc_handle;
    a_uint32_t  headroom = sizeof(htc_frame_hdr_t) + 
                          hif_get_reserveheadroom(sc->hif_handle);
    return headroom;
 
}

a_status_t 
htc_wait_for_target_ready(htc_handle_t htc_handle)
{
    a_status_t    status = A_STATUS_OK;

    atd_trace(ATD_DEBUG_FUNCTRACE,("removed the wait concept \n"));

/* removed wait concept 
    htc_softc_t *sc = (htc_softc_t *)htc_handle;

    hif_start(sc->hif_handle);
    adf_os_print("[%s%d]Startingmutex waiting...\n",adf_os_function, __LINE__);

    adf_os_sem_acquire(&sc->htc_rdy_sem);

    adf_os_print("[%s%d]Finish mutex waiting... \n",adf_os_function, __LINE__);
*/
    return status;

}

a_status_t 
htc_send(htc_handle_t htc_handle, adf_nbuf_t netbuf, 
                      htc_endpointid_t epid)
{
    htc_softc_t *sc = (htc_softc_t *)htc_handle;
    a_status_t status = A_STATUS_OK;
    htc_endpoint_t *ep;
    a_uint32_t pldlen;

    atd_trace(ATD_DEBUG_FUNCTRACE,("Start "));
    ep = &sc->ep_list[epid];


    pldlen = (a_uint16_t) adf_nbuf_len(netbuf)  ;

    status = htc_issue_send(sc ,netbuf, 0, pldlen, epid);

    atd_trace(ATD_DEBUG_FUNCTRACE,("End "));
    return status;
}
void 
htc_cleanup(htc_handle_t htc_handle)
{
    htc_softc_t *sc = (htc_softc_t *)htc_handle;

    /* free our instance */
    adf_os_mem_free(sc);
}


static a_uint8_t 
htc_map_ep2ulpipe(htc_endpointid_t epid )
{   
    a_uint8_t pipeid;
    switch(epid){
        case HTC_CONTROL_EPID :
            pipeid = hif_get_ulpipe(HIF_CONTROL_PIPE);
            break;

        case HTC_WMICMD_EPID :
            pipeid = hif_get_ulpipe(HIF_COMMAND_PIPE);
            break;

        case HTC_DATA_EPID :
            pipeid = hif_get_ulpipe(HIF_DATA_LP_PIPE);
            break;

        case HTC_WSUPP_EPID :
            pipeid = hif_get_ulpipe(HIF_WSUPP_PIPE);
            break;

        default:
            printk("Un Suported epid");
            pipeid =-1;
 
    }
    return pipeid;
}

static a_uint8_t 
htc_map_ep2dlpipe(htc_endpointid_t epid )
{   
    a_uint8_t pipeid;
    switch(epid){
        case HTC_CONTROL_EPID :
            pipeid = hif_get_dlpipe(HIF_CONTROL_PIPE);
            break;

        case HTC_WMICMD_EPID :
            pipeid = hif_get_dlpipe(HIF_COMMAND_PIPE);
            break;

        case HTC_DATA_EPID :
            pipeid = hif_get_dlpipe(HIF_DATA_LP_PIPE);
            break;

        case HTC_WSUPP_EPID :
            pipeid = hif_get_dlpipe(HIF_WSUPP_PIPE);
            break;

        default:
            printk("Un Suported epid");
            pipeid =-1;
    }
    return pipeid;
}


static htc_endpointid_t 
htc_map_svc2epid(htc_service_t svcid)
{
    htc_endpointid_t epid;

    switch (svcid){
        case WMI_CONTROL_SVC:
             epid = HTC_WMICMD_EPID; 
             break;

        case WMI_DATA_BE_SVC:
             epid = HTC_DATA_EPID;
             break;

        case WMI_WSUPP_SVC:
             epid = HTC_WSUPP_EPID;
             break;

        default :
            epid = 0xFF;
            printk("Undefined service ");
            break;
     } 
    return epid;

}

a_status_t
htc_connect_service (htc_handle_t htc_handle ,htc_connect_service_req_t *creq,
                     htc_connect_service_resp_t  *cresp)
{
    a_status_t status = A_STATUS_OK;   
    htc_softc_t *sc = (htc_softc_t *)htc_handle;
    htc_endpoint_t *ep;
    htc_endpointid_t epid;
    
    atd_trace(ATD_DEBUG_FUNCTRACE,("Start %x ",(unsigned int )sc));

   /*Static mapping instead of doing connect*/ 
    epid = htc_map_svc2epid(creq->svcid); 
   
    atd_trace(ATD_DEBUG_LEVEL0,("Start ep %d svc %d \n",epid,creq->svcid));
    if(epid == 0xFF){
        status = A_STATUS_FAILED;
        goto out;
    }
    ep = &sc->ep_list[epid];
    adf_os_mem_copy(&ep->ep_cb,&creq->cb, sizeof(htc_epcallback_t));
    ep->ul_pipeid = htc_map_ep2ulpipe(epid); 
    ep->dl_pipeid = htc_map_ep2dlpipe(epid);
    cresp->epid = epid;
out:
    return status;

}


static hif_status_t 
htc_recv(void *ctx, adf_nbuf_t netbuf, a_uint8_t pipeid)
{
    htc_softc_t *sc = (htc_softc_t *)ctx;
    a_uint8_t  *netdata;
    a_uint32_t  netlen;
    htc_frame_hdr_t *htchdr;
    htc_endpoint_t *ep;
    a_uint16_t pldlen;
    
    atd_trace(ATD_DEBUG_FUNCTRACE,("Start  sc %x ",(unsigned int )sc));
    adf_nbuf_peek_header(netbuf, &netdata, &netlen);
    netlen = adf_nbuf_len(netbuf);

    htchdr = (htc_frame_hdr_t *)netdata;

    if (htchdr->epid >= ENDPOINT_MAX) {
        htc_trc_error("Wrong EPID recv %d \n",htchdr->epid);
        adf_nbuf_free(netbuf);
        goto out;
    }

    atd_trace(ATD_DEBUG_LEVEL0,(" Header epid %x \n",htchdr->epid));


    ep = &sc->ep_list[htchdr->epid];

    if ((htchdr->flags != 0) && (htchdr->epid != HTC_CONTROL_EPID)) {
        htc_trc_error("Unhandled Data flag %d \n \n",htchdr->flags);
        adf_nbuf_free(netbuf);
        goto out;
    }
   
    adf_nbuf_pull_head(netbuf, sizeof(htc_frame_hdr_t));
    netlen = adf_nbuf_len(netbuf);
    pldlen = adf_os_ntohs(htchdr->pldlen);

    if (netlen != pldlen) {
      atd_trace(ATD_DEBUG_LEVEL0,("Length Mismatch netlen %d pldlen %d \n",
                  netlen,pldlen));
      adf_nbuf_trim_tail(netbuf,(netlen-pldlen));
      netlen = adf_nbuf_len(netbuf);
      atd_trace(ATD_DEBUG_LEVEL0,("Length After trimming  netlen %d pldlen %d",
                adf_nbuf_len(netbuf),pldlen));
    }


    if (ep->ep_cb.ep_rx) {
        ep->ep_cb.ep_rx(ep->ep_cb.ep_ctx, netbuf, htchdr->epid);
    } else {
        htc_trc_error("EpRecv callback is not registered! HTC is going to"
                      "free the net buffer.\n");
        adf_nbuf_free(netbuf);
    }
out:

    atd_trace(ATD_DEBUG_FUNCTRACE,("End ... "));
    return A_STATUS_OK;

}
static void
htc_control_msg_recv(void * ctx, adf_nbuf_t netbuf, htc_endpointid_t epid)
{
    htc_softc_t * sc = (htc_softc_t *)ctx;
    a_uint8_t *netdata;
    a_uint32_t netlen;
    htc_unknown_msg_t *htcmsg;

    atd_trace(ATD_DEBUG_FUNCTRACE,("Start "));

    adf_nbuf_peek_header(netbuf, &netdata, &netlen);
 
    htcmsg = (htc_unknown_msg_t *)netdata ; 
            


    switch(adf_os_ntohs(htcmsg->msg_id)) {
        case HTC_MSG_READY_ID:
            htc_process_tgt_ready(sc);
            break;
        default:
            htc_trc_error("UnKnown HTC control message \n");
            break;
    }

    atd_trace(ATD_DEBUG_FUNCTRACE,("Start "));

    adf_nbuf_free(netbuf);

}
static a_status_t 
htc_process_tgt_ready(htc_softc_t * sc )
{
    a_status_t status = A_STATUS_OK;

    atd_trace(ATD_DEBUG_FUNCTRACE,("%s Ready Message recv \n",adf_os_function));


    if(sc->htc_ready != NULL)
        sc->htc_ready(sc->host_handle);
    else
        adf_os_print("Ready handler null \n");


    return status;
}

static hif_status_t 
htc_send_comp(void *ctx, adf_nbuf_t netbuf)
{
    htc_softc_t *sc = (htc_softc_t *)ctx;
    a_uint8_t  *netdata;
    a_uint32_t  netlen;
    htc_frame_hdr_t *htchdr;
    htc_endpoint_t *ep ;

    atd_trace(ATD_DEBUG_FUNCTRACE,("Start "));

    adf_nbuf_peek_header(netbuf, &netdata, &netlen);
    netlen = adf_nbuf_len(netbuf);

    htchdr = (htc_frame_hdr_t *)netdata;
    ep = &sc->ep_list[htchdr->epid];

    /* nofity upper layer */            

    if (ep->ep_cb.ep_txcomp) {
        /* give the packet to the upper layer */
        ep->ep_cb.ep_txcomp(ep->ep_cb.ep_ctx, netbuf, 
                htchdr->epid);
    }
    else 
        adf_nbuf_free(netbuf);

    atd_trace(ATD_DEBUG_FUNCTRACE,("End "));

    return A_STATUS_OK;
}


static a_status_t  
htc_issue_send(htc_softc_t *sc, adf_nbuf_t netbuf, 
              a_uint8_t flag, a_uint16_t len, a_uint8_t epid)

{
    hif_status_t  status ;

    htc_endpoint_t  *ep = &sc->ep_list[epid];
    htc_frame_hdr_t *htchdr;

    atd_trace(ATD_DEBUG_FUNCTRACE,("Start "));
    /* setup HTC frame header */
    htchdr  = (htc_frame_hdr_t *)adf_nbuf_push_head(netbuf, 
                                                  sizeof(htc_frame_hdr_t ));


    htchdr->epid   = epid;
    htchdr->flags  = flag;
    htchdr->pldlen = adf_os_htons(len);

    status = hif_send(sc->hif_handle,ep->ul_pipeid, netbuf);

    atd_trace(ATD_DEBUG_FUNCTRACE,("End "));
    if(status != HIF_STATUS_OK)
        return  A_STATUS_FAILED;
    else
        return  A_STATUS_OK;
}

/*
int
init_htc_host(void)
{
    adf_os_print("HTC HOST Version Wasp Loaded...\n");
    return 0;
}

void
exit_htc_host(void)
{
    adf_os_print("HTC HOST UnLoaded...\n");

}


adf_os_export_symbol( htc_create);
adf_os_export_symbol( htc_start);
adf_os_export_symbol( htc_cleanup);
adf_os_export_symbol( htc_waitfor_targetready);
adf_os_export_symbol( htc_send);
adf_os_virt_module_init(init_htc_host);
adf_os_virt_module_exit(exit_htc_host);

*/



