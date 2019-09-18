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

#ifndef _HIF_USB_H
#define _HIF_USB_H

#include <linux/types.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include "hif_api.h"
#include <linux/skbuff.h>
#include <linux/usb.h>

#define PRODUCT_AR9330          0x9330
#define PRODUCT_AR9342          0x9342

#define hif_usb_assert(_cond)    ({     \
        int __ret = !(_cond);                  \
        if (__ret) {                  \
        dump_stack();               \
        panic("Asserting %s:%d for condition = !%s\n",   \
            __FUNCTION__, __LINE__, #_cond);  \
        }                               \
        __ret;                            \
        })

#define timersub(a, b, result)                                                \
  do {                                                                        \
    (result).tv_sec = (a).tv_sec - (b).tv_sec;                                \
    (result).tv_usec = (a).tv_usec - (b).tv_usec;                             \
    if ((result).tv_usec < 0) {                                               \
      --(result).tv_sec;                                                      \
      (result).tv_usec += 1000000;                                            \
    }                                                                         \
  } while (0)

#define RX_URB_BUF_ALLOC_TIMER    100

#define DISABLE_USB_MPHP
//#define ROM_ENUM_DISABLED


#define MAX_EVENT_BUFFER_SIZE      512
#ifndef ATH_SUPPORT_AOW
#define ATH_SUPPORT_AOW 0
#endif 

#if (ATH_SUPPORT_AOW == 1)
#define MAX_ISOC_BUFFER_SIZE      32768
/* #define NUM_STEREO 2 */

/* 4=1ms --> Support 4 Stereo
   5=2ms --> Support 2 Stereo
*/
#define USB_bInterval 4				/* Variable */
#define SAMPLE_FREQ 48000 			/* Variable */
#define SAMPLE_SIZE 2     			/* Variable */
#define MAX_STEREO_CHANNELS 4 		/* Constant */

#define ISOEP_INTVAL_MS ((125 * (2<<(USB_bInterval-2)))/1000) 	/* Constant */
#define AUDSIZE_MONO_MS    (SAMPLE_FREQ * SAMPLE_SIZE * 1 /1000) /* Constant */
#define AUDSIZE_STEREO_MS (SAMPLE_FREQ * SAMPLE_SIZE * 2 /1000) /* Constant */
#define AUDSIZE_STEREO_nMS (AUDSIZE_STEREO_MS * ISOEP_INTVAL_MS)

#define AUDSIZE_ALLCH_MS    (AUDSIZE_MONO_MS * num_channels)  /* Constant */
#define AUDSIZE_ALLCH_nMS   (AUDSIZE_MONO_MS * ISOEP_INTVAL_MS * num_channels)  /* Constant */
#define MAX_AUDSIZE       (AUDSIZE_STEREO_MS * MAX_STEREO_CHANNELS * ISOEP_INTVAL_MS ) /* Constant */

#define QUEUE_DEPETH_300MS (300/ISOEP_INTVAL_MS) /* Constant */
#define QUEUE_DEPETH_200MS (200/ISOEP_INTVAL_MS) /* Constant */
#define QUEUE_DEPETH_100MS (100/ISOEP_INTVAL_MS) /* Constant */
#endif


#define CMD_PIPE_INDEX              0
#define LP_TX_PIPE_INDEX	    1
#ifndef DISABLE_USB_MPHP
#define MAX_TX_PIPES	            4
#define HP_TX_PIPE_INDEX            2
#define MP_TX_PIPE_INDEX            3
#else
#define MAX_TX_PIPES	            2
#endif
#define MAX_RX_PIPES                2

#ifdef HIF_STREAM_MODE
#define HIF_USB_STREAM_TAG_LEN      4
#define HIF_USB_STREAM_TAG          0x697e
#define MAX_TX_URB_NUM              8
#define TX_URB_NUM_NORMAL           3
#define MAX_USB_TX_BUF_SIZE         15800
#define MAX_TX_BUF_NUM              512
#define MAX_RX_BUF_NUM              512

#define MAX_TX_AGGREGATE_NUM_ITEM   50
#define MAX_TX_AGGREGATE_NUM        12
#define L1_TX_AGGREGATE_NUM         10
#define TX_PACKET_QUEUE_THR         7000
#define RX_PACKET_QUEUE_THR         7000

#define MAX_RX_URB_NUM              8
#define MAX_RX_BUFFER_SIZE          15800
#define MAX_USB_IN_NUM              50 //20
#define HIF_USB_MAX_PACKET_LEN      4000
#define HIF_TIMER_PERIOD            4 /* 4ms */
#define HIF_TIMER_JIFFIES_CNT       (HIF_TIMER_PERIOD * HZ) / 1000
#define HIF_ONE_SEC_ITER            (1000 / HIF_TIMER_PERIOD)
#define HIF_USB_TX_TIMEOUT          1000
#define HIF_QUEUE_SIZE              10240
#else
#define MAX_TX_URB_NUM              32
#define MAX_RX_URB_NUM              32
#define MAX_RX_BUFFER_SIZE          1664
#endif
#define MAX_CMD_URB_NUM             4

typedef struct usb_rx_info
{
    uint16_t pkt_len;
    uint16_t offset;
} usb_rx_info_t;

#define urb_t                       struct urb

/* USB Endpoint definition */
#define USB_DATA_TX_PIPE                1
#define USB_DATA_RX_PIPE                2
#if (ATH_SUPPORT_AOW == 1)
#define ISO_OUT_PIPE                3
#endif
#ifdef ROM_ENUM_DISABLED
#define USB_EVENT_PIPE                 3
#else
#define USB_EVENT_PIPE                 4
#endif
#define USB_CMD_PIPE                4

#ifndef DISABLE_USB_MPHP
#define USB_DATA_HP_TX_PIPE             5
#define USB_DATA_MP_TX_PIPE             6
#endif

struct _hif_usb_device;


typedef struct usb_tx_urb_ctx {
    uint8_t                 in_use;
    uint8_t                 pipe_index;
    struct _hif_usb_device  *device;
    struct sk_buff          *buf;
    urb_t                   *tx_urb;
    struct usb_tx_urb_ctx   *next;
    struct usb_tx_urb_ctx   *alloc_next;
#ifdef HIF_STREAM_MODE
    uint8_t                 *usb_txbuf;
#endif
} usb_tx_urb_ctx_t;


typedef int32_t (*submit_fn_t)(urb_t*, struct usb_device*,
                               uint16_t, uint16_t , uint8_t*,
                               int, usb_complete_t, void*, int);

#ifdef HIF_STREAM_MODE
typedef struct _usb_tx_buf_t {
    struct sk_buff*       buf;
    struct timeval        timeval;
} usb_tx_buf_t;
#endif

typedef struct _hif_usb_tx_pipe {
    uint8_t               pipe_num;
    spinlock_t		      pipe_lock;
    uint16_t              urb_cnt;
    usb_tx_urb_ctx_t*     head;
    usb_tx_urb_ctx_t*     tail;
    usb_tx_urb_ctx_t*	  alloc_head;
    submit_fn_t           submit;
    usb_complete_t        tx_complete_cb;
#ifdef HIF_STREAM_MODE
    usb_tx_buf_t*         txbuf_q;
    int16_t               txbuf_head;
    int16_t               txbuf_tail;
    int16_t               txbuf_cnt;
    int                   total_size;
    atomic_t              txframe_persec;
    atomic_t              txframe_cnt;
    struct timeval        last_time_xmit;
    spinlock_t            xmit_lock;
#endif
} hif_usb_tx_pipe_t;

#define MAX_APP_CONTEXT 2

typedef struct _usb_rx_urb_ctx {
    uint8_t                 in_use;
    struct _hif_usb_device  *device;
    struct sk_buff          *buf;
    urb_t                   *rx_urb;
} usb_rx_urb_ctx_t;

typedef struct _hif_usb_device {
    void                    *app_handle;
    hif_callback_t  callbacks[MAX_APP_CONTEXT];

    /* App Context used for remove handler */
    void*			        app_context;
    int			            attach_app_idx;

    /*Usb specific information */
    struct usb_device       *udev;
    struct usb_interface    *interface;

    /*For Rx Data */
    usb_rx_urb_ctx_t         rx_urb_ctx[MAX_RX_URB_NUM];

    /*For Interrupt Data */
    struct sk_buff *        event_buf;
    urb_t                   *event_urb;

#if (ATH_SUPPORT_AOW == 1)
    /*For ISOC Data */
    //char isoc_data[MAX_ISOC_BUFFER_SIZE/AUDSIZE][AUDSIZE];
    struct sk_buff_head isoc_data;
	int isoc_data_init;
	int isoc_data_len;
	int isoc_intval;
	int isoc_in_progress;
    urb_t                   *isoc_urb;
#endif

    /*For Tx Data */
    hif_usb_tx_pipe_t      tx_pipe[MAX_TX_PIPES];

    /*skb queue for rx pkts */
    struct sk_buff_head    rx_queue;
    spinlock_t             rxq_lock;

    /*tasklet for processing rx pkts */
    struct tasklet_struct  rx_tasklet;

#ifdef HIF_STREAM_MODE
    struct timer_list      one_sec_timer;

    atomic_t               rxframe_persec;
    atomic_t               rxframe_cnt;
#endif

    /*Flag for surprise removal */
    uint8_t                 surprise_removed;

    uint8_t                 protocol_fail_cnt;

} hif_usb_device_t;



#endif
