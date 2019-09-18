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
 * @Abstract: WMI API definations
 *
 */
#ifndef WMI_API_H_
#define WMI_API_H_

#include <wmi.h>

typedef void* wmi_handle_t;
#define WMI_SVC_MSG_SIZE                1536    /* maximum size of any WMI 
                                                   control or event message */
#define WMI_MAX_RECVQUEUE_LEN 1
typedef void (* wmi_cmd_handler_t)(void *ctx, a_uint16_t cmdid,
                                   a_uint8_t *buffer, a_int32_t Length);

/* command dispatch entry */

typedef struct _wmi_dispatch_entry {      
    wmi_cmd_handler_t      cmd_handler;    /* dispatch function */
    wmi_id_t               cmdid;          /* WMI command to dispatch from */
    a_uint16_t             flags;
} wmi_dispatch_entry_t;

/* dispatch table that is used to register a set of dispatch entries */
typedef struct _wmi_dispatch_table {
    void                 *ctx;        
    int                   noentries;  
    wmi_dispatch_entry_t *disp_entry;           /* start of table */
} wmi_dispatch_table_t;


#define wmi_dispatch_entry_count(_table) \
        (sizeof((_table)) / sizeof(wmi_dispatch_entry_t))

/** 
 * @brief 
 * 
 * @param htccontex
 * 
 * @return 
 */
wmi_handle_t 
wmi_init (void *htccontex);

/** 
 * @brief 
 * 
 * @param handle    (WMI Softc)
 * @param val       (0:non-stop, 1:stop)
 * 
 * @return 
 */
void wmi_set_stopflag(wmi_handle_t handle, a_uint8_t val);

/** 
 * @brief De-initiallize the WMI
 * 
 * @param handle    (WMI Softc to free)
 */
void wmi_cleanup(wmi_handle_t handle);

/** 
 * @brief Allocate a WMI SKB for Event or Response
 * 
 * @param handle    (WMI Softc)
 * @param len       (Len of buffer including ATD payload)
 * 
 * @return          (SKB or NULL) 
 */
struct sk_buff * 
wmi_alloc_event(wmi_handle_t handle, a_uint32_t len);

/** 
 * @brief Resgister Dispatch Table
 * 
 * @param handle
 * @param table
 */
void 
wmi_register_disp_table(wmi_handle_t handle, wmi_dispatch_table_t *table);

/** 
 * @brief Send a WMI Event 
 * 
 * @param handle    (WMI Softc)
 * @param netbuf    (Buffer to send with WMI header space & payload)
 * @param id        (wmi_id_t type)
 * @param seqno     (Sequence number)
 * @param len       (len of the buffer)
 */
void 
wmi_send_event(wmi_handle_t handle, struct sk_buff * netbuf, wmi_id_t  id);
/** 
 * @brief 
 * 
 * @param handle    (WMI Softc)
 * @param cmd_id    (WMI CMD id for which the response is sent)
 * @param status    (atd status )
 * @param buffer    (Buffer to send the data)
 * @param len       (Length of the buffer)
 */
void 
wmi_cmd_rsp(wmi_handle_t handle, wmi_id_t cmd_id, a_status_t status, 
            a_uint8_t *buffer, a_int32_t len);
#if 0
/** 
 * @brief Return the total reserve headroom needed by WMI pkt
 * 
 * @param handle
 * 
 * @return Total (WMI_HDR + HTC_HDR + HIF_HDR)
 */
static inline a_uint32_t
wmi_reserve_header(wmi_softc_t *sc)
{
    return (sizeof(wmi_hdr_t) + htc_get_reserveheadroom(sc->htc_handle));
}
#endif

#endif /*  WMI_API_H_ */

