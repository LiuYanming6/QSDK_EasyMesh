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


#ifndef     __ATD_TGT_BYP_H
#define     __ATD_TGT_BYP_H

#include <atd_trc.h>
#include <bypass_types.h>

enum atd_lanbyp_mode {
    ATD_LANBYP_DIS  = 0,  
    ATD_LANBYP_TUNNEL, 
    ATD_LANBYP_VLAN,
};

#define     MAX_HOST_ADDR   32

struct atd_tgt_dev;
struct atd_tgt_vap;

typedef struct sk_buff * (*byp_filter_fn_t) (void *ctx, 
                                struct atd_tgt_vap *vap, struct sk_buff *skb);

/* Disabling all if-checks for optimization */
#define byp_dbg_printk(x...)        
/* #define byp_dbg_printk(x...)      atd_trace(ATD_DEBUG_FUNCTRACE, (x))      */
/* #define byp_dbg_printk(x...)          printk(x)    */




/** 
 ** @brief  Host Bridge MAC Addresses
 **/
typedef struct host_br_addr_s {
    a_uint32_t      vld_mask;                             /**< Vld Bit mask */
    a_uint8_t       addr[MAX_HOST_ADDR][ACFG_MACADDR_LEN];/**< MAC Addresses */
} host_br_addr_t;


/** 
 * @brief Bypass Module Softc
 */
typedef struct byp_sc_s {
    struct atd_tgt_dev      *atd_handle;    /**< Parent ATD Handle */
    a_uint8_t               lan_bypass_mode;
    byp_filter_fn_t         filter_fn;      /**< Packet Filter for
                                             detecting LAN packets */
    appbr_cb_t              *cb;            /**< Application Bridge Callback*/
    host_br_addr_t          *host_addr_list;
} byp_sc_t;

typedef void * byp_handle_t;


#if (OFFLOAD_LAN_BYPASS == 1)

#define     BYPSC_ATDHDL(x)      ((x)->atd_handle)   
#define     BYPSC_BYPMODE(x)     ((x)->lan_bypass_mode)   
#define     BYPSC_CB(x)          ((x)->cb)
#define     BYPSC_HSTADDR_TBL(x) ((x)->host_addr_list)  
#define     BYPSC_FILTER(x)      ((x)->filter_fn)

/** 
 * @brief Init Routine for Bypass Module. Returns the module softc pointer  
 * 
 * @param atd_handle
 * 
 * @return 
 */
byp_handle_t
atd_tgt_byp_init(void *atd_handle);

/** 
 * @brief     ApplicationBridge Callback for Bypass Application
 * 
 * @param ctx
 * @param skb
 * 
 * @return 
 */
a_status_t
atd_tgt_byp_cb(void * ctx, struct sk_buff *skb);

/** 
 * @brief   Cleanup routine for Bypass Module 
 * 
 * @param ctx
 * 
 * @return 
 */
void
atd_tgt_byp_cleanup(void *ctx);

/** 
 * @brief Rx MAC Addr Filter to check if the Packet 
 * should be sent to Host over ATD
 * 
 * @param ctx
 * @param vap
 * @param skb
 * 
 * @return 
 */
a_uint32_t atd_tgt_byp_for_host(void *ctx, struct atd_tgt_vap   *vap, 
                                struct sk_buff *skb);

/** 
 * @brief Check Bypass Enable
 * 
 * @return 1, if LAN Bypass Mode is enabled, 
 *         0, if disabled 
 */
a_uint32_t atd_tgt_byp_mode(void *ctx);

/** 
 * @brief Is this Ethernet Packet a Multicast 
 * 
 * @param skb
 * 
 * @return 
 */
#define         atd_tgt_byp_is_mcast(skb)         (((skb)->data[0]) & 0x1)

#else /** OFFLOAD_LAN_BYPASS */

#define         atd_tgt_byp_init(x)             (x)

#define         atd_tgt_byp_cb(x, y)            (A_STATUS_OK)

#define         atd_tgt_byp_cleanup(x)          do {} while(0)

#define         atd_tgt_byp_for_host(x)         (1)

#define         atd_tgt_byp_mode(x)             (ATD_LANBYP_DIS)

#define         atd_tgt_byp_is_mcast(x)         (0)

#define         BYPSC_FILTER(x)                 (NULL) 

#endif /** OFFLOAD_LAN_BYPASS */

#endif
