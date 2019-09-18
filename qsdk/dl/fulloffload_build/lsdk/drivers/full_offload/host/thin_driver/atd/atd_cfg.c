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

#include "atd_internal.h"
#include <band_steering.h>

static a_status_t atd_cfg_set_ssid(adf_drv_handle_t hdl, acfg_ssid_t  *ssid);
static a_status_t atd_cfg_get_ssid(adf_drv_handle_t hdl, acfg_ssid_t  *ssid);
static a_status_t atd_cfg_set_testmode(adf_drv_handle_t hdl, 
                                       acfg_testmode_t  *testmode);
static a_status_t atd_cfg_get_testmode(adf_drv_handle_t hdl, 
                                       acfg_testmode_t  *testmode);
static a_status_t atd_cfg_get_rssi(adf_drv_handle_t hdl, acfg_rssi_t  *rssi);
static a_status_t atd_cfg_get_custdata(adf_drv_handle_t hdl, acfg_custdata_t  *custdata);
static a_status_t atd_cfg_delete_vap(adf_drv_handle_t  hdl );
static a_status_t atd_cfg_set_vap_param(adf_drv_handle_t hdl, 
                                        a_uint32_t param,  a_uint32_t value);
static a_status_t atd_cfg_get_vap_param(adf_drv_handle_t hdl, 
                                        a_uint32_t param,  a_uint32_t *value);
static a_status_t atd_cfg_set_vap_vendor_param(adf_drv_handle_t hdl, acfg_vendor_param_req_t *req);
static a_status_t atd_cfg_get_vap_vendor_param(adf_drv_handle_t hdl, acfg_vendor_param_req_t *req);

static a_status_t atd_cfg_set_wifi_param(adf_drv_handle_t hdl, 
                                         a_uint32_t param, a_uint32_t value);
static a_status_t atd_cfg_get_wifi_param(adf_drv_handle_t hdl, 
                                         a_uint32_t param, a_uint32_t *value);
a_status_t atd_cfg_get_opmode(adf_drv_handle_t  hdl, acfg_opmode_t  *mode);
a_status_t atd_cfg_set_chmode(adf_drv_handle_t  hdl, acfg_chmode_t  *mode);
a_status_t atd_cfg_get_chmode(adf_drv_handle_t hdl, acfg_chmode_t  *mode);
static a_status_t atd_cfg_get_wireless_name(adf_drv_handle_t hdl, 
                                           u_int8_t *name,  a_uint32_t maxlen);
static a_status_t atd_cfg_set_ap(adf_drv_handle_t hdl, acfg_macaddr_t *mac);
static a_status_t atd_cfg_get_ap(adf_drv_handle_t hdl, acfg_macaddr_t *mac);
static a_status_t atd_cfg_get_rate(adf_drv_handle_t hdl, a_uint32_t *rate);
static a_status_t atd_cfg_set_powmgmt(adf_drv_handle_t hdl, acfg_powmgmt_t *pm);
static a_status_t atd_cfg_get_powmgmt(adf_drv_handle_t hdl, acfg_powmgmt_t *pm);
static a_status_t atd_cfg_set_scan(adf_drv_handle_t hdl, acfg_set_scan_t *scan);
static a_status_t atd_cfg_get_scan_results(adf_drv_handle_t hdl, 
                                           acfg_scan_t *scan);
static a_status_t atd_cfg_get_ath_stats(adf_drv_handle_t hdl, 
                                           acfg_ath_stats_t *ath_stats);
static a_status_t atd_cfg_clr_ath_stats(adf_drv_handle_t hdl, 
                                           acfg_ath_stats_t *ath_stats);
static a_status_t atd_cfg_get_range(adf_drv_handle_t hdl, acfg_range_t *range);

static a_status_t atd_cfg_set_chan(adf_drv_handle_t hdl, a_uint8_t ieee_chan);
static a_status_t atd_cfg_get_chan(adf_drv_handle_t hdl, a_uint8_t *ieee_chan);
static a_status_t atd_cfg_set_freq(adf_drv_handle_t hdl, acfg_freq_t  *freq);
static a_status_t atd_cfg_get_freq(adf_drv_handle_t hdl, acfg_freq_t  *freq);
static a_status_t atd_cfg_set_rts(adf_drv_handle_t hdl, acfg_rts_t *rts);
static a_status_t atd_cfg_get_rts(adf_drv_handle_t hdl, acfg_rts_t *rts);
static a_status_t atd_cfg_get_frag(adf_drv_handle_t hdl, acfg_frag_t *frag);
static a_status_t atd_cfg_set_frag(adf_drv_handle_t hdl, acfg_frag_t *frag);
static a_status_t atd_cfg_set_txpow(adf_drv_handle_t hdl, acfg_txpow_t *txpow);
static a_status_t atd_cfg_get_txpow(adf_drv_handle_t hdl, acfg_txpow_t *txpow);
static a_status_t atd_cfg_set_encode(adf_drv_handle_t hdl, 
                                     acfg_encode_t  *encode);
static a_status_t atd_cfg_get_encode(adf_drv_handle_t hdl, 
                                     acfg_encode_t  *encode);
static a_status_t atd_cfg_set_rate(adf_drv_handle_t hdl, acfg_rate_t  *rate);
static a_status_t atd_cfg_get_stats(adf_drv_handle_t hdl, acfg_stats_t  *stat);

static a_status_t atd_cfg_set_opmode(adf_drv_handle_t hdl, acfg_opmode_t mode);
static a_status_t atd_cfg_set_phymode(adf_drv_handle_t  hdl, 
                                      acfg_phymode_t  mode);
static a_status_t atd_cfg_get_phymode(adf_drv_handle_t  hdl, 
                                      acfg_phymode_t  *mode);
static a_status_t atd_cfg_get_stainfo(adf_drv_handle_t hdl, a_uint8_t *buff, 
                               a_uint32_t buflen);
static a_status_t atd_cfg_tx99tool(adf_drv_handle_t hdl, 
                                   acfg_tx99_t *tx99_wcmd);

static a_status_t atd_cfg_set_reg(adf_drv_handle_t hdl, 
                                     a_uint32_t offset, a_uint32_t  value);
static a_status_t atd_cfg_get_reg(adf_drv_handle_t hdl, 
                                     a_uint32_t offset, a_uint32_t *value);
static a_status_t atd_cfg_set_hwaddr(adf_drv_handle_t hdl, acfg_macaddr_t *mac);

static a_status_t atd_cfg_set_vap_wmmparams(adf_drv_handle_t hdl, 
                                        a_uint32_t *param,  a_uint32_t value);

static a_status_t atd_cfg_get_vap_wmmparams(adf_drv_handle_t hdl, 
                                        a_uint32_t *param,  a_uint32_t *value);

static a_status_t atd_cfg_nawds_config(adf_drv_handle_t hdl, 
                                        acfg_nawds_cfg_t *nawds_config);

static a_status_t atd_cfg_doth_chsw(adf_drv_handle_t hdl, 
                                        acfg_doth_chsw_t *chsw);

static a_status_t atd_cfg_addmac(adf_drv_handle_t hdl, 
                                        acfg_macaddr_t *addr);

static a_status_t atd_cfg_delmac(adf_drv_handle_t hdl, 
                                        acfg_macaddr_t *addr);

static a_status_t atd_cfg_kickmac(adf_drv_handle_t hdl, 
                                        acfg_macaddr_t *addr);

static a_status_t atd_cfg_set_mlme(adf_drv_handle_t hdl, 
                                        acfg_mlme_t *mlme);

static a_status_t atd_cfg_send_mgmt(adf_drv_handle_t hdl, 
                                        acfg_mgmt_t *addr);

static a_status_t atd_cfg_set_optie(adf_drv_handle_t hdl, 
                                        acfg_ie_t *ie);

static a_status_t atd_cfg_set_filterframe(adf_drv_handle_t hdl, 
                                        acfg_filter_t *filterframe);

static a_status_t atd_cfg_set_acparams(adf_drv_handle_t hdl, 
                                        a_uint32_t *ac);

static a_status_t atd_cfg_dbgreq(adf_drv_handle_t hdl,
                                        acfg_athdbg_req_t *dbgreq);
static a_status_t atd_cfg_get_wpa_ie(adf_drv_handle_t hdl, a_uint8_t *buff, 
                               a_uint32_t buflen);
static a_status_t atd_cfg_set_appiebuf(adf_drv_handle_t hdl, 
                                        acfg_appie_t *addr);

static a_status_t atd_cfg_set_key(adf_drv_handle_t hdl, 
                                        acfg_key_t *addr);
static a_status_t atd_cfg_del_key(adf_drv_handle_t hdl, 
                                        acfg_delkey_t *addr);
static a_status_t atd_cfg_get_key(adf_drv_handle_t hdl, a_uint8_t *buff, 
                               a_uint32_t buflen);
static a_status_t atd_cfg_get_sta_stats(adf_drv_handle_t hdl, a_uint8_t *buff, 
                               a_uint32_t buflen);

static a_status_t atd_cfg_get_chan_info(adf_drv_handle_t hdl, 
                                        acfg_chan_info_t *chan_info);
static a_status_t atd_cfg_get_chan_list(adf_drv_handle_t hdl, 
                                        acfg_opaque_t *chan_list);
static a_status_t atd_cfg_get_mac_address(adf_drv_handle_t hdl, 
                                        acfg_macacl_t *mac_addr_list);
static a_status_t atd_cfg_get_p2p_param(adf_drv_handle_t hdl, 
                                        acfg_p2p_param_t *p2p_param);
static a_status_t atd_cfg_set_p2p_param(adf_drv_handle_t hdl, 
                                        acfg_p2p_param_t *p2p_param);
static a_status_t atd_cfg_acl_setmac(adf_drv_handle_t hdl, 
                                        acfg_macaddr_t *mac,
										a_uint8_t add);
static a_status_t atd_cfg_get_profile(adf_drv_handle_t hdl, 
										acfg_radio_vap_info_t *profile);
static a_status_t atd_cfg_phyerr(adf_drv_handle_t hdl, acfg_ath_diag_t *ath_diag);

static a_status_t atd_cfg_set_chwidthswitch(adf_drv_handle_t  hdl, acfg_set_chn_width_t* chnw);

static a_status_t atd_cfg_set_atf_ssid(adf_drv_handle_t  hdl,
        acfg_atf_ssid_val_t* atf_ssid);
static a_status_t atd_cfg_set_atf_sta(adf_drv_handle_t  hdl,
        acfg_atf_sta_val_t* atf_sta);

static a_status_t atd_cfg_set_country(adf_drv_handle_t  hdl, acfg_set_country_t *setcountry);

#define PROTO_WSUPP(name)                                         \
    static a_status_t                                             \
    atd_cfg_##name(adf_drv_handle_t hdl, acfg_wsupp_info_t *wsupp);

PROTO_WSUPP(wsupp_init);
PROTO_WSUPP(wsupp_fini);
PROTO_WSUPP(wsupp_if_add);
PROTO_WSUPP(wsupp_if_remove);
PROTO_WSUPP(wsupp_nw_create);
PROTO_WSUPP(wsupp_nw_delete);
PROTO_WSUPP(wsupp_nw_set);
PROTO_WSUPP(wsupp_nw_get);
PROTO_WSUPP(wsupp_nw_list);
PROTO_WSUPP(wsupp_wps_req);
PROTO_WSUPP(wsupp_set);

/** 
 * @brief WIFI Config API(s)
 */
atd_cfg_wifi_t     wifi_cfg = {
    .create_vap         = atd_cfg_create_vap,
    .set_radio_param    = atd_cfg_set_wifi_param,
    .get_radio_param    = atd_cfg_get_wifi_param,
    .set_reg            = atd_cfg_set_reg,
    .get_reg            = atd_cfg_get_reg,
    .tx99tool           = atd_cfg_tx99tool,
    .set_hwaddr         = atd_cfg_set_hwaddr,
    .get_ath_stats      = atd_cfg_get_ath_stats,
    .clr_ath_stats      = atd_cfg_clr_ath_stats,
    .get_profile        = atd_cfg_get_profile,
    .phyerr             = atd_cfg_phyerr,
    .set_country        = atd_cfg_set_country,
};


/** 
 * @brief VAP Config API(s)
 */
atd_cfg_vap_t      vap_cfg = {
    .set_ssid         = atd_cfg_set_ssid,
    .get_ssid         = atd_cfg_get_ssid,
    .set_testmode     = atd_cfg_set_testmode,
    .get_testmode     = atd_cfg_get_testmode,
    .get_rssi         = atd_cfg_get_rssi,
    .get_custdata     = atd_cfg_get_custdata,
    .delete_vap       = atd_cfg_delete_vap,
    .set_vap_param    = atd_cfg_set_vap_param,
    .get_vap_param    = atd_cfg_get_vap_param,
    .set_vap_vendor_param = atd_cfg_set_vap_vendor_param,
    .get_vap_vendor_param = atd_cfg_get_vap_vendor_param,
    .get_opmode       = atd_cfg_get_opmode,
    .set_chmode       = atd_cfg_set_chmode,
    .get_chmode       = atd_cfg_get_chmode,
    .get_wireless_name= atd_cfg_get_wireless_name,
    .set_ap           = atd_cfg_set_ap,
    .get_ap           = atd_cfg_get_ap,
    .set_rate         = atd_cfg_set_rate,
    .get_rate         = atd_cfg_get_rate,
    .set_powmgmt      = atd_cfg_set_powmgmt,
    .get_powmgmt      = atd_cfg_get_powmgmt,
    .set_scan         = atd_cfg_set_scan,
    .get_scan_results = atd_cfg_get_scan_results,
    .set_opmode       = atd_cfg_set_opmode,
    .get_range        = atd_cfg_get_range,
    .set_freq         = atd_cfg_set_freq,
    .get_freq         = atd_cfg_get_freq,
    .set_channel      = atd_cfg_set_chan,
    .get_channel      = atd_cfg_get_chan,
    .set_phymode      = atd_cfg_set_phymode,
    .get_phymode      = atd_cfg_get_phymode,
    .get_sta_info     = atd_cfg_get_stainfo, 
    .set_rts          = atd_cfg_set_rts,
    .get_rts          = atd_cfg_get_rts,
    .set_frag         = atd_cfg_set_frag,
    .get_frag         = atd_cfg_get_frag,
    .set_txpow        = atd_cfg_set_txpow,
    .get_txpow        = atd_cfg_get_txpow,
    .set_encode       = atd_cfg_set_encode,
    .get_encode       = atd_cfg_get_encode,
    .get_stats        = atd_cfg_get_stats,
    .doth_chsw        = atd_cfg_doth_chsw,
    .addmac           = atd_cfg_addmac,
    .delmac           = atd_cfg_delmac,
    .kickmac          = atd_cfg_kickmac,

    /* security cfgs */
    .wsupp_init       = atd_cfg_wsupp_init,
    .wsupp_fini       = atd_cfg_wsupp_fini,
    .wsupp_if_add     = atd_cfg_wsupp_if_add,
    .wsupp_if_remove  = atd_cfg_wsupp_if_remove,
    .wsupp_nw_create  = atd_cfg_wsupp_nw_create,
    .wsupp_nw_delete  = atd_cfg_wsupp_nw_delete,
    .wsupp_nw_set     = atd_cfg_wsupp_nw_set,
    .wsupp_nw_get     = atd_cfg_wsupp_nw_get,
    .wsupp_nw_list    = atd_cfg_wsupp_nw_list,
    .wsupp_wps_req    = atd_cfg_wsupp_wps_req,
    .wsupp_set        = atd_cfg_wsupp_set,
	
	/* wmm cfgs */
	.set_wmmparams    = atd_cfg_set_vap_wmmparams,
	.get_wmmparams    = atd_cfg_get_vap_wmmparams,

    /* nawds config */
    .config_nawds     = atd_cfg_nawds_config,

    .set_mlme         = atd_cfg_set_mlme, 
    .set_optie        = atd_cfg_set_optie,
    .set_filterframe  = atd_cfg_set_filterframe, 
    .set_acparams     = atd_cfg_set_acparams,
    .get_wpa_ie       = atd_cfg_get_wpa_ie, 
    .set_appiebuf     = atd_cfg_set_appiebuf,
    .set_key          = atd_cfg_set_key, 
    .del_key          = atd_cfg_del_key, 
    .get_key          = atd_cfg_get_key, 
    .get_sta_stats    = atd_cfg_get_sta_stats, 
    .get_chan_info    = atd_cfg_get_chan_info, 
    .get_chan_list    = atd_cfg_get_chan_list, 
    .get_mac_address  = atd_cfg_get_mac_address, 
    .get_p2p_param    = atd_cfg_get_p2p_param, 
    .set_p2p_param    = atd_cfg_set_p2p_param, 
    .dbgreq           = atd_cfg_dbgreq,
    .send_mgmt        = atd_cfg_send_mgmt,
    .acl_setmac	      = atd_cfg_acl_setmac,
    .set_chn_widthswitch = atd_cfg_set_chwidthswitch,
    .set_atf_ssid     = atd_cfg_set_atf_ssid,
    .set_atf_sta      = atd_cfg_set_atf_sta,
};

void atd_convert_ad_ntoh(void *out, void *in, int ad_id)
{
    if(ad_id == (0x8000 | SPECTRAL_GET_CHANINFO))
    {
        int i;
        SPECTRAL_CHAN_STATS_T *chan_info = (SPECTRAL_CHAN_STATS_T *)out;
        atd_SPECTRAL_CHAN_STATS_T *atd = (atd_SPECTRAL_CHAN_STATS_T *)in;

        for(i = 0; i < MAX_NUM_CHANNELS; i++)
        {
            chan_info[i].cycle_count = adf_os_ntohl(atd[i].cycle_count);
            chan_info[i].channel_load = adf_os_ntohl(atd[i].channel_load);
            chan_info[i].per = adf_os_ntohl(atd[i].per);
            chan_info[i].noisefloor = adf_os_ntohl(atd[i].noisefloor);
            chan_info[i].comp_usablity = adf_os_ntohs(atd[i].comp_usablity);
            chan_info[i].maxregpower = atd[i].maxregpower;
        }
    }
    else if (ad_id == (0x8000 | SPECTRAL_GET_CONFIG))
    {
        HAL_SPECTRAL_PARAM *gc = (HAL_SPECTRAL_PARAM *)out;
        atd_HAL_SPECTRAL_PARAM_t *atd = (atd_HAL_SPECTRAL_PARAM_t *)in;

        gc->ss_fft_period = adf_os_ntohs(atd->ss_fft_period);
        gc->ss_period = adf_os_ntohs(atd->ss_period);
        gc->ss_count = adf_os_ntohs(atd->ss_count);
        gc->ss_short_report = adf_os_ntohs(atd->ss_short_report);
        gc->radar_bin_thresh_sel = atd->radar_bin_thresh_sel;
        gc->ss_spectral_pri = adf_os_ntohs(atd->ss_spectral_pri);
        gc->ss_fft_size = adf_os_ntohs(atd->ss_fft_size);
        gc->ss_gc_ena = adf_os_ntohs(atd->ss_gc_ena);
        gc->ss_restart_ena = adf_os_ntohs(atd->ss_restart_ena);
        gc->ss_noise_floor_ref = adf_os_ntohs(atd->ss_noise_floor_ref);
        gc->ss_init_delay = adf_os_ntohs(atd->ss_init_delay);
        gc->ss_nb_tone_thr = adf_os_ntohs(atd->ss_nb_tone_thr);
        gc->ss_str_bin_thr = adf_os_ntohs(atd->ss_str_bin_thr);
        gc->ss_wb_rpt_mode = adf_os_ntohs(atd->ss_wb_rpt_mode);
        gc->ss_rssi_rpt_mode = adf_os_ntohs(atd->ss_rssi_rpt_mode);
        gc->ss_rssi_thr = adf_os_ntohs(atd->ss_rssi_thr);
        gc->ss_pwr_format = adf_os_ntohs(atd->ss_pwr_format);
        gc->ss_rpt_mode = adf_os_ntohs(atd->ss_rpt_mode);
        gc->ss_bin_scale = adf_os_ntohs(atd->ss_bin_scale);
        gc->ss_dBm_adj = adf_os_ntohs(atd->ss_dBm_adj);
        gc->ss_chn_mask = adf_os_ntohs(atd->ss_chn_mask);
        memcpy(gc->ss_nf_cal, atd->ss_nf_cal, AH_MAX_CHAINS*2);
        memcpy(gc->ss_nf_pwr, atd->ss_nf_pwr, AH_MAX_CHAINS*2);
        gc->ss_nf_temp_data = adf_os_ntohl(atd->ss_nf_temp_data);
    }
    else
        *((a_uint32_t *)out) = adf_os_ntohl((*((a_uint32_t *)in)));
}

#define vzalloc(size, ptr) \
    do {\
        ptr = vmalloc(size);\
        if(ptr)\
        memset(ptr, 0, size);\
    }while(0);

static a_status_t
atd_cfg_phyerr(adf_drv_handle_t hdl, acfg_ath_diag_t *ath_diag)
{
    atd_host_wifi_t    *wifi   = (atd_host_wifi_t *)hdl;
    a_status_t          status = A_STATUS_OK;
    adf_nbuf_t          nbuf;
    a_uint8_t          *resp = NULL;
    a_uint32_t          resplen = 0;
    atd_ath_diag_t     *phyerr;
    atd_hdr_t          *hdrp;
    a_uint32_t          len = sizeof(atd_hdr_t) +  sizeof(atd_ath_diag_t)
                        + ((ath_diag->ad_id & 0x4000) ? ath_diag->ad_in_size : 0);

    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("atd_phyerr"));

    if ((nbuf = _atd_wmi_msg_alloc(__wifi2wmi(wifi))) == NULL) {
        adf_os_print("Unable to allocate wmi buffer \n");
        status = A_STATUS_ENOMEM;
        goto fail ;
    }

    hdrp                  =  (atd_hdr_t *)adf_nbuf_put_tail(nbuf, len);
    hdrp->wifi_index      =  wifi->wifi_index;
    hdrp->vap_index       =  -1;

    phyerr = (atd_ath_diag_t *)(hdrp + 1);

    adf_os_mem_copy(phyerr->ad_name, ath_diag->ad_name, ACFG_MAX_IFNAME);
    phyerr->ad_id = adf_os_htons(ath_diag->ad_id);
    phyerr->ad_in_size = adf_os_htons(ath_diag->ad_in_size);
    phyerr->ad_out_size = adf_os_htonl(ath_diag->ad_out_size);

    if (ath_diag->ad_id & 0x4000) {
        /* Currently we support only bangradar, enable, disable and ignorecac.
         * Out of these only ignorecac has a in-param, of size 4 bytes. */
        if(ath_diag->ad_id == (0x4000 | SPECTRAL_SET_CONFIG))
        {
            HAL_SPECTRAL_PARAM *in = (HAL_SPECTRAL_PARAM *)ath_diag->ad_in_data;
            atd_HAL_SPECTRAL_PARAM_t *atd_in = (atd_HAL_SPECTRAL_PARAM_t *)(phyerr + 1);

            atd_in->ss_fft_period = adf_os_htons(in->ss_fft_period);
            atd_in->ss_period = adf_os_htons(in->ss_period);
            atd_in->ss_count = adf_os_htons(in->ss_count);
            atd_in->ss_short_report = adf_os_htons(in->ss_short_report);
            atd_in->radar_bin_thresh_sel = in->radar_bin_thresh_sel;
            atd_in->ss_spectral_pri = adf_os_htons(in->ss_spectral_pri);
            atd_in->ss_fft_size = adf_os_htons(in->ss_fft_size);
            atd_in->ss_gc_ena = adf_os_htons(in->ss_gc_ena);
            atd_in->ss_restart_ena = adf_os_htons(in->ss_restart_ena);
            atd_in->ss_noise_floor_ref = adf_os_htons(in->ss_noise_floor_ref);
            atd_in->ss_init_delay = adf_os_htons(in->ss_init_delay);
            atd_in->ss_nb_tone_thr = adf_os_htons(in->ss_nb_tone_thr);
            atd_in->ss_str_bin_thr = adf_os_htons(in->ss_str_bin_thr);
            atd_in->ss_wb_rpt_mode = adf_os_htons(in->ss_wb_rpt_mode);
            atd_in->ss_rssi_rpt_mode = adf_os_htons(in->ss_rssi_rpt_mode);
            atd_in->ss_rssi_thr = adf_os_htons(in->ss_rssi_thr);
            atd_in->ss_pwr_format = adf_os_htons(in->ss_pwr_format);
            atd_in->ss_rpt_mode = adf_os_htons(in->ss_rpt_mode);
            atd_in->ss_bin_scale = adf_os_htons(in->ss_bin_scale);
            atd_in->ss_dBm_adj = adf_os_htons(in->ss_dBm_adj);
            atd_in->ss_chn_mask = adf_os_htons(in->ss_chn_mask);
            memcpy(atd_in->ss_nf_cal, in->ss_nf_cal, AH_MAX_CHAINS*2);
            memcpy(atd_in->ss_nf_pwr, in->ss_nf_pwr, AH_MAX_CHAINS*2);
            atd_in->ss_nf_temp_data = adf_os_htonl(in->ss_nf_temp_data);
        }
        else if (ath_diag->ad_in_size == sizeof(a_uint32_t))
            *(a_uint32_t *)(phyerr + 1) = adf_os_htonl((*((a_uint32_t *)(ath_diag->ad_in_data))));
    }
    if(ath_diag->ad_out_size && ath_diag->ad_out_data)
    {
       vzalloc(ath_diag->ad_out_size, resp);
       if(resp == NULL){
           status = A_STATUS_ENOMEM;
           goto fail;
       }
       resplen = ath_diag->ad_out_size;
    }

    status = wmi_cmd_send(__wifi2wmi(wifi), WMI_CMD_PHYERR, nbuf,
                          resp, resplen);

    /* Got response, Its time todo ntoh conversion */
    if(ath_diag->ad_out_size && ath_diag->ad_out_data)
        atd_convert_ad_ntoh(ath_diag->ad_out_data, resp, ath_diag->ad_id);

fail:
    if(resp)
        vfree(resp);

    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("End stat %x ",status ));
    return status;
}


static a_status_t 
atd_cfg_tx99tool(adf_drv_handle_t hdl, acfg_tx99_t *tx99_wcmd)
{
    atd_host_wifi_t     *wifi  = (atd_host_wifi_t * ) hdl;
    a_status_t          status = A_STATUS_OK;
    adf_nbuf_t          nbuf;
    atd_tx99_t          *tx99_param;
    atd_hdr_t           *hdrp;
    a_uint32_t          len = sizeof(atd_hdr_t) +  sizeof(atd_tx99_t);

    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("atd_cfg_tx99tool"));

    if ((nbuf = _atd_wmi_msg_alloc(__wifi2wmi(wifi))) == NULL) {
        adf_os_print("Unable to allocate wmi buffer \n");
        status = A_STATUS_ENOMEM;
        goto fail ;
    }
    
    hdrp                  =  (atd_hdr_t *)adf_nbuf_put_tail(nbuf, len);
    hdrp->wifi_index      =   wifi->wifi_index;
    hdrp->vap_index       =   -1;

    tx99_param = (atd_tx99_t *)(hdrp + 1);

    memcpy(tx99_param->if_name, tx99_wcmd->if_name, ACFG_MAX_IFNAME);
    tx99_param->type = adf_os_htonl(tx99_wcmd->type);
    tx99_param->data.freq = adf_os_htonl(tx99_wcmd->data.freq);
    tx99_param->data.htmode = adf_os_htonl(tx99_wcmd->data.htmode);
    tx99_param->data.htext = adf_os_htonl(tx99_wcmd->data.htext);
    tx99_param->data.rate = adf_os_htonl(tx99_wcmd->data.rate);
    tx99_param->data.rc = adf_os_htonl(tx99_wcmd->data.rc);
    tx99_param->data.power = adf_os_htonl(tx99_wcmd->data.power);
    tx99_param->data.txmode = adf_os_htonl(tx99_wcmd->data.txmode);
    tx99_param->data.chanmask = adf_os_htonl(tx99_wcmd->data.chanmask);
    tx99_param->data.type = adf_os_htonl(tx99_wcmd->data.type);

    status = wmi_cmd_send(__wifi2wmi(wifi), WMI_CMD_TX99TOOL, nbuf,
                          NULL, 0);

fail:
    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("End stat %x ",status ));
    return status;
}

a_status_t 
atd_cfg_create_vap(adf_drv_handle_t       hdl,
                   a_uint8_t              icp_name[ACFG_MAX_IFNAME],
                   acfg_opmode_t          icp_opmode,  
                   a_int32_t              icp_vapid,  
                   acfg_vapinfo_flags_t   icp_vapflags)
{
    atd_host_wifi_t         *sc  = (atd_host_wifi_t * ) hdl;
    adf_net_dev_info_t       info  = {{0}};
    adf_dev_sw_t             dev_switch = {0};
    atd_host_vap_t          *vap = NULL;
    atd_vap_create_cmd_t    *cmd ;
    atd_vap_create_rsp_t     resp ;
    wmi_handle_t             wmih =  __wifi2wmi(sc);
    wmi_handle_t             htch =  __wifi2htc(sc);
    a_uint32_t               len, unit, error,  status  = A_STATUS_OK;
    adf_nbuf_t               nbuf;
    
    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG),
              ("icp_name %s icp_opmode, %x icp_vapflags %x", 
               icp_name, icp_opmode, icp_vapflags));

    len = sizeof(atd_vap_create_cmd_t);

    if ((nbuf = _atd_wmi_msg_alloc(wmih)) == NULL) {
        atd_trace(ATD_DEBUG_CFG, ("Unable to allocate wmi buffer \n"));
        goto fail_unit ;
    }

    cmd =  (atd_vap_create_cmd_t *)adf_nbuf_put_tail(nbuf, len);

    error = adf_net_ifc_name2unit(icp_name, &unit);
    if (error)
        goto fail_unit ;

    /* Allocation is tricky here, so let's give a few explanation.
     * We are basically trying to handle two cases:
     * - if the number isn't specified by the user, we have to allocate one,
     *   in which case we need to make sure it's not already been allocated
     *   already. User says "ath" and we create "athN" with N being a new unit#
     * - if the number is specified, we just need to make sure it's not been
     *   allocated already, which we check using dev_get_by_name()
     */
    if (unit == -1)
    {
        unit = adf_net_new_wlanunit();
        if (unit == -1)
        {
            goto fail_unit ;
        }
        adf_os_snprint(cmd->icp_name, sizeof(cmd->icp_name), "%s%d", icp_name, unit);
        info.unit = unit;
    }
    else
    {
        int dev_exist = adf_net_dev_exist_by_name(icp_name);
        if (dev_exist)
            goto fail_unit;

        unit = adf_net_new_wlanunit();
        info.unit = unit;
        adf_os_snprint(cmd->icp_name, sizeof(cmd->icp_name), "%s", icp_name);
    }

//    adf_os_str_ncopy(cmd->icp_name, icp_name, ACFG_MAX_IFNAME);
    cmd->icp_opmode  = adf_os_htons(icp_opmode );
    cmd->icp_flags   = adf_os_htons(icp_vapflags);
    cmd->icp_vapid   = adf_os_htonl(icp_vapid);
    cmd->wifi_index  = sc->wifi_index;

    status  = wmi_cmd_send(wmih, WMI_CMD_VAP_CREATE, nbuf, 
                          (a_uint8_t *)&resp, sizeof(atd_vap_create_rsp_t));

    atd_trace(ATD_DEBUG_CFG,("WMI Status %x ",status));
    if (status != A_STATUS_OK) {
        atd_trace(ATD_DEBUG_CFG, (" wmi cmd failure stat = %x\n", status));
        goto fail;
    }
    

    /*
     * successful. Create local vap
     */
    if ((vap = adf_os_mem_alloc(NULL, sizeof(atd_host_vap_t))) == NULL) {
        atd_trace(ATD_DEBUG_CFG, ("Error in allocating vap \n"));
        adf_os_assert(0);
    }

    adf_os_mem_zero(vap, sizeof(atd_host_vap_t));
    vap->wifisc      = sc;
    vap->vap_index   = resp.vap_index ;
    vap->vap_opmode  = icp_opmode;

    /* Protect the VAP Array */
    atd_spin_lock_bh(sc->dsc);      /* Lock */

    adf_os_assert(sc->vap_list[vap->vap_index] == NULL);
    
    sc->vap_list[vap->vap_index] = vap;

    atd_spin_unlock_bh(sc->dsc);    /* Unlock */

    atd_trace(ATD_DEBUG_LEVEL3,("vap name %s rsp index %d  \
                mac %x:%x:%x:%x:%x:%x ",\
                resp.name,\
                resp.vap_index,\
                resp.vap_bssid[0],  \
                resp.vap_bssid[1],  \
                resp.vap_bssid[2],  \
                resp.vap_bssid[3],  \
                resp.vap_bssid[4],  \
                resp.vap_bssid[5]   \
                ));
    atd_trace(ATD_DEBUG_FUNCTRACE,("vapname %s len %d \n", \
                resp.name, adf_os_str_len(resp.name)));

    /* save vap name returned by target */
    adf_os_str_ncopy((char *)icp_name, resp.name,  adf_os_str_len(resp.name));
    icp_name[ adf_os_str_len(resp.name) ] = '\0' ;

    adf_os_str_ncopy(info.if_name, resp.name, adf_os_str_len(resp.name));
    adf_os_mem_copy(info.dev_addr, &resp.vap_bssid[0], ACFG_MACADDR_LEN);

    info.header_len = sizeof(atd_tx_hdr_t) + 
                      htc_get_reserveheadroom(htch);

    
    dev_switch.drv_ioctl       = atd_vap_ioctl;
    dev_switch.drv_tx_timeout  = atd_vap_tx_timeout ;
    dev_switch.drv_open        = atd_vap_open;
    dev_switch.drv_close       = atd_vap_close;
    dev_switch.drv_tx          = atd_host_tx;
    dev_switch.drv_cmd         = atd_vap_cmd;

    vap->nethandle = adf_net_create_vapdev(vap, &dev_switch, &info, &vap_cfg);

    if(vap->nethandle == ADF_NET_NULL ) {
        atd_trace(ATD_DEBUG_CFG, ("Failure during vap_attach \n"));
        status = A_STATUS_EINVAL;
        goto fail;
    }
    
    atd_trace(ATD_DEBUG_FUNCTRACE,("VAP created  _adf_net_ifname %s \n",
                 adf_net_ifname(vap->nethandle)));

    return status;

fail :
/* Free the allocated bitmap for vap index in adf */
    adf_net_delete_wlanunit(unit);
fail_unit :

    if(vap) adf_os_mem_free(vap);

    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("End stat %x ",status ));
    return status;
}



/** 
 * @brief utility function for most wmi commands that use the common header
 * 
 * @param n
 * @param pldlen
 * @param w
 * @param v
 * 
 * @return 
 */
static inline a_uint8_t*
atd_prep_cfg_hdr(adf_nbuf_t n, a_uint32_t pldlen, atd_host_wifi_t *w, 
                 atd_host_vap_t *v)
{
    atd_hdr_t  *hdrp;
    int         len = sizeof(atd_hdr_t) + pldlen;

    hdrp              =  (atd_hdr_t *)adf_nbuf_put_tail(n, len);
    if (hdrp == NULL)
        return NULL;
    hdrp->wifi_index  =   w->wifi_index;
    hdrp->vap_index   =   v->vap_index;

    return (a_uint8_t *)(hdrp + 1);
}

static a_status_t 
atd_cfg_delete_vap(adf_drv_handle_t  hdl )
{
    atd_host_vap_t    *vap    = (atd_host_vap_t * )hdl;
    atd_host_wifi_t *wifi   = vap->wifisc;
    a_status_t         status = A_STATUS_OK;
    adf_nbuf_t         nbuf;

    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("delete vap 0x%p ", 
               (vap->nethandle)));
    adf_net_stop_queue(vap->nethandle); 

    if ((nbuf = _atd_wmi_msg_alloc(__wifi2wmi(wifi))) == NULL) {
        atd_trace(ATD_DEBUG_CFG, ("Unable to allocate wmi buffer \n"));
        status = A_STATUS_ENOMEM;
        goto fail ;
    }

    atd_prep_cfg_hdr(nbuf, 0 , wifi, vap);

    /* Free the wlan unit index maintained at adf for VAP's */
    adf_net_free_wlanunit(vap->nethandle);

    /*Delete net device before sending delete vap cmd to target for ensuring
     vap deletion after net device deletion*/
    adf_net_delete_dev(vap->nethandle); 
    vap->nethandle = NULL;

    status = wmi_cmd_send(__wifi2wmi(wifi), WMI_CMD_VAP_DELETE , nbuf,
                          NULL , 0);

    if (status != A_STATUS_OK) {
        atd_trace(ATD_DEBUG_CFG, (" wmi_cmd_send Failed stat = %x\n", status));
        goto fail;
    }
    
    
fail:    

    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("End stat %x ",status ));
    return status;
}


a_status_t 
atd_cfg_set_ssid(adf_drv_handle_t hdl, acfg_ssid_t  *ssid)
{
    atd_host_vap_t     *vap    = (atd_host_vap_t * )hdl;
    atd_host_wifi_t *wifi   = vap->wifisc;
    atd_ssid_t         *pld;
    a_status_t          status = A_STATUS_OK;
    adf_nbuf_t          nbuf;

    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("ssid %s len %d ", \
                ssid->name,  ssid->len));

    /*Error Check*/
    if(ssid->len > (ACFG_MAX_SSID_LEN + 1)){
        atd_trace(ATD_DEBUG_CFG, (" Ssid Len too large \n"));
        status = A_STATUS_EINVAL;
        goto fail;
    }

    if ((nbuf = _atd_wmi_msg_alloc(__wifi2wmi(wifi))) == NULL) {
        atd_trace(ATD_DEBUG_CFG, ("Unable to allocate wmi buffer \n"));
        status = A_STATUS_ENOMEM;
        goto fail ;
    }

    pld       = (atd_ssid_t *)atd_prep_cfg_hdr(nbuf, sizeof(atd_ssid_t), 
                                                 wifi, vap);

    pld->len = adf_os_htonl(ssid->len);
    adf_os_mem_copy(pld->name, ssid->name, ssid->len);

    status = wmi_cmd_send(__wifi2wmi(wifi), WMI_CMD_SSID_SET, nbuf,
                          NULL, 0);

fail:
    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("End stat %x ",status ));
    return status;
}

a_status_t 
atd_cfg_get_ssid(adf_drv_handle_t hdl, acfg_ssid_t  *ssid)
{
    atd_host_vap_t     *vap    = (atd_host_vap_t * )hdl;
    atd_host_wifi_t *wifi   = vap->wifisc;
    a_status_t          status = A_STATUS_OK;
    adf_nbuf_t          nbuf;
    atd_ssid_t          ssid_resp = {0};

    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("ssid %s len %d ", \
                ssid->name, ssid->len));

    if ((nbuf = _atd_wmi_msg_alloc(__wifi2wmi(wifi))) == NULL) {
        atd_trace(ATD_DEBUG_CFG, ("Unable to allocate wmi buffer \n"));
        status = A_STATUS_ENOMEM;
        goto fail ;
    }

    atd_prep_cfg_hdr(nbuf, 0, wifi, vap);

    status = wmi_cmd_send(__wifi2wmi(wifi), WMI_CMD_SSID_GET, nbuf,
                          (a_uint8_t *)&ssid_resp, sizeof(atd_ssid_t));

    if (status != A_STATUS_OK) {
        atd_trace(ATD_DEBUG_CFG, (" wmi_cmd_send Failed stat = %x\n", status));
        status = A_STATUS_EINVAL;
        goto fail;
    }

    ssid->len = adf_os_ntohl(ssid_resp.len);
    adf_os_mem_copy(ssid->name, ssid_resp.name, (ACFG_MAX_SSID_LEN + 1));

    atd_trace(ATD_DEBUG_CFG, ("ssid %s len %d ",ssid->name,ssid->len));

fail:    

    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("End stat %x ",status ));
    return status;
}

a_status_t 
atd_cfg_set_testmode(adf_drv_handle_t hdl, acfg_testmode_t  *testmode)
{
    atd_host_vap_t     *vap    = (atd_host_vap_t * )hdl;
    atd_host_wifi_t *wifi   = vap->wifisc;
    atd_testmode_t         *pld;
    a_status_t          status = A_STATUS_OK;
    adf_nbuf_t          nbuf;

    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), \
              ("testmode operation %d ", testmode->operation));

    if ((nbuf = _atd_wmi_msg_alloc(__wifi2wmi(wifi))) == NULL) {
        atd_trace(ATD_DEBUG_CFG, ("Unable to allocate wmi buffer \n"));
        status = A_STATUS_ENOMEM;
        goto fail ;
    }

    pld       = (atd_testmode_t *)atd_prep_cfg_hdr(nbuf, sizeof(atd_testmode_t), 
                                                 wifi, vap);

    adf_os_mem_copy(pld->bssid, testmode->bssid, ACFG_MACADDR_LEN);
    pld->chan = adf_os_htonl(testmode->chan);
    pld->operation = adf_os_htons(testmode->operation) ;
    pld->antenna = testmode->antenna;
    pld->rx = testmode->rx;
    pld->rssi_combined = adf_os_htonl(testmode->rssi_combined) ;
    pld->rssi0 = adf_os_htonl(testmode->rssi0);
    pld->rssi1 = adf_os_htonl(testmode->rssi1);
    pld->rssi2 = adf_os_htonl(testmode->rssi2);

    status = wmi_cmd_send(__wifi2wmi(wifi), WMI_CMD_TESTMODE_SET, nbuf,
                          NULL, 0);

fail:
    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("End stat %x ",status ));
    return status;
}

a_status_t 
atd_cfg_get_testmode(adf_drv_handle_t hdl, acfg_testmode_t  *testmode)
{
    atd_host_vap_t     *vap    = (atd_host_vap_t * )hdl;
    atd_host_wifi_t *wifi   = vap->wifisc;
    atd_testmode_t         *pld, result;
    a_status_t          status = A_STATUS_OK;
    adf_nbuf_t          nbuf;

    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("atd_cfg_get_testmode"));

    if ((nbuf = _atd_wmi_msg_alloc(__wifi2wmi(wifi))) == NULL) {
        atd_trace(ATD_DEBUG_CFG, ("Unable to allocate wmi buffer \n"));
        status = A_STATUS_ENOMEM;
        goto fail ;
    }

    pld       = (atd_testmode_t *)atd_prep_cfg_hdr(nbuf, sizeof(atd_testmode_t), 
                                                 wifi, vap);

    adf_os_mem_copy(pld->bssid, testmode->bssid, ACFG_MACADDR_LEN);
    pld->chan = adf_os_htonl(testmode->chan);
    pld->operation = adf_os_htons(testmode->operation) ;
    pld->antenna = testmode->antenna;
    pld->rx = testmode->rx;
    pld->rssi_combined = adf_os_htonl(testmode->rssi_combined) ;
    pld->rssi0 = adf_os_htonl(testmode->rssi0);
    pld->rssi1 = adf_os_htonl(testmode->rssi1);
    pld->rssi2 = adf_os_htonl(testmode->rssi2);

    status = wmi_cmd_send(__wifi2wmi(wifi), WMI_CMD_TESTMODE_GET, nbuf,
                          (a_uint8_t *)&result, sizeof(atd_testmode_t));

    if (status != A_STATUS_OK) {
        atd_trace(ATD_DEBUG_CFG, (" wmi_cmd_send Failed stat = %x\n", status));
        status = A_STATUS_EINVAL;
        goto fail;
    }

    adf_os_mem_copy(testmode->bssid, result.bssid, ACFG_MACADDR_LEN);
    testmode->chan = adf_os_ntohl(result.chan);
    testmode->operation = adf_os_ntohs(result.operation) ;
    testmode->antenna = result.antenna;
    testmode->rx = result.rx;
    testmode->rssi_combined = adf_os_ntohl(result.rssi_combined) ;
    testmode->rssi0 = adf_os_ntohl(result.rssi0);
    testmode->rssi1 = adf_os_ntohl(result.rssi1);
    testmode->rssi2 = adf_os_ntohl(result.rssi2);

fail:    

    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("End stat %x ",status ));
    return status;
}

a_status_t 
atd_cfg_get_rssi(adf_drv_handle_t hdl, acfg_rssi_t  *rssi)
{
    atd_host_vap_t     *vap    = (atd_host_vap_t * )hdl;
    atd_host_wifi_t *wifi   = vap->wifisc;
    a_status_t          status = A_STATUS_OK;
    adf_nbuf_t          nbuf;

    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), (" "));

    if ((nbuf = _atd_wmi_msg_alloc(__wifi2wmi(wifi))) == NULL) {
        atd_trace(ATD_DEBUG_CFG, ("Unable to allocate wmi buffer \n"));
        status = A_STATUS_ENOMEM;
        goto fail ;
    }

    atd_prep_cfg_hdr(nbuf, 0, wifi, vap);

    status = wmi_cmd_send(__wifi2wmi(wifi), WMI_CMD_RSSI_GET, nbuf,
                          (a_uint8_t *)rssi, sizeof(atd_rssi_t));

    if (status != A_STATUS_OK) {
        atd_trace(ATD_DEBUG_CFG, (" wmi_cmd_send Failed stat = %x\n", status));
        status = A_STATUS_EINVAL;
        goto fail;
    }

fail:    

    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("End stat %x ",status ));
    return status;
}

a_status_t 
atd_cfg_get_custdata(adf_drv_handle_t hdl, acfg_custdata_t  *custdata)
{
    atd_host_vap_t     *vap    = (atd_host_vap_t * )hdl;
    atd_host_wifi_t *wifi   = vap->wifisc;
    a_status_t          status = A_STATUS_OK;
    adf_nbuf_t          nbuf;

    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), (" "));

    if ((nbuf = _atd_wmi_msg_alloc(__wifi2wmi(wifi))) == NULL) {
        atd_trace(ATD_DEBUG_CFG, ("Unable to allocate wmi buffer \n"));
        status = A_STATUS_ENOMEM;
        goto fail ;
    }

    atd_prep_cfg_hdr(nbuf, 0, wifi, vap);

    status = wmi_cmd_send(__wifi2wmi(wifi), WMI_CMD_CUSTDATA_GET, nbuf,
                          (a_uint8_t *)custdata, sizeof(atd_custdata_t));

    if (status != A_STATUS_OK) {
        atd_trace(ATD_DEBUG_CFG, (" wmi_cmd_send Failed stat = %x\n", status));
        status = A_STATUS_EINVAL;
        goto fail;
    }

fail:    

    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("End stat %x ",status ));
    return status;
}

static a_status_t 
atd_cfg_set_vap_param(adf_drv_handle_t hdl, a_uint32_t param, a_uint32_t value)
{
    atd_host_vap_t         *vap    = (atd_host_vap_t * )hdl;
    atd_host_wifi_t     *wifi   = vap->wifisc;
    a_status_t         status = A_STATUS_OK;
    adf_nbuf_t         nbuf;
    atd_param_t       *vap_param;


    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("param  %d value %d ",\
                param, value ));

    if ((nbuf = _atd_wmi_msg_alloc(__wifi2wmi(wifi))) == NULL) {
        atd_trace(ATD_DEBUG_CFG, ("Unable to allocate wmi buffer \n"));
        status = A_STATUS_ENOMEM;
        goto fail ;
    }

    vap_param     = (atd_param_t *)atd_prep_cfg_hdr(nbuf, sizeof(atd_param_t), 
                                                 wifi, vap);
    vap_param->param     =   adf_os_htonl(param);
    vap_param->val       =   adf_os_htonl(value);


    status = wmi_cmd_send(__wifi2wmi(wifi), WMI_CMD_VAP_PARAM_SET, nbuf,
                          NULL, 0);

fail:

    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("End stat %x ",status ));
    return status;
}

static a_status_t 
atd_cfg_get_vap_param(adf_drv_handle_t hdl, a_uint32_t param, a_uint32_t *value)
{
    atd_host_vap_t    *vap    = (atd_host_vap_t * )hdl;
    atd_host_wifi_t     *wifi   = vap->wifisc;
    a_status_t         status = A_STATUS_OK;
    adf_nbuf_t         nbuf;
    atd_param_t       *vap_param;
    atd_param_t        paramresult;


    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("param  %d ",param ));

    if ((nbuf = _atd_wmi_msg_alloc(__wifi2wmi(wifi))) == NULL) {
        atd_trace(ATD_DEBUG_CFG, ("Unable to allocate wmi buffer \n"));
        status = A_STATUS_ENOMEM;
        goto fail ;
    }

    vap_param     = (atd_param_t *)atd_prep_cfg_hdr(nbuf, sizeof(atd_param_t), 
                                                 wifi, vap);
    vap_param->param     =   adf_os_htonl(param);
    vap_param->val       =   0;


    status = wmi_cmd_send(__wifi2wmi(wifi), WMI_CMD_VAP_PARAM_GET, nbuf,
                          (a_uint8_t *)&paramresult, sizeof(atd_param_t));


    paramresult.param = adf_os_ntohl(paramresult.param );

    value[0] = adf_os_ntohl(paramresult.val);

    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("val   %d ",value[0] ));

fail:
    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("End stat %x ",status ));
    return status;
}

static 
a_status_t atd_cfg_set_vap_vendor_param(adf_drv_handle_t hdl, acfg_vendor_param_req_t *req)
{
    atd_host_vap_t         *vap    = (atd_host_vap_t * )hdl;
    atd_host_wifi_t     *wifi   = vap->wifisc;
    a_status_t         status = A_STATUS_OK;
    adf_nbuf_t         nbuf;
    atd_vendor_param_t *vendor_param;

    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("param  %d ",\
                req->param));

    if ((nbuf = _atd_wmi_msg_alloc(__wifi2wmi(wifi))) == NULL) {
        atd_trace(ATD_DEBUG_CFG, ("Unable to allocate wmi buffer \n"));
        status = A_STATUS_ENOMEM;
        goto fail ;
    }

    vendor_param     = (atd_vendor_param_t *)atd_prep_cfg_hdr(nbuf, sizeof(atd_vendor_param_t),
                                                 wifi, vap);
    vendor_param->param     =   adf_os_htonl(req->param);
    vendor_param->type      =   adf_os_htonl(req->type);

    if(req->type == ACFG_TYPE_INT)
	 *(a_uint32_t *)&vendor_param->data = adf_os_htonl((*(a_uint32_t *)&req->data));
    else
         memcpy(&vendor_param->data, &req->data, sizeof(acfg_vendor_param_data_t));

    status = wmi_cmd_send(__wifi2wmi(wifi), WMI_CMD_VAP_VENDOR_PARAM_SET, nbuf,
                          NULL, 0);

fail:

    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("End stat %x ",status ));
    return status;
}

static a_status_t
atd_cfg_get_vap_vendor_param(adf_drv_handle_t hdl, acfg_vendor_param_req_t *req)
{
    atd_host_vap_t    *vap    = (atd_host_vap_t * )hdl;
    atd_host_wifi_t     *wifi   = vap->wifisc;
    a_status_t         status = A_STATUS_OK;
    adf_nbuf_t         nbuf;
    atd_vendor_param_t       *vendor_param;
    atd_vendor_param_t        paramresult;


    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("param  %d ", req->param ));

    if ((nbuf = _atd_wmi_msg_alloc(__wifi2wmi(wifi))) == NULL) {
        atd_trace(ATD_DEBUG_CFG, ("Unable to allocate wmi buffer \n"));
        status = A_STATUS_ENOMEM;
        goto fail ;
    }

    vendor_param     = (atd_vendor_param_t *)atd_prep_cfg_hdr(nbuf, sizeof(atd_vendor_param_t),
                                                 wifi, vap);
    vendor_param->param     =   adf_os_htonl(req->param);
    memset(&vendor_param->data, 0, sizeof(acfg_vendor_param_data_t));

    status = wmi_cmd_send(__wifi2wmi(wifi), WMI_CMD_VAP_VENDOR_PARAM_GET, nbuf,
                          (a_uint8_t *)&paramresult, sizeof(atd_vendor_param_t));

    req->param = adf_os_ntohl(paramresult.param);
    req->type = adf_os_ntohl(paramresult.type);

    if(req->type == ACFG_TYPE_INT)
        *(a_uint32_t *)&req->data = adf_os_ntohl(*(a_uint32_t *)&paramresult.data);
    else
        memcpy(&req->data, &paramresult.data, sizeof(acfg_vendor_param_data_t));
    
fail:
    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("End stat %x ",status ));
    return status;
}

static a_status_t 
atd_cfg_set_wifi_param(adf_drv_handle_t hdl, a_uint32_t param, a_uint32_t value)
{
    atd_host_wifi_t     *wifi  = (atd_host_wifi_t * ) hdl;
    a_status_t         status = A_STATUS_OK;
    adf_nbuf_t         nbuf;
    atd_param_t       *wifi_param;
    atd_hdr_t         *hdrp;
    a_uint32_t         len = sizeof(atd_hdr_t) +  sizeof(atd_param_t);


    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("param  %d value %d " \
              ,param, value ));


    if ((nbuf = _atd_wmi_msg_alloc(__wifi2wmi(wifi))) == NULL) {
        atd_trace(ATD_DEBUG_CFG, ("Unable to allocate wmi buffer \n"));
        status = A_STATUS_ENOMEM;
        goto fail ;
    }


    hdrp                  =  (atd_hdr_t *)adf_nbuf_put_tail(nbuf, len);
    hdrp->wifi_index      =   wifi->wifi_index;
    hdrp->vap_index       =   -1;

    wifi_param            =  (atd_param_t *)(hdrp + 1);

    wifi_param->param     =   adf_os_htonl(param);
    wifi_param->val       =   adf_os_htonl(value);


    status = wmi_cmd_send(__wifi2wmi(wifi), WMI_CMD_WIFI_PARAM_SET, nbuf,
                          NULL, 0);

fail:
    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("End stat %x ",status ));
    return status;
}

static a_status_t 
atd_cfg_get_wifi_param(adf_drv_handle_t hdl, a_uint32_t param, 
                       a_uint32_t *value)
{
    atd_host_wifi_t     *wifi  = (atd_host_wifi_t * ) hdl;
    a_status_t         status = A_STATUS_OK;
    adf_nbuf_t         nbuf;
    atd_param_t       *wifi_param;
    atd_param_t        paramresult;
    atd_hdr_t         *hdrp;
    a_uint32_t         len = sizeof(atd_hdr_t) +  sizeof(atd_param_t);



    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("param  %d ",param ));

    if ((nbuf = _atd_wmi_msg_alloc(__wifi2wmi(wifi))) == NULL) {
        atd_trace(ATD_DEBUG_CFG, ("Unable to allocate wmi buffer \n"));
        status = A_STATUS_ENOMEM;
        goto fail ;
    }

    hdrp                  =  (atd_hdr_t *)adf_nbuf_put_tail(nbuf, len);
    hdrp->wifi_index      =   wifi->wifi_index;
    hdrp->vap_index       =   -1;

    wifi_param            =  (atd_param_t *)(hdrp + 1);

    wifi_param->param     =   adf_os_htonl(param);
    wifi_param->val       =   0;


    status = wmi_cmd_send(__wifi2wmi(wifi), WMI_CMD_WIFI_PARAM_GET, nbuf,
                          (a_uint8_t *)&paramresult, sizeof(atd_param_t));

    paramresult.param = adf_os_ntohl(paramresult.param );

    value[0] = adf_os_ntohl(paramresult.val);

    atd_trace(ATD_DEBUG_CFG, ("val   %d ", value[0]));

fail:
    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("End stat %x ",status ));
   return status;
}

a_status_t 
atd_cfg_set_opmode(adf_drv_handle_t hdl, acfg_opmode_t mode)
{
    atd_host_vap_t    *vap    = (atd_host_vap_t * )hdl;
    atd_host_wifi_t   *wifi   = vap->wifisc;
    a_status_t         status = A_STATUS_OK;
    a_int32_t          resp;
    adf_nbuf_t         nbuf;
    acfg_opmode_t     *pmode;

    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), \
              (" opmode %d ", (a_uint32_t)mode));
	
    if ((nbuf = _atd_wmi_msg_alloc(__wifi2wmi(wifi))) == NULL) {
        atd_trace(ATD_DEBUG_CFG, ("Unable to allocate wmi buffer \n"));
        status = A_STATUS_ENOMEM;
        goto fail ;
    }

    pmode = (acfg_opmode_t *)atd_prep_cfg_hdr(nbuf, sizeof(acfg_opmode_t), 
                                                 wifi, vap);

    *(a_uint32_t *)pmode = adf_os_htonl(mode);
	
    status = wmi_cmd_send(__wifi2wmi(wifi), WMI_CMD_OPMODE_SET, nbuf,
                          (a_uint8_t *)&resp, sizeof(a_int32_t));

    if (status != A_STATUS_OK) {
        atd_trace(ATD_DEBUG_CFG, (" wmi_cmd_send Failed stat = %x\n", status));
        status = A_STATUS_EINVAL;
        goto fail;
    }

    resp = adf_os_ntohl(resp);
    if(resp == -1)
        status = A_STATUS_EINVAL;

fail:
    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("End stat %x ",status ));
    return status;
}

a_status_t 
atd_cfg_get_opmode(adf_drv_handle_t  hdl, acfg_opmode_t  *mode)
{
    atd_host_vap_t    *vap    = (atd_host_vap_t * )hdl;
    atd_host_wifi_t     *wifi   = vap->wifisc;
    a_status_t         status = A_STATUS_OK;
    adf_nbuf_t         nbuf;

    atd_trace((ATD_DEBUG_FUNCTRACE|ATD_DEBUG_CFG), \
              (" opmode %d ", (a_uint32_t)mode));


    if ((nbuf = _atd_wmi_msg_alloc(__wifi2wmi(wifi))) == NULL) {
        atd_trace(ATD_DEBUG_CFG, ("Unable to allocate wmi buffer \n"));
        status = A_STATUS_ENOMEM;
        goto fail ;
    }

    atd_prep_cfg_hdr(nbuf, 0, wifi, vap);

    status = wmi_cmd_send(__wifi2wmi(wifi), WMI_CMD_OPMODE_GET, nbuf,
            (a_uint8_t *)mode, sizeof(acfg_opmode_t));

    mode[0] = adf_os_ntohl(mode[0]);



fail:
    atd_trace( (ATD_DEBUG_FUNCTRACE|ATD_DEBUG_CFG), (" End ... \n"));

    return status;
}


a_status_t 
atd_cfg_set_chmode(adf_drv_handle_t hdl, acfg_chmode_t  *mode)
{
    atd_host_vap_t     *vap    = (atd_host_vap_t * )hdl;
    atd_host_wifi_t    *wifi   = vap->wifisc;
    atd_chmode_t       *pmode;
    a_status_t          status = A_STATUS_OK;
    adf_nbuf_t          nbuf;

    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("chmode %s len %d ", \
                mode->mode,  mode->len));

    /*Error Check*/
    if(mode->len > ACFG_MAX_CHMODE_LEN){
        atd_trace(ATD_DEBUG_CFG, (" Chmode Len too large \n"));
        status = A_STATUS_EINVAL;
        goto fail;
    }
    if( mode->mode[mode->len-1] != '\0'){
        atd_trace(ATD_DEBUG_CFG, ("Unterminated string ..."));
        status = A_STATUS_EINVAL;
        goto fail;
    }

    if ((nbuf = _atd_wmi_msg_alloc(__wifi2wmi(wifi))) == NULL) {
        atd_trace(ATD_DEBUG_CFG, ("Unable to allocate wmi buffer \n"));
        status = A_STATUS_ENOMEM;
        goto fail ;
    }

    pmode       = (atd_chmode_t *)atd_prep_cfg_hdr(nbuf, sizeof(atd_chmode_t), 
                                                 wifi, vap);

    pmode->len  = adf_os_htonl(mode->len);
    adf_os_mem_copy(pmode->mode, mode->mode, mode->len);

    status = wmi_cmd_send(__wifi2wmi(wifi), WMI_CMD_CHMODE_SET, nbuf,
                          NULL, 0);


fail:
    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("End stat %x ",status ));
    return status;
}


a_status_t 
atd_cfg_get_chmode(adf_drv_handle_t hdl, acfg_chmode_t *mode)
{
    atd_host_vap_t     *vap    = (atd_host_vap_t * )hdl;
    atd_host_wifi_t    *wifi   = vap->wifisc;
    a_status_t          status = A_STATUS_OK;
    adf_nbuf_t          nbuf;

    atd_chmode_t    atd_mode = {0};      

    if ((nbuf = _atd_wmi_msg_alloc(__wifi2wmi(wifi))) == NULL) {
        atd_trace(ATD_DEBUG_CFG, ("Unable to allocate wmi buffer \n"));
        status = A_STATUS_ENOMEM;
        goto fail ;
    }

    atd_prep_cfg_hdr(nbuf, 0, wifi, vap);

    status = wmi_cmd_send(__wifi2wmi(wifi), WMI_CMD_CHMODE_GET, nbuf,
                            (a_uint8_t *)&atd_mode, sizeof(atd_chmode_t));

    mode->len = adf_os_ntohl(atd_mode.len);
    adf_os_mem_copy(mode->mode, atd_mode.mode, mode->len);

fail:
    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("End stat %x ",status ));
    return status;
}



static a_status_t 
atd_cfg_get_wireless_name(adf_drv_handle_t hdl, u_int8_t * name, 
                           a_uint32_t maxlen)
                          
{
    atd_host_vap_t    *vap    = (atd_host_vap_t * )hdl;
    atd_host_wifi_t     *wifi   = vap->wifisc;
    a_status_t         status = A_STATUS_OK;
    adf_nbuf_t         nbuf;

    atd_trace((ATD_DEBUG_FUNCTRACE|ATD_DEBUG_CFG), \
              ("Start maxlen %d \n",maxlen));

    if ((nbuf = _atd_wmi_msg_alloc(__wifi2wmi(wifi))) == NULL) {
        atd_trace(ATD_DEBUG_CFG, ("Unable to allocate wmi buffer \n"));
        status = A_STATUS_ENOMEM;
        goto fail ;
    }

    atd_prep_cfg_hdr(nbuf, 0, wifi, vap);


    status = wmi_cmd_send(__wifi2wmi(wifi), WMI_CMD_WIRELESS_NAME_GET, nbuf,
            (a_uint8_t *) name ,maxlen);

    
    atd_trace(ATD_DEBUG_CFG, (" RESULT name %s \n",name));

fail:    
    atd_trace( (ATD_DEBUG_FUNCTRACE|ATD_DEBUG_CFG), (" End ... \n"));
    return status;
}

a_status_t 
atd_cfg_set_ap(adf_drv_handle_t  hdl, acfg_macaddr_t *mac)
{
    atd_host_vap_t    *vap = (atd_host_vap_t *)hdl;
    atd_host_wifi_t   *wifi = vap->wifisc;
    a_status_t         status = A_STATUS_OK;
    adf_nbuf_t         nbuf;
    acfg_macaddr_t    *pmac;
    a_int32_t          resp;

    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("Start"));
	
    if ((nbuf = _atd_wmi_msg_alloc(__wifi2wmi(wifi))) == NULL) {
        atd_trace(ATD_DEBUG_CFG, ("Unable to allocate wmi buffer \n"));
        status = A_STATUS_ENOMEM;
        goto fail ;
    }

    pmac = (acfg_macaddr_t *)atd_prep_cfg_hdr(nbuf, sizeof(acfg_macaddr_t), 
                                              wifi, vap);

    memcpy(pmac->addr, mac->addr, ACFG_MACADDR_LEN);

    atd_trace(ATD_DEBUG_CFG, (" ap mac addr %02x:%02x:%02x:%02x:%02x:%02x", \
                pmac->addr[0], pmac->addr[1], pmac->addr[2], pmac->addr[3], \
                pmac->addr[4], pmac->addr[5]));
	
    status = wmi_cmd_send(__wifi2wmi(wifi), WMI_CMD_AP_SET, nbuf, 
            (a_uint8_t *)&resp, sizeof(a_int32_t));

    if (status != A_STATUS_OK) {
        atd_trace(ATD_DEBUG_CFG, (" wmi_cmd_send Failed stat = %x\n", status));
        status = A_STATUS_EINVAL;
        goto fail;
    }

    resp = adf_os_ntohl(resp);
    if(resp == -1)
        status = A_STATUS_EINVAL;

fail:
    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("End stat %x ",status ));
    return status;
}

static a_status_t 
atd_cfg_get_ap(adf_drv_handle_t hdl, acfg_macaddr_t *mac)
{
    atd_host_vap_t    *vap    = (atd_host_vap_t * )hdl;
    atd_host_wifi_t   *wifi   = vap->wifisc;
    a_status_t         status = A_STATUS_OK;
    adf_nbuf_t         nbuf;

    atd_trace( (ATD_DEBUG_FUNCTRACE|ATD_DEBUG_CFG), ("Start"));

    if ((nbuf = _atd_wmi_msg_alloc(__wifi2wmi(wifi))) == NULL) {
        atd_trace(ATD_DEBUG_CFG, ("Unable to allocate wmi buffer \n"));
        status = A_STATUS_ENOMEM;
        goto fail ;
    }

    atd_prep_cfg_hdr(nbuf, 0, wifi, vap);


    status = wmi_cmd_send(__wifi2wmi(wifi), WMI_CMD_AP_GET, nbuf,
            (a_uint8_t *)mac->addr , ACFG_MACADDR_LEN);

    
    atd_trace(ATD_DEBUG_CFG, (" ap mac addr %x:%x:%x:%x:%x:%x", \
                mac->addr[0], mac->addr[1], mac->addr[2], mac->addr[3], \
                mac->addr[4], mac->addr[5]));

fail:    
    atd_trace( (ATD_DEBUG_FUNCTRACE|ATD_DEBUG_CFG), (" End ... \n"));
    return status;
}

a_status_t 
atd_cfg_set_rate(adf_drv_handle_t hdl, acfg_rate_t *rate)
{
    atd_host_vap_t    *vap    = (atd_host_vap_t * )hdl;
    atd_host_wifi_t     *wifi   = vap->wifisc;
    a_status_t         status = A_STATUS_OK;
    a_int32_t          resp;
    adf_nbuf_t         nbuf;
    atd_rate_t        *prate;

    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("Start"));

    if ((nbuf = _atd_wmi_msg_alloc(__wifi2wmi(wifi))) == NULL) {
        atd_trace(ATD_DEBUG_CFG, ("Unable to allocate wmi buffer \n"));
        status = A_STATUS_ENOMEM;
        goto fail ;
    }

    prate = (atd_rate_t *)atd_prep_cfg_hdr(nbuf, sizeof(atd_rate_t), 
                                            wifi, vap);

    prate->fixed = rate->fixed;
    prate->value = adf_os_htonl(rate->value);

    atd_trace(ATD_DEBUG_CFG, ("SET value %d , fixed %d", prate->value, \
              prate->fixed));

    status = wmi_cmd_send(__wifi2wmi(wifi), WMI_CMD_RATE_SET, nbuf, 
            (a_uint8_t *)&resp, sizeof(a_int32_t));

    if (status != A_STATUS_OK) {
        atd_trace(ATD_DEBUG_CFG, (" wmi_cmd_send Failed stat = %x\n", status));
        status = A_STATUS_EINVAL;
    }

    resp = adf_os_ntohl(resp);
    if(resp == -1)
        status = A_STATUS_EINVAL;

fail:
    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("End stat %x ",status ));
    return status;
}

static a_status_t 
atd_cfg_get_rate(adf_drv_handle_t hdl, a_uint32_t *rate)
{
    atd_host_vap_t    *vap    = (atd_host_vap_t * )hdl;
    atd_host_wifi_t   *wifi   = vap->wifisc;
    a_status_t         status = A_STATUS_OK;
    adf_nbuf_t         nbuf;

    atd_trace((ATD_DEBUG_FUNCTRACE|ATD_DEBUG_CFG), ("Start"));

    if ((nbuf = _atd_wmi_msg_alloc(__wifi2wmi(wifi))) == NULL) {
        atd_trace(ATD_DEBUG_CFG, ("Unable to allocate wmi buffer \n"));
        status = A_STATUS_ENOMEM;
        goto fail ;
    }

    atd_prep_cfg_hdr(nbuf, 0, wifi, vap);

    status = wmi_cmd_send(__wifi2wmi(wifi), WMI_CMD_RATE_GET, nbuf,
            (a_uint8_t *)rate , sizeof(a_uint32_t ));

    if (status != A_STATUS_OK) {
        atd_trace(ATD_DEBUG_CFG, (" wmi_cmd_send Failed stat = %x\n", status));
        status =  A_STATUS_EINVAL;
    }
    rate[0] = adf_os_ntohl(rate[0]);
 
    atd_trace(ATD_DEBUG_CFG, (" RESULT rate %d",rate[0]));

fail:    
    atd_trace( (ATD_DEBUG_FUNCTRACE|ATD_DEBUG_CFG), (" End ... \n"));
    return status;
}

a_status_t 
atd_cfg_set_powmgmt(adf_drv_handle_t  hdl, acfg_powmgmt_t *pm)
{
    atd_host_vap_t *vap = (atd_host_vap_t *)hdl;
    atd_host_wifi_t *wifi = vap->wifisc;
    a_status_t status = A_STATUS_OK;
    adf_nbuf_t nbuf;
    atd_powmgmt_t *mgmt;
    a_int32_t resp;

    atd_trace( (ATD_DEBUG_FUNCTRACE|ATD_DEBUG_CFG), ("Start"));
	
    if ((nbuf = _atd_wmi_msg_alloc(__wifi2wmi(wifi))) == NULL) {
        atd_trace(ATD_DEBUG_CFG, ("Unable to allocate wmi buffer \n"));
        status = A_STATUS_ENOMEM;
        goto fail ;
    }

    mgmt = (atd_powmgmt_t *)atd_prep_cfg_hdr(nbuf, sizeof(atd_powmgmt_t), 
                                              wifi, vap);
	
    mgmt->disabled = pm->disabled;
    mgmt->flags = adf_os_htons(pm->flags);
    mgmt->val = adf_os_htonl(pm->val);

    status = wmi_cmd_send(__wifi2wmi(wifi), WMI_CMD_POWMGMT_SET, nbuf, 
            (a_uint8_t *)&resp, sizeof(a_int32_t));

    if (status != A_STATUS_OK) {
        atd_trace(ATD_DEBUG_CFG, (" wmi_cmd_send Failed stat = %x\n", status));
        status = A_STATUS_EINVAL;
        goto fail;
    }

    resp = adf_os_ntohl(resp);
    if(resp == -1)
        status = A_STATUS_EINVAL;

fail:
    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("End stat %x ",status ));
    return status;
}

static a_status_t 
atd_cfg_get_powmgmt(adf_drv_handle_t hdl, acfg_powmgmt_t *pm)
{
    atd_host_vap_t    *vap    = (atd_host_vap_t * )hdl;
    atd_host_wifi_t   *wifi   = vap->wifisc;
    a_status_t         status = A_STATUS_OK;
    adf_nbuf_t         nbuf;
    atd_powmgmt_t      powmgmt;

    atd_trace((ATD_DEBUG_FUNCTRACE|ATD_DEBUG_CFG), ("Start"));

    if ((nbuf = _atd_wmi_msg_alloc(__wifi2wmi(wifi))) == NULL) {
        atd_trace(ATD_DEBUG_CFG, ("Unable to allocate wmi buffer \n"));
        status = A_STATUS_ENOMEM;
        goto fail ;
    }

    atd_prep_cfg_hdr(nbuf, 0, wifi, vap);

    status = wmi_cmd_send(__wifi2wmi(wifi), WMI_CMD_POWMGMT_GET, nbuf,
            (a_uint8_t *)&powmgmt , sizeof(atd_powmgmt_t));

    if (status != A_STATUS_OK) {
        atd_trace(ATD_DEBUG_CFG, (" wmi_cmd_send Failed stat = %x\n", status));
        status =  A_STATUS_EINVAL;
        goto fail;
    }
	
    pm->val = adf_os_ntohl(powmgmt.val);
    pm->flags = adf_os_ntohs(powmgmt.flags);
    pm->disabled = powmgmt.disabled;
 
    atd_trace(ATD_DEBUG_CFG, \
             ("RESULT pm.val %d pm.flags 0x%04x pm.disabled %d", 
		     pm->val, pm->flags, pm->disabled));

fail:
    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("End stat %x ",status ));
    return status;
}

/** 
 * @brief Get ath stats from target 
 * 
 * @param hdl
 * @param scan
 * 
 * @return status
 */
a_status_t
atd_cfg_get_ath_stats(adf_drv_handle_t hdl, acfg_ath_stats_t *athstats_wcmd)
{
    atd_host_wifi_t     *wifi  = (atd_host_wifi_t *)hdl;
    a_status_t          status = A_STATUS_OK;
    adf_nbuf_t          nbuf;
    atd_ath_stats_t     *athstats_param;
    atd_hdr_t           *hdrp;
    a_uint32_t          len    = sizeof(atd_hdr_t) + sizeof(atd_ath_stats_t);
    a_uint32_t          resp_size;
    void                *resp  = NULL;

    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("atd_cfg_get_ath_stats"));

    if (athstats_wcmd->size < adf_os_max(sizeof(acfg_ath_stats_11n_t), sizeof(acfg_ath_stats_11ac_t))) {
        atd_trace(ATD_DEBUG_CFG, ("Buffer size too small\n"));
        status = A_STATUS_ENOSPC;
        goto fail;
    }

    resp_size = sizeof(atd_ath_stats_t);
    resp_size += adf_os_max(sizeof(atd_ath_stats_11n_t), sizeof(atd_ath_stats_11ac_t));
    resp = adf_os_mem_alloc(NULL, resp_size);
    if (resp == NULL) {
        atd_trace(ATD_DEBUG_CFG, ("Unable to allocate memory \n"));
        status = A_STATUS_ENOMEM;
        goto fail;
    }

    if ((nbuf = _atd_wmi_msg_alloc(__wifi2wmi(wifi))) == NULL) {
        atd_trace(ATD_DEBUG_CFG, ("Unable to allocate wmi buffer \n"));
        status = A_STATUS_ENOMEM;
        goto fail;
    }

    hdrp                  =   (atd_hdr_t *)adf_nbuf_put_tail(nbuf, len);
    hdrp->wifi_index      =   wifi->wifi_index;
    hdrp->vap_index       =   -1;

    athstats_param = (atd_ath_stats_t *)(hdrp + 1);
    athstats_param->size = adf_os_htonl(resp_size);
    athstats_param->offload_if = adf_os_htonl(athstats_wcmd->offload_if);

    status = wmi_cmd_send(__wifi2wmi(wifi), WMI_CMD_ATH_STATS_GET, nbuf,
                          resp, resp_size);
    if (status != A_STATUS_OK) {
        atd_trace(ATD_DEBUG_CFG, (" wmi_cmd_send Failed stat = %x\n", status));
        goto fail;
    }

    athstats_param = (atd_ath_stats_t *)resp;
    len = adf_os_ntohl(athstats_param->size);
    athstats_wcmd->offload_if = adf_os_ntohl(athstats_param->offload_if);

    if (athstats_wcmd->offload_if == 1) {
        a_uint32_t i;
        acfg_ath_stats_11ac_t *ac_stats = (acfg_ath_stats_11ac_t *)(athstats_wcmd->address);
        atd_ath_stats_11ac_t *stats = (atd_ath_stats_11ac_t *)((a_uint8_t *)resp + sizeof(atd_ath_stats_t));
        athstats_wcmd->size = sizeof(acfg_ath_stats_11ac_t);

        ac_stats->txrx_stats_level = adf_os_ntohl(stats->txrx_stats_level);
        ac_stats->txrx_stats.tx.from_stack.pkts = adf_os_be64_to_cpu(stats->txrx_stats.tx.from_stack.pkts);
        ac_stats->txrx_stats.tx.from_stack.bytes = adf_os_be64_to_cpu(stats->txrx_stats.tx.from_stack.bytes);
        ac_stats->txrx_stats.tx.delivered.pkts = adf_os_be64_to_cpu(stats->txrx_stats.tx.delivered.pkts);
        ac_stats->txrx_stats.tx.delivered.bytes = adf_os_be64_to_cpu(stats->txrx_stats.tx.delivered.bytes);
        ac_stats->txrx_stats.tx.dropped.host_reject.pkts = adf_os_be64_to_cpu(stats->txrx_stats.tx.dropped.host_reject.pkts);
        ac_stats->txrx_stats.tx.dropped.host_reject.bytes = adf_os_be64_to_cpu(stats->txrx_stats.tx.dropped.host_reject.bytes);
        ac_stats->txrx_stats.tx.dropped.download_fail.pkts = adf_os_be64_to_cpu(stats->txrx_stats.tx.dropped.download_fail.pkts);
        ac_stats->txrx_stats.tx.dropped.download_fail.bytes = adf_os_be64_to_cpu(stats->txrx_stats.tx.dropped.download_fail.bytes);
        ac_stats->txrx_stats.tx.dropped.target_discard.pkts = adf_os_be64_to_cpu(stats->txrx_stats.tx.dropped.target_discard.pkts);
        ac_stats->txrx_stats.tx.dropped.target_discard.bytes = adf_os_be64_to_cpu(stats->txrx_stats.tx.dropped.target_discard.bytes);
        ac_stats->txrx_stats.tx.dropped.no_ack.pkts = adf_os_be64_to_cpu(stats->txrx_stats.tx.dropped.no_ack.pkts);
        ac_stats->txrx_stats.tx.dropped.no_ack.bytes = adf_os_be64_to_cpu(stats->txrx_stats.tx.dropped.no_ack.bytes);
        ac_stats->txrx_stats.tx.desc_in_use = adf_os_ntohl(stats->txrx_stats.tx.desc_in_use);
        ac_stats->txrx_stats.tx.desc_alloc_fails = adf_os_ntohl(stats->txrx_stats.tx.desc_alloc_fails);
        ac_stats->txrx_stats.tx.ce_ring_full = adf_os_ntohl(stats->txrx_stats.tx.ce_ring_full);
        ac_stats->txrx_stats.tx.dma_map_error = adf_os_ntohl(stats->txrx_stats.tx.dma_map_error);
        ac_stats->txrx_stats.tx.mgmt.pkts = adf_os_be64_to_cpu(stats->txrx_stats.tx.mgmt.pkts);
        ac_stats->txrx_stats.tx.mgmt.bytes = adf_os_be64_to_cpu(stats->txrx_stats.tx.mgmt.bytes);
        ac_stats->txrx_stats.rx.delivered.pkts = adf_os_be64_to_cpu(stats->txrx_stats.rx.delivered.pkts);
        ac_stats->txrx_stats.rx.delivered.bytes = adf_os_be64_to_cpu(stats->txrx_stats.rx.delivered.bytes);
        ac_stats->txrx_stats.rx.forwarded.pkts = adf_os_be64_to_cpu(stats->txrx_stats.rx.forwarded.pkts);
        ac_stats->txrx_stats.rx.forwarded.bytes = adf_os_be64_to_cpu(stats->txrx_stats.rx.forwarded.bytes);
        ac_stats->stats.tx.comp_queued = adf_os_ntohl(stats->stats.tx.comp_queued);
        ac_stats->stats.tx.comp_delivered = adf_os_ntohl(stats->stats.tx.comp_delivered);
        ac_stats->stats.tx.msdu_enqued = adf_os_ntohl(stats->stats.tx.msdu_enqued);
        ac_stats->stats.tx.mpdu_enqued = adf_os_ntohl(stats->stats.tx.mpdu_enqued);
        ac_stats->stats.tx.wmm_drop = adf_os_ntohl(stats->stats.tx.wmm_drop);
        ac_stats->stats.tx.local_enqued = adf_os_ntohl(stats->stats.tx.local_enqued);
        ac_stats->stats.tx.local_freed = adf_os_ntohl(stats->stats.tx.local_freed);
        ac_stats->stats.tx.hw_queued = adf_os_ntohl(stats->stats.tx.hw_queued);
        ac_stats->stats.tx.hw_reaped = adf_os_ntohl(stats->stats.tx.hw_reaped);
        ac_stats->stats.tx.underrun = adf_os_ntohl(stats->stats.tx.underrun);
        ac_stats->stats.tx.tx_abort = adf_os_ntohl(stats->stats.tx.tx_abort);
        ac_stats->stats.tx.mpdus_requed = adf_os_ntohl(stats->stats.tx.mpdus_requed);
        ac_stats->stats.tx.tx_xretry = adf_os_ntohl(stats->stats.tx.tx_ko);
        ac_stats->stats.tx.data_rc = adf_os_ntohl(stats->stats.tx.data_rc);
        ac_stats->stats.tx.self_triggers = adf_os_ntohl(stats->stats.tx.self_triggers);
        ac_stats->stats.tx.sw_retry_failure = adf_os_ntohl(stats->stats.tx.sw_retry_failure);
        ac_stats->stats.tx.illgl_rate_phy_err = adf_os_ntohl(stats->stats.tx.illgl_rate_phy_err);
        ac_stats->stats.tx.pdev_cont_xretry = adf_os_ntohl(stats->stats.tx.pdev_cont_xretry);
        ac_stats->stats.tx.pdev_tx_timeout = adf_os_ntohl(stats->stats.tx.pdev_tx_timeout);
        ac_stats->stats.tx.pdev_resets = adf_os_ntohl(stats->stats.tx.pdev_resets);
        ac_stats->stats.tx.stateless_tid_alloc_failure = adf_os_ntohl(stats->stats.tx.stateless_tid_alloc_failure);
        ac_stats->stats.tx.phy_underrun = adf_os_ntohl(stats->stats.tx.phy_underrun);
        ac_stats->stats.tx.txop_ovf = adf_os_ntohl(stats->stats.tx.txop_ovf);
        ac_stats->stats.rx.mid_ppdu_route_change = adf_os_ntohl(stats->stats.rx.mid_ppdu_route_change);
        ac_stats->stats.rx.status_rcvd = adf_os_ntohl(stats->stats.rx.status_rcvd);
        ac_stats->stats.rx.r0_frags = adf_os_ntohl(stats->stats.rx.r0_frags);
        ac_stats->stats.rx.r1_frags = adf_os_ntohl(stats->stats.rx.r1_frags);
        ac_stats->stats.rx.r2_frags = adf_os_ntohl(stats->stats.rx.r2_frags);
        ac_stats->stats.rx.r3_frags = adf_os_ntohl(stats->stats.rx.r3_frags);
        ac_stats->stats.rx.htt_msdus = adf_os_ntohl(stats->stats.rx.htt_msdus);
        ac_stats->stats.rx.htt_mpdus = adf_os_ntohl(stats->stats.rx.htt_mpdus);
        ac_stats->stats.rx.loc_msdus = adf_os_ntohl(stats->stats.rx.loc_msdus);
        ac_stats->stats.rx.loc_mpdus = adf_os_ntohl(stats->stats.rx.loc_mpdus);
        ac_stats->stats.rx.oversize_amsdu = adf_os_ntohl(stats->stats.rx.oversize_amsdu);
        ac_stats->stats.rx.phy_errs = adf_os_ntohl(stats->stats.rx.phy_errs);
        ac_stats->stats.rx.phy_err_drop = adf_os_ntohl(stats->stats.rx.phy_err_drop);
        ac_stats->stats.rx.mpdu_errs = adf_os_ntohl(stats->stats.rx.mpdu_errs);
        ac_stats->stats.mem.dram_free_size = adf_os_ntohl(stats->stats.mem.dram_remain);
        ac_stats->stats.mem.iram_free_size = adf_os_ntohl(stats->stats.mem.iram_remain);
        ac_stats->stats.peer.dummy = adf_os_ntohl(stats->stats.peer.dummy);
        ac_stats->interface_stats.tx_beacon = adf_os_be64_to_cpu(stats->interface_stats.tx_beacon);
        ac_stats->interface_stats.be_nobuf = adf_os_ntohl(stats->interface_stats.be_nobuf);
        ac_stats->interface_stats.tx_buf_count = adf_os_ntohl(stats->interface_stats.tx_buf_count);
        ac_stats->interface_stats.tx_packets = adf_os_ntohl(stats->interface_stats.tx_packets);
        ac_stats->interface_stats.rx_packets = adf_os_ntohl(stats->interface_stats.rx_packets);
        ac_stats->interface_stats.tx_mgmt = adf_os_ntohl(stats->interface_stats.tx_mgmt);
        ac_stats->interface_stats.tx_num_data = adf_os_ntohl(stats->interface_stats.tx_num_data);
        ac_stats->interface_stats.rx_num_data = adf_os_ntohl(stats->interface_stats.rx_num_data);
        ac_stats->interface_stats.rx_mgmt = adf_os_ntohl(stats->interface_stats.rx_mgmt);
        ac_stats->interface_stats.rx_num_mgmt = adf_os_ntohl(stats->interface_stats.rx_num_mgmt);
        ac_stats->interface_stats.rx_num_ctl = adf_os_ntohl(stats->interface_stats.rx_num_ctl);
        ac_stats->interface_stats.tx_rssi = adf_os_ntohl(stats->interface_stats.tx_rssi);
        for (i = 0; i < 10; i++) {
            ac_stats->interface_stats.tx_mcs[i] = adf_os_ntohl(stats->interface_stats.tx_mcs[i]);
            ac_stats->interface_stats.rx_mcs[i] = adf_os_ntohl(stats->interface_stats.rx_mcs[i]);
        }
        ac_stats->interface_stats.rx_rssi_comb = adf_os_ntohl(stats->interface_stats.rx_rssi_comb);
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
        ac_stats->interface_stats.rx_bytes = adf_os_be64_to_cpu(stats->interface_stats.rx_bytes);
        ac_stats->interface_stats.tx_bytes = adf_os_be64_to_cpu(stats->interface_stats.tx_bytes);
        ac_stats->interface_stats.tx_compaggr = adf_os_ntohl(stats->interface_stats.tx_compaggr);
        ac_stats->interface_stats.rx_aggr = adf_os_ntohl(stats->interface_stats.rx_aggr);
        ac_stats->interface_stats.tx_bawadv = adf_os_ntohl(stats->interface_stats.tx_bawadv);
        ac_stats->interface_stats.tx_compunaggr = adf_os_ntohl(stats->interface_stats.tx_compunaggr);
        ac_stats->interface_stats.rx_overrun = adf_os_ntohl(stats->interface_stats.rx_overrun);
        ac_stats->interface_stats.rx_badcrypt = adf_os_ntohl(stats->interface_stats.rx_badcrypt);
        ac_stats->interface_stats.rx_badmic = adf_os_ntohl(stats->interface_stats.rx_badmic);
        ac_stats->interface_stats.rx_crcerr = adf_os_ntohl(stats->interface_stats.rx_crcerr);
        ac_stats->interface_stats.rx_phyerr = adf_os_ntohl(stats->interface_stats.rx_phyerr);
        ac_stats->interface_stats.ackRcvBad = adf_os_ntohl(stats->interface_stats.ackRcvBad);
        ac_stats->interface_stats.rtsBad = adf_os_ntohl(stats->interface_stats.rtsBad);
        ac_stats->interface_stats.rtsGood = adf_os_ntohl(stats->interface_stats.rtsGood);
        ac_stats->interface_stats.fcsBad = adf_os_ntohl(stats->interface_stats.fcsBad);
        ac_stats->interface_stats.noBeacons = adf_os_ntohl(stats->interface_stats.noBeacons);
        ac_stats->interface_stats.mib_int_count = adf_os_ntohl(stats->interface_stats.mib_int_count);
        ac_stats->interface_stats.rx_looplimit_start = adf_os_ntohl(stats->interface_stats.rx_looplimit_start);
        ac_stats->interface_stats.rx_looplimit_end = adf_os_ntohl(stats->interface_stats.rx_looplimit_end);
        ac_stats->interface_stats.ap_stats_tx_cal_enable = stats->interface_stats.ap_stats_tx_cal_enable;
    } else if (athstats_wcmd->offload_if == 0) {
        a_uint32_t i;
        acfg_ath_stats_11n_t *n_stats = (acfg_ath_stats_11n_t *)(athstats_wcmd->address);
        atd_ath_stats_11n_t *stats = (atd_ath_stats_11n_t *)((a_uint8_t *)resp + sizeof(atd_ath_stats_t));
        athstats_wcmd->size = sizeof(acfg_ath_stats_11n_t);

        n_stats->ast_watchdog = adf_os_ntohl(stats->ast_watchdog);
        n_stats->ast_resetOnError = adf_os_ntohl(stats->ast_resetOnError);
        n_stats->ast_mat_ucast_encrypted = adf_os_ntohl(stats->ast_mat_ucast_encrypted);
        n_stats->ast_mat_rx_recrypt = adf_os_ntohl(stats->ast_mat_rx_recrypt);
        n_stats->ast_mat_rx_decrypt = adf_os_ntohl(stats->ast_mat_rx_decrypt);
        n_stats->ast_mat_key_switch = adf_os_ntohl(stats->ast_mat_key_switch);
        n_stats->ast_hardware = adf_os_ntohl(stats->ast_hardware);
        n_stats->ast_bmiss = adf_os_ntohl(stats->ast_bmiss);
        n_stats->ast_rxorn = adf_os_ntohl(stats->ast_rxorn);
        n_stats->ast_rxorn_bmiss = adf_os_ntohl(stats->ast_rxorn_bmiss);
        n_stats->ast_rxeol = adf_os_ntohl(stats->ast_rxeol);
        n_stats->ast_txurn = adf_os_ntohl(stats->ast_txurn);
        n_stats->ast_txto = adf_os_ntohl(stats->ast_txto);
        n_stats->ast_cst = adf_os_ntohl(stats->ast_cst);
        n_stats->ast_mib = adf_os_ntohl(stats->ast_mib);
        n_stats->ast_rx = adf_os_ntohl(stats->ast_rx);
        n_stats->ast_rxdesc = adf_os_ntohl(stats->ast_rxdesc);
        n_stats->ast_rxerr = adf_os_ntohl(stats->ast_rxerr);
        n_stats->ast_rxnofrm = adf_os_ntohl(stats->ast_rxnofrm);
        n_stats->ast_tx = adf_os_ntohl(stats->ast_tx);
        n_stats->ast_txdesc = adf_os_ntohl(stats->ast_txdesc);
        n_stats->ast_tim_timer = adf_os_ntohl(stats->ast_tim_timer);
        n_stats->ast_bbevent = adf_os_ntohl(stats->ast_bbevent);
        n_stats->ast_rxphy = adf_os_ntohl(stats->ast_rxphy);
        n_stats->ast_rxkcm = adf_os_ntohl(stats->ast_rxkcm);
        n_stats->ast_swba = adf_os_ntohl(stats->ast_swba);
        n_stats->ast_brssi = adf_os_ntohl(stats->ast_brssi);
        n_stats->ast_bnr = adf_os_ntohl(stats->ast_bnr);
        n_stats->ast_tim = adf_os_ntohl(stats->ast_tim);
        n_stats->ast_dtim = adf_os_ntohl(stats->ast_dtim);
        n_stats->ast_dtimsync = adf_os_ntohl(stats->ast_dtimsync);
        n_stats->ast_gpio = adf_os_ntohl(stats->ast_gpio);
        n_stats->ast_cabend = adf_os_ntohl(stats->ast_cabend);
        n_stats->ast_tsfoor = adf_os_ntohl(stats->ast_tsfoor);
        n_stats->ast_gentimer = adf_os_ntohl(stats->ast_gentimer);
        n_stats->ast_gtt = adf_os_ntohl(stats->ast_gtt);
        n_stats->ast_fatal = adf_os_ntohl(stats->ast_fatal);
        n_stats->ast_tx_packets = adf_os_ntohl(stats->ast_tx_packets);
        n_stats->ast_rx_packets = adf_os_ntohl(stats->ast_rx_packets);
        n_stats->ast_tx_mgmt = adf_os_ntohl(stats->ast_tx_mgmt);
        n_stats->ast_tx_discard = adf_os_ntohl(stats->ast_tx_discard);
        n_stats->ast_tx_invalid = adf_os_ntohl(stats->ast_tx_invalid);
        n_stats->ast_tx_qstop = adf_os_ntohl(stats->ast_tx_qstop);
        n_stats->ast_tx_encap = adf_os_ntohl(stats->ast_tx_encap);
        n_stats->ast_tx_nonode = adf_os_ntohl(stats->ast_tx_nonode);
        n_stats->ast_tx_nobuf = adf_os_ntohl(stats->ast_tx_nobuf);
        n_stats->ast_tx_stop = adf_os_ntohl(stats->ast_tx_stop);
        n_stats->ast_tx_resume = adf_os_ntohl(stats->ast_tx_resume);
        n_stats->ast_tx_nobufmgt = adf_os_ntohl(stats->ast_tx_nobufmgt);
        n_stats->ast_tx_xretries = adf_os_ntohl(stats->ast_tx_xretries);
        n_stats->ast_tx_hw_retries = adf_os_be64_to_cpu(stats->ast_tx_hw_retries);
        n_stats->ast_tx_hw_success = adf_os_be64_to_cpu(stats->ast_tx_hw_success);
        n_stats->ast_tx_fifoerr = adf_os_ntohl(stats->ast_tx_fifoerr);
        n_stats->ast_tx_filtered = adf_os_ntohl(stats->ast_tx_filtered);
        n_stats->ast_tx_badrate = adf_os_ntohl(stats->ast_tx_badrate);
        n_stats->ast_tx_noack = adf_os_ntohl(stats->ast_tx_noack);
        n_stats->ast_tx_cts = adf_os_ntohl(stats->ast_tx_cts);
        n_stats->ast_tx_shortpre = adf_os_ntohl(stats->ast_tx_shortpre);
        n_stats->ast_tx_altrate = adf_os_ntohl(stats->ast_tx_altrate);
        n_stats->ast_tx_protect = adf_os_ntohl(stats->ast_tx_protect);
        n_stats->ast_rx_orn = adf_os_ntohl(stats->ast_rx_orn);
        n_stats->ast_rx_badcrypt = adf_os_ntohl(stats->ast_rx_badcrypt);
        n_stats->ast_rx_badmic = adf_os_ntohl(stats->ast_rx_badmic);
        n_stats->ast_rx_nobuf = adf_os_ntohl(stats->ast_rx_nobuf);
        n_stats->ast_rx_swdecrypt = adf_os_ntohl(stats->ast_rx_swdecrypt);
        n_stats->ast_rx_hal_in_progress = adf_os_ntohl(stats->ast_rx_hal_in_progress);
        n_stats->ast_rx_num_data = adf_os_ntohl(stats->ast_rx_num_data);
        n_stats->ast_rx_num_mgmt = adf_os_ntohl(stats->ast_rx_num_mgmt);
        n_stats->ast_rx_num_ctl = adf_os_ntohl(stats->ast_rx_num_ctl);
        n_stats->ast_rx_num_unknown = adf_os_ntohl(stats->ast_rx_num_unknown);
        n_stats->ast_max_pkts_per_intr = adf_os_ntohl(stats->ast_max_pkts_per_intr);
        for (i = 0; i <= ACFG_MAX_INTR_BKT; i++)
            n_stats->ast_pkts_per_intr[i] = adf_os_ntohl(stats->ast_pkts_per_intr[i]);
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
        n_stats->ast_be_xmit = adf_os_ntohl(stats->ast_be_xmit);
        n_stats->ast_be_nobuf = adf_os_ntohl(stats->ast_be_nobuf);
        n_stats->ast_per_cal = adf_os_ntohl(stats->ast_per_cal);
        n_stats->ast_per_calfail = adf_os_ntohl(stats->ast_per_calfail);
        n_stats->ast_per_rfgain = adf_os_ntohl(stats->ast_per_rfgain);
        n_stats->ast_rate_calls = adf_os_ntohl(stats->ast_rate_calls);
        n_stats->ast_rate_raise = adf_os_ntohl(stats->ast_rate_raise);
        n_stats->ast_rate_drop = adf_os_ntohl(stats->ast_rate_drop);
        n_stats->ast_ant_defswitch = adf_os_ntohl(stats->ast_ant_defswitch);
        n_stats->ast_ant_txswitch = adf_os_ntohl(stats->ast_ant_txswitch);
        for (i = 0; i < 8; i++)
            n_stats->ast_ant_rx[i] = adf_os_ntohl(stats->ast_ant_rx[i]);
        for (i = 0; i < 8; i++)
            n_stats->ast_ant_tx[i] = adf_os_ntohl(stats->ast_ant_tx[i]);
        n_stats->ast_rx_bytes = adf_os_be64_to_cpu(stats->ast_rx_bytes);
        n_stats->ast_tx_bytes = adf_os_be64_to_cpu(stats->ast_tx_bytes);
        for (i = 0; i < 16; i++)
            n_stats->ast_rx_num_qos_data[i] = adf_os_ntohl(stats->ast_rx_num_qos_data[i]);
        n_stats->ast_rx_num_nonqos_data = adf_os_ntohl(stats->ast_rx_num_nonqos_data);
        for (i = 0; i < 16; i++)
            n_stats->ast_txq_packets[i] = adf_os_ntohl(stats->ast_txq_packets[i]);
        for (i = 0; i < 16; i++)
            n_stats->ast_txq_xretries[i] = adf_os_ntohl(stats->ast_txq_xretries[i]);
        for (i = 0; i < 16; i++)
            n_stats->ast_txq_fifoerr[i] = adf_os_ntohl(stats->ast_txq_fifoerr[i]);
        for (i = 0; i < 16; i++)
            n_stats->ast_txq_filtered[i] = adf_os_ntohl(stats->ast_txq_filtered[i]);
        for (i = 0; i < 16; i++)
            n_stats->ast_txq_athbuf_limit[i] = adf_os_ntohl(stats->ast_txq_athbuf_limit[i]);
        for (i = 0; i < 16; i++)
            n_stats->ast_txq_nobuf[i] = adf_os_ntohl(stats->ast_txq_nobuf[i]);
        n_stats->ast_num_rxchain = stats->ast_num_rxchain;
        n_stats->ast_num_txchain = stats->ast_num_txchain;
        n_stats->ast_11n_stats.tx_pkts = adf_os_ntohl(stats->ast_11n_stats.tx_pkts);
        n_stats->ast_11n_stats.tx_checks = adf_os_ntohl(stats->ast_11n_stats.tx_checks);
        n_stats->ast_11n_stats.tx_drops = adf_os_ntohl(stats->ast_11n_stats.tx_drops);
        n_stats->ast_11n_stats.tx_minqdepth = adf_os_ntohl(stats->ast_11n_stats.tx_minqdepth);
        n_stats->ast_11n_stats.tx_queue = adf_os_ntohl(stats->ast_11n_stats.tx_queue);
        n_stats->ast_11n_stats.tx_resetq = adf_os_ntohl(stats->ast_11n_stats.tx_resetq);
        n_stats->ast_11n_stats.tx_comps = adf_os_ntohl(stats->ast_11n_stats.tx_comps);
        n_stats->ast_11n_stats.tx_comperror = adf_os_ntohl(stats->ast_11n_stats.tx_comperror);
        n_stats->ast_11n_stats.tx_unaggr_comperror = adf_os_ntohl(stats->ast_11n_stats.tx_unaggr_comperror);
        n_stats->ast_11n_stats.tx_stopfiltered = adf_os_ntohl(stats->ast_11n_stats.tx_stopfiltered);
        n_stats->ast_11n_stats.tx_qnull = adf_os_ntohl(stats->ast_11n_stats.tx_qnull);
        n_stats->ast_11n_stats.tx_noskbs = adf_os_ntohl(stats->ast_11n_stats.tx_noskbs);
        n_stats->ast_11n_stats.tx_nobufs = adf_os_ntohl(stats->ast_11n_stats.tx_nobufs);
        n_stats->ast_11n_stats.tx_badsetups = adf_os_ntohl(stats->ast_11n_stats.tx_badsetups);
        n_stats->ast_11n_stats.tx_normnobufs = adf_os_ntohl(stats->ast_11n_stats.tx_normnobufs);
        n_stats->ast_11n_stats.tx_schednone = adf_os_ntohl(stats->ast_11n_stats.tx_schednone);
        n_stats->ast_11n_stats.tx_bars = adf_os_ntohl(stats->ast_11n_stats.tx_bars);
        n_stats->ast_11n_stats.tx_legacy = adf_os_ntohl(stats->ast_11n_stats.tx_legacy);
        n_stats->ast_11n_stats.txunaggr_single = adf_os_ntohl(stats->ast_11n_stats.txunaggr_single);
        n_stats->ast_11n_stats.txbar_xretry = adf_os_ntohl(stats->ast_11n_stats.txbar_xretry);
        n_stats->ast_11n_stats.txbar_compretries = adf_os_ntohl(stats->ast_11n_stats.txbar_compretries);
        n_stats->ast_11n_stats.txbar_errlast = adf_os_ntohl(stats->ast_11n_stats.txbar_errlast);
        n_stats->ast_11n_stats.tx_compunaggr = adf_os_ntohl(stats->ast_11n_stats.tx_compunaggr);
        n_stats->ast_11n_stats.txunaggr_xretry = adf_os_ntohl(stats->ast_11n_stats.txunaggr_xretry);
        n_stats->ast_11n_stats.tx_compaggr = adf_os_ntohl(stats->ast_11n_stats.tx_compaggr);
        n_stats->ast_11n_stats.tx_bawadv = adf_os_ntohl(stats->ast_11n_stats.tx_bawadv);
        n_stats->ast_11n_stats.tx_bawretries = adf_os_ntohl(stats->ast_11n_stats.tx_bawretries);
        n_stats->ast_11n_stats.tx_bawnorm = adf_os_ntohl(stats->ast_11n_stats.tx_bawnorm);
        n_stats->ast_11n_stats.tx_bawupdates = adf_os_ntohl(stats->ast_11n_stats.tx_bawupdates);
        n_stats->ast_11n_stats.tx_bawupdtadv = adf_os_ntohl(stats->ast_11n_stats.tx_bawupdtadv);
        n_stats->ast_11n_stats.tx_retries = adf_os_ntohl(stats->ast_11n_stats.tx_retries);
        n_stats->ast_11n_stats.tx_xretries = adf_os_ntohl(stats->ast_11n_stats.tx_xretries);
        n_stats->ast_11n_stats.tx_aggregates = adf_os_ntohl(stats->ast_11n_stats.tx_aggregates);
        n_stats->ast_11n_stats.tx_sf_hw_xretries = adf_os_ntohl(stats->ast_11n_stats.tx_sf_hw_xretries);
        n_stats->ast_11n_stats.tx_aggr_frames = adf_os_ntohl(stats->ast_11n_stats.tx_aggr_frames);
        n_stats->ast_11n_stats.txaggr_noskbs = adf_os_ntohl(stats->ast_11n_stats.txaggr_noskbs);
        n_stats->ast_11n_stats.txaggr_nobufs = adf_os_ntohl(stats->ast_11n_stats.txaggr_nobufs);
        n_stats->ast_11n_stats.txaggr_badkeys = adf_os_ntohl(stats->ast_11n_stats.txaggr_badkeys);
        n_stats->ast_11n_stats.txaggr_schedwindow = adf_os_ntohl(stats->ast_11n_stats.txaggr_schedwindow);
        n_stats->ast_11n_stats.txaggr_single = adf_os_ntohl(stats->ast_11n_stats.txaggr_single);
        n_stats->ast_11n_stats.txaggr_mimo = adf_os_ntohl(stats->ast_11n_stats.txaggr_mimo);
        n_stats->ast_11n_stats.txaggr_compgood = adf_os_ntohl(stats->ast_11n_stats.txaggr_compgood);
        n_stats->ast_11n_stats.txaggr_comperror = adf_os_ntohl(stats->ast_11n_stats.txaggr_comperror);
        n_stats->ast_11n_stats.txaggr_compxretry = adf_os_ntohl(stats->ast_11n_stats.txaggr_compxretry);
        n_stats->ast_11n_stats.txaggr_compretries = adf_os_ntohl(stats->ast_11n_stats.txaggr_compretries);
        n_stats->ast_11n_stats.txunaggr_compretries = adf_os_ntohl(stats->ast_11n_stats.txunaggr_compretries);
        n_stats->ast_11n_stats.txaggr_prepends = adf_os_ntohl(stats->ast_11n_stats.txaggr_prepends);
        n_stats->ast_11n_stats.txaggr_filtered = adf_os_ntohl(stats->ast_11n_stats.txaggr_filtered);
        n_stats->ast_11n_stats.txaggr_fifo = adf_os_ntohl(stats->ast_11n_stats.txaggr_fifo);
        n_stats->ast_11n_stats.txaggr_xtxop = adf_os_ntohl(stats->ast_11n_stats.txaggr_xtxop);
        n_stats->ast_11n_stats.txaggr_desc_cfgerr = adf_os_ntohl(stats->ast_11n_stats.txaggr_desc_cfgerr);
        n_stats->ast_11n_stats.txaggr_data_urun = adf_os_ntohl(stats->ast_11n_stats.txaggr_data_urun);
        n_stats->ast_11n_stats.txaggr_delim_urun = adf_os_ntohl(stats->ast_11n_stats.txaggr_delim_urun);
        n_stats->ast_11n_stats.txaggr_errlast = adf_os_ntohl(stats->ast_11n_stats.txaggr_errlast);
        n_stats->ast_11n_stats.txunaggr_errlast = adf_os_ntohl(stats->ast_11n_stats.txunaggr_errlast);
        n_stats->ast_11n_stats.txaggr_longretries = adf_os_ntohl(stats->ast_11n_stats.txaggr_longretries);
        n_stats->ast_11n_stats.txaggr_shortretries = adf_os_ntohl(stats->ast_11n_stats.txaggr_shortretries);
        n_stats->ast_11n_stats.txaggr_timer_exp = adf_os_ntohl(stats->ast_11n_stats.txaggr_timer_exp);
        n_stats->ast_11n_stats.txaggr_babug = adf_os_ntohl(stats->ast_11n_stats.txaggr_babug);
        n_stats->ast_11n_stats.txrifs_single = adf_os_ntohl(stats->ast_11n_stats.txrifs_single);
        n_stats->ast_11n_stats.txrifs_babug = adf_os_ntohl(stats->ast_11n_stats.txrifs_babug);
        n_stats->ast_11n_stats.txaggr_badtid = adf_os_ntohl(stats->ast_11n_stats.txaggr_badtid);
        n_stats->ast_11n_stats.txrifs_compretries = adf_os_ntohl(stats->ast_11n_stats.txrifs_compretries);
        n_stats->ast_11n_stats.txrifs_bar_alloc = adf_os_ntohl(stats->ast_11n_stats.txrifs_bar_alloc);
        n_stats->ast_11n_stats.txrifs_bar_freed = adf_os_ntohl(stats->ast_11n_stats.txrifs_bar_freed);
        n_stats->ast_11n_stats.txrifs_compgood = adf_os_ntohl(stats->ast_11n_stats.txrifs_compgood);
        n_stats->ast_11n_stats.txrifs_prepends = adf_os_ntohl(stats->ast_11n_stats.txrifs_prepends);
        n_stats->ast_11n_stats.tx_comprifs = adf_os_ntohl(stats->ast_11n_stats.tx_comprifs);
        n_stats->ast_11n_stats.tx_compnorifs = adf_os_ntohl(stats->ast_11n_stats.tx_compnorifs);
        n_stats->ast_11n_stats.rx_pkts = adf_os_ntohl(stats->ast_11n_stats.rx_pkts);
        n_stats->ast_11n_stats.rx_aggr = adf_os_ntohl(stats->ast_11n_stats.rx_aggr);
        n_stats->ast_11n_stats.rx_aggrbadver = adf_os_ntohl(stats->ast_11n_stats.rx_aggrbadver);
        n_stats->ast_11n_stats.rx_bars = adf_os_ntohl(stats->ast_11n_stats.rx_bars);
        n_stats->ast_11n_stats.rx_nonqos = adf_os_ntohl(stats->ast_11n_stats.rx_nonqos);
        n_stats->ast_11n_stats.rx_seqreset= adf_os_ntohl(stats->ast_11n_stats.rx_seqreset);
        n_stats->ast_11n_stats.rx_oldseq = adf_os_ntohl(stats->ast_11n_stats.rx_oldseq);
        n_stats->ast_11n_stats.rx_bareset = adf_os_ntohl(stats->ast_11n_stats.rx_bareset);
        n_stats->ast_11n_stats.rx_baresetpkts = adf_os_ntohl(stats->ast_11n_stats.rx_baresetpkts);
        n_stats->ast_11n_stats.rx_dup = adf_os_ntohl(stats->ast_11n_stats.rx_dup);
        n_stats->ast_11n_stats.rx_baadvance = adf_os_ntohl(stats->ast_11n_stats.rx_baadvance);
        n_stats->ast_11n_stats.rx_recvcomp = adf_os_ntohl(stats->ast_11n_stats.rx_recvcomp);
        n_stats->ast_11n_stats.rx_bardiscard = adf_os_ntohl(stats->ast_11n_stats.rx_bardiscard);
        n_stats->ast_11n_stats.rx_barcomps = adf_os_ntohl(stats->ast_11n_stats.rx_barcomps);
        n_stats->ast_11n_stats.rx_barrecvs = adf_os_ntohl(stats->ast_11n_stats.rx_barrecvs);
        n_stats->ast_11n_stats.rx_skipped = adf_os_ntohl(stats->ast_11n_stats.rx_skipped);
        n_stats->ast_11n_stats.rx_comp_to = adf_os_ntohl(stats->ast_11n_stats.rx_comp_to);
        n_stats->ast_11n_stats.rx_timer_starts = adf_os_ntohl(stats->ast_11n_stats.rx_timer_starts);
        n_stats->ast_11n_stats.rx_timer_stops = adf_os_ntohl(stats->ast_11n_stats.rx_timer_stops);
        n_stats->ast_11n_stats.rx_timer_run = adf_os_ntohl(stats->ast_11n_stats.rx_timer_run);
        n_stats->ast_11n_stats.rx_timer_more = adf_os_ntohl(stats->ast_11n_stats.rx_timer_more);
        n_stats->ast_11n_stats.wd_tx_active = adf_os_ntohl(stats->ast_11n_stats.wd_tx_active);
        n_stats->ast_11n_stats.wd_tx_inactive = adf_os_ntohl(stats->ast_11n_stats.wd_tx_inactive);
        n_stats->ast_11n_stats.wd_tx_hung = adf_os_ntohl(stats->ast_11n_stats.wd_tx_hung);
        n_stats->ast_11n_stats.wd_spurious = adf_os_ntohl(stats->ast_11n_stats.wd_spurious);
        n_stats->ast_11n_stats.tx_requeue = adf_os_ntohl(stats->ast_11n_stats.tx_requeue);
        n_stats->ast_11n_stats.tx_drain_txq = adf_os_ntohl(stats->ast_11n_stats.tx_drain_txq);
        n_stats->ast_11n_stats.tx_drain_tid = adf_os_ntohl(stats->ast_11n_stats.tx_drain_tid);
        n_stats->ast_11n_stats.tx_cleanup_tid = adf_os_ntohl(stats->ast_11n_stats.tx_cleanup_tid);
        n_stats->ast_11n_stats.tx_drain_bufs = adf_os_ntohl(stats->ast_11n_stats.tx_drain_bufs);
        n_stats->ast_11n_stats.tx_tidpaused = adf_os_ntohl(stats->ast_11n_stats.tx_tidpaused);
        n_stats->ast_11n_stats.tx_tidresumed = adf_os_ntohl(stats->ast_11n_stats.tx_tidresumed);
        n_stats->ast_11n_stats.tx_unaggr_filtered = adf_os_ntohl(stats->ast_11n_stats.tx_unaggr_filtered);
        n_stats->ast_11n_stats.tx_aggr_filtered = adf_os_ntohl(stats->ast_11n_stats.tx_aggr_filtered);
        n_stats->ast_11n_stats.tx_filtered = adf_os_ntohl(stats->ast_11n_stats.tx_filtered);
        n_stats->ast_11n_stats.rx_rb_on = adf_os_ntohl(stats->ast_11n_stats.rx_rb_on);
        n_stats->ast_11n_stats.rx_rb_off = adf_os_ntohl(stats->ast_11n_stats.rx_rb_off);
        n_stats->ast_11n_stats.rx_dsstat_err = adf_os_ntohl(stats->ast_11n_stats.rx_dsstat_err);
#ifdef ATH_SUPPORT_TxBF
        n_stats->ast_11n_stats.bf_stream_miss = adf_os_ntohl(stats->ast_11n_stats.bf_stream_miss);
        n_stats->ast_11n_stats.bf_bandwidth_miss = adf_os_ntohl(stats->ast_11n_stats.bf_bandwidth_miss);
        n_stats->ast_11n_stats.bf_destination_miss = adf_os_ntohl(stats->ast_11n_stats.bf_destination_miss);
#endif
        n_stats->ast_dfs_stats.dfs_stats_valid = adf_os_ntohl(stats->ast_dfs_stats.dfs_stats_valid);
        n_stats->ast_dfs_stats.event_count = adf_os_ntohl(stats->ast_dfs_stats.event_count);
        n_stats->ast_dfs_stats.event_count = adf_os_ntohl(stats->ast_dfs_stats.event_count);
        n_stats->ast_dfs_stats.num_filter = adf_os_ntohl(stats->ast_dfs_stats.num_filter);
        for (i = 0; i < ACFG_MAX_DFS_FILTER; i++) {
            n_stats->ast_dfs_stats.fstat[i].max_pri_count = adf_os_ntohl(stats->ast_dfs_stats.fstat[i].max_pri_count);
            n_stats->ast_dfs_stats.fstat[i].max_used_pri = adf_os_ntohl(stats->ast_dfs_stats.fstat[i].max_used_pri);
            n_stats->ast_dfs_stats.fstat[i].excess_pri = adf_os_ntohl(stats->ast_dfs_stats.fstat[i].excess_pri);
            n_stats->ast_dfs_stats.fstat[i].pri_threshold_reached = adf_os_ntohl(stats->ast_dfs_stats.fstat[i].pri_threshold_reached);
            n_stats->ast_dfs_stats.fstat[i].dur_threshold_reached = adf_os_ntohl(stats->ast_dfs_stats.fstat[i].dur_threshold_reached);
            n_stats->ast_dfs_stats.fstat[i].rssi_threshold_reached = adf_os_ntohl(stats->ast_dfs_stats.fstat[i].rssi_threshold_reached);
            n_stats->ast_dfs_stats.fstat[i].filter_id = adf_os_ntohl(stats->ast_dfs_stats.fstat[i].filter_id);
        }
        n_stats->ast_bb_hang = adf_os_ntohl(stats->ast_bb_hang);
        n_stats->ast_mac_hang = adf_os_ntohl(stats->ast_mac_hang);
#if ATH_WOW
        n_stats->ast_wow_wakeups = adf_os_ntohl(stats->ast_wow_wakeups);
        n_stats->ast_wow_wakeupsok = adf_os_ntohl(stats->ast_wow_wakeupsok);
        n_stats->ast_wow_wakeupserror = adf_os_ntohl(stats->ast_wow_wakeupserror);
#if ATH_WOW_DEBUG
        n_stats->ast_normal_sleeps = adf_os_ntohl(stats->ast_normal_sleeps);
        n_stats->ast_normal_wakeups = adf_os_ntohl(stats->ast_normal_wakeups);
        n_stats->ast_wow_sleeps = adf_os_ntohl(stats->ast_wow_sleeps);
        n_stats->ast_wow_sleeps_nonet = adf_os_ntohl(stats->ast_wow_sleeps_nonet);
#endif
#endif
#ifdef ATH_SUPPORT_UAPSD
        n_stats->ast_uapsdqnulbf_unavail = adf_os_ntohl(stats->ast_uapsdqnulbf_unavail);
        n_stats->ast_uapsdqnul_pkts = adf_os_ntohl(stats->ast_uapsdqnul_pkts);
        n_stats->ast_uapsdtriggers = adf_os_ntohl(stats->ast_uapsdtriggers);
        n_stats->ast_uapsdnodeinvalid = adf_os_ntohl(stats->ast_uapsdnodeinvalid);
        n_stats->ast_uapsdeospdata = adf_os_ntohl(stats->ast_uapsdeospdata);
        n_stats->ast_uapsddata_pkts = adf_os_ntohl(stats->ast_uapsddata_pkts);
        n_stats->ast_uapsddatacomp = adf_os_ntohl(stats->ast_uapsddatacomp);
        n_stats->ast_uapsdqnulcomp = adf_os_ntohl(stats->ast_uapsdqnulcomp);
        n_stats->ast_uapsddataqueued = adf_os_ntohl(stats->ast_uapsddataqueued);
        n_stats->ast_uapsdedmafifofull = adf_os_ntohl(stats->ast_uapsdedmafifofull);
#endif
#if ATH_SUPPORT_VOWEXT
        for (i = 0; i < 4; i++)
            n_stats->ast_vow_ul_tx_calls[i] = adf_os_ntohl(stats->ast_vow_ul_tx_calls[i]);
        for (i = 0; i < 4; i++)
            n_stats->ast_vow_ath_txq_calls[i] = adf_os_ntohl(stats->ast_vow_ath_txq_calls[i]);
        n_stats->ast_vow_ath_be_drop = adf_os_ntohl(stats->ast_vow_ath_be_drop);
        n_stats->ast_vow_ath_bk_drop = adf_os_ntohl(stats->ast_vow_ath_bk_drop);
#endif
#if ATH_SUPPORT_CFEND
        n_stats->ast_cfend_sched = adf_os_ntohl(stats->ast_cfend_sched);
        n_stats->ast_cfend_sent = adf_os_ntohl(stats->ast_cfend_sent);
#endif
#ifdef VOW_LOGLATENCY
        for (i = 0; i < ACFG_LATENCY_CATS; i++) {
            a_uint32_t j;
            for (j = 0; j < ACFG_LATENCY_BINS; j++) {
                n_stats->ast_retry_delay[i][j] = adf_os_ntohl(stats->ast_retry_delay[i][j]);
                n_stats->ast_queue_delay[i][j] = adf_os_ntohl(stats->ast_queue_delay[i][j]);
            }
        }
#endif
#if UMAC_SUPPORT_VI_DBG
        for (i = 0; i < ACFG_VI_LOG_LEN; i++) {
            n_stats->vi_timestamp[i] = adf_os_ntohl(stats->vi_timestamp[i]);
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
            n_stats->vi_tx_frame_cnt[i] = adf_os_ntohl(stats->vi_tx_frame_cnt[i]);
            n_stats->vi_rx_frame_cnt[i] = adf_os_ntohl(stats->vi_rx_frame_cnt[i]);
            n_stats->vi_rx_clr_cnt[i] = adf_os_ntohl(stats->vi_rx_clr_cnt[i]);
            n_stats->vi_rx_ext_clr_cnt[i] = adf_os_ntohl(stats->vi_rx_ext_clr_cnt[i]);
            n_stats->vi_cycle_cnt[i] = adf_os_ntohl(stats->vi_cycle_cnt[i]);
        }
        n_stats->vi_stats_index = stats->vi_stats_index;
#endif
#ifdef ATH_SUPPORT_TxBF
        n_stats->ast_txbf = stats->ast_txbf;
        n_stats->ast_lastratecode = stats->ast_lastratecode;
        n_stats->ast_sounding_count = adf_os_ntohl(stats->ast_sounding_count);
        n_stats->ast_txbf_rpt_count = adf_os_ntohl(stats->ast_txbf_rpt_count);
        for (i = 0; i <= ACFG_MCS_RATES; i++)
            n_stats->ast_mcs_count[i] = adf_os_ntohl(stats->ast_mcs_count[i]);
#endif
#if ATH_RX_LOOPLIMIT_TIMER
        n_stats->ast_rx_looplimit_start = adf_os_ntohl(stats->ast_rx_looplimit_start);
        n_stats->ast_rx_looplimit_end = adf_os_ntohl(stats->ast_rx_looplimit_end);
#endif
        n_stats->ast_chan_clr_cnt = adf_os_ntohl(stats->ast_chan_clr_cnt);
        n_stats->ast_cycle_cnt = adf_os_ntohl(stats->ast_cycle_cnt);
        n_stats->ast_noise_floor = adf_os_ntohs(stats->ast_noise_floor);
        n_stats->ast_mib_stats.ast_ackrcv_bad = adf_os_ntohl(stats->ast_mib_stats.ast_ackrcv_bad);
        n_stats->ast_mib_stats.ast_rts_bad = adf_os_ntohl(stats->ast_mib_stats.ast_rts_bad);
        n_stats->ast_mib_stats.ast_rts_good = adf_os_ntohl(stats->ast_mib_stats.ast_rts_good);
        n_stats->ast_mib_stats.ast_fcs_bad = adf_os_ntohl(stats->ast_mib_stats.ast_fcs_bad);
        n_stats->ast_mib_stats.ast_beacons = adf_os_ntohl(stats->ast_mib_stats.ast_beacons);
#ifdef ATH_SUPPORT_HTC
        n_stats->ast_mib_stats.ast_tgt_stats.tx_shortretry = adf_os_ntohl(stats->ast_mib_stats.ast_tgt_stats.tx_shortretry);
        n_stats->ast_mib_stats.ast_tgt_stats.tx_longretry = adf_os_ntohl(stats->ast_mib_stats.ast_tgt_stats.tx_longretry);
        n_stats->ast_mib_stats.ast_tgt_stats.tx_xretries = adf_os_ntohl(stats->ast_mib_stats.ast_tgt_stats.tx_xretries);
        n_stats->ast_mib_stats.ast_tgt_stats.ht_tx_unaggr_xretry = adf_os_ntohl(stats->ast_mib_stats.ast_tgt_stats.ht_tx_unaggr_xretry);
        n_stats->ast_mib_stats.ast_tgt_stats.ht_tx_xretries = adf_os_ntohl(stats->ast_mib_stats.ast_tgt_stats.ht_tx_xretriesht_tx_xretries);
        n_stats->ast_mib_stats.ast_tgt_stats.tx_pkt = adf_os_ntohl(stats->ast_mib_stats.ast_tgt_stats.tx_pkt);
        n_stats->ast_mib_stats.ast_tgt_stats.tx_aggr = adf_os_ntohl(stats->ast_mib_stats.ast_tgt_stats.tx_aggr);
        n_stats->ast_mib_stats.ast_tgt_stats.tx_retry = adf_os_ntohl(stats->ast_mib_stats.ast_tgt_stats.tx_retry);
        n_stats->ast_mib_stats.ast_tgt_stats.txaggr_retry = adf_os_ntohl(stats->ast_mib_stats.ast_tgt_stats.txaggr_retry);
        n_stats->ast_mib_stats.ast_tgt_stats.txaggr_sub_retry = adf_os_ntohl(stats->ast_mib_stats.ast_tgt_stats.txaggr_sub_retry);
#endif
        for (i = 0; i < ACFG_MAX_BB_PANICS; i++) {
            n_stats->ast_bb_panic[i].valid = adf_os_ntohl(stats->ast_bb_panic[i].valid);
            n_stats->ast_bb_panic[i].status = adf_os_ntohl(stats->ast_bb_panic[i].status);
            n_stats->ast_bb_panic[i].tsf = adf_os_ntohl(stats->ast_bb_panic[i].tsf);
            n_stats->ast_bb_panic[i].phy_panic_wd_ctl1 = adf_os_ntohl(stats->ast_bb_panic[i].phy_panic_wd_ctl1);
            n_stats->ast_bb_panic[i].phy_panic_wd_ctl2 = adf_os_ntohl(stats->ast_bb_panic[i].phy_panic_wd_ctl2);
            n_stats->ast_bb_panic[i].phy_gen_ctrl = adf_os_ntohl(stats->ast_bb_panic[i].phy_gen_ctrl);
            n_stats->ast_bb_panic[i].rxc_pcnt = adf_os_ntohl(stats->ast_bb_panic[i].rxc_pcnt);
            n_stats->ast_bb_panic[i].rxf_pcnt = adf_os_ntohl(stats->ast_bb_panic[i].rxf_pcnt);
            n_stats->ast_bb_panic[i].txf_pcnt = adf_os_ntohl(stats->ast_bb_panic[i].txf_pcnt);
            n_stats->ast_bb_panic[i].cycles = adf_os_ntohl(stats->ast_bb_panic[i].cycles);
            n_stats->ast_bb_panic[i].wd = adf_os_ntohl(stats->ast_bb_panic[i].wd);
            n_stats->ast_bb_panic[i].det = adf_os_ntohl(stats->ast_bb_panic[i].det);
            n_stats->ast_bb_panic[i].rdar = adf_os_ntohl(stats->ast_bb_panic[i].rdar);
            n_stats->ast_bb_panic[i].r_odfm = adf_os_ntohl(stats->ast_bb_panic[i].r_odfm);
            n_stats->ast_bb_panic[i].r_cck = adf_os_ntohl(stats->ast_bb_panic[i].r_cck);
            n_stats->ast_bb_panic[i].t_odfm = adf_os_ntohl(stats->ast_bb_panic[i].t_odfm);
            n_stats->ast_bb_panic[i].t_cck = adf_os_ntohl(stats->ast_bb_panic[i].t_cck);
            n_stats->ast_bb_panic[i].agc = adf_os_ntohl(stats->ast_bb_panic[i].agc);
            n_stats->ast_bb_panic[i].src = adf_os_ntohl(stats->ast_bb_panic[i].src);
        }
    }

fail:
    if (resp)
        adf_os_mem_free(resp);

    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("End stat %x ",status ));
    return status;
}

/** 
 * @brief Clear ath stats from target 
 * 
 * @param hdl
 * @param scan
 * 
 * @return status
 */
a_status_t
atd_cfg_clr_ath_stats(adf_drv_handle_t hdl, acfg_ath_stats_t *athstats_wcmd)
{
    atd_host_wifi_t     *wifi  = (atd_host_wifi_t * ) hdl;
    a_status_t          status = A_STATUS_OK;
    adf_nbuf_t          nbuf;
    atd_hdr_t           *hdrp;
    a_uint32_t          len = sizeof(atd_hdr_t);


    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("atd_cfg_clr_ath_stats"));

    if ((nbuf = _atd_wmi_msg_alloc(__wifi2wmi(wifi))) == NULL) {
        adf_os_print("Unable to allocate wmi buffer \n");
        status = A_STATUS_ENOMEM;
        goto fail ;
    }

    hdrp                  =  (atd_hdr_t *)adf_nbuf_put_tail(nbuf, len);
    hdrp->wifi_index      =   wifi->wifi_index;
    hdrp->vap_index       =   -1;

    status = wmi_cmd_send(__wifi2wmi(wifi), WMI_CMD_ATH_STATS_CLR, nbuf,
            NULL, 0);

fail:
    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("End %x ",status ));
    return status;
}

/** 
 * @brief Get Scan results from Target 
 * 
 * @param hdl
 * @param scan
 * 
 * @return status
 */
a_status_t 
atd_cfg_get_scan_results(adf_drv_handle_t hdl, acfg_scan_t *scan)
{
    atd_host_vap_t    *vap    = (atd_host_vap_t * )hdl;
    atd_host_wifi_t   *wifi   = vap->wifisc;
    a_status_t         status = A_STATUS_OK;
    a_uint32_t         rlen   = 0, count = 0, i;
    atd_scan_t        *pld;
    adf_nbuf_t         nbuf;
    void              *results = NULL;
    a_uint8_t         *pos, *end;
    acfg_scanresult_t *sl;

    atd_trace(ATD_DEBUG_FUNCTRACE, ("scan len %d \n", scan->len));


    /* First Get the scan space Length */
    if ((nbuf = _atd_wmi_msg_alloc(__wifi2wmi(wifi))) == NULL) {
        atd_trace(ATD_DEBUG_CFG, ("Unable to allocate wmi buffer \n"));
        status = A_STATUS_ENOMEM;
        goto fail ;
    }

    pld = (atd_scan_t *)atd_prep_cfg_hdr(nbuf, sizeof(a_uint32_t), 
            wifi, vap);

    status = wmi_cmd_send(__wifi2wmi(wifi), WMI_CMD_SCAN_SPACE_GET, nbuf,
                         (a_uint8_t* )&rlen, sizeof(a_uint32_t));


    rlen =  adf_os_ntohl(rlen);

    if( rlen > scan->len )
    {
        status = A_STATUS_E2BIG;
        goto fail;
    }

    results = adf_os_mem_alloc(NULL, rlen);
    if (!results) {
        atd_trace(ATD_DEBUG_ERROR, ("Unable to allocate response buffer \n"));
        status = A_STATUS_ENOMEM;
        goto fail;
    }

    if ((nbuf = _atd_wmi_msg_alloc(__wifi2wmi(wifi))) == NULL) {
        atd_trace(ATD_DEBUG_CFG, ("Unable to allocate wmi buffer \n"));
        status = A_STATUS_ENOMEM;
        goto fail_free ;
    }

    pld = (atd_scan_t *)atd_prep_cfg_hdr(nbuf, sizeof(a_uint32_t), 
                                         wifi, vap);

    pld->len = adf_os_htonl(scan->len);

    status = wmi_cmd_send(__wifi2wmi(wifi), WMI_CMD_SCAN_RESULTS_GET, nbuf,
                          results, rlen);
    

    /* Length of valid scan data 
     * for use by upper layers. 
    */
    scan->len = rlen ;

    pos = results;
    end = (results + rlen);
    sl = (acfg_scanresult_t *)scan->results;
    while (pos + sizeof(atd_scan_result_t) <= end) {
        atd_scan_result_t *sr;
        a_uint8_t *next;
        
        sr = (atd_scan_result_t *) pos;
        next = (pos + sizeof(*sr) + adf_os_ntohs(sr->isr_len));
        if (next > end)
            break;

        if (count + sizeof(*sl) + 
            adf_os_ntohs(sr->isr_len) > rlen)
            break;

        sl->isr_len = sizeof(*sl) + adf_os_ntohs(sr->isr_len);
        sl->isr_freq = adf_os_ntohs(sr->isr_freq);
        sl->isr_flags = adf_os_ntohl(sr->isr_flags);
        sl->isr_noise = sr->isr_noise;
        sl->isr_rssi = sr->isr_rssi;
        sl->isr_intval = sr->isr_intval;
        sl->isr_capinfo = adf_os_ntohs(sr->isr_capinfo);
        sl->isr_erp = sr->isr_erp;
        for (i = 0; i < ACFG_MACADDR_LEN; i++)
            sl->isr_bssid[i] = sr->isr_bssid[i];
        sl->isr_nrates = sr->isr_nrates;
        for (i = 0; i < 36; i++)
            sl->isr_rates[i] = sr->isr_rates[i];
        sl->isr_ssid_len = sr->isr_ssid_len;
        sl->isr_ie_len = adf_os_ntohs(sr->isr_ie_len);    
        memcpy(sl + 1, sr + 1, adf_os_ntohs(sr->isr_len));

        count += sizeof(*sl) + adf_os_ntohs(sr->isr_len);
        sl = (acfg_scanresult_t *)((a_uint8_t *)(sl + 1) + 
                                  (adf_os_ntohs(sr->isr_len)));
        pos = next;
    }

    adf_os_mem_free(results);
 
    atd_trace(ATD_DEBUG_FUNCTRACE, ("End."));
    return status;

fail_free:
    adf_os_mem_free(results);

fail:    
    atd_trace(ATD_DEBUG_FUNCTRACE, ("Failed... End."));
    return status;

}

/** 
 * @brief set scan request 
 * 
 * @param hdl
 * @param scan
 * 
 * @return 
 */
a_status_t 
atd_cfg_set_scan(adf_drv_handle_t hdl, acfg_set_scan_t *scan)
{
    atd_host_vap_t    *vap    = (atd_host_vap_t * )hdl;
    atd_host_wifi_t   *wifi   = vap->wifisc;
    a_status_t         status = A_STATUS_OK;
    a_uint32_t         resp, i;
    adf_nbuf_t         nbuf;
    atd_set_scan_t     *pld;

    atd_trace(ATD_DEBUG_FUNCTRACE, ("scan len %d \n", scan->len));


    if ((nbuf = _atd_wmi_msg_alloc(__wifi2wmi(wifi))) == NULL) {
        atd_trace(ATD_DEBUG_CFG, ("Unable to allocate wmi buffer \n"));
        status = A_STATUS_ENOMEM;
        goto fail ;
    }

    pld = (atd_set_scan_t *)atd_prep_cfg_hdr(nbuf, sizeof(atd_set_scan_t), 
            wifi, vap);

    pld->point_flags =  adf_os_htons(scan->point_flags);
    pld->len = adf_os_htons(scan->len);

    pld->scan_type = scan->scan_type;
    pld->essid_len = scan->essid_len;
    pld->num_channels = scan->num_channels;
    pld->flags = scan->flags;
    pld->bssid.sa_family = adf_os_htons(scan->bssid.sa_family);
    memcpy(pld->bssid.sa_data, scan->bssid.sa_data, 14);
    memcpy(pld->essid, scan->essid, ACFG_MAX_SSID_LEN);
    pld->min_channel_time = adf_os_htonl(scan->min_channel_time);
    pld->max_channel_time = adf_os_htonl(scan->max_channel_time);
    for(i=0; i<scan->num_channels; i++)
    {
        pld->channel_list[i].m = adf_os_htonl(scan->channel_list[i].m);
        pld->channel_list[i].e = adf_os_htons(scan->channel_list[i].e);
        pld->channel_list[i].i = scan->channel_list[i].i;
        pld->channel_list[i].flags = scan->channel_list[i].flags;
    }

    status = wmi_cmd_send(__wifi2wmi(wifi), WMI_CMD_SCAN_SET, nbuf,
            (a_uint8_t *)&resp, sizeof(a_uint32_t));

    if (status != A_STATUS_OK) {
        atd_trace(ATD_DEBUG_CFG, (" wmi_cmd_send Failed stat = %x\n", status));
        goto fail ;
    }

    atd_trace(ATD_DEBUG_FUNCTRACE, ("End."));
    return status;


fail:    
    atd_trace(ATD_DEBUG_FUNCTRACE, ("Failed End Status %d.",status));
    return status;
}

static a_status_t 
atd_cfg_get_range(adf_drv_handle_t hdl, acfg_range_t *range)
{
    atd_host_vap_t    *vap    = (atd_host_vap_t * )hdl;
    atd_host_wifi_t   *wifi   = vap->wifisc;
    a_status_t         status = A_STATUS_OK;
    adf_nbuf_t         nbuf;
    atd_range_buf_t    range_buf;
    acfg_range_buf_t  *tmp = (acfg_range_buf_t *)range->buff;
    a_uint32_t         i;

    atd_trace((ATD_DEBUG_FUNCTRACE|ATD_DEBUG_CFG), ("Start"));

    if ((nbuf = _atd_wmi_msg_alloc(__wifi2wmi(wifi))) == NULL) {
        atd_trace(ATD_DEBUG_CFG, ("Unable to allocate wmi buffer \n"));
        status = A_STATUS_ENOMEM;
        goto fail ;
    }

    atd_prep_cfg_hdr(nbuf, 0, wifi, vap);

    atd_trace(ATD_DEBUG_CFG, (" Range len %d \n ", range->len));

    status = wmi_cmd_send(__wifi2wmi(wifi), WMI_CMD_RANGE_GET, nbuf,
                          (a_uint8_t *)&range_buf, sizeof(atd_range_buf_t));

    if (status != A_STATUS_OK) {
        atd_trace(ATD_DEBUG_CFG, (" wmi_cmd_send Failed stat = %x\n ", status));
        status =  A_STATUS_EINVAL;
        goto fail;
    }

    tmp->throughput = adf_os_ntohl(range_buf.throughput);
    tmp->min_nwid = adf_os_ntohl(range_buf.min_nwid);
    tmp->min_nwid = adf_os_ntohl(range_buf.min_nwid);
    tmp->old_num_channels = adf_os_ntohs(range_buf.old_num_channels);
    tmp->old_num_frequency = range_buf.old_num_frequency;
#if WIRELESS_EXT >= 17
    for (i = 0; i < 6; i++)
        tmp->event_capa[i] = adf_os_ntohl(range_buf.event_capa[i]);
#endif
    tmp->sensitivity = adf_os_ntohl(range_buf.sensitivity);
    tmp->max_qual.qual = range_buf.max_qual.qual;
    tmp->max_qual.level = range_buf.max_qual.level;
    tmp->max_qual.noise = range_buf.max_qual.noise;
    tmp->max_qual.updated = range_buf.max_qual.updated;
    tmp->avg_qual.qual = range_buf.avg_qual.qual;
    tmp->avg_qual.level = range_buf.avg_qual.level;
    tmp->avg_qual.noise = range_buf.avg_qual.noise;
    tmp->avg_qual.updated = range_buf.avg_qual.updated;
    tmp->num_bitrates = range_buf.num_bitrates;
    for (i = 0; i < ACFG_MAX_BITRATE; i++)
        tmp->bitrate[i] = adf_os_ntohl(range_buf.bitrate[i]);
    tmp->min_rts = adf_os_ntohl(range_buf.min_rts);
    tmp->max_rts = adf_os_ntohl(range_buf.max_rts);
    tmp->min_frag = adf_os_ntohl(range_buf.min_frag);
    tmp->max_frag = adf_os_ntohl(range_buf.max_frag);
    tmp->min_pmp = adf_os_ntohl(range_buf.min_pmp);
    tmp->max_pmp = adf_os_ntohl(range_buf.max_pmp);
    tmp->min_pmt = adf_os_ntohl(range_buf.min_pmt);
    tmp->max_pmt = adf_os_ntohl(range_buf.max_pmt);
    tmp->pmp_flags = adf_os_ntohs(range_buf.pmp_flags);
    tmp->pmt_flags = adf_os_ntohs(range_buf.pmt_flags);
    tmp->pm_capa = adf_os_ntohs(range_buf.pm_capa);
    for (i = 0; i < ACFG_MAX_ENCODING; i++)
        tmp->encoding_size[i] = adf_os_ntohs(range_buf.encoding_size[i]);
    tmp->num_encoding_sizes = range_buf.num_encoding_sizes;
    tmp->max_encoding_tokens = range_buf.max_encoding_tokens;
    tmp->encoding_login_index = range_buf.encoding_login_index;
    tmp->txpower_capa = adf_os_ntohs(range_buf.txpower_capa);
    tmp->num_txpower = range_buf.num_txpower;
#if WIRELESS_EXT >= 10
    for (i = 0; i < ACFG_MAX_TXPOWER; i++)
        tmp->txpower[i] = adf_os_ntohl(range_buf.txpower[i]);
#endif
    tmp->we_version_compiled = range_buf.we_version_compiled;
    tmp->we_version_source = range_buf.we_version_source;
    tmp->retry_capa = adf_os_ntohs(range_buf.retry_capa);
    tmp->retry_flags = adf_os_ntohs(range_buf.retry_flags);
    tmp->r_time_flags = adf_os_ntohs(range_buf.r_time_flags);
    tmp->min_retry = adf_os_ntohl(range_buf.min_retry);
    tmp->max_retry = adf_os_ntohl(range_buf.max_retry);
    tmp->min_r_time = adf_os_ntohl(range_buf.min_r_time);
    tmp->max_r_time = adf_os_ntohl(range_buf.max_r_time);
    tmp->num_channels = adf_os_ntohs(range_buf.num_channels);
    tmp->num_frequency = range_buf.num_frequency;
    for (i = 0; i < ACFG_MAX_FREQ; i++) {
        tmp->freq[i].m = adf_os_ntohl(range_buf.freq[i].m);
        tmp->freq[i].e = adf_os_ntohs(range_buf.freq[i].e);
        tmp->freq[i].i = range_buf.freq[i].i;
        tmp->freq[i].flags = range_buf.freq[i].flags;
    }
#if WIRELESS_EXT >= 18
    tmp->enc_capa = adf_os_ntohl(range_buf.enc_capa);
#endif

    atd_trace(ATD_DEBUG_CFG, (" RESULT "));
    return status;

fail:    
    atd_trace( (ATD_DEBUG_FUNCTRACE|ATD_DEBUG_CFG), (" End ... \n"));
    return status;
}


/** 
 * @brief Set Phymode
 * 
 * @param hdl
 * @param mode
 * 
 * @return 
 */
static a_status_t
atd_cfg_set_phymode(adf_drv_handle_t  hdl, acfg_phymode_t  mode)
{
    atd_host_vap_t    *vap    = (atd_host_vap_t * )hdl;
    atd_host_wifi_t   *wifi   = vap->wifisc;
    a_status_t         status = A_STATUS_OK;
    adf_nbuf_t         nbuf;
    acfg_phymode_t *ptr_mode ;

    atd_trace(ATD_DEBUG_FUNCTRACE, ("Start"));

    if ((nbuf = _atd_wmi_msg_alloc(__wifi2wmi(wifi))) == NULL) {
        atd_trace(ATD_DEBUG_CFG, ("Unable to allocate wmi buffer \n"));
        status = A_STATUS_ENOMEM;
        goto fail ;
    }

    ptr_mode = (acfg_phymode_t *)atd_prep_cfg_hdr(nbuf, \
                                sizeof(acfg_phymode_t), wifi, vap);

    *ptr_mode = adf_os_htonl(mode);

    status = wmi_cmd_send(__wifi2wmi(wifi),WMI_CMD_PHYMODE_SET, nbuf, NULL, 0);
    if(status != A_STATUS_OK){
        atd_trace(ATD_DEBUG_CFG, (" wmi_cmd_send Failed stat = %x\n", status));
        status =  A_STATUS_EINVAL;
        goto fail;
    }

fail:
    atd_trace( ATD_DEBUG_FUNCTRACE, (" End ... \n"));
    return status;
}

/**
 * @brief Get Phymode
 *
 * @param hdl
 * @param mode
 *
 * @return
 */
static a_status_t
atd_cfg_get_phymode(adf_drv_handle_t  hdl, acfg_phymode_t *mode)
{
    atd_host_vap_t    *vap    = (atd_host_vap_t * )hdl;
    atd_host_wifi_t   *wifi   = vap->wifisc;
    a_status_t         status = A_STATUS_OK;
    adf_nbuf_t         nbuf;
    acfg_phymode_t     resp = 0;

    atd_trace(ATD_DEBUG_FUNCTRACE, ("Start"));

    if ((nbuf = _atd_wmi_msg_alloc(__wifi2wmi(wifi))) == NULL) {
        atd_trace(ATD_DEBUG_CFG, ("Unable to allocate wmi buffer \n"));
        status = A_STATUS_ENOMEM;
        goto fail ;
    }

    atd_prep_cfg_hdr(nbuf, 0, wifi, vap);

    status = wmi_cmd_send(__wifi2wmi(wifi),WMI_CMD_PHYMODE_GET, nbuf,
            (a_uint8_t *)&resp,
            sizeof(acfg_phymode_t));

    if(status != A_STATUS_OK){
        atd_trace(ATD_DEBUG_CFG, (" wmi_cmd_send Failed stat = %x\n", status));
        status =  A_STATUS_EINVAL;
        goto fail;
    }

    *mode = adf_os_ntohl(resp);

fail:
    atd_trace( ATD_DEBUG_FUNCTRACE, (" End ... \n"));
    return status;
}

/** 
 * @brief Get info on associated stations.
 * 
 * @param hdl
 * @param buff
 * @param buflen
 * 
 * @return 
 */
static a_status_t 
atd_cfg_get_stainfo(adf_drv_handle_t hdl, a_uint8_t *buff, a_uint32_t buflen)
{
    atd_host_vap_t  *vap    = (atd_host_vap_t * )hdl;
    atd_host_wifi_t *wifi   = vap->wifisc;
    a_status_t       status = A_STATUS_OK;
    adf_nbuf_t       nbuf;
    a_uint32_t       hostlen;
    a_uint32_t      *pld;
    void            *resp_buff = NULL;
    acfg_stainfo_t  *resp;
    atd_stainfo_t   *si;
    a_uint32_t       i, count = 0;
    u_int8_t        *pos, *end, *next;

    atd_trace(ATD_DEBUG_FUNCTRACE, ("Start"));

    resp_buff = adf_os_mem_alloc(NULL, buflen);
    if (!resp_buff) {
        atd_trace(ATD_DEBUG_ERROR, ("Unable to allocate response buffer \n"));
        status = A_STATUS_ENOMEM;
        goto fail;
    }

    if ((nbuf = _atd_wmi_msg_alloc(__wifi2wmi(wifi))) == NULL) {
        atd_trace(ATD_DEBUG_CFG, ("Unable to allocate wmi buffer \n"));
        status = A_STATUS_ENOMEM;
        goto fail_free;
    }

    pld = (a_uint32_t *)atd_prep_cfg_hdr(nbuf, sizeof(a_uint32_t), wifi, vap);

    *pld = adf_os_htonl(buflen);
    atd_trace(ATD_DEBUG_CFG, ("User sent buflen = %d\n",buflen));

    status = wmi_cmd_send(__wifi2wmi(wifi), \
            WMI_CMD_STA_INFO_GET, nbuf, resp_buff, buflen);

    if(status != A_STATUS_OK){
        atd_trace(ATD_DEBUG_CFG, (" wmi_cmd_send Failed stat = %x\n", status));
        status =  A_STATUS_EINVAL;
        goto fail_free;
    }

    hostlen = adf_os_ntohl(*(a_uint32_t *)resp_buff);
    pos = (a_uint8_t *)resp_buff + sizeof(a_uint32_t);
    end = (pos + hostlen);
    resp = (acfg_stainfo_t *)(buff + sizeof(a_uint32_t));
    while (pos + sizeof(atd_stainfo_t) <= end) {
        si = (atd_stainfo_t *)pos;

        next = (pos + sizeof(*si) + adf_os_ntohs(si->isi_len));
        if (next > end)
            break;

        resp->isi_len = sizeof(*resp) + adf_os_ntohs(si->isi_len);
        resp->isi_freq = adf_os_ntohs(si->isi_freq);
        resp->isi_flags = adf_os_ntohl(si->isi_flags);
        resp->isi_state = adf_os_ntohs(si->isi_state);
        resp->isi_authmode = si->isi_authmode;
        resp->isi_rssi = si->isi_rssi;
        resp->isi_capinfo = adf_os_ntohs(si->isi_capinfo);
        resp->isi_athflags = si->isi_athflags;
        resp->isi_erp = si->isi_erp;
        resp->isi_ps = si->isi_ps;
        adf_os_mem_copy(resp->isi_macaddr, si->isi_macaddr, ACFG_MACADDR_LEN);
        resp->isi_nrates = si->isi_nrates;
        adf_os_mem_copy(resp->isi_rates, si->isi_rates, 36);
        resp->isi_txrate = si->isi_txrate;
        resp->isi_txratekbps = adf_os_ntohl(si->isi_txratekbps);
        resp->isi_ie_len = adf_os_ntohs(si->isi_ie_len);
        resp->isi_associd = adf_os_ntohs(si->isi_associd);
        resp->isi_txpower = adf_os_ntohs(si->isi_txpower);
        resp->isi_vlan = adf_os_ntohs(si->isi_vlan);
        for (i = 0 ; i < 17; i++)
            resp->isi_txseqs[i] = adf_os_ntohs(si->isi_txseqs[i]);
        for (i = 0 ; i < 17; i++)
            resp->isi_rxseqs[i] = adf_os_ntohs(si->isi_rxseqs[i]);
        resp->isi_inact = adf_os_ntohs(si->isi_inact);
        resp->isi_uapsd = si->isi_uapsd;
        resp->isi_opmode = si->isi_opmode;
        resp->isi_cipher = si->isi_cipher;
        resp->isi_assoc_time = adf_os_ntohl(si->isi_assoc_time);
        resp->isi_tr069_assoc_time_sec = adf_os_ntohl(si->isi_tr069_assoc_time_sec);
        resp->isi_tr069_assoc_time_nsec = adf_os_ntohl(si->isi_tr069_assoc_time_nsec);
        resp->isi_htcap = adf_os_ntohs(si->isi_htcap);
        resp->isi_rxratekbps = adf_os_ntohl(si->isi_rxratekbps);
        resp->isi_maxrate_per_client = si->isi_maxrate_per_client;
        resp->isi_stamode = adf_os_ntohs(si->isi_stamode);
        memcpy(resp + 1, si + 1, adf_os_ntohs(si->isi_len));

        count += sizeof(*resp) + adf_os_ntohs(si->isi_len);
        resp = (acfg_stainfo_t *)((a_uint8_t *)(resp + 1) + 
                                  (adf_os_ntohs(si->isi_len)));
        pos = next;
    }
    *(a_uint32_t *)buff = count +12;

    atd_trace(ATD_DEBUG_FUNCTRACE, \
            ("Returned length = %d",*(a_uint32_t *)buff));
fail_free:
    adf_os_mem_free(resp_buff);

fail:
    atd_trace( ATD_DEBUG_FUNCTRACE, (" End ... \n"));
    return status;
}


/** 
 * @brief Set MLME
 * 
 * @param hdl
 * @param mode
 * 
 * @return 
 */
a_status_t 
atd_cfg_set_mlme(adf_drv_handle_t hdl, acfg_mlme_t  *mlme)
{
    atd_host_vap_t     *vap    = (atd_host_vap_t * )hdl;
    atd_host_wifi_t    *wifi   = vap->wifisc;
    atd_mlme_t         *pld;
    a_status_t          status = A_STATUS_OK;
    adf_nbuf_t          nbuf;

    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("mlme len "));

    if ((nbuf = _atd_wmi_msg_alloc(__wifi2wmi(wifi))) == NULL) {
        atd_trace(ATD_DEBUG_CFG, ("Unable to allocate wmi buffer \n"));
        status = A_STATUS_ENOMEM;
        goto fail ;
    }

    pld  = (atd_mlme_t *)atd_prep_cfg_hdr(nbuf, sizeof(atd_mlme_t), wifi, vap);

    pld->op = mlme->op;
    pld->ssid_len = mlme->ssid_len;
    pld->reason = adf_os_htons(mlme->reason);
    pld->seq = adf_os_htons(mlme->seq);
    adf_os_mem_copy(pld->macaddr, mlme->macaddr, ACFG_MACADDR_LEN);
    adf_os_mem_copy(pld->ssid, mlme->ssid, ACFG_MAX_SSID_LEN);
    adf_os_mem_copy(pld->optie, mlme->optie, ACFG_MAX_IELEN * 2);
    pld->optie_len = adf_os_htons(mlme->optie_len);

    status = wmi_cmd_send(__wifi2wmi(wifi), WMI_CMD_MLME_SET, nbuf,
                          NULL, 0);

fail:
    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("End stat %x ",status ));
    return status;
}



/** 
 * @brief Send MGMT frame
 * 
 * @param hdl
 * @param mode
 * 
 * @return 
 */
a_status_t 
atd_cfg_send_mgmt(adf_drv_handle_t hdl, acfg_mgmt_t  *mgmt)
{
    atd_host_vap_t     *vap    = (atd_host_vap_t * )hdl;
    atd_host_wifi_t    *wifi   = vap->wifisc;
    atd_mgmt_t         *pld;
    a_status_t          status = A_STATUS_OK;
    adf_nbuf_t          nbuf;

    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("mgmt "));

    if (mgmt->buflen + sizeof(atd_mgmt_t) + sizeof(atd_hdr_t) > WMI_DEF_MSG_LEN) {
        adf_os_print("buffer to big to fit in wmi \n");
        goto fail;
    }

    if ((nbuf = _atd_wmi_msg_alloc(__wifi2wmi(wifi))) == NULL) {
        atd_trace(ATD_DEBUG_CFG, ("Unable to allocate wmi buffer \n"));
        status = A_STATUS_ENOMEM;
        goto fail ;
    }

    pld = (atd_mgmt_t *)atd_prep_cfg_hdr(nbuf, 
                        sizeof(atd_mgmt_t) + mgmt->buflen, wifi, vap);
    if (pld == NULL) {
        status = A_STATUS_ENOMEM;
        goto fail;
    }
    adf_os_mem_copy(pld->macaddr, mgmt->macaddr, ACFG_MACADDR_LEN);
    pld->buflen = adf_os_htonl(mgmt->buflen);
    adf_os_mem_copy(pld->buf, mgmt->buf, mgmt->buflen);

    status = wmi_cmd_send(__wifi2wmi(wifi), WMI_CMD_SEND_MGMT, nbuf,
                          NULL, 0);

fail:
    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("End stat %x ",status ));
    return status;
}



/** 
 * @brief Set Opt IE
 * 
 * @param hdl
 * @param mode
 * 
 * @return 
 */
a_status_t 
atd_cfg_set_optie(adf_drv_handle_t hdl, acfg_ie_t  *ie)
{
    atd_host_vap_t     *vap    = (atd_host_vap_t * )hdl;
    atd_host_wifi_t    *wifi   = vap->wifisc;
    void               *pie;
    a_status_t          status = A_STATUS_OK;
    adf_nbuf_t          nbuf;

    /*Error Check*/
    if(ie->len <  0){
        atd_trace(ATD_DEBUG_CFG, ("Invalid IE length\n"));
        status = A_STATUS_EINVAL;
        goto fail;
    }

    if ((nbuf = _atd_wmi_msg_alloc(__wifi2wmi(wifi))) == NULL) {
        atd_trace(ATD_DEBUG_CFG, ("Unable to allocate wmi buffer \n"));
        status = A_STATUS_ENOMEM;
        goto fail ;
    }

    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), 
                                ("IE len - %d ",ie->len));

    pie  = atd_prep_cfg_hdr(nbuf, ie->len + sizeof(a_uint32_t),
                                                 wifi, vap);

    *(a_uint32_t *)pie = adf_os_htonl(ie->len);
    adf_os_mem_copy(((a_uint8_t *)pie + sizeof(a_uint32_t)), ie->data, ie->len);

    status = wmi_cmd_send(__wifi2wmi(wifi), WMI_CMD_OPTIE_SET, nbuf,
                          NULL, 0);

fail:
    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG),
                                ("End status %x ",status ));
    return status;
}



/** 
 * @brief Get wpa ie about associating station
 * 
 * @param hdl
 * @param buff
 * @param buflen
 * 
 * @return 
 */
static a_status_t
atd_cfg_get_wpa_ie(adf_drv_handle_t hdl, a_uint8_t *buff, a_uint32_t buflen)
{

    atd_host_vap_t    *vap     = (atd_host_vap_t * )hdl;
    atd_host_wifi_t     *wifi  = vap->wifisc;
    a_status_t         status  = A_STATUS_OK;
    adf_nbuf_t         nbuf;
    void              *ptr_buflen;

    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("wpaie = %p", buff ));

    if ((nbuf = _atd_wmi_msg_alloc(__wifi2wmi(wifi))) == NULL) {
        atd_trace(ATD_DEBUG_CFG, ("Unable to allocate wmi buffer \n"));
        status = A_STATUS_ENOMEM;
        goto fail ;
    }

    ptr_buflen = (a_uint32_t *)atd_prep_cfg_hdr(nbuf, \
                                VALID_PAYLOAD_SIZE + sizeof(a_uint32_t), wifi, vap);

    *(a_uint32_t *)ptr_buflen = adf_os_htonl(buflen);

    atd_trace(ATD_DEBUG_CFG, ("User sent buflen = %d\n",buflen));

    memcpy(((a_uint8_t *)ptr_buflen) + sizeof(a_uint32_t), (a_uint8_t *)buff, VALID_PAYLOAD_SIZE);

    status      = wmi_cmd_send(__wifi2wmi(wifi), WMI_CMD_WPA_IE_GET, nbuf,
                          buff, buflen);

    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("wpaie   %p ", buff));

fail:
    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("End stat %x ",status ));
    return status;
}


/** 
 * @brief Set AC Params
 * 
 * @param hdl
 * @param filterframe
 * 
 * @return 
 */
a_status_t 
atd_cfg_set_acparams(adf_drv_handle_t hdl, a_uint32_t *ac)
{
    atd_host_vap_t     *vap    = (atd_host_vap_t * )hdl;
    atd_host_wifi_t    *wifi   = vap->wifisc;
    a_status_t          status = A_STATUS_OK;
    adf_nbuf_t          nbuf;
    a_uint32_t i, *pac;

    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("set AC params"));

    if ((nbuf = _atd_wmi_msg_alloc(__wifi2wmi(wifi))) == NULL) {
        atd_trace(ATD_DEBUG_CFG, ("Unable to allocate wmi buffer \n"));
        status = A_STATUS_ENOMEM;
        goto fail ;
    }

    pac = (a_uint32_t *)atd_prep_cfg_hdr(nbuf, ACFG_MAX_ACPARAMS * sizeof(a_uint32_t), wifi, vap);

    for(i=0; i < ACFG_MAX_ACPARAMS; i++)
        pac[i] = adf_os_htonl(ac[i]) ;
    
    status = wmi_cmd_send(__wifi2wmi(wifi), WMI_CMD_ACPARAMS_SET, nbuf,
                          NULL, 0);

fail:
    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("End stat %x ",status ));
    return status;
}


/** 
 * @brief Set Filterframe
 * 
 * @param hdl
 * @param filterframe
 * 
 * @return 
 */
a_status_t 
atd_cfg_set_filterframe(adf_drv_handle_t hdl, acfg_filter_t  *filterframe)
{
    atd_host_vap_t     *vap    = (atd_host_vap_t * )hdl;
    atd_host_wifi_t    *wifi   = vap->wifisc;
    atd_filter_t       *pld;
    a_status_t          status = A_STATUS_OK;
    adf_nbuf_t          nbuf;

    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("filter frame "));

    if ((nbuf = _atd_wmi_msg_alloc(__wifi2wmi(wifi))) == NULL) {
        atd_trace(ATD_DEBUG_CFG, ("Unable to allocate wmi buffer \n"));
        status = A_STATUS_ENOMEM;
        goto fail ;
    }

    pld = (atd_filter_t *)atd_prep_cfg_hdr(nbuf, sizeof(atd_filter_t), wifi, vap);

    pld->filter = adf_os_htonl(filterframe->filter);

    status = wmi_cmd_send(__wifi2wmi(wifi), WMI_CMD_FILTERFRAME_SET, nbuf,
                          NULL, 0);

fail:
    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("End stat %x ",status ));
    return status;
}

/**
* @brief Send dbgreqframe
*
* @param hdl
* @param filterframe
*
* @return
*/
a_status_t
atd_cfg_dbgreq(adf_drv_handle_t hdl, acfg_athdbg_req_t  *dbgreq)
{
    atd_host_vap_t      *vap    = (atd_host_vap_t * )hdl;
    atd_host_wifi_t     *wifi   = vap->wifisc;
    a_status_t           status = A_STATUS_OK;
    adf_nbuf_t           nbuf;
    atd_athdbg_req_t    *pld;
    a_uint32_t           i, j;
    a_uint32_t           len = sizeof(atd_athdbg_req_t);
    a_uint32_t           resp_len = sizeof(atd_athdbg_req_t);
    atd_athdbg_req_t    *resp_buf = NULL;
    atd_rrmstats_t      *rrm_stats;
    acfg_rrmstats_t     *rrmstats;
    atd_bcnrpt_t        *bcn_rpt;
    acfg_bcnrpt_t       *bcnrpt;
    atd_acs_dbg_t       *acs_dbg;
    acfg_acs_dbg_t      *acsdbg;

    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("dbgreq frame "));

    switch (dbgreq->cmd) {
        case ACFG_DBGREQ_GETRRMSTATS:
            len += sizeof(atd_rrmstats_t);
            resp_len += sizeof(atd_rrmstats_t);
            break;
        case ACFG_DBGREQ_GETBCNRPT:
            len += sizeof(atd_bcnrpt_t);
            resp_len += sizeof(atd_bcnrpt_t);
            break;
        case ACFG_DBGREQ_GETACSREPORT:
            len += sizeof(atd_acs_dbg_t);
            resp_len += sizeof(atd_acs_dbg_t);
            break;
    }

    resp_buf = (atd_athdbg_req_t *)adf_os_mem_alloc(NULL, resp_len);
    if (resp_buf == NULL) {
        atd_trace(ATD_DEBUG_CFG, ("Memory allocation failed \n"));
        status = A_STATUS_ENOMEM;
        goto fail;
    }

    if ((nbuf = _atd_wmi_msg_alloc(__wifi2wmi(wifi))) == NULL) {
        atd_trace(ATD_DEBUG_CFG, ("Unable to allocate wmi buffer \n"));
        status = A_STATUS_ENOMEM;
        goto fail;
    }


    pld = (atd_athdbg_req_t *)atd_prep_cfg_hdr(nbuf, len, wifi, vap);

    pld->cmd = dbgreq->cmd;
    adf_os_mem_copy(pld->dstmac, dbgreq->dstmac, ACFG_MACADDR_LEN);
    switch (dbgreq->cmd) {
        case ACFG_DBGREQ_SENDADDBA:
        case ACFG_DBGREQ_SENDDELBA:
        case ACFG_DBGREQ_SETADDBARESP:
        case ACFG_DBGREQ_GETADDBASTATS:
        case ACFG_DBGREQ_SENDSINGLEAMSDU:
            pld->data.param[0] = adf_os_htonl(dbgreq->data.param[0]);
            pld->data.param[1] = adf_os_htonl(dbgreq->data.param[1]);
            pld->data.param[2] = adf_os_htonl(dbgreq->data.param[2]);
            pld->data.param[3] = adf_os_htonl(dbgreq->data.param[3]);
            break;
        case ACFG_DBGREQ_SENDBCNRPT:
            pld->data.bcnrpt.num_rpt = adf_os_htons(dbgreq->data.bcnrpt.num_rpt);
            pld->data.bcnrpt.regclass = dbgreq->data.bcnrpt.regclass;
            pld->data.bcnrpt.channum = dbgreq->data.bcnrpt.channum;
            pld->data.bcnrpt.random_ivl = adf_os_htons(dbgreq->data.bcnrpt.random_ivl);
            pld->data.bcnrpt.duration = adf_os_htons(dbgreq->data.bcnrpt.duration);
            pld->data.bcnrpt.reqmode = dbgreq->data.bcnrpt.reqmode;
            pld->data.bcnrpt.reqtype = dbgreq->data.bcnrpt.reqtype;
            adf_os_mem_copy(pld->data.bcnrpt.bssid, dbgreq->data.bcnrpt.bssid, 6);
            pld->data.bcnrpt.mode = dbgreq->data.bcnrpt.mode;
            pld->data.bcnrpt.req_ssid = dbgreq->data.bcnrpt.req_ssid;
            pld->data.bcnrpt.rep_cond = dbgreq->data.bcnrpt.rep_cond;
            pld->data.bcnrpt.rep_thresh = dbgreq->data.bcnrpt.rep_thresh;
            pld->data.bcnrpt.rep_detail = dbgreq->data.bcnrpt.rep_detail;
            pld->data.bcnrpt.req_ie = dbgreq->data.bcnrpt.req_ie;
            pld->data.bcnrpt.num_chanrep = dbgreq->data.bcnrpt.num_chanrep;
            for (i = 0; i < 2; i++) {
                pld->data.bcnrpt.apchanrep[i].regclass = dbgreq->data.bcnrpt.apchanrep[i].regclass;
                pld->data.bcnrpt.apchanrep[i].numchans = dbgreq->data.bcnrpt.apchanrep[i].numchans;
                for (j = 0; j < 5; j++)
                    pld->data.bcnrpt.apchanrep[i].channum[j] = dbgreq->data.bcnrpt.apchanrep[i].channum[j];
            }
            break;
        case ACFG_DBGREQ_SENDTSMRPT:
            pld->data.tsmrpt.num_rpt = adf_os_htons(dbgreq->data.tsmrpt.num_rpt);
            pld->data.tsmrpt.rand_ivl = adf_os_htons(dbgreq->data.tsmrpt.rand_ivl);
            pld->data.tsmrpt.meas_dur = adf_os_htons(dbgreq->data.tsmrpt.meas_dur);
            pld->data.tsmrpt.reqmode = dbgreq->data.tsmrpt.reqmode;
            pld->data.tsmrpt.reqtype = dbgreq->data.tsmrpt.reqtype;
            pld->data.tsmrpt.tid = dbgreq->data.tsmrpt.tid;
            adf_os_mem_copy(pld->data.tsmrpt.macaddr, dbgreq->data.tsmrpt.macaddr, 6);
            pld->data.tsmrpt.bin0_range = dbgreq->data.tsmrpt.bin0_range;
            pld->data.tsmrpt.trig_cond = dbgreq->data.tsmrpt.trig_cond;
            pld->data.tsmrpt.avg_err_thresh = dbgreq->data.tsmrpt.avg_err_thresh;
            pld->data.tsmrpt.cons_err_thresh = dbgreq->data.tsmrpt.cons_err_thresh;
            pld->data.tsmrpt.delay_thresh = dbgreq->data.tsmrpt.delay_thresh;
            pld->data.tsmrpt.meas_count = dbgreq->data.tsmrpt.meas_count;
            pld->data.tsmrpt.trig_timeout = dbgreq->data.tsmrpt.trig_timeout;
            break;
        case ACFG_DBGREQ_SENDNEIGRPT:
            pld->data.neigrpt.dialogtoken = dbgreq->data.neigrpt.dialogtoken;
            adf_os_mem_copy(pld->data.neigrpt.ssid, dbgreq->data.neigrpt.ssid, 32);
            pld->data.neigrpt.ssid_len = dbgreq->data.neigrpt.ssid_len;
            break;
        case ACFG_DBGREQ_SENDLMREQ:
            break;
        case ACFG_DBGREQ_SENDBSTMREQ:
            pld->data.bstmreq.dialogtoken = dbgreq->data.bstmreq.dialogtoken;
            pld->data.bstmreq.candidate_list = dbgreq->data.bstmreq.candidate_list;
            pld->data.bstmreq.disassoc = dbgreq->data.bstmreq.disassoc;
            pld->data.bstmreq.disassoc_timer = adf_os_htons(dbgreq->data.bstmreq.disassoc_timer);
            pld->data.bstmreq.validity_itvl = dbgreq->data.bstmreq.validity_itvl;
            break;
        case ACFG_DBGREQ_SENDCHLOADREQ:
            adf_os_mem_copy(pld->data.chloadrpt.dstmac, dbgreq->data.chloadrpt.dstmac, 6);
            pld->data.chloadrpt.num_rpts = adf_os_htons(dbgreq->data.chloadrpt.num_rpts);
            pld->data.chloadrpt.regclass = dbgreq->data.chloadrpt.regclass;
            pld->data.chloadrpt.chnum = dbgreq->data.chloadrpt.chnum;
            pld->data.chloadrpt.r_invl = adf_os_htons(dbgreq->data.chloadrpt.r_invl);
            pld->data.chloadrpt.m_dur = adf_os_htons(dbgreq->data.chloadrpt.m_dur);
            pld->data.chloadrpt.cond = dbgreq->data.chloadrpt.cond;
            pld->data.chloadrpt.c_val = dbgreq->data.chloadrpt.c_val;
            break;
        case ACFG_DBGREQ_SENDSTASTATSREQ:
            adf_os_mem_copy(pld->data.stastats.dstmac, dbgreq->data.stastats.dstmac, 6);
            pld->data.stastats.num_rpts = adf_os_htons(dbgreq->data.stastats.num_rpts);
            pld->data.stastats.m_dur = adf_os_htons(dbgreq->data.stastats.m_dur);
            pld->data.stastats.r_invl = adf_os_htons(dbgreq->data.stastats.r_invl);
            pld->data.stastats.gid = dbgreq->data.stastats.gid;
            break;
        case ACFG_DBGREQ_SENDNHIST:
            pld->data.nhist.num_rpts = adf_os_htons(dbgreq->data.nhist.num_rpts);
            adf_os_mem_copy(pld->data.nhist.dstmac, dbgreq->data.nhist.dstmac, 6);
            pld->data.nhist.regclass = dbgreq->data.nhist.regclass;
            pld->data.nhist.chnum = dbgreq->data.nhist.chnum;
            pld->data.nhist.r_invl = adf_os_htons(dbgreq->data.nhist.r_invl);
            pld->data.nhist.m_dur = adf_os_htons(dbgreq->data.nhist.m_dur);
            pld->data.nhist.cond = dbgreq->data.nhist.cond;
            pld->data.nhist.c_val = dbgreq->data.nhist.c_val;
            break;
        case ACFG_DBGREQ_SENDDELTS:
            pld->data.param[0] = adf_os_htonl(dbgreq->data.param[0]);
            break;
        case ACFG_DBGREQ_SENDADDTSREQ:
            pld->data.tsinfo.traffic_type = dbgreq->data.tsinfo.traffic_type;
            pld->data.tsinfo.direction = dbgreq->data.tsinfo.direction;
            pld->data.tsinfo.dot1Dtag = dbgreq->data.tsinfo.dot1Dtag;
            pld->data.tsinfo.tid = dbgreq->data.tsinfo.tid;
            pld->data.tsinfo.acc_policy_edca = dbgreq->data.tsinfo.acc_policy_edca;
            pld->data.tsinfo.acc_policy_hcca = dbgreq->data.tsinfo.acc_policy_hcca;
            pld->data.tsinfo.aggregation = dbgreq->data.tsinfo.aggregation;
            pld->data.tsinfo.psb = dbgreq->data.tsinfo.psb;
            pld->data.tsinfo.ack_policy = dbgreq->data.tsinfo.ack_policy;
            pld->data.tsinfo.norminal_msdu_size = adf_os_htons(dbgreq->data.tsinfo.norminal_msdu_size);
            pld->data.tsinfo.max_msdu_size = adf_os_htons(dbgreq->data.tsinfo.max_msdu_size);
            pld->data.tsinfo.min_srv_interval = adf_os_htonl(dbgreq->data.tsinfo.min_srv_interval);
            pld->data.tsinfo.max_srv_interval = adf_os_htonl(dbgreq->data.tsinfo.max_srv_interval);
            pld->data.tsinfo.inactivity_interval = adf_os_htonl(dbgreq->data.tsinfo.inactivity_interval);
            pld->data.tsinfo.suspension_interval = adf_os_htonl(dbgreq->data.tsinfo.suspension_interval);
            pld->data.tsinfo.srv_start_time = adf_os_htonl(dbgreq->data.tsinfo.srv_start_time);
            pld->data.tsinfo.min_data_rate = adf_os_htonl(dbgreq->data.tsinfo.min_data_rate);
            pld->data.tsinfo.mean_data_rate = adf_os_htonl(dbgreq->data.tsinfo.mean_data_rate);
            pld->data.tsinfo.peak_data_rate = adf_os_htonl(dbgreq->data.tsinfo.peak_data_rate);
            pld->data.tsinfo.max_burst_size = adf_os_htonl(dbgreq->data.tsinfo.max_burst_size);
            pld->data.tsinfo.delay_bound = adf_os_htonl(dbgreq->data.tsinfo.delay_bound);
            pld->data.tsinfo.min_phy_rate = adf_os_htonl(dbgreq->data.tsinfo.min_phy_rate);
            pld->data.tsinfo.surplus_bw = adf_os_htons(dbgreq->data.tsinfo.surplus_bw);
            pld->data.tsinfo.medium_time = adf_os_htons(dbgreq->data.tsinfo.medium_time);
            break;
        case ACFG_DBGREQ_SENDLCIREQ:
            adf_os_mem_copy(pld->data.lci_req.dstmac, dbgreq->data.lci_req.dstmac, 6);
            pld->data.lci_req.num_rpts = adf_os_htons(dbgreq->data.lci_req.num_rpts);
            pld->data.lci_req.location = dbgreq->data.lci_req.location;
            pld->data.lci_req.lat_res = dbgreq->data.lci_req.lat_res;
            pld->data.lci_req.long_res = dbgreq->data.lci_req.long_res;
            pld->data.lci_req.alt_res = dbgreq->data.lci_req.alt_res;
            pld->data.lci_req.azi_res = dbgreq->data.lci_req.azi_res;
            pld->data.lci_req.azi_type = dbgreq->data.lci_req.azi_type;
            break;
        case ACFG_DBGREQ_GETRRMSTATS:
            pld->data.rrmstats_req.index = adf_os_htonl(dbgreq->data.rrmstats_req.index);
            pld->data.rrmstats_req.data_size = adf_os_htonl(dbgreq->data.rrmstats_req.data_size);
            rrm_stats = (atd_rrmstats_t *)(pld + 1);
            rrmstats = (acfg_rrmstats_t *)(dbgreq->data.rrmstats_req.data_addr);
            for (i = 0; i < ACFG_MAX_IEEE_CHAN; i++) {
                rrm_stats->chann_load[i] = rrmstats->chann_load[i];
                rrm_stats->noise_data[i].antid = rrmstats->noise_data[i].antid;
                rrm_stats->noise_data[i].anpi = rrmstats->noise_data[i].anpi;
                for (j = 0; j < 11; j++)
                    rrm_stats->noise_data[i].ipi[j] = rrmstats->noise_data[i].ipi[j];
            }
            rrm_stats->ni_rrm_stats.gid0.txfragcnt = adf_os_htonl(rrmstats->ni_rrm_stats.gid0.txfragcnt);
            rrm_stats->ni_rrm_stats.gid0.mcastfrmcnt = adf_os_htonl(rrmstats->ni_rrm_stats.gid0.mcastfrmcnt);
            rrm_stats->ni_rrm_stats.gid0.failcnt = adf_os_htonl(rrmstats->ni_rrm_stats.gid0.failcnt);
            rrm_stats->ni_rrm_stats.gid0.rxfragcnt = adf_os_htonl(rrmstats->ni_rrm_stats.gid0.rxfragcnt);
            rrm_stats->ni_rrm_stats.gid0.mcastrxfrmcnt = adf_os_htonl(rrmstats->ni_rrm_stats.gid0.mcastrxfrmcnt);
            rrm_stats->ni_rrm_stats.gid0.fcserrcnt = adf_os_htonl(rrmstats->ni_rrm_stats.gid0.fcserrcnt);
            rrm_stats->ni_rrm_stats.gid0.txfrmcnt = adf_os_htonl(rrmstats->ni_rrm_stats.gid0.txfrmcnt);
            rrm_stats->ni_rrm_stats.gid1.rty = adf_os_htonl(rrmstats->ni_rrm_stats.gid1.rty);
            rrm_stats->ni_rrm_stats.gid1.multirty = adf_os_htonl(rrmstats->ni_rrm_stats.gid1.multirty);
            rrm_stats->ni_rrm_stats.gid1.frmdup = adf_os_htonl(rrmstats->ni_rrm_stats.gid1.frmdup);
            rrm_stats->ni_rrm_stats.gid1.rtsuccess = adf_os_htonl(rrmstats->ni_rrm_stats.gid1.rtsuccess);
            rrm_stats->ni_rrm_stats.gid1.rtsfail = adf_os_htonl(rrmstats->ni_rrm_stats.gid1.rtsfail);
            rrm_stats->ni_rrm_stats.gid1.ackfail = adf_os_htonl(rrmstats->ni_rrm_stats.gid1.ackfail);
            for (i = 0; i < 8; i++) {
                rrm_stats->ni_rrm_stats.gidupx[i].qostxfragcnt = adf_os_htonl(rrmstats->ni_rrm_stats.gidupx[i].qostxfragcnt);
                rrm_stats->ni_rrm_stats.gidupx[i].qosfailedcnt = adf_os_htonl(rrmstats->ni_rrm_stats.gidupx[i].qosfailedcnt);
                rrm_stats->ni_rrm_stats.gidupx[i].qosrtycnt = adf_os_htonl(rrmstats->ni_rrm_stats.gidupx[i].qosrtycnt);
                rrm_stats->ni_rrm_stats.gidupx[i].multirtycnt = adf_os_htonl(rrmstats->ni_rrm_stats.gidupx[i].multirtycnt);
                rrm_stats->ni_rrm_stats.gidupx[i].qosfrmdupcnt = adf_os_htonl(rrmstats->ni_rrm_stats.gidupx[i].qosfrmdupcnt);
                rrm_stats->ni_rrm_stats.gidupx[i].qosrtssuccnt = adf_os_htonl(rrmstats->ni_rrm_stats.gidupx[i].qosrtssuccnt);
                rrm_stats->ni_rrm_stats.gidupx[i].qosrtsfailcnt = adf_os_htonl(rrmstats->ni_rrm_stats.gidupx[i].qosrtsfailcnt);
                rrm_stats->ni_rrm_stats.gidupx[i].qosackfailcnt = adf_os_htonl(rrmstats->ni_rrm_stats.gidupx[i].qosackfailcnt);
                rrm_stats->ni_rrm_stats.gidupx[i].qosrxfragcnt = adf_os_htonl(rrmstats->ni_rrm_stats.gidupx[i].qosrxfragcnt);
                rrm_stats->ni_rrm_stats.gidupx[i].qostxfrmcnt = adf_os_htonl(rrmstats->ni_rrm_stats.gidupx[i].qostxfrmcnt);
                rrm_stats->ni_rrm_stats.gidupx[i].qosdiscadrfrmcnt = adf_os_htonl(rrmstats->ni_rrm_stats.gidupx[i].qosdiscadrfrmcnt);
                rrm_stats->ni_rrm_stats.gidupx[i].qosmpdurxcnt = adf_os_htonl(rrmstats->ni_rrm_stats.gidupx[i].qosmpdurxcnt);
                rrm_stats->ni_rrm_stats.gidupx[i].qosrtyrxcnt = adf_os_htonl(rrmstats->ni_rrm_stats.gidupx[i].qosrtyrxcnt);
            }
            rrm_stats->ni_rrm_stats.gid10.ap_avg_delay = rrmstats->ni_rrm_stats.gid10.ap_avg_delay;
            rrm_stats->ni_rrm_stats.gid10.be_avg_delay = rrmstats->ni_rrm_stats.gid10.be_avg_delay;
            rrm_stats->ni_rrm_stats.gid10.bk_avg_delay = rrmstats->ni_rrm_stats.gid10.bk_avg_delay;
            rrm_stats->ni_rrm_stats.gid10.vi_avg_delay = rrmstats->ni_rrm_stats.gid10.vi_avg_delay;
            rrm_stats->ni_rrm_stats.gid10.vo_avg_delay = rrmstats->ni_rrm_stats.gid10.vo_avg_delay;
            rrm_stats->ni_rrm_stats.gid10.st_cnt = adf_os_htons(rrmstats->ni_rrm_stats.gid10.st_cnt);
            rrm_stats->ni_rrm_stats.gid10.ch_util = rrmstats->ni_rrm_stats.gid10.ch_util;
            rrm_stats->ni_rrm_stats.tsm_data.tid = rrmstats->ni_rrm_stats.tsm_data.tid;
            rrm_stats->ni_rrm_stats.tsm_data.brange = rrmstats->ni_rrm_stats.tsm_data.brange;
            adf_os_mem_copy(rrm_stats->ni_rrm_stats.tsm_data.mac, rrmstats->ni_rrm_stats.tsm_data.mac, 6);
            rrm_stats->ni_rrm_stats.tsm_data.tx_cnt = adf_os_htonl(rrmstats->ni_rrm_stats.tsm_data.tx_cnt);
            rrm_stats->ni_rrm_stats.tsm_data.discnt = adf_os_htonl(rrmstats->ni_rrm_stats.tsm_data.discnt);
            rrm_stats->ni_rrm_stats.tsm_data.multirtycnt = adf_os_htonl(rrmstats->ni_rrm_stats.tsm_data.multirtycnt);
            rrm_stats->ni_rrm_stats.tsm_data.cfpoll = adf_os_htonl(rrmstats->ni_rrm_stats.tsm_data.cfpoll);
            rrm_stats->ni_rrm_stats.tsm_data.qdelay = adf_os_htonl(rrmstats->ni_rrm_stats.tsm_data.qdelay);
            rrm_stats->ni_rrm_stats.tsm_data.txdelay = adf_os_htonl(rrmstats->ni_rrm_stats.tsm_data.txdelay);
            for (i = 0; i < 6; i++)
                rrm_stats->ni_rrm_stats.tsm_data.bin[i] = adf_os_htonl(rrmstats->ni_rrm_stats.tsm_data.bin[i]);
            for (i = 0; i < 12; i++) {
                rrm_stats->ni_rrm_stats.frmcnt_data[i].phytype = rrmstats->ni_rrm_stats.frmcnt_data[i].phytype;
                rrm_stats->ni_rrm_stats.frmcnt_data[i].arcpi = rrmstats->ni_rrm_stats.frmcnt_data[i].arcpi;
                rrm_stats->ni_rrm_stats.frmcnt_data[i].lrsni = rrmstats->ni_rrm_stats.frmcnt_data[i].lrsni;
                rrm_stats->ni_rrm_stats.frmcnt_data[i].lrcpi = rrmstats->ni_rrm_stats.frmcnt_data[i].lrcpi;
                rrm_stats->ni_rrm_stats.frmcnt_data[i].antid = rrmstats->ni_rrm_stats.frmcnt_data[i].antid;
                adf_os_mem_copy(rrm_stats->ni_rrm_stats.frmcnt_data[i].ta, rrmstats->ni_rrm_stats.frmcnt_data[i].ta, 6);
                adf_os_mem_copy(rrm_stats->ni_rrm_stats.frmcnt_data[i].bssid, rrmstats->ni_rrm_stats.frmcnt_data[i].bssid, 6);
                rrm_stats->ni_rrm_stats.frmcnt_data[i].frmcnt = adf_os_htons(rrmstats->ni_rrm_stats.frmcnt_data[i].frmcnt);
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
            rrm_stats->ni_rrm_stats.ni_rrm_lciinfo.lat_integ = adf_os_htons(rrmstats->ni_rrm_stats.ni_rrm_lciinfo.lat_integ);
            rrm_stats->ni_rrm_stats.ni_rrm_lciinfo.long_integ = adf_os_htons(rrmstats->ni_rrm_stats.ni_rrm_lciinfo.long_integ);
            rrm_stats->ni_rrm_stats.ni_rrm_lciinfo.azimuth = adf_os_htons(rrmstats->ni_rrm_stats.ni_rrm_lciinfo.azimuth);
            rrm_stats->ni_rrm_stats.ni_rrm_lciinfo.lat_frac = adf_os_htonl(rrmstats->ni_rrm_stats.ni_rrm_lciinfo.lat_frac);
            rrm_stats->ni_rrm_stats.ni_rrm_lciinfo.long_frac = adf_os_htonl(rrmstats->ni_rrm_stats.ni_rrm_lciinfo.long_frac);
            rrm_stats->ni_rrm_stats.ni_rrm_lciinfo.alt_integ = adf_os_htonl(rrmstats->ni_rrm_stats.ni_rrm_lciinfo.alt_integ);
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
            rrm_stats->ni_rrm_stats.ni_vap_lciinfo.lat_integ = adf_os_htons(rrmstats->ni_rrm_stats.ni_vap_lciinfo.lat_integ);
            rrm_stats->ni_rrm_stats.ni_vap_lciinfo.long_integ = adf_os_htons(rrmstats->ni_rrm_stats.ni_vap_lciinfo.long_integ);
            rrm_stats->ni_rrm_stats.ni_vap_lciinfo.azimuth = adf_os_htons(rrmstats->ni_rrm_stats.ni_vap_lciinfo.azimuth);
            rrm_stats->ni_rrm_stats.ni_vap_lciinfo.lat_frac = adf_os_htonl(rrmstats->ni_rrm_stats.ni_vap_lciinfo.lat_frac);
            rrm_stats->ni_rrm_stats.ni_vap_lciinfo.long_frac = adf_os_htonl(rrmstats->ni_rrm_stats.ni_vap_lciinfo.long_frac);
            rrm_stats->ni_rrm_stats.ni_vap_lciinfo.alt_integ = adf_os_htonl(rrmstats->ni_rrm_stats.ni_vap_lciinfo.alt_integ);
            break;
        case ACFG_DBGREQ_SENDFRMREQ:
            adf_os_mem_copy(pld->data.frm_req.dstmac, dbgreq->data.frm_req.dstmac, 6);
            adf_os_mem_copy(pld->data.frm_req.peermac, dbgreq->data.frm_req.peermac, 6);
            pld->data.frm_req.num_rpts = adf_os_htons(dbgreq->data.frm_req.num_rpts);
            pld->data.frm_req.regclass = dbgreq->data.frm_req.regclass;
            pld->data.frm_req.chnum = dbgreq->data.frm_req.chnum;
            break;
        case ACFG_DBGREQ_GETBCNRPT:
            pld->data.rrmstats_req.index = adf_os_htonl(dbgreq->data.rrmstats_req.index);
            pld->data.rrmstats_req.data_size = adf_os_htonl(dbgreq->data.rrmstats_req.data_size);
            bcn_rpt = (atd_bcnrpt_t *)(pld + 1);
            bcnrpt = (acfg_bcnrpt_t *)(dbgreq->data.rrmstats_req.data_addr);
            adf_os_mem_copy(bcn_rpt->bssid, bcnrpt->bssid, 6);
            bcn_rpt->rsni = bcnrpt->rsni;
            bcn_rpt->rcpi = bcnrpt->rcpi;
            bcn_rpt->chnum = bcnrpt->chnum;
            bcn_rpt->more = bcnrpt->more;
            break;
        case ACFG_DBGREQ_GETRRSSI:
            break;
        case ACFG_DBGREQ_GETACSREPORT:
            pld->data.acs_rep.index = adf_os_htonl(dbgreq->data.acs_rep.index);
            pld->data.acs_rep.data_size = adf_os_htonl(dbgreq->data.acs_rep.data_size);
            acs_dbg = (atd_acs_dbg_t *)(pld + 1);
            acsdbg = (acfg_acs_dbg_t *)(dbgreq->data.acs_rep.data_addr);
            acs_dbg->nchans = acsdbg->nchans;
            acs_dbg->entry_id = acsdbg->entry_id;
            acs_dbg->chan_freq = adf_os_htons(acsdbg->chan_freq);
            acs_dbg->ieee_chan = acsdbg->ieee_chan;
            acs_dbg->chan_nbss = acsdbg->chan_nbss;
            acs_dbg->chan_maxrssi = adf_os_htonl(acsdbg->chan_maxrssi);
            acs_dbg->chan_minrssi = adf_os_htonl(acsdbg->chan_minrssi);
            acs_dbg->noisefloor = adf_os_htons(acsdbg->noisefloor);
            acs_dbg->channel_loading = adf_os_htons(acsdbg->channel_loading);
            acs_dbg->chan_load = adf_os_htonl(acsdbg->chan_load);
            acs_dbg->sec_chan = acsdbg->sec_chan;
            break;
        case ACFG_DBGREQ_BSTEERING_SET_PARAMS:
            pld->data.bst.inactivity_timeout_normal = adf_os_htonl(dbgreq->data.bst.inactivity_timeout_normal);
            pld->data.bst.inactivity_timeout_overload = adf_os_htonl(dbgreq->data.bst.inactivity_timeout_overload);
            pld->data.bst.inactivity_check_period = adf_os_htonl(dbgreq->data.bst.inactivity_check_period);
            pld->data.bst.utilization_sample_period = adf_os_htonl(dbgreq->data.bst.utilization_sample_period);
            pld->data.bst.utilization_average_num_samples = adf_os_htonl(dbgreq->data.bst.utilization_average_num_samples);
            pld->data.bst.inactive_rssi_crossing_threshold = adf_os_htonl(dbgreq->data.bst.inactive_rssi_crossing_threshold);
            pld->data.bst.low_rssi_crossing_threshold = adf_os_htonl(dbgreq->data.bst.low_rssi_crossing_threshold);
            break;
        case ACFG_DBGREQ_BSTEERING_GET_PARAMS:
            break;
        case ACFG_DBGREQ_BSTEERING_SET_DBG_PARAMS:
            pld->data.bst_dbg.raw_log_enable = dbgreq->data.bst_dbg.raw_log_enable;
            break;
        case ACFG_DBGREQ_BSTEERING_GET_DBG_PARAMS:
            break;
        case ACFG_DBGREQ_BSTEERING_ENABLE:
            pld->data.bsteering_enable = dbgreq->data.bsteering_enable;
            break;
        case ACFG_DBGREQ_BSTEERING_SET_OVERLOAD:
            pld->data.bsteering_overload = dbgreq->data.bsteering_overload;
            break;
        case ACFG_DBGREQ_BSTEERING_GET_OVERLOAD:
            break;
        case ACFG_DBGREQ_BSTEERING_GET_RSSI:
            break;
        case ACFG_DBGREQ_BSTEERING_SET_PROBE_RESP_WH:
            pld->data.bsteering_probe_resp_wh = dbgreq->data.bsteering_probe_resp_wh;
            break;
            
        default:
            break;
    }

    status = wmi_cmd_send(__wifi2wmi(wifi), WMI_CMD_DBGREQ, nbuf, (a_uint8_t *)resp_buf, resp_len);
    if (status != A_STATUS_OK) {
        atd_trace(ATD_DEBUG_CFG, (" wmi_cmd_send Failed stat = %x\n", status));
        status = A_STATUS_EINVAL;
        goto fail;
    }

    dbgreq->cmd = resp_buf->cmd;
    adf_os_mem_copy(dbgreq->dstmac, resp_buf->dstmac, ACFG_MACADDR_LEN);
    switch (dbgreq->cmd) {
        case ACFG_DBGREQ_SENDADDBA:
        case ACFG_DBGREQ_SENDDELBA:
        case ACFG_DBGREQ_SETADDBARESP:
        case ACFG_DBGREQ_GETADDBASTATS:
        case ACFG_DBGREQ_SENDSINGLEAMSDU:
            dbgreq->data.param[0] = adf_os_ntohl(resp_buf->data.param[0]);
            dbgreq->data.param[1] = adf_os_ntohl(resp_buf->data.param[1]);
            dbgreq->data.param[2] = adf_os_ntohl(resp_buf->data.param[2]);
            dbgreq->data.param[3] = adf_os_ntohl(resp_buf->data.param[3]);
            break;
        case ACFG_DBGREQ_SENDBCNRPT:
            dbgreq->data.bcnrpt.num_rpt = adf_os_ntohs(resp_buf->data.bcnrpt.num_rpt);
            dbgreq->data.bcnrpt.regclass = resp_buf->data.bcnrpt.regclass;
            dbgreq->data.bcnrpt.channum = resp_buf->data.bcnrpt.channum;
            dbgreq->data.bcnrpt.random_ivl = adf_os_ntohs(resp_buf->data.bcnrpt.random_ivl);
            dbgreq->data.bcnrpt.duration = adf_os_ntohs(resp_buf->data.bcnrpt.duration);
            dbgreq->data.bcnrpt.reqmode = resp_buf->data.bcnrpt.reqmode;
            dbgreq->data.bcnrpt.reqtype = resp_buf->data.bcnrpt.reqtype;
            adf_os_mem_copy(dbgreq->data.bcnrpt.bssid, resp_buf->data.bcnrpt.bssid, 6);
            dbgreq->data.bcnrpt.mode = resp_buf->data.bcnrpt.mode;
            dbgreq->data.bcnrpt.req_ssid = resp_buf->data.bcnrpt.req_ssid;
            dbgreq->data.bcnrpt.rep_cond = resp_buf->data.bcnrpt.rep_cond;
            dbgreq->data.bcnrpt.rep_thresh = resp_buf->data.bcnrpt.rep_thresh;
            dbgreq->data.bcnrpt.rep_detail = resp_buf->data.bcnrpt.rep_detail;
            dbgreq->data.bcnrpt.req_ie = resp_buf->data.bcnrpt.req_ie;
            dbgreq->data.bcnrpt.num_chanrep = resp_buf->data.bcnrpt.num_chanrep;
            for (i = 0; i < 2; i++) {
                dbgreq->data.bcnrpt.apchanrep[i].regclass = resp_buf->data.bcnrpt.apchanrep[i].regclass;
                dbgreq->data.bcnrpt.apchanrep[i].numchans = resp_buf->data.bcnrpt.apchanrep[i].numchans;
                for (j = 0; j < 5; j++)
                    dbgreq->data.bcnrpt.apchanrep[i].channum[j] = resp_buf->data.bcnrpt.apchanrep[i].channum[j];
            }
            break;
        case ACFG_DBGREQ_SENDTSMRPT:
            dbgreq->data.tsmrpt.num_rpt = adf_os_ntohs(resp_buf->data.tsmrpt.num_rpt);
            dbgreq->data.tsmrpt.rand_ivl = adf_os_ntohs(resp_buf->data.tsmrpt.rand_ivl);
            dbgreq->data.tsmrpt.meas_dur = adf_os_ntohs(resp_buf->data.tsmrpt.meas_dur);
            dbgreq->data.tsmrpt.reqmode = resp_buf->data.tsmrpt.reqmode;
            dbgreq->data.tsmrpt.reqtype = resp_buf->data.tsmrpt.reqtype;
            dbgreq->data.tsmrpt.tid = resp_buf->data.tsmrpt.tid;
            adf_os_mem_copy(dbgreq->data.tsmrpt.macaddr, resp_buf->data.tsmrpt.macaddr, 6);
            dbgreq->data.tsmrpt.bin0_range = resp_buf->data.tsmrpt.bin0_range;
            dbgreq->data.tsmrpt.trig_cond = resp_buf->data.tsmrpt.trig_cond;
            dbgreq->data.tsmrpt.avg_err_thresh = resp_buf->data.tsmrpt.avg_err_thresh;
            dbgreq->data.tsmrpt.cons_err_thresh = resp_buf->data.tsmrpt.cons_err_thresh;
            dbgreq->data.tsmrpt.delay_thresh = resp_buf->data.tsmrpt.delay_thresh;
            dbgreq->data.tsmrpt.meas_count = resp_buf->data.tsmrpt.meas_count;
            dbgreq->data.tsmrpt.trig_timeout = resp_buf->data.tsmrpt.trig_timeout;
            break;
        case ACFG_DBGREQ_SENDNEIGRPT:
            dbgreq->data.neigrpt.dialogtoken = resp_buf->data.neigrpt.dialogtoken;
            adf_os_mem_copy(dbgreq->data.neigrpt.ssid, resp_buf->data.neigrpt.ssid, 32);
            dbgreq->data.neigrpt.ssid_len = resp_buf->data.neigrpt.ssid_len;
            break;
        case ACFG_DBGREQ_SENDLMREQ:
            break;
        case ACFG_DBGREQ_SENDBSTMREQ:
            dbgreq->data.bstmreq.dialogtoken = resp_buf->data.bstmreq.dialogtoken;
            dbgreq->data.bstmreq.candidate_list = resp_buf->data.bstmreq.candidate_list;
            dbgreq->data.bstmreq.disassoc = resp_buf->data.bstmreq.disassoc;
            dbgreq->data.bstmreq.disassoc_timer = adf_os_ntohs(resp_buf->data.bstmreq.disassoc_timer);
            dbgreq->data.bstmreq.validity_itvl = resp_buf->data.bstmreq.validity_itvl;
            break;
        case ACFG_DBGREQ_SENDCHLOADREQ:
            adf_os_mem_copy(dbgreq->data.chloadrpt.dstmac, resp_buf->data.chloadrpt.dstmac, 6);
            dbgreq->data.chloadrpt.num_rpts = adf_os_ntohs(resp_buf->data.chloadrpt.num_rpts);
            dbgreq->data.chloadrpt.regclass = resp_buf->data.chloadrpt.regclass;
            dbgreq->data.chloadrpt.chnum = resp_buf->data.chloadrpt.chnum;
            dbgreq->data.chloadrpt.r_invl = adf_os_ntohs(resp_buf->data.chloadrpt.r_invl);
            dbgreq->data.chloadrpt.m_dur = adf_os_ntohs(resp_buf->data.chloadrpt.m_dur);
            dbgreq->data.chloadrpt.cond = resp_buf->data.chloadrpt.cond;
            dbgreq->data.chloadrpt.c_val = resp_buf->data.chloadrpt.c_val;
            break;
        case ACFG_DBGREQ_SENDSTASTATSREQ:
            adf_os_mem_copy(dbgreq->data.stastats.dstmac, resp_buf->data.stastats.dstmac, 6);
            dbgreq->data.stastats.num_rpts = adf_os_ntohs(resp_buf->data.stastats.num_rpts);
            dbgreq->data.stastats.m_dur = adf_os_ntohs(resp_buf->data.stastats.m_dur);
            dbgreq->data.stastats.r_invl = adf_os_ntohs(resp_buf->data.stastats.r_invl);
            dbgreq->data.stastats.gid = resp_buf->data.stastats.gid;
            break;
        case ACFG_DBGREQ_SENDNHIST:
            dbgreq->data.nhist.num_rpts = adf_os_ntohs(resp_buf->data.nhist.num_rpts);
            adf_os_mem_copy(dbgreq->data.nhist.dstmac, resp_buf->data.nhist.dstmac, 6);
            dbgreq->data.nhist.regclass = resp_buf->data.nhist.regclass;
            dbgreq->data.nhist.chnum = resp_buf->data.nhist.chnum;
            dbgreq->data.nhist.r_invl = adf_os_ntohs(resp_buf->data.nhist.r_invl);
            dbgreq->data.nhist.m_dur = adf_os_ntohs(resp_buf->data.nhist.m_dur);
            dbgreq->data.nhist.cond = resp_buf->data.nhist.cond;
            dbgreq->data.nhist.c_val = resp_buf->data.nhist.c_val;
            break;
        case ACFG_DBGREQ_SENDDELTS:
            dbgreq->data.param[0] = adf_os_ntohl(resp_buf->data.param[0]);
            break;
        case ACFG_DBGREQ_SENDADDTSREQ:
            dbgreq->data.tsinfo.traffic_type = resp_buf->data.tsinfo.traffic_type;
            dbgreq->data.tsinfo.direction = resp_buf->data.tsinfo.direction;
            dbgreq->data.tsinfo.dot1Dtag = resp_buf->data.tsinfo.dot1Dtag;
            dbgreq->data.tsinfo.tid = resp_buf->data.tsinfo.tid;
            dbgreq->data.tsinfo.acc_policy_edca = resp_buf->data.tsinfo.acc_policy_edca;
            dbgreq->data.tsinfo.acc_policy_hcca = resp_buf->data.tsinfo.acc_policy_hcca;
            dbgreq->data.tsinfo.aggregation = resp_buf->data.tsinfo.aggregation;
            dbgreq->data.tsinfo.psb = resp_buf->data.tsinfo.psb;
            dbgreq->data.tsinfo.ack_policy = resp_buf->data.tsinfo.ack_policy;
            dbgreq->data.tsinfo.norminal_msdu_size = adf_os_ntohs(resp_buf->data.tsinfo.norminal_msdu_size);
            dbgreq->data.tsinfo.max_msdu_size = adf_os_ntohs(resp_buf->data.tsinfo.max_msdu_size);
            dbgreq->data.tsinfo.min_srv_interval = adf_os_ntohl(resp_buf->data.tsinfo.min_srv_interval);
            dbgreq->data.tsinfo.max_srv_interval = adf_os_ntohl(resp_buf->data.tsinfo.max_srv_interval);
            dbgreq->data.tsinfo.inactivity_interval = adf_os_ntohl(resp_buf->data.tsinfo.inactivity_interval);
            dbgreq->data.tsinfo.suspension_interval = adf_os_ntohl(resp_buf->data.tsinfo.suspension_interval);
            dbgreq->data.tsinfo.srv_start_time = adf_os_ntohl(resp_buf->data.tsinfo.srv_start_time);
            dbgreq->data.tsinfo.min_data_rate = adf_os_ntohl(resp_buf->data.tsinfo.min_data_rate);
            dbgreq->data.tsinfo.mean_data_rate = adf_os_ntohl(resp_buf->data.tsinfo.mean_data_rate);
            dbgreq->data.tsinfo.peak_data_rate = adf_os_ntohl(resp_buf->data.tsinfo.peak_data_rate);
            dbgreq->data.tsinfo.max_burst_size = adf_os_ntohl(resp_buf->data.tsinfo.max_burst_size);
            dbgreq->data.tsinfo.delay_bound = adf_os_ntohl(resp_buf->data.tsinfo.delay_bound);
            dbgreq->data.tsinfo.min_phy_rate = adf_os_ntohl(resp_buf->data.tsinfo.min_phy_rate);
            dbgreq->data.tsinfo.surplus_bw = adf_os_ntohs(resp_buf->data.tsinfo.surplus_bw);
            dbgreq->data.tsinfo.medium_time = adf_os_ntohs(resp_buf->data.tsinfo.medium_time);
            break;
        case ACFG_DBGREQ_SENDLCIREQ:
            adf_os_mem_copy(dbgreq->data.lci_req.dstmac, resp_buf->data.lci_req.dstmac, 6);
            dbgreq->data.lci_req.num_rpts = adf_os_ntohs(resp_buf->data.lci_req.num_rpts);
            dbgreq->data.lci_req.location = resp_buf->data.lci_req.location;
            dbgreq->data.lci_req.lat_res = resp_buf->data.lci_req.lat_res;
            dbgreq->data.lci_req.long_res = resp_buf->data.lci_req.long_res;
            dbgreq->data.lci_req.alt_res = resp_buf->data.lci_req.alt_res;
            dbgreq->data.lci_req.azi_res = resp_buf->data.lci_req.azi_res;
            dbgreq->data.lci_req.azi_type = resp_buf->data.lci_req.azi_type;
            break;
        case ACFG_DBGREQ_GETRRMSTATS:
            dbgreq->data.rrmstats_req.index = adf_os_ntohl(resp_buf->data.rrmstats_req.index);
            dbgreq->data.rrmstats_req.data_size = adf_os_ntohl(resp_buf->data.rrmstats_req.data_size);
            rrm_stats = (atd_rrmstats_t *)(resp_buf + 1);
            rrmstats = (acfg_rrmstats_t *)(dbgreq->data.rrmstats_req.data_addr);
            for (i = 0; i < ACFG_MAX_IEEE_CHAN; i++) {
                rrmstats->chann_load[i] = rrm_stats->chann_load[i];
                rrmstats->noise_data[i].antid = rrm_stats->noise_data[i].antid;
                rrmstats->noise_data[i].anpi = rrm_stats->noise_data[i].anpi;
                for (j = 0; j < 11; j++)
                    rrmstats->noise_data[i].ipi[j] = rrm_stats->noise_data[i].ipi[j];
            }
            rrmstats->ni_rrm_stats.gid0.txfragcnt = adf_os_ntohl(rrm_stats->ni_rrm_stats.gid0.txfragcnt);
            rrmstats->ni_rrm_stats.gid0.mcastfrmcnt = adf_os_ntohl(rrm_stats->ni_rrm_stats.gid0.mcastfrmcnt);
            rrmstats->ni_rrm_stats.gid0.failcnt = adf_os_ntohl(rrm_stats->ni_rrm_stats.gid0.failcnt);
            rrmstats->ni_rrm_stats.gid0.rxfragcnt = adf_os_ntohl(rrm_stats->ni_rrm_stats.gid0.rxfragcnt);
            rrmstats->ni_rrm_stats.gid0.mcastrxfrmcnt = adf_os_ntohl(rrm_stats->ni_rrm_stats.gid0.mcastrxfrmcnt);
            rrmstats->ni_rrm_stats.gid0.fcserrcnt = adf_os_ntohl(rrm_stats->ni_rrm_stats.gid0.fcserrcnt);
            rrmstats->ni_rrm_stats.gid0.txfrmcnt = adf_os_ntohl(rrm_stats->ni_rrm_stats.gid0.txfrmcnt);
            rrmstats->ni_rrm_stats.gid1.rty = adf_os_ntohl(rrm_stats->ni_rrm_stats.gid1.rty);
            rrmstats->ni_rrm_stats.gid1.multirty = adf_os_ntohl(rrm_stats->ni_rrm_stats.gid1.multirty);
            rrmstats->ni_rrm_stats.gid1.frmdup = adf_os_ntohl(rrm_stats->ni_rrm_stats.gid1.frmdup);
            rrmstats->ni_rrm_stats.gid1.rtsuccess = adf_os_ntohl(rrm_stats->ni_rrm_stats.gid1.rtsuccess);
            rrmstats->ni_rrm_stats.gid1.rtsfail = adf_os_ntohl(rrm_stats->ni_rrm_stats.gid1.rtsfail);
            rrmstats->ni_rrm_stats.gid1.ackfail = adf_os_ntohl(rrm_stats->ni_rrm_stats.gid1.ackfail);
            for (i = 0; i < 8; i++) {
                rrmstats->ni_rrm_stats.gidupx[i].qostxfragcnt = adf_os_ntohl(rrm_stats->ni_rrm_stats.gidupx[i].qostxfragcnt);
                rrmstats->ni_rrm_stats.gidupx[i].qosfailedcnt = adf_os_ntohl(rrm_stats->ni_rrm_stats.gidupx[i].qosfailedcnt);
                rrmstats->ni_rrm_stats.gidupx[i].qosrtycnt = adf_os_ntohl(rrm_stats->ni_rrm_stats.gidupx[i].qosrtycnt);
                rrmstats->ni_rrm_stats.gidupx[i].multirtycnt = adf_os_ntohl(rrm_stats->ni_rrm_stats.gidupx[i].multirtycnt);
                rrmstats->ni_rrm_stats.gidupx[i].qosfrmdupcnt = adf_os_ntohl(rrm_stats->ni_rrm_stats.gidupx[i].qosfrmdupcnt);
                rrmstats->ni_rrm_stats.gidupx[i].qosrtssuccnt = adf_os_ntohl(rrm_stats->ni_rrm_stats.gidupx[i].qosrtssuccnt);
                rrmstats->ni_rrm_stats.gidupx[i].qosrtsfailcnt = adf_os_ntohl(rrm_stats->ni_rrm_stats.gidupx[i].qosrtsfailcnt);
                rrmstats->ni_rrm_stats.gidupx[i].qosackfailcnt = adf_os_ntohl(rrm_stats->ni_rrm_stats.gidupx[i].qosackfailcnt);
                rrmstats->ni_rrm_stats.gidupx[i].qosrxfragcnt = adf_os_ntohl(rrm_stats->ni_rrm_stats.gidupx[i].qosrxfragcnt);
                rrmstats->ni_rrm_stats.gidupx[i].qostxfrmcnt = adf_os_ntohl(rrm_stats->ni_rrm_stats.gidupx[i].qostxfrmcnt);
                rrmstats->ni_rrm_stats.gidupx[i].qosdiscadrfrmcnt = adf_os_ntohl(rrm_stats->ni_rrm_stats.gidupx[i].qosdiscadrfrmcnt);
                rrmstats->ni_rrm_stats.gidupx[i].qosmpdurxcnt = adf_os_ntohl(rrm_stats->ni_rrm_stats.gidupx[i].qosmpdurxcnt);
                rrmstats->ni_rrm_stats.gidupx[i].qosrtyrxcnt = adf_os_ntohl(rrm_stats->ni_rrm_stats.gidupx[i].qosrtyrxcnt);
            }
            rrmstats->ni_rrm_stats.gid10.ap_avg_delay = rrm_stats->ni_rrm_stats.gid10.ap_avg_delay;
            rrmstats->ni_rrm_stats.gid10.be_avg_delay = rrm_stats->ni_rrm_stats.gid10.be_avg_delay;
            rrmstats->ni_rrm_stats.gid10.bk_avg_delay = rrm_stats->ni_rrm_stats.gid10.bk_avg_delay;
            rrmstats->ni_rrm_stats.gid10.vi_avg_delay = rrm_stats->ni_rrm_stats.gid10.vi_avg_delay;
            rrmstats->ni_rrm_stats.gid10.vo_avg_delay = rrm_stats->ni_rrm_stats.gid10.vo_avg_delay;
            rrmstats->ni_rrm_stats.gid10.st_cnt = adf_os_ntohs(rrm_stats->ni_rrm_stats.gid10.st_cnt);
            rrmstats->ni_rrm_stats.gid10.ch_util = rrm_stats->ni_rrm_stats.gid10.ch_util;
            rrmstats->ni_rrm_stats.tsm_data.tid = rrm_stats->ni_rrm_stats.tsm_data.tid;
            rrmstats->ni_rrm_stats.tsm_data.brange = rrm_stats->ni_rrm_stats.tsm_data.brange;
            adf_os_mem_copy(rrmstats->ni_rrm_stats.tsm_data.mac, rrm_stats->ni_rrm_stats.tsm_data.mac, 6);
            rrmstats->ni_rrm_stats.tsm_data.tx_cnt = adf_os_ntohl(rrm_stats->ni_rrm_stats.tsm_data.tx_cnt);
            rrmstats->ni_rrm_stats.tsm_data.discnt = adf_os_ntohl(rrm_stats->ni_rrm_stats.tsm_data.discnt);
            rrmstats->ni_rrm_stats.tsm_data.multirtycnt = adf_os_ntohl(rrm_stats->ni_rrm_stats.tsm_data.multirtycnt);
            rrmstats->ni_rrm_stats.tsm_data.cfpoll = adf_os_ntohl(rrm_stats->ni_rrm_stats.tsm_data.cfpoll);
            rrmstats->ni_rrm_stats.tsm_data.qdelay = adf_os_ntohl(rrm_stats->ni_rrm_stats.tsm_data.qdelay);
            rrmstats->ni_rrm_stats.tsm_data.txdelay = adf_os_ntohl(rrm_stats->ni_rrm_stats.tsm_data.txdelay);
            for (i = 0; i < 6; i++)
                rrmstats->ni_rrm_stats.tsm_data.bin[i] = adf_os_ntohl(rrm_stats->ni_rrm_stats.tsm_data.bin[i]);
            for (i = 0; i < 12; i++) {
                rrmstats->ni_rrm_stats.frmcnt_data[i].phytype = rrm_stats->ni_rrm_stats.frmcnt_data[i].phytype;
                rrmstats->ni_rrm_stats.frmcnt_data[i].arcpi = rrm_stats->ni_rrm_stats.frmcnt_data[i].arcpi;
                rrmstats->ni_rrm_stats.frmcnt_data[i].lrsni = rrm_stats->ni_rrm_stats.frmcnt_data[i].lrsni;
                rrmstats->ni_rrm_stats.frmcnt_data[i].lrcpi = rrm_stats->ni_rrm_stats.frmcnt_data[i].lrcpi;
                rrmstats->ni_rrm_stats.frmcnt_data[i].antid = rrm_stats->ni_rrm_stats.frmcnt_data[i].antid;
                adf_os_mem_copy(rrmstats->ni_rrm_stats.frmcnt_data[i].ta, rrm_stats->ni_rrm_stats.frmcnt_data[i].ta, 6);
                adf_os_mem_copy(rrmstats->ni_rrm_stats.frmcnt_data[i].bssid, rrm_stats->ni_rrm_stats.frmcnt_data[i].bssid, 6);
                rrmstats->ni_rrm_stats.frmcnt_data[i].frmcnt = adf_os_ntohs(rrm_stats->ni_rrm_stats.frmcnt_data[i].frmcnt);
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
            rrmstats->ni_rrm_stats.ni_rrm_lciinfo.lat_integ = adf_os_ntohs(rrm_stats->ni_rrm_stats.ni_rrm_lciinfo.lat_integ);
            rrmstats->ni_rrm_stats.ni_rrm_lciinfo.long_integ = adf_os_ntohs(rrm_stats->ni_rrm_stats.ni_rrm_lciinfo.long_integ);
            rrmstats->ni_rrm_stats.ni_rrm_lciinfo.azimuth = adf_os_ntohs(rrm_stats->ni_rrm_stats.ni_rrm_lciinfo.azimuth);
            rrmstats->ni_rrm_stats.ni_rrm_lciinfo.lat_frac = adf_os_ntohl(rrm_stats->ni_rrm_stats.ni_rrm_lciinfo.lat_frac);
            rrmstats->ni_rrm_stats.ni_rrm_lciinfo.long_frac = adf_os_ntohl(rrm_stats->ni_rrm_stats.ni_rrm_lciinfo.long_frac);
            rrmstats->ni_rrm_stats.ni_rrm_lciinfo.alt_integ = adf_os_ntohl(rrm_stats->ni_rrm_stats.ni_rrm_lciinfo.alt_integ);
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
            rrmstats->ni_rrm_stats.ni_vap_lciinfo.lat_integ = adf_os_ntohs(rrm_stats->ni_rrm_stats.ni_vap_lciinfo.lat_integ);
            rrmstats->ni_rrm_stats.ni_vap_lciinfo.long_integ = adf_os_ntohs(rrm_stats->ni_rrm_stats.ni_vap_lciinfo.long_integ);
            rrmstats->ni_rrm_stats.ni_vap_lciinfo.azimuth = adf_os_ntohs(rrm_stats->ni_rrm_stats.ni_vap_lciinfo.azimuth);
            rrmstats->ni_rrm_stats.ni_vap_lciinfo.lat_frac = adf_os_ntohl(rrm_stats->ni_rrm_stats.ni_vap_lciinfo.lat_frac);
            rrmstats->ni_rrm_stats.ni_vap_lciinfo.long_frac = adf_os_ntohl(rrm_stats->ni_rrm_stats.ni_vap_lciinfo.long_frac);
            rrmstats->ni_rrm_stats.ni_vap_lciinfo.alt_integ = adf_os_ntohl(rrm_stats->ni_rrm_stats.ni_vap_lciinfo.alt_integ);
            break;
        case ACFG_DBGREQ_SENDFRMREQ:
            adf_os_mem_copy(dbgreq->data.frm_req.dstmac, resp_buf->data.frm_req.dstmac, 6);
            adf_os_mem_copy(dbgreq->data.frm_req.peermac, resp_buf->data.frm_req.peermac, 6);
            dbgreq->data.frm_req.num_rpts = adf_os_ntohs(resp_buf->data.frm_req.num_rpts);
            dbgreq->data.frm_req.regclass = resp_buf->data.frm_req.regclass;
            dbgreq->data.frm_req.chnum = resp_buf->data.frm_req.chnum;
            break;
        case ACFG_DBGREQ_GETBCNRPT:
            dbgreq->data.rrmstats_req.index = adf_os_ntohl(resp_buf->data.rrmstats_req.index);
            dbgreq->data.rrmstats_req.data_size = adf_os_ntohl(resp_buf->data.rrmstats_req.data_size);
            bcn_rpt = (atd_bcnrpt_t *)(resp_buf + 1);
            bcnrpt = (acfg_bcnrpt_t *)(dbgreq->data.rrmstats_req.data_addr);
            adf_os_mem_copy(bcnrpt->bssid, bcn_rpt->bssid, 6);
            bcnrpt->rsni = bcn_rpt->rsni;
            bcnrpt->rcpi = bcn_rpt->rcpi;
            bcnrpt->chnum = bcn_rpt->chnum;
            bcnrpt->more = bcn_rpt->more;
            break;
        case ACFG_DBGREQ_GETRRSSI:
            break;
        case ACFG_DBGREQ_GETACSREPORT:
            dbgreq->data.acs_rep.index = adf_os_ntohl(resp_buf->data.acs_rep.index);
            dbgreq->data.acs_rep.data_size = adf_os_ntohl(resp_buf->data.acs_rep.data_size);
            acs_dbg = (atd_acs_dbg_t *)(resp_buf + 1);
            acsdbg = (acfg_acs_dbg_t *)(dbgreq->data.acs_rep.data_addr);
            acsdbg->nchans = acs_dbg->nchans;
            acsdbg->entry_id = acs_dbg->entry_id;
            acsdbg->chan_freq = adf_os_ntohs(acs_dbg->chan_freq);
            acsdbg->ieee_chan = acs_dbg->ieee_chan;
            acsdbg->chan_nbss = acs_dbg->chan_nbss;
            acsdbg->chan_maxrssi = adf_os_ntohl(acs_dbg->chan_maxrssi);
            acsdbg->chan_minrssi = adf_os_ntohl(acs_dbg->chan_minrssi);
            acsdbg->noisefloor = adf_os_ntohs(acs_dbg->noisefloor);
            acsdbg->channel_loading = adf_os_ntohs(acs_dbg->channel_loading);
            acsdbg->chan_load = adf_os_ntohl(acs_dbg->chan_load);
            acsdbg->sec_chan = acs_dbg->sec_chan;
            break;
        case ACFG_DBGREQ_BSTEERING_SET_PARAMS:
            break;
        case ACFG_DBGREQ_BSTEERING_GET_PARAMS:
            dbgreq->data.bst.inactivity_timeout_normal = adf_os_ntohl(resp_buf->data.bst.inactivity_timeout_normal);
            dbgreq->data.bst.inactivity_timeout_overload = adf_os_ntohl(resp_buf->data.bst.inactivity_timeout_overload);
            dbgreq->data.bst.inactivity_check_period = adf_os_ntohl(resp_buf->data.bst.inactivity_check_period);
            dbgreq->data.bst.utilization_sample_period = adf_os_ntohl(resp_buf->data.bst.utilization_sample_period);
            dbgreq->data.bst.utilization_average_num_samples = adf_os_ntohl(resp_buf->data.bst.utilization_average_num_samples);
            dbgreq->data.bst.inactive_rssi_crossing_threshold = adf_os_ntohl(resp_buf->data.bst.inactive_rssi_crossing_threshold);
            dbgreq->data.bst.low_rssi_crossing_threshold = adf_os_ntohl(resp_buf->data.bst.low_rssi_crossing_threshold);
            break;
        case ACFG_DBGREQ_BSTEERING_SET_DBG_PARAMS:
            break;
        case ACFG_DBGREQ_BSTEERING_GET_DBG_PARAMS:
            dbgreq->data.bst_dbg.raw_log_enable = resp_buf->data.bst_dbg.raw_log_enable;
            break;
        case ACFG_DBGREQ_BSTEERING_ENABLE:
            break;
        case ACFG_DBGREQ_BSTEERING_SET_OVERLOAD:
            break;
        case ACFG_DBGREQ_BSTEERING_GET_OVERLOAD:
            dbgreq->data.bsteering_overload = resp_buf->data.bsteering_overload;
            break;
        case ACFG_DBGREQ_BSTEERING_GET_RSSI:
            adf_os_mem_copy(dbgreq->data.bst_rssi_req.sender_addr, resp_buf->data.bst_rssi_req.sender_addr, 6);
            dbgreq->data.bst_rssi_req.num_measurements = adf_os_ntohs(resp_buf->data.bst_rssi_req.num_measurements);
            break;
        case ACFG_DBGREQ_BSTEERING_SET_PROBE_RESP_WH:
            break;
        default:
            break;
    }

fail:
    if (resp_buf)
        adf_os_mem_free(resp_buf);

    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("End stat %x ",status ));
    return status;
}

/** 
 * @brief Set Appiebuf
 * 
 * @param hdl
 * @param appiebuf
 * 
 * @return 
 */
a_status_t 
atd_cfg_set_appiebuf(adf_drv_handle_t hdl, acfg_appie_t  *appiebuf)
{
    atd_host_vap_t     *vap    = (atd_host_vap_t * )hdl;
    atd_host_wifi_t    *wifi   = vap->wifisc;
    atd_appie_t        *pld;
    a_status_t          status = A_STATUS_OK;
    adf_nbuf_t          nbuf;

    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("appiebuf %p ", \
                appiebuf));

    if ((nbuf = _atd_wmi_msg_alloc(__wifi2wmi(wifi))) == NULL) {
        atd_trace(ATD_DEBUG_CFG, ("Unable to allocate wmi buffer \n"));
        status = A_STATUS_ENOMEM;
        goto fail ;
    }

    pld = (atd_appie_t *)atd_prep_cfg_hdr(nbuf, sizeof(atd_appie_t) + 
                                          appiebuf->buflen, wifi, vap);

    pld->frmtype = adf_os_htonl(appiebuf->frmtype);
    pld->buflen = adf_os_htonl(appiebuf->buflen);
    adf_os_mem_copy(pld->buf, appiebuf->buf, appiebuf->buflen);

    status = wmi_cmd_send(__wifi2wmi(wifi), WMI_CMD_APPIEBUF_SET, nbuf,
            NULL, 0);

fail:
    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("End stat %x ",status ));
    return status;
}


/** 
 * @brief  Set Key
 * 
 * @param hdl
 * @param setkey
 * 
 * @return 
 */
a_status_t 
atd_cfg_set_key(adf_drv_handle_t hdl, acfg_key_t  *setkey)
{
    atd_host_vap_t     *vap    = (atd_host_vap_t * )hdl;
    atd_host_wifi_t    *wifi   = vap->wifisc;
    atd_key_t          *pld;
    a_status_t          status = A_STATUS_OK;
    adf_nbuf_t          nbuf;

    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("setkey "));

    if ((nbuf = _atd_wmi_msg_alloc(__wifi2wmi(wifi))) == NULL) {
        atd_trace(ATD_DEBUG_CFG, ("Unable to allocate wmi buffer \n"));
        status = A_STATUS_ENOMEM;
        goto fail ;
    }

    pld = (atd_key_t *)atd_prep_cfg_hdr(nbuf, sizeof(atd_key_t), wifi, vap);

    pld->type = setkey->type;
    pld->pad = setkey->pad;
    pld->keyix = adf_os_htons(setkey->keyix);
    pld->keylen = setkey->keylen;
    pld->flags = setkey->flags;
    adf_os_mem_copy(pld->macaddr, setkey->macaddr, ACFG_MACADDR_LEN);
    pld->keyrsc = adf_os_cpu_to_be64(setkey->keyrsc);
    pld->keytsc = adf_os_cpu_to_be64(setkey->keytsc);
    adf_os_mem_copy(pld->keydata, setkey->keydata, ACFG_KEYDATA_LEN);

    status = wmi_cmd_send(__wifi2wmi(wifi), WMI_CMD_SET_KEY, nbuf,
                          NULL, 0);

fail:
    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("End stat %x ",status ));
    return status;
}



/** 
 * @brief del key
 * 
 * @param hdl
 * @param delkey
 * 
 * @return 
 */
a_status_t 
atd_cfg_del_key(adf_drv_handle_t hdl, acfg_delkey_t  *delkey)
{
    atd_host_vap_t     *vap    = (atd_host_vap_t * )hdl;
    atd_host_wifi_t    *wifi   = vap->wifisc;
    atd_delkey_t       *pld;
    a_status_t          status = A_STATUS_OK;
    adf_nbuf_t          nbuf;

    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("del key "));

    if ((nbuf = _atd_wmi_msg_alloc(__wifi2wmi(wifi))) == NULL) {
        atd_trace(ATD_DEBUG_CFG, ("Unable to allocate wmi buffer \n"));
        status = A_STATUS_ENOMEM;
        goto fail ;
    }

    pld = (atd_delkey_t *)atd_prep_cfg_hdr(nbuf, sizeof(atd_delkey_t), wifi, vap);

    pld->idx = delkey->idx;
    adf_os_mem_copy(pld->addr, delkey->addr, ACFG_MACADDR_LEN);
    
    status = wmi_cmd_send(__wifi2wmi(wifi), WMI_CMD_DEL_KEY, nbuf,
                          NULL, 0);


fail:
    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("End stat %x ",status ));
    return status;
}




/** 
 * @brief Get Key
 * 
 * @param hdl
 * @param buff
 * @param buflen
 * 
 * @return 
 */
static a_status_t
atd_cfg_get_key(adf_drv_handle_t hdl, a_uint8_t *buff, a_uint32_t buflen)
{

    atd_host_vap_t    *vap    = (atd_host_vap_t * )hdl;
    atd_host_wifi_t   *wifi   = vap->wifisc;
    a_status_t         status = A_STATUS_OK;
    adf_nbuf_t         nbuf;
    atd_key_t         *pld, result = {0};
    acfg_key_t        *getkey;

    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("get key = %p", buff ));

    if (buflen != sizeof(acfg_key_t)) {
        atd_trace(ATD_DEBUG_CFG, ("Keylen invalid %d, expected %d \n", 
                  buflen, sizeof(acfg_key_t)));
        status = A_STATUS_EINVAL;
        goto fail ;
    }

    getkey = (acfg_key_t *)buff;

    if ((nbuf = _atd_wmi_msg_alloc(__wifi2wmi(wifi))) == NULL) {
        atd_trace(ATD_DEBUG_CFG, ("Unable to allocate wmi buffer \n"));
        status = A_STATUS_ENOMEM;
        goto fail ;
    }


    pld = (atd_key_t *)atd_prep_cfg_hdr(nbuf, sizeof(atd_key_t), wifi, vap);

    atd_trace(ATD_DEBUG_CFG, ("User sent buflen = %d\n",buflen));

    pld->type = getkey->type;
    pld->pad = getkey->pad;
    pld->keyix = adf_os_htons(getkey->keyix);
    pld->keylen = getkey->keylen;
    pld->flags = getkey->flags;
    adf_os_mem_copy(pld->macaddr, getkey->macaddr, ACFG_MACADDR_LEN);
    pld->keyrsc = adf_os_cpu_to_be64(getkey->keyrsc);
    pld->keytsc = adf_os_cpu_to_be64(getkey->keytsc);
    adf_os_mem_copy(pld->keydata, getkey->keydata, ACFG_KEYDATA_LEN);

    status      = wmi_cmd_send(__wifi2wmi(wifi), WMI_CMD_GET_KEY, nbuf,
                          (a_uint8_t *)(&result), sizeof(atd_key_t));

    getkey->type = result.type;
    getkey->pad = result.pad;
    getkey->keyix = adf_os_ntohs(result.keyix);
    getkey->keylen = result.keylen;
    getkey->flags = result.flags;
    adf_os_mem_copy(getkey->macaddr, result.macaddr, ACFG_MACADDR_LEN);
    getkey->keyrsc = adf_os_be64_to_cpu(result.keyrsc);
    getkey->keytsc = adf_os_be64_to_cpu(result.keytsc);
    adf_os_mem_copy(getkey->keydata, result.keydata, ACFG_KEYDATA_LEN);

    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("key   %p ", buff));

fail:
    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("End stat %x ",status ));
    return status;
}


/** 
 * @brief Get STA Stats
 * 
 * @param hdl
 * @param buff
 * @param buflen
 * 
 * @return 
 */
static a_status_t
atd_cfg_get_sta_stats(adf_drv_handle_t hdl, a_uint8_t *buff, a_uint32_t buflen)
{
    atd_host_vap_t    *vap     = (atd_host_vap_t * )hdl;
    atd_host_wifi_t   *wifi    = vap->wifisc;
    a_status_t         status  = A_STATUS_OK;
    adf_nbuf_t         nbuf;
    a_int32_t         *ptr_buflen;

    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("sta stats = %p", buff ));

    if ((nbuf = _atd_wmi_msg_alloc(__wifi2wmi(wifi))) == NULL) {
        atd_trace(ATD_DEBUG_CFG, ("Unable to allocate wmi buffer \n"));
        status = A_STATUS_ENOMEM;
        goto fail ;
    }

    ptr_buflen = (a_uint32_t *)atd_prep_cfg_hdr(nbuf, \
                                (VALID_PAYLOAD_SIZE + sizeof(a_uint32_t)), wifi, vap);

    atd_trace(ATD_DEBUG_CFG, ("User sent buflen = %d\n",buflen));


    *(a_uint32_t *)ptr_buflen = adf_os_htonl(buflen);
    memcpy((a_uint8_t *)ptr_buflen + sizeof(a_uint32_t), (a_uint8_t *)buff, VALID_PAYLOAD_SIZE);

    status      = wmi_cmd_send(__wifi2wmi(wifi), WMI_CMD_STA_STATS_GET, nbuf,
                          buff, buflen);

    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("STA stats   %p ", buff));

fail:
    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("End stat %x ",status ));
    return status;
}


/** 
 * WMI Events
 */

typedef void  (*__atd_event_fn_t)(atd_host_wifi_t  *wifi, atd_host_vap_t  *vap,
                                  wmi_id_t wmi, adf_nbuf_t  buf);


#define PROTO_EVENT(name)                                           \
    static void atd_event_##name(atd_host_wifi_t *, atd_host_vap_t *, \
                                 wmi_id_t wmi, adf_nbuf_t)


PROTO_EVENT(none);
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
PROTO_EVENT(create_vap);
PROTO_EVENT(delete_vap);
PROTO_EVENT(restore_wifi);
PROTO_EVENT(delete_wifi);
PROTO_EVENT(wsupp_generic);
PROTO_EVENT(byp_vap_lanen);
PROTO_EVENT(byp_vap_landis);
PROTO_EVENT(iw_custom);
PROTO_EVENT(iw_assocreqie);
PROTO_EVENT(proxyarp_send);
PROTO_EVENT(spectral_msg_send);
PROTO_EVENT(bsteer_send);

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


#define EVENT_NUM(x)        (x - WMI_EVENT_START)
#define EVENT_IDX(x)            [EVENT_NUM(WMI_EVENT_##x)]
#define EVENT_IDX_WSUPP(x)      [EVENT_NUM(WMI_EVENT_WSUPP_##x)]

const __atd_event_fn_t     atd_events[] = {
    EVENT_IDX(START)          = atd_event_none,
    EVENT_IDX(SCAN_DONE)      = atd_event_scan_done,
    EVENT_IDX(ASSOC_AP)       = atd_event_assoc_ap,
    EVENT_IDX(ASSOC_STA)      = atd_event_assoc_sta,
    EVENT_IDX(CHAN_START)     = atd_event_chan_start,
    EVENT_IDX(CHAN_END)       = atd_event_chan_end,
    EVENT_IDX(RX_MGMT)        = atd_event_rx_mgmt,
    EVENT_IDX(SENT_ACTION)    = atd_event_sent_action,
    EVENT_IDX(DELETE_VAP)     = atd_event_delete_vap,
    EVENT_IDX(CREATE_VAP)     = atd_event_create_vap,
    EVENT_IDX(RESTORE_WIFI)    = atd_event_restore_wifi,
    EVENT_IDX(DELETE_WIFI)    = atd_event_delete_wifi,
    EVENT_IDX(IF_RUNNING_STA) = atd_event_if_running_sta,
    EVENT_IDX(IF_NOT_RUNNING_STA) = atd_event_if_not_running_sta,
    EVENT_IDX(LEAVE_AP)       = atd_event_leave_ap,
    EVENT_IDX(GEN_IE)         = atd_event_gen_ie,
    //EVENT_IDX(ASSOC_REQ_IE)   = atd_event_assoc_req_ie,
    EVENT_IDX(IW_CUSTOM)      = atd_event_iw_custom,
    EVENT_IDX(IW_ASSOCREQIE)  = atd_event_iw_assocreqie,

    EVENT_IDX(AUTH_COMP_AP)  = atd_event_iw_auth_comp_ap,
	EVENT_IDX(ASSOC_COMP_AP)  = atd_event_iw_assoc_comp_ap,
    EVENT_IDX(DEAUTH_COMP_AP) = atd_event_iw_deauth_comp_ap,
    EVENT_IDX(AUTH_IND_AP)    = atd_event_iw_auth_ind_ap,
    EVENT_IDX(AUTH_COMP_STA)  = atd_event_iw_auth_comp_sta,
    EVENT_IDX(ASSOC_COMP_STA) = atd_event_iw_assoc_comp_sta,
    EVENT_IDX(DEAUTH_COMP_STA)    = atd_event_iw_deauth_comp_sta,
    EVENT_IDX(DISASSOC_COMP_STA)  = atd_event_iw_disassoc_comp_sta,
    EVENT_IDX(AUTH_IND_STA)   = atd_event_iw_auth_ind_sta,
	EVENT_IDX(DEAUTH_IND_STA) = atd_event_iw_deauth_ind_sta,
    EVENT_IDX(ASSOC_IND_STA)  = atd_event_iw_assoc_ind_sta,
    EVENT_IDX(DISASSOC_IND_STA)   = atd_event_iw_disassoc_ind_sta,
    EVENT_IDX(DEAUTH_IND_AP)   = atd_event_iw_deauth_ind_ap,
    EVENT_IDX(DISASSOC_IND_AP)   = atd_event_iw_disassoc_ind_ap,
    EVENT_IDX(WAPI)   = atd_event_iw_wapi,

    /* security WMI events */
    EVENT_IDX_WSUPP(RAW_MESSAGE)                = atd_event_wsupp_generic,
    EVENT_IDX_WSUPP(AP_STA_CONNECTED)           = atd_event_wsupp_generic,
    EVENT_IDX_WSUPP(WPA_EVENT_CONNECTED)        = atd_event_wsupp_generic,
    EVENT_IDX_WSUPP(WPA_EVENT_DISCONNECTED)     = atd_event_wsupp_generic,
    EVENT_IDX_WSUPP(WPA_EVENT_TERMINATING)      = atd_event_wsupp_generic,
    EVENT_IDX_WSUPP(WPA_EVENT_SCAN_RESULTS)     = atd_event_wsupp_generic,
    EVENT_IDX_WSUPP(WPS_EVENT_ENROLLEE_SEEN)    = atd_event_wsupp_generic,

    /* LAN Bypass event */
    EVENT_IDX(BYP_VAP_LANEN)    = atd_event_byp_vap_lanen,
    EVENT_IDX(BYP_VAP_LANDIS)   = atd_event_byp_vap_landis,
    
    EVENT_IDX(PROXYARP_SEND)    = atd_event_proxyarp_send,

    /* Spectral sample message */
    EVENT_IDX(SPECTRAL_MSG_SEND) = atd_event_spectral_msg_send,
    /* Band steering nl event to LBD */
    EVENT_IDX(BSTEER_SEND)       = atd_event_bsteer_send,
};

#define max_event_num   \
    ((sizeof(atd_events)/sizeof(atd_events[0])) + WMI_EVENT_START)

/** 
 * @brief ATD Event runner
 * 
 * @param dev_sc
 * @param id
 * @param buf
 */
void
atd_event_run(struct atd_host_dev *dev_sc, wmi_id_t   event, 
              adf_nbuf_t  buf)
{
    atd_hdr_t          *hdr;
    a_uint32_t          len  = 0;
    atd_host_wifi_t    *wifi = NULL;
    atd_host_vap_t     *vap  = NULL;

    atd_trace(ATD_DEBUG_FUNCTRACE, (" .START "));

    adf_nbuf_peek_header(buf, (a_uint8_t **)&hdr, &len);
    adf_nbuf_pull_head(buf, sizeof(struct atd_hdr));
    
    wifi = dev_sc->wifi_list[hdr->wifi_index];

    if (hdr->vap_index < 0xFF)
        vap = wifi->vap_list[hdr->vap_index];
    
    adf_os_assert(event < max_event_num);
    
    if ((event == WMI_EVENT_CREATE_VAP) && (vap != NULL))
        event = WMI_EVENT_START;
    
    if (event > max_event_num) event = WMI_EVENT_START; 

    atd_events[EVENT_NUM(event)](wifi, vap, event, buf);
  
    atd_trace(ATD_DEBUG_CFG, ("Freeing the event"));

    /* Freeing the event */
    adf_nbuf_free(buf);

    atd_trace(ATD_DEBUG_FUNCTRACE, (" .END "));
}


/** 
 * @brief 
 * 
 * @param wifi
 * @param vap
 * @param wmi
 * @param buf
 */
void  
atd_event_none(atd_host_wifi_t  *wifi, atd_host_vap_t  *vap, 
                wmi_id_t wmi, adf_nbuf_t  buf)
{
    atd_trace(ATD_DEBUG_FUNCTRACE,("%s:unhandled event \n",adf_os_function));
}

/** 
 * @brief 
 * 
 * @param wifi
 * @param vap
 * @param wmi
 * @param buf
 */
void
atd_event_iw_custom(atd_host_wifi_t   *wifi, atd_host_vap_t   *vap, 
                    wmi_id_t wmi, adf_nbuf_t buf)
{
    a_uint32_t          len     = 0;
    acfg_os_event_t     event   = {.id = ACFG_EV_MIC_FAIL};
    acfg_wsupp_custom_message_t *data    = NULL; 
    acfg_wsupp_custom_message_t  *mic    = (void *)&event.data;

    adf_os_assert(vap);

    atd_trace(ATD_DEBUG_FUNCTRACE, (" .START "));

    if (vap == NULL)
        return;

    adf_nbuf_peek_header(buf, (a_uint8_t **)&data, &len);
    adf_nbuf_pull_head(buf, sizeof(acfg_wsupp_custom_message_t));

    adf_os_mem_copy(mic->raw_message, data->raw_message, strlen(data->raw_message));

    adf_net_indicate_event(vap->nethandle, &event);

    atd_trace(ATD_DEBUG_FUNCTRACE, (" .END "));
}

/** 
 * @brief 
 * 
 * @param wifi
 * @param vap
 * @param wmi
 * @param buf
 */
void
atd_event_iw_assocreqie(atd_host_wifi_t   *wifi, atd_host_vap_t   *vap, 
                    wmi_id_t wmi, adf_nbuf_t buf)
{
    a_uint32_t          len      = 0;
    a_uint8_t          *data     = NULL;
    acfg_os_event_t     event    = {.id = ACFG_EV_PROBE_REQ};
    acfg_probe_req_t   *pr_req   = (void *)&event.data;

    adf_os_assert(vap);

    atd_trace(ATD_DEBUG_FUNCTRACE, (" .START "));

    if (vap == NULL)
        return;

    adf_nbuf_peek_header(buf, (a_uint8_t **)&data, &len);
    adf_nbuf_pull_head(buf, len);

    pr_req->len = len;
    pr_req->data = adf_os_mem_alloc(NULL, len);
    if (pr_req->data == NULL)
        goto done;

    adf_os_mem_copy(pr_req->data, data, len);

    adf_net_indicate_event(vap->nethandle, &event);

    adf_os_mem_free(pr_req->data);

done:
    atd_trace(ATD_DEBUG_FUNCTRACE, (" .END "));
}

void
atd_event_iw_auth_comp_ap(atd_host_wifi_t   *wifi, atd_host_vap_t   *vap, 
                    wmi_id_t wmi, adf_nbuf_t buf)
{
    a_uint32_t          len     = 0;
    acfg_os_event_t     event   = {.id = ACFG_EV_AUTH_AP};
    atd_auth_t *data    = NULL; 
    acfg_auth_t  *auth    = (void *)&event.data;

    adf_os_assert(vap);

    atd_trace(ATD_DEBUG_FUNCTRACE, (" .START "));

    if (vap == NULL)
        return;

    adf_nbuf_peek_header(buf, (a_uint8_t **)&data, &len);
    adf_nbuf_pull_head(buf, sizeof(atd_auth_t));

	auth->status  = adf_os_ntohl(data->status);
	auth->frame_send  = data->frame_send;
	
    adf_net_indicate_event(vap->nethandle, &event);

    atd_trace(ATD_DEBUG_FUNCTRACE, (" .END "));
}


void
atd_event_iw_assoc_comp_ap(atd_host_wifi_t   *wifi, atd_host_vap_t   *vap, 
                    wmi_id_t wmi, adf_nbuf_t buf)
{
    a_uint32_t          len     = 0;
    acfg_os_event_t     event   = {.id = ACFG_EV_ASSOC_AP};
    atd_assoc_t *data    = NULL; 
    acfg_assoc_t  *assoc   = (void *)&event.data;

    adf_os_assert(vap);

    atd_trace(ATD_DEBUG_FUNCTRACE, (" .START "));

    if (vap == NULL)
        return;

    adf_nbuf_peek_header(buf, (a_uint8_t **)&data, &len);
    adf_nbuf_pull_head(buf, sizeof(atd_assoc_t));

	assoc->status  = adf_os_ntohl(data->status);
	assoc->frame_send  = data->frame_send;
	
    adf_net_indicate_event(vap->nethandle, &event);

    atd_trace(ATD_DEBUG_FUNCTRACE, (" .END "));
}

void
atd_event_iw_deauth_comp_ap(atd_host_wifi_t   *wifi, atd_host_vap_t   *vap, 
                    wmi_id_t wmi, adf_nbuf_t buf)
{
    a_uint32_t          len     = 0;
    acfg_os_event_t     event   = {.id = ACFG_EV_DEAUTH_AP};
    atd_dauth_t *data    = NULL; 
    acfg_dauth_t  *deauth   = (void *)&event.data;

    adf_os_assert(vap);

    atd_trace(ATD_DEBUG_FUNCTRACE, (" .START "));

    if (vap == NULL)
        return;

    adf_nbuf_peek_header(buf, (a_uint8_t **)&data, &len);
    adf_nbuf_pull_head(buf, sizeof(atd_dauth_t));

	deauth->status  = adf_os_ntohl(data->status);
	deauth->frame_send  = data->frame_send;
    adf_os_mem_copy(deauth->macaddr, data->macaddr, ACFG_MACADDR_LEN);
	
    adf_net_indicate_event(vap->nethandle, &event);

    atd_trace(ATD_DEBUG_FUNCTRACE, (" .END "));
}


void
atd_event_iw_auth_ind_ap(atd_host_wifi_t   *wifi, atd_host_vap_t   *vap, 
                    wmi_id_t wmi, adf_nbuf_t buf)
{
    a_uint32_t          len     = 0;
    acfg_os_event_t     event   = {.id = ACFG_EV_AUTH_AP};
    atd_auth_t *data    = NULL; 
    acfg_auth_t  *auth   = (void *)&event.data;

    adf_os_assert(vap);

    atd_trace(ATD_DEBUG_FUNCTRACE, (" .START "));

    if (vap == NULL)
        return;

    adf_nbuf_peek_header(buf, (a_uint8_t **)&data, &len);
    adf_nbuf_pull_head(buf, sizeof(atd_auth_t));

	auth->status  = adf_os_ntohl(data->status);
	auth->frame_send  = data->frame_send;
    adf_os_mem_copy(auth->macaddr, data->macaddr, ACFG_MACADDR_LEN);
	
    adf_net_indicate_event(vap->nethandle, &event);

    atd_trace(ATD_DEBUG_FUNCTRACE, (" .END "));
}

void
atd_event_iw_auth_comp_sta(atd_host_wifi_t   *wifi, atd_host_vap_t   *vap, 
                    wmi_id_t wmi, adf_nbuf_t buf)
{
    a_uint32_t          len     = 0;
    acfg_os_event_t     event   = {.id = ACFG_EV_AUTH_STA};
    atd_auth_t *data    = NULL; 
    acfg_auth_t  *auth   = (void *)&event.data;

    adf_os_assert(vap);

    atd_trace(ATD_DEBUG_FUNCTRACE, (" .START "));

    if (vap == NULL)
        return;

    adf_nbuf_peek_header(buf, (a_uint8_t **)&data, &len);
    adf_nbuf_pull_head(buf, sizeof(atd_auth_t));

	auth->status  = adf_os_ntohl(data->status);
	auth->frame_send  = data->frame_send;
	
    adf_net_indicate_event(vap->nethandle, &event);

    atd_trace(ATD_DEBUG_FUNCTRACE, (" .END "));
}

void
atd_event_iw_assoc_comp_sta(atd_host_wifi_t   *wifi, atd_host_vap_t   *vap, 
                    wmi_id_t wmi, adf_nbuf_t buf)
{
    a_uint32_t          len     = 0;
    acfg_os_event_t     event   = {.id = ACFG_EV_ASSOC_STA};
    atd_assoc_t *data    = NULL; 
    acfg_assoc_t  *assoc   = (void *)&event.data;

    adf_os_assert(vap);

    atd_trace(ATD_DEBUG_FUNCTRACE, (" .START "));

    if (vap == NULL)
        return;

    adf_nbuf_peek_header(buf, (a_uint8_t **)&data, &len);
    adf_nbuf_pull_head(buf, sizeof(atd_assoc_t));

	assoc->status  = adf_os_ntohl(data->status);
	assoc->frame_send  = data->frame_send;
	
    adf_net_indicate_event(vap->nethandle, &event);

    atd_trace(ATD_DEBUG_FUNCTRACE, (" .END "));
}


void
atd_event_iw_deauth_comp_sta(atd_host_wifi_t   *wifi, atd_host_vap_t   *vap, 
                    wmi_id_t wmi, adf_nbuf_t buf)
{
    a_uint32_t          len     = 0;
    acfg_os_event_t     event   = {.id = ACFG_EV_DEAUTH_STA};
    atd_dauth_t *data    = NULL; 
    acfg_dauth_t  *auth   = (void *)&event.data;

    adf_os_assert(vap);

    atd_trace(ATD_DEBUG_FUNCTRACE, (" .START "));

    if (vap == NULL)
        return;

    adf_nbuf_peek_header(buf, (a_uint8_t **)&data, &len);
    adf_nbuf_pull_head(buf, sizeof(atd_dauth_t));

	auth->status  = adf_os_ntohl(data->status);
	auth->frame_send  = data->frame_send;
    adf_os_mem_copy(auth->macaddr, data->macaddr, ACFG_MACADDR_LEN);
	
    adf_net_indicate_event(vap->nethandle, &event);

    atd_trace(ATD_DEBUG_FUNCTRACE, (" .END "));
}


void
atd_event_iw_disassoc_comp_sta(atd_host_wifi_t   *wifi, atd_host_vap_t   *vap, 
                    wmi_id_t wmi, adf_nbuf_t buf)
{
    a_uint32_t          len     = 0;
    acfg_os_event_t     event   = {.id = ACFG_EV_DISASSOC_STA};
    atd_disassoc_t *data    = NULL; 
    acfg_disassoc_t  *disassoc   = (void *)&event.data;

    adf_os_assert(vap);

    atd_trace(ATD_DEBUG_FUNCTRACE, (" .START "));

    if (vap == NULL)
        return;

    adf_nbuf_peek_header(buf, (a_uint8_t **)&data, &len);
    adf_nbuf_pull_head(buf, sizeof(atd_disassoc_t));

	disassoc->status  = adf_os_ntohl(data->status);
	disassoc->reason  = adf_os_ntohl(data->reason);
	disassoc->frame_send  = data->frame_send;
    adf_os_mem_copy(disassoc->macaddr, data->macaddr, ACFG_MACADDR_LEN);
	
    adf_net_indicate_event(vap->nethandle, &event);

    atd_trace(ATD_DEBUG_FUNCTRACE, (" .END "));
}

void
atd_event_iw_auth_ind_sta(atd_host_wifi_t   *wifi, atd_host_vap_t   *vap, 
                    wmi_id_t wmi, adf_nbuf_t buf)
{
    a_uint32_t          len     = 0;
    acfg_os_event_t     event   = {.id = ACFG_EV_AUTH_STA};
    atd_auth_t *data    = NULL; 
    acfg_auth_t  *auth   = (void *)&event.data;

    adf_os_assert(vap);

    atd_trace(ATD_DEBUG_FUNCTRACE, (" .START "));

    if (vap == NULL)
        return;

    adf_nbuf_peek_header(buf, (a_uint8_t **)&data, &len);
    adf_nbuf_pull_head(buf, sizeof(atd_auth_t));

	auth->status  = adf_os_ntohl(data->status);
	auth->frame_send  = data->frame_send;
    adf_os_mem_copy(auth->macaddr, data->macaddr, ACFG_MACADDR_LEN);
	
    adf_net_indicate_event(vap->nethandle, &event);

    atd_trace(ATD_DEBUG_FUNCTRACE, (" .END "));
}

void
atd_event_iw_deauth_ind_sta(atd_host_wifi_t   *wifi, atd_host_vap_t   *vap, 
                    wmi_id_t wmi, adf_nbuf_t buf)
{
    a_uint32_t          len     = 0;
    acfg_os_event_t     event   = {.id = ACFG_EV_DEAUTH_STA};
    atd_dauth_t *data    = NULL; 
    acfg_dauth_t  *deauth   = (void *)&event.data;

    adf_os_assert(vap);

    atd_trace(ATD_DEBUG_FUNCTRACE, (" .START "));

    if (vap == NULL)
        return;

    adf_nbuf_peek_header(buf, (a_uint8_t **)&data, &len);
    adf_nbuf_pull_head(buf, sizeof(atd_dauth_t));

	deauth->reason  = adf_os_ntohl(data->reason);
	deauth->frame_send  = data->frame_send;
    adf_os_mem_copy(deauth->macaddr, data->macaddr, ACFG_MACADDR_LEN);
	
    adf_net_indicate_event(vap->nethandle, &event);

    atd_trace(ATD_DEBUG_FUNCTRACE, (" .END "));
}

void
atd_event_iw_assoc_ind_sta(atd_host_wifi_t   *wifi, atd_host_vap_t   *vap, 
                    wmi_id_t wmi, adf_nbuf_t buf)
{
    a_uint32_t          len     = 0;
    acfg_os_event_t     event   = {.id = ACFG_EV_ASSOC_STA};
    atd_assoc_t *data    = NULL; 
    acfg_assoc_t  *assoc   = (void *)&event.data;

    adf_os_assert(vap);

    atd_trace(ATD_DEBUG_FUNCTRACE, (" .START "));

    if (vap == NULL)
        return;

    adf_nbuf_peek_header(buf, (a_uint8_t **)&data, &len);
    adf_nbuf_pull_head(buf, sizeof(atd_assoc_t));

	assoc->status  = adf_os_ntohl(data->status);
	assoc->frame_send  = data->frame_send;
    adf_os_mem_copy(assoc->bssid, data->bssid, ACFG_MACADDR_LEN);
	
    adf_net_indicate_event(vap->nethandle, &event);

    atd_trace(ATD_DEBUG_FUNCTRACE, (" .END "));
}


void
atd_event_iw_disassoc_ind_sta(atd_host_wifi_t   *wifi, atd_host_vap_t   *vap, 
                    wmi_id_t wmi, adf_nbuf_t buf)
{
    a_uint32_t          len     = 0;
    acfg_os_event_t     event   = {.id = ACFG_EV_DISASSOC_STA};
    atd_disassoc_t *data    = NULL; 
    acfg_disassoc_t  *disassoc   = (void *)&event.data;

    adf_os_assert(vap);

    atd_trace(ATD_DEBUG_FUNCTRACE, (" .START "));

    if (vap == NULL)
        return;

    adf_nbuf_peek_header(buf, (a_uint8_t **)&data, &len);
    adf_nbuf_pull_head(buf, sizeof(atd_disassoc_t));

	disassoc->reason  = adf_os_ntohl(data->reason);
	disassoc->frame_send  = data->frame_send;
    adf_os_mem_copy(disassoc->macaddr, data->macaddr, ACFG_MACADDR_LEN);
	
    adf_net_indicate_event(vap->nethandle, &event);

    atd_trace(ATD_DEBUG_FUNCTRACE, (" .END "));
}

void
atd_event_iw_deauth_ind_ap(atd_host_wifi_t   *wifi, atd_host_vap_t   *vap, 
                    wmi_id_t wmi, adf_nbuf_t buf)
{
    a_uint32_t          len     = 0;
    acfg_os_event_t     event   = {.id = ACFG_EV_DEAUTH_AP};
    atd_dauth_t *data    = NULL; 
    acfg_dauth_t  *deauth   = (void *)&event.data;

    adf_os_assert(vap);

    atd_trace(ATD_DEBUG_FUNCTRACE, (" .START "));

    if (vap == NULL)
        return;

    adf_nbuf_peek_header(buf, (a_uint8_t **)&data, &len);
    adf_nbuf_pull_head(buf, sizeof(atd_dauth_t));

	deauth->reason  = adf_os_ntohl(data->reason);
	deauth->frame_send  = data->frame_send;
    adf_os_mem_copy(deauth->macaddr, data->macaddr, ACFG_MACADDR_LEN);
	
    adf_net_indicate_event(vap->nethandle, &event);

    atd_trace(ATD_DEBUG_FUNCTRACE, (" .END "));
}

void
atd_event_iw_disassoc_ind_ap(atd_host_wifi_t   *wifi, atd_host_vap_t   *vap, 
                    wmi_id_t wmi, adf_nbuf_t buf)
{
    a_uint32_t          len     = 0;
    acfg_os_event_t     event   = {.id = ACFG_EV_DISASSOC_AP};
    atd_disassoc_t *data    = NULL; 
    acfg_disassoc_t  *disassoc   = (void *)&event.data;

    adf_os_assert(vap);

    atd_trace(ATD_DEBUG_FUNCTRACE, (" .START "));

    if (vap == NULL)
        return;

    adf_nbuf_peek_header(buf, (a_uint8_t **)&data, &len);
    adf_nbuf_pull_head(buf, sizeof(atd_disassoc_t));

	disassoc->reason  = adf_os_ntohl(data->reason);
	disassoc->frame_send  = data->frame_send;
    adf_os_mem_copy(disassoc->macaddr, data->macaddr, ACFG_MACADDR_LEN);
	
    adf_net_indicate_event(vap->nethandle, &event);

    atd_trace(ATD_DEBUG_FUNCTRACE, (" .END "));
}

/** 
 * @brief 
 * 
 * @param wifi
 * @param vap
 * @param wmi
 * @param buf
 */
void
atd_event_iw_wapi(atd_host_wifi_t   *wifi, atd_host_vap_t   *vap, 
                    wmi_id_t wmi, adf_nbuf_t buf)
{
    a_uint32_t          len     = 0;
    acfg_os_event_t     event   = {.id = ACFG_EV_WAPI};
    acfg_wsupp_custom_message_t *data    = NULL; 
    acfg_wsupp_custom_message_t  *wapi    = (void *)&event.data;

    adf_os_assert(vap);

    atd_trace(ATD_DEBUG_FUNCTRACE, (" .START "));

    if (vap == NULL)
        return;

    adf_nbuf_peek_header(buf, (a_uint8_t **)&data, &len);
    adf_nbuf_pull_head(buf, len);

    adf_os_mem_copy(wapi->raw_message, data->raw_message, len);

    adf_net_indicate_event(vap->nethandle, &event);

    atd_trace(ATD_DEBUG_FUNCTRACE, (" .END "));
}


/** 
 * @brief 
 * 
 * @param wifi
 * @param vap
 * @param wmi
 * @param buf
 */
void
atd_event_scan_done(atd_host_wifi_t   *wifi, atd_host_vap_t   *vap, 
                    wmi_id_t wmi, adf_nbuf_t buf)
{
    a_uint32_t          len     = 0;
    acfg_os_event_t     event   = {.id = ACFG_EV_SCAN_DONE};
    atd_scan_done_t    *data    = NULL; 
    acfg_scan_done_t   *scan    = (void *)&event.data;

    adf_os_assert(vap);

    atd_trace(ATD_DEBUG_FUNCTRACE, (" .START "));

    if (vap == NULL)
        return;

    adf_nbuf_peek_header(buf, (a_uint8_t **)&data, &len);
    adf_nbuf_pull_head(buf, sizeof(atd_scan_done_t));

    scan->size    = adf_os_ntohl(data->size);

    adf_net_indicate_event(vap->nethandle, &event);

    atd_trace(ATD_DEBUG_FUNCTRACE, (" .END "));
}

/** 
 * @brief Send Station Join event to the Application
 * 
 * @param wifi
 * @param vap
 * @param wmi
 * @param buf
 */
void
atd_event_assoc_ap(atd_host_wifi_t   *wifi, atd_host_vap_t   *vap, 
                  wmi_id_t wmi, adf_nbuf_t buf)
{
    a_uint32_t          len         = 0;
    acfg_os_event_t     event       = {.id = ACFG_EV_ASSOC_AP};
    atd_assoc_ap_t     *data        = NULL; 
    acfg_assoc_t    *assoc_ap    = (void *)&event.data;
    
    adf_os_assert(vap);

    atd_trace(ATD_DEBUG_FUNCTRACE, (" .START "));

    if (vap == NULL)
        return;

    adf_nbuf_peek_header(buf, (a_uint8_t **)&data, &len);
    adf_nbuf_pull_head(buf, sizeof(atd_assoc_ap_t));

    assoc_ap->status  = adf_os_ntohl(data->status);
    adf_os_mem_copy(assoc_ap->bssid, data->bssid, ACFG_MACADDR_LEN);

    adf_net_indicate_event(vap->nethandle, &event);

    atd_trace(ATD_DEBUG_FUNCTRACE, (" .END "));
}

/** 
 * @brief Send a Associated to an AP event to the application
 * 
 * @param wifi
 * @param vap
 * @param wmi
 * @param buf
 */
void
atd_event_assoc_sta(atd_host_wifi_t   *wifi, atd_host_vap_t   *vap, 
                    wmi_id_t wmi, adf_nbuf_t buf)
{
    a_uint32_t          len         = 0;
    acfg_os_event_t     event       = {.id = ACFG_EV_ASSOC_STA};
    atd_assoc_sta_t    *data       = NULL; 
    acfg_assoc_t   *assoc_sta  = (void *)&event.data;
    
    adf_os_assert(vap);

    atd_trace(ATD_DEBUG_FUNCTRACE, (" .START "));

    if (vap == NULL)
        return;

    adf_nbuf_peek_header(buf, (a_uint8_t **)&data, &len);
    adf_nbuf_pull_head(buf, sizeof(atd_assoc_sta_t));

    assoc_sta->status  = adf_os_ntohl(data->status);
    adf_os_mem_copy(assoc_sta->bssid, data->bssid, ACFG_MACADDR_LEN);
        
    adf_net_indicate_event(vap->nethandle, &event);

    atd_trace(ATD_DEBUG_FUNCTRACE, (" .END "));

    return;
}

/** 
 * @brief Radio On Channel Event
 * 
 * @param wifi
 * @param vap
 * @param wmi
 * @param buf
 */
void
atd_event_chan_start(atd_host_wifi_t   *wifi, atd_host_vap_t   *vap, 
                  wmi_id_t wmi, adf_nbuf_t buf)
{
    a_uint32_t          len         = 0;
    void               *data        = NULL; 
    acfg_os_event_t     event       = {.id = ACFG_EV_CHAN_START};
    atd_chan_start_t   *payload;
    acfg_chan_start_t  *chan_start  = (void *)&event.data;
    
    adf_os_assert(vap);

    atd_trace(ATD_DEBUG_FUNCTRACE, (" .START "));

    if (vap == NULL)
        return;

    adf_nbuf_peek_header(buf, (a_uint8_t **)&data, &len);
    payload = (atd_chan_start_t *)data;

    adf_nbuf_pull_head(buf, len);

    chan_start->freq     = adf_os_ntohl(payload->freq);
    chan_start->duration = adf_os_ntohl(payload->duration);
    chan_start->req_id   = adf_os_ntohl(payload->req_id);

    adf_net_indicate_event(vap->nethandle, &event);

    atd_trace(ATD_DEBUG_FUNCTRACE, (" .END "));
}

/** 
 * @brief Radio Off Channel Event
 * 
 * @param wifi
 * @param vap
 * @param wmi
 * @param buf
 */
void
atd_event_chan_end(atd_host_wifi_t   *wifi, atd_host_vap_t   *vap, 
                  wmi_id_t wmi, adf_nbuf_t buf)
{
    a_uint32_t          len         = 0;
    void               *data        = NULL; 
    acfg_os_event_t     event       = {.id = ACFG_EV_CHAN_END};
    atd_chan_end_t     *payload;
    acfg_chan_end_t    *chan_end    = (void *)&event.data;

    adf_os_assert(vap);

    atd_trace(ATD_DEBUG_FUNCTRACE, (" .START "));

    if (vap == NULL)
        return;

    adf_nbuf_peek_header(buf, (a_uint8_t **)&data, &len);
    payload = (atd_chan_end_t *)data;

    adf_nbuf_pull_head(buf, len);

    chan_end->freq       = adf_os_ntohl(payload->freq);
    chan_end->reason     = adf_os_ntohl(payload->reason);
    chan_end->duration   = adf_os_ntohl(payload->duration);
    chan_end->req_id     = adf_os_ntohl(payload->req_id);

    adf_net_indicate_event(vap->nethandle, &event);

    atd_trace(ATD_DEBUG_FUNCTRACE, (" .END "));
}

/** 
 * @brief Received Management Frame Event
 * 
 * @param wifi
 * @param vap
 * @param wmi
 * @param buf
 */
void
atd_event_rx_mgmt(atd_host_wifi_t   *wifi, atd_host_vap_t   *vap, 
                  wmi_id_t wmi, adf_nbuf_t buf)
{
    acfg_os_event_t     event       = {.id = ACFG_EV_RX_MGMT};
    
    adf_os_assert(vap);

    atd_trace(ATD_DEBUG_FUNCTRACE, (" .START "));

    if (vap == NULL)
        return;

    adf_net_indicate_event(vap->nethandle, &event);

    atd_trace(ATD_DEBUG_FUNCTRACE, (" .END "));
}

/** 
 * @brief Sent Action Frame Event
 * 
 * @param wifi
 * @param vap
 * @param wmi
 * @param buf
 */
void
atd_event_sent_action(atd_host_wifi_t   *wifi, atd_host_vap_t   *vap, 
                  wmi_id_t wmi, adf_nbuf_t buf)
{
    acfg_os_event_t     event       = {.id = ACFG_EV_SENT_ACTION};
    
    adf_os_assert(vap);

    atd_trace(ATD_DEBUG_FUNCTRACE, (" .START "));

    if (vap == NULL)
        return;

    adf_net_indicate_event(vap->nethandle, &event);

    atd_trace(ATD_DEBUG_FUNCTRACE, (" .END "));
}

/** 
 * @brief Station Leaving an AP Event
 * 
 * @param wifi
 * @param vap
 * @param wmi
 * @param buf
 */
void
atd_event_leave_ap(atd_host_wifi_t   *wifi, atd_host_vap_t   *vap, 
                  wmi_id_t wmi, adf_nbuf_t buf)
{
    a_uint32_t          len         = 0;
    void               *data        = NULL; 
    acfg_os_event_t     event       = {.id = ACFG_EV_LEAVE_AP};
    
    adf_os_assert(vap);

    atd_trace(ATD_DEBUG_FUNCTRACE, (" .START "));

    if (vap == NULL)
        return;

    adf_nbuf_peek_header(buf, (a_uint8_t **)&data, &len);
    adf_nbuf_pull_head(buf, len);

    adf_os_mem_copy(event.data.leave_ap.mac, data, 
            len > ACFG_MACADDR_LEN ? ACFG_MACADDR_LEN : len);

    adf_net_indicate_event(vap->nethandle, &event);

    atd_trace(ATD_DEBUG_FUNCTRACE, (" .END "));
}

/** 
 * @brief Pass IE Event
 * 
 * @param wifi
 * @param vap
 * @param wmi
 * @param buf
 */
void
atd_event_gen_ie(atd_host_wifi_t   *wifi, atd_host_vap_t   *vap, 
                  wmi_id_t wmi, adf_nbuf_t buf)
{
    a_uint32_t          len         = 0;
    void               *data        = NULL; 
    acfg_os_event_t     event       = {.id = ACFG_EV_GEN_IE};
    
    adf_os_assert(vap);

    atd_trace(ATD_DEBUG_FUNCTRACE, (" .START "));

    if (vap == NULL)
        return;

    adf_nbuf_peek_header(buf, (a_uint8_t **)&data, &len);
    adf_nbuf_pull_head(buf, len);

    event.data.gen_ie.len = 0;
    event.data.gen_ie.data = adf_os_mem_alloc(NULL, len);
    if (!event.data.gen_ie.data) {
        atd_trace(ATD_DEBUG_ERROR, ("Unable to allocate IE buffer \n"));
    } else {
        event.data.gen_ie.len = len;
        adf_os_mem_copy(event.data.gen_ie.data, data, len);
    }

    adf_net_indicate_event(vap->nethandle, &event);

    if (event.data.gen_ie.data)
        adf_os_mem_free(event.data.gen_ie.data);

    atd_trace(ATD_DEBUG_FUNCTRACE, (" .END "));
}

#if 0
/** 
 * @brief Pass Assoc Req IE Event
 * 
 * @param wifi
 * @param vap
 * @param wmi
 * @param buf
 */
void
atd_event_assoc_req_ie(atd_host_wifi_t   *wifi, atd_host_vap_t   *vap, 
                  wmi_id_t wmi, adf_nbuf_t buf)
{
    a_uint32_t          len         = 0;
    void               *data        = NULL; 
    acfg_os_event_t     event       = {.id = ACFG_EV_ASSOC_REQ_IE};
    
    adf_os_assert(vap);

    atd_trace(ATD_DEBUG_FUNCTRACE, (" .START "));

    adf_nbuf_peek_header(buf, (a_uint8_t **)&data, &len);
    adf_nbuf_pull_head(buf, len);

    event.data.gen_ie.len = 0;
    event.data.gen_ie.data = adf_os_mem_alloc(NULL, len);
    if (!event.data.gen_ie.data) {
        atd_trace(ATD_DEBUG_ERROR, ("Unable to allocate IE buffer \n"));
    } else {
        event.data.gen_ie.len = len;
        adf_os_mem_copy(event.data.gen_ie.data, data, len);
    }

    adf_net_indicate_event(vap->nethandle, &event);

    if (event.data.gen_ie.data)
        adf_os_mem_free(event.data.gen_ie.data);

    atd_trace(ATD_DEBUG_FUNCTRACE, (" .END "));
}
#endif

/** 
 * @brief 
 * 
 * @param wifi
 * @param vap
 * @param wmi
 * @param buf
 */
void
atd_event_if_running_sta(atd_host_wifi_t   *wifi, atd_host_vap_t   *vap, 
                    wmi_id_t wmi, adf_nbuf_t buf)
{
    a_uint32_t          len         = 0;
    atd_assoc_sta_t    *data       = NULL; 
    
    adf_os_assert(vap);

    if (vap == NULL)
        return;

    atd_trace(ATD_DEBUG_FUNCTRACE, (" .START "));

    adf_nbuf_peek_header(buf, (a_uint8_t **)&data, &len);
    adf_nbuf_pull_head(buf, sizeof(atd_assoc_sta_t));

    adf_net_carrier_on(vap->nethandle);       
    atd_trace(ATD_DEBUG_FUNCTRACE, (" .END "));

    return;
}

/** 
 * @brief 
 * 
 * @param wifi
 * @param vap
 * @param wmi
 * @param buf
 */
void
atd_event_if_not_running_sta(atd_host_wifi_t   *wifi, atd_host_vap_t   *vap, 
                    wmi_id_t wmi, adf_nbuf_t buf)
{
    a_uint32_t          len         = 0;
    atd_assoc_sta_t    *data       = NULL; 
    
    adf_os_assert(vap);

    atd_trace(ATD_DEBUG_FUNCTRACE, (" .START "));

    if (vap == NULL)
        return;

    adf_nbuf_peek_header(buf, (a_uint8_t **)&data, &len);
    adf_nbuf_pull_head(buf, sizeof(atd_assoc_sta_t));

    adf_net_carrier_off(vap->nethandle);       
    atd_trace(ATD_DEBUG_FUNCTRACE, (" .END "));

    return;
}

/** 
 * @brief Send a create vap event to the application
 * 
 * @param wifi
 * @param vap
 * @param wmi
 * @param buf
 */
void
atd_event_create_vap(atd_host_wifi_t   *wifi, atd_host_vap_t   *vap_input, 
                  wmi_id_t wmi, adf_nbuf_t buf)
{
    adf_net_dev_info_t       info  = {{0}};
    adf_dev_sw_t             dev_switch = {0};
    atd_host_vap_t          *vap = NULL;
    wmi_handle_t             htch =  __wifi2htc(wifi);
    atd_create_vap_t     *data    = NULL;
    a_uint32_t              len         = 0,error,unit;
    a_uint8_t              if_name[ACFG_MAX_IFNAME];
    
    atd_trace(ATD_DEBUG_FUNCTRACE, (" .START "));

    adf_nbuf_peek_header(buf, (a_uint8_t **)&data, &len);
    adf_nbuf_pull_head(buf, sizeof(struct acfg_create_vap));

    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG),
              ("vap_name %s", 
               data->if_name));

    /*
     * successful. Create local vap
     */
    if ((vap = adf_os_mem_alloc(NULL, sizeof(atd_host_vap_t))) == NULL) {
        atd_trace(ATD_DEBUG_CFG, ("Error in allocating vap \n"));
        adf_os_assert(0);
    }

    adf_os_mem_zero(vap, sizeof(atd_host_vap_t));
    vap->wifisc      = wifi;
    vap->vap_index   = data->vap_index;

    /* Protect the VAP Array */
    atd_spin_lock_bh(wifi->dsc);      /* Lock */

    adf_os_assert(wifi->vap_list[vap->vap_index] == NULL);
    
    wifi->vap_list[vap->vap_index] = vap;

    atd_spin_unlock_bh(wifi->dsc);    /* Unlock */

    atd_trace(ATD_DEBUG_LEVEL3,("vap name %s vap index %d  \
                mac %x:%x:%x:%x:%x:%x ",\
                data->if_name,\
                data->vap_index,\
                data->mac_addr[0],  \
                data->mac_addr[1],  \
                data->mac_addr[2],  \
                data->mac_addr[3],  \
                data->mac_addr[4],  \
                data->mac_addr[5]   \
                ));
    atd_trace(ATD_DEBUG_FUNCTRACE,("vapname %s len %d \n", \
                data->if_name, adf_os_str_len(data->if_name)));

    /* Check if wlan unit undex is passed in event data,
    If so, extract and reserve that in bitmap index maintained by ADF
    If not, reserve a new one and start using */
    error = adf_net_ifc_name2unit(data->if_name, &unit);
    if (error)
        goto fail_unit ;

    /* Allocation is tricky here, so let's give a few explanation.
     * We are basically trying to handle two cases:
     * - if the number isn't specified by the user, we have to allocate one,
     *   in which case we need to make sure it's not already been allocated
     *   already. User says "ath" and we create "athN" with N being a new unit#
     * - if the number is specified, we just need to make sure it's not been
     *   allocated already, which we check using dev_get_by_name()
     */
    if (unit == -1)
    {
        unit = adf_net_new_wlanunit();
        if (unit == -1)
        {
            goto fail_unit ;
        }
        adf_os_snprint(if_name, sizeof(if_name), "%s", data->if_name);
        adf_os_snprint(data->if_name, sizeof(data->if_name), "%s%d", if_name,unit);
        info.unit = unit;
    }
    else
    {
        int dev_exist = adf_net_dev_exist_by_name(data->if_name);
        if (dev_exist)
            goto fail_unit;

        unit = adf_net_new_wlanunit();
        info.unit = unit;
    }

    adf_os_str_ncopy(info.if_name, data->if_name, 
                     adf_os_str_len(data->if_name));
    adf_os_mem_copy(info.dev_addr, &data->mac_addr[0], ACFG_MACADDR_LEN);

    info.header_len = sizeof(atd_tx_hdr_t) + 
                      htc_get_reserveheadroom(htch);

    
    dev_switch.drv_ioctl       = atd_vap_ioctl;
    dev_switch.drv_tx_timeout  = atd_vap_tx_timeout ;
    dev_switch.drv_open        = atd_vap_open;
    dev_switch.drv_close       = atd_vap_close;
    dev_switch.drv_tx          = atd_host_tx;
    dev_switch.drv_cmd         = atd_vap_cmd;

    vap->nethandle = adf_net_create_vapdev(vap, &dev_switch, &info, &vap_cfg);

    if(vap->nethandle == ADF_NET_NULL ) {
        atd_trace(ATD_DEBUG_CFG, ("Failure during vap_attach \n"));
        goto fail;
    }
    
    atd_trace(ATD_DEBUG_FUNCTRACE,("VAP created  _adf_net_ifname %s \n",
                 adf_net_ifname(vap->nethandle)));

    return;

fail:
/* Free the allocated bitmap for vap index in adf */
    adf_net_delete_wlanunit(unit);

fail_unit :
    if(vap) adf_os_mem_free(vap);
    
    return;
}

 /* @brief Delete VAP event, 
 * 
 * @param wifi
 * @param vap
 * @param wmi
 * @param buf
 *
 * @note  This assumes that all events on this VAP has been 
 *        reliably being delivered & processed before this
 *        arrives
 */
void
atd_event_delete_vap(atd_host_wifi_t    *wifi, atd_host_vap_t   *vap,
                     wmi_id_t wmi, adf_nbuf_t     buf)
{
    atd_trace(ATD_DEBUG_FUNCTRACE, (" .START "));

    atd_trace(ATD_DEBUG_CFG, ("VAP delete %s\n", 
                adf_net_ifname(vap->nethandle)));

    adf_os_assert(vap);

    if (vap == NULL)
        return;

    if (vap->nethandle) {
        adf_net_stop_queue(vap->nethandle); 
    }

    atd_free_vap_index(wifi, vap->vap_index);

    if(vap->nethandle){
        /* Free the wlan unit index maintained at adf for VAP's */
        adf_net_free_wlanunit(vap->nethandle);

        adf_net_delete_dev(vap->nethandle); 
    }

    adf_os_mem_free(vap);

    atd_trace(ATD_DEBUG_FUNCTRACE, (" .END "));
}

void
atd_event_restore_wifi(atd_host_wifi_t *wifi, atd_host_vap_t *vap,
                      wmi_id_t wmi, adf_nbuf_t buf)
{
    adf_net_dev_info_t       info  = {{0}};
    adf_dev_sw_t             dev_switch = {0};
    atd_restore_wifi_t       *data;
    htc_handle_t             htch =  __wifi2htc(wifi);
    a_uint32_t              len         = 0;

    atd_trace(ATD_DEBUG_FUNCTRACE, (" .START "));

    adf_nbuf_peek_header(buf, (a_uint8_t **)&data, &len);
    adf_nbuf_pull_head(buf, sizeof(struct acfg_restore_wifi));

    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG),
              ("wifi_name %s",
               data->if_name));

    adf_os_str_ncopy(info.if_name, data->if_name,
                     adf_os_str_len(data->if_name));
    adf_os_mem_copy(info.dev_addr, &data->mac_addr[0], ACFG_MACADDR_LEN);

    info.header_len = sizeof(atd_tx_hdr_t) +
                      htc_get_reserveheadroom(htch);

    dev_switch.drv_open             = atd_wifi_open ;
    dev_switch.drv_close            = atd_wifi_close;
    dev_switch.drv_tx               = atd_wifi_tx   ;
    dev_switch.drv_cmd              = atd_wifi_cmd  ;
    dev_switch.drv_ioctl            = atd_wifi_ioctl;
    dev_switch.drv_tx_timeout       = atd_wifi_tx_timeout ;

    wifi->netdev = adf_net_create_wifidev(wifi, &dev_switch, &info, &wifi_cfg);

    atd_trace(ATD_DEBUG_FUNCTRACE, (" .END "));
    return;

}

 /* @brief Delete WIFI event,
 *
 * @param wifi
 * @param vap
 * @param wmi
 * @param buf
 *
 */
void
atd_event_delete_wifi(atd_host_wifi_t    *wifi, atd_host_vap_t   *vap,
                     wmi_id_t wmi, adf_nbuf_t     buf)
{
   atd_trace(ATD_DEBUG_FUNCTRACE, (" .START "));

    adf_os_assert(wifi);

    if (wifi->netdev) {
        adf_net_stop_queue(wifi->netdev);
    }

    if(wifi->netdev){
        adf_net_delete_dev(wifi->netdev);
    }

    atd_trace(ATD_DEBUG_FUNCTRACE, (" .END "));
}

 /* @brief wsupp event generic handler
 * 
 * @param wifi
 * @param vap
 * @param wmi
 * @param buf
 */
#define do_case(x, y) \
    case WMI_EVENT_WSUPP_ ##x: \
        event.id = ACFG_EV_WSUPP_ ##x; \
        adf_os_mem_copy(&event.data, data, sizeof(y)); \
        break
void
atd_event_wsupp_generic(atd_host_wifi_t    *wifi, atd_host_vap_t   *vap,
                     wmi_id_t wmi, adf_nbuf_t     buf)
{
    a_uint32_t len;
    acfg_os_event_t event;
    a_uint8_t *data; 
    
    adf_os_assert(vap);
    atd_trace(ATD_DEBUG_FUNCTRACE, (" .START "));

    if (vap == NULL)
        return;

    adf_nbuf_peek_header(buf, &data, &len);

    switch (wmi) {
    do_case(RAW_MESSAGE, acfg_wsupp_raw_message_t);
    do_case(AP_STA_CONNECTED, acfg_wsupp_ap_sta_conn_t);
    do_case(WPA_EVENT_CONNECTED, acfg_wsupp_wpa_conn_t);
    do_case(WPA_EVENT_DISCONNECTED, acfg_wsupp_wpa_conn_t);
    do_case(WPA_EVENT_TERMINATING, acfg_wsupp_wpa_conn_t);
    do_case(WPA_EVENT_SCAN_RESULTS, acfg_wsupp_wpa_conn_t);
    do_case(WPS_EVENT_ENROLLEE_SEEN, acfg_wsupp_wps_enrollee_t);
    default:
        atd_trace(ATD_DEBUG_CFG, ("invalid wsupp wmi event: %d\n", wmi));
        return;
    }
        
    adf_net_indicate_event(vap->nethandle, &event);

    atd_trace(ATD_DEBUG_FUNCTRACE, (" .END "));
}


/** 
 * @brief Handle Bypass VAPCTRL event - Set LAN enable flag
 * 
 * @param wifi
 * @param vap
 * @param wmi
 * @param buf
 */
void
atd_event_byp_vap_lanen(atd_host_wifi_t    *wifi, atd_host_vap_t   *vap,
                     wmi_id_t wmi, adf_nbuf_t     buf)
{

    atd_trace(ATD_DEBUG_FUNCTRACE, (" .START "));

    if (vap == NULL)
        return;

    atd_trace(ATD_DEBUG_CFG, ("BYPASS VAP Enable LAN Access%s\n", 
                adf_net_ifname(vap->nethandle)));

    vap->lan_en     =   1;

    atd_trace(ATD_DEBUG_FUNCTRACE, (" .END "));
}

/** 
 * @brief  Handle Bypass VAPCTRL event - Reset LAN enable flag
 * 
 * @param wifi
 * @param vap
 * @param wmi
 * @param buf
 */
void
atd_event_byp_vap_landis(atd_host_wifi_t    *wifi, atd_host_vap_t   *vap,
                     wmi_id_t wmi, adf_nbuf_t     buf)
{

    atd_trace(ATD_DEBUG_FUNCTRACE, (" .START "));

    if (vap == NULL)
        return;

    atd_trace(ATD_DEBUG_CFG, ("BYPASS VAP Enable LAN Access%s\n", 
                adf_net_ifname(vap->nethandle)));

    vap->lan_en     =   0;

    atd_trace(ATD_DEBUG_FUNCTRACE, (" .END "));
}

void
atd_event_proxyarp_send(atd_host_wifi_t    *wifi, atd_host_vap_t   *vap,
        wmi_id_t wmi, adf_nbuf_t     buf)
{
    a_uint32_t  len = 0;
    a_uint8_t   *data = NULL;
    a_uint16_t  wbuf_len = 0;
    a_uint8_t   *nbuf_hdr = NULL;
    adf_nbuf_t  nbuf = NULL;

    if (vap == NULL)
        return;

    adf_nbuf_peek_header(buf, (a_uint8_t **)&data, &len);

    adf_os_mem_copy((a_uint8_t *)&wbuf_len, data, sizeof(a_uint16_t));

    wbuf_len = adf_os_ntohs(wbuf_len);
    data += sizeof(a_uint16_t);

    nbuf = adf_nbuf_alloc(NULL, wbuf_len, 0, 0, 0);
    nbuf_hdr = (a_uint8_t *)adf_nbuf_put_tail(nbuf, wbuf_len);
    adf_os_mem_copy(nbuf_hdr, data, wbuf_len);
    nbuf->len = wbuf_len;
    adf_net_dev_tx(vap->nethandle, nbuf);

}

#define MAX_SPECTRAL_PAYLOAD 1500
void atd_convert_spectral_samp_msg_ntoh(atd_spectral_samp_msg_t *out, atd_spectral_samp_msg_t *in)
{
    int i;

    out->signature = adf_os_ntohl(in->signature);
    out->freq      = adf_os_ntohs(in->freq);
    out->freq_loading = adf_os_ntohs(in->freq_loading);
    out->dcs_enabled = adf_os_ntohs(in->dcs_enabled);
    out->int_type = adf_os_ntohl(in->int_type);
    memcpy(out->macaddr, in->macaddr, 6);

    out->samp_data.spectral_data_len = adf_os_ntohs(in->samp_data.spectral_data_len);
    out->samp_data.spectral_rssi = adf_os_ntohs(in->samp_data.spectral_rssi);
    out->samp_data.spectral_combined_rssi = in->samp_data.spectral_combined_rssi;
    out->samp_data.spectral_upper_rssi = in->samp_data.spectral_upper_rssi;
    out->samp_data.spectral_lower_rssi = in->samp_data.spectral_lower_rssi;
    memcpy(out->samp_data.spectral_chain_ctl_rssi, in->samp_data.spectral_chain_ctl_rssi, MAX_SPECTRAL_CHAINS);
    memcpy(out->samp_data.spectral_chain_ext_rssi, in->samp_data.spectral_chain_ext_rssi, MAX_SPECTRAL_CHAINS);
    out->samp_data.spectral_max_scale = in->samp_data.spectral_max_scale;
    out->samp_data.spectral_bwinfo = adf_os_ntohs(in->samp_data.spectral_bwinfo);
    out->samp_data.spectral_tstamp = adf_os_ntohl(in->samp_data.spectral_tstamp);
    out->samp_data.spectral_max_index = adf_os_ntohs(in->samp_data.spectral_max_index);
    out->samp_data.spectral_max_mag = adf_os_ntohs(in->samp_data.spectral_max_mag);
    out->samp_data.spectral_max_exp = in->samp_data.spectral_max_exp;
    out->samp_data.spectral_last_tstamp = adf_os_ntohl(in->samp_data.spectral_last_tstamp);
    out->samp_data.spectral_upper_max_index = adf_os_ntohs(in->samp_data.spectral_upper_max_index);
    out->samp_data.spectral_lower_max_index = adf_os_ntohs(in->samp_data.spectral_lower_max_index);
    out->samp_data.spectral_nb_upper = in->samp_data.spectral_nb_upper;
    out->samp_data.spectral_nb_lower = in->samp_data.spectral_nb_lower;

    out->samp_data.classifier_params.spectral_20_40_mode = 
        adf_os_ntohl(in->samp_data.classifier_params.spectral_20_40_mode);
    out->samp_data.classifier_params.spectral_dc_index = 
        adf_os_ntohl(in->samp_data.classifier_params.spectral_dc_index);
    out->samp_data.classifier_params.spectral_dc_in_mhz = 
        adf_os_ntohl(in->samp_data.classifier_params.spectral_dc_in_mhz);
    out->samp_data.classifier_params.upper_chan_in_mhz = 
        adf_os_ntohl(in->samp_data.classifier_params.upper_chan_in_mhz);
    out->samp_data.classifier_params.lower_chan_in_mhz = 
        adf_os_ntohl(in->samp_data.classifier_params.lower_chan_in_mhz);

    out->samp_data.bin_pwr_count = adf_os_ntohs(in->samp_data.bin_pwr_count);
    memcpy(out->samp_data.bin_pwr, in->samp_data.bin_pwr, MAX_NUM_BINS);
    
    out->samp_data.interf_list.count = adf_os_ntohs(in->samp_data.interf_list.count);

    for (i = 0; i < MAX_INTERF; i++)
    {
        out->samp_data.interf_list.interf[i].interf_type = 
            in->samp_data.interf_list.interf[i].interf_type;
        out->samp_data.interf_list.interf[i].interf_min_freq = 
            adf_os_ntohs(in->samp_data.interf_list.interf[i].interf_min_freq);
        out->samp_data.interf_list.interf[i].interf_max_freq = 
            adf_os_ntohs(in->samp_data.interf_list.interf[i].interf_max_freq);
    }

    out->samp_data.noise_floor = adf_os_ntohs(in->samp_data.noise_floor);
    out->samp_data.ch_width = adf_os_ntohl(in->samp_data.ch_width);
}

extern struct sock *atd_spectral_nl_sock;
void
atd_event_spectral_msg_send(atd_host_wifi_t *wifi, 
        atd_host_vap_t *vap,
        wmi_id_t wmi, 
        adf_nbuf_t buf)
{
    a_uint32_t  len = 0;
    atd_spectral_samp_msg_t *data = NULL;
    adf_nbuf_t  nbuf = NULL;
    struct nlmsghdr *nlh;

    adf_nbuf_peek_header(buf, (a_uint8_t **)&data, &len);

    nbuf = dev_alloc_skb(MAX_SPECTRAL_PAYLOAD);

    if (nbuf != NULL) {
        skb_put(nbuf, MAX_SPECTRAL_PAYLOAD);
        nlh = (struct nlmsghdr*)nbuf->data;
        nlh->nlmsg_len   = NLMSG_SPACE(sizeof(SPECTRAL_SAMP_MSG));
        nlh->nlmsg_pid   = 0;
        nlh->nlmsg_flags = 0;
        /* Its time todo ntoh conversion */
        atd_convert_spectral_samp_msg_ntoh(NLMSG_DATA(nlh), data);
        netlink_broadcast(atd_spectral_nl_sock, 
                nbuf, 
                0,
                1, 
                GFP_ATOMIC);
    }
}

/**
 * @brief  Convert network order to host order
 *
 * @param out
 * @param in
 */
void 
atd_convert_bsteer_event_ntoh(ath_netlink_bsteering_event_t *out, 
        atd_bsteering_event_t *in)
{
    out->type = adf_os_ntohl(in->type);
    out->band_index = adf_os_ntohl(in->band_index);

    switch(out->type)
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
                adf_os_ntohl(in->data.bs_rssi_xing.inact_rssi_xing);
            out->data.bs_rssi_xing.low_rssi_xing =
                adf_os_ntohl(in->data.bs_rssi_xing.low_rssi_xing);
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

extern a_uint32_t atd_bsteer_pid;
extern struct sock *atd_bsteer_nl_sock;

/**
 * @brief  Band steering events from TARGET, converted to netlink msg
 *
 * @param wifi
 * @param vap
 * @param wmi
 * @param buf
 */
void
atd_event_bsteer_send(atd_host_wifi_t *wifi,
        atd_host_vap_t *vap,
        wmi_id_t wmi,
        adf_nbuf_t buf)
{
    a_uint32_t  len = 0;
    atd_bsteering_event_t *event = NULL;
    adf_nbuf_t  nbuf = NULL;
    struct nlmsghdr *nlh;

    adf_nbuf_peek_header(buf, (a_uint8_t **)&event, &len);

    nbuf = nlmsg_new(sizeof(struct ath_netlink_bsteering_event), GFP_ATOMIC);

    if (nbuf != NULL) {
        nlh = nlmsg_put(nbuf,
                atd_bsteer_pid,
                0,
                adf_os_ntohl(event->type),
                sizeof(struct ath_netlink_bsteering_event),
                0);
        NETLINK_CB(nbuf).pid = 0;        /* from kernel */
        NETLINK_CB(nbuf).dst_group = 0;  /* unicast */
        /* Its time todo ntoh conversion */
        atd_convert_bsteer_event_ntoh(NLMSG_DATA(nlh), event);

        netlink_unicast(atd_bsteer_nl_sock,
                nbuf,
                atd_bsteer_pid,
                MSG_DONTWAIT);
    }
}

a_status_t 
atd_cfg_set_chan(adf_drv_handle_t hdl, a_uint8_t  chan)
{
    atd_host_vap_t    *vap    = (atd_host_vap_t * )hdl;
    atd_host_wifi_t   *wifi   = vap->wifisc;
    a_status_t         status = A_STATUS_OK;
    a_int32_t          resp;
    adf_nbuf_t         nbuf;
    acfg_freq_t       *pfreq;

    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("Start"));

    if ((nbuf = _atd_wmi_msg_alloc(__wifi2wmi(wifi))) == NULL) {
        atd_trace(ATD_DEBUG_CFG, ("Unable to allocate wmi buffer \n"));
        status = A_STATUS_ENOMEM;
        goto fail ;
    }

    pfreq = (acfg_freq_t *)atd_prep_cfg_hdr(nbuf, sizeof(acfg_freq_t), 
                                            wifi, vap);

    pfreq->m = adf_os_htonl(chan);
    pfreq->e = 0;
	
    atd_trace(ATD_DEBUG_CFG, ("SET value m %d , e %d", pfreq->m, pfreq->e));

    status = wmi_cmd_send(__wifi2wmi(wifi), WMI_CMD_FREQ_SET, nbuf, 
            (a_uint8_t *)&resp, sizeof(a_int32_t));

    if (status != A_STATUS_OK) {
        atd_trace(ATD_DEBUG_CFG, (" wmi_cmd_send Failed stat = %x\n", status));
        status = A_STATUS_EINVAL;
     }

    resp = adf_os_ntohl(resp);
    if(resp != 0)
        status = A_STATUS_EINVAL;

fail:
    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("End stat %x ",status ));
    return status;
}

/*
 * Convert MHz frequency to IEEE channel number.
 */
a_uint32_t
__mhz2ieee(a_uint32_t freq)
{
#define IS_CHAN_IN_PUBLIC_SAFETY_BAND(_c) ((_c) > 4940 && (_c) < 4990)
    if (freq == 2484)
        return 14;

    if (freq < 2484)
        return (freq - 2407) / 5;

    if (freq < 5000) {
        if (IS_CHAN_IN_PUBLIC_SAFETY_BAND(freq)) {
            return ((freq * 10) + (((freq % 5) == 2) ? 5 : 0) - 49400)/5;
        } else if (freq > 4900) {
            return (freq - 4000) / 5;
        } else {
            return 15 + ((freq - 2512) / 20);
        }
    }
    return (freq - 5000) / 5;
}

a_status_t 
atd_cfg_get_chan(adf_drv_handle_t hdl, a_uint8_t *chan)
{
    atd_host_vap_t *vap = (atd_host_vap_t *)hdl;
    atd_host_wifi_t *wifi = vap->wifisc;
    a_status_t status = A_STATUS_OK;
    acfg_freq_t resp;
    a_uint32_t freq;
    adf_nbuf_t nbuf;

    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("Start"));

    if ((nbuf = _atd_wmi_msg_alloc(__wifi2wmi(wifi))) == NULL) {
        atd_trace(ATD_DEBUG_CFG, ("Unable to allocate wmi buffer \n"));
        status = A_STATUS_ENOMEM;
        goto fail ;
    }

    atd_prep_cfg_hdr(nbuf, 0, wifi, vap);

    status = wmi_cmd_send(__wifi2wmi(wifi), WMI_CMD_FREQ_GET, nbuf,
            (a_uint8_t *)&resp, sizeof(acfg_freq_t));

    if (status != A_STATUS_OK) {
        atd_trace(ATD_DEBUG_CFG, (" wmi_cmd_send Failed stat = %x\n", status));
        status =  A_STATUS_EINVAL;
        goto fail;
    }

    freq = adf_os_ntohl(resp.m);

    if(adf_os_ntohs(resp.e) == 1)
        *chan = (u_int8_t)__mhz2ieee(freq / 100000);
    else
        *chan = freq;

    atd_trace(ATD_DEBUG_CFG, ("RESULT chan %d ", *chan));

fail:
    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("End stat %x ",status));
    return status;
}

a_status_t 
atd_cfg_set_freq(adf_drv_handle_t hdl, acfg_freq_t *freq)
{
    atd_host_vap_t    *vap    = (atd_host_vap_t * )hdl;
    atd_host_wifi_t   *wifi   = vap->wifisc;
    a_status_t         status = A_STATUS_OK;
    a_int32_t          resp;
    adf_nbuf_t         nbuf;
    acfg_freq_t       *pfreq;

    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("Start"));

    if ((nbuf = _atd_wmi_msg_alloc(__wifi2wmi(wifi))) == NULL) {
        atd_trace(ATD_DEBUG_CFG, ("Unable to allocate wmi buffer \n"));
        status = A_STATUS_ENOMEM;
        goto fail ;
    }

    pfreq = (acfg_freq_t *)atd_prep_cfg_hdr(nbuf, sizeof(acfg_freq_t), 
                                            wifi, vap);

    pfreq->m = adf_os_htonl(freq->m);
    pfreq->e = adf_os_htons(freq->e);
	
    atd_trace(ATD_DEBUG_CFG, ("SET value m %d , e %d", pfreq->m, pfreq->e));

    status = wmi_cmd_send(__wifi2wmi(wifi), WMI_CMD_FREQ_SET, nbuf, 
            (a_uint8_t *)&resp, sizeof(a_int32_t));

    if (status != A_STATUS_OK) {
        atd_trace(ATD_DEBUG_CFG, (" wmi_cmd_send Failed stat = %x\n", status));
        status = A_STATUS_EINVAL;
     }

    resp = adf_os_ntohl(resp);
    if(resp != 0)
        status = A_STATUS_EINVAL;

fail:
    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("End stat %x ",status ));
    return status;
}

a_status_t 
atd_cfg_get_freq(adf_drv_handle_t  hdl, acfg_freq_t *freq)
{
    atd_host_vap_t *vap = (atd_host_vap_t *)hdl;
    atd_host_wifi_t *wifi = vap->wifisc;
    a_status_t status = A_STATUS_OK;
    acfg_freq_t resp;
    adf_nbuf_t nbuf;

    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("Start"));
	
    if ((nbuf = _atd_wmi_msg_alloc(__wifi2wmi(wifi))) == NULL) {
        atd_trace(ATD_DEBUG_CFG, ("Unable to allocate wmi buffer \n"));
        status = A_STATUS_ENOMEM;
        goto fail ;
    }

    atd_prep_cfg_hdr(nbuf, 0, wifi, vap);

    status = wmi_cmd_send(__wifi2wmi(wifi), WMI_CMD_FREQ_GET, nbuf, 
            (a_uint8_t *)&resp, sizeof(acfg_freq_t));

    if (status != A_STATUS_OK) {
        atd_trace(ATD_DEBUG_CFG, (" wmi_cmd_send Failed stat = %x\n", status));
        status =  A_STATUS_EINVAL;
        goto fail;
    }
	
    freq->m = adf_os_ntohl(resp.m);
    freq->e = adf_os_ntohs(resp.e);

    atd_trace(ATD_DEBUG_CFG, ("RESULT m %d , e %d ", freq->m, freq->e));
	
fail:
    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("End stat %x ",status ));
    return status;
}



a_status_t 
atd_cfg_set_rts(adf_drv_handle_t hdl, acfg_rts_t *rts)
{
    atd_host_vap_t    *vap    = (atd_host_vap_t * )hdl;
    atd_host_wifi_t   *wifi   = vap->wifisc;
    a_status_t         status = A_STATUS_OK;
    a_int32_t          resp;
    adf_nbuf_t         nbuf;
    atd_rts_t         *prts;

    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("Start"));

    if ((nbuf = _atd_wmi_msg_alloc(__wifi2wmi(wifi))) == NULL) {
        atd_trace(ATD_DEBUG_CFG, ("Unable to allocate wmi buffer \n"));
        status = A_STATUS_ENOMEM;
        goto fail ;
    }

    prts = (atd_rts_t *)atd_prep_cfg_hdr(nbuf, sizeof(atd_rts_t), 
                                            wifi, vap);

    atd_trace(ATD_DEBUG_CFG, ("SET rts - %d ; flags - 0x%x,", 
					prts->val, prts->flags));

    prts->val = adf_os_htonl(rts->val);
    prts->flags = adf_os_htonl(rts->flags);
	
    status = wmi_cmd_send(__wifi2wmi(wifi), WMI_CMD_RTS_SET, nbuf, 
            (a_uint8_t *)&resp, sizeof(a_int32_t));

    if (status != A_STATUS_OK) {
        atd_trace(ATD_DEBUG_CFG, (" wmi_cmd_send Failed stat = %x\n", status));
        status = A_STATUS_EINVAL;
     }

    resp = adf_os_ntohl(resp);
    if(resp != 0)
        status = A_STATUS_EINVAL;

fail:
    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("End stat %x ",status ));
    return status;
}



a_status_t 
atd_cfg_get_rts(adf_drv_handle_t  hdl, acfg_rts_t *rts)
{
    atd_host_vap_t *vap = (atd_host_vap_t *)hdl;
    atd_host_wifi_t *wifi = vap->wifisc;
    a_status_t status = A_STATUS_OK;
    atd_rts_t resp;
    adf_nbuf_t nbuf;

    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("Start"));
	
    if ((nbuf = _atd_wmi_msg_alloc(__wifi2wmi(wifi))) == NULL) {
        atd_trace(ATD_DEBUG_CFG, ("Unable to allocate wmi buffer \n"));
        status = A_STATUS_ENOMEM;
        goto fail ;
    }

    atd_prep_cfg_hdr(nbuf, 0, wifi, vap);

    status = wmi_cmd_send(__wifi2wmi(wifi), WMI_CMD_RTS_GET, nbuf, 
            (a_uint8_t *)&resp, sizeof(atd_rts_t));

    if (status != A_STATUS_OK) {
        atd_trace(ATD_DEBUG_CFG, (" wmi_cmd_send Failed stat = %x\n", status));
        status =  A_STATUS_EINVAL;
        goto fail;
    }
	
    rts->val = adf_os_ntohl(resp.val);
    rts->flags = adf_os_ntohl(resp.flags);

    atd_trace(ATD_DEBUG_CFG, ("RESULT val %d , flags 0x%04x", \
              rts->val, rts->flags));

fail:
    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("End stat %x ",status ));
    return status;
}



a_status_t 
atd_cfg_set_frag(adf_drv_handle_t hdl, acfg_frag_t *frag)
{
    atd_host_vap_t    *vap    = (atd_host_vap_t * )hdl;
    atd_host_wifi_t   *wifi   = vap->wifisc;
    a_status_t         status = A_STATUS_OK;
    a_int32_t          resp;
    adf_nbuf_t         nbuf;
    atd_frag_t        *pfrag;

    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("Start"));

    if ((nbuf = _atd_wmi_msg_alloc(__wifi2wmi(wifi))) == NULL) {
        atd_trace(ATD_DEBUG_CFG, ("Unable to allocate wmi buffer \n"));
        status = A_STATUS_ENOMEM;
        goto fail ;
    }

    pfrag = (atd_frag_t *)atd_prep_cfg_hdr(nbuf, sizeof(atd_frag_t), 
                                            wifi, vap);

    atd_trace(ATD_DEBUG_CFG, ("SET frag - %d ; flags - 0x%x,", 
					pfrag->val, pfrag->flags));

    pfrag->val = adf_os_htonl(frag->val);
    pfrag->flags = adf_os_htonl(frag->flags);
	
    status = wmi_cmd_send(__wifi2wmi(wifi), WMI_CMD_FRAG_SET, nbuf, 
            (a_uint8_t *)&resp, sizeof(a_int32_t));

    if (status != A_STATUS_OK) {
        atd_trace(ATD_DEBUG_CFG, (" wmi_cmd_send Failed stat = %x\n", status));
        status = A_STATUS_EINVAL;
     }

    resp = adf_os_ntohl(resp);
    if(resp != 0)
        status = A_STATUS_EINVAL;

fail:
    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("End stat %x ",status ));
    return status;
}



a_status_t 
atd_cfg_get_frag(adf_drv_handle_t  hdl, acfg_frag_t *frag)
{
    atd_host_vap_t *vap = (atd_host_vap_t *)hdl;
    atd_host_wifi_t *wifi = vap->wifisc;
    a_status_t status = A_STATUS_OK;
    atd_frag_t resp;
    adf_nbuf_t nbuf;

    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("Start"));
	
    if ((nbuf = _atd_wmi_msg_alloc(__wifi2wmi(wifi))) == NULL) {
        atd_trace(ATD_DEBUG_CFG, ("Unable to allocate wmi buffer \n"));
        status = A_STATUS_ENOMEM;
        goto fail ;
    }

    atd_prep_cfg_hdr(nbuf, 0, wifi, vap);
	
    status = wmi_cmd_send(__wifi2wmi(wifi), WMI_CMD_FRAG_GET, nbuf, 
            (a_uint8_t *)&resp, sizeof(atd_frag_t));

    if (status != A_STATUS_OK) {
        atd_trace(ATD_DEBUG_CFG, (" wmi_cmd_send Failed stat = %x\n", status));
        status =  A_STATUS_EINVAL;
        goto fail;
    }
	
    frag->val = adf_os_ntohl(resp.val);
    frag->flags = adf_os_ntohl(resp.flags);

    atd_trace(ATD_DEBUG_CFG, ("RESULT val %d , flags 0x%04x", 
              frag->val, frag->flags));
	
fail:
    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("End stat %x ",status ));
    return status;
}



a_status_t 
atd_cfg_set_txpow(adf_drv_handle_t hdl, acfg_txpow_t *txpow)
{
    atd_host_vap_t    *vap    = (atd_host_vap_t * )hdl;
    atd_host_wifi_t   *wifi   = vap->wifisc;
    a_status_t         status = A_STATUS_OK;
    a_int32_t          resp;
    adf_nbuf_t         nbuf;
    atd_txpow_t       *ptxpow;

    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("Start"));

    if ((nbuf = _atd_wmi_msg_alloc(__wifi2wmi(wifi))) == NULL) {
        atd_trace(ATD_DEBUG_CFG, ("Unable to allocate wmi buffer \n"));
        status = A_STATUS_ENOMEM;
        goto fail ;
    }

    ptxpow = (atd_txpow_t *)atd_prep_cfg_hdr(nbuf, sizeof(atd_txpow_t), 
                                            wifi, vap);

    atd_trace(ATD_DEBUG_CFG, ("SET txpow - %d ; flags - 0x%x,", 
					txpow->val, txpow->flags));

    ptxpow->val = adf_os_htonl(txpow->val);
    ptxpow->flags = adf_os_htonl(txpow->flags);
	
    status = wmi_cmd_send(__wifi2wmi(wifi), WMI_CMD_TXPOW_SET, nbuf, 
            (a_uint8_t *)&resp, sizeof(a_int32_t));

    if (status != A_STATUS_OK) {
        atd_trace(ATD_DEBUG_CFG, (" wmi_cmd_send Failed stat = %x\n", status));
        status = A_STATUS_EINVAL;
     }

    resp = adf_os_ntohl(resp);
    if(resp != 0)
        status = A_STATUS_EINVAL;

fail:
    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("End stat %x ",status ));
    return status;
}




a_status_t 
atd_cfg_get_txpow(adf_drv_handle_t  hdl, acfg_txpow_t *txpow)
{
    atd_host_vap_t *vap = (atd_host_vap_t *)hdl;
    atd_host_wifi_t *wifi = vap->wifisc;
    a_status_t status = A_STATUS_OK;
    atd_txpow_t resp;
    adf_nbuf_t nbuf;

    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("Start"));
	
    if ((nbuf = _atd_wmi_msg_alloc(__wifi2wmi(wifi))) == NULL) {
        atd_trace(ATD_DEBUG_CFG, ("Unable to allocate wmi buffer \n"));
        status = A_STATUS_ENOMEM;
        goto fail ;
    }

    atd_prep_cfg_hdr(nbuf, 0, wifi, vap);
	
    status = wmi_cmd_send(__wifi2wmi(wifi), WMI_CMD_TXPOW_GET, nbuf, 
            (a_uint8_t *)&resp, sizeof(atd_txpow_t));

    if (status != A_STATUS_OK) {
        atd_trace(ATD_DEBUG_CFG, (" wmi_cmd_send Failed stat = %x\n", status));
        status =  A_STATUS_EINVAL;
        goto fail;
    }
	
    txpow->val = adf_os_ntohl(resp.val);
    txpow->flags = adf_os_ntohl(resp.flags);

    atd_trace(ATD_DEBUG_CFG, ("RESULT val %d , flags 0x%04x", 
              txpow->val, txpow->flags));
	
fail:
    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("End stat %x ",status ));
    return status;
}

a_status_t 
atd_cfg_set_encode(adf_drv_handle_t  hdl, acfg_encode_t *encode)
{
    atd_host_vap_t *vap = (atd_host_vap_t *)hdl;
    atd_host_wifi_t *wifi = vap->wifisc;
    a_status_t status = A_STATUS_OK;
    adf_nbuf_t nbuf;
    atd_encode_rsp_t *pkey;
    a_int32_t          resp;

    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("Start"));
    if ((nbuf = _atd_wmi_msg_alloc(__wifi2wmi(wifi))) == NULL) {
        atd_trace(ATD_DEBUG_CFG, ("Unable to allocate wmi buffer \n"));
        status = A_STATUS_ENOMEM;
        goto fail ;
    }

    pkey = (atd_encode_rsp_t *)atd_prep_cfg_hdr(nbuf, 
                                                sizeof(atd_encode_rsp_t), 
                                                wifi, vap);
	
    pkey->flag = adf_os_htonl(encode->flags);
    pkey->len = adf_os_htonl(encode->len);
	if (encode->len)	 {
		adf_os_mem_copy(pkey->key, encode->buff, ACFG_ENCODING_TOKEN_MAX);
	}	
    
    atd_trace(ATD_DEBUG_CFG, ("SET len %d , flags 0x%04x", \
              pkey->len, pkey->flag));
    status = wmi_cmd_send(__wifi2wmi(wifi), WMI_CMD_ENCODE_SET, nbuf, 
            (a_uint8_t *)&resp, sizeof(a_int32_t));

    if (status != A_STATUS_OK) {
        atd_trace(ATD_DEBUG_CFG, (" wmi_cmd_send Failed stat = %x\n", status));
        status = A_STATUS_EINVAL;
        goto fail;
    }

	resp = adf_os_ntohl(resp);
	if(resp == -1)
		status = A_STATUS_EINVAL;

fail:
	atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("End stat %x ",status ));
	return status;
}

a_status_t 
atd_cfg_get_encode(adf_drv_handle_t  hdl, acfg_encode_t *encode)
{
    atd_host_vap_t *vap = (atd_host_vap_t *)hdl;
    atd_host_wifi_t *wifi = vap->wifisc;
    a_status_t status = A_STATUS_OK;
    adf_nbuf_t nbuf;
    atd_encode_rsp_t key;

    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("Start"));
	
    if ((nbuf = _atd_wmi_msg_alloc(__wifi2wmi(wifi))) == NULL) {
        atd_trace(ATD_DEBUG_CFG, ("Unable to allocate wmi buffer \n"));
        status = A_STATUS_ENOMEM;
        goto fail ;
    }

    atd_prep_cfg_hdr(nbuf, 0, wifi, vap);
	
    status = wmi_cmd_send(__wifi2wmi(wifi), WMI_CMD_ENCODE_GET, nbuf, 
            (a_uint8_t *)&key, sizeof(atd_encode_rsp_t));

    if (status != A_STATUS_OK) {
        atd_trace(ATD_DEBUG_CFG, (" wmi_cmd_send Failed stat = %x\n", status));
        status =  A_STATUS_EINVAL;
        goto fail;
    }

    encode->len = adf_os_ntohl(key.len);
    encode->flags = adf_os_ntohl(key.flag);
    adf_os_mem_copy(encode->buff, key.key, ACFG_ENCODING_TOKEN_MAX);

    atd_trace(ATD_DEBUG_CFG, ("RESULT len %d , flags 0x%04x", \
              encode->len, encode->flags));
	
fail:
    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("End stat %x ",status ));
    return status;
}

a_status_t 
atd_cfg_get_stats(adf_drv_handle_t  hdl, acfg_stats_t *stat)
{
    atd_host_vap_t *vap = (atd_host_vap_t *)hdl;
    atd_host_wifi_t *wifi = vap->wifisc;
    a_status_t status = A_STATUS_OK;
    adf_nbuf_t nbuf;
    atd_stats_t resp;

    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("Start"));

    if ((nbuf = _atd_wmi_msg_alloc(__wifi2wmi(wifi))) == NULL) {
        atd_trace(ATD_DEBUG_CFG, ("Unable to allocate wmi buffer \n"));
        status = A_STATUS_ENOMEM;
        goto fail ;
    }

    atd_prep_cfg_hdr(nbuf, 0, wifi, vap);
	
    status = wmi_cmd_send(__wifi2wmi(wifi), WMI_CMD_STATS_GET, nbuf, 
            (a_uint8_t *)&resp, sizeof(atd_stats_t));

    if (status != A_STATUS_OK) {
        atd_trace(ATD_DEBUG_CFG, (" wmi_cmd_send Failed stat = %x\n", status));
        status =  A_STATUS_EINVAL;
        goto fail;
    }

    stat->status = adf_os_ntohl(resp.status);
    stat->link_quality = adf_os_ntohl(resp.link_quality);
    stat->signal_level = adf_os_ntohl(resp.signal_level);
    stat->noise_level = adf_os_ntohl(resp.noise_level);
    stat->updated = adf_os_ntohl(resp.updated);
    stat->discard_nwid = adf_os_ntohl(resp.discard_nwid);
    stat->discard_code = adf_os_ntohl(resp.discard_code);
    stat->missed_beacon = adf_os_ntohl(resp.missed_beacon);
	
fail:
    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("End stat %x ",status ));
    return status;
}

/* security cfgs */
a_status_t
atd_cfg_wsupp_generic_request(adf_drv_handle_t hdl, wmi_id_t wid, 
        void *data, a_uint32_t data_len,
        void *reply, a_uint32_t reply_len)
{
    atd_host_vap_t *vap = (atd_host_vap_t * )hdl;
    atd_host_wifi_t *wifi = vap->wifisc;
    a_status_t status = A_STATUS_OK;
    adf_nbuf_t nbuf;
    void *dataptr;

    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("wid: %d len: %d %d", \
                wid, data_len, reply_len));

    if ((nbuf = _atd_wmi_msg_alloc(__wifi2wmi(wifi))) == NULL) {
        atd_trace(ATD_DEBUG_CFG, ("Unable to allocate wmi buffer \n"));
        status = A_STATUS_ENOMEM;
        goto fail ;
    }

    dataptr = (acfg_wsupp_info_t *)atd_prep_cfg_hdr(nbuf, data_len, wifi, vap);

    memcpy(dataptr, data, data_len);
	
    status = wmi_cmd_send(__wifi2wmi(wifi), wid, nbuf,
                          (a_uint8_t *) reply, reply_len);


    if (status != A_STATUS_OK){
        atd_trace(ATD_DEBUG_CFG, (" wmi_cmd_send Failed stat = %x\n", status));
        status = A_STATUS_EINVAL;
        goto fail;
    }

fail:    
    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("End stat %x ",status ));
    return status;

}

a_status_t 
atd_cfg_wsupp_init(adf_drv_handle_t  hdl, acfg_wsupp_info_t *winfo)
{
    u_int32_t retval;
    a_status_t status = A_STATUS_OK;
    status = atd_cfg_wsupp_generic_request(hdl, WMI_CMD_WSUPP_INIT, 
        (void *) winfo, sizeof(acfg_wsupp_info_t),
        (void *) &retval, sizeof(retval));
	retval = adf_os_ntohl(retval);
	atd_trace(ATD_DEBUG_CFG, ("%s: retval: %d\n", __func__, retval));
    return status;
}

a_status_t 
atd_cfg_wsupp_fini(adf_drv_handle_t  hdl, acfg_wsupp_info_t *winfo)
{
    u_int32_t retval;
    a_status_t status = A_STATUS_OK;
    status = atd_cfg_wsupp_generic_request(hdl, WMI_CMD_WSUPP_FINI, 
        (void *) winfo, sizeof(acfg_wsupp_info_t),
        (void *) &retval, sizeof(retval));
	retval = adf_os_ntohl(retval);
	atd_trace(ATD_DEBUG_CFG, ("%s: retval: %d\n", __func__, retval));
    return status;
}

a_status_t 
atd_cfg_wsupp_if_add(adf_drv_handle_t hdl, acfg_wsupp_info_t *winfo)
{
    u_int32_t retval;
    a_status_t status = A_STATUS_OK;
    status = atd_cfg_wsupp_generic_request(hdl, WMI_CMD_WSUPP_ADD_IF, 
        (void *) winfo, sizeof(acfg_wsupp_info_t),
        (void *) &retval, sizeof(retval));
    retval = adf_os_ntohl(retval);
    atd_trace(ATD_DEBUG_CFG, ("%s: retval: %d\n", __func__, retval));
    return status;
}

a_status_t 
atd_cfg_wsupp_if_remove(adf_drv_handle_t hdl, acfg_wsupp_info_t *winfo)
{
    u_int32_t retval;
    a_status_t status = A_STATUS_OK;
    status = atd_cfg_wsupp_generic_request(hdl, WMI_CMD_WSUPP_REMOVE_IF, 
        (void *) winfo, sizeof(acfg_wsupp_info_t),
        (void *) &retval, sizeof(retval));
    retval = adf_os_ntohl(retval);
    atd_trace(ATD_DEBUG_CFG, ("%s: retval: %d\n", __func__, retval));
    return status;
}

a_status_t 
atd_cfg_wsupp_nw_create(adf_drv_handle_t hdl, acfg_wsupp_info_t *winfo)
{
    u_int32_t retval;
    a_status_t status = A_STATUS_OK;
    status = atd_cfg_wsupp_generic_request(hdl, WMI_CMD_WSUPP_ADD_NW, 
        (void *) winfo, sizeof(acfg_wsupp_info_t),
        (void *) &retval, sizeof(retval));
    winfo->u.nw_cfg.networkid = adf_os_ntohl(retval);
    atd_trace(ATD_DEBUG_CFG, ("%s: retval: %d\n", __func__, retval));
    return status;
}

a_status_t 
atd_cfg_wsupp_nw_delete(adf_drv_handle_t hdl, acfg_wsupp_info_t *winfo)
{
    u_int32_t retval;
    a_status_t status = A_STATUS_OK;

    winfo->u.nw_cfg.networkid = adf_os_htonl(winfo->u.nw_cfg.networkid);
    status = atd_cfg_wsupp_generic_request(hdl, WMI_CMD_WSUPP_REMOVE_NW, 
        (void *) winfo, sizeof(acfg_wsupp_info_t),
        (void *) &retval, sizeof(retval));
    retval = adf_os_ntohl(retval);
    atd_trace(ATD_DEBUG_CFG, ("%s: retval: %d\n", __func__, retval));
    return status;
}

a_status_t 
atd_cfg_wsupp_nw_set(adf_drv_handle_t hdl, acfg_wsupp_info_t *winfo)
{
    u_int32_t retval;
    a_status_t status = A_STATUS_OK;

    winfo->u.nw_cfg.item = adf_os_htonl(winfo->u.nw_cfg.item);
    winfo->u.nw_cfg.networkid = adf_os_htonl(winfo->u.nw_cfg.networkid);
    status = atd_cfg_wsupp_generic_request(hdl, WMI_CMD_WSUPP_SET_NETWORK, 
        (void *) winfo, sizeof(acfg_wsupp_info_t),
        (void *) &retval, sizeof(retval));
    retval = adf_os_ntohl(retval);
    atd_trace(ATD_DEBUG_CFG, ("%s: retval: %d\n", __func__, retval));
    return status;
}

a_status_t 
atd_cfg_wsupp_nw_get(adf_drv_handle_t hdl, acfg_wsupp_info_t *winfo)
{
    acfg_wsupp_get_reply_t retval;
    a_status_t status = A_STATUS_OK;

    winfo->u.nw_cfg.item = adf_os_htonl(winfo->u.nw_cfg.item);
    winfo->u.nw_cfg.networkid = adf_os_htonl(winfo->u.nw_cfg.networkid);
    status = atd_cfg_wsupp_generic_request(hdl, WMI_CMD_WSUPP_GET_NETWORK, 
        (void *) winfo, sizeof(acfg_wsupp_info_t),
        (void *) &retval, sizeof(retval));
    memcpy(winfo->u.nw_cfg.reply, retval.reply, retval.len);
	winfo->u.nw_cfg.reply[retval.len] = '\0';
    winfo->u.nw_cfg.reply_len = retval.len;
    return status;
}

a_status_t 
atd_cfg_wsupp_nw_list(adf_drv_handle_t hdl, acfg_wsupp_info_t *winfo)
{
    acfg_wsupp_get_reply_t retval;
    a_status_t status = A_STATUS_OK;

    status = atd_cfg_wsupp_generic_request(hdl, WMI_CMD_WSUPP_LIST_NETWORK, 
        (void *) winfo, sizeof(acfg_wsupp_info_t),
        (void *) &retval, sizeof(retval));
    memcpy(winfo->u.nw_cfg.reply, retval.reply, retval.len);
    winfo->u.nw_cfg.reply[retval.len] = '\0';
    winfo->u.nw_cfg.reply_len = retval.len;
    return status;
}

a_status_t 
atd_cfg_wsupp_wps_req(adf_drv_handle_t hdl, acfg_wsupp_info_t *winfo)
{
    a_uint8_t retval[ACFG_WSUPP_REPLY_LEN];
    a_status_t status = A_STATUS_OK;

    winfo->u.wps_cfg.type = adf_os_htonl(winfo->u.wps_cfg.type);
    status = atd_cfg_wsupp_generic_request(hdl, WMI_CMD_WSUPP_SET_WPS, 
        (void *) winfo, sizeof(acfg_wsupp_info_t),
        (void *) &retval, sizeof(retval));
    memcpy(winfo->u.wps_cfg.reply, retval, strlen(retval));
    winfo->u.wps_cfg.reply_len = strlen(retval);
    return status;
}

a_status_t 
atd_cfg_wsupp_set(adf_drv_handle_t hdl, acfg_wsupp_info_t *winfo)
{
    u_int32_t retval;
    a_status_t status = A_STATUS_OK;

    winfo->u.rt_cfg.type = adf_os_htonl(winfo->u.rt_cfg.type);
    status = atd_cfg_wsupp_generic_request(hdl, WMI_CMD_WSUPP_SET, 
        (void *) winfo, sizeof(acfg_wsupp_info_t),
        (void *) &retval, sizeof(retval));
    retval = adf_os_ntohl(retval);
    atd_trace(ATD_DEBUG_CFG, ("%s: retval: %d\n", __func__, retval));
    return status;
}

static a_status_t 
atd_cfg_set_reg(adf_drv_handle_t hdl, a_uint32_t offset, a_uint32_t value)
{
    atd_host_wifi_t    *wifi  = (atd_host_wifi_t * )hdl;
    a_status_t         status = A_STATUS_OK;
    adf_nbuf_t         nbuf;
    atd_param_t       *wifi_param;
    atd_hdr_t         *hdrp;
    a_uint32_t         len = sizeof(atd_hdr_t) +  sizeof(atd_param_t);

    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("offset  %x value %x ", \
              offset, value));


    if ((nbuf = _atd_wmi_msg_alloc(__wifi2wmi(wifi))) == NULL) {
        atd_trace(ATD_DEBUG_CFG, ("Unable to allocate wmi buffer \n"));
        status = A_STATUS_ENOMEM;
        goto fail ;
    }

    hdrp                  =  (atd_hdr_t *)adf_nbuf_put_tail(nbuf, len);
    hdrp->wifi_index      =   wifi->wifi_index;
    hdrp->vap_index       =   -1;

    wifi_param            =  (atd_param_t *)(hdrp + 1);

    wifi_param->param     =   adf_os_htonl(offset);
    wifi_param->val       =   adf_os_htonl(value);

    status = wmi_cmd_send(__wifi2wmi(wifi), WMI_CMD_REG_SET, nbuf, NULL, 0);

fail:
    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("End stat %x ",status));
    return status;
}

static a_status_t 
atd_cfg_get_reg(adf_drv_handle_t hdl, a_uint32_t offset, a_uint32_t *value)
{
    atd_host_wifi_t    *wifi  = (atd_host_wifi_t * )hdl;
    a_status_t         status = A_STATUS_OK;
    adf_nbuf_t         nbuf;
    atd_param_t       *wifi_param;
    atd_param_t        paramresult;
    atd_hdr_t         *hdrp;
    a_uint32_t         len = sizeof(atd_hdr_t) +  sizeof(atd_param_t);

    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("offset %x ",offset));

    if ((nbuf = _atd_wmi_msg_alloc(__wifi2wmi(wifi))) == NULL) {
        atd_trace(ATD_DEBUG_CFG, ("Unable to allocate wmi buffer \n"));
        status = A_STATUS_ENOMEM;
        goto fail ;
    }

    hdrp                  =  (atd_hdr_t *)adf_nbuf_put_tail(nbuf, len);
    hdrp->wifi_index      =   wifi->wifi_index;
    hdrp->vap_index       =   -1;

    wifi_param            =  (atd_param_t *)(hdrp + 1);

    wifi_param->param     =   adf_os_htonl(offset);
    wifi_param->val       =   0;


    status = wmi_cmd_send(__wifi2wmi(wifi), WMI_CMD_REG_GET, nbuf,
                          (a_uint8_t *)&paramresult, sizeof(atd_param_t));

    paramresult.param = adf_os_ntohl(paramresult.param);
    *value = adf_os_ntohl(paramresult.val);

    atd_trace(ATD_DEBUG_CFG, ("val   %x ", value[0]));

fail:
    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("End stat %x ",status));
   return status;
}

static a_status_t 
atd_cfg_set_hwaddr(adf_drv_handle_t hdl, acfg_macaddr_t *mac)
{
    atd_host_wifi_t   *wifi   = (atd_host_wifi_t * )hdl;
    a_status_t         status = A_STATUS_OK;
    adf_nbuf_t         nbuf;
    acfg_macaddr_t    *pmac;
    a_int32_t          resp;
    atd_hdr_t         *hdrp;
    a_uint32_t         len = sizeof(atd_hdr_t) + sizeof(acfg_macaddr_t);

    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("Start"));
    if ((nbuf = _atd_wmi_msg_alloc(__wifi2wmi(wifi))) == NULL) {
        atd_trace(ATD_DEBUG_CFG, ("Unable to allocate wmi buffer \n"));
        status = A_STATUS_ENOMEM;
        goto fail ;
    }

    hdrp                  =  (atd_hdr_t *)adf_nbuf_put_tail(nbuf, len);
    hdrp->wifi_index      =   wifi->wifi_index;
    hdrp->vap_index       =   -1;

    pmac                  =  (acfg_macaddr_t *)(hdrp + 1);
    memcpy(pmac, mac->addr, ACFG_MACADDR_LEN);

    status = wmi_cmd_send(__wifi2wmi(wifi), WMI_CMD_HWADDR_SET, nbuf,
                          (a_uint8_t *)&resp, sizeof(a_int32_t));

    if (status != A_STATUS_OK) {
        atd_trace(ATD_DEBUG_CFG, (" wmi_cmd_send Failed stat = %x\n", status));
        status = A_STATUS_EINVAL;
        goto fail;
    }

    resp = adf_os_ntohl(resp);
    if(resp == -1)
        status = A_STATUS_EINVAL;

fail:
    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("End stat %x ",status));
    return status;
}

static a_status_t 
atd_cfg_set_vap_wmmparams(adf_drv_handle_t hdl, a_uint32_t *param, a_uint32_t value)
{
    atd_host_vap_t    *vap    = (atd_host_vap_t * )hdl;
    atd_host_wifi_t     *wifi   = vap->wifisc;
    a_status_t         status = A_STATUS_OK;
    adf_nbuf_t         nbuf;
    atd_wmmparams_t    *vap_param;

    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("param = %d",*(a_uint32_t *)param ));

    if ((nbuf = _atd_wmi_msg_alloc(__wifi2wmi(wifi))) == NULL) {
        atd_trace(ATD_DEBUG_CFG, ("Unable to allocate wmi buffer \n"));
        status = A_STATUS_ENOMEM;
        goto fail ;
    }

    vap_param     = (atd_wmmparams_t *)atd_prep_cfg_hdr(nbuf, sizeof(atd_wmmparams_t), 
                                                 wifi, vap);
    vap_param->param[0]     =   adf_os_htonl(*(a_uint32_t *)param);
	vap_param->param[1]     =   adf_os_htonl(*((a_uint32_t *)param+1));
	vap_param->param[2]     =   adf_os_htonl(*((a_uint32_t *)param+2));
	vap_param->val       	=   adf_os_htonl(value);

    status = wmi_cmd_send(__wifi2wmi(wifi), WMI_CMD_WMMPARAMS_SET, nbuf,
                          NULL, 0);
fail:

    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("End stat %x ",status ));
    return status;
}

static a_status_t 
atd_cfg_get_vap_wmmparams(adf_drv_handle_t hdl, a_uint32_t *param, a_uint32_t *value)
{
    atd_host_vap_t    *vap    = (atd_host_vap_t * )hdl;
    atd_host_wifi_t     *wifi   = vap->wifisc;
    a_status_t         status = A_STATUS_OK;
    adf_nbuf_t         nbuf;
    atd_wmmparams_t    *vap_param;
    atd_param_t        paramresult;


    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("param = %d",*(a_uint32_t *)param ));

    if ((nbuf = _atd_wmi_msg_alloc(__wifi2wmi(wifi))) == NULL) {
        atd_trace(ATD_DEBUG_CFG, ("Unable to allocate wmi buffer \n"));
        status = A_STATUS_ENOMEM;
        goto fail ;
    }

    vap_param     = (atd_wmmparams_t *)atd_prep_cfg_hdr(nbuf, sizeof(atd_wmmparams_t), 
                                                 wifi, vap);
    vap_param->param[0]     =   adf_os_htonl(*(a_uint32_t *)param);
	vap_param->param[1]     =   adf_os_htonl(*((a_uint32_t *)param+1));
	vap_param->param[2]     =   adf_os_htonl(*((a_uint32_t *)param+2));
    vap_param->val       =   0;

    status = wmi_cmd_send(__wifi2wmi(wifi), WMI_CMD_WMMPARAMS_GET, nbuf,
                          (a_uint8_t *)&paramresult, sizeof(atd_param_t));


    paramresult.param = adf_os_ntohl(paramresult.param );

    value[0] = adf_os_ntohl(paramresult.val);
    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("val   %d ",value[0] ));

fail:
    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("End stat %x ",status ));
    return status;
}

static a_status_t atd_cfg_nawds_config(adf_drv_handle_t hdl, 
                                        acfg_nawds_cfg_t *nawds_conf)
{

    atd_host_vap_t     *vap    = (atd_host_vap_t * )hdl;
    atd_host_wifi_t *wifi   = vap->wifisc;
    atd_nawds_cfg_t         *pld, result = {0};
    a_status_t          status = A_STATUS_OK;
    adf_nbuf_t          nbuf;

    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("nawds cmd %d ", \
                nawds_conf->cmdtype));


    switch(nawds_conf->cmdtype)
    {
        case NAWDS_SET_MODE:
        case NAWDS_SET_DEFCAPS:
        case NAWDS_SET_OVERRIDE:
        case NAWDS_SET_ADDR:
        case NAWDS_CLR_ADDR:
        case NAWDS_GET:
            if ((nbuf = _atd_wmi_msg_alloc(__wifi2wmi(wifi))) == NULL) {
                atd_trace(ATD_DEBUG_CFG, ("Unable to allocate wmi buffer \n"));
                status = A_STATUS_ENOMEM;
                goto fail ;
            }
            pld = (atd_nawds_cfg_t *)atd_prep_cfg_hdr(nbuf, 
                                                 sizeof(atd_nawds_cfg_t), 
                                                 wifi, vap);
            pld->cmdtype = adf_os_htonl(nawds_conf->cmdtype);
            pld->data.nawds.num = nawds_conf->data.nawds.num;
            pld->data.nawds.mode = nawds_conf->data.nawds.mode;
            pld->data.nawds.defcaps = nawds_conf->data.nawds.defcaps;
            pld->data.nawds.override = nawds_conf->data.nawds.override;
            adf_os_mem_copy(pld->data.nawds.mac, 
                            nawds_conf->data.nawds.mac, ACFG_MACADDR_LEN);
            pld->data.nawds.caps = nawds_conf->data.nawds.caps;
            if(nawds_conf->cmdtype == NAWDS_GET)
            {
                status = wmi_cmd_send(__wifi2wmi(wifi), WMI_CMD_NAWDS_CONFIG, 
                        nbuf,(a_uint8_t*)&result, sizeof(atd_nawds_cfg_t));
                nawds_conf->cmdtype = adf_os_ntohl(result.cmdtype);
                nawds_conf->status = adf_os_ntohl(result.status);
                nawds_conf->data.nawds.num = result.data.nawds.num;
                nawds_conf->data.nawds.mode = result.data.nawds.mode;
                nawds_conf->data.nawds.defcaps = result.data.nawds.defcaps;
                nawds_conf->data.nawds.override = result.data.nawds.override;
                adf_os_mem_copy(nawds_conf->data.nawds.mac, 
                                result.data.nawds.mac, ACFG_MACADDR_LEN);
                nawds_conf->data.nawds.caps = result.data.nawds.caps;
            }
            else
            {
                status = wmi_cmd_send(__wifi2wmi(wifi), WMI_CMD_NAWDS_CONFIG, 
                        nbuf, NULL, 0);
            }
            break;
        
        case WNM_SET_BSSMAX:
        case WNM_GET_BSSMAX:
        case WNM_TFS_ADD:
        case WNM_TFS_DELETE:
        case WNM_SET_TIMBCAST:
        case WNM_GET_TIMBCAST:
            if ((nbuf = _atd_wmi_msg_alloc(__wifi2wmi(wifi))) == NULL) {
                atd_trace(ATD_DEBUG_CFG, ("Unable to allocate wmi buffer \n"));
                status = A_STATUS_ENOMEM;
                goto fail ;
            }
            pld = (atd_nawds_cfg_t *)atd_prep_cfg_hdr(nbuf, 
                                                 sizeof(atd_nawds_cfg_t), 
                                                 wifi, vap);
            pld->cmdtype = adf_os_htonl(nawds_conf->cmdtype);
            adf_os_mem_copy(&pld->data.wnm, &nawds_conf->data.wnm, sizeof(struct acfg_wnm_cfg));
            if(nawds_conf->cmdtype == WNM_GET_BSSMAX 
                || nawds_conf->cmdtype == WNM_GET_TIMBCAST)
            {
                status = wmi_cmd_send(__wifi2wmi(wifi), WMI_CMD_NAWDS_CONFIG, 
                        nbuf,(a_uint8_t*)&result, sizeof(atd_nawds_cfg_t));
                nawds_conf->cmdtype = adf_os_ntohl(result.cmdtype);
                nawds_conf->status = adf_os_ntohl(result.status);
                adf_os_mem_copy(&nawds_conf->data.wnm, &result.data.wnm, sizeof(struct acfg_wnm_cfg));
            }
            else
            {
                status = wmi_cmd_send(__wifi2wmi(wifi), WMI_CMD_NAWDS_CONFIG, 
                        nbuf, NULL, 0);
            }
            break;
        
        default:
            atd_trace(ATD_DEBUG_CFG, (" Invalid Nawds Command \n"));
            status = A_STATUS_EINVAL;
            goto fail;

    }

fail:
    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("End stat %x ",status ));
    return status;
}


static a_status_t 
atd_cfg_doth_chsw(adf_drv_handle_t hdl, acfg_doth_chsw_t *chsw)
{
    atd_host_vap_t *vap = (atd_host_vap_t *)hdl;
    atd_host_wifi_t *wifi = vap->wifisc;
    a_status_t         status = A_STATUS_OK;
    adf_nbuf_t         nbuf;
    acfg_doth_chsw_t   *pchsw;
    a_int32_t          resp;
    atd_hdr_t         *hdrp;
    a_uint32_t         len = sizeof(atd_hdr_t) + sizeof(acfg_doth_chsw_t);

    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("Start"));
    if ((nbuf = _atd_wmi_msg_alloc(__wifi2wmi(wifi))) == NULL) {
        atd_trace(ATD_DEBUG_CFG, ("Unable to allocate wmi buffer \n"));
        status = A_STATUS_ENOMEM;
        goto fail ;
    }

    hdrp                  =  (atd_hdr_t *)adf_nbuf_put_tail(nbuf, len);
    hdrp->wifi_index      =   wifi->wifi_index;
    hdrp->vap_index       =   vap->vap_index;

    pchsw                 =  (acfg_doth_chsw_t *)(hdrp + 1);
    pchsw->channel        = adf_os_htonl(chsw->channel) ;
    pchsw->time           = adf_os_htonl(chsw->time) ;

    status = wmi_cmd_send(__wifi2wmi(wifi), WMI_CMD_DOTH_CHSW, nbuf,
                          (a_uint8_t *)&resp, sizeof(acfg_doth_chsw_t));

    if (status != A_STATUS_OK) {
        atd_trace(ATD_DEBUG_CFG, (" wmi_cmd_send Failed stat = %x\n", status));
        status = A_STATUS_EINVAL;
        goto fail;
    }

    resp = adf_os_ntohl(resp);
    if(resp == -1)
        status = A_STATUS_EINVAL;

fail:
    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("End stat %x ",status));
    return status;
}



/** 
 * @brief 
 * 
 * @param hdl
 * @param addr
 * 
 * @return 
 */
static a_status_t 
atd_cfg_addmac(adf_drv_handle_t hdl, acfg_macaddr_t *addr)
{
    atd_host_vap_t *vap = (atd_host_vap_t *)hdl;
    atd_host_wifi_t *wifi = vap->wifisc;
    a_status_t         status = A_STATUS_OK;
    adf_nbuf_t         nbuf;
    acfg_macaddr_t   *paddr;
    a_int32_t          resp;
    atd_hdr_t         *hdrp;
    a_uint32_t         len = sizeof(atd_hdr_t) + sizeof(acfg_macaddr_t);

    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("Start"));
    if ((nbuf = _atd_wmi_msg_alloc(__wifi2wmi(wifi))) == NULL) {
        atd_trace(ATD_DEBUG_CFG, ("Unable to allocate wmi buffer \n"));
        status = A_STATUS_ENOMEM;
        goto fail ;
    }

    hdrp                  =  (atd_hdr_t *)adf_nbuf_put_tail(nbuf, len);
    hdrp->wifi_index      =   wifi->wifi_index;
    hdrp->vap_index       =   vap->vap_index;

    paddr                 =  (acfg_macaddr_t *)(hdrp + 1);
    memcpy(paddr, addr, sizeof(acfg_macaddr_t));

    status = wmi_cmd_send(__wifi2wmi(wifi), WMI_CMD_ADDMAC, nbuf,
                          (a_uint8_t *)&resp, sizeof(acfg_macaddr_t));

    if (status != A_STATUS_OK) {
        atd_trace(ATD_DEBUG_CFG, (" wmi_cmd_send Failed stat = %x\n", status));
        status = A_STATUS_EINVAL;
        goto fail;
    }

    resp = adf_os_ntohl(resp);
    if(resp == -1)
        status = A_STATUS_EINVAL;

fail:
    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("End stat %x ",status));
    return status;
}




/** 
 * @brief 
 * 
 * @param hdl
 * @param addr
 * 
 * @return 
 */
static a_status_t 
atd_cfg_delmac(adf_drv_handle_t hdl, acfg_macaddr_t *addr)
{
    atd_host_vap_t *vap = (atd_host_vap_t *)hdl;
    atd_host_wifi_t *wifi = vap->wifisc;
    a_status_t         status = A_STATUS_OK;
    adf_nbuf_t         nbuf;
    acfg_macaddr_t   *paddr;
    a_int32_t          resp;
    atd_hdr_t         *hdrp;
    a_uint32_t         len = sizeof(atd_hdr_t) + sizeof(acfg_macaddr_t);

    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("Start"));
    if ((nbuf = _atd_wmi_msg_alloc(__wifi2wmi(wifi))) == NULL) {
        atd_trace(ATD_DEBUG_CFG, ("Unable to allocate wmi buffer \n"));
        status = A_STATUS_ENOMEM;
        goto fail ;
    }

    hdrp                  =  (atd_hdr_t *)adf_nbuf_put_tail(nbuf, len);
    hdrp->wifi_index      =   wifi->wifi_index;
    hdrp->vap_index       =   vap->vap_index;

    paddr                 =  (acfg_macaddr_t *)(hdrp + 1);
    memcpy(paddr, addr, sizeof(acfg_macaddr_t));

    status = wmi_cmd_send(__wifi2wmi(wifi), WMI_CMD_DELMAC, nbuf,
                          (a_uint8_t *)&resp, sizeof(resp));

    if (status != A_STATUS_OK) {
        atd_trace(ATD_DEBUG_CFG, (" wmi_cmd_send Failed stat = %x\n", status));
        status = A_STATUS_EINVAL;
        goto fail;
    }

    resp = adf_os_ntohl(resp);
    if(resp == -1)
        status = A_STATUS_EINVAL;

fail:
    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("End stat %x ",status));
    return status;
}




/** 
 * @brief 
 * 
 * @param hdl
 * @param addr
 * 
 * @return 
 */
static a_status_t 
atd_cfg_kickmac(adf_drv_handle_t hdl, acfg_macaddr_t *addr)
{
    atd_host_vap_t *vap = (atd_host_vap_t *)hdl;
    atd_host_wifi_t *wifi = vap->wifisc;
    a_status_t         status = A_STATUS_OK;
    adf_nbuf_t         nbuf;
    acfg_macaddr_t   *paddr;
    a_int32_t          resp;
    atd_hdr_t         *hdrp;
    a_uint32_t         len = sizeof(atd_hdr_t) + sizeof(acfg_macaddr_t);


    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("Start"));
    if ((nbuf = _atd_wmi_msg_alloc(__wifi2wmi(wifi))) == NULL) {
        atd_trace(ATD_DEBUG_CFG, ("Unable to allocate wmi buffer \n"));
        status = A_STATUS_ENOMEM;
        goto fail ;
    }

    hdrp                  =  (atd_hdr_t *)adf_nbuf_put_tail(nbuf, len);
    hdrp->wifi_index      =   wifi->wifi_index;
    hdrp->vap_index       =   vap->vap_index;

    paddr                 =  (acfg_macaddr_t *)(hdrp + 1);
    memcpy(paddr, addr, sizeof(acfg_macaddr_t));

    status = wmi_cmd_send(__wifi2wmi(wifi), WMI_CMD_KICKMAC, nbuf,
                          (a_uint8_t *)&resp, sizeof(resp));

    if (status != A_STATUS_OK) {
        atd_trace(ATD_DEBUG_CFG, (" wmi_cmd_send Failed stat = %x\n", status));
        status = A_STATUS_EINVAL;
        goto fail;
    }

    resp = adf_os_ntohl(resp);
    if(resp == -1)
        status = A_STATUS_EINVAL;

fail:
    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("End stat %x ",status));
    return status;
}


/** 
 * @brief Get Channel Info
 * 
 * @param hdl
 * @param mode
 * 
 * @return 
 */
a_status_t 
atd_cfg_get_chan_info(adf_drv_handle_t hdl, acfg_chan_info_t *chan_info)
{
    atd_host_vap_t     *vap    = (atd_host_vap_t *)hdl;
    atd_host_wifi_t    *wifi   = vap->wifisc;
    a_status_t          status = A_STATUS_OK;
    adf_nbuf_t          nbuf;
    atd_chan_info_t    *chan;
    a_uint32_t          i = 0;

    chan = adf_os_mem_alloc(NULL, sizeof(atd_chan_info_t));
    if (!chan) {
        atd_trace(ATD_DEBUG_ERROR, ("Unable to allocate response buffer \n"));
        status = A_STATUS_ENOMEM;
        goto fail;
    }

    if ((nbuf = _atd_wmi_msg_alloc(__wifi2wmi(wifi))) == NULL) {
        atd_trace(ATD_DEBUG_CFG, ("Unable to allocate wmi buffer \n"));
        status = A_STATUS_ENOMEM;
        goto fail_free;
    }

    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), 
                                ("chan_info - %p ", chan_info));

    atd_prep_cfg_hdr(nbuf, 0, wifi, vap);

    status = wmi_cmd_send(__wifi2wmi(wifi), WMI_CMD_CHAN_INFO_GET, nbuf, 
                          (a_uint8_t *)chan, sizeof(atd_chan_info_t));

    if (status == A_STATUS_OK) {
        chan_info->num_channels = adf_os_ntohl(chan->num_channels);
        for (i = 0; i < chan_info->num_channels; i++) {
            chan_info->channel[i].freq = adf_os_ntohs(chan->channel[i].freq);
            chan_info->channel[i].flags = adf_os_ntohl(chan->channel[i].flags);
            chan_info->channel[i].flag_ext = chan->channel[i].flag_ext;
            chan_info->channel[i].ieee_num = chan->channel[i].ieee_num;
            chan_info->channel[i].max_reg_pwr = chan->channel[i].max_reg_pwr;
            chan_info->channel[i].max_pwr = chan->channel[i].max_pwr;
            chan_info->channel[i].min_pwr = chan->channel[i].min_pwr;
            chan_info->channel[i].reg_class = chan->channel[i].reg_class;
            chan_info->channel[i].antenna_max = chan->channel[i].antenna_max;
            chan_info->channel[i].vht_ch_freq_seg1 = chan->channel[i].vht_ch_freq_seg1;
            chan_info->channel[i].vht_ch_freq_seg2 = chan->channel[i].vht_ch_freq_seg2;
        }
    }

fail_free:
    adf_os_mem_free(chan);

fail:
    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG),
                                ("End status %x ", status));
    return status;
}


/** 
 * @brief Get Channel List
 * 
 * @param hdl
 * @param mode
 * 
 * @return 
 */
a_status_t 
atd_cfg_get_chan_list(adf_drv_handle_t hdl, acfg_opaque_t *chan_list)
{
    atd_host_vap_t     *vap    = (atd_host_vap_t *)hdl;
    atd_host_wifi_t    *wifi   = vap->wifisc;
    a_status_t          status = A_STATUS_OK;
    adf_nbuf_t          nbuf;
    a_uint32_t          *buf;

    /*Error Check*/
    if(chan_list->len < 0) {
        atd_trace(ATD_DEBUG_CFG, ("Invalid length\n"));
        status = A_STATUS_EINVAL;
        goto fail;
    }

    if ((nbuf = _atd_wmi_msg_alloc(__wifi2wmi(wifi))) == NULL) {
        atd_trace(ATD_DEBUG_CFG, ("Unable to allocate wmi buffer \n"));
        status = A_STATUS_ENOMEM;
        goto fail;
    }

    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), 
                                ("len - %d ", chan_list->len));

    buf = (a_uint32_t *)atd_prep_cfg_hdr(nbuf, sizeof(a_uint32_t), wifi, vap);

    *buf = adf_os_htonl(chan_list->len);

    status = wmi_cmd_send(__wifi2wmi(wifi), WMI_CMD_CHAN_LIST_GET, nbuf, 
                          chan_list->pointer, chan_list->len);

fail:
    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG),
                                ("End status %x ", status));
    return status;
}


/** 
 * @brief Get ACL MAC Address List
 * 
 * @param hdl
 * @param mode
 * 
 * @return 
 */
a_status_t 
atd_cfg_get_mac_address(adf_drv_handle_t hdl, acfg_macacl_t *mac_addr_list)
{
    atd_host_vap_t     *vap    = (atd_host_vap_t *)hdl;
    atd_host_wifi_t    *wifi   = vap->wifisc;
    a_status_t          status = A_STATUS_OK;
    adf_nbuf_t          nbuf;
    a_uint32_t          *buf;
    atd_macacl_t        mac_addr_list_resp;
    a_uint8_t           i;

    if ((nbuf = _atd_wmi_msg_alloc(__wifi2wmi(wifi))) == NULL) {
        atd_trace(ATD_DEBUG_CFG, ("Unable to allocate wmi buffer \n"));
        status = A_STATUS_ENOMEM;
    }

    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), 
                                ("get mac addr "));

    buf = (a_uint32_t *)atd_prep_cfg_hdr(nbuf, 0, wifi, vap);


    status = wmi_cmd_send(__wifi2wmi(wifi), 
                          WMI_CMD_MAC_ADDR_GET, 
                          nbuf, (a_uint8_t *)&mac_addr_list_resp, 
                          sizeof(atd_macacl_t));
    mac_addr_list->num = adf_os_ntohl(mac_addr_list_resp.num);
    for (i = 0; i < mac_addr_list->num; i++)
        adf_os_mem_copy(mac_addr_list->macaddr[i], 
                        mac_addr_list_resp.macaddr[i], 
                        ACFG_MACADDR_LEN);

    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG),
                                ("End status %x ", status));
    return status;
}


/** 
 * @brief Get P2P Param
 * 
 * @param hdl
 * @param mode
 * 
 * @return 
 */
a_status_t 
atd_cfg_get_p2p_param(adf_drv_handle_t hdl, acfg_p2p_param_t *p2p_param)
{
    atd_host_vap_t     *vap    = (atd_host_vap_t *)hdl;
    atd_host_wifi_t    *wifi   = vap->wifisc;
    a_status_t          status = A_STATUS_OK;
    adf_nbuf_t          nbuf;
    a_uint32_t          *buf;
    a_uint32_t          *resp_buf;

    /*Error Check*/
    if(p2p_param->length < 0) {
        atd_trace(ATD_DEBUG_CFG, ("Invalid length\n"));
        status = A_STATUS_EINVAL;
        goto fail;
    }

    resp_buf = adf_os_mem_alloc(NULL, p2p_param->length + sizeof(a_uint32_t));
    if (!resp_buf) {
        atd_trace(ATD_DEBUG_CFG, ("Unable to allocate response buffer \n"));
        status = A_STATUS_ENOMEM;
        goto fail;
    }

    if ((nbuf = _atd_wmi_msg_alloc(__wifi2wmi(wifi))) == NULL) {
        atd_trace(ATD_DEBUG_CFG, ("Unable to allocate wmi buffer \n"));
        adf_os_mem_free(resp_buf);
        status = A_STATUS_ENOMEM;
        goto fail;
    }

    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), 
                                ("len - %d ", p2p_param->length));

    if (p2p_param->param == IEEE80211_IOC_P2P_FETCH_FRAME) {
        a_uint32_t is_empty[2] = {0};

        buf = (a_uint32_t *)atd_prep_cfg_hdr(nbuf, 2 * sizeof(a_uint32_t), wifi, vap);

        *buf = adf_os_htonl(sizeof(a_uint32_t));
        *(buf + 1) =  adf_os_htonl(IEEE80211_IOC_P2P_FRAME_LIST_EMPTY);

        status = wmi_cmd_send(__wifi2wmi(wifi), 
                          WMI_CMD_P2P_PARAM_GET, 
                          nbuf, (a_uint8_t *)is_empty, sizeof(is_empty));

        if (status == A_STATUS_OK && 
                adf_os_ntohl(is_empty[0]) == sizeof(a_uint32_t) && 
                is_empty[1]) {
           atd_trace(ATD_DEBUG_CFG, ("Return ENOSPC \n"));
           adf_os_mem_free(resp_buf);
           status = A_STATUS_ENOSPC;
           goto fail;
        }
    
        if ((nbuf = _atd_wmi_msg_alloc(__wifi2wmi(wifi))) == NULL) {
            atd_trace(ATD_DEBUG_CFG, ("Unable to allocate wmi buffer \n"));
            adf_os_mem_free(resp_buf);
            status = A_STATUS_ENOMEM;
            goto fail;
        }
    }

    buf = (a_uint32_t *)atd_prep_cfg_hdr(nbuf, 2 * sizeof(a_uint32_t), wifi, vap);

    *buf = adf_os_htonl(p2p_param->length);
    *(buf + 1) =  adf_os_htonl(p2p_param->param);

    status = wmi_cmd_send(__wifi2wmi(wifi), 
                          WMI_CMD_P2P_PARAM_GET, 
                          nbuf, (a_uint8_t *)resp_buf, 
                          p2p_param->length + sizeof(a_uint32_t));

    if (status == A_STATUS_OK) {
        if (p2p_param->length > adf_os_ntohl(*resp_buf))
            p2p_param->length = adf_os_ntohl(*resp_buf);
        adf_os_mem_copy(p2p_param->pointer, 
                        resp_buf + 1, p2p_param->length);

        switch (p2p_param->param) {
            case IEEE80211_IOC_P2P_FIND_BEST_CHANNEL:
            {
                int i;
                acfg_best_freq_t *f = (acfg_best_freq_t *)p2p_param->pointer;
                for (i = 0; i < ACFG_MAX_BEST_CHANS; i++)
                    f->freq[i] = adf_os_ntohl(f->freq[i]);
            }
            break;

            case IEEE80211_IOC_P2P_FETCH_FRAME:
            {
                acfg_rx_frame_t *fr = (acfg_rx_frame_t *)p2p_param->pointer;
                fr->freq = adf_os_ntohl(fr->freq);
                fr->type = adf_os_ntohl(fr->type);
            }
            break;

            case IEEE80211_IOC_P2P_NOA_INFO:
            {
                 int i;
                 acfg_noa_info_t *noa = (acfg_noa_info_t *)p2p_param->pointer;
                 noa->tsf = adf_os_ntohl(noa->tsf);
                 for (i = 0; i < ACFG_MAX_NOA_DESCS; i++) {
                     noa->desc[i].type_count = adf_os_ntohl(noa->desc[i].type_count);
                     noa->desc[i].duration = adf_os_ntohl(noa->desc[i].duration);
                     noa->desc[i].interval = adf_os_ntohl(noa->desc[i].interval);
                     noa->desc[i].start_time = adf_os_ntohl(noa->desc[i].start_time);
                 }
            }
            break;
        }
    }

    adf_os_mem_free(resp_buf);

fail:
    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG),
                                ("End status %x ", status));
    return status;
}


/** 
 * @brief Set P2P Param
 * 
 * @param hdl
 * @param mode
 * 
 * @return 
 */
a_status_t 
atd_cfg_set_p2p_param(adf_drv_handle_t hdl, acfg_p2p_param_t *p2p_param)
{
    atd_host_vap_t     *vap    = (atd_host_vap_t *)hdl;
    atd_host_wifi_t    *wifi   = vap->wifisc;
    a_status_t          status = A_STATUS_OK;
    adf_nbuf_t          nbuf;
    a_uint32_t          *buf;
    int                 n;

    /*Error Check*/
    if(p2p_param->length < 0) {
        atd_trace(ATD_DEBUG_CFG, ("Invalid length\n"));
        status = A_STATUS_EINVAL;
        goto fail;
    }

    n = ((p2p_param->length + (2 * sizeof(a_uint32_t))) / WMI_DEF_MSG_LEN) + 1;

    //if ((nbuf = _atd_wmi_msg_alloc(__wifi2wmi(wifi))) == NULL)
    if ((nbuf = wmi_msg_alloc(__wifi2wmi(wifi), n * WMI_DEF_MSG_LEN)) == NULL) {
        atd_trace(ATD_DEBUG_CFG, ("Unable to allocate wmi buffer \n"));
        status = A_STATUS_ENOMEM;
        goto fail;
    }

    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), 
                                ("len - %d ", p2p_param->length));

    buf = (a_uint32_t *)atd_prep_cfg_hdr(nbuf, 
                    (2 * sizeof(a_uint32_t)) + p2p_param->length, wifi, vap);

    switch (p2p_param->param) {
        case IEEE80211_IOC_P2P_SET_CHANNEL:
        {
            acfg_set_chan_t *set_chan = (acfg_set_chan_t *)p2p_param->pointer;
            set_chan->freq = adf_os_htonl(set_chan->freq);
            set_chan->req_id = adf_os_htonl(set_chan->req_id);
            set_chan->time = adf_os_htonl(set_chan->time);
        }
        break;

        case IEEE80211_IOC_P2P_SEND_ACTION:
        {
            acfg_send_act_t *act = (acfg_send_act_t *)p2p_param->pointer;
            act->freq = adf_os_htonl(act->freq);
        }
        break;

        case IEEE80211_IOC_SCAN_REQ:
        {
            int i;
            acfg_scan_req_t *scan = (acfg_scan_req_t *)p2p_param->pointer;
            for (i = 0; i < ACFG_MAX_SCAN_FREQ; i++)
                scan->freq[i] = adf_os_htonl(scan->freq[i]);
            scan->len = adf_os_htons(scan->len);
        }
        break;

        case IEEE80211_IOC_P2P_GO_NOA:
        {
            int i, num;
            u_int16_t tmp;
            acfg_noa_req_t *noa = (acfg_noa_req_t *)p2p_param->pointer;
            num = p2p_param->length / sizeof(*noa);
            for (i = 0; i < num && i < ACFG_MAX_NOA_REQ; i++) {
                adf_os_mem_copy(&tmp, noa[i].offset_next_tbtt, sizeof(tmp));
                tmp = adf_os_htons(tmp);
                adf_os_mem_copy(noa[i].offset_next_tbtt, &tmp, sizeof(tmp));
                adf_os_mem_copy(&tmp, noa[i].duration, sizeof(tmp));
                tmp = adf_os_htons(tmp);
                adf_os_mem_copy(noa[i].duration, &tmp, sizeof(tmp));
            }
        }
        break;
    }

    *buf = adf_os_htonl(p2p_param->length);
    *(buf + 1) =  adf_os_htonl(p2p_param->param);
    adf_os_mem_copy(buf + 2, p2p_param->pointer, p2p_param->length);

    status = wmi_cmd_send(__wifi2wmi(wifi), 
                          WMI_CMD_P2P_PARAM_SET, nbuf, NULL, 0);

fail:
    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG),
                                ("End status %x ", status));
    return status;
}

a_status_t 
atd_cfg_acl_setmac(adf_drv_handle_t hdl, acfg_macaddr_t *mac, a_uint8_t add)
{
    a_status_t          status = A_STATUS_OK;
    atd_host_vap_t    *vap = (atd_host_vap_t *)hdl;
    atd_host_wifi_t   *wifi = vap->wifisc;
    adf_nbuf_t         nbuf;
    acfg_macaddr_t    *pmac;
    a_int32_t          resp;

    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("Start"));
	
    if ((nbuf = _atd_wmi_msg_alloc(__wifi2wmi(wifi))) == NULL) {
        atd_trace(ATD_DEBUG_CFG, ("Unable to allocate wmi buffer \n"));
        status = A_STATUS_ENOMEM;
        goto fail ;
    }
    pmac = (acfg_macaddr_t *)atd_prep_cfg_hdr(nbuf, sizeof(acfg_macaddr_t), 
                                              wifi, vap);

    memcpy(pmac->addr, mac->addr, ACFG_MACADDR_LEN);

    atd_trace(ATD_DEBUG_CFG, (" ap mac addr %02x:%02x:%02x:%02x:%02x:%02x", \
                pmac->addr[0], pmac->addr[1], pmac->addr[2], pmac->addr[3], \
                pmac->addr[4], pmac->addr[5]));
	if (add) {
	    status = wmi_cmd_send(__wifi2wmi(wifi), WMI_CMD_ACL_ADDMAC, nbuf, 
    	        (a_uint8_t *)&resp, sizeof(a_int32_t));
	} else {
	    status = wmi_cmd_send(__wifi2wmi(wifi), WMI_CMD_ACL_DELMAC, nbuf, 
    	        (a_uint8_t *)&resp, sizeof(a_int32_t));
	}
    if (status != A_STATUS_OK) {
        atd_trace(ATD_DEBUG_CFG, (" wmi_cmd_send Failed stat = %x\n", status));
        status = A_STATUS_EINVAL;
        goto fail;
    }
    resp = adf_os_ntohl(resp);
    if(resp == -1)
        status = A_STATUS_EINVAL;

fail:
    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("End stat %x ",status ));

    return status;
}

a_status_t
atd_cfg_get_profile(adf_drv_handle_t       hdl, acfg_radio_vap_info_t *profile)
{
    atd_host_wifi_t         *wifi  = (atd_host_wifi_t * ) hdl;
    a_status_t         status = A_STATUS_OK;
    adf_nbuf_t         nbuf;
    atd_hdr_t         *hdrp;
    a_uint32_t         len = sizeof(atd_hdr_t);
    atd_radio_vap_info_t profile_result;
    a_uint8_t          i;

    atd_trace((ATD_DEBUG_FUNCTRACE | ATD_DEBUG_CFG), ("get profile"));
    if ((nbuf = _atd_wmi_msg_alloc(__wifi2wmi(wifi))) == NULL) {
        atd_trace(ATD_DEBUG_CFG, ("Unable to allocate wmi buffer \n"));
        status = A_STATUS_ENOMEM;
        goto fail ;
    }
    hdrp                  =  (atd_hdr_t *)adf_nbuf_put_tail(nbuf, len);
    hdrp->wifi_index      =   wifi->wifi_index;
    hdrp->vap_index       =   -1;
    status = wmi_cmd_send(__wifi2wmi(wifi), WMI_CMD_WIFI_GET_PROFILE, nbuf,
                          (a_uint8_t *)&profile_result, sizeof(atd_radio_vap_info_t));
    if(status != A_STATUS_OK)
    {
        atd_trace(ATD_DEBUG_CFG, ("wmi_cmd_send Failed stat = %x\n", status));
        status =  A_STATUS_EINVAL;
        goto fail;
    }

    adf_os_mem_copy(profile->radio_name, profile_result.radio_name, ACFG_MAX_IFNAME);
    profile->chan = profile_result.chan;
    profile->freq = profile_result.freq;
    profile->country_code = profile_result.country_code;
    adf_os_mem_copy(profile->radio_mac, profile_result.radio_mac, ACFG_MACADDR_LEN);
    profile->num_vaps = profile_result.num_vaps;
    profile->ampdu = 0;
    profile->ampdu_limit_bytes = 0;
    profile->ampdu_subframes = 0;
    for (i = 0; i < profile->num_vaps; i++) {
        adf_os_mem_copy(profile->vap_info[i].vap_name, 
                        profile_result.vap_info[i].vap_name, ACFG_MAX_IFNAME);
        adf_os_mem_copy(profile->vap_info[i].vap_mac, 
                        profile_result.vap_info[i].vap_mac, ACFG_MACADDR_LEN);
        profile->vap_info[i].phymode = profile_result.vap_info[i].phymode;
        profile->vap_info[i].sec_method = profile_result.vap_info[i].sec_method;
        profile->vap_info[i].cipher = profile_result.vap_info[i].cipher;
        adf_os_mem_copy(profile->vap_info[i].wep_key, 
                        profile_result.vap_info[i].wep_key, ACFG_MAX_PSK_LEN);
        profile->vap_info[i].wep_key_len = 
                                    profile_result.vap_info[i].wep_key_len;
        profile->vap_info[i].wep_key_idx = 
                                    profile_result.vap_info[i].wep_key_idx;
        profile->vap_info[i].wds_enabled = 0;
        adf_os_mem_set(profile->vap_info[i].wds_addr, 0, ACFG_MACSTR_LEN);
        profile->vap_info[i].wds_flags = 0;
    }

	atd_convert_profile(profile);
fail:
	return status;	
}

a_status_t atd_cfg_set_chwidthswitch(adf_drv_handle_t  hdl, acfg_set_chn_width_t* chnw)
{
    atd_host_vap_t    *vap    = (atd_host_vap_t * )hdl;
    atd_host_wifi_t   *wifi   = vap->wifisc;
    a_status_t         status = A_STATUS_OK;
    adf_nbuf_t         nbuf;
    atd_set_chn_width_t        *chnwidthswitch;

    atd_trace(ATD_DEBUG_FUNCTRACE, ("Start"));

    if ((nbuf = _atd_wmi_msg_alloc(__wifi2wmi(wifi))) == NULL) {
        atd_trace(ATD_DEBUG_CFG, ("Unable to allocate wmi buffer \n"));
        status = A_STATUS_ENOMEM;
        goto fail ;
    }

    chnwidthswitch = (atd_set_chn_width_t *)atd_prep_cfg_hdr(nbuf, sizeof(atd_set_chn_width_t), wifi, vap);
    chnwidthswitch->setchnwidth[0] = adf_os_htonl(chnw->setchnwidth[0]);
    chnwidthswitch->setchnwidth[1] = adf_os_htonl(chnw->setchnwidth[1]);
    chnwidthswitch->setchnwidth[2] = adf_os_htonl(chnw->setchnwidth[2]);


    status = wmi_cmd_send(__wifi2wmi(wifi),WMI_CMD_CHNWIDTHSWITCH_SET, nbuf, NULL, 0);

    if(status != A_STATUS_OK){
        atd_trace(ATD_DEBUG_CFG, (" wmi_cmd_send Failed stat = %x\n", status));
        status =  A_STATUS_EINVAL;
    }

fail:
    atd_trace( ATD_DEBUG_FUNCTRACE, (" End ... \n"));
    return status;
}

a_status_t atd_cfg_set_atf_ssid(adf_drv_handle_t  hdl, acfg_atf_ssid_val_t* atf_ssid)
{
    atd_host_vap_t    *vap    = (atd_host_vap_t * )hdl;
    atd_host_wifi_t   *wifi   = vap->wifisc;
    a_status_t         status = A_STATUS_OK;
    adf_nbuf_t         nbuf;
    atd_atf_ssid_val_t *ssid;

    atd_trace(ATD_DEBUG_FUNCTRACE, ("Start"));

    if ((nbuf = _atd_wmi_msg_alloc(__wifi2wmi(wifi))) == NULL) {
        atd_trace(ATD_DEBUG_CFG, ("Unable to allocate wmi buffer \n"));
        status = A_STATUS_ENOMEM;
        goto fail ;
    }

    ssid = (atd_atf_ssid_val_t *)atd_prep_cfg_hdr(nbuf, sizeof(atd_atf_ssid_val_t), wifi, vap);
    ssid->id_type = adf_os_htons(atf_ssid->id_type);
    memcpy(ssid->ssid, atf_ssid->ssid, ACFG_MAX_SSID_LEN+1);
    ssid->value = adf_os_htonl(atf_ssid->value);

    status = wmi_cmd_send(__wifi2wmi(wifi), WMI_CMD_SET_ATF_SSID, nbuf, NULL, 0);

    if(status != A_STATUS_OK){
        atd_trace(ATD_DEBUG_CFG, (" wmi_cmd_send Failed stat = %x\n", status));
        status =  A_STATUS_EINVAL;
    }

fail:
    atd_trace( ATD_DEBUG_FUNCTRACE, (" End ... \n"));
    return status;
}

a_status_t atd_cfg_set_atf_sta(adf_drv_handle_t  hdl, acfg_atf_sta_val_t* atf_sta)
{
    atd_host_vap_t    *vap    = (atd_host_vap_t * )hdl;
    atd_host_wifi_t   *wifi   = vap->wifisc;
    a_status_t         status = A_STATUS_OK;
    adf_nbuf_t         nbuf;
    atd_atf_sta_val_t *sta;

    atd_trace(ATD_DEBUG_FUNCTRACE, ("Start"));

    if ((nbuf = _atd_wmi_msg_alloc(__wifi2wmi(wifi))) == NULL) {
        atd_trace(ATD_DEBUG_CFG, ("Unable to allocate wmi buffer \n"));
        status = A_STATUS_ENOMEM;
        goto fail ;
    }

    sta = (atd_atf_sta_val_t *)atd_prep_cfg_hdr(nbuf, sizeof(atd_atf_sta_val_t), wifi, vap);
    sta->id_type = adf_os_htons(atf_sta->id_type);
    memcpy(sta->sta_mac, atf_sta->sta_mac, ACFG_MACADDR_LEN);
    sta->value = adf_os_htonl(atf_sta->value);

    status = wmi_cmd_send(__wifi2wmi(wifi), WMI_CMD_SET_ATF_STA, nbuf, NULL, 0);

    if(status != A_STATUS_OK){
        atd_trace(ATD_DEBUG_CFG, (" wmi_cmd_send Failed stat = %x\n", status));
        status =  A_STATUS_EINVAL;
    }

fail:
    atd_trace( ATD_DEBUG_FUNCTRACE, (" End ... \n"));
    return status;
}

a_status_t atd_cfg_set_country(adf_drv_handle_t  hdl, acfg_set_country_t* set_country)
{
    atd_host_wifi_t    *wifi   = (atd_host_wifi_t *)hdl;
    a_status_t         status = A_STATUS_OK;
    adf_nbuf_t         nbuf;
    atd_set_country_t  *tempsetcountry;
    atd_hdr_t          *hdrp;
    a_uint32_t          len = sizeof(atd_hdr_t) +  sizeof(atd_set_country_t);

    atd_trace(ATD_DEBUG_FUNCTRACE, ("Start"));

    if ((nbuf = _atd_wmi_msg_alloc(__wifi2wmi(wifi))) == NULL) {
        atd_trace(ATD_DEBUG_CFG, ("Unable to allocate wmi buffer \n"));
        status = A_STATUS_ENOMEM;
        goto fail ;
    }

    hdrp                  =  (atd_hdr_t *)adf_nbuf_put_tail(nbuf, len);
    hdrp->wifi_index      =  wifi->wifi_index;
    hdrp->vap_index       =  -1;

    tempsetcountry = (atd_set_country_t *)(hdrp + 1);

    tempsetcountry->setcountry[0] = set_country->setcountry[0];
    tempsetcountry->setcountry[1] = set_country->setcountry[1];
    tempsetcountry->setcountry[2] = set_country->setcountry[2];

    status = wmi_cmd_send(__wifi2wmi(wifi),WMI_CMD_COUNTRY_SET, nbuf, NULL, 0);

    if(status != A_STATUS_OK){
        atd_trace(ATD_DEBUG_CFG, (" wmi_cmd_send Failed stat = %x\n", status));
        status =  A_STATUS_EINVAL;
    }

fail:
    atd_trace( ATD_DEBUG_FUNCTRACE, (" End ... \n"));
    return status;
}
