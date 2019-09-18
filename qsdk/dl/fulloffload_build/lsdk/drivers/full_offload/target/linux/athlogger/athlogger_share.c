#include <linux/types.h>
#include <linux/version.h>
#include <linux/skbuff.h>
#include <linux/netdevice.h>
#include <linux/if_ether.h>
#include <linux/etherdevice.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/timer.h>
#include <linux/if_vlan.h>
#include <linux/list.h>

#include "a_base_types.h"
#include "hif_api.h"

#include "athlogger.h"

/* TODO: Design a Polling mechanism (ndo_poll_controller) like netpoll to 
  transfer the log to Host, even after the interrupts disabled. */

/* Abstraction for Bus Layer*/
#define athlog_send_skb(logger,skb) \
              hif_send_dbglog(logger->share_handle, HIF_DBGLOG_PIPE, skb)

#define athlog_bus_headroom(logger) \
              hif_get_reserveheadroom(logger->share_handle)

/*TODO:  replace with adf_nbuf_alloc function */
static inline struct sk_buff*
athlog_alloc_skb(athlogger_t *logger, size_t size, int reserve, int align)
{
    struct sk_buff *skb;
    unsigned long offset;

    if(align)
        size += (align - 1);

    skb = alloc_skb(size, GFP_ATOMIC);
    if (skb == NULL) {
        return NULL;
    }

    /* Align & make sure that the tail & data are adjusted properly */
    if(align){
        offset = ((unsigned long) skb->data) % align;
        if(offset)
            skb_reserve(skb, align - offset);
    }

    skb_reserve(skb, reserve);
    return skb;
}

static inline void
athlog_free_skb(struct sk_buff *skb)
{
    kfree_skb(skb);
}

static hif_status_t athlog_tx_finish(void *ctx, struct sk_buff *skb)
{
/*    athlogger_t *logger = (athlogger_t *)ctx; */

    ATHLOGGER_DBG_PRINT("\n%s: DBGLOG Send Complete ", __func__);
    return HIF_STATUS_OK;
}

static hif_status_t
athlog_recv(void *ctx, struct sk_buff *skb, a_uint8_t pipeid)
{
    unsigned char *data;
    unsigned int len = 0;
    unsigned long flags;
    athlogger_t *logger = (athlogger_t *)ctx;
    athlog_hdr_t *loghdr;
    athlogger_buf_t *buf = NULL;
    
    if (logger->mode & ATHLOGGER_MODE_TARGET) {
        printk("\n%s: ERROR: Target Mode Should not get DBGLOG", __func__);
        athlog_free_skb(skb);
        return HIF_STATUS_OK;
    } 

    /* Host Mode */
    loghdr = (athlog_hdr_t *)skb->data;
    len = skb->len;
    data =  skb->data + sizeof(athlog_hdr_t);
    len = len - sizeof (athlog_hdr_t);

    /* process log hdr */
    if(loghdr->type == ATHLOGGER_LOG_TYPE_CRASH) {
        ATHLOGGER_DBG_PRINT("\n%s:Recvd %d bytes CRASH log ", __func__, len);
        buf = logger->crashbuf;
    }else {
        ATHLOGGER_DBG_PRINT("\n%s:Recvd %d bytes Live log", __func__, len);
        buf = logger->logbuf;
    }
    spin_lock_irqsave(&logger->lock, flags);
    athlog_buf_write(buf, data, len);
    spin_unlock_irqrestore(&logger->lock, flags);        

    athlog_free_skb(skb);
    return HIF_STATUS_OK;
}

static hif_status_t athlog_transport_start(hif_handle_t hif_handle)
{
     hif_callback_t    hifcb;
     athlogger_t *logger = athlog_get_handle();

     ATHLOGGER_DBG_PRINT("\n%s()++ hif_handle:0x%x", \
                            __func__, (unsigned int)hif_handle );
     hifcb.context = logger;
     hifcb.send_buf_done  = athlog_tx_finish;
     hifcb.recv_buf  = athlog_recv;
     hif_register_for_dbglog(hif_handle, &hifcb);

     logger->share_handle = hif_handle;

     /* start the logger as hif detected a device */
     schedule_work(&logger->boot_worker);

     ATHLOGGER_DBG_PRINT("\n%s()--", __func__);
     return HIF_STATUS_OK;
}

static hif_status_t athlog_transport_stop(void *ctx)
{
     athlogger_t *logger = (athlogger_t *)ctx;

     ATHLOGGER_DBG_PRINT("\n%s++", __func__);
     logger->share_handle = NULL;
     ATHLOGGER_DBG_PRINT("\n%s()--", __func__);
     return HIF_STATUS_OK;
}

void athlog_transport_register(athlogger_t *logger)
{
     hif_os_callback_t cb;

     ATHLOGGER_DBG_PRINT("\n%s()++", __func__);
     memset(&cb, 0, sizeof(cb));
     cb.device_detect  = athlog_transport_start;
     cb.device_disconnect = athlog_transport_stop;
  
     hif_init_dbglog(&cb);
     ATHLOGGER_DBG_PRINT("\n%s()--", __func__);
}

void athlog_transport_unregister(athlogger_t *logger)
{
     hif_os_callback_t cb;
     hif_callback_t    hifcb;
     hif_handle_t hif_handle = (hif_handle_t)logger->share_handle;

     ATHLOGGER_DBG_PRINT("\n%s()++", __func__);

     if(hif_handle) {
       memset(&hifcb, 0, sizeof(hifcb));
       hif_register_for_dbglog(hif_handle, &hifcb);

       memset(&cb, 0, sizeof(cb));
       hif_init_dbglog(&cb);
     }

     ATHLOGGER_DBG_PRINT("\n%s()--", __func__);
}

/* msg should include logger Header that identifies logging module */
athlog_share_status_t
athlog_send_log(athlogger_t *logger, const char *msg, int len)
{
     struct sk_buff *skb;
     unsigned int headroom;
     hif_status_t status = HIF_STATUS_OK;
     a_uint8_t    *data;
     athlog_hdr_t *loghdr;

     if(!logger->share_handle)
       return (ATHLOGGER_SHARE_INVALID);

     headroom = athlog_bus_headroom(logger) + sizeof(athlog_hdr_t);
     /* Allocate skb and copy the data */
     skb = athlog_alloc_skb(logger, (len + headroom), headroom, 0);
     if (!skb)
         return (ATHLOGGER_SHARE_FAILED);

     /* copy the data to the buffer */
     data = (a_uint8_t *)skb_put(skb,len);
     memcpy(data, msg, len);

     /* attach athlogger header */
     loghdr = (athlog_hdr_t *)skb_push(skb, sizeof(athlog_hdr_t));
     if(logger->state == ATHLOGGER_STATE_BOOT)
         loghdr->type = ATHLOGGER_LOG_TYPE_CRASH;
     else
         loghdr->type = ATHLOGGER_LOG_TYPE_LIVE;
     loghdr->flags  = 0;
     loghdr->len    = htons(len);

     /* send the skb */
     status = athlog_send_skb(logger, skb);
     if(status != HIF_STATUS_OK)
         return (ATHLOGGER_SHARE_FAILED);
     else
         return (ATHLOGGER_SHARE_SUCESS);
}


