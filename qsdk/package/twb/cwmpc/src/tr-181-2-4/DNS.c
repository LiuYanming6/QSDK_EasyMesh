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
 * $Id: DNS.c,v 1.2 2012/06/13 16:07:50 dmounday Exp $
 *----------------------------------------------------------------------*/

#include "paramTree.h"
#include "rpc.h"
#include "gslib/src/utils.h"
#include "soapRpc/rpcUtils.h"
#include "soapRpc/rpcMethods.h"
#include "soapRpc/cwmpSession.h"

#include "DNS.h"

/**@obj DNSClientServer **/
CPE_STATUS  addDNSClientServer(CWMPObject *o, Instance *ip)
{
	/* add instance data */
	DNSClientServer *p = (DNSClientServer *)GS_MALLOC( sizeof(struct DNSClientServer));
	memset(p, 0, sizeof(struct DNSClientServer));
	ip->cpeData = (void *)p;
	return CPE_OK;
}
CPE_STATUS  delDNSClientServer(CWMPObject *o, Instance *ip)
{
	/* delete instance data */
	DNSClientServer *p = (DNSClientServer *)ip->cpeData;
	if( p ){
		//TODO: free instance data
		GS_FREE(p);
	}
	return CPE_OK;
}

/**@param DNSClientServer_Enable                     **/
CPE_STATUS setDNSClientServer_Enable(Instance *ip, char *value)
{
	DNSClientServer *p = (DNSClientServer *)ip->cpeData;
	if ( p ){
		p->enable=testBoolean(value);
	}
	return CPE_OK;
}
CPE_STATUS getDNSClientServer_Enable(Instance *ip, char **value)
{
	DNSClientServer *p = (DNSClientServer *)ip->cpeData;
	if ( p ){
		*value = GS_STRDUP(p->enable? "true": "false");
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param DNSClientServer_Status                     **/
CPE_STATUS getDNSClientServer_Status(Instance *ip, char **value)
{
	DNSClientServer *p = (DNSClientServer *)ip->cpeData;
	if ( p ){
		if ( p->status )
			*value = GS_STRDUP(p->status);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param DNSClientServer_DNSServer                     **/
CPE_STATUS setDNSClientServer_DNSServer(Instance *ip, char *value)
{
	DNSClientServer *p = (DNSClientServer *)ip->cpeData;
	if ( p ){
		COPYSTR(p->dNSServer, value);
	}
	return CPE_OK;
}
CPE_STATUS getDNSClientServer_DNSServer(Instance *ip, char **value)
{
	DNSClientServer *p = (DNSClientServer *)ip->cpeData;
	if ( p ){
		if ( p->dNSServer )
			*value = GS_STRDUP(p->dNSServer);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param DNSClientServer_Interface                     **/
CPE_STATUS setDNSClientServer_Interface(Instance *ip, char *value)
{
	DNSClientServer *p = (DNSClientServer *)ip->cpeData;
	if ( p ){
		//TODO: resolve Instance pointers.
	}
	return CPE_OK;
}
CPE_STATUS getDNSClientServer_Interface(Instance *ip, char **value)
{
	DNSClientServer *p = (DNSClientServer *)ip->cpeData;
	if ( p ){
		*value = cwmpGetInstancePathStr(p->interface);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param DNSClientServer_Type                     **/
CPE_STATUS getDNSClientServer_Type(Instance *ip, char **value)
{
	DNSClientServer *p = (DNSClientServer *)ip->cpeData;
	if ( p ){
		if ( p->type )
			*value = GS_STRDUP(p->type);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@endobj DNSClientServer **/

/**@obj DNSClient **/

/**@param DNSClient_Enable                     **/
CPE_STATUS setDNSClient_Enable(Instance *ip, char *value)
{
	GS_DNSClient *p = (GS_DNSClient *)ip->cpeData;
	if ( p ){
		p->enable=testBoolean(value);
	}
	return CPE_OK;
}
CPE_STATUS getDNSClient_Enable(Instance *ip, char **value)
{
	GS_DNSClient *p = (GS_DNSClient *)ip->cpeData;
	if ( p ){
		*value = GS_STRDUP(p->enable? "true": "false");
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param DNSClient_Status                     **/
CPE_STATUS getDNSClient_Status(Instance *ip, char **value)
{
	GS_DNSClient *p = (GS_DNSClient *)ip->cpeData;
	if ( p ){
		if ( p->status )
			*value = GS_STRDUP(p->status);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param DNSClient_ServerNumberOfEntries                     **/
CPE_STATUS getDNSClient_ServerNumberOfEntries(Instance *ip, char **value)
{
	cwmpGetObjInstanceCntStr("Server", value);
	return CPE_OK;
}
/**@endparam                                               **/
/**@endobj DNSClient **/

/**@obj DNS **/

/**@param DNS_SupportedRecordTypes                     **/
CPE_STATUS getDNS_SupportedRecordTypes(Instance *ip, char **value)
{
	GS_DNS *p = (GS_DNS *)ip->cpeData;
	if ( p ){
		if ( p->supportedRecordTypes )
			*value = GS_STRDUP(p->supportedRecordTypes);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@endobj DNS **/












