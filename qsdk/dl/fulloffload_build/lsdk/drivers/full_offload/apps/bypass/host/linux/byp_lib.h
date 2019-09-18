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

#ifndef __BYP_LIB_H__
#define __BYP_LIB_H__

#include <appbr_types.h>
#include <bypass_types.h>

#define BYP_LIB_CMD_NORMAL       0
#define BYP_LIB_CMD_RELIABLE     1

#define BYP_LIB_CMD_NO_RESP_REQ  0
#define BYP_LIB_CMD_RESP_REQ     1

typedef appbr_status_t (*byp_lib_fn)(char * args[], int argc, void *buf, 
                                     a_bool_t reliable, a_uint32_t timeout);

/** 
 * @brief  Set LAN/Bridge Access for a VAP
 * 
 * @param args[]
 * @param buf
 * @param reliable
 * @param timeout
 * 
 * @return 
 */
appbr_status_t
byp_lib_vap_ctrl_add(char * args[], int argc, void   *buf, a_bool_t   reliable, 
                a_uint32_t  timeout);

/** 
 * @brief Reset LAN/Bridge Access for a VAP
 * 
 * @param args[]
 * @param argc
 * @param buf
 * @param reliable
 * @param timeout
 * 
 * @return 
 */
appbr_status_t
byp_lib_vap_ctrl_del(char * args[], int argc, void   *buf, a_bool_t   reliable, 
                a_uint32_t  timeout);

/** 
 * @brief           Send Bridge Create Command to Target side ACT Application
 * 
 * @param args[]    command arguments  
 * @param buf       response buffer (optional)  
 * @param reliable  1: ack required
 * @param timeout   timeout for ack/ response 
 * 
 * @return          Command Status 
 */
appbr_status_t
byp_lib_bridge_create(char * args[], int argc, void   *buf, a_bool_t   reliable, 
                      a_uint32_t  timeout);

/** 
 * @brief           Send Bridge Delete Command to Target side ACT Application
 * 
 * @param args[]    command arguments  
 * @param buf       response buffer (optional)  
 * @param reliable  1: ack required
 * @param timeout   timeout for ack/ response 
 * 
 * @return          Command Status 
 */
appbr_status_t
byp_lib_bridge_delete(char * args[], int argc, void   *buf, a_bool_t   reliable, 
                     a_uint32_t  timeout);

/** 
 * @brief           Send VLAN Create Command to Target side BypassT Application
 * 
 * @param args[]    command arguments  
 * @param buf       response buffer (optional)  
 * @param reliable  1: ack required
 * @param timeout   timeout for ack/ response 
 * 
 * @return          Command Status 
 */
appbr_status_t
byp_lib_vlan_create(char * args[], int argc, void   *buf, a_bool_t   reliable, 
        a_uint32_t  timeout);

/** 
 * @brief           Send VLAN Delete Command to Target side BypassT Application
 * 
 * @param args[]    command arguments  
 * @param buf       response buffer (optional)  
 * @param reliable  1: ack required
 * @param timeout   timeout for ack/ response 
 * 
 * @return          Command Status 
 */
appbr_status_t
byp_lib_vlan_delete(char * args[], int argc, void   *buf, a_bool_t   reliable, 
        a_uint32_t  timeout);

/** 
 * @brief           Send "Add Host Bridge MAC Address" Command to Target side 
 *                  ACT Application
 * 
 * @param args[]    command arguments  
 * @param buf       response buffer (optional)  
 * @param reliable  1: ack required
 * @param timeout   timeout for ack/ response 
 * 
 * @return          Command Status 
 */
appbr_status_t
byp_lib_bridge_add_mac(char * args[], int argc, void   *buf, 
        a_bool_t   reliable, 
        a_uint32_t  timeout);

/** 
 * @brief 
 * 
 * @param args[]
 * @param buf
 * @param reliable
 * @param timeout
 * 
 * @return 
 */
appbr_status_t
byp_lib_bridge_add_hostif(char * args[], int argc, void   *buf, 
        a_bool_t   reliable, 
        a_uint32_t  timeout);

/** 
 * @brief 
 * 
 * @param args[]
 * @param buf
 * @param reliable
 * @param timeout
 * 
 * @return 
 */
appbr_status_t
byp_lib_bridge_del_hostif(char * args[], int argc, void   *buf, 
        a_bool_t   reliable, 
        a_uint32_t  timeout);

/** 
 * @brief           Send "Remove Host Bridge MAC Address"Command to Target side
 *                  ACT Application
 * 
 * @param args[]    command arguments  
 * @param buf       response buffer (optional)  
 * @param reliable  1: ack required
 * @param timeout   timeout for ack/ response 
 * 
 * @return          Command Status 
 */
appbr_status_t
byp_lib_bridge_del_mac(char * args[], int argc, void   *buf, a_bool_t   reliable, 
        a_uint32_t  timeout);

/** 
 * @brief   List the MAC addresses in Packet Filter Table
 * 
 * @param args[]
 * @param buf
 * @param reliable
 * @param timeout
 * 
 * @return 
 */
appbr_status_t
byp_lib_filter_listmac(char * args[], int argc, void   *buf, a_bool_t   reliable, 
        a_uint32_t  timeout);

/** 
 * @brief          
 * 
 * @param args[]    command arguments  
 * @param buf       response buffer (optional)  
 * @param reliable  1: ack required
 * @param timeout   timeout for ack/ response 
 * 
 * @return          Command Status 
 */
appbr_status_t
byp_lib_brif_up(char * args[], int argc, void   *buf, a_bool_t   reliable, 
        a_uint32_t  timeout);

/** 
 * @brief           Send "Bring Bridge Interface Down" Command to Target side 
 *                  ACT Application
 * 
 * @param args[]    command arguments  
 * @param buf       response buffer (optional)  
 * @param reliable  1: ack required
 * @param timeout   timeout for ack/ response 
 * 
 * @return          Command Status 
 */
appbr_status_t
byp_lib_brif_down(char * args[], int argc, void   *buf, a_bool_t   reliable, 
        a_uint32_t  timeout);

/** 
 * @brief           Send "LAN Bypass Enable/Disable" Command to Target side
 *                  ACT Application
 * 
 * @param args[]    command arguments  
 * @param buf       response buffer (optional)  
 * @param reliable  1: ack required
 * @param timeout   timeout for ack/ response 
 * 
 * @return          Command Status 
 */
appbr_status_t
byp_lib_bypass_enable(char * args[], int argc, void   *buf, a_bool_t   reliable, 
        a_uint32_t  timeout);

/** 
 * @brief           Send VLAN Group Create Command to Target side BypassT Application
 * 
 * @param args[]    command arguments  
 * @param buf       response buffer (optional)  
 * @param reliable  1: ack required
 * @param timeout   timeout for ack/ response 
 * 
 * @return          Command Status 
 */
appbr_status_t
byp_lib_vlangrp_create(char * args[], int argc, void   *buf, a_bool_t   reliable, 
        a_uint32_t  timeout);

/** 
 * @brief           Send VLAN Group Delete Command to Target side BypassT Application
 * 
 * @param args[]    command arguments  
 * @param buf       response buffer (optional)  
 * @param reliable  1: ack required
 * @param timeout   timeout for ack/ response 
 * 
 * @return          Command Status 
 */
appbr_status_t
byp_lib_vlangrp_delete(char * args[], int argc, void   *buf, a_bool_t   reliable, 
        a_uint32_t  timeout);

/** 
 * @brief           Send VLAN Group Add VAP Command to Target side BypassT Application
 * 
 * @param args[]    command arguments  
 * @param buf       response buffer (optional)  
 * @param reliable  1: ack required
 * @param timeout   timeout for ack/ response 
 * 
 * @return          Command Status 
 */
appbr_status_t
byp_lib_vlangrp_addvap(char * args[], int argc, void   *buf, a_bool_t   reliable, 
        a_uint32_t  timeout);

/** 
 * @brief           Send VLAN Group Del Vap Command to Target side BypassT Application
 * 
 * @param args[]    command arguments  
 * @param buf       response buffer (optional)  
 * @param reliable  1: ack required
 * @param timeout   timeout for ack/ response 
 * 
 * @return          Command Status 
 */
appbr_status_t
byp_lib_vlangrp_delvap(char * args[], int argc, void   *buf, a_bool_t   reliable, 
        a_uint32_t  timeout);

/** 
 * @brief Initialize appbr Interface
 * 
 * @return 
 */
appbr_status_t
byp_lib_open_appbr(void);

/** 
 * @brief  Close appbr Interface
 */
void 
byp_lib_close_appbr(void);

#endif /* __BYP_LIB_H__ */
