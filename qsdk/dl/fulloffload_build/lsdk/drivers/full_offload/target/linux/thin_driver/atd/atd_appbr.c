/*
 ** Copyright (c) 2008-2010, Atheros Communications Inc.
 **
 ** Permission to use, copy, modify, and/or distribute this software for any
 ** purpose with or without fee is hereby granted, provided that the above
 ** copyright notice and this permission notice appear in all copies.
 **
 ** THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 ** WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 ** MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 ** ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 ** WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 ** ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 ** OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 **/

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
#include <atd_hdr.h>
#include <atd_cmd.h>
#include <atd_internal.h>
#include <atd_appbr.h>
#include <atd_byp.h>

static  atd_tgt_netlink_handle_t    appbr_nl_hdl    =   NULL;

/** 
 * @brief 
 * 
 * @param fn
 * @param app_id
 * 
 * @return 
 */
a_status_t
atd_tgt_appbr_register_cb(void * ctx, appbr_cb_t * cb, a_uint32_t app_id)
{

    appbr_sc_t * appbr_sc  = (appbr_sc_t *)ctx;

    if(cb->fn == NULL)
    {
        printk("atd_tgt_appbr_register_cb:Deregstering CBfor %d app\n",app_id);
    }

    appbr_sc->appbr_cb_list[app_id].fn   = cb->fn;
    appbr_sc->appbr_cb_list[app_id].ctx  = cb->ctx;

    return A_STATUS_OK;

}

/** 
 * @brief  Receive a Netlink Message and Send it as WMI command to host
 * 
 * @param ctx
 * @param netbuf
 * @param addr
 */
static void
atd_tgt_appbr_nlinput(void *ctx, struct sk_buff *skb, int addr)
{
    return;
}

/** 
 * @brief 
 * 
 * @param ctx   - ATD Handle
 * @param cmdid
 * @param buffer
 * @param len
 */
    void
atd_tgt_appbr_input(void *ctx, a_uint16_t cmdid,
        a_uint8_t *buffer, a_int32_t len)
{

    struct      sk_buff *skb;
    a_uint8_t   app_id, *buf;

    atd_appbr_hdr_t *appbr_hdr  =   (atd_appbr_hdr_t *) buffer;
    atd_tgt_dev_t   *atd_sc     =   (atd_tgt_dev_t *) ctx;

    appbr_sc_t      *appbr_sc   =   (appbr_sc_t *) atd_sc->appbr_handle;

    appbr_cb_t      *appbr_cb;

    a_uint32_t  msg_len, status = A_STATUS_OK;

    app_id      =   appbr_hdr->data[0] & ATD_HDR_APPID_MASK;

    appbr_dbg_printk("atd_tgt_acb_generic:got msg from app %d \n", app_id);

    appbr_cb    =   (appbr_cb_t *) &appbr_sc->appbr_cb_list[app_id];

    /** 
     * Pull the appbr Header
     **/
    buf         =   buffer  + sizeof(atd_appbr_hdr_t);
    msg_len     =   len     - sizeof(atd_appbr_hdr_t);

    /** 
     * Allocate and Populate a SKB
     **/
    skb         =   atd_tgt_netlink_alloc(msg_len);

    if (!skb)
        goto out;

    skb_put(skb, msg_len);

    memcpy(skb->data, buf, msg_len);

    if(appbr_cb->fn)
    {
        appbr_dbg_printk("atd_tgt_acb_generic:invoking callback \n");
        status  =   appbr_cb->fn(appbr_cb->ctx, skb);
    }
    else
    {
        /* Fall Through */
    }

    if(status == A_STATUS_FAILED)
    {
        printk("atd_tgt_acb_generic:callback returned error status\n");
        goto out;
    }


    /** 
     * Send SKB to User App
     **/
    atd_tgt_netlink_unicast(appbr_sc->netlink_handle, skb, 
                            APPBR_APPID_TO_MSGADDR(app_id));

    wmi_cmd_rsp(atd_sc->wmi_handle, cmdid, status, NULL, 0);

out:
    return;
}


appbr_handle_t
atd_tgt_appbr_init(void *atd_handle,a_uint32_t atd_netlink_num)
{
    appbr_sc_t  *appbr_sc;

    appbr_sc = kzalloc(sizeof(appbr_sc_t), GFP_KERNEL);

    memset(appbr_sc, 0 , sizeof(appbr_sc_t));

    if(appbr_sc == NULL) {
        printk("APPBR device not created\n");
        return NULL;
    }

    appbr_sc->atd_handle     =   atd_handle;

    if(!appbr_nl_hdl)
    {
        appbr_nl_hdl    =  atd_tgt_netlink_create(atd_tgt_appbr_nlinput, 
                (void *) appbr_sc, atd_netlink_num, 0);

        if (appbr_nl_hdl == NULL) {
            kfree(appbr_sc);
            printk("ATD:no netlink interface created\n");
            return NULL;
        }

        appbr_dbg_printk("ATD Netlink if number created: %d\n", atd_netlink_num);
    }

    appbr_sc->netlink_handle =  appbr_nl_hdl;

    return (appbr_sc);
}

/** 
 * @brief 
 * 
 * @param ctx
 */
    void
atd_tgt_appbr_cleanup(void *ctx)
{
    /* typecast just in case any other resources has to be freed up*/
    appbr_sc_t  *appbr_sc   =   (appbr_sc_t  *) ctx;

    atd_tgt_netlink_delete(appbr_sc->netlink_handle);

    kfree(appbr_sc);
}
