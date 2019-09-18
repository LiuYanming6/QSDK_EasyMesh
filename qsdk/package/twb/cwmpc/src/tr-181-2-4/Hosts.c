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
 * $Id: Hosts.c,v 1.2 2012/06/13 16:07:50 dmounday Exp $
 *----------------------------------------------------------------------*/

#include "paramTree.h"
#include "rpc.h"
#include "gslib/src/utils.h"
#include "soapRpc/rpcUtils.h"
#include "soapRpc/rpcMethods.h"
#include "soapRpc/cwmpSession.h"

#include "Hosts.h"

/**@obj HostsHost **/
CPE_STATUS  addHostsHost(CWMPObject *o, Instance *ip)
{
	/* add instance data */
	GS_HostsHost *p = (GS_HostsHost *)GS_MALLOC( sizeof(struct GS_HostsHost));
	memset(p, 0, sizeof(struct GS_HostsHost));
	ip->cpeData = (void *)p;
	return CPE_OK;
}
CPE_STATUS  delHostsHost(CWMPObject *o, Instance *ip)
{
	/* delete instance data */
	GS_HostsHost *p = (GS_HostsHost *)ip->cpeData;
	if( p ){
		//TODO: free instance data
		GS_FREE(p);
	}
	return CPE_OK;
}

/**@param HostsHost_PhysAddress                     **/
CPE_STATUS getHostsHost_PhysAddress(Instance *ip, char **value)
{
	GS_HostsHost *p = (GS_HostsHost *)ip->cpeData;
	if ( p ){
		if ( p->physAddress )
			*value = GS_STRDUP(p->physAddress);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param HostsHost_IPAddress                     **/
CPE_STATUS getHostsHost_IPAddress(Instance *ip, char **value)
{
	GS_HostsHost *p = (GS_HostsHost *)ip->cpeData;
	if ( p ){
		if ( p->iPAddress )
			*value = GS_STRDUP(p->iPAddress);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param HostsHost_AddressSource                     **/
CPE_STATUS getHostsHost_AddressSource(Instance *ip, char **value)
{
	GS_HostsHost *p = (GS_HostsHost *)ip->cpeData;
	if ( p ){
		if ( p->addressSource )
			*value = GS_STRDUP(p->addressSource);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param HostsHost_LeaseTimeRemaining                     **/
CPE_STATUS getHostsHost_LeaseTimeRemaining(Instance *ip, char **value)
{
	GS_HostsHost *p = (GS_HostsHost *)ip->cpeData;
	if ( p ){
		char    buf[10];
		snprintf(buf,sizeof(buf),"%d", p->leaseTimeRemaining);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param HostsHost_Layer1Interface                     **/
CPE_STATUS getHostsHost_Layer1Interface(Instance *ip, char **value)
{
	GS_HostsHost *p = (GS_HostsHost *)ip->cpeData;
	if ( p ){
		*value = cwmpGetInstancePathStr(p->layer1Interface);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param HostsHost_Layer3Interface                     **/
CPE_STATUS getHostsHost_Layer3Interface(Instance *ip, char **value)
{
	GS_HostsHost *p = (GS_HostsHost *)ip->cpeData;
	if ( p ){
		*value = cwmpGetInstancePathStr(p->layer3Interface);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param HostsHost_HostName                     **/
CPE_STATUS getHostsHost_HostName(Instance *ip, char **value)
{
	GS_HostsHost *p = (GS_HostsHost *)ip->cpeData;
	if ( p ){
		if ( p->hostName )
			*value = GS_STRDUP(p->hostName);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param HostsHost_Active                     **/
CPE_STATUS getHostsHost_Active(Instance *ip, char **value)
{
	GS_HostsHost *p = (GS_HostsHost *)ip->cpeData;
	if ( p ){
		*value = GS_STRDUP(p->active? "true": "false");
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@endobj HostsHost **/

/**@obj Hosts **/

/**@param Hosts_HostNumberOfEntries                     **/
CPE_STATUS getHosts_HostNumberOfEntries(Instance *ip, char **value)
{
	cwmpGetObjInstanceCntStr("Host", value);
	return CPE_OK;
}
/**@endparam                                               **/
/**@endobj Hosts **/












