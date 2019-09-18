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

#ifndef __BYP_RPLAY_H__

#define __BYP_RPLAY_H__

#include <syslog.h>

typedef void (*byp_rplay_fn_entry)( void *cmd_buf, 
                                	a_uint32_t  size, 
                                	a_bool_t	ack_required, 
                                    a_bool_t	resp_required);
	
#define byp_err_log(x...)           syslog(LOG_ERR, x)
#define byp_dbg_log(x...)           syslog(LOG_DEBUG, x)
/** 
 * @brief Receive Command Request from Host
 * 
 * @param 
 */
void
byp_cmgr_recv_cmd_remote(void *cmd_buf, char *hst_if);

/** 
 * @brief                Set LAN/Bridge Access for a VAP
 * 
 * @param cmd_buf 	     points to received command  
 * @param size	         points to size of command data 
 * @param ack_required   1: ack required, 0: not required 
 * @param resp_required  1: resp required, 0: not required 
 */
void 
byp_rplay_vap_ctrl(void    *cmd_buf, a_uint32_t  size, 
                   a_bool_t	ack_required, a_bool_t	resp_required);

/** 
 * @brief                Bridge Create
 * 
 * @param cmd_buf 	     points to received command  
 * @param size	         points to size of command data 
 * @param ack_required   1: ack required, 0: not required 
 * @param resp_required  1: resp required, 0: not required 
 */
void 
byp_rplay_bridge_create(void    *cmd_buf, a_uint32_t  size, 
                        a_bool_t	ack_required, a_bool_t	resp_required);

/** 
 * @brief                Delete Bridge
 * 
 * @param cmd_buf 	     points to received command  
 * @param size	         points to size of command data 
 * @param ack_required   1: ack required, 0: not required 
 * @param resp_required	 1: resp required, 0: not required 
 */
void 
byp_rplay_bridge_delete(void    *cmd_buf, a_uint32_t  size, 
                        a_bool_t	ack_required, a_bool_t	resp_required);

/** 
 * @brief                VLAN Interface Create
 * 
 * @param cmd_buf 	     points to received command  
 * @param size	         points to size of command data 
 * @param ack_required   1: ack required, 0: not required 
 * @param resp_required  1: resp required, 0: not required 
 */
void 
byp_rplay_vlan_create(void    *cmd_buf, a_uint32_t  size, 
                        a_bool_t	ack_required, a_bool_t	resp_required);

/** 
 * @brief                Delete a VLAN Device
 * 
 * @param cmd_buf 	     points to received command  
 * @param size	         points to size of command data 
 * @param ack_required   1: ack required, 0: not required 
 * @param resp_required	 1: resp required, 0: not required 
 */
void 
byp_rplay_vlan_delete(void    *cmd_buf, a_uint32_t  size, 
                        a_bool_t	ack_required, a_bool_t	resp_required);

/** 
 * @brief               Add Host MAC Address 
 * 
 * @param cmd_buf	    points to received command  
 * @param size	        points to size of command data
 * @param ack_required	1: ack required, 0: not required 
 * @param resp_required	1: resp required, 0: not required
 */
void 
byp_rplay_filter_addmac(void    *cmd_buf, a_uint32_t  size, 
                        a_bool_t	ack_required, a_bool_t	resp_required);

/** 
 * @brief               Remove Host MAC Address 
 * 
 * @param cmd_buf	    points to received command  
 * @param size	        points to size of command data
 * @param ack_required	1: ack required, 0: not required 
 * @param resp_required	1: resp required, 0: not required
 */
void 
byp_rplay_filter_delmac(void    *cmd_buf, a_uint32_t  size, 
                        a_bool_t	ack_required, a_bool_t	resp_required);

/** 
 * @brief               Bring a Bridge Interface Up 
 * 
 * @param cmd_buf	    points to received command  
 * @param size	        points to size of command data
 * @param ack_required	1: ack required, 0: not required 
 * @param resp_required	1: resp required, 0: not required
 */
void 
byp_rplay_brif_up(void    *cmd_buf, a_uint32_t  size, 
                        a_bool_t	ack_required, a_bool_t	resp_required);

/** 
 * @brief               Bring Bridge Interface Down 
 * 
 * @param cmd_buf	    points to received command  
 * @param size	        points to size of command data
 * @param ack_required	1: ack required, 0: not required 
 * @param resp_required	1: resp required, 0: not required
 */
void 
byp_rplay_brif_down(void    *cmd_buf, a_uint32_t  size, 
                        a_bool_t	ack_required, a_bool_t	resp_required);
/** 
 * @brief               LAN Bypass Enable 
 * 
 * @param cmd_buf	    points to received command  
 * @param size	        points to size of command data
 * @param ack_required	1: ack required, 0: not required 
 * @param resp_required	1: resp required, 0: not required
 */
void 
byp_rplay_bypass_enable(void    *cmd_buf, a_uint32_t  size, 
                        a_bool_t	ack_required, a_bool_t	resp_required);

/** 
 * @brief                VLAN Group Create
 * 
 * @param cmd_buf 	     points to received command  
 * @param size	         points to size of command data 
 * @param ack_required   1: ack required, 0: not required 
 * @param resp_required  1: resp required, 0: not required 
 */
void 
byp_rplay_vlangrp_create(void    *cmd_buf, a_uint32_t  size, 
                        a_bool_t	ack_required, a_bool_t	resp_required);

/** 
 * @brief                Delete a VLAN Group
 * 
 * @param cmd_buf 	     points to received command  
 * @param size	         points to size of command data 
 * @param ack_required   1: ack required, 0: not required 
 * @param resp_required	 1: resp required, 0: not required 
 */
void 
byp_rplay_vlangrp_delete(void    *cmd_buf, a_uint32_t  size, 
                        a_bool_t	ack_required, a_bool_t	resp_required);


/** 
 * @brief                Add VAP to a VLAN Group
 * 
 * @param cmd_buf 	     points to received command  
 * @param size	         points to size of command data 
 * @param ack_required   1: ack required, 0: not required 
 * @param resp_required  1: resp required, 0: not required 
 */
void 
byp_rplay_vlangrp_addvap(void    *cmd_buf, a_uint32_t  size, 
                        a_bool_t	ack_required, a_bool_t	resp_required);

/** 
 * @brief                Delete VAP from a VLAN Group
 * 
 * @param cmd_buf 	     points to received command  
 * @param size	         points to size of command data 
 * @param ack_required   1: ack required, 0: not required 
 * @param resp_required	 1: resp required, 0: not required 
 */
void 
byp_rplay_vlangrp_delvap(void    *cmd_buf, a_uint32_t  size, 
                        a_bool_t	ack_required, a_bool_t	resp_required);

#endif /* __BYP_RPLAY_H__ */

