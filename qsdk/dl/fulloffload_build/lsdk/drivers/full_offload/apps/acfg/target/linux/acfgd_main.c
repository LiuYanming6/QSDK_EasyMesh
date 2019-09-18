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
#include <stdio.h>
#include <stdlib.h>
#include <acfg_api_types.h>
#include <appbr_types.h>
#include <appbr_if.h>
#include <acfgd_rplay.h>

a_uint8_t recv_cmd[ACFG_DL_MAX_CMDSZ];

/** 
 * @brief Main Function
 * 
 * @return 
 */
int 
main()
{

    int ret_status;

    openlog("ACFGD", LOG_CONS | LOG_PID, LOG_DAEMON);

	if((ret_status = appbr_if_open_dl_conn(APPBR_ACFG)) != APPBR_STAT_OK)
		return -1;
	
	if((ret_status = appbr_if_open_ul_conn(APPBR_ACFG)) != APPBR_STAT_OK)
		return -1;
		
	while(1)
	{
		if((ret_status = appbr_if_wait_for_response((void *)recv_cmd, \
                         ACFG_DL_MAX_CMDSZ, -1)) != APPBR_STAT_OK)
        {
		    acfg_err_log("Error Receiving message from Socket \n");
            continue;
        }
           
		acfg_cmgr_recv_cmd_remote((void *)recv_cmd);
	}	
}
