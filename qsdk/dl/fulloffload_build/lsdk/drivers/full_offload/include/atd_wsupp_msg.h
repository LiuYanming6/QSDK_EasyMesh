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

#ifndef ATD_WSUPP_MSG_H
#define ATD_WSUPP_MSG_H

#include <a_base_types.h>
#include <acfg_api_types.h>

#define WSUPP_MSG_UNIQUE_BIN_LEN    ACFG_WSUPP_UNIQUE_LEN
#define WSUPP_MSG_IFNAME_LEN        ACFG_MAX_IFNAME

enum {
    WSUPP_MSG_TYPE_NONE = 0,
    WSUPP_MSG_TYPE_WMI,
    WSUPP_MSG_TYPE_WMI_EVENT,
    WSUPP_MSG_TYPE_WPACLI_GLOBAL,
    WSUPP_MSG_TYPE_WPACLI_IFACE,
    WSUPP_MSG_TYPE_WPACLI_EVENT,
    WSUPP_MSG_TYPE_INTERNAL,
};

typedef struct wsupp_msg {
    int     wm_type;
    int     wm_wmi;
    char    wm_unique[WSUPP_MSG_UNIQUE_BIN_LEN];
    char    wm_ifname[WSUPP_MSG_IFNAME_LEN];
    int     wm_len;
    char    wm_data[0];
} __attribute__((__packed__)) wsupp_msg_t;

typedef struct wsupp_wmi_reply {
    a_status_t status;
    char data[0];
} __attribute__((__packed__)) wsupp_wmi_reply_t;

#endif

