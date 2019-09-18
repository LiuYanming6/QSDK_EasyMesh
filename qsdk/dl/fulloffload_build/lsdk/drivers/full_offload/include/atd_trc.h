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

#ifndef __ATD_TRC_H
#define __ATD_TRC_H


#define ATD_DEBUG_ENABLE 1


#ifdef ATD_DEBUG_ENABLE

#define ATD_DEBUG_FUNCTRACE     0x01
#define ATD_DEBUG_LEVEL0        0x02
#define ATD_DEBUG_LEVEL1        0x04
#define ATD_DEBUG_LEVEL2        0x08
#define ATD_DEBUG_LEVEL3        0x10
#define ATD_DEBUG_ERROR         0x20
#define ATD_DEBUG_CFG           0x40

extern int atd_dbg_mask ;

#define atd_trace(log_level, data)  do {    \
  if(log_level & atd_dbg_mask) {            \
    adf_os_print("%s ",adf_os_function);    \
    adf_os_print data ;                    \
    adf_os_print("\n");                     \
  }                                         \
} while (0)

#else

#define atd_trace(log_level, data) 

#endif

#endif /* __ATD_TRC_H */
