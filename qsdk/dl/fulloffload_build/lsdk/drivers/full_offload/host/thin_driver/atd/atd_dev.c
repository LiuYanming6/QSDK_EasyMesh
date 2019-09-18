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

#include<linux/kmod.h>

#include "atd_internal.h"
#include "atd_wsupp.h"
#include "atd_appbr.h"
#include "atd_byp.h"

#define ATD_MAXDEVICES 10

extern atd_cfg_wifi_t  wifi_cfg;

/** 
 * @brief 
 * 
 * @param hif_handle
 * 
 * @return 
 */
static hif_status_t  
atd_device_detect    (hif_handle_t hif_handle);

/** 
 * @brief 
 * 
 * @param ctx
 * 
 * @return 
 */
static hif_status_t  
atd_device_disconnect    (void *ctx );

/** 
 * @brief 
 * 
 * @param dsc
 * 
 * @return 
 */
static a_status_t    
atd_connect_services (atd_host_dev_t * dsc );

/** 
 * @brief 
 * 
 * @param dsc
 * 
 * @return 
 */
static void          
atd_device_cleanup   (atd_host_dev_t * dsc);

/** 
 * @brief 
 * 
 * @param dsc
 * @param resp
 * 
 * @return 
 */
static a_uint32_t  
atd_query_num_radio    (atd_host_dev_t *dsc, atd_radio_query_resp_t * resp);

/** 
 * @brief 
 * @return 
 */
static hif_status_t 
atd_target_bootinit(void);

/** 
 * @brief 
 * 
 * @param dsc
 * 
 * @return 
 */
static a_status_t  
atd_configure_target   (atd_host_dev_t *dsc);

/** 
 * @brief Cleanup the wifi
 * 
 * @param dsc
 * @param wifi_index
 */
static void       
atd_wifi_cleanup(atd_host_dev_t  *dsc, a_uint8_t  wifi_index);

/** 
 * @brief 
 * 
 * @param dsc
 * @param resp
 * 
 * @return 
 */
static a_status_t 
atd_setup_wifi(atd_host_dev_t *dsc, atd_radio_query_resp_t * resp);

/** 
 * @brief 
 * 
 * @param vap
 * @param nbuf
 * 
 * @return 
 */
static adf_os_inline a_status_t 
atd_send_prepare(struct atd_host_vap *vap, adf_nbuf_t nbuf);

/** 
 * @brief 
 * 
 * @param ctx
 * 
 * @return 
 */
static a_status_t  
ath_device_htcready(void *ctx );

/** 
 * @brief 
 * 
 * @param ctx
 */
static void   
ath_device_htcready_worker(void *ctx );


/** 
 * @brief 
 * 
 * @param ctx
 * @param cbargs
 * 
 * @return 
 */
static a_status_t 
atd_wps_cb(void *ctx, adf_wps_cb_args_t *cbargs) ;


/** 
 * @brief 
 * 
 * @param ctx
 */
static void 
pbc_worker(void *ctx) ;

/** 
 * @brief send device state change event to the ACFG clients
 * 
 * @param nethandle  - nethandle of the device to be informed
 * @param prev_state - previous state of the device
 * @param new_state  - new state of the device
 */
void
atd_event_device_state_change(adf_net_handle_t nethandle,
                                acfg_device_state_t prev_state,
                                acfg_device_state_t new_state);


/** 
 * @brief  Put the target ln power down mode
 * 
 * @param dsc
 * 
 * @return 
 */
static a_status_t 
atd_pwrdown_device(atd_host_dev_t *dsc);

#ifdef ATD_DEBUG_ENABLE
int atd_dbg_mask ;
#endif

atd_host_dev_t * atd_devices[ATD_MAXDEVICES];


/* 
 * Module configurable  parameter
 */

a_uint32_t atd_target_devtype = WASP_TARGET;
a_uint32_t atd_tgt_pwrdown_support = 0;


static hif_status_t  
atd_device_detect( hif_handle_t hif_handle)
{
    atd_host_dev_t        *dsc;
    int                    dev_index;
    a_status_t             status = A_STATUS_OK;
    htc_config_t           htccfg;

    atd_trace(ATD_DEBUG_FUNCTRACE,(" x "));

    dsc = adf_os_mem_alloc(NULL, sizeof(struct  atd_host_dev));
    if(dsc == NULL){
        atd_trace(ATD_DEBUG_FUNCTRACE, ("Device Mem Allocation Failure \n"));
        status = A_STATUS_FAILED;
        goto fail;
    }
    adf_os_mem_zero(dsc, sizeof(struct atd_host_dev));

    for (dev_index = 0; dev_index < ATD_MAXDEVICES; dev_index++) 
        if (!atd_devices[dev_index]) break;
    

    if (dev_index == ATD_MAXDEVICES) {
        atd_trace(ATD_DEBUG_FUNCTRACE,\
                ("atd_device_detect : max devices reached\n"));
        status = A_STATUS_FAILED;
        goto fail;
    }

    atd_devices[dev_index] = dsc;
    dsc->target_type       = atd_target_devtype ;
    dsc->dev_index         = dev_index;

    /* Initialize the ref count & acquire the sem_del  */
    atd_init_ref(dsc);

    /* Init the BIG Locks & Mutexes */
    adf_os_spinlock_init(&dsc->spin_lock);
    adf_os_init_mutex(&dsc->mutex);

    atd_trace(ATD_DEBUG_FUNCTRACE, ("%s : atd_target_devtype =%d \n", \
                                    adf_os_function,atd_target_devtype));

    hif_claim_device(hif_handle, dsc);
    dsc->hif_handle = hif_handle; 

    if ((status = atd_configure_target(dsc)) != A_STATUS_OK) {
        adf_os_print("Target Configuration failed \n");
        goto fail;
    }
    /*at this point The firmware download has finish No FWD or BMI exist*/
    adf_os_mem_zero(&htccfg, sizeof(htccfg));

    htccfg.ctx        = dsc;
    htccfg.hif_handle = dsc->hif_handle;
    htccfg.htc_ready  = ath_device_htcready;
    if ((dsc->htc_handle   = htc_create(&htccfg)) == NULL)
        status= A_STATUS_FAILED;
fail:
    if(status != A_STATUS_OK){
        if(dsc != NULL ) {
            if (dev_index < ATD_MAXDEVICES)
                atd_devices[dev_index] = NULL;
            atd_device_cleanup(dsc);
        }
    }

    return status;

}

static a_status_t
ath_device_htcready(void *ctx ){

    atd_host_dev_t * dsc = (atd_host_dev_t *)ctx;

    atd_trace(ATD_DEBUG_FUNCTRACE,(" Start "));

    if(dsc->wmi_handle != NULL)
    {
		printk("Invalid HTC READY Received Ignoring......\n");
		return A_STATUS_OK;
    }

    adf_os_create_work(NULL, &dsc->htcrdy_work, ath_device_htcready_worker, 
                     (void  *)dsc);

    adf_os_create_work(NULL, &dsc->pbc_work, pbc_worker, 
                     (void  *)dsc);

    atd_trace(ATD_DEBUG_LEVEL0,(" scheduling work "));

    adf_os_sched_work(NULL, &dsc->htcrdy_work);

    atd_trace(ATD_DEBUG_FUNCTRACE,(" End "));

    return A_STATUS_OK;
}

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,31))
extern struct net init_net;
#endif
struct sock *atd_spectral_nl_sock;
struct sock *atd_bsteer_nl_sock;
a_uint32_t atd_bsteer_pid;

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,31))
void atd_spectral_nl_data_ready(struct sock *sk, int len)
{
            printk("%s %d\n", __func__, __LINE__);
                    return;
}

#else
void atd_spectral_nl_data_ready(struct sk_buff *skb)
{
            printk("%s %d\n", __func__, __LINE__);
                    return;
}
#endif /* VERSION*/

static void atd_band_steering_netlink_receive(struct sk_buff *__skb)
{
    struct sk_buff *skb;
    struct nlmsghdr *nlh = NULL;
    u_int8_t *data = NULL;

    if ((skb = skb_get(__skb)) != NULL){
        /* process netlink message pointed by skb->data */
        nlh = (struct nlmsghdr *)skb->data;
        data = NLMSG_DATA(nlh);
        atd_bsteer_pid = nlh->nlmsg_pid;
        printk("Band steering events being sent to PID:%d\n", 
                atd_bsteer_pid);
        kfree_skb(skb);
    }
}

static void 
ath_device_htcready_worker(void *ctx)
{

    atd_host_dev_t * dsc = (atd_host_dev_t *)ctx;
    a_status_t status = A_STATUS_OK;
    atd_radio_query_resp_t  radiorsp;
    atd_host_wifi_t *wifi;
    a_uint32_t i;

    atd_trace(ATD_DEBUG_FUNCTRACE,(" Start "));

    if (dsc == NULL) {
        adf_os_print("%s: invalid context\n", __func__);
        status = A_STATUS_FAILED;
        goto fail;
    }

    dsc->wmi_handle = wmi_init(dsc, dsc->htc_handle, atd_event_run);

    if (dsc->wmi_handle == NULL) {
        adf_os_print("wmi init failed \n");
        status = A_STATUS_FAILED;
        goto fail;
    }

    dsc->wsupp_handle = wsupp_helper_init(dsc, dsc->htc_handle);
    if (dsc->wsupp_handle == NULL) {
        adf_os_print("wsupp helper init failed\n");
        status = A_STATUS_FAILED;
        goto fail;
    }

    if ((status = atd_connect_services(dsc)) != A_STATUS_OK) {
        adf_os_print("Connect Service Failure failed \n");
        goto fail;
    }

    htc_start(dsc->htc_handle);

    if ((status = atd_query_num_radio(dsc, &radiorsp)) != A_STATUS_OK) {
        adf_os_print("No radio detected \n");
        goto fail;
    }

    atd_trace(ATD_DEBUG_FUNCTRACE, ("Number of Radio Detected  %d \n",\
                radiorsp.num_radio));

    /*
     * populate deffault device mac addr 
     * register the wifi0 as per the num of radio 
     */ 
    status = atd_setup_wifi(dsc, &radiorsp);

    if(status != A_STATUS_OK)
        goto fail;

    /* Register WPS PBC callback for radios handled by offload stack. 
     * We register this only for one radio as the WPS PBC button is 
     * not radio specific and is common for all vaps on all radios.
     */
    if (!dsc->pbc_cb_radio)
    {
        for(i = 0 ; i < MAX_NUM_RADIO ; i++)
        {
            wifi = dsc->wifi_list[i];
            if(wifi)
            {
                adf_net_register_wpscb(wifi->netdev, (void *)wifi, atd_wps_cb);
                dsc->pbc_cb_radio = wifi ;
                break;
            }
        }
    }


    dsc->appbr_handle   =   atd_appbr_init(dsc, APPBR_NETLINK_NUM);
    if (dsc->appbr_handle == NULL) {
        adf_os_print("Application Bridge init failed\n");
        status = A_STATUS_FAILED;
        goto fail;
    }

    dsc->byp_handle     =   atd_byp_init(dsc);
    if (dsc->byp_handle == NULL) {
        adf_os_print("Bypass Module init failed\n");
        status = A_STATUS_FAILED;
        goto fail;
    }

    if (atd_spectral_nl_sock == NULL) {
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,31))
        atd_spectral_nl_sock = (struct sock *)netlink_kernel_create(NETLINK_ATHEROS, 1,&atd_spectral_nl_data_ready, THIS_MODULE);
#else
        atd_spectral_nl_sock = (struct sock *)netlink_kernel_create(&init_net,NETLINK_ATHEROS, 1,&atd_spectral_nl_data_ready, NULL, THIS_MODULE);
#endif
        if (atd_spectral_nl_sock == NULL) {
            printk("%s Netlink interface create FAILED : %d\n", __func__, NETLINK_ATHEROS);
            status = A_STATUS_FAILED;
        }
        else
            printk("%s Netlink interface created : %d\n", __func__, NETLINK_ATHEROS);
    }
#define NETLINK_BAND_STEERING_EVENT 21
    if(atd_bsteer_nl_sock == NULL) {
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,31))
        atd_bsteer_nl_sock = (struct sock *)netlink_kernel_create(NETLINK_BAND_STEERING_EVENT,
                1,
                &atd_band_steering_netlink_receive,
                THIS_MODULE);
#else
        atd_bsteer_nl_sock = (struct sock *)netlink_kernel_create(&init_net,
                NETLINK_BAND_STEERING_EVENT,
                1,
                &atd_band_steering_netlink_receive,
                NULL,
                THIS_MODULE);
#endif
        if (atd_bsteer_nl_sock == NULL) {
            printk("%s Netlink interface create FAILED : %d\n",
                    __func__,
                    NETLINK_BAND_STEERING_EVENT);
            status = A_STATUS_FAILED;
        }
        else
            printk("%s Netlink interface created : %d\n",
                    __func__,
                    NETLINK_BAND_STEERING_EVENT);
    }
    /* intimate target up event . Use nethandle as NULL for generic event */
    atd_event_device_state_change(NULL, ACFG_DEVICE_STATE_RESET, ACFG_DEVICE_STATE_READY);

fail:
    if (status != A_STATUS_OK) {
        if (dsc != NULL ) {
            atd_devices[dsc->dev_index] = NULL;
            atd_device_cleanup(dsc);
        }
    }

    atd_trace(ATD_DEBUG_FUNCTRACE,(" End... "));
    return ;
}


hif_status_t
atd_device_disconnect(void *ctx)
{
    int i;
    atd_host_dev_t *dsc = (atd_host_dev_t *) ctx;

    atd_trace(ATD_DEBUG_FUNCTRACE,(" Start "));

    for (i = 0; i < ATD_MAXDEVICES; i++) {
        if (atd_devices[i] != dsc)
            continue;
        atd_trace(ATD_DEBUG_FUNCTRACE,(" dsc %p index %d ", atd_devices[i], i));
        atd_devices[i] = NULL;
        atd_device_cleanup(dsc);
        break;
    }

    atd_trace(ATD_DEBUG_FUNCTRACE,(" End "));

    return 0;
}


a_status_t 
atd_connect_services(atd_host_dev_t * dsc )
{
    a_status_t status =A_STATUS_OK;
    htc_connect_service_req_t  creq;
    htc_connect_service_resp_t cresp;
    htc_handle_t htc_handle = dsc->htc_handle;
    wmi_handle_t wmi_handle = dsc->wmi_handle;

    atd_trace(ATD_DEBUG_FUNCTRACE,(" Start "));
    status = wmi_connect(htc_handle , wmi_handle , &dsc->wmi_cmd_ep);
    if (status == A_STATUS_FAILED) {
        adf_os_print("wmi connect failed!");
        return status;
    }

    atd_trace(ATD_DEBUG_LEVEL3,("%s : wmiconnect  %d \n",adf_os_function,
                                dsc->wmi_cmd_ep));
    /* wsupp helper service */
    status = wsupp_helper_connect(dsc->wsupp_handle, &dsc->wsupp_ep);
    if (status == A_STATUS_FAILED) {
        adf_os_print("wsupp helper connect failed!");
        return status;
    }
    atd_trace(ATD_DEBUG_LEVEL3,("%s: wsupp helper epid: %d\n",adf_os_function,
                                dsc->wsupp_ep));

    adf_os_mem_zero(&creq, sizeof(htc_connect_service_req_t));

    creq.cb.ep_ctx       = dsc;
    creq.cb.ep_txcomp   = NULL;
    creq.cb.ep_rx       = atd_host_rx;
    creq.svcid          = WMI_DATA_BE_SVC;
    
    atd_trace(ATD_DEBUG_FUNCTRACE, ("Start connect service \n"));

    status = htc_connect_service(htc_handle, &creq, &cresp);

    dsc->data_ep = cresp.epid;


    if (status == A_STATUS_FAILED){
        adf_os_print("Failed to connect services!");
        return status ;
    }

    atd_trace(ATD_DEBUG_LEVEL3,("data_ep  %d \n",dsc->data_ep)); 

    atd_trace(ATD_DEBUG_FUNCTRACE,(" End "));
    return status;
}


static void 
atd_device_cleanup(atd_host_dev_t * dsc)
{
    a_uint32_t i = 0;

    atd_trace(ATD_DEBUG_FUNCTRACE,(".Start"));

    /* intimate target down event */
    atd_event_device_state_change(NULL, ACFG_DEVICE_STATE_READY, ACFG_DEVICE_STATE_RESET);

    /** 
     * Power down the device if it is supported
     */
    if(atd_tgt_pwrdown_support)
        atd_pwrdown_device(dsc);

    /* All cleanup should happen before interface cleanup */
    /* Unregister WPS PBC callback */
    if(dsc->pbc_cb_radio)
    {
        adf_net_unregister_wpscb(dsc->pbc_cb_radio->netdev) ;
        dsc->pbc_cb_radio = NULL;
    }

    if (dsc->byp_handle)
        atd_byp_cleanup(dsc->byp_handle);

    if (dsc->appbr_handle)
        atd_appbr_cleanup(dsc->appbr_handle);

    if (dsc->wsupp_handle)
        wsupp_helper_cleanup(dsc->wsupp_handle);

    if (dsc->wmi_handle)
        wmi_stop(dsc->wmi_handle);
    if (dsc->wmi_handle)
        wmi_drain_eventq(dsc->wmi_handle);
    if (dsc->wmi_handle)
        wmi_cleanup(dsc->wmi_handle);

    if (dsc->htc_handle)
        htc_cleanup(dsc->htc_handle);

    if (dsc->hif_handle)
        hif_cleanup(dsc->hif_handle);

    atd_mutex_lock(dsc);

    atd_trace(ATD_DEBUG_CFG,("Cleaning interfaces"));

    for(i = 0; i < MAX_NUM_RADIO; i++) 
        atd_wifi_cleanup(dsc, i);

    atd_trace(ATD_DEBUG_CFG,("Cleaning Handles & Events"));

    adf_os_destroy_work(NULL, &dsc->htcrdy_work);

    adf_os_destroy_work(NULL, &dsc->pbc_work);
    
    atd_mutex_unlock(dsc);

    /* Checkpoint after this there is nobody out there  */
    atd_flush_ref(dsc);
    
    atd_trace(ATD_DEBUG_CFG,("Flush Done"));

    adf_os_spinlock_destroy(&dsc->spin_lock);

    adf_os_mem_free(dsc);

    atd_trace(ATD_DEBUG_FUNCTRACE,(".End"));
}


a_uint32_t 
atd_query_num_radio(atd_host_dev_t *dsc,
                    atd_radio_query_resp_t * resp)
{
    a_status_t status = A_STATUS_OK;
    a_uint8_t *data;
    a_uint32_t i;
    adf_nbuf_t nbuf;
    
    atd_trace(ATD_DEBUG_FUNCTRACE,(" x "));

    if ((nbuf = _atd_wmi_msg_alloc(dsc->wmi_handle)) == NULL) {
        adf_os_print("Unable to allocate wmi buffer \n");
        goto out ;
    }

    data = (a_uint8_t *) adf_nbuf_put_tail(nbuf, sizeof(a_uint32_t));
    data[0] = 0;


    status = wmi_cmd_send(dsc->wmi_handle, WMI_CMD_RADIO_QUERY, nbuf,
                          (a_uint8_t *)resp, sizeof(atd_radio_query_resp_t));

    if(status != A_STATUS_OK){
        adf_os_print(" wmi_cmd_send Failed stat = %x\n", status);
        goto out;
    }

    for(i=0 ; i < resp->num_radio ; i++){
        adf_os_print("wifi mac address name %d %s  :%x:%x:%x:%x:%x:%x \n",
                      i,
                      &resp->ifname[i][0],
                      resp->mac_addr[i][0], 
                      resp->mac_addr[i][1], 
                      resp->mac_addr[i][2], 
                      resp->mac_addr[i][3], 
                      resp->mac_addr[i][4],
                      resp->mac_addr[i][5]);
    }
out:
    return status ;
}

a_uint32_t 
atd_pwrdown_device(atd_host_dev_t *dsc)
                    
{
    a_status_t status = A_STATUS_OK;
    a_uint8_t *data;
    adf_nbuf_t nbuf;
    
    atd_trace(ATD_DEBUG_FUNCTRACE,(" x "));

    if ((nbuf = _atd_wmi_msg_alloc(dsc->wmi_handle)) == NULL) {
        adf_os_print("Unable to allocate wmi buffer \n");
        goto out ;
    }

    data = (a_uint8_t *) adf_nbuf_put_tail(nbuf, sizeof(a_uint32_t));
    data[0] = 0;


    status = wmi_cmd_send(dsc->wmi_handle, WMI_CMD_PWRDOWN, nbuf,
                          NULL, 0);

    if(status != A_STATUS_OK) {
        adf_os_print(" wmi_cmd_send Failed stat = %x\n", status);
        goto out;
    }

out:
    return status ;
}


a_status_t  
atd_setup_wifi(atd_host_dev_t * dsc, 
        atd_radio_query_resp_t * radiorsp)
{
    a_status_t status = A_STATUS_OK;
    int i =0;

    atd_trace(ATD_DEBUG_FUNCTRACE,(" x "));
    for (i=0;i< radiorsp->num_radio;i++)
    {

        adf_dev_sw_t dev_switch={0};
        adf_net_dev_info_t info = {{0}};
        dsc->wifi_list[i] = adf_os_mem_alloc(NULL, sizeof(struct atd_host_wifi)); 
        if(dsc->wifi_list[i] == NULL){
            adf_os_print("Error in allocationg wifi");
            return A_STATUS_FAILED;
        }
        adf_os_mem_zero(dsc->wifi_list[i], sizeof(struct atd_host_wifi));
        dsc->wifi_list[i]->wifi_index = i;
        dsc->wifi_list[i]->dsc = dsc ;
        /*populate switch */
        dev_switch.drv_open             = atd_wifi_open ;
        dev_switch.drv_close            = atd_wifi_close;
        dev_switch.drv_tx               = atd_wifi_tx   ;
        dev_switch.drv_cmd              = atd_wifi_cmd  ;
        dev_switch.drv_ioctl            = atd_wifi_ioctl;
        dev_switch.drv_tx_timeout       = atd_wifi_tx_timeout ;  
        
        atd_trace(ATD_DEBUG_FUNCTRACE, ("Creating netdev ..... %d %s \n", \
                                                i, &radiorsp->ifname[i][0]));
        adf_os_str_ncopy(info.if_name,&radiorsp->ifname[i][0], 
                         adf_os_str_len(&radiorsp->ifname[i][0]));

        adf_os_mem_copy(info.dev_addr, &radiorsp->mac_addr[i], 
                        ACFG_MACADDR_LEN);

        info.header_len = sizeof(atd_tx_hdr_t) + 
                          htc_get_reserveheadroom(dsc->htc_handle);


        dsc->wifi_list[i]->netdev = adf_net_create_wifidev(dsc->wifi_list[i], 
                                                           &dev_switch, 
                                                           &info, 
                                                           &wifi_cfg);

    }
    return status;

}

/** 
 * @brief This function will cleanup the VAP 
 * 
 * @param wifi
 * @param vap_index
 * 
 * @return 
 */
void
atd_vap_cleanup(atd_host_wifi_t     *wifi, a_uint8_t    vap_index)
{
    atd_host_vap_t      *vap;

    atd_trace(ATD_DEBUG_FUNCTRACE, (".Start"));

    vap = atd_free_vap_index(wifi, vap_index);

    if (!vap) goto bail;

    atd_trace(ATD_DEBUG_CFG, ("Deleted net dev %s \n", 
                adf_net_ifname(vap->nethandle)));

    adf_os_assert(vap->nethandle);

    if(vap->nethandle)
    {
        /* Free bitmap index of VAP array maintained by ADF */
        adf_net_free_wlanunit(vap->nethandle);
        adf_net_delete_dev(vap->nethandle);
    }

    adf_os_mem_free(vap);

bail:
    atd_trace(ATD_DEBUG_FUNCTRACE, (".End"));
}


void 
atd_wifi_cleanup(atd_host_dev_t     *dsc, a_uint8_t   wifi_index)
{
    atd_host_wifi_t    *wifi = NULL;
    int                 i = 0;


    atd_trace(ATD_DEBUG_FUNCTRACE,(" Start "));

    /* Protect the wifi list access */
    atd_spin_lock_bh(dsc);              /* Lock */

    wifi = dsc->wifi_list[wifi_index];

    dsc->wifi_list[wifi_index] = NULL;
   
    atd_spin_unlock_bh(dsc);            /* Unlock */
    
    if (!wifi)  goto bail;

    for (i = 0; i < MAX_VAP_SUPPORTED; i++) 
        atd_vap_cleanup(wifi, i);

    adf_os_assert(wifi->netdev);

    adf_net_delete_dev(wifi->netdev);

    adf_os_mem_free(wifi);

bail:
    atd_trace(ATD_DEBUG_FUNCTRACE,(" End "));
}



a_status_t atd_wifi_open(adf_drv_handle_t hdl)
{
    a_status_t status = A_STATUS_OK ;

    atd_trace(ATD_DEBUG_FUNCTRACE,("Start"));
    atd_trace(ATD_DEBUG_ERROR,("Not expected"));
    atd_trace(ATD_DEBUG_FUNCTRACE,("End"));

    return status;
}

void  
atd_wifi_close(adf_drv_handle_t hdl)
{
    atd_trace(ATD_DEBUG_FUNCTRACE,("Start"));
    atd_trace(ATD_DEBUG_ERROR,("Not expected"));
    atd_trace(ATD_DEBUG_FUNCTRACE,("End"));
}

a_status_t 
atd_wifi_tx_timeout(adf_drv_handle_t hdl)
{

    a_status_t status = A_STATUS_OK ;

    atd_trace(ATD_DEBUG_FUNCTRACE,("Start"));
    atd_trace(ATD_DEBUG_ERROR,("Not expected"));
    atd_trace(ATD_DEBUG_FUNCTRACE,("End"));
 
    return status;

}
a_status_t 
atd_wifi_cmd(adf_drv_handle_t  drv_hdl, adf_net_cmd_t cmd, 
             adf_net_cmd_data_t  *data)
{
    a_status_t status = A_STATUS_OK ;

    atd_trace(ATD_DEBUG_FUNCTRACE,("Start"));
    atd_trace(ATD_DEBUG_ERROR,("Not expected"));
    atd_trace(ATD_DEBUG_FUNCTRACE,("End"));
 
    return status;
}

a_status_t 
atd_wifi_ioctl(adf_drv_handle_t hdl, a_int32_t num, void *data)
{

    a_status_t status = A_STATUS_OK ;
    atd_trace(ATD_DEBUG_FUNCTRACE,("Start"));
    atd_trace(ATD_DEBUG_ERROR,("Not expected"));
    atd_trace(ATD_DEBUG_FUNCTRACE,("End"));
 
    return status;


}

/*
 * Vap configuration 
 */ 
a_status_t 
atd_vap_ioctl(adf_drv_handle_t hdl, a_int32_t num, void *data)
{

    a_status_t status = A_STATUS_OK ;
    atd_trace(ATD_DEBUG_FUNCTRACE,("Start"));
    atd_trace(ATD_DEBUG_ERROR,("Not expected"));
    atd_trace(ATD_DEBUG_FUNCTRACE,("End"));
 
    return status;


}
a_status_t 
atd_vap_tx_timeout(adf_drv_handle_t hdl)
{

    a_status_t status = A_STATUS_OK ;

    atd_trace(ATD_DEBUG_FUNCTRACE,("start"));
    atd_trace(ATD_DEBUG_ERROR,("not expected"));
    atd_trace(ATD_DEBUG_FUNCTRACE,("end"));
    return status;

}
a_status_t
atd_vap_open(adf_drv_handle_t hdl)
{
    a_status_t status = A_STATUS_OK;
    struct atd_host_vap * vap = (struct atd_host_vap *)hdl;
    adf_net_handle_t nethandle = vap->nethandle;
    atd_host_wifi_t * wifi = vap->wifisc;
    atd_vap_open_cmd_t * cmd;  
    wmi_handle_t   wmi_handle =  __wifi2wmi(wifi);
    adf_nbuf_t nbuf = NULL;


    atd_trace(ATD_DEBUG_FUNCTRACE,("Start"));

    nbuf = _atd_wmi_msg_alloc(wmi_handle);

    if(nbuf == NULL){
        adf_os_print("Unable to allocate wmi buffer \n");
        goto out ;
    }
    
    cmd = (atd_vap_open_cmd_t *) adf_nbuf_put_tail(nbuf,
                                        sizeof(atd_vap_open_cmd_t));
    cmd->wifi_index = wifi->wifi_index ;
    cmd->vap_index  = vap->vap_index ;
    status = wmi_cmd_send( wmi_handle, WMI_CMD_VAP_OPEN, nbuf,
                           NULL, 0);

    if(status){
        adf_os_print("wmi_cmd_send fail  = %x\n", status);
        goto out;
    }

    adf_net_start_queue(nethandle);

out :    

    atd_trace(ATD_DEBUG_FUNCTRACE,("End ..."));
    return status;
}

void
atd_vap_close(adf_drv_handle_t hdl)
{

    a_status_t status = A_STATUS_OK;
    struct atd_host_vap * vap = (struct atd_host_vap *)hdl;
    adf_net_handle_t nethandle = vap->nethandle;
    atd_host_wifi_t * wifi = vap->wifisc;
    atd_vap_close_cmd_t * cmd;  
    wmi_handle_t   wmi_handle =  __wifi2wmi(wifi);
    adf_nbuf_t nbuf = NULL;



    atd_trace(ATD_DEBUG_FUNCTRACE,(" Start  "));

    nbuf = _atd_wmi_msg_alloc(wmi_handle);

    if(nbuf == NULL){
        adf_os_print("Unable to allocate wmi buffer \n");
        goto out ;
    }
    
    cmd = (atd_vap_open_cmd_t *) adf_nbuf_put_tail(nbuf,
                                        sizeof(atd_vap_close_cmd_t));
    cmd->wifi_index = wifi->wifi_index ;
    cmd->vap_index  = vap->vap_index ;
    status = wmi_cmd_send( wmi_handle, WMI_CMD_VAP_CLOSE, nbuf,
                           NULL, 0);

    if(status){
        adf_os_print("wmi_cmd_send fail  = %x\n", status);
        goto out;
    }


    if(adf_net_is_running(nethandle))
        adf_net_stop_queue(nethandle);
out:
    atd_trace(ATD_DEBUG_FUNCTRACE,(" End ... "));
    return;
}


a_status_t
atd_vap_cmd(adf_drv_handle_t  drv_hdl, adf_net_cmd_t cmd,
            adf_net_cmd_data_t  *data)
{   
    a_status_t  error = A_STATUS_OK;

    atd_trace(ATD_DEBUG_FUNCTRACE,(" x "));
    atd_trace(ATD_DEBUG_FUNCTRACE, (" %s cmd %d \n",adf_os_function,cmd));
    switch (cmd) {
        case ADF_NET_CMD_SET_MCAST:
            atd_trace(ATD_DEBUG_FUNCTRACE,(" %s : ADF_NET_CMD_SET_MCAST: \n",\
                    adf_os_function));
            break;
        case ADF_NET_CMD_SET_MAC_ADDR:
            atd_trace(ATD_DEBUG_FUNCTRACE,(" %s : ADF_NET_CMD_SET_MAC_ADDR:\n",\
                    adf_os_function));
            break;
        default:
            atd_trace(ATD_DEBUG_FUNCTRACE,(" %s : default \n",adf_os_function));
            error = A_STATUS_EINVAL;
            break;
    }
    return error;
}


a_int32_t atd_device_module_init(void )
{

    int i;
    hif_os_callback_t cb;

    atd_trace(ATD_DEBUG_FUNCTRACE,(" Start "));

    for(i=0;i<ATD_MAXDEVICES;i++){
        atd_devices[i] = NULL;
    }
    adf_os_mem_zero(&cb,sizeof(cb));
    cb.device_detect  = atd_device_detect;
    cb.device_disconnect =  atd_device_disconnect;
    cb.device_bootinit = atd_target_bootinit;
    hif_init(&cb);

    atd_trace(ATD_DEBUG_FUNCTRACE,(" End "));
    return 0;
}

void atd_device_module_exit( void )
{
    int i;

    atd_trace(ATD_DEBUG_FUNCTRACE,(" Start "));

    for (i = 0; i < ATD_MAXDEVICES; i++) {
        atd_host_dev_t *dsc = atd_devices[i];
        if(dsc == NULL)
            continue;
        atd_trace(ATD_DEBUG_FUNCTRACE,(" dsc %p index %d ", dsc, i));
        atd_devices[i] = NULL;
        atd_device_cleanup(dsc);
    }
    if(atd_spectral_nl_sock)
    {
        sock_release(atd_spectral_nl_sock->sk_socket);
        atd_spectral_nl_sock = NULL;
    }
    if(atd_bsteer_nl_sock)
    {
        sock_release(atd_bsteer_nl_sock->sk_socket);
        atd_bsteer_nl_sock = NULL;
    }
    atd_trace(ATD_DEBUG_FUNCTRACE,(" End "));
}


a_status_t
atd_host_tx(adf_drv_handle_t hdl, adf_nbuf_t nbuf)
{
    a_status_t status = A_STATUS_OK;
    struct atd_host_vap   *vap   = (struct atd_host_vap *)hdl;
    atd_host_dev_t *dsc;
    byp_sc_t * byp_handle;
    byp_filter_fn_t byp_filter;


    atd_trace(ATD_DEBUG_FUNCTRACE,(" Start "));
    if(vap->wifisc == NULL){
        goto out;
    }
    dsc     = vap->wifisc->dsc;


    if (adf_net_is_running(vap->nethandle))
    {
        /*adf_nbuf_peek_header(nbuf, (a_uint8_t **)&eh, &len);*/

        /* For this require sta association event should come from target . 
           an  = ieee80211_find_txnode(vap, eh->ether_dhost);

           if(an == NULL);
           goto dropframe;
         */

        byp_handle  = (byp_sc_t *) dsc->byp_handle;
        byp_filter  = BYPSC_FILTER(byp_handle);

        if(byp_filter)
        {
            nbuf = byp_filter(byp_handle, vap, nbuf);

            if(!nbuf)
                goto out;
        }

        status = atd_send_prepare(vap, nbuf);
        if(status != A_STATUS_OK ){
            atd_trace(ATD_DEBUG_ERROR,(" Unable to Prepare buf for Tx "));
            status = A_STATUS_FAILED;
            goto out;
        }
        status = htc_send(dsc->htc_handle, nbuf, dsc->data_ep);
        if(status != A_STATUS_OK ){
            atd_trace(ATD_DEBUG_ERROR,(" Unable to Send to HTC  "));
            status = A_STATUS_FAILED;
            goto out;
        }
    }else{
        atd_trace(ATD_DEBUG_ERROR,(" vap is not running  "));
        status = A_STATUS_FAILED;
    }
out :
    if(status != A_STATUS_OK){
        if( nbuf != ADF_NBUF_NULL )
            adf_nbuf_free(nbuf);

    } 

    atd_trace(ATD_DEBUG_FUNCTRACE,(" Start "));
    return 0;
}

/**
 * Send & Recv
 */


a_status_t 
atd_wifi_tx(adf_drv_handle_t hdl, adf_nbuf_t nbuf)
{

    a_status_t status = A_STATUS_OK ;

    atd_trace(ATD_DEBUG_FUNCTRACE,("start"));

    atd_trace(ATD_DEBUG_ERROR,("not expected"));
    adf_nbuf_free(nbuf);

    atd_trace(ATD_DEBUG_FUNCTRACE,("end"));
 
    return status;

}

static adf_os_inline a_status_t
atd_send_prepare(struct atd_host_vap *vap, adf_nbuf_t nbuf)
{

     atd_tx_hdr_t * dh;
     atd_host_wifi_t * wifi = vap->wifisc;
     a_uint32_t headroom = htc_get_reserveheadroom(wifi->dsc->htc_handle)
                           + sizeof(atd_tx_hdr_t);

    atd_trace(ATD_DEBUG_FUNCTRACE,(" x "));

    if(adf_os_unlikely(adf_nbuf_headroom(nbuf) < headroom)) {
        /*
         * XXX Enable this print for non-i386 platforms
         */
        atd_trace(ATD_DEBUG_FUNCTRACE,("Headroom Less\n"));
        nbuf = adf_nbuf_realloc_headroom(nbuf, headroom );
        if (nbuf == ADF_NBUF_NULL) {
            adf_os_print("Cant allocate nbuf\n");
            return A_STATUS_FAILED;
        }

    }
    dh = (atd_tx_hdr_t*)adf_nbuf_push_head(nbuf, 
                                            sizeof(atd_tx_hdr_t));
    dh->vap_index   = vap->vap_index ;
    dh->wifi_index = wifi->wifi_index;
    return A_STATUS_OK;
}

/*Recieve Path */

#define atd_host_find_vap(_wifi,_vapindex) _wifi->vap_list[_vapindex]
void   
atd_host_rx(void *instance, adf_nbuf_t nbuf, htc_endpointid_t ep )
{

    a_uint8_t *anbdata;
    a_uint32_t anblen;
    atd_rx_hdr_t *rxhdr;
    atd_host_wifi_t * wifi = NULL;
    struct atd_host_vap *vap =NULL;
    atd_host_dev_t *adsc = (atd_host_dev_t *) instance;

    atd_trace(ATD_DEBUG_FUNCTRACE,(" x "));
    
    adf_nbuf_peek_header(nbuf, &anbdata, &anblen);
    rxhdr = (atd_rx_hdr_t *)anbdata;

    if (rxhdr->wifi_index >= MAX_NUM_RADIO) {
        adf_os_print(" Invalid Radio  Index %d \n", rxhdr->wifi_index);
        adf_nbuf_free(nbuf);
        return;
    }
   
    /* Protect the entire Receive path  */

    atd_spin_lock(adsc);  /* Lock */

    if (adsc->wifi_list[rxhdr->wifi_index] == NULL) {
        adf_os_print("Invalide radio index %d \n",rxhdr->wifi_index);
        adf_nbuf_free(nbuf);
        return;
    }
    wifi = adsc->wifi_list[rxhdr->wifi_index];
    if (rxhdr->vap_index >= MAX_VAP_SUPPORTED) {
        adf_os_print(" Invalid Vap Index %d \n", rxhdr->vap_index);
        adf_nbuf_free(nbuf);
        return;
    }
    
    vap = atd_host_find_vap(wifi, rxhdr->vap_index);
    if (vap == NULL ) {
        adf_os_print("Vap not found radio index %d vap index %d \n",
                     rxhdr->wifi_index,rxhdr->vap_index);
        adf_nbuf_free(nbuf);
        return;

    }

    adf_nbuf_pull_head(nbuf, sizeof(atd_rx_hdr_t));

    adf_net_indicate_packet(vap->nethandle, nbuf, adf_nbuf_len(nbuf));

    atd_spin_unlock(adsc);    /* Unlock */
}


/*-------------------------- Transmit and Rx path end -----------------------*/

static hif_status_t
atd_target_bootinit(void)
{
  
    a_status_t status;

    atd_trace(ATD_DEBUG_FUNCTRACE,("start"));

    status = fwd_bootfirmware_download();

    atd_trace(ATD_DEBUG_FUNCTRACE,("end"));

    return status;

}

static a_status_t  
atd_configure_target(atd_host_dev_t *dsc)
{
    a_status_t status;

    atd_trace(ATD_DEBUG_FUNCTRACE,("start"));

    status = fwd_firmware_download(dsc->hif_handle);

    atd_trace(ATD_DEBUG_FUNCTRACE,("end"));

    return status;
}


/** 
 * @brief Worker for handling WPS PBC events
 * 
 * @param ctx
 */
static void 
pbc_worker(void *ctx)
{
    int ret ;
    atd_host_dev_t * dsc = (atd_host_dev_t *)ctx;
    a_uint32_t num_vaps, 
               radio_count,
               vap_count,
               temp,
               prefix_len ;

    adf_os_size_t  msgsize;
    atd_host_wifi_t *wifi_list ;
    atd_host_vap_t *vap_list ;

    const char *ws_msgprefix = "PBC_" ;
    char *ws_helperapp = "/sbin/wsupp_helper" ;

    char *envp[] = { 
                    "HOME=/root",
                    "PATH=/bin:/sbin:/usr/sbin:/usr/bin", 
                    NULL} ;
    char **argv ;
	static char *tag = "PUSH-BUTTON.indication";
   
    prefix_len = adf_os_str_len((const char *)ws_msgprefix) ;

    /* count the number of vaps */
    num_vaps=0;
    for(radio_count = 0 ; radio_count < MAX_NUM_RADIO; radio_count++)
    {
        wifi_list = dsc->wifi_list[radio_count] ;
        if(wifi_list)
        {
            for(vap_count = 0 ; vap_count < MAX_VAP_SUPPORTED ; vap_count++)
            {
                vap_list = wifi_list->vap_list[vap_count] ;
                if(vap_list) {
					adf_send_custom_wireless_event(vap_list->nethandle, tag);	
                    num_vaps++;
				}
            }
        }
    }// end for

    /* allocate memory for argument vector */
    temp = sizeof(char *) * (num_vaps + 2) ;
    argv = (char **)adf_os_mem_alloc(NULL, temp);
    adf_os_mem_set(argv, 0 , temp );

    /* Populate argument vector.
     * Allocate memory for each element 
     * and place '<msgprefix> <vap name>' in the vector.
     */
    argv[0] = ws_helperapp ;    /* First arg is app name itself */
    argv[num_vaps+1] = NULL ;   /* NULL terminated arg list */
    temp = 1 ;                  /* Set temp to index of first 
                                 * arg after appname  
                                 */
    for(radio_count = 0 ; radio_count < MAX_NUM_RADIO; radio_count++)
    {
        wifi_list = dsc->wifi_list[radio_count] ;
        if(wifi_list)
        {
            for(vap_count = 0 ; vap_count < MAX_VAP_SUPPORTED ; vap_count++)
            {
                adf_net_handle_t nethdl ;
                vap_list = wifi_list->vap_list[vap_count] ;
                if(vap_list)
                {
                    char *vapname ;

                    nethdl = vap_list->nethandle ;
                    vapname = hdl_to_netdev(nethdl)->name ;
                    msgsize = prefix_len 
                                + adf_os_str_len((const char *)vapname);
                    
                    argv[temp] = (char *)adf_os_mem_alloc(NULL, msgsize);

                    /* copy prefix and vapname to arg vactor */
                    adf_os_mem_copy(argv[temp] , (const void *)ws_msgprefix, 
                                                                prefix_len);

                    adf_os_mem_copy(argv[temp]+prefix_len, 
                                    (const void *)vapname, 
                                    adf_os_str_len((const char *)vapname) );

                    temp++;
                }
            }
        }//end if
    }// end for


    if(num_vaps)
    {
        /* execute wsupp helper */
        ret = call_usermodehelper(ws_helperapp, argv, envp, UMH_WAIT_EXEC) ;
		
        if(ret != 0)
        {
            adf_os_print("Error creating wsupp helper process \n");
        }
		
    }

    /* Free argument vector */
    if(num_vaps)
    {
        for(temp = 1 ; temp <= num_vaps; temp++)
        {
            if(argv[temp])
            {
                adf_os_mem_free(argv[temp]);
                argv[temp] = NULL ;
            }
        }
    }

    adf_os_mem_free(argv);
    
    return ;
}


/** 
  * @brief Called in interrupt context. 
  *        Executes wsupp helper.
  *        
  * @param ctx[in] 
  * 
  * @return 
  */
static a_status_t
atd_wps_cb(void *ctx, adf_wps_cb_args_t *cbargs)
{
    atd_host_dev_t * dsc = NULL;
    atd_host_wifi_t * wifi = (atd_host_wifi_t *)ctx;

    dsc = wifi->dsc ;

    if(cbargs->push_dur > WPS_DEF_PBC_DUR_IGNORE_SECS)
    {
#ifdef WSUPP_HELPER_DEBUG
        adf_os_print("%s: Ignoring PBC as duration more than %ds\n",
                                 __func__, WPS_DEF_PBC_DUR_IGNORE_SECS );
#endif
        return A_STATUS_OK ;
    }

    adf_os_sched_work(NULL, &dsc->pbc_work);
    
    return A_STATUS_OK ;
}

/** 
 * @brief send device state change event to the ACFG clients
 * 
 * @param nethandle  - nethandle of the device to be informed
 * @param prev_state - previous state of the device
 * @param new_state  - new state of the device
 */
void
atd_event_device_state_change(adf_net_handle_t nethandle,
                                acfg_device_state_t prev_state, 
                                acfg_device_state_t new_state)
{
    acfg_os_event_t     event   = {.id = ACFG_EV_DEVICE_STATE_CHANGE};
    acfg_device_state_data_t *device_state_data    = (void *)&event.data;

    atd_trace(ATD_DEBUG_FUNCTRACE, (" .START "));

    device_state_data->prev_state  = adf_os_ntohl(prev_state);
    device_state_data->curr_state  = adf_os_ntohl(new_state);

    adf_net_indicate_event(nethandle, &event);

    atd_trace(ATD_DEBUG_FUNCTRACE, (" .END "));
}


adf_os_virt_module_name(thin_driver);

adf_os_virt_module_init(atd_device_module_init);
adf_os_virt_module_exit(atd_device_module_exit);
adf_os_declare_param(atd_target_devtype , ADF_OS_PARAM_TYPE_INT32);
adf_os_declare_param(atd_tgt_pwrdown_support , ADF_OS_PARAM_TYPE_INT32);

#ifdef ATD_DEBUG_ENABLE
adf_os_declare_param(atd_dbg_mask, ADF_OS_PARAM_TYPE_INT32);
#endif



