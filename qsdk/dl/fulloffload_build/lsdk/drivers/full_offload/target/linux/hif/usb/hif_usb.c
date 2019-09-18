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

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/delay.h>
#include <linux/ioport.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/smp_lock.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/timer.h>
#include <linux/list.h>
#include <linux/interrupt.h>
#include <linux/utsname.h>
#include <linux/device.h>
#include <linux/moduleparam.h>
#include <linux/workqueue.h>
#include <linux/vmalloc.h>
#include <linux/compiler.h>
#include <linux/netdevice.h>
#include <linux/skbuff.h>
#include <linux/dma-mapping.h>

#include <asm/byteorder.h>
#include <asm/io.h>
#include <asm/irq.h>
#include <asm/system.h>
#include <asm/unaligned.h>

#if LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,15)
#include <linux/usb_ch9.h>
#include <linux/usb_gadget.h>
#else
#include <linux/usb/ch9.h>
#include <linux/usb/gadget.h>
#endif
#ifndef BUILD_PARTNER_PLATFORM
#include <../arch/mips/include/asm/mach-ar7240/ar7240.h>
#endif
#include <linux/proc_fs.h>

#include "hif_usb.h"

#define SETUP_SIZE 512
#define app_dev_ready(_cb, _hif_hdl)  (_cb).device_detect((_hif_hdl))

#define app_dev_disconnect(_cb, _hif_hdl, ctx) \
              (_cb).device_disconnect(_hif_hdl, ctx)

#define app_dev_suspend(_cb, _hif_hdl)  (_cb).device_suspend((_hif_hdl))

#define app_dev_resume(_cb, _hif_hdl)  (_cb).device_resume((_hif_hdl))

#define  app_ind_xmitted(skb) \
              g_usb_ctx.hif_cb.send_buf_done(g_usb_ctx.hif_cb.context, skb)

#define app_ind_pkt(skb, pipe) \
             g_usb_ctx.hif_cb.recv_buf(g_usb_ctx.hif_cb.context, skb, pipe)


#define HIF_USB_PIPE_RX             1
#define HIF_USB_PIPE_COMMAND        4
#define HIF_USB_PIPE_TX             2
#if (ATH_SUPPORT_AOW == 1)
#define HIF_USB_PIPE_ISOC_RX			3
#endif

#ifdef ROM_ENUM_DISABLED
#define HIF_USB_PIPE_INTERRUPT      3
#else
#define HIF_USB_PIPE_INTERRUPT      4
#endif

#define CMD_PIPE_INDEX 0
#define LP_RX_PIPE_INDEX	1
#define TX_PIPE_INDEX 2
#define EVENT_PIPE_INDEX 3
#if (ATH_SUPPORT_AOW == 1)
#define ISOC_PIPE_INDEX 4
#endif


#define MAX_RX_PIPES 2

#define USB_SEND    0
#define USB_RECV    1

#ifdef HIF_STREAM_MODE
#if HIF_STREAM_SCHEDULE_METHOD == 1
uint16_t
hif_usb_check_txbuf_cnt(hif_usb_pipe_t *pipe);
void
hif_usb_xmit_txq(unsigned long context);
#else
uint16_t
hif_usb_check_txbuf_cnt(hif_usb_pipe_t *pipe, struct timeval *timestamp, int *hif_txq_size);
void
hif_usb_start_timer(hif_usb_context_t *hif_dev);
void
hif_usb_stop_timer(hif_usb_context_t *hif_dev);
hif_status_t
hif_usb_xmit(hif_usb_pipe_t *pipe, uint32_t pipe_id, int flush);
#endif
int
hif_usb_rx_destream(struct sk_buff *skb, int length, usb_rx_info_t *rx_info);
#endif

static const int index_to_pipe[MAX_PIPES] = {HIF_USB_PIPE_COMMAND,
    HIF_USB_PIPE_RX,
    HIF_USB_PIPE_TX,
    HIF_USB_PIPE_INTERRUPT
#if (ATH_SUPPORT_AOW == 1)
	,HIF_USB_PIPE_ISOC_RX
#endif

};
#ifdef HIF_USB_DEBUG
#define hif_usb_print(x) printk x
#else
#define hif_usb_print(x) 
#endif

hif_usb_context_t g_usb_ctx;
hif_os_callback_t drv_callbacks;

static struct sk_buff*
hif_usb_alloc_skb(uint32_t size, uint32_t reserve, uint32_t align)
{
    struct sk_buff *skb = NULL;
    size += align;

    skb = dev_alloc_skb(size + reserve + align);
    if(!skb)
        return NULL;

    if(align){
        reserve += (ALIGN((unsigned long)skb->data, align) - 
                (unsigned long)skb->data);
        skb_reserve(skb, reserve);

        hif_usb_assert(((unsigned long)skb->data % align) == 0);
    }

    return skb;
}

static inline int
hif_usb_queue_req(struct usb_ep *ep, struct usb_request *req, uint8_t* data, 
                  uint32_t length, int dma_direction)
{
    int status = HIF_STATUS_OK;
    req->buf = data;
    req->length = length;
    req->dma = dma_map_single(g_usb_ctx.gadget->dev.parent,
            req->buf, length, dma_direction);

    status = usb_ep_queue(ep, req, GFP_ATOMIC);
    if (status < 0) {
        hif_usb_print(("%s usb_ep_q failed unmapping buffer \n",__FUNCTION__));
        dma_unmap_single(g_usb_ctx.gadget->dev.parent, req->dma, length,
                         dma_direction);
        req->buf = NULL;
        req->length = 0;
    }
    return status;
}


static inline void 
hif_usb_put_usb_req(hif_usb_pipe_t *pipe, hif_usb_request_t *usb_req)
{
    hif_usb_request_t *tmp_ctx;
    unsigned long flag;  
    spin_lock_irqsave(&(pipe->list_lock), flag);
    usb_req->next = NULL;
    if(pipe->head == NULL)
        pipe->head = pipe->tail = usb_req;
    else{
        tmp_ctx = pipe->tail;
        pipe->tail = usb_req;
        tmp_ctx->next = pipe->tail;
    }
    pipe->req_count++;
    hif_usb_print(("%s pipe: %p hifIdx %d cnt: %d putReq: %p \n",__FUNCTION__,
                   pipe, pipe->number, pipe->req_count, usb_req));
    spin_unlock_irqrestore(&(pipe->list_lock), flag);
}

static inline hif_usb_request_t*
hif_usb_get_usb_req(hif_usb_pipe_t *pipe, int use_more)
{
    hif_usb_request_t *result = NULL;
    unsigned long flag;  
    spin_lock_irqsave(&(pipe->list_lock), flag);

#ifdef HIF_STREAM_MODE
    #if HIF_STREAM_SCHEDULE_METHOD != 1
    if (pipe->number == HIF_USB_PIPE_TX &&
        use_more == 0 && pipe->req_count <= (USB_MAX_TX_BUFFER - TX_URB_NUM_NORMAL)) {
        spin_unlock_irqrestore(&(pipe->list_lock), flag);
        return result;
    }
    #endif
#endif

    if(pipe->head != NULL){
        result = pipe->head;
        pipe->head = pipe->head->next;
        if(pipe->head == NULL)
            pipe->tail = NULL;
        pipe->req_count--;

        hif_usb_print(("%s pipe: %p hifIdx %d count: %d getReq: %p\n",
                       __FUNCTION__, pipe, pipe->number, pipe->req_count, 
                       result));
    }
    else
        hif_usb_print(("%s pipe:%p hifIdx %d count %d No Request\n",
                       __FUNCTION__, pipe, pipe->number, pipe->req_count));

    spin_unlock_irqrestore(&(pipe->list_lock), flag);

    return result;
}

static void 
hif_usb_tx_complete(struct usb_ep *ep, struct usb_request *req)
{
    int status = req->status;
    hif_usb_request_t *hif_req = req->context;
    struct sk_buff *skb = hif_req->skb;
#ifdef HIF_STREAM_MODE
    hif_usb_pipe_t *pipe = (hif_usb_pipe_t *)hif_req->parent;
#endif

    hif_usb_print(("%s tx status %d skb->len %d req actual %d req len %d \n",
                   __FUNCTION__, status, skb->len, req->actual, req->length));

    if (skb != NULL) {
        dma_unmap_single(g_usb_ctx.gadget->dev.parent, req->dma,
                skb->len, DMA_TO_DEVICE);
    }
    hif_req->skb = NULL;
    hif_usb_put_usb_req(hif_req->parent, hif_req);

    if(status != 0){
        printk("Status=%d Actual length =%d \n", status, req->actual);

        if (skb != NULL)
            dev_kfree_skb_any(skb);
    }
    else{
#ifdef LOOPBACK_SUPPORT
        dev_kfree_skb_any(skb);
#else
        if (skb != NULL) {
#ifdef HIF_STREAM_MODE
            if (pipe->number == HIF_USB_PIPE_TX) {
                /* Because the buf contains usb tx tag, we need to revert the header */
                skb_pull(skb, HIF_USB_STREAM_TAG_LEN);
            }
#endif

            app_ind_xmitted(skb);
        }
#endif
    }

#ifdef HIF_STREAM_MODE
    #if HIF_STREAM_SCHEDULE_METHOD != 1
    if (pipe->number == HIF_USB_PIPE_TX) {
        /* check whether there is any pending buffer needed to be sent */
        if (hif_usb_check_txbuf_cnt(pipe, NULL, NULL) != 0) {
            hif_usb_xmit(pipe, HIF_USB_PIPE_TX, 0);
        }
    }
    #endif /* #if HIF_STREAM_SCHEDULE_METHOD != 1 */
#endif
    hif_usb_print(("%s Exit \n",__FUNCTION__));
}


void
hif_usb_tx_tasklet(unsigned long context)
{
    hif_usb_context_t *device = (hif_usb_context_t*)context;
    struct sk_buff    *buf    = NULL;
    unsigned long      flag   = 0;

    hif_usb_print(("%s Entry \n",__FUNCTION__));

    do
    {
        spin_lock_irqsave(&device->txq_lock, flag);
        buf = __skb_dequeue(&device->tx_queue);
        spin_unlock_irqrestore(&device->txq_lock, flag);

        if(!buf)
            break;

        app_ind_pkt(buf, 0);

    }while(1);
}

#ifdef HIF_STREAM_MODE
int
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

            if (index > USB_MAX_RX_BUFF_SIZE)
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
            index += 4;
        }

    }
    return rx_index;
}
#endif
#if (ATH_SUPPORT_AOW == 1)
#define USB_bInterval 4				/* Variable */
#define ISOEP_INTVAL_MS ((125 * (2<<(USB_bInterval-2)))/1000) 	/* Constant */
#define QUEUE_DEPTH_100MS (aud_channels*100/ISOEP_INTVAL_MS)
#define QUEUE_DEPTH_50MS (aud_channels/2*100/ISOEP_INTVAL_MS)
#define AUDSIZE_STEREO_nMS (192*ISOEP_INTVAL_MS)
#define I2SBUFSIZE 192
struct sk_buff_head audio_data;
struct sk_buff_head audio_free;
static int aud_channels = 0;
static void 
hif_usb_isoc_complete(struct usb_ep *ep, struct usb_request *req)
{
    hif_usb_request_t *hif_req = req->context;
    hif_usb_pipe_t *pipe = hif_req->parent;
    struct sk_buff *skb = hif_req->skb, *skb2 = NULL;
	int status,i;

    dma_unmap_single(g_usb_ctx.gadget->dev.parent,req->dma,
            AUDBUF_SIZE, DMA_FROM_DEVICE);
	

	if((req->actual % AUDSIZE_STEREO_nMS)!=0)
			printk(KERN_ERR "Strnage size\n");
	
	if((req->actual *2 / AUDSIZE_STEREO_nMS) !=  aud_channels)
	{
		printk(KERN_ERR "Channel Number changes form %d to %d\n", aud_channels, 2*req->actual / AUDSIZE_STEREO_nMS );
		aud_channels = 2*req->actual / AUDSIZE_STEREO_nMS;
		while((skb2 = skb_dequeue(&audio_data)))
			dev_kfree_skb_any(skb2);
	}
	if(req->actual != 0)
	{
		for(i=0;i<req->actual /I2SBUFSIZE;i++)
		{
			skb2 = NULL;
			/* Depth than 200 is known problematic */
			if (skb_queue_len(&audio_data) > QUEUE_DEPTH_100MS) {
				skb2 = skb_dequeue(&audio_data);
				if (net_ratelimit())
					printk(KERN_ERR "Too long\n");
			}
			if(skb2 == NULL)
				skb2 = dev_alloc_skb(I2SBUFSIZE);
			if(!skb2) 
			{
				printk(KERN_ERR "OOM1\n");
				goto func_exit;
			}
			memcpy(skb2->data, skb->data+i*I2SBUFSIZE, I2SBUFSIZE);
			skb_queue_tail(&audio_data, skb2);
		}
	}
func_exit:
    status = usb_ep_queue(pipe->ep, req, GFP_ATOMIC);
	if(status )
		printk("re enqueue failed=%d\n", status);
	return ;

}
void hif_aow_get_channels(int *channels) 
{
	*channels =  aud_channels;
	return ;
}
char *hif_aow_isoc_buffer(int size)
{
	static char buf[I2SBUFSIZE*8*4];
	struct sk_buff *skb;
	int i, pos=0;
	static unsigned long emptyAudio = 0;

	if(size > sizeof(buf))
		printk(KERN_ERR "Over size req\n");
	if((size % I2SBUFSIZE) != 0)	
		printk("has fraction, size=%d\n", size);
	for(i=0;i< size/I2SBUFSIZE;i++)
	{ 
		skb = skb_dequeue(&audio_data);
		if(!skb) 
		{
			if(printk_ratelimit())
				printk("!!!! %s, DATA insufficient\n", __func__);
			if(i == 0)
				emptyAudio ++;
			break;
		}
		memcpy(buf+pos, skb->data, I2SBUFSIZE);
		pos += I2SBUFSIZE;
		dev_kfree_skb_any(skb);
		emptyAudio  = 0;
	}
	memset(buf+pos, 0, sizeof(buf)-pos);
	
	if(emptyAudio > 50)
		return NULL;
	else
		return (char *)buf;
}
#endif

static void 
hif_usb_rx_complete(struct usb_ep *ep, struct usb_request *req)
{
    int status = req->status;
    hif_usb_request_t *hif_req = req->context;
    hif_usb_pipe_t *pipe = hif_req->parent;
    struct sk_buff *skb = hif_req->skb;

    dma_unmap_single(g_usb_ctx.gadget->dev.parent,req->dma,
            skb->len, DMA_FROM_DEVICE);
    if(status != 0){		
        switch (status) 
        {
            printk("%s status = 0x%x\n" ,__FUNCTION__, status);
            case -ECONNABORTED:		/* hardware forced ep reset */
                printk("Connection Aborted \n");
                break;
            case -ECONNRESET:		/* request dequeued */
                printk("Connection Reset \n");
                break;
            case -ESHUTDOWN:		/* disconnect from host */
                printk("Connection shutdown: actual len %d orig len %d \n",
                        req->actual, req->length);
                break;
            default:
                printk("%s status =%d actual len %d orig length %d \n",
                       __FUNCTION__, status, req->actual, req->length);
                break;    
        }
        hif_usb_put_usb_req(pipe, hif_req);	
    }
    else{
#ifdef HIF_STREAM_MODE
        int i;
        int rxbuf_num;
        usb_rx_info_t rx_info[MAX_USB_IN_NUM];
        struct sk_buff *rxbuf_pool[MAX_USB_IN_NUM];

        rxbuf_num = hif_usb_rx_destream(skb, req->actual, rx_info);

        /* limit the number of queued packets */
        if (g_usb_ctx.tx_queue.qlen + rxbuf_num > MAX_RX_BUF_NUM) {
            if (net_ratelimit())
                printk("%s: throttling: %d %d\n", __func__, g_usb_ctx.tx_queue.qlen, rxbuf_num);
            rxbuf_num = MAX_RX_BUF_NUM - g_usb_ctx.tx_queue.qlen;
        }
        if (rxbuf_num < 0)
            rxbuf_num = 0;

        for (i = 0; i < rxbuf_num; i++) {
            struct sk_buff *new_skb;
            int offset;

            new_skb = hif_usb_alloc_skb(rx_info[i].pkt_len, 32, 4);

            if (new_skb == NULL) {
                printk("%s: can't allocate rx buffer\n", __func__);
                rxbuf_num = i;
                break;
            }

            offset = rx_info[i].offset;
            memcpy(new_skb->data, &(skb->data[offset]), rx_info[i].pkt_len);

            /* Set skb buffer length */
            #ifdef NET_SKBUFF_DATA_USES_OFFSET
            new_skb->tail = 0;
            new_skb->len = 0;
            #else
            new_skb->tail = new_skb->data;
            new_skb->len = 0;
            #endif
            skb_put(new_skb, rx_info[i].pkt_len);
            rxbuf_pool[i] = new_skb;
        }

        hif_usb_queue_req(ep, req, skb->data, USB_MAX_RX_BUFF_SIZE,
                          DMA_FROM_DEVICE);

        for (i = 0; i < rxbuf_num; i++) {
            __skb_queue_tail(&g_usb_ctx.tx_queue, rxbuf_pool[i]);
        }

        if (g_usb_ctx.tx_queue.qlen)  
           tasklet_schedule(&g_usb_ctx.tx_tasklet);

#else
        struct sk_buff  *new_skb;
        new_skb = hif_usb_alloc_skb(USB_MAX_RX_BUFF_SIZE, 32, 4);
        hif_usb_assert(new_skb);
        hif_req->skb = new_skb;

        skb_put(skb, req->actual);

        hif_usb_queue_req(ep, req, new_skb->data, USB_MAX_RX_BUFF_SIZE, 
                          DMA_FROM_DEVICE);
#ifdef LOOPBACK_SUPPORT
        if(pipe->number == HIF_USB_PIPE_RX)
        {
            hif_send(&g_usb_ctx,HIF_USB_PIPE_TX,skb);
        }
        else
        {
            dev_kfree_skb_any(skb);
        }
#else
            __skb_queue_tail(&g_usb_ctx.tx_queue, skb);
            if(g_usb_ctx.tx_queue.qlen <= 1)  
                tasklet_schedule(&g_usb_ctx.tx_tasklet);

#endif
#endif /* #ifdef HIF_STREAM_MODE */
    }

}

static void 
hif_usb_init_tx_pipe(struct usb_ep *ep, int pipe_index, int buf_num, 
                     complete_t func)
{
    hif_usb_pipe_t *pipe;
    int index;
    pipe = &g_usb_ctx.pipes[pipe_index];
    pipe->direction = USB_SEND;
    pipe->ep = ep;
    pipe->head = NULL;
    pipe->tail = NULL;
#ifdef HIF_STREAM_MODE
    if (pipe_index == TX_PIPE_INDEX) {
        pipe->txbuf_q = kmalloc(sizeof(usb_tx_buf_t) * MAX_TX_BUF_NUM, GFP_KERNEL);
        pipe->txbuf_head = 0;
        pipe->txbuf_tail = 0;
        pipe->txbuf_cnt = 0;
        spin_lock_init(&pipe->pipe_lock);
        spin_lock_init(&pipe->xmit_lock);
        hif_usb_assert(pipe->txbuf_q);
    }
#endif
    spin_lock_init(&pipe->list_lock);
    pipe->number = index_to_pipe[pipe_index];
    pipe->descriptor = get_endpt_des(pipe->number, 1);
    hif_usb_print(("%s Entry \n",__FUNCTION__));

    for(index = 0; index < buf_num; index++){
        hif_usb_request_t *usb_req;
        struct usb_request *req;
        usb_req = kzalloc(sizeof(hif_usb_request_t),GFP_KERNEL);
        hif_usb_assert(usb_req);
        usb_req->next = NULL;
        usb_req->parent = pipe;
        usb_req->skb = NULL;
        usb_req->request=usb_ep_alloc_request(ep, GFP_KERNEL);
        req = usb_req->request;
        hif_usb_assert(req);
        req->complete = func;
        req->context = usb_req;	
#ifdef HIF_STREAM_MODE
        if (pipe_index == TX_PIPE_INDEX) {
            usb_req->usb_txbuf = kmalloc(USB_TX_BUF_SIZE, GFP_ATOMIC);
            hif_usb_assert(usb_req->usb_txbuf);
        }
        else
            usb_req->usb_txbuf = NULL;
#endif
        hif_usb_print(("%s FOR TX PIPE %p index:hifindex %d:%d req: %p \n",
                       __FUNCTION__, pipe, pipe_index, pipe->number,usb_req));
        hif_usb_put_usb_req(pipe, usb_req);   
    }
    hif_usb_print(("%s Exit \n",__FUNCTION__));
}

static void 
hif_usb_init_rx_pipe(struct usb_ep *ep, int pipe_index, int buf_num, 
                     complete_t func)
{
    hif_usb_pipe_t *pipe;
    int index;
    pipe = &g_usb_ctx.pipes[pipe_index];
    pipe->direction = USB_RECV;
    pipe->ep = ep;
    pipe->head = NULL;
    pipe->tail = NULL;
    spin_lock_init(&pipe->list_lock);
    pipe->number = index_to_pipe[pipe_index];
    pipe->descriptor = get_endpt_des(pipe->number, 0);

    hif_usb_print(("%s Entry \n",__FUNCTION__));
    for(index = 0; index < buf_num; index++){
        hif_usb_request_t *usb_req;
        struct usb_request *req;
        usb_req = kzalloc(sizeof(hif_usb_request_t),GFP_KERNEL);
        hif_usb_assert(usb_req);
        usb_req->next = NULL;
        usb_req->parent = pipe;
        usb_req->request=usb_ep_alloc_request(ep, GFP_KERNEL);
        req = usb_req->request;
        hif_usb_assert(req);

        if (pipe_index == CMD_PIPE_INDEX)
            usb_req->skb = hif_usb_alloc_skb(USB_MAX_CMD_BUFF_SIZE, 32, 4);
        else
            usb_req->skb = hif_usb_alloc_skb(USB_MAX_RX_BUFF_SIZE, 32, 4);

        hif_usb_assert(usb_req->skb);
        req->complete = func;
        req->context = usb_req;
        hif_usb_print(("%s FOR RX PIPE %p index:hifindex %d:%d req: %p \n",
                       __FUNCTION__, pipe, pipe_index, pipe->number, usb_req));
        hif_usb_put_usb_req(pipe, usb_req);   
    }
    hif_usb_print(("%s Exit\n",__FUNCTION__));

}

static void 
hif_usb_free_pipes(hif_usb_context_t *hif_dev)
{
    int index;
    hif_usb_request_t *usb_req;
    for(index = 0; index < MAX_PIPES; index++){
        hif_usb_pipe_t *pipe = &hif_dev->pipes[index];
        usb_req = hif_usb_get_usb_req(pipe, 1);
        while(usb_req){
            if(usb_req->skb){
                dev_kfree_skb_any(usb_req->skb);
            }

#ifdef HIF_STREAM_MODE
            if (index == TX_PIPE_INDEX) {
                if (usb_req->usb_txbuf) {
                    kfree(usb_req->usb_txbuf);
                }
            }
#endif

            usb_ep_free_request(hif_dev->pipes[index].ep,usb_req->request);
            kfree(usb_req);
            usb_req = hif_usb_get_usb_req(pipe, 1);

        }

    }
}

static inline int 
hif_usb_init_pipes(struct usb_gadget *gadget)
{
    struct usb_ep	*ep;
    int ep_num = 0;
    hif_usb_print(("%s Entry \n",__FUNCTION__));
    list_for_each_entry(ep, &gadget->ep_list, ep_list) 
    {
        if (0 == strcmp(ep->name, "ep1out-bulk")) {
            hif_usb_print(("LP DnQ Pipe Found \n"));
            ep->driver_data = &g_usb_ctx;
            ep_num++;
            hif_usb_init_rx_pipe(ep, LP_RX_PIPE_INDEX, USB_MAX_RX_BUFFER, 
                                 hif_usb_rx_complete);
        }
#ifdef ROM_ENUM_DISABLED
        else if(0 == strcmp(ep->name, "ep4out-bulk")) {
#else
        else if(0 == strcmp(ep->name, "ep4out-int")){
#endif
            hif_usb_print(("HP DnQ Pipe Found \n"));
            ep->driver_data = &g_usb_ctx;
            ep_num++;
            hif_usb_init_rx_pipe(ep, CMD_PIPE_INDEX, USB_MAX_CMD_BUFFER, 
                                 hif_usb_rx_complete);
        }
#if (ATH_SUPPORT_AOW == 1)
        else if(strstr(ep->name, "ep3out-iso"))
		{
            printk(KERN_ERR "Event ISO Pipe Found:%s \n", ep->name);
            ep->driver_data = &g_usb_ctx;
			skb_queue_head_init(&audio_data);
			skb_queue_head_init(&audio_free);
			printk("audio_data buffer freeing TODO!\n");
            ep_num++;
            hif_usb_init_rx_pipe(ep, ISOC_PIPE_INDEX, 
                                 ISOC_MAX_BUFFER, 
                                 hif_usb_isoc_complete);
		}
#endif

        else if(0 == strcmp(ep->name, "ep2in-bulk")) {
            hif_usb_print(("Tx UpQ Pipe Found \n"));
            ep->driver_data = &g_usb_ctx;
            ep_num++;
            hif_usb_init_tx_pipe(ep, TX_PIPE_INDEX, USB_MAX_TX_BUFFER, 
                                 hif_usb_tx_complete);
        }
#ifdef ROM_ENUM_DISABLED
        else if(0 == strcmp(ep->name, "ep3in-bulk")) {
#else
        else if(0 == strcmp(ep->name, "ep4in-int")) {
#endif
            hif_usb_print(("Event UpQ Pipe Found \n"));
            ep->driver_data = &g_usb_ctx;
            ep_num++;
            hif_usb_init_tx_pipe(ep, EVENT_PIPE_INDEX, 
                                 USB_MAX_EVENT_BUFFER, 
                                 hif_usb_tx_complete);
        }
        else
            hif_usb_print(("%s ep name does not match \n",
                                   __FUNCTION__));

    }
    return ep_num;
}

static void 
hif_usb_setup_complete (struct usb_ep *ep, struct usb_request *req)
{
    if (req->status || req->actual != req->length)
        printk ("setup complete --> %d, %d/%d\n",
                req->status, req->actual, req->length);

}

static void
hif_usb_unbind (struct usb_gadget *gadget)
{
    hif_usb_context_t *hif_dev = get_gadget_data(gadget);
    struct usb_request *setup = hif_dev->setup_req;
    if(setup){
        if(setup->buf)
#if LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,15)
            usb_ep_free_buffer(gadget->ep0, setup->buf, setup->dma, 
                               setup->length);
#else
        dma_free_coherent(gadget->dev.parent, setup->length, setup->buf, 
                          setup->dma);
#endif
        usb_ep_free_request(gadget->ep0, setup);
    }

#ifdef HIF_STREAM_MODE
#if HIF_STREAM_SCHEDULE_METHOD != 1
    hif_usb_stop_timer(hif_dev);
#endif /* #if HIF_STREAM_SCHEDULE_METHOD != 1 */
#endif

    hif_usb_free_pipes(hif_dev);
    set_gadget_data(gadget,NULL);
    return;
}

static int
hif_usb_bind (struct usb_gadget *gadget)
{
    struct usb_ep  *ep;
    int result = 0;
    int ep_count = 0;

    g_usb_ctx.gadget = gadget;

    hif_usb_print(("%s Entry \n",__FUNCTION__));

    list_for_each_entry (ep, &gadget->ep_list, ep_list) 
        ep->driver_data = NULL;	

    set_gadget_data(gadget, &g_usb_ctx);

    ep_count = hif_usb_init_pipes(gadget);
    hif_usb_print(("Found %d Eps \n",ep_count));
    if(ep_count != MAX_PIPES){
        result = -ENODEV;
        goto fail;
    }
    /* Allocate setup response and buffer */
    g_usb_ctx.setup_req = usb_ep_alloc_request(gadget->ep0, GFP_KERNEL);
    if (!g_usb_ctx.setup_req){
        result = -ENOMEM;
        goto fail;
    }
#if LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,15)
    g_usb_ctx.setup_req->buf = usb_ep_alloc_buffer(gadget->ep0, SETUP_SIZE, 
                                                    &g_usb_ctx.setup_req->dma
                                                     ,GFP_KERNEL);
#else
    g_usb_ctx.setup_req->buf = dma_alloc_coherent(
                                                g_usb_ctx.gadget->dev.parent,
                                                SETUP_SIZE, 
                                                &g_usb_ctx.setup_req->dma, 
                                                GFP_KERNEL);
#endif

    if(!g_usb_ctx.setup_req->buf){
        hif_usb_print(("%s allocate setup Req Buffer Failed\n",__FUNCTION__));
        result = -ENOMEM;
        goto fail;
    }
    gadget->ep0->driver_data = &g_usb_ctx;

    g_usb_ctx.setup_req->complete = hif_usb_setup_complete;

    goto done;
fail:
    hif_usb_unbind(gadget);
done:
    return result;

}

extern struct usb_device_descriptor device_des;
extern struct usb_qualifier_descriptor qualifier_des;
#define MAX_CONFIG_INTERFACES 1

void 
hif_usb_config(hif_usb_context_t *hif_dev)
{
    /*enable all the endpoints */
    int index;
    hif_usb_print(("%s Entry \n",__FUNCTION__));
    for(index =0; index < MAX_PIPES; index++){
        hif_usb_pipe_t *pipe = &hif_dev->pipes[index];
        hif_usb_print(("Enabling Pipe %d \n",index));
        usb_ep_enable(pipe->ep, pipe->descriptor);
    }
    hif_usb_print(("%s Calling device inserted handler \n",__FUNCTION__));
    app_dev_ready(drv_callbacks, hif_dev);
}

static int
hif_usb_setup (struct usb_gadget *gadget, const struct usb_ctrlrequest *ctrl)
{
    hif_usb_context_t   *hif_dev = get_gadget_data (gadget);
    struct usb_request	*req = hif_dev->setup_req;
    int			value = -EOPNOTSUPP;
    uint16_t			w_index = le16_to_cpu(ctrl->wIndex);
    uint16_t			w_value = le16_to_cpu(ctrl->wValue);
    uint16_t			w_length = le16_to_cpu(ctrl->wLength);

    extern uint8_t read_from_otp;

    req->zero = 0;

    hif_usb_print(("%s Entry \n",__FUNCTION__));

    switch (ctrl->bRequest) {

        case USB_REQ_GET_DESCRIPTOR:
            hif_usb_print(("%s Get Descriptor Request\n",__FUNCTION__));
            if (ctrl->bRequestType != USB_DIR_IN)
                goto unknown;
            switch (w_value >> 8) {

                case USB_DT_DEVICE:
                    hif_usb_print(("%s Device Descriptor Get Request\n",
                                    __FUNCTION__));
                    value = min (w_length, (uint16_t) sizeof device_des);
                    memcpy (req->buf, &device_des, value);

                    break;
                case USB_DT_DEVICE_QUALIFIER:
                    hif_usb_print(("%s Device Qualifier Descriptor Request \n",
                                   __FUNCTION__));
                    if (!gadget->is_dualspeed)
                        break;
                    value = min (w_length, (uint16_t) sizeof qualifier_des);
                    memcpy (req->buf, &qualifier_des, value);
                    break;

                case USB_DT_OTHER_SPEED_CONFIG:

                    hif_usb_print(("%s Other speec conf Descriptor Request \n",
                                   __FUNCTION__));
                    if (!gadget->is_dualspeed)
                        break;
                case USB_DT_CONFIG:

                    hif_usb_print(("%s Config Descriptor Get Request\n",
                                   __FUNCTION__));
                    value = get_configuration(ctrl, req->buf);
                    if (value >= 0)
                        value = min (w_length, (u16) value);

                    break;

                case USB_DT_STRING:

                    hif_usb_print(("%s String Descriptor Get Request\n",
                                   __FUNCTION__));
                    value = get_string_descriptor(ctrl, req->buf, read_from_otp);
                    if(value >= 0)
                        value = min(w_length,(uint16_t)value);
                    break;
            }

            break;

        case USB_REQ_SET_CONFIGURATION:
            hif_usb_print(("%s Set Configuration Request\n",__FUNCTION__));
            if (ctrl->bRequestType != 0)
                goto unknown;
            if (gadget->a_hnp_support)
                printk ("HNP available\n");
            else if (gadget->a_alt_hnp_support)
                printk ("HNP needs a different root port\n");
            else
                printk ("HNP inactive\n");

            if(w_value == 1){
                if(hif_dev->config == w_value)
                    printk("Device Already Configured \n");
                if(!hif_dev->hif_ready)
                {
                    printk("Calling hif_usb_config function \n");
                    hif_dev->config = 1;
                    hif_usb_config(hif_dev);
                    hif_dev->hif_ready = 1;
                }
                value = 0;
            }
            else{ 
                printk("Invalid value %d \n",hif_dev->config);
                value = -EINVAL;
            }

            break;
        case USB_REQ_GET_CONFIGURATION:
            hif_usb_print(("%s Get Configuration Request\n",__FUNCTION__));
            if (ctrl->bRequestType != USB_DIR_IN){
                printk("Get Interface req type is not In failed \n");
                goto unknown;
            }
            *(uint8_t *)req->buf = hif_dev->config;
            value = min (w_length, (u16) 1);
            break;

        case USB_REQ_SET_INTERFACE:

            printk("USB Request : Set Interface value =%d \n",w_value);
            hif_dev->interface = w_value;
            if(hif_dev->start_hif){
                hif_dev->start_hif = 0;
                hif_start(hif_dev);
            }
            value = 0;


            break;
        case USB_REQ_GET_INTERFACE:

            hif_usb_print(("%s Get Interface Request\n",__FUNCTION__));
            if (ctrl->bRequestType != USB_DIR_IN)
            {
                printk("Get Interface req type is not In failed \n");
                goto unknown;
            }
            if (w_index >= MAX_CONFIG_INTERFACES)
                break;

            value = hif_dev->interface;

            if (value < 0)
                break;
            *((uint8_t *)req->buf) = value;
            value = min(w_length, (uint16_t) 1);

            break;

#define USB_REQ_CUSTOM_TARGETRESET 0x55
        case USB_REQ_CUSTOM_TARGETRESET:
            printk ("Boot Req %02x.%02x v%04x i%04x l%d\n",
                    ctrl->bRequestType, ctrl->bRequest,
                    w_value, w_index, w_length);

            printk("Recieved Reset Request ");
            printk("Recieved Reset Request ");
            /*Mark completion*/
            for(;;) {
                printk("ROM  boot reset bit 8 \n");
#ifndef BUILD_PARTNER_PLATFORM
                ar7240_reg_wr (AR7240_GPIO_OE,
                        ar7240_reg_rd(AR7240_GPIO_OE) & (~(1 <<  8)));
                ar7240_reg_wr(AR7240_RESET, AR7240_RESET_FULL_CHIP);
#endif
            }

            break;

#define USB_REQ_CUSTOM_TARGETMODE 0x56
        case USB_REQ_CUSTOM_TARGETMODE:
            printk("Query mode: %x %x %x %x\n", 
                w_value, w_index, w_length, ctrl->bRequestType);
            *((int32_t *)req->buf) = 1;
            value = 4;
            break;

        default:
unknown:
            printk ("unknown control req%02x.%02x v%04x i%04x l%d\n",
                    ctrl->bRequestType, ctrl->bRequest,
                    w_value, w_index, w_length);
    }

    /* respond with data transfer before status phase? */
    if (value >= 0) {
        req->length = value;
        if(value < w_length){
            printk("Added req->zero entry \n");
            req->zero = value < w_length;
        }
        req->status = 0;
        value = usb_ep_queue (gadget->ep0, req, GFP_ATOMIC);
        if (value < 0) {
            hif_usb_print(("%s ep_queue --> %d\n", __FUNCTION__, value));
            req->status = 0;
            hif_usb_setup_complete (gadget->ep0, req);
        }
    }

    /* device either stalls (value < 0) or reports success */
    return value;
}

static void 
hif_usb_disconnect(struct usb_gadget *gadget)
{
    hif_usb_context_t  *hif_dev;
    int index;
    hif_dev = get_gadget_data(gadget);

    if(hif_dev->config == 0){
        hif_usb_print(("Already Disconnected .... \n"));
    }

    for(index = 0; index < MAX_PIPES; index++){
        hif_usb_pipe_t *pipe = &hif_dev->pipes[index];
        usb_ep_disable(pipe->ep);
    }

    hif_dev->config = 0;
    hif_dev->start_hif = 1;

}

static void
hif_usb_suspend(struct usb_gadget *gadget)
{
    hif_usb_print(("Device Suspend \n"));
}


static void
hif_usb_resume(struct usb_gadget *gadget)
{
    hif_usb_print(("Device Resumed \n"));

}


static const char usb_str_product[] = "Atheros-usb";
static const char driver_name[] 	= "Atheros-hif-usb";
static struct usb_gadget_driver hif_usb_driver = {
	.speed		= USB_SPEED_HIGH,	// dual speed (full and high)
	.function	= (char*)usb_str_product,
	.bind		= hif_usb_bind,
	.unbind		= hif_usb_unbind,
	.setup		= hif_usb_setup,
	.disconnect	= hif_usb_disconnect,
	.suspend	= hif_usb_suspend,
	.resume	= hif_usb_resume,
	.driver 	= {
		.name		= (char*)driver_name,
		.owner		= THIS_MODULE,
	},
};


void 
init_usb_drv(void)
{
    /* setup all the descriptor needed for configuration */
    usb_gadget_register_driver(&hif_usb_driver);
#if (TARGET_STANDALONE == 1)
	printk("TARGET is in standalone mode. USB is left uninitialized\n");
#else
#ifdef ROM_ENUM_DISABLED
    printk("ROM Enum Disabled calling hif_usb_config function \n");
    hif_usb_config(&g_usb_ctx);
#endif
#endif

}


void hif_setup_descriptor(void)
{
    init_device_des();
    init_configuration_des();
    init_endpt_des();
    init_qualifier_des();
    init_other_speed_cfg_des();
}

hif_status_t 
hif_init(hif_os_callback_t *callbacks)
{
    drv_callbacks.device_detect = callbacks->device_detect;
    drv_callbacks.device_disconnect = callbacks->device_disconnect;
    drv_callbacks.device_suspend = callbacks->device_suspend;
    drv_callbacks.device_resume = callbacks->device_resume;	

    hif_read_usb_config();
    hif_setup_descriptor();
    init_usb_drv();

    return HIF_STATUS_OK;
}

hif_status_t   
hif_cleanup(hif_handle_t  hif_handle)
{
    usb_gadget_unregister_driver(&hif_usb_driver);
    return HIF_STATUS_OK;
}

void 
hif_claim_device(hif_handle_t hif_dev, void *context)
{
    g_usb_ctx.app_handle = context;
}

hif_status_t        
hif_start(hif_handle_t hif_handle)
{
    int index;
    hif_usb_context_t *hif_dev = (hif_usb_context_t*)hif_handle;
    hif_usb_request_t *req;
    hif_usb_pipe_t *pipe;

    hif_usb_print(("%s Entry \n",__FUNCTION__));
#ifndef ROM_ENUM_DISABLED
    if(!hif_dev->config){
        hif_usb_print(("%s device not configured yet can not start HIF \n",
                       __FUNCTION__));
        return HIF_STATUS_ERROR;
    }
#endif

    /*queue all the ep object for receiving */
    for(index = 0; index < MAX_RX_PIPES; index++){
        int count = 0;
        pipe = &hif_dev->pipes[index];
        req = hif_usb_get_usb_req(pipe, 1);
        while(req != NULL){
            hif_usb_print(("%s pipe:%p Req:%p index:hifindex %d:%d \n",
                           __FUNCTION__, pipe, req, index, pipe->number));
            if (pipe->number == HIF_USB_PIPE_COMMAND)
                hif_usb_queue_req(pipe->ep, req->request, req->skb->data ,
                                  USB_MAX_CMD_BUFF_SIZE , DMA_FROM_DEVICE);
            else
                hif_usb_queue_req(pipe->ep, req->request, req->skb->data ,
                                  USB_MAX_RX_BUFF_SIZE , DMA_FROM_DEVICE);
            count++;
            req = hif_usb_get_usb_req(pipe, 1);
        } 
        hif_usb_print(("%s : pipe : %p totalReq %d \n",__FUNCTION__, 
                       pipe, count));
    }
#if (ATH_SUPPORT_AOW == 1)
	printk("Starting ISO Rece\n");
	index = 4;
    {
        int count = 0;
        pipe = &hif_dev->pipes[index];
        req = hif_usb_get_usb_req(pipe, 1);
        while(req != NULL){
            hif_usb_print(("%s pipe:%p Req:%p index:hifindex %d:%d \n",
                           __FUNCTION__, pipe, req, index, pipe->number));
            hif_usb_queue_req(pipe->ep, req->request, req->skb->data ,
                              USB_MAX_RX_BUFF_SIZE , DMA_FROM_DEVICE);
            count++;
            req = hif_usb_get_usb_req(pipe, 1);
        } 
        hif_usb_print(("%s : pipe : %p totalReq %d \n",__FUNCTION__, 
                       pipe, count));
    }
#endif


    /* 
     * Process Tx packets in SoftIRQ Context Only
     */
    tasklet_init(&hif_dev->tx_tasklet, hif_usb_tx_tasklet, 
            (unsigned long)hif_dev);
    skb_queue_head_init(&hif_dev->tx_queue);
    spin_lock_init(&hif_dev->txq_lock);

#ifdef HIF_STREAM_MODE
#if HIF_STREAM_SCHEDULE_METHOD == 1
    tasklet_init(&hif_dev->stream_tx_tasklet, hif_usb_xmit_txq, 
            (unsigned long)hif_dev);
#else
    hif_usb_start_timer(hif_dev);
#endif /* #if HIF_STREAM_SCHEDULE_METHOD == 1 */
#endif

    return HIF_STATUS_OK;

}

#ifdef HIF_STREAM_MODE

#if HIF_STREAM_SCHEDULE_METHOD != 1

void hif_usb_timer(unsigned long param)
{
    hif_usb_context_t *hif_dev = (hif_usb_context_t *) param;
    hif_usb_pipe_t *pipe = &hif_dev->pipes[TX_PIPE_INDEX];    
    uint16_t txbuf_cnt;
    int hif_txq_size;
    struct timeval tv;
    static int iter = 0;

    iter++;

    if (iter >= HIF_ONE_SEC_ITER) {
        atomic_set(&(pipe->txframe_persec), atomic_read(&(pipe->txframe_cnt)));
        atomic_set(&(pipe->txframe_cnt), 0);
        iter = 0;
    }

    txbuf_cnt = hif_usb_check_txbuf_cnt(pipe, &tv, &hif_txq_size);

    if (txbuf_cnt != 0) {
        struct timeval now;
        struct timeval result;

        do_gettimeofday(&now);

        timersub(now, tv, result);

        if (result.tv_usec >= 2000) {
            hif_usb_xmit(pipe, HIF_USB_PIPE_TX, 1);
        }

    }

    mod_timer(&(hif_dev->one_sec_timer), jiffies + HIF_TIMER_JIFFIES_CNT);
}

void 
hif_usb_start_timer(hif_usb_context_t *hif_dev)
{
    init_timer(&hif_dev->one_sec_timer);
    hif_dev->one_sec_timer.function = hif_usb_timer;
    hif_dev->one_sec_timer.data = (unsigned long)hif_dev;

    mod_timer(&(hif_dev->one_sec_timer), jiffies + HIF_TIMER_JIFFIES_CNT);
}

void
hif_usb_stop_timer(hif_usb_context_t *hif_dev)
{
    del_timer(&(hif_dev->one_sec_timer));
}

#endif /* #if HIF_STREAM_SCHEDULE_METHOD != 1 */

int
hif_usb_put_txbuf(hif_usb_pipe_t *pipe, struct sk_buff *buf)
{
    unsigned long flags;

    spin_lock_irqsave(&(pipe->pipe_lock), flags);

    if (pipe->txbuf_cnt < MAX_TX_BUF_NUM)
    {
        usb_tx_buf_t *txbuf = &(pipe->txbuf_q[pipe->txbuf_tail]);

        txbuf->buf = buf;

        pipe->txbuf_tail = ((pipe->txbuf_tail+1) & (MAX_TX_BUF_NUM - 1));
        pipe->txbuf_cnt++;
#if HIF_STREAM_SCHEDULE_METHOD != 1
        do_gettimeofday(&txbuf->timeval);
        pipe->total_size += buf->len;
#endif
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

#if HIF_STREAM_SCHEDULE_METHOD == 1
uint16_t
hif_usb_check_txbuf_cnt(hif_usb_pipe_t *pipe)
{
    uint16_t txbuf_cnt;
    unsigned long flags;

    spin_lock_irqsave(&(pipe->pipe_lock), flags);

    txbuf_cnt = pipe->txbuf_cnt;

    spin_unlock_irqrestore(&(pipe->pipe_lock), flags);
    return txbuf_cnt;
}
#else
uint16_t
hif_usb_check_txbuf_cnt(hif_usb_pipe_t *pipe, struct timeval *timestamp, int *hif_txq_size)
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

        if (total_size >= USB_TX_BUF_SIZE)
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
#endif /* #if HIF_STREAM_SCHEDULE_METHOD == 1 */

usb_tx_buf_t*
hif_usb_get_txbuf(hif_usb_pipe_t *pipe)
{
    unsigned long flags;
    usb_tx_buf_t *txbuf;

    spin_lock_irqsave(&(pipe->pipe_lock), flags);

    if (pipe->txbuf_cnt > 0)
    {
        txbuf = &(pipe->txbuf_q[pipe->txbuf_head]);
        pipe->txbuf_head = ((pipe->txbuf_head+1) & (MAX_TX_BUF_NUM - 1));
        pipe->txbuf_cnt--;
#if HIF_STREAM_SCHEDULE_METHOD != 1
        pipe->total_size -= txbuf->buf->len;
#endif
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

#if HIF_STREAM_SCHEDULE_METHOD == 1
void
hif_usb_xmit_txq(unsigned long context)
{
    hif_usb_context_t *device = (hif_usb_context_t*)context;
    hif_usb_pipe_t *pipe = &device->pipes[TX_PIPE_INDEX];
    hif_usb_request_t *hif_req;
    usb_tx_buf_t *txbuf;
    uint16_t *txbuf_hdr;
    uint32_t len = 0;
    uint16_t offset = 0;
    uint8_t *txbuf_ptr;
    int last_pad;
    int pktcount = 0;

again:
    /* peek txq */
    if (hif_usb_check_txbuf_cnt(pipe) == 0)
        return;

    /* get usb ctx */
    hif_req = hif_usb_get_usb_req(pipe, 1);
    if (hif_req == NULL) {
        /* after completion, tx_tasklet would be scheduled */
        return;
    }
    pktcount = len = offset = last_pad = 0;
    while((txbuf = hif_usb_get_txbuf(pipe))) {
        /* check if buffer is enough */
        if (offset + txbuf->buf->len + (HIF_USB_STREAM_TAG_LEN << 1)
                >= USB_TX_BUF_SIZE) {
            hif_usb_put_txbuf(pipe, txbuf->buf);
            break;
        }

        /* streaming the packet */
        txbuf_ptr = hif_req->usb_txbuf + offset;
        txbuf_hdr = (uint16_t *)txbuf_ptr;

        /* add packet length and tag information */
        txbuf_ptr += HIF_USB_STREAM_TAG_LEN;
        *txbuf_hdr++ = cpu_to_le16(txbuf->buf->len);
        *txbuf_hdr++ = cpu_to_le16(HIF_USB_STREAM_TAG);

        len = txbuf->buf->len;
        memcpy(txbuf_ptr, txbuf->buf->data, len);
        len += HIF_USB_STREAM_TAG_LEN;

        app_ind_xmitted(txbuf->buf);

        last_pad = HIF_USB_STREAM_TAG_LEN - (len & (HIF_USB_STREAM_TAG_LEN - 1));
        if (last_pad == HIF_USB_STREAM_TAG_LEN)
            last_pad = 0;

        offset += (len + last_pad);

        if (pktcount++ >= MAX_TX_AGGREGATE_NUM) {
            break;
        }
    }

    /* FIXME: deal with failed case */
    if (offset) {
        hif_usb_queue_req(pipe->ep, hif_req->request, hif_req->usb_txbuf, offset - last_pad, DMA_TO_DEVICE);
        goto again;
    } else {
        hif_usb_put_usb_req(pipe, hif_req);
    }
}
#else
hif_status_t
__hif_usb_xmit(hif_usb_pipe_t *pipe, uint32_t pipe_id, int flush)
{
    hif_status_t status = HIF_STATUS_OK;
    hif_usb_request_t *hif_req;
    struct timeval timestamp;
    int i;
    int16_t txbuf_cnt;
    int hif_txq_size;
    usb_tx_buf_t *txbuf_pool[MAX_TX_AGGREGATE_NUM_ITEM];

    timestamp.tv_usec = 0;

    if (flush == 0)
        hif_req = hif_usb_get_usb_req(pipe, 0);
    else
        hif_req = hif_usb_get_usb_req(pipe, 1);

    if (hif_req == NULL) {
        return status;
    }

    txbuf_cnt = hif_usb_check_txbuf_cnt(pipe, &timestamp, &hif_txq_size);

    if (txbuf_cnt == 0) {
        /* Give the urb back */
        hif_usb_put_usb_req(pipe, hif_req);
        goto done;
    }
    else if ((atomic_read(&(pipe->txframe_persec)) >= L1_TX_PACKET_THR) &&
             (hif_txq_size < HIF_QUEUE_SIZE) && flush == 0) {
        struct timeval now;
        struct timeval result;

        do_gettimeofday(&now);

        timersub(now, timestamp, result);

        if (result.tv_usec <= HIF_USB_TX_TIMEOUT) {
            /* Give the urb back */
            hif_usb_put_usb_req(pipe, hif_req);
            goto done;
        }
    }

xmit_start:

    if (txbuf_cnt == 1) {
        uint16_t *txbuf_hdr;
        usb_tx_buf_t *txbuf = hif_usb_get_txbuf(pipe);

        if (txbuf == NULL) {
            hif_usb_put_usb_req(pipe, hif_req);
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

                hif_usb_put_usb_req(pipe, hif_req);
                goto done;
            }
        }

        skb_push(txbuf->buf, HIF_USB_STREAM_TAG_LEN);

        txbuf_hdr = (uint16_t *)txbuf->buf->data;
        *txbuf_hdr++ = cpu_to_le16(txbuf->buf->len - HIF_USB_STREAM_TAG_LEN);
        *txbuf_hdr++ = cpu_to_le16(HIF_USB_STREAM_TAG);

        hif_req->skb = txbuf->buf;

        status = hif_usb_queue_req(pipe->ep, hif_req->request, txbuf->buf->data, 
                                   txbuf->buf->len, DMA_TO_DEVICE);
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

            txbuf_ptr = hif_req->usb_txbuf;
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

        hif_req->skb = NULL;

        if (txbuf_cnt != 0) {
            status = hif_usb_queue_req(pipe->ep, hif_req->request, hif_req->usb_txbuf, 
                                       len, DMA_TO_DEVICE);
        }
        else {
            goto done;
        }
    }

    if (status) {
        /* Free tx buffer */
        hif_usb_print(("%s Failed to send Data for pipe %p hif_req %p \n",
                    __FUNCTION__, pipe, hif_req));

        if (txbuf_cnt != 1) {
            for (i = 0; i < txbuf_cnt; i++) {
                struct sk_buff *buf = txbuf_pool[i]->buf;
                app_ind_xmitted(buf);
            }
        }
        else {
            /* Because the buf contains usb tx tag, we need to revert the header */
            skb_pull(hif_req->skb, HIF_USB_STREAM_TAG_LEN);
            app_ind_xmitted(hif_req->skb);
        }

        hif_req->skb = NULL;
        hif_usb_put_usb_req(pipe, hif_req);

        /* we need to keep return value successfull, or we might have double free problems */
        status = HIF_STATUS_OK;
        goto done;
    }

    atomic_add(txbuf_cnt, &pipe->txframe_cnt);

    if (txbuf_cnt != 1) {
        for (i = 0; i < txbuf_cnt; i++) {
            struct sk_buff *buf = txbuf_pool[i]->buf;
            app_ind_xmitted(buf);
        }
    }

    /* check whether we need to resend */
    txbuf_cnt = hif_usb_check_txbuf_cnt(pipe, &timestamp, &hif_txq_size);

    if (hif_txq_size > USB_TX_BUF_SIZE) {
        hif_req = hif_usb_get_usb_req(pipe, 1);

        if (hif_req == NULL) {
            goto done;
        }

        goto xmit_start;
    }

done:
    return status;
}

hif_status_t
hif_usb_xmit(hif_usb_pipe_t *pipe, uint32_t pipe_id, int flush)
{
    hif_status_t status;
    unsigned long flag;

    spin_lock_irqsave(&(pipe->xmit_lock), flag);
    status = __hif_usb_xmit(pipe, pipe_id, flush);
    spin_unlock_irqrestore(&(pipe->xmit_lock), flag);

    return status;
}
#endif /* #if HIF_STREAM_SCHEDULE_METHOD == 1 */

#endif

hif_status_t
hif_send(hif_handle_t hif_handle, uint32_t pipe_id, struct sk_buff *buf)
{
    hif_usb_pipe_t *pipe;
    hif_usb_request_t *hif_req;
    int status = HIF_STATUS_OK;
    hif_usb_context_t *hif_dev = (hif_usb_context_t*)hif_handle;
    hif_usb_print(("%s Entry \n",__FUNCTION__));
    switch(pipe_id){

        case HIF_USB_PIPE_TX:
            pipe = &hif_dev->pipes[TX_PIPE_INDEX];
            break;

        case HIF_USB_PIPE_INTERRUPT:
            pipe = &hif_dev->pipes[EVENT_PIPE_INDEX];
            break;

        default:
            printk("Invalid Pipe Id for transferring data \n");
            return status;
    }
#ifdef HIF_STREAM_MODE
    if (pipe_id == HIF_USB_PIPE_TX) {
        pipe = &hif_dev->pipes[TX_PIPE_INDEX];

        /* queue buffer into txbuf_q */
        if (hif_usb_put_txbuf(pipe, buf) != 0) {
            hif_usb_print(("%s failed to put txbuf into usb pipe\n", __func__));
            return HIF_STATUS_ERROR;
        }

    #if HIF_STREAM_SCHEDULE_METHOD == 1
        tasklet_schedule(&hif_dev->stream_tx_tasklet);
    #else
        hif_usb_xmit(pipe, HIF_USB_PIPE_TX, 0);
    #endif /* #if HIF_STREAM_SCHEDULE_METHOD != 1 */    
    }
    else {
#endif
    hif_req = hif_usb_get_usb_req(pipe, 1);
    if (hif_req) {
        hif_req->skb = buf;
        hif_usb_print(("%s pipe %p hifidx %d queued tx req %p\n",__FUNCTION__, 
                       pipe, pipe->number, hif_req));
        status = hif_usb_queue_req(pipe->ep, hif_req->request, buf->data, 
                                   buf->len, DMA_TO_DEVICE);   
        if (status < 0) {
            hif_usb_print(("%s Error in Transferring data\n",__FUNCTION__));
            hif_req->skb = NULL;
            hif_usb_put_usb_req(pipe, hif_req);
            status = HIF_STATUS_ERROR;
        }
    }
    else
        status = HIF_STATUS_ERROR;
#ifdef HIF_STREAM_MODE
    }
#endif
    return status;
}

uint32_t         
hif_get_reserveheadroom(hif_handle_t handle)
{
#ifdef HIF_STREAM_MODE
    return HIF_USB_STREAM_TAG_LEN;
#else
    return 0;
#endif
}

void 
hif_register(hif_handle_t handle, hif_callback_t *cb) 
{

    hif_usb_print(("%s Entry \n",__FUNCTION__));
    g_usb_ctx.hif_cb.send_buf_done = cb->send_buf_done;
    g_usb_ctx.hif_cb.recv_buf      = cb->recv_buf;
    g_usb_ctx.hif_cb.context       = cb->context;

    return;
}

uint32_t 
hif_get_dlpipe(hif_pipetype_t pipe_type)
{
    uint32_t dl_pipe;
    switch(pipe_type)
    {
        case HIF_CONTROL_PIPE:
            dl_pipe = HIF_USB_PIPE_COMMAND; 
            break;
        case HIF_COMMAND_PIPE:
            dl_pipe = HIF_USB_PIPE_RX; 
            break;
        case HIF_DATA_LP_PIPE:
            dl_pipe = HIF_USB_PIPE_RX; 
            break;
        case HIF_WSUPP_PIPE:
            dl_pipe = HIF_USB_PIPE_RX; 
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
            ul_pipe = HIF_USB_PIPE_INTERRUPT; 
            break;
        case HIF_COMMAND_PIPE:
            ul_pipe = HIF_USB_PIPE_TX; 
            break;
        case HIF_DATA_LP_PIPE:
            ul_pipe = HIF_USB_PIPE_TX; 
            break;
        case HIF_WSUPP_PIPE:
            ul_pipe = HIF_USB_PIPE_TX; 
            break;
        default:
            ul_pipe = 0;
            break;
    }
    return ul_pipe;
}

int 
init_usb_hif(void)
{
    printk("USB Target HIF Loaded...\n");
    return 0;
}

void 
exit_usb_hif(void)
{
    printk("USB TARGET HIF UnLoaded...\n");
}

EXPORT_SYMBOL(hif_register);
EXPORT_SYMBOL(hif_send);
EXPORT_SYMBOL(hif_start);
EXPORT_SYMBOL(hif_cleanup);
EXPORT_SYMBOL(hif_init);
EXPORT_SYMBOL(hif_get_dlpipe);
EXPORT_SYMBOL(hif_get_ulpipe);
EXPORT_SYMBOL(hif_get_reserveheadroom);
EXPORT_SYMBOL(hif_claim_device);
#if (ATH_SUPPORT_AOW == 1)
EXPORT_SYMBOL(hif_aow_isoc_buffer);
EXPORT_SYMBOL(hif_aow_get_channels);
#endif
module_init(init_usb_hif);
module_exit(exit_usb_hif);

MODULE_LICENSE("Dual BSD/GPL");



