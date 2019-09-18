/*----------------------------------------------------------------------*
 * Gatespace
 * Copyright 2011, 2012 Gatespace Networks, Inc., All Rights Reserved.
 * Gatespace Networks, Inc. confidential material.
 *----------------------------------------------------------------------*
 * File Name  :
 * Description: Auto-generated getter/setter stubs file.
 *----------------------------------------------------------------------*
 * $Revision: 1.1 $
 *
 * $Id: InternetGatewayDevice.c,v 1.1 2012/05/10 17:38:08 dmounday Exp $
 *----------------------------------------------------------------------*/

#include "paramTree.h"
#include "rpc.h"
#include "gslib/src/utils.h"
#include "soapRpc/rpcUtils.h"
#include "soapRpc/rpcMethods.h"
#include "soapRpc/cwmpSession.h"


/**@param _LANDeviceNumberOfEntries                     **/
CPE_STATUS get_LANDeviceNumberOfEntries(Instance *ip, char **value)
{
	cwmpGetObjInstanceCntStr("LANDevice", value);
	return CPE_OK;
}
/**@endparam                                               **/

/**@param _WANDeviceNumberOfEntries                     **/
CPE_STATUS get_WANDeviceNumberOfEntries(Instance *ip, char **value)
{
	cwmpGetObjInstanceCntStr("WANDevice", value);
	return CPE_OK;
}
/**@endparam                                               **/

/**@param _DeviceSummary                     **/
CPE_STATUS get_DeviceSummary(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
