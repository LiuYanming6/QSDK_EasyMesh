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

#define __ACFG_PHYMODE_STRINGS__

#include <linux/skbuff.h>
#include <linux/netdevice.h>
#include <linux/rtnetlink.h>
#include <linux/wireless.h>
#include <net/iw_handler.h>
#include <linux/vmalloc.h>
#include <linux/if_arp.h>       /* XXX for ARPHRD_ETHER */

#include <a_base_types.h>
#include <hif_api.h>
#include <htc_api.h>
#include <wmi_api.h>
#include <acfg_api_types.h>
#include <acfg_drv_if.h>
#include <atd_hdr.h>
#include <atd_cmd.h>
#include <atd_event.h>
#include <atd_internal.h>
#include <acfg_event_types.h>
#include <atd_wsupp_msg.h>
#include <atd_appbr.h>

#include <wcfg.h>

#ifndef BUILD_PARTNER_PLATFORM
#include <../arch/mips/include/asm/mach-ar7240/ar7240.h>
#endif
#include"ieee80211_external.h"

static int
atd_tgt_acfg2ieee(a_uint16_t param)
{
    a_uint16_t value;
    
    switch (param)
    {
        case ACFG_PARAM_VAP_SHORT_GI:
            value = IEEE80211_PARAM_SHORT_GI;
            break;
        case ACFG_PARAM_VAP_AMPDU:
            value = IEEE80211_PARAM_AMPDU;
            break;
        case ACFG_PARAM_AUTHMODE:
            value = IEEE80211_PARAM_AUTHMODE;
            break;
        case ACFG_PARAM_BEACON_INTERVAL:
            value = IEEE80211_PARAM_BEACON_INTERVAL;
            break;
        case ACFG_PARAM_PUREG:
            value = IEEE80211_PARAM_PUREG;
            break;
        case ACFG_PARAM_WDS:
            value = IEEE80211_PARAM_WDS;
            break;
        case ACFG_PARAM_11N_RATE:
            value = IEEE80211_PARAM_11N_RATE;
            break;
        case ACFG_PARAM_11N_RETRIES:
            value = IEEE80211_PARAM_11N_RETRIES;
            break;
        case ACFG_PARAM_DBG_LVL:
            value = IEEE80211_PARAM_DBG_LVL;
            break;
        case ACFG_PARAM_STA_FORWARD:
            value = IEEE80211_PARAM_STA_FORWARD;
            break;
        case ACFG_PARAM_PUREN:
            value = IEEE80211_PARAM_PUREN;
            break;
        case ACFG_PARAM_NO_EDGE_CH:
            value = IEEE80211_PARAM_NO_EDGE_CH;
            break;
        case ACFG_PARAM_VAP_IND:
            value = IEEE80211_PARAM_VAP_IND;
            break;
        case ACFG_PARAM_WPS:
            value = IEEE80211_PARAM_WPS;
            break;
        case ACFG_PARAM_EXTAP:
            value = IEEE80211_PARAM_EXTAP;
            break;
        case ACFG_PARAM_TDLS_ENABLE:
            value = IEEE80211_PARAM_TDLS_ENABLE;
            break;
        case ACFG_PARAM_SET_TDLS_RMAC:
            value = IEEE80211_PARAM_SET_TDLS_RMAC;
            break;
        case ACFG_PARAM_CLR_TDLS_RMAC:
            value = IEEE80211_PARAM_CLR_TDLS_RMAC;
            break;
        case ACFG_PARAM_TDLS_MACADDR1:
            value = IEEE80211_PARAM_TDLS_MACADDR1;
            break;
        case ACFG_PARAM_TDLS_MACADDR2:
            value = IEEE80211_PARAM_TDLS_MACADDR2;
            break;
        case ACFG_PARAM_SW_WOW:
            value = IEEE80211_PARAM_SW_WOW;
            break;
        case ACFG_PARAM_SCANVALID:
            value = IEEE80211_PARAM_SCANVALID;
            break;
        case ACFG_PARAM_PERIODIC_SCAN:
            value = IEEE80211_PARAM_PERIODIC_SCAN;
            break;
        case ACFG_PARAM_SCANBAND:
            value = IEEE80211_PARAM_SCAN_BAND;
            break;
        case ACFG_PARAM_WMMPARAMS_CWMIN:
            value = IEEE80211_WMMPARAMS_CWMIN;
            break;			
        case ACFG_PARAM_WMMPARAMS_CWMAX:
            value = IEEE80211_WMMPARAMS_CWMAX;
            break;	       
		case ACFG_PARAM_WMMPARAMS_AIFS:
            value = IEEE80211_WMMPARAMS_AIFS;
            break;					
        case ACFG_PARAM_WMMPARAMS_TXOPLIMIT:
            value = IEEE80211_WMMPARAMS_TXOPLIMIT;
            break;	
        case ACFG_PARAM_WMMPARAMS_ACM:
            value = IEEE80211_WMMPARAMS_ACM;
            break;	
        case ACFG_PARAM_WMMPARAMS_NOACKPOLICY:
            value = IEEE80211_WMMPARAMS_NOACKPOLICY;
            break;								
        case ACFG_PARAM_PROXYARP_CAP:
            value = IEEE80211_PARAM_PROXYARP_CAP;
            break;					
		case ACFG_PARAM_HIDE_SSID:
            value = IEEE80211_PARAM_HIDESSID;
            break;
		case ACFG_PARAM_DOTH:
            value = IEEE80211_PARAM_DOTH;
            break;
        case ACFG_PARAM_COEXT_DISABLE:
            value = IEEE80211_PARAM_COEXT_DISABLE;
            break;
        case ACFG_PARAM_APBRIDGE:
            value = IEEE80211_PARAM_APBRIDGE;
            break;
        case ACFG_PARAM_AMPDU:
            value = IEEE80211_PARAM_AMPDU;
            break;
        case ACFG_PARAM_DGAF_DISABLE:
            value = IEEE80211_PARAM_DGAF_DISABLE;
            break;
        case ACFG_PARAM_L2TIF_CAP:
            value = IEEE80211_PARAM_L2TIF_CAP;
            break;
        case ACFG_PARAM_QBSS_LOAD:
            value = IEEE80211_PARAM_QBSS_LOAD;
            break;
        case ACFG_PARAM_ROAMING:
            value = IEEE80211_PARAM_ROAMING;
            break;
        case ACFG_PARAM_AUTO_ASSOC:
            value = IEEE80211_PARAM_AUTO_ASSOC;
            break;
        case ACFG_PARAM_UAPSD:
            value = IEEE80211_PARAM_UAPSDINFO;
            break;
#if UMAC_SUPPORT_WNM
        case ACFG_PARAM_WNM_ENABLE:
            value = IEEE80211_PARAM_WNM_CAP;
            break;
        case ACFG_PARAM_WNM_BSSMAX:
            value = IEEE80211_PARAM_WNM_BSS_CAP;
            break;                      
        case ACFG_PARAM_WNM_TFS:
            value = IEEE80211_PARAM_WNM_TFS_CAP;
            break;                      
        case ACFG_PARAM_WNM_TIM:
            value = IEEE80211_PARAM_WNM_TIM_CAP;
            break;                      
        case ACFG_PARAM_WNM_SLEEP:
            value = IEEE80211_PARAM_WNM_SLEEP_CAP;
            break;                      
#endif
        case ACFG_PARAM_CHANBW:
            value = IEEE80211_PARAM_CHANBW;
            break;
        case ACFG_PARAM_BURST:
            value = IEEE80211_PARAM_BURST;
            break;                      
        case ACFG_PARAM_AMSDU:
            value = IEEE80211_PARAM_AMSDU;
            break;                      
        case ACFG_PARAM_MAXSTA:
            value = IEEE80211_PARAM_MAXSTA;
            break;
        case ACFG_PARAM_SETADDBAOPER:         
            value = IEEE80211_PARAM_SETADDBAOPER;    
            break;                      
        case ACFG_PARAM_OPMODE_NOTIFY:        
            value = IEEE80211_PARAM_OPMODE_NOTIFY;
            break;                      
        case ACFG_PARAM_WEP_TKIP_HT:          
            value = IEEE80211_PARAM_WEP_TKIP_HT;
            break;                      
        case ACFG_PARAM_CWM_ENABLE:           
            value = IEEE80211_PARAM_CWM_ENABLE;
            break;                      
        case ACFG_PARAM_MAX_AMPDU:            
            value = IEEE80211_PARAM_MAX_AMPDU;
            break;                      
        case ACFG_PARAM_VHT_MAX_AMPDU:        
            value = IEEE80211_PARAM_VHT_MAX_AMPDU;   
            break;                      
        case ACFG_PARAM_ENABLE_RTSCTS:        
            value = IEEE80211_PARAM_ENABLE_RTSCTS;
            break;                      

        default:
            value = param;
            break;
    }
    return value;    
}

/**************WMI function handler ****************************/

#define ATD_TGT_CMD_PROTOTYPE(_func_name)  \
        static void _func_name  ( void *ctx,  a_uint16_t cmdid, \
                                   a_uint8_t *buffer,\
                                   a_int32_t len)

ATD_TGT_CMD_PROTOTYPE(handle_echo_command);
ATD_TGT_CMD_PROTOTYPE(atd_tgt_radio_queury);
ATD_TGT_CMD_PROTOTYPE(atd_tgt_vap_create);
ATD_TGT_CMD_PROTOTYPE(atd_tgt_vap_delete);
ATD_TGT_CMD_PROTOTYPE(atd_tgt_vap_open);
ATD_TGT_CMD_PROTOTYPE(atd_tgt_vap_close);
ATD_TGT_CMD_PROTOTYPE(atd_tgt_set_ssid);
ATD_TGT_CMD_PROTOTYPE(atd_tgt_get_ssid);
ATD_TGT_CMD_PROTOTYPE(atd_tgt_set_testmode);
ATD_TGT_CMD_PROTOTYPE(atd_tgt_get_testmode);
ATD_TGT_CMD_PROTOTYPE(atd_tgt_get_rssi);
ATD_TGT_CMD_PROTOTYPE(atd_tgt_get_custdata);
ATD_TGT_CMD_PROTOTYPE(atd_tgt_set_vap_param);
ATD_TGT_CMD_PROTOTYPE(atd_tgt_get_vap_param);
ATD_TGT_CMD_PROTOTYPE(atd_tgt_set_wifi_param);
ATD_TGT_CMD_PROTOTYPE(atd_tgt_get_wifi_param);
ATD_TGT_CMD_PROTOTYPE(atd_tgt_set_opmode);
ATD_TGT_CMD_PROTOTYPE(atd_tgt_get_opmode);
ATD_TGT_CMD_PROTOTYPE(atd_tgt_set_chmode);
ATD_TGT_CMD_PROTOTYPE(atd_tgt_get_chmode);
ATD_TGT_CMD_PROTOTYPE(atd_tgt_get_wireless_name);
ATD_TGT_CMD_PROTOTYPE(atd_tgt_set_ap);
ATD_TGT_CMD_PROTOTYPE(atd_tgt_get_ap);
ATD_TGT_CMD_PROTOTYPE(atd_tgt_get_rate);
ATD_TGT_CMD_PROTOTYPE(atd_tgt_set_scan);
ATD_TGT_CMD_PROTOTYPE(atd_tgt_get_ath_stats);
ATD_TGT_CMD_PROTOTYPE(atd_tgt_clr_ath_stats);
ATD_TGT_CMD_PROTOTYPE(atd_tgt_get_scanresults);
ATD_TGT_CMD_PROTOTYPE(atd_tgt_get_scan_space);
ATD_TGT_CMD_PROTOTYPE(atd_tgt_set_freq);
ATD_TGT_CMD_PROTOTYPE(atd_tgt_get_freq);
ATD_TGT_CMD_PROTOTYPE(atd_tgt_set_rts);
ATD_TGT_CMD_PROTOTYPE(atd_tgt_get_rts);
ATD_TGT_CMD_PROTOTYPE(atd_tgt_set_frag);
ATD_TGT_CMD_PROTOTYPE(atd_tgt_get_frag);
ATD_TGT_CMD_PROTOTYPE(atd_tgt_set_txpow);
ATD_TGT_CMD_PROTOTYPE(atd_tgt_get_txpow);
ATD_TGT_CMD_PROTOTYPE(atd_tgt_get_range);

ATD_TGT_CMD_PROTOTYPE(atd_tgt_set_phymode);
ATD_TGT_CMD_PROTOTYPE(atd_tgt_get_phymode);
ATD_TGT_CMD_PROTOTYPE(atd_tgt_get_stainfo);
ATD_TGT_CMD_PROTOTYPE(atd_tgt_set_encode);
ATD_TGT_CMD_PROTOTYPE(atd_tgt_get_encode);
ATD_TGT_CMD_PROTOTYPE(atd_tgt_set_rate);
ATD_TGT_CMD_PROTOTYPE(atd_tgt_get_stats);
ATD_TGT_CMD_PROTOTYPE(atd_tgt_set_powmgmt);
ATD_TGT_CMD_PROTOTYPE(atd_tgt_get_powmgmt);
ATD_TGT_CMD_PROTOTYPE(atd_tgt_set_reg);
ATD_TGT_CMD_PROTOTYPE(atd_tgt_get_reg);
ATD_TGT_CMD_PROTOTYPE(atd_tgt_tx99tool);
ATD_TGT_CMD_PROTOTYPE(atd_tgt_set_hwaddr);
ATD_TGT_CMD_PROTOTYPE(atd_tgt_wsupp_request);
ATD_TGT_CMD_PROTOTYPE(atd_tgt_set_wmmparams);
ATD_TGT_CMD_PROTOTYPE(atd_tgt_get_wmmparams);
ATD_TGT_CMD_PROTOTYPE(atd_tgt_nawds_config);
ATD_TGT_CMD_PROTOTYPE(atd_tgt_doth_chswitch);
ATD_TGT_CMD_PROTOTYPE(atd_tgt_addmac);
ATD_TGT_CMD_PROTOTYPE(atd_tgt_delmac);
ATD_TGT_CMD_PROTOTYPE(atd_tgt_kickmac);
ATD_TGT_CMD_PROTOTYPE(atd_tgt_set_mlme);
ATD_TGT_CMD_PROTOTYPE(atd_tgt_set_optie);
ATD_TGT_CMD_PROTOTYPE(atd_tgt_get_wpa_ie);
ATD_TGT_CMD_PROTOTYPE(atd_tgt_set_acparams);
ATD_TGT_CMD_PROTOTYPE(atd_tgt_set_filterframe);
ATD_TGT_CMD_PROTOTYPE(atd_tgt_set_appiebuf);
ATD_TGT_CMD_PROTOTYPE(atd_tgt_set_key);
ATD_TGT_CMD_PROTOTYPE(atd_tgt_del_key);
ATD_TGT_CMD_PROTOTYPE(atd_tgt_get_key);
ATD_TGT_CMD_PROTOTYPE(atd_tgt_get_sta_stats);
ATD_TGT_CMD_PROTOTYPE(atd_tgt_get_chan_info);
ATD_TGT_CMD_PROTOTYPE(atd_tgt_get_chan_list);
ATD_TGT_CMD_PROTOTYPE(atd_tgt_get_mac_address);
ATD_TGT_CMD_PROTOTYPE(atd_tgt_get_p2p_param);
ATD_TGT_CMD_PROTOTYPE(atd_tgt_set_p2p_param);
ATD_TGT_CMD_PROTOTYPE(atd_tgt_pwrdown);
ATD_TGT_CMD_PROTOTYPE(atd_tgt_dbgreq);
ATD_TGT_CMD_PROTOTYPE(atd_tgt_send_mgmt);
ATD_TGT_CMD_PROTOTYPE(atd_tgt_acl_addmac);
ATD_TGT_CMD_PROTOTYPE(atd_tgt_acl_delmac);
ATD_TGT_CMD_PROTOTYPE(atd_tgt_get_profile);
ATD_TGT_CMD_PROTOTYPE(atd_tgt_phyerr);
ATD_TGT_CMD_PROTOTYPE(atd_tgt_set_vap_vendor_param);
ATD_TGT_CMD_PROTOTYPE(atd_tgt_get_vap_vendor_param);
ATD_TGT_CMD_PROTOTYPE(atd_tgt_set_chnwidthswitch); 
ATD_TGT_CMD_PROTOTYPE(atd_tgt_set_atf_ssid); 
ATD_TGT_CMD_PROTOTYPE(atd_tgt_set_atf_sta); 
ATD_TGT_CMD_PROTOTYPE(atd_tgt_set_country); 

wmi_dispatch_entry_t  wmidispatchentry[] =
{
    { handle_echo_command,          WMI_CMD_ECHO              , 0 },
    { atd_tgt_radio_queury,         WMI_CMD_RADIO_QUERY       , 0 },
    { atd_tgt_vap_create,           WMI_CMD_VAP_CREATE        , 0 },
    { atd_tgt_vap_delete,           WMI_CMD_VAP_DELETE        , 0 },
    { atd_tgt_vap_open,             WMI_CMD_VAP_OPEN          , 0 },
    { atd_tgt_vap_close,            WMI_CMD_VAP_CLOSE         , 0 },
    { atd_tgt_set_ssid,             WMI_CMD_SSID_SET          , 0 },
    { atd_tgt_get_ssid,             WMI_CMD_SSID_GET          , 0 },
    { atd_tgt_set_testmode,         WMI_CMD_TESTMODE_SET      , 0 },
    { atd_tgt_get_testmode,         WMI_CMD_TESTMODE_GET      , 0 },
    { atd_tgt_get_rssi,             WMI_CMD_RSSI_GET          , 0 },
    { atd_tgt_get_custdata,         WMI_CMD_CUSTDATA_GET      , 0 },
    { atd_tgt_set_vap_param,        WMI_CMD_VAP_PARAM_SET     , 0 },
    { atd_tgt_get_vap_param,        WMI_CMD_VAP_PARAM_GET     , 0 },
    { atd_tgt_set_wifi_param,       WMI_CMD_WIFI_PARAM_SET    , 0 },
    { atd_tgt_get_wifi_param,       WMI_CMD_WIFI_PARAM_GET    , 0 },
    { atd_tgt_set_opmode,           WMI_CMD_OPMODE_SET        , 0 },
    { atd_tgt_get_opmode,           WMI_CMD_OPMODE_GET        , 0 },
    { atd_tgt_set_chmode,           WMI_CMD_CHMODE_SET        , 0 },
    { atd_tgt_get_chmode,           WMI_CMD_CHMODE_GET        , 0 },
    { atd_tgt_get_wireless_name,    WMI_CMD_WIRELESS_NAME_GET , 0 },
    { atd_tgt_set_ap,               WMI_CMD_AP_SET            , 0 },
    { atd_tgt_get_ap,               WMI_CMD_AP_GET            , 0 },
    { atd_tgt_get_rate,             WMI_CMD_RATE_GET          , 0 },
    { atd_tgt_set_scan,             WMI_CMD_SCAN_SET          , 0 },
    { atd_tgt_get_scanresults,      WMI_CMD_SCAN_RESULTS_GET  , 0 },
    { atd_tgt_get_ath_stats,        WMI_CMD_ATH_STATS_GET     , 0 },
    { atd_tgt_clr_ath_stats,        WMI_CMD_ATH_STATS_CLR     , 0 },
    { atd_tgt_get_scan_space,       WMI_CMD_SCAN_SPACE_GET    , 0 },
    { atd_tgt_get_range,            WMI_CMD_RANGE_GET         , 0 },  
    { atd_tgt_set_freq,             WMI_CMD_FREQ_SET          , 0 },
    { atd_tgt_get_freq,             WMI_CMD_FREQ_GET          , 0 },
    { atd_tgt_set_phymode,          WMI_CMD_PHYMODE_SET       , 0 },
    { atd_tgt_get_phymode,          WMI_CMD_PHYMODE_GET       , 0 },
    { atd_tgt_get_stainfo,          WMI_CMD_STA_INFO_GET      , 0 },
    { atd_tgt_set_rts,              WMI_CMD_RTS_SET           , 0 },
    { atd_tgt_get_rts,              WMI_CMD_RTS_GET           , 0 },
    { atd_tgt_set_frag,             WMI_CMD_FRAG_SET          , 0 },
    { atd_tgt_get_frag,             WMI_CMD_FRAG_GET          , 0 },
    { atd_tgt_set_txpow,            WMI_CMD_TXPOW_SET         , 0 },
    { atd_tgt_get_txpow,            WMI_CMD_TXPOW_GET         , 0 },
    { atd_tgt_set_encode,           WMI_CMD_ENCODE_SET        , 0 },
    { atd_tgt_get_encode,           WMI_CMD_ENCODE_GET        , 0 },
    { atd_tgt_set_rate,             WMI_CMD_RATE_SET          , 0 },
    { atd_tgt_get_stats,            WMI_CMD_STATS_GET         , 0 },
    { atd_tgt_set_powmgmt,          WMI_CMD_POWMGMT_SET       , 0 },
    { atd_tgt_get_powmgmt,          WMI_CMD_POWMGMT_GET       , 0 },
    { atd_tgt_set_reg,              WMI_CMD_REG_SET           , 0 },
    { atd_tgt_get_reg,              WMI_CMD_REG_GET           , 0 },
    { atd_tgt_tx99tool,             WMI_CMD_TX99TOOL          , 0 },
    { atd_tgt_set_hwaddr,           WMI_CMD_HWADDR_SET        , 0 },
    { atd_tgt_doth_chswitch,        WMI_CMD_DOTH_CHSW         , 0 },
    { atd_tgt_addmac,               WMI_CMD_ADDMAC            , 0 },
    { atd_tgt_delmac,               WMI_CMD_DELMAC            , 0 },
    { atd_tgt_kickmac,              WMI_CMD_KICKMAC           , 0 },
    { atd_tgt_wsupp_request,        WMI_CMD_WSUPP_INIT        , 0 },
    { atd_tgt_wsupp_request,        WMI_CMD_WSUPP_FINI        , 0 },
    { atd_tgt_wsupp_request,        WMI_CMD_WSUPP_ADD_IF      , 0 },
    { atd_tgt_wsupp_request,        WMI_CMD_WSUPP_REMOVE_IF   , 0 },
    { atd_tgt_wsupp_request,        WMI_CMD_WSUPP_ADD_NW      , 0 },
    { atd_tgt_wsupp_request,        WMI_CMD_WSUPP_REMOVE_NW   , 0 },
    { atd_tgt_wsupp_request,        WMI_CMD_WSUPP_SET_NETWORK , 0 },
    { atd_tgt_wsupp_request,        WMI_CMD_WSUPP_GET_NETWORK , 0 },
    { atd_tgt_wsupp_request,        WMI_CMD_WSUPP_LIST_NETWORK, 0 },
    { atd_tgt_wsupp_request,        WMI_CMD_WSUPP_SET_WPS     , 0 },
    { atd_tgt_wsupp_request,        WMI_CMD_WSUPP_SET         , 0 },
	{ atd_tgt_set_wmmparams,        WMI_CMD_WMMPARAMS_SET     , 0 },
	{ atd_tgt_get_wmmparams,        WMI_CMD_WMMPARAMS_GET     , 0 },
    { atd_tgt_nawds_config,         WMI_CMD_NAWDS_CONFIG      , 0 },
    { atd_tgt_set_mlme,             WMI_CMD_MLME_SET          , 0 },
    { atd_tgt_set_optie,            WMI_CMD_OPTIE_SET         , 0 },
    { atd_tgt_get_wpa_ie,           WMI_CMD_WPA_IE_GET        , 0 },
    { atd_tgt_set_acparams,         WMI_CMD_ACPARAMS_SET      , 0 },
    { atd_tgt_set_filterframe,      WMI_CMD_FILTERFRAME_SET   , 0 },
    { atd_tgt_set_appiebuf,         WMI_CMD_APPIEBUF_SET      , 0 },
    { atd_tgt_set_key,              WMI_CMD_SET_KEY           , 0 },
    { atd_tgt_del_key,              WMI_CMD_DEL_KEY           , 0 },
    { atd_tgt_get_key,              WMI_CMD_GET_KEY           , 0 },
    { atd_tgt_get_sta_stats,        WMI_CMD_STA_STATS_GET     , 0 },
    { atd_tgt_get_chan_info,        WMI_CMD_CHAN_INFO_GET     , 0 },
    { atd_tgt_get_chan_list,        WMI_CMD_CHAN_LIST_GET     , 0 },
    { atd_tgt_get_mac_address,      WMI_CMD_MAC_ADDR_GET      , 0 },
    { atd_tgt_get_p2p_param,        WMI_CMD_P2P_PARAM_GET     , 0 },
    { atd_tgt_set_p2p_param,        WMI_CMD_P2P_PARAM_SET     , 0 },
    { atd_tgt_appbr_input,          WMI_CMD_APPBR             , 0 },
    { atd_tgt_pwrdown,              WMI_CMD_PWRDOWN           , 0 },
    { atd_tgt_dbgreq,               WMI_CMD_DBGREQ            , 0 },
    { atd_tgt_send_mgmt,            WMI_CMD_SEND_MGMT         , 0 },
    { atd_tgt_acl_addmac,           WMI_CMD_ACL_ADDMAC           , 0 },
    { atd_tgt_acl_delmac,           WMI_CMD_ACL_DELMAC           , 0 },
    { atd_tgt_get_profile,          WMI_CMD_WIFI_GET_PROFILE           , 0 },
    { atd_tgt_phyerr,               WMI_CMD_PHYERR            , 0 },
    { atd_tgt_set_vap_vendor_param, WMI_CMD_VAP_VENDOR_PARAM_SET, 0},
    { atd_tgt_get_vap_vendor_param, WMI_CMD_VAP_VENDOR_PARAM_GET, 0},
    { atd_tgt_set_chnwidthswitch,   WMI_CMD_CHNWIDTHSWITCH_SET, 0}, 
    { atd_tgt_set_atf_ssid,         WMI_CMD_SET_ATF_SSID, 0}, 
    { atd_tgt_set_atf_sta,          WMI_CMD_SET_ATF_STA, 0}, 
    { atd_tgt_set_country,          WMI_CMD_COUNTRY_SET, 0}, 
};

extern atd_tgt_dev_t     *g_atd_dev;


/**************WMI function handler end ******************************/

void   
atd_tgt_wmi_init(atd_tgt_dev_t * atsc)
{

    wmi_dispatch_table_t * wmidispatchtable;


    atd_trace(ATD_DEBUG_FUNCTRACE,(" Start"));
    /* Init target-side WMI */
    wmidispatchtable =(wmi_dispatch_table_t *)
                      kzalloc(sizeof(wmi_dispatch_table_t),GFP_KERNEL);

    memset(wmidispatchtable, 0, sizeof(wmi_dispatch_table_t));

    wmidispatchtable->noentries = 
        wmi_dispatch_entry_count(wmidispatchentry);

    wmidispatchtable->disp_entry = &wmidispatchentry[0];
    wmidispatchtable->ctx = atsc;


    
    atsc->wmi_handle = wmi_init((void *)atsc->htc_handle);

    wmi_register_disp_table(atsc->wmi_handle, wmidispatchtable);
    atd_trace(ATD_DEBUG_FUNCTRACE,(" End"));

}

void
atd_tgt_wmi_set_stopflag(wmi_handle_t wmi_handle, a_uint8_t val)
{
    wmi_set_stopflag(wmi_handle, val);
}


/* target WMI command handler */
void 
handle_echo_command(void *ctx, a_uint16_t cmdid, 
                    a_uint8_t *buffer, a_int32_t len)
{
        atd_tgt_dev_t *atsc = (atd_tgt_dev_t *)ctx;
        a_status_t status = A_STATUS_OK;

        atd_trace(ATD_DEBUG_FUNCTRACE,("cmd %d    ", cmdid));
        wmi_cmd_rsp(atsc->wmi_handle, WMI_CMD_ECHO, status, buffer, len);
}


void 
atd_tgt_radio_queury(void *ctx, a_uint16_t cmdid,
                     a_uint8_t *buffer, a_int32_t len)
{
    a_status_t status = 0;
    atd_tgt_dev_t *atsc = (atd_tgt_dev_t *)ctx;
    atd_radio_query_resp_t resp;
    
    int i;
    struct net_device *netdev;
    
    atd_trace(ATD_DEBUG_FUNCTRACE, ("cmd %d   ",
                    cmdid));

    for(i=0;i< atsc->wifi_cnt ;i++){
        if(atsc->wifi_list[i] != NULL){
            netdev = atsc->wifi_list[i]->netdev;
            memcpy(&resp.mac_addr[i][0], netdev->dev_addr, 
                    ACFG_MACADDR_LEN);

            memcpy(&resp.ifname[i][0],netdev->name,ACFG_MAX_IFNAME);
            atd_print_mac_addr(&resp.mac_addr[i][0], __func__);
        }
    }

    resp.num_radio = atsc->wifi_cnt;

    wmi_cmd_rsp(atsc->wmi_handle, cmdid, status,
                (a_uint8_t *)&resp, sizeof(atd_radio_query_resp_t));

}

a_status_t 
atd_tgt_set_vap_index (atd_tgt_wifi_t * wifisc, 
                       atd_tgt_vap_t * vap) 
{ 
    int i ;
    
    for (i = 0; i< MAX_VAP_SUPPORTED; i++) {
        if (wifisc->vap_list[i] == NULL) {
            wifisc->vap_list[i] = vap ;
            vap->vap_index = i ;
	    vap->vap_netdev->irq = (vap->vap_netdev->irq | i);
            atd_trace(ATD_DEBUG_CFG, ("%s : vap %d \n", __func__, i));
            break;
        }
    }
    if(i == MAX_VAP_SUPPORTED){
        atd_trace(ATD_DEBUG_CFG, ("Max vap reached \n"));
        return A_STATUS_FAILED;
    }

    wifisc->vap_cnt++;

    return A_STATUS_OK;
}
void 
atd_tgt_vap_open(void *ctx, a_uint16_t cmdid, 
                    a_uint8_t *buffer, a_int32_t len)
{
    a_status_t status = A_STATUS_OK;
    atd_vap_open_cmd_t *vapcmd;
    atd_tgt_dev_t *atsc = (atd_tgt_dev_t *)ctx;
    atd_tgt_wifi_t *wifisc =NULL;
    atd_tgt_vap_t * vap;
 

    atd_trace(ATD_DEBUG_FUNCTRACE,(" cmd %d   ", \
                cmdid));

    atd_assert(len >= sizeof(atd_vap_open_cmd_t));

    vapcmd = (atd_vap_open_cmd_t *) buffer;

    atd_trace(ATD_DEBUG_CFG,("radio %d vap %d ", \
                vapcmd->wifi_index, vapcmd->vap_index));


    wifisc = atsc->wifi_list[vapcmd->wifi_index];
    if(wifisc == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    vap    = wifisc->vap_list[vapcmd->vap_index];
    if(vap == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    rtnl_lock();
    dev_open(vap->vap_netdev);
    rtnl_unlock();

#ifdef NETDEV_ALLMULTI
    if (vap->vap_netdev->netdev_ops &&
            vap->vap_netdev->netdev_ops->ndo_set_multicast_list) {
        vap->vap_netdev->flags |= IFF_ALLMULTI;
        vap->vap_netdev->netdev_ops->ndo_set_multicast_list(vap->vap_netdev);
    }
#endif

fail:
    wmi_cmd_rsp(atsc->wmi_handle, cmdid, status, NULL, 0); 
}
void 
atd_tgt_vap_close(void *ctx, a_uint16_t cmdid, 
                    a_uint8_t *buffer, a_int32_t len)
{
    a_status_t status = A_STATUS_OK;
    atd_vap_close_cmd_t *vapcmd;
    atd_tgt_dev_t *atsc = (atd_tgt_dev_t *)ctx;
    atd_tgt_wifi_t *wifisc =NULL;
    atd_tgt_vap_t * vap;
 

    atd_trace(ATD_DEBUG_FUNCTRACE,(" cmd %d   ", \
                cmdid));

    atd_assert(len >= sizeof(atd_vap_open_cmd_t));

    vapcmd = (atd_vap_open_cmd_t *) buffer;

    atd_trace(ATD_DEBUG_CFG,("radio %d vap %d ", vapcmd->wifi_index, \
                                    vapcmd->vap_index));


    wifisc = atsc->wifi_list[vapcmd->wifi_index];
    if(wifisc == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    vap    = wifisc->vap_list[vapcmd->vap_index];
    if(vap == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

#ifdef NETDEV_ALLMULTI
    if (vap->vap_netdev->netdev_ops &&
            vap->vap_netdev->netdev_ops->ndo_set_multicast_list) {
        vap->vap_netdev->flags &= ~IFF_ALLMULTI;
        vap->vap_netdev->netdev_ops->ndo_set_multicast_list(vap->vap_netdev);
    }
#endif

    rtnl_lock();
    dev_close(vap->vap_netdev);
    rtnl_unlock();

fail:
    wmi_cmd_rsp(atsc->wmi_handle, cmdid, status, NULL ,0);
}


void atd_tgt_vap_create(void *ctx, a_uint16_t cmdid, 
                        a_uint8_t *buffer, a_int32_t len)
{
    atd_vap_create_rsp_t vaprsp;
    atd_vap_create_cmd_t *vapcmd;

    atd_tgt_dev_t *atsc = (atd_tgt_dev_t *)ctx;
    atd_tgt_wifi_t *wifisc =NULL;
    struct ifreq ifr;
    atd_tgt_vap_t * vap;
    a_status_t status = A_STATUS_OK;
    struct net_device * netdev;
    struct ieee80211_clone_params cp;

    atd_trace(ATD_DEBUG_FUNCTRACE,("cmd %d    ", \
                cmdid));

    atd_assert(len >= sizeof(atd_vap_create_cmd_t));

    vapcmd = (atd_vap_create_cmd_t *) buffer;
    
    atd_trace(ATD_DEBUG_CFG,("radio%d:cmd.icp_flags%x cmd.icp_opmode%x \
                name %s  ", vapcmd->wifi_index, ntohs(vapcmd->icp_flags),\
                ntohs(vapcmd->icp_opmode), &vapcmd->icp_name[0]));

    atsc->dont_send_create_event = 1;

    memset(&ifr, 0, sizeof(ifr));
    memset(&cp , 0, sizeof(cp));

    cp.icp_opmode = ntohs(vapcmd->icp_opmode);
    cp.icp_flags  = ntohs(vapcmd->icp_flags);
    cp.icp_vapid  = ntohl(vapcmd->icp_vapid);
    memcpy(&cp.icp_name[0], &vapcmd->icp_name[0], ACFG_MAX_IFNAME);

    wifisc = atsc->wifi_list[vapcmd->wifi_index];

    ifr.ifr_data = (void *) &cp;

    status  = netdev_ioctl(wifisc->netdev,&ifr,SIOC80211IFCREATE);

    if(status != A_STATUS_OK){
        atd_trace(ATD_DEBUG_CFG, ("vap creation error status  %x \n",status));
        goto fail;
    }


    memcpy(&cp.icp_name[0], &ifr,sizeof(cp.icp_name));


    atd_trace(ATD_DEBUG_CFG,("vap instance created cp->icp_name  %s  %d  \
                                len %d ", cp.icp_name,\
                                 sizeof(cp.icp_name),strlen(cp.icp_name)));


    netdev      =  atd_netdev_get_by_name(cp.icp_name);

    if(netdev == NULL){
        atd_trace(ATD_DEBUG_CFG, ("vap creation error \n"));
        status = A_STATUS_ENXIO;
        goto fail ;
    }

    vap = kzalloc(sizeof(atd_tgt_vap_t), GFP_KERNEL);
    if(vap == NULL ){
        atd_trace(ATD_DEBUG_CFG, ("Unable to allocate memory \n"));
        status = A_STATUS_ENOMEM;
        goto fail ;
    }
    vap->vap_netdev = netdev;

    if(vapcmd->wifi_index)
	netdev->irq=0x80;
    else
	netdev->irq=0;

    status = atd_tgt_set_vap_index(wifisc, vap);
    if(status != A_STATUS_OK){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    vap->wifisc      = wifisc ;

    memcpy(&vap->mac_addr[0], &vap->vap_netdev->dev_addr[0], ACFG_MACADDR_LEN);

    vaprsp.vap_index = vap->vap_index ;
    memcpy(&vaprsp.vap_bssid[0], &vap->mac_addr[0], ACFG_MACADDR_LEN);
    memcpy(&vaprsp.name, vap->vap_netdev->name, IFNAMSIZ);
    atd_print_mac_addr(&vap->mac_addr[0], __func__);

    atsc->dont_send_create_event = 0;

fail:
    atsc->dont_send_create_event = 0;
    wmi_cmd_rsp(atsc->wmi_handle, cmdid, status, 
                (a_uint8_t *)&vaprsp , sizeof (atd_vap_create_rsp_t));
}

static void 
atd_tgt_vap_delete(void *ctx, a_uint16_t cmdid, 
                  a_uint8_t *buffer, a_int32_t len)
{
    a_status_t status = A_STATUS_OK;
    atd_tgt_dev_t *atsc = (atd_tgt_dev_t *)ctx;
    atd_tgt_wifi_t *wifisc =NULL;
    struct ifreq ifr;
    atd_tgt_vap_t * vap;

    atd_hdr_t  *atdhdr;

    atd_trace(ATD_DEBUG_FUNCTRACE, ("cmd %d    ", \
                cmdid));

    atd_assert(len >= sizeof(atd_hdr_t));

    atdhdr  = (atd_hdr_t *) buffer;


    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), (" uuuuu radio %d vap %d ", 
              atdhdr->wifi_index, atdhdr->vap_index ));

    wifisc = atsc->wifi_list[atdhdr->wifi_index];

    if(wifisc == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    vap    = wifisc->vap_list[atdhdr->vap_index];
    if(vap == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

   
    /** 
     * This will send the VAP Delete event to the HOST
     */

    status = netdev_ioctl(vap->vap_netdev, &ifr, SIOC80211IFDESTROY);
    

    atd_assert(status == A_STATUS_OK);

fail:
    wmi_cmd_rsp(atsc->wmi_handle, cmdid, status, NULL, 0);
}


void 
atd_tgt_set_ssid(void *ctx, a_uint16_t cmdid, 
                  a_uint8_t *buffer, a_int32_t len)
{
    a_status_t status = A_STATUS_OK;
    atd_tgt_dev_t *atsc = (atd_tgt_dev_t *)ctx;
    atd_tgt_wifi_t *wifisc =NULL;
    atd_tgt_vap_t * vap;
    union  iwreq_data iwreqdata;
    struct iw_point *iwpdata  = &iwreqdata.data;
    struct iw_request_info info;
    char tmpssid[ACFG_MAX_SSID_LEN + 1];

    atd_hdr_t  *atdhdr;
    atd_ssid_t  *ssid;

    atd_trace(ATD_DEBUG_FUNCTRACE, ("cmd %d    ", \
                cmdid));

    atd_assert(len >= (sizeof(atd_hdr_t) + sizeof(atd_ssid_t)));
  
    atdhdr  = (atd_hdr_t *) buffer;
    ssid    = (atd_ssid_t  *) (buffer + sizeof(atd_hdr_t)) ;

    ssid->len = ntohl(ssid->len);

    atd_trace(ATD_DEBUG_CFG, ("radio %d vap %d ssid %s len %d ", 
              atdhdr->wifi_index, atdhdr->vap_index, &ssid->name[0],
              ntohl(ssid->len)));

    wifisc = atsc->wifi_list[atdhdr->wifi_index];

    if(wifisc == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    vap    = wifisc->vap_list[atdhdr->vap_index];
 
    if(vap == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }




    iwpdata->flags = 1;
    iwpdata->length = ssid->len; 
    memset(tmpssid, 0, (ACFG_MAX_SSID_LEN + 1));
    memcpy(tmpssid, ssid->name, ssid->len);

    status = netdev_wext_handler(vap->vap_netdev, SIOCSIWESSID, &info, 
                                 &iwreqdata, tmpssid); 
    if(status != A_STATUS_OK){
        atd_trace(ATD_DEBUG_CFG, ("ssid set erro \n"));
    }

fail:
    wmi_cmd_rsp(atsc->wmi_handle, cmdid, status, NULL, 0);
}


void 
atd_tgt_get_ssid(void *ctx, a_uint16_t cmdid, 
                  a_uint8_t *buffer, a_int32_t len)
{
    a_status_t status = A_STATUS_OK;
    atd_tgt_dev_t *atsc = (atd_tgt_dev_t *)ctx;
    atd_tgt_wifi_t *wifisc =NULL;
    atd_tgt_vap_t * vap;
    union  iwreq_data iwreqdata;
    struct iw_point *iwpdata  = &iwreqdata.data;
    struct iw_request_info info;
    char tmpssid[ACFG_MAX_SSID_LEN + 1];

    atd_hdr_t  *atdhdr;
    atd_ssid_t  ssid;

    atd_trace(ATD_DEBUG_FUNCTRACE, ("cmd %d    ", \
                cmdid));

    atd_assert(len >= sizeof(atd_hdr_t));
  
    atdhdr  = (atd_hdr_t *) buffer;


    atd_trace(ATD_DEBUG_CFG, ("radio %d vap %d ", atdhdr->wifi_index, \
                atdhdr->vap_index));

    wifisc = atsc->wifi_list[atdhdr->wifi_index];
    if(wifisc == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    vap    = wifisc->vap_list[atdhdr->vap_index];
    if(vap == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }



    status = netdev_wext_handler(vap->vap_netdev, SIOCGIWESSID, &info, 
                                 &iwreqdata, tmpssid); 
    
    ssid.len = htonl(iwpdata->length);
    memcpy(ssid.name, tmpssid, iwpdata->length);

    atd_trace(ATD_DEBUG_CFG, ("ssid  %s len %d ", \
              &ssid.name[0], iwpdata->length));
fail:
     
    wmi_cmd_rsp(atsc->wmi_handle, cmdid, status, (a_uint8_t *)&ssid , 
                sizeof (atd_ssid_t));
}

void 
atd_tgt_set_testmode(void *ctx, a_uint16_t cmdid, 
                  a_uint8_t *buffer, a_int32_t len)
{
    a_status_t status = A_STATUS_OK;
    atd_tgt_dev_t *atsc = (atd_tgt_dev_t *)ctx;
    atd_tgt_wifi_t *wifisc =NULL;
    atd_tgt_vap_t * vap;
    a_uint8_t      *results= NULL;
    struct iwreq req;

    atd_hdr_t   *atdhdr;
    atd_vendor_t  *vendor;
    atd_testmode_t  *testmode;
    acfg_testmode_t  *test_mode;
    
    atd_trace(ATD_DEBUG_FUNCTRACE,(" cmd %d ", \
                cmdid));

    atd_assert(len >= (sizeof(atd_hdr_t) + sizeof(atd_testmode_t)));
    
    atdhdr  = (atd_hdr_t *) buffer;
    testmode    = (atd_testmode_t  *) (buffer + sizeof(atd_hdr_t)) ;

    wifisc = atsc->wifi_list[atdhdr->wifi_index];
    if(wifisc == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    vap  = wifisc->vap_list[atdhdr->vap_index];
    if(vap == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    memset(&req, 0, sizeof(struct iwreq));
    results = kzalloc(sizeof(atd_vendor_t)+sizeof(acfg_testmode_t), GFP_KERNEL);
    if(results == NULL ){
        status = A_STATUS_ENXIO;
        goto fail ;
    }

    vendor = (atd_vendor_t  *)results;
    vendor->cmd = IOCTL_SET_MASK | IEEE80211_IOCTL_TESTMODE;

    test_mode = (acfg_testmode_t *)(results+sizeof(atd_vendor_t));
    memcpy(test_mode->bssid, testmode->bssid, ACFG_MACADDR_LEN);
    test_mode->chan = ntohl(testmode->chan);
    test_mode->operation = ntohs(testmode->operation) ;
    test_mode->antenna = testmode->antenna;
    test_mode->rx = testmode->rx;
    test_mode->rssi_combined = ntohl(testmode->rssi_combined) ;
    test_mode->rssi0 = ntohl(testmode->rssi0);
    test_mode->rssi1 = ntohl(testmode->rssi1);
    test_mode->rssi2 = ntohl(testmode->rssi2);

    req.u.data.pointer = (a_uint8_t *)results;
    req.u.data.length  = sizeof(atd_vendor_t)+sizeof(acfg_testmode_t);
    req.u.data.flags   = 1;

    status = netdev_ioctl(vap->vap_netdev,(struct ifreq *)&req,
                          SIOCDEVVENDOR);

    if(status != A_STATUS_OK){
        atd_trace(ATD_DEBUG_CFG, ("get_testmode error \n"));
    }

fail:
    wmi_cmd_rsp(atsc->wmi_handle, cmdid, status, NULL, 0);
    
    if (results)    
        kfree(results);

    atd_trace(ATD_DEBUG_FUNCTRACE,(" End"));
}


void 
atd_tgt_get_testmode(void *ctx, a_uint16_t cmdid, 
                  a_uint8_t *buffer, a_int32_t len)
{
    a_status_t status = A_STATUS_OK;
    atd_tgt_dev_t *atsc = (atd_tgt_dev_t *)ctx;
    atd_tgt_wifi_t *wifisc =NULL;
    atd_tgt_vap_t * vap;
    a_uint8_t      *results= NULL;
    struct iwreq req;

    atd_hdr_t   *atdhdr;
    atd_vendor_t  *vendor;
    atd_testmode_t  *testmode, resp;
    acfg_testmode_t  *test_mode;
    
    atd_trace(ATD_DEBUG_FUNCTRACE,(" cmd %d ", \
                cmdid));

    atd_assert(len >= (sizeof(atd_hdr_t) + sizeof(atd_testmode_t)));

    atdhdr  = (atd_hdr_t *) buffer;
    testmode    = (atd_testmode_t  *) (buffer + sizeof(atd_hdr_t)) ;

    wifisc = atsc->wifi_list[atdhdr->wifi_index];
    if(wifisc == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    vap  = wifisc->vap_list[atdhdr->vap_index];
    if(vap == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    memset(&req, 0, sizeof(struct iwreq));
    results = kzalloc(sizeof(atd_vendor_t)+sizeof(acfg_testmode_t), GFP_KERNEL);
    if(results == NULL ){
        status = A_STATUS_ENXIO;
        goto fail ;
    }

    vendor = (atd_vendor_t  *)results;
    vendor->cmd = IOCTL_GET_MASK | IEEE80211_IOCTL_TESTMODE;

    test_mode = (acfg_testmode_t *)(results+sizeof(atd_vendor_t));
    memcpy(test_mode->bssid, testmode->bssid, ACFG_MACADDR_LEN);
    test_mode->chan = ntohl(testmode->chan);
    test_mode->operation = ntohs(testmode->operation) ;
    test_mode->antenna = testmode->antenna;
    test_mode->rx = testmode->rx;
    test_mode->rssi_combined = ntohl(testmode->rssi_combined) ;
    test_mode->rssi0 = ntohl(testmode->rssi0);
    test_mode->rssi1 = ntohl(testmode->rssi1);
    test_mode->rssi2 = ntohl(testmode->rssi2);

    req.u.data.pointer = (a_uint8_t *)results;
    req.u.data.length  = sizeof(atd_vendor_t)+sizeof(acfg_testmode_t);
    req.u.data.flags   = 1;

    status = netdev_ioctl(vap->vap_netdev,(struct ifreq *)&req,
                          SIOCDEVVENDOR);

    if(status != A_STATUS_OK){
        atd_trace(ATD_DEBUG_CFG, ("get_testmode error \n"));
    }

    memcpy(resp.bssid, test_mode->bssid, ACFG_MACADDR_LEN);
    resp.chan = htonl(test_mode->chan);
    resp.operation = htons(test_mode->operation) ;
    resp.antenna = test_mode->antenna;
    resp.rx = test_mode->rx;
    resp.rssi_combined = htonl(test_mode->rssi_combined) ;
    resp.rssi0 = htonl(test_mode->rssi0);
    resp.rssi1 = htonl(test_mode->rssi1);
    resp.rssi2 = htonl(test_mode->rssi2);

fail:
    wmi_cmd_rsp(atsc->wmi_handle, cmdid, status, 
                (a_uint8_t *)&resp, sizeof(atd_testmode_t));
    
    if (results)
        kfree(results);

    atd_trace(ATD_DEBUG_FUNCTRACE,(" End"));
}

void 
atd_tgt_get_rssi(void *ctx, a_uint16_t cmdid, 
                  a_uint8_t *buffer, a_int32_t len)
{
    a_status_t status      = A_STATUS_OK;
    atd_tgt_dev_t *atsc    = (atd_tgt_dev_t *)ctx;
    atd_tgt_wifi_t *wifisc = NULL;
    atd_tgt_vap_t  *vap    = NULL;
    a_uint8_t      *results= NULL;
    struct iwreq req;

    atd_hdr_t   *atdhdr;
    atd_vendor_t  *vendor;
    atd_rssi_t  *rssi;
    
    atd_trace(ATD_DEBUG_FUNCTRACE,(" cmd %d ", \
                cmdid));

    atd_assert(len >= sizeof(atd_hdr_t));

    atdhdr  = (atd_hdr_t *) buffer;

    wifisc = atsc->wifi_list[atdhdr->wifi_index];
    if(wifisc == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    vap  = wifisc->vap_list[atdhdr->vap_index];
    if(vap == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    memset(&req, 0, sizeof(struct iwreq));
    results = kzalloc(sizeof(atd_vendor_t)+sizeof(atd_rssi_t), GFP_KERNEL);
    if(results == NULL ){
        status = A_STATUS_ENXIO;
        goto fail ;
    }

    vendor = (atd_vendor_t  *)results;
    vendor->cmd = IOCTL_GET_MASK | IEEE80211_IOCTL_RSSI;

    req.u.data.pointer = (a_uint8_t *)results;
    req.u.data.length  = sizeof(atd_vendor_t)+sizeof(atd_rssi_t);
    req.u.data.flags   = 1;

    status = netdev_ioctl(vap->vap_netdev,(struct ifreq *)&req,
                          SIOCDEVVENDOR);

    if(status != A_STATUS_OK){
        atd_trace(ATD_DEBUG_CFG, ("get_rssi error \n"));
    }

fail:
    wmi_cmd_rsp(atsc->wmi_handle, cmdid, status, results+sizeof(atd_vendor_t), 
            req.u.data.length-sizeof(atd_vendor_t));
    
    kfree(results);

    atd_trace(ATD_DEBUG_FUNCTRACE,(" End"));
}

void 
atd_tgt_get_custdata(void *ctx, a_uint16_t cmdid, 
                  a_uint8_t *buffer, a_int32_t len)
{
    a_status_t status      = A_STATUS_OK;
    atd_tgt_dev_t *atsc    = (atd_tgt_dev_t *)ctx;
    atd_tgt_wifi_t *wifisc = NULL;
    atd_tgt_vap_t  *vap    = NULL;
    a_uint8_t      *results= NULL;
    struct iwreq req;

    atd_hdr_t   *atdhdr;
    atd_vendor_t  *vendor;
    atd_custdata_t  *custdata;
    
    atd_trace(ATD_DEBUG_FUNCTRACE,(" cmd %d ", \
                cmdid));

    atd_assert(len >= sizeof(atd_hdr_t));

    atdhdr  = (atd_hdr_t *) buffer;

    wifisc = atsc->wifi_list[atdhdr->wifi_index];
    if(wifisc == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    vap  = wifisc->vap_list[atdhdr->vap_index];
    if(vap == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    memset(&req, 0, sizeof(struct iwreq));
    results = kzalloc(sizeof(atd_vendor_t)+sizeof(atd_custdata_t), GFP_KERNEL);
    if(results == NULL ){
        status = A_STATUS_ENXIO;
        goto fail ;
    }

    vendor = (atd_vendor_t  *)results;
    vendor->cmd = IOCTL_GET_MASK | IEEE80211_IOCTL_CUSTDATA;

    req.u.data.pointer = (a_uint8_t *)results;
    req.u.data.length  = sizeof(atd_vendor_t)+sizeof(atd_custdata_t);
    req.u.data.flags   = 1;

    status = netdev_ioctl(vap->vap_netdev,(struct ifreq *)&req,
                          SIOCDEVVENDOR);

    if(status != A_STATUS_OK){
        atd_trace(ATD_DEBUG_CFG, ("get_custdata error \n"));
    }

fail:
    wmi_cmd_rsp(atsc->wmi_handle, cmdid, status, results+sizeof(atd_vendor_t), 
            req.u.data.length-sizeof(atd_vendor_t));
    
    kfree(results);

    atd_trace(ATD_DEBUG_FUNCTRACE,(" End"));
}

void 
atd_tgt_set_vap_param(void *ctx, a_uint16_t cmdid, 
                      a_uint8_t *buffer, a_int32_t len)
{

    a_status_t              status = A_STATUS_OK;
    atd_tgt_dev_t            *atsc = (atd_tgt_dev_t *)ctx;
    atd_tgt_wifi_t       *wifisc = NULL;
    atd_tgt_vap_t           *vap;
    atd_hdr_t               *atdhdr;
    struct iw_request_info  info = {0};
    union  iwreq_data       iwreq = {{0}};
    atd_param_t             *vap_param;

    atd_trace(ATD_DEBUG_FUNCTRACE, ("cmd %d    ", cmdid));

    atd_assert(len >= (sizeof(atd_hdr_t) + sizeof(atd_param_t)));
  
    atdhdr  = (atd_hdr_t *) buffer;
    vap_param    = (atd_param_t  *) (buffer + sizeof(atd_hdr_t)) ;

    vap_param->param = atd_tgt_acfg2ieee(ntohl ( vap_param->param ));
    vap_param->val   = ntohl ( vap_param->val );



    atd_trace(ATD_DEBUG_FUNCTRACE, (" radio %d vap %d param  %x  val %d", \
                                 atdhdr->wifi_index, atdhdr->vap_index,\
                                 vap_param->param, vap_param->val));

    wifisc = atsc->wifi_list[atdhdr->wifi_index];
    if(wifisc == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    vap    = wifisc->vap_list[atdhdr->vap_index];
    if(vap == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    info.cmd = IEEE80211_IOCTL_SETPARAM;
    info.flags = 0;
    iwreq.mode = vap_param->param;
    memcpy(iwreq.name + sizeof(uint32_t), &vap_param->val, sizeof(uint32_t));

    status = netdev_wext_handler(vap->vap_netdev, IEEE80211_IOCTL_SETPARAM,
                                 &info, &iwreq, (char *)&iwreq.mode);

fail:
    wmi_cmd_rsp(atsc->wmi_handle, cmdid, status, NULL, 0);
}

void 
atd_tgt_get_vap_param(void *ctx, a_uint16_t cmdid, 
                      a_uint8_t *buffer, a_int32_t len)
{

    a_status_t status = A_STATUS_OK;
    atd_tgt_dev_t *atsc = (atd_tgt_dev_t *)ctx;
    atd_tgt_wifi_t *wifisc =NULL;
    atd_tgt_vap_t * vap;
    atd_hdr_t   *atdhdr;

    struct iw_request_info info = {0};
    union  iwreq_data iwreq = {{0}};

    atd_param_t  *vap_param;
    atd_param_t   paramresult;

    atd_trace(ATD_DEBUG_FUNCTRACE, ("cmd %d    ",\
                cmdid));

    atd_assert(len >= (sizeof(atd_hdr_t) + sizeof(atd_param_t)));
  
    atdhdr  = (atd_hdr_t *) buffer;
    vap_param    = (atd_param_t  *) (buffer + sizeof(atd_hdr_t)) ;

    vap_param->param = atd_tgt_acfg2ieee(ntohl ( vap_param->param ));
    vap_param->val   = 0;



    atd_trace(ATD_DEBUG_FUNCTRACE, (" radio %d vap %d param  %x  val %d", \
                                 atdhdr->wifi_index, atdhdr->vap_index,\
                                 vap_param->param, vap_param->val));

    wifisc = atsc->wifi_list[atdhdr->wifi_index];
    if(wifisc == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    vap    = wifisc->vap_list[atdhdr->vap_index];
    if(vap == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    info.cmd = IEEE80211_IOCTL_GETPARAM;
    info.flags = 0;
    iwreq.mode = vap_param->param;

    status = netdev_wext_handler(vap->vap_netdev, IEEE80211_IOCTL_GETPARAM,
                                 &info, &iwreq, (char *)&iwreq.mode);

    paramresult.val   =   htonl(iwreq.mode);
    paramresult.param = 0;

    atd_trace(ATD_DEBUG_FUNCTRACE,("Result val %d \n",vap_param->param));

fail:
    wmi_cmd_rsp(atsc->wmi_handle, cmdid, status, (a_uint8_t *)&paramresult , 
                sizeof (atd_param_t));
}

void
atd_tgt_set_vap_vendor_param(void *ctx, a_uint16_t cmdid,
                      a_uint8_t *buffer, a_int32_t len)
{

    a_status_t              status = A_STATUS_OK;
    atd_tgt_dev_t           *atsc = (atd_tgt_dev_t *)ctx;
    atd_tgt_wifi_t          *wifisc = NULL;
    atd_tgt_vap_t           *vap;
    atd_hdr_t               *atdhdr;
    struct ifreq            ifr;
    atd_vendor_param_t      *vendor_param;

    atd_trace(ATD_DEBUG_FUNCTRACE, ("cmd %d    ", cmdid));

    atdhdr  = (atd_hdr_t *) buffer;
    vendor_param    = (atd_vendor_param_t  *) (buffer + sizeof(atd_hdr_t)) ;

    vendor_param->param = ntohl(vendor_param->param);
    vendor_param->type  = ntohl(vendor_param->type);

    if(vendor_param->type == ACFG_TYPE_INT)
        *(a_uint32_t *)&vendor_param->data = ntohl((*(a_uint32_t *)&vendor_param->data));

    atd_trace(ATD_DEBUG_FUNCTRACE, (" radio %d vap %d param  %x", \
                                 atdhdr->wifi_index, atdhdr->vap_index,\
                                 vendor_param->param));

    wifisc = atsc->wifi_list[atdhdr->wifi_index];
    if(wifisc == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }
    vap = wifisc->vap_list[atdhdr->vap_index];
    if(vap == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    memset(&ifr, 0, sizeof(ifr));
    ifr.ifr_data = (void *)vendor_param;

    status = netdev_ioctl(vap->vap_netdev, &ifr, LINUX_PVT_SET_VENDORPARAM);

fail:
    wmi_cmd_rsp(atsc->wmi_handle, cmdid, status, NULL, 0);
}

void
atd_tgt_get_vap_vendor_param(void *ctx, a_uint16_t cmdid,
                      a_uint8_t *buffer, a_int32_t len)
{

    a_status_t status = A_STATUS_OK;
    atd_tgt_dev_t *atsc = (atd_tgt_dev_t *)ctx;
    atd_tgt_wifi_t *wifisc =NULL;
    atd_tgt_vap_t * vap;
    atd_hdr_t   *atdhdr;

    struct ifreq            ifr;
    atd_vendor_param_t      *vendor_param;
    atd_vendor_param_t      paramresult;

    atd_trace(ATD_DEBUG_FUNCTRACE, ("cmd %d    ",\
                cmdid));

    atdhdr  = (atd_hdr_t *) buffer;
    vendor_param    = (atd_param_t  *) (buffer + sizeof(atd_hdr_t)) ;

    vendor_param->param = ntohl(vendor_param->param);

    atd_trace(ATD_DEBUG_FUNCTRACE, (" radio %d vap %d param  %x", \
                                 atdhdr->wifi_index, atdhdr->vap_index,\
                                 vendor_param->param));

    wifisc = atsc->wifi_list[atdhdr->wifi_index];
    if(wifisc == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    vap    = wifisc->vap_list[atdhdr->vap_index];
    if(vap == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    memset(&ifr, 0, sizeof(ifr));
    ifr.ifr_data = (void *)vendor_param;

    status = netdev_ioctl(vap->vap_netdev, &ifr, LINUX_PVT_GET_VENDORPARAM);

    if(vendor_param->type == ACFG_TYPE_INT)
        *(a_uint32_t *)&paramresult.data   =   htonl((*(a_uint32_t *)&vendor_param->data));
    else
        memcpy(&paramresult.data, &vendor_param->data, sizeof(acfg_vendor_param_data_t));

    paramresult.type   =   htonl(vendor_param->type);
    paramresult.param = 0;

fail:
    wmi_cmd_rsp(atsc->wmi_handle, cmdid, status, (a_uint8_t *)&paramresult ,
                sizeof(atd_vendor_param_t));
}

void 
atd_tgt_set_wifi_param(void *ctx, a_uint16_t cmdid, 
                      a_uint8_t *buffer, a_int32_t len)
{

    a_status_t status = A_STATUS_OK;
    atd_tgt_dev_t *atsc = (atd_tgt_dev_t *)ctx;
    atd_tgt_wifi_t *wifisc =NULL;
    atd_hdr_t   *atdhdr;

    struct iw_request_info info;
    union  iwreq_data iwreq;

    atd_param_t  *wifi_param;
    a_uint32_t rsp = 0 ;

    atd_trace(ATD_DEBUG_FUNCTRACE, ("cmd %d    ", \
                cmdid));

    atd_assert(len >= (sizeof(atd_hdr_t) + sizeof(wifi_param)));
  
    atdhdr  = (atd_hdr_t *) buffer;
    wifi_param    = (atd_param_t  *) (buffer + sizeof(atd_hdr_t)) ;

    wifi_param->param = ntohl ( wifi_param->param);
    wifi_param->val   = ntohl ( wifi_param->val );



    atd_trace(ATD_DEBUG_FUNCTRACE, (" radio %d vap %d param  %x  val %d", \
                                 atdhdr->wifi_index, atdhdr->vap_index,\
                                 wifi_param->param, wifi_param->val));

    wifisc = atsc->wifi_list[atdhdr->wifi_index];
    if(wifisc == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }


    status = netdev_wext_handler(wifisc->netdev, ATH_HAL_IOCTL_SETPARAM,
                                 &info, &iwreq, (char *)wifi_param);

fail:

    wmi_cmd_rsp(atsc->wmi_handle, cmdid, status, (a_uint8_t *)&rsp , 
                sizeof (a_int32_t));
}

void 
atd_tgt_get_wifi_param(void *ctx, a_uint16_t cmdid, 
                      a_uint8_t *buffer, a_int32_t len)
{

    a_status_t status = A_STATUS_OK;
    atd_tgt_dev_t *atsc = (atd_tgt_dev_t *)ctx;
    atd_tgt_wifi_t *wifisc =NULL;
    atd_hdr_t   *atdhdr;

    struct iw_request_info info;
    union  iwreq_data iwreq;

    atd_param_t  *wifi_param;
    atd_param_t   paramresult;

    atd_trace(ATD_DEBUG_FUNCTRACE, ("cmd %d    ", cmdid));

    atd_assert(len >= (sizeof(atd_hdr_t) + sizeof(wifi_param)));

    atdhdr  = (atd_hdr_t *) buffer;
    wifi_param    = (atd_param_t  *) (buffer + sizeof(atd_hdr_t)) ;

    wifi_param->param = ntohl ( wifi_param->param);



    atd_trace(ATD_DEBUG_FUNCTRACE, (" radio %d vap %d param  %x  val %d", \
                                 atdhdr->wifi_index, atdhdr->vap_index,\
                                 wifi_param->param, wifi_param->val));

    wifisc = atsc->wifi_list[atdhdr->wifi_index];

    if(wifisc == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }



    status = netdev_wext_handler(wifisc->netdev, ATH_HAL_IOCTL_GETPARAM, 
                                 &info, &iwreq, (char *)&wifi_param->param);
   
    paramresult.val     = htonl(wifi_param->param );
    paramresult.param   = 0;

    atd_trace(ATD_DEBUG_FUNCTRACE ,("Result val %d \n",wifi_param->param));


fail:

    wmi_cmd_rsp(atsc->wmi_handle, cmdid, status, (a_uint8_t *)&paramresult , 
                sizeof (atd_param_t));
}


void 
atd_tgt_get_opmode(void *ctx, a_uint16_t cmdid, 
                  a_uint8_t *buffer, a_int32_t len)
{
    a_status_t status = A_STATUS_OK;
    atd_tgt_dev_t *atsc = (atd_tgt_dev_t *)ctx;
    atd_tgt_wifi_t *wifisc =NULL;
    atd_tgt_vap_t * vap;
    struct iw_request_info info;

    atd_hdr_t   *atdhdr;
    union  iwreq_data iwreq;
    a_uint8_t   *extra =  NULL;

    atd_trace(ATD_DEBUG_FUNCTRACE, ("cmd %d    ", cmdid));

    atd_assert(len >= sizeof(atd_hdr_t));
  
    atdhdr  = (atd_hdr_t  *) buffer;

    wifisc = atsc->wifi_list[atdhdr->wifi_index];
    if(wifisc == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    vap    = wifisc->vap_list[atdhdr->vap_index];

    if(vap == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    status = netdev_wext_handler(vap->vap_netdev, SIOCGIWMODE, &info, &iwreq, 
                                 extra); 

    atd_trace(ATD_DEBUG_FUNCTRACE , ("mode %x ",iwreq.mode));
    iwreq.mode = htonl(iwreq.mode);

fail:

    wmi_cmd_rsp(atsc->wmi_handle, cmdid, status, (a_uint8_t *) (&iwreq.mode ), 
                sizeof (a_int32_t));
}

void 
atd_tgt_set_chmode(void *ctx, a_uint16_t cmdid, 
                  a_uint8_t *buffer, a_int32_t len)
{
    a_status_t status = A_STATUS_OK;
    atd_tgt_dev_t *atsc = (atd_tgt_dev_t *)ctx;
    atd_tgt_wifi_t *wifisc =NULL;
    atd_tgt_vap_t * vap;
    union  iwreq_data iwreqdata;
    struct iw_point *iwpdata  = &iwreqdata.data;
    struct iw_request_info info;

    atd_hdr_t  *atdhdr;
    atd_chmode_t *mode;

    atd_trace(ATD_DEBUG_FUNCTRACE, ("cmd %d    ", cmdid));

    atd_assert(len >= (sizeof(atd_chmode_t) + sizeof(atd_hdr_t)));

    atdhdr  = (atd_hdr_t *) buffer;
    mode    = (atd_chmode_t *) (buffer + sizeof(atd_hdr_t));

    mode->len = ntohl(mode->len);
    atd_assert(mode->len <= ACFG_MAX_CHMODE_LEN);

    atd_trace(ATD_DEBUG_CFG, ("radio %d vap %d chmode %s len %d ", 
              atdhdr->wifi_index, atdhdr->vap_index, &mode->mode[0],
              mode->len));

    wifisc = atsc->wifi_list[atdhdr->wifi_index];

    if(wifisc == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    vap    = wifisc->vap_list[atdhdr->vap_index];

    if(vap == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    info.cmd = IEEE80211_IOCTL_SETMODE;
    info.flags = 0;
    iwpdata->length = mode->len;
    iwpdata->flags = 0;
    iwpdata->pointer = (char *)mode->mode;

    status = netdev_wext_handler(vap->vap_netdev, IEEE80211_IOCTL_SETMODE, &info, 
                                 &iwreqdata, NULL); 
    if(status != A_STATUS_OK){
        atd_trace(ATD_DEBUG_CFG, ("chmode set erro \n"));
    }

fail:
    wmi_cmd_rsp(atsc->wmi_handle, cmdid, status, NULL, 0);
}

void 
atd_tgt_get_chmode(void *ctx, a_uint16_t cmdid, 
                  a_uint8_t *buffer, a_int32_t len)
{

    a_status_t status = A_STATUS_OK;
    atd_tgt_dev_t *atsc = (atd_tgt_dev_t *)ctx;
    atd_tgt_wifi_t *wifisc =NULL;
    atd_tgt_vap_t * vap;
    union  iwreq_data iwreq;
    struct iw_request_info info;

    atd_hdr_t  *atdhdr;

    atd_chmode_t mode = {0};

    atd_trace(ATD_DEBUG_FUNCTRACE, ("cmd %d    ", cmdid));

    atd_assert(len >= sizeof(atd_hdr_t));
  
    atdhdr      = (atd_hdr_t  *)  buffer;
    wifisc = atsc->wifi_list[atdhdr->wifi_index];
    if(wifisc == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    vap    = wifisc->vap_list[atdhdr->vap_index];
    if(vap == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    status = netdev_wext_handler(vap->vap_netdev, IEEE80211_IOCTL_GETMODE, &info, &iwreq, 
                                 mode.mode); 

    mode.len = htonl(iwreq.data.length);

fail:
    wmi_cmd_rsp(atsc->wmi_handle, cmdid, status, (a_uint8_t *)&mode, sizeof(atd_chmode_t));
}


void 
atd_tgt_get_wireless_name(void *ctx, a_uint16_t cmdid, 
                           a_uint8_t *buffer, a_int32_t len)
{
    a_status_t status = A_STATUS_OK;
    atd_tgt_dev_t *atsc = (atd_tgt_dev_t *)ctx;
    atd_tgt_wifi_t *wifisc =NULL;
    atd_tgt_vap_t * vap;
    atd_hdr_t   *atdhdr;

    struct iw_request_info info;
    union  iwreq_data iwreq;
    a_uint8_t   *extra = NULL;


    atd_trace(ATD_DEBUG_FUNCTRACE, ("cmd %d    ", cmdid));

    atd_assert(len >= sizeof(atd_hdr_t));
  
    atdhdr      = (atd_hdr_t  *)  buffer;
    wifisc = atsc->wifi_list[atdhdr->wifi_index];
    if(wifisc == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    vap    = wifisc->vap_list[atdhdr->vap_index];
    if(vap == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    status = netdev_wext_handler(vap->vap_netdev, SIOCGIWNAME, &info, &iwreq, 
                                 extra); 

    atd_trace(ATD_DEBUG_CFG , (" wireless name %s ",iwreq.name));
fail:
    wmi_cmd_rsp(atsc->wmi_handle, cmdid, status, (a_uint8_t *)&iwreq.name[0], 
                ACFG_MAX_IFNAME);
}

void 
atd_tgt_set_ap (void *ctx, a_uint16_t cmdid, 
                           a_uint8_t *buffer, a_int32_t len)
{
    a_status_t      status = A_STATUS_OK;
    atd_tgt_dev_t  *atsc = (atd_tgt_dev_t *)ctx;
    atd_tgt_wifi_t *wifisc =NULL;
    atd_tgt_vap_t  *vap;
    atd_hdr_t	   *atdhdr;

    struct iw_request_info info;
    union  iwreq_data iwreq;

    acfg_macaddr_t *paddr;
    a_uint32_t rsp = 0 ;

    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("cmd %d	 ", cmdid));

    atd_assert(len >= (sizeof(acfg_macaddr_t) + sizeof(atd_hdr_t)));

    atdhdr	= (atd_hdr_t *) buffer;
    paddr   = (acfg_macaddr_t  *) (buffer + sizeof(atd_hdr_t)) ;

    wifisc = atsc->wifi_list[atdhdr->wifi_index];
    if(wifisc == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    vap    = wifisc->vap_list[atdhdr->vap_index];
    if(vap == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }
	
    memcpy(iwreq.ap_addr.sa_data, paddr->addr, ACFG_MACADDR_LEN);
	
    atd_trace(ATD_DEBUG_CFG, ("ap macaddr %02x:%02x:%02x:%02x:%02x:%02x", \
                               iwreq.ap_addr.sa_data[0], iwreq.ap_addr.sa_data[1], \
                               iwreq.ap_addr.sa_data[2], iwreq.ap_addr.sa_data[3], \
                               iwreq.ap_addr.sa_data[4], iwreq.ap_addr.sa_data[5]));

    status = netdev_wext_handler(vap->vap_netdev, SIOCSIWAP, &info, &iwreq, NULL);
	
    if (status != A_STATUS_OK) {
        atd_trace(ATD_DEBUG_CFG, ("set encode error, status = %d\n", status));
        rsp = 0xFFFFFFFF;
    }
		
fail:

    wmi_cmd_rsp(atsc->wmi_handle, cmdid, status, (a_uint8_t *)&rsp, 
                sizeof (a_int32_t));
}

void 
atd_tgt_get_ap (void *ctx, a_uint16_t cmdid, 
                           a_uint8_t *buffer, a_int32_t len)
{
    a_status_t status = A_STATUS_OK;
    atd_tgt_dev_t *atsc = (atd_tgt_dev_t *)ctx;
    atd_tgt_wifi_t *wifisc =NULL;
    atd_tgt_vap_t * vap;
    atd_hdr_t   *atdhdr;

    struct iw_request_info info;
    union  iwreq_data iwreq;
    a_uint8_t   *extra = NULL;



    atd_trace(ATD_DEBUG_FUNCTRACE, ("cmd %d    ", \
                cmdid));

    atd_assert(len >= sizeof(atd_hdr_t));
  
    atdhdr      = (atd_hdr_t  *)  buffer;

    wifisc = atsc->wifi_list[atdhdr->wifi_index];
    if(wifisc == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    vap    = wifisc->vap_list[atdhdr->vap_index];
    if(vap == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    status = netdev_wext_handler(vap->vap_netdev, SIOCGIWAP, &info, &iwreq, 
                                 extra); 

   
    atd_trace(ATD_DEBUG_CFG, (" ap mac addr %x:%x:%x:%x:%x:%x family %x ", \
              iwreq.ap_addr.sa_data[0], iwreq.ap_addr.sa_data[1], \
              iwreq.ap_addr.sa_data[2], iwreq.ap_addr.sa_data[3], \
              iwreq.ap_addr.sa_data[4], iwreq.ap_addr.sa_data[5], \
              iwreq.ap_addr.sa_family));

fail:
    wmi_cmd_rsp(atsc->wmi_handle, cmdid, status, 
                 (a_uint8_t *)&iwreq.ap_addr.sa_data[0], ACFG_MACADDR_LEN);
}

void 
atd_tgt_get_rate(void *ctx, a_uint16_t cmdid, 
                           a_uint8_t *buffer, a_int32_t len)
{
    a_status_t status = A_STATUS_OK;
    atd_tgt_dev_t *atsc = (atd_tgt_dev_t *)ctx;
    atd_tgt_wifi_t *wifisc =NULL;
    atd_tgt_vap_t * vap;
    atd_hdr_t   *atdhdr;

    struct iw_request_info info;
    union  iwreq_data iwreq;
    a_uint8_t   *extra = NULL;


    atd_trace(ATD_DEBUG_FUNCTRACE, ("cmd %d    ", cmdid));

    atd_assert(len >= sizeof(atd_hdr_t));
  
    atdhdr      = (atd_hdr_t  *)  buffer;
    wifisc = atsc->wifi_list[atdhdr->wifi_index];
    if(wifisc == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    vap    = wifisc->vap_list[atdhdr->vap_index];
    if(vap == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }
    status = netdev_wext_handler(vap->vap_netdev, SIOCGIWRATE, &info, &iwreq, 
                                 extra); 


    atd_trace(ATD_DEBUG_CFG, (" rrq->fixed %d rrq->value %d  ", \
                                  iwreq.bitrate.fixed, iwreq.bitrate.value));
    
    iwreq.bitrate.value = htonl(iwreq.bitrate.value);
fail:

    wmi_cmd_rsp(atsc->wmi_handle, cmdid, status,
                    (a_uint8_t *)&iwreq.bitrate.value, sizeof(a_uint32_t));
}


static void 
atd_tgt_set_scan(void *ctx, a_uint16_t cmdid, 
                  a_uint8_t *buffer, a_int32_t len)
{
    a_status_t     status = A_STATUS_OK;
    atd_tgt_dev_t *atsc   = (atd_tgt_dev_t *)ctx;
    atd_tgt_wifi_t *wifisc= NULL;
    atd_tgt_vap_t  *vap   = NULL;
    acfg_set_scan_t *scan = NULL;
    a_uint8_t      *extra = NULL;
    atd_set_scan_t *pld =NULL;
    a_uint16_t      i, pld_len;

    union  iwreq_data iwreqdata;
    struct iw_point *iwpdata  = &iwreqdata.data;
    struct iw_request_info info;


    atd_hdr_t   *atdhdr;

    atd_trace(ATD_DEBUG_FUNCTRACE,(" cmd %d  ", \
                cmdid));

    atdhdr  = (atd_hdr_t *) buffer;
    pld = (atd_set_scan_t *)(atdhdr + 1);

    wifisc = atsc->wifi_list[atdhdr->wifi_index];
    if(wifisc == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    vap    = wifisc->vap_list[atdhdr->vap_index];

    if(vap == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    pld_len = ntohs(pld->len);

    scan = (acfg_set_scan_t *)kzalloc(sizeof(acfg_set_scan_t), GFP_KERNEL);
    if(scan == NULL ){
        status = A_STATUS_ENXIO;
        goto fail ;
    }

    iwpdata->flags   = ntohs(pld->point_flags);
    iwpdata->length  = pld_len;

    scan->scan_type = pld->scan_type;
    scan->essid_len = pld->essid_len;
    scan->num_channels = pld->num_channels;
    scan->flags = pld->flags;
    scan->bssid.sa_family = ntohs(pld->bssid.sa_family);
    memcpy(scan->bssid.sa_data, pld->bssid.sa_data, 14);
    memcpy(scan->essid, pld->essid, ACFG_MAX_SSID_LEN);
    scan->min_channel_time = ntohl(pld->min_channel_time);
    scan->max_channel_time = ntohl(pld->max_channel_time);
    for(i=0; i<scan->num_channels; i++)
    {
        scan->channel_list[i].m = ntohl(pld->channel_list[i].m);
        scan->channel_list[i].e = ntohs(pld->channel_list[i].e);
        scan->channel_list[i].i = pld->channel_list[i].i;
        scan->channel_list[i].flags = pld->channel_list[i].flags;
    }
    iwpdata->pointer = scan;

    status = netdev_wext_handler(vap->vap_netdev, SIOCSIWSCAN, &info, 
                                 &iwreqdata, (a_uint8_t *)extra); 
    if(status != A_STATUS_OK){
        atd_trace(ATD_DEBUG_CFG, ("scan set error \n"));
    }
fail:
    wmi_cmd_rsp(atsc->wmi_handle, cmdid, status, NULL, 0);

    kfree(scan);

    atd_trace(ATD_DEBUG_FUNCTRACE,(" End"));
}

static void
atd_tgt_get_ath_stats(void *ctx, a_uint16_t cmdid,
                  a_uint8_t *buffer, a_int32_t len)
{
    a_status_t status = A_STATUS_OK;
    atd_tgt_dev_t *atsc = (atd_tgt_dev_t *)ctx;
    atd_tgt_wifi_t *wifisc = NULL;
    struct ifreq ifr;
    atd_hdr_t *atdhdr;
    atd_ath_stats_t *athstats;
    acfg_ath_stats_t ath_stats = {0};
    a_uint32_t resp_size = 0;
    void *resp = NULL;
    
    atd_trace(ATD_DEBUG_FUNCTRACE,(" cmd %d ", cmdid));

    atd_assert(len >= (sizeof(atd_hdr_t) + sizeof(atd_ath_stats_t)));
    atdhdr = (atd_hdr_t *)buffer;
    athstats = (atd_ath_stats_t *)(buffer + sizeof(atd_hdr_t));

    atd_trace(ATD_DEBUG_FUNCTRACE, (" radio %d vap %d ", \
                          atdhdr->wifi_index, atdhdr->vap_index));

    wifisc = atsc->wifi_list[atdhdr->wifi_index];
    if (wifisc == NULL) {
        status = A_STATUS_ENXIO;
        goto fail;
    }

    ath_stats.offload_if = -1;
    ath_stats.size = max(sizeof(acfg_ath_stats_11n_t), sizeof(acfg_ath_stats_11ac_t));
    ath_stats.address = kzalloc(ath_stats.size, GFP_KERNEL);
    if (ath_stats.address == NULL) {
        status = A_STATUS_ENOMEM;
        goto fail;
    }

    memset(&ifr, 0, sizeof(ifr));
    ifr.ifr_data = (void *)&ath_stats;

    status = netdev_ioctl(wifisc->netdev, &ifr, LINUX_PVT_GETATH_STATS);

    if (status != A_STATUS_OK) {
        atd_trace(ATD_DEBUG_CFG, ("athstats results fetch error \n"));
    } else {
        resp_size = ntohl(athstats->size);
        resp = kzalloc(resp_size, GFP_KERNEL);
        if (resp == NULL) {
            resp_size = 0;
            status = A_STATUS_ENOMEM;
            goto fail;
        }

        athstats = (atd_ath_stats_t *)resp;
        athstats->offload_if = htonl(ath_stats.offload_if);
        switch (ath_stats.offload_if) {
            case 0:
            {
                a_uint32_t i;
                acfg_ath_stats_11n_t *stats = (acfg_ath_stats_11n_t *)(ath_stats.address);
                atd_ath_stats_11n_t *n_stats = (atd_ath_stats_11n_t *)((a_uint8_t *)resp + sizeof(atd_ath_stats_t));
                athstats->size = htonl(sizeof(atd_ath_stats_11n_t));

                n_stats->ast_watchdog = htonl(stats->ast_watchdog);
                n_stats->ast_resetOnError = htonl(stats->ast_resetOnError);
                n_stats->ast_mat_ucast_encrypted = htonl(stats->ast_mat_ucast_encrypted);
                n_stats->ast_mat_rx_recrypt = htonl(stats->ast_mat_rx_recrypt);
                n_stats->ast_mat_rx_decrypt = htonl(stats->ast_mat_rx_decrypt);
                n_stats->ast_mat_key_switch = htonl(stats->ast_mat_key_switch);
                n_stats->ast_hardware = htonl(stats->ast_hardware);
                n_stats->ast_bmiss = htonl(stats->ast_bmiss);
                n_stats->ast_rxorn = htonl(stats->ast_rxorn);
                n_stats->ast_rxorn_bmiss = htonl(stats->ast_rxorn_bmiss);
                n_stats->ast_rxeol = htonl(stats->ast_rxeol);
                n_stats->ast_txurn = htonl(stats->ast_txurn);
                n_stats->ast_txto = htonl(stats->ast_txto);
                n_stats->ast_cst = htonl(stats->ast_cst);
                n_stats->ast_mib = htonl(stats->ast_mib);
                n_stats->ast_rx = htonl(stats->ast_rx);
                n_stats->ast_rxdesc = htonl(stats->ast_rxdesc);
                n_stats->ast_rxerr = htonl(stats->ast_rxerr);
                n_stats->ast_rxnofrm = htonl(stats->ast_rxnofrm);
                n_stats->ast_tx = htonl(stats->ast_tx);
                n_stats->ast_txdesc = htonl(stats->ast_txdesc);
                n_stats->ast_tim_timer = htonl(stats->ast_tim_timer);
                n_stats->ast_bbevent = htonl(stats->ast_bbevent);
                n_stats->ast_rxphy = htonl(stats->ast_rxphy);
                n_stats->ast_rxkcm = htonl(stats->ast_rxkcm);
                n_stats->ast_swba = htonl(stats->ast_swba);
                n_stats->ast_brssi = htonl(stats->ast_brssi);
                n_stats->ast_bnr = htonl(stats->ast_bnr);
                n_stats->ast_tim = htonl(stats->ast_tim);
                n_stats->ast_dtim = htonl(stats->ast_dtim);
                n_stats->ast_dtimsync = htonl(stats->ast_dtimsync);
                n_stats->ast_gpio = htonl(stats->ast_gpio);
                n_stats->ast_cabend = htonl(stats->ast_cabend);
                n_stats->ast_tsfoor = htonl(stats->ast_tsfoor);
                n_stats->ast_gentimer = htonl(stats->ast_gentimer);
                n_stats->ast_gtt = htonl(stats->ast_gtt);
                n_stats->ast_fatal = htonl(stats->ast_fatal);
                n_stats->ast_tx_packets = htonl(stats->ast_tx_packets);
                n_stats->ast_rx_packets = htonl(stats->ast_rx_packets);
                n_stats->ast_tx_mgmt = htonl(stats->ast_tx_mgmt);
                n_stats->ast_tx_discard = htonl(stats->ast_tx_discard);
                n_stats->ast_tx_invalid = htonl(stats->ast_tx_invalid);
                n_stats->ast_tx_qstop = htonl(stats->ast_tx_qstop);
                n_stats->ast_tx_encap = htonl(stats->ast_tx_encap);
                n_stats->ast_tx_nonode = htonl(stats->ast_tx_nonode);
                n_stats->ast_tx_nobuf = htonl(stats->ast_tx_nobuf);
                n_stats->ast_tx_stop = htonl(stats->ast_tx_stop);
                n_stats->ast_tx_resume = htonl(stats->ast_tx_resume);
                n_stats->ast_tx_nobufmgt = htonl(stats->ast_tx_nobufmgt);
                n_stats->ast_tx_xretries = htonl(stats->ast_tx_xretries);
                n_stats->ast_tx_hw_retries = cpu_to_be64(stats->ast_tx_hw_retries);
                n_stats->ast_tx_hw_success = cpu_to_be64(stats->ast_tx_hw_success);
                n_stats->ast_tx_fifoerr = htonl(stats->ast_tx_fifoerr);
                n_stats->ast_tx_filtered = htonl(stats->ast_tx_filtered);
                n_stats->ast_tx_badrate = htonl(stats->ast_tx_badrate);
                n_stats->ast_tx_noack = htonl(stats->ast_tx_noack);
                n_stats->ast_tx_cts = htonl(stats->ast_tx_cts);
                n_stats->ast_tx_shortpre = htonl(stats->ast_tx_shortpre);
                n_stats->ast_tx_altrate = htonl(stats->ast_tx_altrate);
                n_stats->ast_tx_protect = htonl(stats->ast_tx_protect);
                n_stats->ast_rx_orn = htonl(stats->ast_rx_orn);
                n_stats->ast_rx_badcrypt = htonl(stats->ast_rx_badcrypt);
                n_stats->ast_rx_badmic = htonl(stats->ast_rx_badmic);
                n_stats->ast_rx_nobuf = htonl(stats->ast_rx_nobuf);
                n_stats->ast_rx_swdecrypt = htonl(stats->ast_rx_swdecrypt);
                n_stats->ast_rx_hal_in_progress = htonl(stats->ast_rx_hal_in_progress);
                n_stats->ast_rx_num_data = htonl(stats->ast_rx_num_data);
                n_stats->ast_rx_num_mgmt = htonl(stats->ast_rx_num_mgmt);
                n_stats->ast_rx_num_ctl = htonl(stats->ast_rx_num_ctl);
                n_stats->ast_rx_num_unknown = htonl(stats->ast_rx_num_unknown);
                n_stats->ast_max_pkts_per_intr = htonl(stats->ast_max_pkts_per_intr);
                for (i = 0; i <= ATH_STATS_MAX_INTR_BKT; i++)
                    n_stats->ast_pkts_per_intr[i] = htonl(stats->ast_pkts_per_intr[i]);
                n_stats->ast_tx_rssi = stats->ast_tx_rssi;
                n_stats->ast_tx_rssi_ctl0 = stats->ast_tx_rssi_ctl0;
                n_stats->ast_tx_rssi_ctl1 = stats->ast_tx_rssi_ctl1;
                n_stats->ast_tx_rssi_ctl2 = stats->ast_tx_rssi_ctl2;
                n_stats->ast_tx_rssi_ext0 = stats->ast_tx_rssi_ext0;
                n_stats->ast_tx_rssi_ext1 = stats->ast_tx_rssi_ext1;
                n_stats->ast_tx_rssi_ext2 = stats->ast_tx_rssi_ext2;
                n_stats->ast_rx_rssi = stats->ast_rx_rssi;
                n_stats->ast_rx_rssi_ctl0 = stats->ast_rx_rssi_ctl0;
                n_stats->ast_rx_rssi_ctl1 = stats->ast_rx_rssi_ctl1;
                n_stats->ast_rx_rssi_ctl2 = stats->ast_rx_rssi_ctl2;
                n_stats->ast_rx_rssi_ext0 = stats->ast_rx_rssi_ext0;
                n_stats->ast_rx_rssi_ext1 = stats->ast_rx_rssi_ext1;
                n_stats->ast_rx_rssi_ext2 = stats->ast_rx_rssi_ext2;
                n_stats->ast_be_xmit = htonl(stats->ast_be_xmit);
                n_stats->ast_be_nobuf = htonl(stats->ast_be_nobuf);
                n_stats->ast_per_cal = htonl(stats->ast_per_cal);
                n_stats->ast_per_calfail = htonl(stats->ast_per_calfail);
                n_stats->ast_per_rfgain = htonl(stats->ast_per_rfgain);
                n_stats->ast_rate_calls = htonl(stats->ast_rate_calls);
                n_stats->ast_rate_raise = htonl(stats->ast_rate_raise);
                n_stats->ast_rate_drop = htonl(stats->ast_rate_drop);
                n_stats->ast_ant_defswitch = htonl(stats->ast_ant_defswitch);
                n_stats->ast_ant_txswitch = htonl(stats->ast_ant_txswitch);
                for (i = 0; i < 8; i++)
                    n_stats->ast_ant_rx[i] = htonl(stats->ast_ant_rx[i]);
                for (i = 0; i < 8; i++)
                    n_stats->ast_ant_tx[i] = htonl(stats->ast_ant_tx[i]);
                n_stats->ast_rx_bytes = cpu_to_be64(stats->ast_rx_bytes);
                n_stats->ast_tx_bytes = cpu_to_be64(stats->ast_tx_bytes);
                for (i = 0; i < 16; i++)
                    n_stats->ast_rx_num_qos_data[i] = htonl(stats->ast_rx_num_qos_data[i]);
                n_stats->ast_rx_num_nonqos_data = htonl(stats->ast_rx_num_nonqos_data);
                for (i = 0; i < 16; i++)
                    n_stats->ast_txq_packets[i] = htonl(stats->ast_txq_packets[i]);
                for (i = 0; i < 16; i++)
                    n_stats->ast_txq_xretries[i] = htonl(stats->ast_txq_xretries[i]);
                for (i = 0; i < 16; i++)
                    n_stats->ast_txq_fifoerr[i] = htonl(stats->ast_txq_fifoerr[i]);
                for (i = 0; i < 16; i++)
                    n_stats->ast_txq_filtered[i] = htonl(stats->ast_txq_filtered[i]);
                for (i = 0; i < 16; i++)
                    n_stats->ast_txq_athbuf_limit[i] = htonl(stats->ast_txq_athbuf_limit[i]);
                for (i = 0; i < 16; i++)
                    n_stats->ast_txq_nobuf[i] = htonl(stats->ast_txq_nobuf[i]);
                n_stats->ast_num_rxchain = stats->ast_num_rxchain;
                n_stats->ast_num_txchain = stats->ast_num_txchain;
                n_stats->ast_11n_stats.tx_pkts = htonl(stats->ast_11n_stats.tx_pkts);
                n_stats->ast_11n_stats.tx_checks = htonl(stats->ast_11n_stats.tx_checks);
                n_stats->ast_11n_stats.tx_drops = htonl(stats->ast_11n_stats.tx_drops);
                n_stats->ast_11n_stats.tx_minqdepth = htonl(stats->ast_11n_stats.tx_minqdepth);
                n_stats->ast_11n_stats.tx_queue = htonl(stats->ast_11n_stats.tx_queue);
                n_stats->ast_11n_stats.tx_resetq = htonl(stats->ast_11n_stats.tx_resetq);
                n_stats->ast_11n_stats.tx_comps = htonl(stats->ast_11n_stats.tx_comps);
                n_stats->ast_11n_stats.tx_comperror = htonl(stats->ast_11n_stats.tx_comperror);
                n_stats->ast_11n_stats.tx_unaggr_comperror = htonl(stats->ast_11n_stats.tx_unaggr_comperror);
                n_stats->ast_11n_stats.tx_stopfiltered = htonl(stats->ast_11n_stats.tx_stopfiltered);
                n_stats->ast_11n_stats.tx_qnull = htonl(stats->ast_11n_stats.tx_qnull);
                n_stats->ast_11n_stats.tx_noskbs = htonl(stats->ast_11n_stats.tx_noskbs);
                n_stats->ast_11n_stats.tx_nobufs = htonl(stats->ast_11n_stats.tx_nobufs);
                n_stats->ast_11n_stats.tx_badsetups = htonl(stats->ast_11n_stats.tx_badsetups);
                n_stats->ast_11n_stats.tx_normnobufs = htonl(stats->ast_11n_stats.tx_normnobufs);
                n_stats->ast_11n_stats.tx_schednone = htonl(stats->ast_11n_stats.tx_schednone);
                n_stats->ast_11n_stats.tx_bars = htonl(stats->ast_11n_stats.tx_bars);
                n_stats->ast_11n_stats.tx_legacy = htonl(stats->ast_11n_stats.tx_legacy);
                n_stats->ast_11n_stats.txunaggr_single = htonl(stats->ast_11n_stats.txunaggr_single);
                n_stats->ast_11n_stats.txbar_xretry = htonl(stats->ast_11n_stats.txbar_xretry);
                n_stats->ast_11n_stats.txbar_compretries = htonl(stats->ast_11n_stats.txbar_compretries);
                n_stats->ast_11n_stats.txbar_errlast = htonl(stats->ast_11n_stats.txbar_errlast);
                n_stats->ast_11n_stats.tx_compunaggr = htonl(stats->ast_11n_stats.tx_compunaggr);
                n_stats->ast_11n_stats.txunaggr_xretry = htonl(stats->ast_11n_stats.txunaggr_xretry);
                n_stats->ast_11n_stats.tx_compaggr = htonl(stats->ast_11n_stats.tx_compaggr);
                n_stats->ast_11n_stats.tx_bawadv = htonl(stats->ast_11n_stats.tx_bawadv);
                n_stats->ast_11n_stats.tx_bawretries = htonl(stats->ast_11n_stats.tx_bawretries);
                n_stats->ast_11n_stats.tx_bawnorm = htonl(stats->ast_11n_stats.tx_bawnorm);
                n_stats->ast_11n_stats.tx_bawupdates = htonl(stats->ast_11n_stats.tx_bawupdates);
                n_stats->ast_11n_stats.tx_bawupdtadv = htonl(stats->ast_11n_stats.tx_bawupdtadv);
                n_stats->ast_11n_stats.tx_retries = htonl(stats->ast_11n_stats.tx_retries);
                n_stats->ast_11n_stats.tx_xretries = htonl(stats->ast_11n_stats.tx_xretries);
                n_stats->ast_11n_stats.tx_aggregates = htonl(stats->ast_11n_stats.tx_aggregates);
                n_stats->ast_11n_stats.tx_sf_hw_xretries = htonl(stats->ast_11n_stats.tx_sf_hw_xretries);
                n_stats->ast_11n_stats.tx_aggr_frames = htonl(stats->ast_11n_stats.tx_aggr_frames);
                n_stats->ast_11n_stats.txaggr_noskbs = htonl(stats->ast_11n_stats.txaggr_noskbs);
                n_stats->ast_11n_stats.txaggr_nobufs = htonl(stats->ast_11n_stats.txaggr_nobufs);
                n_stats->ast_11n_stats.txaggr_badkeys = htonl(stats->ast_11n_stats.txaggr_badkeys);
                n_stats->ast_11n_stats.txaggr_schedwindow = htonl(stats->ast_11n_stats.txaggr_schedwindow);
                n_stats->ast_11n_stats.txaggr_single = htonl(stats->ast_11n_stats.txaggr_single);
                n_stats->ast_11n_stats.txaggr_mimo = htonl(stats->ast_11n_stats.txaggr_mimo);
                n_stats->ast_11n_stats.txaggr_compgood = htonl(stats->ast_11n_stats.txaggr_compgood);
                n_stats->ast_11n_stats.txaggr_comperror = htonl(stats->ast_11n_stats.txaggr_comperror);
                n_stats->ast_11n_stats.txaggr_compxretry = htonl(stats->ast_11n_stats.txaggr_compxretry);
                n_stats->ast_11n_stats.txaggr_compretries = htonl(stats->ast_11n_stats.txaggr_compretries);
                n_stats->ast_11n_stats.txunaggr_compretries = htonl(stats->ast_11n_stats.txunaggr_compretries);
                n_stats->ast_11n_stats.txaggr_prepends = htonl(stats->ast_11n_stats.txaggr_prepends);
                n_stats->ast_11n_stats.txaggr_filtered = htonl(stats->ast_11n_stats.txaggr_filtered);
                n_stats->ast_11n_stats.txaggr_fifo = htonl(stats->ast_11n_stats.txaggr_fifo);
                n_stats->ast_11n_stats.txaggr_xtxop = htonl(stats->ast_11n_stats.txaggr_xtxop);
                n_stats->ast_11n_stats.txaggr_desc_cfgerr = htonl(stats->ast_11n_stats.txaggr_desc_cfgerr);
                n_stats->ast_11n_stats.txaggr_data_urun = htonl(stats->ast_11n_stats.txaggr_data_urun);
                n_stats->ast_11n_stats.txaggr_delim_urun = htonl(stats->ast_11n_stats.txaggr_delim_urun);
                n_stats->ast_11n_stats.txaggr_errlast = htonl(stats->ast_11n_stats.txaggr_errlast);
                n_stats->ast_11n_stats.txunaggr_errlast = htonl(stats->ast_11n_stats.txunaggr_errlast);
                n_stats->ast_11n_stats.txaggr_longretries = htonl(stats->ast_11n_stats.txaggr_longretries);
                n_stats->ast_11n_stats.txaggr_shortretries = htonl(stats->ast_11n_stats.txaggr_shortretries);
                n_stats->ast_11n_stats.txaggr_timer_exp = htonl(stats->ast_11n_stats.txaggr_timer_exp);
                n_stats->ast_11n_stats.txaggr_babug = htonl(stats->ast_11n_stats.txaggr_babug);
                n_stats->ast_11n_stats.txrifs_single = htonl(stats->ast_11n_stats.txrifs_single);
                n_stats->ast_11n_stats.txrifs_babug = htonl(stats->ast_11n_stats.txrifs_babug);
                n_stats->ast_11n_stats.txaggr_badtid = htonl(stats->ast_11n_stats.txaggr_badtid);
                n_stats->ast_11n_stats.txrifs_compretries = htonl(stats->ast_11n_stats.txrifs_compretries);
                n_stats->ast_11n_stats.txrifs_bar_alloc = htonl(stats->ast_11n_stats.txrifs_bar_alloc);
                n_stats->ast_11n_stats.txrifs_bar_freed = htonl(stats->ast_11n_stats.txrifs_bar_freed);
                n_stats->ast_11n_stats.txrifs_compgood = htonl(stats->ast_11n_stats.txrifs_compgood);
                n_stats->ast_11n_stats.txrifs_prepends = htonl(stats->ast_11n_stats.txrifs_prepends);
                n_stats->ast_11n_stats.tx_comprifs = htonl(stats->ast_11n_stats.tx_comprifs);
                n_stats->ast_11n_stats.tx_compnorifs = htonl(stats->ast_11n_stats.tx_compnorifs);
                n_stats->ast_11n_stats.rx_pkts = htonl(stats->ast_11n_stats.rx_pkts);
                n_stats->ast_11n_stats.rx_aggr = htonl(stats->ast_11n_stats.rx_aggr);
                n_stats->ast_11n_stats.rx_aggrbadver = htonl(stats->ast_11n_stats.rx_aggrbadver);
                n_stats->ast_11n_stats.rx_bars = htonl(stats->ast_11n_stats.rx_bars);
                n_stats->ast_11n_stats.rx_nonqos = htonl(stats->ast_11n_stats.rx_nonqos);
                n_stats->ast_11n_stats.rx_seqreset= htonl(stats->ast_11n_stats.rx_seqreset);
                n_stats->ast_11n_stats.rx_oldseq = htonl(stats->ast_11n_stats.rx_oldseq);
                n_stats->ast_11n_stats.rx_bareset = htonl(stats->ast_11n_stats.rx_bareset);
                n_stats->ast_11n_stats.rx_baresetpkts = htonl(stats->ast_11n_stats.rx_baresetpkts);
                n_stats->ast_11n_stats.rx_dup = htonl(stats->ast_11n_stats.rx_dup);
                n_stats->ast_11n_stats.rx_baadvance = htonl(stats->ast_11n_stats.rx_baadvance);
                n_stats->ast_11n_stats.rx_recvcomp = htonl(stats->ast_11n_stats.rx_recvcomp);
                n_stats->ast_11n_stats.rx_bardiscard = htonl(stats->ast_11n_stats.rx_bardiscard);
                n_stats->ast_11n_stats.rx_barcomps = htonl(stats->ast_11n_stats.rx_barcomps);
                n_stats->ast_11n_stats.rx_barrecvs = htonl(stats->ast_11n_stats.rx_barrecvs);
                n_stats->ast_11n_stats.rx_skipped = htonl(stats->ast_11n_stats.rx_skipped);
                n_stats->ast_11n_stats.rx_comp_to = htonl(stats->ast_11n_stats.rx_comp_to);
                n_stats->ast_11n_stats.rx_timer_starts = htonl(stats->ast_11n_stats.rx_timer_starts);
                n_stats->ast_11n_stats.rx_timer_stops = htonl(stats->ast_11n_stats.rx_timer_stops);
                n_stats->ast_11n_stats.rx_timer_run = htonl(stats->ast_11n_stats.rx_timer_run);
                n_stats->ast_11n_stats.rx_timer_more = htonl(stats->ast_11n_stats.rx_timer_more);
                n_stats->ast_11n_stats.wd_tx_active = htonl(stats->ast_11n_stats.wd_tx_active);
                n_stats->ast_11n_stats.wd_tx_inactive = htonl(stats->ast_11n_stats.wd_tx_inactive);
                n_stats->ast_11n_stats.wd_tx_hung = htonl(stats->ast_11n_stats.wd_tx_hung);
                n_stats->ast_11n_stats.wd_spurious = htonl(stats->ast_11n_stats.wd_spurious);
                n_stats->ast_11n_stats.tx_requeue = htonl(stats->ast_11n_stats.tx_requeue);
                n_stats->ast_11n_stats.tx_drain_txq = htonl(stats->ast_11n_stats.tx_drain_txq);
                n_stats->ast_11n_stats.tx_drain_tid = htonl(stats->ast_11n_stats.tx_drain_tid);
                n_stats->ast_11n_stats.tx_cleanup_tid = htonl(stats->ast_11n_stats.tx_cleanup_tid);
                n_stats->ast_11n_stats.tx_drain_bufs = htonl(stats->ast_11n_stats.tx_drain_bufs);
                n_stats->ast_11n_stats.tx_tidpaused = htonl(stats->ast_11n_stats.tx_tidpaused);
                n_stats->ast_11n_stats.tx_tidresumed = htonl(stats->ast_11n_stats.tx_tidresumed);
                n_stats->ast_11n_stats.tx_unaggr_filtered = htonl(stats->ast_11n_stats.tx_unaggr_filtered);
                n_stats->ast_11n_stats.tx_aggr_filtered = htonl(stats->ast_11n_stats.tx_aggr_filtered);
                n_stats->ast_11n_stats.tx_filtered = htonl(stats->ast_11n_stats.tx_filtered);
                n_stats->ast_11n_stats.rx_rb_on = htonl(stats->ast_11n_stats.rx_rb_on);
                n_stats->ast_11n_stats.rx_rb_off = htonl(stats->ast_11n_stats.rx_rb_off);
                n_stats->ast_11n_stats.rx_dsstat_err = htonl(stats->ast_11n_stats.rx_dsstat_err);
#ifdef ATH_SUPPORT_TxBF
                n_stats->ast_11n_stats.bf_stream_miss = htonl(stats->ast_11n_stats.bf_stream_miss);
                n_stats->ast_11n_stats.bf_bandwidth_miss = htonl(stats->ast_11n_stats.bf_bandwidth_miss);
                n_stats->ast_11n_stats.bf_destination_miss = htonl(stats->ast_11n_stats.bf_destination_miss);
#endif
                n_stats->ast_dfs_stats.dfs_stats_valid = htonl(stats->ast_dfs_stats.dfs_stats_valid);
                n_stats->ast_dfs_stats.event_count = htonl(stats->ast_dfs_stats.event_count);
                n_stats->ast_dfs_stats.event_count = htonl(stats->ast_dfs_stats.event_count);
                n_stats->ast_dfs_stats.num_filter = htonl(stats->ast_dfs_stats.num_filter);
                for (i = 0; i < DFS_MAX_FILTER; i++) {
                    n_stats->ast_dfs_stats.fstat[i].max_pri_count = htonl(stats->ast_dfs_stats.fstat[i].max_pri_count);
                    n_stats->ast_dfs_stats.fstat[i].max_used_pri = htonl(stats->ast_dfs_stats.fstat[i].max_used_pri);
                    n_stats->ast_dfs_stats.fstat[i].excess_pri = htonl(stats->ast_dfs_stats.fstat[i].excess_pri);
                    n_stats->ast_dfs_stats.fstat[i].pri_threshold_reached = htonl(stats->ast_dfs_stats.fstat[i].pri_threshold_reached);
                    n_stats->ast_dfs_stats.fstat[i].dur_threshold_reached = htonl(stats->ast_dfs_stats.fstat[i].dur_threshold_reached);
                    n_stats->ast_dfs_stats.fstat[i].rssi_threshold_reached = htonl(stats->ast_dfs_stats.fstat[i].rssi_threshold_reached);
                    n_stats->ast_dfs_stats.fstat[i].filter_id = htonl(stats->ast_dfs_stats.fstat[i].filter_id);
                }
                n_stats->ast_bb_hang = htonl(stats->ast_bb_hang);
                n_stats->ast_mac_hang = htonl(stats->ast_mac_hang);
#if ATH_WOW
                n_stats->ast_wow_wakeups = htonl(stats->ast_wow_wakeups);
                n_stats->ast_wow_wakeupsok = htonl(stats->ast_wow_wakeupsok);
                n_stats->ast_wow_wakeupserror = htonl(stats->ast_wow_wakeupserror);
#if ATH_WOW_DEBUG
                n_stats->ast_normal_sleeps = htonl(stats->ast_normal_sleeps);
                n_stats->ast_normal_wakeups = htonl(stats->ast_normal_wakeups);
                n_stats->ast_wow_sleeps = htonl(stats->ast_wow_sleeps);
                n_stats->ast_wow_sleeps_nonet = htonl(stats->ast_wow_sleeps_nonet);
#endif
#endif
#ifdef ATH_SUPPORT_UAPSD
                n_stats->ast_uapsdqnulbf_unavail = htonl(stats->ast_uapsdqnulbf_unavail);
                n_stats->ast_uapsdqnul_pkts = htonl(stats->ast_uapsdqnul_pkts);
                n_stats->ast_uapsdtriggers = htonl(stats->ast_uapsdtriggers);
                n_stats->ast_uapsdnodeinvalid = htonl(stats->ast_uapsdnodeinvalid);
                n_stats->ast_uapsdeospdata = htonl(stats->ast_uapsdeospdata);
                n_stats->ast_uapsddata_pkts = htonl(stats->ast_uapsddata_pkts);
                n_stats->ast_uapsddatacomp = htonl(stats->ast_uapsddatacomp);
                n_stats->ast_uapsdqnulcomp = htonl(stats->ast_uapsdqnulcomp);
                n_stats->ast_uapsddataqueued = htonl(stats->ast_uapsddataqueued);
                n_stats->ast_uapsdedmafifofull = htonl(stats->ast_uapsdedmafifofull);
#endif
#if ATH_SUPPORT_VOWEXT
                for (i = 0; i < 4; i++)
                    n_stats->ast_vow_ul_tx_calls[i] = htonl(stats->ast_vow_ul_tx_calls[i]);
                for (i = 0; i < 4; i++)
                    n_stats->ast_vow_ath_txq_calls[i] = htonl(stats->ast_vow_ath_txq_calls[i]);
                n_stats->ast_vow_ath_be_drop = htonl(stats->ast_vow_ath_be_drop);
                n_stats->ast_vow_ath_bk_drop = htonl(stats->ast_vow_ath_bk_drop);
#endif
#if ATH_SUPPORT_CFEND
                n_stats->ast_cfend_sched = htonl(stats->ast_cfend_sched);
                n_stats->ast_cfend_sent = htonl(stats->ast_cfend_sent);
#endif
#ifdef VOW_LOGLATENCY
                for (i = 0; i < ATH_STATS_LATENCY_CATS; i++) {
                    a_uint32_t j;
                    for (j = 0; j < ATH_STATS_LATENCY_BINS; j++) {
                        n_stats->ast_retry_delay[i][j] = htonl(stats->ast_retry_delay[i][j]);
                        n_stats->ast_queue_delay[i][j] = htonl(stats->ast_queue_delay[i][j]);
                    }
                }
#endif
#if UMAC_SUPPORT_VI_DBG
                for (i = 0; i < ATH_STATS_VI_LOG_LEN; i++) {
                    n_stats->vi_timestamp[i] = htonl(stats->vi_timestamp[i]);
                    n_stats->vi_rssi_ctl0[i] = stats->vi_rssi_ctl0[i];
                    n_stats->vi_rssi_ctl1[i] = stats->vi_rssi_ctl1[i];
                    n_stats->vi_rssi_ctl2[i] = stats->vi_rssi_ctl2[i];
                    n_stats->vi_rssi_ext0[i] = stats->vi_rssi_ext0[i];
                    n_stats->vi_rssi_ext1[i] = stats->vi_rssi_ext1[i];
                    n_stats->vi_rssi_ext2[i] = stats->vi_rssi_ext2[i];
                    n_stats->vi_rssi[i] = stats->vi_rssi[i];
                    n_stats->vi_evm0[i] = stats->vi_evm0[i];
                    n_stats->vi_evm1[i] = stats->vi_evm1[i];
                    n_stats->vi_evm2[i] = stats->vi_evm2[i];
                    n_stats->vi_rs_rate[i] = stats->vi_rs_rate[i];
                    n_stats->vi_tx_frame_cnt[i] = htonl(stats->vi_tx_frame_cnt[i]);
                    n_stats->vi_rx_frame_cnt[i] = htonl(stats->vi_rx_frame_cnt[i]);
                    n_stats->vi_rx_clr_cnt[i] = htonl(stats->vi_rx_clr_cnt[i]);
                    n_stats->vi_rx_ext_clr_cnt[i] = htonl(stats->vi_rx_ext_clr_cnt[i]);
                    n_stats->vi_cycle_cnt[i] = htonl(stats->vi_cycle_cnt[i]);
                }
                n_stats->vi_stats_index = stats->vi_stats_index;
#endif
#ifdef ATH_SUPPORT_TxBF
                n_stats->ast_txbf = stats->ast_txbf;
                n_stats->ast_lastratecode = stats->ast_lastratecode;
                n_stats->ast_sounding_count = htonl(stats->ast_sounding_count);
                n_stats->ast_txbf_rpt_count = htonl(stats->ast_txbf_rpt_count);
                for (i = 0; i <= MCS_RATE; i++)
                    n_stats->ast_mcs_count[i] = htonl(stats->ast_mcs_count[i]);
#endif
#if ATH_RX_LOOPLIMIT_TIMER
                n_stats->ast_rx_looplimit_start = htonl(stats->ast_rx_looplimit_start);
                n_stats->ast_rx_looplimit_end = htonl(stats->ast_rx_looplimit_end);
#endif
                n_stats->ast_chan_clr_cnt = htonl(stats->ast_chan_clr_cnt);
                n_stats->ast_cycle_cnt = htonl(stats->ast_cycle_cnt);
                n_stats->ast_noise_floor = htons(stats->ast_noise_floor);
                n_stats->ast_mib_stats.ast_ackrcv_bad = htonl(stats->ast_mib_stats.ast_ackrcv_bad);
                n_stats->ast_mib_stats.ast_rts_bad = htonl(stats->ast_mib_stats.ast_rts_bad);
                n_stats->ast_mib_stats.ast_rts_good = htonl(stats->ast_mib_stats.ast_rts_good);
                n_stats->ast_mib_stats.ast_fcs_bad = htonl(stats->ast_mib_stats.ast_fcs_bad);
                n_stats->ast_mib_stats.ast_beacons = htonl(stats->ast_mib_stats.ast_beacons);
#ifdef ATH_SUPPORT_HTC
                n_stats->ast_mib_stats.ast_tgt_stats.tx_shortretry = htonl(stats->ast_mib_stats.ast_tgt_stats.tx_shortretry);
                n_stats->ast_mib_stats.ast_tgt_stats.tx_longretry = htonl(stats->ast_mib_stats.ast_tgt_stats.tx_longretry);
                n_stats->ast_mib_stats.ast_tgt_stats.tx_xretries = htonl(stats->ast_mib_stats.ast_tgt_stats.tx_xretries);
                n_stats->ast_mib_stats.ast_tgt_stats.ht_tx_unaggr_xretry = htonl(stats->ast_mib_stats.ast_tgt_stats.ht_tx_unaggr_xretry);
                n_stats->ast_mib_stats.ast_tgt_stats.ht_tx_xretries = htonl(stats->ast_mib_stats.ast_tgt_stats.ht_tx_xretriesht_tx_xretries);
                n_stats->ast_mib_stats.ast_tgt_stats.tx_pkt = htonl(stats->ast_mib_stats.ast_tgt_stats.tx_pkt);
                n_stats->ast_mib_stats.ast_tgt_stats.tx_aggr = htonl(stats->ast_mib_stats.ast_tgt_stats.tx_aggr);
                n_stats->ast_mib_stats.ast_tgt_stats.tx_retry = htonl(stats->ast_mib_stats.ast_tgt_stats.tx_retry);
                n_stats->ast_mib_stats.ast_tgt_stats.txaggr_retry = htonl(stats->ast_mib_stats.ast_tgt_stats.txaggr_retry);
                n_stats->ast_mib_stats.ast_tgt_stats.txaggr_sub_retry = htonl(stats->ast_mib_stats.ast_tgt_stats.txaggr_sub_retry);
#endif
                for (i = 0; i < MAX_BB_PANICS; i++) {
                    n_stats->ast_bb_panic[i].valid = htonl(stats->ast_bb_panic[i].valid);
                    n_stats->ast_bb_panic[i].status = htonl(stats->ast_bb_panic[i].status);
                    n_stats->ast_bb_panic[i].tsf = htonl(stats->ast_bb_panic[i].tsf);
                    n_stats->ast_bb_panic[i].phy_panic_wd_ctl1 = htonl(stats->ast_bb_panic[i].phy_panic_wd_ctl1);
                    n_stats->ast_bb_panic[i].phy_panic_wd_ctl2 = htonl(stats->ast_bb_panic[i].phy_panic_wd_ctl2);
                    n_stats->ast_bb_panic[i].phy_gen_ctrl = htonl(stats->ast_bb_panic[i].phy_gen_ctrl);
                    n_stats->ast_bb_panic[i].rxc_pcnt = htonl(stats->ast_bb_panic[i].rxc_pcnt);
                    n_stats->ast_bb_panic[i].rxf_pcnt = htonl(stats->ast_bb_panic[i].rxf_pcnt);
                    n_stats->ast_bb_panic[i].txf_pcnt = htonl(stats->ast_bb_panic[i].txf_pcnt);
                    n_stats->ast_bb_panic[i].cycles = htonl(stats->ast_bb_panic[i].cycles);
                    n_stats->ast_bb_panic[i].wd = htonl(stats->ast_bb_panic[i].wd);
                    n_stats->ast_bb_panic[i].det = htonl(stats->ast_bb_panic[i].det);
                    n_stats->ast_bb_panic[i].rdar = htonl(stats->ast_bb_panic[i].rdar);
                    n_stats->ast_bb_panic[i].r_odfm = htonl(stats->ast_bb_panic[i].r_odfm);
                    n_stats->ast_bb_panic[i].r_cck = htonl(stats->ast_bb_panic[i].r_cck);
                    n_stats->ast_bb_panic[i].t_odfm = htonl(stats->ast_bb_panic[i].t_odfm);
                    n_stats->ast_bb_panic[i].t_cck = htonl(stats->ast_bb_panic[i].t_cck);
                    n_stats->ast_bb_panic[i].agc = htonl(stats->ast_bb_panic[i].agc);
                    n_stats->ast_bb_panic[i].src = htonl(stats->ast_bb_panic[i].src);
                }
            }
            break;
            case 1:
            {
                a_uint32_t i;
                acfg_ath_stats_11ac_t *stats = (acfg_ath_stats_11ac_t *)(ath_stats.address);
                atd_ath_stats_11ac_t *ac_stats = (atd_ath_stats_11ac_t *)((a_uint8_t *)resp + sizeof(atd_ath_stats_t));
                athstats->size = htonl(sizeof(atd_ath_stats_11ac_t));

                ac_stats->txrx_stats_level = htonl(stats->txrx_stats_level);
#ifndef BIG_ENDIAN_HOST
                ac_stats->txrx_stats.tx.from_stack.pkts = cpu_to_be64(stats->txrx_stats.tx.from_stack.pkts);
                ac_stats->txrx_stats.tx.from_stack.bytes = cpu_to_be64(stats->txrx_stats.tx.from_stack.bytes);
                ac_stats->txrx_stats.tx.delivered.pkts = cpu_to_be64(stats->txrx_stats.tx.delivered.pkts);
                ac_stats->txrx_stats.tx.delivered.bytes = cpu_to_be64(stats->txrx_stats.tx.delivered.bytes);
                ac_stats->txrx_stats.tx.dropped.host_reject.pkts = cpu_to_be64(stats->txrx_stats.tx.dropped.host_reject.pkts);
                ac_stats->txrx_stats.tx.dropped.host_reject.bytes = cpu_to_be64(stats->txrx_stats.tx.dropped.host_reject.bytes);
                ac_stats->txrx_stats.tx.dropped.download_fail.pkts = cpu_to_be64(stats->txrx_stats.tx.dropped.download_fail.pkts);
                ac_stats->txrx_stats.tx.dropped.download_fail.bytes = cpu_to_be64(stats->txrx_stats.tx.dropped.download_fail.bytes);
                ac_stats->txrx_stats.tx.dropped.target_discard.pkts = cpu_to_be64(stats->txrx_stats.tx.dropped.target_discard.pkts);
                ac_stats->txrx_stats.tx.dropped.target_discard.bytes = cpu_to_be64(stats->txrx_stats.tx.dropped.target_discard.bytes);
                ac_stats->txrx_stats.tx.dropped.no_ack.pkts = cpu_to_be64(stats->txrx_stats.tx.dropped.no_ack.pkts);
                ac_stats->txrx_stats.tx.dropped.no_ack.bytes = cpu_to_be64(stats->txrx_stats.tx.dropped.no_ack.bytes);
#else
                ac_stats->txrx_stats.tx.from_stack.pkts = htonl(stats->txrx_stats.tx.from_stack.pkts);
                ac_stats->txrx_stats.tx.from_stack.bytes = htonl(stats->txrx_stats.tx.from_stack.bytes);
                ac_stats->txrx_stats.tx.delivered.pkts = htonl(stats->txrx_stats.tx.delivered.pkts);
                ac_stats->txrx_stats.tx.delivered.bytes = htonl(stats->txrx_stats.tx.delivered.bytes);
                ac_stats->txrx_stats.tx.dropped.host_reject.pkts = htonl(stats->txrx_stats.tx.dropped.host_reject.pkts);
                ac_stats->txrx_stats.tx.dropped.host_reject.bytes = htonl(stats->txrx_stats.tx.dropped.host_reject.bytes);
                ac_stats->txrx_stats.tx.dropped.download_fail.pkts = htonl(stats->txrx_stats.tx.dropped.download_fail.pkts);
                ac_stats->txrx_stats.tx.dropped.download_fail.bytes = htonl(stats->txrx_stats.tx.dropped.download_fail.bytes);
                ac_stats->txrx_stats.tx.dropped.target_discard.pkts = htonl(stats->txrx_stats.tx.dropped.target_discard.pkts);
                ac_stats->txrx_stats.tx.dropped.target_discard.bytes = htonl(stats->txrx_stats.tx.dropped.target_discard.bytes);
                ac_stats->txrx_stats.tx.dropped.no_ack.pkts = htonl(stats->txrx_stats.tx.dropped.no_ack.pkts);
                ac_stats->txrx_stats.tx.dropped.no_ack.bytes = htonl(stats->txrx_stats.tx.dropped.no_ack.bytes);
#endif
                ac_stats->txrx_stats.tx.desc_in_use = htonl(stats->txrx_stats.tx.desc_in_use);
                ac_stats->txrx_stats.tx.desc_alloc_fails = htonl(stats->txrx_stats.tx.desc_alloc_fails);
                ac_stats->txrx_stats.tx.ce_ring_full = htonl(stats->txrx_stats.tx.ce_ring_full);
                ac_stats->txrx_stats.tx.dma_map_error = htonl(stats->txrx_stats.tx.dma_map_error);
#ifndef BIG_ENDIAN_HOST
                ac_stats->txrx_stats.tx.mgmt.pkts = cpu_to_be64(stats->txrx_stats.tx.mgmt.pkts);
                ac_stats->txrx_stats.tx.mgmt.bytes = cpu_to_be64(stats->txrx_stats.tx.mgmt.bytes);
                ac_stats->txrx_stats.rx.delivered.pkts = cpu_to_be64(stats->txrx_stats.rx.delivered.pkts);
                ac_stats->txrx_stats.rx.delivered.bytes = cpu_to_be64(stats->txrx_stats.rx.delivered.bytes);
                ac_stats->txrx_stats.rx.forwarded.pkts = cpu_to_be64(stats->txrx_stats.rx.forwarded.pkts);
                ac_stats->txrx_stats.rx.forwarded.bytes = cpu_to_be64(stats->txrx_stats.rx.forwarded.bytes);
#else
                ac_stats->txrx_stats.tx.mgmt.pkts = htonl(stats->txrx_stats.tx.mgmt.pkts);
                ac_stats->txrx_stats.tx.mgmt.bytes = htonl(stats->txrx_stats.tx.mgmt.bytes);
                ac_stats->txrx_stats.rx.delivered.pkts = htonl(stats->txrx_stats.rx.delivered.pkts);
                ac_stats->txrx_stats.rx.delivered.bytes = htonl(stats->txrx_stats.rx.delivered.bytes);
                ac_stats->txrx_stats.rx.forwarded.pkts = htonl(stats->txrx_stats.rx.forwarded.pkts);
                ac_stats->txrx_stats.rx.forwarded.bytes = htonl(stats->txrx_stats.rx.forwarded.bytes);
#endif
                ac_stats->stats.tx.comp_queued = htonl(stats->stats.tx.comp_queued);
                ac_stats->stats.tx.comp_delivered = htonl(stats->stats.tx.comp_delivered);
                ac_stats->stats.tx.msdu_enqued = htonl(stats->stats.tx.msdu_enqued);
                ac_stats->stats.tx.mpdu_enqued = htonl(stats->stats.tx.mpdu_enqued);
                ac_stats->stats.tx.wmm_drop = htonl(stats->stats.tx.wmm_drop);
                ac_stats->stats.tx.local_enqued = htonl(stats->stats.tx.local_enqued);
                ac_stats->stats.tx.local_freed = htonl(stats->stats.tx.local_freed);
                ac_stats->stats.tx.hw_queued = htonl(stats->stats.tx.hw_queued);
                ac_stats->stats.tx.hw_reaped = htonl(stats->stats.tx.hw_reaped);
                ac_stats->stats.tx.underrun = htonl(stats->stats.tx.underrun);
                ac_stats->stats.tx.tx_abort = htonl(stats->stats.tx.tx_abort);
                ac_stats->stats.tx.mpdus_requed = htonl(stats->stats.tx.mpdus_requed);
                ac_stats->stats.tx.tx_ko = htonl(stats->stats.tx.tx_xretry);
                ac_stats->stats.tx.data_rc = htonl(stats->stats.tx.data_rc);
                ac_stats->stats.tx.self_triggers = htonl(stats->stats.tx.self_triggers);
                ac_stats->stats.tx.sw_retry_failure = htonl(stats->stats.tx.sw_retry_failure);
                ac_stats->stats.tx.illgl_rate_phy_err = htonl(stats->stats.tx.illgl_rate_phy_err);
                ac_stats->stats.tx.pdev_cont_xretry = htonl(stats->stats.tx.pdev_cont_xretry);
                ac_stats->stats.tx.pdev_tx_timeout = htonl(stats->stats.tx.pdev_tx_timeout);
                ac_stats->stats.tx.pdev_resets = htonl(stats->stats.tx.pdev_resets);
                ac_stats->stats.tx.stateless_tid_alloc_failure = htonl(stats->stats.tx.stateless_tid_alloc_failure);
                ac_stats->stats.tx.phy_underrun = htonl(stats->stats.tx.phy_underrun);
                ac_stats->stats.tx.txop_ovf = htonl(stats->stats.tx.txop_ovf);
                ac_stats->stats.rx.mid_ppdu_route_change = htonl(stats->stats.rx.mid_ppdu_route_change);
                ac_stats->stats.rx.status_rcvd = htonl(stats->stats.rx.status_rcvd);
                ac_stats->stats.rx.r0_frags = htonl(stats->stats.rx.r0_frags);
                ac_stats->stats.rx.r1_frags = htonl(stats->stats.rx.r1_frags);
                ac_stats->stats.rx.r2_frags = htonl(stats->stats.rx.r2_frags);
                ac_stats->stats.rx.r3_frags = htonl(stats->stats.rx.r3_frags);
                ac_stats->stats.rx.htt_msdus = htonl(stats->stats.rx.htt_msdus);
                ac_stats->stats.rx.htt_mpdus = htonl(stats->stats.rx.htt_mpdus);
                ac_stats->stats.rx.loc_msdus = htonl(stats->stats.rx.loc_msdus);
                ac_stats->stats.rx.loc_mpdus = htonl(stats->stats.rx.loc_mpdus);
                ac_stats->stats.rx.oversize_amsdu = htonl(stats->stats.rx.oversize_amsdu);
                ac_stats->stats.rx.phy_errs = htonl(stats->stats.rx.phy_errs);
                ac_stats->stats.rx.phy_err_drop = htonl(stats->stats.rx.phy_err_drop);
                ac_stats->stats.rx.mpdu_errs = htonl(stats->stats.rx.mpdu_errs);
                ac_stats->stats.mem.dram_remain = htonl(stats->stats.mem.dram_free_size);
                ac_stats->stats.mem.iram_remain = htonl(stats->stats.mem.iram_free_size);
                ac_stats->stats.peer.dummy = htonl(stats->stats.peer.dummy);
                ac_stats->interface_stats.tx_beacon = cpu_to_be64(stats->interface_stats.tx_beacon);
                ac_stats->interface_stats.be_nobuf = htonl(stats->interface_stats.be_nobuf);
                ac_stats->interface_stats.tx_buf_count = htonl(stats->interface_stats.tx_buf_count);
                ac_stats->interface_stats.tx_packets = htonl(stats->interface_stats.tx_packets);
                ac_stats->interface_stats.rx_packets = htonl(stats->interface_stats.rx_packets);
                ac_stats->interface_stats.tx_mgmt = htonl(stats->interface_stats.tx_mgmt);
                ac_stats->interface_stats.tx_num_data = htonl(stats->interface_stats.tx_num_data);
                ac_stats->interface_stats.rx_num_data = htonl(stats->interface_stats.rx_num_data);
                ac_stats->interface_stats.rx_mgmt = htonl(stats->interface_stats.rx_mgmt);
                ac_stats->interface_stats.rx_num_mgmt = htonl(stats->interface_stats.rx_num_mgmt);
                ac_stats->interface_stats.rx_num_ctl = htonl(stats->interface_stats.rx_num_ctl);
                ac_stats->interface_stats.tx_rssi = htonl(stats->interface_stats.tx_rssi);
                for (i = 0; i < 10; i++) {
                    ac_stats->interface_stats.tx_mcs[i] = htonl(stats->interface_stats.tx_mcs[i]);
                    ac_stats->interface_stats.rx_mcs[i] = htonl(stats->interface_stats.rx_mcs[i]);
                }
                ac_stats->interface_stats.rx_rssi_comb = htonl(stats->interface_stats.rx_rssi_comb);
                ac_stats->interface_stats.rx_rssi_chain0.rx_rssi_pri20 = stats->interface_stats.rx_rssi_chain0.rx_rssi_pri20;
                ac_stats->interface_stats.rx_rssi_chain0.rx_rssi_sec20 = stats->interface_stats.rx_rssi_chain0.rx_rssi_sec20;
                ac_stats->interface_stats.rx_rssi_chain0.rx_rssi_sec40 = stats->interface_stats.rx_rssi_chain0.rx_rssi_sec40;
                ac_stats->interface_stats.rx_rssi_chain0.rx_rssi_sec80 = stats->interface_stats.rx_rssi_chain0.rx_rssi_sec80;
                ac_stats->interface_stats.rx_rssi_chain1.rx_rssi_pri20 = stats->interface_stats.rx_rssi_chain1.rx_rssi_pri20;
                ac_stats->interface_stats.rx_rssi_chain1.rx_rssi_sec20 = stats->interface_stats.rx_rssi_chain1.rx_rssi_sec20;
                ac_stats->interface_stats.rx_rssi_chain1.rx_rssi_sec40 = stats->interface_stats.rx_rssi_chain1.rx_rssi_sec40;
                ac_stats->interface_stats.rx_rssi_chain1.rx_rssi_sec80 = stats->interface_stats.rx_rssi_chain1.rx_rssi_sec80;
                ac_stats->interface_stats.rx_rssi_chain2.rx_rssi_pri20 = stats->interface_stats.rx_rssi_chain2.rx_rssi_pri20;
                ac_stats->interface_stats.rx_rssi_chain2.rx_rssi_sec20 = stats->interface_stats.rx_rssi_chain2.rx_rssi_sec20;
                ac_stats->interface_stats.rx_rssi_chain2.rx_rssi_sec40 = stats->interface_stats.rx_rssi_chain2.rx_rssi_sec40;
                ac_stats->interface_stats.rx_rssi_chain2.rx_rssi_sec80 = stats->interface_stats.rx_rssi_chain2.rx_rssi_sec80;
                ac_stats->interface_stats.rx_rssi_chain3.rx_rssi_pri20 = stats->interface_stats.rx_rssi_chain3.rx_rssi_pri20;
                ac_stats->interface_stats.rx_rssi_chain3.rx_rssi_sec20 = stats->interface_stats.rx_rssi_chain3.rx_rssi_sec20;
                ac_stats->interface_stats.rx_rssi_chain3.rx_rssi_sec40 = stats->interface_stats.rx_rssi_chain3.rx_rssi_sec40;
                ac_stats->interface_stats.rx_rssi_chain3.rx_rssi_sec80 = stats->interface_stats.rx_rssi_chain3.rx_rssi_sec80;
                ac_stats->interface_stats.rx_bytes = cpu_to_be64(stats->interface_stats.rx_bytes);
                ac_stats->interface_stats.tx_bytes = cpu_to_be64(stats->interface_stats.tx_bytes);
                ac_stats->interface_stats.tx_compaggr = htonl(stats->interface_stats.tx_compaggr);
                ac_stats->interface_stats.rx_aggr = htonl(stats->interface_stats.rx_aggr);
                ac_stats->interface_stats.tx_bawadv = htonl(stats->interface_stats.tx_bawadv);
                ac_stats->interface_stats.tx_compunaggr = htonl(stats->interface_stats.tx_compunaggr);
                ac_stats->interface_stats.rx_overrun = htonl(stats->interface_stats.rx_overrun);
                ac_stats->interface_stats.rx_badcrypt = htonl(stats->interface_stats.rx_badcrypt);
                ac_stats->interface_stats.rx_badmic = htonl(stats->interface_stats.rx_badmic);
                ac_stats->interface_stats.rx_crcerr = htonl(stats->interface_stats.rx_crcerr);
                ac_stats->interface_stats.rx_phyerr = htonl(stats->interface_stats.rx_phyerr);
                ac_stats->interface_stats.ackRcvBad = htonl(stats->interface_stats.ackRcvBad);
                ac_stats->interface_stats.rtsBad = htonl(stats->interface_stats.rtsBad);
                ac_stats->interface_stats.rtsGood = htonl(stats->interface_stats.rtsGood);
                ac_stats->interface_stats.fcsBad = htonl(stats->interface_stats.fcsBad);
                ac_stats->interface_stats.noBeacons = htonl(stats->interface_stats.noBeacons);
                ac_stats->interface_stats.mib_int_count = htonl(stats->interface_stats.mib_int_count);
                ac_stats->interface_stats.rx_looplimit_start = htonl(stats->interface_stats.rx_looplimit_start);
                ac_stats->interface_stats.rx_looplimit_end = htonl(stats->interface_stats.rx_looplimit_end);
                ac_stats->interface_stats.ap_stats_tx_cal_enable = stats->interface_stats.ap_stats_tx_cal_enable;
            }
            break;
        }
    }

fail:
    wmi_cmd_rsp(atsc->wmi_handle, cmdid, status, resp, resp_size);

    if (ath_stats.address)
       kfree(ath_stats.address);
    if (resp)
        kfree(resp);
 
    atd_trace(ATD_DEBUG_FUNCTRACE,(" End"));
}

static void
atd_tgt_clr_ath_stats(void *ctx, a_uint16_t cmdid,
                  a_uint8_t *buffer, a_int32_t len)
{
    a_status_t status      = A_STATUS_OK;
    atd_tgt_dev_t *atsc    = (atd_tgt_dev_t *)ctx;
    atd_tgt_wifi_t *wifisc = NULL;
    struct ifreq ifr;
    atd_hdr_t   *atdhdr;
    
    atd_trace(ATD_DEBUG_FUNCTRACE,(" cmd %d ", cmdid));

    atd_assert(len >= sizeof(atd_hdr_t));

    atdhdr  = (atd_hdr_t *) buffer;

    atd_trace(ATD_DEBUG_FUNCTRACE, (" radio %d ", \
                          atdhdr->wifi_index ));

    wifisc = atsc->wifi_list[atdhdr->wifi_index];
    if(wifisc == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    memset(&ifr, 0, sizeof(ifr));
    ifr.ifr_data = (void *) NULL;

    status = netdev_ioctl(wifisc->netdev, &ifr, LINUX_PVT_CLRATH_STATS);

    if(status != A_STATUS_OK){
        atd_trace(ATD_DEBUG_CFG, ("athstats clear error \n"));
    }

fail:
    wmi_cmd_rsp(atsc->wmi_handle, cmdid, status, NULL, 0);

    atd_trace(ATD_DEBUG_FUNCTRACE,(" End"));
}

static void 
atd_tgt_get_scanresults(void *ctx, a_uint16_t cmdid, 
                  a_uint8_t *buffer, a_int32_t len)
{
    a_status_t status      = A_STATUS_OK;
    atd_tgt_dev_t *atsc    = (atd_tgt_dev_t *)ctx;
    atd_tgt_wifi_t *wifisc = NULL;
    atd_tgt_vap_t  *vap    = NULL;
    a_uint8_t      *results= NULL;
    a_uint8_t      *response= NULL;
    struct iwreq req;
    a_uint8_t *pos, *end;
    a_uint32_t count = 0, i;
    atd_scan_result_t *sl;

    atd_hdr_t   *atdhdr;
    atd_scan_t  *scan;

    atd_trace(ATD_DEBUG_FUNCTRACE,(" cmd %d ", \
                cmdid));

    /* host only sends hdr+len */
    atd_assert(len >= (sizeof(atd_hdr_t) + sizeof(a_uint32_t)));

    atdhdr  = (atd_hdr_t *) buffer;
    scan    = (atd_scan_t  *) (buffer + sizeof(atd_hdr_t)) ;

    scan->len = ntohl(scan->len);

    memset(&req, 0, sizeof(struct iwreq));

    wifisc = atsc->wifi_list[atdhdr->wifi_index];
    if(wifisc == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    vap    = wifisc->vap_list[atdhdr->vap_index];
    if(vap == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    results = kzalloc(scan->len, GFP_KERNEL);
    response = kzalloc(scan->len, GFP_KERNEL);

    if(results == NULL || response == NULL) {
        status = A_STATUS_ENOMEM;
        goto fail;
    }

    req.u.data.pointer = (a_uint8_t *)results;
    req.u.data.length  = scan->len;
    req.u.data.flags   = 0;

#define IEEE80211_IOCTL_SCAN_RESULTS (SIOCDEVPRIVATE+9)

    status = netdev_ioctl(vap->vap_netdev,(struct ifreq *)&req,
                          IEEE80211_IOCTL_SCAN_RESULTS);

    if(status != A_STATUS_OK){
        atd_trace(ATD_DEBUG_CFG, ("scan results fetch error \n"));
    } else {
        pos = results;
        end = (results + req.u.data.length);
        sl = (atd_scan_result_t *)response;
        while (pos + sizeof(struct ieee80211req_scan_result) <= end) {
            struct ieee80211req_scan_result *sr;
            a_uint8_t *next;
        
            sr = (struct ieee80211req_scan_result *) pos;
            next = pos + sr->isr_len;
            if (next > end)
                break;
            pos += sizeof(*sr);
            if (pos + sr->isr_ssid_len + sr->isr_ie_len > end)
                break;
           
            sl->isr_len = htons(sr->isr_len - sizeof(*sr));
            sl->isr_freq = htons(sr->isr_freq);
            sl->isr_flags = htonl(sr->isr_flags);
            sl->isr_noise = sr->isr_noise;
            sl->isr_rssi = sr->isr_rssi;
            sl->isr_intval = sr->isr_intval;
            sl->isr_capinfo = htons(sr->isr_capinfo);
            sl->isr_erp = sr->isr_erp;
            for (i = 0; i < ACFG_MACADDR_LEN; i++)
                sl->isr_bssid[i] = sr->isr_bssid[i];
            sl->isr_nrates = sr->isr_nrates;
            for (i = 0; i < 36; i++)
                sl->isr_rates[i] = sr->isr_rates[i];
            sl->isr_ssid_len = sr->isr_ssid_len;
            sl->isr_ie_len = htons(sr->isr_ie_len);    
            memcpy(sl + 1, sr + 1, sr->isr_len - sizeof(*sr));

            sl = (atd_scan_result_t *)((u_int8_t *)(sl + 1) 
                                       + sr->isr_len - sizeof(*sr));
            count += sizeof(*sl) + sr->isr_len - sizeof(*sr);
            pos = next;
        }
    }
fail:
    wmi_cmd_rsp(atsc->wmi_handle, cmdid, status,
            response, count);

    if (response)
        kfree(response);
    if (results)
        kfree(results);

    atd_trace(ATD_DEBUG_FUNCTRACE,(" End"));
}

/** 
 * @brief Get Scanlist space from Newma
 * 
 * @param ctx
 * @param cmdid
 * @param seqno
 * @param buffer
 * @param len
 */
static void 
atd_tgt_get_scan_space(void *ctx, a_uint16_t cmdid, 
                  a_uint8_t *buffer, a_int32_t len)
{
    a_status_t status      = A_STATUS_OK;
    atd_tgt_dev_t *atsc    = (atd_tgt_dev_t *)ctx;
    atd_tgt_wifi_t *wifisc = NULL;
    atd_tgt_vap_t  *vap    = NULL;
    a_uint32_t rlen        = 0;
    a_uint8_t *param       = NULL;
    struct iwreq req;

    atd_hdr_t  *atdhdr;


    atd_trace(ATD_DEBUG_FUNCTRACE,(" Start"));

    atd_assert(len >= sizeof(atd_hdr_t));

    atdhdr  = (atd_hdr_t *) buffer;

    memset(&req, 0, sizeof(struct iwreq));
    param   = kzalloc(2 * sizeof(a_uint32_t), GFP_KERNEL);
    if(param == NULL ){
        status = A_STATUS_ENXIO;
        goto fail ;
    }

    req.u.data.pointer  = (a_uint8_t *)param;
    req.u.data.length   = 0;

    wifisc = atsc->wifi_list[atdhdr->wifi_index];
    if(wifisc == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    vap    = wifisc->vap_list[atdhdr->vap_index];
    if(vap == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }
#define IOCTL_GET_SCAN_SPACE (SIOCDEVPRIVATE + 16)
    status = netdev_ioctl(vap->vap_netdev,(struct ifreq *)&req,IOCTL_GET_SCAN_SPACE);
    if(status != A_STATUS_OK){
        atd_trace(ATD_DEBUG_CFG, ("get scan space error \n"));
    }

    rlen = req.u.data.length;
    rlen =  htonl(rlen);

fail:
    wmi_cmd_rsp(atsc->wmi_handle, cmdid, status, (a_uint8_t *)&rlen, 
            sizeof(a_uint32_t));

    kfree(param);

    atd_trace(ATD_DEBUG_FUNCTRACE,(" End"));
}


/** 
 * @brief 
 * 
 * @param ctx
 * @param cmdid
 * @param buffer
 * @param len
 */
static void 
atd_tgt_get_range (void *ctx, a_uint16_t cmdid, 
                   a_uint8_t *buffer, a_int32_t len)
{
    a_status_t      status = A_STATUS_OK;
    atd_tgt_dev_t  *atsc   = (atd_tgt_dev_t *)ctx;
    atd_tgt_wifi_t *wifisc = NULL;
    atd_tgt_vap_t  *vap    = NULL;
    atd_hdr_t      *atdhdr;
    struct iw_range *tmp;
    a_uint8_t       *rangebuf = NULL;
    struct iw_request_info info;
    union  iwreq_data iwreq;
    atd_range_buf_t *resp = NULL;
    a_uint32_t i, l = 0;

    atd_trace(ATD_DEBUG_FUNCTRACE, ("Start..."));

    atd_assert(len >= sizeof(atd_hdr_t));

    rangebuf = kmalloc(sizeof(struct iw_range), GFP_ATOMIC);
    if (rangebuf == NULL) {
        printk("\n %s : allocation failed\n", __FUNCTION__);
        status = A_STATUS_ENOMEM;
        goto fail;
    }
    memset(rangebuf, 0, sizeof(struct iw_range));

    tmp = (struct iw_range *)rangebuf;
  
    atdhdr      = (atd_hdr_t  *)  buffer;
    wifisc = atsc->wifi_list[atdhdr->wifi_index];
    if(wifisc == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    vap    = wifisc->vap_list[atdhdr->vap_index];
    if(vap == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    resp = kmalloc(sizeof(atd_range_buf_t), GFP_ATOMIC);
    if (resp == NULL) {
        printk("\n %s : allocation failed\n", __FUNCTION__);
        status = A_STATUS_ENOMEM;
        goto fail;
    }
    memset(resp, 0, sizeof(atd_range_buf_t));

    l = sizeof(atd_range_buf_t);

    status = netdev_wext_handler(vap->vap_netdev, SIOCGIWRANGE, &info, 
                                 (void *)&iwreq, (a_uint8_t *)rangebuf); 

    if(status != A_STATUS_OK) {
        atd_trace(ATD_DEBUG_CFG, ("Range Get error \n"));
    } else {
        resp->throughput = htonl(tmp->throughput);
        resp->min_nwid = htonl(tmp->min_nwid);
        resp->min_nwid = htonl(tmp->min_nwid);
        resp->old_num_channels = htons(tmp->old_num_channels);
        resp->old_num_frequency = tmp->old_num_frequency;
#if WIRELESS_EXT >= 17
        for (i = 0; i < 6; i++)
            resp->event_capa[i] = htonl(tmp->event_capa[i]);
#endif
        resp->sensitivity = htonl(tmp->sensitivity);
        resp->max_qual.qual = tmp->max_qual.qual;
        resp->max_qual.level = tmp->max_qual.level;
        resp->max_qual.noise = tmp->max_qual.noise;
        resp->max_qual.updated = tmp->max_qual.updated;
        resp->avg_qual.qual = tmp->avg_qual.qual;
        resp->avg_qual.level = tmp->avg_qual.level;
        resp->avg_qual.noise = tmp->avg_qual.noise;
        resp->avg_qual.updated = tmp->avg_qual.updated;
        resp->num_bitrates = tmp->num_bitrates;
        for (i = 0; i < ACFG_MAX_BITRATE; i++)
            resp->bitrate[i] = htonl(tmp->bitrate[i]);
        resp->min_rts = htonl(tmp->min_rts);
        resp->max_rts = htonl(tmp->max_rts);
        resp->min_frag = htonl(tmp->min_frag);
        resp->max_frag = htonl(tmp->max_frag);
        resp->min_pmp = htonl(tmp->min_pmp);
        resp->max_pmp = htonl(tmp->max_pmp);
        resp->min_pmt = htonl(tmp->min_pmt);
        resp->max_pmt = htonl(tmp->max_pmt);
        resp->pmp_flags = htons(tmp->pmp_flags);
        resp->pmt_flags = htons(tmp->pmt_flags);
        resp->pm_capa = htons(tmp->pm_capa);
        for (i = 0; i < ACFG_MAX_ENCODING; i++)
            resp->encoding_size[i] = htons(tmp->encoding_size[i]);
        resp->num_encoding_sizes = tmp->num_encoding_sizes;
        resp->max_encoding_tokens = tmp->max_encoding_tokens;
        resp->encoding_login_index = tmp->encoding_login_index;
        resp->txpower_capa = htons(tmp->txpower_capa);
        resp->num_txpower = tmp->num_txpower;
#if WIRELESS_EXT >= 10
        for (i = 0; i < ACFG_MAX_TXPOWER; i++)
            resp->txpower[i] = htonl(tmp->txpower[i]);
#endif
        resp->we_version_compiled = tmp->we_version_compiled;
        resp->we_version_source = tmp->we_version_source;
        resp->retry_capa = htons(tmp->retry_capa);
        resp->retry_flags = htons(tmp->retry_flags);
        resp->r_time_flags = htons(tmp->r_time_flags);
        resp->min_retry = htonl(tmp->min_retry);
        resp->max_retry = htonl(tmp->max_retry);
        resp->min_r_time = htonl(tmp->min_r_time);
        resp->max_r_time = htonl(tmp->max_r_time);
        resp->num_channels = htons(tmp->num_channels);
        resp->num_frequency = tmp->num_frequency;
        for (i = 0; i < ACFG_MAX_FREQ; i++) {
            resp->freq[i].m = htonl(tmp->freq[i].m);
            resp->freq[i].e = htons(tmp->freq[i].e);
            resp->freq[i].i = tmp->freq[i].i;
            resp->freq[i].flags = tmp->freq[i].flags;
        }
#if WIRELESS_EXT >= 18
        resp->enc_capa = htonl(tmp->enc_capa);
#endif
    }

fail:
    wmi_cmd_rsp(atsc->wmi_handle, cmdid, status, (a_uint8_t *)resp, l);

    if (resp)
        kfree(resp);
    if (rangebuf)
        kfree(rangebuf);
    
    atd_trace(ATD_DEBUG_FUNCTRACE ,(" End"));
}



/** 
 * @brief 
 * 
 * @param ctx
 * @param cmdid
 * @param buffer
 * @param len
 */
void
atd_tgt_set_opmode(void *ctx, a_uint16_t cmdid,
                          a_uint8_t *buffer, a_int32_t len)
{
    a_status_t status = A_STATUS_OK;
    atd_tgt_dev_t *atsc = (atd_tgt_dev_t *)ctx;
    atd_tgt_wifi_t *wifisc =NULL;
    atd_tgt_vap_t * vap;
    union  iwreq_data iwreqdata;
    struct iw_request_info info;
    atd_hdr_t  *atdhdr;
    a_uint32_t *ptr_mode ;
    
    atd_trace((ATD_DEBUG_FUNCTRACE|ATD_DEBUG_CFG) , ("cmd %d ", cmdid));

    atd_assert(len >= (sizeof(atd_hdr_t) + sizeof(a_uint32_t)));

    atdhdr  = (atd_hdr_t *) buffer;
    ptr_mode = (a_uint32_t *)(buffer + sizeof(atd_hdr_t)) ;

    wifisc = atsc->wifi_list[atdhdr->wifi_index];
    if(wifisc == NULL) {
        status = A_STATUS_ENXIO;
        goto fail;
    }

    vap = wifisc->vap_list[atdhdr->vap_index];
    if(vap == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    info.cmd = SIOCSIWMODE ;
    info.flags = 0 ;
    iwreqdata.mode = ntohl(*ptr_mode);

    /* 
     * Convert ACFG_OPMODE_* to IW_MODE_* 
     */
    switch(iwreqdata.mode)
    {
        case ACFG_OPMODE_IBSS: iwreqdata.mode = IW_MODE_ADHOC; break;
        case ACFG_OPMODE_STA: iwreqdata.mode = IW_MODE_INFRA; break;
        case ACFG_OPMODE_HOSTAP: iwreqdata.mode = IW_MODE_MASTER; break;
        case ACFG_OPMODE_MONITOR: iwreqdata.mode = IW_MODE_MONITOR; break;
        default: iwreqdata.mode = IW_MODE_AUTO; break;
    }
    
    atd_trace((ATD_DEBUG_FUNCTRACE|ATD_DEBUG_CFG), \
                                        ("opmode %d ", iwreqdata.mode));

    status = netdev_wext_handler(vap->vap_netdev, SIOCSIWMODE, &info,
                                                 &iwreqdata, NULL);
    if(status != A_STATUS_OK) {
        atd_trace(ATD_DEBUG_CFG, ("opmode set error \n"));
    }

fail:
    wmi_cmd_rsp(atsc->wmi_handle, cmdid, status, NULL, 0);
}

                                        
/** 
 * @brief 
 * 
 * @param ctx
 * @param cmdid
 * @param buffer
 * @param len
 */
static void
atd_tgt_set_freq (void *ctx, a_uint16_t cmdid,
                           a_uint8_t *buffer, a_int32_t len)
{
    a_status_t      status = A_STATUS_OK;
    atd_tgt_dev_t  *atsc   = (atd_tgt_dev_t *)ctx;
    atd_tgt_wifi_t *wifisc = NULL;
    atd_tgt_vap_t  *vap    = NULL;
    atd_hdr_t      *atdhdr;
    a_uint32_t resp ;
    acfg_freq_t *freq = NULL;
    struct iw_request_info info;
    union  iwreq_data iwreq;

    atd_trace(ATD_DEBUG_FUNCTRACE, ("Start..."));

    atd_assert(len >= (sizeof(atd_hdr_t) + sizeof(acfg_freq_t)));

    atdhdr      = (atd_hdr_t  *)  buffer;
    freq = (acfg_freq_t *)(buffer + sizeof(atd_hdr_t)) ;

    wifisc = atsc->wifi_list[atdhdr->wifi_index];
    if(wifisc == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    vap = wifisc->vap_list[atdhdr->vap_index];
    if(vap == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    info.cmd = SIOCSIWFREQ ;
    info.flags = 0 ;
    iwreq.freq.m = ntohl(freq->m);
    iwreq.freq.e = ntohs(freq->e);
    iwreq.freq.i = 0;
    iwreq.freq.flags = IW_FREQ_FIXED ;

    /* Bring the exponent down to 1 */
    while(iwreq.freq.e > 1)
    {
        iwreq.freq.m *= 10 ;
        iwreq.freq.e-- ;
    }

    atd_trace(ATD_DEBUG_CFG, ("%s(): cmdid=%d; m=%d e=%d\n",\
                 __FUNCTION__,cmdid,iwreq.freq.m,iwreq.freq.e));
    status = netdev_wext_handler(vap->vap_netdev, SIOCSIWFREQ, &info,
                                              (void *)&iwreq, NULL);
    resp = 0;
    if(status != A_STATUS_OK){
        resp = htonl(1) ;
        atd_trace(ATD_DEBUG_CFG, ("Frequency Set error \n"));
    }

fail:
    wmi_cmd_rsp(atsc->wmi_handle, cmdid, status, (a_uint8_t *)&resp, sizeof(resp));
    atd_trace(ATD_DEBUG_FUNCTRACE ,(" End"));
}


/** 
 * @brief 
 * 
 * @param ctx
 * @param cmdid
 * @param buffer
 * @param len
 */
static void
atd_tgt_set_phymode(void *ctx, a_uint16_t cmdid,
                           a_uint8_t *buffer, a_int32_t len)
{
    a_status_t      status = A_STATUS_OK;
    atd_tgt_dev_t  *atsc   = (atd_tgt_dev_t *)ctx;
    atd_tgt_wifi_t *wifisc = NULL;
    atd_tgt_vap_t  *vap    = NULL;
    atd_hdr_t      *atdhdr;
    struct iw_request_info info;
    union  iwreq_data iwreq;
    acfg_phymode_t *ptr_mode = NULL, mode;

    atd_trace(ATD_DEBUG_FUNCTRACE, ("Start..."));

    atd_assert(len >= (sizeof(atd_hdr_t) + sizeof(acfg_phymode_t)));

    atdhdr      = (atd_hdr_t  *)  buffer;
    ptr_mode = (acfg_phymode_t *)(buffer + sizeof(atd_hdr_t));
    mode = ntohl(*ptr_mode);

    wifisc = atsc->wifi_list[atdhdr->wifi_index];
    if(wifisc == NULL) {
        status = A_STATUS_ENXIO;
        goto fail;
    }

    vap = wifisc->vap_list[atdhdr->vap_index];
    if(vap == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    atd_trace(ATD_DEBUG_FUNCTRACE, ("%s(): cmdid=%d; phymode = %d, %s\n",\
                __FUNCTION__,cmdid,mode,phymode_strings[mode]) );

    info.cmd = IEEE80211_IOCTL_SETMODE ;
    info.flags = 0 ;
    iwreq.data.length = strlen(phymode_strings[mode]) + 1 ;
    iwreq.data.flags = 0 ;
    iwreq.data.pointer = (char *)phymode_strings[mode] ;

    status = netdev_wext_handler(vap->vap_netdev, \
       IEEE80211_IOCTL_SETMODE, &info,(void *)&iwreq, \
      (char *)phymode_strings[mode] );

    if(status != A_STATUS_OK){
        atd_trace(ATD_DEBUG_CFG, ("Phymode Set error \n"));
    }

fail:
    wmi_cmd_rsp(atsc->wmi_handle, cmdid, status, NULL, 0);
    atd_trace(ATD_DEBUG_FUNCTRACE ,(" End"));
}

/**
 * @brief
 *
 * @param ctx
 * @param cmdid
 * @param buffer
 * @param len
 */
static void
atd_tgt_get_phymode(void *ctx, a_uint16_t cmdid,
                           a_uint8_t *buffer, a_int32_t len)
{
    a_status_t      status = A_STATUS_OK;
    atd_tgt_dev_t  *atsc   = (atd_tgt_dev_t *)ctx;
    atd_tgt_wifi_t *wifisc = NULL;
    atd_tgt_vap_t  *vap    = NULL;
    atd_hdr_t      *atdhdr;
    struct iw_request_info info;
    union  iwreq_data iwreq;
    acfg_phymode_t mode = ACFG_PHYMODE_INVALID;
    char mode_str[50] = {0};

    atd_trace(ATD_DEBUG_FUNCTRACE, ("Start..."));

    atd_assert(len >= sizeof(atd_hdr_t));

    atdhdr = (atd_hdr_t  *)buffer;

    wifisc = atsc->wifi_list[atdhdr->wifi_index];
    if(wifisc == NULL) {
        status = A_STATUS_ENXIO;
        goto fail;
    }

    vap = wifisc->vap_list[atdhdr->vap_index];
    if(vap == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    atd_trace(ATD_DEBUG_FUNCTRACE, ("%s(): cmdid=%d \n",\
                __FUNCTION__,cmdid) );

    info.cmd = IEEE80211_IOCTL_GETMODE ;
    info.flags = 0 ;

    iwreq.data.length = sizeof(mode_str);
    iwreq.data.pointer = (char *)mode_str;
    iwreq.data.flags = 0 ;

    status = netdev_wext_handler(vap->vap_netdev, \
       IEEE80211_IOCTL_GETMODE, &info,(void *)&iwreq, \
      (char *)mode_str);

    if(status != A_STATUS_OK){
        atd_trace(ATD_DEBUG_CFG, ("Phymode Get error \n"));
    }

    if(mode_str[0] != '\0')
    {
        a_uint8_t i;
        for(i = 0; phymode_strings[i] != NULL; i++)
        {
            if(!strncmp(phymode_strings[i], mode_str, strlen(mode_str)))
                mode = i;
        }
    }

    mode = htonl(mode);

fail:
    wmi_cmd_rsp(atsc->wmi_handle, cmdid, status, (a_uint8_t *)&mode, sizeof(acfg_phymode_t));
    atd_trace(ATD_DEBUG_FUNCTRACE ,(" End"));
}

/** 
 * @brief Get info on associated stations
 * 
 * @param ctx
 * @param cmdid
 * @param buffer
 * @param len
 */
static void
atd_tgt_get_stainfo(void *ctx, a_uint16_t cmdid,
                           a_uint8_t *buffer, a_int32_t len)
{
    a_status_t      status = A_STATUS_OK;
    atd_tgt_dev_t  *atsc   = (atd_tgt_dev_t *)ctx;
    atd_tgt_wifi_t *wifisc = NULL;
    atd_tgt_vap_t  *vap    = NULL;
    atd_hdr_t      *atdhdr;
    struct iwreq    iwr;
    a_uint32_t      hostlen;
    a_uint32_t      i, count = 0;
    void           *infobuff = NULL, *respbuff = NULL;
    a_uint8_t      *pos, *end, *next;
    struct ieee80211req_sta_info *si;
    atd_stainfo_t *resp;

    atd_trace(ATD_DEBUG_FUNCTRACE, ("Start..."));

    atd_assert(len >= (sizeof(atd_hdr_t) + sizeof(a_uint32_t)));

    atdhdr = (atd_hdr_t  *)  buffer;
    hostlen = ntohl(*(a_uint32_t *)(buffer + sizeof(atd_hdr_t)));

    memset(&iwr, 0, sizeof(iwr));

    wifisc = atsc->wifi_list[atdhdr->wifi_index];
    if(wifisc == NULL) {
        status = A_STATUS_ENXIO;
        goto fail;
    }

    vap = wifisc->vap_list[atdhdr->vap_index];
    if(vap == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }
    
    atd_trace(ATD_DEBUG_CFG, ("%s(): hostlen = %d \n",__FUNCTION__,hostlen));

    infobuff = kzalloc(hostlen, GFP_KERNEL);
    if(!infobuff) {
        atd_trace(ATD_DEBUG_FUNCTRACE ,("Error - Nomem"));
        status = A_STATUS_ENOMEM;
        goto fail;
    }

    respbuff = kzalloc(hostlen, GFP_KERNEL);
    if(!respbuff) {
        atd_trace(ATD_DEBUG_FUNCTRACE ,("Error - Nomem"));
        status = A_STATUS_ENOMEM;
        goto fail;
    }

    iwr.u.data.length = hostlen - sizeof(a_uint32_t);
    iwr.u.data.pointer = (void *)((a_uint8_t *)infobuff + sizeof(a_uint32_t));

    status = netdev_ioctl(vap->vap_netdev,\
            (struct ifreq *)&iwr,LINUX_PVT_GETSTA_INFO);

    pos = (a_uint8_t *)infobuff + sizeof(a_uint32_t);
    end = (pos + iwr.u.data.length);
    resp = (atd_stainfo_t *)((a_uint8_t *)respbuff + sizeof(a_uint32_t));
    while (pos + sizeof(struct ieee80211req_sta_info) <= end) {
        si = (struct ieee80211req_sta_info *)pos;

        if (si->isi_len <= 0)
            break;
        next = (pos + si->isi_len);
        if (next > end)
            break;

        resp->isi_len = htons(si->isi_len - sizeof(*si));
        resp->isi_freq = htons(si->isi_freq);
        resp->isi_flags = htonl(si->isi_flags);
        resp->isi_state = htons(si->isi_state);
        resp->isi_authmode = si->isi_authmode;
        resp->isi_rssi = si->isi_rssi;
        resp->isi_capinfo = htons(si->isi_capinfo);
        resp->isi_athflags = si->isi_athflags;
        resp->isi_erp = si->isi_erp;
        resp->isi_ps = si->isi_ps;
        memcpy(resp->isi_macaddr, si->isi_macaddr, ACFG_MACADDR_LEN);
        resp->isi_nrates = si->isi_nrates;
        memcpy(resp->isi_rates, si->isi_rates, 36);
        resp->isi_txrate = si->isi_txrate;
        resp->isi_txratekbps = htonl(si->isi_txratekbps);
        resp->isi_ie_len = htons(si->isi_ie_len);
        resp->isi_associd = htons(si->isi_associd);
        resp->isi_txpower = htons(si->isi_txpower);
        resp->isi_vlan = htons(si->isi_vlan);
        for (i = 0 ; i < 17; i++)
            resp->isi_txseqs[i] = htons(si->isi_txseqs[i]);
        for (i = 0 ; i < 17; i++)
            resp->isi_rxseqs[i] = htons(si->isi_rxseqs[i]);
        resp->isi_inact = htons(si->isi_inact);
        resp->isi_uapsd = si->isi_uapsd;
        resp->isi_opmode = si->isi_opmode;
        resp->isi_cipher = si->isi_cipher;
        resp->isi_assoc_time = htonl(si->isi_assoc_time);
        resp->isi_tr069_assoc_time_sec = htonl(si->isi_tr069_assoc_time.tv_sec);
        resp->isi_tr069_assoc_time_nsec = htonl(si->isi_tr069_assoc_time.tv_nsec);
        resp->isi_htcap = htons(si->isi_htcap);
        resp->isi_rxratekbps = htonl(si->isi_rxratekbps);
        resp->isi_maxrate_per_client = si->isi_maxrate_per_client;
        resp->isi_stamode = htons(si->isi_stamode);
        memcpy(resp + 1, si + 1, si->isi_len - sizeof(*si));

        count += sizeof(*resp) + si->isi_len - sizeof(*si);
        resp = (atd_stainfo_t *)((a_uint8_t *)(resp + 1) 
                                 + (si->isi_len - sizeof(*si)));
        pos = next;
    }
    *(a_uint32_t *)respbuff = htonl(count);
    count += sizeof(a_uint32_t);

fail:
    wmi_cmd_rsp(atsc->wmi_handle, cmdid, status, respbuff, count);

    if (respbuff)
        kfree(respbuff);
    if (infobuff)
        kfree(infobuff);

    atd_trace(ATD_DEBUG_FUNCTRACE ,(" End"));
}


void 
atd_tgt_set_mlme(void *ctx, a_uint16_t cmdid, 
                  a_uint8_t *buffer, a_int32_t len)
{
    a_status_t status = A_STATUS_OK;
    atd_tgt_dev_t *atsc = (atd_tgt_dev_t *)ctx;
    atd_tgt_wifi_t *wifisc =NULL;
    atd_tgt_vap_t * vap;
    union  iwreq_data iwreqdata;
    struct iw_point *iwpdata  = &iwreqdata.data;
    struct iw_request_info info;
    atd_hdr_t  *atdhdr;
    atd_mlme_t *setmlme;
    struct ieee80211req_mlme *mlme = NULL;

    atd_trace(ATD_DEBUG_FUNCTRACE, ("cmd %d    ", cmdid));

    atd_assert(len >= (sizeof(atd_hdr_t) + sizeof(atd_mlme_t)));
    atdhdr = (atd_hdr_t *) buffer;
    setmlme = (atd_mlme_t *)(buffer + sizeof(atd_hdr_t));


    atd_trace(ATD_DEBUG_CFG, ("radio %d vap %d ", 
              atdhdr->wifi_index, atdhdr->vap_index));


    wifisc = atsc->wifi_list[atdhdr->wifi_index];

    if(wifisc == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    vap    = wifisc->vap_list[atdhdr->vap_index];

    if(vap == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }


    vzalloc(sizeof(struct ieee80211req_mlme), mlme);
    if(mlme == NULL){
        printk("\n %s : allocation failed\n", __FUNCTION__);
        status = A_STATUS_ENOMEM;
        goto fail; 
    }

    mlme->im_op = setmlme->op;
    mlme->im_ssid_len = setmlme->ssid_len;
    mlme->im_reason = ntohs(setmlme->reason);
    mlme->im_seq = ntohs(setmlme->seq);
    memcpy(mlme->im_macaddr, setmlme->macaddr, ACFG_MACADDR_LEN);
    memcpy(mlme->im_ssid, setmlme->ssid, ACFG_MAX_SSID_LEN);
    memcpy(mlme->im_optie, setmlme->optie, ACFG_MAX_IELEN * 2);
    mlme->im_optie_len = ntohs(setmlme->optie_len);

    info.cmd = IEEE80211_IOCTL_SETMLME;
    info.flags = 0;
    iwpdata->length = sizeof(struct ieee80211req_mlme);
    iwpdata->flags = 0;
    iwpdata->pointer = (char *)mlme;

    status = netdev_wext_handler(vap->vap_netdev, IEEE80211_IOCTL_SETMLME, &info, 
                                 &iwreqdata, (char *)mlme); 
    if(status != A_STATUS_OK){
        atd_trace(ATD_DEBUG_CFG, ("mlme set erro \n"));
    }

    vfree(mlme);

fail:
    wmi_cmd_rsp(atsc->wmi_handle, cmdid, status, NULL, 0);
}



/** 
 * @brief Set Opt IE
 * 
 * @param ctx
 * @param cmdid
 * @param buffer
 * @param len
 */
static void
atd_tgt_set_optie(void *ctx, a_uint16_t cmdid,
                           a_uint8_t *buffer, a_int32_t len)
{
    a_status_t      status = A_STATUS_OK;
    atd_tgt_dev_t  *atsc   = (atd_tgt_dev_t *)ctx;
    atd_tgt_wifi_t *wifisc = NULL;
    atd_tgt_vap_t  *vap    = NULL;
    atd_hdr_t      *atdhdr;
    struct iw_request_info info;
    union  iwreq_data iwreq;
    a_uint32_t *plen = NULL ;
    a_uint32_t ielen ;

    atd_trace(ATD_DEBUG_FUNCTRACE, ("Start..."));

    atd_assert(len >= (sizeof(atd_hdr_t) + sizeof(a_uint32_t)));
    
    atdhdr      = (atd_hdr_t  *)  buffer;

    wifisc = atsc->wifi_list[atdhdr->wifi_index];
    if(wifisc == NULL) {
        status = A_STATUS_ENXIO;
        goto fail;
    }

    vap = wifisc->vap_list[atdhdr->vap_index];
    if(vap == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    plen = (a_uint32_t *)(buffer + sizeof(atd_hdr_t));
    ielen = ntohl(*plen);

    atd_trace(ATD_DEBUG_FUNCTRACE, ("%s(): cmdid=%d; IE len = %d \n",\
                __FUNCTION__,cmdid,ielen) );

    info.cmd = IEEE80211_IOCTL_SETOPTIE ;
    info.flags = 0 ;
    iwreq.data.length = ielen ;
    iwreq.data.flags = 0 ;
    iwreq.data.pointer = (a_uint8_t *)plen + sizeof(a_uint32_t);

    status = netdev_wext_handler(vap->vap_netdev, \
       IEEE80211_IOCTL_SETOPTIE, &info,(void *)&iwreq, \
          iwreq.data.pointer );

    if(status != A_STATUS_OK){
        atd_trace(ATD_DEBUG_CFG, ("Setoptie error \n"));
    }

fail:
    wmi_cmd_rsp(atsc->wmi_handle, cmdid, status, NULL, 0);
    atd_trace(ATD_DEBUG_FUNCTRACE ,(" End"));
}



/** 
 * @brief Get WPAIE
 * 
 * @param ctx
 * @param cmdid
 * @param buffer
 * @param len
 */
void 
atd_tgt_get_wpa_ie(void *ctx, a_uint16_t cmdid, 
                  a_uint8_t *buffer, a_int32_t len)
{
    a_status_t status = A_STATUS_OK;
    atd_tgt_dev_t *atsc = (atd_tgt_dev_t *)ctx;
    atd_tgt_wifi_t *wifisc =NULL;
    atd_tgt_vap_t * vap;
    atd_hdr_t   *atdhdr;
    void *wpaiebuff = NULL, *p_wpa_ie = NULL;
    struct iwreq req = {{{0}}};
    uint32_t pldlen = 0;
    
    atd_trace(ATD_DEBUG_FUNCTRACE, ("cmd %d    ",\
                cmdid));

    atd_assert(len >= (sizeof(atd_hdr_t) + sizeof(a_uint32_t)));
  
    atdhdr     = (atd_hdr_t *) buffer;
    p_wpa_ie   = (acfg_opaque_t *) (buffer + sizeof(atd_hdr_t)) ;
    pldlen     = ntohl(*(a_uint32_t *)p_wpa_ie);

    atd_assert(len >= (sizeof(atd_hdr_t) + sizeof(a_uint32_t) + VALID_PAYLOAD_SIZE));

    atd_trace(ATD_DEBUG_FUNCTRACE, (" radio %d vap %d ", \
                                 atdhdr->wifi_index, atdhdr->vap_index));

    wifisc = atsc->wifi_list[atdhdr->wifi_index];
    if(wifisc == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    vap    = wifisc->vap_list[atdhdr->vap_index];
    if(vap == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    vzalloc(pldlen, wpaiebuff);
    if(wpaiebuff == NULL){
        status = A_STATUS_ENOMEM;
        goto fail;
    }

    memcpy((char *)wpaiebuff, (char *)p_wpa_ie + sizeof(a_uint32_t), VALID_PAYLOAD_SIZE);
    req.u.data.pointer  = (a_uint8_t *)wpaiebuff;
    req.u.data.length   = pldlen;
    req.u.data.flags    = 1;

#define IOCTL_GETWPAIE (SIOCDEVPRIVATE + 4)
    status = netdev_ioctl(vap->vap_netdev, (struct ifreq *)&req, IOCTL_GETWPAIE);

    atd_trace(ATD_DEBUG_FUNCTRACE,("wpaie %p \n", p_wpa_ie));

fail:
    wmi_cmd_rsp(atsc->wmi_handle, cmdid, status, (a_uint8_t *)wpaiebuff, 
                req.u.data.length);

    if(wpaiebuff)
        vfree(wpaiebuff);

}

/** 
 * @brief Set AC Params
 * 
 * @param ctx
 * @param cmdid
 * @param buffer
 * @param len
 */
void 
atd_tgt_set_acparams(void *ctx, a_uint16_t cmdid, 
                  a_uint8_t *buffer, a_int32_t len)
{
    a_status_t status = A_STATUS_OK;
    atd_tgt_dev_t *atsc = (atd_tgt_dev_t *)ctx;
    atd_tgt_wifi_t *wifisc =NULL;
    atd_tgt_vap_t * vap;
    union  iwreq_data iwreqdata;
    struct iw_point *iwpdata  = &iwreqdata.data;
    struct iw_request_info info;
    atd_hdr_t  *atdhdr;
    a_uint32_t *ac;
    a_uint32_t i;

    atd_trace(ATD_DEBUG_FUNCTRACE, ("cmd %d    ", cmdid));

    atd_assert(len >= (sizeof(atd_hdr_t) + sizeof(a_uint32_t)));

    atdhdr       = (atd_hdr_t *) buffer;

    wifisc = atsc->wifi_list[atdhdr->wifi_index];
    if(wifisc == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    vap    = wifisc->vap_list[atdhdr->vap_index];
    if(vap == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    atd_trace(ATD_DEBUG_CFG, ("Setting AC Params\n"));

    ac = (a_uint32_t *) (buffer + sizeof(atd_hdr_t));

    printk("%s: Setting AC Params ",__FUNCTION__);
    for(i = 0; i < ACFG_MAX_ACPARAMS; i++)
    {
      *ac = ntohl(ac[i]);
      printk(" %d ",*ac);
    }
    printk("\n");
    
    info.cmd = IEEE80211_IOCTL_SET_ACPARAMS;
    info.flags = 0;
    iwpdata->length = ACFG_MAX_ACPARAMS * sizeof(*ac);
    iwpdata->flags = 0;
    iwpdata->pointer = (char *)ac;

    status = netdev_wext_handler(vap->vap_netdev, IEEE80211_IOCTL_SET_ACPARAMS, &info, 
                                 &iwreqdata, (char *)ac); 
    if(status != A_STATUS_OK){
        atd_trace(ATD_DEBUG_CFG, ("set AC params error \n"));
    }

fail:
    wmi_cmd_rsp(atsc->wmi_handle, cmdid, status, NULL, 0);
}

/**
 * @brief Send Mgmtframe
 *
 * @param ctx
 * @param cmdid
 * @param buffer
 * @param len
 */
void
atd_tgt_send_mgmt(void *ctx, a_uint16_t cmdid,
                  a_uint8_t *buffer, a_int32_t len)
{
    a_status_t status = A_STATUS_OK;
    atd_tgt_dev_t *atsc = (atd_tgt_dev_t *)ctx;
    atd_tgt_wifi_t *wifisc =NULL;
    atd_tgt_vap_t * vap;
    union  iwreq_data iwreqdata;
    struct iw_point *iwpdata  = &iwreqdata.data;
    struct iw_request_info info;
    atd_hdr_t  *atdhdr;
    atd_mgmt_t  *mgmt;
    struct ieee80211req_mgmtbuf *mgmt_buf = NULL;
    a_uint32_t l;

    atd_trace(ATD_DEBUG_FUNCTRACE, ("cmd %d    ", cmdid));

    atd_assert(len >= (sizeof(atd_hdr_t) + sizeof(atd_mgmt_t)));
    atdhdr = (atd_hdr_t *) buffer;
    mgmt = (atd_mgmt_t *)(buffer + sizeof(atd_hdr_t));


    atd_trace(ATD_DEBUG_CFG, ("radio %d vap %d ",
              atdhdr->wifi_index, atdhdr->vap_index));

    wifisc = atsc->wifi_list[atdhdr->wifi_index];

    if(wifisc == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    vap    = wifisc->vap_list[atdhdr->vap_index];

    if(vap == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }
    
    l = ntohl(mgmt->buflen);
    vzalloc(sizeof(struct ieee80211req_mgmtbuf) + l, mgmt_buf);
    if(mgmt_buf == NULL){
        atd_trace(ATD_DEBUG_CFG, ("\n %s : allocation failed\n", __FUNCTION__));
        status = A_STATUS_ENOMEM;
        goto fail;
    }

    memcpy(mgmt_buf->macaddr, mgmt->macaddr, ACFG_MACADDR_LEN);
    mgmt_buf->buflen = ntohl(mgmt->buflen);
    memcpy(mgmt_buf->buf, mgmt->buf, l);

    info.cmd = IEEE80211_IOCTL_SEND_MGMT;
    info.flags = 0;
    iwpdata->length = sizeof(struct ieee80211req_mgmtbuf) + l;
    iwpdata->flags = 0;
    iwpdata->pointer = (char *)mgmt_buf;

    status = netdev_wext_handler(vap->vap_netdev, IEEE80211_IOCTL_SEND_MGMT, &info,
                                 &iwreqdata, (char *)mgmt_buf);
    if(status != A_STATUS_OK){
        atd_trace(ATD_DEBUG_CFG, ("mgmt send set erro \n"));
    }

    vfree(mgmt_buf);

fail:
    wmi_cmd_rsp(atsc->wmi_handle, cmdid, status, NULL, 0);
}


/**
* @brief Send dbgreqframe
*
* @param ctx
* @param cmdid
* @param buffer
* @param len
*/
void
atd_tgt_dbgreq(void *ctx, a_uint16_t cmdid,
        a_uint8_t *buffer, a_int32_t len)
{
    a_status_t status = A_STATUS_OK;
    atd_tgt_dev_t *atsc = (atd_tgt_dev_t *)ctx;
    atd_tgt_wifi_t *wifisc = NULL;
    atd_tgt_vap_t *vap;
    union  iwreq_data iwreqdata;
    struct iw_point *iwpdata  = &iwreqdata.data;
    struct iw_request_info info;
    atd_hdr_t *atdhdr;
    a_uint32_t rlen = 0;
    atd_athdbg_req_t *req, *resp = NULL;
    struct ieee80211req_athdbg *dbgreq = NULL;
    void *p = NULL;
    a_uint32_t i, j;
    atd_rrmstats_t *rrm_stats;
    ieee80211_rrmstats_t *rrmstats;
    atd_bcnrpt_t *bcn_rpt;
    ieee80211_bcnrpt_t *bcnrpt;
    atd_acs_dbg_t *acs_dbg;
    struct ieee80211_acs_dbg *acsdbg;

    atd_trace(ATD_DEBUG_FUNCTRACE, ("cmd %d    ", cmdid));

    atd_assert(len >= (sizeof(atd_hdr_t) + sizeof(atd_athdbg_req_t)));
    atdhdr = (atd_hdr_t *)buffer;
    req    = (atd_athdbg_req_t *)(buffer + sizeof(atd_hdr_t));


    atd_trace(ATD_DEBUG_CFG, ("radio %d vap %d ",
                atdhdr->wifi_index, atdhdr->vap_index));


    wifisc = atsc->wifi_list[atdhdr->wifi_index];
    if (wifisc == NULL) {
        status = A_STATUS_ENXIO;
        goto fail;
    }

    vap    = wifisc->vap_list[atdhdr->vap_index];
    if (vap == NULL) {
        status = A_STATUS_ENXIO;
        goto fail;
    }

    vzalloc(sizeof(struct ieee80211req_athdbg), dbgreq);
    if (dbgreq == NULL) {
        atd_trace(ATD_DEBUG_CFG, ("memory allocation failed \n"));
        status = A_STATUS_ENOMEM;
        goto fail;
    }

    dbgreq->cmd = req->cmd;
    memcpy(dbgreq->dstmac, req->dstmac, ACFG_MACADDR_LEN);
    switch (dbgreq->cmd) {
        case IEEE80211_DBGREQ_SENDADDBA:
        case IEEE80211_DBGREQ_SENDDELBA:
        case IEEE80211_DBGREQ_SETADDBARESP:
        case IEEE80211_DBGREQ_GETADDBASTATS:
        case IEEE80211_DBGREQ_SENDSINGLEAMSDU:
            dbgreq->data.param[0] = ntohl(req->data.param[0]);
            dbgreq->data.param[1] = ntohl(req->data.param[1]);
            dbgreq->data.param[2] = ntohl(req->data.param[2]);
            dbgreq->data.param[3] = ntohl(req->data.param[3]);
            break;
        case IEEE80211_DBGREQ_SENDBCNRPT:
            dbgreq->data.bcnrpt.num_rpt = ntohs(req->data.bcnrpt.num_rpt);
            dbgreq->data.bcnrpt.regclass = req->data.bcnrpt.regclass;
            dbgreq->data.bcnrpt.channum = req->data.bcnrpt.channum;
            dbgreq->data.bcnrpt.random_ivl = ntohs(req->data.bcnrpt.random_ivl);
            dbgreq->data.bcnrpt.duration = ntohs(req->data.bcnrpt.duration);
            dbgreq->data.bcnrpt.reqmode = req->data.bcnrpt.reqmode;
            dbgreq->data.bcnrpt.reqtype = req->data.bcnrpt.reqtype;
            memcpy(dbgreq->data.bcnrpt.bssid, req->data.bcnrpt.bssid, 6);
            dbgreq->data.bcnrpt.mode = req->data.bcnrpt.mode;
            dbgreq->data.bcnrpt.req_ssid = req->data.bcnrpt.req_ssid;
            dbgreq->data.bcnrpt.rep_cond = req->data.bcnrpt.rep_cond;
            dbgreq->data.bcnrpt.rep_thresh = req->data.bcnrpt.rep_thresh;
            dbgreq->data.bcnrpt.rep_detail = req->data.bcnrpt.rep_detail;
            dbgreq->data.bcnrpt.req_ie = req->data.bcnrpt.req_ie;
            dbgreq->data.bcnrpt.num_chanrep = req->data.bcnrpt.num_chanrep;
            for (i = 0; i < 2; i++) {
                dbgreq->data.bcnrpt.apchanrep[i].regclass = req->data.bcnrpt.apchanrep[i].regclass;
                dbgreq->data.bcnrpt.apchanrep[i].numchans = req->data.bcnrpt.apchanrep[i].numchans;
                for (j = 0; j < 5; j++)
                    dbgreq->data.bcnrpt.apchanrep[i].channum[j] = req->data.bcnrpt.apchanrep[i].channum[j];
            }
            break;
        case IEEE80211_DBGREQ_SENDTSMRPT:
            dbgreq->data.tsmrpt.num_rpt = ntohs(req->data.tsmrpt.num_rpt);
            dbgreq->data.tsmrpt.rand_ivl = ntohs(req->data.tsmrpt.rand_ivl);
            dbgreq->data.tsmrpt.meas_dur = ntohs(req->data.tsmrpt.meas_dur);
            dbgreq->data.tsmrpt.reqmode = req->data.tsmrpt.reqmode;
            dbgreq->data.tsmrpt.reqtype = req->data.tsmrpt.reqtype;
            dbgreq->data.tsmrpt.tid = req->data.tsmrpt.tid;
            memcpy(dbgreq->data.tsmrpt.macaddr, req->data.tsmrpt.macaddr, 6);
            dbgreq->data.tsmrpt.bin0_range = req->data.tsmrpt.bin0_range;
            dbgreq->data.tsmrpt.trig_cond = req->data.tsmrpt.trig_cond;
            dbgreq->data.tsmrpt.avg_err_thresh = req->data.tsmrpt.avg_err_thresh;
            dbgreq->data.tsmrpt.cons_err_thresh = req->data.tsmrpt.cons_err_thresh;
            dbgreq->data.tsmrpt.delay_thresh = req->data.tsmrpt.delay_thresh;
            dbgreq->data.tsmrpt.meas_count = req->data.tsmrpt.meas_count;
            dbgreq->data.tsmrpt.trig_timeout = req->data.tsmrpt.trig_timeout;
            break;
        case IEEE80211_DBGREQ_SENDNEIGRPT:
            dbgreq->data.neigrpt.dialogtoken = req->data.neigrpt.dialogtoken;
            memcpy(dbgreq->data.neigrpt.ssid, req->data.neigrpt.ssid, 32);
            dbgreq->data.neigrpt.ssid_len = req->data.neigrpt.ssid_len;
            break;
        case IEEE80211_DBGREQ_SENDLMREQ:
            break;
        case IEEE80211_DBGREQ_SENDBSTMREQ:
            dbgreq->data.bstmreq.dialogtoken = req->data.bstmreq.dialogtoken;
            dbgreq->data.bstmreq.candidate_list = req->data.bstmreq.candidate_list;
            dbgreq->data.bstmreq.disassoc = req->data.bstmreq.disassoc;
            dbgreq->data.bstmreq.disassoc_timer = ntohs(req->data.bstmreq.disassoc_timer);
            dbgreq->data.bstmreq.validity_itvl = req->data.bstmreq.validity_itvl;
            break;
        case IEEE80211_DBGREQ_SENDCHLOADREQ:
            memcpy(dbgreq->data.chloadrpt.dstmac, req->data.chloadrpt.dstmac, 6);
            dbgreq->data.chloadrpt.num_rpts = ntohs(req->data.chloadrpt.num_rpts);
            dbgreq->data.chloadrpt.regclass = req->data.chloadrpt.regclass;
            dbgreq->data.chloadrpt.chnum = req->data.chloadrpt.chnum;
            dbgreq->data.chloadrpt.r_invl = ntohs(req->data.chloadrpt.r_invl);
            dbgreq->data.chloadrpt.m_dur = ntohs(req->data.chloadrpt.m_dur);
            dbgreq->data.chloadrpt.cond = req->data.chloadrpt.cond;
            dbgreq->data.chloadrpt.c_val = req->data.chloadrpt.c_val;
            break;
        case IEEE80211_DBGREQ_SENDSTASTATSREQ:
            memcpy(dbgreq->data.stastats.dstmac, req->data.stastats.dstmac, 6);
            dbgreq->data.stastats.num_rpts = ntohs(req->data.stastats.num_rpts);
            dbgreq->data.stastats.m_dur = ntohs(req->data.stastats.m_dur);
            dbgreq->data.stastats.r_invl = ntohs(req->data.stastats.r_invl);
            dbgreq->data.stastats.gid = req->data.stastats.gid;
            break;
        case IEEE80211_DBGREQ_SENDNHIST:
            dbgreq->data.nhist.num_rpts = ntohs(req->data.nhist.num_rpts);
            memcpy(dbgreq->data.nhist.dstmac, req->data.nhist.dstmac, 6);
            dbgreq->data.nhist.regclass = req->data.nhist.regclass;
            dbgreq->data.nhist.chnum = req->data.nhist.chnum;
            dbgreq->data.nhist.r_invl = ntohs(req->data.nhist.r_invl);
            dbgreq->data.nhist.m_dur = ntohs(req->data.nhist.m_dur);
            dbgreq->data.nhist.cond = req->data.nhist.cond;
            dbgreq->data.nhist.c_val = req->data.nhist.c_val;
            break;
        case IEEE80211_DBGREQ_SENDDELTS:
            dbgreq->data.param[0] = ntohl(req->data.param[0]);
            break;
        case IEEE80211_DBGREQ_SENDADDTSREQ:
            dbgreq->data.tsinfo.traffic_type = req->data.tsinfo.traffic_type;
            dbgreq->data.tsinfo.direction = req->data.tsinfo.direction;
            dbgreq->data.tsinfo.dot1Dtag = req->data.tsinfo.dot1Dtag;
            dbgreq->data.tsinfo.tid = req->data.tsinfo.tid;
            dbgreq->data.tsinfo.acc_policy_edca = req->data.tsinfo.acc_policy_edca;
            dbgreq->data.tsinfo.acc_policy_hcca = req->data.tsinfo.acc_policy_hcca;
            dbgreq->data.tsinfo.aggregation = req->data.tsinfo.aggregation;
            dbgreq->data.tsinfo.psb = req->data.tsinfo.psb;
            dbgreq->data.tsinfo.ack_policy = req->data.tsinfo.ack_policy;
            dbgreq->data.tsinfo.norminal_msdu_size = ntohs(req->data.tsinfo.norminal_msdu_size);
            dbgreq->data.tsinfo.max_msdu_size = ntohs(req->data.tsinfo.max_msdu_size);
            dbgreq->data.tsinfo.min_srv_interval = ntohl(req->data.tsinfo.min_srv_interval);
            dbgreq->data.tsinfo.max_srv_interval = ntohl(req->data.tsinfo.max_srv_interval);
            dbgreq->data.tsinfo.inactivity_interval = ntohl(req->data.tsinfo.inactivity_interval);
            dbgreq->data.tsinfo.suspension_interval = ntohl(req->data.tsinfo.suspension_interval);
            dbgreq->data.tsinfo.srv_start_time = ntohl(req->data.tsinfo.srv_start_time);
            dbgreq->data.tsinfo.min_data_rate = ntohl(req->data.tsinfo.min_data_rate);
            dbgreq->data.tsinfo.mean_data_rate = ntohl(req->data.tsinfo.mean_data_rate);
            dbgreq->data.tsinfo.peak_data_rate = ntohl(req->data.tsinfo.peak_data_rate);
            dbgreq->data.tsinfo.max_burst_size = ntohl(req->data.tsinfo.max_burst_size);
            dbgreq->data.tsinfo.delay_bound = ntohl(req->data.tsinfo.delay_bound);
            dbgreq->data.tsinfo.min_phy_rate = ntohl(req->data.tsinfo.min_phy_rate);
            dbgreq->data.tsinfo.surplus_bw = ntohs(req->data.tsinfo.surplus_bw);
            dbgreq->data.tsinfo.medium_time = ntohs(req->data.tsinfo.medium_time);
            break;
        case IEEE80211_DBGREQ_SENDLCIREQ:
            memcpy(dbgreq->data.lci_req.dstmac, req->data.lci_req.dstmac, 6);
            dbgreq->data.lci_req.num_rpts = ntohs(req->data.lci_req.num_rpts);
            dbgreq->data.lci_req.location = req->data.lci_req.location;
            dbgreq->data.lci_req.lat_res = req->data.lci_req.lat_res;
            dbgreq->data.lci_req.long_res = req->data.lci_req.long_res;
            dbgreq->data.lci_req.alt_res = req->data.lci_req.alt_res;
            dbgreq->data.lci_req.azi_res = req->data.lci_req.azi_res;
            dbgreq->data.lci_req.azi_type = req->data.lci_req.azi_type;
            break;
        case IEEE80211_DBGREQ_GETRRMSTATS:
            dbgreq->data.rrmstats_req.index = ntohl(req->data.rrmstats_req.index);
            dbgreq->data.rrmstats_req.data_size = ntohl(req->data.rrmstats_req.data_size);
            p = kzalloc(sizeof(ieee80211_rrmstats_t), GFP_KERNEL);
            if (p == NULL) {
                atd_trace(ATD_DEBUG_CFG, ("memory allocation failed \n"));
                status = A_STATUS_ENOMEM;
                goto fail;
            }
            dbgreq->data.rrmstats_req.data_addr = p;
            rrm_stats = (atd_rrmstats_t *)(req + 1);
            rrmstats = (ieee80211_rrmstats_t *)(dbgreq->data.rrmstats_req.data_addr);
            for (i = 0; i < ACFG_MAX_IEEE_CHAN; i++) {
                rrmstats->chann_load[i] = rrm_stats->chann_load[i];
                rrmstats->noise_data[i].antid = rrm_stats->noise_data[i].antid;
                rrmstats->noise_data[i].anpi = rrm_stats->noise_data[i].anpi;
                for (j = 0; j < 11; j++)
                    rrmstats->noise_data[i].ipi[j] = rrm_stats->noise_data[i].ipi[j];
            }
            rrmstats->ni_rrm_stats.gid0.txfragcnt = ntohl(rrm_stats->ni_rrm_stats.gid0.txfragcnt);
            rrmstats->ni_rrm_stats.gid0.mcastfrmcnt = ntohl(rrm_stats->ni_rrm_stats.gid0.mcastfrmcnt);
            rrmstats->ni_rrm_stats.gid0.failcnt = ntohl(rrm_stats->ni_rrm_stats.gid0.failcnt);
            rrmstats->ni_rrm_stats.gid0.rxfragcnt = ntohl(rrm_stats->ni_rrm_stats.gid0.rxfragcnt);
            rrmstats->ni_rrm_stats.gid0.mcastrxfrmcnt = ntohl(rrm_stats->ni_rrm_stats.gid0.mcastrxfrmcnt);
            rrmstats->ni_rrm_stats.gid0.fcserrcnt = ntohl(rrm_stats->ni_rrm_stats.gid0.fcserrcnt);
            rrmstats->ni_rrm_stats.gid0.txfrmcnt = ntohl(rrm_stats->ni_rrm_stats.gid0.txfrmcnt);
            rrmstats->ni_rrm_stats.gid1.rty = ntohl(rrm_stats->ni_rrm_stats.gid1.rty);
            rrmstats->ni_rrm_stats.gid1.multirty = ntohl(rrm_stats->ni_rrm_stats.gid1.multirty);
            rrmstats->ni_rrm_stats.gid1.frmdup = ntohl(rrm_stats->ni_rrm_stats.gid1.frmdup);
            rrmstats->ni_rrm_stats.gid1.rtsuccess = ntohl(rrm_stats->ni_rrm_stats.gid1.rtsuccess);
            rrmstats->ni_rrm_stats.gid1.rtsfail = ntohl(rrm_stats->ni_rrm_stats.gid1.rtsfail);
            rrmstats->ni_rrm_stats.gid1.ackfail = ntohl(rrm_stats->ni_rrm_stats.gid1.ackfail);
            for (i = 0; i < 8; i++) {
                rrmstats->ni_rrm_stats.gidupx[i].qostxfragcnt = ntohl(rrm_stats->ni_rrm_stats.gidupx[i].qostxfragcnt);
                rrmstats->ni_rrm_stats.gidupx[i].qosfailedcnt = ntohl(rrm_stats->ni_rrm_stats.gidupx[i].qosfailedcnt);
                rrmstats->ni_rrm_stats.gidupx[i].qosrtycnt = ntohl(rrm_stats->ni_rrm_stats.gidupx[i].qosrtycnt);
                rrmstats->ni_rrm_stats.gidupx[i].multirtycnt = ntohl(rrm_stats->ni_rrm_stats.gidupx[i].multirtycnt);
                rrmstats->ni_rrm_stats.gidupx[i].qosfrmdupcnt = ntohl(rrm_stats->ni_rrm_stats.gidupx[i].qosfrmdupcnt);
                rrmstats->ni_rrm_stats.gidupx[i].qosrtssuccnt = ntohl(rrm_stats->ni_rrm_stats.gidupx[i].qosrtssuccnt);
                rrmstats->ni_rrm_stats.gidupx[i].qosrtsfailcnt = ntohl(rrm_stats->ni_rrm_stats.gidupx[i].qosrtsfailcnt);
                rrmstats->ni_rrm_stats.gidupx[i].qosackfailcnt = ntohl(rrm_stats->ni_rrm_stats.gidupx[i].qosackfailcnt);
                rrmstats->ni_rrm_stats.gidupx[i].qosrxfragcnt = ntohl(rrm_stats->ni_rrm_stats.gidupx[i].qosrxfragcnt);
                rrmstats->ni_rrm_stats.gidupx[i].qostxfrmcnt = ntohl(rrm_stats->ni_rrm_stats.gidupx[i].qostxfrmcnt);
                rrmstats->ni_rrm_stats.gidupx[i].qosdiscadrfrmcnt = ntohl(rrm_stats->ni_rrm_stats.gidupx[i].qosdiscadrfrmcnt);
                rrmstats->ni_rrm_stats.gidupx[i].qosmpdurxcnt = ntohl(rrm_stats->ni_rrm_stats.gidupx[i].qosmpdurxcnt);
                rrmstats->ni_rrm_stats.gidupx[i].qosrtyrxcnt = ntohl(rrm_stats->ni_rrm_stats.gidupx[i].qosrtyrxcnt);
            }
            rrmstats->ni_rrm_stats.gid10.ap_avg_delay = rrm_stats->ni_rrm_stats.gid10.ap_avg_delay;
            rrmstats->ni_rrm_stats.gid10.be_avg_delay = rrm_stats->ni_rrm_stats.gid10.be_avg_delay;
            rrmstats->ni_rrm_stats.gid10.bk_avg_delay = rrm_stats->ni_rrm_stats.gid10.bk_avg_delay;
            rrmstats->ni_rrm_stats.gid10.vi_avg_delay = rrm_stats->ni_rrm_stats.gid10.vi_avg_delay;
            rrmstats->ni_rrm_stats.gid10.vo_avg_delay = rrm_stats->ni_rrm_stats.gid10.vo_avg_delay;
            rrmstats->ni_rrm_stats.gid10.st_cnt = ntohs(rrm_stats->ni_rrm_stats.gid10.st_cnt);
            rrmstats->ni_rrm_stats.gid10.ch_util = rrm_stats->ni_rrm_stats.gid10.ch_util;
            rrmstats->ni_rrm_stats.tsm_data.tid = rrm_stats->ni_rrm_stats.tsm_data.tid;
            rrmstats->ni_rrm_stats.tsm_data.brange = rrm_stats->ni_rrm_stats.tsm_data.brange;
            memcpy(rrmstats->ni_rrm_stats.tsm_data.mac, rrm_stats->ni_rrm_stats.tsm_data.mac, 6);
            rrmstats->ni_rrm_stats.tsm_data.tx_cnt = ntohl(rrm_stats->ni_rrm_stats.tsm_data.tx_cnt);
            rrmstats->ni_rrm_stats.tsm_data.discnt = ntohl(rrm_stats->ni_rrm_stats.tsm_data.discnt);
            rrmstats->ni_rrm_stats.tsm_data.multirtycnt = ntohl(rrm_stats->ni_rrm_stats.tsm_data.multirtycnt);
            rrmstats->ni_rrm_stats.tsm_data.cfpoll = ntohl(rrm_stats->ni_rrm_stats.tsm_data.cfpoll);
            rrmstats->ni_rrm_stats.tsm_data.qdelay = ntohl(rrm_stats->ni_rrm_stats.tsm_data.qdelay);
            rrmstats->ni_rrm_stats.tsm_data.txdelay = ntohl(rrm_stats->ni_rrm_stats.tsm_data.txdelay);
            for (i = 0; i < 6; i++)
                rrmstats->ni_rrm_stats.tsm_data.bin[i] = ntohl(rrm_stats->ni_rrm_stats.tsm_data.bin[i]);
            for (i = 0; i < 12; i++) {
                rrmstats->ni_rrm_stats.frmcnt_data[i].phytype = rrm_stats->ni_rrm_stats.frmcnt_data[i].phytype;
                rrmstats->ni_rrm_stats.frmcnt_data[i].arcpi = rrm_stats->ni_rrm_stats.frmcnt_data[i].arcpi;
                rrmstats->ni_rrm_stats.frmcnt_data[i].lrsni = rrm_stats->ni_rrm_stats.frmcnt_data[i].lrsni;
                rrmstats->ni_rrm_stats.frmcnt_data[i].lrcpi = rrm_stats->ni_rrm_stats.frmcnt_data[i].lrcpi;
                rrmstats->ni_rrm_stats.frmcnt_data[i].antid = rrm_stats->ni_rrm_stats.frmcnt_data[i].antid;
                memcpy(rrmstats->ni_rrm_stats.frmcnt_data[i].ta, rrm_stats->ni_rrm_stats.frmcnt_data[i].ta, 6);
                memcpy(rrmstats->ni_rrm_stats.frmcnt_data[i].bssid, rrm_stats->ni_rrm_stats.frmcnt_data[i].bssid, 6);
                rrmstats->ni_rrm_stats.frmcnt_data[i].frmcnt = ntohs(rrm_stats->ni_rrm_stats.frmcnt_data[i].frmcnt);
            }
            rrmstats->ni_rrm_stats.lm_data.tx_pow = rrm_stats->ni_rrm_stats.lm_data.tx_pow;
            rrmstats->ni_rrm_stats.lm_data.lmargin = rrm_stats->ni_rrm_stats.lm_data.lmargin;
            rrmstats->ni_rrm_stats.lm_data.rxant = rrm_stats->ni_rrm_stats.lm_data.rxant;
            rrmstats->ni_rrm_stats.lm_data.txant = rrm_stats->ni_rrm_stats.lm_data.txant;
            rrmstats->ni_rrm_stats.lm_data.rcpi = rrm_stats->ni_rrm_stats.lm_data.rcpi;
            rrmstats->ni_rrm_stats.lm_data.rsni = rrm_stats->ni_rrm_stats.lm_data.rsni;
            rrmstats->ni_rrm_stats.ni_rrm_lciinfo.id = rrm_stats->ni_rrm_stats.ni_rrm_lciinfo.id;
            rrmstats->ni_rrm_stats.ni_rrm_lciinfo.len = rrm_stats->ni_rrm_stats.ni_rrm_lciinfo.len;
            rrmstats->ni_rrm_stats.ni_rrm_lciinfo.lat_res = rrm_stats->ni_rrm_stats.ni_rrm_lciinfo.lat_res;
            rrmstats->ni_rrm_stats.ni_rrm_lciinfo.alt_type = rrm_stats->ni_rrm_stats.ni_rrm_lciinfo.alt_type;
            rrmstats->ni_rrm_stats.ni_rrm_lciinfo.long_res = rrm_stats->ni_rrm_stats.ni_rrm_lciinfo.long_res;
            rrmstats->ni_rrm_stats.ni_rrm_lciinfo.alt_res = rrm_stats->ni_rrm_stats.ni_rrm_lciinfo.alt_res;
            rrmstats->ni_rrm_stats.ni_rrm_lciinfo.azi_res = rrm_stats->ni_rrm_stats.ni_rrm_lciinfo.azi_res;
            rrmstats->ni_rrm_stats.ni_rrm_lciinfo.alt_frac = rrm_stats->ni_rrm_stats.ni_rrm_lciinfo.alt_frac;
	    rrmstats->ni_rrm_stats.ni_rrm_lciinfo.datum = rrm_stats->ni_rrm_stats.ni_rrm_lciinfo.datum;
            rrmstats->ni_rrm_stats.ni_rrm_lciinfo.azi_type = rrm_stats->ni_rrm_stats.ni_rrm_lciinfo.azi_type;
            rrmstats->ni_rrm_stats.ni_rrm_lciinfo.lat_integ = ntohs(rrm_stats->ni_rrm_stats.ni_rrm_lciinfo.lat_integ);
            rrmstats->ni_rrm_stats.ni_rrm_lciinfo.long_integ = ntohs(rrm_stats->ni_rrm_stats.ni_rrm_lciinfo.long_integ);
            rrmstats->ni_rrm_stats.ni_rrm_lciinfo.azimuth = ntohs(rrm_stats->ni_rrm_stats.ni_rrm_lciinfo.azimuth);
            rrmstats->ni_rrm_stats.ni_rrm_lciinfo.lat_frac = ntohl(rrm_stats->ni_rrm_stats.ni_rrm_lciinfo.lat_frac);
            rrmstats->ni_rrm_stats.ni_rrm_lciinfo.long_frac = ntohl(rrm_stats->ni_rrm_stats.ni_rrm_lciinfo.long_frac);
            rrmstats->ni_rrm_stats.ni_rrm_lciinfo.alt_integ = ntohl(rrm_stats->ni_rrm_stats.ni_rrm_lciinfo.alt_integ);
            rrmstats->ni_rrm_stats.ni_vap_lciinfo.id = rrm_stats->ni_rrm_stats.ni_vap_lciinfo.id;
            rrmstats->ni_rrm_stats.ni_vap_lciinfo.len = rrm_stats->ni_rrm_stats.ni_vap_lciinfo.len;
            rrmstats->ni_rrm_stats.ni_vap_lciinfo.lat_res = rrm_stats->ni_rrm_stats.ni_vap_lciinfo.lat_res;
            rrmstats->ni_rrm_stats.ni_vap_lciinfo.alt_type = rrm_stats->ni_rrm_stats.ni_vap_lciinfo.alt_type;
            rrmstats->ni_rrm_stats.ni_vap_lciinfo.long_res = rrm_stats->ni_rrm_stats.ni_vap_lciinfo.long_res;
            rrmstats->ni_rrm_stats.ni_vap_lciinfo.alt_res = rrm_stats->ni_rrm_stats.ni_vap_lciinfo.alt_res;
            rrmstats->ni_rrm_stats.ni_vap_lciinfo.azi_res = rrm_stats->ni_rrm_stats.ni_vap_lciinfo.azi_res;
            rrmstats->ni_rrm_stats.ni_vap_lciinfo.alt_frac = rrm_stats->ni_rrm_stats.ni_vap_lciinfo.alt_frac;
	    rrmstats->ni_rrm_stats.ni_vap_lciinfo.datum = rrm_stats->ni_rrm_stats.ni_vap_lciinfo.datum;
            rrmstats->ni_rrm_stats.ni_vap_lciinfo.azi_type = rrm_stats->ni_rrm_stats.ni_vap_lciinfo.azi_type;
            rrmstats->ni_rrm_stats.ni_vap_lciinfo.lat_integ = ntohs(rrm_stats->ni_rrm_stats.ni_vap_lciinfo.lat_integ);
            rrmstats->ni_rrm_stats.ni_vap_lciinfo.long_integ = ntohs(rrm_stats->ni_rrm_stats.ni_vap_lciinfo.long_integ);
            rrmstats->ni_rrm_stats.ni_vap_lciinfo.azimuth = ntohs(rrm_stats->ni_rrm_stats.ni_vap_lciinfo.azimuth);
            rrmstats->ni_rrm_stats.ni_vap_lciinfo.lat_frac = ntohl(rrm_stats->ni_rrm_stats.ni_vap_lciinfo.lat_frac);
            rrmstats->ni_rrm_stats.ni_vap_lciinfo.long_frac = ntohl(rrm_stats->ni_rrm_stats.ni_vap_lciinfo.long_frac);
            rrmstats->ni_rrm_stats.ni_vap_lciinfo.alt_integ = ntohl(rrm_stats->ni_rrm_stats.ni_vap_lciinfo.alt_integ);
            break;
        case IEEE80211_DBGREQ_SENDFRMREQ:
            memcpy(dbgreq->data.frm_req.dstmac, req->data.frm_req.dstmac, 6);
            memcpy(dbgreq->data.frm_req.peermac, req->data.frm_req.peermac, 6);
            dbgreq->data.frm_req.num_rpts = ntohs(req->data.frm_req.num_rpts);
            dbgreq->data.frm_req.regclass = req->data.frm_req.regclass;
            dbgreq->data.frm_req.chnum = req->data.frm_req.chnum;
            break;
        case IEEE80211_DBGREQ_GETBCNRPT:
            dbgreq->data.rrmstats_req.index = ntohl(req->data.rrmstats_req.index);
            dbgreq->data.rrmstats_req.data_size = ntohl(req->data.rrmstats_req.data_size);
            p = kzalloc(sizeof(ieee80211_bcnrpt_t), GFP_KERNEL);
            if (p == NULL) {
                atd_trace(ATD_DEBUG_CFG, ("memory allocation failed \n"));
                status = A_STATUS_ENOMEM;
                goto fail;
            }
            dbgreq->data.rrmstats_req.data_addr = p;
            bcn_rpt = (atd_bcnrpt_t *)(req + 1);
            bcnrpt = (ieee80211_bcnrpt_t *)(dbgreq->data.rrmstats_req.data_addr);
            memcpy(bcnrpt->bssid, bcn_rpt->bssid, 6);
            bcnrpt->rsni = bcn_rpt->rsni;
            bcnrpt->rcpi = bcn_rpt->rcpi;
            bcnrpt->chnum = bcn_rpt->chnum;
            bcnrpt->more = bcn_rpt->more;
            break;
        case IEEE80211_DBGREQ_GETRRSSI:
            break;
        case IEEE80211_DBGREQ_GETACSREPORT:
            dbgreq->data.acs_rep.index = ntohl(req->data.acs_rep.index);
            dbgreq->data.acs_rep.data_size = ntohl(req->data.acs_rep.data_size);
            p = kzalloc(sizeof(struct ieee80211_acs_dbg), GFP_KERNEL);
            if (p == NULL) {
                atd_trace(ATD_DEBUG_CFG, ("memory allocation failed \n"));
                status = A_STATUS_ENOMEM;
                goto fail;
            }
            dbgreq->data.acs_rep.data_addr = p;
            acs_dbg = (atd_acs_dbg_t *)(req + 1);
            acsdbg = (struct ieee80211_acs_dbg *)(dbgreq->data.acs_rep.data_addr);
            acsdbg->nchans = acs_dbg->nchans;
            acsdbg->entry_id = acs_dbg->entry_id;
            acsdbg->chan_freq = ntohs(acs_dbg->chan_freq);
            acsdbg->ieee_chan = acs_dbg->ieee_chan;
            acsdbg->chan_nbss = acs_dbg->chan_nbss;
            acsdbg->chan_maxrssi = ntohl(acs_dbg->chan_maxrssi);
            acsdbg->chan_minrssi = ntohl(acs_dbg->chan_minrssi);
            acsdbg->noisefloor = ntohs(acs_dbg->noisefloor);
            acsdbg->channel_loading = ntohs(acs_dbg->channel_loading);
            acsdbg->chan_load = ntohl(acs_dbg->chan_load);
            acsdbg->sec_chan = acs_dbg->sec_chan;
            break;
        case ACFG_DBGREQ_BSTEERING_SET_PARAMS:
            dbgreq->data.bsteering_param.inactivity_timeout_normal = ntohl(req->data.bst.inactivity_timeout_normal);
            dbgreq->data.bsteering_param.inactivity_timeout_overload = ntohl(req->data.bst.inactivity_timeout_overload);
            dbgreq->data.bsteering_param.inactivity_check_period = ntohl(req->data.bst.inactivity_check_period);
            dbgreq->data.bsteering_param.utilization_sample_period = ntohl(req->data.bst.utilization_sample_period);
            dbgreq->data.bsteering_param.utilization_average_num_samples = ntohl(req->data.bst.utilization_average_num_samples);
            dbgreq->data.bsteering_param.inactive_rssi_crossing_threshold = ntohl(req->data.bst.inactive_rssi_crossing_threshold);
            dbgreq->data.bsteering_param.low_rssi_crossing_threshold = ntohl(req->data.bst.low_rssi_crossing_threshold);
            break;
        case ACFG_DBGREQ_BSTEERING_GET_PARAMS:
            break;
        case ACFG_DBGREQ_BSTEERING_SET_DBG_PARAMS:
            dbgreq->data.bsteering_dbg_param.raw_chan_util_log_enable = 
              (req->data.bst_dbg.raw_log_enable & 0x1) ? 1 : 0;
            dbgreq->data.bsteering_dbg_param.raw_rssi_log_enable = 
              (req->data.bst_dbg.raw_log_enable & 0x2) ? 1 : 0;
            break;
        case ACFG_DBGREQ_BSTEERING_GET_DBG_PARAMS:
            break;
        case ACFG_DBGREQ_BSTEERING_ENABLE:
            dbgreq->data.bsteering_enable = req->data.bsteering_enable;
            break;
        case ACFG_DBGREQ_BSTEERING_SET_OVERLOAD:
            dbgreq->data.bsteering_overload = req->data.bsteering_overload;
            break;
        case ACFG_DBGREQ_BSTEERING_GET_OVERLOAD:
            break;
        case ACFG_DBGREQ_BSTEERING_GET_RSSI:
            break;
        case ACFG_DBGREQ_BSTEERING_SET_PROBE_RESP_WH:
            dbgreq->data.bsteering_probe_resp_wh = req->data.bsteering_probe_resp_wh;
            break;

        default:
            break;
    }

    rlen = sizeof(atd_athdbg_req_t);
    switch (dbgreq->cmd) {
        case IEEE80211_DBGREQ_GETRRMSTATS:
            rlen += sizeof(atd_rrmstats_t);
            break;
        case IEEE80211_DBGREQ_GETBCNRPT:
            rlen += sizeof(atd_bcnrpt_t);
            break;
        case IEEE80211_DBGREQ_GETACSREPORT:
            rlen += sizeof(atd_acs_dbg_t);
            break;
    }

    vzalloc(rlen, resp);
    if (resp == NULL) {
        rlen = 0;
        atd_trace(ATD_DEBUG_CFG, ("memory allocation failed \n"));
        status = A_STATUS_ENOMEM;
        goto fail;
    }

    info.cmd = IEEE80211_IOCTL_DBGREQ;
    info.flags = 0;
    iwpdata->length = sizeof(struct ieee80211req_athdbg);
    iwpdata->flags = 0;
    iwpdata->pointer = (char *)dbgreq;

    status = netdev_wext_handler(vap->vap_netdev, IEEE80211_IOCTL_DBGREQ, &info,
            &iwreqdata, (char *)dbgreq);
    if (status != A_STATUS_OK) {
        atd_trace(ATD_DEBUG_CFG, ("ath dbgreq error \n"));
    }

    resp->cmd = dbgreq->cmd;
    memcpy(resp->dstmac, dbgreq->dstmac, ACFG_MACADDR_LEN);
    switch (dbgreq->cmd) {
        case IEEE80211_DBGREQ_SENDADDBA:
        case IEEE80211_DBGREQ_SENDDELBA:
        case IEEE80211_DBGREQ_SETADDBARESP:
        case IEEE80211_DBGREQ_GETADDBASTATS:
        case IEEE80211_DBGREQ_SENDSINGLEAMSDU:
            resp->data.param[0] = htonl(dbgreq->data.param[0]);
            resp->data.param[1] = htonl(dbgreq->data.param[1]);
            resp->data.param[2] = htonl(dbgreq->data.param[2]);
            resp->data.param[3] = htonl(dbgreq->data.param[3]);
            break;
        case IEEE80211_DBGREQ_SENDBCNRPT:
            resp->data.bcnrpt.num_rpt = htons(dbgreq->data.bcnrpt.num_rpt);
            resp->data.bcnrpt.regclass = dbgreq->data.bcnrpt.regclass;
            resp->data.bcnrpt.channum = dbgreq->data.bcnrpt.channum;
            resp->data.bcnrpt.random_ivl = htons(dbgreq->data.bcnrpt.random_ivl);
            resp->data.bcnrpt.duration = htons(dbgreq->data.bcnrpt.duration);
            resp->data.bcnrpt.reqmode = dbgreq->data.bcnrpt.reqmode;
            resp->data.bcnrpt.reqtype = dbgreq->data.bcnrpt.reqtype;
            memcpy(resp->data.bcnrpt.bssid, dbgreq->data.bcnrpt.bssid, 6);
            resp->data.bcnrpt.mode = dbgreq->data.bcnrpt.mode;
            resp->data.bcnrpt.req_ssid = dbgreq->data.bcnrpt.req_ssid;
            resp->data.bcnrpt.rep_cond = dbgreq->data.bcnrpt.rep_cond;
            resp->data.bcnrpt.rep_thresh = dbgreq->data.bcnrpt.rep_thresh;
            resp->data.bcnrpt.rep_detail = dbgreq->data.bcnrpt.rep_detail;
            resp->data.bcnrpt.req_ie = dbgreq->data.bcnrpt.req_ie;
            resp->data.bcnrpt.num_chanrep = dbgreq->data.bcnrpt.num_chanrep;
            for (i = 0; i < 2; i++) {
                resp->data.bcnrpt.apchanrep[i].regclass = dbgreq->data.bcnrpt.apchanrep[i].regclass;
                resp->data.bcnrpt.apchanrep[i].numchans = dbgreq->data.bcnrpt.apchanrep[i].numchans;
                for (j = 0; j < 5; j++)
                    resp->data.bcnrpt.apchanrep[i].channum[j] = dbgreq->data.bcnrpt.apchanrep[i].channum[j];
            }
            break;
        case IEEE80211_DBGREQ_SENDTSMRPT:
            resp->data.tsmrpt.num_rpt = htons(dbgreq->data.tsmrpt.num_rpt);
            resp->data.tsmrpt.rand_ivl = htons(dbgreq->data.tsmrpt.rand_ivl);
            resp->data.tsmrpt.meas_dur = htons(dbgreq->data.tsmrpt.meas_dur);
            resp->data.tsmrpt.reqmode = dbgreq->data.tsmrpt.reqmode;
            resp->data.tsmrpt.reqtype = dbgreq->data.tsmrpt.reqtype;
            resp->data.tsmrpt.tid = dbgreq->data.tsmrpt.tid;
            memcpy(resp->data.tsmrpt.macaddr, dbgreq->data.tsmrpt.macaddr, 6);
            resp->data.tsmrpt.bin0_range = dbgreq->data.tsmrpt.bin0_range;
            resp->data.tsmrpt.trig_cond = dbgreq->data.tsmrpt.trig_cond;
            resp->data.tsmrpt.avg_err_thresh = dbgreq->data.tsmrpt.avg_err_thresh;
            resp->data.tsmrpt.cons_err_thresh = dbgreq->data.tsmrpt.cons_err_thresh;
            resp->data.tsmrpt.delay_thresh = dbgreq->data.tsmrpt.delay_thresh;
            resp->data.tsmrpt.meas_count = dbgreq->data.tsmrpt.meas_count;
            resp->data.tsmrpt.trig_timeout = dbgreq->data.tsmrpt.trig_timeout;
            break;
        case IEEE80211_DBGREQ_SENDNEIGRPT:
            resp->data.neigrpt.dialogtoken = dbgreq->data.neigrpt.dialogtoken;
            memcpy(resp->data.neigrpt.ssid, dbgreq->data.neigrpt.ssid, 32);
            resp->data.neigrpt.ssid_len = dbgreq->data.neigrpt.ssid_len;
            break;
        case IEEE80211_DBGREQ_SENDLMREQ:
            break;
        case IEEE80211_DBGREQ_SENDBSTMREQ:
            resp->data.bstmreq.dialogtoken = dbgreq->data.bstmreq.dialogtoken;
            resp->data.bstmreq.candidate_list = dbgreq->data.bstmreq.candidate_list;
            resp->data.bstmreq.disassoc = dbgreq->data.bstmreq.disassoc;
            resp->data.bstmreq.disassoc_timer = htons(dbgreq->data.bstmreq.disassoc_timer);
            resp->data.bstmreq.validity_itvl = dbgreq->data.bstmreq.validity_itvl;
            break;
        case IEEE80211_DBGREQ_SENDCHLOADREQ:
            memcpy(resp->data.chloadrpt.dstmac, dbgreq->data.chloadrpt.dstmac, 6);
            resp->data.chloadrpt.num_rpts = htons(dbgreq->data.chloadrpt.num_rpts);
            resp->data.chloadrpt.regclass = dbgreq->data.chloadrpt.regclass;
            resp->data.chloadrpt.chnum = dbgreq->data.chloadrpt.chnum;
            resp->data.chloadrpt.r_invl = htons(dbgreq->data.chloadrpt.r_invl);
            resp->data.chloadrpt.m_dur = htons(dbgreq->data.chloadrpt.m_dur);
            resp->data.chloadrpt.cond = dbgreq->data.chloadrpt.cond;
            resp->data.chloadrpt.c_val = dbgreq->data.chloadrpt.c_val;
            break;
        case IEEE80211_DBGREQ_SENDSTASTATSREQ:
            memcpy(resp->data.stastats.dstmac, dbgreq->data.stastats.dstmac, 6);
            resp->data.stastats.num_rpts = htons(dbgreq->data.stastats.num_rpts);
            resp->data.stastats.m_dur = htons(dbgreq->data.stastats.m_dur);
            resp->data.stastats.r_invl = htons(dbgreq->data.stastats.r_invl);
            resp->data.stastats.gid = dbgreq->data.stastats.gid;
            break;
        case IEEE80211_DBGREQ_SENDNHIST:
            resp->data.nhist.num_rpts = htons(dbgreq->data.nhist.num_rpts);
            memcpy(resp->data.nhist.dstmac, dbgreq->data.nhist.dstmac, 6);
            resp->data.nhist.regclass = dbgreq->data.nhist.regclass;
            resp->data.nhist.chnum = dbgreq->data.nhist.chnum;
            resp->data.nhist.r_invl = htons(dbgreq->data.nhist.r_invl);
            resp->data.nhist.m_dur = htons(dbgreq->data.nhist.m_dur);
            resp->data.nhist.cond = dbgreq->data.nhist.cond;
            resp->data.nhist.c_val = dbgreq->data.nhist.c_val;
            break;
        case IEEE80211_DBGREQ_SENDDELTS:
            resp->data.param[0] = htonl(dbgreq->data.param[0]);
            break;
        case IEEE80211_DBGREQ_SENDADDTSREQ:
            resp->data.tsinfo.traffic_type = dbgreq->data.tsinfo.traffic_type;
            resp->data.tsinfo.direction = dbgreq->data.tsinfo.direction;
            resp->data.tsinfo.dot1Dtag = dbgreq->data.tsinfo.dot1Dtag;
            resp->data.tsinfo.tid = dbgreq->data.tsinfo.tid;
            resp->data.tsinfo.acc_policy_edca = dbgreq->data.tsinfo.acc_policy_edca;
            resp->data.tsinfo.acc_policy_hcca = dbgreq->data.tsinfo.acc_policy_hcca;
            resp->data.tsinfo.aggregation = dbgreq->data.tsinfo.aggregation;
            resp->data.tsinfo.psb = dbgreq->data.tsinfo.psb;
            resp->data.tsinfo.ack_policy = dbgreq->data.tsinfo.ack_policy;
            resp->data.tsinfo.norminal_msdu_size = htons(dbgreq->data.tsinfo.norminal_msdu_size);
            resp->data.tsinfo.max_msdu_size = htons(dbgreq->data.tsinfo.max_msdu_size);
            resp->data.tsinfo.min_srv_interval = htonl(dbgreq->data.tsinfo.min_srv_interval);
            resp->data.tsinfo.max_srv_interval = htonl(dbgreq->data.tsinfo.max_srv_interval);
            resp->data.tsinfo.inactivity_interval = htonl(dbgreq->data.tsinfo.inactivity_interval);
            resp->data.tsinfo.suspension_interval = htonl(dbgreq->data.tsinfo.suspension_interval);
            resp->data.tsinfo.srv_start_time = htonl(dbgreq->data.tsinfo.srv_start_time);
            resp->data.tsinfo.min_data_rate = htonl(dbgreq->data.tsinfo.min_data_rate);
            resp->data.tsinfo.mean_data_rate = htonl(dbgreq->data.tsinfo.mean_data_rate);
            resp->data.tsinfo.peak_data_rate = htonl(dbgreq->data.tsinfo.peak_data_rate);
            resp->data.tsinfo.max_burst_size = htonl(dbgreq->data.tsinfo.max_burst_size);
            resp->data.tsinfo.delay_bound = htonl(dbgreq->data.tsinfo.delay_bound);
            resp->data.tsinfo.min_phy_rate = htonl(dbgreq->data.tsinfo.min_phy_rate);
            resp->data.tsinfo.surplus_bw = htons(dbgreq->data.tsinfo.surplus_bw);
            resp->data.tsinfo.medium_time = htons(dbgreq->data.tsinfo.medium_time);
            break;
        case IEEE80211_DBGREQ_SENDLCIREQ:
            memcpy(resp->data.lci_req.dstmac, dbgreq->data.lci_req.dstmac, 6);
            resp->data.lci_req.num_rpts = htons(dbgreq->data.lci_req.num_rpts);
            resp->data.lci_req.location = dbgreq->data.lci_req.location;
            resp->data.lci_req.lat_res = dbgreq->data.lci_req.lat_res;
            resp->data.lci_req.long_res = dbgreq->data.lci_req.long_res;
            resp->data.lci_req.alt_res = dbgreq->data.lci_req.alt_res;
            resp->data.lci_req.azi_res = dbgreq->data.lci_req.azi_res;
            resp->data.lci_req.azi_type = dbgreq->data.lci_req.azi_type;
            break;
        case IEEE80211_DBGREQ_GETRRMSTATS:
            resp->data.rrmstats_req.index = htonl(dbgreq->data.rrmstats_req.index);
            resp->data.rrmstats_req.data_size = htonl(dbgreq->data.rrmstats_req.data_size);
            rrm_stats = (atd_rrmstats_t *)(resp + 1);
            rrmstats = (ieee80211_rrmstats_t *)(dbgreq->data.rrmstats_req.data_addr);
            for (i = 0; i < ACFG_MAX_IEEE_CHAN; i++) {
                rrm_stats->chann_load[i] = rrmstats->chann_load[i];
                rrm_stats->noise_data[i].antid = rrmstats->noise_data[i].antid;
                rrm_stats->noise_data[i].anpi = rrmstats->noise_data[i].anpi;
                for (j = 0; j < 11; j++)
                    rrm_stats->noise_data[i].ipi[j] = rrmstats->noise_data[i].ipi[j];
            }
            rrm_stats->ni_rrm_stats.gid0.txfragcnt = htonl(rrmstats->ni_rrm_stats.gid0.txfragcnt);
            rrm_stats->ni_rrm_stats.gid0.mcastfrmcnt = htonl(rrmstats->ni_rrm_stats.gid0.mcastfrmcnt);
            rrm_stats->ni_rrm_stats.gid0.failcnt = htonl(rrmstats->ni_rrm_stats.gid0.failcnt);
            rrm_stats->ni_rrm_stats.gid0.rxfragcnt = htonl(rrmstats->ni_rrm_stats.gid0.rxfragcnt);
            rrm_stats->ni_rrm_stats.gid0.mcastrxfrmcnt = htonl(rrmstats->ni_rrm_stats.gid0.mcastrxfrmcnt);
            rrm_stats->ni_rrm_stats.gid0.fcserrcnt = htonl(rrmstats->ni_rrm_stats.gid0.fcserrcnt);
            rrm_stats->ni_rrm_stats.gid0.txfrmcnt = htonl(rrmstats->ni_rrm_stats.gid0.txfrmcnt);
            rrm_stats->ni_rrm_stats.gid1.rty = htonl(rrmstats->ni_rrm_stats.gid1.rty);
            rrm_stats->ni_rrm_stats.gid1.multirty = htonl(rrmstats->ni_rrm_stats.gid1.multirty);
            rrm_stats->ni_rrm_stats.gid1.frmdup = htonl(rrmstats->ni_rrm_stats.gid1.frmdup);
            rrm_stats->ni_rrm_stats.gid1.rtsuccess = htonl(rrmstats->ni_rrm_stats.gid1.rtsuccess);
            rrm_stats->ni_rrm_stats.gid1.rtsfail = htonl(rrmstats->ni_rrm_stats.gid1.rtsfail);
            rrm_stats->ni_rrm_stats.gid1.ackfail = htonl(rrmstats->ni_rrm_stats.gid1.ackfail);
            for (i = 0; i < 8; i++) {
                rrm_stats->ni_rrm_stats.gidupx[i].qostxfragcnt = htonl(rrmstats->ni_rrm_stats.gidupx[i].qostxfragcnt);
                rrm_stats->ni_rrm_stats.gidupx[i].qosfailedcnt = htonl(rrmstats->ni_rrm_stats.gidupx[i].qosfailedcnt);
                rrm_stats->ni_rrm_stats.gidupx[i].qosrtycnt = htonl(rrmstats->ni_rrm_stats.gidupx[i].qosrtycnt);
                rrm_stats->ni_rrm_stats.gidupx[i].multirtycnt = htonl(rrmstats->ni_rrm_stats.gidupx[i].multirtycnt);
                rrm_stats->ni_rrm_stats.gidupx[i].qosfrmdupcnt = htonl(rrmstats->ni_rrm_stats.gidupx[i].qosfrmdupcnt);
                rrm_stats->ni_rrm_stats.gidupx[i].qosrtssuccnt = htonl(rrmstats->ni_rrm_stats.gidupx[i].qosrtssuccnt);
                rrm_stats->ni_rrm_stats.gidupx[i].qosrtsfailcnt = htonl(rrmstats->ni_rrm_stats.gidupx[i].qosrtsfailcnt);
                rrm_stats->ni_rrm_stats.gidupx[i].qosackfailcnt = htonl(rrmstats->ni_rrm_stats.gidupx[i].qosackfailcnt);
                rrm_stats->ni_rrm_stats.gidupx[i].qosrxfragcnt = htonl(rrmstats->ni_rrm_stats.gidupx[i].qosrxfragcnt);
                rrm_stats->ni_rrm_stats.gidupx[i].qostxfrmcnt = htonl(rrmstats->ni_rrm_stats.gidupx[i].qostxfrmcnt);
                rrm_stats->ni_rrm_stats.gidupx[i].qosdiscadrfrmcnt = htonl(rrmstats->ni_rrm_stats.gidupx[i].qosdiscadrfrmcnt);
                rrm_stats->ni_rrm_stats.gidupx[i].qosmpdurxcnt = htonl(rrmstats->ni_rrm_stats.gidupx[i].qosmpdurxcnt);
                rrm_stats->ni_rrm_stats.gidupx[i].qosrtyrxcnt = htonl(rrmstats->ni_rrm_stats.gidupx[i].qosrtyrxcnt);
            }
            rrm_stats->ni_rrm_stats.gid10.ap_avg_delay = rrmstats->ni_rrm_stats.gid10.ap_avg_delay;
            rrm_stats->ni_rrm_stats.gid10.be_avg_delay = rrmstats->ni_rrm_stats.gid10.be_avg_delay;
            rrm_stats->ni_rrm_stats.gid10.bk_avg_delay = rrmstats->ni_rrm_stats.gid10.bk_avg_delay;
            rrm_stats->ni_rrm_stats.gid10.vi_avg_delay = rrmstats->ni_rrm_stats.gid10.vi_avg_delay;
            rrm_stats->ni_rrm_stats.gid10.vo_avg_delay = rrmstats->ni_rrm_stats.gid10.vo_avg_delay;
            rrm_stats->ni_rrm_stats.gid10.st_cnt = htons(rrmstats->ni_rrm_stats.gid10.st_cnt);
            rrm_stats->ni_rrm_stats.gid10.ch_util = rrmstats->ni_rrm_stats.gid10.ch_util;
            rrm_stats->ni_rrm_stats.tsm_data.tid = rrmstats->ni_rrm_stats.tsm_data.tid;
            rrm_stats->ni_rrm_stats.tsm_data.brange = rrmstats->ni_rrm_stats.tsm_data.brange;
            memcpy(rrm_stats->ni_rrm_stats.tsm_data.mac, rrmstats->ni_rrm_stats.tsm_data.mac, 6);
            rrm_stats->ni_rrm_stats.tsm_data.tx_cnt = htonl(rrmstats->ni_rrm_stats.tsm_data.tx_cnt);
            rrm_stats->ni_rrm_stats.tsm_data.discnt = htonl(rrmstats->ni_rrm_stats.tsm_data.discnt);
            rrm_stats->ni_rrm_stats.tsm_data.multirtycnt = htonl(rrmstats->ni_rrm_stats.tsm_data.multirtycnt);
            rrm_stats->ni_rrm_stats.tsm_data.cfpoll = htonl(rrmstats->ni_rrm_stats.tsm_data.cfpoll);
            rrm_stats->ni_rrm_stats.tsm_data.qdelay = htonl(rrmstats->ni_rrm_stats.tsm_data.qdelay);
            rrm_stats->ni_rrm_stats.tsm_data.txdelay = htonl(rrmstats->ni_rrm_stats.tsm_data.txdelay);
            for (i = 0; i < 6; i++)
                rrm_stats->ni_rrm_stats.tsm_data.bin[i] = htonl(rrmstats->ni_rrm_stats.tsm_data.bin[i]);
            for (i = 0; i < 12; i++) {
                rrm_stats->ni_rrm_stats.frmcnt_data[i].phytype = rrmstats->ni_rrm_stats.frmcnt_data[i].phytype;
                rrm_stats->ni_rrm_stats.frmcnt_data[i].arcpi = rrmstats->ni_rrm_stats.frmcnt_data[i].arcpi;
                rrm_stats->ni_rrm_stats.frmcnt_data[i].lrsni = rrmstats->ni_rrm_stats.frmcnt_data[i].lrsni;
                rrm_stats->ni_rrm_stats.frmcnt_data[i].lrcpi = rrmstats->ni_rrm_stats.frmcnt_data[i].lrcpi;
                rrm_stats->ni_rrm_stats.frmcnt_data[i].antid = rrmstats->ni_rrm_stats.frmcnt_data[i].antid;
                memcpy(rrm_stats->ni_rrm_stats.frmcnt_data[i].ta, rrmstats->ni_rrm_stats.frmcnt_data[i].ta, 6);
                memcpy(rrm_stats->ni_rrm_stats.frmcnt_data[i].bssid, rrmstats->ni_rrm_stats.frmcnt_data[i].bssid, 6);
                rrm_stats->ni_rrm_stats.frmcnt_data[i].frmcnt = htons(rrmstats->ni_rrm_stats.frmcnt_data[i].frmcnt);
            }
            rrm_stats->ni_rrm_stats.lm_data.tx_pow = rrmstats->ni_rrm_stats.lm_data.tx_pow;
            rrm_stats->ni_rrm_stats.lm_data.lmargin = rrmstats->ni_rrm_stats.lm_data.lmargin;
            rrm_stats->ni_rrm_stats.lm_data.rxant = rrmstats->ni_rrm_stats.lm_data.rxant;
            rrm_stats->ni_rrm_stats.lm_data.txant = rrmstats->ni_rrm_stats.lm_data.txant;
            rrm_stats->ni_rrm_stats.lm_data.rcpi = rrmstats->ni_rrm_stats.lm_data.rcpi;
            rrm_stats->ni_rrm_stats.lm_data.rsni = rrmstats->ni_rrm_stats.lm_data.rsni;
            rrm_stats->ni_rrm_stats.ni_rrm_lciinfo.id = rrmstats->ni_rrm_stats.ni_rrm_lciinfo.id;
            rrm_stats->ni_rrm_stats.ni_rrm_lciinfo.len = rrmstats->ni_rrm_stats.ni_rrm_lciinfo.len;
            rrm_stats->ni_rrm_stats.ni_rrm_lciinfo.lat_res = rrmstats->ni_rrm_stats.ni_rrm_lciinfo.lat_res;
            rrm_stats->ni_rrm_stats.ni_rrm_lciinfo.alt_type = rrmstats->ni_rrm_stats.ni_rrm_lciinfo.alt_type;
            rrm_stats->ni_rrm_stats.ni_rrm_lciinfo.long_res = rrmstats->ni_rrm_stats.ni_rrm_lciinfo.long_res;
            rrm_stats->ni_rrm_stats.ni_rrm_lciinfo.alt_res = rrmstats->ni_rrm_stats.ni_rrm_lciinfo.alt_res;
            rrm_stats->ni_rrm_stats.ni_rrm_lciinfo.azi_res = rrmstats->ni_rrm_stats.ni_rrm_lciinfo.azi_res;
            rrm_stats->ni_rrm_stats.ni_rrm_lciinfo.alt_frac = rrmstats->ni_rrm_stats.ni_rrm_lciinfo.alt_frac;
	    rrm_stats->ni_rrm_stats.ni_rrm_lciinfo.datum = rrmstats->ni_rrm_stats.ni_rrm_lciinfo.datum;
            rrm_stats->ni_rrm_stats.ni_rrm_lciinfo.azi_type = rrmstats->ni_rrm_stats.ni_rrm_lciinfo.azi_type;
            rrm_stats->ni_rrm_stats.ni_rrm_lciinfo.lat_integ = htons(rrmstats->ni_rrm_stats.ni_rrm_lciinfo.lat_integ);
            rrm_stats->ni_rrm_stats.ni_rrm_lciinfo.long_integ = htons(rrmstats->ni_rrm_stats.ni_rrm_lciinfo.long_integ);
            rrm_stats->ni_rrm_stats.ni_rrm_lciinfo.azimuth = htons(rrmstats->ni_rrm_stats.ni_rrm_lciinfo.azimuth);
            rrm_stats->ni_rrm_stats.ni_rrm_lciinfo.lat_frac = htonl(rrmstats->ni_rrm_stats.ni_rrm_lciinfo.lat_frac);
            rrm_stats->ni_rrm_stats.ni_rrm_lciinfo.long_frac = htonl(rrmstats->ni_rrm_stats.ni_rrm_lciinfo.long_frac);
            rrm_stats->ni_rrm_stats.ni_rrm_lciinfo.alt_integ = htonl(rrmstats->ni_rrm_stats.ni_rrm_lciinfo.alt_integ);
            rrm_stats->ni_rrm_stats.ni_vap_lciinfo.id = rrmstats->ni_rrm_stats.ni_vap_lciinfo.id;
            rrm_stats->ni_rrm_stats.ni_vap_lciinfo.len = rrmstats->ni_rrm_stats.ni_vap_lciinfo.len;
            rrm_stats->ni_rrm_stats.ni_vap_lciinfo.lat_res = rrmstats->ni_rrm_stats.ni_vap_lciinfo.lat_res;
            rrm_stats->ni_rrm_stats.ni_vap_lciinfo.alt_type = rrmstats->ni_rrm_stats.ni_vap_lciinfo.alt_type;
            rrm_stats->ni_rrm_stats.ni_vap_lciinfo.long_res = rrmstats->ni_rrm_stats.ni_vap_lciinfo.long_res;
            rrm_stats->ni_rrm_stats.ni_vap_lciinfo.alt_res = rrmstats->ni_rrm_stats.ni_vap_lciinfo.alt_res;
            rrm_stats->ni_rrm_stats.ni_vap_lciinfo.azi_res = rrmstats->ni_rrm_stats.ni_vap_lciinfo.azi_res;
            rrm_stats->ni_rrm_stats.ni_vap_lciinfo.alt_frac = rrmstats->ni_rrm_stats.ni_vap_lciinfo.alt_frac;
	    rrm_stats->ni_rrm_stats.ni_vap_lciinfo.datum = rrmstats->ni_rrm_stats.ni_vap_lciinfo.datum;
            rrm_stats->ni_rrm_stats.ni_vap_lciinfo.azi_type = rrmstats->ni_rrm_stats.ni_vap_lciinfo.azi_type;
            rrm_stats->ni_rrm_stats.ni_vap_lciinfo.lat_integ = htons(rrmstats->ni_rrm_stats.ni_vap_lciinfo.lat_integ);
            rrm_stats->ni_rrm_stats.ni_vap_lciinfo.long_integ = htons(rrmstats->ni_rrm_stats.ni_vap_lciinfo.long_integ);
            rrm_stats->ni_rrm_stats.ni_vap_lciinfo.azimuth = htons(rrmstats->ni_rrm_stats.ni_vap_lciinfo.azimuth);
            rrm_stats->ni_rrm_stats.ni_vap_lciinfo.lat_frac = htonl(rrmstats->ni_rrm_stats.ni_vap_lciinfo.lat_frac);
            rrm_stats->ni_rrm_stats.ni_vap_lciinfo.long_frac = htonl(rrmstats->ni_rrm_stats.ni_vap_lciinfo.long_frac);
            rrm_stats->ni_rrm_stats.ni_vap_lciinfo.alt_integ = htonl(rrmstats->ni_rrm_stats.ni_vap_lciinfo.alt_integ);
            break;
        case IEEE80211_DBGREQ_SENDFRMREQ:
            memcpy(resp->data.frm_req.dstmac, dbgreq->data.frm_req.dstmac, 6);
            memcpy(resp->data.frm_req.peermac, dbgreq->data.frm_req.peermac, 6);
            resp->data.frm_req.num_rpts = htons(dbgreq->data.frm_req.num_rpts);
            resp->data.frm_req.regclass = dbgreq->data.frm_req.regclass;
            resp->data.frm_req.chnum = dbgreq->data.frm_req.chnum;
            break;
        case IEEE80211_DBGREQ_GETBCNRPT:
            resp->data.rrmstats_req.index = htonl(dbgreq->data.rrmstats_req.index);
            resp->data.rrmstats_req.data_size = htonl(dbgreq->data.rrmstats_req.data_size);
            bcn_rpt = (atd_bcnrpt_t *)(resp + 1);
            bcnrpt = (ieee80211_bcnrpt_t *)(dbgreq->data.rrmstats_req.data_addr);
            memcpy(bcn_rpt->bssid, bcnrpt->bssid, 6);
            bcn_rpt->rsni = bcnrpt->rsni;
            bcn_rpt->rcpi = bcnrpt->rcpi;
            bcn_rpt->chnum = bcnrpt->chnum;
            bcn_rpt->more = bcnrpt->more;
            break;
        case IEEE80211_DBGREQ_GETRRSSI:
            break;
        case IEEE80211_DBGREQ_GETACSREPORT:
            resp->data.acs_rep.index = htonl(dbgreq->data.acs_rep.index);
            resp->data.acs_rep.data_size = htonl(dbgreq->data.acs_rep.data_size);
            acs_dbg = (atd_acs_dbg_t *)(resp + 1);
            acsdbg = (struct ieee80211_acs_dbg *)(dbgreq->data.acs_rep.data_addr);
            acs_dbg->nchans = acsdbg->nchans;
            acs_dbg->entry_id = acsdbg->entry_id;
            acs_dbg->chan_freq = htons(acsdbg->chan_freq);
            acs_dbg->ieee_chan = acsdbg->ieee_chan;
            acs_dbg->chan_nbss = acsdbg->chan_nbss;
            acs_dbg->chan_maxrssi = htonl(acsdbg->chan_maxrssi);
            acs_dbg->chan_minrssi = htonl(acsdbg->chan_minrssi);
            acs_dbg->noisefloor = htons(acsdbg->noisefloor);
            acs_dbg->channel_loading = htons(acsdbg->channel_loading);
            acs_dbg->chan_load = htonl(acsdbg->chan_load);
            acs_dbg->sec_chan = acsdbg->sec_chan;
            break;
        case ACFG_DBGREQ_BSTEERING_SET_PARAMS:
            break;
        case ACFG_DBGREQ_BSTEERING_GET_PARAMS:
            resp->data.bst.inactivity_timeout_normal = htonl(dbgreq->data.bsteering_param.inactivity_timeout_normal);
            resp->data.bst.inactivity_timeout_overload = htonl(dbgreq->data.bsteering_param.inactivity_timeout_overload);
            resp->data.bst.inactivity_check_period = htonl(dbgreq->data.bsteering_param.inactivity_check_period);
            resp->data.bst.utilization_sample_period = htonl(dbgreq->data.bsteering_param.utilization_sample_period);
            resp->data.bst.utilization_average_num_samples = htonl(dbgreq->data.bsteering_param.utilization_average_num_samples);
            resp->data.bst.inactive_rssi_crossing_threshold = htonl(dbgreq->data.bsteering_param.inactive_rssi_crossing_threshold);
            resp->data.bst.low_rssi_crossing_threshold = htonl(dbgreq->data.bsteering_param.low_rssi_crossing_threshold);
            break;
        case ACFG_DBGREQ_BSTEERING_SET_DBG_PARAMS:
            break;
        case ACFG_DBGREQ_BSTEERING_GET_DBG_PARAMS:
            resp->data.bst_dbg.raw_log_enable = 0;
          if (dbgreq->data.bsteering_dbg_param.raw_chan_util_log_enable)
            resp->data.bst_dbg.raw_log_enable += 0x1;
          if (dbgreq->data.bsteering_dbg_param.raw_rssi_log_enable)
            resp->data.bst_dbg.raw_log_enable += 0x2;
            break;
        case ACFG_DBGREQ_BSTEERING_ENABLE:
            break;
        case ACFG_DBGREQ_BSTEERING_SET_OVERLOAD:
            break;
        case ACFG_DBGREQ_BSTEERING_GET_OVERLOAD:
            resp->data.bsteering_overload = dbgreq->data.bsteering_overload;
            break;
        case ACFG_DBGREQ_BSTEERING_GET_RSSI:
            memcpy(resp->data.bst_rssi_req.sender_addr, dbgreq->data.bsteering_rssi_req.sender_addr, 6);
            resp->data.bst_rssi_req.num_measurements = htons(dbgreq->data.bsteering_rssi_req.num_measurements);
            break;
        case ACFG_DBGREQ_BSTEERING_SET_PROBE_RESP_WH:
            break;

        default:
            break;
    }

fail:
    wmi_cmd_rsp(atsc->wmi_handle, cmdid, status, (a_uint8_t *)resp, rlen);

    if (resp)
        vfree(resp);
    if (p)
        kfree(p);
    if (dbgreq)
        vfree(dbgreq);
}


/** 
 * @brief Set Filterframe 
 * 
 * @param ctx
 * @param cmdid
 * @param buffer
 * @param len
 */
void 
atd_tgt_set_filterframe(void *ctx, a_uint16_t cmdid, 
                  a_uint8_t *buffer, a_int32_t len)
{
    a_status_t status = A_STATUS_OK;
    atd_tgt_dev_t *atsc = (atd_tgt_dev_t *)ctx;
    atd_tgt_wifi_t *wifisc =NULL;
    atd_tgt_vap_t * vap;
    union  iwreq_data iwreqdata;
    struct iw_point *iwpdata  = &iwreqdata.data;
    struct iw_request_info info;
    atd_hdr_t  *atdhdr;
    atd_filter_t *filterframe;
    struct ieee80211req_set_filter *fltrfrm;

    atd_trace(ATD_DEBUG_FUNCTRACE, ("cmd %d    ", cmdid));

    atd_assert(len >= (sizeof(atd_hdr_t) + sizeof(atd_filter_t)));
    atdhdr = (atd_hdr_t *) buffer;
    filterframe = (atd_filter_t *)(buffer + sizeof(atd_hdr_t));


    atd_trace(ATD_DEBUG_CFG, ("radio %d vap %d ", 
              atdhdr->wifi_index, atdhdr->vap_index));

    wifisc = atsc->wifi_list[atdhdr->wifi_index];

    if(wifisc == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    vap    = wifisc->vap_list[atdhdr->vap_index];

    if(vap == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    vzalloc(sizeof(struct ieee80211req_set_filter), fltrfrm);
    if(fltrfrm == NULL){
        atd_trace(ATD_DEBUG_CFG, ("\n %s : allocation failed\n", __FUNCTION__));
        status = A_STATUS_ENOMEM;
        goto fail; 
    }

    fltrfrm->app_filterype = ntohl(filterframe->filter);

    info.cmd = IEEE80211_IOCTL_FILTERFRAME;
    info.flags = 0;
    iwpdata->length = sizeof(struct ieee80211req_set_filter);
    iwpdata->flags = 0;
    iwpdata->pointer = (char *)fltrfrm;

    status = netdev_wext_handler(vap->vap_netdev, IEEE80211_IOCTL_FILTERFRAME, &info, 
                                 &iwreqdata, (char *)fltrfrm); 
    if(status != A_STATUS_OK){
        atd_trace(ATD_DEBUG_CFG, ("filter frame set erro \n"));
    }

    vfree(fltrfrm);

fail:
    wmi_cmd_rsp(atsc->wmi_handle, cmdid, status, NULL, 0);
}


/** 
 * @brief Set Appiebuf
 * 
 * @param ctx
 * @param cmdid
 * @param buffer
 * @param len
 */
void 
atd_tgt_set_appiebuf(void *ctx, a_uint16_t cmdid, 
                  a_uint8_t *buffer, a_int32_t len)
{
    a_status_t status = A_STATUS_OK;
    atd_tgt_dev_t *atsc = (atd_tgt_dev_t *)ctx;
    atd_tgt_wifi_t *wifisc =NULL;
    atd_tgt_vap_t * vap;
    union  iwreq_data iwreqdata;
    struct iw_point *iwpdata  = &iwreqdata.data;
    struct iw_request_info info;
    atd_hdr_t  *atdhdr;
    atd_appie_t *appie;
    struct ieee80211req_getset_appiebuf *appie_buf = NULL;
    a_uint32_t l;

    atd_trace(ATD_DEBUG_FUNCTRACE, ("cmd %d    ", cmdid));

    atd_assert(len >= (sizeof(atd_hdr_t) + sizeof(atd_appie_t)));

    if( buffer == NULL || len < 0 ){
        atd_trace(ATD_DEBUG_CFG, ("\n %s : buffer or len invalid \n", __FUNCTION__));
        status = A_STATUS_EINVAL;
        goto fail;
    }

    atdhdr = (atd_hdr_t *) buffer;
    appie = (atd_appie_t *)(buffer + sizeof(atd_hdr_t));

    atd_trace(ATD_DEBUG_CFG, ("radio %d vap %d ", 
              atdhdr->wifi_index, atdhdr->vap_index));


    wifisc = atsc->wifi_list[atdhdr->wifi_index];

    if(wifisc == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    vap    = wifisc->vap_list[atdhdr->vap_index];

    if(vap == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    l = ntohl(appie->buflen);
    vzalloc(sizeof(struct ieee80211req_getset_appiebuf) + l, appie_buf);
    if(appie_buf == NULL){
        atd_trace(ATD_DEBUG_CFG, ("\n %s : allocation failed\n", __FUNCTION__));
        status = A_STATUS_ENOMEM;
        goto fail; 
    }

    appie_buf->app_frmtype = ntohl(appie->frmtype);
    appie_buf->app_buflen = ntohl(appie->buflen);
    memcpy(appie_buf->app_buf, appie->buf, l);

    info.cmd = IEEE80211_IOCTL_SET_APPIEBUF;
    info.flags = 0;
    iwpdata->length = sizeof(struct ieee80211req_getset_appiebuf) + l; 
    iwpdata->flags = 0;
    iwpdata->pointer = (char *)appie_buf;

    status = netdev_wext_handler(vap->vap_netdev, IEEE80211_IOCTL_SET_APPIEBUF, &info, 
                                 &iwreqdata, (char *)appie_buf); 
    if(status != A_STATUS_OK){
        atd_trace(ATD_DEBUG_CFG, ("appiebuf set erro \n"));
    }

    vfree(appie_buf);

fail:
    wmi_cmd_rsp(atsc->wmi_handle, cmdid, status, NULL, 0);
}



/** 
 * @brief Del Key
 * 
 * @param ctx
 * @param cmdid
 * @param buffer
 * @param len
 */
void 
atd_tgt_del_key(void *ctx, a_uint16_t cmdid, 
                  a_uint8_t *buffer, a_int32_t len)
{
    a_status_t status = A_STATUS_OK;
    atd_tgt_dev_t *atsc = (atd_tgt_dev_t *)ctx;
    atd_tgt_wifi_t *wifisc =NULL;
    atd_tgt_vap_t * vap;
    union  iwreq_data iwreqdata;
    struct iw_point *iwpdata  = &iwreqdata.data;
    struct iw_request_info info;
    atd_hdr_t  *atdhdr;
    atd_delkey_t *delkey;
    struct ieee80211req_del_key *del_key = NULL;

    atd_trace(ATD_DEBUG_FUNCTRACE, ("cmd %d    ", cmdid));

    atd_assert(len >= (sizeof(atd_hdr_t) + sizeof(atd_delkey_t)));
    atdhdr = (atd_hdr_t *) buffer;
    delkey = (atd_delkey_t *)(buffer + sizeof(atd_hdr_t));


    atd_trace(ATD_DEBUG_CFG, ("radio %d vap %d ", 
              atdhdr->wifi_index, atdhdr->vap_index));

    wifisc = atsc->wifi_list[atdhdr->wifi_index];

    if(wifisc == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    vap    = wifisc->vap_list[atdhdr->vap_index];

    if(vap == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    vzalloc(sizeof(struct ieee80211req_del_key), del_key);
    if(del_key == NULL){
            printk("\n %s : allocation failed\n", __FUNCTION__);
            status = A_STATUS_ENOMEM;
            goto fail; 
    }

    del_key->idk_keyix = delkey->idx;
    memcpy(del_key->idk_macaddr, delkey->addr, ACFG_MACADDR_LEN);

    info.cmd = IEEE80211_IOCTL_DELKEY;
    info.flags = 0;
    iwpdata->length = sizeof(struct ieee80211req_del_key); 
    iwpdata->flags = 0;
    iwpdata->pointer = (char *)del_key;

    status = netdev_wext_handler(vap->vap_netdev, IEEE80211_IOCTL_DELKEY, &info, 
                                 &iwreqdata, (char *)del_key); 
    if(status != A_STATUS_OK){
        atd_trace(ATD_DEBUG_CFG, ("del key set erro \n"));
    }

    vfree(del_key);

fail:
    wmi_cmd_rsp(atsc->wmi_handle, cmdid, status, NULL, 0);
}



/** 
 * @brief Set Key
 * 
 * @param ctx
 * @param cmdid
 * @param buffer
 * @param len
 */
void 
atd_tgt_set_key(void *ctx, a_uint16_t cmdid, 
                  a_uint8_t *buffer, a_int32_t len)
{
    a_status_t status = A_STATUS_OK;
    atd_tgt_dev_t *atsc = (atd_tgt_dev_t *)ctx;
    atd_tgt_wifi_t *wifisc =NULL;
    atd_tgt_vap_t * vap;
    union  iwreq_data iwreqdata;
    struct iw_point *iwpdata  = &iwreqdata.data;
    struct iw_request_info info;
    atd_hdr_t  *atdhdr;
    atd_key_t  *setkey;
    struct ieee80211req_key *ik = NULL;

    atd_trace(ATD_DEBUG_FUNCTRACE, ("cmd %d    ", cmdid));

    atd_assert(len >= (sizeof(atd_hdr_t) + sizeof(atd_key_t)));

    atdhdr = (atd_hdr_t *) buffer;
    setkey = (atd_key_t *)(buffer + sizeof(atd_hdr_t));

    atd_trace(ATD_DEBUG_CFG, ("radio %d vap %d ", 
              atdhdr->wifi_index, atdhdr->vap_index));

    wifisc = atsc->wifi_list[atdhdr->wifi_index];

    if(wifisc == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    vap    = wifisc->vap_list[atdhdr->vap_index];

    if(vap == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    vzalloc(sizeof(struct ieee80211req_key), ik);
    if(ik == NULL){
        atd_trace(ATD_DEBUG_CFG, ("\n %s : allocation failed\n", __FUNCTION__));
        status = A_STATUS_ENOMEM;
        goto fail; 
    }

    ik->ik_type = setkey->type;
    ik->ik_pad = setkey->pad;
    ik->ik_keyix = ntohs(setkey->keyix);
    ik->ik_keylen = setkey->keylen;
    ik->ik_flags = setkey->flags;
    memcpy(ik->ik_macaddr, setkey->macaddr, ACFG_MACADDR_LEN);
    ik->ik_keyrsc = be64_to_cpu(setkey->keyrsc);
    ik->ik_keytsc = be64_to_cpu(setkey->keytsc);
    memcpy(ik->ik_keydata, setkey->keydata, ACFG_KEYDATA_LEN);

    info.cmd = IEEE80211_IOCTL_SETKEY;
    info.flags = 0;
    iwpdata->length = sizeof(struct ieee80211req_key);
    iwpdata->flags = 0;
    iwpdata->pointer = (char *)ik;

    status = netdev_wext_handler(vap->vap_netdev, IEEE80211_IOCTL_SETKEY, &info, 
                                 &iwreqdata, (char *)ik); 
    if(status != A_STATUS_OK){
        atd_trace(ATD_DEBUG_CFG, ("set key set erro \n"));
    }

    vfree(ik);

fail:
    wmi_cmd_rsp(atsc->wmi_handle, cmdid, status, NULL, 0);
}

/** 
 * @brief Get Key
 * 
 * @param ctx
 * @param cmdid
 * @param buffer
 * @param len
 */
void 
atd_tgt_get_key(void *ctx, a_uint16_t cmdid, 
                  a_uint8_t *buffer, a_int32_t len)
{
    a_status_t status = A_STATUS_OK;
    atd_tgt_dev_t *atsc = (atd_tgt_dev_t *)ctx;
    atd_tgt_wifi_t *wifisc =NULL;
    atd_tgt_vap_t * vap;
    atd_hdr_t   *atdhdr;
    struct iwreq req = {{{0}}};
    atd_key_t *getkey, *result = NULL;
    struct ieee80211req_key *ik = NULL;
    
    atd_trace(ATD_DEBUG_FUNCTRACE, ("cmd %d    ",\
                cmdid));

    atd_assert(len >= (sizeof(atd_hdr_t) + sizeof(atd_key_t)));
  
    atdhdr = (atd_hdr_t *) buffer;
    getkey = (atd_key_t *)(buffer + sizeof(atd_hdr_t)) ;

    atd_trace(ATD_DEBUG_FUNCTRACE, (" radio %d vap %d ", \
                                 atdhdr->wifi_index, atdhdr->vap_index));

    wifisc = atsc->wifi_list[atdhdr->wifi_index];
    if(wifisc == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    vap    = wifisc->vap_list[atdhdr->vap_index];
    if(vap == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    vzalloc(sizeof(struct ieee80211req_key), ik);
    if(ik == NULL){
        atd_trace(ATD_DEBUG_CFG, ("\n %s : allocation failed\n", __FUNCTION__));
        status = A_STATUS_ENOMEM;
        goto fail; 
    }

    vzalloc(sizeof(atd_key_t), result);
    if(result == NULL){
        atd_trace(ATD_DEBUG_CFG, ("\n %s : allocation failed\n", __FUNCTION__));
        status = A_STATUS_ENOMEM;
        goto fail; 
    }

    ik->ik_type = getkey->type;
    ik->ik_pad = getkey->pad;
    ik->ik_keyix = ntohs(getkey->keyix);
    ik->ik_keylen = getkey->keylen;
    ik->ik_flags = getkey->flags;
    memcpy(ik->ik_macaddr, getkey->macaddr, ACFG_MACADDR_LEN);
    ik->ik_keyrsc = be64_to_cpu(getkey->keyrsc);
    ik->ik_keytsc = be64_to_cpu(getkey->keytsc);
    memcpy(ik->ik_keydata, getkey->keydata, ACFG_KEYDATA_LEN);

    req.u.data.pointer  = (a_uint8_t *)ik;
    req.u.data.length   = sizeof(struct ieee80211req_key);
    req.u.data.flags    = 1;

#define IOCTL_GET_KEY (SIOCDEVPRIVATE + 3)
    status = netdev_ioctl(vap->vap_netdev, (struct ifreq *)&req, IOCTL_GET_KEY);

    result->type = ik->ik_type;
    result->pad = ik->ik_pad;
    result->keyix = htons(ik->ik_keyix);
    result->keylen = ik->ik_keylen;
    result->flags = ik->ik_flags;
    memcpy(result->macaddr, ik->ik_macaddr, ACFG_MACADDR_LEN);
    result->keyrsc = cpu_to_be64(ik->ik_keyrsc);
    result->keytsc = cpu_to_be64(ik->ik_keytsc);
    memcpy(result->keydata, ik->ik_keydata, ACFG_KEYDATA_LEN);

    req.u.data.length = sizeof(atd_key_t);
   
    atd_trace(ATD_DEBUG_FUNCTRACE,("getkey %p \n", getkey));
 
fail:
    wmi_cmd_rsp(atsc->wmi_handle, cmdid, status, (a_uint8_t *)result, 
                req.u.data.length);
    if (result)
        vfree(result);
    if(ik)
        vfree(ik);
}


void 
atd_tgt_get_sta_stats(void *ctx, a_uint16_t cmdid, 
                  a_uint8_t *buffer, a_int32_t len)
{
    a_status_t status = A_STATUS_OK;
    atd_tgt_dev_t *atsc = (atd_tgt_dev_t *)ctx;
    atd_tgt_wifi_t *wifisc =NULL;
    atd_tgt_vap_t * vap;
    atd_hdr_t   *atdhdr;
    void *statsbuff = NULL;
    void *p_stats;
    struct iwreq req = {{{0}}};
    uint32_t pldlen = 0;
    
    atd_trace(ATD_DEBUG_FUNCTRACE, ("cmd %d    ",\
                cmdid));

    atd_assert(len >= (sizeof(atd_hdr_t) + sizeof(a_uint32_t)));

    atdhdr     = (atd_hdr_t *) buffer;
    p_stats    = (acfg_opaque_t *)(buffer + sizeof(atd_hdr_t)) ;
    pldlen     = *(a_uint32_t *)p_stats;
    pldlen     =   ntohl(pldlen);

    atd_assert(len >= (sizeof(atd_hdr_t) + sizeof(a_uint32_t) + VALID_PAYLOAD_SIZE));

    atd_trace(ATD_DEBUG_FUNCTRACE, (" radio %d vap %d ", \
                                 atdhdr->wifi_index, atdhdr->vap_index));

    wifisc = atsc->wifi_list[atdhdr->wifi_index];
    if(wifisc == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    vap    = wifisc->vap_list[atdhdr->vap_index];
    if(vap == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    if(pldlen > 0){
        vzalloc(pldlen, statsbuff);

        if(statsbuff == NULL){
            atd_trace(ATD_DEBUG_CFG, ("\n %s : allocation failed\n", __FUNCTION__));
            status = A_STATUS_ENOMEM;
            goto fail; 
        }

        memcpy((a_uint8_t *)statsbuff, (char *)p_stats + sizeof(a_uint32_t), VALID_PAYLOAD_SIZE);
    }

    req.u.data.pointer  = (a_uint8_t *)statsbuff;
    req.u.data.length   = pldlen; 
    req.u.data.flags    = 1;

#define IOCTL_STA_STATS (SIOCDEVPRIVATE + 5)
    status = netdev_ioctl(vap->vap_netdev, (struct ifreq *)&req, IOCTL_STA_STATS);


    atd_trace(ATD_DEBUG_FUNCTRACE,("stats %p \n", p_stats));

fail:
    wmi_cmd_rsp(atsc->wmi_handle, cmdid, status, (a_uint8_t *)statsbuff, 
                req.u.data.length);
    if(statsbuff)
        vfree(statsbuff);
}


/**
 * ATD Events 
 */
#define PROTO_EVENT(name)   \
    static void atd_tgt_##name(struct net_device  *dev, \
                                union iwreq_data  *wreq, \
                                const int8_t  *buf)
PROTO_EVENT(noevent);
PROTO_EVENT(scan_done);
PROTO_EVENT(assoc_ap);
PROTO_EVENT(assoc_sta);
PROTO_EVENT(chan_start);
PROTO_EVENT(chan_end);
PROTO_EVENT(rx_mgmt);
PROTO_EVENT(sent_action);
PROTO_EVENT(if_running_sta);
PROTO_EVENT(if_not_running_sta);
PROTO_EVENT(leave_ap);
PROTO_EVENT(gen_ie);
//PROTO_EVENT(assoc_req_ie);
PROTO_EVENT(iw_custom);
PROTO_EVENT(iw_assocreqie);

PROTO_EVENT(iw_auth_comp_ap);
PROTO_EVENT(iw_assoc_comp_ap);
PROTO_EVENT(iw_deauth_comp_ap);
PROTO_EVENT(iw_auth_ind_ap);
PROTO_EVENT(iw_auth_comp_sta);
PROTO_EVENT(iw_assoc_comp_sta);
PROTO_EVENT(iw_deauth_comp_sta);
PROTO_EVENT(iw_disassoc_comp_sta);
PROTO_EVENT(iw_auth_ind_sta);
PROTO_EVENT(iw_deauth_ind_sta);
PROTO_EVENT(iw_assoc_ind_sta);
PROTO_EVENT(iw_disassoc_ind_sta);
PROTO_EVENT(iw_deauth_ind_ap);
PROTO_EVENT(iw_disassoc_ind_ap);
PROTO_EVENT(iw_wapi);

typedef void (*__atd_event_fn_t)(struct net_device *dev, 
                                 union iwreq_data  *wreq, const int8_t  *buf);

typedef struct custom_event {
    const a_uint8_t    *name;
    a_uint32_t          len;
    wmi_id_t            wmi_event;
} custom_event_t;


#define EVENT_NUM(x)        (x - WMI_EVENT_START)
#define EVENT_IDX(x)        [EVENT_NUM(x)]

const __atd_event_fn_t     atd_events[] = {
    EVENT_IDX(WMI_EVENT_START)          = atd_tgt_noevent,
    EVENT_IDX(WMI_EVENT_SCAN_DONE)      = atd_tgt_scan_done,
    EVENT_IDX(WMI_EVENT_ASSOC_AP)       = atd_tgt_assoc_ap,
    EVENT_IDX(WMI_EVENT_ASSOC_STA)      = atd_tgt_assoc_sta,
    EVENT_IDX(WMI_EVENT_IF_RUNNING_STA)      = atd_tgt_if_running_sta,
    EVENT_IDX(WMI_EVENT_CHAN_START)     = atd_tgt_chan_start,
    EVENT_IDX(WMI_EVENT_CHAN_END)       = atd_tgt_chan_end,
    EVENT_IDX(WMI_EVENT_RX_MGMT)        = atd_tgt_rx_mgmt,
    EVENT_IDX(WMI_EVENT_SENT_ACTION)    = atd_tgt_sent_action,
    EVENT_IDX(WMI_EVENT_IF_NOT_RUNNING_STA) = atd_tgt_if_not_running_sta,
    EVENT_IDX(WMI_EVENT_LEAVE_AP)       = atd_tgt_leave_ap,
    EVENT_IDX(WMI_EVENT_GEN_IE)         = atd_tgt_gen_ie,
    //EVENT_IDX(WMI_EVENT_ASSOC_REQ_IE)   = atd_tgt_assoc_req_ie,
    EVENT_IDX(WMI_EVENT_IW_CUSTOM)      = atd_tgt_iw_custom,
    EVENT_IDX(WMI_EVENT_IW_ASSOCREQIE)  = atd_tgt_iw_assocreqie,

	EVENT_IDX(WMI_EVENT_AUTH_COMP_AP)	= atd_tgt_iw_auth_comp_ap,
	EVENT_IDX(WMI_EVENT_ASSOC_COMP_AP)	= atd_tgt_iw_assoc_comp_ap,
	EVENT_IDX(WMI_EVENT_DEAUTH_COMP_AP)	= atd_tgt_iw_deauth_comp_ap,
	EVENT_IDX(WMI_EVENT_AUTH_IND_AP)	= atd_tgt_iw_auth_ind_ap,
	EVENT_IDX(WMI_EVENT_AUTH_COMP_STA)	= atd_tgt_iw_auth_comp_sta,
	EVENT_IDX(WMI_EVENT_ASSOC_COMP_STA)	= atd_tgt_iw_assoc_comp_sta,
	EVENT_IDX(WMI_EVENT_DEAUTH_COMP_STA)	= atd_tgt_iw_deauth_comp_sta,
	EVENT_IDX(WMI_EVENT_DISASSOC_COMP_STA)	= atd_tgt_iw_disassoc_comp_sta,
	EVENT_IDX(WMI_EVENT_AUTH_IND_STA)	= atd_tgt_iw_auth_ind_sta,
	EVENT_IDX(WMI_EVENT_DEAUTH_IND_STA)	= atd_tgt_iw_deauth_ind_sta,
	EVENT_IDX(WMI_EVENT_ASSOC_IND_STA)	= atd_tgt_iw_assoc_ind_sta,
	EVENT_IDX(WMI_EVENT_DISASSOC_IND_STA)	= atd_tgt_iw_disassoc_ind_sta,
	EVENT_IDX(WMI_EVENT_DEAUTH_IND_AP)	= atd_tgt_iw_deauth_ind_ap,
	EVENT_IDX(WMI_EVENT_DISASSOC_IND_AP)	= atd_tgt_iw_disassoc_ind_ap,
	EVENT_IDX(WMI_EVENT_WAPI)		= atd_tgt_iw_wapi,
};


#define __BIND_EV(str, event)   {       \
    .name       = str,                  \
    .len        = (sizeof(str) - 1),    \
    .wmi_event  = event,                \
}


custom_event_t      iw_custom_events[] = {
    __BIND_EV("MLME-REPLAYFAILURE.indication", WMI_EVENT_START),
    __BIND_EV("MLME-MICHAELMICFAILURE.indication", WMI_EVENT_IW_CUSTOM),
    __BIND_EV("PUSH-BUTTON.indication", WMI_EVENT_START),
    __BIND_EV("Manage.prob_req", WMI_EVENT_IW_ASSOCREQIE),
    __BIND_EV("Manage.auth", WMI_EVENT_IW_ASSOCREQIE),
    __BIND_EV("Manage.assoc_req", WMI_EVENT_IW_ASSOCREQIE),
    __BIND_EV("Manage.action", WMI_EVENT_IW_ASSOCREQIE),
};

#define iw_custom_events_sz  \
    (sizeof(iw_custom_events)/sizeof(iw_custom_events[0]))

/** 
 * @brief Convert a Custom IW event into a WMI_EVENT
 * 
 * @param extra
 * 
 * @return 
 */
wmi_id_t
custom_to_wmi(union iwreq_data      *wreq, const int8_t  *extra)
{
    custom_event_t  *cust;
    uint16_t         i;

    if (wreq->data.flags == IEEE80211_EV_CHAN_START)
        return WMI_EVENT_CHAN_START;
    else if (wreq->data.flags == IEEE80211_EV_CHAN_END)
        return WMI_EVENT_CHAN_END;
    else if (wreq->data.flags == IEEE80211_EV_RX_MGMT)
        return WMI_EVENT_RX_MGMT;
    else if (wreq->data.flags == IEEE80211_EV_P2P_SEND_ACTION_CB)
        return WMI_EVENT_SENT_ACTION;
    else if (wreq->data.flags == IEEE80211_EV_AUTH_COMPLETE_AP)
        return WMI_EVENT_AUTH_COMP_AP;
    else if (wreq->data.flags == IEEE80211_EV_ASSOC_COMPLETE_AP)
        return WMI_EVENT_ASSOC_COMP_AP;
    else if (wreq->data.flags == IEEE80211_EV_DEAUTH_COMPLETE_AP)
        return WMI_EVENT_DEAUTH_COMP_AP;
    else if (wreq->data.flags == IEEE80211_EV_AUTH_IND_AP)
        return WMI_EVENT_AUTH_IND_AP;
    else if (wreq->data.flags == IEEE80211_EV_AUTH_COMPLETE_STA)
        return WMI_EVENT_AUTH_COMP_STA;
    else if (wreq->data.flags == IEEE80211_EV_ASSOC_COMPLETE_STA)
        return WMI_EVENT_ASSOC_COMP_STA;
    else if (wreq->data.flags == IEEE80211_EV_DEAUTH_COMPLETE_STA)
        return WMI_EVENT_DEAUTH_COMP_STA;
    else if (wreq->data.flags == IEEE80211_EV_DISASSOC_COMPLETE_STA)
        return WMI_EVENT_DISASSOC_COMP_STA;
    else if (wreq->data.flags == IEEE80211_EV_AUTH_IND_STA)
        return WMI_EVENT_AUTH_IND_STA;
    else if (wreq->data.flags == IEEE80211_EV_DEAUTH_IND_STA)
        return WMI_EVENT_DEAUTH_IND_STA;
    else if (wreq->data.flags == IEEE80211_EV_ASSOC_IND_STA)
        return WMI_EVENT_ASSOC_IND_STA;
    else if (wreq->data.flags == IEEE80211_EV_DISASSOC_IND_STA)
        return WMI_EVENT_DISASSOC_IND_STA;
    else if (wreq->data.flags == IEEE80211_EV_DEAUTH_IND_AP)
        return WMI_EVENT_DEAUTH_IND_AP;
    else if (wreq->data.flags == IEEE80211_EV_DISASSOC_IND_AP)
        return WMI_EVENT_DISASSOC_IND_AP;
    else if (wreq->data.flags == IEEE80211_EV_WAPI)
        return WMI_EVENT_WAPI;
		
    if(!extra){
        if (wreq->data.flags == IEEE80211_EV_IF_RUNNING){
            return WMI_EVENT_IF_RUNNING_STA;
        } else if (wreq->data.flags == IEEE80211_EV_IF_NOT_RUNNING){
            return WMI_EVENT_IF_NOT_RUNNING_STA;
        } else
            return WMI_EVENT_START;
    }

    for (i = 0; i < iw_custom_events_sz; i++) {
        cust = &iw_custom_events[i];

        if(!strncmp(extra, cust->name, cust->len))
            return cust->wmi_event;
    }
    return WMI_EVENT_START;
}

/** 
 * @brief Convert the IW Command into a WMI event
 * 
 * @param iw_cmd    (IW Command)
 * @param extra     (Command specific data)
 * 
 * @return 
 */
static inline wmi_id_t
iw_to_wmi_event(int iw_cmd, union iwreq_data      *wreq, const int8_t  *extra)
{
    switch (iw_cmd) {
        case SIOCGIWSCAN:       return WMI_EVENT_SCAN_DONE;
        case IWEVREGISTERED:    return WMI_EVENT_ASSOC_AP;
        case SIOCGIWAP:         return WMI_EVENT_ASSOC_STA;
        case IWEVCUSTOM:        return custom_to_wmi(wreq, extra);
        case IWEVEXPIRED:       return WMI_EVENT_LEAVE_AP;
        //case IWEVASSOCREQIE:    return WMI_EVENT_ASSOC_REQ_IE;
        case IWEVGENIE:         return WMI_EVENT_GEN_IE;
        case IWEVASSOCREQIE:    return custom_to_wmi(wreq, extra);
        default:                return WMI_EVENT_START;
    }
}

/** 
 * @brief ATD Event handler function called from the WLAN driver
 * 
 * @param dev   (Netdevice handle)
 * @param cmd   (IW Command)
 * @param wreq  (Wireless request)
 * @param extra (Custom or Extra IW contents)
 */
void
atd_event_handler(struct net_device     *dev, unsigned int  cmd,
			      union iwreq_data      *wreq, char *extra)
{
    a_uint32_t  index;
    
    atd_trace(ATD_DEBUG_FUNCTRACE, (".START"));

    index = EVENT_NUM(iw_to_wmi_event(cmd, wreq, extra));
    
    atd_trace(ATD_DEBUG_CFG, ("index = %x",index));

    atd_trace(ATD_DEBUG_CFG, (">>> Events %d\n",index));
    if(atd_events[index]) atd_events[index](dev, wreq, extra); 

    wireless_send_event(dev, cmd, wreq, extra);

    atd_trace(ATD_DEBUG_FUNCTRACE, (".END"));
}
EXPORT_SYMBOL(atd_event_handler);

/** 
 * @brief 
 * 
 * @param dev
 * @param wreq
 * @param buf
 */
static void 
atd_tgt_noevent(struct net_device  *dev, union iwreq_data  *wreq, 
               const int8_t   *buf)
{
    atd_trace(ATD_DEBUG_CFG, ("%s:No event mapping with WMI \n", __func__));
//    dump_stack();
}


/** 
 * @brief 
 * 
 * @param dev
 * @param dev_sc
 * @param skb
 * @param payload_len
 * 
 * @return 
 */
atd_tgt_vap_t *
atd_tgt_vap_event_alloc(struct net_device  *dev, atd_tgt_dev_t  *dev_sc,
                        struct sk_buff **skb, a_uint32_t  payload_len)
{
    atd_tgt_vap_t      *vap;
    atd_hdr_t          *hdr;

#if (TARGET_STANDALONE == 1)
	return NULL;
#endif
    vap = atd_tgt_find_vap(dev_sc, dev);
    if(!vap){
        atd_trace(ATD_DEBUG_CFG, ("%s:unknown interface %s", __func__, dev->name));
        return NULL;
    }
    
    payload_len += sizeof(struct atd_hdr);

    *skb = wmi_alloc_event(dev_sc->wmi_handle, payload_len);
    if (skb == NULL) {
        return NULL;
    }

    hdr = (void *)skb_put(*skb, sizeof(struct atd_hdr));
    
    hdr->wifi_index = vap->wifisc->wifi_index;
    hdr->vap_index  = vap->vap_index;

    return vap;
}

/** 
 * @brief 
 * 
 * @param dev
 * @param dev_sc
 * @param skb
 * @param payload_len
 * 
 * @return 
 */
atd_tgt_wifi_t *
atd_tgt_wifi_event_alloc(struct net_device  *dev, atd_tgt_dev_t  *dev_sc,
                         struct sk_buff **skb, a_uint32_t  payload_len)
{
    atd_tgt_wifi_t      *wifi;
    atd_hdr_t           *hdr;

    wifi = atd_tgt_find_wifi(dev_sc, dev);
    if(!wifi){
        atd_trace(ATD_DEBUG_CFG, ("%s:unknown interface %s", __func__, dev->name));
        return NULL;
    }
    
    payload_len += sizeof(struct atd_hdr);

    *skb = wmi_alloc_event(dev_sc->wmi_handle, payload_len);
    if(!(*skb)){
	atd_trace(ATD_DEBUG_CFG, ("%s:skb allocation failed %s", __func__, dev->name));
	return NULL;
    }

    hdr = (void *)skb_put(*skb, sizeof(struct atd_hdr));
    
    hdr->wifi_index = wifi->wifi_index;
    hdr->vap_index  = 0xFF;

    return wifi;
}

/** 
 * @brief Send a VAP Specific event
 * 
 * @param vap
 * @param event
 */
void
atd_tgt_vap_event(atd_tgt_vap_t  *vap, wmi_id_t  event)
{
    a_uint32_t          payload_len;
    struct sk_buff     *skb;
    atd_hdr_t          *hdr;
    atd_tgt_dev_t      *dev_sc = g_atd_dev;

    payload_len = sizeof(struct atd_hdr);

    skb = wmi_alloc_event(dev_sc->wmi_handle, payload_len);
    
    hdr = (void *)skb_put(skb, payload_len);
    
    hdr->wifi_index = vap->wifisc->wifi_index;
    hdr->vap_index  = vap->vap_index;

    wmi_send_event(dev_sc->wmi_handle, skb, event);
}

/** 
 * @brief Send a WIFI Specific event
 * 
 * @param vap
 * @param event
 */
void
atd_tgt_wifi_event(atd_tgt_wifi_t  *wifi, wmi_id_t  event)
{
    a_uint32_t          payload_len;
    struct sk_buff     *skb;
    atd_hdr_t          *hdr;
    atd_tgt_dev_t      *dev_sc = g_atd_dev;

    payload_len = sizeof(struct atd_hdr);

    skb = wmi_alloc_event(dev_sc->wmi_handle, payload_len);
    
    hdr = (void *)skb_put(skb, payload_len);
    
    hdr->wifi_index = wifi->wifi_index;
    hdr->vap_index  = 0xFF;

    wmi_send_event(dev_sc->wmi_handle, skb, event);
}
/** 
 * @brief 
 * 
 * @param dev
 * @param wreq
 * @param buf
 */
void 
atd_tgt_create_vap(atd_tgt_vap_t  *vap, wmi_id_t  event)
{
    atd_tgt_dev_t      *dev_sc = g_atd_dev;
    atd_create_vap_t     *payload;
    a_uint32_t          payload_len = sizeof(*payload);
    struct sk_buff     *skb = NULL;

    if (!atd_tgt_vap_event_alloc(vap->vap_netdev, dev_sc, &skb, payload_len))
        goto done;

    payload = (void *)skb_put(skb, payload_len);

    payload->wifi_index = vap->wifisc->wifi_index;
    payload->vap_index = vap->vap_index;
    strncpy(payload->if_name, vap->vap_netdev->name, ACFG_MAX_IFNAME);
    memcpy(payload->mac_addr, vap->mac_addr, ACFG_MACADDR_LEN);
    
    wmi_send_event(dev_sc->wmi_handle, skb, event);
done:
    return;
} 

/**
 * @brief
 *
 * @param wifi 
 * @param event 
 */
void
atd_tgt_restore_wifi(atd_tgt_wifi_t  *wifi, wmi_id_t  event)
{
    atd_tgt_dev_t      *dev_sc = g_atd_dev;
    atd_restore_wifi_t *payload;
    a_uint32_t          payload_len = sizeof(*payload);
    struct sk_buff     *skb = NULL;

    if (!atd_tgt_wifi_event_alloc(wifi->netdev, dev_sc, &skb, payload_len))
        goto done;
    
    payload = (void *)skb_put(skb, payload_len);

    payload->wifi_index = wifi->wifi_index;
    strncpy(payload->if_name, wifi->netdev->name, ACFG_MAX_IFNAME);
    memcpy(payload->mac_addr, wifi->mac_addr, ACFG_MACADDR_LEN);

    wmi_send_event(dev_sc->wmi_handle, skb, event);
done:
    return;
}

void
atd_proxy_arp_send(struct sk_buff *wbuf)
{
    atd_tgt_dev_t       *dev_sc = g_atd_dev;
    a_uint8_t           *payload;
    a_uint16_t          wbuf_len = wbuf->len;
    /* payload: skb_len + sk_buff */
    a_uint32_t          payload_len = sizeof(a_uint16_t) + wbuf_len;
    struct net_device   *dev   = wbuf->dev;
    struct sk_buff      *skb   = NULL;
    atd_tgt_vap_t       *vap   = NULL;

    atd_trace(ATD_DEBUG_FUNCTRACE,(" Start"));

    if ( (vap = atd_tgt_vap_event_alloc(dev, dev_sc, &skb, payload_len)) == NULL )
        goto done;

    payload = (a_uint8_t *)skb_put(skb, payload_len);

    memcpy(payload, (a_uint8_t *)&wbuf_len, sizeof(a_uint16_t));
    payload += sizeof(a_uint16_t);
    memcpy(payload, (a_uint8_t *)wbuf->data, wbuf->len);
    wmi_send_event(dev_sc->wmi_handle, skb, WMI_EVENT_PROXYARP_SEND);

done:
    return;    
} 

EXPORT_SYMBOL(atd_proxy_arp_send);
void atd_convert_spectral_samp_msg_hton(atd_spectral_samp_msg_t *out, 
        atd_spectral_samp_msg_t *in)
{
    int i;

    out->signature = htonl(in->signature);
    out->freq      = htons(in->freq);
    out->freq_loading = htons(in->freq_loading);
    out->dcs_enabled = htons(in->dcs_enabled);
    out->int_type = htonl(in->int_type);
    memcpy(out->macaddr, in->macaddr, 6);

    out->samp_data.spectral_data_len = htons(in->samp_data.spectral_data_len);
    out->samp_data.spectral_rssi = htons(in->samp_data.spectral_rssi);
    out->samp_data.spectral_combined_rssi = in->samp_data.spectral_combined_rssi;
    out->samp_data.spectral_upper_rssi = in->samp_data.spectral_upper_rssi;
    out->samp_data.spectral_lower_rssi = in->samp_data.spectral_lower_rssi;
    memcpy(out->samp_data.spectral_chain_ctl_rssi, in->samp_data.spectral_chain_ctl_rssi, MAX_SPECTRAL_CHAINS);
    memcpy(out->samp_data.spectral_chain_ext_rssi, in->samp_data.spectral_chain_ext_rssi, MAX_SPECTRAL_CHAINS);
    out->samp_data.spectral_max_scale = in->samp_data.spectral_max_scale;
    out->samp_data.spectral_bwinfo = htons(in->samp_data.spectral_bwinfo);
    out->samp_data.spectral_tstamp = htonl(in->samp_data.spectral_tstamp);
    out->samp_data.spectral_max_index = htons(in->samp_data.spectral_max_index);
    out->samp_data.spectral_max_mag = htons(in->samp_data.spectral_max_mag);
    out->samp_data.spectral_max_exp = in->samp_data.spectral_max_exp;
    out->samp_data.spectral_last_tstamp = htonl(in->samp_data.spectral_last_tstamp);
    out->samp_data.spectral_upper_max_index = htons(in->samp_data.spectral_upper_max_index);
    out->samp_data.spectral_lower_max_index = htons(in->samp_data.spectral_lower_max_index);
    out->samp_data.spectral_nb_upper = in->samp_data.spectral_nb_upper;
    out->samp_data.spectral_nb_lower = in->samp_data.spectral_nb_lower;

    out->samp_data.classifier_params.spectral_20_40_mode =
        htonl(in->samp_data.classifier_params.spectral_20_40_mode);
    out->samp_data.classifier_params.spectral_dc_index =
        htonl(in->samp_data.classifier_params.spectral_dc_index);
    out->samp_data.classifier_params.spectral_dc_in_mhz =
        htonl(in->samp_data.classifier_params.spectral_dc_in_mhz);
    out->samp_data.classifier_params.upper_chan_in_mhz =
        htonl(in->samp_data.classifier_params.upper_chan_in_mhz);
    out->samp_data.classifier_params.lower_chan_in_mhz =
        htonl(in->samp_data.classifier_params.lower_chan_in_mhz);

    out->samp_data.bin_pwr_count = htons(in->samp_data.bin_pwr_count);
    memcpy(out->samp_data.bin_pwr, in->samp_data.bin_pwr, MAX_NUM_BINS);

    out->samp_data.interf_list.count = htons(in->samp_data.interf_list.count);

    for (i = 0; i < MAX_INTERF; i++)
    {
        out->samp_data.interf_list.interf[i].interf_type =
            in->samp_data.interf_list.interf[i].interf_type;
        out->samp_data.interf_list.interf[i].interf_min_freq =
            htons(in->samp_data.interf_list.interf[i].interf_min_freq);
        out->samp_data.interf_list.interf[i].interf_max_freq =
            htons(in->samp_data.interf_list.interf[i].interf_max_freq);
    }

    out->samp_data.noise_floor = htons(in->samp_data.noise_floor);
    out->samp_data.ch_width = htonl(in->samp_data.ch_width);
}

void
atd_spectral_msg_send(struct net_device *dev, 
        SPECTRAL_SAMP_MSG *msg, 
        a_uint16_t msg_len)
{
    atd_tgt_dev_t       *dev_sc = g_atd_dev;
    atd_tgt_wifi_t      *wifi;
    atd_hdr_t           *hdr;
    atd_spectral_samp_msg_t *payload;
    struct sk_buff      *skb   = NULL;

    atd_trace(ATD_DEBUG_FUNCTRACE,(" Start"));

    skb = wmi_alloc_event(dev_sc->wmi_handle, 
            sizeof(struct atd_hdr) + msg_len);
    hdr = (void *)skb_put(skb, 
            sizeof(struct atd_hdr) + msg_len);
    payload = (atd_spectral_samp_msg_t *)(hdr + 1);

    wifi = atd_tgt_find_wifi(dev_sc, dev);

    hdr->wifi_index = wifi->wifi_index;
    hdr->vap_index  = 0xFF;

    /* Its time todo hton convertion */
    atd_convert_spectral_samp_msg_hton(payload, msg);

    wmi_send_event(dev_sc->wmi_handle, skb, WMI_EVENT_SPECTRAL_MSG_SEND);
}

EXPORT_SYMBOL(atd_spectral_msg_send);

/**
 * @brief
 *
 * @param out
 * @param in
 */
void atd_convert_bsteer_event_hton(atd_bsteering_event_t *out,
        struct ath_netlink_bsteering_event *in)
{
    out->type = htonl(in->type);
    out->band_index = htonl(in->band_index);

    switch(in->type)
    {
        case ATH_EVENT_BSTEERING_CHAN_UTIL:
            out->data.bs_chan_util.utilization =
                in->data.bs_chan_util.utilization;
            break;
        case ATH_EVENT_BSTEERING_PROBE_REQ:
            memcpy(out->data.bs_probe.sender_addr,
                    in->data.bs_probe.sender_addr,
                    ACFG_MACADDR_LEN);
            out->data.bs_probe.rssi = in->data.bs_probe.rssi;
            break;
        case ATH_EVENT_BSTEERING_NODE_AUTHORIZED:
            memcpy(out->data.bs_node_authorized.client_addr,
                    in->data.bs_node_authorized.client_addr,
                    ACFG_MACADDR_LEN);
            break;
        case ATH_EVENT_BSTEERING_TX_AUTH_FAIL:
            memcpy(out->data.bs_auth.client_addr,
                    in->data.bs_auth.client_addr,
                    ACFG_MACADDR_LEN);
            out->data.bs_auth.rssi = in->data.bs_auth.rssi;
            break;
        case ATH_EVENT_BSTEERING_CLIENT_ACTIVITY_CHANGE:
            memcpy(out->data.bs_activity_change.client_addr,
                    in->data.bs_activity_change.client_addr,
                    ACFG_MACADDR_LEN);
            out->data.bs_activity_change.activity =
                in->data.bs_activity_change.activity;
            break;
        case ATH_EVENT_BSTEERING_CLIENT_RSSI_CROSSING:
            memcpy(out->data.bs_rssi_xing.client_addr,
                    in->data.bs_rssi_xing.client_addr,
                    ACFG_MACADDR_LEN);
            out->data.bs_rssi_xing.rssi = in->data.bs_rssi_xing.rssi;
            out->data.bs_rssi_xing.inact_rssi_xing =
                htonl(in->data.bs_rssi_xing.inact_rssi_xing);
            out->data.bs_rssi_xing.low_rssi_xing =
                htonl(in->data.bs_rssi_xing.low_rssi_xing);
            break;
        case ATH_EVENT_BSTEERING_CLIENT_RSSI_MEASUREMENT:
            memcpy(out->data.bs_rssi_measurement.client_addr,
                    in->data.bs_rssi_measurement.client_addr,
                    ACFG_MACADDR_LEN);
            out->data.bs_rssi_measurement.rssi =
                in->data.bs_rssi_measurement.rssi;
            break;
        case ATH_EVENT_BSTEERING_DBG_CHAN_UTIL:
        case ATH_EVENT_BSTEERING_DBG_RSSI:
        default:
            break;
    }
}

/**
 * @brief
 *
 * @param netdev
 * @param event
 * @param msg_len
 */
void
atd_bsteer_event_send(struct net_device *dev,
        struct ath_netlink_bsteering_event *event,
        a_uint16_t msg_len)
{
    atd_tgt_dev_t       *dev_sc = g_atd_dev;
    atd_tgt_wifi_t      *wifi;
    atd_hdr_t           *hdr;
    atd_bsteering_event_t *payload;
    struct sk_buff      *skb   = NULL;

    atd_trace(ATD_DEBUG_FUNCTRACE,(" Start"));

    skb = wmi_alloc_event(dev_sc->wmi_handle,
            sizeof(struct atd_hdr) + msg_len);
    hdr = (void *)skb_put(skb,
            sizeof(struct atd_hdr) + msg_len);
    payload = (atd_bsteering_event_t *)(hdr + 1);

    wifi = atd_tgt_find_wifi(dev_sc, dev);

    hdr->wifi_index = wifi->wifi_index;
    hdr->vap_index  = 0xFF;

    /* Its time todo hton convertion */
    atd_convert_bsteer_event_hton(payload, event);

    wmi_send_event(dev_sc->wmi_handle, skb, WMI_EVENT_BSTEER_SEND);
}
EXPORT_SYMBOL(atd_bsteer_event_send);

/** 
 * @brief 
 * 
 * @param dev
 * @param wreq
 * @param buf
 */
static void 
atd_tgt_chan_start(struct net_device  *dev, union iwreq_data  *wreq, 
                  const int8_t   *buf)
{
    atd_tgt_dev_t      *dev_sc = g_atd_dev;
    a_uint32_t          payload_len = sizeof(atd_chan_start_t);
    struct sk_buff     *skb = NULL;
    atd_chan_start_t   *payload;
    acfg_chan_start_t  *chan_start = (acfg_chan_start_t *)buf;
    
    atd_trace(ATD_DEBUG_FUNCTRACE, (".START"));

    if (!atd_tgt_vap_event_alloc(dev, dev_sc, &skb, payload_len))
        goto done;

    payload = (atd_chan_start_t *)skb_put(skb, payload_len);

    payload->freq = htonl(chan_start->freq);
    payload->duration = htonl(chan_start->duration);
    payload->req_id = htonl(chan_start->req_id);

    atd_trace(ATD_DEBUG_FUNCTRACE, (".END"));

    wmi_send_event(dev_sc->wmi_handle, skb, WMI_EVENT_CHAN_START);
done:
    return;

} 

/** 
 * @brief 
 * 
 * @param dev
 * @param wreq
 * @param buf
 */
static void 
atd_tgt_chan_end(struct net_device  *dev, union iwreq_data  *wreq, 
                  const int8_t   *buf)
{
    atd_tgt_dev_t      *dev_sc = g_atd_dev;
    a_uint32_t          payload_len = sizeof(atd_chan_end_t);
    struct sk_buff     *skb = NULL;
    atd_chan_end_t     *payload;
    acfg_chan_end_t    *chan_end = (acfg_chan_end_t *)buf;
    
    atd_trace(ATD_DEBUG_FUNCTRACE, (".START"));

    if (!atd_tgt_vap_event_alloc(dev, dev_sc, &skb, payload_len))
        goto done;

    payload = (atd_chan_end_t *)skb_put(skb, payload_len);
    
    payload->freq = htonl(chan_end->freq);
    payload->reason = htonl(chan_end->reason);
    payload->duration = htonl(chan_end->duration);
    payload->req_id = htonl(chan_end->req_id);

    atd_trace(ATD_DEBUG_FUNCTRACE, (".END"));

    wmi_send_event(dev_sc->wmi_handle, skb, WMI_EVENT_CHAN_END);
done:
    return;

} 

/** 
 * @brief 
 * 
 * @param dev
 * @param wreq
 * @param buf
 */
static void 
atd_tgt_rx_mgmt(struct net_device  *dev, union iwreq_data  *wreq, 
                  const int8_t   *buf)
{
    atd_tgt_dev_t      *dev_sc = g_atd_dev;
    a_uint32_t          payload_len = wreq->data.length;
    struct sk_buff     *skb = NULL;
    
    atd_trace(ATD_DEBUG_FUNCTRACE, (".START"));

    if (!atd_tgt_vap_event_alloc(dev, dev_sc, &skb, payload_len))
        goto done;

    atd_trace(ATD_DEBUG_FUNCTRACE, (".END"));

    wmi_send_event(dev_sc->wmi_handle, skb, WMI_EVENT_RX_MGMT);
done:
    return;

} 

/** 
 * @brief 
 * 
 * @param dev
 * @param wreq
 * @param buf
 */
static void 
atd_tgt_sent_action(struct net_device  *dev, union iwreq_data  *wreq, 
                  const int8_t   *buf)
{
    atd_tgt_dev_t      *dev_sc = g_atd_dev;
    a_uint32_t          payload_len = wreq->data.length;
    struct sk_buff     *skb = NULL;
    
    atd_trace(ATD_DEBUG_FUNCTRACE, (".START"));

    if (!atd_tgt_vap_event_alloc(dev, dev_sc, &skb, payload_len))
        goto done;

    atd_trace(ATD_DEBUG_FUNCTRACE, (".END"));

    wmi_send_event(dev_sc->wmi_handle, skb, WMI_EVENT_SENT_ACTION);
done:
    return;

} 

/** 
 * @brief 
 * 
 * @param dev
 * @param wreq
 * @param buf
 */
static void
atd_tgt_leave_ap(struct net_device  *dev, union iwreq_data  *wreq, 
                  const int8_t   *buf)
{
    atd_tgt_dev_t      *dev_sc = g_atd_dev;
    struct sk_buff     *skb = NULL;
    void               *payload;
    
    atd_trace(ATD_DEBUG_FUNCTRACE, (".START"));

    if (!atd_tgt_vap_event_alloc(dev, dev_sc, &skb, IEEE80211_ADDR_LEN))
        goto done;

    payload = (void *)skb_put(skb, IEEE80211_ADDR_LEN);

    memcpy(payload, wreq->addr.sa_data, IEEE80211_ADDR_LEN);

    atd_trace(ATD_DEBUG_FUNCTRACE, (".END"));

    wmi_send_event(dev_sc->wmi_handle, skb, WMI_EVENT_LEAVE_AP);

done:
    return;
} 

/** 
 * @brief 
 * 
 * @param dev
 * @param wreq
 * @param buf
 */
static void
atd_tgt_gen_ie(struct net_device  *dev, union iwreq_data  *wreq, 
                  const int8_t   *buf)
{
    atd_tgt_dev_t      *dev_sc = g_atd_dev;
    struct sk_buff     *skb = NULL;
    void               *payload;
    a_uint32_t          payload_len = wreq->data.length;
    
    atd_trace(ATD_DEBUG_FUNCTRACE, (".START"));

    if (!atd_tgt_vap_event_alloc(dev, dev_sc, &skb, payload_len))
        goto done;

    payload = (void *)skb_put(skb, payload_len);

    memcpy(payload, buf, payload_len);

    atd_trace(ATD_DEBUG_FUNCTRACE, (".END"));

    wmi_send_event(dev_sc->wmi_handle, skb, WMI_EVENT_GEN_IE);

done:
    return;
} 

#if 0
/** 
 * @brief 
 * 
 * @param dev
 * @param wreq
 * @param buf
 */
static void
atd_tgt_assoc_req_ie(struct net_device  *dev, union iwreq_data  *wreq, 
                  const int8_t   *buf)
{
    atd_tgt_dev_t      *dev_sc = g_atd_dev;
    struct sk_buff     *skb = NULL;
    void               *payload;
    a_uint32_t          payload_len = wreq->data.length;
    
    atd_trace(ATD_DEBUG_FUNCTRACE, (".START"));

    if (!atd_tgt_vap_event_alloc(dev, dev_sc, &skb, payload_len))
        goto done;

    payload = (void *)skb_put(skb, payload_len);

    memcpy(payload, buf, payload_len);

    atd_trace(ATD_DEBUG_FUNCTRACE, (".END"));

    wmi_send_event(dev_sc->wmi_handle, skb, WMI_EVENT_ASSOC_REQ_IE);

done:
    return;
} 
#endif

/** 
 * @brief 
 * 
 * @param dev
 * @param wreq
 * @param buf
 */
static void 
atd_tgt_assoc_ap(struct net_device  *dev, union iwreq_data  *wreq, 
                 const int8_t   *buf)
{
    atd_tgt_dev_t      *dev_sc = g_atd_dev;
    atd_assoc_ap_t     *payload;
    a_uint32_t          payload_len = sizeof(*payload);
    struct sk_buff     *skb = NULL;

    if (!atd_tgt_vap_event_alloc(dev, dev_sc, &skb, payload_len))
        goto done;

    payload = (void *)skb_put(skb, payload_len);

    payload->status = htonl(ACFG_ASSOC_SUCCESS);
    memcpy(payload->bssid, wreq->addr.sa_data, ACFG_MACADDR_LEN);
    
    wmi_send_event(dev_sc->wmi_handle, skb, WMI_EVENT_ASSOC_AP);
done:
    return;
} 

/** 
 * @brief 
 * 
 * @param dev
 * @param wreq
 * @param buf
 */
static void 
atd_tgt_assoc_sta(struct net_device  *dev, union iwreq_data  *wreq, 
                  const int8_t   *buf)
{
    atd_tgt_dev_t      *dev_sc = g_atd_dev;
    atd_assoc_sta_t    *payload;
    a_uint32_t          payload_len = sizeof(*payload);
    struct sk_buff     *skb = NULL;
    
    atd_trace(ATD_DEBUG_FUNCTRACE, (".START"));

    if (!atd_tgt_vap_event_alloc(dev, dev_sc, &skb, payload_len))
        goto done;

    payload = (void *)skb_put(skb, payload_len);

    payload->status = htonl(ACFG_ASSOC_SUCCESS);
    memcpy(payload->bssid, wreq->addr.sa_data, ACFG_MACADDR_LEN);
    memset(&payload->ssid, 0, sizeof(atd_ssid_t));
    
    atd_trace(ATD_DEBUG_FUNCTRACE, (".END"));

    wmi_send_event(dev_sc->wmi_handle, skb, WMI_EVENT_ASSOC_STA);
done:
    return;

} 

/** 
 * @brief 
 * 
 * @param dev
 * @param wreq
 * @param buf
 */
static void 
atd_tgt_iw_custom(struct net_device  *dev, union iwreq_data  *wreq, 
                  const int8_t   *buf)
{
    atd_tgt_dev_t      *dev_sc = g_atd_dev;
    acfg_wsupp_custom_message_t *payload;
    a_uint32_t          payload_len = sizeof(*payload);
    struct sk_buff     *skb = NULL;
    
    atd_trace(ATD_DEBUG_FUNCTRACE, (".START"));

    /* 
     * buf len should be less than raw message. 
     * This is to leave atleast a byte to null terminate in the following memset.
     * XXX: Assumes the custom message is always a string.
     */
    atd_assert(strlen(buf) < sizeof(payload->raw_message));

    if (!atd_tgt_vap_event_alloc(dev, dev_sc, &skb, payload_len))
        goto done;

    payload = (void *)skb_put(skb, payload_len);

    memset(payload->raw_message,0,sizeof(payload->raw_message));

    memcpy(payload->raw_message, buf, strlen(buf));
    
    atd_trace(ATD_DEBUG_FUNCTRACE, (".END"));

    wmi_send_event(dev_sc->wmi_handle, skb, WMI_EVENT_IW_CUSTOM);
done:
    return;

} 


/** 
 * @brief 
 * IWEVASSOCREQIE is used for sending probe request frames
 * This is HACK to overcome limitations of 256 bytes of
 * IWEVCUSTOM
 * 
 * @param dev
 * @param wreq
 * @param buf
 */

static void 
atd_tgt_iw_assocreqie(struct net_device  *dev, union iwreq_data  *wreq, 
                  const int8_t   *buf)
{
    atd_tgt_dev_t      *dev_sc = g_atd_dev;
    a_uint8_t          *payload;
    struct sk_buff     *skb = NULL;

    atd_trace(ATD_DEBUG_FUNCTRACE, (".START"));

    if (!atd_tgt_vap_event_alloc(dev, dev_sc, &skb, wreq->data.length))
        goto done;

    payload = (void *)skb_put(skb, wreq->data.length);

    memcpy(payload, buf, wreq->data.length);

    atd_trace(ATD_DEBUG_FUNCTRACE, (".END"));

    wmi_send_event(dev_sc->wmi_handle, skb, WMI_EVENT_IW_ASSOCREQIE);

done:
    return;
}


static void 
atd_tgt_iw_auth_comp_ap(struct net_device  *dev, union iwreq_data  *wreq, 
                  const int8_t   *buf)
{
    atd_tgt_dev_t      *dev_sc = g_atd_dev;
    atd_auth_t         *auth;
    a_uint32_t          payload_len = sizeof(*auth);
    struct sk_buff     *skb = NULL;
	struct ev_msg *msg;
    
    atd_trace(ATD_DEBUG_FUNCTRACE, (".START"));

    if (!atd_tgt_vap_event_alloc(dev, dev_sc, &skb, payload_len))
        goto done;
    auth = (void *)skb_put(skb, payload_len);
	msg = (struct ev_msg *)buf;
	auth->status = htonl(msg->status);
	auth->frame_send = 1;

    atd_trace(ATD_DEBUG_FUNCTRACE, (".END"));
	
    wmi_send_event(dev_sc->wmi_handle, skb, WMI_EVENT_AUTH_COMP_AP);
done:
    return;
}

static void 
atd_tgt_iw_assoc_comp_ap(struct net_device  *dev, union iwreq_data  *wreq, 
                  const int8_t   *buf)
{
    atd_tgt_dev_t      *dev_sc = g_atd_dev;
    atd_assoc_t        *assoc;
    a_uint32_t          payload_len = sizeof(*assoc);
    struct sk_buff     *skb = NULL;
	struct ev_msg *msg;
    
    atd_trace(ATD_DEBUG_FUNCTRACE, (".START"));
    if (!atd_tgt_vap_event_alloc(dev, dev_sc, &skb, payload_len))
        goto done;
    assoc = (void *)skb_put(skb, payload_len);
	msg = (struct ev_msg *)buf;
	assoc->status = htonl(msg->status);
	assoc->frame_send = 1;

    atd_trace(ATD_DEBUG_FUNCTRACE, (".END"));
	
    wmi_send_event(dev_sc->wmi_handle, skb, WMI_EVENT_ASSOC_COMP_AP);
done:
    return;
}

static void 
atd_tgt_iw_deauth_comp_ap(struct net_device  *dev, union iwreq_data  *wreq, 
                  const int8_t   *buf)
{
    atd_tgt_dev_t      *dev_sc = g_atd_dev;
    atd_dauth_t        *deauth;
    a_uint32_t          payload_len = sizeof(*deauth);
    struct sk_buff     *skb = NULL;
	struct ev_msg *msg;
    
    atd_trace(ATD_DEBUG_FUNCTRACE, (".START"));
    if (!atd_tgt_vap_event_alloc(dev, dev_sc, &skb, payload_len))
        goto done;
    deauth = (void *)skb_put(skb, payload_len);
	msg = (struct ev_msg *)buf;
	deauth->status = htonl(msg->status);
	memcpy(deauth->macaddr, msg->addr, ACFG_MACADDR_LEN);
	deauth->frame_send = 1;

    atd_trace(ATD_DEBUG_FUNCTRACE, (".END"));
	
    wmi_send_event(dev_sc->wmi_handle, skb, WMI_EVENT_DEAUTH_COMP_AP);
done:
    return;
}


static void 
atd_tgt_iw_auth_ind_ap(struct net_device  *dev, union iwreq_data  *wreq, 
                  const int8_t   *buf)
{
    atd_tgt_dev_t      *dev_sc = g_atd_dev;
    atd_auth_t         *auth;
    a_uint32_t          payload_len = sizeof(*auth);
    struct sk_buff     *skb = NULL;
	struct ev_msg *msg;
    
    atd_trace(ATD_DEBUG_FUNCTRACE, (".START"));
    if (!atd_tgt_vap_event_alloc(dev, dev_sc, &skb, payload_len))
        goto done;
    auth = (void *)skb_put(skb, payload_len);
	msg = (struct ev_msg *)buf;
	auth->status = htonl(msg->status);
	memcpy(auth->macaddr, msg->addr, ACFG_MACADDR_LEN);
	auth->frame_send = 0;

    atd_trace(ATD_DEBUG_FUNCTRACE, (".END"));
	
    wmi_send_event(dev_sc->wmi_handle, skb, WMI_EVENT_AUTH_IND_AP);
done:
    return;
}

static void 
atd_tgt_iw_auth_comp_sta(struct net_device  *dev, union iwreq_data  *wreq, 
                  const int8_t   *buf)
{
    atd_tgt_dev_t      *dev_sc = g_atd_dev;
    atd_auth_t         *auth;
    a_uint32_t          payload_len = sizeof(*auth);
    struct sk_buff     *skb = NULL;
	struct ev_msg *msg;
    
    atd_trace(ATD_DEBUG_FUNCTRACE, (".START"));
    if (!atd_tgt_vap_event_alloc(dev, dev_sc, &skb, payload_len))
        goto done;
    auth = (void *)skb_put(skb, payload_len);
	msg = (struct ev_msg *)buf;
	auth->status = htonl(msg->status);
	auth->frame_send = 1;

    atd_trace(ATD_DEBUG_FUNCTRACE, (".END"));
	
    wmi_send_event(dev_sc->wmi_handle, skb, WMI_EVENT_AUTH_COMP_STA);
done:
    return;
}

static void 
atd_tgt_iw_assoc_comp_sta(struct net_device  *dev, union iwreq_data  *wreq, 
                  const int8_t   *buf)
{
    atd_tgt_dev_t      *dev_sc = g_atd_dev;
    atd_assoc_t        *assoc;
    a_uint32_t          payload_len = sizeof(*assoc);
    struct sk_buff     *skb = NULL;
	struct ev_msg *msg;
    
    atd_trace(ATD_DEBUG_FUNCTRACE, (".START"));
    if (!atd_tgt_vap_event_alloc(dev, dev_sc, &skb, payload_len))
        goto done;
    assoc = (void *)skb_put(skb, payload_len);
	msg = (struct ev_msg *)buf;
	assoc->status = htonl(msg->status);
	assoc->frame_send = 1;

    atd_trace(ATD_DEBUG_FUNCTRACE, (".END"));
	
    wmi_send_event(dev_sc->wmi_handle, skb, WMI_EVENT_ASSOC_COMP_STA);
done:
    return;
}

static void 
atd_tgt_iw_deauth_comp_sta(struct net_device  *dev, union iwreq_data  *wreq, 
                  const int8_t   *buf)
{
    atd_tgt_dev_t      *dev_sc = g_atd_dev;
    atd_dauth_t        *deauth;
    a_uint32_t          payload_len = sizeof(*deauth);
    struct sk_buff     *skb = NULL;
	struct ev_msg *msg;
    
    atd_trace(ATD_DEBUG_FUNCTRACE, (".START"));
    if (!atd_tgt_vap_event_alloc(dev, dev_sc, &skb, payload_len))
        goto done;
    deauth = (void *)skb_put(skb, payload_len);
	msg = (struct ev_msg *)buf;
	deauth->status = htonl(msg->status);
	memcpy(deauth->macaddr, msg->addr, ACFG_MACADDR_LEN);
	deauth->frame_send = 1;

    atd_trace(ATD_DEBUG_FUNCTRACE, (".END"));
	
    wmi_send_event(dev_sc->wmi_handle, skb, WMI_EVENT_DEAUTH_COMP_STA);
done:
    return;
}

static void 
atd_tgt_iw_disassoc_comp_sta(struct net_device  *dev, union iwreq_data  *wreq, 
                  const int8_t   *buf)
{
    atd_tgt_dev_t      *dev_sc = g_atd_dev;
    atd_disassoc_t     *disassoc;
    a_uint32_t          payload_len = sizeof(*disassoc);
    struct sk_buff     *skb = NULL;
	struct ev_msg *msg;
    
    atd_trace(ATD_DEBUG_FUNCTRACE, (".START"));
    if (!atd_tgt_vap_event_alloc(dev, dev_sc, &skb, payload_len))
        goto done;
    disassoc = (void *)skb_put(skb, payload_len);
	msg = (struct ev_msg *)buf;
	disassoc->status = htonl(msg->status);
	disassoc->reason = htonl(msg->reason);
	memcpy(disassoc->macaddr, msg->addr, ACFG_MACADDR_LEN);
	disassoc->frame_send = 1;

    atd_trace(ATD_DEBUG_FUNCTRACE, (".END"));
	
    wmi_send_event(dev_sc->wmi_handle, skb, WMI_EVENT_DISASSOC_COMP_STA);
done:
    return;
}

static void 
atd_tgt_iw_auth_ind_sta(struct net_device  *dev, union iwreq_data  *wreq, 
                  const int8_t   *buf)
{
    atd_tgt_dev_t      *dev_sc = g_atd_dev;
    atd_auth_t         *auth;
    a_uint32_t          payload_len = sizeof(*auth);
    struct sk_buff     *skb = NULL;
	struct ev_msg *msg;
    
    atd_trace(ATD_DEBUG_FUNCTRACE, (".START"));
    if (!atd_tgt_vap_event_alloc(dev, dev_sc, &skb, payload_len))
        goto done;
    auth = (void *)skb_put(skb, payload_len);
	msg = (struct ev_msg *)buf;
	auth->status = htonl(msg->status);
	memcpy(auth->macaddr, msg->addr, ACFG_MACADDR_LEN);
	auth->frame_send = 1;

    atd_trace(ATD_DEBUG_FUNCTRACE, (".END"));
	
    wmi_send_event(dev_sc->wmi_handle, skb, WMI_EVENT_AUTH_IND_STA);
done:
    return;
}


static void 
atd_tgt_iw_deauth_ind_sta(struct net_device  *dev, union iwreq_data  *wreq, 
                  const int8_t   *buf)
{
    atd_tgt_dev_t      *dev_sc = g_atd_dev;
    atd_dauth_t        *deauth;
    a_uint32_t          payload_len = sizeof(*deauth);
    struct sk_buff     *skb = NULL;
	struct ev_msg *msg;
    
    atd_trace(ATD_DEBUG_FUNCTRACE, (".START"));
    if (!atd_tgt_vap_event_alloc(dev, dev_sc, &skb, payload_len))
        goto done;
    deauth = (void *)skb_put(skb, payload_len);
	msg = (struct ev_msg *)buf;
	deauth->reason = htonl(msg->reason);
	memcpy(deauth->macaddr, msg->addr, ACFG_MACADDR_LEN);
	deauth->frame_send = 1;

    atd_trace(ATD_DEBUG_FUNCTRACE, (".END"));
	
    wmi_send_event(dev_sc->wmi_handle, skb, WMI_EVENT_DEAUTH_IND_STA);
done:
    return;
}

static void 
atd_tgt_iw_assoc_ind_sta(struct net_device  *dev, union iwreq_data  *wreq, 
                  const int8_t   *buf)
{
    atd_tgt_dev_t      *dev_sc = g_atd_dev;
    atd_assoc_t        *assoc;
    a_uint32_t          payload_len = sizeof(*assoc);
    struct sk_buff     *skb = NULL;
	struct ev_msg *msg;
    
    atd_trace(ATD_DEBUG_FUNCTRACE, (".START"));
    if (!atd_tgt_vap_event_alloc(dev, dev_sc, &skb, payload_len))
        goto done;
    assoc = (void *)skb_put(skb, payload_len);
	msg = (struct ev_msg *)buf;
	assoc->status = htonl(msg->status);
	memcpy(assoc->bssid, msg->addr, ACFG_MACADDR_LEN);
	assoc->frame_send = 1;

    atd_trace(ATD_DEBUG_FUNCTRACE, (".END"));
	
    wmi_send_event(dev_sc->wmi_handle, skb, WMI_EVENT_ASSOC_IND_STA);
done:
    return;
}

static void 
atd_tgt_iw_disassoc_ind_sta(struct net_device  *dev, union iwreq_data  *wreq, 
                  const int8_t   *buf)
{
    atd_tgt_dev_t      *dev_sc = g_atd_dev;
    atd_disassoc_t     *disassoc;
    a_uint32_t          payload_len = sizeof(*disassoc);
    struct sk_buff     *skb = NULL;
	struct ev_msg *msg;
    
    atd_trace(ATD_DEBUG_FUNCTRACE, (".START"));
    if (!atd_tgt_vap_event_alloc(dev, dev_sc, &skb, payload_len))
        goto done;
    disassoc = (void *)skb_put(skb, payload_len);
	msg = (struct ev_msg *)buf;
	disassoc->reason = htonl(msg->reason);
	memcpy(disassoc->macaddr, msg->addr, ACFG_MACADDR_LEN);
	disassoc->frame_send = 1;

    atd_trace(ATD_DEBUG_FUNCTRACE, (".END"));
	
    wmi_send_event(dev_sc->wmi_handle, skb, WMI_EVENT_DISASSOC_IND_STA);
done:
    return;
}

static void 
atd_tgt_iw_deauth_ind_ap(struct net_device  *dev, union iwreq_data  *wreq, 
                  const int8_t   *buf)
{
    atd_tgt_dev_t      *dev_sc = g_atd_dev;
    atd_dauth_t        *deauth;
    a_uint32_t          payload_len = sizeof(*deauth);
    struct sk_buff     *skb = NULL;
	struct ev_msg *msg;
    
    atd_trace(ATD_DEBUG_FUNCTRACE, (".START"));
    if (!atd_tgt_vap_event_alloc(dev, dev_sc, &skb, payload_len))
        goto done;
    deauth = (void *)skb_put(skb, payload_len);
	msg = (struct ev_msg *)buf;
	deauth->reason = htonl(msg->reason);
	memcpy(deauth->macaddr, msg->addr, ACFG_MACADDR_LEN);
	deauth->frame_send = 0;

    atd_trace(ATD_DEBUG_FUNCTRACE, (".END"));
	
    wmi_send_event(dev_sc->wmi_handle, skb, WMI_EVENT_DEAUTH_IND_AP);
done:
    return;
}

static void 
atd_tgt_iw_disassoc_ind_ap(struct net_device  *dev, union iwreq_data  *wreq, 
                  const int8_t   *buf)
{
    atd_tgt_dev_t      *dev_sc = g_atd_dev;
    atd_disassoc_t     *disassoc;
    a_uint32_t          payload_len = sizeof(*disassoc);
    struct sk_buff     *skb = NULL;
	struct ev_msg *msg;
    
    atd_trace(ATD_DEBUG_FUNCTRACE, (".START"));
    if (!atd_tgt_vap_event_alloc(dev, dev_sc, &skb, payload_len))
        goto done;
    disassoc = (void *)skb_put(skb, payload_len);
	msg = (struct ev_msg *)buf;
	disassoc->reason = htonl(msg->reason);
	memcpy(disassoc->macaddr, msg->addr, ACFG_MACADDR_LEN);
	disassoc->frame_send = 0;

    atd_trace(ATD_DEBUG_FUNCTRACE, (".END"));
	
    wmi_send_event(dev_sc->wmi_handle, skb, WMI_EVENT_DISASSOC_IND_AP);
done:
    return;
}


/** 
 * @brief 
 * 
 * @param dev
 * @param wreq
 * @param buf
 */
static void 
atd_tgt_iw_wapi(struct net_device  *dev, union iwreq_data  *wreq, 
                  const int8_t   *buf)
{
    atd_tgt_dev_t      *dev_sc = g_atd_dev;
    acfg_wsupp_custom_message_t *payload;
    a_uint32_t          payload_len = sizeof(*payload);
    struct sk_buff     *skb = NULL;
    
    atd_trace(ATD_DEBUG_FUNCTRACE, (".START"));

    payload_len =  wreq->data.length;

    atd_assert(payload_len <= sizeof(payload->raw_message));

    if (!atd_tgt_vap_event_alloc(dev, dev_sc, &skb, payload_len))
        goto done;

    payload = (void *)skb_put(skb, payload_len);

    memcpy(payload->raw_message, buf, payload_len); 
    atd_trace(ATD_DEBUG_FUNCTRACE, (".END"));

    wmi_send_event(dev_sc->wmi_handle, skb, WMI_EVENT_WAPI);
done:
    return;

} 


/** 
 * @brief 
 * 
 * @param dev
 * @param wreq
 * @param buf
 */
static void 
atd_tgt_if_running_sta(struct net_device  *dev, union iwreq_data  *wreq, 
                  const int8_t   *buf)
{
    atd_tgt_dev_t      *dev_sc = g_atd_dev;
    atd_assoc_sta_t    *payload;
    a_uint32_t          payload_len = sizeof(*payload);
    struct sk_buff     *skb = NULL;
    
    atd_trace(ATD_DEBUG_FUNCTRACE, (".START"));

    if (!atd_tgt_vap_event_alloc(dev, dev_sc, &skb, payload_len))
        goto done;

    payload = (void *)skb_put(skb, payload_len);

    payload->status = htonl(ACFG_ASSOC_SUCCESS);
    memcpy(payload->bssid, wreq->addr.sa_data, ACFG_MACADDR_LEN);
    memset(&payload->ssid, 0, sizeof(atd_ssid_t));
    
    atd_trace(ATD_DEBUG_FUNCTRACE, (".END"));

    wmi_send_event(dev_sc->wmi_handle, skb, WMI_EVENT_IF_RUNNING_STA);
done:
    return;

} 

/** 
 * @brief 
 * 
 * @param dev
 * @param wreq
 * @param buf
 */
static void 
atd_tgt_if_not_running_sta(struct net_device  *dev, union iwreq_data  *wreq, 
                  const int8_t   *buf)
{
    atd_tgt_dev_t      *dev_sc = g_atd_dev;
    atd_assoc_sta_t    *payload;
    a_uint32_t          payload_len = sizeof(*payload);
    struct sk_buff     *skb = NULL;
    
    atd_trace(ATD_DEBUG_FUNCTRACE, (".START"));

    if (!atd_tgt_vap_event_alloc(dev, dev_sc, &skb, payload_len))
        goto done;

    payload = (void *)skb_put(skb, payload_len);

    payload->status = htonl(ACFG_ASSOC_SUCCESS);
    memcpy(payload->bssid, wreq->addr.sa_data, ACFG_MACADDR_LEN);
    memset(&payload->ssid, 0, sizeof(atd_ssid_t));
    
    atd_trace(ATD_DEBUG_FUNCTRACE, (".END"));

    wmi_send_event(dev_sc->wmi_handle, skb, WMI_EVENT_IF_NOT_RUNNING_STA);
done:
    return;

}

/** 
 * @brief 
 * 
 * @param dev
 * @param wreq
 * @param buf
 */
static void 
atd_tgt_scan_done(struct net_device  *dev, union iwreq_data  *wreq, 
                        const int8_t   *buf)
{
    atd_tgt_dev_t       *dev_sc = g_atd_dev;
    atd_scan_done_t     *payload;
    a_uint32_t          payload_len = sizeof(*payload);
    struct iwreq        req;
    a_status_t status = A_STATUS_OK;

    struct sk_buff     *skb   = NULL;
    atd_tgt_vap_t      *vap   = NULL;
    a_uint8_t          *param = NULL;
    
    atd_trace(ATD_DEBUG_FUNCTRACE,(">> SCAN DONE RECVD\n"));
    atd_trace(ATD_DEBUG_FUNCTRACE,(" Start"));

    if ( (vap = atd_tgt_vap_event_alloc(dev, dev_sc, &skb, payload_len)) == NULL )
        goto done;

    payload = (void *)skb_put(skb, payload_len);

    memset(&req, 0, sizeof(struct iwreq));
    param = kzalloc(2 * sizeof(a_uint32_t), GFP_KERNEL);
    if(param == NULL ){
        status = A_STATUS_ENXIO;
        goto done ;
    }

    req.u.data.pointer  = (a_uint8_t *)param;
    req.u.data.length   = 0;

#define IOCTL_GET_SCAN_SPACE (SIOCDEVPRIVATE + 16)
    status = netdev_ioctl(vap->vap_netdev,(struct ifreq *)&req,IOCTL_GET_SCAN_SPACE);
    if(status != A_STATUS_OK){
        payload->status = htonl(ACFG_LENGTH_FAIL);
    }
    else
        payload->status = htonl(ACFG_SCAN_SUCCESS);

    payload->size = htonl(req.u.data.length);

    wmi_send_event(dev_sc->wmi_handle, skb, WMI_EVENT_SCAN_DONE);

    atd_trace(ATD_DEBUG_FUNCTRACE,(" End"));
done:
    return;

} 


void 
atd_tgt_get_freq(void *ctx, a_uint16_t cmdid,  
                  a_uint8_t *buffer, a_int32_t len)
{
    a_status_t status = A_STATUS_OK;
    atd_tgt_dev_t *atsc = (atd_tgt_dev_t *)ctx;
    atd_tgt_wifi_t *wifisc;
    atd_tgt_vap_t *vap;
    struct iw_request_info info;

    atd_hdr_t *atdhdr;
    acfg_freq_t freq;

    union iwreq_data iwreqdata;

    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("cmd %d    ", cmdid));

    atd_assert(len >= (sizeof(atd_hdr_t)));

    atdhdr = (atd_hdr_t *) buffer;

    wifisc = atsc->wifi_list[atdhdr->wifi_index];
    if(wifisc == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    vap    = wifisc->vap_list[atdhdr->vap_index];
    if(vap == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    status = netdev_wext_handler(vap->vap_netdev, SIOCGIWFREQ, &info, &iwreqdata, NULL);

    if (status != A_STATUS_OK) {
        atd_trace(ATD_DEBUG_CFG, ("get freq error, status = %d\n", status));
        goto fail;
    }

    atd_trace(ATD_DEBUG_CFG, ("iwreq->freq.m %d iwreq->freq.e %d", \
                               iwreqdata.freq.m, iwreqdata.freq.e));

    freq.m = htonl(iwreqdata.freq.m);
    freq.e = htons(iwreqdata.freq.e);
	
fail:
	
    wmi_cmd_rsp(atsc->wmi_handle, cmdid, status, (a_uint8_t *)&freq, 
                sizeof (acfg_freq_t));
}



/** 
 * @brief Set RTS threshold
 * 
 * @param ctx
 * @param cmdid
 * @param buffer
 * @param len
 */
static void
atd_tgt_set_rts(void *ctx, a_uint16_t cmdid,
                           a_uint8_t *buffer, a_int32_t len)
{
    a_status_t      status = A_STATUS_OK;
    atd_tgt_dev_t  *atsc   = (atd_tgt_dev_t *)ctx;
    atd_tgt_wifi_t *wifisc = NULL;
    atd_tgt_vap_t  *vap    = NULL;
    atd_hdr_t      *atdhdr;
    a_uint32_t resp = 0;
    atd_rts_t *rts = NULL;
    struct iw_request_info info;
    union  iwreq_data iwreq;

    atd_trace(ATD_DEBUG_FUNCTRACE, ("Start..."));

    atd_assert(len >= (sizeof(atd_hdr_t) + sizeof(atd_rts_t)));

    atdhdr = (atd_hdr_t  *)buffer;
    rts = (atd_rts_t *)(buffer + sizeof(atd_hdr_t)) ;

    wifisc = atsc->wifi_list[atdhdr->wifi_index];
    if(wifisc == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    vap = wifisc->vap_list[atdhdr->vap_index];
    if(vap == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    rts->val = ntohl(rts->val);
    rts->flags = ntohl(rts->flags);

    info.cmd = SIOCSIWRTS ;
    info.flags = 0 ;
    iwreq.rts.value = rts->val;
    iwreq.rts.disabled = 0;
    iwreq.rts.fixed = 0;
    iwreq.rts.flags = 0;

    if(rts->flags & ACFG_RTS_DISABLED)
        iwreq.rts.disabled = 1;

    if(rts->flags & ACFG_RTS_FIXED)
        iwreq.rts.fixed = 1;
    
    atd_trace(ATD_DEBUG_CFG, ("%s(): cmdid=%d; rts= %d; disabled= %d; fixed= %d \n",
                __FUNCTION__,cmdid,iwreq.rts.value, 
		iwreq.rts.disabled, iwreq.rts.fixed));

    status = netdev_wext_handler(vap->vap_netdev, SIOCSIWRTS, &info,
                                              (void *)&iwreq, NULL);
    
fail:
    if(status != A_STATUS_OK){
        resp = 1;
        atd_trace(ATD_DEBUG_CFG, ("Frequency Set error \n"));
    }

    resp = htonl(resp) ;
    wmi_cmd_rsp(atsc->wmi_handle, cmdid, status, (a_uint8_t *)&resp, sizeof(resp));
    atd_trace(ATD_DEBUG_FUNCTRACE ,(" End"));
}



void 
atd_tgt_get_rts(void *ctx, a_uint16_t cmdid,  
                  a_uint8_t *buffer, a_int32_t len)
{
    a_status_t status = A_STATUS_OK;
    atd_tgt_dev_t *atsc = (atd_tgt_dev_t *)ctx;
    atd_tgt_wifi_t *wifisc;
    atd_tgt_vap_t *vap;
    struct iw_request_info info;

    atd_hdr_t *atdhdr;
    atd_rts_t rts;

    union iwreq_data iwreqdata;
    struct iw_param *param = &iwreqdata.param;

    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("cmd %d    ", cmdid));

    atd_assert(len >= (sizeof(atd_hdr_t)));

    atdhdr = (atd_hdr_t *) buffer;

    wifisc = atsc->wifi_list[atdhdr->wifi_index];
    if(wifisc == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    vap    = wifisc->vap_list[atdhdr->vap_index];
    if(vap == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    status = netdev_wext_handler(vap->vap_netdev, SIOCGIWRTS, &info, &iwreqdata, NULL);

    if (status != A_STATUS_OK) {
        atd_trace(ATD_DEBUG_CFG, ("get rts error, status = %d\n", status));
        goto fail;
    }

    atd_trace(ATD_DEBUG_CFG, ("value %d disabled %d fixed %d", \
                               param->value, param->disabled, param->fixed));

    rts.val = htonl(param->value);
    rts.flags = 0;
    if (param->disabled)
        rts.flags |= ACFG_RTS_DISABLED;
    if (param->fixed)
        rts.flags |= ACFG_RTS_FIXED;
    rts.flags = htonl(rts.flags);

fail:
	
    wmi_cmd_rsp(atsc->wmi_handle, cmdid, status, (a_uint8_t *)&rts, 
                sizeof (atd_rts_t));
}





/** 
 * @brief Set fragmentation threshold
 * 
 * @param ctx
 * @param cmdid
 * @param buffer
 * @param len
 */
static void
atd_tgt_set_frag(void *ctx, a_uint16_t cmdid,
                           a_uint8_t *buffer, a_int32_t len)
{
    a_status_t      status = A_STATUS_OK;
    atd_tgt_dev_t  *atsc   = (atd_tgt_dev_t *)ctx;
    atd_tgt_wifi_t *wifisc = NULL;
    atd_tgt_vap_t  *vap    = NULL;
    atd_hdr_t      *atdhdr;
    a_uint32_t resp = 0;
    atd_frag_t *frag = NULL;
    struct iw_request_info info;
    union  iwreq_data iwreq;

    atd_trace(ATD_DEBUG_FUNCTRACE, ("Start..."));

    atd_assert(len >= (sizeof(atd_hdr_t) + sizeof(atd_frag_t)));

    atdhdr = (atd_hdr_t  *)buffer;
    frag = (atd_frag_t *)(buffer + sizeof(atd_hdr_t)) ;

    wifisc = atsc->wifi_list[atdhdr->wifi_index];
    if(wifisc == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    vap = wifisc->vap_list[atdhdr->vap_index];
    if(vap == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    frag->val = ntohl(frag->val);
    frag->flags = ntohl(frag->flags);

    info.cmd = SIOCSIWFRAG ;
    info.flags = 0 ;
    iwreq.frag.value = frag->val;
    iwreq.frag.disabled = 0;
    iwreq.frag.fixed = 0;
    iwreq.frag.flags = 0;

    if(frag->flags & ACFG_FRAG_DISABLED)
        iwreq.frag.disabled = 1;

    if(frag->flags & ACFG_FRAG_FIXED)
        iwreq.frag.fixed = 1;
    
    atd_trace(ATD_DEBUG_CFG, ("%s(): cmdid=%d; frag= %d; disabled= %d; fixed= %d \n",
                __FUNCTION__,cmdid,iwreq.frag.value, 
		iwreq.frag.disabled, iwreq.frag.fixed));

    status = netdev_wext_handler(vap->vap_netdev, SIOCSIWFRAG, &info,
                                              (void *)&iwreq, NULL);
    
fail:
    if(status != A_STATUS_OK){
        resp = 1;
        atd_trace(ATD_DEBUG_CFG, ("Frequency Set error \n"));
    }

    resp = htonl(resp) ;
    wmi_cmd_rsp(atsc->wmi_handle, cmdid, status, (a_uint8_t *)&resp, sizeof(resp));
    atd_trace(ATD_DEBUG_FUNCTRACE ,(" End"));
}



void 
atd_tgt_get_frag(void *ctx, a_uint16_t cmdid,  
                  a_uint8_t *buffer, a_int32_t len)
{
    a_status_t status = A_STATUS_OK;
    atd_tgt_dev_t *atsc = (atd_tgt_dev_t *)ctx;
    atd_tgt_wifi_t *wifisc;
    atd_tgt_vap_t *vap;
    struct iw_request_info info;

    atd_hdr_t *atdhdr;
    atd_frag_t frag;

    union iwreq_data iwreqdata;
    struct iw_param *param = &iwreqdata.param;

    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("cmd %d    ", cmdid));

    atd_assert(len >= (sizeof(atd_hdr_t)));

    atdhdr = (atd_hdr_t *) buffer;
	
    wifisc = atsc->wifi_list[atdhdr->wifi_index];
    if(wifisc == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    vap    = wifisc->vap_list[atdhdr->vap_index];
    if(vap == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    status = netdev_wext_handler(vap->vap_netdev, SIOCGIWFRAG, &info, &iwreqdata, NULL);

    if (status != A_STATUS_OK) {
        atd_trace(ATD_DEBUG_CFG, ("get frag error, status = %d\n", status));
        goto fail;
    }

    atd_trace(ATD_DEBUG_CFG, ("value %d disabled %d fixed %d", \
                               param->value, param->disabled, param->fixed));
	
    frag.val = htonl(param->value);
    frag.flags = 0;
    if (param->disabled)
        frag.flags |= ACFG_FRAG_DISABLED;
    if (param->fixed)
        frag.flags |= ACFG_FRAG_FIXED;
    frag.flags = htonl(frag.flags);
	
fail:
	
	wmi_cmd_rsp(atsc->wmi_handle, cmdid, status, (a_uint8_t *)&frag, 
		        sizeof (atd_frag_t));
}



/** 
 * @brief Set default TxPower
 * 
 * @param ctx
 * @param cmdid
 * @param buffer
 * @param len
 */
static void
atd_tgt_set_txpow(void *ctx, a_uint16_t cmdid,
                           a_uint8_t *buffer, a_int32_t len)
{
    a_status_t      status = A_STATUS_OK;
    atd_tgt_dev_t  *atsc   = (atd_tgt_dev_t *)ctx;
    atd_tgt_wifi_t *wifisc = NULL;
    atd_tgt_vap_t  *vap    = NULL;
    atd_hdr_t      *atdhdr;
    a_uint32_t resp = 0;
    atd_txpow_t *txpow = NULL;
    struct iw_request_info info;
    union  iwreq_data iwreq;

    atd_trace(ATD_DEBUG_FUNCTRACE, ("Start..."));

    atd_assert(len >= (sizeof(atd_hdr_t) + sizeof(atd_txpow_t)));

    atdhdr = (atd_hdr_t  *)buffer;
    txpow = (atd_txpow_t *)(buffer + sizeof(atd_hdr_t)) ;

    wifisc = atsc->wifi_list[atdhdr->wifi_index];
    if(wifisc == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    vap = wifisc->vap_list[atdhdr->vap_index];
    if(vap == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    txpow->val = ntohl(txpow->val);
    txpow->flags = ntohl(txpow->flags);

    info.cmd = SIOCSIWTXPOW ;
    info.flags = 0 ;
    iwreq.txpower.value = txpow->val;
    iwreq.txpower.disabled = 0;
    iwreq.txpower.fixed = 0;

    /* Assume Tx power is in dBm */
    iwreq.txpower.flags = 0;

    if(txpow->flags & ACFG_TXPOW_DISABLED)
        iwreq.txpower.disabled = 1;

    if(txpow->flags & ACFG_TXPOW_FIXED)
        iwreq.txpower.fixed = 1;
    
    atd_trace(ATD_DEBUG_CFG, ("%s(): cmdid=%d; txpow= %ddBm; disabled= %d; fixed= %d \n",
                __FUNCTION__,cmdid,iwreq.txpower.value, 
		iwreq.frag.disabled, iwreq.frag.fixed));

    status = netdev_wext_handler(vap->vap_netdev, SIOCSIWTXPOW, &info,
                                              (void *)&iwreq, NULL);
    
fail:
    if(status != A_STATUS_OK){
        resp = 1;
        atd_trace(ATD_DEBUG_CFG, ("TxPower Set error \n"));
    }

    resp = htonl(resp) ;
    wmi_cmd_rsp(atsc->wmi_handle, cmdid, status, (a_uint8_t *)&resp, sizeof(resp));
    atd_trace(ATD_DEBUG_FUNCTRACE ,(" End"));
}




void 
atd_tgt_get_txpow(void *ctx, a_uint16_t cmdid,  
                  a_uint8_t *buffer, a_int32_t len)
{
    a_status_t status = A_STATUS_OK;
    atd_tgt_dev_t *atsc = (atd_tgt_dev_t *)ctx;
    atd_tgt_wifi_t *wifisc;
    atd_tgt_vap_t *vap;
    struct iw_request_info info;

    atd_hdr_t *atdhdr;
    atd_txpow_t txpow;

    union iwreq_data iwreqdata;
    struct iw_param *param = &iwreqdata.txpower;

    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("cmd %d    ", cmdid));

    atd_assert(len >= (sizeof(atd_hdr_t)));

    atdhdr = (atd_hdr_t *) buffer;
	
    wifisc = atsc->wifi_list[atdhdr->wifi_index];
    if(wifisc == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    vap    = wifisc->vap_list[atdhdr->vap_index];
    if(vap == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    status = netdev_wext_handler(vap->vap_netdev, SIOCGIWTXPOW, &info, &iwreqdata, NULL);

    if (status != A_STATUS_OK) {
        atd_trace(ATD_DEBUG_CFG, ("get txpow error, status = %d\n", status));
        goto fail;
    }

    atd_trace(ATD_DEBUG_CFG, ("value %d disabled %d fixed %d", \
                               param->value, param->disabled, param->fixed));

    txpow.val = htonl(param->value);
    txpow.flags = 0;
    if (param->disabled)
        txpow.flags |= ACFG_TXPOW_DISABLED;
    if (param->fixed)
        txpow.flags |= ACFG_TXPOW_FIXED;
    txpow.flags = htonl(txpow.flags);
	
fail:
	
    wmi_cmd_rsp(atsc->wmi_handle, cmdid, status, (a_uint8_t *)&txpow, 
                sizeof (atd_txpow_t));
}

void 
atd_tgt_set_powmgmt(void *ctx, a_uint16_t cmdid, 
                      a_uint8_t *buffer, a_int32_t len)
{
    a_status_t status = A_STATUS_OK;
    atd_tgt_dev_t *atsc = (atd_tgt_dev_t *)ctx;
    atd_tgt_wifi_t *wifisc =NULL;
    atd_tgt_vap_t * vap;
    atd_hdr_t   *atdhdr;

    struct iw_request_info info;
    union iwreq_data iwreq;

    atd_powmgmt_t *mgmt;
    a_uint32_t rsp = 0 ;

    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("cmd %d    ", cmdid));

    atd_assert(len >= (sizeof(atd_hdr_t) + sizeof(atd_powmgmt_t)));

    atdhdr  = (atd_hdr_t *) buffer;
    mgmt    = (atd_powmgmt_t *) (buffer + sizeof(atd_hdr_t));

    wifisc = atsc->wifi_list[atdhdr->wifi_index];
    if(wifisc == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    vap    = wifisc->vap_list[atdhdr->vap_index];
    if(vap == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    iwreq.power.disabled = mgmt->disabled;
    iwreq.power.flags = ntohs(mgmt->flags);
    iwreq.power.value = ntohl(mgmt->val);

    atd_trace(ATD_DEBUG_CFG, ("value %d disabled %d flags 0x%04x", \
                               iwreq.power.value, iwreq.power.disabled, iwreq.power.flags));
	
    status = netdev_wext_handler(vap->vap_netdev, SIOCSIWPOWER, &info, &iwreq, NULL);

    if (status != A_STATUS_OK) {
        atd_trace(ATD_DEBUG_CFG, ("set power mgmt error, status = %d\n", status));
        rsp = 0xFFFFFFFF;
    }
	
fail:

    wmi_cmd_rsp(atsc->wmi_handle, cmdid, status, (a_uint8_t *)&rsp, 
                sizeof (a_int32_t));
}

void 
atd_tgt_get_powmgmt(void *ctx, a_uint16_t cmdid,  
                  a_uint8_t *buffer, a_int32_t len)
{
    a_status_t status = A_STATUS_OK;
    atd_tgt_dev_t *atsc = (atd_tgt_dev_t *)ctx;
    atd_tgt_wifi_t *wifisc;
    atd_tgt_vap_t *vap;
    struct iw_request_info info;

    atd_hdr_t *atdhdr;
    atd_powmgmt_t powmgmt;

    union iwreq_data iwreqdata;
    struct iw_param *param = &iwreqdata.param;

    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("cmd %d    ", cmdid));

    atd_assert(len >= (sizeof(atd_hdr_t)));

    atdhdr = (atd_hdr_t *) buffer;
	
    wifisc = atsc->wifi_list[atdhdr->wifi_index];
    if(wifisc == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    vap    = wifisc->vap_list[atdhdr->vap_index];
    if(vap == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    memset(&iwreqdata, 0, sizeof(iwreqdata));
    status = netdev_wext_handler(vap->vap_netdev, SIOCGIWPOWER, &info, &iwreqdata, NULL);

    if (status != A_STATUS_OK) {
		atd_trace(ATD_DEBUG_CFG, ("get powmgmt error, status = %d\n", status));
        goto fail;
    }

    atd_trace(ATD_DEBUG_CFG, ("value %d disabled %d flags 0x%04x", \
                               param->value, param->disabled, param->flags));

    powmgmt.val = htonl(param->value);
    powmgmt.flags = htons(param->flags);
    powmgmt.disabled = param->disabled;
		
fail:
	
    wmi_cmd_rsp(atsc->wmi_handle, cmdid, status, (a_uint8_t *)&powmgmt, 
                sizeof (atd_powmgmt_t));
}

void 
atd_tgt_set_encode(void *ctx, a_uint16_t cmdid, 
                      a_uint8_t *buffer, a_int32_t len)
{
    a_status_t status = A_STATUS_OK;
    atd_tgt_dev_t *atsc = (atd_tgt_dev_t *)ctx;
    atd_tgt_wifi_t *wifisc =NULL;
    atd_tgt_vap_t * vap;
    atd_hdr_t   *atdhdr;

    struct iw_request_info info;
    union iwreq_data iwreq;

    atd_encode_rsp_t *pencode;
    a_uint32_t rsp = 0 ;

    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("cmd %d    ", cmdid));

    atd_assert(len >= (sizeof(atd_hdr_t) + sizeof(atd_encode_rsp_t)));

    atdhdr  = (atd_hdr_t *) buffer;
    pencode = (atd_encode_rsp_t  *) (buffer + sizeof(atd_hdr_t)) ;

    wifisc = atsc->wifi_list[atdhdr->wifi_index];
    if(wifisc == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    vap    = wifisc->vap_list[atdhdr->vap_index];
    if(vap == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    iwreq.data.flags = (a_uint16_t)ntohl(pencode->flag);
    iwreq.data.length = (a_uint16_t)ntohl(pencode->len);

    atd_trace(ATD_DEBUG_CFG, ("len %d flags 0x%04x", \
                               iwreq.data.length, iwreq.data.flags));
    status = netdev_wext_handler(vap->vap_netdev, SIOCSIWENCODE, &info, &iwreq, 
                                (char *)pencode->key);

    if (status != A_STATUS_OK) {
        atd_trace(ATD_DEBUG_CFG, ("set encode error, status = %d\n", status));
        rsp = 0xFFFFFFFF;
    }
	
fail:

    wmi_cmd_rsp(atsc->wmi_handle, cmdid, status, (a_uint8_t *)&rsp, 
                sizeof (a_int32_t));
}

void 
atd_tgt_get_encode(void *ctx, a_uint16_t cmdid, 
                  a_uint8_t *buffer, a_int32_t len)
{
    a_status_t status = A_STATUS_OK;
    atd_tgt_dev_t *atsc = (atd_tgt_dev_t *)ctx;
    atd_tgt_wifi_t *wifisc;
    atd_tgt_vap_t *vap;
    struct iw_request_info info;

    atd_hdr_t *atdhdr;
    atd_encode_rsp_t encode;

    union iwreq_data iwreqdata;
    struct iw_point *pencode = &iwreqdata.data;

    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("cmd %d    ", cmdid));

    atd_assert(len >= (sizeof(atd_hdr_t)));

    atdhdr = (atd_hdr_t *) buffer;
	
    wifisc = atsc->wifi_list[atdhdr->wifi_index];
    if(wifisc == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    vap    = wifisc->vap_list[atdhdr->vap_index];
    if(vap == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    iwreqdata.data.length = IW_ENCODING_TOKEN_MAX;
    iwreqdata.data.flags = 0;
    status = netdev_wext_handler(vap->vap_netdev, SIOCGIWENCODE, &info, &iwreqdata, 
                                (char *)&encode.key[0]);

    if (status != A_STATUS_OK) {
        atd_trace(ATD_DEBUG_CFG, ("get encode error, status = %d\n", status));
        goto fail;
    }

    atd_trace(ATD_DEBUG_CFG, ("len %d flags 0x%04x", \
                               pencode->length, pencode->flags));

    encode.flag = htons(pencode->flags);
    encode.len = htons(pencode->length);

fail:
	
    wmi_cmd_rsp(atsc->wmi_handle, cmdid, status, (a_uint8_t *)&encode, 
                sizeof (atd_encode_rsp_t));
}

void 
atd_tgt_set_rate(void *ctx, a_uint16_t cmdid, 
                      a_uint8_t *buffer, a_int32_t len)
{
    a_status_t status = A_STATUS_OK;
    atd_tgt_dev_t *atsc = (atd_tgt_dev_t *)ctx;
    atd_tgt_wifi_t *wifisc =NULL;
    atd_tgt_vap_t * vap;
    atd_hdr_t   *atdhdr;

    struct iw_request_info info;
    union iwreq_data iwreq;

    atd_rate_t  *rate;
    a_uint32_t rsp = 0 ;

    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("cmd %d    ", cmdid));

    atd_assert(len >= (sizeof(atd_hdr_t) + sizeof(atd_rate_t)));

    atdhdr  = (atd_hdr_t *) buffer;
    rate    = (atd_rate_t  *) (buffer + sizeof(atd_hdr_t)) ;

    wifisc = atsc->wifi_list[atdhdr->wifi_index];
    if(wifisc == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    vap    = wifisc->vap_list[atdhdr->vap_index];
    if(vap == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    iwreq.bitrate.fixed = rate->fixed;
    iwreq.bitrate.value = ntohl(rate->value);

    atd_trace(ATD_DEBUG_CFG, ("value %d fixed %d", \
                               iwreq.bitrate.value, iwreq.bitrate.fixed));

    status = netdev_wext_handler(vap->vap_netdev, SIOCSIWRATE, &info, &iwreq, NULL);

    if (status != A_STATUS_OK) {
        atd_trace(ATD_DEBUG_CFG, ("set rate error, status = %d\n", status));
        rsp = 0xFFFFFFFF;
    }
	
fail:

    wmi_cmd_rsp(atsc->wmi_handle, cmdid, status, (a_uint8_t *)&rsp, 
                sizeof (a_int32_t));
}

void 
atd_tgt_get_stats(void *ctx, a_uint16_t cmdid, 
                  a_uint8_t *buffer, a_int32_t len)
{
    a_status_t status = A_STATUS_OK;
    atd_tgt_dev_t *atsc = (atd_tgt_dev_t *)ctx;
    atd_tgt_wifi_t *wifisc;
    atd_tgt_vap_t *vap;

    union iwreq_data iwreq;

    atd_hdr_t *atdhdr;
    struct iw_statistics *stats;
    atd_stats_t resp;

    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("cmd %d    ", cmdid));

    atd_assert(len >= (sizeof(atd_hdr_t)));

    atdhdr = (atd_hdr_t *) buffer;
	
    wifisc = atsc->wifi_list[atdhdr->wifi_index];
    if(wifisc == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    vap    = wifisc->vap_list[atdhdr->vap_index];
    if(vap == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    iwreq.data.flags = 1;
    iwreq.data.length = sizeof(struct iw_statistics);

    stats = get_wireless_stats(vap->vap_netdev);
    resp.status = htonl(stats->status);
    resp.link_quality = htonl(stats->qual.qual);
    resp.signal_level = htonl(stats->qual.level);
    resp.noise_level = htonl(stats->qual.noise);
    resp.updated = htonl(stats->qual.updated);
    resp.discard_nwid = htonl(stats->discard.nwid);
    resp.discard_code = htonl(stats->discard.code);
    resp.missed_beacon = htonl(stats->miss.beacon);
	
fail:

    wmi_cmd_rsp(atsc->wmi_handle, cmdid, status, (a_uint8_t *)&resp, 
                sizeof (atd_stats_t));
}

void 
atd_tgt_tx99tool(void *ctx, a_uint16_t cmdid, 
                      a_uint8_t *buffer, a_int32_t len)
{
    a_status_t status = A_STATUS_OK;
    atd_tgt_dev_t *atsc = (atd_tgt_dev_t *)ctx;
    atd_tgt_wifi_t *wifisc =NULL;
    atd_hdr_t   *atdhdr;
    struct ifreq ifr;

    atd_tx99_t  *tx99_param;
    a_uint32_t rsp = 0 ;
    acfg_tx99_t tx99 = {{0}};

    atd_trace(ATD_DEBUG_FUNCTRACE, ("cmd %d    ", \
                cmdid));

    atd_assert(len >= (sizeof(atd_hdr_t) + sizeof(atd_tx99_t)));  
    atdhdr = (atd_hdr_t *) buffer;
    tx99_param = (atd_tx99_t *)(buffer + sizeof(atd_hdr_t)) ;

    memcpy(tx99.if_name, tx99_param->if_name, ACFG_MAX_IFNAME);
    tx99.type = ntohl(tx99_param->type);
    tx99.data.freq = ntohl(tx99_param->data.freq);
    tx99.data.htmode = ntohl(tx99_param->data.htmode);
    tx99.data.htext = ntohl(tx99_param->data.htext);
    tx99.data.rate = ntohl(tx99_param->data.rate);
    tx99.data.rc = ntohl(tx99_param->data.rc);
    tx99.data.power = ntohl(tx99_param->data.power);
    tx99.data.txmode = ntohl(tx99_param->data.txmode);
    tx99.data.chanmask = ntohl(tx99_param->data.chanmask);
    tx99.data.type = ntohl(tx99_param->data.type);

    memset(&ifr, 0, sizeof(ifr));
    ifr.ifr_data = (void *)&tx99;

    atd_trace(ATD_DEBUG_FUNCTRACE, (" radio %d vap %d ", \
                                 atdhdr->wifi_index, atdhdr->vap_index));

    wifisc = atsc->wifi_list[atdhdr->wifi_index];
    if(wifisc == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    status  = netdev_ioctl(wifisc->netdev,&ifr,SIOCIOCTLTX99);

fail:

    wmi_cmd_rsp(atsc->wmi_handle, cmdid, status, (a_uint8_t *)&rsp , 
                sizeof (a_int32_t));
}

void 
atd_tgt_nawds_config(void *ctx, a_uint16_t cmdid, 
                      a_uint8_t *buffer, a_int32_t len)
{
    a_status_t status = A_STATUS_OK;
    atd_tgt_dev_t *atsc = (atd_tgt_dev_t *)ctx;
    atd_tgt_wifi_t *wifisc =NULL;
    atd_tgt_vap_t  *vap;
    atd_hdr_t   *atdhdr;
    struct iwreq req;
    atd_nawds_cfg_t  *nawds_config, resp = {0};
    struct ieee80211_wlanconfig nawds;

    atd_trace(ATD_DEBUG_FUNCTRACE, ("cmd %d    ", \
                cmdid));

    atd_assert(len >= (sizeof(atd_hdr_t) + sizeof(atd_nawds_cfg_t)));

    atdhdr = (atd_hdr_t *) buffer;
    nawds_config = (atd_nawds_cfg_t *)(buffer + sizeof(atd_hdr_t));
    nawds.cmdtype = ntohl(nawds_config->cmdtype);

    switch(nawds.cmdtype)
    {
        case NAWDS_SET_MODE:
        case NAWDS_SET_DEFCAPS:
        case NAWDS_SET_OVERRIDE:
        case NAWDS_SET_ADDR:
        case NAWDS_CLR_ADDR:
        case NAWDS_GET:
            nawds.data.nawds.num = nawds_config->data.nawds.num;
            nawds.data.nawds.mode = nawds_config->data.nawds.mode;
            nawds.data.nawds.defcaps = nawds_config->data.nawds.defcaps;
            nawds.data.nawds.override = nawds_config->data.nawds.override;
            memcpy(nawds.data.nawds.mac, nawds_config->data.nawds.mac, ACFG_MACADDR_LEN);
            nawds.data.nawds.caps = nawds_config->data.nawds.caps;
            break;
        case WNM_SET_BSSMAX:
        case WNM_GET_BSSMAX:
        case WNM_TFS_ADD:
        case WNM_TFS_DELETE:
        case WNM_SET_TIMBCAST:
        case WNM_GET_TIMBCAST:
            memcpy(&nawds.data.wnm, &nawds_config->data.wnm, sizeof(struct acfg_wnm_cfg));
            break;
        default:
            status = A_STATUS_EINVAL;
            goto fail;
    }

    memset(&req, 0, sizeof(struct iwreq));
    req.u.data.pointer = (a_uint8_t*)&nawds;
    req.u.data.length  = sizeof(struct ieee80211_wlanconfig);
    req.u.data.flags = 1;

    atd_trace(ATD_DEBUG_FUNCTRACE, (" radio %d vap %d ", \
                                 atdhdr->wifi_index, atdhdr->vap_index));

    wifisc = atsc->wifi_list[atdhdr->wifi_index];
    if(wifisc == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    vap = wifisc->vap_list[atdhdr->vap_index];
    if(vap == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    status  = netdev_ioctl(vap->vap_netdev,(struct ifreq *)&req, SIOCIOCTLNAWDS);

fail:
    
    if(nawds.cmdtype == NAWDS_GET 
        || nawds.cmdtype == WNM_GET_BSSMAX 
        || nawds.cmdtype == WNM_GET_TIMBCAST)
    {
        resp.cmdtype = htonl(nawds.cmdtype);
        resp.status = htonl(nawds.status);
        if (nawds.cmdtype == NAWDS_GET) {
            resp.data.nawds.num = nawds.data.nawds.num;
            resp.data.nawds.mode = nawds.data.nawds.mode;
            resp.data.nawds.defcaps = nawds.data.nawds.defcaps;
            resp.data.nawds.override = nawds.data.nawds.override;
            memcpy(resp.data.nawds.mac, nawds.data.nawds.mac, ACFG_MACADDR_LEN);
            resp.data.nawds.caps = nawds.data.nawds.caps;
        } else {
            memcpy(&resp.data.wnm, &nawds.data.wnm, sizeof(struct acfg_wnm_cfg));
        }
        wmi_cmd_rsp(atsc->wmi_handle, cmdid, status, (a_uint8_t *)&resp, 
                sizeof(atd_nawds_cfg_t));
    }
    else
        wmi_cmd_rsp(atsc->wmi_handle, cmdid, status, 
                NULL , 0);
}


/* security APIs */
static void
atd_tgt_wsupp_response_wmi(atd_tgt_dev_t *atsc, 
        wsupp_msg_t *wmsg, int len)
{
    wsupp_wmi_reply_t *wmi_reply;
    a_int32_t rsplen;;
    
    wmi_reply = (wsupp_wmi_reply_t *) wmsg->wm_data;
    rsplen = len - sizeof(wsupp_msg_t) - sizeof(wmi_reply->status);
    wmi_cmd_rsp(atsc->wmi_handle, wmsg->wm_wmi, wmi_reply->status, 
        (a_uint8_t *) wmi_reply->data, rsplen);
#if 0
    wmi_reply->data[rsplen] = '\0';
    atd_trace(ATD_DEBUG_CFG,("%s: status: %d len: %d: %s\n", __func__, 
        wmi_reply->status, rsplen, wmi_reply->data));
#endif
}

static void
atd_tgt_wsupp_response_wmi_event(atd_tgt_dev_t *atsc, 
        wsupp_msg_t *wmsg, int len)
{
    char *data;
    struct net_device *dev;
    struct sk_buff *skb = NULL;

    dev = dev_get_by_name(&init_net, wmsg->wm_ifname);
    if (dev == NULL) {
        atd_trace(ATD_DEBUG_CFG, ("failed to deliver wmi event: %d if: %s\n", 
                    wmsg->wm_wmi, wmsg->wm_ifname));
        return;
    }

    if (!atd_tgt_vap_event_alloc(dev, atsc, &skb, wmsg->wm_len))
        goto done;

    data = skb_put(skb, wmsg->wm_len);
    memcpy(data, wmsg->wm_data, wmsg->wm_len);

    wmi_send_event(atsc->wmi_handle, skb, htonl(wmsg->wm_wmi));

done:
    dev_put(dev);
    return;
}

int
atd_tgt_wsupp_response(void *ctx, char *buf, int len)
{
    atd_tgt_dev_t *atsc = (atd_tgt_dev_t *)ctx;
    wsupp_msg_t *wmsg = (wsupp_msg_t *) buf;

    if (wmsg->wm_type == WSUPP_MSG_TYPE_WMI) {
        atd_tgt_wsupp_response_wmi(atsc, wmsg, len);
        return 1;
    } else if (wmsg->wm_type == WSUPP_MSG_TYPE_WMI_EVENT) {
        atd_tgt_wsupp_response_wmi_event(atsc, wmsg, len);
        return 1;
    }

    return 0;
}

void 
atd_tgt_wsupp_request(void *ctx, a_uint16_t cmdid, 
                      a_uint8_t *buffer, a_int32_t len)
{
    a_status_t status = A_STATUS_FAILED;
    atd_tgt_dev_t *atsc = (atd_tgt_dev_t *)ctx;
    atd_hdr_t *atdhdr;
    a_uint32_t rsp = 0;
    acfg_wsupp_info_t *winfo;
    wsupp_msg_t *wmsg = NULL;

    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("cmd: %d", cmdid));

    atd_assert(len >= (sizeof(atd_hdr_t) + sizeof(acfg_wsupp_info_t)));

    atdhdr = (atd_hdr_t *) buffer;
    winfo = (acfg_wsupp_info_t *) (buffer + sizeof(atd_hdr_t)) ;

    wmsg = kmalloc(sizeof(wsupp_msg_t) + sizeof(winfo->u), GFP_ATOMIC);
    if (wmsg == NULL) {
        atd_trace(ATD_DEBUG_CFG, ("%s: failed to alloc: %d\n", __func__, cmdid));
        goto error;
    }

    wmsg->wm_type = ntohl(WSUPP_MSG_TYPE_WMI);
    wmsg->wm_wmi = ntohl(cmdid);
    memcpy(wmsg->wm_unique, winfo->unique, WSUPP_MSG_UNIQUE_BIN_LEN);
    strncpy(wmsg->wm_ifname, winfo->ifname, WSUPP_MSG_IFNAME_LEN);
    wmsg->wm_len = ntohl(sizeof(winfo->u));
    memcpy(wmsg->wm_data, &winfo->u, sizeof(winfo->u));

#if 1
    /* send request to wsupp helper */
    if (atd_tgt_wsupp_helper_wmi_send(atsc->wsupp_handle, cmdid, 
        (char *) wmsg, sizeof(wsupp_msg_t) + sizeof(winfo->u))) {
        atd_trace(ATD_DEBUG_CFG, ("%s: failed to send wmi: %d\n", __func__, cmdid));
        goto error;
    }
#else
    /* reply directly for WMI debugging */
    status = A_STATUS_OK;
    wmi_cmd_rsp(atsc->wmi_handle, cmdid, status, (a_uint8_t *)&rsp, 
        sizeof (a_int32_t));
#endif

    kfree(wmsg);

    return;
error:
    wmi_cmd_rsp(atsc->wmi_handle, cmdid, status, (a_uint8_t *)&rsp, 
        sizeof (a_int32_t));
    if (wmsg)
        kfree(wmsg);
}

void 
atd_tgt_set_reg(void *ctx, a_uint16_t cmdid, 
                      a_uint8_t *buffer, a_int32_t len)
{
    a_status_t status = A_STATUS_OK;
    atd_tgt_dev_t *atsc = (atd_tgt_dev_t *)ctx;
    atd_tgt_wifi_t *wifisc =NULL;
    atd_hdr_t   *atdhdr;

    atd_param_t  *wifi_param;
    a_uint32_t rsp = 0 ;

    atd_trace(ATD_DEBUG_FUNCTRACE, ("cmd %d    ", \
                cmdid));

    atd_assert(len >= (sizeof(atd_hdr_t) + sizeof(atd_param_t)));

    atdhdr = (atd_hdr_t *) buffer;
    wifi_param = (atd_param_t  *)(buffer + sizeof(atd_hdr_t));

    wifi_param->param = ntohl(wifi_param->param);
    wifi_param->val   = ntohl(wifi_param->val);

    atd_trace(ATD_DEBUG_FUNCTRACE, (" radio %d vap %d offset %x  val %x", \
                                 atdhdr->wifi_index, atdhdr->vap_index,\
                                 wifi_param->param, wifi_param->val));

    wifisc = atsc->wifi_list[atdhdr->wifi_index];
    if(wifisc == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

#ifndef BUILD_PARTNER_PLATFORM
    ar7240_reg_wr(wifi_param->param, wifi_param->val);
#endif

fail:

    wmi_cmd_rsp(atsc->wmi_handle, cmdid, status, (a_uint8_t *)&rsp, 
                sizeof (a_int32_t));
}

void 
atd_tgt_get_reg(void *ctx, a_uint16_t cmdid, 
                      a_uint8_t *buffer, a_int32_t len)
{
    a_status_t status = A_STATUS_OK;
    atd_tgt_dev_t *atsc = (atd_tgt_dev_t *)ctx;
    atd_tgt_wifi_t *wifisc =NULL;
    atd_hdr_t   *atdhdr;

    atd_param_t  *wifi_param;
    atd_param_t   paramresult;

    atd_trace(ATD_DEBUG_FUNCTRACE, ("cmd %d    ", cmdid));

    atd_assert(len >= (sizeof(atd_hdr_t)));

    atdhdr = (atd_hdr_t *) buffer;
    wifi_param = (atd_param_t *)(buffer + sizeof(atd_hdr_t));

    wifi_param->param = ntohl(wifi_param->param);

    atd_trace(ATD_DEBUG_FUNCTRACE, (" radio %d vap %d param %x val %x", \
                                 atdhdr->wifi_index, atdhdr->vap_index,\
                                 wifi_param->param, wifi_param->val));

    wifisc = atsc->wifi_list[atdhdr->wifi_index];

    if(wifisc == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

#ifndef BUILD_PARTNER_PLATFORM
    wifi_param->val = ar7240_reg_rd(wifi_param->param);
    paramresult.val = htonl(wifi_param->val);
#endif

    atd_trace(ATD_DEBUG_FUNCTRACE ,("Result val %x \n",wifi_param->val));

fail:

    wmi_cmd_rsp(atsc->wmi_handle, cmdid, status, (a_uint8_t *)&paramresult, 
                sizeof (atd_param_t));
}

void 
atd_tgt_set_hwaddr(void *ctx, a_uint16_t cmdid, 
                      a_uint8_t *buffer, a_int32_t len)
{
    a_status_t status = A_STATUS_OK;
    atd_tgt_dev_t *atsc = (atd_tgt_dev_t *)ctx;
    atd_tgt_wifi_t *wifisc =NULL;
    atd_hdr_t   *atdhdr;
    acfg_macaddr_t *paddr;
    struct sockaddr mac;
    a_uint32_t resp = 0;

    atd_trace(ATD_DEBUG_FUNCTRACE, ("cmd %d    ", cmdid));

    atd_assert(len >= (sizeof(atd_hdr_t) + sizeof(acfg_macaddr_t)));

    atdhdr = (atd_hdr_t *) buffer;
    paddr  = (acfg_macaddr_t *)(buffer + sizeof(atd_hdr_t));

    atd_trace(ATD_DEBUG_FUNCTRACE, (" radio %d vap %d mac %02x:%02x:%02x:%02x:%02x:%02x", \
                                 atdhdr->wifi_index, atdhdr->vap_index,\
                                 paddr->addr[0], paddr->addr[1], paddr->addr[2],
                                 paddr->addr[3], paddr->addr[4], paddr->addr[5]));

    wifisc = atsc->wifi_list[atdhdr->wifi_index];

    if(wifisc == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    memcpy(mac.sa_data, paddr, ACFG_MACADDR_LEN);

    status  = netdev_set_macaddr(wifisc->netdev, &mac);

    if (status != A_STATUS_OK) {
        atd_trace(ATD_DEBUG_CFG, ("set hw mac address error, status = %d\n", status));
        goto fail;
    }

fail:

    wmi_cmd_rsp(atsc->wmi_handle, cmdid, status, (a_uint8_t *)&resp, 
                sizeof (a_uint32_t));
}

void 
atd_tgt_set_wmmparams(void *ctx, a_uint16_t cmdid, 
                      a_uint8_t *buffer, a_int32_t len)
{

    a_status_t              status = A_STATUS_OK;
    atd_tgt_dev_t            *atsc = (atd_tgt_dev_t *)ctx;
    atd_tgt_wifi_t       *wifisc = NULL;
    atd_tgt_vap_t           *vap;
    atd_hdr_t               *atdhdr;
    struct iw_request_info  info = {0};
    union  iwreq_data       iwreq = {{0}};
    atd_wmmparams_t             *vap_param;

    atd_trace(ATD_DEBUG_FUNCTRACE, ("cmd %d    ", cmdid));

    atd_assert(len >= (sizeof(atd_hdr_t) + sizeof(atd_wmmparams_t)));
  
    atdhdr  = (atd_hdr_t *) buffer;
    vap_param    = (atd_wmmparams_t  *) (buffer + sizeof(atd_hdr_t)) ;

    vap_param->param[0] = atd_tgt_acfg2ieee(ntohl ( vap_param->param[0] ));
	vap_param->param[1] = ntohl ( vap_param->param[1] );
	vap_param->param[2] = ntohl ( vap_param->param[2] );	
    vap_param->val   = ntohl ( vap_param->val );



    atd_trace(ATD_DEBUG_FUNCTRACE, (" radio %d vap %d param  %x  val %d", \
                                 atdhdr->wifi_index, atdhdr->vap_index,\
                                 vap_param->param[0], vap_param->val));

    wifisc = atsc->wifi_list[atdhdr->wifi_index];
    if(wifisc == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    vap    = wifisc->vap_list[atdhdr->vap_index];
    if(vap == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    status = netdev_wext_handler(vap->vap_netdev, IEEE80211_IOCTL_SETWMMPARAMS,
                                 &info, &iwreq, (char *)vap_param);

fail:
    wmi_cmd_rsp(atsc->wmi_handle, cmdid, status, NULL, 0);
}

void 
atd_tgt_get_wmmparams(void *ctx, a_uint16_t cmdid, 
                  a_uint8_t *buffer, a_int32_t len)
{
    a_status_t status = A_STATUS_OK;
    atd_tgt_dev_t *atsc = (atd_tgt_dev_t *)ctx;
    atd_tgt_wifi_t *wifisc =NULL;
    atd_tgt_vap_t * vap;
    atd_hdr_t   *atdhdr;

    struct iw_request_info info;
    union  iwreq_data iwreq;

    atd_wmmparams_t  *vap_param;
    atd_param_t   paramresult;

    atd_trace(ATD_DEBUG_FUNCTRACE, ("cmd %d    ",\
                cmdid));

    atd_assert(len >= (sizeof(atd_hdr_t) + sizeof(atd_wmmparams_t)));
  
    atdhdr  = (atd_hdr_t *) buffer;
    vap_param    = (atd_wmmparams_t  *) (buffer + sizeof(atd_hdr_t)) ;

    vap_param->param[0] = atd_tgt_acfg2ieee(ntohl ( vap_param->param[0] ));
	vap_param->param[1] = ntohl ( vap_param->param[1] );
	vap_param->param[2] = ntohl ( vap_param->param[2] );
    vap_param->val   = 0;



    atd_trace(ATD_DEBUG_FUNCTRACE, (" radio %d vap %d param  %x  val %d", \
                                 atdhdr->wifi_index, atdhdr->vap_index,\
                                 vap_param->param[0], vap_param->val));

    wifisc = atsc->wifi_list[atdhdr->wifi_index];
    if(wifisc == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    vap    = wifisc->vap_list[atdhdr->vap_index];
    if(vap == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }
    status = netdev_wext_handler(vap->vap_netdev, IEEE80211_IOCTL_GETWMMPARAMS,
                                 &info, &iwreq, (char *)&vap_param->param);

    paramresult.val   =   htonl(vap_param->param[0] );
    paramresult.param = 0;
    atd_trace(ATD_DEBUG_FUNCTRACE,("Result val %d \n",vap_param->param[0]));

fail:
    wmi_cmd_rsp(atsc->wmi_handle, cmdid, status, (a_uint8_t *)&paramresult , 
                sizeof (atd_param_t));
}



void 
atd_tgt_doth_chswitch(void *ctx, a_uint16_t cmdid, 
                      a_uint8_t *buffer, a_int32_t len)
{
    a_status_t status = A_STATUS_OK;
    atd_tgt_dev_t *atsc = (atd_tgt_dev_t *)ctx;
    atd_tgt_wifi_t *wifisc =NULL;
    atd_tgt_vap_t * vap;
    atd_hdr_t   *atdhdr;

    struct iw_request_info info;
    union iwreq_data iwreq;

    acfg_doth_chsw_t *chsw ;
    a_uint32_t rsp = 0 ;
    a_uint32_t i[2] ;

    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("cmd %d    ", cmdid));

    atd_assert(len >= (sizeof(atd_hdr_t) + sizeof(acfg_doth_chsw_t)));

    atdhdr  = (atd_hdr_t *) buffer;
    chsw    = (acfg_doth_chsw_t  *) (buffer + sizeof(atd_hdr_t)) ;

    wifisc = atsc->wifi_list[atdhdr->wifi_index];
    if(wifisc == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    vap    = wifisc->vap_list[atdhdr->vap_index];
    if(vap == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    i[0] = ntohl(chsw->channel) ;
    i[1] = ntohl(chsw->time) ;

    atd_trace(ATD_DEBUG_CFG, ("channel %d time %d", \
                               i[0],i[1]));

    status = netdev_wext_handler(vap->vap_netdev, 
            IEEE80211_IOCTL_CHANSWITCH, &info, &iwreq, (a_uint8_t *)i);

    if (status != A_STATUS_OK) {
        atd_trace(ATD_DEBUG_CFG, ("error, status = %d\n", status));
        rsp = 0xFFFFFFFF;
    }
	
fail:

    wmi_cmd_rsp(atsc->wmi_handle, cmdid, status, (a_uint8_t *)&rsp, 
                sizeof (a_int32_t));
}



/** 
 * @brief Add Mac address to ACL list
 * 
 * @param ctx
 * @param cmdid
 * @param buffer
 * @param len
 */
void 
atd_tgt_addmac(void *ctx, a_uint16_t cmdid, 
                      a_uint8_t *buffer, a_int32_t len)
{
    a_status_t status = A_STATUS_OK;
    atd_tgt_dev_t *atsc = (atd_tgt_dev_t *)ctx;
    atd_tgt_wifi_t *wifisc =NULL;
    atd_tgt_vap_t * vap;
    atd_hdr_t   *atdhdr;
    struct sockaddr sa ;

    struct iw_request_info info;
    union iwreq_data iwreq;

    acfg_macaddr_t *paddr ;
    a_uint32_t rsp = 0 ;

    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("cmd %d    ", cmdid));

    atd_assert(len >= (sizeof(atd_hdr_t) + sizeof(acfg_macaddr_t)));

    atdhdr  = (atd_hdr_t *) buffer;
    paddr    = (acfg_macaddr_t  *) (buffer + sizeof(atd_hdr_t)) ;

    wifisc = atsc->wifi_list[atdhdr->wifi_index];
    if(wifisc == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    vap    = wifisc->vap_list[atdhdr->vap_index];
    if(vap == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    sa.sa_family = AF_INET ;
    memcpy(sa.sa_data, paddr->addr, ACFG_MACADDR_LEN) ;
#if 0
    printk("%s(): Got addr - %x:%x:%x:%x:%x:%x \n",__FUNCTION__,(unsigned int)paddr->addr[0],
            (unsigned int)paddr->addr[1],
                (unsigned int)paddr->addr[2],(unsigned int)paddr->addr[3],
                (unsigned int)paddr->addr[4],(unsigned int)paddr->addr[5]);
#endif

    status = netdev_wext_handler(vap->vap_netdev, 
            IEEE80211_IOCTL_ADDMAC, &info, &iwreq, (a_uint8_t *)&sa);

    if (status != A_STATUS_OK) {
        atd_trace(ATD_DEBUG_CFG, ("error, status = %d\n", status));
        rsp = 0xFFFFFFFF;
    }
	
fail:

    wmi_cmd_rsp(atsc->wmi_handle, cmdid, status, (a_uint8_t *)&rsp, 
                sizeof (a_int32_t));
}


    
    
/** 
 * @brief Delete Mac address from ACL list
 * 
 * @param ctx
 * @param cmdid
 * @param buffer
 * @param len
 */
void 
atd_tgt_delmac(void *ctx, a_uint16_t cmdid, 
                      a_uint8_t *buffer, a_int32_t len)
{
    a_status_t status = A_STATUS_OK;
    atd_tgt_dev_t *atsc = (atd_tgt_dev_t *)ctx;
    atd_tgt_wifi_t *wifisc =NULL;
    atd_tgt_vap_t * vap;
    atd_hdr_t   *atdhdr;
    struct sockaddr sa ;

    struct iw_request_info info;
    union iwreq_data iwreq;

    acfg_macaddr_t *paddr ;
    a_uint32_t rsp = 0 ;

    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("cmd %d    ", cmdid));

    atd_assert(len >= (sizeof(atd_hdr_t) + sizeof(acfg_macaddr_t)));

    atdhdr  = (atd_hdr_t *) buffer;
    paddr    = (acfg_macaddr_t  *) (buffer + sizeof(atd_hdr_t)) ;

    wifisc = atsc->wifi_list[atdhdr->wifi_index];
    if(wifisc == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    vap    = wifisc->vap_list[atdhdr->vap_index];
    if(vap == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    sa.sa_family = AF_INET ;
    memcpy(sa.sa_data, paddr->addr, ACFG_MACADDR_LEN) ;

#if 0
    printk("%s(): Got addr - %x:%x:%x:%x:%x:%x \n",
            __FUNCTION__,(unsigned int)paddr->addr[0],
                (unsigned int)paddr->addr[1],
                (unsigned int)paddr->addr[2],(unsigned int)paddr->addr[3],
                (unsigned int)paddr->addr[4],(unsigned int)paddr->addr[5]);
#endif

    status = netdev_wext_handler(vap->vap_netdev, 
            IEEE80211_IOCTL_DELMAC, &info, &iwreq, (a_uint8_t *)&sa);

    if (status != A_STATUS_OK) {
        atd_trace(ATD_DEBUG_CFG, ("error, status = %d\n", status));
        rsp = 0xFFFFFFFF;
    }
	
fail:

    wmi_cmd_rsp(atsc->wmi_handle, cmdid, status, (a_uint8_t *)&rsp, 
                sizeof (a_int32_t));
}



/** 
 * @brief Disassociate station
 * 
 * @param ctx
 * @param cmdid
 * @param buffer
 * @param len
 */
void 
atd_tgt_kickmac(void *ctx, a_uint16_t cmdid, 
                      a_uint8_t *buffer, a_int32_t len)
{
    a_status_t status = A_STATUS_OK;
    atd_tgt_dev_t *atsc = (atd_tgt_dev_t *)ctx;
    atd_tgt_wifi_t *wifisc =NULL;
    atd_tgt_vap_t * vap;
    atd_hdr_t   *atdhdr;
    struct sockaddr sa ;

    struct iw_request_info info;
    union iwreq_data iwreq;

    acfg_macaddr_t *paddr ;
    a_uint32_t rsp = 0 ;

    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("cmd %d    ", cmdid));

    atd_assert(len >= (sizeof(atd_hdr_t) + sizeof(acfg_macaddr_t)));

    atdhdr  = (atd_hdr_t *) buffer;
    paddr    = (acfg_macaddr_t  *) (buffer + sizeof(atd_hdr_t)) ;

    wifisc = atsc->wifi_list[atdhdr->wifi_index];
    if(wifisc == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    vap    = wifisc->vap_list[atdhdr->vap_index];
    if(vap == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    sa.sa_family = ARPHRD_ETHER ;
    memcpy(sa.sa_data, paddr->addr, ACFG_MACADDR_LEN) ;

    atd_print_mac_addr(paddr->addr, "Got kickmac address- ");

    status = netdev_wext_handler(vap->vap_netdev, 
            IEEE80211_IOCTL_KICKMAC, &info, &iwreq, (a_uint8_t *)&sa);

    if (status != A_STATUS_OK) {
        atd_trace(ATD_DEBUG_CFG, ("error, status = %d\n", status));
        rsp = 0xFFFFFFFF;
    }
	
fail:
    rsp = htonl(rsp);
    wmi_cmd_rsp(atsc->wmi_handle, cmdid, status, (a_uint8_t *)&rsp, 
                sizeof (a_int32_t));
}

void 
atd_tgt_pwrdown(void *ctx, a_uint16_t cmdid, 
                      a_uint8_t *buffer, a_int32_t len)
{

    a_status_t status = 0;
    a_uint32_t tmp = 0 ;
    atd_tgt_dev_t *atsc = (atd_tgt_dev_t *)ctx;
    
    wmi_cmd_rsp(atsc->wmi_handle, cmdid, status, NULL, 0);
               
    printk("Received WMI Power down request \n");

#ifndef BUILD_PARTNER_PLATFORM
    tmp = ar7240_reg_rd(AR7240_RST_MISC2);

    ar7240_reg_wr(AR7240_RST_MISC2, ((tmp & ~(RST_MISC2_PRESTN_EPPHY | RST_MISC2_PRESTN_RCPHY)) |  RST_MISC2_EP_REGWR_EN));

    ar7240_reg_wr(AR7240_SRIF_PMU1, 0x633c8176);
    ar7240_reg_wr(AR7240_SRIF_PMU2, 0x10200000);
    ar7240_reg_wr(AR7240_USB_PORTSC0, 0x18800205);

    ar7240_reg_wr(0x18116cc0, 0x1061060e);
    ar7240_reg_wr(0x18116cc8, 0x7c0c);
    ar7240_reg_wr(AR7240_PCI_LCL_PWRMGMT, 0x80);

    ar7240_reg_wr(AR7240_PCIE_PLL_CONFIG,       0x40010400);
    ar7240_reg_wr(AR7240_AUDIO_PLL_CONFIG,      0x000010bf);
    ar7240_reg_wr(AR7240_SWITCH_CLK_CONFIG,     0x00000205);
    printk("EP Shutdown \n");

    ar7240_reg_wr(0xb8116d00, 0x1061060e);
    ar7240_reg_wr(0xb8116d08, 0x5c0c);
    printk("RC Shutdown \n");

    ar7240_reg_wr(AR7240_SRIF_PMU1, 0x603c8176);
    ar7240_reg_wr(AR7240_SRIF_PMU2, 0x10240000);
    ar7240_reg_wr(AR7240_SRIF_PMU2, 0x12240000);
    ar7240_reg_wr(AR7240_USB_CONFIG, 0xd);
    ar7240_reg_wr(AR7240_USB_PHYCONFIG, 0x3);
    printk("USB Shutdown \n");
#endif /* BUILD_PARTNER_PLATFORM */

}


/** 
 * @brief Get Chan List
 * 
 * @param ctx
 * @param cmdid
 * @param buffer
 * @param len
 */
void 
atd_tgt_get_chan_list(void *ctx, a_uint16_t cmdid, 
                  a_uint8_t *buffer, a_int32_t len)
{
    a_status_t status = A_STATUS_OK;
    atd_tgt_dev_t *atsc = (atd_tgt_dev_t *)ctx;
    atd_tgt_wifi_t *wifisc = NULL;
    atd_tgt_vap_t  *vap = NULL;
    atd_hdr_t *atdhdr;
    void *chan_list = NULL;
    uint32_t length = 0;
    struct iw_request_info info;
    union  iwreq_data iwreq = {{0}};
    
    atd_trace(ATD_DEBUG_FUNCTRACE, ("cmd %d    ", cmdid));

    atd_assert(len >= (sizeof(atd_hdr_t) + sizeof(uint32_t)));
  
    atdhdr     = (atd_hdr_t *)buffer;
    length     = ntohl(*(uint32_t *)(buffer + sizeof(atd_hdr_t)));

    atd_trace(ATD_DEBUG_FUNCTRACE, (" radio %d vap %d ",
                                 atdhdr->wifi_index, atdhdr->vap_index));

    wifisc = atsc->wifi_list[atdhdr->wifi_index];
    vap    = wifisc->vap_list[atdhdr->vap_index];
    if(wifisc == NULL || vap == NULL) {
        length = 0;
        status = A_STATUS_ENXIO;
        goto fail;
    }

    vzalloc(length, chan_list);
    if(chan_list == NULL) {
        length = 0;
        status = A_STATUS_ENOMEM;
        goto fail;
    }

    info.cmd = IEEE80211_IOCTL_GETCHANLIST;
    info.flags = 0;
    iwreq.data.length = length;
    iwreq.data.flags = 0;
    iwreq.data.pointer = chan_list;

    status = netdev_wext_handler(vap->vap_netdev, IEEE80211_IOCTL_GETCHANLIST, 
                                    &info, (void *)&iwreq, iwreq.data.pointer);

    atd_trace(ATD_DEBUG_FUNCTRACE,("chan list %d\n", iwreq.data.length));

fail:
    wmi_cmd_rsp(atsc->wmi_handle, cmdid, status, (uint8_t *)chan_list, length);

    if (chan_list)
        vfree(chan_list);
}


/** 
 * @brief Get Chan Info
 * 
 * @param ctx
 * @param cmdid
 * @param buffer
 * @param len
 */
void 
atd_tgt_get_chan_info(void *ctx, a_uint16_t cmdid, 
                  a_uint8_t *buffer, a_int32_t len)
{
    a_status_t status = A_STATUS_OK;
    atd_tgt_dev_t *atsc = (atd_tgt_dev_t *)ctx;
    atd_tgt_wifi_t *wifisc = NULL;
    atd_tgt_vap_t  *vap = NULL;
    atd_hdr_t *atdhdr;
    struct iw_request_info info;
    union  iwreq_data iwreq = {{0}};
    atd_chan_info_t *chan = NULL;
    struct ieee80211req_chaninfo *chan_info = NULL;
    a_uint32_t i = 0;
    
    atd_trace(ATD_DEBUG_FUNCTRACE, ("cmd %d    ", cmdid));

    atd_assert(len >= (sizeof(atd_hdr_t)));
  
    atdhdr     = (atd_hdr_t *)buffer;

    atd_trace(ATD_DEBUG_FUNCTRACE, (" radio %d vap %d ",
                                 atdhdr->wifi_index, atdhdr->vap_index));

    wifisc = atsc->wifi_list[atdhdr->wifi_index];
    vap    = wifisc->vap_list[atdhdr->vap_index];
    if(wifisc == NULL || vap == NULL) {
        status = A_STATUS_ENXIO;
        goto fail;
    }

    vzalloc(sizeof(struct ieee80211req_chaninfo), chan_info);
    if(chan_info == NULL){
        atd_trace(ATD_DEBUG_CFG, ("\n %s : allocation failed\n", __FUNCTION__));
        status = A_STATUS_ENOMEM;
        goto fail; 
    }

    vzalloc(sizeof(atd_chan_info_t), chan);
    if(chan_info == NULL) {
        status = A_STATUS_ENOMEM;
        goto fail;
    }

    info.cmd = IEEE80211_IOCTL_GETCHANINFO;
    info.flags = 0;
    iwreq.data.length = sizeof(struct ieee80211req_chaninfo);
    iwreq.data.flags = 0;
    iwreq.data.pointer = chan_info;

    status = netdev_wext_handler(vap->vap_netdev, IEEE80211_IOCTL_GETCHANINFO, 
                                    &info, (void *)&iwreq, iwreq.data.pointer);

    if (status == A_STATUS_OK) {
        for (i = 0; i < chan_info->ic_nchans; i++) {
            chan->channel[i].freq = htons(chan_info->ic_chans[i].ic_freq);
            chan->channel[i].flags = htonl(chan_info->ic_chans[i].ic_flags);
            chan->channel[i].flag_ext = chan_info->ic_chans[i].ic_flagext;
            chan->channel[i].ieee_num = chan_info->ic_chans[i].ic_ieee;
            chan->channel[i].max_reg_pwr = chan_info->ic_chans[i].ic_maxregpower;
            chan->channel[i].max_pwr = chan_info->ic_chans[i].ic_maxpower;
            chan->channel[i].min_pwr = chan_info->ic_chans[i].ic_minpower;
            chan->channel[i].reg_class = chan_info->ic_chans[i].ic_regClassId;
            chan->channel[i].antenna_max = chan_info->ic_chans[i].ic_antennamax;
            chan->channel[i].vht_ch_freq_seg1 = chan_info->ic_chans[i].ic_vhtop_ch_freq_seg1;
            chan->channel[i].vht_ch_freq_seg2 = chan_info->ic_chans[i].ic_vhtop_ch_freq_seg2;
        }
        chan->num_channels = htonl(chan_info->ic_nchans);
    }

    iwreq.data.length = sizeof(atd_chan_info_t);

    atd_trace(ATD_DEBUG_FUNCTRACE,("chan info %d\n", iwreq.data.length));

fail:
    wmi_cmd_rsp(atsc->wmi_handle, cmdid, status, 
                (uint8_t *)chan, iwreq.data.length);

    if (chan)
        vfree(chan);
    if (chan_info)
        vfree(chan_info);
}


/** 
 * @brief Get ACL MAC Address List
 * 
 * @param ctx
 * @param cmdid
 * @param buffer
 * @param len
 */
void 
atd_tgt_get_mac_address(void *ctx, a_uint16_t cmdid, 
                  a_uint8_t *buffer, a_int32_t len)
{
    a_status_t status = A_STATUS_OK;
    atd_tgt_dev_t *atsc = (atd_tgt_dev_t *)ctx;
    atd_tgt_wifi_t *wifisc = NULL;
    atd_tgt_vap_t  *vap = NULL;
    atd_hdr_t *atdhdr;
    atd_macacl_t mac_list;
    struct iw_request_info info;
    union  iwreq_data iwreq = {{0}};
    struct sockaddr *sa;
	a_uint8_t i = 0;
    
    atd_trace(ATD_DEBUG_FUNCTRACE, ("cmd %d    ", cmdid));
  
    atdhdr     = (atd_hdr_t *)buffer;

    atd_trace(ATD_DEBUG_FUNCTRACE, (" radio %d vap %d ",
                                 atdhdr->wifi_index, atdhdr->vap_index));

    atd_assert(len >= (sizeof(atd_hdr_t)));

    wifisc = atsc->wifi_list[atdhdr->wifi_index];
    vap    = wifisc->vap_list[atdhdr->vap_index];
    if(wifisc == NULL || vap == NULL) {
        status = A_STATUS_ENXIO;
        goto fail;
    }

    vzalloc(ACFG_MACADDR_LEN * ACFG_MAX_ACL_NODE, sa);
    if(sa == NULL) {
        status = A_STATUS_ENOMEM;
        goto fail;
    }

    info.cmd = IEEE80211_IOCTL_GET_MACADDR;
    info.flags = 0;
    iwreq.data.flags = 0;
    iwreq.data.pointer = (uint8_t *)sa;

    status = netdev_wext_handler(vap->vap_netdev, IEEE80211_IOCTL_GET_MACADDR, 
                                    &info, (void *)&iwreq, iwreq.data.pointer);
	for (i = 0; i < iwreq.data.length; i++) {
		memcpy(mac_list.macaddr[i], sa[0].sa_data, ACFG_MACADDR_LEN);
	}	

    mac_list.num = htonl(iwreq.data.length);

	if (sa != NULL) {
		vfree(sa);
	}
    atd_trace(ATD_DEBUG_FUNCTRACE,("acl mac list %d\n", iwreq.data.length));

fail:
    wmi_cmd_rsp(atsc->wmi_handle, cmdid, status, (uint8_t *)&mac_list, 
							sizeof(atd_macacl_t));

}


/** 
 * @brief Get P2P param
 * 
 * @param ctx
 * @param cmdid
 * @param buffer
 * @param len
 */
void 
atd_tgt_get_p2p_param(void *ctx, a_uint16_t cmdid, 
                  a_uint8_t *buffer, a_int32_t len)
{
    a_status_t status = A_STATUS_OK;
    atd_tgt_dev_t *atsc = (atd_tgt_dev_t *)ctx;
    atd_tgt_wifi_t *wifisc = NULL;
    atd_tgt_vap_t  *vap = NULL;
    atd_hdr_t *atdhdr;
    uint32_t length = 0, param = 0;
    struct iwreq iwr = {{{0}}};
    void *resp_buf = NULL;
    
    atd_trace(ATD_DEBUG_FUNCTRACE, ("cmd %d    ", cmdid));

    atd_assert(len >= (sizeof(atd_hdr_t) + 2 * sizeof(a_uint32_t)));
  
    atdhdr     = (atd_hdr_t *)buffer;
    length     = ntohl(*(uint32_t *)(buffer + sizeof(atd_hdr_t)));
    param      = ntohl(*(uint32_t *)(buffer + sizeof(atd_hdr_t) + sizeof(uint32_t)));

    atd_trace(ATD_DEBUG_FUNCTRACE, (" radio %d vap %d ",
                                 atdhdr->wifi_index, atdhdr->vap_index));

    wifisc = atsc->wifi_list[atdhdr->wifi_index];
    vap    = wifisc->vap_list[atdhdr->vap_index];
    if(wifisc == NULL || vap == NULL) {
        length = 0;
        status = A_STATUS_ENXIO;
        goto fail;
    }

    vzalloc(length + sizeof(uint32_t), resp_buf);
    if(resp_buf == NULL) {
        length = 0;
        status = A_STATUS_ENOMEM;
        goto fail;
    }

    iwr.u.data.length = length;
    iwr.u.data.flags = param;
    iwr.u.data.pointer = (uint8_t *)resp_buf + sizeof(uint32_t);

    status = netdev_ioctl(vap->vap_netdev, (struct ifreq *)&iwr, 
                                        IEEE80211_IOCTL_P2P_BIG_PARAM);

    ((uint32_t *)resp_buf)[0] = htonl(iwr.u.data.length);

    length += sizeof(uint32_t);

    switch (param) {
        case IEEE80211_IOC_P2P_FIND_BEST_CHANNEL:
        {
            int i;
            acfg_best_freq_t *f = (acfg_best_freq_t *)iwr.u.data.pointer;
            for (i = 0; i < ACFG_MAX_BEST_CHANS; i++)
                f->freq[i] = htonl(f->freq[i]);
        }
        break;

        case IEEE80211_IOC_P2P_FETCH_FRAME:
        {
            acfg_rx_frame_t *fr = (acfg_rx_frame_t *)iwr.u.data.pointer;
            fr->freq = htonl(fr->freq);
            fr->type = htonl(fr->type);
        }
        break;

        case IEEE80211_IOC_P2P_NOA_INFO:
        {
             int i;
             acfg_noa_info_t *noa = (acfg_noa_info_t *)iwr.u.data.pointer;
             noa->tsf = htonl(noa->tsf);
             for (i = 0; i < ACFG_MAX_NOA_DESCS; i++) {
                 noa->desc[i].type_count = htonl(noa->desc[i].type_count);
                 noa->desc[i].duration = htonl(noa->desc[i].duration);
                 noa->desc[i].interval = htonl(noa->desc[i].interval);
                 noa->desc[i].start_time = htonl(noa->desc[i].start_time);
             }
        }
        break;
    }

    atd_trace(ATD_DEBUG_FUNCTRACE,("p2p get generic %d\n", iwr.u.data.length));

fail:
    wmi_cmd_rsp(atsc->wmi_handle, cmdid, status, (uint8_t *)resp_buf, length);

    if (resp_buf)
        vfree(resp_buf);
}


/** 
 * @brief Set P2P param
 * 
 * @param ctx
 * @param cmdid
 * @param buffer
 * @param len
 */
void 
atd_tgt_set_p2p_param(void *ctx, a_uint16_t cmdid, 
                  a_uint8_t *buffer, a_int32_t len)
{
    a_status_t status = A_STATUS_OK;
    atd_tgt_dev_t *atsc = (atd_tgt_dev_t *)ctx;
    atd_tgt_wifi_t *wifisc = NULL;
    atd_tgt_vap_t  *vap = NULL;
    atd_hdr_t *atdhdr;
    uint32_t length = 0, param = 0;
    struct iwreq iwr = {{{0}}};
    
    atd_trace(ATD_DEBUG_FUNCTRACE, ("cmd %d    ", cmdid));

    atd_assert(len >= (sizeof(atd_hdr_t) + sizeof(uint32_t) * 2));
  
    atdhdr     = (atd_hdr_t *)buffer;
    length     = ntohl(*(uint32_t *)(buffer + sizeof(atd_hdr_t)));
    param      = ntohl(*(uint32_t *)(buffer + sizeof(atd_hdr_t) + sizeof(uint32_t)));

    atd_trace(ATD_DEBUG_FUNCTRACE, (" radio %d vap %d ",
                                 atdhdr->wifi_index, atdhdr->vap_index));

    wifisc = atsc->wifi_list[atdhdr->wifi_index];
    vap    = wifisc->vap_list[atdhdr->vap_index];
    if(wifisc == NULL || vap == NULL) {
        status = A_STATUS_ENXIO;
        goto fail;
    }

    iwr.u.data.length = length;
    iwr.u.data.flags = param;
    iwr.u.data.pointer = buffer + sizeof(atd_hdr_t) + (2 * sizeof(uint32_t));

    switch (param) {
        case IEEE80211_IOC_P2P_SET_CHANNEL:
        {
            acfg_set_chan_t *set_chan = (acfg_set_chan_t *)iwr.u.data.pointer;
            set_chan->freq = ntohl(set_chan->freq);
            set_chan->req_id = ntohl(set_chan->req_id);
            set_chan->time = ntohl(set_chan->time);
        }
        break;

        case IEEE80211_IOC_P2P_SEND_ACTION:
        {
            acfg_send_act_t *act = (acfg_send_act_t *)iwr.u.data.pointer;
            act->freq = ntohl(act->freq);
        }
        break;

        case IEEE80211_IOC_SCAN_REQ:
        {
            int i;
            acfg_scan_req_t *scan = (acfg_scan_req_t *)iwr.u.data.pointer;
            for (i = 0; i < ACFG_MAX_SCAN_FREQ; i++)
                scan->freq[i] = ntohl(scan->freq[i]);
            scan->len = ntohs(scan->len);
        }
        break;

        case IEEE80211_IOC_P2P_GO_NOA:
        {
            int i, num;
            uint16_t tmp;
            acfg_noa_req_t *noa = (acfg_noa_req_t *)iwr.u.data.pointer;
            num = length / sizeof(*noa);
            for (i = 0; i < num && i < ACFG_MAX_NOA_REQ; i++) {
                memcpy(&tmp, noa[i].offset_next_tbtt, sizeof(tmp));
                tmp = ntohs(tmp);
                memcpy(noa[i].offset_next_tbtt, &tmp, sizeof(tmp));
                memcpy(&tmp, noa[i].duration, sizeof(tmp));
                tmp = ntohs(tmp);
                memcpy(noa[i].duration, &tmp, sizeof(tmp));
            }
        }
        break;
    }

    status = netdev_ioctl(vap->vap_netdev, (struct ifreq *)&iwr, 
                                        IEEE80211_IOCTL_P2P_BIG_PARAM);

    atd_trace(ATD_DEBUG_FUNCTRACE,("p2p set param %d\n", iwr.u.data.length));

fail:
    wmi_cmd_rsp(atsc->wmi_handle, cmdid, status, NULL, 0); 
}


void 
atd_tgt_acl_addmac(void *ctx, a_uint16_t cmdid, 
                  a_uint8_t *buffer, a_int32_t len)
{
	a_status_t status = A_STATUS_OK;
    atd_tgt_dev_t *atsc = (atd_tgt_dev_t *)ctx;
    atd_tgt_wifi_t *wifisc = NULL;
    atd_tgt_vap_t  *vap = NULL;
    atd_hdr_t *atdhdr;
	atd_macaddr_t *mac;
	struct iw_request_info info;
    union  iwreq_data iwreqdata;
    struct sockaddr sa;


    atd_trace(ATD_DEBUG_FUNCTRACE, ("cmd %d    ", \
                cmdid));

    atd_assert(len >= (sizeof(atd_hdr_t) + sizeof(acfg_macaddr_t)));

    atdhdr     = (atd_hdr_t *)buffer;
	wifisc = atsc->wifi_list[atdhdr->wifi_index];

    if(wifisc == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }
    vap    = wifisc->vap_list[atdhdr->vap_index];
	if(vap == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }
    mac    = (atd_macaddr_t  *) (buffer + sizeof(atd_hdr_t)) ;
	memcpy(sa.sa_data, mac->addr, ACFG_MACADDR_LEN);
	info.cmd = IEEE80211_IOCTL_ADDMAC ;
    info.flags = 0;
    iwreqdata.data.length = ACFG_MACADDR_LEN;
    iwreqdata.data.flags = 0 ;
	status = netdev_wext_handler(vap->vap_netdev, \
                                 IEEE80211_IOCTL_ADDMAC,
                                 &info,
                                 (void *)&iwreqdata,
                                 (char *)&sa);

	if(status != A_STATUS_OK){
        atd_trace(ATD_DEBUG_CFG, ("ssid set erro \n"));
    }

fail:
    wmi_cmd_rsp(atsc->wmi_handle, cmdid, status, NULL, 0);
}


void 
atd_tgt_acl_delmac(void *ctx, a_uint16_t cmdid, 
                  a_uint8_t *buffer, a_int32_t len)
{
	a_status_t status = A_STATUS_OK;
    atd_tgt_dev_t *atsc = (atd_tgt_dev_t *)ctx;
    atd_tgt_wifi_t *wifisc = NULL;
    atd_tgt_vap_t  *vap = NULL;
    atd_hdr_t *atdhdr;
	atd_macaddr_t *mac;
	struct iw_request_info info;
    union  iwreq_data iwreqdata;
    struct sockaddr sa;


    atd_trace(ATD_DEBUG_FUNCTRACE, ("cmd %d    ", \
                cmdid));

    atd_assert(len >= (sizeof(atd_hdr_t) + sizeof(acfg_macaddr_t)));

    atdhdr     = (atd_hdr_t *)buffer;
	wifisc = atsc->wifi_list[atdhdr->wifi_index];

    if(wifisc == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }
    vap    = wifisc->vap_list[atdhdr->vap_index];
	if(vap == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }
    mac    = (atd_macaddr_t  *) (buffer + sizeof(atd_hdr_t)) ;
	memcpy(sa.sa_data, mac->addr, ACFG_MACADDR_LEN);
	info.cmd = IEEE80211_IOCTL_DELMAC ;
    info.flags = 0;
    iwreqdata.data.length = ACFG_MACADDR_LEN;
    iwreqdata.data.flags = 0 ;
	status = netdev_wext_handler(vap->vap_netdev, \
                                 IEEE80211_IOCTL_DELMAC,
                                 &info,
                                 (void *)&iwreqdata,
                                 (char *)&sa);

	if(status != A_STATUS_OK){
        atd_trace(ATD_DEBUG_CFG, ("ssid set erro \n"));
    }

fail:
    wmi_cmd_rsp(atsc->wmi_handle, cmdid, status, NULL, 0);
}

void 
atd_tgt_convert_profile(struct ieee80211_profile *profile,
						atd_radio_vap_info_t *atd_profile)
{
	a_uint8_t i, kid;

	strncpy(atd_profile->radio_name, profile->radio_name, IFNAMSIZ);
	atd_profile->chan = profile->channel;
	atd_profile->freq = htonl(profile->freq);
	atd_profile->country_code = htons(profile->cc);
	memcpy(atd_profile->radio_mac, profile->radio_mac, ACFG_MACADDR_LEN);
	for (i = 0; i < profile->num_vaps; i++) {
		strncpy(atd_profile->vap_info[i].vap_name,
                                    profile->vap_profile[i].name,
                                    IFNAMSIZ);
		atd_profile->vap_info[i].phymode = 
									htonl(profile->vap_profile[i].phymode);
		memcpy(atd_profile->vap_info[i].vap_mac,
                profile->vap_profile[i].vap_mac,
                ACFG_MACADDR_LEN);
		switch (profile->vap_profile[i].sec_method)
		{
			case IEEE80211_AUTH_NONE:
            case IEEE80211_AUTH_OPEN:
                if(profile->vap_profile[i].cipher & (1 << IEEE80211_CIPHER_WEP))

                {
                    atd_profile->vap_info[i].cipher =
                                        ACFG_WLAN_PROFILE_CIPHER_METH_WEP;
                    atd_profile->vap_info[i].sec_method =
                                            ACFG_WLAN_PROFILE_SEC_METH_OPEN;
                } else {
                    atd_profile->vap_info[i].cipher =
                                        ACFG_WLAN_PROFILE_CIPHER_METH_NONE;
                    atd_profile->vap_info[i].sec_method =
                                            ACFG_WLAN_PROFILE_SEC_METH_OPEN;
                }
                break;
			 case IEEE80211_AUTH_AUTO:
                atd_profile->vap_info[i].sec_method =
                                            ACFG_WLAN_PROFILE_SEC_METH_AUTO;
                if(profile->vap_profile[i].cipher &
                        (1 << IEEE80211_CIPHER_WEP))

                {
                    atd_profile->vap_info[i].cipher =
                                        ACFG_WLAN_PROFILE_CIPHER_METH_WEP;
                }
                break;
			 case IEEE80211_AUTH_SHARED:
                atd_profile->vap_info[i].sec_method =
                                            ACFG_WLAN_PROFILE_SEC_METH_SHARED;
                break;
            case IEEE80211_AUTH_WPA:
                atd_profile->vap_info[i].sec_method =
                                            ACFG_WLAN_PROFILE_SEC_METH_WPA;
                break;
            case IEEE80211_AUTH_RSNA:
                atd_profile->vap_info[i].sec_method =
                                            ACFG_WLAN_PROFILE_SEC_METH_WPA2;
                break;
		}
		if (profile->vap_profile[i].cipher & (1 << IEEE80211_CIPHER_TKIP)) {
            atd_profile->vap_info[i].cipher |=
                                        ACFG_WLAN_PROFILE_CIPHER_METH_TKIP;
        }
        if ((profile->vap_profile[i].cipher & (1 << IEEE80211_CIPHER_AES_OCB))
                                        ||
            (profile->vap_profile[i].cipher & (1 <<IEEE80211_CIPHER_AES_CCM)))
        {
            atd_profile->vap_info[i].cipher |=
                                        ACFG_WLAN_PROFILE_CIPHER_METH_AES;
        }
		for (kid = 0; kid < 4; kid++) {
			if (profile->vap_profile[i].wep_key_len[kid]) {
				memcpy(atd_profile->vap_info[i].wep_key,
                	    profile->vap_profile[i].wep_key[kid],
                    	profile->vap_profile[i].wep_key_len[kid]);
        		atd_profile->vap_info[i].wep_key_len =
                	    profile->vap_profile[i].wep_key_len[kid];
				atd_profile->vap_info[i].wep_key_idx = kid;
			}
		}
	}
	atd_profile->num_vaps = profile->num_vaps;		
}

void atd_convert_ad_hton(void *out, void *in, int ad_id)
{
    if(ad_id == (0x8000 | SPECTRAL_GET_CHANINFO))
    {
        int i;
        atd_SPECTRAL_CHAN_STATS_T *atd = (atd_SPECTRAL_CHAN_STATS_T *)out;
        SPECTRAL_CHAN_STATS_T *chan_info = (SPECTRAL_CHAN_STATS_T *)in;

        for(i = 0; i < MAX_NUM_CHANNELS; i++)
        {
            atd[i].cycle_count = htonl(chan_info[i].cycle_count);
            atd[i].channel_load = htonl(chan_info[i].channel_load);
            atd[i].per = htonl(chan_info[i].per);
            atd[i].noisefloor = htonl(chan_info[i].noisefloor);
            atd[i].comp_usablity = htons(chan_info[i].comp_usablity);
            atd[i].maxregpower = chan_info[i].maxregpower;
        }
    }
    else if (ad_id == (0x8000 | SPECTRAL_GET_CONFIG))
    {
        atd_HAL_SPECTRAL_PARAM_t *atd = (atd_HAL_SPECTRAL_PARAM_t *)out;
        HAL_SPECTRAL_PARAM *gc = (HAL_SPECTRAL_PARAM *)in;

        atd->ss_fft_period = htons(gc->ss_fft_period);
        atd->ss_period = htons(gc->ss_period);
        atd->ss_count = htons(gc->ss_count);
        atd->ss_short_report = htons(gc->ss_short_report);
        atd->radar_bin_thresh_sel = gc->radar_bin_thresh_sel;
        atd->ss_spectral_pri = htons(gc->ss_spectral_pri);
        atd->ss_fft_size = htons(gc->ss_fft_size);
        atd->ss_gc_ena = htons(gc->ss_gc_ena);
        atd->ss_restart_ena = htons(gc->ss_restart_ena);
        atd->ss_noise_floor_ref = htons(gc->ss_noise_floor_ref);
        atd->ss_init_delay = htons(gc->ss_init_delay);
        atd->ss_nb_tone_thr = htons(gc->ss_nb_tone_thr);
        atd->ss_str_bin_thr = htons(gc->ss_str_bin_thr);
        atd->ss_wb_rpt_mode = htons(gc->ss_wb_rpt_mode);
        atd->ss_rssi_rpt_mode = htons(gc->ss_rssi_rpt_mode);
        atd->ss_rssi_thr = htons(gc->ss_rssi_thr);
        atd->ss_pwr_format = htons(gc->ss_pwr_format);
        atd->ss_rpt_mode = htons(gc->ss_rpt_mode);
        atd->ss_bin_scale = htons(gc->ss_bin_scale);
        atd->ss_dBm_adj = htons(gc->ss_dBm_adj);
        atd->ss_chn_mask = htons(gc->ss_chn_mask);
        memcpy(atd->ss_nf_cal, gc->ss_nf_cal, AH_MAX_CHAINS*2);
        memcpy(atd->ss_nf_pwr, gc->ss_nf_pwr, AH_MAX_CHAINS*2);
        atd->ss_nf_temp_data = htonl(gc->ss_nf_temp_data);
    }
    else
        *((a_uint32_t *)out) = htonl((*((a_uint32_t *)in)));
}

void
atd_tgt_phyerr(void *ctx, a_uint16_t cmdid,
                  a_uint8_t *buffer, a_int32_t len)
{
    a_status_t status      = A_STATUS_OK;
    atd_tgt_dev_t *atsc    = (atd_tgt_dev_t *)ctx;
    atd_tgt_wifi_t *wifisc = NULL;
    struct ifreq ifr;
    atd_hdr_t *atdhdr;
    atd_ath_diag_t *phyerr;
    struct ath_diag ath_diag;
    void *indata = NULL, *outdata = NULL, *atd_outdata = NULL;

    atd_trace(ATD_DEBUG_FUNCTRACE,(" cmd %d ", cmdid));

    atdhdr   = (atd_hdr_t *)buffer;
    phyerr   = (atd_ath_diag_t *)(buffer + sizeof(atd_hdr_t)) ;

    atd_trace(ATD_DEBUG_FUNCTRACE, (" radio %d vap %d ", \
                          atdhdr->wifi_index, atdhdr->vap_index));

    wifisc = atsc->wifi_list[atdhdr->wifi_index];
    if (wifisc == NULL) {
        status = A_STATUS_ENXIO;
        goto fail;
    }

    memset(&ath_diag, 0, sizeof(ath_diag));
    memcpy(ath_diag.ad_name, phyerr->ad_name, ACFG_MAX_IFNAME);
    ath_diag.ad_id = ntohs(phyerr->ad_id);
    ath_diag.ad_in_size = ntohs(phyerr->ad_in_size);
    ath_diag.ad_out_size = ntohl(phyerr->ad_out_size);

    if (ath_diag.ad_id & 0x4000) {
        indata = kzalloc(ath_diag.ad_in_size, GFP_KERNEL);
        if (indata == NULL) {
            status = A_STATUS_ENOMEM;
            goto fail;
        }
        /* Currently we support only bangradar, enable, disable and ignorecac.
         * Out of these only ignorecac has a in-param, of size 4 bytes. */
        if(ath_diag.ad_id == (0x4000 | SPECTRAL_SET_CONFIG))
        {
            HAL_SPECTRAL_PARAM *in = (HAL_SPECTRAL_PARAM *)indata;
            atd_HAL_SPECTRAL_PARAM_t *atd_in = (atd_HAL_SPECTRAL_PARAM_t *)(phyerr + 1);

            in->ss_fft_period = ntohs(atd_in->ss_fft_period);
            in->ss_period = ntohs(atd_in->ss_period);
            in->ss_count = ntohs(atd_in->ss_count);
            in->ss_short_report = ntohs(atd_in->ss_short_report);
            in->radar_bin_thresh_sel = atd_in->radar_bin_thresh_sel;
            in->ss_spectral_pri = ntohs(atd_in->ss_spectral_pri);
            in->ss_fft_size = ntohs(atd_in->ss_fft_size);
            in->ss_gc_ena = ntohs(atd_in->ss_gc_ena);
            in->ss_restart_ena = ntohs(atd_in->ss_restart_ena);
            in->ss_noise_floor_ref = ntohs(atd_in->ss_noise_floor_ref);
            in->ss_init_delay = ntohs(atd_in->ss_init_delay);
            in->ss_nb_tone_thr = ntohs(atd_in->ss_nb_tone_thr);
            in->ss_str_bin_thr = ntohs(atd_in->ss_str_bin_thr);
            in->ss_wb_rpt_mode = ntohs(atd_in->ss_wb_rpt_mode);
            in->ss_rssi_rpt_mode = ntohs(atd_in->ss_rssi_rpt_mode);
            in->ss_rssi_thr = ntohs(atd_in->ss_rssi_thr);
            in->ss_pwr_format = ntohs(atd_in->ss_pwr_format);
            in->ss_rpt_mode = ntohs(atd_in->ss_rpt_mode);
            in->ss_bin_scale = ntohs(atd_in->ss_bin_scale);
            in->ss_dBm_adj = ntohs(atd_in->ss_dBm_adj);
            in->ss_chn_mask = ntohs(atd_in->ss_chn_mask);
            memcpy(in->ss_nf_cal, atd_in->ss_nf_cal, AH_MAX_CHAINS*2);
            memcpy(in->ss_nf_pwr, atd_in->ss_nf_pwr, AH_MAX_CHAINS*2);
            in->ss_nf_temp_data = ntohl(atd_in->ss_nf_temp_data);
        }
        else if (ath_diag.ad_in_size == sizeof(a_uint32_t))
            *((a_uint32_t *)indata) = ntohl((*((a_uint32_t *)(phyerr + 1))));
        ath_diag.ad_in_data = indata;
    }
    if(ath_diag.ad_out_size)
    {
        vzalloc(ath_diag.ad_out_size, outdata);
        vzalloc(ath_diag.ad_out_size, atd_outdata);
        if ((outdata == NULL) || (atd_outdata == NULL)) {
            status = A_STATUS_ENOMEM;
            goto fail;
        }
        ath_diag.ad_out_data = outdata;
    }

    memset(&ifr, 0, sizeof(ifr));
    ifr.ifr_data = (void *)&ath_diag;

    status = netdev_ioctl(wifisc->netdev, &ifr, LINUX_PVT_PHYERR);

    if (status != A_STATUS_OK) {
        atd_trace(ATD_DEBUG_CFG, (" PHYERR results fetch error \n"));
    }

    if(ath_diag.ad_out_size)
        atd_convert_ad_hton(atd_outdata, ath_diag.ad_out_data, ath_diag.ad_id);

fail:
    wmi_cmd_rsp(atsc->wmi_handle, cmdid, status, 
            atd_outdata, 
            ath_diag.ad_out_size);

    if (indata)
        kfree(indata);
    if (outdata)
        vfree(outdata);
    if (atd_outdata)
        vfree(atd_outdata);

    atd_trace(ATD_DEBUG_FUNCTRACE,(" End"));
}

void
atd_tgt_get_profile(void *ctx, a_uint16_t cmdid, 
                  a_uint8_t *buffer, a_int32_t len)
{
	a_status_t status = A_STATUS_OK;
    atd_tgt_dev_t *atsc = (atd_tgt_dev_t *)ctx;
    atd_tgt_wifi_t *wifisc =NULL;
    atd_hdr_t   *atdhdr;
	atd_radio_vap_info_t result;
	struct ieee80211_profile *profile;
    struct ifreq ifr;

    atd_assert(len >= (sizeof(atd_hdr_t)));

    atdhdr  = (atd_hdr_t *) buffer;
	wifisc = atsc->wifi_list[atdhdr->wifi_index];
	memset(&result, 0, sizeof (atd_radio_vap_info_t));	
	profile = (struct ieee80211_profile *)kmalloc (
                        sizeof (struct ieee80211_profile), GFP_KERNEL);
    if (profile == NULL) {
        status = A_STATUS_ENOMEM;
        goto fail;
    }

	memset(profile, 0, sizeof (struct ieee80211_profile));
    ifr.ifr_data = (void *)profile;
    status  = netdev_ioctl(wifisc->netdev, &ifr, SIOCG80211PROFILE);
    if(status != A_STATUS_OK)
    {
        goto fail;
    }

	atd_tgt_convert_profile(profile, &result);
	
fail:
	if (profile != NULL) {
        kfree(profile);
    }
    wmi_cmd_rsp(atsc->wmi_handle, cmdid, status, (a_uint8_t *)&result , 
                sizeof (atd_radio_vap_info_t));
}

static void atd_tgt_set_chnwidthswitch (void *ctx, a_uint16_t cmdid,
                           a_uint8_t *buffer, a_int32_t len)
{
    a_status_t      status = A_STATUS_OK;
    atd_tgt_dev_t  *atsc   = (atd_tgt_dev_t *)ctx;
    atd_tgt_wifi_t *wifisc = NULL;
    atd_tgt_vap_t  *vap    = NULL;
    atd_hdr_t      *atdhdr;
    struct iw_request_info info = {0};
    union  iwreq_data iwreq ={0};
    atd_set_chn_width_t *chwidthswitch;

    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("cmd %d    ", cmdid));

    atd_assert(len >= (sizeof(atd_set_chn_width_t) + sizeof(atd_hdr_t)));
    atdhdr      = (atd_hdr_t *) buffer;

    chwidthswitch = (atd_set_chn_width_t  *) (buffer + sizeof(atd_hdr_t)) ;

    chwidthswitch->setchnwidth[0] = ntohl(chwidthswitch->setchnwidth[0]);
    chwidthswitch->setchnwidth[1] = ntohl(chwidthswitch->setchnwidth[1]);
    chwidthswitch->setchnwidth[2] = ntohl(chwidthswitch->setchnwidth[2]);

    wifisc = atsc->wifi_list[atdhdr->wifi_index];
    if(wifisc == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    vap    = wifisc->vap_list[atdhdr->vap_index];
    if(vap == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    info.cmd = IEEE80211_IOCTL_CHN_WIDTHSWITCH ;
    info.flags = 0 ;
    iwreq.data.length = sizeof(atd_set_chn_width_t);
    iwreq.data.flags = 0 ;
    iwreq.data.pointer = (a_uint32_t *)chwidthswitch->setchnwidth;

    status = netdev_wext_handler(vap->vap_netdev, IEEE80211_IOCTL_CHN_WIDTHSWITCH, 
                    &info,(void *)&iwreq, (char *)chwidthswitch->setchnwidth);

   if(status != A_STATUS_OK){
        atd_trace(ATD_DEBUG_CFG, ("channel width switch Set error \n"));
    }

fail:
    wmi_cmd_rsp(atsc->wmi_handle, cmdid, status, NULL, 0);
    atd_trace(ATD_DEBUG_FUNCTRACE ,(" End"));
}

static void atd_tgt_set_atf_ssid(void *ctx, a_uint16_t cmdid,
        a_uint8_t *buffer, a_int32_t len)
{
    a_status_t      status = A_STATUS_OK;
    atd_tgt_dev_t  *atsc   = (atd_tgt_dev_t *)ctx;
    atd_tgt_wifi_t *wifisc = NULL;
    atd_tgt_vap_t  *vap    = NULL;
    atd_hdr_t      *atdhdr;
    struct ifreq   ifr;
    atd_atf_ssid_val_t *ssid;
    acfg_atf_ssid_val_t *atf_ssid = NULL;

    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("cmd %d    ", cmdid));

    atd_assert(len >= (sizeof(atd_atf_ssid_val_t) + sizeof(atd_hdr_t)));
    atdhdr      = (atd_hdr_t *) buffer;

    ssid = (atd_atf_ssid_val_t *) (buffer + sizeof(atd_hdr_t)) ;

    atf_ssid = (acfg_atf_ssid_val_t *)kmalloc (
            sizeof (acfg_atf_ssid_val_t), GFP_KERNEL);
    if (atf_ssid == NULL) {
        status = A_STATUS_ENOMEM;
        goto fail;
    }

    atf_ssid->id_type = ntohs(ssid->id_type);
    atf_ssid->value = ntohl(ssid->value);
    memcpy(atf_ssid->ssid, ssid->ssid, ACFG_MAX_SSID_LEN+1);

    wifisc = atsc->wifi_list[atdhdr->wifi_index];
    if(wifisc == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    vap = wifisc->vap_list[atdhdr->vap_index];
    if(vap == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    ifr.ifr_data = (void *)atf_ssid;

    status = netdev_ioctl(vap->vap_netdev, &ifr, IEEE80211_IOCTL_CONFIG_GENERIC);

    if(status != A_STATUS_OK){
        atd_trace(ATD_DEBUG_CFG, ("ATF SSID Set error \n"));
    }

fail:
    if(atf_ssid)
        kfree(atf_ssid);
    wmi_cmd_rsp(atsc->wmi_handle, cmdid, status, NULL, 0);
    atd_trace(ATD_DEBUG_FUNCTRACE ,(" End"));
}

static void atd_tgt_set_atf_sta(void *ctx, a_uint16_t cmdid,
        a_uint8_t *buffer, a_int32_t len)
{
    a_status_t      status = A_STATUS_OK;
    atd_tgt_dev_t  *atsc   = (atd_tgt_dev_t *)ctx;
    atd_tgt_wifi_t *wifisc = NULL;
    atd_tgt_vap_t  *vap    = NULL;
    atd_hdr_t      *atdhdr;
    struct ifreq   ifr;
    atd_atf_sta_val_t *sta;
    acfg_atf_sta_val_t *atf_sta = NULL;

    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("cmd %d    ", cmdid));

    atd_assert(len >= (sizeof(atd_atf_sta_val_t) + sizeof(atd_hdr_t)));
    atdhdr      = (atd_hdr_t *) buffer;

    sta = (atd_atf_sta_val_t *) (buffer + sizeof(atd_hdr_t)) ;

    atf_sta = (acfg_atf_sta_val_t *)kmalloc (
            sizeof (acfg_atf_sta_val_t), GFP_KERNEL);
    if (atf_sta == NULL) {
        status = A_STATUS_ENOMEM;
        goto fail;
    }

    atf_sta->id_type = ntohs(sta->id_type);
    atf_sta->value = ntohl(sta->value);
    memcpy(atf_sta->sta_mac, sta->sta_mac, ACFG_MACADDR_LEN);

    wifisc = atsc->wifi_list[atdhdr->wifi_index];
    if(wifisc == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    vap = wifisc->vap_list[atdhdr->vap_index];
    if(vap == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    ifr.ifr_data = (void *)atf_sta;

    status = netdev_ioctl(vap->vap_netdev, &ifr, IEEE80211_IOCTL_CONFIG_GENERIC);

    if(status != A_STATUS_OK){
        atd_trace(ATD_DEBUG_CFG, ("channel width switch Set error \n"));
    }

fail:
    if(atf_sta)
        kfree(atf_sta);
    wmi_cmd_rsp(atsc->wmi_handle, cmdid, status, NULL, 0);
    atd_trace(ATD_DEBUG_FUNCTRACE ,(" End"));
}

static void atd_tgt_set_country (void *ctx, a_uint16_t cmdid,
                           a_uint8_t *buffer, a_int32_t len)
{

    a_status_t      status = A_STATUS_OK;
    atd_tgt_dev_t  *atsc   = (atd_tgt_dev_t *)ctx;
    atd_tgt_wifi_t *wifisc = NULL;
    atd_tgt_vap_t  *vap    = NULL;
    atd_hdr_t      *atdhdr;
    atd_set_country_t *countrycode;
    struct iw_request_info info = {0};
    union  iwreq_data iwreq = {0};

    atd_trace(ATD_DEBUG_FUNCTRACE,(" cmd %d ", cmdid));
    atd_assert(len >= (sizeof(atd_hdr_t) + sizeof(atd_set_country_t)));
    atdhdr  = (atd_hdr_t *) buffer;
    countrycode = (atd_set_country_t  *) (buffer + sizeof(atd_hdr_t)) ;

    wifisc = atsc->wifi_list[atdhdr->wifi_index];
    if(wifisc == NULL){
        status = A_STATUS_ENXIO;
        goto fail;
    }

    iwreq.data.length = sizeof(atd_set_country_t);
    iwreq.data.flags = 0 ;
    iwreq.data.pointer = (a_uint8_t *)countrycode->setcountry;

    status = netdev_wext_handler(wifisc->netdev, ATH_IOCTL_SETCOUNTRY,
                                 &info, &iwreq, (char *)countrycode->setcountry);

    if(status != A_STATUS_OK){
         atd_trace(ATD_DEBUG_CFG, ("Country set error \n"));
    }

fail:
    wmi_cmd_rsp(atsc->wmi_handle, cmdid, status, NULL, 0);

}                                                                     



