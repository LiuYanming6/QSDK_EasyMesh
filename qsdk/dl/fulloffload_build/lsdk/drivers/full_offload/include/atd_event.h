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

#ifndef __ATD_EVENT_H
#define __ATD_EVENT_H

#include <acfg_event_types.h>
#include <spectral_data.h>

/** 
 * @brief Scan done event
 */
typedef struct atd_scan_done {
    acfg_scan_done_status_t     status;
    a_uint32_t                  size;
} __attribute__((__packed__)) atd_scan_done_t;

/** 
 * @brief Restore wifi Event
 */
typedef acfg_restore_wifi_t   atd_restore_wifi_t;

/** 
 * @brief Create Vap Event
 */
typedef acfg_create_vap_t   atd_create_vap_t;

/** 
 * @brief Association Done Event
 */
typedef struct atd_assoc_ap {
    acfg_assoc_status_t     status;
    a_uint8_t               bssid[ACFG_MACADDR_LEN];
} __attribute__((__packed__)) atd_assoc_ap_t;

/** 
 * @brief Association Fail Event
 */
typedef struct atd_assoc_sta {
    acfg_assoc_status_t     status;
    atd_ssid_t              ssid;
    a_uint8_t               bssid[ACFG_MACADDR_LEN];
} __attribute__((__packed__)) atd_assoc_sta_t;

/** 
 * @brief De-authentication Notification
 */
typedef struct atd_dauth {
    atd_ssid_t              ssid;
    acfg_dauth_reason_t     reason;
    acfg_assoc_status_t     status;
    a_uint8_t               macaddr[ACFG_MACADDR_LEN];
    a_uint8_t               frame_send;
} __attribute__((__packed__)) atd_dauth_t;

typedef struct atd_assoc {
    acfg_assoc_status_t     status;
    atd_ssid_t              ssid;
    a_uint8_t               bssid[ACFG_MACADDR_LEN];
    a_uint8_t               frame_send;
} __attribute__((__packed__)) atd_assoc_t;

typedef struct atd_disassoc {
    acfg_reason_t           reason;
    acfg_assoc_status_t     status;
    a_uint8_t               macaddr[ACFG_MACADDR_LEN];
    a_uint8_t               frame_send;
} __attribute__((__packed__)) atd_disassoc_t;

typedef struct atd_auth {
    acfg_assoc_status_t     status;
    a_uint8_t               macaddr[ACFG_MACADDR_LEN];
    a_uint8_t               frame_send;
} __attribute__((__packed__)) atd_auth_t;

typedef struct atd_chan_start {
    a_uint32_t              freq;
    a_uint32_t              duration;
    a_uint32_t              req_id;
} __attribute__((__packed__)) atd_chan_start_t;

typedef struct atd_chan_end {
    a_uint32_t              freq;
    a_uint32_t              reason;
    a_uint32_t              duration;
    a_uint32_t              req_id;
} __attribute__((__packed__)) atd_chan_end_t;

/* Spectral sample message */
/* already packed by the sender */
typedef SPECTRAL_SAMP_MSG atd_spectral_samp_msg_t;

typedef struct atd_bs_probe_req_ind {
    /* The MAC address of the client that sent the probe request.*/
    u_int8_t sender_addr[ACFG_MACADDR_LEN];
    /*  The RSSI of the received probe request.*/
    u_int8_t rssi;
} __attribute__((__packed__)) atd_bs_probe_req_ind_t;

typedef struct atd_bs_auth_reject_ind {
    /* The MAC address of the client to which the authentication message
     *         was sent with a failure code.*/
    u_int8_t client_addr[ACFG_MACADDR_LEN];
    /* The RSSI of the received authentication message (the one that
     *        triggered the rejection).*/
    u_int8_t rssi;
} __attribute__((__packed__)) atd_bs_auth_reject_ind_t;

typedef struct atd_bs_node_authorized_ind {
    /* The MAC address of the client that is having its authorize status
     *        changed.*/
    u_int8_t client_addr[ACFG_MACADDR_LEN];
} __attribute__((__packed__)) atd_bs_node_authorized_ind_t;

typedef struct atd_bs_activity_change_ind {
    /* The MAC address of the client that activity status changes */
    u_int8_t client_addr[ACFG_MACADDR_LEN];
    /* Activity status*/
    u_int8_t activity;
} __attribute__((__packed__)) atd_bs_activity_change_ind_t;

typedef struct atd_bs_chan_utilization_ind {
    /* The current utilization on the band, expressed as a percentage.*/
    u_int8_t utilization;
} __attribute__((__packed__)) atd_bs_chan_utilization_ind_t;

typedef struct atd_bs_rssi_xing_threshold_ind {
    /* The MAC address of the client */
    u_int8_t client_addr[ACFG_MACADDR_LEN];
    /* The measured RSSI */
    u_int8_t rssi;
    /* Flag indicating if it crossed inactivity RSSI threshold */
    u_int32_t inact_rssi_xing;
    /* Flag indicating if it crossed low RSSI threshold */
    u_int32_t low_rssi_xing;
} __attribute__((__packed__)) atd_bs_rssi_xing_threshold_ind_t;

typedef struct atd_bs_rssi_measurement_ind {
    /* The MAC address of the client */
    u_int8_t client_addr[ACFG_MACADDR_LEN];
    /* The measured RSSI */
    u_int8_t rssi;
} __attribute__((__packed__)) atd_bs_rssi_measurement_ind_t;

typedef struct atd_bsteering_event {
    /* The type of the event: One of ATH_BSTEERING_EVENT.*/
    u_int32_t type;
    /* The band on which the event occurred.*/
    u_int32_t band_index;
    /* The data for the event. Which member is valid is based on the
     * type field.*/
    union {
        struct atd_bs_probe_req_ind bs_probe;
        struct atd_bs_node_authorized_ind bs_node_authorized;
        struct atd_bs_activity_change_ind bs_activity_change;
        struct atd_bs_auth_reject_ind bs_auth;
        struct atd_bs_chan_utilization_ind bs_chan_util;
        struct atd_bs_rssi_xing_threshold_ind bs_rssi_xing;
        struct atd_bs_rssi_measurement_ind bs_rssi_measurement;
    } data;
} __attribute__((__packed__)) atd_bsteering_event_t;

#endif /* __ATD_EVENT_H */
