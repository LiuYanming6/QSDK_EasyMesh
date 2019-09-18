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
 * @Abstract: WMI Data Structure for interfacing between Target and Host
 *
 */
#ifndef WMI_H_
#define WMI_H_
#define MAX_NUM_RADIO 2
#define MAX_VAP_SUPPORTED 64
#define WMI_DEF_MSG_LEN 2048





enum wmi_id {
    WMI_CMD_ECHO,
    WMI_CMD_RADIO_QUERY,
    WMI_CMD_VAP_CREATE,
    WMI_CMD_VAP_DELETE,
    WMI_CMD_VAP_OPEN,
    WMI_CMD_VAP_CLOSE,
    WMI_CMD_SSID_SET,
    WMI_CMD_SSID_GET,
    WMI_CMD_TESTMODE_SET,
    WMI_CMD_TESTMODE_GET,
    WMI_CMD_RSSI_GET,
    WMI_CMD_CUSTDATA_GET,
    WMI_CMD_VAP_PARAM_SET,
    WMI_CMD_VAP_PARAM_GET,
    WMI_CMD_WIFI_PARAM_SET,
    WMI_CMD_WIFI_PARAM_GET,
    WMI_CMD_OPMODE_SET,
    WMI_CMD_OPMODE_GET,
    WMI_CMD_CHMODE_SET,
    WMI_CMD_CHMODE_GET,
    WMI_CMD_WIRELESS_NAME_GET,
    WMI_CMD_AP_SET,
    WMI_CMD_AP_GET,
    WMI_CMD_RATE_GET,
    WMI_CMD_SCAN_SET,
    WMI_CMD_SCAN_RESULTS_GET,
    WMI_CMD_ATH_STATS_GET,
    WMI_CMD_ATH_STATS_CLR,
    WMI_CMD_SCAN_SPACE_GET,
    WMI_CMD_RANGE_GET  ,
    WMI_CMD_FREQ_SET,
    WMI_CMD_FREQ_GET,
    WMI_CMD_PHYMODE_SET,
    WMI_CMD_PHYMODE_GET,
    WMI_CMD_STA_INFO_GET, 
    WMI_CMD_RTS_SET,
    WMI_CMD_RTS_GET,
    WMI_CMD_FRAG_SET,
    WMI_CMD_FRAG_GET,
    WMI_CMD_TXPOW_SET,
    WMI_CMD_TXPOW_GET,
    WMI_CMD_ENCODE_SET,
    WMI_CMD_ENCODE_GET,
    WMI_CMD_RATE_SET,
    WMI_CMD_STATS_GET,
    WMI_CMD_POWMGMT_SET,
    WMI_CMD_POWMGMT_GET,
    WMI_CMD_REG_SET,
    WMI_CMD_REG_GET,
    WMI_CMD_TX99TOOL,
    WMI_CMD_HWADDR_SET,
    WMI_CMD_DOTH_CHSW,
    WMI_CMD_ADDMAC,
    WMI_CMD_DELMAC,
    WMI_CMD_KICKMAC,

    /* security WMI commands */
    WMI_CMD_WSUPP_INIT,
    WMI_CMD_WSUPP_FINI,
    WMI_CMD_WSUPP_ADD_IF,
    WMI_CMD_WSUPP_REMOVE_IF,
    WMI_CMD_WSUPP_ADD_NW,
    WMI_CMD_WSUPP_REMOVE_NW,
    WMI_CMD_WSUPP_SET_NETWORK,
    WMI_CMD_WSUPP_GET_NETWORK,
    WMI_CMD_WSUPP_LIST_NETWORK,
    WMI_CMD_WSUPP_SET_WPS,
    WMI_CMD_WSUPP_SET,

    
    /* WMM PARAMS WMI commands */
    WMI_CMD_WMMPARAMS_SET,
    WMI_CMD_WMMPARAMS_GET,

    /* WMI NAWDS config */
    WMI_CMD_NAWDS_CONFIG,
	
    WMI_CMD_MLME_SET, 
    WMI_CMD_OPTIE_SET, 
    WMI_CMD_WPA_IE_GET, 
    WMI_CMD_ACPARAMS_SET,
    WMI_CMD_FILTERFRAME_SET, 
    WMI_CMD_APPIEBUF_SET, 
    WMI_CMD_SET_KEY, 
    WMI_CMD_DEL_KEY, 
    WMI_CMD_GET_KEY, 
    WMI_CMD_STA_STATS_GET, 
    WMI_CMD_CHAN_INFO_GET, 
    WMI_CMD_CHAN_LIST_GET, 
    WMI_CMD_MAC_ADDR_GET, 
    WMI_CMD_P2P_PARAM_GET,
    WMI_CMD_P2P_PARAM_SET,

    /* APPBR Commands */
    WMI_CMD_APPBR,        /**< Pass through APPBR command */

    /* Power Management Commands */
    WMI_CMD_PWRDOWN,        /**< Put device in low power mode */
    
    WMI_CMD_DBGREQ,
    WMI_CMD_SEND_MGMT,
	
    WMI_CMD_ACL_ADDMAC,
    WMI_CMD_ACL_DELMAC,
    WMI_CMD_WIFI_GET_PROFILE,
    WMI_CMD_PHYERR,
    WMI_CMD_VAP_VENDOR_PARAM_SET, 
    WMI_CMD_VAP_VENDOR_PARAM_GET, 
    WMI_CMD_CHNWIDTHSWITCH_SET,
    WMI_CMD_SET_ATF_SSID,
    WMI_CMD_SET_ATF_STA,
    WMI_CMD_COUNTRY_SET,
    WMI_CMD_MAX     = 0x7FFF,   /**< Command End */

    WMI_EVENT_START = 0x8000,   /**< Event Start */

    WMI_EVENT_RESTORE_WIFI,     /**< Restore WIFI */
    WMI_EVENT_DELETE_WIFI,      /**< Delete WIFI */
    WMI_EVENT_CREATE_VAP,       /**< Create VAP */
    WMI_EVENT_DELETE_VAP,       /**< Delete VAP */
    WMI_EVENT_SCAN_DONE,        /**< Scan Done */
    WMI_EVENT_ASSOC_AP,         /**< Assoc AP */
    WMI_EVENT_ASSOC_STA,        /**< STA join */
    WMI_EVENT_CHAN_START,       /**< Radio On Channel */
    WMI_EVENT_CHAN_END,         /**< Radio Off Channel */
    WMI_EVENT_RX_MGMT,          /**< Received Management Frame */
    WMI_EVENT_SENT_ACTION,      /**< Sent Action Frame */
    WMI_EVENT_LEAVE_AP,         /**< Leave AP */
    WMI_EVENT_LEAVE_STA,        /**< Leave STA */
    WMI_EVENT_IF_RUNNING_STA,   /**< IF_RUNNING STA */
    WMI_EVENT_IF_NOT_RUNNING_STA,   /**< IF_NOT_RUNNING STA */
    WMI_EVENT_GEN_IE,           /**< Pass IE */
    //WMI_EVENT_ASSOC_REQ_IE,     /**< Pass Assoc Req IE */
    WMI_EVENT_IW_CUSTOM,        /**< IW_CUSTOM */
    WMI_EVENT_IW_ASSOCREQIE,    /**< IW_ASSOCREQIE for Probe request frames */
    WMI_EVENT_MIC_FAIL,
    WMI_EVENT_AUTH_COMP_AP,
    WMI_EVENT_ASSOC_COMP_AP,
    WMI_EVENT_DEAUTH_COMP_AP,
    WMI_EVENT_AUTH_IND_AP,
    WMI_EVENT_AUTH_COMP_STA,
    WMI_EVENT_ASSOC_COMP_STA,
    WMI_EVENT_DEAUTH_COMP_STA,
    WMI_EVENT_DISASSOC_COMP_STA,
    WMI_EVENT_AUTH_IND_STA,
    WMI_EVENT_DEAUTH_IND_STA,
    WMI_EVENT_ASSOC_IND_STA,
    WMI_EVENT_DISASSOC_IND_STA,	
    WMI_EVENT_DEAUTH_IND_AP,
    WMI_EVENT_DISASSOC_IND_AP,
    WMI_EVENT_WAPI,
	
    /* security events */
    WMI_EVENT_WSUPP_RAW_MESSAGE,            /**< WSUPP event: associated */
    WMI_EVENT_WSUPP_AP_STA_CONNECTED,       /**< WSUPP event: associated */
    WMI_EVENT_WSUPP_WPA_EVENT_CONNECTED,    /**< WSUPP event: WPA connected */
    WMI_EVENT_WSUPP_WPA_EVENT_DISCONNECTED, /**< WSUPP event: WPA disconnected */
    WMI_EVENT_WSUPP_WPA_EVENT_TERMINATING,  /**< WSUPP event: WPA terminating */
    WMI_EVENT_WSUPP_WPA_EVENT_SCAN_RESULTS, /**< WSUPP event: WPA get scan result */
    WMI_EVENT_WSUPP_WPS_EVENT_ENROLLEE_SEEN,/**< WSUPP event: enrollee seen */

    /* APPBR events */
    WMI_EVENT_APPBR,        /**< Pass through APPBR event */

    /* BYPASS events */
    WMI_EVENT_BYP_VAP_LANEN,       /**< Enable LAN Access for VAP in Bypass */
    WMI_EVENT_BYP_VAP_LANDIS,      /**< Disable LAN Access for VAP in Bypass */
    
    WMI_EVENT_PROXYARP_SEND,       /**< Sending Proxy ARP packet to host */ 

    /* Spectral samples */
    WMI_EVENT_SPECTRAL_MSG_SEND,

    /* Band steering nl event */
    WMI_EVENT_BSTEER_SEND,
    
    WMI_EVENT_MAX   = 0xFFFF    /**< Event End */
}; 
typedef a_uint16_t wmi_id_t;
#define WMI_MAX_EVENT_BUFFER_SIZE 1200
#define WMIRSP_FLAG_MORE 0x1
typedef struct {
    wmi_id_t     id;
    a_uint8_t    seqno;
    a_uint8_t    flag;
    a_uint8_t    fragseq;
    a_uint8_t    pad[3];
} __attribute__((__packed__)) wmi_hdr_t;

typedef struct wmi_rsp_hdr {
    a_status_t status;
} wmi_rsp_hdr_t;


#endif /*  WMI_H_ */

