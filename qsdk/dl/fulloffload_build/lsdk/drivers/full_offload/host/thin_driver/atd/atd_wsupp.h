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

#ifndef __ATD_WSUPP_H
#define __ATD_WSUPP_H

#include <atd_internal.h>

typedef void * wsupp_handle_t;

#define WPS_DEF_PBC_DUR_IGNORE_SECS     10  /* Default duration in seconds
                                             * after which we ignore
                                             * WPS button push
                                             */

/** 
 * @brief Initialize the wsupp helper for a device
 * 
 * @param dev
 * @param htc_handle
 * 
 * @return 
 */
wsupp_handle_t 
wsupp_helper_init(void *dev, htc_handle_t htc_handle);

/** 
 * @brief Cleanup the wsupp helper
 * 
 * @param wsupp_handle
 */
void 
wsupp_helper_cleanup(wsupp_handle_t wsupp_handle);

/** 
 * @brief Bind the wsupp helper service with HTC endpoint number
 * 
 * @param wsupp_handle
 * @param wh_epid
 * 
 * @return 
 */
a_status_t  
wsupp_helper_connect(wsupp_handle_t wsupp_handle, htc_endpointid_t *wh_epid);

#endif
