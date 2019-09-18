/*
 * Copyright (c) 2018 Qualcomm Innovation Center, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Innovation Center, Inc.
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

#ifndef DIV_ROUND_UP
#define DIV_ROUND_UP(x, y) ((x + (y - 1)) / (y))
#endif

#include <wlanstats_common.h>

static void usage(const char *name, const char *reason, ...);

#define die(fmt, ...)							\
		do {							\
			fprintf(stderr, "errno: %d (%s), " fmt "\n",	\
				errno,					\
				strerror(errno),			\
				## __VA_ARGS__);			\
			exit(EXIT_FAILURE);				\
		} while (0)

static void peer_rx_stats_handle(struct wlanstats_uapi_ioctl *cmd)
{
    wlanstats_peer_rx_stats_handle(cmd);
}

static void peer_rx_stats_prepare(int argc,
				  const char **argv,
				  struct wlanstats_uapi_ioctl *cmd)
{
    char mac[6] = {};
    int err;

    if (argc < 4)
        usage(argv[0], "peer_rx_stats: missing argument: mac address");
    if (argc > 4)
        usage(argv[0], "peer_rx_stats: too many arguments");

    err = sscanf(argv[3],
            "%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx",
            &mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5]);
    if (err != 6)
        die("sscanf()=%d of mac address failed", err);

    wlanstats_peer_rx_stats_prepare(cmd, mac);
}

static void peer_tx_stats_prepare(int argc,
				  const char **argv,
				  struct wlanstats_uapi_ioctl *cmd)
{
    char mac[6] = {};
    int err;

    if (argc < 4)
        usage(argv[0], "peer_tx_stats: missing argument: mac address");
    if (argc > 4)
        usage(argv[0], "peer_tx_stats: too many arguments");

    err = sscanf(argv[3],
            "%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx",
            &mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5]);
    if (err != 6)
        die("sscanf()=%d of mac address failed", err);

    wlanstats_peer_tx_stats_prepare(cmd, mac);
}

static void peer_tx_stats_handle(struct wlanstats_uapi_ioctl *cmd)
{
    wlanstats_peer_tx_stats_handle(cmd);
}

static void survey_chan_prepare(int argc,
				const char **argv,
				struct wlanstats_uapi_ioctl *cmd)
{
    wlanstats_survey_chan_prepare(cmd);
}

static void survey_chan_handle(struct wlanstats_uapi_ioctl *cmd)
{
    wlanstats_survey_chan_handle(cmd);
}

static void survey_bss_prepare(int argc,
			       const char **argv,
			       struct wlanstats_uapi_ioctl *cmd)
{
    wlanstats_survey_bss_prepare(cmd);
}

static void survey_bss_handle(struct wlanstats_uapi_ioctl *cmd)
{
    wlanstats_survey_bss_handle(cmd);
}

static void q_util_prepare(int argc,
                            const char **argv,
                            struct wlanstats_uapi_ioctl *cmd)
{
    wlanstats_q_util_prepare(cmd);
}

static void q_util_handle(struct wlanstats_uapi_ioctl *cmd)
{
    wlanstats_q_util_handle(cmd);
}

static void svc_prepare(int argc,
		        const char **argv,
		        struct wlanstats_uapi_ioctl *cmd)
{
    int ret = 0;
    switch (argc) {
        case 4:
            ret = wlanstats_svc_prepare(argc, cmd, argv+3);
            break;
        case 5:
            ret = wlanstats_svc_prepare(argc, cmd, argv+4);
            break;
        default:
            ret = wlanstats_svc_prepare(argc, cmd, NULL);
            break;
    }

    if (ret < 0) {
        die("Exiting\n");
    }
}

static void svc_handle(struct wlanstats_uapi_ioctl *cmd)
{
    wlanstats_svc_handle(cmd);
}

struct command {
	const char *name;
	const char *usage;
	void (*prepare)(int argc,
			const char **argv,
			struct wlanstats_uapi_ioctl *cmd);
	void (*handle)(struct wlanstats_uapi_ioctl *cmd);
};

static const struct command commands[] = {
	{
		.name = "peer_rx_stats",
		.usage = "<xx:yy:zz:aa:bb:cc>",
		.prepare = peer_rx_stats_prepare,
		.handle = peer_rx_stats_handle,
	},
	{
		.name = "survey_chan",
		.usage = "",
		.prepare = survey_chan_prepare,
		.handle = survey_chan_handle,
	},
	{
		.name = "survey_bss",
		.usage = "",
		.prepare = survey_bss_prepare,
		.handle = survey_bss_handle,
	},
	{
		.name = "peer_tx_stats",
		.usage = "<xx:yy:zz:aa:bb:cc>",
		.prepare = peer_tx_stats_prepare,
		.handle = peer_tx_stats_handle,
	},
	{
		.name = "q_util",
		.usage = "",
		.prepare = q_util_prepare,
		.handle = q_util_handle,
	},
	{
		.name = "svc",
		.usage = "[<service name> [0|1]]",
		.prepare = svc_prepare,
		.handle = svc_handle,
	},
};

static void usage(const char *name, const char *reason, ...)
{
	int i;
	va_list args;

	fprintf(stderr, "usage error: ");
	va_start(args, reason);
	vfprintf(stderr, reason, args);
	va_end(args);
	fprintf(stderr, "\n\n");

	fprintf(stderr,
		"usage: %s <wifiX> <command> [arguments..]\n"
		"commands:\n",
		name);

	for (i = 0; i < WSTATS_ARRAY_SIZE(commands); i++)
		fprintf(stderr, "  %s %s\n",
			commands[i].name,
			commands[i].usage);

	exit(EXIT_FAILURE);
}

int main(int argc, const char **argv)
{
    struct iwreq iwr = {0};
    struct wlanstats_uapi_ioctl iocmd = {0};
    const struct command *cmd = NULL;
    int i;

    if (argc < 2)
        usage(argv[0], "missing argument: interface");
    if (argc < 3)
        usage(argv[0], "missing argument: command");

    for (i = 0; i < WSTATS_ARRAY_SIZE(commands); i++) {
        if (!strcmp(commands[i].name, argv[2])) {
            cmd = &commands[i];
            break;
        }
    }

    if (!cmd)
        usage(argv[0],
                "invalid argument: command '%s' not found",
                argv[2]);

    cmd->prepare(argc, argv, &iocmd);

    if (wlanstats_send_ioctl_command(argv[1], &iwr, &iocmd) < 0)
        return 0;

    cmd->handle(&iocmd);

    return 0;
}
