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

#include "atd_internal.h"
#include <atd_appbr.h>

static adf_netlink_handle_t     appbr_nl_handle =   NULL;

/** 
 * @brief 
 * 
 * @param fn
 * @param app_id
 * 
 * @return 
 */
a_status_t
atd_appbr_register_cb(void * ctx, appbr_cb_t * cb, a_uint32_t app_id)
{

    appbr_sc_t * appbr_sc  = (appbr_sc_t *)ctx;

    if(ctx  == NULL)
        return A_STATUS_FAILED;

    if(cb->fn == NULL)
    {
        appbr_dbg_print("Deregistering callback for %d app \n", app_id);
    }

    /** There can be a race condition if the callback is being updated when 
     * callback function is just being invoked from another context. 
     * To avoid , a protection mechanism can be added in form of a mutex.
     *
     * eg:
     * cb->mutex = appbr_sc->cb_list[app_id].mutex
     * Use lock(cb->mutex), release(cb->mutex) for mutual exclusion.
     **/

    appbr_sc->cb_list[app_id].fn   = cb->fn;
    appbr_sc->cb_list[app_id].ctx  = cb->ctx;

    return A_STATUS_OK;

}

/** 
 * @brief  Receive a Netlink Message and Send it as WMI command to Target
 * 
 * @param ctx
 * @param netbuf
 * @param addr
 */
static void
atd_appbr_netlink_input(void *ctx, adf_nbuf_t netbuf, adf_netlink_addr_t addr)
{

    a_uint32_t  netbuf_len, len;

    a_uint8_t   *netbuf_data, *buf;

    a_uint32_t  app_id, status = A_STATUS_OK;

    adf_nbuf_t  wmi_buf;

    atd_appbr_hdr_t * appbr_hdr;

    appbr_cb_t * appbr_cb;

    appbr_sc_t * appbr_sc     =   (appbr_sc_t *)ctx;
    atd_host_dev_t * atd_sc   =   (atd_host_dev_t *) appbr_sc->atd_handle;

    appbr_dbg_print("atd_netlink_input netlink addr = %x\n", addr);

    app_id     =    APPBR_MSGADDR_TO_APPID(addr);

    appbr_cb   =    (appbr_cb_t *) &appbr_sc->cb_list[app_id];

    /** Allocate a Message SKB for WMI */
    if ((wmi_buf = _atd_wmi_msg_alloc(atd_sc->wmi_handle)) == NULL) 
    {
        adf_os_print("Unable to allocate wmi buffer \n");
        status = A_STATUS_ENOMEM;
        goto fail ;
    }

    /** Get length and Data pointer from netbuf */
    adf_nbuf_peek_header(netbuf, &netbuf_data, &netbuf_len);

    len                 =   netbuf_len + sizeof(atd_appbr_hdr_t);

    /** Add APPBR Header (App ID) to WMI SKB, and append with data from 
     *  recieved netbuf */
    buf                 =   adf_nbuf_put_tail(wmi_buf, len);
    appbr_hdr           =   (atd_appbr_hdr_t *) buf;
    buf                 +=  sizeof(atd_appbr_hdr_t);

    appbr_hdr->data[0]  =   app_id;

    adf_os_mem_copy(buf, netbuf_data, netbuf_len);

    /** 
     * Invoke the registered application-specific callback handler
     */
    if(appbr_cb->fn)
    {
        status  =   appbr_cb->fn(appbr_cb->ctx, netbuf);
    }
    else
    {
        /* Fall Through */
    }

    if(status == A_STATUS_FAILED)
    {
        adf_os_print("appbr callback returned fail status \n");
        goto    fail;
    }


    appbr_dbg_print("atd_netlink_input :sending wmi_cmd \n");
    /* Transmit to Remote Interface via WMI */
    /** TODO Do we need Response buffer?? */

    status  =   wmi_cmd_send(atd_sc->wmi_handle, WMI_CMD_APPBR, wmi_buf, \
            NULL, 0);

fail:
    return;
}

    void
atd_appbr_netlink_output(void *ctx, adf_nbuf_t netbuf, adf_netlink_addr_t addr)
{

    adf_nbuf_t nlbuf;
    a_uint32_t netbuf_len, status;
    a_uint8_t *netbuf_data, *nldata;
    appbr_sc_t * appbr_sc = (appbr_sc_t *)ctx;

    adf_nbuf_peek_header(netbuf, &netbuf_data, &netbuf_len);

    netbuf_len  =   adf_os_ntohs(netbuf_len);
    nlbuf       =   adf_netlink_alloc(netbuf_len);

    if (!nlbuf) {
        adf_os_print("%s: failed to alloc nbuf\n", adf_os_function);
        goto fail;
    }

    nldata  =   adf_nbuf_put_tail(nlbuf, netbuf_len);
    adf_os_mem_copy(nldata, netbuf_data, netbuf_len);

    status  =   adf_netlink_unicast(appbr_sc->netlink_handle, netbuf, addr);

    if(status)
    {
        adf_os_print("%s: failed to send netlink msg\n", adf_os_function);
    }

fail:
    dev_kfree_skb_any(netbuf);

}
/** 
 * @brief Initialize Netlink interface for ATD APPBR
 * 
 * @param ctx
 * 
 * @return 
 */
appbr_handle_t
atd_appbr_init(void * atd_handle, a_uint32_t atd_netlink_num)
{
    appbr_sc_t  *   appbr_sc;

    appbr_sc = adf_os_mem_alloc(NULL,sizeof(appbr_sc_t));

    if(appbr_sc == NULL) {
        adf_os_print("APPBR device not created\n");
        return NULL;
    }

    appbr_sc->atd_handle        = atd_handle;

    if(!appbr_nl_handle)
    {
        appbr_nl_handle   =   adf_netlink_create(atd_appbr_netlink_input,
                appbr_sc,atd_netlink_num, 0);

        if (appbr_nl_handle == NULL) {
            adf_os_mem_free(appbr_sc);
            adf_os_print("ATD APPBR:no netlink interface created\n");
            return NULL;
        }

        adf_os_print("ATD APPBR Netlink iface number created: %d\n", 
                     atd_netlink_num);
    }

    appbr_sc->netlink_handle    =   appbr_nl_handle;

    return (appbr_sc);
}


/** 
 * @brief Free up all the resources allocated by App Br module
 * 
 * @param ctx
 */
void
atd_appbr_cleanup(void *ctx) 
{

    appbr_sc_t  *appbr_sc   =   (appbr_sc_t  *) ctx;

    if(appbr_sc == NULL)
       return;

    if(appbr_sc->netlink_handle)
        adf_netlink_delete(appbr_sc->netlink_handle);

    appbr_nl_handle = NULL;
    adf_os_print("APPBR Netlink iface deleted \n");

    adf_os_mem_free(appbr_sc);
}

