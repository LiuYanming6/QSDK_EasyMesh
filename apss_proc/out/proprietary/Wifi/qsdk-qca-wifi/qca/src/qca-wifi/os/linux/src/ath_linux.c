/*
 * Copyright (c) 2013, 2017-2019 Qualcomm Innovation Center, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Innovation Center, Inc.
 */
/*
 * Copyright (c) 2010, Atheros Communications Inc.
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
 * 2013 Qualcomm Atheros, Inc.
 * All Rights Reserved.
 * Qualcomm Atheros Confidential and Proprietary.
 */

#include "qdf_dev.h"
#include "qdf_vfs.h"

#include <osdep.h>
#include <wbuf.h>

#include "if_media.h"
#include "ieee80211_var.h"
#include "ieee80211_aponly.h"
#include "if_athvar.h"
#include "ath_cwm.h"
#include "osif_private.h"
#include "if_athproto.h"

#ifdef ATH_BAND_STEERING
#include "ath_band_steering.h"
#endif

#ifdef ATH_PARAMETER_API
#include "ath_papi.h"
#endif

#ifdef ATH_TX99_DIAG
#include "ath_tx99.h"
#endif

#include "ath_netlink.h"

#include "ald_netlink.h"

#if ATH_SUPPORT_FLOWMAC_MODULE
#include <flowmac_api.h>
#endif

#if UMAC_SUPPORT_ACFG
#include <ieee80211_ioctl_acfg.h>
#include <acfg_api_types.h>
#include <acfg_drv_event.h>
#endif

#include "ath_ucfg.h"
/*
 * Maximum acceptable MTU
 * MAXFRAMEBODY - WEP - QOS - RSN/WPA:
 * 2312 - 8 - 2 - 12 = 2290
 */
#define ATH_MAX_MTU     2290
#define ATH_MIN_MTU     32

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,1,0)
#define IRQF_DISABLED 0x00000020
#endif

/* Temperature update support */
#define MIN_TEMPERATURE -255
#define ROOM_TEMPERATURE 25

#if defined(ATH_TX_BUF_FLOW_CNTL) ||  defined(ATH_DEBUG)
extern int ACBKMinfree;
extern int ACBEMinfree;
extern int ACVIMinfree;
extern int ACVOMinfree;
extern int CABMinfree;
extern int UAPSDMinfree;
#endif

static int ath_config_thermal_mitigation_param (struct ath_softc *sc);
extern struct ath_softc_net80211 *global_scn[10];
extern int num_global_scn;
int osif_vap_hardstart(struct sk_buff *skb, struct net_device *dev);
extern void osif_hal_log_ani_callback_register(void *ani_func);
#ifdef HOST_OFFLOAD
extern u_int32_t hif_rom_mode(void);
#endif

static struct ath_reg_parm ath_params = {
    .wModeSelect = MODE_SELECT_ALL,
    .NetBand = MODE_SELECT_ALL,
    .txAggrEnable = 1,
    .rxAggrEnable = 1,
    .txAmsduEnable = 1,
    .aggrLimit = IEEE80211_AMPDU_LIMIT_DEFAULT,
    .aggrSubframes = IEEE80211_AMPDU_SUBFRAME_DEFAULT,
    .aggrProtDuration = 8192,
    .aggrProtMax = 8192,
    .txRifsEnable = 0,
    .rifsAggrDiv = IEEE80211_RIFS_AGGR_DIV,
    .txChainMaskLegacy = 1,
    .rxChainMaskLegacy = 1,
    .rxChainDetectThreshA = 35,
    .rxChainDetectThreshG = 35,
    .rxChainDetectDeltaA = 30,
    .rxChainDetectDeltaG = 30,
    .calibrationTime = 30,
#if ATH_SUPPORT_LED
    .gpioPinFuncs = {GPIO_PIN_FUNC_0,GPIO_PIN_FUNC_1,GPIO_PIN_FUNC_2},
    .gpioLedCustom = ATH_LED_CUSTOMER,
#else
    .gpioPinFuncs = {1, 7, 7},
#endif
    .hwTxRetries = 4,
    .extendedChanMode = 1,
    .DmaStopWaitTime = 4,
    .swBeaconProcess = 1,
    .stbcEnable = 1,
    .ldpcEnable = 1,
    .cwmEnable = 1,
    .wpsButtonGpio = 0,
#ifdef ATH_SUPPORT_TxBF
    .TxBFSwCvTimeout = 1000 ,
#endif
#if ATH_SUPPORT_SPECTRAL
	.spectralEnable = 1,
#endif
#if ATH_SUPPORT_PAPRD
    .paprdEnable = 1,
#endif
#if ATH_TX_BUF_FLOW_CNTL
    .ACBKMinfree = 48,
    .ACBEMinfree = 32,
    .ACVIMinfree = 16,
    .ACVOMinfree = 0,
    .CABMinfree = 48,
    .UAPSDMinfree = 0,
#endif
#ifdef ATH_SWRETRY
    .numSwRetries = 2,
#endif
#if ATH_SUPPORT_FLOWMAC_MODULE
    .osnetif_flowcntrl = 0,
    .os_ethflowmac_enable = 0,
#endif
#if ATH_SUPPORT_AGGR_BURST
    .burstEnable = 0,
    .burstDur = 5100,
#endif
};

static struct ieee80211_reg_parameters wlan_reg_params = {
    .transmitRetrySet = 0x04040404,
    .sleepTimePwrSave = 100,         /* wake up every beacon */
    .sleepTimePwrSaveMax = 1000,     /* wake up every 10 th beacon */
    .sleepTimePerf=100,              /* station wakes after this many mS in max performance mode */
    .inactivityTimePwrSaveMax=400,   /* in max PS mode, how long (in mS) w/o Tx/Rx before going back to sleep */
    .inactivityTimePwrSave=200,      /* in normal PS mode, how long (in mS) w/o Tx/Rx before going back to sleep */
    .inactivityTimePerf=400,         /* in max perf mode, how long (in mS) w/o Tx/Rx before going back to sleep */
    .psPollEnabled=0,                /* Use PS-POLL to retrieve data frames after TIM is received */
    .wmeEnabled    = 1,
    .enable2GHzHt40Cap = 1,
    .cwmEnable = 1,
    .cwmExtBusyThreshold = IEEE80211_CWM_EXTCH_BUSY_THRESHOLD,
    .ignore11dBeacon = 1,
    .p2pGoUapsdEnable = 1,
    .extapUapsdEnable = 1,
    .shortPreamble = 1,               /*ShortPreamble is enabled default */
#ifdef ATH_SUPPORT_TxBF
    .autocvupdate = 0,
#define DEFAULT_PER_FOR_CVUPDATE 30
    .cvupdateper = DEFAULT_PER_FOR_CVUPDATE,
#endif
};

/* The code below is used to register a hw_caps file in sysfs */
static ssize_t wifi_hwcaps_show(struct device *dev,
                               struct device_attribute *attr, char *buf)
{
    struct net_device *net = to_net_dev(dev);
    struct ath_softc_net80211 *scn = ath_netdev_priv(net);
    struct ieee80211com *ic = &scn->sc_ic;

    u_int32_t hw_caps = ic->ic_modecaps;

    strlcpy(buf, "802.11", strlen("802.11") + 1);
    if(hw_caps &
        1 << IEEE80211_MODE_11A)
        strlcat(buf, "a", strlen("a") + strlen(buf) + 1);
    if(hw_caps &
        1 << IEEE80211_MODE_11B)
        strlcat(buf, "b", strlen("b") + strlen(buf) + 1);
    if(hw_caps &
        1 << IEEE80211_MODE_11G)
        strlcat(buf, "g", strlen("g") + strlen(buf) + 1);
    if(hw_caps &
        (1 << IEEE80211_MODE_11NA_HT20 |
         1 << IEEE80211_MODE_11NG_HT20 |
         1 << IEEE80211_MODE_11NA_HT40PLUS |
         1 << IEEE80211_MODE_11NA_HT40MINUS |
         1 << IEEE80211_MODE_11NG_HT40PLUS |
         1 << IEEE80211_MODE_11NG_HT40MINUS |
         1 << IEEE80211_MODE_11NG_HT40 |
         1 << IEEE80211_MODE_11NA_HT40))
        strlcat(buf, "n", strlen("n") + strlen(buf) + 1);
    if(hw_caps &
        (1 << IEEE80211_MODE_11AC_VHT20 |
         1 << IEEE80211_MODE_11AC_VHT40PLUS |
         1 << IEEE80211_MODE_11AC_VHT40MINUS |
         1 << IEEE80211_MODE_11AC_VHT40 |
         1 << IEEE80211_MODE_11AC_VHT80 |
         1 << IEEE80211_MODE_11AC_VHT160 |
         1 << IEEE80211_MODE_11AC_VHT80_80))
        strlcat(buf, "/ac", strlen("/ac") + strlen(buf) + 1);
    return strlen(buf);
}
static DEVICE_ATTR(hwcaps, S_IRUGO, wifi_hwcaps_show, NULL);

/* Handler for sysfs entry hwmodes - returns all the hwmodes supported by the radio */
static ssize_t wifi_hwmodes_show(struct device *dev,
                               struct device_attribute *attr, char *buf)
{
    struct ath_softc_net80211 *scn = NULL;
    struct ieee80211com *ic = NULL;
    u_int32_t hw_caps = 0;
    struct net_device *net = to_net_dev(dev);

    scn = ath_netdev_priv(net);
    if(!scn){
       return 0;
    }

    ic = &scn->sc_ic;
    if(!ic){
        return 0;
    }

    hw_caps = ic->ic_modecaps;

    if(hw_caps &
        1 << IEEE80211_MODE_11A)
        strlcat(buf, "11A ", strlen("11A") + strlen(buf) + 1);
    if(hw_caps &
        1 << IEEE80211_MODE_11B)
        strlcat(buf, "11B ", strlen("11B") + strlen(buf) + 1);
    if(hw_caps &
        1 << IEEE80211_MODE_11G)
        strlcat(buf, "11G ", strlen("11G") + strlen(buf) + 1);
    if(hw_caps &
        1 << IEEE80211_MODE_FH)
        strlcat(buf, "FH ", strlen("FH") + strlen(buf) + 1);
    if(hw_caps &
        1 << IEEE80211_MODE_TURBO_A)
        strlcat(buf, "TURBO_A ", strlen("TURBO_A") + strlen(buf) + 1);
    if(hw_caps &
        1 << IEEE80211_MODE_TURBO_G)
        strlcat(buf, "TURBO_G ", strlen("TURBO_G") + strlen(buf) + 1);
    if(hw_caps &
        1 << IEEE80211_MODE_11NA_HT20)
        strlcat(buf, "11NA_HT20 ", strlen("11NA_HT20") + strlen(buf) + 1);
    if(hw_caps &
        1 << IEEE80211_MODE_11NG_HT20)
        strlcat(buf, "11NG_HT20 ", strlen("11NG_HT20") + strlen(buf) + 1);
    if(hw_caps &
        1 << IEEE80211_MODE_11NA_HT40PLUS)
        strlcat(buf, "11NA_HT40PLUS ", strlen("11NA_HT40PLUS") + strlen(buf) + 1);
    if(hw_caps &
        1 << IEEE80211_MODE_11NA_HT40MINUS)
        strlcat(buf, "11NA_HT40MINUS ", strlen("11NA_HT40MINUS") + strlen(buf) + 1);
    if(hw_caps &
        1 << IEEE80211_MODE_11NG_HT40PLUS)
        strlcat(buf, "11NG_HT40PLUS ", strlen("11NG_HT40PLUS") + strlen(buf) + 1);
    if(hw_caps &
        1 << IEEE80211_MODE_11NG_HT40MINUS)
        strlcat(buf, "11NG_HT40MINUS ", strlen("11NG_HT40MINUS") + strlen(buf) + 1);
    if(hw_caps &
        1 << IEEE80211_MODE_11NG_HT40)
        strlcat(buf, "11NG_HT40 ", strlen("11NG_HT40") + strlen(buf) + 1);
    if(hw_caps &
        1 << IEEE80211_MODE_11NA_HT40)
        strlcat(buf, "11NA_HT40 ", strlen("11NA_HT40") + strlen(buf) + 1);

    return strlen(buf);
}
static DEVICE_ATTR(hwmodes, S_IRUGO, wifi_hwmodes_show, NULL);

/*Handler for sysfs entry 2g_maxchwidth - returns the maximum channel width supported by the device in 2.4GHz*/
static ssize_t wifi_2g_maxchwidth_show(struct device *dev,
                               struct device_attribute *attr, char *buf)
{
    struct ath_softc_net80211 *scn = NULL;
    struct ieee80211com *ic = NULL;
    u_int32_t hw_caps = 0;
    struct net_device *net = to_net_dev(dev);

    scn = ath_netdev_priv(net);
    if(!scn){
       return 0;
    }

    ic = &scn->sc_ic;
    if(!ic){
        return 0;
    }

    hw_caps = ic->ic_modecaps;

    if (hw_caps &
        (1 << IEEE80211_MODE_11NG_HT40MINUS |
         1 << IEEE80211_MODE_11NG_HT40PLUS |
	 1 << IEEE80211_MODE_11NG_HT40)){
        strlcpy(buf, "40", strlen("40") + 1);
    }
    else if (hw_caps & (1 << IEEE80211_MODE_11NG_HT20 )){
        strlcpy(buf, "20", strlen("20") + 1);
    }

    /* NOTE: Only >=n chipsets are considered for now since productization will
     * involve only such chipsets. In the unlikely case where lower chipsets/crimped
     * phymodes are to be handled, it is a separate TODO and relevant enums need
     * to be considered.*/

    return strlen(buf);
}

static DEVICE_ATTR(2g_maxchwidth, S_IRUGO, wifi_2g_maxchwidth_show, NULL);

/*Handler for sysfs entry 5g_maxchwidth - returns the maximum channel width supported by the device in 5GHz*/
static ssize_t wifi_5g_maxchwidth_show(struct device *dev,
                               struct device_attribute *attr, char *buf)
{
    struct ath_softc_net80211 *scn = NULL;
    struct ieee80211com *ic = NULL;
    u_int32_t hw_caps = 0;
    struct net_device *net = to_net_dev(dev);

    scn = ath_netdev_priv(net);
    if(!scn){
       return 0;
    }

    ic = &scn->sc_ic;
    if(!ic){
        return 0;
    }

    hw_caps = ic->ic_modecaps;

    if (hw_caps &
        (1 <<  IEEE80211_MODE_11NA_HT40 |
         1 <<  IEEE80211_MODE_11NA_HT40MINUS |
         1 <<  IEEE80211_MODE_11NA_HT40PLUS)){
	strlcpy(buf, "40", strlen("40") + 1);
    }
    else if (hw_caps &
        (1 << IEEE80211_MODE_11NA_HT20)){
        strlcpy(buf, "20", strlen("20") + 1);
    }

    /* NOTE: Only >=n chipsets are considered for now since productization will
     * involve only such chipsets. In the unlikely case where lower chipsets/crimped
     * phymodes are to be handled, it is a separate TODO and relevant enums need
     * to be considered.*/

    return strlen(buf);
}

static DEVICE_ATTR(5g_maxchwidth, S_IRUGO, wifi_5g_maxchwidth_show, NULL);

static struct attribute *wifi_device_attrs[] = {
    &dev_attr_hwcaps.attr,
    &dev_attr_hwmodes.attr,       /*sysfs entry for displaying all the hwmodes supported by the radio*/
    &dev_attr_5g_maxchwidth.attr, /*sysfs entry for displaying the max channel width supported in 5Ghz*/
    &dev_attr_2g_maxchwidth.attr, /*sysfs entry for displaying the max channel width supported in 2.4Ghz*/

    NULL
};

static struct attribute_group wifi_attr_group = {
       .attrs  = wifi_device_attrs,
};

/*
 * Definition of methods and sysfs attributes needed to control
 * thermal level of wifi device.
 */

ssize_t wifi_thermal_temp_show (struct device *dev,
        struct device_attribute *attr, char *buf)
{
    struct net_device *net = to_net_dev(dev);
    struct ath_softc_net80211 *scn = ath_netdev_priv(net);
    struct ath_softc *sc  =  ATH_DEV_TO_SC(scn->sc_dev);
    uint32_t sensor_tempval = 0;
    int len = -1;

    if (!sc) {
        return -EINVAL;
    }
    sensor_tempval = sc->thermal_param.th_stats.temp;
    len = snprintf(buf, (ssize_t)PAGE_SIZE, "%d\n", sensor_tempval);
    TH_DEBUG_PRINT(TH_DEBUG_LVL2, sc,"%s: sensor_val = %d, len = %d\n", __func__, sensor_tempval, len);
    return len;
}


ssize_t wifi_thermal_mode_store (struct device *dev,
        struct device_attribute *attr, const char *buf, size_t size)
{
    struct net_device *net = to_net_dev(dev);
    struct ath_softc_net80211 *scn = ath_netdev_priv(net);
    struct ath_softc *sc  =  ATH_DEV_TO_SC(scn->sc_dev);
    uint32_t mode = INVALID_THERMAL_PARAM;
    int len = -1;
    char cbuf[12] = {'\0',};
    char *enabled = "enabled";
    char *disabled = "disabled";

    if (!sc || (size > 10)) {
        return -EINVAL;
    }
    memcpy(cbuf, buf, size);
    cbuf[size] = '\0';

    if ((strlen(enabled) == size -1) && (!strncmp(cbuf, enabled, size - 1))) {
        mode = THERMAL_MITIGATION_ENABLED;
    } else if ((strlen(disabled) == size -1) && (!strncmp(cbuf, disabled, size - 1))) {
        mode = THERMAL_MITIGATION_DISABLED;
    }
    if (mode != INVALID_THERMAL_PARAM) {
        if (mode != sc->thermal_param.th_cfg.enable) {
            sc->thermal_param.th_cfg.enable = mode;
        }
        len = size;
    } else {
        len = -EINVAL;
    }
    TH_DEBUG_PRINT(TH_DEBUG_LVL2, sc,"%s: cbuf = %s, mode=(0x%x), len = %d, zize = %zd\n", __func__, cbuf, mode, len, size);
    return len;
}

ssize_t wifi_thermal_mode_show (struct device *dev,
        struct device_attribute *attr, char *buf)
{
    struct net_device *net = to_net_dev(dev);
    struct ath_softc_net80211 *scn = ath_netdev_priv(net);
    struct ath_softc *sc  =  ATH_DEV_TO_SC(scn->sc_dev);
    uint32_t enable = 0;
    int len = -1;
    char *enabled = "enabled";
    char *disabled = "disabled";

    if (!sc) {
        return -EINVAL;
    }
    enable = sc->thermal_param.th_cfg.enable;
    len = snprintf(buf, (ssize_t)PAGE_SIZE, "%s\n", (enable ? enabled : disabled));
    TH_DEBUG_PRINT(TH_DEBUG_LVL2, sc,"%s: mode = %s, len = %d\n", __func__, (enable ? enabled : disabled), len);
    return len;
}


static void da_thermal_tmd_config (struct ath_softc *sc)
{
    uint8_t change = TH_FALSE;

    if (sc->thermal_param.tmd_cfg.cfg_ready == TMD_CONFIG_READY) {
        if (sc->thermal_param.tmd_cfg.dcoffpercent !=
                sc->thermal_param.th_cfg.levelconf[sc->thermal_param.tmd_cfg.level].dcoffpercent) {
            sc->thermal_param.th_cfg.levelconf[sc->thermal_param.tmd_cfg.level].dcoffpercent =
                sc->thermal_param.tmd_cfg.dcoffpercent;
            change = TH_TRUE;
        }
        sc->thermal_param.tmd_cfg.cfg_ready = TH_FALSE;
    }
}

ssize_t wifi_thermal_dutycycle_store (struct device *dev,
        struct device_attribute *attr, const char *buf, size_t size)
{
    struct net_device *net = to_net_dev(dev);
    struct ath_softc_net80211 *scn = ath_netdev_priv(net);
    struct ath_softc *sc  =  ATH_DEV_TO_SC(scn->sc_dev);
    uint32_t dc = INVALID_THERMAL_PARAM;
    int len = -1;
    char cbuf[5] = {'\0',};

    if (!sc || (size > 4)) {
        return -EINVAL;
    }
    memcpy(cbuf, buf, size);
    cbuf[size] = '\0';

    if (sscanf(cbuf, "%d", &dc) > 0) {
        if (dc >= THERMAL_MIN_DUTY_CYCLE && dc <= THERMAL_MAX_DUTY_CYCLE) {
            if (sc->thermal_param.th_cfg.dc != dc) {
                sc->thermal_param.th_cfg.dc = dc;
            }
            len = size;
        }
    } else {
        len = -EINVAL;
    }
    TH_DEBUG_PRINT(TH_DEBUG_LVL2, sc, "%s: dc: %d, len: %d\n", __func__, dc, len);
    return len;
}


ssize_t wifi_thermal_dutycycle_show (struct device *dev,
        struct device_attribute *attr, char *buf)
{
    struct net_device *net = to_net_dev(dev);
    struct ath_softc_net80211 *scn = ath_netdev_priv(net);
    struct ath_softc *sc  =  ATH_DEV_TO_SC(scn->sc_dev);
    uint32_t dc = 0;
    int len = -1;

    if (!sc) {
        return -EINVAL;
    }
    dc = sc->thermal_param.th_cfg.dc;
    len = snprintf(buf, (ssize_t)PAGE_SIZE, "%d\n", dc);
    TH_DEBUG_PRINT(TH_DEBUG_LVL2, sc,"%s: dc = %d, len = %d\n", __func__, dc, len);
    return len;
}


ssize_t wifi_thermal_thlvl_store (struct device *dev,
        struct device_attribute *attr, const char *buf, size_t size)
{
    struct net_device *net = to_net_dev(dev);
    struct ath_softc_net80211 *scn = ath_netdev_priv(net);
    struct ath_softc *sc  =  ATH_DEV_TO_SC(scn->sc_dev);
    uint32_t thlvl = INVALID_THERMAL_PARAM;
    int len = -1;
    char cbuf[5] = {'\0',};

    if (!sc || (size > 4)) {
        return -EINVAL;
    }
    memcpy(cbuf, buf, size);
    cbuf[size] = '\0';

    if (sscanf(cbuf, "%d", &thlvl) > 0) {
        if (thlvl >= 0 && thlvl < THERMAL_LEVELS) {
            sc->thermal_param.tmd_cfg.level = thlvl;
            sc->thermal_param.tmd_cfg.cfg_ready |= TMD_CONFIG_LEVEL;
            da_thermal_tmd_config(sc);
            len = size;
        } else {
            len = -EINVAL;
        }
    } else {
        len = -EINVAL;
    }

    TH_DEBUG_PRINT(TH_DEBUG_LVL2, sc,"%s: thlvl = %d, len = %d\n", __func__, thlvl, len);
    return len;
}


ssize_t wifi_thermal_thlvl_show (struct device *dev,
        struct device_attribute *attr, char *buf)
{
    struct net_device *net = to_net_dev(dev);
    struct ath_softc_net80211 *scn = ath_netdev_priv(net);
    struct ath_softc *sc  =  ATH_DEV_TO_SC(scn->sc_dev);
    uint32_t lvl = 0;
    int len = -1;

    if (!sc) {
        return -EINVAL;
    }
    lvl = sc->thermal_param.tmd_cfg.level;
    len = snprintf(buf, (ssize_t)PAGE_SIZE, "%d\n", lvl);
    TH_DEBUG_PRINT(TH_DEBUG_LVL2, sc,"%s: level = %d, len = %d\n", __func__, lvl, len);
    return len;
}


ssize_t wifi_thermal_offpercent_store (struct device *dev,
        struct device_attribute *attr, const char *buf, size_t size)
{
    struct net_device *net = to_net_dev(dev);
    struct ath_softc_net80211 *scn = ath_netdev_priv(net);
    struct ath_softc *sc  =  ATH_DEV_TO_SC(scn->sc_dev);
    uint32_t off = INVALID_THERMAL_PARAM;
    int len = -1;
    char cbuf[5] = {'\0',};

    if (!sc || (size > 4)) {
        return -EINVAL;
    }
    memcpy(cbuf, buf, size);
    cbuf[size] = '\0';

    if (sscanf(cbuf, "%d", &off) > 0) {
        if (off >= THERMAL_MIN_OFF_PERCENT && off <= THERMAL_MAX_OFF_PERCENT) {
            sc->thermal_param.tmd_cfg.dcoffpercent = off;
            sc->thermal_param.tmd_cfg.cfg_ready |= TMD_CONFIG_DCOFF_PERCENT;
            da_thermal_tmd_config(sc);
            len = size;
        } else {
            len = -EINVAL;
        }
    } else {
        len = -EINVAL;
    }

    TH_DEBUG_PRINT(TH_DEBUG_LVL2, sc,"%s: off = %d, len = %d\n", __func__, off, len);
    return len;
}


ssize_t wifi_thermal_offpercent_show (struct device *dev,
        struct device_attribute *attr, char *buf)
{
    struct net_device *net = to_net_dev(dev);
    struct ath_softc_net80211 *scn = ath_netdev_priv(net);
    struct ath_softc *sc  =  ATH_DEV_TO_SC(scn->sc_dev);
    uint32_t off = 0;
    int len = -1;

    if (!sc) {
        return -EINVAL;
    }
    off = sc->thermal_param.tmd_cfg.dcoffpercent;
    len = snprintf(buf, (ssize_t)PAGE_SIZE, "%d\n", off);
    TH_DEBUG_PRINT(TH_DEBUG_LVL1, sc,"%s: offpercent = %d, len = %d\n", __func__, off, len);
    return len;
}


static DEVICE_ATTR(temp, S_IRUGO, wifi_thermal_temp_show, NULL);
static DEVICE_ATTR(mode, S_IRUGO | S_IWUSR, wifi_thermal_mode_show, wifi_thermal_mode_store);
static DEVICE_ATTR(dc, S_IRUGO | S_IWUSR, wifi_thermal_dutycycle_show, wifi_thermal_dutycycle_store);
static DEVICE_ATTR(thlvl, S_IRUGO | S_IWUSR, wifi_thermal_thlvl_show, wifi_thermal_thlvl_store);
static DEVICE_ATTR(off, S_IRUGO | S_IWUSR, wifi_thermal_offpercent_show, wifi_thermal_offpercent_store);

static struct attribute *wifi_thermal_attrs[] = {
    &dev_attr_temp.attr,
    &dev_attr_mode.attr,
    &dev_attr_dc.attr,
    &dev_attr_thlvl.attr,
    &dev_attr_off.attr,
    NULL
};

static struct attribute_group wifi_thermal_group = {
    .attrs  = wifi_thermal_attrs,
    .name = "thermal",
};

void modify_thermal_timer(struct ath_softc *sc, A_UINT32 ms)
{
    //Cancel old timer first
    OS_CANCEL_TIMER(&sc->sc_tt_timer);
    if(ms)
    {
        //setup the new timer
        OS_SET_TIMER(&sc->sc_tt_timer, ms);
    }
}

int getNewLevel(struct ath_softc *sc)
{
    int level = sc->thermal_param.th_stats.level;
    int nextlevel = level;
    /* Calculate next level
     * Temperature might be negative, need to compare with INT type.
     */
    if ((sc->thermal_param.th_stats.temp < sc->thermal_param.th_cfg.levelconf[level].tmplwm) && (level > 0)) {
        nextlevel--;
    }
    if ((sc->thermal_param.th_stats.temp > sc->thermal_param.th_cfg.levelconf[level].tmphwm) && (level < THERMAL_LEVELS - 1 )) {
        nextlevel++;
    }
    /* Updates stats */
    if(nextlevel != level) {
        sc->thermal_param.th_stats.levstats[nextlevel].levelcount++;
    } else {
        sc->thermal_param.th_stats.levstats[nextlevel].dccount++;
    }

    return nextlevel;
}

/*
 * Resume tx DMA
 */
static void tt_resumetxdma(struct ath_softc *sc, int mask)
{
    struct ath_hal      *ah     = sc->sc_ah;
    int i ;

    /* Re-enable interrupts */
    ath_hal_intrset(ah, sc->sc_imask);

    for (i = 0; i < HAL_NUM_TX_QUEUES - 1; i++) {
        if (mask & (1<< i)) {
            if(ATH_TXQ_SETUP(sc,i)) {
                struct ath_txq *txq = &sc->sc_txq[i];
                if ((txq != sc->sc_cabq) && (txq != sc->sc_uapsdq)) {
                    ATH_TXQ_LOCK(txq);
                    ath_txq_schedule(sc, txq);
                    ATH_TXQ_UNLOCK(txq);
                }
            }
        }
    }
}

void tt_tx_off(struct ath_softc *sc)
{
    if (ath_hal_get_device_info(sc->sc_ah, HAL_MAC_VERSION) == AR_SREV_VERSION_JET) {
        sc->sc_tx_on = 0;
    }
}

void tt_tx_on(struct ath_softc *sc)
{
    int  mask;
    sc->sc_tx_on = 1;
    //Turn all TX queues back on
    mask = 0 |
        (1 << WME_AC_BE) |
        (1 << WME_AC_BK) |
        (1 << WME_AC_VI) |
        (1 << WME_AC_VO) |
        0;
    tt_resumetxdma(sc,mask);
}

int16_t get_current_temperature(struct ath_softc *sc)
{
    int16_t temp_read;
    int16_t temp = 0, final_temp = 0;
    u_int8_t txchainmask, rxchainmask;
    bool retval;

    txchainmask = sc->sc_config.txchainmask;
    rxchainmask = sc->sc_config.rxchainmask;
    temp = MIN_TEMPERATURE;
    sc->sc_tt_chain=1; /* read temperature from chain0 sensor */
    ath_hal_set_temp_deg_c_chain(sc->sc_ah, sc->sc_tt_chain);
    retval = ath_hal_get_temp_deg_c(sc->sc_ah, &temp_read);

    if (!retval) {
        TH_DEBUG_PRINT(TH_DEBUG_LVL0, sc,"%s: Failed to get the temperature details.\n", __FUNCTION__);
        return -1;
    }

    switch(sc->sc_tt_chain)
    {
        case 0x1:
            sc->sc_tt_temp1= temp_read;
            break;
        case 0x2:
            sc->sc_tt_temp2= temp_read;
            break;
        case 0x4:
            sc->sc_tt_temp3= temp_read;
            break;
        case 0x8:
            sc->sc_tt_temp4= temp_read;
            break;
        default:
            break;
    }

    if (temp_read > temp)
        temp = temp_read;
    if (temp != MIN_TEMPERATURE) {
        final_temp = temp;
    } else {
        final_temp = ROOM_TEMPERATURE;
    }

    return final_temp;
}

static
OS_TIMER_FUNC(thermal_timeout)
{
    struct ath_softc    *sc;
    OS_GET_TIMER_ARG(sc, struct ath_softc *);

    if((sc->sc_tx_on == 0) && (sc->thermal_param.th_cfg.levelconf[sc->thermal_param.th_stats.level].dcoffpercent < 100)) {
        //If traffic was off, then turn it on, we are in the middle of duty cycle
        tt_tx_on(sc);
        //reset the timer to on time %
        modify_thermal_timer(sc, sc->thermal_param.th_cfg.dc - (sc->thermal_param.th_cfg.levelconf[sc->thermal_param.th_stats.level].dcoffpercent * sc->thermal_param.th_cfg.dc) /100);
    } else {
        //We are in the beginning of the duty cycle
        //read the temperate and set the state.
        if (sc->sc_tt_temp) {
            sc->thermal_param.th_stats.temp = sc->sc_tt_temp;
        } else {
            sc->thermal_param.th_stats.temp = get_current_temperature(sc);
        }
        if (sc->sc_tt_level) {
            sc->thermal_param.th_stats.level = sc->sc_tt_level;
        } else {
            sc->thermal_param.th_stats.level = getNewLevel(sc);
        }
        //Any level above zero should start with off state then on.
        if(sc->thermal_param.th_cfg.levelconf[sc->thermal_param.th_stats.level].dcoffpercent) {
            tt_tx_off(sc);
            /* reset the timer to on or off time sc->thermal_param.th_cfg.levelconf[sc->thermal_param.th_stats.level] */
            /* If dcoff is not 0, dont turn off the timer. */
            modify_thermal_timer(sc, (sc->thermal_param.th_cfg.levelconf[sc->thermal_param.th_stats.level].dcoffpercent * sc->thermal_param.th_cfg.dc + 100 -1)/100);
        } else {
            //reset the timer to level0wait
            modify_thermal_timer(sc, LEVEL0WAIT);
        }
        //on level 0 because of longer polling time (1sec) we send the event right away to host
        if((++sc->sc_dc_per_event >= sc->thermal_param.th_cfg.dc_per_event) || (sc->thermal_param.th_stats.level == 0 ))
        {
            sc->sc_dc_per_event =0;
        }
    }
}

int ath_config_thermal_mitigation_param(struct ath_softc *sc)
{
    if (!(sc->thermal_param.tt_support)) {
        TH_DEBUG_PRINT(TH_DEBUG_LVL0, sc,"%s:TT not supported\n", __FUNCTION__);
        return TH_FAIL;
    }

    if ((sc->thermal_param.th_cfg.enable) && (atomic_read(&sc->sc_nap_vaps_up) > 0 || atomic_read(&sc->sc_nsta_vaps_up) > 0)) {
        TH_DEBUG_PRINT(TH_DEBUG_LVL0, sc,"%s:Active VAP found; starting the Thermal timer.\n", __FUNCTION__);
        modify_thermal_timer(sc, THERMAL_CONFIG_DEFAULT_DUTY_CYCLE);
    } else {
        TH_DEBUG_PRINT(TH_DEBUG_LVL0, sc,"%s: Cancelling Thermal timer.\n", __FUNCTION__);
        modify_thermal_timer(sc, 0);
    }

    return 0;
}

static void da_ath_thermal_param_init (struct ath_softc *sc)
{
    OS_MEMSET(&(sc->thermal_param.th_stats), 0, sizeof(sc->thermal_param.th_stats));
    OS_MEMSET(&(sc->thermal_param.th_cfg), 0, sizeof(sc->thermal_param.th_cfg));
    OS_MEMSET(&(sc->thermal_param.tmd_cfg), 0, sizeof(sc->thermal_param.tmd_cfg));
    sc->thermal_param.th_cfg.enable = 0;
    sc->thermal_param.th_cfg.dc = THERMAL_CONFIG_DEFAULT_DUTY_CYCLE;
    sc->thermal_param.th_cfg.dc_per_event = THERMAL_WMI_EVENT_DC;
    sc->thermal_param.th_cfg.log_lvl = TH_DEBUG_LVL1;

    /*
     *    Level    tmplwm (C)       tmphwm (C)       dcoffpercent (%)
     *      0         NIL              110                 0
     *      1         100              120                50
     *      2         110              135                90
     *      3         125              MAX               100
     */

    /* thermal level / zone 0 config */
    sc->thermal_param.th_cfg.levelconf[0].tmplwm = THERMAL_CONFIG_TMPLWM0;
    sc->thermal_param.th_cfg.levelconf[0].tmphwm = THERMAL_CONFIG_TMPHWM0; /* Celsius */
    sc->thermal_param.th_cfg.levelconf[0].dcoffpercent = THERMAL_CONFIG_DCOFF0;   /* milli seconds */
    sc->thermal_param.th_cfg.levelconf[0].priority = THERMAL_ALL_UNICAST_DATA_QUEUES;
    sc->thermal_param.th_cfg.levelconf[0].policy = THERMAL_POLICY_QUEUE_PAUSE;
    /* thermal level / zone 1 config */
    sc->thermal_param.th_cfg.levelconf[1].tmplwm = THERMAL_CONFIG_TMPLWM1;
    sc->thermal_param.th_cfg.levelconf[1].tmphwm = THERMAL_CONFIG_TMPHWM1; /* Celsius */
    sc->thermal_param.th_cfg.levelconf[1].dcoffpercent = THERMAL_CONFIG_DCOFF1;    /* milli seconds */
    sc->thermal_param.th_cfg.levelconf[1].priority = THERMAL_ALL_UNICAST_DATA_QUEUES;
    sc->thermal_param.th_cfg.levelconf[1].policy = THERMAL_POLICY_QUEUE_PAUSE;
    /* thermal level / zone 2 config */
    sc->thermal_param.th_cfg.levelconf[2].tmplwm = THERMAL_CONFIG_TMPLWM2;
    sc->thermal_param.th_cfg.levelconf[2].tmphwm = THERMAL_CONFIG_TMPHWM2; /* Celsius */
    sc->thermal_param.th_cfg.levelconf[2].dcoffpercent = THERMAL_CONFIG_DCOFF2;    /* milli seconds */
    sc->thermal_param.th_cfg.levelconf[2].priority = THERMAL_ALL_UNICAST_DATA_QUEUES;
    sc->thermal_param.th_cfg.levelconf[2].policy = THERMAL_POLICY_QUEUE_PAUSE;
    /* thermal level / zone 3 config */
    sc->thermal_param.th_cfg.levelconf[3].tmplwm = THERMAL_CONFIG_TMPLWM3;
    sc->thermal_param.th_cfg.levelconf[3].tmphwm = THERMAL_CONFIG_TMPHWM3; /* Celsius */
    sc->thermal_param.th_cfg.levelconf[3].dcoffpercent = THERMAL_CONFIG_DCOFF3;    /* milli seconds */
    sc->thermal_param.th_cfg.levelconf[3].priority = THERMAL_ALL_UNICAST_DATA_QUEUES;
    sc->thermal_param.th_cfg.levelconf[3].policy = THERMAL_POLICY_QUEUE_PAUSE;
    sc->sc_dc_per_event = 0;
    /* Create the timer */
    OS_INIT_TIMER (sc->sc_osdev, &sc->sc_tt_timer, thermal_timeout, (void *) sc, QDF_TIMER_TYPE_WAKE_APPS);
}

/*
 * Register Thermal Mitigation Functionality
 */

int32_t ath_thermal_mitigation_attach (struct ath_softc_net80211 *scn,
        struct net_device *dev)
{
    struct ath_softc *sc  =  ATH_DEV_TO_SC(scn->sc_dev);

    if (!sc) {
        TH_DEBUG_PRINT(TH_DEBUG_LVL0, sc,"Error: %s: NULL sc\n", __func__);
        return -EINVAL;
    }

    sc->thermal_param.th_cfg.log_lvl = TH_DEBUG_LVL0;
    TH_DEBUG_PRINT(TH_DEBUG_LVL1, sc, "%s: ++\n", __func__);

    sc->sc_tx_on = 1;
    if (sc->thermal_param.tt_support) {
        if (qdf_vfs_set_file_attributes(&dev->dev.kobj, &wifi_thermal_group)) {
            TH_DEBUG_PRINT(TH_DEBUG_LVL0, sc, "%s: unable to register wifi_thermal_group for %s\n", __func__, dev->name);
            return TH_FAIL;
        }
        da_ath_thermal_param_init(sc);
    } else {
        TH_DEBUG_PRINT(TH_DEBUG_LVL0, sc, "%s: TT not supported \n", __func__);
    }

    TH_DEBUG_PRINT(TH_DEBUG_LVL1, sc, "%s: --\n", __func__);

    return 0;
}
EXPORT_SYMBOL(ath_thermal_mitigation_attach);

int ath_thermal_mitigation_detach(struct ath_softc_net80211 *scn,
        struct net_device *dev) {
    struct ath_softc *sc  =  ATH_DEV_TO_SC(scn->sc_dev);

    if (!sc) {
        TH_DEBUG_PRINT(TH_DEBUG_LVL0, sc,"Error: %s: NULL sc\n", __func__);
        return -EINVAL;
    }

    if (sc->thermal_param.tt_support) {
        sc->thermal_param.th_cfg.enable = TH_FALSE;
        qdf_vfs_clear_file_attributes(&dev->dev.kobj, &wifi_thermal_group);
    }

    return 0;
}
EXPORT_SYMBOL(ath_thermal_mitigation_detach);

/*
** Prototype for iw attach
*/

#ifdef ATH_SUPPORT_LINUX_STA
#ifdef CONFIG_SYSCTL
void ath_dynamic_sysctl_register(struct ath_softc *sc);
void ath_dynamic_sysctl_unregister(struct ath_softc *sc);
#endif
#endif
#if OS_SUPPORT_ASYNC_Q
static void os_async_mesg_handler( void  *ctx, u_int16_t  mesg_type, u_int16_t  mesg_len, void  *mesg );
#endif

void ath_iw_attach(struct net_device *dev);
#if !NO_SIMPLE_CONFIG
extern int32_t unregister_simple_config_callback(char *name);
extern int32_t register_simple_config_callback (char *name, void *callback, void *arg1, void *arg2);
static irqreturn_t jumpstart_intr(int cpl, void *dev_id, struct pt_regs *regs, void *push_dur);
#endif

#if defined(QCA_PARTNER_PLATFORM) && !QCA_PARTNER_CBM_DIRECTPATH
extern 	int osif_pltfrm_interupt_register(struct net_device *dev,int type, void * irs_handler, void * msi_ce_handler);
extern int osif_pltfrm_interupt_unregister(struct net_device *dev,int level);
extern void ath_pltfrm_init( struct net_device *dev );
extern void ath_pltfrm_exit( struct net_device *dev );
#endif

#ifdef ATH_TX99_DIAG
extern u_int8_t tx99_ioctl(ath_dev_t dev, struct ath_softc *sc, int cmd, void *addr);
#endif

#ifndef ADF_SUPPORT
void *
OS_ALLOC_VAP(osdev_t osdev, u_int32_t len)
{
    void *netif;

    netif = OS_MALLOC(osdev, len, GFP_KERNEL);
    if (netif != NULL)
        OS_MEMZERO(netif, len);

    return netif;
}

void
OS_FREE_VAP(void *netif)
{
    OS_FREE(netif);
}

#endif

int
ath_get_netif_settings(ieee80211_handle_t ieee)
{
    struct ieee80211com *ic = NET80211_HANDLE(ieee);
    struct ath_softc_net80211 *scn = ATH_SOFTC_NET80211(ic);
    osdev_t osdev = scn->sc_osdev;
    struct net_device *dev =  osdev->netdev;
    int flags = 0;

    if (dev->flags & IFF_RUNNING)
        flags |= ATH_NETIF_RUNNING;
    if (dev->flags & IFF_PROMISC)
        flags |= ATH_NETIF_PROMISCUOUS;
    if (dev->flags & IFF_ALLMULTI)
        flags |= ATH_NETIF_ALLMULTI;

    return flags;
}

#if ATH_VAP_DELETE_ON_RMMOD
static int
ath_vap_delete_on_rmmod(struct ath_softc_net80211 *scn)
{
    struct ieee80211vap *vap = NULL, *tvap = NULL;
    struct ieee80211com *ic = &scn->sc_ic;
    int delete_in_progress = 0;
#define  WAIT_VAP_IDLE_INTERVALL 10
    int gracetime = WAIT_VAP_IDLE_INTERVALL;

    do {
        IEEE80211_COMM_LOCK(ic);
        if (!TAILQ_EMPTY(&ic->ic_vaps)) {
            TAILQ_FOREACH_SAFE(vap, &ic->ic_vaps, iv_next, tvap) {
                if (((osif_dev *)((os_if_t)vap->iv_ifp))->is_delete_in_progress) {
                    delete_in_progress = 1;
                }
            }
        }
        IEEE80211_COMM_UNLOCK(ic);

        if (delete_in_progress) {
            msleep(1000);
            if (--gracetime == 0) {
                QDF_PRINT_INFO(ic->ic_print_idx, QDF_MODULE_ID_ANY, QDF_TRACE_LEVEL_INFO, KERN_ERR "timeout waiting for vap %s\n",
                       ((osif_dev *)((os_if_t)vap->iv_ifp))->netdev->name);
                return -1;
            }
        } else {
            break;
        }
    } while(1);
    /*
     *  If packets are heldup in the lmac queue, free the node ref count.
     */
    ath_draintxq(scn->sc_dev,0,0);

    /*
     * Don't add any more VAPs after this.
     * Else probably the detach should be done with rtnl_lock() held.
     */
    scn->sc_in_delete = 1;

    /* Bring down and delete vaps */
    if (!TAILQ_EMPTY(&ic->ic_vaps)) {
        TAILQ_FOREACH_SAFE(vap, &ic->ic_vaps, iv_next, tvap) {
            QDF_PRINT_INFO(ic->ic_print_idx, QDF_MODULE_ID_ANY, QDF_TRACE_LEVEL_INFO, KERN_ERR "%s: vap %s still registered, cleaning up\n",
                   __func__, ((osif_dev *)((os_if_t)vap->iv_ifp))->netdev->name);
            dev_close(((osif_dev *)((os_if_t)vap->iv_ifp))->netdev);
            osif_ioctl_delete_vap(((osif_dev *)((os_if_t)vap->iv_ifp))->netdev);
        }
    }
    return 0;
}
#else
static int
ath_vap_delete_on_rmmod(struct ath_softc_net80211 *scn)
{
    /* dummy */
    return 0;
}
#endif /* ATH_VAP_DELETE_ON_RMMOD */

/*
 * Merge multicast addresses from all vap's to form the
 * hardware filter.  Ideally we should only inspect our
 * own list and the 802.11 layer would merge for us but
 * that's a bit difficult so for now we put the onus on
 * the driver.
 */
void
ath_mcast_merge(ieee80211_handle_t ieee, u_int32_t mfilt[2])
{
    struct ieee80211com *ic = NET80211_HANDLE(ieee);
    struct ieee80211vap *vap;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 34)
    struct netdev_hw_addr *ha;
#else
    struct dev_mc_list *mc;
#endif
    u_int32_t val;
    u_int8_t pos;

    mfilt[0] = mfilt[1] = 0;
    /* XXX locking */
    TAILQ_FOREACH(vap, &ic->ic_vaps, iv_next) {
        struct net_device *dev;
        os_if_t osif = vap->iv_ifp;
        if (osif == NULL)
            continue;

        dev = ((osif_dev *)osif)->netdev;
        if (dev == NULL)
            continue;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 34)
        netdev_for_each_mc_addr(ha, dev) {
            /* calculate XOR of eight 6-bit values */
            val = LE_READ_4(ha->addr + 0);
            pos = (val >> 18) ^ (val >> 12) ^ (val >> 6) ^ val;
            val = LE_READ_4(ha->addr + 3);
            pos ^= (val >> 18) ^ (val >> 12) ^ (val >> 6) ^ val;
            pos &= 0x3f;
            mfilt[pos / 32] |= (1 << (pos % 32));
        }
#else
        for (mc = dev->mc_list; mc; mc = mc->next) {
            /* calculate XOR of eight 6bit values */
            val = LE_READ_4(mc->dmi_addr + 0);
            pos = (val >> 18) ^ (val >> 12) ^ (val >> 6) ^ val;
            val = LE_READ_4(mc->dmi_addr + 3);
            pos ^= (val >> 18) ^ (val >> 12) ^ (val >> 6) ^ val;
            pos &= 0x3f;
            mfilt[pos / 32] |= (1 << (pos % 32));
        }
#endif
    }
}


static int
ath_netdev_open(struct net_device *dev)
{
    struct ath_softc_net80211 *scn = ath_netdev_priv(dev);
    int ath_ret;
    struct ieee80211com *ic = &scn->sc_ic;
    struct ieee80211vap *vap;
    osif_dev  *osifp;
    struct net_device *netdev;
    u_int8_t myaddr[IEEE80211_ADDR_LEN];
    u_int8_t id = 0;

#ifdef ATH_BUS_PM
    if (scn->sc_osdev->isDeviceAsleep)
        return -EPERM;
#endif /* ATH_BUS_PM */

    ath_ret = ath_resume(scn);
    if(ath_ret == 0){
        dev->flags |= IFF_UP | IFF_RUNNING;      /* we are ready to go */
        /*  If physical radio interface wifiX is shutdown,all virtual interfaces(athX) should gets shutdown and
            all these downed virtual interfaces should gets up when physical radio interface(wifiX) is up.Refer EV 116786.
         */
        vap = TAILQ_FIRST(&ic->ic_vaps);
        while (vap != NULL) {
            osifp = (osif_dev *)vap->iv_ifp;
            netdev = osifp->netdev;
            ieee80211vap_get_macaddr(vap, myaddr);
            ATH_GET_VAP_ID(myaddr, wlan_vap_get_hw_macaddr(vap), id);
            if( ic->id_mask_vap_downed & ( 1 << id ) ){
                dev_change_flags(netdev,netdev->flags | ( IFF_UP ));
                ic->id_mask_vap_downed &= (~( 1 << id ));
            }
            vap = TAILQ_NEXT(vap, iv_next);
        }
    }
    return ath_ret;
}

static int
ath_netdev_stop(struct net_device *dev)
{
    struct ath_softc_net80211 *scn = ath_netdev_priv(dev);

    struct ieee80211com *ic = &scn->sc_ic;
    struct ieee80211vap *vap;
    osif_dev  *osifp;
    struct net_device *netdev;
    u_int8_t myaddr[IEEE80211_ADDR_LEN];
    u_int8_t id = 0;

    /*  If physical radio interface wifiX is shutdown,all virtual interfaces(athX) should gets shutdown and
        all these downed virtual interfaces should gets up when physical radio interface(wifiX) is up.Refer EV 116786.
     */

    vap = TAILQ_FIRST(&ic->ic_vaps);
    while (vap != NULL) {
        osifp = (osif_dev *)vap->iv_ifp;
        netdev = osifp->netdev;
        if (IS_IFUP(netdev)) {
            dev_change_flags(netdev,netdev->flags & ( ~IFF_UP ));
            ieee80211vap_get_macaddr(vap, myaddr);
            ATH_GET_VAP_ID(myaddr, wlan_vap_get_hw_macaddr(vap), id);
            ic->id_mask_vap_downed |= ( 1 << id);
        }
        vap = TAILQ_NEXT(vap, iv_next);
    }
    dev->flags &= ~IFF_RUNNING;

    ic->ic_pwrsave_set_state(ic,IEEE80211_PWRSAVE_FULL_SLEEP);
    return 0;
}



#if QCA_PARTNER_PLATFORM
int
#else
static int
#endif
ath_netdev_hardstart_generic(struct sk_buff *skb, struct net_device *dev)
{
    struct ath_softc_net80211 *scn = ath_netdev_priv(dev);
    struct ath_softc *sc = ATH_DEV_TO_SC(scn->sc_dev);
    struct ieee80211_cb *cb;
    struct ieee80211_node *ni;
    int error = 0;
    struct ieee80211com *ic = &scn->sc_ic;
    struct ether_header *eh = (struct ether_header *)skb->data;
    int ismulti = IEEE80211_IS_MULTICAST(eh->ether_dhost) ? 1 : 0;
    u_int16_t addba_status;
    u_int32_t txq_depth, txq_aggr_depth;
    int  buffer_limit;
    u_int32_t  txbuf_freecount;
    struct ath_txq *txq;
    int qnum;
    int early_drop = 1; /* allow tx buffer calculation to drop the packet by default */
    /* make early_drop = 0 for important control plane packets like EAPOL and DHCP */

    cb = (struct ieee80211_cb *)skb->cb;
    ni = cb->ni;
#if !ATH_SUPPORT_VOWEXT
    if(wbuf_is_highpriority(skb))
	early_drop = 0;
#endif

#if defined(ATH_SUPPORT_P2P)
     cb->complete_handler = NULL;
     cb->complete_handler_arg = NULL;
#endif  /* ATH_SUPPORT_P2P */
    /*
     * device must be up and running
     */
    if ((dev->flags & (IFF_RUNNING|IFF_UP)) != (IFF_RUNNING|IFF_UP)) {
        error = -ENETDOWN;
        goto bad;
    }

    /*
     * NB: check for valid node in case kernel directly sends packets
     * on wifiX interface (such as broadcast packets generated by ipv6)
     */
    if (ni == NULL) {
        qdf_nbuf_free(skb);
        return 0;
    }

    if (unlikely(atomic_read(&sc->sc_in_reset))){
        goto bad;
    }

#ifdef ATH_SUPPORT_UAPSD
    /* Limit UAPSD node queue depth to WME_UAPSD_NODE_MAXQDEPTH */
    if ((ni->ni_flags & IEEE80211_NODE_UAPSD) &&
        scn->sc_ops->uapsd_depth(ATH_NODE_NET80211(ni)->an_sta) >= WME_UAPSD_NODE_MAXQDEPTH)
    {
        goto bad;
    }
#endif

    qnum =  scn->sc_ac2q[skb->priority];
    txq  = &sc->sc_txq[qnum];

    txq_depth = scn->sc_ops->txq_depth(scn->sc_dev, scn->sc_ac2q[skb->priority]);
    txq_aggr_depth = scn->sc_ops->txq_aggr_depth(scn->sc_dev, scn->sc_ac2q[skb->priority]);

    ic->ic_addba_status(ni, cb->u_tid, &addba_status);

    /*
     * This logic throttles legacy and unaggregated HT frames if they share the hardware
     * queue with aggregates. This improves the transmit throughput performance to
     * aggregation enabled nodes when they coexist with legacy nodes.
     */
    /* Do not throttle EAPOL packets - this causes the REKEY packets
     * to be dropped and station disconnects.
     */
    DPRINTF(scn, ATH_DEBUG_RESET, "skb->priority=%d cb->u_tid=%d addba_status=%d txq_aggr_depth=%d txq_depth=%d\n",skb->priority, cb->u_tid, addba_status, txq_aggr_depth, txq_depth);

    if ((addba_status != IEEE80211_STATUS_SUCCESS)
        && (txq_aggr_depth > 0)
        && early_drop)
		{
        if (txq_depth >= 25) {
            goto bad;
        }
        }

    /*
     * Try to avoid running out of descriptors
     */
     txbuf_freecount = scn->sc_ops->get_txbuf_free(scn->sc_dev);
#if !ATH_SUPPORT_VOWEXT
     if (ismulti && early_drop) {
#else
    if (ismulti) {
#endif
         buffer_limit = MULTICAST_DROP_THRESHOLD;
         if (txbuf_freecount <= buffer_limit)   /* check for 10% txbuf availability*/
         {
            goto bad;
        }
    }
    /* Reserve 16 Tx buffers for EAPOL, DHCP, ARP and QOS NULL frames */
    if (early_drop && (txbuf_freecount <=  txq->axq_minfree + 16 )) {
            goto bad;
    }

#if !ATH_DATABUS_ERROR_RESET_LOCK_3PP
    error = ath_tx_send(skb);
#else
    ATH_INTERNAL_RESET_LOCK(sc);
    error = ath_tx_send(skb);
    ATH_INTERNAL_RESET_UNLOCK(sc);
#endif


    if (error) {
        DPRINTF(scn, ATH_DEBUG_XMIT, "%s: Tx failed with error %d\n",
            __func__, error);
    }
    return 0;

bad:
    __11nstats(((struct ath_softc *)(scn->sc_dev)),tx_drops);
    sc->sc_stats.ast_tx_nobuf++;
    sc->sc_stats.ast_txq_packets[scn->sc_ac2q[skb->priority]]++;
    sc->sc_stats.ast_txq_nobuf[scn->sc_ac2q[skb->priority]]++;

    IEEE80211_TX_COMPLETE_WITH_ERROR(skb);
    DPRINTF(scn, ATH_DEBUG_XMIT, "%s: Tx failed with error %d\n",
            __func__, error);
    return 0;
}



static int
ath_netdev_hardstart(struct sk_buff *skb, struct net_device *dev)
{
    do_ath_netdev_hardstart(skb,dev);
}
static void
ath_netdev_tx_timeout(struct net_device *dev)
{
    struct ath_softc_net80211 *scn = ath_netdev_priv(dev);

    DPRINTF(scn, ATH_DEBUG_WATCHDOG, "%s: %sRUNNING\n",
            __func__, (dev->flags & IFF_RUNNING) ? "" : "!");

	if (dev->flags & IFF_RUNNING) {
        scn->sc_ops->reset_start(scn->sc_dev, 0, 0, 0);
        scn->sc_ops->reset(scn->sc_dev);
        scn->sc_ops->reset_end(scn->sc_dev, 0);
	}
}

static int
ath_netdev_set_macaddr(struct net_device *dev, void *addr)
{
    struct ath_softc_net80211 *scn = ath_netdev_priv(dev);
    struct ieee80211com *ic = &scn->sc_ic;
    struct sockaddr *mac = addr;

    if (netif_running(dev)) {
        DPRINTF(scn, ATH_DEBUG_ANY,
            "%s: cannot set address; device running\n", __func__);
        return -EBUSY;
    }
    DPRINTF(scn, ATH_DEBUG_ANY, "%s: %.2x:%.2x:%.2x:%.2x:%.2x:%.2x\n",
        __func__,
        mac->sa_data[0], mac->sa_data[1], mac->sa_data[2],
        mac->sa_data[3], mac->sa_data[4], mac->sa_data[5]);

    /* XXX not right for multiple vap's */
    IEEE80211_ADDR_COPY(ic->ic_myaddr, mac->sa_data);
    IEEE80211_ADDR_COPY(ic->ic_my_hwaddr, mac->sa_data);
    IEEE80211_ADDR_COPY(dev->dev_addr, mac->sa_data);
    scn->sc_ops->set_macaddr(scn->sc_dev, dev->dev_addr);
    return 0;
}

static void
ath_netdev_set_mcast_list(struct net_device *dev)
{
    struct ath_softc_net80211 *scn = ath_netdev_priv(dev);
    scn->sc_ops->mc_upload(scn->sc_dev);
}

static int
ath_change_mtu(struct net_device *dev, int mtu)
{
    if (!(ATH_MIN_MTU < mtu && mtu <= ATH_MAX_MTU)) {
        DPRINTF((struct ath_softc_net80211 *) ath_netdev_priv(dev),
            ATH_DEBUG_ANY, "%s: invalid %d, min %u, max %u\n",
            __func__, mtu, ATH_MIN_MTU, ATH_MAX_MTU);
        return -EINVAL;
    }
    DPRINTF((struct ath_softc_net80211 *) ath_netdev_priv(dev), ATH_DEBUG_ANY,
        "%s: %d\n", __func__, mtu);

    dev->mtu = mtu;
    return 0;
}

#ifdef ATH_USB
#include "usb_eth.h"
#else
extern int ath_ioctl_ethtool(struct ath_softc_net80211 *scn, int cmd, void *addr);
#endif

#if ATH_SUPPORT_WRAP && DBDC_REPEATER_SUPPORT
int32_t ath_ioctl_get_primary_radio(struct ath_softc_net80211 *scn, void *param)
{
    int primary_radio;
    struct ieee80211com *ic = &scn->sc_ic;

    if(ic->ic_primary_radio) {
        primary_radio = 1;
    } else {
        primary_radio = 0;
    }

    return _copy_to_user(param , &primary_radio, sizeof(primary_radio));
}

int32_t ath_ioctl_get_mpsta_mac_addr(struct ath_softc_net80211 *scn, caddr_t param)
{
    struct ieee80211com *ic = &scn->sc_ic;
    struct ieee80211vap *mpsta_vap;

    if (ic->ic_mpsta_vap == NULL)
    {
        return -EFAULT;
    }

    mpsta_vap = ic->ic_mpsta_vap;

    return _copy_to_user((caddr_t)param , mpsta_vap->iv_myaddr, IEEE80211_ADDR_LEN);
}

void ath_ioctl_disassoc_clients(struct ath_softc_net80211 *scn)
{
        struct ieee80211com *ic = &scn->sc_ic;
        struct ieee80211vap           *tmp_vap;
        struct ieee80211com           *tmp_ic;
        int i=0;

        for (i=0; i < MAX_RADIO_CNT; i++) {
                tmp_ic = ic->ic_global_list->global_ic[i];
                if (tmp_ic) {
                        TAILQ_FOREACH(tmp_vap, &tmp_ic->ic_vaps, iv_next) {
                                if ((tmp_vap->iv_opmode == IEEE80211_M_HOSTAP) && ieee80211_vap_ready_is_set(tmp_vap)) {
                                        wlan_iterate_station_list(tmp_vap, sta_disassoc, NULL);
                                }
                        }
                }
        }
}

int32_t ath_ioctl_get_force_client_mcast(struct ath_softc_net80211 *scn, void *param)
{
    int force_client_mcast;
    struct ieee80211com *ic = &scn->sc_ic;

    if(ic->ic_global_list->force_client_mcast_traffic) {
        force_client_mcast = 1;
    } else {
        force_client_mcast = 0;
    }

    return _copy_to_user(param , &force_client_mcast, sizeof(force_client_mcast));
}

int32_t ath_ioctl_get_max_priority_radio(struct ath_softc_net80211 *scn, void *param)
{

    char wifi_iface[IFNAMSIZ];
    struct ieee80211com *ic = &scn->sc_ic;
    struct global_ic_list   *glist = ic->ic_global_list;
    struct ieee80211vap           *max_priority_stavap_up;
    struct ieee80211com *max_priority_ic;
    struct net_device *n_dev;

    memset(&wifi_iface[0], 0, sizeof(wifi_iface));
    GLOBAL_IC_LOCK(glist);
    max_priority_stavap_up = glist->max_priority_stavap_up;
    GLOBAL_IC_UNLOCK(glist);
    if (max_priority_stavap_up && ieee80211_vap_ready_is_set(max_priority_stavap_up)) {
        max_priority_ic = max_priority_stavap_up->iv_ic;
        n_dev = max_priority_ic->ic_osdev->netdev;
        OS_MEMCPY(wifi_iface, n_dev->name, sizeof(wifi_iface));
        qdf_print("max priority radio:%s\n",wifi_iface);
    }
    return _copy_to_user(param , wifi_iface, IFNAMSIZ);
}
#endif


void ath_ioctl_iface_mgr_status(struct ath_softc_net80211 *scn, void *param)
{
    struct ieee80211com *ic = &scn->sc_ic;
    u_int8_t iface_mgr_up;

    if(copy_from_user(&iface_mgr_up, param, sizeof(u_int8_t))) {
	return;
    }

    GLOBAL_IC_LOCK(ic->ic_global_list);
    if (iface_mgr_up == 1) {
        ic->ic_global_list->iface_mgr_up = 1;
        qdf_print("setting iface_mgr_up to 1\n");
    } else {
        ic->ic_global_list->iface_mgr_up = 0;
        qdf_print("setting iface_mgr_up to 0\n");
    }
    GLOBAL_IC_UNLOCK(ic->ic_global_list);
}

u_int8_t ath_ioctl_get_stavap_connection(struct ath_softc_net80211 *scn, void *param)
{
    u_int8_t stavap_up;
    struct ieee80211com *ic = &scn->sc_ic;
    struct ieee80211vap           *stavap = ic->ic_sta_vap;
    osif_dev *osifp;

    osifp = (osif_dev *)stavap->iv_ifp;
    if(osifp == NULL) {
        return -EINVAL;
    }
    if (osifp->is_up) {
        stavap_up = 1;
    } else {
        stavap_up = 0;
    }

    return _copy_to_user(param , &stavap_up, sizeof(stavap_up));
}

u_int16_t ath_ioctl_get_disconnection_timeout(struct ath_softc_net80211 *scn, void *param)
{
    u_int16_t timeout;
    struct ieee80211com *ic = &scn->sc_ic;
    u_int16_t disconnect_timeout = ic->ic_global_list->disconnect_timeout;
    u_int16_t reconfiguration_timeout = ic->ic_global_list->reconfiguration_timeout;

    if (disconnect_timeout >= reconfiguration_timeout) {
        timeout = disconnect_timeout;
        qdf_print("disconnect_timeout:%d\n",disconnect_timeout);
    } else {
        timeout = reconfiguration_timeout;
        qdf_print("reconfiguration_timeout:%d\n",reconfiguration_timeout);
    }

    return _copy_to_user(param , &timeout, sizeof(timeout));
}

int32_t ath_ioctl_get_preferred_uplink(struct ath_softc_net80211 *scn, void *param)
{
    int preferredUplink;
    struct ieee80211com *ic = &scn->sc_ic;

    if(ic->ic_preferredUplink) {
        preferredUplink = 1;
    } else {
        preferredUplink = 0;
    }

    return _copy_to_user(param , &preferredUplink, sizeof(preferredUplink));
}

int ath_ioctl_set_thermal_handler (struct ath_softc_net80211 *scn, caddr_t param)
{
    struct th_config_t th_config;
    struct ath_softc *sc  =  ATH_DEV_TO_SC(scn->sc_dev);
    int error = 0;
    int cfg_changed = TH_FALSE;
    int i = 0;

    if (!sc) {
        TH_DEBUG_PRINT(TH_DEBUG_LVL0, sc,"%s: Error invalid sc:\n", __func__);
        return -EINVAL;
    }

    if (!sc->thermal_param.tt_support){
        TH_DEBUG_PRINT(TH_DEBUG_LVL0, sc, "%s: TT not supported\n", __func__);
        return -EINVAL;
    }

    error = __xcopy_from_user(&th_config, param, sizeof(th_config));

    if (error) {
        TH_DEBUG_PRINT(TH_DEBUG_LVL0, sc,"%s: Error in copying data from user space:\n", __func__);
        error = -EFAULT;
    } else {
        if ((th_config.enable != INVALID_THERMAL_PARAM) &&
                (sc->thermal_param.th_cfg.enable != th_config.enable)) {
            sc->thermal_param.th_cfg.enable = th_config.enable;
            cfg_changed = TH_TRUE;
        }

        if ((th_config.dc != INVALID_THERMAL_PARAM) &&
                (sc->thermal_param.th_cfg.dc != th_config.dc)) {
            sc->thermal_param.th_cfg.dc = th_config.dc;
            cfg_changed = TH_TRUE;
        }

        if ((th_config.dc_per_event != INVALID_THERMAL_PARAM) &&
                (sc->thermal_param.th_cfg.dc_per_event != th_config.dc_per_event)) {
            sc->thermal_param.th_cfg.dc_per_event = th_config.dc_per_event;
            cfg_changed = TH_TRUE;
        }

        if (th_config.log_lvl != INVALID_THERMAL_PARAM) {
            sc->thermal_param.th_cfg.log_lvl = th_config.log_lvl | TH_DEBUG_LVL1;
        }

        for (i = 0; i < THERMAL_LEVELS; i++) {

            if ((th_config.levelconf[i].tmplwm != INVALID_THERMAL_PARAM) &&
                    (sc->thermal_param.th_cfg.levelconf[i].tmplwm != th_config.levelconf[i].tmplwm)) {
                sc->thermal_param.th_cfg.levelconf[i].tmplwm = th_config.levelconf[i].tmplwm;
                cfg_changed = TH_TRUE;
            }
            if ((th_config.levelconf[i].tmphwm != INVALID_THERMAL_PARAM) &&
                    (sc->thermal_param.th_cfg.levelconf[i].tmphwm != th_config.levelconf[i].tmphwm)) {
                sc->thermal_param.th_cfg.levelconf[i].tmphwm = th_config.levelconf[i].tmphwm;
                cfg_changed = TH_TRUE;
            }
            if ((th_config.levelconf[i].dcoffpercent != INVALID_THERMAL_PARAM) &&
                    (sc->thermal_param.th_cfg.levelconf[i].dcoffpercent != th_config.levelconf[i].dcoffpercent)) {
                sc->thermal_param.th_cfg.levelconf[i].dcoffpercent = th_config.levelconf[i].dcoffpercent;
                cfg_changed = TH_TRUE;
            }
            if ((th_config.levelconf[i].priority != INVALID_THERMAL_PARAM) &&
                    (sc->thermal_param.th_cfg.levelconf[i].priority != th_config.levelconf[i].priority)) {
                sc->thermal_param.th_cfg.levelconf[i].priority = th_config.levelconf[i].priority;
                cfg_changed = TH_TRUE;
            }
            if ((th_config.levelconf[i].policy != INVALID_THERMAL_PARAM) &&
                    (sc->thermal_param.th_cfg.levelconf[i].policy != th_config.levelconf[i].policy)) {
                sc->thermal_param.th_cfg.levelconf[i].policy = th_config.levelconf[i].policy;
                /* only one policy THERMAL_POLICY_QUEUE_PAUSE is supported as of now */
                /* cfg_changed = TH_TRUE; */
            }
        }
        if (cfg_changed == TH_TRUE) {
            printk("Thermal Throttling flag set to : %d. \n",sc->thermal_param.th_cfg.enable);
            error = ath_config_thermal_mitigation_param(sc);
        }
    }
    return error;
}

int ath_ioctl_get_thermal_handler (struct ath_softc_net80211 *scn, caddr_t *param)
{
    struct thermal_param th_param;
    struct ath_softc *sc  =  ATH_DEV_TO_SC(scn->sc_dev);
    int error = 0;

    if (!sc->thermal_param.tt_support){
        TH_DEBUG_PRINT(TH_DEBUG_LVL0, sc, "%s: TT not supported \n", __func__);
        return -EINVAL;
    }
    OS_MEMCPY(&th_param, &sc->thermal_param, sizeof(th_param));
    error = _copy_to_user((caddr_t)param, &th_param, sizeof(th_param));
    if (error) {
        TH_DEBUG_PRINT(TH_DEBUG_LVL0, sc,"%s: Error in copying data to user space:\n", __func__);
        error = -EFAULT;
    }

    return error;
}

extern unsigned long ath_ioctl_debug;           /* defined in ah_osdep.c  */

struct ioctl_name_tbl {
    unsigned int ioctl;
    char *name;
};

static struct ioctl_name_tbl ioctl_names[] =
{
    {SIOCGATHSTATS, "SIOCGATHSTATS"},
    {SIOCGATHSTATSCLR,"SIOCGATHSTATSCLR"},
    {SIOCGATHPHYSTATS, "SIOCGATHPHYSTATS"},
    {SIOCGATHPHYSTATSCUR, "SIOCGATHPHYSTATSCUR"},
    {SIOCGATHDIAG, "SIOCGATHDIAG"},
#if defined(ATH_SUPPORT_DFS) || defined(ATH_SUPPORT_SPECTRAL)
    {SIOCGATHPHYERR,"SIOCGATHPHYERR"},
#endif
    {SIOCETHTOOL,"SIOCETHTOOL"},
    {SIOC80211IFCREATE, "SIOC80211IFCREATE"},
#ifdef ATH_TX99_DIAG
    {SIOCIOCTLTX99, "SIOCIOCTLTX99"},
#endif
    {0, NULL},
};

static char *find_ath_std_ioctl_name(int param)
{
    int i = 0;
    for (i = 0; ioctl_names[i].ioctl; i++) {
        if (ioctl_names[i].ioctl == param)
            return(ioctl_names[i].name ? ioctl_names[i].name : "UNNAMED");
    }
    return("Unknown IOCTL");
}

int ath_ucfg_create_vap(struct ath_softc_net80211 *scn, struct ieee80211_clone_params *cp, char *dev_name)
{
    struct net_device *dev = scn->sc_osdev->netdev;
    struct ifreq ifr;
    int status;

    if (scn->sc_in_delete) {
        return -ENODEV;
    }

    memset(&ifr, 0, sizeof(ifr));
    ifr.ifr_data = (void *) cp;

    scn->sc_osdev->vap_hardstart = osif_vap_hardstart;
    status = osif_ioctl_create_vap(dev, &ifr, cp, scn->sc_osdev);

    /* return final device name */
    if(strlcpy(dev_name, ifr.ifr_name, IFNAMSIZ) >= IFNAMSIZ) {
        qdf_print("%s:Interface Name too long.%s\n",__func__,ifr.ifr_name);
        return -1;
    }

    return status;
}

int wlan_get_vap_info(struct ieee80211vap *vap,
        struct ieee80211vap_profile *vap_profile,
        void *handle);
int ath_ucfg_get_vap_info(struct ath_softc_net80211 *scn,
                                struct ieee80211_profile *profile)
{
    struct net_device *dev = scn->sc_osdev->netdev;
    struct ieee80211com *ic = &scn->sc_ic;
    struct ieee80211vap *vap = NULL;
    struct ieee80211vap_profile *vap_profile;
    wlan_chan_t chan;

    if(strlcpy(profile->radio_name, dev->name, IFNAMSIZ) >= IFNAMSIZ) {
        qdf_print("%s:Device Name too long.%s\n",__func__,dev->name);
        return -1;
    }
    wlan_get_device_mac_addr(ic, profile->radio_mac);
    profile->cc = (u_int16_t)wlan_get_device_param(ic,
            IEEE80211_DEVICE_COUNTRYCODE);
    chan = wlan_get_dev_current_channel(ic);
    if (chan != NULL) {
        profile->channel = chan->ic_ieee;
        profile->freq = chan->ic_freq;
    }
    TAILQ_FOREACH(vap, &ic->ic_vaps, iv_next) {
        vap_profile = &profile->vap_profile[profile->num_vaps];
        wlan_get_vap_info(vap, vap_profile, (void *)scn->sc_osdev);
        profile->num_vaps++;
    }
    return 0;
}

#if UMAC_SUPPORT_ACFG
extern int acfg_handle_ath_ioctl(struct net_device *dev, void *data);
#endif

static int
ath_ioctl(struct net_device *dev, struct ifreq *ifr, int cmd)
{
    struct ath_softc_net80211 *scn = ath_netdev_priv(dev);
    struct ieee80211com *ic = &scn->sc_ic;
    enum ieee80211_phymode mode;
    struct ath_stats *as;
    struct ath_stats_container *asc = NULL;
    struct ath_phy_stats_container *apsc = NULL;
    struct ath_phy_stats *ps = NULL;
    struct ath_chan_stats chanstats;
    struct ath_mib_mac_stats mibstats;
    struct extended_ioctl_wrapper extended_cmd;
    WIRELESS_MODE wmode;
    int error=0;
    struct ath_diag ad;

    if (ath_ioctl_debug) {
        QDF_PRINT_INFO(ic->ic_print_idx, QDF_MODULE_ID_ANY, QDF_TRACE_LEVEL_INFO, "***%s:  dev=%s  ioctl=0%04X  name=%s\n", __func__, dev->name, cmd, find_ath_std_ioctl_name(cmd));
    }

    switch (cmd) {
    case SIOCGATHEACS:
#if ATH_SUPPORT_SPECTRAL
        error = osif_ioctl_eacs(dev, ifr, scn->sc_osdev);
#endif
        break;
    case SIOCGATHSTATS:
        if(((dev->flags & IFF_UP) == 0 ) || ((atomic_read( &ATH_DEV_TO_SC(scn->sc_dev)->sc_nap_vaps_up) <= 0) \
                    && (atomic_read( &ATH_DEV_TO_SC(scn->sc_dev)->sc_nsta_vaps_up) <= 0))){
            return -ENXIO;
        }

	asc = (struct ath_stats_container *)kmalloc(sizeof(struct ath_stats_container), GFP_KERNEL);
	if(asc == NULL) {
		error = -ENOMEM;
		break;
	}
	if(copy_from_user(asc, ifr->ifr_data, sizeof(struct ath_stats_container))) {
	    kfree(asc);
	    return -EFAULT;
	}
        if(asc->size == 0 || asc->address == NULL) {
           error = -EFAULT;
        }else {
            as = scn->sc_ops->get_ath_stats(scn->sc_dev);
            if (ATH_DEV_TO_SC(scn->sc_dev)->sc_nvaps > 0) {
                scn->sc_ops->ath_dev_get_chan_stats(scn->sc_dev, &chanstats);
                as->ast_chan_clr_cnt = chanstats.chan_clr_cnt;
                as->ast_cycle_cnt = chanstats.cycle_cnt;
                as->ast_noise_floor = scn->sc_ops->ath_dev_get_noisefloor(scn->sc_dev);
            } else {
                as->ast_chan_clr_cnt = 0;
                as->ast_cycle_cnt = 0;
                as->ast_noise_floor = 0;
            }
            scn->sc_ops->ath_update_mibMacstats(scn->sc_dev);
            scn->sc_ops->ath_get_mibMacstats(scn->sc_dev, &mibstats);
            memcpy((void *)&(as->ast_mib_stats), (void *)&mibstats,
                                     sizeof(struct ath_mib_mac_stats));
	    if(_copy_to_user(asc->address, as, sizeof(struct ath_stats)))
		error = -EFAULT;
	    else
		error = 0;
	}
        asc->offload_if = 0;
        asc->size = sizeof(struct ath_stats);

	if(copy_to_user(ifr->ifr_data, asc, sizeof(struct ath_stats_container)))
	    error = -EFAULT;
	else
	    error = 0;

	kfree(asc);
        break;
    case SIOCGATHSTATSCLR:
        as = scn->sc_ops->get_ath_stats(scn->sc_dev);
        memset(as, 0, sizeof(struct ath_stats));
        error = 0;
        break;
    case SIOCGATHPHYSTATS:
        if((( dev->flags & IFF_UP) == 0 ) || ((atomic_read( &ATH_DEV_TO_SC(scn->sc_dev)->sc_nap_vaps_up) <= 0) \
                    && (atomic_read( &ATH_DEV_TO_SC(scn->sc_dev)->sc_nsta_vaps_up) <= 0))){
            return -ENXIO;
        }
        ps = scn->sc_ops->get_phy_stats_allmodes(scn->sc_dev);
        if (_copy_to_user(ifr->ifr_data, ps,
                          sizeof(struct ath_phy_stats) * WIRELESS_MODE_MAX)) {
            error = -EFAULT;
        }
        else {
            error = 0;
        }
        break;
    case SIOCGATHPHYSTATSCUR:
        if(((dev->flags & IFF_UP) == 0) || ((atomic_read( &ATH_DEV_TO_SC(scn->sc_dev)->sc_nap_vaps_up) <= 0) \
                    &&  (atomic_read( &ATH_DEV_TO_SC(scn->sc_dev)->sc_nsta_vaps_up) <= 0))){
            return -ENXIO;
        }
        mode = ieee80211_get_current_phymode(ic);
        wmode = ath_ieee2wmode(mode);

        if (wmode == WIRELESS_MODE_MAX) {
            ASSERT(0);
            return 0;
        }

	apsc = (struct ath_phy_stats_container *)kmalloc(sizeof(struct ath_phy_stats_container), GFP_KERNEL);
	if(apsc == NULL) {
	    error = -ENOMEM;
	    break;
	}

	if(copy_from_user(apsc, ifr->ifr_data, sizeof(struct ath_phy_stats_container))) {
	    kfree(apsc);
	    return -EFAULT;
	}

        if (apsc->size == 0 || apsc->address == NULL) {
           error = -EFAULT;
        } else {
            ps = scn->sc_ops->get_phy_stats(scn->sc_dev, wmode);
	    if(_copy_to_user(apsc->address, ps, sizeof(struct ath_phy_stats)))
		error = -EFAULT;
	    else
		error = 0;
        }
	kfree(apsc);
        break;
    case SIOCGATHDIAG:
        if (!capable(CAP_NET_ADMIN))
            error = -EPERM;
        else {

	    if(copy_from_user(&ad, ifr->ifr_data, sizeof(ad))) {
		return -EFAULT;
	    }
            error = ath_ucfg_diag(scn, &ad);
	}
        break;
#if defined(ATH_SUPPORT_DFS) || defined(ATH_SUPPORT_SPECTRAL)
    case SIOCGATHPHYERR:
        if (!capable(CAP_NET_ADMIN)) {
            error = -EPERM;
        } else {

            if (__xcopy_from_user(&ad, ifr->ifr_data, sizeof(ad))) {
                return -EFAULT;
            }
            error = ath_ucfg_phyerr(scn, &ad);
        }
        break;
#endif
    case SIOCETHTOOL:
        if (__xcopy_from_user(&cmd, ifr->ifr_data, sizeof(cmd)))
            error = -EFAULT;
        else
            error = ath_ioctl_ethtool(scn, cmd, ifr->ifr_data);
        break;
    case SIOC80211IFCREATE:
	{
    	struct ieee80211_clone_params cp;
            if (scn->sc_in_delete) {
                return -ENODEV;
            }
            if (__xcopy_from_user(&cp, ifr->ifr_data, sizeof(cp))) {
       		return -EFAULT;
	    }
            error = ath_ucfg_create_vap(scn, &cp, ifr->ifr_name);
	}
        break;
    case SIOCGATHEXTENDED:
    {
        if (__xcopy_from_user(&extended_cmd, ifr->ifr_data, sizeof(extended_cmd))) {
            return (-EFAULT);
        }

        if (scn->sc_in_delete) {
            error = -ENODEV;
        }

        switch (extended_cmd.cmd) {

            case EXTENDED_SUBIOCTL_CHANNEL_SWITCH:
                error = ieee80211_extended_ioctl_chan_switch(dev, ic, extended_cmd.data);
                break;

            case EXTENDED_SUBIOCTL_CHANNEL_SCAN:
                error = ieee80211_extended_ioctl_chan_scan(dev, ic, extended_cmd.data);
                break;

            case EXTENDED_SUBIOCTL_REPEATER_MOVE:
                error = ieee80211_extended_ioctl_rep_move(dev, ic, extended_cmd.data);
                break;

#if ATH_SUPPORT_WRAP && DBDC_REPEATER_SUPPORT
            case EXTENDED_SUBIOCTL_GET_PRIMARY_RADIO:
                error = ath_ioctl_get_primary_radio(scn, extended_cmd.data);
                break;
            case EXTENDED_SUBIOCTL_GET_MPSTA_MAC_ADDR:
                error = ath_ioctl_get_mpsta_mac_addr(scn, extended_cmd.data);
                break;
            case EXTENDED_SUBIOCTL_DISASSOC_CLIENTS:
                ath_ioctl_disassoc_clients(scn);
                break;
            case EXTENDED_SUBIOCTL_GET_FORCE_CLIENT_MCAST:
                error = ath_ioctl_get_force_client_mcast(scn, extended_cmd.data);
                break;
            case EXTENDED_SUBIOCTL_GET_MAX_PRIORITY_RADIO:
                error = ath_ioctl_get_max_priority_radio(scn, extended_cmd.data);
                break;
#endif
            case EXTENDED_SUBIOCTL_IFACE_MGR_STATUS:
                ath_ioctl_iface_mgr_status(scn, extended_cmd.data);
                break;
            case EXTENDED_SUBIOCTL_GET_STAVAP_CONNECTION:
                error = ath_ioctl_get_stavap_connection(scn, extended_cmd.data);
                break;
            case EXTENDED_SUBIOCTL_GET_DISCONNECTION_TIMEOUT:
                error = ath_ioctl_get_disconnection_timeout(scn, extended_cmd.data);
                break;
#if DBDC_REPEATER_SUPPORT
            case EXTENDED_SUBIOCTL_GET_PREF_UPLINK:
                error = ath_ioctl_get_preferred_uplink(scn, extended_cmd.data);
                break;
#endif
            case EXTENDED_SUBIOCTL_THERMAL_SET_PARAM:
                error = ath_ioctl_set_thermal_handler(scn, extended_cmd.data);
                break;

            case EXTENDED_SUBIOCTL_THERMAL_GET_PARAM:
                error = ath_ioctl_get_thermal_handler(scn, extended_cmd.data);
                break;

            default:
                QDF_PRINT_INFO(ic->ic_print_idx, QDF_MODULE_ID_ANY, QDF_TRACE_LEVEL_INFO, "%s: unsupported extended command %d\n", __func__, extended_cmd.cmd);
                break;
        }
    }
        break;

#ifdef ATH_TX99_DIAG
    case SIOCIOCTLTX99:
        error = tx99_ioctl(dev, ATH_DEV_TO_SC(scn->sc_dev), cmd, ifr->ifr_data);
        break;
#endif
#ifdef ATH_SUPPORT_LINUX_VENDOR
    case SIOCDEVVENDOR:
        error = osif_ioctl_vendor(dev, ifr, 0);
        break;
#endif
#ifdef ATH_BUS_PM
    case SIOCSATHSUSPEND:
        {
            struct ieee80211com *ic = &scn->sc_ic;
            struct ieee80211vap *tmpvap;
            int val = 0;
            if (__xcopy_from_user(&val, ifr->ifr_data, sizeof(int)))
                return -EFAULT;

            if(val) {
              /* suspend only if all vaps are down */
                TAILQ_FOREACH(tmpvap, &ic->ic_vaps, iv_next) {
                    struct net_device *tmpdev = ((osif_dev *)tmpvap->iv_ifp)->netdev;
                    if (tmpdev->flags & IFF_RUNNING)
                        return -EBUSY;
                }
                error = bus_device_suspend(scn->sc_osdev);
                if (!error)
                    scn->sc_osdev->isDeviceAsleep = val;
            }
            else {
                int isDeviceAsleepSave = scn->sc_osdev->isDeviceAsleep;
                scn->sc_osdev->isDeviceAsleep = val;
                error = bus_device_resume(scn->sc_osdev);
                if (error)
                    scn->sc_osdev->isDeviceAsleep = isDeviceAsleepSave;
            }
        }
      break;
#endif /* ATH_BUS_PM */
	case SIOCG80211PROFILE:
        {
            struct ieee80211_profile *profile;

            profile = (struct ieee80211_profile *)kmalloc(
                            sizeof (struct ieee80211_profile), GFP_KERNEL);
            if (profile == NULL) {
                error = -ENOMEM;
                break;
            }
            OS_MEMSET(profile, 0, sizeof (struct ieee80211_profile));

            error = ath_ucfg_get_vap_info(scn, profile);

            error = _copy_to_user(ifr->ifr_data, profile,
                            sizeof (struct ieee80211_profile));

            kfree(profile);
            profile = NULL;
        }
        break;
    case SIOCGSETCTLPOW:
        {
            u_int8_t *ctl  = (u_int8_t *)OS_MALLOC(scn->sc_osdev,1024, GFP_KERNEL);
            u_int32_t i = 0;
            if (__xcopy_from_user(ctl, ifr->ifr_data, sizeof(ctl))) {
                OS_FREE(ctl);
                return -EFAULT;
            }
            QDF_PRINT_INFO(ic->ic_print_idx, QDF_MODULE_ID_ANY, QDF_TRACE_LEVEL_INFO, "ctl ioctl :\n");
            for (i = 0; i < 1024; i++) {
                if ((i % 16) == 15) {
                    QDF_PRINT_INFO(ic->ic_print_idx, QDF_MODULE_ID_ANY, QDF_TRACE_LEVEL_INFO, "\n");
                }
                QDF_PRINT_INFO(ic->ic_print_idx, QDF_MODULE_ID_ANY, QDF_TRACE_LEVEL_INFO, " %02X", ctl[i]);
            }
            QDF_PRINT_INFO(ic->ic_print_idx, QDF_MODULE_ID_ANY, QDF_TRACE_LEVEL_INFO, "\n");

            scn->sc_ops->set_ctl_pwr(scn->sc_dev, ctl + 4, *(u_int32_t *)ctl, 0);
            OS_FREE(ctl);

        }
        break;

#if UMAC_SUPPORT_ACFG
	 case ACFG_PVT_IOCTL:
        error = acfg_handle_ath_ioctl(dev, ifr->ifr_data);
        break;
#endif

    default:
        error = -EINVAL;
        break;
    }
	return error;
}

/*
 * Return netdevice statistics.
 */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 36)
static struct rtnl_link_stats64 *
ath_getstats(struct net_device *dev, struct rtnl_link_stats64* stats64)
#else
static struct net_device_stats *
ath_getstats(struct net_device *dev)
#endif
{
    struct ath_softc_net80211 *scn = ath_netdev_priv(dev);
    struct ath_stats *as;
    struct ath_phy_stats *ps;
    struct ath_11n_stats *ans;
    struct ath_dfs_stats *dfss;
    WIRELESS_MODE wmode;
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 36)
    struct net_device_stats *stats;
#else
    struct rtnl_link_stats64 * stats;
#endif

    stats = &scn->sc_osdev->devstats;
    as = scn->sc_ops->get_ath_stats(scn->sc_dev);
    ans = scn->sc_ops->get_11n_stats(scn->sc_dev);
    dfss = scn->sc_ops->get_dfs_stats(scn->sc_dev);
    /* update according to private statistics */
    stats->tx_errors = as->ast_tx_xretries
             + as->ast_tx_fifoerr
             + as->ast_tx_filtered
             ;
    stats->tx_dropped = as->ast_tx_nobuf
            + as->ast_tx_encap
            + as->ast_tx_nonode
            + as->ast_tx_nobufmgt;
    /* Add tx mgmt(includes beacon), tx, 11n tx */
    stats->tx_packets = as->ast_tx_mgmt
            + as->ast_tx_packets
            + ans->tx_pkts;
    /* 11n rx (rx mgmt is included) */
    stats->rx_packets = ans->rx_pkts;

    /* TX and RX bytes (includes mgmt.) */
    stats->tx_bytes = as->ast_tx_bytes;
    stats->rx_bytes = as->ast_rx_bytes;

    for (wmode = 0; wmode < WIRELESS_MODE_MAX; wmode++) {
        ps = scn->sc_ops->get_phy_stats(scn->sc_dev, wmode);

        stats->rx_errors = ps->ast_rx_fifoerr;
        stats->rx_dropped = ps->ast_rx_tooshort;
        stats->rx_crc_errors = ps->ast_rx_crcerr;
    }

#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 36)
    return stats;
#else
    memcpy(stats64, stats, sizeof(struct rtnl_link_stats64));
    return stats64;
#endif
}

static void
ath_tasklet(TQUEUE_ARG data)
{
    struct net_device *dev = (struct net_device *)data;
    struct ath_softc_net80211 *scn = ath_netdev_priv(dev);

    do_ath_handle_intr(scn->sc_dev);
}



irqreturn_t
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,19)
ath_isr_generic(int irq, void *dev_id)
#else
ath_isr_generic(int irq, void *dev_id, struct pt_regs *regs)
#endif
{
    struct net_device *dev = dev_id;
    struct ath_softc_net80211 *scn = ath_netdev_priv(dev);
    int sched, needmark = 0;

#if PCI_INTERRUPT_WAR_ENABLE
    scn->int_scheduled_cnt++;
#endif

    /* always acknowledge the interrupt */
    sched = scn->sc_ops->isr(scn->sc_dev);

    switch(sched)
    {
    case ATH_ISR_NOSCHED:
        return  IRQ_HANDLED;

    case ATH_ISR_NOTMINE:
        return IRQ_NONE;

    default:
        if ((dev->flags & (IFF_RUNNING|IFF_UP)) != (IFF_RUNNING|IFF_UP))
        {
            DPRINTF_INTSAFE(scn, ATH_DEBUG_INTR, "%s: flags 0x%x\n", __func__, dev->flags);
            scn->sc_ops->disable_interrupt(scn->sc_dev);     /* disable further intr's */
            return IRQ_HANDLED;
        }
    }

    /*
    ** See if the transmit queue processing needs to be scheduled
    */

    ATH_SCHEDULE_TQUEUE(&scn->sc_osdev->intr_tq, &needmark);
    if (needmark)
        mark_bh(IMMEDIATE_BH);

    return IRQ_HANDLED;
}

/*
Interrupt could missing on some 3rd party platform,
and cause stop beaconning, or loading FW fail.
This WAR would detect interrupt count periodically,
and do a chip reset when interrupt missing detected.
*/
#if PCI_INTERRUPT_WAR_ENABLE
static
OS_TIMER_FUNC(ath_int_status_poll_timer)
{
    struct net_device *dev;
    struct ath_softc_net80211 *scn;
    struct ath_softc *sc;
    OS_GET_TIMER_ARG(dev, struct net_device *);
    scn = ath_netdev_priv(dev);
    sc = ATH_DEV_TO_SC(scn->sc_dev);

    if (!sc->sc_nvaps || sc->sc_invalid || sc->sc_scanning || sc->sc_dfs_wait) {
        scn->pre_int_scheduled_cnt = 0;
        OS_SET_TIMER(&scn->int_status_poll_timer, scn->int_status_poll_interval);
        return;
    }

    /* check whether internal reset is necessary to recover from wifi interrupt stop */
    if (scn->pre_int_scheduled_cnt !=0 && (scn->pre_int_scheduled_cnt == scn->int_scheduled_cnt)) {
        ath_bstuck_tasklet(sc);
                ATH_CLEAR_HANGS(sc);
        QDF_PRINT_INFO(sc->sc_print_idx, QDF_MODULE_ID_ANY, QDF_TRACE_LEVEL_INFO, "%s: issue internal reset to recover wifi interrupt stop, count=%d\n",__func__,scn->int_scheduled_cnt);
    }

    scn->pre_int_scheduled_cnt = scn->int_scheduled_cnt;

    OS_SET_TIMER(&scn->int_status_poll_timer, scn->int_status_poll_interval);
    return;
}
#endif


irqreturn_t
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,19)
ath_isr(int irq, void *dev_id)
{
    do_ath_isr(irq,dev_id);
}
#else
ath_isr(int irq, void *dev_id, struct pt_regs *regs)
{
    do_ath_isr(irq,dev_id,regs);
}
#endif


#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,30)
static const struct net_device_ops athdev_net_ops = {
    .ndo_open    = ath_netdev_open,
    .ndo_stop    = ath_netdev_stop,
    .ndo_start_xmit = ath_netdev_hardstart,
    .ndo_set_mac_address = ath_netdev_set_macaddr,
    .ndo_tx_timeout = ath_netdev_tx_timeout,
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 36)
    .ndo_get_stats64 = ath_getstats,
#else
    .ndo_get_stats = ath_getstats,
#endif
    .ndo_change_mtu = ath_change_mtu,
#if LINUX_VERSION_CODE < KERNEL_VERSION(3,2,0)
    .ndo_set_multicast_list = ath_netdev_set_mcast_list,
#else
    .ndo_set_rx_mode = ath_netdev_set_mcast_list,
#endif
    .ndo_do_ioctl = ath_ioctl,
};
#endif

#if ATH_SUPPORT_FLOWMAC_MODULE
int8_t
ath_flowmac_pause_wifi (struct net_device *dev, u_int8_t pause, u_int32_t duration)
{
    return 0;
}

int8_t
ath_flowmac_ratelimit_wifi(struct net_device *dev, flow_dir_t dir,
                void *rate)
{
    return 0;
}

int
ath_flowmac_notify_wifi(struct net_device *dev, flowmac_event_t event,
                void *event_data)
{
    struct ath_softc_net80211 *scn = ath_netdev_priv(dev);
    struct ath_softc *sc = ATH_DEV_TO_SC(scn->sc_dev);

    if (event == FLOWMAC_ENABLE) {
        QDF_PRINT_INFO(sc->sc_print_idx, QDF_MODULE_ID_ANY, QDF_TRACE_LEVEL_INFO, "%s Flowmac is enabled\n",__func__);
        sc->sc_osnetif_flowcntrl = 1;
    } else if (event == FLOWMAC_DISABLE)  {
        QDF_PRINT_INFO(sc->sc_print_idx, QDF_MODULE_ID_ANY, QDF_TRACE_LEVEL_INFO, "%s flowmac is disabled \n", __func__);
        sc->sc_osnetif_flowcntrl = 0;
    }
    sc->sc_ieee_ops->notify_flowmac_state(sc->sc_ieee, sc->sc_osnetif_flowcntrl);
    return 0;
}

int
ath_flowmac_attach(ath_dev_t dev)
{
    flow_ops_t ops;
    struct ath_softc *sc = ATH_DEV_TO_SC(dev);

    if (!dev) {
        QDF_PRINT_INFO(sc->sc_print_idx, QDF_MODULE_ID_ANY, QDF_TRACE_LEVEL_INFO, "%s dev pointer is wrong \n", __func__);
        return -EINVAL;
    }
    /* regiser with the flow mac driver */
    ops.pause = ath_flowmac_pause_wifi;
    ops.rate_limit  = ath_flowmac_ratelimit_wifi;
    ops.notify_event = ath_flowmac_notify_wifi;

    sc->sc_flowmac_dev = flowmac_register(sc->sc_osdev->netdev, &ops, 0, 0, 0, 0);
    if (sc->sc_flowmac_dev) {
        QDF_PRINT_INFO(sc->sc_print_idx, QDF_MODULE_ID_ANY, QDF_TRACE_LEVEL_INFO, "%s flowmac registration is successful %pK\n",
                __func__, sc->sc_flowmac_dev);
        return 0;
    }
    return -EINVAL;

}
EXPORT_SYMBOL(ath_flowmac_attach);
int
ath_flowmac_detach(ath_dev_t dev)
{
    struct ath_softc *sc = ATH_DEV_TO_SC(dev);

    if (!dev) {
        QDF_PRINT_INFO(sc->sc_print_idx, QDF_MODULE_ID_ANY, QDF_TRACE_LEVEL_INFO, "%s dev pointer is wrong \n", __func__);
        return -EINVAL;
    }
    if (sc->sc_flowmac_dev) {
        flowmac_unregister(sc->sc_flowmac_dev);
        sc->sc_flowmac_dev = NULL;
    }
    return 0;
}
#endif

int
osif_vap_hardstart(struct sk_buff *skb, struct net_device *dev)
{
    do_osif_vap_hardstart(skb,dev);
    return 0;
}

int
__ath_attach(u_int16_t devid, struct net_device *dev, QDF_BUS_CONTEXT *bus_context, osdev_t osdev)
{
    struct ath_softc_net80211 *scn = ath_netdev_priv(dev);
    struct ieee80211com *ic = &scn->sc_ic;
    int error = 0;
    struct hal_reg_parm hal_conf_parm;
    hal_conf_parm.calInFlash = bus_context->cal_in_flash;

    QDF_PRINT_INFO(ic->ic_print_idx, QDF_MODULE_ID_ANY, QDF_TRACE_LEVEL_INFO, "%s: Set global_scn[%d]\n", __func__, num_global_scn);
    global_scn[num_global_scn++] = scn;
#if ATH_DEBUG
    ath_params.ACBKMinfree = ath_params.ACBKMinfree == ACBKMinfree ? ath_params.ACBKMinfree : ACBKMinfree;
    ath_params.ACBEMinfree = ath_params.ACBEMinfree == ACBEMinfree ? ath_params.ACBEMinfree : ACBEMinfree;
    ath_params.ACVIMinfree = ath_params.ACVIMinfree == ACVIMinfree ? ath_params.ACVIMinfree : ACVIMinfree;
    ath_params.ACVOMinfree = ath_params.ACVOMinfree == ACVOMinfree ? ath_params.ACVOMinfree : ACVOMinfree;
    ath_params.CABMinfree = ath_params.CABMinfree == CABMinfree ? ath_params.CABMinfree : CABMinfree;
    ath_params.UAPSDMinfree = ath_params.UAPSDMinfree == UAPSDMinfree ? ath_params.UAPSDMinfree : UAPSDMinfree;
    QDF_PRINT_INFO(ic->ic_print_idx, QDF_MODULE_ID_ANY, QDF_TRACE_LEVEL_INFO, "*** All the minfree values should be <= ATH_TXBUF-32, otherwise default value will be used instead ***\n");
    QDF_PRINT_INFO(ic->ic_print_idx, QDF_MODULE_ID_ANY, QDF_TRACE_LEVEL_INFO, "ACBKMinfree = %d\n", ath_params.ACBKMinfree);
    QDF_PRINT_INFO(ic->ic_print_idx, QDF_MODULE_ID_ANY, QDF_TRACE_LEVEL_INFO, "ACBEMinfree = %d\n", ath_params.ACBEMinfree);
    QDF_PRINT_INFO(ic->ic_print_idx, QDF_MODULE_ID_ANY, QDF_TRACE_LEVEL_INFO, "ACVIMinfree = %d\n", ath_params.ACVIMinfree);
    QDF_PRINT_INFO(ic->ic_print_idx, QDF_MODULE_ID_ANY, QDF_TRACE_LEVEL_INFO, "ACVOMinfree = %d\n", ath_params.ACVOMinfree);
    QDF_PRINT_INFO(ic->ic_print_idx, QDF_MODULE_ID_ANY, QDF_TRACE_LEVEL_INFO, "CABMinfree = %d\n", ath_params.CABMinfree);
    QDF_PRINT_INFO(ic->ic_print_idx, QDF_MODULE_ID_ANY, QDF_TRACE_LEVEL_INFO, "UAPSDMinfree = %d\n", ath_params.UAPSDMinfree);
    QDF_PRINT_INFO(ic->ic_print_idx, QDF_MODULE_ID_ANY, QDF_TRACE_LEVEL_INFO, "ATH_TXBUF=%d\n", ATH_TXBUF);
#elif QCA_AIRTIME_FAIRNESS
    QDF_PRINT_INFO(ic->ic_print_idx, QDF_MODULE_ID_ANY, QDF_TRACE_LEVEL_INFO, "ATH_TXBUF=%d\n", ATH_TXBUF);
#endif

    /* show that no dedicated amem instance has been created yet */
    scn->amem.handle = NULL;
    /*
     * create and initialize ath layer
     */
#if ATH_SUPPORT_FLOWMAC_MODULE
    ath_params.osnetif_flowcntrl = 0; /* keep it disabled by default */
    ath_params.os_ethflowmac_enable = 0; /* keep it disabled by default */
#endif

#if defined(QCA_PARTNER_PLATFORM) && !QCA_PARTNER_CBM_DIRECTPATH
    ath_pltfrm_init( dev );
#endif
    /* initialize the qdf_dev handle */
    scn->qdf_dev = kmalloc(sizeof(*(osdev->qdf_dev)),
                             GFP_KERNEL);

    if (scn->qdf_dev == NULL) {
        goto bad;
    }
    osdev->qdf_dev = scn->qdf_dev;

    memset(scn->qdf_dev, 0, sizeof(*(scn->qdf_dev)));
    scn->qdf_dev->drv = osdev;
    scn->qdf_dev->drv_hdl = osdev->bdev; /* bus handle */
    scn->qdf_dev->dev = osdev->device; /* device */

    /*
     * 1. Resolving name to avoid a crash in request_irq() on new kernels
     * 2. Getting device name early to save it to bus context for Caldata in file solution
     */
    dev_alloc_name(dev, dev->name);

#ifdef QCA_PARTNER_PLATFORM
    ath_pltfrm_init( dev );
#endif

#ifdef AH_CAL_IN_FILE_HOST
    /* Need this only for flash mode*/
#ifdef HOST_OFFLOAD
    if(!hif_rom_mode())
#endif
    {
    	bus_context->bc_tag = (char *)(dev->name);
    }
#endif
    /*
     * Set bus context to osdev
     */
    osdev->bc = *bus_context;
    error = ath_attach(devid, bus_context, scn, osdev, &ath_params, &hal_conf_parm, &wlan_reg_params);

    if (error != 0)
        goto bad1;

#ifndef REMOVE_PKT_LOG
    if(enable_pktlog_support) {
        osif_hal_log_ani_callback_register((void*)ath_hal_log_ani_callback_register);
        scn->pl_dev->name = dev->name;
    }
#endif

#if ATH_BAND_STEERING
    if(EOK != ath_band_steering_netlink_init()) {
        QDF_PRINT_INFO(ic->ic_print_idx, QDF_MODULE_ID_ANY, QDF_TRACE_LEVEL_INFO, "Band steering socket init failed __investigate__\n");
    }
#endif

#if ATH_PARAMETER_API
    if(EOK != ath_papi_netlink_init()) {
        QDF_PRINT_INFO(ic->ic_print_idx, QDF_MODULE_ID_ANY, QDF_TRACE_LEVEL_INFO, "Parameter API socket init failed __investigate__\n");
    }
#endif

    ath_adhoc_netlink_init();

#if ATH_RXBUF_RECYCLE
    ath_rxbuf_recycle_init(osdev);
#endif

    ald_init_netlink();

#if UMAC_SUPPORT_ACFG
    acfg_event_netlink_init();
#endif

    osif_attach(dev);
    /* For STA Mode default CWM mode is Auto */
    if ( ic->ic_opmode == IEEE80211_M_STA)
        ic->ic_cwm_set_mode(ic, IEEE80211_CWM_MODE2040);

#if 0 /* TBD: is it taken care in UMAC ? */
    /*
     * This will be used in linux client while processing
     * country ie in 11d beacons
     */
    ic->ic_ignore_11dbeacon = 0;

    /*
     * commommode is used while determining the channel power
     * in standard client mode
     */
    ic->ic_commonmode = ic->ic_country.isMultidomain;

#endif

    /*
     * initialize tx/rx engine
     */
    error = scn->sc_ops->tx_init(scn->sc_dev, ATH_TXBUF);
    if (error != 0)
        goto bad2;

    error = scn->sc_ops->rx_init(scn->sc_dev, ATH_RXBUF);
    if (error != 0)
        goto bad3;

    /*
     * setup net device
     */
#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,30)
    dev->netdev_ops = &athdev_net_ops;
#else
    dev->open = ath_netdev_open;
    dev->stop = ath_netdev_stop;
    dev->hard_start_xmit = ath_netdev_hardstart;
    dev->set_mac_address = ath_netdev_set_macaddr;
    dev->tx_timeout = ath_netdev_tx_timeout;
    dev->set_multicast_list = ath_netdev_set_mcast_list;
    dev->do_ioctl = ath_ioctl;
    dev->get_stats = ath_getstats;
    dev->change_mtu = ath_change_mtu;
#endif
    dev->watchdog_timeo = 5 * HZ;           /* XXX */
    dev->tx_queue_len = ATH_TXBUF-1;        /* 1 for mgmt frame */
#ifdef USE_HEADERLEN_RESV
    dev->hard_header_len += sizeof (struct ieee80211_qosframe) + sizeof(struct llc) + IEEE80211_ADDR_LEN + IEEE80211_WEP_IVLEN + IEEE80211_WEP_KIDLEN;
#endif

    /*
    ** Attach the iwpriv handlers
    */

    ath_iw_attach(dev);

    /*
     * setup interrupt serivce routine
     */

#ifdef ADF_SUPPORT
    ATH_INIT_TQUEUE(&osdev->intr_tq, (qdf_defer_fn_t)ath_tasklet, (void*)dev);
#else
   ATH_INIT_TQUEUE(&osdev->intr_tq, ath_tasklet, dev);
#endif

#if OS_SUPPORT_ASYNC_Q
   OS_MESGQ_INIT(osdev, &osdev->async_q, sizeof(os_async_q_mesg),
        OS_ASYNC_Q_MAX_MESGS,os_async_mesg_handler,  osdev,MESGQ_PRIORITY_NORMAL,MESGQ_ASYNCHRONOUS_EVENT_DELIVERY);
#endif


#if defined(QCA_PARTNER_PLATFORM) && !QCA_PARTNER_CBM_DIRECTPATH
    error = osif_pltfrm_interupt_register(dev, DIRECT_TYPE, ath_isr, NULL);
#else
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,20)
    error = request_irq(dev->irq, ath_isr, SA_SHIRQ | SA_SAMPLE_RANDOM, dev->name, dev);
#else
    if (bus_context->bc_bustype == QDF_BUS_TYPE_AHB) {
        error = request_irq(dev->irq, ath_isr, IRQF_DISABLED, dev->name, dev);
    } else {
        error = request_irq(dev->irq, ath_isr, IRQF_SHARED, dev->name, dev);
    }
#endif

#endif
    if (error)
    {
        QDF_PRINT_INFO(ic->ic_print_idx, QDF_MODULE_ID_ANY, QDF_TRACE_LEVEL_INFO, KERN_WARNING "%s: request_irq failed\n", dev->name);
        error = -EIO;
        goto bad4;
    }

#if PCI_INTERRUPT_WAR_ENABLE
    {
        QDF_PRINT_INFO(ic->ic_print_idx, QDF_MODULE_ID_ANY, QDF_TRACE_LEVEL_INFO, KERN_INFO "%s : Interrupt WA timer is enabled\n", __func__);
        scn->int_status_poll_interval = 200; //ms
        OS_INIT_TIMER(scn->sc_osdev, &scn->int_status_poll_timer, ath_int_status_poll_timer, dev);
        OS_SET_TIMER(&scn->int_status_poll_timer, 1000); /* start polling after 1 sec */
    }
#endif
    /* Kernel 2.6.25 needs valid dev_addr before  register_netdev */
    IEEE80211_ADDR_COPY(dev->dev_addr,ic->ic_myaddr);

    /*
     * finally register netdev and ready to go
     */
    if ((error = register_netdev(dev)) != 0) {
        QDF_PRINT_INFO(ic->ic_print_idx, QDF_MODULE_ID_ANY, QDF_TRACE_LEVEL_INFO, KERN_ERR "%s: unable to register device\n", dev->name);
        goto bad5;
    }
#if ATH_SSID_STEERING
    if(EOK != ath_ssid_steering_netlink_init()) {
        QDF_PRINT_INFO(ic->ic_print_idx, QDF_MODULE_ID_ANY, QDF_TRACE_LEVEL_INFO, "SSID steering socket init failed __investigate__\n");
        goto bad4;
    }
#endif

#if !NO_SIMPLE_CONFIG
    /* Request Simple Config intr handler */
    register_simple_config_callback (dev->name, (void *) jumpstart_intr, (void *) dev,
                                     (void *)&osdev->sc_push_button_dur);
#endif
    if (qdf_vfs_set_file_attributes(&dev->dev.kobj, &wifi_attr_group)) {
        QDF_PRINT_INFO(ic->ic_print_idx, QDF_MODULE_ID_ANY, QDF_TRACE_LEVEL_INFO, "%s: sysfs_create_group failed\n", __func__);
    }

#ifdef ATH_SUPPORT_LINUX_STA
#ifdef CONFIG_SYSCTL
    ath_dynamic_sysctl_register(ATH_DEV_TO_SC(scn->sc_dev));
#endif
#endif

#if ATH_SUPPORT_FLOWMAC_MODULE
    ath_flowmac_attach(scn->sc_dev);
#endif

    error = ath_thermal_mitigation_attach(scn, dev);
    if (error) {
        printk("%s: unable to attach TT\n", dev->name);
        goto bad5;
    }
    return 0;

bad5:
    error = ath_thermal_mitigation_detach(scn, dev);
    if (error) {
        QDF_PRINT_INFO(ic->ic_print_idx, QDF_MODULE_ID_ANY, QDF_TRACE_LEVEL_INFO, "%s: unable to detach Thermal throttling since sc in null.\n", __func__);
    }
    qdf_vfs_clear_file_attributes(&dev->dev.kobj, &wifi_attr_group);
#if defined(QCA_PARTNER_PLATFORM) && !QCA_PARTNER_CBM_DIRECTPATH
    osif_pltfrm_interupt_unregister(dev,0);
    ath_pltfrm_exit(dev);
#else
    free_irq(dev->irq, dev);
#endif
bad4:
    scn->sc_ops->rx_cleanup(scn->sc_dev);
bad3:
    scn->sc_ops->tx_cleanup(scn->sc_dev);
bad2:

    osif_detach(dev);

#if UMAC_SUPPORT_ACFG
    acfg_event_netlink_delete();
#endif

    ald_destroy_netlink();

#if ATH_RXBUF_RECYCLE
    ath_rxbuf_recycle_destroy(osdev);
#endif /* ATH_RXBUF_RECYCLE */

    ath_adhoc_netlink_delete();

#if ATH_BAND_STEERING
    ath_band_steering_netlink_delete();
#endif

#if ATH_SSID_STEERING
    ath_ssid_steering_netlink_delete();
#endif

#if ATH_PARAMETER_API
    ath_papi_netlink_delete();
#endif

    ath_detach(scn);
bad1:
    if(scn->qdf_dev) {
        OS_FREE(scn->qdf_dev);
    }
bad:
    global_scn[--num_global_scn] = NULL;
    return error;
}

int
__ath_detach(struct net_device *dev)
{
    struct ath_softc_net80211 *scn = ath_netdev_priv(dev);
    struct ieee80211com *ic = &scn->sc_ic;
    int ret, i, error = 0;

    /* == Fixed crash removing umac.ko while downloading target firmware */
    if (!scn->sc_dev) {
        return -EINVAL;
    }
    rtnl_lock();
#ifdef CONFIG_COMCERTO_CUSTOM_SKB_LAYOUT
    qdf_device_remove_file(&dev->dev, &dev_attr_custom_skb_enable);
#endif
    ath_vap_delete_on_rmmod(scn);

    osif_detach(dev);

    if (dev->irq)
    {
#if defined(QCA_PARTNER_PLATFORM) && !QCA_PARTNER_CBM_DIRECTPATH
        osif_pltfrm_interupt_unregister(dev,0);
	ath_pltfrm_exit(dev);
#else
        free_irq(dev->irq, dev);
#endif
    }

#ifdef ATH_SUPPORT_LINUX_STA
#ifdef CONFIG_SYSCTL
    ath_dynamic_sysctl_unregister(ATH_DEV_TO_SC(scn->sc_dev));
#endif
#endif

#ifndef NO_SIMPLE_CONFIG
    unregister_simple_config_callback(dev->name);
#endif
    error = ath_thermal_mitigation_detach(scn, dev);
    if (error) {
        QDF_PRINT_INFO(ic->ic_print_idx, QDF_MODULE_ID_ANY, QDF_TRACE_LEVEL_INFO, "%s: unable to detach Thermal throttling since sc in null.\n", __func__);
    }
    qdf_vfs_clear_file_attributes(&dev->dev.kobj, &wifi_attr_group);
    unregister_netdevice(dev);

#if OS_SUPPORT_ASYNC_Q
   OS_MESGQ_DRAIN(&scn->sc_osdev->async_q,NULL);
   OS_MESGQ_DESTROY(&scn->sc_osdev->async_q);
#endif
    scn->sc_ops->rx_cleanup(scn->sc_dev);
    scn->sc_ops->tx_cleanup(scn->sc_dev);
#if PCI_INTERRUPT_WAR_ENABLE
    OS_CANCEL_TIMER(&scn->int_status_poll_timer);
    OS_FREE_TIMER(&scn->int_status_poll_timer);
#endif


    ath_adhoc_netlink_delete();

#if ATH_BAND_STEERING
    ath_band_steering_netlink_delete();
#endif

#if ATH_SSID_STEERING
    ath_ssid_steering_netlink_delete();
#endif

#if ATH_PARAMETER_API
    ath_papi_netlink_delete();
#endif

#if UMAC_SUPPORT_ACFG
    acfg_event_netlink_delete();
#endif

#if ATH_RXBUF_RECYCLE
    ath_rxbuf_recycle_destroy(scn->sc_osdev);
#endif /* ATH_RXBUF_RECYCLE */

    ald_destroy_netlink();

#if ATH_SUPPORT_FLOWMAC_MODULE
    ath_flowmac_detach(scn->sc_dev);
#endif

#ifndef REMOVE_PKT_LOG
    if(enable_pktlog_support) {
        osif_hal_log_ani_callback_register(NULL);
    }
#endif

    ret = ath_detach(scn);

    if(scn->qdf_dev) {
        OS_FREE(scn->qdf_dev);
        scn->qdf_dev = NULL;
    }

    for (i = 0; i < num_global_scn; i++) {
        if(global_scn[i] == scn){
            global_scn[i] = NULL;
            break;
        }
    }
    num_global_scn--;

    rtnl_unlock();
    return ret;
}

int
__ath_remove_interface(struct net_device *dev)
{
    struct ath_softc_net80211 *scn = ath_netdev_priv(dev);
    struct ieee80211com *ic = &scn->sc_ic;
    struct ieee80211vap *vap;
    struct ieee80211vap *vapnext;
    osif_dev  *osifp;
    struct net_device *netdev;

    vap = TAILQ_FIRST(&ic->ic_vaps);
    while (vap != NULL) {
        /* osif_ioctl_delete_vap() destroy vap->iv_next information,
        so need to store next VAP address in vapnext */
        vapnext = TAILQ_NEXT(vap, iv_next);
        osifp = (osif_dev *)vap->iv_ifp;
        netdev = osifp->netdev;
        QDF_PRINT_INFO(ic->ic_print_idx, QDF_MODULE_ID_ANY, QDF_TRACE_LEVEL_INFO, "Remove interface on %s\n",netdev->name);
        rtnl_lock();
        dev_close(netdev);
        osif_ioctl_delete_vap(netdev);
        rtnl_unlock();
        vap = vapnext;
    }
    return 0;
}

void
__ath_set_delete(struct net_device *dev)
{
}

int
__ath_suspend(struct net_device *dev)
{
    return ath_netdev_stop(dev);
}

int
__ath_resume(struct net_device *dev)
{
    return ath_netdev_open(dev);
}
#if !NO_SIMPLE_CONFIG
/*
 * Handler for front panel SW jumpstart switch
 */
static irqreturn_t
jumpstart_intr (int cpl, void *dev_id, struct pt_regs *regs, void *push_time)
{
    struct net_device *dev = dev_id;
    struct ath_softc_net80211 *scn = ath_netdev_priv(dev);
    struct ieee80211com *ic = &scn->sc_ic;
    struct ieee80211vap *vap;
    u_int32_t           push_duration;
    int is_ap_vap_notified = 0;
    /*
    ** Iterate through all VAPs, since any of them may have WPS enabled
    */

    vap = TAILQ_FIRST(&ic->ic_vaps);
    while (vap != NULL) {
        if (push_time) {
            push_duration = *(u_int32_t *)push_time;
        } else {
            push_duration = 0;
        }
#if ATH_SUPPORT_HYFI_ENHANCEMENTS
        if (!ieee80211_vap_nopbn_is_set(vap))
        {
#endif
            /* Since we are having single physical push button on device ,
               and for push button + muti bss combination mode we will be have limitations,
               we designed that,physical push button notification will be sent to
               first AP vap(main BSS) and all sta vaps.*/
            if( vap->iv_opmode != IEEE80211_M_HOSTAP || is_ap_vap_notified == 0 ){
                QDF_PRINT_INFO(ic->ic_print_idx, QDF_MODULE_ID_ANY, QDF_TRACE_LEVEL_INFO, "SC Pushbutton Notify on %s for %d sec(s) and the vap %pK dev %pK:\n",dev->name,
                        push_duration, vap, (struct net_device *)(((osif_dev *)vap->iv_ifp)->netdev));
                osif_notify_push_button ((((osif_dev *)vap->iv_ifp)->os_handle), push_duration);
                if(vap->iv_opmode == IEEE80211_M_HOSTAP)
                    is_ap_vap_notified = 1;
            }
#if ATH_SUPPORT_HYFI_ENHANCEMENTS
        }
#endif
        vap = TAILQ_NEXT(vap, iv_next);
    }
    return IRQ_HANDLED;
}
#endif

#if OS_SUPPORT_ASYNC_Q
static void os_async_mesg_handler( void  *ctx, u_int16_t  mesg_type, u_int16_t  mesg_len, void  *mesg )
{
    if (mesg_type == OS_SCHEDULE_ROUTING_MESG_TYPE) {
        os_schedule_routing_mesg  *s_mesg = (os_schedule_routing_mesg *) mesg;
        s_mesg->routine(s_mesg->context, NULL);
    }
}
#endif

void
init_wlan(void)
{

}

#ifdef ATH_USB

#define IS_UP(_dev) \
    (((_dev)->flags & (IFF_RUNNING|IFF_UP)) == (IFF_RUNNING|IFF_UP))

static int ath_reinit_interface(struct net_device *dev)
{
    struct ath_softc_net80211 *scn = ath_netdev_priv(dev);
    struct ieee80211com *ic = &scn->sc_ic;
    struct ieee80211vap *vap;
    struct ieee80211vap *vapnext;
    osif_dev  *osifp;
    struct net_device *netdev;

    vap = TAILQ_FIRST(&ic->ic_vaps);
    while (vap != NULL) {
        vapnext = TAILQ_NEXT(vap, iv_next);
        osifp = (osif_dev *)vap->iv_ifp;
        netdev = osifp->netdev;
        rtnl_lock();
        if (IS_UP(netdev)) {
            dev_close(netdev);
            msleep(50);
            dev_open(netdev);
        }
        rtnl_unlock();
        vap = vapnext;
    }

    return 0;
}

void ath_usb_vap_restart(struct net_device *dev)
{
    struct ath_softc_net80211 *scn = ath_netdev_priv(dev);

    cmpxchg((int*)(&(scn->sc_dev_enabled)), 1, 0);
    ath_netdev_open(dev);
    ath_reinit_interface(dev);
}

EXPORT_SYMBOL(__ath_detach);
EXPORT_SYMBOL(init_wlan);
EXPORT_SYMBOL(__ath_remove_interface);
EXPORT_SYMBOL(__ath_attach);
EXPORT_SYMBOL(ath_usb_vap_restart);
EXPORT_SYMBOL(__ath_set_delete);
#endif

