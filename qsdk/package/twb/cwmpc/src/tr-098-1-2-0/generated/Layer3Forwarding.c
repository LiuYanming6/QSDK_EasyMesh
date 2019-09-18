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
 * $Id: Layer3Forwarding.c,v 1.1 2012/05/10 17:38:00 dmounday Exp $
 *----------------------------------------------------------------------*/

#include "paramTree.h"
#include "rpc.h"
#include "gslib/src/utils.h"
#include "soapRpc/rpcUtils.h"
#include "soapRpc/rpcMethods.h"
#include "soapRpc/cwmpSession.h"


/**@obj Layer3ForwardingForwarding **/
CPE_STATUS  addLayer3ForwardingForwarding(CWMPObject *o, Instance *ip)
{
	/* add instance data */
	return CPE_OK;
}
CPE_STATUS  delLayer3ForwardingForwarding(CWMPObject *o, Instance *ip)
{
	/* delete instance data */
	return CPE_OK;
}
CPE_STATUS  commitLayer3ForwardingForwarding(CWMPObject *o, Instance *ip, eCommitCBType cmt)
{
	/* commit object instance */
	return CPE_OK;
}

/**@param Layer3ForwardingForwarding_Enable                     **/
CPE_STATUS setLayer3ForwardingForwarding_Enable(Instance *ip, char *value)
{
	/* set parameter */
	return CPE_OK;
}
CPE_STATUS getLayer3ForwardingForwarding_Enable(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/

/**@param Layer3ForwardingForwarding_Status                     **/
CPE_STATUS getLayer3ForwardingForwarding_Status(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/

/**@param Layer3ForwardingForwarding_Type                     **/
CPE_STATUS setLayer3ForwardingForwarding_Type(Instance *ip, char *value)
{
	/* set parameter */
	return CPE_OK;
}
CPE_STATUS getLayer3ForwardingForwarding_Type(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/

/**@param Layer3ForwardingForwarding_DestIPAddress                     **/
CPE_STATUS setLayer3ForwardingForwarding_DestIPAddress(Instance *ip, char *value)
{
	/* set parameter */
	return CPE_OK;
}
CPE_STATUS getLayer3ForwardingForwarding_DestIPAddress(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/

/**@param Layer3ForwardingForwarding_DestSubnetMask                     **/
CPE_STATUS setLayer3ForwardingForwarding_DestSubnetMask(Instance *ip, char *value)
{
	/* set parameter */
	return CPE_OK;
}
CPE_STATUS getLayer3ForwardingForwarding_DestSubnetMask(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/

/**@param Layer3ForwardingForwarding_SourceIPAddress                     **/
CPE_STATUS setLayer3ForwardingForwarding_SourceIPAddress(Instance *ip, char *value)
{
	/* set parameter */
	return CPE_OK;
}
CPE_STATUS getLayer3ForwardingForwarding_SourceIPAddress(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/

/**@param Layer3ForwardingForwarding_SourceSubnetMask                     **/
CPE_STATUS setLayer3ForwardingForwarding_SourceSubnetMask(Instance *ip, char *value)
{
	/* set parameter */
	return CPE_OK;
}
CPE_STATUS getLayer3ForwardingForwarding_SourceSubnetMask(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/

/**@param Layer3ForwardingForwarding_GatewayIPAddress                     **/
CPE_STATUS setLayer3ForwardingForwarding_GatewayIPAddress(Instance *ip, char *value)
{
	/* set parameter */
	return CPE_OK;
}
CPE_STATUS getLayer3ForwardingForwarding_GatewayIPAddress(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/

/**@param Layer3ForwardingForwarding_Interface                     **/
CPE_STATUS setLayer3ForwardingForwarding_Interface(Instance *ip, char *value)
{
	/* set parameter */
	return CPE_OK;
}
CPE_STATUS getLayer3ForwardingForwarding_Interface(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/

/**@param Layer3ForwardingForwarding_ForwardingMetric                     **/
CPE_STATUS setLayer3ForwardingForwarding_ForwardingMetric(Instance *ip, char *value)
{
	/* set parameter */
	return CPE_OK;
}
CPE_STATUS getLayer3ForwardingForwarding_ForwardingMetric(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/

/**@param Layer3ForwardingForwarding_StaticRoute                     **/
CPE_STATUS getLayer3ForwardingForwarding_StaticRoute(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@endobj Layer3ForwardingForwarding **/

/**@obj Layer3Forwarding **/

/**@param Layer3Forwarding_DefaultConnectionService                     **/
CPE_STATUS setLayer3Forwarding_DefaultConnectionService(Instance *ip, char *value)
{
	/* set parameter */
	return CPE_OK;
}
CPE_STATUS getLayer3Forwarding_DefaultConnectionService(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/

/**@param Layer3Forwarding_ForwardNumberOfEntries                     **/
CPE_STATUS getLayer3Forwarding_ForwardNumberOfEntries(Instance *ip, char **value)
{
	cwmpGetObjInstanceCntStr("Forwarding", value);
	return CPE_OK;
}
/**@endparam                                               **/
/**@endobj Layer3Forwarding **/
