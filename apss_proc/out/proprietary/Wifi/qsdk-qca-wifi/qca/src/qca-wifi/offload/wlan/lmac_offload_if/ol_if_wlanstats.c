/*
 * Copyright (c) 2018 Qualcomm Innovation Center, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Innovation Center, Inc.
 */

#ifdef ATH_SUPPORT_RDKB

#include <linux/average.h>
#include <linux/types.h>
#include <linux/bug.h>
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,2,0)
# include <linux/export.h>
#else
# include <linux/module.h>
#endif
#include <qdf_lock.h>
#include <net/iw_handler.h>
#include <wlanstats_uapi.h>
#include <wlanstats.h>
#include <ieee80211_var.h>
#include <ol_if_athvar.h>
#include <if_athrate.h>
QDF_COMPILE_TIME_ASSERT(qdf_array_q_size,
        sizeof(((struct wlanstats_uapi_ioctl *)0)->u.q_util.get.q) ==
        sizeof(((struct wlanstats_ctx *)0)->q_util.q));

QDF_COMPILE_TIME_ASSERT(qdf_tx_stats_size,
		sizeof(((struct wlanstats_uapi_ioctl *)0)->u.peer_tx_stats.get.stats) ==
		sizeof(((struct wlan_tx_stats *)0)->stats));

static int wlan_ioctl_get_q_util(struct net_device *dev,
        struct iw_request_info *info,
        union iwreq_data *wrqu,
        char *extra,
        struct wlanstats_uapi_ioctl *cmd)
{
    struct ieee80211com *ic;
    struct wlanstats_ctx *ctx;

    ic = ath_netdev_priv(dev);
    ctx = &ic->wlanstats_ctx;

    BUILD_BUG_ON(ARRAY_SIZE(cmd->u.q_util.get.q) !=
            ARRAY_SIZE(ctx->q_util.q));

    cmd->u.q_util.get.cnt = ctx->q_util.cnt;
    qdf_mem_copy(cmd->u.q_util.get.q,
            ctx->q_util.q,
            sizeof(cmd->u.q_util.get.q));

    return 0;
}

static struct ieee80211_node *
wlanstats_find_node(struct ieee80211_node_table *nt,
        const uint8_t addr[ETH_ALEN])
{
    struct ieee80211_node *ni;
    int hash;

    hash = IEEE80211_NODE_HASH(addr);
    LIST_FOREACH(ni, &nt->nt_hash[hash], ni_hash)
        if (IEEE80211_ADDR_EQ(ni->ni_macaddr, addr))
            return ni;

    return NULL;
}

static int wlan_ioctl_get_rx_stats(struct net_device *dev,
        struct iw_request_info *info,
        union iwreq_data *wrqu,
        char *extra,
        struct wlanstats_uapi_ioctl *cmd)
{
    struct ieee80211com *ic;
    struct ieee80211_node *ni;
    struct ieee80211_node_table *nt;
    struct wlan_rx_stats *src;
    rwlock_state_t lock_state;
    int err;
    int i,j,k;

    ic = ath_netdev_priv(dev);
    nt = &ic->ic_sta;

    ni = ieee80211_find_node(nt, cmd->u.peer_rx_stats.set.addr);
    if (!ni) {
        err = -ENOENT;
	return err;
    }

    src = ni->rx_stats;

    BUILD_BUG_ON(ARRAY_SIZE(cmd->u.peer_rx_stats.get.stats) !=
            ARRAY_SIZE(src->stats));

    qdf_mem_set(&cmd->u.peer_rx_stats.get, sizeof(cmd->u.peer_rx_stats.get), 0);

    cmd->u.peer_rx_stats.get.cookie = ni->wlanstats_peer_cookie;

    for (i = 0; i < ARRAY_SIZE(src->stats); i++) {
        cmd->u.peer_rx_stats.get.stats[i].num_bytes = src->stats[i].num_bytes;
        cmd->u.peer_rx_stats.get.stats[i].num_msdus = src->stats[i].num_msdus;
        cmd->u.peer_rx_stats.get.stats[i].num_mpdus = src->stats[i].num_mpdus;
        cmd->u.peer_rx_stats.get.stats[i].num_ppdus = src->stats[i].num_ppdus;
        cmd->u.peer_rx_stats.get.stats[i].num_retries = src->stats[i].num_retries;
        cmd->u.peer_rx_stats.get.stats[i].num_sgi = src->stats[i].num_sgi;
        cmd->u.peer_rx_stats.get.stats[i].ave_rssi = ewma_rx_rssi_read(&src->stats[i].ave_rssi);
        for (j = 0; j < WLANSTATS_MAX_RSSI_ANT; j++)
            for (k = 0; k < WLANSTATS_MAX_RSSI_HT; k++)
                cmd->u.peer_rx_stats.get.stats[i].ave_rssi_ant[j][k] = ewma_rx_rssi_read(&src->stats[i].ave_rssi_ant[j][k]);
    }

    err = 0;

    ieee80211_free_node(ni);

    return err;
}

#if LINUX_VERSION_CODE < KERNEL_VERSION(3,3,0)
static inline __uint64_t rol64(__uint64_t word, unsigned int shift)
{
    return (word << shift) | (word >> (64 - shift));
}
#endif

static int wlan_ioctl_get_tx_stats(struct net_device *dev,
        struct iw_request_info *info,
        union iwreq_data *wrqu,
        char *extra,
        struct wlanstats_uapi_ioctl *cmd)
{
    struct ieee80211com *ic;
    struct ieee80211_node *ni;
    struct ieee80211_node_table *nt;
    struct wlanstats_ctx *ctx;
    struct wlan_tx_stats *src;
    unsigned long flags;
    rwlock_state_t lock_state;
    int err;
    int i;

    ic = ath_netdev_priv(dev);
    nt = &ic->ic_sta;
    ctx = &ic->wlanstats_ctx;

    OS_BEACON_WRITE_LOCK(&nt->nt_nodelock, &lock_state, flags);

    ni = wlanstats_find_node(nt, cmd->u.peer_tx_stats.set.addr);
    if (!ni) {
        err = -ENOENT;
        goto unlock;
    }

    src = ni->tx_stats;

    BUILD_BUG_ON(ARRAY_SIZE(cmd->u.peer_tx_stats.get.stats) !=
            ARRAY_SIZE(src->stats));

    qdf_mem_set(&cmd->u.peer_tx_stats.get, sizeof(cmd->u.peer_tx_stats.get), 0);

    cmd->u.peer_tx_stats.get.cookie = ni->wlanstats_peer_cookie;

    qdf_spin_lock_irqsave(&ctx->lock);

    for (i = 0; i < ARRAY_SIZE(src->stats); i++) {
        cmd->u.peer_tx_stats.get.stats[i].attempts = src->stats[i].attempts;
        cmd->u.peer_tx_stats.get.stats[i].success = src->stats[i].success;
        cmd->u.peer_tx_stats.get.stats[i].ppdus = src->stats[i].ppdus;
    }

    for (i = 0; i < ARRAY_SIZE(src->sojourn); i++) {
        cmd->u.peer_tx_stats.get.sojourn[i].ave_sojourn_msec = ewma_tx_lag_read(&src->sojourn[i].ave_sojourn_msec);
        cmd->u.peer_tx_stats.get.sojourn[i].sum_sojourn_msec = src->sojourn[i].sum_sojourn_msec;
        cmd->u.peer_tx_stats.get.sojourn[i].num_sojourn_mpdus = src->sojourn[i].num_sojourn_mpdus;
    }

    qdf_spin_unlock_irqrestore(&ctx->lock);

    err = 0;

unlock:
    OS_BEACON_WRITE_UNLOCK(&nt->nt_nodelock, &lock_state, flags);

    return err;
}

static int wlan_ioctl_get_svc(struct net_device *dev,
        struct iw_request_info *info,
        union iwreq_data *wrqu,
        char *extra,
        struct wlanstats_uapi_ioctl *cmd)
{
    struct ieee80211com *ic;
    struct wlanstats_ctx *ctx;
    enum wlanstats_uapi_ioctl_svc svc;
    u32 modify;
    u32 enabled;

    ic = ath_netdev_priv(dev);
    ctx = &ic->wlanstats_ctx;

    svc = cmd->u.svc.set.svc;
    modify = cmd->u.svc.set.modify;
    enabled = cmd->u.svc.set.enabled;

    if (svc >= ARRAY_SIZE(ctx->svc))
        return -ENOENT;

    if (modify)
        ctx->svc[svc] = enabled;

    cmd->u.svc.get.svc = svc;
    cmd->u.svc.get.enabled = ctx->svc[svc];

    return 0;
}

static int
wlan_ioctl_get_survey_chan(struct net_device *dev,
        struct iw_request_info *info,
        union iwreq_data *wrqu,
        char *extra,
        struct wlanstats_uapi_ioctl *cmd)
{
    struct ieee80211com *ic;
    struct scan_chan_stats *chan_stats;
    int i, j;

    ic = ath_netdev_priv(dev);
    if (!ic)
        return -EINVAL;

    chan_stats = &ic->ic_scan_chan_stats;
    for (i = 0, j = 0; i < MAX_DUAL_BAND_SCAN_CHANS; i++) {
        if (j >= ARRAY_SIZE(cmd->u.survey_chan.get.channels))
            break;
        cmd->u.survey_chan.get.channels[j].freq = chan_stats->stats[i].freq;
        cmd->u.survey_chan.get.channels[j].total = chan_stats->stats[i].cycle_cnt;
        cmd->u.survey_chan.get.channels[j].tx = chan_stats->stats[i].tx_frm_cnt;
        cmd->u.survey_chan.get.channels[j].rx = chan_stats->stats[i].rx_frm_cnt;
        cmd->u.survey_chan.get.channels[j].busy = chan_stats->stats[i].clear_cnt;
        j++;
    }

    return 0;
}

static int
wlan_ioctl_get_survey_bss(struct net_device *dev,
        struct iw_request_info *info,
        union iwreq_data *wrqu,
        char *extra,
        struct wlanstats_uapi_ioctl *cmd)
{
    struct ieee80211com *ic;
    struct channel_stats *bss_stats;

    ic = ath_netdev_priv(dev);
    if (!ic)
        return -EINVAL;
    bss_stats = &ic->ic_home_chan_stats;

    cmd->u.survey_bss.get.rx_bss = bss_stats->bss_rx_cnt;
    cmd->u.survey_bss.get.total = bss_stats->cycle_cnt;
    cmd->u.survey_bss.get.tx = bss_stats->tx_frm_cnt;
    cmd->u.survey_bss.get.rx = bss_stats->rx_frm_cnt;
    cmd->u.survey_bss.get.busy = bss_stats->clear_cnt;
    cmd->u.survey_bss.get.busy_ext = bss_stats->ext_busy_cnt;

    return 0;
}

typedef int (*wlanstats_ioctl_op)(struct net_device *dev,
        struct iw_request_info *info,
        union iwreq_data *wrqu,
        char *extra,
        struct wlanstats_uapi_ioctl *cmd);

static const wlanstats_ioctl_op get_ops[WLAN_UAPI_IOCTL_CMD_MAX] = {
    [WLAN_UAPI_IOCTL_CMD_PEER_RX_STATS] = wlan_ioctl_get_rx_stats,
    [WLAN_UAPI_IOCTL_CMD_PEER_TX_STATS] = wlan_ioctl_get_tx_stats,
    [WLAN_UAPI_IOCTL_CMD_Q_UTIL] = wlan_ioctl_get_q_util,
    [WLAN_UAPI_IOCTL_CMD_SVC] = wlan_ioctl_get_svc,
    [WLAN_UAPI_IOCTL_CMD_SURVEY_CHAN] = wlan_ioctl_get_survey_chan,
    [WLAN_UAPI_IOCTL_CMD_SURVEY_BSS] = wlan_ioctl_get_survey_bss,
};

bool wlanstats_svc_is_enabled(struct ieee80211com *ic, enum wlanstats_uapi_ioctl_svc svc)
{
    struct wlanstats_ctx *ctx = &ic->wlanstats_ctx;

    if (svc >= ARRAY_SIZE(ctx->svc))
        return false;

    return ctx->svc[svc];
}
EXPORT_SYMBOL(wlanstats_svc_is_enabled);

static void __wlanstats_peer_tx_stats_collect(struct ieee80211com *ic,
        struct ieee80211_node *ni,
        uint32_t rix,
        uint32_t attempts,
        uint32_t success,
        uint32_t ppdus)
{
    if (!ni->tx_stats) {
        return;
    }
    ni->tx_stats->stats[rix].attempts += attempts;
    ni->tx_stats->stats[rix].success += success;
    ni->tx_stats->stats[rix].ppdus += ppdus;
}

static int wlanstats_get_rate_ar98xx(const uint32_t *ppdu_start,
        const uint32_t *ppdu_end,
        int try,
        uint32_t *idx,
        uint32_t *rate,
        uint32_t *bw,
        uint32_t *attempts)
{
    bool is_rts;
    bool is_packet;
    uint32_t series;
    uint32_t bw_rts;
    uint32_t bw_packet;

    is_rts = !!(ppdu_end[try] & BIT(30));
    is_packet = !!(ppdu_end[try] & BIT(31));
    bw_rts = (ppdu_end[try] >> 26) & 0x3;
    bw_packet = (ppdu_end[try] >> 28) & 0x3;
    series = (ppdu_end[try] >> 24) & 0x1;
    if (is_packet) {
        *bw = bw_packet;
    } else if (is_rts) {
        return -1;
    } else {
        return -1;
    }

    *idx = (series * 4) + (*bw);
    *rate = ppdu_start[21 + ((*idx) * 4) + 1];
    *attempts = is_packet;

    WARN_ON_ONCE(unlikely(bw_rts != bw_packet));

    return 0;
}

static int wlanstats_calc_tx_acks_ar98xx(const uint32_t *ppdu_start,
        const uint32_t *ppdu_end,
        uint32_t idx)
{
    uint32_t seqstart;
    uint32_t ackstart;
    uint64_t seqmap;
    uint64_t ackmap;
    int seqdelta;

    seqstart = ppdu_start[2];
    seqstart >>= 0;
    seqstart &= BIT(12) - 1;

    seqmap = ppdu_start[4];
    seqmap <<= 32;
    seqmap |= ppdu_start[3];

    ackstart = ppdu_end[16 + 0];
    ackstart >>= 0;
    ackstart &= BIT(12) - 1;

    ackmap = ppdu_end[16 + 2];
    ackmap <<= 32;
    ackmap |= ppdu_end[16 + 1];

    seqdelta = ackstart - seqstart;

    if (seqdelta < 0)
        seqdelta += 64;

    if (seqdelta >= 64)
        return 0;

    ackmap = rol64(ackmap, seqdelta);
    ackmap &= seqmap;

    return hweight64(ackmap);
}

static int wlanstats_calc_tx_rix_ar98xx(uint32_t rate, uint32_t bw)
{
    struct wlanstats_cmn_rate_info ri = {};
    static const uint32_t ofdm_map[] = {
        [0] = 6,
        [1] = 4,
        [2] = 2,
        [3] = 0,
        [4] = 7,
        [5] = 5,
        [6] = 3,
        [7] = 1,
    };
    static const uint32_t cck_map[] = {
        [0] = 11,
        [1] = 10,
        [2] = 9,
        [3] = 8,
        [4] = 14,
        [5] = 13,
        [6] = 12,
    };
    uint32_t mcs;
    uint32_t nss;
    uint32_t preamble;

    mcs = (rate >> 24) & 0xf;
    nss = (rate >> 28) & 0x3;
    preamble = (rate >> 30) & 0x3;

    switch (preamble) {
        case 0:
            ri.mcs = ofdm_map[mcs];
            break;
        case 1:
            ri.is_cck = 1;
            ri.mcs = cck_map[mcs];
            break;
        case 2:
        case 3:
            ri.is_ht = 1;
            ri.mcs = mcs;
            ri.nss = nss;
            ri.bw = bw;
            break;
    }

    return wlanstats_cmn_calc_rix(&ri);
}

void wlan_tx_stats_collect_ar98xx(struct ieee80211com *ic,
        struct ieee80211_node *ni,
        uint32_t *ppdu_start,
        uint32_t *ppdu_end)
{
    struct wlanstats_ctx *ctx = &ic->wlanstats_ctx;
    int try;
    int rix;
    int err;
    bool is_tx_ok;
    bool is_ampdu;
    bool is_noack;
    bool is_last;
    uint32_t num_tries;
    uint32_t bw;
    uint32_t idx;
    uint32_t rate;
    uint32_t attempts;
    uint32_t success;

    is_tx_ok = !!(ppdu_end[16] & BIT(31));
    is_ampdu = !!(ppdu_start[13] & BIT(15));
    is_noack = !!(ppdu_start[13] & BIT(16));
    num_tries = (ppdu_end[16 + 11] >> 24) & 0x1f;
    num_tries = min(num_tries, 16U);

    if (is_noack)
        return;

    if (num_tries == 0)
        return;

    qdf_spin_lock_bh(&ctx->lock);

    for (try = 0; try < num_tries; try++) {
        err = wlanstats_get_rate_ar98xx(ppdu_start,
                ppdu_end,
                try,
                &idx,
                &rate,
                &bw,
                &attempts);
        if (err)
            continue;

        rix = wlanstats_calc_tx_rix_ar98xx(rate, bw);
        success = 0;
        is_last = !!(try == (num_tries - 1));

        if (is_ampdu) {
            attempts *= hweight32(ppdu_start[3]) +
                hweight32(ppdu_start[4]);

            if (is_last && is_tx_ok)
                success = wlanstats_calc_tx_acks_ar98xx(ppdu_start,
                        ppdu_end,
                        idx);
        } else {
            attempts *= 1;

            if (is_last && is_tx_ok)
                success = 1;
        }

        __wlanstats_peer_tx_stats_collect(ic, ni, rix, attempts, success, 1);
    }

    qdf_spin_unlock_bh(&ctx->lock);
}
qdf_export_symbol(wlan_tx_stats_collect_ar98xx);

void wlanstats_sojourn_collect(struct ieee80211com *ic,
        struct ieee80211_node *ni,
        int tidno,
        qdf_nbuf_t netbuf)
{
    struct wlanstats_ctx *ctx;
    unsigned long delta_ms;

    if (!IS_WLANSTATS_ENABLED(ic))
        return;

    if (!wlanstats_svc_is_enabled(ic, WLAN_UAPI_IOCTL_SVC_SOJOURN))
        return;

    if (tidno >= ARRAY_SIZE(ni->tx_stats->sojourn))
        tidno = 0;

    ctx = &ic->wlanstats_ctx;
    delta_ms = qdf_ktime_to_ms(net_timedelta(netbuf->tstamp));

    qdf_spin_lock_irqsave(&ctx->lock);

    ewma_tx_lag_add(&ni->tx_stats->sojourn[tidno].ave_sojourn_msec, delta_ms);
    ni->tx_stats->sojourn[tidno].sum_sojourn_msec += delta_ms;
    ni->tx_stats->sojourn[tidno].num_sojourn_mpdus++;

    qdf_spin_unlock_irqrestore(&ctx->lock);
}
qdf_export_symbol(wlanstats_sojourn_collect);

int wlanstats_ioctl(struct net_device *dev,
        struct iw_request_info *info,
        union iwreq_data *wrqu,
        char *extra)
{
    struct wlanstats_uapi_ioctl *cmd = (void *)extra;
    struct wlanstats_ctx *ctx;
    struct ieee80211com *ic;

    cmd = wrqu->data.pointer;
    ic = ath_netdev_priv(dev);
    ctx = &ic->wlanstats_ctx;
    if (!IS_WLANSTATS_ENABLED(ic)) {
        qdf_print("stats configuration not enabled\n");
        return -EOPNOTSUPP;
    }

    switch (info->cmd) {
        case WLANSTATS_UAPI_IOCTL_SET:
            if (cmd->cmd >= WLAN_UAPI_IOCTL_CMD_MAX ||
                    !get_ops[cmd->cmd])
                return -EOPNOTSUPP;

            qdf_mem_copy(&ctx->req, cmd, sizeof(*cmd));
            return 0;
        case WLANSTATS_UAPI_IOCTL_GET:
            qdf_mem_copy(cmd, &ctx->req, sizeof(*cmd));

            if (cmd->cmd >= WLAN_UAPI_IOCTL_CMD_MAX)
                return -EOPNOTSUPP;

            if (!get_ops[cmd->cmd])
                return -EOPNOTSUPP;

            return get_ops[cmd->cmd](dev, info, wrqu, extra, cmd);
    }

    return -EOPNOTSUPP;
}
qdf_export_symbol(wlanstats_ioctl);

int wlanstats_init(struct wlanstats_ctx *ctx)
{
    int i;

    qdf_spinlock_create(&ctx->lock);

    for (i = 0; i < ARRAY_SIZE(ctx->svc); i++)
        ctx->svc[i] = true;

    /*
     * Q utility is a timer based polling and is
     * started from umac itself. For now disable it
     * TODO: Make it runtime On/Off
     */
    ctx->svc[WLAN_UAPI_IOCTL_SVC_Q_UTIL] = false;

    return 0;
}
qdf_export_symbol(wlanstats_init);

void ol_ath_wlanstats_peer_init(struct ieee80211com *ic,
        struct ieee80211_node *ni)
{
    struct wlanstats_ctx *ctx = &ic->wlanstats_ctx;
    struct wlan_rx_stats *rxs = NULL;
    struct wlan_tx_stats *txs = NULL;
    int i,j,k;

    if(IS_WLANSTATS_CFG_ACTIVE(ic)) {
        ni->rx_stats = qdf_mem_malloc(sizeof(struct wlan_rx_stats));
        if (!ni->rx_stats)
            return;

        ni->tx_stats = qdf_mem_malloc(sizeof(struct wlan_tx_stats));
        if (!ni->tx_stats) {
            qdf_mem_free(ni->rx_stats);
            return;
        }
    }

    if (!IS_WLANSTATS_ENABLED(ic))
        return;

    rxs = ni->rx_stats;
    txs = ni->tx_stats;
    /* Note: Locking is assumed to be taken appropriately by the caller. In
     * pratice this is in context of ieee80211_node creation which is
     * locked properly already.
     */
    ni->wlanstats_peer_cookie = ctx->next_peer_cookie++;

    for (i = 0; i < ARRAY_SIZE(rxs->stats); i++) {
        ewma_rx_rssi_init(&rxs->stats[i].ave_rssi);
        for (j = 0; j < WLANSTATS_MAX_RSSI_ANT; j++)
            for (k = 0; k < WLANSTATS_MAX_RSSI_HT; k++)
                ewma_rx_rssi_init(&rxs->stats[i].ave_rssi_ant[j][k]);
    }

    for (i = 0; i < ARRAY_SIZE(txs->sojourn); i++)
        ewma_tx_lag_init(&txs->sojourn[i].ave_sojourn_msec);
}

void ol_ath_wlanstats_peer_deinit(struct ieee80211com *ic,
        struct ieee80211_node *ni)
{
    if(IS_WLANSTATS_CFG_ACTIVE(ic)) {
        qdf_mem_free(ni->rx_stats);
        qdf_mem_free(ni->tx_stats);
    }
}
qdf_export_symbol(ol_ath_wlanstats_peer_deinit);

#endif // ATH_SUPPORT_RDKB
