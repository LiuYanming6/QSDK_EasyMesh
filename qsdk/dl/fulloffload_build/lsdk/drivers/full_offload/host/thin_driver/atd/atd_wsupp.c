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

#include <atd_internal.h>
#include <adf_os_module.h>
#include <adf_net.h>

#include <atd_wsupp_msg.h>
#include <asf_queue.h>

#define WSUPP_HELPER_DEBUG 0

/* private structures for wpa_supplicant message helper */
typedef struct wsupp_helper {
    /* host device links */
    atd_host_dev_t                 *wh_hdev;
    asf_list_entry(wsupp_helper)   wh_list;
    /* htc stuff */
    htc_handle_t            wh_htc;
    htc_endpointid_t        wh_epid;
    /* locking */
    adf_os_mutex_t          wh_mutex;
    adf_os_spinlock_t       wh_spinlock;
    a_uint32_t              wh_stop;
    /* wsupp message queue handling */
    adf_os_work_t           wh_work;
    adf_nbuf_queue_t        wh_msgq;
} wsupp_helper_t;

struct wh_queue_ctx {
    adf_nbuf_queue_t *queue;
    adf_nbuf_t buf;
};

/* global values */
static a_uint32_t atd_netlink_num = NETLINK_GENERIC + 4;
static adf_netlink_handle_t wh_netlink_handle = NULL;
static asf_list_head(wsupp_help_node, wsupp_helper) wsupp_helper_list;

//static a_status_t wsupp_wps_cb(void * ctx, a_uint8_t *vap) ;
//static a_status_t atd_wps_cb(void *ctx, adf_wps_cb_args_t *cbargs) ;

static atd_host_dev_t *
wh_host_dev_get(adf_nbuf_t netbuf)
{
    wsupp_helper_t *wh;
    /* 
     * TODO: obtain correct host device based on the information
     * provided by wsupp bridge 
     */
    wh = (wsupp_helper_t *) asf_list_first(&wsupp_helper_list);
    if (wh == NULL)
        return NULL;
    return wh->wh_hdev;
}

static void
wh_netlink_input(void *ctx, adf_nbuf_t netbuf, adf_netlink_addr_t addr)
{
    atd_host_dev_t *hdev;
    adf_nbuf_t msgbuf;
    atd_wsupp_message_t *msg;
    a_uint32_t reserve;
    a_uint32_t netbuf_len, msgbuf_len;
    a_uint8_t *netbuf_data;

    hdev = wh_host_dev_get(netbuf);
    if (hdev == NULL) {
        adf_os_print("%s: failed to get dev\n", adf_os_function);
        goto fail;
    }

    /* prepare netbuf for transmitting via HTC */
    reserve = htc_get_reserveheadroom(hdev->htc_handle);
    adf_nbuf_peek_header(netbuf, &netbuf_data, &netbuf_len);
    msgbuf_len = netbuf_len + sizeof(atd_wsupp_message_t);
    msgbuf = adf_nbuf_alloc(NULL, msgbuf_len + reserve + 1, reserve, 0, 0);
    if (msgbuf == NULL) {
        adf_os_print("%s: failed to alloc nbuf\n", adf_os_function);
        goto fail;
    }

    msg = (atd_wsupp_message_t *) adf_nbuf_put_tail(msgbuf, msgbuf_len);
    msg->len = adf_os_htons((a_uint16_t)netbuf_len);
    adf_os_mem_copy(msg->data, netbuf_data, netbuf_len);

#if WSUPP_HELPER_DEBUG
    msg->data[msg->len] = '\0';
    adf_os_print("%s: %d: %s\n", __func__, netbuf_len, msg->data);
#endif

    /* send the wsupp message to target */
    htc_send(hdev->htc_handle, msgbuf, hdev->wsupp_ep);

    return;

fail:
    /* netbuf would be freed by netlink framework */
    return;
}

static void
wh_wsupp_output(void *ctx, adf_nbuf_t netbuf)
{
    adf_nbuf_t nlbuf;
    atd_wsupp_message_t *msg;
    a_uint32_t netbuf_len;
    a_uint8_t *netbuf_data, *nldata;

    adf_nbuf_peek_header(netbuf, &netbuf_data, &netbuf_len);
    msg = (atd_wsupp_message_t *) netbuf_data;

    msg->len = adf_os_ntohs(msg->len);

    nlbuf = adf_netlink_alloc(msg->len + 1);
    if (!nlbuf) {
        adf_os_print("%s: failed to alloc nbuf\n", adf_os_function);
        goto fail;
    }

    nldata = adf_nbuf_put_tail(nlbuf, msg->len);
    adf_os_mem_copy(nldata, msg->data, msg->len);

#if WSUPP_HELPER_DEBUG
    nldata[msg->len]  = '\0';
    adf_os_print("%s: %d: %s\n", __func__, msg->len, msg->data);
#endif

    if (wh_netlink_handle) {
        adf_netlink_broadcast(wh_netlink_handle, nlbuf, 1);
    } else {
        dev_kfree_skb_any(nlbuf);
        adf_os_print("%s: failed to send nbuf\n", adf_os_function);
    }
    /* fall through */
fail:
    dev_kfree_skb_any(netbuf);
}

static a_bool_t
wh_msg_get_safe(void *arg)
{
    struct wh_queue_ctx *ctx = (struct wh_queue_ctx *) arg;

    adf_os_assert(ctx);
    adf_os_assert(ctx->queue);

    ctx->buf = NULL;
    ctx->buf = adf_nbuf_queue_remove(ctx->queue);

    return A_TRUE;
}

static void
wh_work_run(void *arg)
{
    wsupp_helper_t *wh = arg;
    struct wh_queue_ctx qctx;

    adf_os_assert(wh);
    
    /* protect the work thread from detach */
    adf_os_mutex_acquire(NULL,&wh->wh_mutex);

    qctx.queue = &wh->wh_msgq;
    do {
        /* protect the message queue  */
        adf_os_spinlock_irq_exec(NULL, &wh->wh_spinlock, 
            wh_msg_get_safe, &qctx);

        if (!qctx.buf) break;

        wh_wsupp_output(wh, qctx.buf);

    } while(1);

    adf_os_mutex_release(NULL,&wh->wh_mutex);
}

static int
wh_global_init(void)
{
    if (wh_netlink_handle == NULL) {
        wh_netlink_handle = adf_netlink_create(wh_netlink_input, NULL, 
                atd_netlink_num, 0);
        if (wh_netlink_handle == NULL) {
            adf_os_print("no netlink interface created\n");
            return -1;
        }
        adf_os_print("Netlink interface number created: %d\n", atd_netlink_num);
        asf_list_init(&wsupp_helper_list);
    }
    return 0;
}

wsupp_handle_t
wsupp_helper_init(void *hdev, htc_handle_t htc_handle)
{
    wsupp_helper_t *wh;

    if (wh_global_init())
        goto fail;

    wh = (wsupp_helper_t *) adf_os_mem_alloc(NULL, sizeof(*wh));
    if (wh == NULL)
        goto fail;

    wh->wh_hdev = hdev;
    wh->wh_htc = htc_handle;
    adf_os_init_mutex(&wh->wh_mutex);
    adf_os_spinlock_init(&wh->wh_spinlock);
    wh->wh_stop = 0;

    adf_nbuf_queue_init(&wh->wh_msgq);    
    adf_os_create_work(NULL, &wh->wh_work, wh_work_run, wh);
    
    asf_list_insert_head(&wsupp_helper_list, wh, wh_list);

    //adf_net_register_wpscb((void *)wh, atd_wps_cb ) ;
 
    return (wsupp_handle_t) wh;
fail:
    return NULL;
}

void
wsupp_helper_cleanup(wsupp_handle_t wsupp_handle)
{
    wsupp_helper_t *wh = wsupp_handle;
    adf_nbuf_t buf;

    /* should run on sleepable context and HTC/HIF disabled */
    adf_os_assert(wh);

    /* prevent msg from queuing */
    wh->wh_stop = 0;

    /* Unregister WPS PBC callback */
    //adf_net_unregister_wpscb((void *)wh, atd_wps_cb ) ;

    /* disable work and acquire the mutex to cleanup msgq */
    adf_os_destroy_work(NULL, &wh->wh_work);
    adf_os_mutex_acquire(NULL,&wh->wh_mutex);
    while((buf = adf_nbuf_queue_remove(&wh->wh_msgq)))
        adf_nbuf_free(buf);
    adf_os_mutex_release(NULL,&wh->wh_mutex);
    adf_os_destroy_work(NULL, &wh->wh_work);

    /* detach from wsupp helper */
    asf_list_remove(wh, wh_list);
    adf_os_mem_free(wh);

    /* cleanup netlink when needed */
    if (wh_netlink_handle && asf_list_empty(&wsupp_helper_list)) {
        adf_os_print("Netlink interface number deleted: %d\n", atd_netlink_num);
        adf_netlink_delete(wh_netlink_handle);
        wh_netlink_handle = NULL;
    }
    
}

static void
wh_htc_txcomp(void *ctx, adf_nbuf_t netbuf, htc_endpointid_t ep)
{
    adf_nbuf_free(netbuf);
}

static a_bool_t
wh_msg_put_safe(void *arg)
{
    struct wh_queue_ctx *ctx = (struct wh_queue_ctx *) arg;

    adf_os_assert(ctx);
    adf_os_assert(ctx->queue);
    adf_os_assert(ctx->buf);

    adf_nbuf_queue_add(ctx->queue, ctx->buf);

    return A_TRUE;
}

static void
wh_htc_rx(void *ctx, adf_nbuf_t netbuf, htc_endpointid_t ep)
{
    wsupp_helper_t *wh = ctx;
    struct wh_queue_ctx qctx;

    adf_os_assert(wh);

    if (wh->wh_stop) {
        adf_nbuf_free(netbuf);
        return;
    }

    qctx.queue = &wh->wh_msgq;
    qctx.buf = netbuf;
    adf_os_spinlock_irq_exec(NULL, &wh->wh_spinlock, 
        wh_msg_put_safe, &qctx);
    
    adf_os_sched_work(NULL, &wh->wh_work);
}

a_status_t
wsupp_helper_connect(wsupp_handle_t wsupp_handle, htc_endpointid_t *wh_epid)
{
    wsupp_helper_t *wh = wsupp_handle;
    a_status_t status = A_STATUS_OK;
    htc_connect_service_req_t conn_req = {{0}};
    htc_connect_service_resp_t conn_resp = {0};

    adf_os_assert(wh);

    conn_req.cb.ep_ctx      = wh;
    conn_req.cb.ep_txcomp   = wh_htc_txcomp;
    conn_req.cb.ep_rx       = wh_htc_rx;
    conn_req.svcid          = WMI_WSUPP_SVC;

    status = htc_connect_service(wh->wh_htc, &conn_req, &conn_resp);
    if (status != A_STATUS_OK) {
        adf_os_print(" Failed to connect to wsupp service\n");
        return status;
    }

    wh->wh_epid = conn_resp.epid;
    *wh_epid = wh->wh_epid;

    return status;
}

adf_os_declare_param(atd_netlink_num, ADF_OS_PARAM_TYPE_INT32);

#if 0
/** 
 * @brief Sends the message 'PBC <vap>' to wsupp bridge.
 * 
 * @param ctx[in] pointer to wsupp_helper_t
 * @param vap[in] pointer to vap name 
 * 
 * @return 
 */
static a_status_t
wsupp_wps_cb(void * ctx, a_uint8_t *vap)
{
    wsupp_helper_t *wh;
    adf_nbuf_t nbuf ;
    atd_wsupp_message_t *msg;
    wsupp_msg_t *wsupp_msg ;
    adf_os_size_t  msgsize;
    adf_os_size_t  hdrsize;
    a_uint8_t *msgcore = "PBC ";
    a_uint8_t *data ;

    wh = (wsupp_helper_t *)ctx ;
    adf_os_assert(wh);

    /* calculate size of nbuf buffer */
    hdrsize = sizeof(atd_wsupp_message_t) + sizeof(wsupp_msg_t) ;
    msgsize = adf_os_str_len((const char *)msgcore) 
                + adf_os_str_len((const char *)vap);

    nbuf = adf_nbuf_alloc( msgsize + hdrsize, 0, 0) ;
    if(!nbuf)
    {
#if WSUPP_HELPER_DEBUG
        adf_os_print("%s: nbuf allocation failed \n", __func__);
#endif
        goto fail;
    }

   /* Init fields of atd_wsupp_message_t header */
    msg = (atd_wsupp_message_t *)adf_nbuf_put_tail(nbuf, 
                            sizeof(atd_wsupp_message_t)) ;

    msg->len = msgsize + sizeof(wsupp_msg_t) ;
    msg->len = adf_os_htons(msg->len);


    /* Init fields of wsupp_msg_t header */
    wsupp_msg = (wsupp_msg_t *)adf_nbuf_put_tail(nbuf,
                            sizeof(wsupp_msg_t));

    wsupp_msg->wm_type = WSUPP_MSG_TYPE_INTERNAL ;
    wsupp_msg->wm_type = adf_os_htons(wsupp_msg->wm_type);
    wsupp_msg->wm_len = msgsize ; 
    wsupp_msg->wm_len = adf_os_htonl(wsupp_msg->wm_len) ;


    /* copy full message into buffer */
    data = (a_uint8_t *)adf_nbuf_put_tail(nbuf, msgsize) ;

    adf_os_mem_copy((void *)data, (const void *)msgcore,
                        adf_os_str_len((const char *)msgcore) );

    adf_os_mem_copy((void *)data + adf_os_str_len(msgcore), (const void *)vap,
                        adf_os_str_len((const char *)vap) );

    wh_htc_rx((void *)wh, nbuf, HTC_WSUPP_EPID) ;

    return A_STATUS_OK ;

fail:
    return A_STATUS_FAILED ;
}
#endif
#if 0
/** 
 * @brief Called in interrupt context. iterates through all vaps
 *        and sends a PBC message to wsupp bridge for each.
 *        
 * @param ctx[in] Pointer to wsupp_helper_t
 * 
 * @return 
 */
static a_status_t 
atd_wps_cb(void *ctx, adf_wps_cb_args_t *cbargs)
{
    wsupp_helper_t *wh;
    atd_host_dev_t *dsc ;
    atd_host_wifi_t *wifi_list ;
    atd_host_vap_t *vap_list ;
    a_uint32_t num_radio ;
    a_uint32_t num_vap ;
    
    wh = (wsupp_helper_t *)ctx ;
    dsc = wh->wh_hdev  ;
    
    if(cbargs->push_dur > WPS_DEF_PBC_DUR_IGNORE_SECS)
    {
#if WSUPP_HELPER_DEBUG
        adf_os_print("%s: Ignoring PBC as duration more than %ds\n",
                             __func__, WPS_DEF_PBC_DUR_IGNORE_SECS );
#endif
        return A_STATUS_OK ;
    }

    /* iterate through all the vaps as any one may have WPS enabled */
    for(num_radio = 0 ; num_radio < MAX_NUM_RADIO; num_radio++)
    {
        wifi_list = dsc->wifi_list[num_radio] ;
        if(wifi_list)
        {
            for(num_vap = 0 ; num_vap < MAX_VAP_SUPPORTED ; num_vap++)
            {
                adf_net_handle_t nethdl ;
            
                vap_list = wifi_list->vap_list[num_vap] ;
                if(vap_list)
                {
                    nethdl = vap_list->nethandle ;  
                    wsupp_wps_cb(wh , hdl_to_netdev(nethdl)->name ) ;
                }
            }
        }
   }// end for

    return A_STATUS_OK ;
}
#endif

