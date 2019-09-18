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

/*
 * @File: 
 * 
 * @Abstract: ATD Data Structure for interfacing between Target and Host
 *
 */
#ifndef __ATD_CMD_H
#define __ATD_CMD_H

#include <acfg_api_types.h>
#include <spectral_cmd.h>

/*
 * Hostapd Ioctls sends max 10 bytes but receives 
 * big size data. How to know valid bytes in the 
 * buffer sent by Hostapd ??. To be optmized
 */
#define VALID_PAYLOAD_SIZE 256

typedef struct {
    a_uint8_t  mac_addr[MAX_NUM_RADIO][ACFG_MACADDR_LEN];
    a_uint8_t  ifname[MAX_NUM_RADIO][ACFG_MAX_IFNAME];
    a_uint8_t  num_radio;
    a_uint8_t  pad[3];
}__attribute__((__packed__)) atd_radio_query_resp_t;

typedef struct {
      a_uint8_t  wifi_index;
      a_uint8_t  vap_index ;
      a_uint16_t res;
} __attribute__((__packed__)) atd_vap_open_cmd_t;

typedef atd_vap_open_cmd_t atd_vap_close_cmd_t;

typedef struct {
    a_uint8_t  wifi_index;
    a_uint8_t  icp_name[ACFG_MAX_IFNAME];        /* parent device */
    a_uint16_t icp_opmode;            /* operating mode */
    a_uint16_t icp_flags;            /* see below */
    a_int32_t  icp_vapid;            /* vap id for MAC request */
} __attribute__((__packed__)) atd_vap_create_cmd_t;

typedef struct {
      a_uint8_t  vap_index ;
      a_uint8_t  vap_bssid[ACFG_MACADDR_LEN];
      a_uint8_t  name [ACFG_MAX_IFNAME];
} __attribute__((__packed__)) atd_vap_create_rsp_t;

typedef struct {
      a_uint32_t param;
      a_uint32_t val;
} __attribute__((__packed__)) atd_param_t;

typedef struct {
      a_uint32_t param[3];
      a_uint32_t val;
} __attribute__((__packed__)) atd_wmmparams_t;

typedef acfg_vendor_param_req_t atd_vendor_param_t;

typedef acfg_rssi_t atd_rssi_t;

typedef acfg_custdata_t atd_custdata_t;

/**
 * @brief Get/Set wireless commands
 */
#define IOCTL_SET_MASK     (0x80000000)
#define IOCTL_GET_MASK     (0x40000000)
#define IOCTL_PHYDEV_MASK  (0x08000000)
#define IOCTL_VIRDEV_MASK  (0x04000000)

#define IEEE80211_IOCTL_TESTMODE            (IOCTL_VIRDEV_MASK | 1)       /* Test Mode for Factory */
#define IEEE80211_IOCTL_STAINFO             (IOCTL_VIRDEV_MASK | 2)       /* Station Information */          
#define IEEE80211_IOCTL_PHYMODE             (IOCTL_VIRDEV_MASK | 3)       /* PHY Mode */          
#define IEEE80211_IOCTL_SCANTIME            (IOCTL_VIRDEV_MASK | 4)       /* Foreground Active Scan Time (T8) */          
#define IEEE80211_IOCTL_SCAN                (IOCTL_VIRDEV_MASK | 5)       /* Foreground Active Scan */  
#define IEEE80211_IOCTL_RSSI                (IOCTL_VIRDEV_MASK | 6)       /* Get RSSI */    
#define IEEE80211_IOCTL_CUSTDATA            (IOCTL_VIRDEV_MASK | 7)       /* Get custdata */  

typedef struct {
      a_uint32_t vendor;
      a_uint32_t cmd;
      a_uint8_t   name[16];
} atd_vendor_t;

typedef struct atd_ssid {
    a_uint32_t  len;
    a_uint8_t   name[ACFG_MAX_SSID_LEN + 1];
} __attribute__((__packed__)) atd_ssid_t;

typedef struct atd_set_scan {
    a_uint8_t            scan_type; /* IW_SCAN_TYPE_{ACTIVE,PASSIVE} */
    a_uint8_t            essid_len;
    a_uint8_t            num_channels; /* num entries in channel_list; */
    a_uint8_t            flags;
    struct {
        a_uint16_t sa_family;    /* Common data: address family and length.  */
        char sa_data[14];        /* Address data.  */
    } bssid;
    a_uint8_t            essid[ACFG_MAX_SSID_LEN];
    a_uint32_t           min_channel_time; /* in TU */
    a_uint32_t           max_channel_time; /* in TU */
    struct {
        a_int32_t        m;
        a_int16_t        e;
        a_uint8_t        i;
        a_uint8_t        flags;
    } channel_list[ACFG_MAX_FREQ];

    a_uint16_t           len;
    a_uint16_t           point_flags;
} __attribute__((__packed__)) atd_set_scan_t;

typedef acfg_scan_t atd_scan_t;

typedef struct atd_ath_stats {
    a_uint32_t          size;
    a_uint32_t          offload_if;
} __attribute__((__packed__)) atd_ath_stats_t;

typedef struct  atd_testmode {
    a_uint8_t     bssid[ACFG_MACADDR_LEN];
    a_int32_t     chan;
    a_uint16_t    operation;
    a_uint8_t     antenna;
    a_uint8_t     rx;
    a_int32_t     rssi_combined;
    a_int32_t     rssi0;
    a_int32_t     rssi1;
    a_int32_t     rssi2;
} __attribute__((__packed__)) atd_testmode_t;

typedef struct atd_chmode {
    a_uint32_t  len;
    a_uint8_t   mode[ACFG_MAX_CHMODE_LEN];
} __attribute__((__packed__)) atd_chmode_t;

typedef struct atd_encode_rsp {
    a_uint32_t len ;
    acfg_encode_flags_t flag ;
    a_uint8_t key[ACFG_ENCODING_TOKEN_MAX] ;
} __attribute__((__packed__)) atd_encode_rsp_t ;

typedef struct atd_powmgmt {
    a_uint32_t val ;    
    a_uint16_t flags ;
    a_uint8_t disabled ;
} __attribute__((__packed__)) atd_powmgmt_t ;

typedef struct atd_txpow {
    a_uint32_t val ;
    acfg_txpow_flags_t flags ;
} __attribute__((__packed__)) atd_txpow_t ;

typedef struct atd_rts {
    a_uint32_t val ;
    acfg_rts_flags_t flags ;
} __attribute__((__packed__)) atd_rts_t ;

typedef struct atd_frag {
    a_uint32_t val ;
    acfg_frag_flags_t flags ;
} __attribute__((__packed__)) atd_frag_t ;

typedef struct atd_range_buf {
    a_uint32_t           throughput;
    a_uint32_t           min_nwid;
    a_uint32_t           max_nwid;
    a_uint16_t           old_num_channels;
    a_uint8_t            old_num_frequency;
#if WIRELESS_EXT >= 17
    a_uint32_t           event_capa[6];
#endif
    a_int32_t            sensitivity;
    struct {
        a_uint8_t        qual;
        a_uint8_t        level;
        a_uint8_t        noise;
        a_uint8_t        updated;
    } max_qual, avg_qual;
    a_uint8_t            num_bitrates;
    a_int32_t            bitrate[ACFG_MAX_BITRATE];
    a_int32_t            min_rts;
    a_int32_t            max_rts;
    a_int32_t            min_frag;
    a_int32_t            max_frag;
    a_int32_t            min_pmp;
    a_int32_t            max_pmp;
    a_int32_t            min_pmt;
    a_int32_t            max_pmt;
    a_uint16_t           pmp_flags;
    a_uint16_t           pmt_flags;
    a_uint16_t           pm_capa;
    a_uint16_t           encoding_size[ACFG_MAX_ENCODING];
    a_uint8_t            num_encoding_sizes;
    a_uint8_t            max_encoding_tokens;
    a_uint8_t            encoding_login_index;
    a_uint16_t           txpower_capa;
    a_uint8_t            num_txpower;
#if WIRELESS_EXT >= 10
    a_int32_t            txpower[ACFG_MAX_TXPOWER];
#endif
    a_uint8_t            we_version_compiled;
    a_uint8_t            we_version_source;
    a_uint16_t           retry_capa;
    a_uint16_t           retry_flags;
    a_uint16_t           r_time_flags;
    a_int32_t            min_retry;
    a_int32_t            max_retry;
    a_int32_t            min_r_time;
    a_int32_t            max_r_time;
    a_uint16_t           num_channels;
    a_uint8_t            num_frequency;
    struct {
        a_int32_t        m;
        a_int16_t        e;
        a_uint8_t        i;
        a_uint8_t        flags;
    } freq[ACFG_MAX_FREQ];
#if WIRELESS_EXT >= 18
    a_uint32_t           enc_capa;
#endif
} __attribute__((__packed__)) atd_range_buf_t;

typedef struct atd_key {
    a_uint8_t        type;
    a_uint8_t        pad;
    a_uint16_t       keyix;
    a_uint8_t        keylen;
    a_uint8_t        flags;
    a_uint8_t        macaddr[ACFG_MACADDR_LEN];
    a_uint64_t       keyrsc;
    a_uint64_t       keytsc;
    a_uint8_t        keydata[ACFG_KEYDATA_LEN];
} __attribute__((__packed__)) atd_key_t;

typedef struct atd_scan_result {
    a_uint16_t       isr_len;
    a_uint16_t       isr_freq;
    a_uint32_t       isr_flags; 
    a_uint8_t        isr_noise;
    a_uint8_t        isr_rssi;
    a_uint8_t        isr_intval;
    a_uint16_t       isr_capinfo;
    a_uint8_t        isr_erp;
    a_uint8_t        isr_bssid[ACFG_MACADDR_LEN];
    a_uint8_t        isr_nrates;
    a_uint8_t        isr_rates[36];
    a_uint8_t        isr_ssid_len;
    a_uint16_t       isr_ie_len;
} __attribute__((__packed__)) atd_scan_result_t;

typedef struct atd_mlme {
    a_uint8_t        op;
    a_uint8_t        ssid_len;
    a_uint16_t       reason;
    a_uint16_t       seq;
    a_uint8_t        macaddr[ACFG_MACADDR_LEN];
    a_uint8_t        ssid[ACFG_MAX_SSID_LEN];
    a_uint8_t        optie[ACFG_MAX_IELEN * 2];
    a_uint16_t       optie_len;
} __attribute__((__packed__)) atd_mlme_t;

typedef struct atd_channel {
    a_uint16_t   freq;
    a_uint32_t   flags;
    a_uint8_t    flag_ext;
    a_uint8_t    ieee_num;
    a_int8_t     max_reg_pwr;
    a_int8_t     max_pwr;
    a_int8_t     min_pwr;
    a_uint8_t    reg_class;
    a_uint8_t    antenna_max;
    a_uint8_t    vht_ch_freq_seg1;
    a_uint8_t    vht_ch_freq_seg2;

} __attribute__((__packed__)) atd_channel_t;

typedef struct atd_chan_info {
    unsigned int   num_channels;
    atd_channel_t  channel[ACFG_MAX_IEEE_CHAN];
} __attribute__((__packed__)) atd_chan_info_t;

typedef acfg_filter_t atd_filter_t;

typedef struct atd_appie {
    a_uint32_t       frmtype;
    a_uint32_t       buflen;
    a_uint8_t        buf[0];
} __attribute__((__packed__)) atd_appie_t;

typedef struct atd_mgmt {
    a_uint8_t        macaddr[ACFG_MACADDR_LEN];
    a_uint32_t       buflen;
    a_uint8_t        buf[0];
} __attribute__((__packed__)) atd_mgmt_t;

typedef acfg_delkey_t atd_delkey_t;

typedef struct atd_nawds_cfg {
    NAWDS_CMDTYPE cmdtype;
    NAWDS_STATUS status;
    union {
        struct {
            a_uint8_t num;
            a_uint8_t mode;
            a_uint8_t defcaps;
            a_uint8_t override;
            a_uint8_t mac[ACFG_MACADDR_LEN];
            a_uint8_t caps;
        }nawds;

        struct acfg_wnm_cfg wnm;
    }data;
} __attribute__((__packed__)) atd_nawds_cfg_t;

typedef struct atd_stainfo {
    a_uint16_t   isi_len;
    a_uint16_t   isi_freq;
    a_uint32_t   isi_flags;
    a_uint16_t   isi_state;
    a_uint8_t    isi_authmode;
    a_int8_t          isi_rssi;
    a_uint16_t   isi_capinfo;
    a_uint8_t    isi_athflags;
    a_uint8_t    isi_erp;
    a_int8_t	 isi_ps;
    a_uint8_t    isi_macaddr[ACFG_MACADDR_LEN];
    a_uint8_t    isi_nrates;

    a_uint8_t    isi_rates[36];
    a_uint8_t    isi_txrate;
    a_uint32_t   isi_txratekbps;
    a_uint16_t   isi_ie_len;
    a_uint16_t   isi_associd;
    a_uint16_t   isi_txpower;
    a_uint16_t   isi_vlan;
    a_uint16_t   isi_txseqs[17];
    a_uint16_t   isi_rxseqs[17];
    a_uint16_t   isi_inact;
    a_uint8_t    isi_uapsd;
    a_uint8_t    isi_opmode;
    a_uint8_t    isi_cipher;
    a_uint32_t   isi_assoc_time;
    a_uint32_t 	 isi_tr069_assoc_time_sec;
    a_uint32_t 	 isi_tr069_assoc_time_nsec;
    a_uint16_t   isi_htcap;
    a_uint32_t   isi_rxratekbps;
    a_uint8_t    isi_maxrate_per_client;
    a_uint16_t    isi_stamode;
} __attribute__((__packed__)) atd_stainfo_t;

typedef acfg_macaddr_t atd_macaddr_t;

typedef struct {
    a_uint8_t          radio_name[ACFG_MAX_IFNAME];
    a_uint8_t          chan;
    a_uint32_t         freq;
    a_uint16_t         country_code;
    a_uint8_t          radio_mac[ACFG_MACADDR_LEN];
    struct {
        a_uint8_t      vap_name[ACFG_MAX_IFNAME];
        a_uint8_t      vap_mac[ACFG_MACADDR_LEN];
        a_uint32_t     phymode;
        a_uint8_t      sec_method;
        a_uint8_t      cipher;
        a_uint8_t      wep_key[ACFG_MAX_PSK_LEN];
        a_uint8_t      wep_key_len;
        a_uint8_t      wep_key_idx;
    } vap_info[ACFG_MAX_VAPS];
    a_uint8_t          num_vaps;
} __attribute__((__packed__)) atd_radio_vap_info_t;

typedef struct atd_tx99_data {
    a_uint32_t freq;
    a_uint32_t htmode;
    a_uint32_t htext;
    a_uint32_t rate;
    a_uint32_t rc;
    a_uint32_t power;
    a_uint32_t txmode;
    a_uint32_t chanmask;
    a_uint32_t type;
} __attribute__((__packed__)) atd_tx99_data_t;

typedef struct atd_tx99 {
    a_uint8_t           if_name[ACFG_MAX_IFNAME];
    acfg_tx99_type_t    type;
    atd_tx99_data_t     data;
} __attribute__((__packed__)) atd_tx99_t;

typedef struct atd_rate {
    a_int32_t value;
    a_uint8_t fixed;
} __attribute__((__packed__)) atd_rate_t ;

typedef struct atd_stats {
    a_uint32_t status;
    a_uint32_t link_quality;
    a_uint32_t signal_level;
    a_uint32_t noise_level;
    a_uint32_t updated;
    a_uint32_t discard_nwid;
    a_uint32_t discard_code;
    a_uint32_t missed_beacon;
} __attribute__((__packed__)) atd_stats_t;

typedef struct atd_ath_diag {
    a_uint8_t  ad_name[ACFG_MAX_IFNAME];
    a_uint16_t ad_id;
    a_uint16_t ad_in_size;
    a_uint32_t ad_out_size;
} __attribute__((__packed__)) atd_ath_diag_t;

typedef struct atd_acl_mac {
    a_uint32_t num;
    a_uint8_t  macaddr[ACFG_MAX_ACL_NODE][ACFG_MACADDR_LEN];
} __attribute__((__packed__)) atd_macacl_t ;

/* for wsupp bridge message exchange */
typedef struct atd_wsupp_message {
    a_uint16_t len;
    a_uint8_t data[0];
} __attribute__((__packed__)) atd_wsupp_message_t;

typedef struct atd_ath_stats_11n {
    a_uint32_t    ast_watchdog;
    a_uint32_t    ast_resetOnError;
    a_uint32_t    ast_mat_ucast_encrypted;
    a_uint32_t    ast_mat_rx_recrypt;
    a_uint32_t    ast_mat_rx_decrypt;
    a_uint32_t    ast_mat_key_switch;
    a_uint32_t    ast_hardware;
    a_uint32_t    ast_bmiss;
    a_uint32_t    ast_rxorn;
    a_uint32_t    ast_rxorn_bmiss;
    a_uint32_t    ast_rxeol;
    a_uint32_t    ast_txurn;
    a_uint32_t    ast_txto;
    a_uint32_t    ast_cst;
    a_uint32_t    ast_mib;
    a_uint32_t    ast_rx;
    a_uint32_t    ast_rxdesc;
    a_uint32_t    ast_rxerr;
    a_uint32_t    ast_rxnofrm;
    a_uint32_t    ast_tx;
    a_uint32_t    ast_txdesc;
    a_uint32_t    ast_tim_timer;
    a_uint32_t    ast_bbevent;
    a_uint32_t    ast_rxphy;
    a_uint32_t    ast_rxkcm;
    a_uint32_t    ast_swba;
    a_uint32_t    ast_brssi;
    a_uint32_t    ast_bnr;
    a_uint32_t    ast_tim;
    a_uint32_t    ast_dtim;
    a_uint32_t    ast_dtimsync;
    a_uint32_t    ast_gpio;
    a_uint32_t    ast_cabend;
    a_uint32_t    ast_tsfoor;
    a_uint32_t    ast_gentimer;
    a_uint32_t    ast_gtt;
    a_uint32_t    ast_fatal;
    a_uint32_t    ast_tx_packets;
    a_uint32_t    ast_rx_packets;
    a_uint32_t    ast_tx_mgmt;
    a_uint32_t    ast_tx_discard;
    a_uint32_t    ast_tx_invalid;
    a_uint32_t    ast_tx_qstop;
    a_uint32_t    ast_tx_encap;
    a_uint32_t    ast_tx_nonode;
    a_uint32_t    ast_tx_nobuf;
    a_uint32_t    ast_tx_stop;
    a_uint32_t    ast_tx_resume;
    a_uint32_t    ast_tx_nobufmgt;
    a_uint32_t    ast_tx_xretries;
    a_uint64_t    ast_tx_hw_retries;
    a_uint64_t    ast_tx_hw_success;
    a_uint32_t    ast_tx_fifoerr;
    a_uint32_t    ast_tx_filtered;
    a_uint32_t    ast_tx_badrate;
    a_uint32_t    ast_tx_noack;
    a_uint32_t    ast_tx_cts;
    a_uint32_t    ast_tx_shortpre;
    a_uint32_t    ast_tx_altrate;
    a_uint32_t    ast_tx_protect;
    a_uint32_t    ast_rx_orn;
    a_uint32_t    ast_rx_badcrypt;
    a_uint32_t    ast_rx_badmic;
    a_uint32_t    ast_rx_nobuf;
    a_uint32_t    ast_rx_swdecrypt;
    a_uint32_t    ast_rx_hal_in_progress;
    a_uint32_t    ast_rx_num_data;
    a_uint32_t    ast_rx_num_mgmt;
    a_uint32_t    ast_rx_num_ctl;
    a_uint32_t    ast_rx_num_unknown;
    a_uint32_t    ast_max_pkts_per_intr;
    a_uint32_t    ast_pkts_per_intr[ACFG_MAX_INTR_BKT+1];
    a_int8_t      ast_tx_rssi;
    a_int8_t      ast_tx_rssi_ctl0;
    a_int8_t      ast_tx_rssi_ctl1;
    a_int8_t      ast_tx_rssi_ctl2;
    a_int8_t      ast_tx_rssi_ext0;
    a_int8_t      ast_tx_rssi_ext1;
    a_int8_t      ast_tx_rssi_ext2;
    a_int8_t      ast_rx_rssi;
    a_int8_t      ast_rx_rssi_ctl0;
    a_int8_t      ast_rx_rssi_ctl1;
    a_int8_t      ast_rx_rssi_ctl2;
    a_int8_t      ast_rx_rssi_ext0;
    a_int8_t      ast_rx_rssi_ext1;
    a_int8_t      ast_rx_rssi_ext2;
    a_uint32_t    ast_be_xmit;
    a_uint32_t    ast_be_nobuf;
    a_uint32_t    ast_per_cal;
    a_uint32_t    ast_per_calfail;
    a_uint32_t    ast_per_rfgain;
    a_uint32_t    ast_rate_calls;
    a_uint32_t    ast_rate_raise;
    a_uint32_t    ast_rate_drop;
    a_uint32_t    ast_ant_defswitch;
    a_uint32_t    ast_ant_txswitch;
    a_uint32_t    ast_ant_rx[8];
    a_uint32_t    ast_ant_tx[8];
    a_uint64_t    ast_rx_bytes;
    a_uint64_t    ast_tx_bytes;
    a_uint32_t    ast_rx_num_qos_data[16];
    a_uint32_t    ast_rx_num_nonqos_data;
    a_uint32_t    ast_txq_packets[16];
    a_uint32_t    ast_txq_xretries[16];
    a_uint32_t    ast_txq_fifoerr[16];
    a_uint32_t    ast_txq_filtered[16];
    a_uint32_t    ast_txq_athbuf_limit[16];
    a_uint32_t    ast_txq_nobuf[16];
    a_uint8_t     ast_num_rxchain;
    a_uint8_t     ast_num_txchain;
    struct {
        a_uint32_t   tx_pkts;
        a_uint32_t   tx_checks;
        a_uint32_t   tx_drops;
        a_uint32_t   tx_minqdepth;
        a_uint32_t   tx_queue;
        a_uint32_t   tx_resetq;
        a_uint32_t   tx_comps;
        a_uint32_t   tx_comperror;
        a_uint32_t   tx_unaggr_comperror;
        a_uint32_t   tx_stopfiltered;
        a_uint32_t   tx_qnull;
        a_uint32_t   tx_noskbs;
        a_uint32_t   tx_nobufs;
        a_uint32_t   tx_badsetups;
        a_uint32_t   tx_normnobufs;
        a_uint32_t   tx_schednone;
        a_uint32_t   tx_bars;
        a_uint32_t   tx_legacy;
        a_uint32_t   txunaggr_single;
        a_uint32_t   txbar_xretry;
        a_uint32_t   txbar_compretries;
        a_uint32_t   txbar_errlast;
        a_uint32_t   tx_compunaggr;
        a_uint32_t   txunaggr_xretry;
        a_uint32_t   tx_compaggr;
        a_uint32_t   tx_bawadv;
        a_uint32_t   tx_bawretries;
        a_uint32_t   tx_bawnorm;
        a_uint32_t   tx_bawupdates;
        a_uint32_t   tx_bawupdtadv;
        a_uint32_t   tx_retries;
        a_uint32_t   tx_xretries;
        a_uint32_t   tx_aggregates;
        a_uint32_t   tx_sf_hw_xretries;
        a_uint32_t   tx_aggr_frames;
        a_uint32_t   txaggr_noskbs;
        a_uint32_t   txaggr_nobufs;
        a_uint32_t   txaggr_badkeys;
        a_uint32_t   txaggr_schedwindow;
        a_uint32_t   txaggr_single;
        a_uint32_t   txaggr_mimo;
        a_uint32_t   txaggr_compgood;
        a_uint32_t   txaggr_comperror;
        a_uint32_t   txaggr_compxretry;
        a_uint32_t   txaggr_compretries;
        a_uint32_t   txunaggr_compretries;
        a_uint32_t   txaggr_prepends;
        a_uint32_t   txaggr_filtered;
        a_uint32_t   txaggr_fifo;
        a_uint32_t   txaggr_xtxop;
        a_uint32_t   txaggr_desc_cfgerr;
        a_uint32_t   txaggr_data_urun;
        a_uint32_t   txaggr_delim_urun;
        a_uint32_t   txaggr_errlast;
        a_uint32_t   txunaggr_errlast;
        a_uint32_t   txaggr_longretries;
        a_uint32_t   txaggr_shortretries;
        a_uint32_t   txaggr_timer_exp;
        a_uint32_t   txaggr_babug;
        a_uint32_t   txrifs_single;
        a_uint32_t   txrifs_babug;
        a_uint32_t   txaggr_badtid;
        a_uint32_t   txrifs_compretries;
        a_uint32_t   txrifs_bar_alloc;
        a_uint32_t   txrifs_bar_freed;
        a_uint32_t   txrifs_compgood;
        a_uint32_t   txrifs_prepends;
        a_uint32_t   tx_comprifs;
        a_uint32_t   tx_compnorifs;
        a_uint32_t   rx_pkts;
        a_uint32_t   rx_aggr;
        a_uint32_t   rx_aggrbadver;
        a_uint32_t   rx_bars;
        a_uint32_t   rx_nonqos;
        a_uint32_t   rx_seqreset;
        a_uint32_t   rx_oldseq;
        a_uint32_t   rx_bareset;
        a_uint32_t   rx_baresetpkts;
        a_uint32_t   rx_dup;
        a_uint32_t   rx_baadvance;
        a_uint32_t   rx_recvcomp;
        a_uint32_t   rx_bardiscard;
        a_uint32_t   rx_barcomps;
        a_uint32_t   rx_barrecvs;
        a_uint32_t   rx_skipped;
        a_uint32_t   rx_comp_to;
        a_uint32_t   rx_timer_starts;
        a_uint32_t   rx_timer_stops;
        a_uint32_t   rx_timer_run;
        a_uint32_t   rx_timer_more;
        a_uint32_t   wd_tx_active;
        a_uint32_t   wd_tx_inactive;
        a_uint32_t   wd_tx_hung;
        a_uint32_t   wd_spurious;
        a_uint32_t   tx_requeue;
        a_uint32_t   tx_drain_txq;
        a_uint32_t   tx_drain_tid;
        a_uint32_t   tx_cleanup_tid;
        a_uint32_t   tx_drain_bufs;
        a_uint32_t   tx_tidpaused;
        a_uint32_t   tx_tidresumed;
        a_uint32_t   tx_unaggr_filtered;
        a_uint32_t   tx_aggr_filtered;
        a_uint32_t   tx_filtered;
        a_uint32_t   rx_rb_on;
        a_uint32_t   rx_rb_off;
        a_uint32_t   rx_dsstat_err;
        a_uint32_t   bf_stream_miss;
        a_uint32_t   bf_bandwidth_miss;
        a_uint32_t   bf_destination_miss;
    } ast_11n_stats;
    struct {
        a_uint32_t           dfs_stats_valid;
        a_uint32_t           event_count;
        a_uint32_t           chirp_count;
        a_uint32_t           num_filter;
        struct {
            a_uint32_t       max_pri_count;
            a_uint32_t       max_used_pri;
            a_uint32_t       excess_pri;
            a_uint32_t       pri_threshold_reached;
            a_uint32_t       dur_threshold_reached;
            a_uint32_t       rssi_threshold_reached;
            a_uint32_t       filter_id;
        } fstat[ACFG_MAX_DFS_FILTER];
    } ast_dfs_stats;
    a_uint32_t   ast_bb_hang;
    a_uint32_t   ast_mac_hang;
    a_uint32_t   ast_wow_wakeups;
    a_uint32_t   ast_wow_wakeupsok;
    a_uint32_t   ast_wow_wakeupserror;
    a_uint32_t   ast_normal_sleeps;
    a_uint32_t   ast_normal_wakeups;
    a_uint32_t   ast_wow_sleeps;
    a_uint32_t   ast_wow_sleeps_nonet;
    a_uint32_t   ast_uapsdqnulbf_unavail;
    a_uint32_t   ast_uapsdqnul_pkts;
    a_uint32_t   ast_uapsdtriggers;
    a_uint32_t   ast_uapsdnodeinvalid;
    a_uint32_t   ast_uapsdeospdata;
    a_uint32_t   ast_uapsddata_pkts;
    a_uint32_t   ast_uapsddatacomp;
    a_uint32_t   ast_uapsdqnulcomp;
    a_uint32_t   ast_uapsddataqueued;
    a_uint32_t   ast_uapsdedmafifofull;
    a_uint32_t   ast_vow_ul_tx_calls[4];
    a_uint32_t   ast_vow_ath_txq_calls[4];
    a_uint32_t   ast_vow_ath_be_drop, ast_vow_ath_bk_drop;
    a_uint32_t   ast_cfend_sched;
    a_uint32_t   ast_cfend_sent;
    a_uint32_t   ast_retry_delay[ACFG_LATENCY_CATS][ACFG_LATENCY_BINS];
    a_uint32_t   ast_queue_delay[ACFG_LATENCY_CATS][ACFG_LATENCY_BINS];
    a_uint32_t   vi_timestamp[ACFG_VI_LOG_LEN];
    a_uint8_t    vi_rssi_ctl0[ACFG_VI_LOG_LEN];
    a_uint8_t    vi_rssi_ctl1[ACFG_VI_LOG_LEN];
    a_uint8_t    vi_rssi_ctl2[ACFG_VI_LOG_LEN];
    a_uint8_t    vi_rssi_ext0[ACFG_VI_LOG_LEN];
    a_uint8_t    vi_rssi_ext1[ACFG_VI_LOG_LEN];
    a_uint8_t    vi_rssi_ext2[ACFG_VI_LOG_LEN];
    a_uint8_t    vi_rssi[ACFG_VI_LOG_LEN];
    a_uint8_t    vi_evm0[ACFG_VI_LOG_LEN];
    a_uint8_t    vi_evm1[ACFG_VI_LOG_LEN];
    a_uint8_t    vi_evm2[ACFG_VI_LOG_LEN];
    a_uint8_t    vi_rs_rate[ACFG_VI_LOG_LEN];
    a_uint32_t   vi_tx_frame_cnt[ACFG_VI_LOG_LEN];
    a_uint32_t   vi_rx_frame_cnt[ACFG_VI_LOG_LEN];
    a_uint32_t   vi_rx_clr_cnt[ACFG_VI_LOG_LEN];
    a_uint32_t   vi_rx_ext_clr_cnt[ACFG_VI_LOG_LEN];
    a_uint32_t   vi_cycle_cnt[ACFG_VI_LOG_LEN];
    a_uint8_t    vi_stats_index;
    a_uint8_t    ast_txbf;
    a_uint8_t    ast_lastratecode;
    a_uint32_t   ast_sounding_count;
    a_uint32_t   ast_txbf_rpt_count;
    a_uint32_t   ast_mcs_count[ACFG_MCS_RATES+1];
    a_uint32_t   ast_rx_looplimit_start;
    a_uint32_t   ast_rx_looplimit_end;
    a_uint32_t   ast_chan_clr_cnt;
    a_uint32_t   ast_cycle_cnt;
    a_int16_t    ast_noise_floor;
    struct {
        a_uint32_t   ast_ackrcv_bad;
        a_uint32_t   ast_rts_bad;
        a_uint32_t   ast_rts_good;
        a_uint32_t   ast_fcs_bad;
        a_uint32_t   ast_beacons;
        struct {
            a_uint32_t   tx_shortretry;
            a_uint32_t   tx_longretry;
            a_uint32_t   tx_xretries;
            a_uint32_t   ht_tx_unaggr_xretry;
            a_uint32_t   ht_tx_xretries;
            a_uint32_t   tx_pkt;
            a_uint32_t   tx_aggr;
            a_uint32_t   tx_retry;
            a_uint32_t   txaggr_retry;
            a_uint32_t   txaggr_sub_retry;
        } ast_tgt_stats;
    } ast_mib_stats;
    struct {
        int        valid;
        a_uint32_t status;
        a_uint32_t tsf;
        a_uint32_t phy_panic_wd_ctl1;
        a_uint32_t phy_panic_wd_ctl2;
        a_uint32_t phy_gen_ctrl;
        a_uint32_t rxc_pcnt;
        a_uint32_t rxf_pcnt;
        a_uint32_t txf_pcnt;
        a_uint32_t cycles;
        a_uint32_t wd;
        a_uint32_t det;
        a_uint32_t rdar;
        a_uint32_t r_odfm;
        a_uint32_t r_cck;
        a_uint32_t t_odfm;
        a_uint32_t t_cck;
        a_uint32_t agc;
        a_uint32_t src;
    } ast_bb_panic[ACFG_MAX_BB_PANICS];
} __attribute__((__packed__)) atd_ath_stats_11n_t;

typedef struct atd_txrx_elem {
    a_uint64_t      pkts;
    a_uint64_t      bytes;
} __attribute__((__packed__)) atd_txrx_elem_t;

typedef struct atd_txrx_stats {
    struct {
        atd_txrx_elem_t      from_stack;
        atd_txrx_elem_t      delivered;
        struct {
            atd_txrx_elem_t  host_reject;
            atd_txrx_elem_t  download_fail;
            atd_txrx_elem_t  target_discard;
            atd_txrx_elem_t  no_ack;
        } dropped;
        a_uint32_t           desc_in_use;
        a_uint32_t           desc_alloc_fails;
        a_uint32_t           ce_ring_full;
        a_uint32_t           dma_map_error;
        atd_txrx_elem_t      mgmt;
    } tx;
    struct {
        atd_txrx_elem_t      delivered;
        atd_txrx_elem_t      forwarded;
    } rx;
} __attribute__((__packed__)) atd_txrx_stats_t;

typedef struct atd_ath_stats_11ac {
    int                       txrx_stats_level;
    atd_txrx_stats_t          txrx_stats;
    struct {
        struct {
            a_int32_t         comp_queued;
            a_int32_t         comp_delivered;
            a_int32_t         msdu_enqued;
            a_int32_t         mpdu_enqued;
            a_int32_t         wmm_drop;
            a_int32_t         local_enqued;
            a_int32_t         local_freed;
            a_int32_t         hw_queued;
            a_int32_t         hw_reaped;
            a_int32_t         underrun;
            a_int32_t         tx_abort;
            a_int32_t         mpdus_requed;
            a_uint32_t        tx_ko;
            a_uint32_t        data_rc;
            a_uint32_t        self_triggers;
            a_uint32_t        sw_retry_failure;
            a_uint32_t        illgl_rate_phy_err;
            a_uint32_t        pdev_cont_xretry;
            a_uint32_t        pdev_tx_timeout;
            a_uint32_t        pdev_resets;
            a_uint32_t        stateless_tid_alloc_failure;
            a_uint32_t        phy_underrun;
            a_uint32_t        txop_ovf;
        } tx;
        struct {
            a_int32_t         mid_ppdu_route_change;
            a_int32_t         status_rcvd;
            a_int32_t         r0_frags;
            a_int32_t         r1_frags;
            a_int32_t         r2_frags;
            a_int32_t         r3_frags;
            a_int32_t         htt_msdus;
            a_int32_t         htt_mpdus;
            a_int32_t         loc_msdus;
            a_int32_t         loc_mpdus;
            a_int32_t         oversize_amsdu;
            a_int32_t         phy_errs;
            a_int32_t         phy_err_drop;
            a_int32_t         mpdu_errs;
        } rx;
        struct {
            a_uint32_t        dram_remain;
            a_uint32_t        iram_remain;
        } mem;
        struct {
	    a_int32_t         dummy;
        } peer;
    } stats;
    struct {
        a_uint64_t    tx_beacon;
        a_uint32_t    be_nobuf;
        a_uint32_t    tx_buf_count; 
        a_uint32_t    tx_packets;
        a_uint32_t    rx_packets;
        a_int32_t     tx_mgmt;
        a_uint32_t    tx_num_data;
        a_uint32_t    rx_num_data;
        a_int32_t     rx_mgmt;
        a_uint32_t    rx_num_mgmt;
        a_uint32_t    rx_num_ctl;
        a_uint32_t    tx_rssi;
        a_uint32_t    tx_mcs[10];
        a_uint32_t    rx_mcs[10];
        a_uint32_t    rx_rssi_comb;
        struct {
            a_uint8_t     rx_rssi_pri20;
            a_uint8_t     rx_rssi_sec20;
            a_uint8_t     rx_rssi_sec40;
            a_uint8_t     rx_rssi_sec80;
        } rx_rssi_chain0;
        struct {
            a_uint8_t     rx_rssi_pri20;
            a_uint8_t     rx_rssi_sec20;
            a_uint8_t     rx_rssi_sec40;
            a_uint8_t     rx_rssi_sec80;
        } rx_rssi_chain1;
        struct {
            a_uint8_t     rx_rssi_pri20;
            a_uint8_t     rx_rssi_sec20;
            a_uint8_t     rx_rssi_sec40;
            a_uint8_t     rx_rssi_sec80;
        } rx_rssi_chain2;
        struct {
            a_uint8_t     rx_rssi_pri20;
            a_uint8_t     rx_rssi_sec20;
            a_uint8_t     rx_rssi_sec40;
            a_uint8_t     rx_rssi_sec80;
        } rx_rssi_chain3;
        a_uint64_t    rx_bytes;
        a_uint64_t    tx_bytes;
        a_uint32_t    tx_compaggr;
        a_uint32_t    rx_aggr;
        a_uint32_t    tx_bawadv;
        a_uint32_t    tx_compunaggr;
        a_uint32_t    rx_overrun;
        a_uint32_t    rx_badcrypt;
        a_uint32_t    rx_badmic;
        a_uint32_t    rx_crcerr;
        a_uint32_t    rx_phyerr;
        a_uint32_t    ackRcvBad;
        a_uint32_t    rtsBad;
        a_uint32_t    rtsGood;
        a_uint32_t    fcsBad;
        a_uint32_t    noBeacons;
        a_uint32_t    mib_int_count;
        a_uint32_t    rx_looplimit_start;
        a_uint32_t    rx_looplimit_end;
        a_uint8_t     ap_stats_tx_cal_enable;
    } interface_stats;
} __attribute__((__packed__)) atd_ath_stats_11ac_t;

typedef struct atd_bcnrpt {
    a_uint8_t bssid[6];
    a_uint8_t rsni;
    a_uint8_t rcpi;
    a_uint8_t chnum;
    a_uint8_t more;
} __attribute__((__packed__)) atd_bcnrpt_t;

typedef struct atd_rrmstats {
    a_uint8_t     chann_load[ACFG_MAX_IEEE_CHAN];
    struct {
        a_uint8_t        antid;
        a_int8_t         anpi;
        a_uint8_t        ipi[11]; 
    } noise_data[ACFG_MAX_IEEE_CHAN];
    struct {
        struct {
            a_uint32_t   txfragcnt;
            a_uint32_t   mcastfrmcnt;
            a_uint32_t   failcnt;
            a_uint32_t   rxfragcnt;
            a_uint32_t   mcastrxfrmcnt;
            a_uint32_t   fcserrcnt;
            a_uint32_t   txfrmcnt;
        } gid0;
        struct {
            a_uint32_t   rty;
            a_uint32_t   multirty;
            a_uint32_t   frmdup;
            a_uint32_t   rtsuccess;
            a_uint32_t   rtsfail;
            a_uint32_t   ackfail;
        } gid1;
        struct {
            a_uint32_t   qostxfragcnt;
            a_uint32_t   qosfailedcnt;
            a_uint32_t   qosrtycnt;
            a_uint32_t   multirtycnt;
            a_uint32_t   qosfrmdupcnt;
            a_uint32_t   qosrtssuccnt;
            a_uint32_t   qosrtsfailcnt;
            a_uint32_t   qosackfailcnt;
            a_uint32_t   qosrxfragcnt;
            a_uint32_t   qostxfrmcnt;
            a_uint32_t   qosdiscadrfrmcnt;
            a_uint32_t   qosmpdurxcnt;
            a_uint32_t   qosrtyrxcnt;
        } gidupx[8];
        struct {
             a_uint8_t   ap_avg_delay;
             a_uint8_t   be_avg_delay;
             a_uint8_t   bk_avg_delay;
             a_uint8_t   vi_avg_delay;
             a_uint8_t   vo_avg_delay;
             a_uint16_t  st_cnt;
             a_uint8_t   ch_util;
        } gid10;
        struct {
            a_uint8_t    tid;
            a_uint8_t    brange;
            a_uint8_t    mac[6];
            a_uint32_t   tx_cnt;
            a_uint32_t   discnt;
            a_uint32_t   multirtycnt;
            a_uint32_t   cfpoll;
            a_uint32_t   qdelay;
            a_uint32_t   txdelay;
            a_uint32_t   bin[6];
        } tsm_data;
        struct {
            a_uint8_t    phytype;
            a_uint8_t    arcpi;
            a_uint8_t    lrsni;
            a_uint8_t    lrcpi;
            a_uint8_t    antid;
            a_uint8_t    ta[6];
            a_uint8_t    bssid[6];
            a_uint16_t   frmcnt;
        } frmcnt_data[12];
        struct {
            a_uint8_t    tx_pow;
            a_uint8_t    lmargin;
            a_uint8_t    rxant;
            a_uint8_t    txant;
            a_uint8_t    rcpi;
            a_uint8_t    rsni;
        } lm_data;
        struct {
            a_uint8_t    id;
            a_uint8_t    len;
            a_uint8_t    lat_res;
            a_uint8_t    alt_type;
            a_uint8_t    long_res;
            a_uint8_t    alt_res;
            a_uint8_t    azi_res;
            a_uint8_t    alt_frac;
            a_uint8_t    datum;
            a_uint8_t    azi_type;
            a_uint16_t   lat_integ;
            a_uint16_t   long_integ;
            a_uint16_t   azimuth;
            a_uint32_t   lat_frac;
            a_uint32_t   long_frac;
            a_uint32_t   alt_integ;
        } ni_rrm_lciinfo;
        struct {
            a_uint8_t    id;
            a_uint8_t    len;
            a_uint8_t    lat_res;
            a_uint8_t    alt_type;
            a_uint8_t    long_res;
            a_uint8_t    alt_res;
            a_uint8_t    azi_res;
            a_uint8_t    alt_frac;
            a_uint8_t    datum;
            a_uint8_t    azi_type;
            a_uint16_t   lat_integ;
            a_uint16_t   long_integ;
            a_uint16_t   azimuth;
            a_uint32_t   lat_frac;
            a_uint32_t   long_frac;
            a_uint32_t   alt_integ;
        } ni_vap_lciinfo;
    } ni_rrm_stats;
} __attribute__((__packed__)) atd_rrmstats_t;

typedef struct atd_acs_dbg {
    a_uint8_t   nchans;
    a_uint8_t   entry_id;
    a_uint16_t  chan_freq;
    a_uint8_t   ieee_chan;
    a_uint8_t   chan_nbss;
    a_int32_t   chan_maxrssi;
    a_int32_t   chan_minrssi;
    a_int16_t   noisefloor;
    a_int16_t   channel_loading;
    a_uint32_t  chan_load;
    a_uint8_t   sec_chan;
} __attribute__((__packed__)) atd_acs_dbg_t;

typedef struct atd_athdbg_req {
    a_uint8_t cmd;
    a_uint8_t dstmac[ACFG_MACADDR_LEN];
    union {
        int param[4];
        struct  {
            a_uint16_t   num_rpt;
            a_uint8_t    regclass;
            a_uint8_t    channum;
            a_uint16_t   random_ivl;
            a_uint16_t   duration;
            a_uint8_t    reqmode;
            a_uint8_t    reqtype;
            a_uint8_t    bssid[6];
            a_uint8_t    mode;
            a_uint8_t    req_ssid;
            a_uint8_t    rep_cond;
            a_uint8_t    rep_thresh;
            a_uint8_t    rep_detail;
            a_uint8_t    req_ie;
            a_uint8_t    num_chanrep;
            struct {
                a_uint8_t regclass;
                a_uint8_t numchans;
                a_uint8_t channum[5];
            } apchanrep[2];
        } bcnrpt;
        struct {
            a_uint16_t   num_rpt;
            a_uint16_t   rand_ivl;
            a_uint16_t   meas_dur;
            a_uint8_t    reqmode;
            a_uint8_t    reqtype;
            a_uint8_t    tid;
            a_uint8_t    macaddr[6];
            a_uint8_t    bin0_range;
            a_uint8_t    trig_cond;
            a_uint8_t    avg_err_thresh;
            a_uint8_t    cons_err_thresh;
            a_uint8_t    delay_thresh;
            a_uint8_t    meas_count;
            a_uint8_t    trig_timeout;
        } tsmrpt;
        struct {
             a_uint8_t   dialogtoken;
             a_uint8_t   ssid[32];
             a_uint8_t   ssid_len;
        } neigrpt;
        struct {
             a_uint8_t   dialogtoken;
             a_uint8_t   candidate_list;
             a_uint8_t   disassoc;
             a_uint16_t  disassoc_timer;
             a_uint8_t   validity_itvl;
        } bstmreq;
        struct {
            a_uint8_t    traffic_type;
            a_uint8_t    direction;
            a_uint8_t    dot1Dtag;
            a_uint8_t    tid;
            a_uint8_t    acc_policy_edca;
            a_uint8_t    acc_policy_hcca;
            a_uint8_t    aggregation;
            a_uint8_t    psb;
            a_uint8_t    ack_policy;
            a_uint16_t   norminal_msdu_size;
            a_uint16_t   max_msdu_size;
            a_uint32_t   min_srv_interval;
            a_uint32_t   max_srv_interval;
            a_uint32_t   inactivity_interval;
            a_uint32_t   suspension_interval;
            a_uint32_t   srv_start_time;
            a_uint32_t   min_data_rate;
            a_uint32_t   mean_data_rate;
            a_uint32_t   peak_data_rate;
            a_uint32_t   max_burst_size;
            a_uint32_t   delay_bound;
            a_uint32_t   min_phy_rate;
            a_uint16_t   surplus_bw;
            a_uint16_t   medium_time;
        } tsinfo;
        struct {
            a_uint8_t    dstmac[6];
            a_uint16_t   num_rpts;
            a_uint8_t    regclass;
            a_uint8_t    chnum;
            a_uint16_t   r_invl;
            a_uint16_t   m_dur;
            a_uint8_t    cond;
            a_uint8_t    c_val;
        } chloadrpt;
        struct {
            a_uint8_t    dstmac[6];
            a_uint16_t   num_rpts;
            a_uint16_t   m_dur;
            a_uint16_t   r_invl;
            a_uint8_t    gid;
        } stastats;
        struct {
            a_uint16_t   num_rpts;
            a_uint8_t    dstmac[6];
            a_uint8_t    regclass;
            a_uint8_t    chnum;
            a_uint16_t   r_invl;
            a_uint16_t   m_dur;
            a_uint8_t    cond;
            a_uint8_t    c_val;
        } nhist;
        struct {
            a_uint8_t    dstmac[6];
            a_uint8_t    peermac[6];
            a_uint16_t   num_rpts;
            a_uint8_t    regclass;
            a_uint8_t    chnum;
            a_uint16_t   r_invl;
            a_uint16_t   m_dur;
            a_uint8_t    ftype;
        } frm_req;
        struct {
            a_uint8_t    dstmac[6];
            a_uint16_t   num_rpts;
            a_uint8_t    location;
            a_uint8_t    lat_res;
            a_uint8_t    long_res;
            a_uint8_t    alt_res;
            a_uint8_t    azi_res;
            a_uint8_t    azi_type;
        } lci_req;
        struct {
            a_uint32_t   index;
            a_uint32_t   data_size;
            void         *data_addr;
        } rrmstats_req;
        struct {
            a_uint32_t   index;
            a_uint32_t   data_size;
            void         *data_addr;
        } acs_rep;
        struct {
            a_uint32_t   data_size;
            a_uint32_t   cmdid;
            void         *data_addr;
        } tr069;
        struct {
            a_uint32_t inactivity_timeout_normal;
            a_uint32_t inactivity_timeout_overload;
            a_uint32_t inactivity_check_period;
            a_uint32_t utilization_sample_period;
            a_uint32_t utilization_average_num_samples;
            a_uint32_t inactive_rssi_crossing_threshold;
            a_uint32_t low_rssi_crossing_threshold;
        } bst;
        struct {
            a_uint8_t  raw_log_enable;
        } bst_dbg;
        struct {
            a_uint8_t sender_addr[ACFG_MACADDR_LEN];
            a_uint16_t num_measurements;
        } bst_rssi_req;
        a_uint8_t bsteering_probe_resp_wh;
        a_uint8_t bsteering_enable;
        a_uint8_t bsteering_overload;
        a_uint8_t bsteering_rssi_num_samples;
        struct {
            a_uint8_t     mu_req_id;
            a_uint8_t     mu_channel;
            a_uint32_t    mu_type;
            a_uint32_t    mu_duration;
            a_uint32_t    lteu_tx_power;
        } mu_scan;
        struct {
            a_uint8_t     lteu_gpio_start;
            a_uint8_t     lteu_num_bins;        
            a_uint8_t     use_actual_nf;        
            a_uint32_t    lteu_weight[ACFG_LTEU_MAX_BINS];
            a_uint32_t    lteu_thresh[ACFG_LTEU_MAX_BINS];
            a_uint32_t    lteu_gamma[ACFG_LTEU_MAX_BINS];
            a_uint32_t    lteu_scan_timeout;
            a_uint32_t    alpha_num_bssid;
        } lteu_cfg_t;
        struct {
            a_uint8_t     scan_req_id;
            a_uint8_t     scan_num_chan;      
            a_uint8_t     scan_channel_list[ACFG_MAX_SCAN_CHANS];
            a_int32_t     scan_type;            
            a_uint32_t    scan_duration;       
            a_uint32_t    scan_repeat_probe_time;
            a_uint32_t    scan_rest_time;       
            a_uint32_t    scan_idle_time;       
            a_uint32_t    scan_probe_delay;     
        } ap_scan_t;
    } data;
} __attribute__((__packed__)) atd_athdbg_req_t;


typedef struct atd_set_chn_width {
    a_uint32_t    setchnwidth[3];
} __attribute__((__packed__)) atd_set_chn_width_t;

typedef struct atd_atf_ssid_val {
    a_uint16_t    id_type;
    a_uint8_t     ssid[ACFG_MAX_SSID_LEN + 1];
    a_uint32_t    value;
} __attribute__((__packed__)) atd_atf_ssid_val_t;

typedef struct atd_atf_sta_val {
    a_uint16_t    id_type;
    a_uint8_t     sta_mac[ACFG_MACADDR_LEN];
    a_uint32_t    value;
}__attribute__((__packed__)) atd_atf_sta_val_t;

typedef struct atd_set_country {
    a_uint8_t   setcountry[3];
} __attribute__((__packed__)) atd_set_country_t;

/* SPECTRAL SCAN defines begin */
typedef struct {
    u_int16_t   ss_fft_period;               /* Skip interval for FFT reports */
    u_int16_t   ss_period;                   /* Spectral scan period */
    u_int16_t   ss_count;                    /* # of reports to return from
                                                ss_active */
    u_int16_t   ss_short_report;             /* Set to report ony 1 set of FFT
                                                results */
    u_int8_t    radar_bin_thresh_sel;
    u_int16_t   ss_spectral_pri;             /* Priority, and are we doing a
                                                noise power cal ? */
    u_int16_t   ss_fft_size;                 /* Defines the number of FFT data
                                                points to compute, defined
                                                as a log index:
                                                num_fft_pts = 2^ss_fft_size */
    u_int16_t   ss_gc_ena;                   /* Set, to enable targeted gain
                                                change before starting the
                                                spectral scan FFT */
    u_int16_t   ss_restart_ena;              /* Set, to enable abort of receive
                                                frames when in high priority
                                                and a spectral scan is queued */
    u_int16_t   ss_noise_floor_ref;          /* Noise floor reference number
                                                signed) for the calculation
                                                of bin power (dBm).
                                                Though stored as an unsigned
                                                value, this should be treated
                                                as a signed 8-bit int. */
    u_int16_t   ss_init_delay;               /* Disallow spectral scan triggers
                                                after tx/rx packets by setting
                                                this delay value to roughly
                                                SIFS time period or greater.
                                                Delay timer counts in units of
                                                0.25us */
    u_int16_t   ss_nb_tone_thr;              /* Number of strong bins
                                                (inclusive) per sub-channel,
                                                below which a signal is declared
                                                a narrowband tone */
    u_int16_t   ss_str_bin_thr;              /* Bin/max_bin ratio threshold over
                                                which a bin is declared strong
                                                (for spectral scan bandwidth
                                                analysis). */
    u_int16_t   ss_wb_rpt_mode;              /* Set this bit to report spectral
                                                scans as EXT_BLOCKER
                                                (phy_error=36), if none of the
                                                sub-channels are deemed
                                                narrowband. */
    u_int16_t   ss_rssi_rpt_mode;            /* Set this bit to report spectral
                                                scans as EXT_BLOCKER
                                                (phy_error=36), if the ADC RSSI
                                                is below the threshold
                                                ss_rssi_thr */
    u_int16_t   ss_rssi_thr;                 /* ADC RSSI must be greater than or
                                                equal to this threshold
                                                (signed Db) to ensure spectral
                                                scan reporting with normal phy
                                                error codes (please see
                                                ss_rssi_rpt_mode above).
                                                Though stored as an unsigned
                                                value, this should be treated
                                                as a signed 8-bit int. */
    u_int16_t   ss_pwr_format;               /* Format of frequency bin
                                                magnitude for spectral scan
                                                triggered FFTs:
0: linear magnitude
1: log magnitude
(20*log10(lin_mag),
1/2 dB step size) */
    u_int16_t   ss_rpt_mode;                 /* Format of per-FFT reports to
                                                software for spectral scan
                                                triggered FFTs.
0: No FFT report
(only pulse end summary)
1: 2-dword summary of metrics
for each completed FFT
2: 2-dword summary +
1x-oversampled bins(in-band)
per FFT
3: 2-dword summary +
2x-oversampled bins (all)
per FFT */
    u_int16_t   ss_bin_scale;                /* Number of LSBs to shift out to
                                                scale the FFT bins for spectral
                                                scan triggered FFTs. */
    u_int16_t   ss_dBm_adj;                  /* Set (with ss_pwr_format=1), to
                                                report bin magnitudes converted
                                                to dBm power using the
                                                noisefloor calibration
                                                results. */
    u_int16_t   ss_chn_mask;                 /* Per chain enable mask to select
                                                input ADC for search FFT. */
    int8_t      ss_nf_cal[AH_MAX_CHAINS*2];  /* nf calibrated values for
                                                ctl+ext */
    int8_t      ss_nf_pwr[AH_MAX_CHAINS*2];  /* nf pwr values for ctl+ext */
    int32_t     ss_nf_temp_data;             /* temperature data taken during
                                                nf scan */
} __attribute__((__packed__)) atd_HAL_SPECTRAL_PARAM_t;

typedef struct atd_spectral_chan_stats {
    int cycle_count;
    int channel_load;
    int per;
    int noisefloor;
    u_int16_t comp_usablity;
    int8_t maxregpower;
} __attribute__((__packed__)) atd_SPECTRAL_CHAN_STATS_T;

#endif /*  __ATD_CMD_H */

