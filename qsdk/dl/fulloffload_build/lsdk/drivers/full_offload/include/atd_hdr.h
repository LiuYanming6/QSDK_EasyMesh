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
#ifndef __ATD_HDR_H
#define __ATD_HDR_H

#include <a_base_types.h>

typedef struct atd_hdr {
    a_uint8_t   wifi_index;
    a_uint8_t   vap_index;
    a_uint16_t  res;
}__attribute__((__packed__)) atd_hdr_t;


typedef atd_hdr_t atd_rx_hdr_t;
typedef atd_hdr_t atd_tx_hdr_t;



#endif /*  __ATD_HDR_H */
