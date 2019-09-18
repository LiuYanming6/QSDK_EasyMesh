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


/**
 * @brief   linux netlink callback function which hides the buffer 
 *          manipulation and provides user context for the registered
 *          user callback
 */
static void 
__atd_tgt_netlink_input(struct sk_buff *skb)
{
    struct sock *sock = skb->sk;
    struct nlmsghdr *nlh = nlmsg_hdr(skb);
    atd_tgt_netlink_softc_t *nlsc;

    if (sock == NULL || sock->sk_user_data == NULL) {
        nldebug("%s: failed to get ctx\n", __func__);
        return;
    }
    
    if (skb->len < NLMSG_SPACE(0)) {
        nldebug("%s: invalid packet len\n", __func__);
        return;
    }

    skb_pull(skb, NLMSG_SPACE(0));

    nlsc = (atd_tgt_netlink_softc_t *) sock->sk_user_data;
    nlsc->input(nlsc->ctx, skb, nlh->nlmsg_pid);
}


/**
 * @brief   linux implementation for netlink interface creation
 */
 atd_tgt_netlink_handle_t
atd_tgt_netlink_create(atd_tgt_netlink_cb_t input, void *ctx, a_uint32_t unit, a_uint32_t groups)
{
    atd_tgt_netlink_softc_t *nlsc;

    nlsc = (atd_tgt_netlink_softc_t *) kzalloc(sizeof(atd_tgt_netlink_softc_t), 
                GFP_ATOMIC);
    if (!nlsc)
        return NULL;

    /* FIXME: compatible with earlier kernel version */
    nlsc->sock = netlink_kernel_create(&init_net, unit, groups, 
               __atd_tgt_netlink_input, NULL, THIS_MODULE);
    if (nlsc->sock == NULL) {
        kfree(nlsc);
        return NULL;
    }

    nlsc->input = input;
    nlsc->ctx = ctx;

    nlsc->sock->sk_user_data = nlsc;

    return (atd_tgt_netlink_handle_t) nlsc;
}


/**
 * @brief   linux implementation for netlink interface removal
 */
 void
atd_tgt_netlink_delete(atd_tgt_netlink_handle_t nlhdl)
{
    atd_tgt_netlink_softc_t *nlsc = (atd_tgt_netlink_softc_t *) nlhdl;

    netlink_kernel_release(nlsc->sock);
    
    kfree(nlsc);
}


/**
 * @brief   allocate necessary space for network buffer which contains
 *          netlink message and user data
 */
 struct sk_buff *
atd_tgt_netlink_alloc(a_uint32_t len)
{
    struct sk_buff *skb = NULL;
    skb = alloc_skb(NLMSG_SPACE(len), GFP_ATOMIC);
    if(skb == NULL) {
        return NULL;
    }

    skb_reserve(skb, NLMSG_SPACE(0));

    return skb;
}


/**
 * @brief   internal function which prepare netlink message in linux
 */
static void
atd_tgt_netlink_prepare(struct sk_buff *skb, 
                      int addr,
                      int groups)
{
    int datalen;
    struct nlmsghdr *nlh = NULL;

    datalen = skb->len;
    nlh = (struct nlmsghdr *) skb_push(skb, NLMSG_SPACE(0));
    nlh->nlmsg_pid = addr;
    nlh->nlmsg_flags = 0;
    nlh->nlmsg_len = NLMSG_LENGTH(datalen);

    //NETLINK_CB(skb).pid = 0;
    //NETLINK_CB(skb).dst_pid = addr;
    NETLINK_CB(skb).dst_group = groups;
}

/**
 * @brief   linux implementation for sending netlink broadcast message
 */
a_status_t
atd_tgt_netlink_broadcast(atd_tgt_netlink_handle_t nlhdl, 
                      struct sk_buff *skb, 
                      int groups)
{
    int ret;
    atd_tgt_netlink_softc_t *nlsc = (atd_tgt_netlink_softc_t *) nlhdl;

    atd_tgt_netlink_prepare(skb, 0, groups);

    ret = netlink_broadcast(nlsc->sock, skb, 0, groups, GFP_ATOMIC);

    if (ret)
        return A_STATUS_FAILED;
    return ret;
}


/**
 ** @brief   linux implementation for sending netlink unicast message
 *  
 */
a_status_t
atd_tgt_netlink_unicast(atd_tgt_netlink_handle_t nlhdl,
        struct sk_buff *skb,
        int addr)
{
    int ret;
    atd_tgt_netlink_softc_t *nlsc = (atd_tgt_netlink_softc_t *) nlhdl;

    atd_tgt_netlink_prepare(skb, addr, 0);

    ret = netlink_unicast(nlsc->sock, skb, addr, 1);

    if (ret)
        return A_STATUS_FAILED;
    return ret;
}

