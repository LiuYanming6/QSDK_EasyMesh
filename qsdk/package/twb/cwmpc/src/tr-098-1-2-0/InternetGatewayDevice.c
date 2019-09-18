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
 * $Id: InternetGatewayDevice.c,v 1.1 2012/05/10 17:37:59 dmounday Exp $
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
	*value = GS_STRDUP("UDPConnReq:1, TraceRoute:1, EthernetLAN:2, EthernetLAN:1, IPPing:1,"
    "ADSLWAN:1, DeviceAssociation:2, DHCPOption:1, DeviceAssociation:1, Time:1,"
    "WiFiLAN:2, Time:2, EthernetWAN:1, WiFiLAN:1, Bridging:2,"
    "Bridging:1, Baseline:2, Baseline:1");
	return CPE_OK;
}
/**@endparam                                               **/
