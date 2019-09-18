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

#include "DeviceInfoProcessStatus.h"

/*-------------------------------------------------------------------------*/
/*                           DEFINITIONS                                   */
/*-------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------*/
/*                           VARIABLES                                     */
/*-------------------------------------------------------------------------*/


/*-------------------------------------------------------------------------*/
/*                           FUNCTIONS                                     */
/*-------------------------------------------------------------------------*/

/**@obj DeviceInfoProcessStatus **/
CPE_STATUS  initDeviceInfoProcessStatus(CWMPObject *o, Instance *ip)
{
	/* initialize object */
	DeviceInfoProcessStatus *p = (DeviceInfoProcessStatus *)GS_MALLOC( sizeof(struct DeviceInfoProcessStatus));
	memset(p, 0, sizeof(struct DeviceInfoProcessStatus));
	ip->cpeData = (void *)p;
	return CPE_OK;
}

/**@param DeviceInfoProcessStatus_CPUUsage                     **/
CPE_STATUS getDeviceInfoProcessStatus_CPUUsage(Instance *ip, char **value)
{
	DeviceInfoProcessStatus *p = (DeviceInfoProcessStatus *)ip->cpeData;
	if ( p ){
        char cmd[128]={0};
        char cmd_result[128]={0};
        char *pos = NULL;
        memset(cmd , 0x0 , sizeof(cmd));
        memset(cmd_result , 0x0 , sizeof(cmd_result));
        sprintf(cmd, "top -b -n1 | grep 'CPU' | awk '{print $2}' | head -n 1 | tr -d '%%'");
        cmd_popen(cmd , cmd_result );
        if ((pos = strchr(cmd_result, '\n')) != NULL)   *pos = '\0';
        if( NULL != cmd_result)     COPYSTR(p->cPUUsage , cmd_result);
        if (p->cPUUsage)
            *value = GS_STRDUP(p->cPUUsage);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@endobj DeviceInfoProcessStatus **/
