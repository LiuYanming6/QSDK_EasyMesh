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
#include <string.h>
#include <stdlib.h>

#include "linux/ac_shims.h"

#include <sys/socket.h>
#include <linux/socket.h>
#include <linux/netlink.h>
#include <linux/if.h>

#include <appbr_if.h>
#include <acfgd_rplay.h>

/*
 * Macro definitions
 */
#define LAN_BR_NAME     "br0" 
#define LAN_IF_NAME     "eth0" 
#define VLAN_BR_PREFIX  "vlbr"

#define CMD_VLAN_CREATE         "vconfig add "
#define CMD_VLAN_DELETE         "vconfig rem "
#define CMD_BRIDGE_CREATE       "brctl addbr " 
#define CMD_BRIDGE_DELETE       "brctl delbr " 
#define CMD_BRIDGE_ADDIF        "brctl addif " 
#define CMD_BRIDGE_REMIF        "brctl delif " 
#define CMD_BRIDGE_SETFD        "brctl setfd " 
#define CMD_IF_CONFIG           "ifconfig " 


/*
 * Local variables
 */
static char cmd_name[ACFG_SZ_REPLAYCMD] = "";

/** 
 * @brief Replay Functions
 */
acfg_rplay_fn_entry acfg_rplay_fn_list[] = {
	acfg_rplay_vlangrp_create,
	acfg_rplay_vlangrp_delete,
	acfg_rplay_vlangrp_addvap,
	acfg_rplay_vlangrp_delvap,
};


/** 
 * @brief Receive Command Requests from Host
 * 
 * @param recv_buf  Receive Buffer
 */
void 
acfg_cmgr_recv_cmd_remote(void *recv_buf)
{
	a_uint32_t  cmd_id          = 0,
                size            = 0;

    a_bool_t    resp_required   = 0;

    char * cmd_buf;

    cmd_buf = (char *) recv_buf + NLMSG_SPACE(0);
	
	struct acfg_dl_cfg_hdr *cmd_hdr = (struct acfg_dl_cfg_hdr *) cmd_buf;

	/** we only accept command requests from host */
	if ((cmd_hdr->data[0] & ACFG_CMD_REQ_MASK) != ACFG_CMD_REQ)
		return;
	
	/** check that command id is valid */
	if (cmd_hdr->data[1] >= ACFG_CMD_LAST)
		return;
	else
		cmd_id = cmd_hdr->data[1];

#if ACFG_ACK_RESP_SUPPORTED
	if(cmd_hdr->data[0] & ACFG_RESP_REQD)
		resp_required = 1;
#endif
	
	/** obtain the size information */	
	size = *(a_uint16_t *)&cmd_hdr->data[2];	

	/** call registered command handler */
	(*acfg_rplay_fn_list[cmd_id])(cmd_buf, size, resp_required);
	return;
} 

/** 
 * @brief Send response back to host
 * 
 * @param cmd_id
 * @param cmd_buf
 * @param size
 * @param ack_status
 */
static void 
acfg_cmgr_send_rsp_remote(
	a_uint8_t   cmd_id, 
	void        *cmd_buf, 
	a_uint16_t  size, 
	a_uint32_t  ack_status
	)
{
	struct acfg_dl_cfg_hdr *cmd_hdr = (struct acfg_dl_cfg_hdr *) cmd_buf;
	
	/** fill command characteristics */
	cmd_hdr->data[0] |= ACFG_CMD_RESP;
	
	if(ack_status == ACFG_RPLAYCMD_SUCCESS)
		cmd_hdr->data[0] |= ACFG_ACK_SUCCESS;
	else
		cmd_hdr->data[0] |= ACFG_ACK_FAILURE;
	
	cmd_hdr->data[1] = cmd_id;
	
	*((a_uint16_t *)&cmd_hdr->data[2]) = (size);
	
	/** send command to remote end via APPBR */
	appbr_if_send_cmd_remote (APPBR_ACFG, cmd_buf, size);
    
	return;
} 


void 
acfg_rplay_vlangrp_create(
	void        *cmd_buf,		
	a_uint32_t  size, 		
	a_bool_t	resp_required
	)
{
    struct acfg_vlangrp_info *vlan_cmd_req = (struct acfg_vlangrp_info *)cmd_buf;
    static struct acfg_dl_cfg_hdr acfg_cmd_resp;
    int err_exec;

    /** create a replay command */
    strcpy(cmd_name, CMD_VLAN_CREATE);
    strcat(cmd_name, LAN_IF_NAME);
    strcat(cmd_name, " ");
    strcat(cmd_name, vlan_cmd_req->vlan_id);
    strcat(cmd_name, " ;");
    strcat(cmd_name, CMD_IF_CONFIG);
    strcat(cmd_name, LAN_IF_NAME);
    strcat(cmd_name, ".");
    strcat(cmd_name, vlan_cmd_req->vlan_id);
    strcat(cmd_name, " up;");
    strcat(cmd_name, CMD_BRIDGE_CREATE);
  	strcat(cmd_name, VLAN_BR_PREFIX);
    strcat(cmd_name, vlan_cmd_req->vlan_id);
    strcat(cmd_name, " ;");
    strcat(cmd_name, CMD_BRIDGE_SETFD);
  	strcat(cmd_name, VLAN_BR_PREFIX);
    strcat(cmd_name, vlan_cmd_req->vlan_id);
    strcat(cmd_name, " 0");
    strcat(cmd_name, " ;");
    strcat(cmd_name, CMD_BRIDGE_ADDIF);
  	strcat(cmd_name, VLAN_BR_PREFIX);
    strcat(cmd_name, vlan_cmd_req->vlan_id);
    strcat(cmd_name, " ");
    strcat(cmd_name, LAN_IF_NAME);
    strcat(cmd_name, ".");
    strcat(cmd_name, vlan_cmd_req->vlan_id);
    strcat(cmd_name, ";");
    strcat(cmd_name, CMD_IF_CONFIG);
  	strcat(cmd_name, VLAN_BR_PREFIX);
    strcat(cmd_name, vlan_cmd_req->vlan_id);
    strcat(cmd_name, " up");


    /** execute a replay command */
    err_exec = system(cmd_name);

}

void 
acfg_rplay_vlangrp_delete(
	void        *cmd_buf,		
	a_uint32_t  size, 		
	a_bool_t	resp_required
	)
{
    struct acfg_vlangrp_info *vlan_cmd_req = (struct acfg_vlangrp_info *)cmd_buf;

    static struct acfg_dl_cfg_hdr acfg_cmd_resp;
    int err_exec;

    /** create a replay command */
    strcpy(cmd_name, CMD_IF_CONFIG);
  	strcat(cmd_name, VLAN_BR_PREFIX);
    strcat(cmd_name, vlan_cmd_req->vlan_id);
    strcat(cmd_name, " down");
    strcat(cmd_name, CMD_BRIDGE_DELETE);
  	strcat(cmd_name, VLAN_BR_PREFIX);
    strcat(cmd_name, vlan_cmd_req->vlan_id);
    strcat(cmd_name, " ;");
    strcat(cmd_name, CMD_VLAN_DELETE);
    strcat(cmd_name, LAN_IF_NAME);
    strcat(cmd_name, ".");
    strcat(cmd_name, vlan_cmd_req->vlan_id);

    /** execute a replay command */
    err_exec = system(cmd_name);

}

void 
acfg_rplay_vlangrp_addvap(
	void        *cmd_buf,		
	a_uint32_t  size, 		
	a_bool_t	resp_required
	)
{
    struct acfg_vlangrp_info *vlan_cmd_req = (struct acfg_vlangrp_info *)cmd_buf;
    static struct acfg_dl_cfg_hdr acfg_cmd_resp;
    int err_exec;

    /** create a replay command */
    strcpy(cmd_name, CMD_BRIDGE_ADDIF);
  	strcat(cmd_name, VLAN_BR_PREFIX);
    strcat(cmd_name, vlan_cmd_req->vlan_id);
  	strcat(cmd_name, " ");
    strcat(cmd_name, vlan_cmd_req->if_name);


    /** execute a replay command */
    err_exec = system(cmd_name);

}

void 
acfg_rplay_vlangrp_delvap(
	void        *cmd_buf,		
	a_uint32_t  size, 		
	a_bool_t	resp_required
	)
{
    struct acfg_vlangrp_info *vlan_cmd_req = (struct acfg_vlangrp_info *)cmd_buf;
    static struct acfg_dl_cfg_hdr acfg_cmd_resp;
    int err_exec;

    /** create a replay command */
    strcpy(cmd_name, CMD_BRIDGE_REMIF);
  	strcat(cmd_name, VLAN_BR_PREFIX);
    strcat(cmd_name, vlan_cmd_req->vlan_id);
  	strcat(cmd_name, " ");
    strcat(cmd_name, vlan_cmd_req->if_name);


    /** execute a replay command */
    err_exec = system(cmd_name);

}
