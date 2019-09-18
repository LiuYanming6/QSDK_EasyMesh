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

#include <appbr_if.h>
#include <byp_lib.h>

#include <linux/ac_shims.h>

/** 
 * @brief Send remote command to Bypass-Appbr interface
 * 
 * @param cmd_id     command identifier    
 * @param cmd_buf    points to command buffer
 * @param size       size of command
 * @param resp_buf   points to response buffer
 * @param reliable   0: no ack 1: ack  
 * @param timeout    timeout for response ack
 * @param node_id    0: send to kernel & uspace, 1: send to uspace only  
 * 
 * @return 
 */
static appbr_status_t 
byp_cmgr_send_cmd(  a_uint8_t cmd_id, void  *cmd_buf, a_uint16_t  size, 
                    void  *resp_buf,  a_bool_t    reliable,a_uint32_t  timeout,
                    a_uint8_t   node_id)
{
	appbr_status_t resp_code = APPBR_STAT_OK;
	struct byp_common *cmd_hdr = (struct byp_common *) cmd_buf;
	
	/**< fill command characteristics */
	cmd_hdr->data[0] |= BYP_CMD_REQ;
	
	cmd_hdr->data[0] |= node_id;
	
	if (reliable)
	{
		/**  timeout is required for reliable commands */
		if (timeout == 0)
			return APPBR_STAT_ENOREQACK;
		else
			cmd_hdr->data[0] |= BYP_ACK_REQD;
	}
	
	if (resp_buf)
	{
		/**  timeout is required for commands with response */
		if (timeout == 0)
			return APPBR_STAT_ENORESPACK;
		else
			cmd_hdr->data[0] |= BYP_RESP_REQD;
	}		
	
	cmd_hdr->data[1] = cmd_id;
	
	*((a_uint16_t *)&cmd_hdr->data[2]) = size;
	
	/**  nd command to remote end via appbr */
	resp_code = appbr_if_send_cmd_remote (APPBR_BYP, cmd_buf, size);
	
	if (resp_code != APPBR_STAT_OK)
		return resp_code;
	
	if (reliable || resp_buf)
	{
		/* block for acknowledgement/ response */
		resp_code = appbr_if_wait_for_response(resp_buf,size, timeout);
	}
	
	return resp_code;
} 

/** 
 * @brief Set LAN Access for a VAP
 * 
 * @param args[]
 * @param buf
 * @param reliable
 * @param 
 * 
 * @return 
 */
appbr_status_t 
byp_lib_vap_ctrl_add(char   *args[],    int argc, void        *buf,
                a_bool_t    reliable,a_uint32_t  timeout)
{
	byp_vapctrl_t byp_cmd;

    memset ((void *)&byp_cmd, 0, sizeof(byp_cmd));	 
	
	if(strlen(args[0]) > BYP_SZ_IFNAME)
	{
	    return(APPBR_STAT_EARGSZ);
	}
	strcpy(byp_cmd.if_name, args[0]);

    byp_cmd.ctrl    =    BYP_VAPCTRL_SET;

    if(argc ==  2)
    {
        byp_cmd.ctrl    |=    BYP_VAPCTRL_BR_EN;  

        if(strlen(args[1]) > BYP_SZ_IFNAME)
        {
            /* printf("bridgename too big %s",args[1]); */
            return(APPBR_STAT_EARGSZ);
        }

        strcpy(byp_cmd.br_name, args[1]);
    }
    else
    {
        byp_cmd.ctrl    |=    BYP_VAPCTRL_LAN_EN;  
    }


    /** send command to remote end */
    return byp_cmgr_send_cmd(BYP_CMD_VAPCTRL, (void *)&byp_cmd, 
            sizeof(byp_cmd), buf, reliable, timeout, BYP_NODE_ALL);

}

appbr_status_t 
byp_lib_vap_ctrl_del(char   *args[],    int argc, void        *buf,
                a_bool_t    reliable,a_uint32_t  timeout)
{
	byp_vapctrl_t byp_cmd;

    memset ((void *)&byp_cmd, 0, sizeof(byp_cmd));	 
	
	if(strlen(args[0]) > BYP_SZ_IFNAME)
	{
	    return(APPBR_STAT_EARGSZ);
	}
	strcpy(byp_cmd.if_name, args[0]);

    byp_cmd.ctrl    =    BYP_VAPCTRL_RESET;

    if(argc ==  2)
    {
        byp_cmd.ctrl    |=    BYP_VAPCTRL_BR_EN;  

        if(strlen(args[1]) > BYP_SZ_IFNAME)
        {
            /* printf("bridgename too big %s",args[1]); */
            return(APPBR_STAT_EARGSZ);
        }

        strcpy(byp_cmd.br_name, args[1]);
    }
    else
    {
        byp_cmd.ctrl    |=    BYP_VAPCTRL_LAN_EN;  
    }

    /** send command to remote end */
    return byp_cmgr_send_cmd(BYP_CMD_VAPCTRL, (void *)&byp_cmd, 
            sizeof(byp_cmd), buf, reliable, timeout, BYP_NODE_ALL);

}
/** 
 * @brief  Create bridge
 * 
 * @param args[]
 * @param buf
 * @param reliable
 * @param 
 * 
 * @return 
 */
appbr_status_t 
byp_lib_bridge_create(char		*args[],    int argc, void        *buf,
                    a_bool_t    reliable, a_uint32_t  timeout)
{
	byp_br_t byp_cmd;

    memset ((void *)&byp_cmd, 0, sizeof(byp_br_t));	 
	
	/* validate argument lengths */
	if(strlen(args[0]) > BYP_SZ_IFNAME)
	{
	    return(APPBR_STAT_EARGSZ);
	}
	
	/** pack arguments */
	strcpy(byp_cmd.br_name, args[0]);
	
	/** send command to remote end */
	return byp_cmgr_send_cmd(BYP_CMD_BRCREATE, (void *)&byp_cmd, 
                             sizeof(struct byp_br), buf, reliable, timeout,
                             BYP_NODE_REMOTE);
}

/** 
 * @brief  Remove bridge
 * 
 * @param args[]	 command arguments  
 * @param buf	     response buffer (optional)
 * @param reliable   1: ack required
 * @param            timeout for ack/ response 
 * 
 * @return 
 */
appbr_status_t 
byp_lib_bridge_delete(char * args[], int argc, void   *buf, 
                      a_bool_t    reliable, a_uint32_t  timeout)	  
{
	struct byp_br byp_cmd;
	
	memset (&byp_cmd, 0, sizeof(struct byp_br));
	
	/** validate argument lengths */
	if(strlen(args[0]) > BYP_SZ_IFNAME)
	{
	    return(APPBR_STAT_EARGSZ);
	}
	
	/** pack arguments */
	strcpy(byp_cmd.br_name, args[0]);
	
	/** send command to remote end */
	return byp_cmgr_send_cmd(BYP_CMD_BRDELETE, (void *)&byp_cmd, 
                            sizeof(struct byp_br), buf, reliable, timeout,
                            BYP_NODE_REMOTE);
}

/** 
 * @brief  Create VLAN
 */
appbr_status_t 
byp_lib_vlan_create(char * args[], int argc, void  *buf, a_bool_t   reliable,
                    a_uint32_t  timeout)
{
	byp_vlan_t byp_cmd;


    memset ((void *)&byp_cmd, 0, sizeof(byp_cmd));	 
	
	/* validate argument lengths */
	if(strlen(args[0]) > BYP_SZ_VIDNAME)
	{
	    return(APPBR_STAT_EARGSZ);
	}

    if(atoi(args[0]) > 4096)
    {
	    return(APPBR_STAT_EARGSZ);
    }
	
	/** pack arguments */
	strcpy(byp_cmd.vlan_id, args[0]);
	
	/** send command to remote end */
	return byp_cmgr_send_cmd(BYP_CMD_VLCREATE, (void *)&byp_cmd, sizeof(byp_cmd),
                             buf, reliable, timeout, BYP_NODE_REMOTE);
}

/** 
 * @brief  Remove a VLAN Device
 * 
 * @return 
 */
appbr_status_t 
byp_lib_vlan_delete(char    *args[],    int argc, void    *buf, 
                    a_bool_t  reliable,	a_uint32_t  timeout	)
{
	byp_vlan_t byp_cmd;
	
	memset (&byp_cmd, 0, sizeof(byp_cmd));
	
	/** validate argument lengths */
	if(strlen(args[0]) > BYP_SZ_VIDNAME)
	{
	    return(APPBR_STAT_EARGSZ);
	}

    if(atoi(args[0]) > 4096)
    {
	    return(APPBR_STAT_EARGSZ);
    }
	
	
	/** pack arguments */
	strcpy(byp_cmd.vlan_id, args[0]);
	
	/** send command to remote end */
	return byp_cmgr_send_cmd(BYP_CMD_VLDELETE, (void *)&byp_cmd, 
                             sizeof(byp_cmd), buf, reliable, timeout,
                             BYP_NODE_REMOTE);
}

/** 
 * @brief  Provide MAC address for bridge
 * 
 * @param args[]
 * @param buf
 * @param reliable
 * @param 
 * 
 * @return 
 */
appbr_status_t 
byp_lib_bridge_add_mac(char *args[], int argc, void *buf, a_bool_t    reliable,
                       a_uint32_t  timeout)
{
	struct byp_br byp_cmd;

    a_uint32_t temp_addr[6];
	
	memset (&byp_cmd, 0, sizeof(struct byp_br));
	
	sscanf(args[0], "%02x:%02x:%02x:%02x:%02x:%02x", 
            &temp_addr[0],
            &temp_addr[1],
            &temp_addr[2],
            &temp_addr[3],
            &temp_addr[4],
            &temp_addr[5]);


    byp_cmd.br_addr[0] = (a_uint8_t) (temp_addr[0]);  
    byp_cmd.br_addr[1] = (a_uint8_t) (temp_addr[1]);
    byp_cmd.br_addr[2] = (a_uint8_t) (temp_addr[2]);
    byp_cmd.br_addr[3] = (a_uint8_t) (temp_addr[3]);
    byp_cmd.br_addr[4] = (a_uint8_t) (temp_addr[4]);
    byp_cmd.br_addr[5] = (a_uint8_t) (temp_addr[5]);

	/* send command to remote end */
	return byp_cmgr_send_cmd(BYP_CMD_ADDMAC, (void *)&byp_cmd, 
                             sizeof(struct byp_br), buf, reliable, timeout,
                             BYP_NODE_ALL);
}

/*
 * Del MAC address for bridge
 */
    appbr_status_t
byp_lib_bridge_del_mac(char *args[], int argc, void *buf, a_bool_t    reliable,
        a_uint32_t  timeout)
{
    struct byp_br byp_cmd;
    a_uint32_t temp_addr[6];

    memset (&byp_cmd, 0, sizeof(struct byp_br));

    sscanf(args[0], "%02x:%02x:%02x:%02x:%02x:%02x", 
            &temp_addr[0],
            &temp_addr[1],
            &temp_addr[2],
            &temp_addr[3],
            &temp_addr[4],
            &temp_addr[5]);


    byp_cmd.br_addr[0] = (a_uint8_t) (temp_addr[0]);  
    byp_cmd.br_addr[1] = (a_uint8_t) (temp_addr[1]);
    byp_cmd.br_addr[2] = (a_uint8_t) (temp_addr[2]);
    byp_cmd.br_addr[3] = (a_uint8_t) (temp_addr[3]);
    byp_cmd.br_addr[4] = (a_uint8_t) (temp_addr[4]);
    byp_cmd.br_addr[5] = (a_uint8_t) (temp_addr[5]);
	
	/** send command to remote end */
	return byp_cmgr_send_cmd(BYP_CMD_DELMAC, (void *)&byp_cmd, 
                             sizeof(struct byp_br), buf, reliable, timeout,
                             BYP_NODE_ALL);
}

/** 
 * @brief  Bringup a Bridge Interface
 * 
 * @param args[]
 * @param buf
 * @param reliable
 * @param 
 * 
 * @return 
 */
appbr_status_t 
byp_lib_brif_up(char *args[], int argc, void *buf, a_bool_t    reliable,
                        a_uint32_t  timeout)
{
	byp_br_t byp_cmd;

    memset ((void *)&byp_cmd, 0, sizeof(byp_br_t));	 

	/** validate argument lengths */
	if(strlen(args[0]) > BYP_SZ_IFNAME)
	{
	    return(APPBR_STAT_EARGSZ);
	}
	
	/** pack arguments */
	strcpy(byp_cmd.br_name, args[0]);
	
	/** send command to remote end */
	return byp_cmgr_send_cmd(BYP_CMD_BRIFUP, (void *)&byp_cmd, 
                             sizeof(struct byp_br), buf, reliable, timeout, 
                             BYP_NODE_REMOTE);
}

/** 
 * @brief  Bring Down Bridge Interface
 * 
 * @param args[]
 * @param buf
 * @param reliable
 * @param 
 * 
 * @return 
 */
appbr_status_t
byp_lib_brif_down(char *args[], int argc, void *buf, a_bool_t    reliable,
                        a_uint32_t  timeout)
{
	byp_br_t byp_cmd;

    memset ((void *)&byp_cmd, 0, sizeof(byp_br_t));	 

	/** validate argument lengths */
	if(strlen(args[0]) > BYP_SZ_IFNAME)
	{
	    return(APPBR_STAT_EARGSZ);
	}
	

	/** pack arguments */
	strcpy(byp_cmd.br_name, args[0]);
	
	/** send command to remote end */
	return byp_cmgr_send_cmd(BYP_CMD_BRIFDOWN, (void *)&byp_cmd, 
                             sizeof(struct byp_br), buf, reliable, timeout, 
                             BYP_NODE_REMOTE);
}

/** 
 * @brief  Create a VLAN Group
 */
appbr_status_t 
byp_lib_vlangrp_create(char * args[], int argc, void     *buf,
                        a_bool_t    reliable,  a_uint32_t  timeout)
{
	byp_vlan_t byp_cmd;

    memset ((void *)&byp_cmd, 0, sizeof(byp_cmd));	 
	
	/* validate argument lengths */
	if(strlen(args[0]) > BYP_SZ_VIDNAME)
	{
	    return(APPBR_STAT_EARGSZ);
	}

    if(atoi(args[0]) > 4096)
    {
	    return(APPBR_STAT_EARGSZ);
    }
	
	
	/** pack arguments */
	strcpy(byp_cmd.vlan_id, args[0]);
	
	/** send command to remote end */
	return byp_cmgr_send_cmd(BYP_CMD_VLGRPCREATE, (void *)&byp_cmd, 
            sizeof(byp_cmd),buf, reliable, timeout, BYP_NODE_ALL);
}

/** 
 * @brief  Remove a VLAN Group
 * 
 * @return 
 */
appbr_status_t 
byp_lib_vlangrp_delete(char *args[], int argc, void   *buf, a_bool_t  reliable,
                       a_uint32_t  timeout)
{
	byp_vlan_t byp_cmd;
	
	memset (&byp_cmd, 0, sizeof(byp_cmd));
	
	/** validate argument lengths */
	if(strlen(args[0]) > BYP_SZ_VIDNAME)
	{
	    return(APPBR_STAT_EARGSZ);
	}

    if(atoi(args[0]) > 4096)
    {
	    return(APPBR_STAT_EARGSZ);
    }
	
	/** pack arguments */
	strcpy(byp_cmd.vlan_id, args[0]);
	
	/** send command to remote end */
	return byp_cmgr_send_cmd(BYP_CMD_VLGRPDELETE, (void *)&byp_cmd, 
                             sizeof(byp_cmd), buf, reliable, timeout,
                             BYP_NODE_REMOTE);
}

/** 
 * @brief  Add VAP to a VLAN Group
 */
appbr_status_t 
byp_lib_vlangrp_addvap(char * args[], int argc, void     *buf,
                       a_bool_t    reliable,    a_uint32_t  timeout)
{
	byp_vlan_t byp_cmd;

    memset ((void *)&byp_cmd, 0, sizeof(byp_cmd));	 
	
	/* validate argument lengths */
	if(strlen(args[0]) > BYP_SZ_VIDNAME)
	{
	    return(APPBR_STAT_EARGSZ);
	}

    if(atoi(args[0]) > 4096)
    {
	    return(APPBR_STAT_EARGSZ);
    }
	
	
	if(strlen(args[1]) > BYP_SZ_IFNAME)
	{
	    return(APPBR_STAT_EARGSZ);
	}

	/** pack arguments */
	strcpy(byp_cmd.vlan_id, args[0]);
	strcpy(byp_cmd.if_name, args[1]);
	
	/** send command to remote end */
	return byp_cmgr_send_cmd(BYP_CMD_VLGRPADDVAP, (void *)&byp_cmd, 
            sizeof(byp_cmd),buf, reliable, timeout, BYP_NODE_ALL);
}

/** 
 * @brief  Remove VAP from a VLAN Group
 */
appbr_status_t 
byp_lib_vlangrp_delvap(char * args[], int argc, void    *buf,
                    a_bool_t    reliable,   a_uint32_t  timeout)
{
	byp_vlan_t byp_cmd;

    memset ((void *)&byp_cmd, 0, sizeof(byp_cmd));	 
	
	/* validate argument lengths */
	if(strlen(args[0]) > BYP_SZ_VIDNAME)
	{
	    return(APPBR_STAT_EARGSZ);
	}
	
    if(atoi(args[0]) > 4096)
    {
	    return(APPBR_STAT_EARGSZ);
    }
	
	if(strlen(args[1]) > BYP_SZ_IFNAME)
	{
	    return(APPBR_STAT_EARGSZ);
	}

	/** pack arguments */
	strcpy(byp_cmd.vlan_id, args[0]);
	strcpy(byp_cmd.if_name, args[1]);
	
	/** send command to remote end */
	return byp_cmgr_send_cmd(BYP_CMD_VLGRPDELVAP, (void *)&byp_cmd, 
            sizeof(byp_cmd),buf, reliable, timeout, BYP_NODE_ALL);
}
/** 
 * @brief  Enable/ Disable Bypass
 * 
 * @param args[]
 * @param buf
 * @param reliable
 * @param 
 * 
 * @return 
 */
appbr_status_t 
byp_lib_bypass_enable(char *args[], int argc, void *buf, a_bool_t    reliable,
                        a_uint32_t  timeout)
{
	byp_stat_t byp_cmd;

	memset (&byp_cmd, 0, sizeof(byp_cmd));

    if(!strcmp(args[0], "dis"))
    {
        byp_cmd.byp_status =   LANBYP_MODE_DISABLED; 
    }
    else if(!strcmp(args[0], "tunnel"))
    {
        byp_cmd.byp_status =   LANBYP_MODE_TUNNEL;
    }
    else if(!strcmp(args[0], "vlan"))
    {
        byp_cmd.byp_status =   LANBYP_MODE_VLAN;
    }
    else
        return  APPBR_STAT_EARGSZ;
    /*    "illegal argument must be [dis | tunnel | vlan]" */

	/** send command to remote end */
	return byp_cmgr_send_cmd(BYP_CMD_BYPEN, (void *)&byp_cmd, 
                             sizeof(byp_cmd), buf, reliable, timeout, 
                             BYP_NODE_ALL);

}

/** 
 * @brief  Open connection to remote node
 * 
 * @return 
 */
appbr_status_t byp_lib_open_appbr()
{

    appbr_status_t ret_status;

	ret_status = appbr_if_open_dl_conn(APPBR_BYP);
    if(ret_status != APPBR_STAT_OK)
		goto out;
	
	ret_status = appbr_if_open_ul_conn(APPBR_BYP);
    if(ret_status != APPBR_STAT_OK)
		goto out;

out:
    return  ret_status;
}

/** 
 * @brief  Close connection to remote node
 */
void byp_lib_close_appbr()
{
	appbr_if_close_dl_conn();

	appbr_if_close_ul_conn();

}
