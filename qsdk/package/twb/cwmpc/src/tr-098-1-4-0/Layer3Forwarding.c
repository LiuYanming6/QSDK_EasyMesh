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

#include "Layer3Forwarding.h"

/**@obj Layer3ForwardingForwarding **/
CPE_STATUS  addLayer3ForwardingForwarding(CWMPObject *o, Instance *ip)
{
	/* add instance data */
	Layer3ForwardingForwarding *p = (Layer3ForwardingForwarding *)GS_MALLOC( sizeof(struct Layer3ForwardingForwarding));
	memset(p, 0, sizeof(struct Layer3ForwardingForwarding));
	ip->cpeData = (void *)p;
	return CPE_OK;
}
CPE_STATUS  delLayer3ForwardingForwarding(CWMPObject *o, Instance *ip)
{
	/* delete instance data */
	Layer3ForwardingForwarding *p = (Layer3ForwardingForwarding *)ip->cpeData;
	if( p ){
		//free instance data
		GS_FREE(p->status);
		GS_FREE(p->type);
		GS_FREE(p->destIPAddress);
		GS_FREE(p->destSubnetMask);
		GS_FREE(p->sourceIPAddress);
		GS_FREE(p->sourceSubnetMask);
		GS_FREE(p->gatewayIPAddress);
		GS_FREE(p);
	}
	return CPE_OK;
}
CPE_STATUS  commitLayer3ForwardingForwarding(CWMPObject *o, Instance *ip, eCommitCBType cmt)
{
	/* commit object instance */
	/* build route command and run it here */
	return CPE_OK;
}

/**@param Layer3ForwardingForwarding_Enable                     **/
CPE_STATUS setLayer3ForwardingForwarding_Enable(Instance *ip, char *value)
{
	Layer3ForwardingForwarding *p = (Layer3ForwardingForwarding *)ip->cpeData;
	if ( p ){
		p->enable=testBoolean(value);
	}
	return CPE_OK;
}
CPE_STATUS getLayer3ForwardingForwarding_Enable(Instance *ip, char **value)
{
	Layer3ForwardingForwarding *p = (Layer3ForwardingForwarding *)ip->cpeData;
	if ( p ){
		*value = GS_STRDUP(p->enable? "true": "false");
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param Layer3ForwardingForwarding_Status                     **/
CPE_STATUS getLayer3ForwardingForwarding_Status(Instance *ip, char **value)
{
	Layer3ForwardingForwarding *p = (Layer3ForwardingForwarding *)ip->cpeData;
	if ( p ){
		if ( p->status )
			*value = GS_STRDUP(p->status);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param Layer3ForwardingForwarding_Type                     **/
CPE_STATUS setLayer3ForwardingForwarding_Type(Instance *ip, char *value)
{
	Layer3ForwardingForwarding *p = (Layer3ForwardingForwarding *)ip->cpeData;
	if ( p ){
		COPYSTR(p->type, value);
	}
	return CPE_OK;
}
CPE_STATUS getLayer3ForwardingForwarding_Type(Instance *ip, char **value)
{
	Layer3ForwardingForwarding *p = (Layer3ForwardingForwarding *)ip->cpeData;
	if ( p ){
		if ( p->type )
			*value = GS_STRDUP(p->type);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param Layer3ForwardingForwarding_DestIPAddress                     **/
CPE_STATUS setLayer3ForwardingForwarding_DestIPAddress(Instance *ip, char *value)
{
	Layer3ForwardingForwarding *p = (Layer3ForwardingForwarding *)ip->cpeData;
	if ( p ){
		COPYSTR(p->destIPAddress, value);
	}
	return CPE_OK;
}
CPE_STATUS getLayer3ForwardingForwarding_DestIPAddress(Instance *ip, char **value)
{
	Layer3ForwardingForwarding *p = (Layer3ForwardingForwarding *)ip->cpeData;
	if ( p ){
		if ( p->destIPAddress )
			*value = GS_STRDUP(p->destIPAddress);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param Layer3ForwardingForwarding_DestSubnetMask                     **/
CPE_STATUS setLayer3ForwardingForwarding_DestSubnetMask(Instance *ip, char *value)
{
	Layer3ForwardingForwarding *p = (Layer3ForwardingForwarding *)ip->cpeData;
	if ( p ){
		COPYSTR(p->destSubnetMask, value);
	}
	return CPE_OK;
}
CPE_STATUS getLayer3ForwardingForwarding_DestSubnetMask(Instance *ip, char **value)
{
	Layer3ForwardingForwarding *p = (Layer3ForwardingForwarding *)ip->cpeData;
	if ( p ){
		if ( p->destSubnetMask )
			*value = GS_STRDUP(p->destSubnetMask);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param Layer3ForwardingForwarding_SourceIPAddress                     **/
CPE_STATUS setLayer3ForwardingForwarding_SourceIPAddress(Instance *ip, char *value)
{
	Layer3ForwardingForwarding *p = (Layer3ForwardingForwarding *)ip->cpeData;
	if ( p ){
		COPYSTR(p->sourceIPAddress, value);
	}
	return CPE_OK;
}
CPE_STATUS getLayer3ForwardingForwarding_SourceIPAddress(Instance *ip, char **value)
{
	Layer3ForwardingForwarding *p = (Layer3ForwardingForwarding *)ip->cpeData;
	if ( p ){
		if ( p->sourceIPAddress )
			*value = GS_STRDUP(p->sourceIPAddress);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param Layer3ForwardingForwarding_SourceSubnetMask                     **/
CPE_STATUS setLayer3ForwardingForwarding_SourceSubnetMask(Instance *ip, char *value)
{
	Layer3ForwardingForwarding *p = (Layer3ForwardingForwarding *)ip->cpeData;
	if ( p ){
		COPYSTR(p->sourceSubnetMask, value);
	}
	return CPE_OK;
}
CPE_STATUS getLayer3ForwardingForwarding_SourceSubnetMask(Instance *ip, char **value)
{
	Layer3ForwardingForwarding *p = (Layer3ForwardingForwarding *)ip->cpeData;
	if ( p ){
		if ( p->sourceSubnetMask )
			*value = GS_STRDUP(p->sourceSubnetMask);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param Layer3ForwardingForwarding_GatewayIPAddress                     **/
CPE_STATUS setLayer3ForwardingForwarding_GatewayIPAddress(Instance *ip, char *value)
{
	Layer3ForwardingForwarding *p = (Layer3ForwardingForwarding *)ip->cpeData;
	if ( p ){
		COPYSTR(p->gatewayIPAddress, value);
	}
	return CPE_OK;
}
CPE_STATUS getLayer3ForwardingForwarding_GatewayIPAddress(Instance *ip, char **value)
{
	Layer3ForwardingForwarding *p = (Layer3ForwardingForwarding *)ip->cpeData;
	if ( p ){
		if ( p->gatewayIPAddress )
			*value = GS_STRDUP(p->gatewayIPAddress);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param Layer3ForwardingForwarding_Interface                     **/
CPE_STATUS setLayer3ForwardingForwarding_Interface(Instance *ip, char *value)
{
	Layer3ForwardingForwarding *p = (Layer3ForwardingForwarding *)ip->cpeData;
	if ( p ){
		//TODO: resolve Instance pointers.
	}
	return CPE_OK;
}
CPE_STATUS getLayer3ForwardingForwarding_Interface(Instance *ip, char **value)
{
	Layer3ForwardingForwarding *p = (Layer3ForwardingForwarding *)ip->cpeData;
	if ( p ){
		*value = cwmpGetInstancePathStr(p->interface);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param Layer3ForwardingForwarding_ForwardingMetric                     **/
CPE_STATUS setLayer3ForwardingForwarding_ForwardingMetric(Instance *ip, char *value)
{
	Layer3ForwardingForwarding *p = (Layer3ForwardingForwarding *)ip->cpeData;
	if ( p ){
		p->forwardingMetric=atoi(value);
	}
	return CPE_OK;
}
CPE_STATUS getLayer3ForwardingForwarding_ForwardingMetric(Instance *ip, char **value)
{
	Layer3ForwardingForwarding *p = (Layer3ForwardingForwarding *)ip->cpeData;
	if ( p ){
		char    buf[10];
		snprintf(buf,sizeof(buf),"%d", p->forwardingMetric);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@endobj Layer3ForwardingForwarding **/

/**@obj Layer3Forwarding **/
CPE_STATUS  initLayer3Forwarding(CWMPObject *o, Instance *ip)
{
	/* initialize object */
	Layer3Forwarding *p = (Layer3Forwarding *)GS_MALLOC( sizeof(struct Layer3Forwarding));
	memset(p, 0, sizeof(struct Layer3Forwarding));
	ip->cpeData = (void *)p;
	return CPE_OK;
}

/**@param Layer3Forwarding_DefaultConnectionService                     **/
CPE_STATUS setLayer3Forwarding_DefaultConnectionService(Instance *ip, char *value)
{
	Layer3Forwarding *p = (Layer3Forwarding *)ip->cpeData;
	if ( p ){
		COPYSTR(p->defaultConnectionService, value);
	}
	return CPE_OK;
}
CPE_STATUS getLayer3Forwarding_DefaultConnectionService(Instance *ip, char **value)
{
	Layer3Forwarding *p = (Layer3Forwarding *)ip->cpeData;
	if ( p ){
		if ( p->defaultConnectionService )
			*value = GS_STRDUP(p->defaultConnectionService);
	}
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





