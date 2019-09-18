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

#include "DeviceInfoMemoryStatus.h"

/**@obj DeviceInfoMemoryStatus **/
CPE_STATUS  initDeviceInfoMemoryStatus(CWMPObject *o, Instance *ip)
{
	/* initialize object */
	DeviceInfoMemoryStatus *p = (DeviceInfoMemoryStatus *)GS_MALLOC( sizeof(struct DeviceInfoMemoryStatus));
	memset(p, 0, sizeof(struct DeviceInfoMemoryStatus));
	ip->cpeData = (void *)p;
	return CPE_OK;
}

/**@param DeviceInfoMemoryStatus_Total                     **/
CPE_STATUS getDeviceInfoMemoryStatus_Total(Instance *ip, char **value)
{
    DeviceInfoMemoryStatus *p = (DeviceInfoMemoryStatus *)ip->cpeData;
    if ( p ){
        char cmd[128]={0};
        char cmd_result[128]={0};
        char *pos = NULL;
        memset(cmd , 0x0 , sizeof(cmd));
        memset(cmd_result , 0x0 , sizeof(cmd_result));
        sprintf(cmd, "free | grep Mem| awk -F \" \" '{usage=$2} END {print usage}'");
        cmd_popen(cmd , cmd_result );
        if ((pos = strchr(cmd_result, '\n')) != NULL)   *pos = '\0';
        if( NULL != cmd_result)     COPYSTR(p->total , cmd_result);
        if ( NULL != p->total)
            *value = GS_STRDUP(p->total);
    }
    return CPE_OK;
}
/**@endparam                                               **/

/**@param DeviceInfoMemoryStatus_Free                     **/
CPE_STATUS getDeviceInfoMemoryStatus_Free(Instance *ip, char **value)
{
	DeviceInfoMemoryStatus *p = (DeviceInfoMemoryStatus *)ip->cpeData;
	if ( p ){
        char cmd[128]={0};
        char cmd_result[128]={0};
        char *pos = NULL;
        memset(cmd , 0x0 , sizeof(cmd));
        memset(cmd_result , 0x0 , sizeof(cmd_result));
        sprintf(cmd, "free | grep Mem| awk -F \" \" '{usage=$4} END {print usage}'");
        cmd_popen(cmd , cmd_result );
        if ((pos = strchr(cmd_result, '\n')) != NULL)   *pos = '\0';
        if( NULL != cmd_result)     COPYSTR(p->free , cmd_result);
        if ( NULL != p->free)
            *value = GS_STRDUP(p->free);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@endobj DeviceInfoMemoryStatus **/
