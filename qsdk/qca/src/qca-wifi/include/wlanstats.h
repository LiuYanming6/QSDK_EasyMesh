/*
 * Copyright (c) 2018 Qualcomm Innovation Center, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Innovation Center, Inc.
 */

#ifdef ATH_SUPPORT_RDKB
#ifndef _WLANSTATS_KAPI_H
#define _WLANSTATS_KAPI_H
#include<qdf_types.h>
#include <qdf_nbuf.h>
#include <qdf_util.h>

struct ieee80211com;
struct ieee80211_node;

QDF_DECLARE_EWMA(rx_rssi, 1024, 8)
QDF_DECLARE_EWMA(tx_lag, 1024, 8)

struct wlanstats_q_util {
    uint64_t q[WLANSTATS_MAX_Q_UTIL];
    uint64_t cnt;
};

struct wlanstats_ctx {
    struct wlanstats_uapi_ioctl req;
    struct wlanstats_q_util q_util;
    qdf_spinlock_t lock;
    uint64_t next_peer_cookie;
    struct wlanstats_cmn_rate_info ri;
    uint64_t rx_ampdu_len;
    uint64_t rx_duration_bss;
    bool svc[WLAN_UAPI_IOCTL_SVC_MAX];
};

struct wlan_rx_stats {
    uint32_t num_bytes;
    uint32_t num_msdus;
    uint32_t num_mpdus;
    uint32_t num_retries;

    struct {
        uint32_t num_bytes;
        uint32_t num_msdus;
        uint32_t num_mpdus;
        uint32_t num_ppdus;
        uint32_t num_retries;
        uint32_t num_sgi;
        struct ewma_rx_rssi ave_rssi;
        struct ewma_rx_rssi ave_rssi_ant[WLANSTATS_MAX_RSSI_ANT][WLANSTATS_MAX_RSSI_HT];
    } stats[WLANSTATS_MAX_ALL];
};

struct wlan_tx_stats {
    struct {
        uint32_t attempts;
        uint32_t success;
        uint32_t ppdus;
    } stats[WLANSTATS_MAX_ALL];
    struct {
        struct ewma_tx_lag ave_sojourn_msec;
        uint64_t sum_sojourn_msec;
        uint32_t num_sojourn_mpdus;
    } sojourn[WLANSTATS_MAX_TID];
};

int wlanstats_init(struct wlanstats_ctx *ctx);
int wlanstats_ioctl(struct net_device *dev,
        struct iw_request_info *info,
        union iwreq_data *wrqu,
        char *extra);
void wlanstats_peer_init(struct ieee80211com *ic,
        struct ieee80211_node *ni);
void wlanstats_tx_stats_collect_ar98xx(struct ieee80211com *ic,
        struct ieee80211_node *ni,
        uint32_t *ppdu_start,
        uint32_t *ppdu_end);
void wlanstats_sojourn_collect(struct ieee80211com *ic,
        struct ieee80211_node *ni,
        int tidno,
        qdf_nbuf_t netbuf);
bool wlanstats_svc_is_enabled(struct ieee80211com *ic,
        enum wlanstats_uapi_ioctl_svc svc);
#endif
#endif // ATH_SUPPORT_RDKB
