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

#ifndef __ATD_INTERNAL_H
#define __ATD_INTERNAL_H

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
#include <adf_os_netlink.h>

#include <htc.h>
#include <wmi.h>
#include <hif_api.h>
#include <htc_api.h>
#include <wmi_api.h>

#include <atd_trc.h>
#include <atd_hdr.h>
#include <atd_cmd.h>
#include <atd_event.h>
#include <atd_wsupp.h>
#include <atd_appbr.h>
#include <atd_byp.h>

struct atd_host_wifi ;
struct atd_host_vap ;
struct byp_sc_s;
struct appbr_sc_s;


typedef struct atd_host_dev {
    a_uint32_t              dev_index ;
    struct atd_host_wifi   *wifi_list[MAX_NUM_RADIO];

    hif_handle_t            hif_handle; /**< HIF Handle */
    htc_handle_t            htc_handle; /**< HTC Handle */
    wmi_handle_t            wmi_handle; /**< WMI handle */
    wsupp_handle_t          wsupp_handle; /**< WSUPP Helper Handle */

    a_uint32_t              target_type;/**< WASP or Hornet */

    htc_endpointid_t        wmi_cmd_ep; /**< WMI Command Endpoint ID */
    htc_endpointid_t        data_ep;    /**< Data Endpoint ID */
    htc_endpointid_t        wsupp_ep;   /**< WSUPP Helper Endpoint ID */
    
    adf_os_spinlock_t       spin_lock;  /**< Big Spinlock */
    adf_os_mutex_t          mutex;      /**< Big Mutex */
    adf_os_atomic_t         ref_cnt;    /**< Device reference count */
    adf_os_sem_t            sem_del;    /**< Delete sync. semaphore */

    adf_os_work_t           htcrdy_work;    /**< HTC ready worker*/
    adf_os_work_t           pbc_work;    /**< WPS PCB worker*/
    /*
     * NOTE: These handles always exist whether or not
     * LANBYPASS is enabled at compile time
     */
    appbr_handle_t          appbr_handle;   /**< App Bridge Module Handle */
    byp_handle_t            byp_handle;     /**< LAN Bypass Module Handle */
    struct atd_host_wifi    *pbc_cb_radio;  /**< PBC callback is registered on this radio */
} atd_host_dev_t;



#define __wifi2wmi(wsc)     wsc->dsc->wmi_handle
#define __wifi2htc(wsc)     wsc->dsc->htc_handle

#define atd_vap2parentnet_handle(_vap)  vap->wifisc->netdev

typedef struct atd_host_wifi {
    atd_host_dev_t       *dsc;

    adf_net_handle_t      netdev;

    struct atd_host_vap  *vap_list[MAX_VAP_SUPPORTED];

    a_uint8_t             wifi_index;
}atd_host_wifi_t;


typedef struct atd_host_vap {
    adf_net_handle_t        nethandle;

    struct atd_host_wifi   *wifisc;
    a_uint8_t               vap_index ;
    a_uint8_t               vap_opmode;

    a_bool_t                lan_en; /** VAP can access LAN */
                                    /** WAN- LAN , WAN - VAP bridges 
                                     * are not supported */
}atd_host_vap_t;


#define WASP_TARGET 1

a_status_t atd_wifi_open(adf_drv_handle_t hdl);
void atd_wifi_close(adf_drv_handle_t hdl);
a_status_t atd_wifi_cmd(adf_drv_handle_t  drv_hdl, adf_net_cmd_t cmd,
             adf_net_cmd_data_t *data);
a_status_t atd_wifi_ioctl(adf_drv_handle_t hdl, a_int32_t num, void *data);
a_status_t atd_wifi_tx(adf_drv_handle_t hdl, adf_nbuf_t pkt);
a_status_t atd_wifi_tx_timeout(adf_drv_handle_t hdl);

a_status_t  atd_vap_open  (adf_drv_handle_t hdl);
void        atd_vap_close (adf_drv_handle_t hdl);
a_status_t  atd_vap_cmd   (adf_drv_handle_t  drv_hdl, adf_net_cmd_t cmd,
                           adf_net_cmd_data_t  *data);
a_status_t  atd_vap_ioctl(adf_drv_handle_t hdl, a_int32_t num, void *data);
a_status_t  atd_vap_tx_timeout(adf_drv_handle_t hdl);
a_status_t  atd_host_tx    (adf_drv_handle_t acore, adf_nbuf_t netbuf );
void        atd_host_rx    (void *, adf_nbuf_t , htc_endpointid_t);


a_status_t atd_cfg_create_vap(adf_drv_handle_t      hdl, 
                              a_uint8_t             icp_name[ACFG_MAX_IFNAME], 
                              acfg_opmode_t         icp_opmode,
                              a_int32_t             icp_vapid,
                              acfg_vapinfo_flags_t  icp_vapflags);


a_status_t fwd_firmware_download(hif_handle_t hif);
a_status_t fwd_bootfirmware_download(void);

#define _atd_wmi_msg_alloc(_wmih) wmi_msg_alloc(_wmih, WMI_DEF_MSG_LEN)



/** 
 * @brief Dev Spin lock
 * 
 * @param dev_sc
 */
static inline void
atd_spin_lock(atd_host_dev_t  *dev_sc)
{
    adf_os_spin_lock(&dev_sc->spin_lock);
}

/** 
 * @brief Dev Spin unlock
 * 
 * @param dev_sc
 */
static inline void
atd_spin_unlock(atd_host_dev_t  *dev_sc)
{
    adf_os_spin_unlock(&dev_sc->spin_lock);
}
/** 
 * @brief Dev Spin lock
 * 
 * @param dev_sc
 */
static inline void
atd_spin_lock_bh(atd_host_dev_t  *dev_sc)
{
    adf_os_spin_lock_bh(&dev_sc->spin_lock);
}

/** 
 * @brief Dev Spin unlock
 * 
 * @param dev_sc
 */
static inline void
atd_spin_unlock_bh(atd_host_dev_t  *dev_sc)
{
    adf_os_spin_unlock_bh(&dev_sc->spin_lock);
}


/** 
 * @brief Dev Mutex lock
 * 
 * @param dev_sc
 */
static inline void
atd_mutex_lock(atd_host_dev_t  *dev_sc)
{
    adf_os_mutex_acquire(NULL, &dev_sc->mutex);
}

/** 
 * @brief Dev Mutex unlock
 * 
 * @param dev_sc
 */
static inline void
atd_mutex_unlock(atd_host_dev_t  *dev_sc)
{
    adf_os_mutex_release(NULL, &dev_sc->mutex);
}

/** 
 * @brief Increment the device reference count & acquire the
 *        delete protection semaphore
 * 
 * @param dev_sc
 */
static inline void
atd_init_ref(atd_host_dev_t      *dev_sc)
{
    adf_os_atomic_init(&dev_sc->ref_cnt);
    adf_os_atomic_inc(&dev_sc->ref_cnt);

    adf_os_sem_init(&dev_sc->sem_del); 
    adf_os_sem_acquire(NULL, &dev_sc->sem_del);
}

/** 
 * @brief wait for the reference to come down to zero
 * 
 * @param dev_sc
 */
static inline void
atd_flush_ref(atd_host_dev_t    *dev_sc)
{
    if (!adf_os_atomic_dec_and_test(&dev_sc->ref_cnt))
        adf_os_sem_acquire(NULL, &dev_sc->sem_del);
    
}
/** 
 * @brief Increment the device reference count
 * 
 * @param dev_sc
 */
static inline void
atd_inc_ref(atd_host_dev_t      *dev_sc)
{
    adf_os_atomic_inc(&dev_sc->ref_cnt);
}

static inline void
atd_dec_ref(atd_host_dev_t      *dev_sc)
{
   if (adf_os_atomic_dec_and_test(&dev_sc->ref_cnt))
      adf_os_sem_release(NULL, &dev_sc->sem_del); 
}
/** 
 * @brief Post a new event in the event Queue
 * 
 * @param dev_sc
 * @param buf
 * 
 */
void
atd_event_run(atd_host_dev_t  *dev_sc, wmi_id_t  id, adf_nbuf_t     buf);

/** 
 * @brief Free up a vap slot from wifi list array
 * 
 * @param wifi
 * @param vap_index
 * 
 * @return 
 */
static inline atd_host_vap_t *
atd_free_vap_index(atd_host_wifi_t    *wifi,  a_uint8_t   vap_index)
{
    atd_host_vap_t      *vap;

    /* Protect the VAP list  */
    atd_spin_lock_bh(wifi->dsc); /* Lock */

    vap = wifi->vap_list[vap_index];

    /* Don't care for the vap_index */
    wifi->vap_list[vap_index] = NULL;   

    atd_spin_unlock_bh(wifi->dsc);  /* Unlock */

    return vap;
}

void
atd_convert_profile(acfg_radio_vap_info_t *atd_profile);


#endif /* __ATD_INTERNAL_H */

