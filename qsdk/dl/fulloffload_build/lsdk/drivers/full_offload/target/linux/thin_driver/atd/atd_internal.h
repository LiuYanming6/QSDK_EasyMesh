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

#include <linux/version.h>

#include <atd_trc.h>
#include <atd_wsupp.h>
#include <atd_appbr.h>
#include <atd_byp.h>

#define adf_os_print printk
#define adf_os_function __func__


#define ATD_TGT_MAX_RADIO       2
#define MAC_ADDR_STRING         "%02x:%02x:%02x:%02x:%02x:%02x"

#define vzalloc(size, ptr) \
    do {\
        ptr = vmalloc(size);\
        if(ptr)\
        memset(ptr, 0, size);\
    }while(0);

struct atd_tgt_wifi;
struct atd_tgt_vap;
/** 
 * @brief Target Device softc
 */
typedef struct atd_tgt_dev {
    htc_handle_t                htc_handle; /**< HTC Handle */
    hif_handle_t                hif_handle; /**< HIF Handle */
    wmi_handle_t                wmi_handle; /**< WMI Handle */
    wsupp_handle_t              wsupp_handle;  /**< WSUPP Helper Handle */

    a_uint32_t                  wifi_cnt;   /**< number of radios */
    a_uint32_t                  wifiphy_max_cnt;/**< XXX: what is this */    

    htc_endpointid_t            data_ep;    /**< Data endpoint */

    a_uint8_t                   dont_send_create_event;

    struct atd_tgt_wifi        *wifi_list[ATD_TGT_MAX_RADIO];

    appbr_handle_t              appbr_handle;   /**< Application Bridge Module Handle */
    byp_handle_t                byp_handle;     /**< LAN Bypass Module Handle */
    a_int32_t                   wifi_unregister; /**< Flag to indicate wifi up or down */ 
}atd_tgt_dev_t;

/** 
 * @brief Wifi Softc
 */
typedef struct atd_tgt_wifi {
    struct net_device          *netdev;         /**< Netdevice handle */  

    a_uint8_t                   wifi_index;     /**< Wifi index */
    a_uint8_t                   vap_cnt;        /**< VAP Count */

    htc_endpointid_t            rx_endpoint;    /**< RX Endpoint */

    a_uint8_t                   mac_addr[ACFG_MACADDR_LEN]; /**< MAC Address */

    struct atd_tgt_vap         *vap_list[MAX_VAP_SUPPORTED];
}atd_tgt_wifi_t;

/** 
 * @brief VAP Softc
 */
typedef struct atd_tgt_vap {
    struct net_device          *vap_netdev; /**< VAP's netdevice handle */

    a_uint8_t                   vap_index;  /**< VAP Index number */

    a_uint8_t                   mac_addr[ACFG_MACADDR_LEN]; /**< MAC Address */

    struct atd_tgt_wifi  *wifisc;     /**< Back pointer to Wifi */

    byp_vapctrl_flg_t           byp_flags;  /**< LAN Bypass Mode related Flags */

    a_uint16_t                  vlan_id;

}atd_tgt_vap_t;


  

void   atd_tgt_wmi_init(atd_tgt_dev_t * atsc);
void   atd_tgt_wmi_set_stopflag(wmi_handle_t wmi_handle, a_uint8_t val);


#ifdef ATD_TGT_DUMP_DATA


static inline void
atd_print_mac_addr(const a_uint8_t *mac, const a_uint8_t  *str)
{
    printk("%s"MAC_ADDR_STRING"\n", str, mac[0], mac[1], mac[2], 
           mac[3], mac[4], mac[5]);
}

static inline void
atd_dump_ether_header(struct sk_buff  *skb)
{
    struct ethhdr   *eh = (struct ethhdr  *)skb->data;

    atd_print_mac_addr(eh->h_source, "source:");
    atd_print_mac_addr(eh->h_dest, "dest:");

    printk("type %x:skb_len %d\n",ntohs(eh->h_proto), skb->len);
}

#else

#define atd_print_mac_addr(x, str)  (0)

#define atd_dump_ether_header(buf)  (0)

#endif

/**
 * @brief Assert
 */
#define atd_assert(expr)  do {                              \
    if(unlikely(!(expr))) {                                 \
        printk(KERN_ERR "Assertion failed! %s:%s %s:%d\n",  \
              #expr, __FUNCTION__, __FILE__, __LINE__);     \
        dump_stack();                                       \
        panic("Take care of the assert first\n");           \
    }                                                       \
}while(0)

/**
 * @brief Warning
 */
#define atd_warn(cond) ({                                   \
    int __ret_warn = !!(cond);                              \
    if (unlikely(__ret_warn)) {                             \
        printk("WARNING: at %s:%d %s()\n", __FILE__,        \
            __LINE__, __FUNCTION__);                        \
        dump_stack();                                       \
    }                                                       \
    unlikely(__ret_warn);                                   \
})


/**
 * Configuration 
 */

/** 
 * @brief Convert a A_STATUS_X to a error number
 * 
 * @param status    (A_STATUS_XXX)
 * 
 * @return          (error number)
 */
static inline int 
status_to_errno(a_status_t status)
{
    switch(status) {
        case A_STATUS_OK:            return 0;
        case A_STATUS_ENOENT:        return ENOENT;
        case A_STATUS_ENOMEM:        return ENOMEM;
        case A_STATUS_EINVAL:        return EINVAL;
        case A_STATUS_EINPROGRESS:   return EINPROGRESS;
        case A_STATUS_EBUSY:         return EBUSY;
        case A_STATUS_ENOTSUPP:      return ENOTSUPP;
        case A_STATUS_EIO:           return EIO;
        case A_STATUS_FAILED:        return ENOTSUPP;
        default:                     return ENOTSUPP;
    }
}

/** 
 * @brief Convert a Error number to A_STATUS_XXX
 * 
 * @param errno     (Error number)
 * 
 * @return          (A_STATUS_XXX)
 */
static inline a_status_t
errno_to_status(int errno)
{
    switch (errno)  {
        case -ENOENT:        return A_STATUS_ENOENT;
        case -ENOMEM:        return A_STATUS_ENOMEM;
        case -EINVAL:        return A_STATUS_EINVAL;
        case -EINPROGRESS:   return A_STATUS_EINPROGRESS;
        case -EBUSY:         return A_STATUS_EBUSY;
        case -E2BIG:         return A_STATUS_E2BIG;
        case -EAGAIN:        return A_STATUS_EAGAIN;
        case -ENXIO:         return A_STATUS_ENXIO;
        case -EFAULT:        return A_STATUS_EFAULT;
        case -EIO:           return A_STATUS_EIO;
        case -EEXIST:        return A_STATUS_EEXIST;
        case -ENETDOWN:      return A_STATUS_ENETDOWN;
        case -EADDRNOTAVAIL: return A_STATUS_EADDRNOTAVAIL;
        case -ENETRESET:     return A_STATUS_ENETRESET;
        case -EOPNOTSUPP:    return A_STATUS_ENOTSUPP;
        case -ENOSPC:        return A_STATUS_ENOSPC;
        default:            return A_STATUS_OK;
    }
}

/** 
 * @brief Get netdevice by name
 * 
 * @param ifname
 * 
 * @return 
 */
static inline struct net_device *
atd_netdev_get_by_name(const a_uint8_t    *ifname)
{
    return dev_get_by_name(&init_net, (const char *)ifname);
}

/** 
 * @brief Send a Netdevice IOCTL
 * 
 * @param dev
 * @param ifr
 * @param cmd
 * 
 * @return 
 */
static inline a_status_t 
netdev_ioctl(struct net_device *dev, struct ifreq   *ifr, int cmd)
{
    int err = ENOTSUPP;
    
    rtnl_lock();

	if (dev->netdev_ops->ndo_do_ioctl)
		 err = dev->netdev_ops->ndo_do_ioctl(dev, ifr, cmd);

    rtnl_unlock();

    return errno_to_status(err);
}

/** 
 * @brief Send a Netdevice set mac address
 * 
 * @param dev
 * @param ifr
 * @param cmd
 * 
 * @return 
 */
static inline a_status_t 
netdev_set_macaddr(struct net_device *dev, void *mac)
{
    int err = ENOTSUPP;
    
    rtnl_lock();

	if (dev->netdev_ops->ndo_set_mac_address)
		 err = dev->netdev_ops->ndo_set_mac_address(dev, mac);

    rtnl_unlock();

    return errno_to_status(err);
}

/** 
 * @brief Fetch a IW handler for the specified command
 * 
 * @param dev
 * @param cmd
 * 
 * @return   (iw_handler or NULL)
 */
static inline iw_handler 
get_wext_handler(struct net_device *dev, unsigned int cmd)
{
    a_uint8_t   idx;

    if (dev->wireless_handlers == NULL)
        return NULL;

    idx = cmd - SIOCIWFIRST;      /* Standard */
    if (idx < dev->wireless_handlers->num_standard)
        return dev->wireless_handlers->standard[idx];

    idx = cmd - SIOCIWFIRSTPRIV;  /* Private */
    if (idx < dev->wireless_handlers->num_private)
        return dev->wireless_handlers->private[idx];

    return NULL;
}

/** 
 * @brief Send a Wireless Extension call to netdevice
 * 
 * @param dev
 * @param cmd   (Command standard or private)
 * @param info  
 * @param wrqu
 * @param data
 * 
 * @return 
 */
static inline a_status_t
netdev_wext_handler(struct net_device *dev, int cmd, 
                    struct iw_request_info  *info, union  iwreq_data  *wrqu, 
                    char  *data)
{
    iw_handler  handler = NULL;
    int         err     = EINVAL;

    handler = get_wext_handler(dev, cmd);

    if(handler) err = handler(dev, info, wrqu, data);

    return errno_to_status(err);

}

static inline struct iw_statistics * 
get_wireless_stats(struct net_device *dev)
{
    if (dev->wireless_handlers == NULL)
        return NULL;

	return dev->wireless_handlers->get_wireless_stats(dev);
}


/**
 * Data Routing 
 */

/** 
 * @brief Indicate packet to wlan
 * 
 * @param skb
 * @param dev
 */
static inline void
atd_tx_to_wlan(struct sk_buff  *skb, struct net_device  *dev)
{
    skb->dev = dev;
    dev_queue_xmit(skb);
}

/** 
 * @brief Target receive handler
 * 
 * @param skb
 */
void   
atd_rx_from_wlan(struct sk_buff *     skb);

void
atd_proxy_arp_send(struct sk_buff *skb);

/** 
 * @brief Find the VAP given netdevice handle
 * 
 * @param sc    (Global Target Softc)
 * @param dev   (Netdevice handle)
 * 
 * @return      (Vap Softc pointer or NULL)
 */
atd_tgt_vap_t *   
atd_tgt_find_vap(atd_tgt_dev_t *sc, struct net_device *dev);

/** 
 * @brief Find the Wifi given net_device handle
 * 
 * @param sc    (Global Target sc)
 * @param dev   (Netdevice handle)
 * 
 * @return      (Wifi Softc pointer or NULL) 
 */
atd_tgt_wifi_t *   
atd_tgt_find_wifi(atd_tgt_dev_t *sc, struct net_device *dev);

/**
 * @brief Send a WIFI Event to the host
 *
 * @param wifi   (Which WIFI will consume the event)
 * @param event (WMI_EVENT_*)
 */
void
atd_tgt_wifi_event(atd_tgt_wifi_t *wifi, wmi_id_t event);

/**
 * @brief Send a WIFI Restore Event to the host
 *
 * @param wifi   (Which WIFI will consume the event)
 * @param event (WMI_EVENT_*)
 */
void
atd_tgt_restore_wifi(atd_tgt_wifi_t *wifi, wmi_id_t event);

/** 
 * @brief Send a VAP Event to the host
 * 
 * @param vap   (Which VAP will consume the event)
 * @param event (WMI_EVENT_*)
 */
void
atd_tgt_vap_event(atd_tgt_vap_t      *vap, wmi_id_t  event);

void
atd_tgt_create_vap(atd_tgt_vap_t      *vap, wmi_id_t  event);

/** 
 * @brief Remove all references of the VAP & Netdevice it binds itself
 * 
 * @param vap
 */
void
atd_tgt_vap_unref(atd_tgt_vap_t     *vap);

a_status_t 
atd_tgt_set_vap_index (atd_tgt_wifi_t * wifisc, atd_tgt_vap_t * vap);

/**
 * Newma Specific values
 */


/*
  Some of the macro hardcoded for now for basic bringup 
  to be removed and the macro refernce to be taken from newma header file 
  by including them in atd. Or else the required 
  values has to be passed from acfg/host.
  */
#define SIOC80211IFCREATE       (SIOCDEVPRIVATE+7)
#define SIOC80211IFDESTROY      (SIOCDEVPRIVATE+8)
#define IEEE80211_IOCTL_GETMODE     (SIOCIWFIRSTPRIV+17)
#define IEEE80211_IOCTL_SETMODE     (SIOCIWFIRSTPRIV+18)
#define IEEE80211_IOCTL_SETPARAM    (SIOCIWFIRSTPRIV+0)
#define IEEE80211_IOCTL_GETPARAM    (SIOCIWFIRSTPRIV+1)
#define IEEE80211_IOCTL_GETWMMPARAMS    (SIOCIWFIRSTPRIV+5)
#define IEEE80211_IOCTL_CHN_WIDTHSWITCH (SIOCIWFIRSTPRIV+28) 

#define SIOCIOCTLTX99               (SIOCDEVPRIVATE+13)
#define SIOCIOCTLNAWDS               (SIOCDEVPRIVATE+12)

#define ATH_HAL_IOCTL_SETPARAM       (SIOCIWFIRSTPRIV+0)
#define ATH_HAL_IOCTL_GETPARAM       (SIOCIWFIRSTPRIV+1)

#define ATH_IOCTL_SETCOUNTRY             (SIOCIWFIRSTPRIV+2) 

#define SIOCDEVVENDOR                   (SIOCDEVPRIVATE+15)    /* Used for ATH_SUPPORT_LINUX_VENDOR */

          


