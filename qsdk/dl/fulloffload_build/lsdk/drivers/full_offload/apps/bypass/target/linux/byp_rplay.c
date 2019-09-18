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
#include <bypass_types.h>
#include <byp_rplay.h>

/*
 * Macro definitions
 */
#define LAN_BR_NAME     "br0" 
#define LAN_IF_NAME   hst_if 
//#define LAN_IF_NAME   "eth0"
#define VLAN_BR_PREFIX  "vlbr"

/*
 * Local variables
 */
static char cmd_name[BYP_SZ_REPLAYCMD] = "";
char *hst_if = "eth0";

/*------------------------------------------------------------------*/
/*
 * Tables
 */
const char *byp_rplay_cmd_list[] = {
	"dummy ",       
	"brctl addif ",  
	"dummy",
	"dummy",
	"dummy",
	"dummy",
	"brctl addif ",
	"brctl delif ",
	"vconfig add ",
	"vconfig rem ",
	"brctl addbr ",
	"brctl delbr ",
	"ifconfig ",
	"ifconfig ",
	NULL
};

/** 
 * @brief Replay Functions
 */
byp_rplay_fn_entry byp_rplay_fn_list[] = {
    byp_rplay_bypass_enable,
	byp_rplay_vap_ctrl,
	byp_rplay_filter_addmac,
	byp_rplay_filter_delmac,
	byp_rplay_vlangrp_create,
	byp_rplay_vlangrp_delete,
	byp_rplay_vlangrp_addvap,
	byp_rplay_vlangrp_delvap,
	byp_rplay_vlan_create,
	byp_rplay_vlan_delete,
	byp_rplay_bridge_create,
	byp_rplay_bridge_delete,
	byp_rplay_brif_up,
	byp_rplay_brif_down,
};


/** 
 * @brief Receive Command Requests from Host
 * 
 * @param recv_buf  Receive Buffer
 */
void 
byp_cmgr_recv_cmd_remote(void *recv_buf, char *h_if)
{
	a_uint32_t  cmd_id          = 0,
                size            = 0;

    a_bool_t    ack_required    = 0,
                resp_required   = 0;

    char * cmd_buf;

    cmd_buf = (char *) recv_buf + NLMSG_SPACE(0);
	
	struct byp_common *cmd_hdr = (struct byp_common *) cmd_buf;

	/** we only accept command requests from host */
	if ((cmd_hdr->data[0] & BYP_CMD_REQ_MASK) != BYP_CMD_REQ)
		return;
	
	/** check that command id is valid */
	if (cmd_hdr->data[1] >= BYP_CMD_LAST)
		return;
	else
		cmd_id = cmd_hdr->data[1];

    hst_if = h_if;

#if BYP_ACK_RESP_SUPPORTED
	if(cmd_hdr->data[0] & BYP_ACK_REQD)
		ack_required = 1;
		
	if(cmd_hdr->data[0] & BYP_RESP_REQD)
		resp_required = 1;
#endif
	
	/** obtain the size information */	
	size = *(a_uint16_t *)&cmd_hdr->data[2];	

	/** call registered command handler */
	(*byp_rplay_fn_list[cmd_id])(cmd_buf, size, ack_required, resp_required);
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
byp_cmgr_send_rsp_remote(
	a_uint8_t   cmd_id, 
	void        *cmd_buf, 
	a_uint16_t  size, 
	a_uint32_t  ack_status
	)
{
	struct byp_common *cmd_hdr = (struct byp_common *) cmd_buf;
	
	/** fill command characteristics */
	cmd_hdr->data[0] |= BYP_CMD_RESP;
	
	if(ack_status == BYP_RPLAYCMD_SUCCESS)
		cmd_hdr->data[0] |= BYP_ACK_SUCCESS;
	else
		cmd_hdr->data[0] |= BYP_ACK_FAILURE;
	
	cmd_hdr->data[1] = cmd_id;
	
	*((a_uint16_t *)&cmd_hdr->data[2]) = (size);
	
	/** send command to remote end via APPBR */
	appbr_if_send_cmd_remote (APPBR_BYP, cmd_buf, size);
    
	return;
} 


void 
byp_rplay_vap_ctrl(
	void        *cmd_buf,		
	a_uint32_t  size, 		
	a_bool_t	ack_required,
	a_bool_t	resp_required
	)
{
	struct byp_vapctrl *byp_cmd_req = (struct byp_vapctrl *)cmd_buf;
	static struct byp_common byp_cmd_resp;
    int err_exec;
	
	strcpy(cmd_name, "");

	/** create a replay command */
    if((byp_cmd_req->ctrl & BYP_VAPCTRL_SETMASK) == BYP_VAPCTRL_SET)
        strcat(cmd_name, "brctl addif ");
    else
        strcat(cmd_name, "brctl delif ");

    if((byp_cmd_req->ctrl & BYP_VAPCTRL_ACCMASK) == BYP_VAPCTRL_LAN_EN)
        strcat(cmd_name, LAN_BR_NAME);
    else
        strcat(cmd_name, byp_cmd_req->br_name);

	strcat(cmd_name, " ");
  	strcat(cmd_name, byp_cmd_req->if_name);

	/** execute a replay command */
	err_exec = system(cmd_name);
  
	if (ack_required)
	{
		memset(&byp_cmd_resp, 0, sizeof(struct byp_common));
		byp_cmgr_send_rsp_remote(BYP_CMD_VAPCTRL, &byp_cmd_resp, 
                                 sizeof(struct byp_common), err_exec);
	}
}

void 
byp_rplay_bridge_create(
	void        *cmd_buf,		
	a_uint32_t  size, 		
	a_bool_t	ack_required,
	a_bool_t	resp_required
	)
{
	struct byp_br *byp_cmd_req = (struct byp_br *)cmd_buf;
	static struct byp_common byp_cmd_resp;
    int err_exec;
	
	/** create a replay command */
	strcpy(cmd_name, byp_rplay_cmd_list[BYP_CMD_BRCREATE]);
  	strcat(cmd_name, byp_cmd_req->br_name);
    strcat(cmd_name, ";");
    strcat(cmd_name, "brctl setfd ");
  	strcat(cmd_name, byp_cmd_req->br_name);
    strcat(cmd_name, " 0;");
  
	/** execute a replay command */
	err_exec = system(cmd_name);
  
	if (ack_required)
	{
		memset(&byp_cmd_resp, 0, sizeof(struct byp_common));
		byp_cmgr_send_rsp_remote(BYP_CMD_BRCREATE, &byp_cmd_resp, 
                                 sizeof(struct byp_common), err_exec);
	}
}

void 
byp_rplay_bridge_delete(
	void        *cmd_buf,		
	a_uint32_t  size, 		
	a_bool_t	ack_required,
	a_bool_t	resp_required
	)
{
	struct byp_br *byp_cmd_req = (struct byp_br *)cmd_buf;
	static struct byp_common byp_cmd_resp;
    int err_exec;
	
	/** create a replay command */
	strcpy(cmd_name, byp_rplay_cmd_list[BYP_CMD_BRDELETE]);
	strcat(cmd_name, byp_cmd_req->br_name);
	
	/** execute a replay command */
	err_exec = system(cmd_name);
  
	if (ack_required)
	{
		memset(&byp_cmd_resp, 0, sizeof(struct byp_common));
		byp_cmgr_send_rsp_remote(BYP_CMD_BRDELETE, &byp_cmd_resp, 
                                 sizeof(struct byp_common), err_exec);
	}
}

void 
byp_rplay_vlan_create(
	void        *cmd_buf,		
	a_uint32_t  size, 		
	a_bool_t	ack_required,
	a_bool_t	resp_required
	)
{
    struct byp_vlan *vlan_cmd_req = (struct byp_vlan *)cmd_buf;
    static struct byp_common byp_cmd_resp;
    int err_exec;

    /** create a replay command */
    strcpy(cmd_name, byp_rplay_cmd_list[BYP_CMD_VLCREATE]);
    strcat(cmd_name, LAN_IF_NAME);
    strcat(cmd_name, " ");
    strcat(cmd_name, vlan_cmd_req->vlan_id);

    /** execute a replay command */
    err_exec = system(cmd_name);

    if (ack_required)
    {
        memset(&byp_cmd_resp, 0, sizeof(struct byp_common));
	    byp_cmgr_send_rsp_remote(BYP_CMD_VLCREATE, &byp_cmd_resp, 
                                 sizeof(struct byp_common), err_exec);
    }
}

void 
byp_rplay_vlan_delete(
	void        *cmd_buf,		
	a_uint32_t  size, 		
	a_bool_t	ack_required,
	a_bool_t	resp_required
	)
{
    struct byp_vlan *vlan_cmd_req = (struct byp_vlan *)cmd_buf;
    static struct byp_common byp_cmd_resp;
    int err_exec;

    /** create a replay command */
    strcpy(cmd_name, byp_rplay_cmd_list[BYP_CMD_VLDELETE]);
    strcat(cmd_name, LAN_IF_NAME);
    strcat(cmd_name, ".");
    strcat(cmd_name, vlan_cmd_req->vlan_id);

    /** execute a replay command */
    err_exec = system(cmd_name);

    if (ack_required)
    {
        memset(&byp_cmd_resp, 0, sizeof(struct byp_common));
	    byp_cmgr_send_rsp_remote(BYP_CMD_VLDELETE, &byp_cmd_resp, 
                                 sizeof(struct byp_common), err_exec);
    }
}

void 
byp_rplay_filter_addmac(
	void        *cmd_buf,		
	a_uint32_t  size, 		
	a_bool_t	ack_required,
	a_bool_t	resp_required
	)
{
    /** struct byp_br *byp_cmd_req = (struct byp_br *)cmd_buf; */
	struct byp_common byp_cmd_resp;
    int err_exec = BYP_RPLAYCMD_SUCCESS;
    
    if (ack_required)
    {
        memset(&byp_cmd_resp, 0, sizeof(struct byp_common));
	    byp_cmgr_send_rsp_remote(BYP_CMD_ADDMAC, &byp_cmd_resp, 
                                 sizeof(struct byp_common), err_exec);
    }
}

void 
byp_rplay_filter_delmac(
	void        *cmd_buf,		
	a_uint32_t  size, 		
	a_bool_t	ack_required,
	a_bool_t	resp_required
	)
{
    /** struct byp_br *byp_cmd_req = (struct byp_br *)cmd_buf; */
	struct byp_common byp_cmd_resp;
    int err_exec = BYP_RPLAYCMD_SUCCESS;
    
    if (ack_required)
    {
        memset(&byp_cmd_resp, 0, sizeof(struct byp_common));
	    byp_cmgr_send_rsp_remote(BYP_CMD_DELMAC, &byp_cmd_resp, 
                                 sizeof(struct byp_common), err_exec);
    }
}

void 
byp_rplay_brif_up(
	void        *cmd_buf,		
	a_uint32_t  size, 		
	a_bool_t	ack_required,
	a_bool_t	resp_required
	)
{
    struct byp_br *byp_cmd_req = (struct byp_br *)cmd_buf;
    static struct byp_common byp_cmd_resp;
    int err_exec;

    /** create a replay command */
    strcpy(cmd_name, byp_rplay_cmd_list[BYP_CMD_BRIFUP]);
    strcat(cmd_name, byp_cmd_req->br_name);
    strcat(cmd_name, " up");

    /** execute a replay command */
    err_exec = system(cmd_name);

    if (ack_required)
    {
        memset(&byp_cmd_resp, 0, sizeof(struct byp_common));
	    byp_cmgr_send_rsp_remote(BYP_CMD_BRIFUP, &byp_cmd_resp, 
                                 sizeof(struct byp_common), err_exec);
    }
}

void 
byp_rplay_brif_down(
	void        *cmd_buf,		
	a_uint32_t  size, 		
	a_bool_t	ack_required,
	a_bool_t	resp_required
	)
{
    struct byp_br *byp_cmd_req = (struct byp_br *)cmd_buf;
    static struct byp_common byp_cmd_resp;
    int err_exec;

    /** create a replay command */
    strcpy(cmd_name, byp_rplay_cmd_list[BYP_CMD_BRIFDOWN]);
    strcat(cmd_name, byp_cmd_req->br_name);
    strcat(cmd_name, " down");

    /** execute a replay command */
    err_exec = system(cmd_name);

    if (ack_required)
    {
        memset(&byp_cmd_resp, 0, sizeof(struct byp_common));
	    byp_cmgr_send_rsp_remote(BYP_CMD_BRIFDOWN, &byp_cmd_resp, 
                                 sizeof(struct byp_common), err_exec);
    }
}

void 
byp_rplay_bypass_enable(
	void        *cmd_buf,		
	a_uint32_t  size, 		
	a_bool_t	ack_required,
	a_bool_t	resp_required
	)
{
    int err_exec;
	struct byp_stat *byp_cmd_req = (struct byp_stat *)cmd_buf;
    struct byp_common byp_cmd_resp;

    if(byp_cmd_req->byp_status == LANBYP_MODE_TUNNEL)
    {
        /** create a replay command */
        strcpy(cmd_name, "brctl addbr ");
        strcat(cmd_name, LAN_BR_NAME);
        strcat(cmd_name, ";");
        strcat(cmd_name, "brctl setfd ");
        strcat(cmd_name, LAN_BR_NAME);
        strcat(cmd_name, " ");
        strcat(cmd_name, "0");
        strcat(cmd_name, ";");
        strcat(cmd_name, "ifconfig ");
        strcat(cmd_name, LAN_BR_NAME);
        strcat(cmd_name, " up");
        strcat(cmd_name, ";");
        strcat(cmd_name, "brctl addif ");
        strcat(cmd_name, LAN_BR_NAME);
        strcat(cmd_name, " ");
        strcat(cmd_name, LAN_IF_NAME);

        /** execute a replay command */
        err_exec = system(cmd_name);

    }


    /** no data is expected from user for this command */
    if (ack_required)
    {
        memset(&byp_cmd_resp, 0, sizeof(struct byp_common));
        byp_cmgr_send_rsp_remote(BYP_CMD_BYPEN, &byp_cmd_resp,
                sizeof(struct byp_common), 0);
    }
}


void 
byp_rplay_vlangrp_create(
	void        *cmd_buf,		
	a_uint32_t  size, 		
	a_bool_t	ack_required,
	a_bool_t	resp_required
	)
{
    struct byp_vlan *vlan_cmd_req = (struct byp_vlan *)cmd_buf;
    static struct byp_common byp_cmd_resp;
    int err_exec;

    /** create a replay command */
#ifdef GMAC_8021Q_SUPPORT
    strcpy(cmd_name, "ifconfig ");
#else
    strcpy(cmd_name, byp_rplay_cmd_list[BYP_CMD_VLCREATE]);
    strcat(cmd_name, LAN_IF_NAME);
    strcat(cmd_name, " ");
    strcat(cmd_name, vlan_cmd_req->vlan_id);
    strcat(cmd_name, " ;");
    strcat(cmd_name, "ifconfig ");
#endif
    strcat(cmd_name, LAN_IF_NAME);
    strcat(cmd_name, ".");
    strcat(cmd_name, vlan_cmd_req->vlan_id);
    strcat(cmd_name, " up;");
    strcat(cmd_name, byp_rplay_cmd_list[BYP_CMD_BRCREATE]);
  	strcat(cmd_name, VLAN_BR_PREFIX);
    strcat(cmd_name, vlan_cmd_req->vlan_id);
    strcat(cmd_name, " ;");
    strcat(cmd_name, "brctl setfd ");
  	strcat(cmd_name, VLAN_BR_PREFIX);
    strcat(cmd_name, vlan_cmd_req->vlan_id);
    strcat(cmd_name, " 0");
    strcat(cmd_name, " ;");
    strcat(cmd_name, "brctl addif ");
  	strcat(cmd_name, VLAN_BR_PREFIX);
    strcat(cmd_name, vlan_cmd_req->vlan_id);
    strcat(cmd_name, " ");
    strcat(cmd_name, LAN_IF_NAME);
    strcat(cmd_name, ".");
    strcat(cmd_name, vlan_cmd_req->vlan_id);
    strcat(cmd_name, ";");
    strcat(cmd_name, byp_rplay_cmd_list[BYP_CMD_BRIFUP]);
  	strcat(cmd_name, VLAN_BR_PREFIX);
    strcat(cmd_name, vlan_cmd_req->vlan_id);
    strcat(cmd_name, " up");

    /** execute a replay command */
    err_exec = system(cmd_name);

    if (ack_required)
    {
        memset(&byp_cmd_resp, 0, sizeof(struct byp_common));
	    byp_cmgr_send_rsp_remote(BYP_CMD_VLGRPCREATE, &byp_cmd_resp, 
                                 sizeof(struct byp_common), err_exec);
    }
}

void 
byp_rplay_vlangrp_delete(
	void        *cmd_buf,		
	a_uint32_t  size, 		
	a_bool_t	ack_required,
	a_bool_t	resp_required
	)
{
    struct byp_vlan *vlan_cmd_req = (struct byp_vlan *)cmd_buf;

    static struct byp_common byp_cmd_resp;
    int err_exec;

    /** create a replay command */
    strcpy(cmd_name, byp_rplay_cmd_list[BYP_CMD_BRIFDOWN]);
  	strcat(cmd_name, VLAN_BR_PREFIX);
    strcat(cmd_name, vlan_cmd_req->vlan_id);
    strcat(cmd_name, " down");
    strcat(cmd_name, byp_rplay_cmd_list[BYP_CMD_BRDELETE]);
  	strcat(cmd_name, VLAN_BR_PREFIX);
    strcat(cmd_name, vlan_cmd_req->vlan_id);
#ifndef GMAC_8021Q_SUPPORT
    strcat(cmd_name, " ;");
    strcat(cmd_name, byp_rplay_cmd_list[BYP_CMD_VLDELETE]);
    strcat(cmd_name, LAN_IF_NAME);
    strcat(cmd_name, ".");
    strcat(cmd_name, vlan_cmd_req->vlan_id);
#endif

    /** execute a replay command */
    err_exec = system(cmd_name);

    if (ack_required)
    {
        memset(&byp_cmd_resp, 0, sizeof(struct byp_common));
	    byp_cmgr_send_rsp_remote(BYP_CMD_VLGRPDELETE, &byp_cmd_resp, 
                                 sizeof(struct byp_common), err_exec);
    }
}

void 
byp_rplay_vlangrp_addvap(
	void        *cmd_buf,		
	a_uint32_t  size, 		
	a_bool_t	ack_required,
	a_bool_t	resp_required
	)
{
    struct byp_vlan *vlan_cmd_req = (struct byp_vlan *)cmd_buf;
    static struct byp_common byp_cmd_resp;
    int err_exec;

    /** create a replay command */
    strcpy(cmd_name, byp_rplay_cmd_list[BYP_CMD_VLGRPADDVAP]);
  	strcat(cmd_name, VLAN_BR_PREFIX);
    strcat(cmd_name, vlan_cmd_req->vlan_id);
  	strcat(cmd_name, " ");
    strcat(cmd_name, vlan_cmd_req->if_name);

    /** execute a replay command */
    err_exec = system(cmd_name);

    if (ack_required)
    {
        memset(&byp_cmd_resp, 0, sizeof(struct byp_common));
	    byp_cmgr_send_rsp_remote(BYP_CMD_VLGRPADDVAP, &byp_cmd_resp, 
                                 sizeof(struct byp_common), err_exec);
    }
}

void 
byp_rplay_vlangrp_delvap(
	void        *cmd_buf,		
	a_uint32_t  size, 		
	a_bool_t	ack_required,
	a_bool_t	resp_required
	)
{
    struct byp_vlan *vlan_cmd_req = (struct byp_vlan *)cmd_buf;
    static struct byp_common byp_cmd_resp;
    int err_exec;

    /** create a replay command */
    strcpy(cmd_name, byp_rplay_cmd_list[BYP_CMD_VLGRPDELVAP]);
  	strcat(cmd_name, VLAN_BR_PREFIX);
    strcat(cmd_name, vlan_cmd_req->vlan_id);
  	strcat(cmd_name, " ");
    strcat(cmd_name, vlan_cmd_req->if_name);

    /** execute a replay command */
    err_exec = system(cmd_name);

    if (ack_required)
    {
        memset(&byp_cmd_resp, 0, sizeof(struct byp_common));
	    byp_cmgr_send_rsp_remote(BYP_CMD_VLGRPDELVAP, &byp_cmd_resp, 
                                 sizeof(struct byp_common), err_exec);
    }
}
