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
#include <linux/skbuff.h>
#include <linux/netdevice.h>
#include <htc_api.h>
#include <htc_internal.h>
#include <atd_trc.h>
#define adf_os_print printk
#define adf_os_function __func__




htc_handle_t 
htc_create(htc_config_t *htc_conf)
{
    a_status_t    status = A_STATUS_OK;
    htc_softc_t       *sc = NULL ;
    htc_endpoint_t   *ep ;
    hif_callback_t    hifcb ;


    atd_trace(ATD_DEBUG_FUNCTRACE,(" Start ."));
    /* allocate HTC instance */

    if ((sc = (htc_softc_t *)kzalloc(sizeof(htc_softc_t),
                                              GFP_KERNEL)) == NULL)
    {
        htc_trc_error("Unable To allocate HTC Instance \n");
        status = A_STATUS_FAILED;
        goto out;
    }

    memset(sc, 0,sizeof(htc_softc_t));


    sc->hif_handle  = htc_conf->hif_handle;
    sc->host_handle = htc_conf->ctx;

    ep = &sc->ep_list[HTC_CONTROL_EPID];
    ep->ul_pipeid = htc_map_ep2ulpipe(HTC_CONTROL_EPID);
    ep->dl_pipeid = htc_map_ep2dlpipe(HTC_CONTROL_EPID);

    ep->ep_cb.ep_rx = htc_control_msg_recv;


    hifcb.context = sc;
    hifcb.send_buf_done  = htc_send_comp;
    hifcb.recv_buf  = htc_recv;

    hif_register(sc->hif_handle,&hifcb);
    hif_start(sc->hif_handle);

out :

    atd_trace(ATD_DEBUG_FUNCTRACE,(" End ."));
    return sc;

}

void 
htc_start(htc_handle_t htc_handle )
{

    htc_softc_t *sc = (htc_softc_t *)htc_handle;
    
    atd_trace(ATD_DEBUG_FUNCTRACE,(" Start ."));
    /*start the hif */
    hif_start(sc->hif_handle);

    atd_trace(ATD_DEBUG_FUNCTRACE,(" End ..."));
}

void *
htc_get_hifhandle(htc_handle_t htc_handle)
{
    htc_softc_t *sc = (htc_softc_t *)htc_handle;
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
void
htc_send_targe_tready(htc_handle_t htc_handle)
{
    a_status_t status;
    struct sk_buff *netbuf;
    htc_softc_t *sc = (htc_softc_t *)htc_handle;
    a_uint32_t  headroom = sizeof(htc_frame_hdr_t) + 
                          hif_get_reserveheadroom(sc->hif_handle);
    htc_ready_msg_t *htc_ready;
    a_uint32_t len;

    atd_trace(ATD_DEBUG_FUNCTRACE,(" Start ."));
    len = headroom + sizeof(htc_ready_msg_t);


    netbuf =  dev_alloc_skb(len);

    if(netbuf == NULL){
        printk("netbuf allocation for htc ready failed \n");
    }

    skb_reserve(netbuf,headroom);
    htc_ready = (htc_ready_msg_t *)skb_put(netbuf, sizeof(htc_ready_msg_t));

    memset(htc_ready,0,sizeof(htc_ready_msg_t));
    htc_ready->MessageID = htons(HTC_MSG_READY_ID);
    sc->htc_stop_flag = 0;
    status = htc_send(sc, netbuf, HTC_CONTROL_EPID);

    if(status != A_STATUS_OK){
        printk(" htc_send_targe_tready :htc_send failed \n");
        dev_kfree_skb_any(netbuf);
    }


    printk("Sending HTC ready \n");

    atd_trace(ATD_DEBUG_FUNCTRACE,(" End ."));

    return ;

}

a_status_t 
htc_send(htc_handle_t htc_handle, struct sk_buff *netbuf, 
                      htc_endpointid_t epid)
{
    htc_softc_t *sc = (htc_softc_t *)htc_handle;
    a_status_t status = A_STATUS_OK;
    htc_endpoint_t *ep;
    a_uint32_t pldlen;

    atd_trace(ATD_DEBUG_FUNCTRACE,(" Start ."));

    if (sc->htc_stop_flag == 2) {
        status = A_STATUS_FAILED;
        goto out;
    }

    ep = &sc->ep_list[epid];


    pldlen = (a_uint16_t) netbuf->len  ;

    status = htc_issue_send(sc ,netbuf, 0, pldlen, epid);
out:

    atd_trace(ATD_DEBUG_FUNCTRACE,(" End ."));
    return status;
}

void 
htc_cleanup(htc_handle_t htc_handle)
{
    htc_softc_t *sc = (htc_softc_t *)htc_handle;

    atd_trace(ATD_DEBUG_FUNCTRACE,(" Start ."));
    hif_cleanup(sc->hif_handle);  
    /* free our instance */
    kfree(sc);

    atd_trace(ATD_DEBUG_FUNCTRACE,(" End ."));
}

void 
htc_set_stopflag(htc_handle_t htc_handle, a_uint8_t val)
{

    htc_softc_t *sc = (htc_softc_t *)htc_handle;
    
    atd_trace(ATD_DEBUG_FUNCTRACE,(" Start ."));
    
    sc->htc_stop_flag = val;

    atd_trace(ATD_DEBUG_FUNCTRACE,(" End ..."));
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

htc_endpointid_t
htc_register_service (htc_handle_t htc_handle ,htc_epcallback_t *cb,
                     htc_service_t  svcid)
{
    a_status_t status = A_STATUS_OK;   
    htc_softc_t *sc = (htc_softc_t *)htc_handle;
    htc_endpoint_t *ep;
    htc_endpointid_t epid;
    
    atd_trace(ATD_DEBUG_FUNCTRACE,(" Start ."));

   /*Static mapping instead of doing connect*/ 
    epid = htc_map_svc2epid(svcid); 
   
    atd_trace(ATD_DEBUG_LEVEL0,(" svcid %d epid %d \n ",svcid,epid));
    if(epid == 0xFF){
        status = A_STATUS_FAILED;
        htc_trc_error("Wrong svcid %d \n",svcid);
        goto out;
    }

    atd_trace(ATD_DEBUG_LEVEL0,(" svcid %d epid %d \n ",svcid,epid));

    ep = &sc->ep_list[epid];
    memcpy(&ep->ep_cb, cb, sizeof(htc_epcallback_t));
    ep->ul_pipeid = htc_map_ep2ulpipe(epid); 
    ep->dl_pipeid = htc_map_ep2dlpipe(epid);
out:

    atd_trace(ATD_DEBUG_FUNCTRACE,(" End ."));
    return epid;

}

static hif_status_t 
htc_recv(void *ctx, struct sk_buff *netbuf, a_uint8_t pipeid)
{
    htc_softc_t *sc = (htc_softc_t *)ctx;
    a_uint8_t  *netdata;
    a_uint32_t  netlen;
    htc_frame_hdr_t *htchdr;
    htc_endpoint_t *ep;

    atd_trace(ATD_DEBUG_FUNCTRACE,(" Start ."));
    netdata = netbuf->data; 
    netlen  = netbuf->len;

    htchdr = (htc_frame_hdr_t *)netdata;

    atd_trace(ATD_DEBUG_LEVEL0,(" ep %d len %d \n",htchdr->epid,netlen));
    if(htchdr->epid > ENDPOINT_MAX){
        htc_trc_error("Wrong EPID recv %d \n",htchdr->epid);
        dev_kfree_skb_any(netbuf);
        goto out;
    }

    ep = &sc->ep_list[htchdr->epid];

    if((htchdr->flags != 0) &&  (htchdr->epid != HTC_CONTROL_EPID)){
        htc_trc_error("Unhandled Data flag %d \n \n",htchdr->flags);
        dev_kfree_skb_any(netbuf);
        goto out;
    }

    skb_pull(netbuf, sizeof(htc_frame_hdr_t));


    if (ep->ep_cb.ep_rx) {
        ep->ep_cb.ep_rx(ep->ep_cb.ep_ctx, netbuf, 
                htchdr->epid);
    }
    else {
        htc_trc_error("EpRecv callback is not registered. \
                           net buf free \n");
        dev_kfree_skb_any(netbuf);
    }
out:

    atd_trace(ATD_DEBUG_FUNCTRACE,(" End ."));
    return A_STATUS_OK;

}

static void
htc_control_msg_recv(void *ctx, struct sk_buff * netbuf, 
                     htc_endpointid_t epid)
{

    htc_trc_error("Not expecting  control message \n \n");

    dev_kfree_skb_any(netbuf);

}

static hif_status_t 
htc_send_comp(void *ctx, struct sk_buff * netbuf)
{
    htc_softc_t *sc = (htc_softc_t *)ctx;
    a_uint8_t  *netdata;
    a_uint32_t  netlen;
    htc_frame_hdr_t *htchdr;
    htc_endpoint_t *ep ;

    atd_trace(ATD_DEBUG_FUNCTRACE,(" Start ."));
    netdata = netbuf->data;
    netlen  = netbuf->len;

    htchdr = (htc_frame_hdr_t *)netdata;
    ep = &sc->ep_list[htchdr->epid];

    /* nofity upper layer */            

    if (ep->ep_cb.ep_txcomp) {
        /* give the packet to the upper layer */
        ep->ep_cb.ep_txcomp(ep->ep_cb.ep_ctx, netbuf, 
                htchdr->epid);
    }
    else 
        dev_kfree_skb_any(netbuf);

    if (sc->htc_stop_flag == 1)
        sc->htc_stop_flag = 2;

    atd_trace(ATD_DEBUG_FUNCTRACE,(" End ."));
    return A_STATUS_OK;
}


static a_status_t  
htc_issue_send(htc_softc_t *sc, struct sk_buff * netbuf, 
              a_uint8_t flag, a_uint16_t len, a_uint8_t epid)

{
    hif_status_t  status ;

    htc_endpoint_t  *ep = &sc->ep_list[epid];
    htc_frame_hdr_t *htchdr;

    atd_trace(ATD_DEBUG_FUNCTRACE,(" Start . "));
    /* setup HTC frame header */
    htchdr  = (htc_frame_hdr_t *) skb_push(netbuf, sizeof(htc_frame_hdr_t ));


    htchdr->epid   = epid;
    htchdr->flags  = flag;
    htchdr->pldlen = htons(len);

    atd_trace(ATD_DEBUG_FUNCTRACE,(" epid %d pipe %d \n",epid,ep->ul_pipeid));

    status = hif_send(sc->hif_handle,ep->ul_pipeid, netbuf);
    if(status != HIF_STATUS_OK)
        return  A_STATUS_FAILED;
    else
        return  A_STATUS_OK;

    atd_trace(ATD_DEBUG_FUNCTRACE,(" End ."));
}

/*
int
init_htc_host(void)
{
    printk("HTC HOST Version Wasp Loaded...\n");
    return 0;
}

void
exit_htc_host(void)
{
    printk("HTC HOST UnLoaded...\n");

}
*/

EXPORT_SYMBOL( htc_create);
EXPORT_SYMBOL( htc_start);
EXPORT_SYMBOL( htc_cleanup);
EXPORT_SYMBOL( htc_send_targe_tready);
EXPORT_SYMBOL( htc_send);
EXPORT_SYMBOL( htc_set_stopflag);
/*module_init(init_htc_host);*/
/*module_exit(exit_htc_host);*/




