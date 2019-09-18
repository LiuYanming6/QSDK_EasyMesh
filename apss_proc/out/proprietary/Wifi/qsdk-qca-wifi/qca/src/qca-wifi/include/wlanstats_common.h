/*
 * Copyright (c) 2018 Qualcomm Innovation Center, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Innovation Center, Inc
 *
 */


#ifndef _PEERSTATS_COMMON_H
#define _PEERSTATS_COMMON_H

#include<stdbool.h>
#include<wlanstats_uapi.h>

#ifndef WSTATS_ARRAY_SIZE
#define WSTATS_ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#endif

void wlanstats_peer_rx_stats_handle(struct wlanstats_uapi_ioctl *cmd);
void wlanstats_peer_rx_stats_prepare(struct wlanstats_uapi_ioctl *cmd, char *mac);
void wlanstats_peer_tx_stats_prepare(struct wlanstats_uapi_ioctl *cmd, char *mac);
void wlanstats_peer_tx_stats_handle(struct wlanstats_uapi_ioctl *cmd);
void wlanstats_survey_chan_prepare(struct wlanstats_uapi_ioctl *cmd);
void wlanstats_survey_chan_handle(struct wlanstats_uapi_ioctl *cmd);
void wlanstats_survey_bss_prepare(struct wlanstats_uapi_ioctl *cmd);
void wlanstats_survey_bss_handle(struct wlanstats_uapi_ioctl *cmd);
void wlanstats_q_util_prepare(struct wlanstats_uapi_ioctl *cmd);
void wlanstats_q_util_handle(struct wlanstats_uapi_ioctl *cmd);
int wlanstats_svc_prepare(int type, struct wlanstats_uapi_ioctl *cmd, const char **input);
void wlanstats_svc_handle(struct wlanstats_uapi_ioctl *cmd);
int wlanstats_send_ioctl_command(const char *interface, struct iwreq *iwr,
        struct wlanstats_uapi_ioctl *iocmd);

#endif
