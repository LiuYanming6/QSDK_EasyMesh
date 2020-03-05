/*----------------------------------------------------------------------*
 * Gatespace
 * Copyright 2011, 2012 Gatespace Networks, Inc., All Rights Reserved.
 * Gatespace Networks, Inc. confidential material.
 *----------------------------------------------------------------------*
 * File Name  :
 * Description: Auto-generated getter/setter stubs file.
 *----------------------------------------------------------------------*
 * $Revision: 1.2 $
 *
 * $Id: stubgenPre,v 1.2 2012/05/10 17:38:08 dmounday Exp $
 *----------------------------------------------------------------------*/

#include "paramTree.h"
#include "rpc.h"
#include "gslib/src/utils.h"
#include "soapRpc/rpcUtils.h"
#include "soapRpc/rpcMethods.h"
#include "soapRpc/cwmpSession.h"

#include "Time.h"

/**@obj Time **/
/**@param Time_Enable                     **/
CPE_STATUS setTime_Enable(Instance *ip, char *value)
{
    char cmd_result[128] = "";
    int ret = testBoolean(value);

    if(ret == 1)
        cmd_popen("/etc/init.d/sysntpd reload", cmd_result);
    else
        cmd_popen("/etc/init.d/sysntpd stop", cmd_result);
        
#if 0
	GS_Time *p = (GS_Time *)ip->cpeData;
	if ( p ){
		p->enable=testBoolean(value);
	}
#endif
	return CPE_OK;
}
CPE_STATUS getTime_Enable(Instance *ip, char **value)
{
    char cmd_result[128] = "";
    cmd_popen("ps | grep -v grep | grep ntpd", cmd_result);
    if(strlen(cmd_result) >0)
        *value = GS_STRDUP("true");
    else
        *value = GS_STRDUP("false");
#if 0
	GS_Time *p = (GS_Time *)ip->cpeData;
	if ( p ){
		*value = GS_STRDUP(p->enable? "true": "false");
	}
#endif
	return CPE_OK;
}
/**@endparam                                               **/
/**@param Time_NTPServer1                     **/
CPE_STATUS setTime_NTPServer1(Instance *ip, char *value)
{
    char cmd[128] = "";
    char cmd_result[128] = "";
    char *ret = NULL;
    const char ch = ' ';

    cmd_popen("uci get system.ntp.server" , cmd);
    if(strlen(cmd) >0) /* NTPServer2*/
    {
        ret = strchr(cmd, ch);
        strtok(ret,"\n");

        memset(cmd_result, 0 , sizeof(cmd_result));
        sprintf(cmd_result, "uci set system.ntp.server='%s %s'", value , ret+1);

        cmd_popen(cmd_result, cmd_result);
        system("uci commit");
        cmd_popen("/etc/init.d/sysntpd reload", cmd_result);
    }

#if 0
	GS_Time *p = (GS_Time *)ip->cpeData;
	if ( p ){
		COPYSTR(p->nTPServer1, value);
	}
#endif
	return CPE_OK;
}
CPE_STATUS getTime_NTPServer1(Instance *ip, char **value)
{
    char cmd_result[128]="";

    cmd_popen("uci get system.ntp.server" , cmd_result);
    if(strlen(cmd_result) >0)
    {
        strtok(cmd_result, " ");
        *value = GS_STRDUP(cmd_result);
    }
#if 0
	GS_Time *p = (GS_Time *)ip->cpeData;
	if ( p ){
		if ( p->nTPServer1 )
			*value = GS_STRDUP(p->nTPServer1);
	}
#endif
	return CPE_OK;
}
/**@endparam                                               **/
/**@param Time_NTPServer2                     **/
CPE_STATUS setTime_NTPServer2(Instance *ip, char *value)
{
    char cmd[128] = "";
    char cmd_result[128] = "";
    char *ret = NULL;

    cmd_popen("uci get system.ntp.server" , cmd);
    if(strlen(cmd) >0) /* NTPServer1*/
    {
        ret = strtok(cmd," ");

        sprintf(cmd_result, "uci set system.ntp.server='%s %s'", ret , value);
        cmd_popen(cmd_result, cmd_result);
        system("uci commit");
        cmd_popen("/etc/init.d/sysntpd reload", cmd_result);
    }

#if 0
	GS_Time *p = (GS_Time *)ip->cpeData;
	if ( p ){
		COPYSTR(p->nTPServer2, value);
	}
#endif
	return CPE_OK;
}
CPE_STATUS getTime_NTPServer2(Instance *ip, char **value)
{
    char cmd_result[128]="";
    const char ch = ' ';
    char *ret = NULL;

    cmd_popen("uci get system.ntp.server" , cmd_result);
    if(strlen(cmd_result) >0)
    {
        ret = strchr(cmd_result, ch);
        strtok(ret,"\n");
        *value = GS_STRDUP(ret+1);
    }
#if 0
	GS_Time *p = (GS_Time *)ip->cpeData;
	if ( p ){
		if ( p->nTPServer2 )
			*value = GS_STRDUP(p->nTPServer2);
	}
#endif
	return CPE_OK;
}
/**@endparam                                               **/
/**@endobj Time **/

