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

#ifndef _HIF_USB_INTERNAL_H
#define _HIF_USB_INTERNAL_H

#include "hif_api.h"
#if LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,19)
#include <linux/usb_ch9.h>
#else
#include <linux/usb/ch9.h>
#endif

#include<linux/interrupt.h>
#include <linux/autoconf.h>

//#define ROM_ENUM_DISABLED
#define hif_usb_assert(_cond)   ({  \
        int __ret = !(_cond); \
        if(__ret){            \
        dump_stack();     \
        panic("Asserting %s:%d for condition = ! %s\n", \
            __FUNCTION__, __LINE__, #_cond);    \
        }                                           \
        __ret;                                      \
        })                                              \

#if (ATH_SUPPORT_AOW == 1)
#define USB_ENDPT_NUM 5
#else
#define USB_ENDPT_NUM 4
#endif


#ifdef HIF_STREAM_MODE
#define HIF_USB_STREAM_TAG          0x697e
#define HIF_USB_STREAM_TAG_LEN      4
#if HIF_STREAM_SCHEDULE_METHOD == 1
#define USB_MAX_TX_BUFFER           8
#else
#ifdef BUILD_8M
#define USB_MAX_TX_BUFFER           6
#else
#define USB_MAX_TX_BUFFER           8
#endif
#define TX_URB_NUM_NORMAL           3
#endif
#ifdef BUILD_8M
#define USB_MAX_RX_BUFFER           6
#else
#define USB_MAX_RX_BUFFER           8
#endif
#define MAX_TX_BUF_NUM              256
#define MAX_RX_BUF_NUM              256

#define MAX_USB_IN_NUM              50
#define MAX_TX_AGGREGATE_NUM        8
#define MAX_TX_AGGREGATE_NUM_ITEM   50
#define USB_MAX_RX_BUFF_SIZE        15800
#define USB_TX_BUF_SIZE             15800
#define HIF_USB_MAX_PACKET_LEN      1600
#define HIF_TIMER_PERIOD            4 /* 4ms */
#define HIF_TIMER_JIFFIES_CNT       (HIF_TIMER_PERIOD * HZ) / 1000
#define HIF_ONE_SEC_ITER            (1000 / HIF_TIMER_PERIOD)
#define L1_TX_PACKET_THR            7000
#define HIF_USB_TX_TIMEOUT          1000
#define HIF_QUEUE_SIZE              10240
#else
#define USB_MAX_RX_BUFF_SIZE        1664
#define USB_MAX_TX_BUFFER           32
#define USB_MAX_RX_BUFFER           32
#endif

#define USB_MAX_CMD_BUFF_SIZE       900

#ifdef BUILD_8M
#define USB_MAX_CMD_BUFFER          1
#define USB_MAX_EVENT_BUFFER        1
#else
#define USB_MAX_CMD_BUFFER          4
#define USB_MAX_EVENT_BUFFER        4
#endif
#if (ATH_SUPPORT_AOW == 1)
#define ISOC_MAX_BUFFER        1
#define AUDBUF_SIZE 4096 /* Sufficient for 7.1CH, 32bit, 48KHz, 2ms data */
#endif


#ifdef HIF_STREAM_MODE
#define timersub(a, b, result)                                                \
  do {                                                                        \
    (result).tv_sec = (a).tv_sec - (b).tv_sec;                                \
    (result).tv_usec = (a).tv_usec - (b).tv_usec;                             \
    if ((result).tv_usec < 0) {                                               \
      --(result).tv_sec;                                                      \
      (result).tv_usec += 1000000;                                            \
    }                                                                         \
  } while (0)

typedef struct usb_rx_info
{
    uint16_t pkt_len;
    uint16_t offset;
} usb_rx_info_t;

typedef struct _usb_tx_buf_t {
    struct sk_buff*       buf;
#ifdef HIF_STREAM_MODE
#if HIF_STREAM_SCHEDULE_METHOD != 1
    struct timeval        timeval;
#endif
#endif
} usb_tx_buf_t;
#endif

struct total_cfg_descriptor {
    struct usb_config_descriptor cfg_des;	//9
    struct usb_interface_descriptor if_des;	//9
    struct usb_endpoint_descriptor endpt_des[USB_ENDPT_NUM];//7*USB_ENDPT_NUM
};

typedef struct usb_config
{
    uint16_t  idVendor;         //= 0x0CF3;
    uint16_t  idProduct;        //= 0x9330;
    uint16_t  iManufacturer;    //= 0x10;
    uint16_t  iProduct;         //= 0x20;
    uint16_t  iSerialNumber;    //= 0x30;
    uint8_t    bMaxPower;        //= 0xFA;
    uint8_t    bUSBPhyBias;      //= 0x3;
} usb_config_t;

typedef struct endpt_config
{
    uint8_t  address;
    uint8_t  direction;
    uint8_t  type;
    uint16_t maxPktSize;
    uint8_t  interval;
} endpt_config_t;

//structure for get info from otp
struct otp_usb_hdr {
    u8 len;
    u8 pad1;
    u8 pad2;
    u8 pad3;
};

#define MAX_STR_SIZE 128
struct usb_string_descriptor_otp
{
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint16_t wData[MAX_STR_SIZE];
};

#ifndef BUILD_PARTNER_PLATFORM
// Read USB info patch from OTP
#define OTP_USB_TBL_OFFSET      0x10
#define OTP_BASE                0x0
#define OTP_SRAM_BASE           0xBD000700

#ifdef CONFIG_MACH_AR934x
#define OTP_STATUS1_ADDRESS     0x101c
#define OTP_STATUS0_ADDRESS     0x1018
#define OTP_MEM_ADDRESS         0x0
#else
#define OTP_STATUS1_ADDRESS     0x15f1c
#define OTP_STATUS0_ADDRESS     0x15f18
#define OTP_MEM_ADDRESS         0x14000
#endif

#define OTP_STATUS0_EFUSE_READ_DATA_VALID_MSB           2
#define OTP_STATUS0_EFUSE_READ_DATA_VALID_LSB           2
#define OTP_STATUS0_EFUSE_READ_DATA_VALID_MASK          0x00000004
#define OTP_STATUS0_EFUSE_READ_DATA_VALID_GET(x)        (((x) & OTP_STATUS0_EFUSE_READ_DATA_VALID_MASK) >> OTP_STATUS0_EFUSE_READ_DATA_VALID_LSB)
#define OTP_STATUS0_EFUSE_READ_DATA_VALID_SET(x)        (((x) << OTP_STATUS0_EFUSE_READ_DATA_VALID_LSB) & OTP_STATUS0_EFUSE_READ_DATA_VALID_MASK)
#define OTP_STATUS0_EFUSE_ACCESS_BUSY_MSB               1
#define OTP_STATUS0_EFUSE_ACCESS_BUSY_LSB               1
#define OTP_STATUS0_EFUSE_ACCESS_BUSY_MASK              0x00000002
#define OTP_STATUS0_EFUSE_ACCESS_BUSY_GET(x)            (((x) & OTP_STATUS0_EFUSE_ACCESS_BUSY_MASK) >> OTP_STATUS0_EFUSE_ACCESS_BUSY_LSB)
#define OTP_STATUS0_EFUSE_ACCESS_BUSY_SET(x)            (((x) << OTP_STATUS0_EFUSE_ACCESS_BUSY_LSB) & OTP_STATUS0_EFUSE_ACCESS_BUSY_MASK)
#endif /* BUILD_PARTNER_PLATFORM */


typedef void (*complete_t)(struct usb_ep*,struct usb_request*);

#if (ATH_SUPPORT_AOW == 1)
#define MAX_PIPES 5
#else
#define MAX_PIPES 4
#endif

typedef struct hif_usb_request
{
    struct sk_buff *skb;   
    struct usb_request* request;
    void   *parent;
    struct hif_usb_request *next;
#ifdef HIF_STREAM_MODE
    uint8_t *usb_txbuf;
#endif
} hif_usb_request_t;

typedef struct hif_usb_pipe
{
    int number;
    int direction;
    struct usb_ep	*ep;
    struct usb_endpoint_descriptor *descriptor;
    spinlock_t	list_lock;
    int req_count;   /*Nbr of element in the list */
    struct hif_usb_request *head;
    struct hif_usb_request *tail;
#ifdef HIF_STREAM_MODE
    usb_tx_buf_t*         txbuf_q;
    int16_t               txbuf_head;
    int16_t               txbuf_tail;
    int16_t               txbuf_cnt;
#if HIF_STREAM_SCHEDULE_METHOD != 1
    int                   total_size;
    atomic_t              txframe_persec;
    atomic_t              txframe_cnt;
#endif
    spinlock_t            pipe_lock;
    spinlock_t            xmit_lock;
#endif
} hif_usb_pipe_t;


typedef struct hif_usb_context {
    hif_callback_t    hif_cb; 
    struct usb_gadget *gadget;
    struct usb_request *setup_req;
    void* app_handle;
#if (ATH_SUPPORT_AOW == 1)
    hif_usb_pipe_t  pipes[MAX_PIPES];
#else
    hif_usb_pipe_t  pipes[4];
#endif
    uint16_t config;
    uint16_t interface;
    uint16_t hif_ready;
    uint16_t start_hif;

    /*skb queue for tx pkts */
    struct sk_buff_head    tx_queue;
    spinlock_t             txq_lock;

    /*tasklet for processing tx pkts */
    struct tasklet_struct  tx_tasklet;
#ifdef HIF_STREAM_MODE
#if HIF_STREAM_SCHEDULE_METHOD == 1
    struct tasklet_struct  stream_tx_tasklet;
#else
    struct timer_list      one_sec_timer;
#endif /* #if HIF_STREAM_SCHEDULE_METHOD == 1 */
#endif

    // Left a door for extension the structure
    void *reserved;      
} hif_usb_context_t;


extern void init_device_des(void);
extern struct usb_endpoint_descriptor *get_endpt_des(int pipe_add, 
                                                     int pipe_dir);
extern void init_qualifier_des(void);
extern void init_configuration_des(void);
extern void init_other_speed_cfg_des(void);
extern void init_endpt_des(void);
extern int get_configuration(const struct usb_ctrlrequest *ctrl, uint8_t *buf);
extern int get_string_descriptor(const struct usb_ctrlrequest *ctrl, 
                                 uint8_t *buf, int read_from_otp);
extern void hif_read_usb_config(void);

#endif
