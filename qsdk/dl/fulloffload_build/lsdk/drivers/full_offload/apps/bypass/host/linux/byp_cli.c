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
#include <bypass_types.h>
#include <linux/ac_shims.h>
#include <byp_lib.h>

/*
 * CONSTANTS 
 */


/* 
 * DECLARATIONS 
 */

static a_status_t
byp_cli_help(char *args[], int argc, void *buf, a_bool_t    reliable,
                                        a_uint32_t  timeout);

/** 
 * @brief Map command line arguments to the proper procedure...
 */
struct byp_cli_cmd {
	const char  *cmd;      /**< Command line shorthand */
	byp_lib_fn  fn;        /**< Subroutine */
	int         min_args;  /**< Min number of arguments */  
	int         max_args;  /**< Max number of arguments */  
    int         mode;      /**< Mode in which command is supported */ 
	const char  *name;     /**< Command description */
	const char  *argsname; /**< Command Args */
};

/**************************** DEFINITIONS **************************/

static const struct byp_cli_cmd byp_cli_cmd_list[] = {

    { "enable", byp_lib_bypass_enable, 1, 1, 
       (LANBYP_MODE_ALL), 
      "LAN Bypass Enable/Disable!!!MUST BE FIRST CMD TO RUN!!!  ",
      "dis | tunnel | vlan" },

    { "vapadd", byp_lib_vap_ctrl_add, 1, 2,  
       LANBYP_MODE_TUNNEL,
       "Add VAP to LAN or a Bridge", 
       "<vap_name> [<bridge_name>]"}, 

    { "vapdel", byp_lib_vap_ctrl_del, 1,  2,  
       LANBYP_MODE_TUNNEL,
       "Del VAP from LAN or a Bridge", 
       "<vap_name> [<bridge_name>]"}, 

	{ "filteradd", byp_lib_bridge_add_mac, 1,1,
       (LANBYP_MODE_TUNNEL | LANBYP_MODE_VLAN), 
      "Add MAC address to Packet Filter", 
      "<mac_address: notation in network byte order>" },

    { "filterdel", byp_lib_bridge_del_mac, 1, 1,
       (LANBYP_MODE_TUNNEL | LANBYP_MODE_VLAN), 
      "Remove MAC address from Packet Filter", 
      "<mac_address: notation in network byte order>" },

    { "vlgrp_create", byp_lib_vlangrp_create, 1, 1,
       (LANBYP_MODE_TUNNEL | LANBYP_MODE_VLAN), 
      "Add a new VLAN Group", "<vlan_id>" },

    { "vlgrp_delete", byp_lib_vlangrp_delete, 1, 1,
       (LANBYP_MODE_TUNNEL | LANBYP_MODE_VLAN), 
      "Remove a VLAN Group", "<vlan_id>" },

    { "vlgrp_addvap", byp_lib_vlangrp_addvap, 2, 2,
       (LANBYP_MODE_TUNNEL | LANBYP_MODE_VLAN), 
      "Add VAP to VLAN Group", "<vlan_id> <vap_name>" },

    { "vlgrp_delvap", byp_lib_vlangrp_delvap, 2, 2,
       (LANBYP_MODE_TUNNEL | LANBYP_MODE_VLAN), 
      "Remove VAP from VLAN Group", "<vlan_id> <vap_name>" },

    { "vlcreate", byp_lib_vlan_create, 1,1,
       (LANBYP_MODE_TUNNEL | LANBYP_MODE_VLAN), 
      "Add a new VLAN Interface", "<vlan_id>" },

    { "vldelete", byp_lib_vlan_delete, 1, 1,
       (LANBYP_MODE_TUNNEL | LANBYP_MODE_VLAN), 
      "Remove a VLAN Interface", "<vlan_id>" },

	{ "brcreate", byp_lib_bridge_create, 1,  1, 
       LANBYP_MODE_TUNNEL,
	  "Create Bridge", "<bridge_name>" },

	{ "brdelete", byp_lib_bridge_delete, 1,  1, 
       LANBYP_MODE_TUNNEL,
      "Delete Bridge", "<bridge_name>" },

	{ "brifup",    byp_lib_brif_up, 1, 1,
       LANBYP_MODE_TUNNEL,
      "Bring Up a Bridge", "<br_name>" },

	{ "brifdown",  byp_lib_brif_down, 1, 1,
       LANBYP_MODE_TUNNEL,
      "Bring Down a Bridge", "<br_name>" },

    { "help", byp_cli_help, 0, 1,
      LANBYP_MODE_ALL,
      "Display Help", "dis |  tunnel | vlan"},

	{ NULL, NULL, 0, 0, 0, NULL,NULL },
};


/** 
 * @brief  Find the most appropriate command matching the command line
 * 
 * @param cmd       points to start of command 
 * @param num_args  number of arguments passed to command 	
 * 
 * @return 
 */
static const struct byp_cli_cmd * 
byp_cli_search_cmd(
	const char * cmd,
	int num_args	 
	)
{
	int wrong_args = 0;
	unsigned int len = strlen(cmd);
	int i;

	/* Go through all commands */
	for(i = 0; byp_cli_cmd_list[i].cmd != NULL; ++i)
    {

		/* No match -> next one */
		if(strncasecmp(byp_cli_cmd_list[i].cmd, cmd, len) != 0)
			continue;

		/* Exact match -> perfect */
		if(len == strlen(byp_cli_cmd_list[i].cmd))
		{
			/* also check the number of arguments */
			if ((num_args < byp_cli_cmd_list[i].min_args) || 
                (num_args > byp_cli_cmd_list[i].max_args))
            {
				wrong_args = 1;
				break;
            }
			else
			{
				return &byp_cli_cmd_list[i];
			}  
		}
	}

	if (wrong_args)
		ac_printf("byp_cli: unexpected number of arguments \"%s\": %d \n", 
			(char *) cmd, num_args);
	else
		ac_printf("byp_cli: unknown command \"%s\"\n", cmd);
    
	return NULL;
}

/** 
 * @brief  Find the individual command and call the appropriate subroutine
 * 
 * @param args[]    Command Line Args
 * @param count     Args Count
 * 
 * @return 
 */
static int
byp_cli_parse_cmd(
    char *    args[],  
    int       count   
    )           
{
	const struct byp_cli_cmd * acmd;
	int   ret = 0;

	/* check if we received a valid command; also verify number 
		of arguments for the command */
	acmd = byp_cli_search_cmd(args[0], (count - 1));
  
	if(acmd == NULL)
	{
		/* Command did not match or wrong number of arguments */
		return(-1);
	}
	
	/* we do NOT support reliable commands 
		and commands with responses  for this release */
	ret = (*acmd->fn)((char **) &args[1], (count - 1), NULL, 0, 0);

	/* Deal with various errors */
	if(ret != APPBR_STAT_OK)
	{
		switch(ret)
		{
			case APPBR_STAT_EARGSZ:
				ac_printf(" Invalid argument size\n");
				break;
			case APPBR_STAT_ENOREQACK:
				ac_printf(" Invld arg: ACKTimeout is reqd for reliable cmd\n");
				break;
			case APPBR_STAT_ENORESPACK:
				ac_printf(" Invalid arg: RESP Timeout is required \n");
				break;
            case APPBR_STAT_ENOMEM:
				ac_printf(" Err in sending cmd to Tgt.Not enough resources\n");
                break;
            case APPBR_STAT_ESENDCMD:
				ac_printf(" Error in sending command to Target \n");
				break;
            case APPBR_STAT_ENOSOCK:
				ac_printf(" Err in creating socket for commn \n");
				break;
            case APPBR_STAT_ERECV:
				ac_printf(" Error in receiving response for command \n");
				break;
            default:
				ac_printf(" Unknown Error while processing command \n");
				break;
		}
	}   
    
	/* Command Completed */
	return(ret);
}

/** 
 * @brief  Display help
 */
static void 
byp_cli_show_help(int mode)
{
  int i;

  ac_printf("Usage: byp_cli <command> \n");
  ac_printf(" command : \n\n");
  for(i = 0; byp_cli_cmd_list[i].cmd != NULL; ++i)
  {
      if((byp_cli_cmd_list[i].mode & mode))
      {
          ac_printf(" %-10s %-45s \n ==> %-30s\n",
                  byp_cli_cmd_list[i].cmd, byp_cli_cmd_list[i].argsname, 
                  byp_cli_cmd_list[i].name);
      }
    
  }
}


static a_status_t 
byp_cli_help(char *args[], int argc, void *buf, a_bool_t    reliable,
                                a_uint32_t  timeout)
{
    if(argc == 0)
        byp_cli_show_help(LANBYP_MODE_ALL);
    else if(!strcmp(args[0], "dis"))
        byp_cli_show_help(LANBYP_MODE_NOTSET);
    else if(!strcmp(args[0], "tunnel"))
        byp_cli_show_help(LANBYP_MODE_TUNNEL);
    else if(!strcmp(args[0], "vlan"))
        byp_cli_show_help(LANBYP_MODE_VLAN);
    else
        printf("illegal mode \n");

    return A_STATUS_OK;

}


/* 
 * Main
 */

/** 
 * @brief The main 
 * 
 * @param argc
 * @param argv
 * 
 * @return 
 */
int 
main(int argc, char ** argv)
{
	int goterr = 0;

	/* Open Socket */
	if((goterr = byp_lib_open_appbr()) < 0)
	{
		ac_perror("socket"); 
		exit(-1);
	}	

	/* No argument : show the list of all device + info */
	if(argc == 1)
		byp_cli_show_help(LANBYP_MODE_ALL);
	else /* Some argument : lets process */
		goterr = byp_cli_parse_cmd(argv + 1, argc - 1);
 
	if (goterr != 0)
		ac_printf("error in command execution \n");	

	/* Close the socket. */
	byp_lib_close_appbr();

	return(goterr);
}
