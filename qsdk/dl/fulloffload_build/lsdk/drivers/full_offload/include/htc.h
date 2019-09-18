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

#ifndef __HTC_H__
#define __HTC_H__

typedef enum
{
    ENDPOINT_UNUSED = -1,
    HTC_CONTROL_EPID = 0,
    HTC_WMICMD_EPID  = 1,
    HTC_DATA_EPID    = 2,  
    HTC_WSUPP_EPID   = 3,  
    ENDPOINT_MAX = 10 
} htc_endpointid_t;
typedef enum
{
    WMI_CONTROL_SVC,
    WMI_DATA_BE_SVC,
    WMI_WSUPP_SVC,
} htc_service_t;


typedef struct _htc_frame_hdr{
    a_uint8_t       epid;
    a_uint8_t       flags;
    a_uint16_t      pldlen;
    a_uint16_t      seqno;

    a_uint16_t      reserved; /**< 2 Byte padding */
} __attribute__((__packed__)) htc_frame_hdr_t;


/* HTC control message IDs */
typedef enum {
    HTC_MSG_READY_ID = 1,
} htc_msgid_t ;
 
/* base message ID header */
typedef struct {
    a_uint16_t msg_id;    
    a_uint16_t pad;    
} __attribute__((__packed__)) htc_unknown_msg_t;
                                                     
typedef struct {
    a_uint16_t  MessageID;   
    a_uint16_t  pad;   
} __attribute__((__packed__)) htc_ready_msg_t;


#endif /* __HTC_H__ */

