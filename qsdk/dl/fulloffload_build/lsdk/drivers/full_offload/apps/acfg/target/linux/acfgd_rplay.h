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

typedef void (*acfg_rplay_fn_entry)( void *cmd_buf, 
                                	a_uint32_t  size, 
                                    a_bool_t	resp_required);
	
#define acfg_err_log(x...)           syslog(LOG_ERR, x)
#define acfg_dbg_log(x...)           syslog(LOG_DEBUG, x)

/** 
 * @brief Receive Command Request from Host
 * 
 * @param 
 */
void
acfg_cmgr_recv_cmd_remote(void *cmd_buf);

/** 
 * @brief                VLAN Group Create
 * 
 * @param cmd_buf 	     points to received command  
 * @param size	         points to size of command data 
 * @param resp_required  1: resp required, 0: not required 
 */
void 
acfg_rplay_vlangrp_create(void    *cmd_buf, a_uint32_t  size, 
                         a_bool_t	resp_required);

/** 
 * @brief                Delete a VLAN Group
 * 
 * @param cmd_buf 	     points to received command  
 * @param size	         points to size of command data 
 * @param resp_required	 1: resp required, 0: not required 
 */
void 
acfg_rplay_vlangrp_delete(void    *cmd_buf, a_uint32_t  size, 
                         a_bool_t	resp_required);


/** 
 * @brief                Add VAP to a VLAN Group
 * 
 * @param cmd_buf 	     points to received command  
 * @param size	         points to size of command data 
 * @param resp_required  1: resp required, 0: not required 
 */
void 
acfg_rplay_vlangrp_addvap(void    *cmd_buf, a_uint32_t  size, 
                        a_bool_t	resp_required);

/** 
 * @brief                Delete VAP from a VLAN Group
 * 
 * @param cmd_buf 	     points to received command  
 * @param size	         points to size of command data 
 * @param resp_required	 1: resp required, 0: not required 
 */
void 
acfg_rplay_vlangrp_delvap(void    *cmd_buf, a_uint32_t  size, 
                        a_bool_t	resp_required);

#endif /* __BYP_RPLAY_H__ */

