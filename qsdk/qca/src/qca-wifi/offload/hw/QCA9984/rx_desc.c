/*
 * Copyright (c) 2015, 2017-2018 Qualcomm Innovation Center, Inc.
 *
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Innovation Center, Inc.
 *
 * 2015 Qualcomm Atheros, Inc.
 * All Rights Reserved.
 * Qualcomm Atheros Confidential and Proprietary.
 */

#include <qdf_mem.h>   /* qdf_mem_malloc,free, etc. */
#include <qdf_types.h> /* qdf_print */
#include <qdf_nbuf.h>
#include <ieee80211_defines.h>
#include <ol_vowext_dbg_defs.h>
#include <ol_ratetable.h>
#include <ol_cfg_raw.h>
#include <ieee80211_smart_ant_api.h>

/* QCA9984 RX descriptor */
#include <QCA9984/hw/interface/rx_location_info.h>
#include <QCA9984/hw/interface/rx_pkt_end.h>
#include <QCA9984/hw/interface/rx_phy_ppdu_end.h>
#include <QCA9984/hw/interface/rx_timing_offset.h>
#include <QCA9984/hw/interface/rx_location_info.h>
#include <QCA9984/hw/tlv/rx_attention.h>
#include <QCA9984/hw/tlv/rx_frag_info.h>
#include <QCA9984/hw/tlv/rx_msdu_start.h>
#include <QCA9984/hw/tlv/rx_msdu_end.h>
#include <QCA9984/hw/tlv/rx_mpdu_start.h>
#include <QCA9984/hw/tlv/rx_mpdu_end.h>
#include <QCA9984/hw/tlv/rx_ppdu_start.h>
#include <QCA9984/hw/tlv/rx_ppdu_end.h>

/* defines HW descriptor format */
#include "../include/rx_desc_internal.h"
#include "../include/ar_internal.h"

#define RX_DESC_ANTENNA_OFFSET 26
#define GET_L3_HEADER_PADDING(rx_desc) (((*(((u_int32_t *) &rx_desc->msdu_end)+ 8)) &\
                                            RX_MSDU_END_8_L3_HEADER_PADDING_MASK) >>\
                                            RX_MSDU_END_8_L3_HEADER_PADDING_LSB)

#define rx_desc_parse_ppdu_start_status(rx_desc, rx_status) \
    _qca9984_rx_desc_parse_ppdu_start_status(rx_desc, rx_status)

#if RX_CHECKSUM_OFFLOAD
#define rx_desc_set_checksum_result(rx_desc, msdu) \
    _qca9984_rx_desc_set_checksum_result(rx_desc, msdu)
#else
#define rx_desc_set_checksum_result(rx_desc, msdu)
#endif

#if HOST_SW_LRO_ENABLE
#define rx_desc_set_lro_info(rx_desc, msdu) \
    _qca9984_rx_desc_set_lro_info(rx_desc, msdu)

static inline void _qca9984_rx_desc_set_lro_info(void *rx_desc,
        qdf_nbuf_t msdu)
{
    struct rx_desc_base *desc = (struct rx_desc_base *)rx_desc;
    struct rx_msdu_start *msdu_start = &desc->msdu_start;
    struct rx_msdu_end *msdu_end = &desc->msdu_end;
    lro_info_t *lro_info;
    int tcp_proto;
    tcp_proto = ((*((u_int32_t *) msdu_start+2) &
                  RX_MSDU_START_2_TCP_PROTO_MASK) >>
                  RX_MSDU_START_2_TCP_PROTO_LSB);

    if (tcp_proto)
    {

        lro_info = (lro_info_t *)qdf_nbuf_get_cb(msdu);
        lro_info->ip_offset     = (((*(u_int32_t *) msdu_start) &
                    RX_MSDU_START_0_IP_OFFSET_MASK) >>
                RX_MSDU_START_0_IP_OFFSET_LSB);
        lro_info->tcp_udp_offset = (((*(u_int32_t *) msdu_start) &
                    RX_MSDU_START_0_TCP_UDP_OFFSET_MASK) >>
                RX_MSDU_START_0_TCP_UDP_OFFSET_LSB);
        lro_info->tcp_proto      = ((*((u_int32_t *) msdu_start + 2) &
                    RX_MSDU_START_2_TCP_PROTO_MASK) >>
                RX_MSDU_START_2_TCP_PROTO_LSB);
        lro_info->ipv6_proto     = ((*((u_int32_t *) msdu_start + 2) &
                    RX_MSDU_START_2_IPV6_PROTO_MASK) >>
                RX_MSDU_START_2_IPV6_PROTO_LSB);
        lro_info->tcp_only_ack   = ((*((u_int32_t *) msdu_start + 2) &
                    RX_MSDU_START_2_TCP_ONLY_ACK_MASK) >>
                RX_MSDU_START_2_TCP_ONLY_ACK_LSB);
        lro_info->flow_id_crc    = ((*((u_int32_t *) msdu_start + 1) &
                    RX_MSDU_START_1_FLOW_ID_CRC_MASK) >>
                RX_MSDU_START_1_FLOW_ID_CRC_LSB);
        lro_info->tcp_udp_cksum   = (((*(u_int32_t *) msdu_end) &
                    RX_MSDU_END_0_TCP_UDP_CHKSUM_MASK) >>
                RX_MSDU_END_0_TCP_UDP_CHKSUM_LSB);
        lro_info->lro_eligible    = ((*((u_int32_t *) msdu_end + 8) &
                    RX_MSDU_END_8_LRO_ELIGIBLE_MASK) >>
                RX_MSDU_END_8_LRO_ELIGIBLE_LSB);

    }
}
#else   /* HOST_SW_LRO_ENABLE */
#define rx_desc_set_lro_info(rx_desc, msdu)
#endif  /* HOST_SW_LRO_ENABLE */


static u_int32_t _qca9984_rx_desc_check_desc_phy_data_type(qdf_nbuf_t head_msdu, uint8_t *rx_phy_data_filter)
{
    struct rx_desc_base *rx_desc;
    qdf_nbuf_t next_msdu = head_msdu;
    int        msdu_chained = 0;
    uint32_t   flag_phy_data = 0;
    uint32_t   *phyerr_mask;

    *rx_phy_data_filter = 0;

    while(next_msdu)
    {
        rx_desc = ar_rx_desc(next_msdu);
        msdu_chained = (((*(u_int32_t *) &rx_desc->frag_info) &
                                          RX_FRAG_INFO_0_RING2_MORE_COUNT_MASK) >>
                                          RX_FRAG_INFO_0_RING2_MORE_COUNT_LSB);


        if (((*(u_int32_t *) &rx_desc->attention) & (RX_ATTENTION_0_LAST_MPDU_MASK)) &&
            ((*(u_int32_t *) &rx_desc->attention) & (RX_ATTENTION_0_PHY_DATA_TYPE_MASK)))
        {

            flag_phy_data = 1;

            phyerr_mask = (uint32_t *)&rx_desc->ppdu_end.rx_phy_ppdu_end;

            if(phyerr_mask[0] & (RX_PHY_PPDU_END_0_ERROR_FALSE_RADAR_EXT_MASK | RX_PHY_PPDU_END_0_ERROR_RADAR_MASK)) {
                *rx_phy_data_filter  |= RX_PHY_DATA_RADAR;
            }
            break;
        }else
            next_msdu = qdf_nbuf_next(next_msdu);

        while(msdu_chained-- && next_msdu)
        {
            if (qdf_unlikely(!next_msdu))
            {
                break;
            }
            next_msdu = qdf_nbuf_next(next_msdu);
        }
    }

    if(flag_phy_data)
        return 0;
    else
        return 1;
}

static u_int32_t _qca9984_rx_desc_get_l3_header_padding(qdf_nbuf_t head_msdu)
{
	struct rx_desc_base *rx_desc = ar_rx_desc(head_msdu);
	return GET_L3_HEADER_PADDING(rx_desc);
}

static u_int32_t _qca9984_msdu_desc_phy_data_type(void *rx_desc)
{
   struct rx_desc_base *desc =
        (struct rx_desc_base *) rx_desc;
    int phy_data_type =
            (((*((u_int32_t *) &desc->attention)) &
             RX_ATTENTION_0_PHY_DATA_TYPE_MASK) >> RX_ATTENTION_0_PHY_DATA_TYPE_LSB);

    return phy_data_type;
}

static void _qca9984_rx_desc_set_checksum_result(void *rx_desc,
        qdf_nbuf_t msdu);
static void _qca9984_rx_desc_parse_ppdu_start_status(void *rx_desc,
        struct ieee80211_rx_status *rs);

/* Unified function definitions */
#include "../include/rx_desc_unified.h"

/* definitions for size and offset functions */
AR_RX_DESC_FIELD_FUNCS(qca9984, attention)
AR_RX_DESC_FIELD_FUNCS(qca9984, frag_info)
AR_RX_DESC_FIELD_FUNCS(qca9984, mpdu_start)
AR_RX_DESC_FIELD_FUNCS(qca9984, msdu_start)
AR_RX_DESC_FIELD_FUNCS(qca9984, msdu_end)
AR_RX_DESC_FIELD_FUNCS(qca9984, mpdu_end)
AR_RX_DESC_FIELD_FUNCS(qca9984, ppdu_start)
AR_RX_DESC_FIELD_FUNCS(qca9984, ppdu_end)

FN_RX_DESC_OFFSETOF_FW_DESC(qca9984)
FN_RX_DESC_OFFSETOF_HDR_STATUS(qca9984)
FN_RX_DESC_SIZE(qca9984)
FN_RX_DESC_FW_DESC_SIZE(qca9984)
FN_RX_DESC_INIT(qca9984)
FN_RX_DESC_MPDU_WIFI_HDR_RETRIEVE(qca9984)
FN_RX_DESC_MPDU_DESC_SEQ_NUM(qca9984)
FN_RX_DESC_MPDU_DESC_PN(qca9984)
FN_RX_DESC_MPDU_DESC_FRDS(qca9984)
FN_RX_DESC_MPDU_DESC_TODS(qca9984)
FN_RX_DESC_MPDU_IS_ENCRYPTED(qca9984)
FN_RX_DESC_GET_ATTN_WORD(qca9984)
FN_RX_DESC_GET_PPDU_START(qca9984)
FN_RX_DESC_ATTN_MSDU_DONE(qca9984)
FN_RX_DESC_MSDU_LENGTH(qca9984)
FN_RX_DESC_MSDU_DESC_COMPLETES_MPDU(qca9984)
FN_RX_DESC_MSDU_HAS_WLAN_MCAST_FLAG(qca9984)
FN_RX_DESC_MSDU_IS_WLAN_MCAST(qca9984)
FN_RX_DESC_MSDU_IS_FRAG(qca9984)
FN_RX_DESC_MSDU_FIRST_MSDU_FLAG(qca9984)
FN_RX_DESC_MSDU_KEY_ID_OCTET(qca9984)
FN_RX_DESC_SET_CHECKSUM_RESULT(qca9984)
FN_RX_DESC_DUMP(qca9984)
FN_RX_GET_MSDU_END(qca9984)	
FN_RX_DESC_CHECK_DESC_MGMT_TYPE(qca9984)
FN_RX_DESC_CHECK_DESC_CTRL_TYPE(qca9984)
FN_RX_DESC_MSDU_DESC_MSDU_CHAINED(qca9984)
FN_RX_DESC_MSDU_DESC_TSF_TIMESTAMP(qca9984)
FN_RX_DESC_AMSDU_POP(qca9984)
FN_RX_DESC_PARSE_PPDU_START_STATUS(qca9984)
FN_RX_DESC_RESTITCH_MPDU_FROM_MSDUS(qca9984)
FN_RX_DESC_GET_VOWEXT_STATS(qca9984)
FN_RX_DESC_GET_SMART_ANT_STATS(qca9984)
#if ATH_DATA_RX_INFO_EN || MESH_MODE_SUPPORT
FN_RX_DESC_UPDATE_PKT_INFO(qca9984)
#endif
FN_RX_DESC_UPDATE_MSDU_INFO(qca9984)

#ifdef ATH_SUPPORT_RDKB
static int
_qca9984_rx_desc_is_data(void *_rxd)
{
    struct rx_desc_base *rxd;
    uint32_t *ptr;
    rxd = _rxd;
    ptr = (void *)&rxd->attention;
    return !(ptr[0] & (RX_ATTENTION_0_NULL_DATA_MASK |
                       RX_ATTENTION_0_MGMT_TYPE_MASK |
                       RX_ATTENTION_0_CTRL_TYPE_MASK));
}

static int
_qca9984_rx_desc_is_first_mpdu(void *_rxd)
{
    struct rx_desc_base *rxd;
    uint32_t *ptr;
    rxd = _rxd;
    ptr = (void *)&rxd->attention;
    return ptr[0] & RX_ATTENTION_0_FIRST_MPDU_MASK;
}

static int
_qca9984_rx_desc_is_retry(void *_rxd)
{
    struct rx_desc_base *rxd;
    uint32_t *ptr;
    rxd = _rxd;
    ptr = (void *)&rxd->mpdu_start;
    return ptr[0] & RX_MPDU_START_0_RETRY_MASK;
}

static void
_qca9984_rx_desc_get_cmn_ri(void *_rxd,
                            struct wlanstats_cmn_rate_info *ri)
{
    struct rx_desc_base *rxd;
    uint32_t *ptr;
    uint32_t ht_sig_1;
    uint32_t ht_sig_2;
    uint32_t group_id;
    uint32_t preamble;
    enum {
        PREAMBLE_LEGACY    =  0x4,
        PREAMBLE_HT        =  0x8,
        PREAMBLE_HT_TXBF   =  0x9,
        PREAMBLE_VHT       =  0xc,
        PREAMBLE_VHT_TXBF  =  0xd,
    };
    enum {
        OFDM_48M = 0,
        OFDM_24M,
        OFDM_12M,
        OFDM_6M,
        OFDM_54M,
        OFDM_36M,
        OFDM_18M,
        OFDM_9M,
    };
    enum {
        CCK_LONG_1M = 1,
        CCK_LONG_2M,
        CCK_LONG_5M,
        CCK_LONG_11M,
        CCK_SHORT_2M,
        CCK_SHORT_5M,
        CCK_SHORT_11M,
    };
    static const u8 ofdm2rix[8] = {
        [OFDM_48M]  =  6,
        [OFDM_24M]  =  4,
        [OFDM_12M]  =  2,
        [OFDM_6M]   =  0,
        [OFDM_54M]  =  7,
        [OFDM_36M]  =  5,
        [OFDM_18M]  =  3,
        [OFDM_9M]   =  1,
    };
    static const u8 cck2rix[8] = {
        [CCK_LONG_11M]   =  3,
        [CCK_LONG_5M]    =  2,
        [CCK_LONG_2M]    =  1,
        [CCK_LONG_1M]    =  0,
        [CCK_SHORT_11M]  =  6,
        [CCK_SHORT_5M]   =  5,
        [CCK_SHORT_2M]   =  4,
    };

    rxd = _rxd;
    ptr = (uint32_t *)&rxd->ppdu_start;

    /* hw reports both vht_sig_a and ht_sig in the same spot */
    ht_sig_1 = ptr[RX_PPDU_START_6_HT_SIG_VHT_SIG_A_1_OFFSET / sizeof(uint32_t)];
    ht_sig_2 = ptr[RX_PPDU_START_7_HT_SIG_VHT_SIG_A_2_OFFSET / sizeof(uint32_t)];

    preamble = ptr[RX_PPDU_START_5_PREAMBLE_TYPE_OFFSET / sizeof(uint32_t)];
    preamble &= RX_PPDU_START_5_PREAMBLE_TYPE_MASK;
    preamble >>= RX_PPDU_START_5_PREAMBLE_TYPE_LSB;

    switch (preamble) {
    case PREAMBLE_LEGACY:
        ri->nss = 0;
        ri->bw = 0;
        ri->sgi = 0;

        ri->is_cck = ptr[RX_PPDU_START_5_L_SIG_RATE_SELECT_OFFSET / sizeof(uint32_t)];
        ri->is_cck &= RX_PPDU_START_5_L_SIG_RATE_SELECT_MASK;
        ri->is_cck >>= RX_PPDU_START_5_L_SIG_RATE_SELECT_LSB;

        ri->mcs = ptr[RX_PPDU_START_5_L_SIG_RATE_OFFSET / sizeof(uint32_t)];
        ri->mcs &= RX_PPDU_START_5_L_SIG_RATE_MASK;
        ri->mcs >>= RX_PPDU_START_5_L_SIG_RATE_LSB;

        if (ri->is_cck)
            ri->mcs = cck2rix[ri->mcs];
        else
            ri->mcs = ofdm2rix[ri->mcs - 8];
        break;
    case PREAMBLE_HT:
    case PREAMBLE_HT_TXBF:
        /* HT-SIG - Table 20-11 in info2 and info3 */

        ri->is_ht = 1;
        ri->mcs = ht_sig_1 & 0x1F;
        ri->nss = ri->mcs >> 3;
        ri->mcs %= 8;
        ri->bw = (ht_sig_1 >> 7) & 1;
        ri->sgi = (ht_sig_2 >> 7) & 1;
        ri->stbc = (ht_sig_2 >> 4) & 3;
        break;
    case PREAMBLE_VHT:
    case PREAMBLE_VHT_TXBF:
        /* VHT-SIG-A1 in info2, VHT-SIG-A2 in info3 */

        ri->is_ht = 1;
        ri->bw = ht_sig_1 & 3;
        ri->sgi = ht_sig_2 & 1;
        group_id = (ht_sig_1 >> 4) & 0x3F;

        if (group_id == 0 || group_id == 63) {
            ri->mcs = (ht_sig_2 >> 4) & 0x0F;
            ri->nss = (ht_sig_1 >> 10) & 0x07;
        } else {
            /* Hardware doesn't decode MU-MIMO VHT-SIG-B into Rx descriptor so
             * it's impossible to decode MCS. Also since firmware consumes
             * Group Id Management frames host has no knowledge regarding
             * group/user position mapping so it's impossible to pick the
             * correct Nsts from VHT-SIG-A1.
             *
             * Bandwidth and SGI are valid so report the rateinfo on
             * best-effort basis.
             */
            ri->mcs = 0;
            ri->nss = 0;
        }

        /* VHT spec says STBC is only 2x1 */
        ri->stbc = ((ht_sig_1 >> 3) & 1) ? ri->nss : 0;
        break;
    default:
        memset(ri, 0, sizeof(*ri));
        break;
    }
}

static void *
_qca9984_rx_desc_get_rssi(void *_rxd, int *co, int *cm, int *cs)
{
    struct rx_desc_base *rxd;
    rxd = _rxd;
    *co = RX_PPDU_START_4_RSSI_COMB_OFFSET / sizeof(uint32_t);
    *cm = RX_PPDU_START_4_RSSI_COMB_MASK;
    *cs = RX_PPDU_START_4_RSSI_COMB_LSB;
    return (void *)&rxd->ppdu_start;
}
#endif

static uint32_t
_qca9984_rx_desc_is_fcs_err(void *_rxd)
{
    struct rx_desc_base *rxd;
    uint32_t *ptr;
    rxd = _rxd;
    ptr = (void *)&rxd->attention;
    return ptr[0] & RX_ATTENTION_0_FCS_ERR_MASK;
}

struct ar_rx_desc_ops qca9984_rx_desc_ops = {
    .sizeof_rx_desc = _qca9984_rx_desc_size,
    .offsetof_attention = _qca9984_rx_desc_offsetof_attention,
    .offsetof_frag_info = _qca9984_rx_desc_offsetof_frag_info,
    .offsetof_mpdu_start = _qca9984_rx_desc_offsetof_mpdu_start,
    .offsetof_msdu_start = _qca9984_rx_desc_offsetof_msdu_start,
    .offsetof_msdu_end = _qca9984_rx_desc_offsetof_msdu_end,
    .offsetof_mpdu_end = _qca9984_rx_desc_offsetof_mpdu_end,
    .offsetof_ppdu_start = _qca9984_rx_desc_offsetof_ppdu_start,
    .offsetof_ppdu_end = _qca9984_rx_desc_offsetof_ppdu_end,
    .offsetof_fw_desc = _qca9984_rx_desc_offsetof_fw_desc,
    .offsetof_hdr_status = _qca9984_rx_desc_offsetof_hdr_status,
    .sizeof_attention = _qca9984_rx_desc_sizeof_attention,
    .sizeof_frag_info = _qca9984_rx_desc_sizeof_frag_info,
    .sizeof_mpdu_start = _qca9984_rx_desc_sizeof_mpdu_start,
    .sizeof_msdu_start = _qca9984_rx_desc_sizeof_msdu_start,
    .sizeof_msdu_end = _qca9984_rx_desc_sizeof_msdu_end,
    .sizeof_mpdu_end = _qca9984_rx_desc_sizeof_mpdu_end,
    .sizeof_ppdu_start = _qca9984_rx_desc_sizeof_ppdu_start,
    .sizeof_ppdu_end = _qca9984_rx_desc_sizeof_ppdu_end,
    .init_desc = _qca9984_rx_desc_init,
    .wifi_hdr_retrieve = _qca9984_rx_desc_mpdu_wifi_hdr_retrieve,
    .mpdu_desc_seq_num = _qca9984_rx_desc_mpdu_desc_seq_num,
    .mpdu_desc_pn = _qca9984_rx_desc_mpdu_desc_pn,
    .mpdu_desc_frds = _qca9984_rx_desc_mpdu_desc_frds,
    .mpdu_desc_tods = _qca9984_rx_desc_mpdu_desc_tods,
    .mpdu_is_encrypted = _qca9984_rx_desc_mpdu_is_encrypted,
    .get_attn_word = _qca9984_rx_desc_get_attn_word,
    .get_ppdu_start = _qca9984_rx_desc_get_ppdu_start,
    .attn_msdu_done = _qca9984_rx_desc_attn_msdu_done,
    .msdu_desc_msdu_length = _qca9984_rx_desc_msdu_length,
    .msdu_desc_completes_mpdu = _qca9984_rx_desc_msdu_desc_completes_mpdu,
    .msdu_has_wlan_mcast_flag = _qca9984_rx_desc_msdu_has_wlan_mcast_flag,
    .msdu_is_wlan_mcast = _qca9984_rx_desc_msdu_is_wlan_mcast,
    .msdu_is_frag = _qca9984_rx_desc_msdu_is_frag,
    .msdu_first_msdu_flag = _qca9984_rx_desc_msdu_first_msdu_flag,
    .msdu_key_id_octet = _qca9984_rx_desc_msdu_key_id_octet,
    .dump_desc = _qca9984_rx_desc_dump,
    .msdu_end = _qca9984_rx_desc_msdu_end,
    .check_desc_mgmt_type = _qca9984_rx_desc_check_desc_mgmt_type,
    .check_desc_ctrl_type = _qca9984_rx_desc_check_desc_ctrl_type,
#ifndef REMOVE_PKT_LOG
    .check_desc_phy_data_type = _qca9984_rx_desc_check_desc_phy_data_type,
#endif
    .get_l3_header_padding = _qca9984_rx_desc_get_l3_header_padding,
    .amsdu_pop = _qca9984_rx_desc_amsdu_pop,
    .restitch_mpdu_from_msdus = _qca9984_rx_desc_restitch_mpdu_from_msdus,
    .get_vowext_stats = _qca9984_rx_desc_get_vowext_stats,
#if UNIFIED_SMARTANTENNA
    .get_smart_ant_stats = _qca9984_rx_desc_get_smart_ant_stats,
#else
    .get_smart_ant_stats = NULL,
#endif
    .msdu_desc_phy_data_type = _qca9984_msdu_desc_phy_data_type,
    .msdu_desc_msdu_chained = _qca9984_msdu_desc_msdu_chained,
    .msdu_desc_tsf_timestamp = _qca9984_msdu_desc_tsf_timestamp,
    .fw_rx_desc_size = _qca9984_fw_rx_desc_size,
#if ATH_DATA_RX_INFO_EN || MESH_MODE_SUPPORT
    .update_pkt_info = _qca9984_rx_desc_update_pkt_info,
#endif
    .update_msdu_info = _qca9984_rx_desc_update_msdu_info,
#ifdef ATH_SUPPORT_RDKB
    .is_data = _qca9984_rx_desc_is_data,
    .is_first_mpdu = _qca9984_rx_desc_is_first_mpdu,
    .is_retry = _qca9984_rx_desc_is_retry,
    .get_cmn_ri = _qca9984_rx_desc_get_cmn_ri,
    .get_rssi = _qca9984_rx_desc_get_rssi,
#endif
    .is_fcs_err = _qca9984_rx_desc_is_fcs_err,
};

struct ar_rx_desc_ops* qca9984_rx_attach(struct ar_s *ar)
{
    /* Attach the function pointers table */
    ar->ar_rx_ops = &qca9984_rx_desc_ops;
    return ar->ar_rx_ops;
}

