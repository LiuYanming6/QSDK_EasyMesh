/*
 * Copyright (c) 2010, Atheros Communications Inc.
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
 * 
 */

#include <linux/skbuff.h>
#include <linux/netdevice.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/rtnetlink.h>
#include <linux/wireless.h>
#include <linux/netlink.h>
#include <net/iw_handler.h>
#include <net/sock.h>

#include <a_base_types.h>
#include <hif_api.h>
#include <htc_api.h>
#include <wmi_api.h>
#include <atd_wsupp.h>
#include <atd_wsupp_msg.h>
#include <atd_hdr.h>
#include <atd_cmd.h>
#include <atd_internal.h>
#include <atd_netlink.h>

#define WSUPP_HELPER_DEBUG 0



/* global values */
static int wh_netlink_num = NETLINK_GENERIC + 6;
static atd_tgt_netlink_handle_t wh_netlink_handle = NULL;

/* HTC callbacks  */
static void wh_htc_txcomp(void *ctx, struct sk_buff *skb, htc_endpointid_t ep);
static void wh_htc_rx(void *ctx, struct sk_buff *skb, htc_endpointid_t ep);

/* private structures for wpa_supplicant message helper */
typedef struct wsupp_helper {
    /* tgt device links */
    atd_tgt_dev_t           *wh_tdev;
    /* htc stuff */
    htc_handle_t            wh_htc;
    htc_endpointid_t        wh_epid;
    /* locking */
    struct mutex            wh_mutex;
    spinlock_t              wh_spinlock;
    a_uint32_t              wh_stop;
    /* wsupp message queue handling */
    wsupp_helper_work_t     wh_work;
    struct sk_buff_head     wh_msgq;
} wsupp_helper_t;

static void
wh_dump_wsupp_msg(char *banner, wsupp_msg_t *wmsg)
{
#if WSUPP_HELPER_DEBUG
    printk("%s: wmsg: type: %d %d if: %s len: %d: data: %s\n",
        banner,
        wmsg->wm_type, wmsg->wm_wmi,
        wmsg->wm_ifname, wmsg->wm_len,
        wmsg->wm_type == WSUPP_MSG_TYPE_WMI ? "WMI" : wmsg->wm_data);
#endif
}

static void
wh_netlink_input(void *ctx, struct sk_buff *skb, int addr)
{
    struct sk_buff *msgbuf;
    wsupp_helper_t *wh = (wsupp_helper_t *) ctx;
    atd_tgt_dev_t *tdev;
    atd_wsupp_message_t *msg;
    a_uint32_t reserve, msgbuf_len;

    wh_assert(wh);

    /* check if this is WMI related */
    if (atd_tgt_wsupp_response(wh->wh_tdev, skb->data, skb->len))
        return;

    /* the normal path */
    tdev = wh->wh_tdev;
    reserve = htc_get_reserveheadroom(tdev->htc_handle);
    msgbuf_len = skb->len + sizeof(atd_wsupp_message_t);

    msgbuf = alloc_skb(msgbuf_len + reserve + 1, GFP_ATOMIC);
    if (!msgbuf) {
        printk("%s: can't alloc skb", __func__);
        goto fail;
    }
    skb_reserve(msgbuf, reserve);

    msg = (atd_wsupp_message_t *) skb_put(msgbuf, msgbuf_len);
    msg->len = skb->len;
    memcpy(msg->data, skb->data, skb->len);

    msg->data[msg->len] = '\0';
    wh_dump_wsupp_msg((char *) __func__, (wsupp_msg_t *) msg->data);

    htc_send(wh->wh_htc, msgbuf, wh->wh_epid);

    return;
fail:
    /* skb would be freed by netlink framework */
    return;
}

static void
wh_wsupp_output(void *ctx, struct sk_buff *buf)
{
    struct sk_buff *skb;
    atd_wsupp_message_t *msg;
    wsupp_helper_t *wh = (wsupp_helper_t *) ctx;

    wh_assert(wh);

    msg = (atd_wsupp_message_t *) buf->data;

    skb = atd_tgt_netlink_alloc(msg->len + 1);
    if (!skb)
        goto out;

    skb_put(skb, msg->len);
    memcpy(skb->data, msg->data, msg->len);

    skb->data[msg->len] = '\0';
    wh_dump_wsupp_msg((char *) __func__, (wsupp_msg_t *) skb->data);

    atd_tgt_netlink_broadcast(wh_netlink_handle, skb, 1);

out:
    dev_kfree_skb_any(buf);
}

static void
wh_work_run(void *arg)
{
    unsigned long flags;
    wsupp_helper_t *wh = arg;
    struct sk_buff *skb;

    wh_assert(wh);

    /* protect the work thread from detach */
    mutex_lock(&wh->wh_mutex);

    do {
        /* protect the message queue  */
        spin_lock_irqsave(&wh->wh_spinlock, flags);
        skb = skb_dequeue(&wh->wh_msgq);
        spin_unlock_irqrestore(&wh->wh_spinlock, flags);

        if(!skb) break;

        wh_wsupp_output(wh, skb);

    } while(1);

    mutex_unlock(&wh->wh_mutex);
}

wsupp_handle_t
atd_tgt_wsupp_helper_init(void *tdev, htc_handle_t htc_handle)
{
    wsupp_helper_t *wh;
    htc_epcallback_t cb;

    wh = (wsupp_helper_t *) kmalloc(sizeof(*wh), GFP_ATOMIC);
    if (wh == NULL)
        return NULL;

    if (wh_netlink_handle == NULL) {
        wh_netlink_handle = atd_tgt_netlink_create(wh_netlink_input, wh,
                wh_netlink_num, 0);
        if (wh_netlink_handle == NULL) {
            printk("no netlink interface created\n");
            kfree(wh);
            return NULL;
        }
        printk("Netlink interface number created: %d\n", wh_netlink_num);
    }

    /* helper context setup */
    wh->wh_tdev = tdev;
    wh->wh_htc = htc_handle;
    mutex_init(&wh->wh_mutex);
    spin_lock_init(&wh->wh_spinlock);
    wh->wh_stop = 0;

    skb_queue_head_init(&wh->wh_msgq);
    wsupp_helper_init_work(&wh->wh_work, wh_work_run, wh);

    /* register service */
    cb.ep_rx       = wh_htc_rx;
    cb.ep_txcomp   = wh_htc_txcomp;
    cb.ep_ctx      = wh;
    wh->wh_epid = htc_register_service(htc_handle, &cb, WMI_WSUPP_SVC);

    return (wsupp_handle_t) wh;
}

void
atd_tgt_wsupp_helper_cleanup(wsupp_handle_t wsupp_handle)
{
    wsupp_helper_t *wh = wsupp_handle;
    struct sk_buff *skb;

    /* should run on sleepable context and HTC/HIF disabled */
    wh_assert(wh);

    /* prevent msg from queuing */
    wh->wh_stop = 0;

    /* disable work and acquire the mutex to cleanup msgq */
    mutex_lock(&wh->wh_mutex);
    while((skb = skb_dequeue(&wh->wh_msgq)))
        dev_kfree_skb_any(skb);
    mutex_unlock(&wh->wh_mutex);
    mutex_destroy(&wh->wh_mutex);

    kfree(wh);

    /* cleanup netlink when needed */
    if (wh_netlink_handle) {
        printk("Netlink interface number deleted: %d\n", wh_netlink_num);
        atd_tgt_netlink_delete(wh_netlink_handle);
    }

}

static void
wh_htc_txcomp(void *ctx, struct sk_buff *skb, htc_endpointid_t ep)
{
    dev_kfree_skb_any(skb);
}

static void
wh_htc_rx(void *ctx, struct sk_buff *skb, htc_endpointid_t ep)
{
    unsigned long flags;
    wsupp_helper_t *wh = ctx;

    wh_assert(wh);

    if (wh->wh_stop) {
        dev_kfree_skb_any(skb);
        return;
    }

    spin_lock_irqsave(&wh->wh_spinlock, flags);
    skb_queue_tail(&wh->wh_msgq, skb);
    spin_unlock_irqrestore(&wh->wh_spinlock, flags);

    wsupp_helper_sched_work(&wh->wh_work);
}

int
atd_tgt_wsupp_helper_wmi_send(wsupp_handle_t wsupp_handle,
        int cmdid, char *data, int datalen)
{
    struct sk_buff *skb;
    atd_wsupp_message_t *msg;
    wsupp_helper_t *wh = (wsupp_helper_t *) wsupp_handle;

    wh_assert(wh);

    skb = alloc_skb(sizeof(atd_wsupp_message_t) + datalen + 1, 
            GFP_ATOMIC);
    if (!skb)
        return -1;

    msg = (atd_wsupp_message_t *) skb_put(skb, 
            sizeof(atd_wsupp_message_t) + datalen);
    msg->len = datalen;
    memcpy(msg->data, data, datalen);
   
    /* reuse the htc path */
    wh_htc_rx(wh, skb, 0);

    return 0;
}


