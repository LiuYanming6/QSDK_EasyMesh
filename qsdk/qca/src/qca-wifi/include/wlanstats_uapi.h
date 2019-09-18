/*
 * Copyright (c) 2018 Qualcomm Innovation Center, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Innovation Center, Inc.
 */

#ifndef _WLANSTATS_UAPI_H
#define _WLANSTATS_UAPI_H

#ifndef __packed
#define __packed __attribute__((packed))
#endif

#ifndef __packed_pad
#define __packed_pad(x) __attribute__((packed, aligned(x)))
#endif

#define MAC_ADDRESS_LEN 6
#define WLANSTATS_MAX_CCK_LONG  4
#define WLANSTATS_MAX_CCK_SHORT 3
#define WLANSTATS_MAX_CCK ((WLANSTATS_MAX_CCK_LONG) + (WLANSTATS_MAX_CCK_SHORT))
#define WLANSTATS_MAX_OFDM 8
#define WLANSTATS_MAX_MCS 10
#define WLANSTATS_MAX_NSS 4
#define WLANSTATS_MAX_BW  4
#define WLANSTATS_MAX_LEGACY (WLANSTATS_MAX_OFDM + WLANSTATS_MAX_CCK)
#define WLANSTATS_MAX_HT_VHT (WLANSTATS_MAX_MCS * WLANSTATS_MAX_NSS * WLANSTATS_MAX_BW)
#define WLANSTATS_MAX_ALL (WLANSTATS_MAX_LEGACY + WLANSTATS_MAX_HT_VHT)

#define WLANSTATS_MAX_CHANS 64

#define WLANSTATS_MAX_RSSI_ANT 4
#define WLANSTATS_MAX_RSSI_HT 4 /* pri20, ext20, ext40, ext80 */

#define WLANSTATS_MAX_Q_UTIL 10
#define WLANSTATS_MAX_TID 16

#define WLANSTATS_UAPI_IOCTL_UNIT_SIZE (4)
#define WLANSTATS_UAPI_IOCTL_UNIT_TYPE (IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED)
#define WLANSTATS_UAPI_IOCTL_SIZE (sizeof(struct wlanstats_uapi_ioctl) / (WLANSTATS_UAPI_IOCTL_UNIT_SIZE))
#define WLANSTATS_UAPI_IOCTL_PARAM ((WLANSTATS_UAPI_IOCTL_UNIT_TYPE) | (WLANSTATS_UAPI_IOCTL_SIZE))

#define WLANSTATS_UAPI_IOCTL_SET_OFFSET 20
#define WLANSTATS_UAPI_IOCTL_GET_OFFSET 21
#define WLANSTATS_UAPI_IOCTL_SET ((SIOCIWFIRSTPRIV) + (WLANSTATS_UAPI_IOCTL_SET_OFFSET))
#define WLANSTATS_UAPI_IOCTL_GET ((SIOCIWFIRSTPRIV) + (WLANSTATS_UAPI_IOCTL_GET_OFFSET))
#define WLANSTATS_UAPI_IOCTL_SET_NAME "set_peer_stats"
#define WLANSTATS_UAPI_IOCTL_GET_NAME "get_peer_stats"

enum wlanstats_uapi_ioctl_cmd {
        WLAN_UAPI_IOCTL_CMD_PEER_RX_STATS = 0,
        WLAN_UAPI_IOCTL_CMD_SURVEY_CHAN,
        WLAN_UAPI_IOCTL_CMD_SURVEY_BSS,
        WLAN_UAPI_IOCTL_CMD_PEER_TX_STATS,
        WLAN_UAPI_IOCTL_CMD_Q_UTIL,
        WLAN_UAPI_IOCTL_CMD_SVC,
        WLAN_UAPI_IOCTL_CMD_MAX, /* keep last */
};

enum wlanstats_uapi_ioctl_svc {
        WLAN_UAPI_IOCTL_SVC_PEER_RX_STATS,
        WLAN_UAPI_IOCTL_SVC_PEER_TX_STATS,
        WLAN_UAPI_IOCTL_SVC_SOJOURN,
        WLAN_UAPI_IOCTL_SVC_SURVEY,
        WLAN_UAPI_IOCTL_SVC_Q_UTIL,
        WLAN_UAPI_IOCTL_SVC_MAX, /* keep last */
};

/**
 * struct wlanstats_uapi_ioctl - main ioctl structure for both GET and SET
 *
 * Each command has 2 substructures - get and set.
 *
 * The set substructure contains request payload, e.g. mac address. It must be
 * send via SET ioctl prior to GET ioctl to define the response. It is echoed
 * back on GET ioctl.
 *
 * The get substructure contains response payload, e.g. stats. It is filled in
 * during GET ioctl based on prior SET ioctl payload.
 *
 * @cmd: ioctl subcommand id, see %wlanstats_uapi_ioctl_cmd
 * @cookie: optional. Echoed back from SET to GET ioctl. Can be used for
 * sequence verification.
 */
struct wlanstats_uapi_ioctl {
        uint32_t cmd; /* @wlanstats_uapi_ioctl_cmd */
        uint64_t cookie;
        union {
                struct {
                        struct {
                                uint8_t addr[MAC_ADDRESS_LEN];
                        } __packed set;
                        struct {
                                /**
                                 * @cookie: unique id of client session. Can be
                                 * used to detect (and discard) readouts
                                 * if given mac address has re-associated
                                 * across multiple ioctl calls.
                                 * @num_retried: number of MPDUs that had
                                 * Re-transmission bit set in Frame
                                 * Control field.
                                 * @num_err_fcs: contains number of MPDUs with
                                 * corrupted FCS within an A-MPDU that had
                                 * some preceeding MPDUs correct (hence
                                 * transmitter address could be derived).
                                 * Otherwise it's impossible to derive per
                                 * station Rx FCS errors.
                                 * @ave_rssi: does not include noise floor.
                                 * Hence it is a positive number.
                                 * @ave_rssi_ant: per-hain RSSI, see @ave_rssi.
                                 */
                                uint64_t cookie;
                                struct {
                                        uint32_t num_bytes;
                                        uint32_t num_msdus;
                                        uint32_t num_mpdus;
                                        uint32_t num_ppdus;
                                        uint32_t num_retries;
                                        uint32_t num_sgi;
                                        uint32_t ave_rssi;
                                        uint8_t ave_rssi_ant[WLANSTATS_MAX_RSSI_ANT][WLANSTATS_MAX_RSSI_HT];
                                } __packed stats[WLANSTATS_MAX_ALL];
                        } __packed get;
                } __packed peer_rx_stats;
                struct {
                        struct {
                        } __packed set;
                        struct {
                                /**
                                 * @freq: 0 means end of list
                                 */
                                struct {
                                        /* mhz */
                                        uint16_t freq;
                                        /* usec */
                                        uint32_t total;
                                        uint32_t tx;
                                        uint32_t rx;
                                        uint32_t busy;
                                } __packed channels[WLANSTATS_MAX_CHANS];
                        } __packed get;
                } __packed survey_chan;
                struct {
                        struct {
                        } __packed set;
                        struct {
                                /* usec */
                                uint64_t total;
                                uint64_t tx;
                                uint64_t rx;
                                uint64_t rx_bss;
                                uint64_t busy;
                                uint64_t busy_ext;
                        } __packed get;
                } __packed survey_bss;
                struct {
                        struct {
                                uint8_t addr[MAC_ADDRESS_LEN];
                        } __packed set;
                        struct {
                                uint64_t cookie;
                                struct {
                                        uint32_t attempts;
                                        uint32_t success;
                                        uint32_t ppdus;
                                } __packed stats[WLANSTATS_MAX_ALL];
                                struct {
                                        uint32_t ave_sojourn_msec;
                                        uint64_t sum_sojourn_msec;
                                        uint32_t num_sojourn_mpdus;
                                } __packed sojourn[WLANSTATS_MAX_TID];
                        } __packed get;
                } __packed peer_tx_stats;
                struct {
                        struct {
                        } __packed set;
                        struct {
                                uint64_t q[WLANSTATS_MAX_Q_UTIL];
                                uint64_t cnt;
                        } __packed get;
                } __packed q_util;
                struct {
                        struct {
                                uint32_t svc;
                                uint32_t modify;
                                uint32_t enabled;
                        } __packed set;
                        struct {
                                uint32_t svc;
                                uint32_t enabled;
                        } __packed get;
                } __packed svc;
        } __packed u;
} __packed_pad(WLANSTATS_UAPI_IOCTL_UNIT_SIZE);

struct wlanstats_cmn_rate_info {
        uint32_t is_cck;
        uint32_t is_ht;
        uint32_t mcs;
        uint32_t nss;
        uint32_t bw;
        uint32_t sgi;
        uint32_t stbc;
};

static inline int wlanstats_cmn_calc_rix(const struct wlanstats_cmn_rate_info *ri)
{
        int rix = 0;
        uint32_t nss;

        nss = ri->nss;
        if (nss >= ri->stbc)
                nss -= ri->stbc;

        if (ri->is_cck || ri->is_ht)
                rix += WLANSTATS_MAX_OFDM;
        if (ri->is_ht)
                rix += WLANSTATS_MAX_CCK;

        rix += ri->mcs;
        rix += nss * (WLANSTATS_MAX_MCS);
        rix += ri->bw  * (WLANSTATS_MAX_MCS * WLANSTATS_MAX_NSS);

        if (rix >= WLANSTATS_MAX_ALL)
                return 0;

        return rix;
}

static inline const char *wlanstats_cmn_bw_str(int bw)
{
        static const char *bw_str[WLANSTATS_MAX_BW] = {
                "20MHz",
                "40MHz",
                "80MHz",
                "160MHz",
        };

        if (bw >= WLANSTATS_MAX_BW)
                return "unknown";

        return bw_str[bw];
}

static inline const char *wlanstats_cmn_cck_str(int rate)
{
        static const char *cck_str[WLANSTATS_MAX_CCK] = {
                "L 1M",
                "L 2M",
                "L 5.5M",
                "L 11M",
                "S 2M",
                "S 5.5M",
                "S 11M",
        };

        if (rate >= WLANSTATS_MAX_CCK)
                return "unknown";

        return cck_str[rate];
}

static inline const char *wlanstats_cmn_ofdm_str(int rate)
{
        static const char *ofdm_str[WLANSTATS_MAX_OFDM] = {
                "6M",
                "9M",
                "12M",
                "18M",
                "24M",
                "36M",
                "48M",
                "54M",
        };

        if (rate >= WLANSTATS_MAX_OFDM)
                return "unknown";

        return ofdm_str[rate];
}

static inline bool
wlanstats_cmn_is_cck_sp(const struct wlanstats_cmn_rate_info *ri)
{
        return ri->mcs >= WLANSTATS_MAX_CCK_LONG;
}

#define WLANSTATS_CCK_SIFS_TIME 10
#define WLANSTATS_CCK_PREAMBLE_BITS 144
#define WLANSTATS_CCK_PLCP_BITS 48
#define WLANSTATS_OFDM_SIFS_TIME 16
#define WLANSTATS_OFDM_PREAMBLE_TIME 20
#define WLANSTATS_OFDM_PLCP_BITS 22
#define WLANSTATS_OFDM_SYMBOL_TIME 4
#define WLANSTATS_L_STF 8
#define WLANSTATS_L_LTF 8
#define WLANSTATS_L_SIG 4
#define WLANSTATS_HT_SIG 8
#define WLANSTATS_HT_STF 4
#define WLANSTATS_HT_LTF(x)  (4 * (x))
#define WLANSTATS_HT_SYMBOL_TIME_LGI(x) ((x) << 2) /* x * 4 us */
#define WLANSTATS_HT_SYMBOL_TIME_SGI(x) (((x) * 18 + 4) / 5) /* x * 3.6 us */
#define WLANSTATS_HT_SYMBOL_TIME(num, sgi)\
        ((sgi)\
         ? WLANSTATS_HT_SYMBOL_TIME_SGI(num)\
         : WLANSTATS_HT_SYMBOL_TIME_LGI(num))

#endif
