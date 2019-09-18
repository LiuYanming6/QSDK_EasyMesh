/*
 * Copyright (c) 2018 Qualcomm Innovation Center, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Innovation Center, Inc
 *
 */

#include <stdio.h>
#include <stdarg.h>
#include <assert.h>
#include <linux/socket.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <stdio.h>
#include <math.h>
#include <errno.h>
#include <fcntl.h>
#include <ctype.h>
#include <stdlib.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <linux/types.h>
#include <linux/if.h>
#include <linux/wireless.h>

#include <wlanstats_common.h>

void wlanstats_peer_rx_stats_handle(struct wlanstats_uapi_ioctl *cmd)
{
    struct wlanstats_cmn_rate_info ri = {};
    int rix;

    printf("\n...................................................................."
            "......................................................................................\n");

    printf("addr = %02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx\n",
            cmd->u.peer_rx_stats.set.addr[0],
            cmd->u.peer_rx_stats.set.addr[1],
            cmd->u.peer_rx_stats.set.addr[2],
            cmd->u.peer_rx_stats.set.addr[3],
            cmd->u.peer_rx_stats.set.addr[4],
            cmd->u.peer_rx_stats.set.addr[5]);
    printf("cookie = %016llx\n",
            cmd->u.peer_rx_stats.get.cookie);

    printf("\n...................................................................."
            "......................................................................................\n");

    printf("%20s | %3s | %10s | %10s | %10s | %10s | %10s | %10s | %10s | %10s | %10s | %10s | %10s | %10s | %10s | %10s | %10s | %10s | %10s | %10s | %10s | %10s | %10s | %10s\n",
            "rate",
            "rix",
            "bytes",
            "msdus",
            "mpdus",
            "ppdus",
            "retries",
            "rssi",
            "rssi 1 p20",
            "rssi 1 e20",
            "rssi 1 e40",
            "rssi 1 e80",
            "rssi 2 p20",
            "rssi 2 e20",
            "rssi 2 e40",
            "rssi 2 e80",
            "rssi 3 p20",
            "rssi 3 e20",
            "rssi 3 e40",
            "rssi 3 e80",
            "rssi 4 p20",
            "rssi 4 e20",
            "rssi 4 e40",
            "rssi 4 e80");

    memset(&ri, 0, sizeof(ri));
    ri.is_cck = 1;
    for (ri.mcs = 0; ri.mcs < WLANSTATS_MAX_CCK; ri.mcs++) {
        rix = wlanstats_cmn_calc_rix(&ri);
        if (!cmd->u.peer_rx_stats.get.stats[rix].num_bytes &&
                !cmd->u.peer_rx_stats.get.stats[rix].num_msdus &&
                !cmd->u.peer_rx_stats.get.stats[rix].num_mpdus &&
                !cmd->u.peer_rx_stats.get.stats[rix].num_ppdus &&
                !cmd->u.peer_rx_stats.get.stats[rix].num_retries &&
                !cmd->u.peer_rx_stats.get.stats[rix].ave_rssi &&
                !cmd->u.peer_rx_stats.get.stats[rix].ave_rssi_ant[0][0] &&
                !cmd->u.peer_rx_stats.get.stats[rix].ave_rssi_ant[0][1] &&
                !cmd->u.peer_rx_stats.get.stats[rix].ave_rssi_ant[0][2] &&
                !cmd->u.peer_rx_stats.get.stats[rix].ave_rssi_ant[0][3] &&
                !cmd->u.peer_rx_stats.get.stats[rix].ave_rssi_ant[1][0] &&
                !cmd->u.peer_rx_stats.get.stats[rix].ave_rssi_ant[1][1] &&
                !cmd->u.peer_rx_stats.get.stats[rix].ave_rssi_ant[1][2] &&
                !cmd->u.peer_rx_stats.get.stats[rix].ave_rssi_ant[1][3] &&
                !cmd->u.peer_rx_stats.get.stats[rix].ave_rssi_ant[2][0] &&
                !cmd->u.peer_rx_stats.get.stats[rix].ave_rssi_ant[2][1] &&
                !cmd->u.peer_rx_stats.get.stats[rix].ave_rssi_ant[2][2] &&
                !cmd->u.peer_rx_stats.get.stats[rix].ave_rssi_ant[2][3] &&
                !cmd->u.peer_rx_stats.get.stats[rix].ave_rssi_ant[3][0] &&
                !cmd->u.peer_rx_stats.get.stats[rix].ave_rssi_ant[3][1] &&
                !cmd->u.peer_rx_stats.get.stats[rix].ave_rssi_ant[3][2] &&
                !cmd->u.peer_rx_stats.get.stats[rix].ave_rssi_ant[3][3]) {
                    continue;
                }
        printf("%4s %15s | %3u | %10u | %10u | %10u | %10u | %10u | %10u | %10u | %10u | %10u | %10u | %10u | %10u | %10u | %10u | %10u | %10u | %10u | %10u | %10u | %10u | %10u | %10u\n",
                "CCK",
                wlanstats_cmn_cck_str(ri.mcs),
                rix,
                cmd->u.peer_rx_stats.get.stats[rix].num_bytes,
                cmd->u.peer_rx_stats.get.stats[rix].num_msdus,
                cmd->u.peer_rx_stats.get.stats[rix].num_mpdus,
                cmd->u.peer_rx_stats.get.stats[rix].num_ppdus,
                cmd->u.peer_rx_stats.get.stats[rix].num_retries,
                cmd->u.peer_rx_stats.get.stats[rix].ave_rssi,
                cmd->u.peer_rx_stats.get.stats[rix].ave_rssi_ant[0][0],
                cmd->u.peer_rx_stats.get.stats[rix].ave_rssi_ant[0][1],
                cmd->u.peer_rx_stats.get.stats[rix].ave_rssi_ant[0][2],
                cmd->u.peer_rx_stats.get.stats[rix].ave_rssi_ant[0][3],
                cmd->u.peer_rx_stats.get.stats[rix].ave_rssi_ant[1][0],
                cmd->u.peer_rx_stats.get.stats[rix].ave_rssi_ant[1][1],
                cmd->u.peer_rx_stats.get.stats[rix].ave_rssi_ant[1][2],
                cmd->u.peer_rx_stats.get.stats[rix].ave_rssi_ant[1][3],
                cmd->u.peer_rx_stats.get.stats[rix].ave_rssi_ant[2][0],
                cmd->u.peer_rx_stats.get.stats[rix].ave_rssi_ant[2][1],
                cmd->u.peer_rx_stats.get.stats[rix].ave_rssi_ant[2][2],
                cmd->u.peer_rx_stats.get.stats[rix].ave_rssi_ant[2][3],
                cmd->u.peer_rx_stats.get.stats[rix].ave_rssi_ant[3][0],
                cmd->u.peer_rx_stats.get.stats[rix].ave_rssi_ant[3][1],
                cmd->u.peer_rx_stats.get.stats[rix].ave_rssi_ant[3][2],
                cmd->u.peer_rx_stats.get.stats[rix].ave_rssi_ant[3][3]);
    }

    memset(&ri, 0, sizeof(ri));
    for (ri.mcs = 0; ri.mcs < WLANSTATS_MAX_OFDM; ri.mcs++) {
        rix = wlanstats_cmn_calc_rix(&ri);
        printf("%4s %15s | %3u | %10u | %10u | %10u | %10u | %10u | %10u | %10u | %10u | %10u | %10u | %10u | %10u | %10u | %10u | %10u | %10u | %10u | %10u | %10u | %10u | %10u | %10u\n",
                "OFDM",
                wlanstats_cmn_ofdm_str(ri.mcs),
                rix,
                cmd->u.peer_rx_stats.get.stats[rix].num_bytes,
                cmd->u.peer_rx_stats.get.stats[rix].num_msdus,
                cmd->u.peer_rx_stats.get.stats[rix].num_mpdus,
                cmd->u.peer_rx_stats.get.stats[rix].num_ppdus,
                cmd->u.peer_rx_stats.get.stats[rix].num_retries,
                cmd->u.peer_rx_stats.get.stats[rix].ave_rssi,
                cmd->u.peer_rx_stats.get.stats[rix].ave_rssi_ant[0][0],
                cmd->u.peer_rx_stats.get.stats[rix].ave_rssi_ant[0][1],
                cmd->u.peer_rx_stats.get.stats[rix].ave_rssi_ant[0][2],
                cmd->u.peer_rx_stats.get.stats[rix].ave_rssi_ant[0][3],
                cmd->u.peer_rx_stats.get.stats[rix].ave_rssi_ant[1][0],
                cmd->u.peer_rx_stats.get.stats[rix].ave_rssi_ant[1][1],
                cmd->u.peer_rx_stats.get.stats[rix].ave_rssi_ant[1][2],
                cmd->u.peer_rx_stats.get.stats[rix].ave_rssi_ant[1][3],
                cmd->u.peer_rx_stats.get.stats[rix].ave_rssi_ant[2][0],
                cmd->u.peer_rx_stats.get.stats[rix].ave_rssi_ant[2][1],
                cmd->u.peer_rx_stats.get.stats[rix].ave_rssi_ant[2][2],
                cmd->u.peer_rx_stats.get.stats[rix].ave_rssi_ant[2][3],
                cmd->u.peer_rx_stats.get.stats[rix].ave_rssi_ant[3][0],
                cmd->u.peer_rx_stats.get.stats[rix].ave_rssi_ant[3][1],
                cmd->u.peer_rx_stats.get.stats[rix].ave_rssi_ant[3][2],
                cmd->u.peer_rx_stats.get.stats[rix].ave_rssi_ant[3][3]);
    }

    printf("\n...................................................................."
            "......................................................................................\n");

    printf("HT / VHT:\n");
    printf("%5s | %3s | %3s | %3s | %10s | %10s | %10s | %10s | %10s | %10s | %10s | %10s | %10s | %10s | %10s | %10s | %10s | %10s | %10s | %10s | %10s | %10s | %10s | %10s | %10s | %10s | %10s\n",
            "bw",
            "nss",
            "mcs",
            "rix",
            "bytes",
            "msdus",
            "mpdus",
            "ppdus",
            "retries",
            "sgi",
            "rssi",
            "rssi 1 p20",
            "rssi 1 e20",
            "rssi 1 e40",
            "rssi 1 e80",
            "rssi 2 p20",
            "rssi 2 e20",
            "rssi 2 e40",
            "rssi 2 e80",
            "rssi 3 p20",
            "rssi 3 e20",
            "rssi 3 e40",
            "rssi 3 e80",
            "rssi 4 p20",
            "rssi 4 e20",
            "rssi 4 e40",
            "rssi 4 e80");

    memset(&ri, 0, sizeof(ri));
    ri.is_ht = 1;
    for (ri.bw = 0; ri.bw < WLANSTATS_MAX_BW; ri.bw++) {
        for (ri.nss = 0; ri.nss < WLANSTATS_MAX_NSS; ri.nss++) {
            for (ri.mcs = 0; ri.mcs < WLANSTATS_MAX_MCS; ri.mcs++) {
                rix = wlanstats_cmn_calc_rix(&ri);
                if (!cmd->u.peer_rx_stats.get.stats[rix].num_bytes &&
                        !cmd->u.peer_rx_stats.get.stats[rix].num_msdus &&
                        !cmd->u.peer_rx_stats.get.stats[rix].num_mpdus &&
                        !cmd->u.peer_rx_stats.get.stats[rix].num_ppdus &&
                        !cmd->u.peer_rx_stats.get.stats[rix].num_retries &&
                        !cmd->u.peer_rx_stats.get.stats[rix].num_sgi &&
                        !cmd->u.peer_rx_stats.get.stats[rix].ave_rssi &&
                        !cmd->u.peer_rx_stats.get.stats[rix].ave_rssi_ant[0][0] &&
                        !cmd->u.peer_rx_stats.get.stats[rix].ave_rssi_ant[0][1] &&
                        !cmd->u.peer_rx_stats.get.stats[rix].ave_rssi_ant[0][2] &&
                        !cmd->u.peer_rx_stats.get.stats[rix].ave_rssi_ant[0][3] &&
                        !cmd->u.peer_rx_stats.get.stats[rix].ave_rssi_ant[1][0] &&
                        !cmd->u.peer_rx_stats.get.stats[rix].ave_rssi_ant[1][1] &&
                        !cmd->u.peer_rx_stats.get.stats[rix].ave_rssi_ant[1][2] &&
                        !cmd->u.peer_rx_stats.get.stats[rix].ave_rssi_ant[1][3] &&
                        !cmd->u.peer_rx_stats.get.stats[rix].ave_rssi_ant[2][0] &&
                        !cmd->u.peer_rx_stats.get.stats[rix].ave_rssi_ant[2][1] &&
                        !cmd->u.peer_rx_stats.get.stats[rix].ave_rssi_ant[2][2] &&
                        !cmd->u.peer_rx_stats.get.stats[rix].ave_rssi_ant[2][3] &&
                        !cmd->u.peer_rx_stats.get.stats[rix].ave_rssi_ant[3][0] &&
                        !cmd->u.peer_rx_stats.get.stats[rix].ave_rssi_ant[3][1] &&
                        !cmd->u.peer_rx_stats.get.stats[rix].ave_rssi_ant[3][2] &&
                        !cmd->u.peer_rx_stats.get.stats[rix].ave_rssi_ant[3][3]) {
                            continue;
                        }
                printf("%5s | %3u | %3u | %3u | %10u | %10u | %10u | %10u | %10u | %10u | %10u | %10hhu | %10hhu | %10hhu | %10hhu | %10hhu | %10hhu | %10hhu | %10hhu | %10hhu | %10hhu | %10hhu | %10hhu | %10hhu | %10hhu | %10hhu | %10hhu\n",
                        wlanstats_cmn_bw_str(ri.bw),
                        ri.nss + 1,
                        ri.mcs,
                        rix,
                        cmd->u.peer_rx_stats.get.stats[rix].num_bytes,
                        cmd->u.peer_rx_stats.get.stats[rix].num_msdus,
                        cmd->u.peer_rx_stats.get.stats[rix].num_mpdus,
                        cmd->u.peer_rx_stats.get.stats[rix].num_ppdus,
                        cmd->u.peer_rx_stats.get.stats[rix].num_retries,
                        cmd->u.peer_rx_stats.get.stats[rix].num_sgi,
                        cmd->u.peer_rx_stats.get.stats[rix].ave_rssi,
                        cmd->u.peer_rx_stats.get.stats[rix].ave_rssi_ant[0][0],
                        cmd->u.peer_rx_stats.get.stats[rix].ave_rssi_ant[0][1],
                        cmd->u.peer_rx_stats.get.stats[rix].ave_rssi_ant[0][2],
                        cmd->u.peer_rx_stats.get.stats[rix].ave_rssi_ant[0][3],
                        cmd->u.peer_rx_stats.get.stats[rix].ave_rssi_ant[1][0],
                        cmd->u.peer_rx_stats.get.stats[rix].ave_rssi_ant[1][1],
                        cmd->u.peer_rx_stats.get.stats[rix].ave_rssi_ant[1][2],
                        cmd->u.peer_rx_stats.get.stats[rix].ave_rssi_ant[1][3],
                        cmd->u.peer_rx_stats.get.stats[rix].ave_rssi_ant[2][0],
                        cmd->u.peer_rx_stats.get.stats[rix].ave_rssi_ant[2][1],
                        cmd->u.peer_rx_stats.get.stats[rix].ave_rssi_ant[2][2],
                        cmd->u.peer_rx_stats.get.stats[rix].ave_rssi_ant[2][3],
                        cmd->u.peer_rx_stats.get.stats[rix].ave_rssi_ant[3][0],
                        cmd->u.peer_rx_stats.get.stats[rix].ave_rssi_ant[3][1],
                        cmd->u.peer_rx_stats.get.stats[rix].ave_rssi_ant[3][2],
                        cmd->u.peer_rx_stats.get.stats[rix].ave_rssi_ant[3][3]);
            }
        }
    }

    printf("\n...................................................................."
            "......................................................................................\n");
}

void wlanstats_peer_rx_stats_prepare(struct wlanstats_uapi_ioctl *cmd, char *mac)
{
    cmd->cmd = WLAN_UAPI_IOCTL_CMD_PEER_RX_STATS;
    memcpy(cmd->u.peer_rx_stats.set.addr, mac, 6);
}

void wlanstats_peer_tx_stats_prepare(struct wlanstats_uapi_ioctl *cmd, char *mac)
{
    cmd->cmd = WLAN_UAPI_IOCTL_CMD_PEER_TX_STATS;
    memcpy(cmd->u.peer_tx_stats.set.addr, mac, 6);
}

void wlanstats_peer_tx_stats_handle(struct wlanstats_uapi_ioctl *cmd)
{
    struct wlanstats_cmn_rate_info ri = {};
    int rix;
    int tid;

    printf("\n...................................................................."
            "......................................................................................\n");
    printf("addr = %02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx\n",
            cmd->u.peer_tx_stats.set.addr[0],
            cmd->u.peer_tx_stats.set.addr[1],
            cmd->u.peer_tx_stats.set.addr[2],
            cmd->u.peer_tx_stats.set.addr[3],
            cmd->u.peer_tx_stats.set.addr[4],
            cmd->u.peer_tx_stats.set.addr[5]);
    printf("cookie = %016llx\n",
            cmd->u.peer_tx_stats.get.cookie);

    printf("\n...................................................................."
            "......................................................................................\n");
    printf("Legacy:\n");
    printf("%20s | %3s | %10s | %10s\n",
            "rate",
            "rix",
            "attempts",
            "success");

    memset(&ri, 0, sizeof(ri));
    ri.is_cck = 1;
    for (ri.mcs = 0; ri.mcs < WLANSTATS_MAX_CCK; ri.mcs++) {
        rix = wlanstats_cmn_calc_rix(&ri);
        if (!cmd->u.peer_tx_stats.get.stats[rix].attempts && !cmd->u.peer_tx_stats.get.stats[rix].success) {
            continue;
        }
        printf("%4s %15s | %3u | %10u | %10u\n",
                "CCK",
                wlanstats_cmn_cck_str(ri.mcs),
                rix,
                cmd->u.peer_tx_stats.get.stats[rix].attempts,
                cmd->u.peer_tx_stats.get.stats[rix].success);
    }

    memset(&ri, 0, sizeof(ri));
    for (ri.mcs = 0; ri.mcs < WLANSTATS_MAX_OFDM; ri.mcs++) {
        rix = wlanstats_cmn_calc_rix(&ri);
        printf("%4s %15s | %3u | %10u | %10u\n",
                "OFDM",
                wlanstats_cmn_ofdm_str(ri.mcs),
                rix,
                cmd->u.peer_tx_stats.get.stats[rix].attempts,
                cmd->u.peer_tx_stats.get.stats[rix].success);
    }

    printf("\n...................................................................."
            "......................................................................................\n");
    printf("HT / VHT:\n");
    printf("%5s | %3s | %3s | %3s | %10s | %10s | %10s\n",
            "bw",
            "nss",
            "mcs",
            "rix",
            "attempts",
            "success",
            "ppdus");

    memset(&ri, 0, sizeof(ri));
    ri.is_ht = 1;
    for (ri.bw = 0; ri.bw < WLANSTATS_MAX_BW; ri.bw++) {
        for (ri.nss = 0; ri.nss < WLANSTATS_MAX_NSS; ri.nss++) {
            for (ri.mcs = 0; ri.mcs < WLANSTATS_MAX_MCS; ri.mcs++) {
                rix = wlanstats_cmn_calc_rix(&ri);
                if (!cmd->u.peer_tx_stats.get.stats[rix].attempts &&
                    !cmd->u.peer_tx_stats.get.stats[rix].success &&
                    !cmd->u.peer_tx_stats.get.stats[rix].ppdus) {
                    continue;
                }
                printf("%5s | %3u | %3u | %3u | %10u | %10u | %10u\n",
                        wlanstats_cmn_bw_str(ri.bw),
                        ri.nss + 1,
                        ri.mcs,
                        rix,
                        cmd->u.peer_tx_stats.get.stats[rix].attempts,
                        cmd->u.peer_tx_stats.get.stats[rix].success,
                        cmd->u.peer_tx_stats.get.stats[rix].ppdus);
            }
        }
    }

    printf("\n...................................................................."
            "......................................................................................\n");

    printf("sojourn:\n");
    printf("%10s %10s %20s %20s\n",
            "tid",
            "ave",
            "sum",
            "num");
    for (tid = 0; tid < WLANSTATS_MAX_TID; tid++) {
        if (!cmd->u.peer_tx_stats.get.sojourn[tid].ave_sojourn_msec &&
            !cmd->u.peer_tx_stats.get.sojourn[tid].sum_sojourn_msec &&
            !cmd->u.peer_tx_stats.get.sojourn[tid].num_sojourn_mpdus) {
            continue;
        }
        printf("%10d %10u %20llu %20u\n",
                tid,
                cmd->u.peer_tx_stats.get.sojourn[tid].ave_sojourn_msec,
                cmd->u.peer_tx_stats.get.sojourn[tid].sum_sojourn_msec,
                cmd->u.peer_tx_stats.get.sojourn[tid].num_sojourn_mpdus);
    }
    printf("\n...................................................................."
            "......................................................................................\n");
}


void wlanstats_survey_chan_prepare(struct wlanstats_uapi_ioctl *cmd)
{
	cmd->cmd = WLAN_UAPI_IOCTL_CMD_SURVEY_CHAN;
}

void wlanstats_survey_chan_handle(struct wlanstats_uapi_ioctl *cmd)
{
    int i;
    uint16_t freq;
    uint32_t total;
    uint32_t tx;
    uint32_t rx;
    uint32_t busy;

    for (i = 0; i < WSTATS_ARRAY_SIZE(cmd->u.survey_chan.get.channels); i++) {
        freq = cmd->u.survey_chan.get.channels[i].freq;

        if (freq == 0)
            break;

        printf("freq %hu ", freq);

        total = cmd->u.survey_chan.get.channels[i].total;
        tx = cmd->u.survey_chan.get.channels[i].tx;
        rx = cmd->u.survey_chan.get.channels[i].rx;
        busy = cmd->u.survey_chan.get.channels[i].busy;

        total /= 100;

        if (total == 0) {
            printf("undefined\n");
            continue;
        }

        tx /= total;
        rx /= total;
        busy /= total;

        printf("tx %u%% rx %u%% busy %u%% (raw tx %u rx %u busy %u total %u)\n",
                tx,
                rx,
                busy,
                cmd->u.survey_chan.get.channels[i].tx,
                cmd->u.survey_chan.get.channels[i].rx,
                cmd->u.survey_chan.get.channels[i].busy,
                cmd->u.survey_chan.get.channels[i].total);
    }
}

void wlanstats_survey_bss_prepare(struct wlanstats_uapi_ioctl *cmd)
{
    cmd->cmd = WLAN_UAPI_IOCTL_CMD_SURVEY_BSS;
}

void wlanstats_survey_bss_handle(struct wlanstats_uapi_ioctl *cmd)
{
    uint64_t total;
    uint64_t tx;
    uint64_t rx;
    uint64_t rx_bss;
    uint64_t busy;
    uint64_t busy_ext;

    total = cmd->u.survey_bss.get.total;
    tx = cmd->u.survey_bss.get.tx;
    rx = cmd->u.survey_bss.get.rx;
    rx_bss = cmd->u.survey_bss.get.rx_bss;
    busy = cmd->u.survey_bss.get.busy;
    busy_ext = cmd->u.survey_bss.get.busy_ext;

    printf("raw cc tx %llu rx %llu rx_bss %llu busy %llu busy_ext %llu total %llu\n",
            tx,
            rx,
            rx_bss,
            busy,
            busy_ext,
            total);

    total /= 100;
    if (total == 0)
        return;

    tx /= total;
    rx /= total;
    rx_bss /= total;
    busy /= total;
    busy_ext /= total;

    printf("percent tx %llu%% rx %llu%% rx_bss %llu%% busy %llu%% busy_ext %llu%%\n",
            tx,
            rx,
            rx_bss,
            busy,
            busy_ext);
}

void wlanstats_q_util_prepare(struct wlanstats_uapi_ioctl *cmd)
{
    cmd->cmd = WLAN_UAPI_IOCTL_CMD_Q_UTIL;
}

void wlanstats_q_util_handle(struct wlanstats_uapi_ioctl *cmd)
{
    int cnt;
    int i;

    cnt = sizeof(cmd->u.q_util.get.q) /
        sizeof(cmd->u.q_util.get.q[0]);

    printf("samples = %llu\n", cmd->u.q_util.get.cnt);
    for (i = 0; i < cnt; i++)
        printf("hwq %2d = %10llu\n", i, cmd->u.q_util.get.q[i]);
}

static const char *const svcs[] = {
    [WLAN_UAPI_IOCTL_SVC_PEER_RX_STATS] = "peer_rx_stats",
    [WLAN_UAPI_IOCTL_SVC_PEER_TX_STATS] = "peer_tx_stats",
    [WLAN_UAPI_IOCTL_SVC_SOJOURN] = "sojourn",
    [WLAN_UAPI_IOCTL_SVC_SURVEY] = "survey",
    [WLAN_UAPI_IOCTL_SVC_Q_UTIL] = "q_util",
};

int wlanstats_svc_prepare(int type, struct wlanstats_uapi_ioctl *cmd, const char **input)
{
    int err;
    int i;

    cmd->cmd = WLAN_UAPI_IOCTL_CMD_SVC;
    cmd->u.svc.set.modify = 0;

    switch (type) {
        case 3:
            printf("known services:\n");
            for (i = 0; i < WSTATS_ARRAY_SIZE(svcs); i++)
                printf("  %s\n", svcs[i]);
            printf("nothing to do");
            return -1;
        case 5:
            cmd->u.svc.set.modify = 1;

            err = sscanf(input[0], "%i", &cmd->u.svc.set.enabled);
            if (err != 1) {
                printf("%s: not a number", input[0]);
                return -1;
            }
            /* fall through */
        case 4:
            for (i = 0; i < WSTATS_ARRAY_SIZE(svcs); i++)
                if (!strcmp(input[0], svcs[i]))
                    break;

            if (i == WSTATS_ARRAY_SIZE(svcs)) {
                printf("%s: unknown service name", input[0]);
                return -1;
            }

            cmd->u.svc.set.svc = i;
            break;
        default:
            printf("too many arguments");
            return -1;
    }
    return 0;
}

void wlanstats_svc_handle(struct wlanstats_uapi_ioctl *cmd)
{
    if (cmd->u.svc.get.svc >= WSTATS_ARRAY_SIZE(svcs)) {
        printf("%d: unknown service number", cmd->u.svc.get.svc);
        return;
    }

    printf("%s: %s\n",
            svcs[cmd->u.svc.get.svc],
            cmd->u.svc.get.enabled ? "enabled" : "disabled");
}

int wlanstats_send_ioctl_command(const char *interface, struct iwreq *iwr,
        struct wlanstats_uapi_ioctl *iocmd)
{
    int fd, res;

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0)
        return -1;

    if (!strstr(interface, "wifi"))
        fprintf(stderr, "warning: %s doesn't look like wifi interface\n",
                interface);

    res = strlcpy(iwr->ifr_ifrn.ifrn_name, interface, sizeof(iwr->ifr_ifrn.ifrn_name));
    if ((res < 0) || (res >= sizeof(iwr->ifr_ifrn.ifrn_name)))
    {
        fprintf(stderr, "error: interface name \"%s\" too long\n", interface);
        goto exit;
    }

    iwr->u.data.pointer = iocmd;
    iwr->u.data.length = WLANSTATS_UAPI_IOCTL_SIZE;

    if (ioctl(fd, WLANSTATS_UAPI_IOCTL_SET, iwr) < 0)
        goto exit;

    if (ioctl(fd, WLANSTATS_UAPI_IOCTL_GET, iwr) < 0)
        goto exit;


    return 0;

exit:
    close(fd);
    return -1;
}
