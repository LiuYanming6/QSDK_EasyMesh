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

#include <linux/types.h>
#include <linux/version.h>
#include <linux/usb.h>
#include <linux/netdevice.h>
#include <linux/skbuff.h>

#include "hif_usb.h"

#define VENDOR_ATHR             0x0CF3  //Atheros
#define PRODUCT_AR9330          0x9330
#define PRODUCT_AR9342          0x9342
#define ZM_MAX_USB_URB_FAIL_COUNT   1

static const char driver_name[] = "Atheros-hif-usb";
static struct usb_interface *g_hif_usb_ifc = NULL;

#define ROM_MODE 1
#ifdef ROM_MODE
static int first_time = 1; 
static int usb_registered = 0;
static int usb_shutdown = 0;
hif_usb_device_t*  g_hif_dev = NULL;
#endif
#if (ATH_SUPPORT_AOW == 1)
#ifndef ATH_SUPPORT_AOW_VSOUND
#define ATH_SUPPORT_AOW_VSOUND 0
#endif
int num_channels = 0;
struct sk_buff *audio_skb = NULL, *empty_skb = NULL;
hif_usb_device_t *aow_isoc_device = NULL;
int isoc_endpoint_found = -1;
#define AOW_ISOC_LOCK()
#define AOW_ISOC_UNLOCK()
#endif


/* table of devices that work with this driver */
static struct usb_device_id hif_ids [] = {
    { USB_DEVICE(VENDOR_ATHR, PRODUCT_AR9330) },
    { USB_DEVICE(VENDOR_ATHR, PRODUCT_AR9342) },
    { }                 /* Terminating entry */
};

#ifndef DISABLE_USB_MPHP
static const int pipe_index[MAX_TX_PIPES+MAX_RX_PIPES+1] = {-1,1,-1,-1,0,2,3};
#else
static const int pipe_index[MAX_TX_PIPES+MAX_RX_PIPES+1] = {-1,1,-1,-1,0};
#endif

MODULE_DEVICE_TABLE(usb, hif_ids);

#define app_dev_inserted(_cb, _hif_hdl)             \
    (_cb).device_detect((_hif_hdl))

#define app_dev_removed(_cb, _app_ctx)              \
    (_cb).device_disconnect((_app_ctx))

#define app_dev_suspend(_cb, _hif_hdl)             \
    (_cb).device_suspend((_hif_hdl))

#define app_dev_resume(_cb, _hif_hdl)             \
    (_cb).device_resume((_hif_hdl))

#define app_ind_xmitted(_cb, _pkt)            \
    if((_cb).send_buf_done)          \
(_cb).send_buf_done((_cb.context), (_pkt))

#define app_ind_pkt(_cb, _pkt, _pipe)         \
    if((_cb).recv_buf)             \
(_cb).recv_buf((_cb.context), (_pkt), (_pipe))

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,5,0)
#define SUBMIT_URB(u,f) usb_submit_urb(u,f)
#define USB_ALLOC_URB(u,f)  usb_alloc_urb(u,f)
#else
#define SUBMIT_URB(u,f) usb_submit_urb(u)
#define USB_ALLOC_URB(u,f)  usb_alloc_urb(u)
#endif

#define HIF_USB_DBG 1

hif_os_callback_t   drv_reg_callbacks;

static int hif_usb_probe(struct usb_interface *interface,
                         const struct usb_device_id *id);
static void hif_usb_disconnect(struct usb_interface *interface);
static int hif_usb_suspend(struct usb_interface *intf, pm_message_t message);
static int hif_usb_resume(struct usb_interface *intf);

#ifdef HIF_STREAM_MODE
static uint16_t
hif_usb_check_txbuf_cnt(hif_usb_tx_pipe_t *pipe, struct timeval *timestamp, int *hif_txq_size);
static hif_status_t
hif_usb_xmit(hif_usb_tx_pipe_t *pipe, uint32_t pipe_id);
static void
hif_usb_start_timer(hif_usb_device_t *hif_dev);
static void
hif_usb_stop_timer(hif_usb_device_t *hif_dev);
static int
hif_usb_rx_destream(struct sk_buff *skb, int length, usb_rx_info_t *rx_info);
#endif

hif_status_t hif_target_reset(hif_handle_t hif_handle);

int32_t 
hif_usb_submit_bulk_urb(urb_t *urb, struct usb_device *usb, uint16_t epnum, 
                        uint16_t direction, uint8_t *transfer_buffer, 
                        int buffer_length, usb_complete_t complete, 
                        void *context, int interval);

int hif_usb_submit_interrupt_buffer(hif_usb_device_t *device);
#if (ATH_SUPPORT_AOW == 1)
int hif_usb_submit_isoc_buffer(void);
#endif
hif_status_t usb_target_mode(struct usb_device *udev);
hif_status_t usb_target_reset(struct usb_device *udev);

static struct usb_driver hif_usb_driver = {
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,5,0))
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,15))
        .owner        = THIS_MODULE,
#endif
#endif
        .name         = driver_name,
        .probe        = hif_usb_probe,
        .disconnect   = hif_usb_disconnect,
        .suspend      = hif_usb_suspend,
        .resume       = hif_usb_resume,
        .id_table     = hif_ids,
    };

#ifdef HIF_USB_DEBUG
#define hif_usb_print(x)  printk x
#else
#define hif_usb_print(x) 
#endif
static inline void
_dbg_print_endpoints(struct usb_interface *interface)
{
    int i;
    struct usb_host_interface *iface_desc = &interface->altsetting[0];
    struct usb_endpoint_descriptor *endpoint;
#if (ATH_SUPPORT_AOW == 1)
    isoc_endpoint_found = 0;
#endif
    for (i = 0; i < iface_desc->desc.bNumEndpoints; ++i)
    {
        endpoint = &iface_desc->endpoint[i].desc;
#if (ATH_SUPPORT_AOW == 1)
        if (((endpoint->bEndpointAddress & 0x80) == 0) &&
                ((endpoint->bmAttributes & 3) == 0x01))
        {
            isoc_endpoint_found = 1;
            /* we found a isochrnous out endpoint */
            printk(KERN_ERR "Isoc out: wMaxPacketSize = %x\n", 
                    le16_to_cpu(endpoint->wMaxPacketSize));
            printk(KERN_ERR "Isoc out : int_interval = %d\n", 
                    endpoint->bInterval);
        }
#endif

        if ((endpoint->bEndpointAddress & 0x80) &&
                ((endpoint->bmAttributes & 3) == 0x02))
        {
            /* we found a bulk in endpoint */
            printk(KERN_ERR "bulk in: wMaxPacketSize = %x\n", 
                    le16_to_cpu(endpoint->wMaxPacketSize));
        }

        if (((endpoint->bEndpointAddress & 0x80) == 0x00) &&
                ((endpoint->bmAttributes & 3) == 0x02))
        {
            /* we found a bulk out endpoint */
            printk(KERN_ERR "bulk out: wMaxPacketSize = %x\n", 
                    le16_to_cpu(endpoint->wMaxPacketSize));
        }

        if ((endpoint->bEndpointAddress & 0x80) &&
                ((endpoint->bmAttributes & 3) == 0x03))
        {
            /* we found a interrupt in endpoint */
            printk(KERN_ERR "interrupt in: wMaxPacketSize = %x\n", 
                    le16_to_cpu(endpoint->wMaxPacketSize));
            printk(KERN_ERR "interrupt in: int_interval = %d\n", 
                    endpoint->bInterval);
        }

        if (((endpoint->bEndpointAddress & 0x80) == 0x00) &&
                ((endpoint->bmAttributes & 3) == 0x03))
        {
            /* we found a interrupt out endpoint */
            printk(KERN_ERR "interrupt out: wMaxPacketSize = %x\n", 
                    le16_to_cpu(endpoint->wMaxPacketSize));
            printk(KERN_ERR "interrupt out: int_interval = %d\n", 
                    endpoint->bInterval);
        }
    }

}

static inline void 
hif_usb_put_tx_ctx(hif_usb_tx_pipe_t *pipe, usb_tx_urb_ctx_t *urb_ctx)
{
    usb_tx_urb_ctx_t *tmp_ctx;
    unsigned long flag;  
    spin_lock_irqsave(&(pipe->pipe_lock), flag);
    urb_ctx->next = NULL;
    urb_ctx->in_use = 0;
    if(pipe->head == NULL)
        pipe->head = pipe->tail = urb_ctx;
    else{
        tmp_ctx = pipe->tail;
        pipe->tail = urb_ctx;
        tmp_ctx->next = pipe->tail;
    }
    pipe->urb_cnt++;
    hif_usb_print(("%s pipe %p hifIndex: %d urb ctx %p elements %d \n",
                __FUNCTION__, pipe, pipe->pipe_num, urb_ctx, pipe->urb_cnt));
    spin_unlock_irqrestore(&(pipe->pipe_lock), flag);
}

static inline usb_tx_urb_ctx_t*
hif_usb_get_tx_ctx(hif_usb_tx_pipe_t *pipe, int use_more)
{
    usb_tx_urb_ctx_t *result = NULL;
    unsigned long flag;  
    spin_lock_irqsave(&(pipe->pipe_lock), flag);

#ifdef HIF_STREAM_MODE
    if (pipe->pipe_num == USB_DATA_TX_PIPE &&
        use_more == 0 && pipe->urb_cnt <= (MAX_TX_URB_NUM - TX_URB_NUM_NORMAL)) {
        spin_unlock_irqrestore(&(pipe->pipe_lock), flag);
        return result;
    }
#endif

    if(pipe->head != NULL){
        result = pipe->head;
        pipe->head = pipe->head->next;
        if(pipe->head == NULL)
            pipe->tail = NULL;
        pipe->urb_cnt--;
        hif_usb_print(("%s pipe %p hifIndex: %d urbCtx %p elements %d \n",
                       __FUNCTION__, pipe, pipe->pipe_num, result, 
                       pipe->urb_cnt));
        result->in_use = 1;

    }
    else
        hif_usb_print(("%s pipe %p hifIndex: %d elements %d Ctx not found\n",
                       __FUNCTION__, pipe, pipe->pipe_num, pipe->urb_cnt));
    spin_unlock_irqrestore(&(pipe->pipe_lock), flag);

    return result;
}

static struct sk_buff *
hif_usb_alloc_skb(uint32_t size, uint32_t reserve, uint32_t align)
{
    struct sk_buff *skb = NULL;

    size += align;

    skb = dev_alloc_skb(size + reserve + align);
    if (!skb)
        return NULL;

    reserve += (ALIGN((unsigned long)skb->data, align) - 
            (unsigned long)skb->data);

    skb_reserve(skb, reserve);

    hif_usb_assert(((unsigned long)skb->data % align) == 0);

    return skb;

}

static inline void
hif_usb_add_timer(struct timer_list *timer, uint32_t delay)
{   
    timer->expires = jiffies + msecs_to_jiffies(delay);
    add_timer(timer);
}

void
hif_usb_process_rxq(unsigned long context)
{
    hif_usb_device_t *device = (hif_usb_device_t*)context;
    struct sk_buff* buf = NULL;
    unsigned long flag;

    hif_usb_print(("%s Entry \n",__FUNCTION__));
    
    while(1)
    {
        spin_lock_irqsave(&device->rxq_lock, flag);
        buf = __skb_dequeue(&device->rx_queue);
        spin_unlock_irqrestore(&device->rxq_lock, flag);

        if(!buf)
            break;

        app_ind_pkt(device->callbacks[device->attach_app_idx], buf, 
                    USB_DATA_RX_PIPE);

    }

}

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,5,0)) || \
    (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,19))
void 
hif_usb_interrupt_xmit_complete(urb_t *urb)
#else
void 
hif_usb_interrupt_xmit_complete(urb_t *urb, struct pt_regs *regs)
#endif
{
    usb_tx_urb_ctx_t *ctx = (usb_tx_urb_ctx_t*)urb->context;
    hif_usb_device_t *device = (hif_usb_device_t *)ctx->device;
    struct sk_buff *buf;

    buf = ctx->buf;
    ctx->buf = NULL;
    hif_usb_put_tx_ctx(&device->tx_pipe[CMD_PIPE_INDEX], ctx);

    if (urb->status != 0) {
        hif_usb_print(("%s : status=0x%x\n", __FUNCTION__, urb->status));
        dev_kfree_skb_any(buf);

        return;
    }

    if ( buf != NULL ) 
        app_ind_xmitted(device->callbacks[device->attach_app_idx], buf);
}


#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,5,0)) || \
    (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,19))
void 
hif_usb_bulk_rx_complete(urb_t *urb)
#else
void 
hif_usb_bulk_rx_complete(urb_t *urb, struct pt_regs *regs)
#endif
{
    usb_rx_urb_ctx_t *urb_ctx = (usb_rx_urb_ctx_t *)urb->context;
    hif_usb_device_t *device = urb_ctx->device;
    struct sk_buff *buf;
    struct sk_buff *new_buf;
    int status = 0;
#ifdef HIF_STREAM_MODE
    int i;
    int rxbuf_num;
    usb_rx_info_t rx_info[MAX_USB_IN_NUM];
    struct sk_buff *rxbuf_pool[MAX_USB_IN_NUM];
#endif

    unsigned long flag;

    /* Check status for URB */
    if (urb->status != 0) {
        if (urb->status == -ENOENT || 
            urb->status == -ECONNRESET ||
            urb->status == -ESHUTDOWN) {
            status = -1;
        }

        hif_usb_print(("%s: urb %s: %d\n", 
            __FUNCTION__, status ? "released" : "resubmit", urb->status));

        buf = urb_ctx->buf;
        urb_ctx->buf = NULL;
        dev_kfree_skb_any(buf);

        if (status)
            return;

        if(device->protocol_fail_cnt >= ZM_MAX_USB_URB_FAIL_COUNT) {
            printk("%s[%d]protocol error cnt=%d,urb->status=%d!!!!\n\r",__func__,__LINE__,device->protocol_fail_cnt,urb->status);
            return;
        }
        device->protocol_fail_cnt++;
          
        goto resubmit;
    }

    device->protocol_fail_cnt = 0;

    if (urb->actual_length == 0) {
        printk(KERN_ERR "Get an URB whose length is zero");
        dev_kfree_skb_any(urb_ctx->buf);
        goto resubmit;
    }

    /* Dequeue skb buffer */
    buf = urb_ctx->buf;

#ifdef HIF_STREAM_MODE
    rxbuf_num = hif_usb_rx_destream(buf, urb->actual_length, rx_info);
    spin_lock_irqsave(&device->rxq_lock, flag);
    /* limit the number of queued packets */
    if (device->rx_queue.qlen + rxbuf_num > MAX_RX_BUF_NUM) {
        if (net_ratelimit())
            printk("%s: throttling: %d %d\n", __func__, device->rx_queue.qlen, rxbuf_num);
        rxbuf_num = MAX_RX_BUF_NUM - device->rx_queue.qlen;
    }
    spin_unlock_irqrestore(&device->rxq_lock, flag);    
    if (rxbuf_num < 0)
        rxbuf_num = 0;

    for (i = 0; i < rxbuf_num; i++) {
        int offset;

        new_buf = hif_usb_alloc_skb(rx_info[i].pkt_len, 32, 4);

        if (new_buf == NULL) {
            printk("%s: can't allocate rx buffer\n", __func__);
            rxbuf_num = i;
            break;
        }

        offset = rx_info[i].offset;
        memcpy(new_buf->data, &(buf->data[offset]), rx_info[i].pkt_len);

        /* Set skb buffer length */
        #ifdef NET_SKBUFF_DATA_USES_OFFSET
        new_buf->tail = 0;
        new_buf->len = 0;
        #else
        new_buf->tail = new_buf->data;
        new_buf->len = 0;
        #endif
        skb_put(new_buf, rx_info[i].pkt_len);
        rxbuf_pool[i] = new_buf;
    }

    atomic_add(rxbuf_num, &device->rxframe_cnt);
    spin_lock_irqsave(&device->rxq_lock, flag);

    for (i = 0; i < rxbuf_num; i++) {
        __skb_queue_tail(&device->rx_queue, rxbuf_pool[i]);
    }
    spin_unlock_irqrestore(&device->rxq_lock, flag);
//    if(device->rx_queue.qlen <= 1)  
    tasklet_schedule(&device->rx_tasklet);

    /* Submit a Rx urb */
    status = hif_usb_submit_bulk_urb(urb_ctx->rx_urb, device->udev, 
            USB_DATA_RX_PIPE, USB_DIR_IN, buf->data, MAX_RX_BUFFER_SIZE,
            hif_usb_bulk_rx_complete, urb_ctx,0);

    hif_usb_assert(!status);
    return;
#else
    urb_ctx->buf = NULL;
    skb_put(buf, urb->actual_length);
    spin_lock_irqsave(&device->rxq_lock, flag);
    __skb_queue_tail(&device->rx_queue, buf);
    spin_unlock_irqrestore(&device->rxq_lock, flag);    
    if(device->rx_queue.qlen <= 1)  
        tasklet_schedule(&device->rx_tasklet);
#endif
resubmit:
    /* Allocate a skb buffer */
    new_buf = hif_usb_alloc_skb(MAX_RX_BUFFER_SIZE, 32, 4);
    hif_usb_assert(new_buf);
    /* Enqueue skb buffer */
    urb_ctx->buf = new_buf;

    /* Submit a Rx urb */
    status = hif_usb_submit_bulk_urb(urb_ctx->rx_urb, device->udev, 
            USB_DATA_RX_PIPE, USB_DIR_IN, new_buf->data, MAX_RX_BUFFER_SIZE,
            hif_usb_bulk_rx_complete, urb_ctx,0);
    hif_usb_assert(!status);
}


#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,5,0)) || \
    (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,19))
void 
hif_usb_interrupt_rx_complete(urb_t *urb)
#else
void 
hif_usb_interrupt_rx_complete(urb_t *urb, struct pt_regs *regs)
#endif
{
    int status = 0;
    hif_usb_device_t *device = (hif_usb_device_t *)urb->context;
    unsigned long flag;

    hif_usb_print(("%s Entry \n",__FUNCTION__));
    /* Check status for URB */
    if (urb->status != 0) {
        if (urb->status == -ENOENT || 
            urb->status == -ECONNRESET ||
            urb->status == -ESHUTDOWN) {
            status = -1;
        }

        hif_usb_print(("%s: urb %s: %d\n", 
            __func__, status ? "released" : "resubmit", urb->status));

        dev_kfree_skb_any(device->event_buf);
        device->event_buf = NULL;
        if (status)
            return;

        if(device->protocol_fail_cnt >= ZM_MAX_USB_URB_FAIL_COUNT) {
            printk("%s[%d]protocol error cnt=%d,urb->status=%d!!!!\n\r",__func__,__LINE__,device->protocol_fail_cnt,urb->status);
            return;
        }
        device->protocol_fail_cnt++;

        goto resubmit;
    }

    device->protocol_fail_cnt = 0;

    if (urb->actual_length == 0) {
        printk(KERN_ERR "Get an URB whose length is zero");
        dev_kfree_skb_any(device->event_buf);
        goto resubmit;
    }

    skb_put(device->event_buf, urb->actual_length);

    spin_lock_irqsave(&device->rxq_lock, flag);
    __skb_queue_tail(&device->rx_queue, device->event_buf);
    spin_unlock_irqrestore(&device->rxq_lock, flag);
    if(device->rx_queue.qlen <= 1)  
        tasklet_schedule(&device->rx_tasklet);

resubmit:
    /* Issue another USB IN URB */
    device->event_buf = NULL;
    status = hif_usb_submit_interrupt_buffer(device);
    hif_usb_assert(!status);
}

#if (ATH_SUPPORT_AOW == 1)
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,5,0)) || \
    (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,19))
void 
hif_usb_isoc_tx_complete(urb_t *urb)
#else
void 
hif_usb_isoc_tx_complete(urb_t *urb, struct pt_regs *regs)
#endif
{
    int status = 0;

    hif_usb_print(("%s Entry \n",__FUNCTION__));
    /* Check status for URB */
	clear_bit(0, (void *)&aow_isoc_device->isoc_in_progress);
    if (urb->status != 0) {
        if (urb->status == -ENOENT || 
            urb->status == -ECONNRESET ||
            urb->status == -ESHUTDOWN) {
            status = -1;
        }

		printk("ISOC urb statis != 0, it is %d\n", urb->status);
        hif_usb_print(("%s: urb %s: %d\n", 
            __func__, status ? "released" : "resubmit", urb->status));

        /* goto resubmit; */
        return ;	
    }

/*resubmit:*/
    /* Issue another USB IN URB */
    status = hif_usb_submit_isoc_buffer();
    /* hif_usb_assert(!status); */
	if(status)
		printk("Resubmit ISOC error\n");
}
#endif


#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,5,0)) || \
    (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,19))
void 
hif_usb_bulk_xmit_complete(urb_t *urb)
#else
void 
hif_usb_bulk_xmit_complete(urb_t *urb, struct pt_regs *regs)
#endif
{
    usb_tx_urb_ctx_t *ctx = (usb_tx_urb_ctx_t *)urb->context;
    hif_usb_device_t *device = (hif_usb_device_t *)ctx->device;
    struct sk_buff *buf;

    buf = ctx->buf;
    ctx->buf = NULL;

    /* Give the urb back */
    hif_usb_assert(ctx->tx_urb == urb);
    hif_usb_put_tx_ctx(&device->tx_pipe[ctx->pipe_index], ctx);

    if (urb->status != 0) {
        hif_usb_print(("%s : status=0x%x\n", __FUNCTION__, urb->status));

        if (buf != NULL) {
            dev_kfree_skb_any(buf);
        }
        return;
    }
    if (buf != NULL) {
#ifdef HIF_STREAM_MODE
        if (ctx->pipe_index == USB_DATA_TX_PIPE) {
            /* Because the buf contains usb tx tag, we need to revert the header */
            skb_pull(buf, HIF_USB_STREAM_TAG_LEN);
        }
#endif
        app_ind_xmitted(device->callbacks[device->attach_app_idx], buf);
    }

#ifdef HIF_STREAM_MODE
    if (ctx->pipe_index == USB_DATA_TX_PIPE) {
        hif_usb_tx_pipe_t *pipe = &device->tx_pipe[ctx->pipe_index];

        /* check whether there is any pending buffer needed to be sent */
        if (hif_usb_check_txbuf_cnt(pipe, NULL, NULL) != 0) {
            hif_usb_xmit(pipe, USB_DATA_TX_PIPE);
        }
    }
#endif
}
#if (ATH_SUPPORT_AOW == 1)
int32_t 
hif_usb_submit_isoc_urb(urb_t *urb, struct usb_device *usb, 
                             uint16_t epnum, uint16_t direction, uint8_t* buf, 
                             int buffer_length, usb_complete_t complete, 
                             void *context, int interval)
{
    uint32_t ret;

    if(direction != USB_DIR_OUT) {
		printk("No IN ISO for AoW\n");
		BUG();
	}

	urb->dev = usb;
	urb->transfer_buffer = buf;
	urb->transfer_buffer_length = buffer_length;
	urb->pipe = usb_sndisocpipe(usb, epnum);
	urb->transfer_flags = URB_ISO_ASAP;
	urb->interval = 1 << (interval-1);
	urb->context = context;
	urb->complete = complete;
	urb->number_of_packets = 1;

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,14)
	/* urb->transfer_flags |= URB_ASYNC_UNLINK; */
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,5,0)
	ret = usb_submit_urb(urb, GFP_ATOMIC);
#else
	ret = usb_submit_urb(urb);
#endif

	return ret;
}
#endif


int32_t 
hif_usb_submit_interrupt_urb(urb_t *urb, struct usb_device *usb, 
                             uint16_t epnum, uint16_t direction, uint8_t* buf, 
                             int buffer_length, usb_complete_t complete, 
                             void *context, int interval)
{
    uint32_t ret;

    if(direction == USB_DIR_OUT) {
        void *transfer_buffer = (void*) buf;
        usb_fill_int_urb(urb, usb, usb_sndintpipe(usb, epnum),
                transfer_buffer, buffer_length, complete, context, interval);
    } else {
        void *transfer_buffer = (void*) buf;

        usb_fill_int_urb(urb, usb, usb_rcvintpipe(usb, epnum),
                transfer_buffer, buffer_length, complete, context, interval);
    }

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,14)
    urb->transfer_flags |= URB_ASYNC_UNLINK;
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,5,0)
    ret = usb_submit_urb(urb, GFP_ATOMIC);
#else
    ret = usb_submit_urb(urb);
#endif

    return ret;
}

int 
hif_usb_submit_interrupt_buffer(hif_usb_device_t *device)
{
    int ret = 0;

    device->event_buf = hif_usb_alloc_skb(MAX_EVENT_BUFFER_SIZE, 32, 4);
    hif_usb_assert(device->event_buf);

    ret = hif_usb_submit_interrupt_urb(device->event_urb, device->udev,
            USB_EVENT_PIPE, USB_DIR_IN, device->event_buf->data,
            MAX_EVENT_BUFFER_SIZE, hif_usb_interrupt_rx_complete, device, 1);

    if(ret){
        hif_usb_print(("Error in submitting URB for interrupt pipe status=%d \n",
                    ret));
        dev_kfree_skb_any(device->event_buf);
        device->event_buf = NULL;
    }
    return ret;
}

#if (ATH_SUPPORT_AOW == 1)
int 
hif_usb_submit_isoc_buffer()
{
    int ret = 0;
	/* static int idx=0; */
	static unsigned long startJ = 0, allCnt=0;
	static int emptySkbUsed = 0;

	if(!aow_isoc_device)
	{
		printk("AOW_ISOC_DEVICE IS NULL\n");
		return -1;
	}
	if(test_and_set_bit(0, (void *)&aow_isoc_device->isoc_in_progress)) {
		printk("isoc is already in progress\n");
		return -1;
	}

	if(startJ == 0) startJ = jiffies;
	allCnt += AUDSIZE_ALLCH_nMS;
	if(jiffies - startJ >= HZ)
	{
		printk("Tx speed=%lu\n", allCnt);
		allCnt = 0;
		startJ = jiffies;
	}

	if(aow_isoc_device->isoc_intval == 0)
	{
		printk("### INITAL ISOC TESTING DATA ###\n");
		aow_isoc_device->isoc_intval = USB_bInterval; /* 4=1ms, */
	}
	aow_isoc_device->isoc_urb->iso_frame_desc[0].length = AUDSIZE_ALLCH_nMS;
	aow_isoc_device->isoc_urb->iso_frame_desc[0].offset = 0;
	aow_isoc_device->isoc_data_len = AUDSIZE_ALLCH_nMS;

	if(audio_skb && (audio_skb != empty_skb) )
		dev_kfree_skb_any(audio_skb);
	audio_skb = skb_dequeue(&aow_isoc_device->isoc_data);
	if(!audio_skb)
	{
		audio_skb = empty_skb;
		while(!empty_skb) printk(KERN_ERR "Empt skb is NULL\n");
		emptySkbUsed++;
		if(printk_ratelimit())
			printk("Using empty skb\n");
	}
	else
	{
		emptySkbUsed = 0;
		if(audio_skb->len !=  AUDSIZE_ALLCH_nMS)
			printk(KERN_ERR "!!!Unexpected Async\n");
	}
	if(emptySkbUsed > 50) {
		printk("No Audio Data Isoc Stopped\n");
		clear_bit(0, (void *)&aow_isoc_device->isoc_in_progress);
		return 0;
	}

    ret = hif_usb_submit_isoc_urb(aow_isoc_device->isoc_urb, aow_isoc_device->udev,
            ISO_OUT_PIPE, USB_DIR_OUT, audio_skb->data,
            aow_isoc_device->isoc_data_len, hif_usb_isoc_tx_complete, 
			aow_isoc_device, aow_isoc_device->isoc_intval);



    if(ret){
        printk(KERN_ERR "Error in submitting URB for isopipe status=%d \n", ret);
		clear_bit(0, (void *)&aow_isoc_device->isoc_in_progress);
    }
    return ret;
}
int hif_usb_fill_isoc_data(char *buf, int size, int channels)
{
	static char localBuf[MAX_AUDSIZE];
	static int localBufIdx = 0;
	int idx = 0;
	static unsigned long startJ=0, allCnt=0;
	struct sk_buff *skb, *skb2 = NULL;
	int remainSpace, remainData, copyLen;

	if (!aow_isoc_device )
	{
		if(printk_ratelimit())
			printk("AOW ISO EPT is not configured\n");
		return -1;
	}
	if(channels < 1)
	{
		printk("Channel too small = %d\n", channels);
		return -1;
	}
	if(channels > 8)
	{
		printk("Channel too bug = %d\n", channels);
		return -1;
	}
	if(channels % 2 ) 
	{
		printk("Odd channels = %d\n", channels);
		return -1;
	}
	if(channels != num_channels)
	{
		printk("Channel from %d to %d\n", num_channels, channels);
		num_channels = channels;
		while( (skb = skb_dequeue(&aow_isoc_device->isoc_data))) 
			dev_kfree_skb_any(skb);
		localBufIdx = 0;
			
	}
	if(startJ == 0) startJ = jiffies;
	if(jiffies - startJ >= HZ)
	{
		printk("Rx speed=%lu\n", allCnt);
		allCnt = 0;
		startJ = jiffies;
	}
	if(size >  MAX_ISOC_BUFFER_SIZE)
	{
		printk("INCOMING BUFFER IS TOO BIG\n");
		return -1;
	}
	AOW_ISOC_LOCK();
	for(idx=0;idx<size;idx++)
	{
		remainSpace = AUDSIZE_ALLCH_nMS - localBufIdx;
		remainData = size - idx;
		if(remainSpace >= remainData)
			copyLen = remainData;
		else
			copyLen = remainSpace;

		memcpy(&localBuf[localBufIdx], &buf[idx], copyLen);
		localBufIdx += copyLen;
		allCnt += copyLen;
		idx += copyLen - 1;
		if(localBufIdx == AUDSIZE_ALLCH_nMS)
		{
			if(skb_queue_len(&aow_isoc_device->isoc_data) > QUEUE_DEPETH_100MS) 
			{
				skb2 = skb_dequeue(&aow_isoc_device->isoc_data);
				if(skb2 == NULL)
					printk(KERN_ERR "SKB2 is NULL\n");
				if(printk_ratelimit())
					printk("Queue full 1\n");
			}
			if(skb2 != NULL)
				skb = skb2;
			else
				skb = dev_alloc_skb(AUDSIZE_ALLCH_nMS);
			if(skb) {
				memcpy(skb->data, localBuf, AUDSIZE_ALLCH_nMS);
				skb->len = AUDSIZE_ALLCH_nMS;
				skb_queue_tail(&aow_isoc_device->isoc_data, skb);
				if(!test_bit(0, (void *)&aow_isoc_device->isoc_in_progress)) 
					hif_usb_submit_isoc_buffer();
			}
			else
				printk("skb allocation failed\n");
			localBufIdx = 0;
		}
		if(localBufIdx > AUDSIZE_ALLCH_nMS)
		{
			printk("BUG! Incorrect idx. Reset to 0\n");
			localBufIdx = 0;
		}
	}
	AOW_ISOC_UNLOCK();
	return 0;
}
#endif

int32_t 
hif_usb_submit_bulk_urb(urb_t *urb, struct usb_device *usb, uint16_t epnum, 
                        uint16_t direction, uint8_t *transfer_buffer, 
                        int buffer_length, usb_complete_t complete, 
                        void *context, int interval)
{
    int32_t ret;

    if(direction == USB_DIR_OUT) {
        usb_fill_bulk_urb(urb, usb, usb_sndbulkpipe(usb, epnum),
                transfer_buffer, buffer_length, complete, context);

        urb->transfer_flags |= URB_ZERO_PACKET;
    }
    else 
        usb_fill_bulk_urb(urb, usb, usb_rcvbulkpipe(usb, epnum),
                transfer_buffer, buffer_length, complete, context);

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,14)
    urb->transfer_flags |= URB_ASYNC_UNLINK;
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,5,0)
    ret = usb_submit_urb(urb, GFP_ATOMIC);
#else
    ret = usb_submit_urb(urb);
#endif
    return ret;
}

static inline void 
hif_usb_free_tx_pipes(hif_usb_device_t *hif_dev, int unlink)
{
    hif_usb_tx_pipe_t *pipe;
    usb_tx_urb_ctx_t *urb_ctx;
    int i;
    /* Unlink & Free all Tx Urbs */
    for(i=0; i<MAX_TX_PIPES; i++){
        pipe = &hif_dev->tx_pipe[i];
        urb_ctx = pipe->alloc_head;
        while(urb_ctx != NULL){
            if (urb_ctx->tx_urb != NULL) {
                if(unlink){
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,14)
                    urb_ctx->tx_urb->transfer_flags &= 
                        ~URB_ASYNC_UNLINK;
#endif
                    usb_kill_urb(urb_ctx->tx_urb);
                }
                usb_free_urb(urb_ctx->tx_urb);

#ifdef HIF_STREAM_MODE
                if (urb_ctx->usb_txbuf != NULL)
                    kfree(urb_ctx->usb_txbuf);
#endif
            }
            urb_ctx = urb_ctx->alloc_next;
        }
#ifdef HIF_STREAM_MODE
        if (pipe->txbuf_q != NULL)
            kfree(pipe->txbuf_q);
#endif
    }

}

static inline int 
hif_usb_init_tx_pipes(hif_usb_device_t  *hif_dev)
{
    hif_usb_tx_pipe_t *pipe;
    int result = 0, i, j;
    int urb_cnt;
    usb_tx_urb_ctx_t   *urb_ctx = NULL;
    urb_t   *urb;

    hif_usb_print(("%s Entry \n",__FUNCTION__));

    hif_dev->protocol_fail_cnt = 0;    

    /*Initialize All Tx Pipes for Data */
    for(i=0; i<MAX_TX_PIPES; i++){
        pipe = &hif_dev->tx_pipe[i];
        pipe->submit = hif_usb_submit_bulk_urb;
        pipe->tx_complete_cb = hif_usb_bulk_xmit_complete;
        pipe->urb_cnt = 0;
#ifdef HIF_STREAM_MODE
        pipe->txbuf_q = NULL;
        pipe->txbuf_head = 0;
        pipe->txbuf_tail = 0;
        pipe->txbuf_cnt = 0;
        pipe->total_size = 0;
        spin_lock_init(&pipe->xmit_lock);
#endif
        urb_cnt = MAX_TX_URB_NUM;
        switch(i){
            case CMD_PIPE_INDEX:
                pipe->pipe_num = USB_CMD_PIPE;
                urb_cnt = MAX_CMD_URB_NUM;
                pipe->submit = hif_usb_submit_interrupt_urb;
                pipe->tx_complete_cb = hif_usb_interrupt_xmit_complete;
                break;
            case LP_TX_PIPE_INDEX:
                pipe->pipe_num = USB_DATA_TX_PIPE;
#ifdef HIF_STREAM_MODE
                pipe->txbuf_q = kmalloc(sizeof(usb_tx_buf_t) * MAX_TX_BUF_NUM, GFP_KERNEL);
                if (pipe->txbuf_q == NULL) {
                    hif_usb_print(("%s: can't allocate txbuf_q\n", __func__));
                    return HIF_STATUS_ERROR;
                }
#endif
                break;
#ifndef DISABLE_USB_MPHP
            case MP_TX_PIPE_INDEX:
                pipe->pipe_num = USB_DATA_TX_PIPE;
                break;
            case HP_TX_PIPE_INDEX:
                pipe->pipe_num = USB_DATA_TX_PIPE;
                break;
#endif
            default:
                pipe->urb_cnt = 0;
                hif_usb_assert(0);
                break;
        }
        spin_lock_init(&pipe->pipe_lock);
        pipe->head = NULL;
        pipe->tail = NULL;
        pipe->alloc_head = NULL;
        for (j=0; j <urb_cnt ; j++){
            urb_ctx = kzalloc(sizeof(usb_tx_urb_ctx_t), GFP_KERNEL);
            urb = USB_ALLOC_URB(0,GFP_KERNEL);
            if((urb_ctx == 0) || (urb == 0)){
                if(urb_ctx){
                    hif_usb_print(("%s urb alloc failed \n",__FUNCTION__));
                    kfree(urb_ctx);
                }
                if(urb){
                    hif_usb_print(("%s urb urbctx alloc failed \n",
                                __FUNCTION__));
                    usb_free_urb(urb);
                }

                /*Free all URBs & ctx  for this  & previous pipe */
                hif_usb_free_tx_pipes(hif_dev, 0);
                goto done;
            }
#ifdef HIF_STREAM_MODE
            if (pipe->pipe_num == LP_TX_PIPE_INDEX) {
                urb_ctx->usb_txbuf = kmalloc(MAX_USB_TX_BUF_SIZE, GFP_ATOMIC);

                if (urb_ctx->usb_txbuf == NULL) {
                    /* Free all URBs & ctx  for this  & previous pipe */
                    hif_usb_free_tx_pipes(hif_dev, 0);
                    goto done;
                }
            }
#endif
            urb_ctx->tx_urb = urb;
            urb_ctx->buf = NULL;
            urb_ctx->device = hif_dev;
            urb_ctx->pipe_index = i;
            if(pipe->tail != NULL)
                pipe->tail->alloc_next = urb_ctx;
            else
                pipe->alloc_head = urb_ctx;
            hif_usb_print(("%s TX PIPE %p hifidx %d put ctx %p \n",
                        __FUNCTION__, pipe, pipe->pipe_num, urb_ctx));
            hif_usb_put_tx_ctx(pipe, urb_ctx);
        }

        hif_usb_print(("%s memory for pipe %d created \n",__FUNCTION__, i));
    }
done:
    return result;

}

static inline void
hif_usb_free_rx_ctx(hif_usb_device_t *hif_dev, int unlink)
{
    int i;
#if (ATH_SUPPORT_AOW == 1)
	struct sk_buff *skb;
#endif

    /* Unlink & Free all Rx URBs */
    for (i = 0; i < MAX_RX_URB_NUM; i++) {
        if (hif_dev->rx_urb_ctx[i].rx_urb != NULL) {
            if(unlink){
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,14)
                hif_dev->rx_urb_ctx[i].rx_urb->transfer_flags &= 
                    ~URB_ASYNC_UNLINK;
#endif
                usb_kill_urb(hif_dev->rx_urb_ctx[i].rx_urb);
            }
            usb_free_urb(hif_dev->rx_urb_ctx[i].rx_urb);
        }
        if (hif_dev->rx_urb_ctx[i].buf) {
            dev_kfree_skb_any(hif_dev->rx_urb_ctx[i].buf);
            hif_dev->rx_urb_ctx[i].buf = NULL;
        }
    }
    if(hif_dev->event_urb != NULL){
        if(unlink){
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,14)
            hif_dev->event_urb->transfer_flags &= ~URB_ASYNC_UNLINK;
#endif
            usb_kill_urb(hif_dev->event_urb);
        }
        usb_free_urb(hif_dev->event_urb);
    }
    if (hif_dev->event_buf) {
        dev_kfree_skb_any(hif_dev->event_buf);
        hif_dev->event_buf = NULL;
    }
#if (ATH_SUPPORT_AOW == 1)
    if(hif_dev->isoc_urb != NULL){
        if(unlink){
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,14)
            hif_dev->isoc_urb->transfer_flags &= ~URB_ASYNC_UNLINK;
#endif
            usb_kill_urb(hif_dev->isoc_urb);
        }
        usb_free_urb(hif_dev->isoc_urb);
		hif_dev->isoc_urb = NULL;
		if(audio_skb  && (audio_skb != empty_skb))
		{
			dev_kfree_skb_any(audio_skb);
			audio_skb = NULL;
		}
		if(empty_skb)
		{
			dev_kfree_skb_any(empty_skb);
			empty_skb = NULL;
		}
		while((skb = skb_dequeue(&aow_isoc_device->isoc_data)))
			dev_kfree_skb_any(skb);
		aow_isoc_device = NULL;
    }
#endif


}

static inline int
hif_usb_init_rx_ctx(hif_usb_device_t *hif_dev)
{
    int result=0,i;
    hif_usb_print(("%s Entry \n",__FUNCTION__));
    /* Allocate all data Rx URBs */
    for (i = 0; i < MAX_RX_URB_NUM; i++){
        hif_dev->rx_urb_ctx[i].rx_urb = USB_ALLOC_URB(0, GFP_KERNEL);
        if (hif_dev->rx_urb_ctx[i].rx_urb == 0){
            hif_usb_free_rx_ctx(hif_dev, 0);
            result = -1;
            goto done;
        }
        hif_usb_print(("%s Allocated %p URB for RX DATA \n",
                    __FUNCTION__, hif_dev->rx_urb_ctx[i].rx_urb));
    }

    hif_dev->event_urb = USB_ALLOC_URB(0, GFP_KERNEL);	
    if (hif_dev->event_urb == 0){
        hif_usb_print(("%s Allocating URB for Regin Failed\n",__FUNCTION__));
        hif_usb_free_rx_ctx(hif_dev, 0);
        result = -1; 
        goto done;
    }
    hif_usb_print(("%s Allocated %p URB for RX EVENT \n",__FUNCTION__, 
                hif_dev->event_urb));
#if (ATH_SUPPORT_AOW == 1)
    hif_dev->isoc_urb = USB_ALLOC_URB(1, GFP_KERNEL);	
    if (hif_dev->isoc_urb == 0){
        hif_usb_print(("%s Allocating URB for ISO OUT\n",__FUNCTION__));
        hif_usb_free_rx_ctx(hif_dev, 0);
        result = -1; 
        goto done;
    }
	else
	{
		aow_isoc_device = hif_dev;
		if(!hif_dev->isoc_data_init)
		{
			skb_queue_head_init(&hif_dev->isoc_data);
			hif_dev->isoc_data_init = 1;
		}
		empty_skb = dev_alloc_skb(MAX_AUDSIZE);
		if(!empty_skb) while(1) printk("OOM x\n");
		memset(empty_skb->data, 0 , MAX_AUDSIZE);
	}
    
	memset(&hif_dev->isoc_urb->iso_frame_desc[0], 0, sizeof(struct usb_iso_packet_descriptor));

	hif_usb_print(("%s Allocated %p URB for ISO OUT \n",__FUNCTION__, 
				hif_dev->isoc_urb));
#endif


done:
    return result;
}

int 
notifyDeviceInsertedHandler(hif_handle_t handle)
{
    hif_usb_device_t *hif_dev = (hif_usb_device_t*)handle;

    hif_usb_print(("%s calling inserted event handler \n",__FUNCTION__));

    /* detach from parent process to avoid zombie */
    daemonize("hif_usb_insert_thread", 0);
    app_dev_inserted(drv_reg_callbacks,hif_dev);
    return 0;
}


void 
hif_usb_detach(struct usb_interface *interface, uint8_t surprise_removed)
{

    hif_usb_device_t *hif_dev = (hif_usb_device_t*)usb_get_intfdata(interface);
    hif_usb_print(("%s Entry\n",__FUNCTION__));

#ifdef ROM_MODE
    if(first_time)
    {
        printk("BootRom Device Disconnect \n");
        goto free_rez;
    }
#endif
    hif_dev->surprise_removed = surprise_removed;

    if (drv_reg_callbacks.device_disconnect) {
        app_dev_removed(drv_reg_callbacks, hif_dev->app_context);
    }

free_rez:
    /* Unlink & Free all Tx Urbs */
    hif_usb_free_tx_pipes(hif_dev, 1);	
    hif_usb_free_rx_ctx(hif_dev, 1);
    tasklet_kill(&hif_dev->rx_tasklet);

#ifdef HIF_STREAM_MODE
    hif_usb_stop_timer(hif_dev);
#endif

#ifdef ROM_MODE		
    if(!first_time)
    {
        printk("%s Freeing hif dev \n", __FUNCTION__);
        kfree(hif_dev);
        /* reset variables to allow another download */
        if (surprise_removed) {
            g_hif_dev = NULL;
            first_time = 1;
        }
    }
    else
    {
        first_time = 0;
    }
#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,5,0))
    usb_set_intfdata(interface, NULL);
#endif
}

uint32_t
usb_get_devid(hif_handle_t handle)
{
    unsigned int  pid;
    hif_usb_device_t *device = (hif_usb_device_t *)handle;
    struct usb_device *dev = interface_to_usbdev(device->interface);

    pid = le16_to_cpu(dev->descriptor.idProduct);

    return pid;
}

static int 
hif_usb_probe(struct usb_interface *interface,
              const struct usb_device_id *id)
{
    struct usb_device *dev = interface_to_usbdev(interface);
    unsigned short vendor_id, product_id;
    int result = 0;
    unsigned int  pid ;
    hif_usb_device_t *hif_dev = NULL;
    int is_romdevice = 0;

    hif_usb_print(("%s Entry \n",__FUNCTION__));

    if (usb_registered == 0) {
        printk("%s: not registered: ignore probing\n", __func__);
        return -1;
    }
    usb_shutdown = 0;

    usb_get_dev(dev);

    vendor_id = dev->descriptor.idVendor;
    product_id = dev->descriptor.idProduct;
    pid  =  (((product_id & 0xff) << 8)  | ((product_id & 0xff00) >> 8) ) 
        & 0x0000ffff  ;
    if(HIF_USB_DBG){
        printk(KERN_NOTICE "vendor_id = %04x\n", vendor_id);
        printk(KERN_NOTICE "product_id = %04x\n", product_id);  
        printk(" pid %x *************\n",pid);
        if (dev->speed == USB_SPEED_HIGH)
            printk(KERN_NOTICE "USB 2.0 Host\n");
        else
            printk(KERN_NOTICE "USB 1.1 Host\n");  
        _dbg_print_endpoints(interface);
    }

#ifdef ROM_MODE
    is_romdevice = usb_target_mode(dev) ? 1 : 0;
    printk("target running in %s mode\n", is_romdevice ? "rom" : "kernel");

    if(!g_hif_dev)
    {
        printk("%s : allocating memory for hif device \n", __FUNCTION__);
#endif
        /* allocate memory for HIF_DEVICE */
        hif_dev = (hif_usb_device_t*)kzalloc(sizeof(hif_usb_device_t), 
                                            GFP_KERNEL);
        if (!hif_dev){
            hif_usb_print(("%s Allocation Failure \n",__FUNCTION__));
            result = -1;
            goto fail;
        }
#ifdef ROM_MODE
    }
    if (is_romdevice) {
        if (first_time) {
            /* the normal path: rom device is inserted firstly */
            g_hif_dev = hif_dev;
        } else {
            /* previous downloading might fail so recover it */
            if (g_hif_dev) {
                /* hif_dev must be NULL */
                if (drv_reg_callbacks.device_disconnect) {
                    app_dev_removed(drv_reg_callbacks, g_hif_dev->app_context);
                }
                hif_dev = g_hif_dev;
            } else {
                /* g_hif_dev must be NULL */
                g_hif_dev = hif_dev;
            }
            first_time = 1;
        }
    } else {
        if (first_time) {
            /* previous target is not cleaned up so reset target */
            printk("disconnect and re-initializing target\n");
            usb_target_reset(dev);
            result = -1;
            goto fail1;
        } else {
            /* the normal path: kernel device is inserted later */
            if (g_hif_dev) {
                hif_dev = g_hif_dev;
            } else {
                printk("unknown initialization sequence\n");
                usb_target_reset(dev);
                result = -1;
                goto fail1;
            }
        }
    }
#endif

    if (hif_dev == NULL) {
        printk("invalid hif handle\n");
        result = -1;
        goto fail;
    }

    hif_dev->udev = dev;
    hif_dev->interface = interface;

    /*initialize tx pipes */
    hif_usb_init_tx_pipes(hif_dev);

    /*initialize rx context for receiving data */
    if(hif_usb_init_rx_ctx(hif_dev)){
        hif_usb_print(("%s init_rx_ctx failed \n",__FUNCTION__));
        hif_usb_free_tx_pipes(hif_dev, 0);
        goto fail1;
    }

    tasklet_init(&hif_dev->rx_tasklet, hif_usb_process_rxq, 
                 (unsigned long)hif_dev);
    skb_queue_head_init(&hif_dev->rx_queue);
    spin_lock_init(&hif_dev->rxq_lock);

#ifdef HIF_STREAM_MODE
    /* Initialize a timer for tx frames calculation */
    hif_usb_start_timer(hif_dev);
#endif

    usb_set_intfdata(interface, hif_dev);

#ifdef ROM_MODE
    if(first_time){
        printk("Probe for BootRom Enumeration \n");

#endif
        if (drv_reg_callbacks.device_detect) {
            hif_usb_print(("%s Notifying device insertion event in thread\n",
                        __FUNCTION__));
            kernel_thread(notifyDeviceInsertedHandler, hif_dev, 
                    CLONE_FS|CLONE_FILES);
        }
#ifdef ROM_MODE
    }
    else
    {
        printk("Probe for Enumeration in Kernel \n");
        hif_start(hif_dev);
    }
#endif

    g_hif_usb_ifc = interface;  
    goto done;

fail1:
    kfree(hif_dev);
fail:
    usb_put_dev(dev);

done:    
    return result;
}


static void 
hif_usb_disconnect(struct usb_interface *interface)
{
    hif_usb_print(("%s\n", __FUNCTION__));

    if ( g_hif_usb_ifc != NULL ) {
        g_hif_usb_ifc = NULL;
        usb_shutdown = 0;
        hif_usb_detach(interface, 1);
    }   
    usb_put_dev(interface_to_usbdev(interface));
}

static int 
hif_usb_suspend(struct usb_interface *interface, pm_message_t message)
{
    printk("%s Entry\n",__FUNCTION__);
    return 0;
}

static int 
hif_usb_resume(struct usb_interface *interface)
{
    printk("%s Entry\n",__FUNCTION__);
    return 0;
}

#ifdef HIF_STREAM_MODE
void hif_usb_timer(unsigned long param)
{
    hif_usb_device_t *hif_dev = (hif_usb_device_t *) param;
    hif_usb_tx_pipe_t *pipe = &hif_dev->tx_pipe[pipe_index[USB_DATA_TX_PIPE]];    
    uint16_t txbuf_cnt;
    int hif_txq_size;
    struct timeval tv;
    static int iter = 0;

    iter++;

    if (iter >= HIF_ONE_SEC_ITER) {
    atomic_set(&(pipe->txframe_persec), atomic_read(&(pipe->txframe_cnt)));
    atomic_set(&(pipe->txframe_cnt), 0);
    atomic_set(&(hif_dev->rxframe_persec), atomic_read(&(hif_dev->rxframe_cnt)));
    atomic_set(&(hif_dev->rxframe_cnt), 0);
        iter = 0;
    }

    txbuf_cnt = hif_usb_check_txbuf_cnt(pipe, &tv, &hif_txq_size);

    if (txbuf_cnt != 0) {
        struct timeval now;
        struct timeval result;

        do_gettimeofday(&now);

        timersub(now, tv, result);

        if (result.tv_usec >= 2000) {
            hif_usb_xmit(pipe, USB_DATA_TX_PIPE);
        }

    }
    mod_timer(&(hif_dev->one_sec_timer), jiffies + HIF_TIMER_JIFFIES_CNT);

}

static void 
hif_usb_start_timer(hif_usb_device_t *hif_dev)
{
    init_timer(&hif_dev->one_sec_timer);
    hif_dev->one_sec_timer.function = hif_usb_timer;
    hif_dev->one_sec_timer.data = (unsigned long)hif_dev;

    mod_timer(&(hif_dev->one_sec_timer), jiffies + HZ);
}

static void
hif_usb_stop_timer(hif_usb_device_t *hif_dev)
{
    del_timer(&(hif_dev->one_sec_timer));
}

static int
hif_usb_put_txbuf(hif_usb_tx_pipe_t *pipe, struct sk_buff *buf)
{
    unsigned long flags;

    spin_lock_irqsave(&(pipe->pipe_lock), flags);

    if (pipe->txbuf_cnt < MAX_TX_BUF_NUM)
    {
        usb_tx_buf_t *txbuf = &(pipe->txbuf_q[pipe->txbuf_tail]);

        txbuf->buf = buf;
        do_gettimeofday(&txbuf->timeval);

        pipe->txbuf_tail = ((pipe->txbuf_tail+1) & (MAX_TX_BUF_NUM - 1));
        pipe->txbuf_cnt++;
        pipe->total_size += buf->len;
    }
    else
    {
        if (net_ratelimit())
            printk("%s: throttling\n", __func__);
        spin_unlock_irqrestore(&(pipe->pipe_lock), flags);
        return HIF_STATUS_ERROR;
    }

    spin_unlock_irqrestore(&(pipe->pipe_lock), flags);

    return 0;
}

static usb_tx_buf_t*
hif_usb_get_txbuf(hif_usb_tx_pipe_t *pipe)
{
    unsigned long flags;
    usb_tx_buf_t *txbuf;

    spin_lock_irqsave(&(pipe->pipe_lock), flags);

    if (pipe->txbuf_cnt > 0)
    {
        txbuf = &(pipe->txbuf_q[pipe->txbuf_head]);
        pipe->txbuf_head = ((pipe->txbuf_head+1) & (MAX_TX_BUF_NUM - 1));
        pipe->txbuf_cnt--;
        pipe->total_size -= txbuf->buf->len;
    }
    else
    {
        if (pipe->txbuf_head != pipe->txbuf_tail)
        {
            printk(KERN_ERR "%s txbuf_q inconsistent: txbuf_head: %d, txbuf_tail: %d\n",
                __func__, pipe->txbuf_head, pipe->txbuf_tail);
        }

        spin_unlock_irqrestore(&(pipe->pipe_lock), flags);
        return NULL;
    }

    spin_unlock_irqrestore(&(pipe->pipe_lock), flags);

    return txbuf;
}

static uint16_t
hif_usb_check_txbuf_cnt(hif_usb_tx_pipe_t *pipe, struct timeval *timestamp, int *hif_txq_size)
{
    int pkt_num = 0;
    int total_size = 0;
    int buf_cnt;
    int index;
    unsigned long flags;
    usb_tx_buf_t *txbuf;

    spin_lock_irqsave(&(pipe->pipe_lock), flags);

    index = pipe->txbuf_head;
    buf_cnt = pipe->txbuf_cnt;

    if (buf_cnt && timestamp) {
        usb_tx_buf_t *txbuf = &(pipe->txbuf_q[pipe->txbuf_head]);
        *timestamp = txbuf->timeval;
    }

    if (hif_txq_size != NULL)
        *hif_txq_size = pipe->total_size;

    while (buf_cnt > 0)
    {
        txbuf = &(pipe->txbuf_q[index]);
        total_size += (txbuf->buf->len + (HIF_USB_STREAM_TAG_LEN << 1));

        if (total_size >= MAX_USB_TX_BUF_SIZE)
            break;

        pkt_num++;
        buf_cnt--;
        index = ((index+1) & (MAX_TX_BUF_NUM - 1));
    }

    spin_unlock_irqrestore(&(pipe->pipe_lock), flags);

    if (pkt_num > MAX_TX_AGGREGATE_NUM_ITEM)
        pkt_num = MAX_TX_AGGREGATE_NUM_ITEM;

    return pkt_num;
}

static int
hif_usb_rx_destream(struct sk_buff *skb, int length, usb_rx_info_t *rx_info)
{
    int index = 0;
    int rx_index = 0;
    uint8_t *data = skb->data;

    /* Peek the USB transfers */
    while (index < length) {
        uint16_t pkt_len;
        uint16_t pkt_tag;

        pkt_len = data[index] + (data[index+1] << 8);
        pkt_tag = data[index+2] + (data[index+3] << 8);

        if (pkt_len > HIF_USB_MAX_PACKET_LEN) {
            printk("%s get error packet length = %d\n", __FUNCTION__, pkt_len);
            break;
        }

        if (pkt_tag == HIF_USB_STREAM_TAG) {
            int pad_len;

            rx_info[rx_index].pkt_len = pkt_len;
            rx_info[rx_index].offset = index + HIF_USB_STREAM_TAG_LEN;
            rx_index++;

            pad_len = HIF_USB_STREAM_TAG_LEN - (pkt_len & (HIF_USB_STREAM_TAG_LEN - 1));

            if (pad_len == HIF_USB_STREAM_TAG_LEN)
                pad_len = 0;

            index = index + HIF_USB_STREAM_TAG_LEN + pkt_len + pad_len;

            if (index > MAX_RX_BUFFER_SIZE)
            {
                printk(KERN_ERR "%s: transfer in the next transaction\n", __func__);
                printk(KERN_ERR "%s: pkt_tag: %04x, pad_len: %04x, transfer len: %d\n",
                    __func__, pkt_tag, pad_len, length);

                rx_index--;
                break;
            }
        }
        else {
            printk(KERN_ERR "Can't find tag, pkt_len: 0x%04x, tag: 0x%04x rxBufPoolIndex: %d\n",
                pkt_len, pkt_tag, rx_index);

            /* find the next tag */
            // index += 4;
            if (rx_index >= 7)
                rx_index--;
            break;
        }
    }

    return rx_index;
}

static hif_status_t
__hif_usb_xmit(hif_usb_tx_pipe_t *pipe, uint32_t pipe_id)
{
    hif_status_t status = HIF_STATUS_OK;
    usb_tx_urb_ctx_t *ctx = NULL;
    struct timeval timestamp;
    int interval = 1;
    int i;
    int16_t txbuf_cnt;
    int hif_txq_size;
    static usb_tx_buf_t *txbuf_pool[MAX_TX_AGGREGATE_NUM_ITEM];
    hif_usb_device_t *device;

    ctx = hif_usb_get_tx_ctx(pipe, 0);
    if (ctx == NULL) {
        goto done;
    }

    device = (hif_usb_device_t *)ctx->device;
    txbuf_cnt = hif_usb_check_txbuf_cnt(pipe, &timestamp, &hif_txq_size);

    if (txbuf_cnt == 0) {
        /* Give the urb back */
        hif_usb_put_tx_ctx(pipe, ctx);
        goto done;
    }
    else if (((atomic_read(&(pipe->txframe_persec)) >= TX_PACKET_QUEUE_THR) ||
              (atomic_read(&(device->rxframe_persec)) >= RX_PACKET_QUEUE_THR)) &&
             (hif_txq_size < HIF_QUEUE_SIZE)) {
        struct timeval now;
        struct timeval result;

        do_gettimeofday(&now);

        if (txbuf_cnt == 1) {
            timersub(now, pipe->last_time_xmit, result);
            if (result.tv_usec >= 500) {
                goto xmit_start;
            }
        }

        timersub(now, timestamp, result);

        if (result.tv_usec <= HIF_USB_TX_TIMEOUT) {
            /* Give the urb back */
            hif_usb_put_tx_ctx(pipe, ctx);
            goto done;
        }
    }

xmit_start:

    if (txbuf_cnt == 1) {
        uint16_t *txbuf_hdr;
        usb_tx_buf_t *txbuf = hif_usb_get_txbuf(pipe);

        if (txbuf == NULL) {
            /* Give the urb back */
            hif_usb_put_tx_ctx(pipe, ctx);
            goto done;
        }

        /* Check the header room */
        if (skb_headroom(txbuf->buf) < HIF_USB_STREAM_TAG_LEN) {
            printk(KERN_INFO "%s: header room is not enough, headroom: %d\n",
                    __func__, skb_headroom(txbuf->buf));

            /* realloc the skb to make sure there is at least 4 bytes headroom */
            if (pskb_expand_head(txbuf->buf,
                          HIF_USB_STREAM_TAG_LEN-skb_headroom(txbuf->buf),
                          0, GFP_ATOMIC)) {
                printk(KERN_ERR "%s: skb_realloc_headroom fail\n", __func__);

                /* currently we drop the tranmission of this packet */
                dev_kfree_skb_any(txbuf->buf);
                txbuf->buf = NULL;

                /* Give the urb back */
                hif_usb_put_tx_ctx(pipe, ctx);
                goto done;
            }
        }

        skb_push(txbuf->buf, HIF_USB_STREAM_TAG_LEN);

        txbuf_hdr = (uint16_t *)txbuf->buf->data;
        *txbuf_hdr++ = cpu_to_le16(txbuf->buf->len - HIF_USB_STREAM_TAG_LEN);
        *txbuf_hdr++ = cpu_to_le16(HIF_USB_STREAM_TAG);

        ctx->buf = txbuf->buf;
        do_gettimeofday(&pipe->last_time_xmit);

        status = pipe->submit(ctx->tx_urb, device->udev, pipe_id,
                USB_DIR_OUT, txbuf->buf->data, txbuf->buf->len,
                pipe->tx_complete_cb, ctx, interval);
    }
    else {
        uint8_t *txbuf_ptr;
        uint16_t *txbuf_hdr;
        uint32_t len = 0;
        uint16_t offset = 0;

        for (i = 0; i < txbuf_cnt; i++) {
            usb_tx_buf_t *txbuf = hif_usb_get_txbuf(pipe);

            if (txbuf == NULL) {
                txbuf_cnt = i;
                break;
            }

            txbuf_ptr = ctx->usb_txbuf;
            txbuf_ptr += offset;

            txbuf_hdr = (uint16_t *)txbuf_ptr;

            /* add packet length and tag information */
            *txbuf_hdr++ = cpu_to_le16(txbuf->buf->len);
            *txbuf_hdr++ = cpu_to_le16(HIF_USB_STREAM_TAG);

            txbuf_ptr += HIF_USB_STREAM_TAG_LEN;
            len = txbuf->buf->len;

            memcpy(txbuf_ptr, txbuf->buf->data, len);

            // add the length of tag
            len += HIF_USB_STREAM_TAG_LEN;

            if (i < (txbuf_cnt-1)) {
                /* pad the buffer to firmware descriptor boundary */
                offset += (((len-1) >> 2) + 1) << 2;
            }
            
            if (i == (txbuf_cnt-1)) {
                len += offset;
            }
            
            txbuf_pool[i] = txbuf;
        }

        ctx->buf = NULL;
        do_gettimeofday(&pipe->last_time_xmit);

        if (txbuf_cnt != 0) {
            status = pipe->submit(ctx->tx_urb, device->udev, pipe_id,
                    USB_DIR_OUT, ctx->usb_txbuf, len,
                    pipe->tx_complete_cb, ctx, interval);

        }
        else {
            goto done;
        }

    }

    if (status) {
        /* Free tx buffer */
        hif_usb_print(("%s Failed to send Data for pipe %p ctx %p \n",
                    __FUNCTION__, pipe, ctx));

        if (txbuf_cnt != 1) {
            for (i = 0; i < txbuf_cnt; i++) {
                struct sk_buff *buf = txbuf_pool[i]->buf;
                app_ind_xmitted(device->callbacks[device->attach_app_idx], buf);
            }
        }
        else {
            /* Because the buf contains usb tx tag, we need to revert the header */
            skb_pull(ctx->buf, HIF_USB_STREAM_TAG_LEN);
            app_ind_xmitted(device->callbacks[device->attach_app_idx], ctx->buf);
        }

        ctx->buf = NULL;
        hif_usb_put_tx_ctx(pipe, ctx);

        /* we need to keep return value successful, or we might have double free problems */
        status = HIF_STATUS_OK;
        goto done;
    }

    atomic_add(txbuf_cnt, &pipe->txframe_cnt);

    if (txbuf_cnt != 1) {
        for (i = 0; i < txbuf_cnt; i++) {
            struct sk_buff *buf = txbuf_pool[i]->buf;
            app_ind_xmitted(device->callbacks[device->attach_app_idx], buf);
        }
    }

    /* check whether we need to resend */
    txbuf_cnt = hif_usb_check_txbuf_cnt(pipe, &timestamp, &hif_txq_size);

    if (hif_txq_size > MAX_USB_TX_BUF_SIZE) {
        ctx = hif_usb_get_tx_ctx(pipe, 1);
        if (ctx == NULL) {
            goto done;
        }

        goto xmit_start;
    }
done:
    return status;
}

static hif_status_t
hif_usb_xmit(hif_usb_tx_pipe_t *pipe, uint32_t pipe_id)
{
    unsigned long flags;
    hif_status_t status;

    spin_lock_irqsave(&(pipe->xmit_lock), flags);
    status = __hif_usb_xmit(pipe, pipe_id);
    spin_unlock_irqrestore(&(pipe->xmit_lock), flags);

    return status;
}
#endif

hif_status_t 
hif_send(hif_handle_t handle, uint32_t pipe_id, struct sk_buff *buf)
{
    hif_status_t status = HIF_STATUS_OK;
    hif_usb_device_t *device = (hif_usb_device_t *)handle;
    hif_usb_tx_pipe_t *pipe;
    int interval = 1;
    usb_tx_urb_ctx_t* ctx = NULL;

    if ( device->surprise_removed ) 
        return HIF_STATUS_ERROR;

#ifdef HIF_STREAM_MODE
    if (pipe_id == USB_DATA_TX_PIPE) {
        pipe = &device->tx_pipe[pipe_index[pipe_id]];

        /* queue buffer into txbuf_q */
        if (hif_usb_put_txbuf(pipe, buf) != 0) {
            hif_usb_print(("%s failed to put txbuf into usb pipe\n", __func__));
            return HIF_STATUS_ERROR;
        }

        status = hif_usb_xmit(pipe, pipe_id);
    }
    else {
#endif
        pipe = &device->tx_pipe[pipe_index[pipe_id]];
        
        ctx = hif_usb_get_tx_ctx(pipe, 0);
        if(ctx == NULL){
            status = HIF_STATUS_ERROR;
            goto done;
        }
        ctx->buf = buf;
        status = pipe->submit(ctx->tx_urb, device->udev, pipe_id,
                USB_DIR_OUT, buf->data, buf->len,
                pipe->tx_complete_cb, ctx, interval);
        
        if(status){
            hif_usb_print(("%s Failed to send Data for pipe %p ctx %p \n",
                        __FUNCTION__, pipe, ctx));
            ctx->buf = NULL;
            hif_usb_put_tx_ctx(pipe, ctx);
            goto done;
        }
#ifdef HIF_STREAM_MODE
    }
#endif

done:
    return ( status ? HIF_STATUS_ERROR : HIF_STATUS_OK);
}

#define USB_REQ_CUSTOM_TARGETRESET 0x55
hif_status_t 
hif_target_reset(hif_handle_t hif_handle)
{

    hif_usb_device_t* device = (hif_usb_device_t*)hif_handle;
    int32_t status;
    struct usb_device *dev = device->udev;
    unsigned int pipe = usb_sndctrlpipe(device->udev, 0);
    uint8_t request = USB_REQ_CUSTOM_TARGETRESET ;
    uint8_t requesttype = USB_DIR_OUT | 0x40 ;
    uint16_t value = 0xA;
    uint16_t index = 0xA;
    int *data;
    uint16_t size = sizeof(int );

    printk("%s Start \n ",__func__);
    data = (int *)kzalloc(sizeof(int), GFP_KERNEL);
    if(!data)
        return HIF_STATUS_ERROR;

    status = usb_control_msg(dev, pipe, request, requesttype, value, index, 
            data, size , 1);

    if (status < 0) {
        printk(" %s status=%d\n",__func__, status);
        status = HIF_STATUS_ERROR;
    } else {
        status = HIF_STATUS_OK;
    }
    kfree(data);
    printk("%s End Status %x \n ",__func__, status);
    return status;
}

hif_status_t 
usb_target_request(struct usb_device *udev, int request)
{
    int32_t status, *data;

    data = (int32_t*)kzalloc(sizeof(int32_t), GFP_KERNEL);
    if(!data)
        return HIF_STATUS_ERROR;

    status = usb_control_msg(udev, usb_rcvctrlpipe(udev, 0), 
            request, USB_DIR_IN | 0x40, 
            0, 0, data, sizeof(int32_t), 100);
    if (status < 0) {
        status = HIF_STATUS_ERROR;
    } else {
        status = HIF_STATUS_OK;
    }
    kfree(data);
    return status;
}

#define USB_REQ_CUSTOM_TARGETMODE 0x56
hif_status_t
usb_target_mode(struct usb_device *udev)
{
    return usb_target_request(udev, USB_REQ_CUSTOM_TARGETMODE);
}

hif_status_t
usb_target_reset(struct usb_device *udev)
{
    return usb_target_request(udev, USB_REQ_CUSTOM_TARGETRESET);
}

hif_status_t  
usb_download_image(hif_handle_t hif_handle, uint8_t cmd, uint32_t tgt_add, 
                      uint8_t* image, uint32_t image_len)
{
    hif_usb_device_t* device = (hif_usb_device_t*)hif_handle;
    int32_t result = 0;
    uint16_t value = (uint16_t)(tgt_add >> 16);
    uint16_t index = (uint16_t)(tgt_add);
    result = usb_control_msg(device->udev, usb_sndctrlpipe(device->udev, 0),
            cmd, USB_DIR_OUT | 0x40, value, index, image,
            image_len, 10000);
    if (result == -ENOENT ||
        result == -ECONNRESET || 
        result == -ESHUTDOWN ||
        result == -ENODEV) {
        usb_shutdown = 1;
    }
    return result;
}

void 
hif_register(hif_handle_t handle, hif_callback_t *callbacks)
{
    hif_usb_device_t *hif_dev = (hif_usb_device_t*) handle;

    hif_dev->app_handle = callbacks->context;
    hif_dev->callbacks[hif_dev->attach_app_idx] = *callbacks; 
}

void 
hif_deregister(hif_handle_t handle)
{
    hif_usb_device_t *hif_dev = (hif_usb_device_t*) handle;
    hif_dev->attach_app_idx++;
}

hif_status_t  
hif_init(hif_os_callback_t *callbacks)
{
    hif_usb_print(("%s Entry \n",__FUNCTION__));
    drv_reg_callbacks.device_detect = callbacks->device_detect;
    drv_reg_callbacks.device_disconnect = callbacks->device_disconnect;
    drv_reg_callbacks.device_suspend = callbacks->device_suspend;
    drv_reg_callbacks.device_resume = callbacks->device_resume;

    hif_usb_print(("%s register usb driver \n",__FUNCTION__));
    usb_registered++;
    usb_register(&hif_usb_driver);
    hif_usb_print(("%s Exit \n",__FUNCTION__));
    return HIF_STATUS_OK;
}

hif_status_t 
hif_start(hif_handle_t handle)
{
    hif_usb_device_t *hif_dev = (hif_usb_device_t *) handle;
    uint16_t i;
    int32_t status=HIF_STATUS_OK;
    struct sk_buff *buf;

    /*Allocate Data for Rx and associate with URB and submit URB*/
    for (i = 0; i < MAX_RX_URB_NUM; i++)
    {
        buf = hif_usb_alloc_skb(MAX_RX_BUFFER_SIZE, 32, 4);
        hif_usb_assert(buf);
        hif_dev->rx_urb_ctx[i].buf = buf;
        hif_dev->rx_urb_ctx[i].device = hif_dev;
        hif_dev->rx_urb_ctx[i].in_use = 1;
        hif_usb_print(("%s RX PIPE submitting %p URB \n",__FUNCTION__, 
                    hif_dev->rx_urb_ctx[i].rx_urb));
        status = hif_usb_submit_bulk_urb(hif_dev->rx_urb_ctx[i].rx_urb, 
                hif_dev->udev, USB_DATA_RX_PIPE, 
                USB_DIR_IN, buf->data, 
                MAX_RX_BUFFER_SIZE,
                hif_usb_bulk_rx_complete, 
                &hif_dev->rx_urb_ctx[i], 0);
        /* disconnect can happen anytime so don't do assert here */
        if (status && !first_time) {
            printk("%s: %d: failed to submit: %d\n", __func__, i, status);
        }
    }
    /*Submit interrupt buffer for receiving event data from target */
    hif_usb_submit_interrupt_buffer(hif_dev);
#if (ATH_SUPPORT_AOW == 1)
    if(isoc_endpoint_found == -1)
        printk("isoc_endpint detection mechanism is inactive\n");
    else if(isoc_endpoint_found == 1)
    {
        printk("ISO start\n");
        hif_usb_submit_isoc_buffer();
    }

#endif

    return status;
}

uint32_t 
hif_get_reserveheadroom(hif_handle_t hif_handle)
{
#ifdef HIF_STREAM_MODE
    return HIF_USB_STREAM_TAG_LEN;
#else
    return 0;
#endif
}

hif_status_t
hif_cleanup(hif_handle_t handle)
{
    hif_usb_device_t *hif_dev = (hif_usb_device_t *)handle;

    hif_usb_print(("%s Entry \n",__FUNCTION__));

    /* sanity check */
    if (hif_dev == NULL)
        return HIF_STATUS_ERROR;

    if (g_hif_usb_ifc == NULL) {
        /* device is already removed so bail out */
        return HIF_STATUS_OK;
    }

    /* when disconnected, avoid race between disconnect() */ 
    if (usb_shutdown) {
        return HIF_STATUS_OK;
    }

    /* deal with cleanup request from atd */
    usb_registered--;
    hif_target_reset(hif_dev);
    hif_usb_detach(hif_dev->interface,0);
    g_hif_usb_ifc = NULL;

    usb_deregister(&hif_usb_driver); 

    return HIF_STATUS_OK;
}

uint32_t 
hif_get_dlpipe(hif_pipetype_t pipe_type)
{
    uint32_t dl_pipe;
    switch(pipe_type)
    {
        case HIF_CONTROL_PIPE:
            dl_pipe = USB_EVENT_PIPE; 
            break;
        case HIF_COMMAND_PIPE:
            dl_pipe = USB_DATA_RX_PIPE; 
            break;
        case HIF_DATA_LP_PIPE:
            dl_pipe = USB_DATA_RX_PIPE; 
            break;
        case HIF_WSUPP_PIPE:
            dl_pipe = USB_DATA_RX_PIPE; 
            break;
        default:
            dl_pipe = 0;
            break;
    }
    return dl_pipe;
}

uint32_t 
hif_get_ulpipe(hif_pipetype_t pipe_type)
{
    uint32_t ul_pipe;
    switch(pipe_type)
    {
        case HIF_CONTROL_PIPE:
            ul_pipe = USB_CMD_PIPE; 
            break;
        case HIF_COMMAND_PIPE:
            ul_pipe = USB_DATA_TX_PIPE; 
            break;
        case HIF_DATA_LP_PIPE:
            ul_pipe = USB_DATA_TX_PIPE; 
            break;
        case HIF_WSUPP_PIPE:
            ul_pipe = USB_DATA_TX_PIPE; 
            break;
        default:
            ul_pipe = 0;
            break;
    }
    return ul_pipe;
}

void 
hif_claim_device(hif_handle_t  device, void *context)
{
    hif_usb_device_t *hif_dev = (hif_usb_device_t *)device;
    hif_dev->app_context = context;	
}

int 
init_usb_hif(void)
{
#if (ATH_SUPPORT_AOW == 1) && (ATH_SUPPORT_AOW_VSOUND == 1)
	void vsound_output_register(void *func);
	vsound_output_register((void *)hif_usb_fill_isoc_data);  
#endif
    printk("USBHIF Version 1.xx Loaded...\n");
    return 0;
}

void 
exit_usb_hif(void)
{
#if (ATH_SUPPORT_AOW == 1) && (ATH_SUPPORT_AOW_VSOUND == 1)
	void vsound_output_unregister(void);
	vsound_output_unregister();
#endif
    if (usb_registered)
        usb_deregister(&hif_usb_driver); 
    printk("USB HIF UnLoaded...\n");
}


EXPORT_SYMBOL(hif_init);
EXPORT_SYMBOL(hif_claim_device);
EXPORT_SYMBOL(hif_register);
EXPORT_SYMBOL(hif_deregister);
EXPORT_SYMBOL(hif_get_dlpipe);
EXPORT_SYMBOL(hif_get_ulpipe);
EXPORT_SYMBOL(hif_start);
EXPORT_SYMBOL(hif_get_reserveheadroom);
EXPORT_SYMBOL(hif_send);
EXPORT_SYMBOL(hif_cleanup);
EXPORT_SYMBOL(usb_get_devid);
EXPORT_SYMBOL(usb_download_image);
#if (ATH_SUPPORT_AOW == 1)
EXPORT_SYMBOL(hif_usb_submit_isoc_buffer);
EXPORT_SYMBOL(hif_usb_fill_isoc_data);
#endif

module_init(init_usb_hif);
module_exit(exit_usb_hif);

MODULE_LICENSE("Dual BSD/GPL");

