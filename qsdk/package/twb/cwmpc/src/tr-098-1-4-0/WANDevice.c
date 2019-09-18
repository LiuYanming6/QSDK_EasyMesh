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
 * $Id: WANDevice.c,v 1.1 2012/05/10 17:38:00 dmounday Exp $
 *----------------------------------------------------------------------*/

#include "paramTree.h"
#include "rpc.h"
#include "gslib/src/utils.h"
#include "soapRpc/rpcUtils.h"
#include "soapRpc/rpcMethods.h"
#include "soapRpc/cwmpSession.h"

#include "WANDevice.h"

/**@obj WANDeviceWANCommonInterfaceConfig **/

/**@param WANDeviceWANCommonInterfaceConfig_EnabledForInternet                     **/
CPE_STATUS setWANDeviceWANCommonInterfaceConfig_EnabledForInternet(Instance *ip, char *value)
{
	WANDeviceWANCommonInterfaceConfig *p = (WANDeviceWANCommonInterfaceConfig *)ip->cpeData;
	if ( p ){
		p->enabledForInternet=testBoolean(value);
	}
	return CPE_OK;
}
CPE_STATUS getWANDeviceWANCommonInterfaceConfig_EnabledForInternet(Instance *ip, char **value)
{
	WANDeviceWANCommonInterfaceConfig *p = (WANDeviceWANCommonInterfaceConfig *)ip->cpeData;
	if ( p ){
		*value = GS_STRDUP(p->enabledForInternet? "true": "false");
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANCommonInterfaceConfig_WANAccessType                     **/
CPE_STATUS getWANDeviceWANCommonInterfaceConfig_WANAccessType(Instance *ip, char **value)
{
	WANDeviceWANCommonInterfaceConfig *p = (WANDeviceWANCommonInterfaceConfig *)ip->cpeData;
	if ( p ){
		if ( p->wANAccessType )
			*value = GS_STRDUP(p->wANAccessType);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANCommonInterfaceConfig_Layer1UpstreamMaxBitRate                     **/
CPE_STATUS getWANDeviceWANCommonInterfaceConfig_Layer1UpstreamMaxBitRate(Instance *ip, char **value)
{
	WANDeviceWANCommonInterfaceConfig *p = (WANDeviceWANCommonInterfaceConfig *)ip->cpeData;
	if ( p ){
		char    buf[10];
		snprintf(buf,sizeof(buf),"%u", p->layer1UpstreamMaxBitRate);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANCommonInterfaceConfig_Layer1DownstreamMaxBitRate                     **/
CPE_STATUS getWANDeviceWANCommonInterfaceConfig_Layer1DownstreamMaxBitRate(Instance *ip, char **value)
{
	WANDeviceWANCommonInterfaceConfig *p = (WANDeviceWANCommonInterfaceConfig *)ip->cpeData;
	if ( p ){
		char    buf[10];
		snprintf(buf,sizeof(buf),"%u", p->layer1DownstreamMaxBitRate);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANCommonInterfaceConfig_PhysicalLinkStatus                     **/
CPE_STATUS getWANDeviceWANCommonInterfaceConfig_PhysicalLinkStatus(Instance *ip, char **value)
{
	WANDeviceWANCommonInterfaceConfig *p = (WANDeviceWANCommonInterfaceConfig *)ip->cpeData;
	if ( p ){
		if ( p->physicalLinkStatus )
			*value = GS_STRDUP(p->physicalLinkStatus);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANCommonInterfaceConfig_TotalBytesSent                     **/
CPE_STATUS getWANDeviceWANCommonInterfaceConfig_TotalBytesSent(Instance *ip, char **value)
{
	WANDeviceWANCommonInterfaceConfig *p = (WANDeviceWANCommonInterfaceConfig *)ip->cpeData;
	if ( p ){
		char    buf[10];
		snprintf(buf,sizeof(buf),"%u", p->totalBytesSent);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANCommonInterfaceConfig_TotalBytesReceived                     **/
CPE_STATUS getWANDeviceWANCommonInterfaceConfig_TotalBytesReceived(Instance *ip, char **value)
{
	WANDeviceWANCommonInterfaceConfig *p = (WANDeviceWANCommonInterfaceConfig *)ip->cpeData;
	if ( p ){
		char    buf[10];
		snprintf(buf,sizeof(buf),"%u", p->totalBytesReceived);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANCommonInterfaceConfig_TotalPacketsSent                     **/
CPE_STATUS getWANDeviceWANCommonInterfaceConfig_TotalPacketsSent(Instance *ip, char **value)
{
	WANDeviceWANCommonInterfaceConfig *p = (WANDeviceWANCommonInterfaceConfig *)ip->cpeData;
	if ( p ){
		char    buf[10];
		snprintf(buf,sizeof(buf),"%u", p->totalPacketsSent);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANCommonInterfaceConfig_TotalPacketsReceived                     **/
CPE_STATUS getWANDeviceWANCommonInterfaceConfig_TotalPacketsReceived(Instance *ip, char **value)
{
	WANDeviceWANCommonInterfaceConfig *p = (WANDeviceWANCommonInterfaceConfig *)ip->cpeData;
	if ( p ){
		char    buf[10];
		snprintf(buf,sizeof(buf),"%u", p->totalPacketsReceived);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@endobj WANDeviceWANCommonInterfaceConfig **/

/**@obj WANDeviceWANConnectionDeviceWANIPConnectionPortMapping **/
CPE_STATUS  addWANDeviceWANConnectionDeviceWANIPConnectionPortMapping(CWMPObject *o, Instance *ip)
{
	/* add instance data */
	WANDeviceWANConnectionDeviceWANIPConnectionPortMapping *p = (WANDeviceWANConnectionDeviceWANIPConnectionPortMapping *)GS_MALLOC( sizeof(struct WANDeviceWANConnectionDeviceWANIPConnectionPortMapping));
	memset(p, 0, sizeof(struct WANDeviceWANConnectionDeviceWANIPConnectionPortMapping));
	ip->cpeData = (void *)p;
	return CPE_OK;
}
CPE_STATUS  delWANDeviceWANConnectionDeviceWANIPConnectionPortMapping(CWMPObject *o, Instance *ip)
{
	/* delete instance data */
	WANDeviceWANConnectionDeviceWANIPConnectionPortMapping *p = (WANDeviceWANConnectionDeviceWANIPConnectionPortMapping *)ip->cpeData;
	if( p ){
		//TODO: free instance data
		GS_FREE(p);
	}
	return CPE_OK;
}

/**@param WANDeviceWANConnectionDeviceWANIPConnectionPortMapping_PortMappingEnabled                     **/
CPE_STATUS setWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_PortMappingEnabled(Instance *ip, char *value)
{
	WANDeviceWANConnectionDeviceWANIPConnectionPortMapping *p = (WANDeviceWANConnectionDeviceWANIPConnectionPortMapping *)ip->cpeData;
	if ( p ){
		p->portMappingEnabled=testBoolean(value);
	}
	return CPE_OK;
}
CPE_STATUS getWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_PortMappingEnabled(Instance *ip, char **value)
{
	WANDeviceWANConnectionDeviceWANIPConnectionPortMapping *p = (WANDeviceWANConnectionDeviceWANIPConnectionPortMapping *)ip->cpeData;
	if ( p ){
		*value = GS_STRDUP(p->portMappingEnabled? "true": "false");
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANIPConnectionPortMapping_PortMappingLeaseDuration                     **/
CPE_STATUS setWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_PortMappingLeaseDuration(Instance *ip, char *value)
{
	WANDeviceWANConnectionDeviceWANIPConnectionPortMapping *p = (WANDeviceWANConnectionDeviceWANIPConnectionPortMapping *)ip->cpeData;
	if ( p ){
		p->portMappingLeaseDuration=atoi(value);
	}
	return CPE_OK;
}
CPE_STATUS getWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_PortMappingLeaseDuration(Instance *ip, char **value)
{
	WANDeviceWANConnectionDeviceWANIPConnectionPortMapping *p = (WANDeviceWANConnectionDeviceWANIPConnectionPortMapping *)ip->cpeData;
	if ( p ){
		char    buf[10];
		snprintf(buf,sizeof(buf),"%u", p->portMappingLeaseDuration);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANIPConnectionPortMapping_RemoteHost                     **/
CPE_STATUS setWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_RemoteHost(Instance *ip, char *value)
{
	WANDeviceWANConnectionDeviceWANIPConnectionPortMapping *p = (WANDeviceWANConnectionDeviceWANIPConnectionPortMapping *)ip->cpeData;
	if ( p ){
		COPYSTR(p->remoteHost, value);
	}
	return CPE_OK;
}
CPE_STATUS getWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_RemoteHost(Instance *ip, char **value)
{
	WANDeviceWANConnectionDeviceWANIPConnectionPortMapping *p = (WANDeviceWANConnectionDeviceWANIPConnectionPortMapping *)ip->cpeData;
	if ( p ){
		if ( p->remoteHost )
			*value = GS_STRDUP(p->remoteHost);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANIPConnectionPortMapping_ExternalPort                     **/
CPE_STATUS setWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_ExternalPort(Instance *ip, char *value)
{
	WANDeviceWANConnectionDeviceWANIPConnectionPortMapping *p = (WANDeviceWANConnectionDeviceWANIPConnectionPortMapping *)ip->cpeData;
	if ( p ){
		p->externalPort=atoi(value);
	}
	return CPE_OK;
}
CPE_STATUS getWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_ExternalPort(Instance *ip, char **value)
{
	WANDeviceWANConnectionDeviceWANIPConnectionPortMapping *p = (WANDeviceWANConnectionDeviceWANIPConnectionPortMapping *)ip->cpeData;
	if ( p ){
		char    buf[10];
		snprintf(buf,sizeof(buf),"%u", p->externalPort);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANIPConnectionPortMapping_InternalPort                     **/
CPE_STATUS setWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_InternalPort(Instance *ip, char *value)
{
	WANDeviceWANConnectionDeviceWANIPConnectionPortMapping *p = (WANDeviceWANConnectionDeviceWANIPConnectionPortMapping *)ip->cpeData;
	if ( p ){
		p->internalPort=atoi(value);
	}
	return CPE_OK;
}
CPE_STATUS getWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_InternalPort(Instance *ip, char **value)
{
	WANDeviceWANConnectionDeviceWANIPConnectionPortMapping *p = (WANDeviceWANConnectionDeviceWANIPConnectionPortMapping *)ip->cpeData;
	if ( p ){
		char    buf[10];
		snprintf(buf,sizeof(buf),"%u", p->internalPort);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANIPConnectionPortMapping_PortMappingProtocol                     **/
CPE_STATUS setWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_PortMappingProtocol(Instance *ip, char *value)
{
	WANDeviceWANConnectionDeviceWANIPConnectionPortMapping *p = (WANDeviceWANConnectionDeviceWANIPConnectionPortMapping *)ip->cpeData;
	if ( p ){
		COPYSTR(p->portMappingProtocol, value);
	}
	return CPE_OK;
}
CPE_STATUS getWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_PortMappingProtocol(Instance *ip, char **value)
{
	WANDeviceWANConnectionDeviceWANIPConnectionPortMapping *p = (WANDeviceWANConnectionDeviceWANIPConnectionPortMapping *)ip->cpeData;
	if ( p ){
		if ( p->portMappingProtocol )
			*value = GS_STRDUP(p->portMappingProtocol);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANIPConnectionPortMapping_InternalClient                     **/
CPE_STATUS setWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_InternalClient(Instance *ip, char *value)
{
	WANDeviceWANConnectionDeviceWANIPConnectionPortMapping *p = (WANDeviceWANConnectionDeviceWANIPConnectionPortMapping *)ip->cpeData;
	if ( p ){
		COPYSTR(p->internalClient, value);
	}
	return CPE_OK;
}
CPE_STATUS getWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_InternalClient(Instance *ip, char **value)
{
	WANDeviceWANConnectionDeviceWANIPConnectionPortMapping *p = (WANDeviceWANConnectionDeviceWANIPConnectionPortMapping *)ip->cpeData;
	if ( p ){
		if ( p->internalClient )
			*value = GS_STRDUP(p->internalClient);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANIPConnectionPortMapping_PortMappingDescription                     **/
CPE_STATUS setWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_PortMappingDescription(Instance *ip, char *value)
{
	WANDeviceWANConnectionDeviceWANIPConnectionPortMapping *p = (WANDeviceWANConnectionDeviceWANIPConnectionPortMapping *)ip->cpeData;
	if ( p ){
		COPYSTR(p->portMappingDescription, value);
	}
	return CPE_OK;
}
CPE_STATUS getWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_PortMappingDescription(Instance *ip, char **value)
{
	WANDeviceWANConnectionDeviceWANIPConnectionPortMapping *p = (WANDeviceWANConnectionDeviceWANIPConnectionPortMapping *)ip->cpeData;
	if ( p ){
		if ( p->portMappingDescription )
			*value = GS_STRDUP(p->portMappingDescription);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@endobj WANDeviceWANConnectionDeviceWANIPConnectionPortMapping **/

/**@obj WANDeviceWANConnectionDeviceWANIPConnectionStats **/

/**@param WANDeviceWANConnectionDeviceWANIPConnectionStats_EthernetBytesSent                     **/
CPE_STATUS getWANDeviceWANConnectionDeviceWANIPConnectionStats_EthernetBytesSent(Instance *ip, char **value)
{
	WANDeviceWANConnectionDeviceWANIPConnectionStats *p = (WANDeviceWANConnectionDeviceWANIPConnectionStats *)ip->cpeData;
	if ( p ){
		char    buf[10];
		snprintf(buf,sizeof(buf),"%u", p->ethernetBytesSent);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANIPConnectionStats_EthernetBytesReceived                     **/
CPE_STATUS getWANDeviceWANConnectionDeviceWANIPConnectionStats_EthernetBytesReceived(Instance *ip, char **value)
{
	WANDeviceWANConnectionDeviceWANIPConnectionStats *p = (WANDeviceWANConnectionDeviceWANIPConnectionStats *)ip->cpeData;
	if ( p ){
		char    buf[10];
		snprintf(buf,sizeof(buf),"%u", p->ethernetBytesReceived);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANIPConnectionStats_EthernetPacketsSent                     **/
CPE_STATUS getWANDeviceWANConnectionDeviceWANIPConnectionStats_EthernetPacketsSent(Instance *ip, char **value)
{
	WANDeviceWANConnectionDeviceWANIPConnectionStats *p = (WANDeviceWANConnectionDeviceWANIPConnectionStats *)ip->cpeData;
	if ( p ){
		char    buf[10];
		snprintf(buf,sizeof(buf),"%u", p->ethernetPacketsSent);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANIPConnectionStats_EthernetPacketsReceived                     **/
CPE_STATUS getWANDeviceWANConnectionDeviceWANIPConnectionStats_EthernetPacketsReceived(Instance *ip, char **value)
{
	WANDeviceWANConnectionDeviceWANIPConnectionStats *p = (WANDeviceWANConnectionDeviceWANIPConnectionStats *)ip->cpeData;
	if ( p ){
		char    buf[10];
		snprintf(buf,sizeof(buf),"%u", p->ethernetPacketsReceived);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@endobj WANDeviceWANConnectionDeviceWANIPConnectionStats **/

/**@obj WANDeviceWANConnectionDeviceWANIPConnection **/
CPE_STATUS  addWANDeviceWANConnectionDeviceWANIPConnection(CWMPObject *o, Instance *ip)
{
	/* add instance data */
	WANDeviceWANConnectionDeviceWANIPConnection *p = (WANDeviceWANConnectionDeviceWANIPConnection *)GS_MALLOC( sizeof(struct WANDeviceWANConnectionDeviceWANIPConnection));
	memset(p, 0, sizeof(struct WANDeviceWANConnectionDeviceWANIPConnection));
	ip->cpeData = (void *)p;
	return CPE_OK;
}
CPE_STATUS  delWANDeviceWANConnectionDeviceWANIPConnection(CWMPObject *o, Instance *ip)
{
	/* delete instance data */
	WANDeviceWANConnectionDeviceWANIPConnection *p = (WANDeviceWANConnectionDeviceWANIPConnection *)ip->cpeData;
	if( p ){
		//TODO: free instance data
		GS_FREE(p);
	}
	return CPE_OK;
}
CPE_STATUS  commitWANDeviceWANConnectionDeviceWANIPConnection(CWMPObject *o, Instance *ip, eCommitCBType cmt)
{
	/* commit object instance */
	return CPE_OK;
}

/**@param WANDeviceWANConnectionDeviceWANIPConnection_Enable                     **/
CPE_STATUS setWANDeviceWANConnectionDeviceWANIPConnection_Enable(Instance *ip, char *value)
{
	WANDeviceWANConnectionDeviceWANIPConnection *p = (WANDeviceWANConnectionDeviceWANIPConnection *)ip->cpeData;
	if ( p ){
		p->enable=testBoolean(value);
	}
	return CPE_OK;
}
CPE_STATUS getWANDeviceWANConnectionDeviceWANIPConnection_Enable(Instance *ip, char **value)
{
	WANDeviceWANConnectionDeviceWANIPConnection *p = (WANDeviceWANConnectionDeviceWANIPConnection *)ip->cpeData;
	if ( p ){
		*value = GS_STRDUP(p->enable? "true": "false");
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANIPConnection_ConnectionStatus                     **/
CPE_STATUS getWANDeviceWANConnectionDeviceWANIPConnection_ConnectionStatus(Instance *ip, char **value)
{
	WANDeviceWANConnectionDeviceWANIPConnection *p = (WANDeviceWANConnectionDeviceWANIPConnection *)ip->cpeData;
	if ( p ){
		if ( p->connectionStatus )
			*value = GS_STRDUP(p->connectionStatus);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANIPConnection_PossibleConnectionTypes                     **/
CPE_STATUS getWANDeviceWANConnectionDeviceWANIPConnection_PossibleConnectionTypes(Instance *ip, char **value)
{
	WANDeviceWANConnectionDeviceWANIPConnection *p = (WANDeviceWANConnectionDeviceWANIPConnection *)ip->cpeData;
	if ( p ){
		if ( p->possibleConnectionTypes )
			*value = GS_STRDUP(p->possibleConnectionTypes);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANIPConnection_ConnectionType                     **/
CPE_STATUS setWANDeviceWANConnectionDeviceWANIPConnection_ConnectionType(Instance *ip, char *value)
{
	WANDeviceWANConnectionDeviceWANIPConnection *p = (WANDeviceWANConnectionDeviceWANIPConnection *)ip->cpeData;
	if ( p ){
		COPYSTR(p->connectionType, value);
	}
	return CPE_OK;
}
CPE_STATUS getWANDeviceWANConnectionDeviceWANIPConnection_ConnectionType(Instance *ip, char **value)
{
	WANDeviceWANConnectionDeviceWANIPConnection *p = (WANDeviceWANConnectionDeviceWANIPConnection *)ip->cpeData;
	if ( p ){
		if ( p->connectionType )
			*value = GS_STRDUP(p->connectionType);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANIPConnection_Name                     **/
CPE_STATUS setWANDeviceWANConnectionDeviceWANIPConnection_Name(Instance *ip, char *value)
{
	WANDeviceWANConnectionDeviceWANIPConnection *p = (WANDeviceWANConnectionDeviceWANIPConnection *)ip->cpeData;
	if ( p ){
		COPYSTR(p->name, value);
	}
	return CPE_OK;
}
CPE_STATUS getWANDeviceWANConnectionDeviceWANIPConnection_Name(Instance *ip, char **value)
{
	WANDeviceWANConnectionDeviceWANIPConnection *p = (WANDeviceWANConnectionDeviceWANIPConnection *)ip->cpeData;
	if ( p ){
		if ( p->name )
			*value = GS_STRDUP(p->name);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANIPConnection_Uptime                     **/
CPE_STATUS getWANDeviceWANConnectionDeviceWANIPConnection_Uptime(Instance *ip, char **value)
{
	WANDeviceWANConnectionDeviceWANIPConnection *p = (WANDeviceWANConnectionDeviceWANIPConnection *)ip->cpeData;
	if ( p ){
		char    buf[10];
		snprintf(buf,sizeof(buf),"%u", p->uptime);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANIPConnection_LastConnectionError                     **/
CPE_STATUS getWANDeviceWANConnectionDeviceWANIPConnection_LastConnectionError(Instance *ip, char **value)
{
	WANDeviceWANConnectionDeviceWANIPConnection *p = (WANDeviceWANConnectionDeviceWANIPConnection *)ip->cpeData;
	if ( p ){
		if ( p->lastConnectionError )
			*value = GS_STRDUP(p->lastConnectionError);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANIPConnection_RSIPAvailable                     **/
CPE_STATUS getWANDeviceWANConnectionDeviceWANIPConnection_RSIPAvailable(Instance *ip, char **value)
{
	WANDeviceWANConnectionDeviceWANIPConnection *p = (WANDeviceWANConnectionDeviceWANIPConnection *)ip->cpeData;
	if ( p ){
		*value = GS_STRDUP(p->rSIPAvailable? "true": "false");
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANIPConnection_NATEnabled                     **/
CPE_STATUS setWANDeviceWANConnectionDeviceWANIPConnection_NATEnabled(Instance *ip, char *value)
{
	WANDeviceWANConnectionDeviceWANIPConnection *p = (WANDeviceWANConnectionDeviceWANIPConnection *)ip->cpeData;
	if ( p ){
		p->nATEnabled=testBoolean(value);
	}
	return CPE_OK;
}
CPE_STATUS getWANDeviceWANConnectionDeviceWANIPConnection_NATEnabled(Instance *ip, char **value)
{
	WANDeviceWANConnectionDeviceWANIPConnection *p = (WANDeviceWANConnectionDeviceWANIPConnection *)ip->cpeData;
	if ( p ){
		*value = GS_STRDUP(p->nATEnabled? "true": "false");
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANIPConnection_AddressingType                     **/
CPE_STATUS setWANDeviceWANConnectionDeviceWANIPConnection_AddressingType(Instance *ip, char *value)
{
	WANDeviceWANConnectionDeviceWANIPConnection *p = (WANDeviceWANConnectionDeviceWANIPConnection *)ip->cpeData;
	if ( p ){
		COPYSTR(p->addressingType, value);
	}
	return CPE_OK;
}
CPE_STATUS getWANDeviceWANConnectionDeviceWANIPConnection_AddressingType(Instance *ip, char **value)
{
	WANDeviceWANConnectionDeviceWANIPConnection *p = (WANDeviceWANConnectionDeviceWANIPConnection *)ip->cpeData;
	if ( p ){
		if ( p->addressingType )
			*value = GS_STRDUP(p->addressingType);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANIPConnection_ExternalIPAddress                     **/
CPE_STATUS setWANDeviceWANConnectionDeviceWANIPConnection_ExternalIPAddress(Instance *ip, char *value)
{
	WANDeviceWANConnectionDeviceWANIPConnection *p = (WANDeviceWANConnectionDeviceWANIPConnection *)ip->cpeData;
	if ( p ){
		COPYSTR(p->externalIPAddress, value);
	}
	return CPE_OK;
}
CPE_STATUS getWANDeviceWANConnectionDeviceWANIPConnection_ExternalIPAddress(Instance *ip, char **value)
{
	WANDeviceWANConnectionDeviceWANIPConnection *p = (WANDeviceWANConnectionDeviceWANIPConnection *)ip->cpeData;
	if ( p ){
		if ( p->externalIPAddress )
			*value = GS_STRDUP(p->externalIPAddress);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANIPConnection_SubnetMask                     **/
CPE_STATUS setWANDeviceWANConnectionDeviceWANIPConnection_SubnetMask(Instance *ip, char *value)
{
	WANDeviceWANConnectionDeviceWANIPConnection *p = (WANDeviceWANConnectionDeviceWANIPConnection *)ip->cpeData;
	if ( p ){
		COPYSTR(p->subnetMask, value);
	}
	return CPE_OK;
}
CPE_STATUS getWANDeviceWANConnectionDeviceWANIPConnection_SubnetMask(Instance *ip, char **value)
{
	WANDeviceWANConnectionDeviceWANIPConnection *p = (WANDeviceWANConnectionDeviceWANIPConnection *)ip->cpeData;
	if ( p ){
		if ( p->subnetMask )
			*value = GS_STRDUP(p->subnetMask);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANIPConnection_DefaultGateway                     **/
CPE_STATUS setWANDeviceWANConnectionDeviceWANIPConnection_DefaultGateway(Instance *ip, char *value)
{
	WANDeviceWANConnectionDeviceWANIPConnection *p = (WANDeviceWANConnectionDeviceWANIPConnection *)ip->cpeData;
	if ( p ){
		COPYSTR(p->defaultGateway, value);
	}
	return CPE_OK;
}
CPE_STATUS getWANDeviceWANConnectionDeviceWANIPConnection_DefaultGateway(Instance *ip, char **value)
{
	WANDeviceWANConnectionDeviceWANIPConnection *p = (WANDeviceWANConnectionDeviceWANIPConnection *)ip->cpeData;
	if ( p ){
		if ( p->defaultGateway )
			*value = GS_STRDUP(p->defaultGateway);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANIPConnection_DNSEnabled                     **/
CPE_STATUS setWANDeviceWANConnectionDeviceWANIPConnection_DNSEnabled(Instance *ip, char *value)
{
	WANDeviceWANConnectionDeviceWANIPConnection *p = (WANDeviceWANConnectionDeviceWANIPConnection *)ip->cpeData;
	if ( p ){
		p->dNSEnabled=testBoolean(value);
	}
	return CPE_OK;
}
CPE_STATUS getWANDeviceWANConnectionDeviceWANIPConnection_DNSEnabled(Instance *ip, char **value)
{
	WANDeviceWANConnectionDeviceWANIPConnection *p = (WANDeviceWANConnectionDeviceWANIPConnection *)ip->cpeData;
	if ( p ){
		*value = GS_STRDUP(p->dNSEnabled? "true": "false");
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANIPConnection_DNSOverrideAllowed                     **/
CPE_STATUS setWANDeviceWANConnectionDeviceWANIPConnection_DNSOverrideAllowed(Instance *ip, char *value)
{
	WANDeviceWANConnectionDeviceWANIPConnection *p = (WANDeviceWANConnectionDeviceWANIPConnection *)ip->cpeData;
	if ( p ){
		p->dNSOverrideAllowed=testBoolean(value);
	}
	return CPE_OK;
}
CPE_STATUS getWANDeviceWANConnectionDeviceWANIPConnection_DNSOverrideAllowed(Instance *ip, char **value)
{
	WANDeviceWANConnectionDeviceWANIPConnection *p = (WANDeviceWANConnectionDeviceWANIPConnection *)ip->cpeData;
	if ( p ){
		*value = GS_STRDUP(p->dNSOverrideAllowed? "true": "false");
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANIPConnection_DNSServers                     **/
CPE_STATUS setWANDeviceWANConnectionDeviceWANIPConnection_DNSServers(Instance *ip, char *value)
{
	WANDeviceWANConnectionDeviceWANIPConnection *p = (WANDeviceWANConnectionDeviceWANIPConnection *)ip->cpeData;
	if ( p ){
		COPYSTR(p->dNSServers, value);
	}
	return CPE_OK;
}
CPE_STATUS getWANDeviceWANConnectionDeviceWANIPConnection_DNSServers(Instance *ip, char **value)
{
	WANDeviceWANConnectionDeviceWANIPConnection *p = (WANDeviceWANConnectionDeviceWANIPConnection *)ip->cpeData;
	if ( p ){
		if ( p->dNSServers )
			*value = GS_STRDUP(p->dNSServers);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANIPConnection_MACAddress                     **/
CPE_STATUS setWANDeviceWANConnectionDeviceWANIPConnection_MACAddress(Instance *ip, char *value)
{
	WANDeviceWANConnectionDeviceWANIPConnection *p = (WANDeviceWANConnectionDeviceWANIPConnection *)ip->cpeData;
	if ( p ){
		COPYSTR(p->mACAddress, value);
	}
	return CPE_OK;
}
CPE_STATUS getWANDeviceWANConnectionDeviceWANIPConnection_MACAddress(Instance *ip, char **value)
{
	WANDeviceWANConnectionDeviceWANIPConnection *p = (WANDeviceWANConnectionDeviceWANIPConnection *)ip->cpeData;
	if ( p ){
		if ( p->mACAddress )
			*value = GS_STRDUP(p->mACAddress);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANIPConnection_ConnectionTrigger                     **/
CPE_STATUS setWANDeviceWANConnectionDeviceWANIPConnection_ConnectionTrigger(Instance *ip, char *value)
{
	WANDeviceWANConnectionDeviceWANIPConnection *p = (WANDeviceWANConnectionDeviceWANIPConnection *)ip->cpeData;
	if ( p ){
		COPYSTR(p->connectionTrigger, value);
	}
	return CPE_OK;
}
CPE_STATUS getWANDeviceWANConnectionDeviceWANIPConnection_ConnectionTrigger(Instance *ip, char **value)
{
	WANDeviceWANConnectionDeviceWANIPConnection *p = (WANDeviceWANConnectionDeviceWANIPConnection *)ip->cpeData;
	if ( p ){
		if ( p->connectionTrigger )
			*value = GS_STRDUP(p->connectionTrigger);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANIPConnection_RouteProtocolRx                     **/
CPE_STATUS setWANDeviceWANConnectionDeviceWANIPConnection_RouteProtocolRx(Instance *ip, char *value)
{
	WANDeviceWANConnectionDeviceWANIPConnection *p = (WANDeviceWANConnectionDeviceWANIPConnection *)ip->cpeData;
	if ( p ){
		COPYSTR(p->routeProtocolRx, value);
	}
	return CPE_OK;
}
CPE_STATUS getWANDeviceWANConnectionDeviceWANIPConnection_RouteProtocolRx(Instance *ip, char **value)
{
	WANDeviceWANConnectionDeviceWANIPConnection *p = (WANDeviceWANConnectionDeviceWANIPConnection *)ip->cpeData;
	if ( p ){
		if ( p->routeProtocolRx )
			*value = GS_STRDUP(p->routeProtocolRx);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANIPConnection_PortMappingNumberOfEntries                     **/
CPE_STATUS getWANDeviceWANConnectionDeviceWANIPConnection_PortMappingNumberOfEntries(Instance *ip, char **value)
{
	cwmpGetObjInstanceCntStr("PortMapping", value);
	return CPE_OK;
}
/**@endparam                                               **/
/**@endobj WANDeviceWANConnectionDeviceWANIPConnection **/

/**@obj WANDeviceWANConnectionDeviceWANPPPConnectionPortMapping **/
CPE_STATUS  addWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping(CWMPObject *o, Instance *ip)
{
	/* add instance data */
	WANDeviceWANConnectionDeviceWANPPPConnectionPortMapping *p = (WANDeviceWANConnectionDeviceWANPPPConnectionPortMapping *)GS_MALLOC( sizeof(struct WANDeviceWANConnectionDeviceWANPPPConnectionPortMapping));
	memset(p, 0, sizeof(struct WANDeviceWANConnectionDeviceWANPPPConnectionPortMapping));
	ip->cpeData = (void *)p;
	return CPE_OK;
}
CPE_STATUS  delWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping(CWMPObject *o, Instance *ip)
{
	/* delete instance data */
	WANDeviceWANConnectionDeviceWANPPPConnectionPortMapping *p = (WANDeviceWANConnectionDeviceWANPPPConnectionPortMapping *)ip->cpeData;
	if( p ){
		//TODO: free instance data
		GS_FREE(p);
	}
	return CPE_OK;
}

/**@param WANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_PortMappingEnabled                     **/
CPE_STATUS setWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_PortMappingEnabled(Instance *ip, char *value)
{
	WANDeviceWANConnectionDeviceWANPPPConnectionPortMapping *p = (WANDeviceWANConnectionDeviceWANPPPConnectionPortMapping *)ip->cpeData;
	if ( p ){
		p->portMappingEnabled=testBoolean(value);
	}
	return CPE_OK;
}
CPE_STATUS getWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_PortMappingEnabled(Instance *ip, char **value)
{
	WANDeviceWANConnectionDeviceWANPPPConnectionPortMapping *p = (WANDeviceWANConnectionDeviceWANPPPConnectionPortMapping *)ip->cpeData;
	if ( p ){
		*value = GS_STRDUP(p->portMappingEnabled? "true": "false");
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_PortMappingLeaseDuration                     **/
CPE_STATUS setWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_PortMappingLeaseDuration(Instance *ip, char *value)
{
	WANDeviceWANConnectionDeviceWANPPPConnectionPortMapping *p = (WANDeviceWANConnectionDeviceWANPPPConnectionPortMapping *)ip->cpeData;
	if ( p ){
		p->portMappingLeaseDuration=atoi(value);
	}
	return CPE_OK;
}
CPE_STATUS getWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_PortMappingLeaseDuration(Instance *ip, char **value)
{
	WANDeviceWANConnectionDeviceWANPPPConnectionPortMapping *p = (WANDeviceWANConnectionDeviceWANPPPConnectionPortMapping *)ip->cpeData;
	if ( p ){
		char    buf[10];
		snprintf(buf,sizeof(buf),"%u", p->portMappingLeaseDuration);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_RemoteHost                     **/
CPE_STATUS setWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_RemoteHost(Instance *ip, char *value)
{
	WANDeviceWANConnectionDeviceWANPPPConnectionPortMapping *p = (WANDeviceWANConnectionDeviceWANPPPConnectionPortMapping *)ip->cpeData;
	if ( p ){
		COPYSTR(p->remoteHost, value);
	}
	return CPE_OK;
}
CPE_STATUS getWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_RemoteHost(Instance *ip, char **value)
{
	WANDeviceWANConnectionDeviceWANPPPConnectionPortMapping *p = (WANDeviceWANConnectionDeviceWANPPPConnectionPortMapping *)ip->cpeData;
	if ( p ){
		if ( p->remoteHost )
			*value = GS_STRDUP(p->remoteHost);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_ExternalPort                     **/
CPE_STATUS setWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_ExternalPort(Instance *ip, char *value)
{
	WANDeviceWANConnectionDeviceWANPPPConnectionPortMapping *p = (WANDeviceWANConnectionDeviceWANPPPConnectionPortMapping *)ip->cpeData;
	if ( p ){
		p->externalPort=atoi(value);
	}
	return CPE_OK;
}
CPE_STATUS getWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_ExternalPort(Instance *ip, char **value)
{
	WANDeviceWANConnectionDeviceWANPPPConnectionPortMapping *p = (WANDeviceWANConnectionDeviceWANPPPConnectionPortMapping *)ip->cpeData;
	if ( p ){
		char    buf[10];
		snprintf(buf,sizeof(buf),"%u", p->externalPort);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_InternalPort                     **/
CPE_STATUS setWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_InternalPort(Instance *ip, char *value)
{
	WANDeviceWANConnectionDeviceWANPPPConnectionPortMapping *p = (WANDeviceWANConnectionDeviceWANPPPConnectionPortMapping *)ip->cpeData;
	if ( p ){
		p->internalPort=atoi(value);
	}
	return CPE_OK;
}
CPE_STATUS getWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_InternalPort(Instance *ip, char **value)
{
	WANDeviceWANConnectionDeviceWANPPPConnectionPortMapping *p = (WANDeviceWANConnectionDeviceWANPPPConnectionPortMapping *)ip->cpeData;
	if ( p ){
		char    buf[10];
		snprintf(buf,sizeof(buf),"%u", p->internalPort);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_PortMappingProtocol                     **/
CPE_STATUS setWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_PortMappingProtocol(Instance *ip, char *value)
{
	WANDeviceWANConnectionDeviceWANPPPConnectionPortMapping *p = (WANDeviceWANConnectionDeviceWANPPPConnectionPortMapping *)ip->cpeData;
	if ( p ){
		COPYSTR(p->portMappingProtocol, value);
	}
	return CPE_OK;
}
CPE_STATUS getWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_PortMappingProtocol(Instance *ip, char **value)
{
	WANDeviceWANConnectionDeviceWANPPPConnectionPortMapping *p = (WANDeviceWANConnectionDeviceWANPPPConnectionPortMapping *)ip->cpeData;
	if ( p ){
		if ( p->portMappingProtocol )
			*value = GS_STRDUP(p->portMappingProtocol);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_InternalClient                     **/
CPE_STATUS setWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_InternalClient(Instance *ip, char *value)
{
	WANDeviceWANConnectionDeviceWANPPPConnectionPortMapping *p = (WANDeviceWANConnectionDeviceWANPPPConnectionPortMapping *)ip->cpeData;
	if ( p ){
		COPYSTR(p->internalClient, value);
	}
	return CPE_OK;
}
CPE_STATUS getWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_InternalClient(Instance *ip, char **value)
{
	WANDeviceWANConnectionDeviceWANPPPConnectionPortMapping *p = (WANDeviceWANConnectionDeviceWANPPPConnectionPortMapping *)ip->cpeData;
	if ( p ){
		if ( p->internalClient )
			*value = GS_STRDUP(p->internalClient);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_PortMappingDescription                     **/
CPE_STATUS setWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_PortMappingDescription(Instance *ip, char *value)
{
	WANDeviceWANConnectionDeviceWANPPPConnectionPortMapping *p = (WANDeviceWANConnectionDeviceWANPPPConnectionPortMapping *)ip->cpeData;
	if ( p ){
		COPYSTR(p->portMappingDescription, value);
	}
	return CPE_OK;
}
CPE_STATUS getWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_PortMappingDescription(Instance *ip, char **value)
{
	WANDeviceWANConnectionDeviceWANPPPConnectionPortMapping *p = (WANDeviceWANConnectionDeviceWANPPPConnectionPortMapping *)ip->cpeData;
	if ( p ){
		if ( p->portMappingDescription )
			*value = GS_STRDUP(p->portMappingDescription);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@endobj WANDeviceWANConnectionDeviceWANPPPConnectionPortMapping **/

/**@obj WANDeviceWANConnectionDeviceWANPPPConnectionStats **/

/**@param WANDeviceWANConnectionDeviceWANPPPConnectionStats_EthernetBytesSent                     **/
CPE_STATUS getWANDeviceWANConnectionDeviceWANPPPConnectionStats_EthernetBytesSent(Instance *ip, char **value)
{
	WANDeviceWANConnectionDeviceWANPPPConnectionStats *p = (WANDeviceWANConnectionDeviceWANPPPConnectionStats *)ip->cpeData;
	if ( p ){
		char    buf[10];
		snprintf(buf,sizeof(buf),"%u", p->ethernetBytesSent);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANPPPConnectionStats_EthernetBytesReceived                     **/
CPE_STATUS getWANDeviceWANConnectionDeviceWANPPPConnectionStats_EthernetBytesReceived(Instance *ip, char **value)
{
	WANDeviceWANConnectionDeviceWANPPPConnectionStats *p = (WANDeviceWANConnectionDeviceWANPPPConnectionStats *)ip->cpeData;
	if ( p ){
		char    buf[10];
		snprintf(buf,sizeof(buf),"%u", p->ethernetBytesReceived);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANPPPConnectionStats_EthernetPacketsSent                     **/
CPE_STATUS getWANDeviceWANConnectionDeviceWANPPPConnectionStats_EthernetPacketsSent(Instance *ip, char **value)
{
	WANDeviceWANConnectionDeviceWANPPPConnectionStats *p = (WANDeviceWANConnectionDeviceWANPPPConnectionStats *)ip->cpeData;
	if ( p ){
		char    buf[10];
		snprintf(buf,sizeof(buf),"%u", p->ethernetPacketsSent);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANPPPConnectionStats_EthernetPacketsReceived                     **/
CPE_STATUS getWANDeviceWANConnectionDeviceWANPPPConnectionStats_EthernetPacketsReceived(Instance *ip, char **value)
{
	WANDeviceWANConnectionDeviceWANPPPConnectionStats *p = (WANDeviceWANConnectionDeviceWANPPPConnectionStats *)ip->cpeData;
	if ( p ){
		char    buf[10];
		snprintf(buf,sizeof(buf),"%u", p->ethernetPacketsReceived);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@endobj WANDeviceWANConnectionDeviceWANPPPConnectionStats **/

/**@obj WANDeviceWANConnectionDeviceWANPPPConnection **/
CPE_STATUS  addWANDeviceWANConnectionDeviceWANPPPConnection(CWMPObject *o, Instance *ip)
{
	/* add instance data */
	WANDeviceWANConnectionDeviceWANPPPConnection *p = (WANDeviceWANConnectionDeviceWANPPPConnection *)GS_MALLOC( sizeof(struct WANDeviceWANConnectionDeviceWANPPPConnection));
	memset(p, 0, sizeof(struct WANDeviceWANConnectionDeviceWANPPPConnection));
	ip->cpeData = (void *)p;
	return CPE_OK;
}
CPE_STATUS  delWANDeviceWANConnectionDeviceWANPPPConnection(CWMPObject *o, Instance *ip)
{
	/* delete instance data */
	WANDeviceWANConnectionDeviceWANPPPConnection *p = (WANDeviceWANConnectionDeviceWANPPPConnection *)ip->cpeData;
	if( p ){
		//TODO: free instance data
		GS_FREE(p);
	}
	return CPE_OK;
}

/**@param WANDeviceWANConnectionDeviceWANPPPConnection_Enable                     **/
CPE_STATUS setWANDeviceWANConnectionDeviceWANPPPConnection_Enable(Instance *ip, char *value)
{
	WANDeviceWANConnectionDeviceWANPPPConnection *p = (WANDeviceWANConnectionDeviceWANPPPConnection *)ip->cpeData;
	if ( p ){
		p->enable=testBoolean(value);
	}
	return CPE_OK;
}
CPE_STATUS getWANDeviceWANConnectionDeviceWANPPPConnection_Enable(Instance *ip, char **value)
{
	WANDeviceWANConnectionDeviceWANPPPConnection *p = (WANDeviceWANConnectionDeviceWANPPPConnection *)ip->cpeData;
	if ( p ){
		*value = GS_STRDUP(p->enable? "true": "false");
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANPPPConnection_ConnectionStatus                     **/
CPE_STATUS getWANDeviceWANConnectionDeviceWANPPPConnection_ConnectionStatus(Instance *ip, char **value)
{
	WANDeviceWANConnectionDeviceWANPPPConnection *p = (WANDeviceWANConnectionDeviceWANPPPConnection *)ip->cpeData;
	if ( p ){
		if ( p->connectionStatus )
			*value = GS_STRDUP(p->connectionStatus);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANPPPConnection_PossibleConnectionTypes                     **/
CPE_STATUS getWANDeviceWANConnectionDeviceWANPPPConnection_PossibleConnectionTypes(Instance *ip, char **value)
{
	WANDeviceWANConnectionDeviceWANPPPConnection *p = (WANDeviceWANConnectionDeviceWANPPPConnection *)ip->cpeData;
	if ( p ){
		if ( p->possibleConnectionTypes )
			*value = GS_STRDUP(p->possibleConnectionTypes);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANPPPConnection_ConnectionType                     **/
CPE_STATUS setWANDeviceWANConnectionDeviceWANPPPConnection_ConnectionType(Instance *ip, char *value)
{
	WANDeviceWANConnectionDeviceWANPPPConnection *p = (WANDeviceWANConnectionDeviceWANPPPConnection *)ip->cpeData;
	if ( p ){
		COPYSTR(p->connectionType, value);
	}
	return CPE_OK;
}
CPE_STATUS getWANDeviceWANConnectionDeviceWANPPPConnection_ConnectionType(Instance *ip, char **value)
{
	WANDeviceWANConnectionDeviceWANPPPConnection *p = (WANDeviceWANConnectionDeviceWANPPPConnection *)ip->cpeData;
	if ( p ){
		if ( p->connectionType )
			*value = GS_STRDUP(p->connectionType);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANPPPConnection_Name                     **/
CPE_STATUS setWANDeviceWANConnectionDeviceWANPPPConnection_Name(Instance *ip, char *value)
{
	WANDeviceWANConnectionDeviceWANPPPConnection *p = (WANDeviceWANConnectionDeviceWANPPPConnection *)ip->cpeData;
	if ( p ){
		COPYSTR(p->name, value);
	}
	return CPE_OK;
}
CPE_STATUS getWANDeviceWANConnectionDeviceWANPPPConnection_Name(Instance *ip, char **value)
{
	WANDeviceWANConnectionDeviceWANPPPConnection *p = (WANDeviceWANConnectionDeviceWANPPPConnection *)ip->cpeData;
	if ( p ){
		if ( p->name )
			*value = GS_STRDUP(p->name);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANPPPConnection_Uptime                     **/
CPE_STATUS getWANDeviceWANConnectionDeviceWANPPPConnection_Uptime(Instance *ip, char **value)
{
	WANDeviceWANConnectionDeviceWANPPPConnection *p = (WANDeviceWANConnectionDeviceWANPPPConnection *)ip->cpeData;
	if ( p ){
		char    buf[10];
		snprintf(buf,sizeof(buf),"%u", p->uptime);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANPPPConnection_LastConnectionError                     **/
CPE_STATUS getWANDeviceWANConnectionDeviceWANPPPConnection_LastConnectionError(Instance *ip, char **value)
{
	WANDeviceWANConnectionDeviceWANPPPConnection *p = (WANDeviceWANConnectionDeviceWANPPPConnection *)ip->cpeData;
	if ( p ){
		if ( p->lastConnectionError )
			*value = GS_STRDUP(p->lastConnectionError);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANPPPConnection_RSIPAvailable                     **/
CPE_STATUS getWANDeviceWANConnectionDeviceWANPPPConnection_RSIPAvailable(Instance *ip, char **value)
{
	WANDeviceWANConnectionDeviceWANPPPConnection *p = (WANDeviceWANConnectionDeviceWANPPPConnection *)ip->cpeData;
	if ( p ){
		*value = GS_STRDUP(p->rSIPAvailable? "true": "false");
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANPPPConnection_NATEnabled                     **/
CPE_STATUS setWANDeviceWANConnectionDeviceWANPPPConnection_NATEnabled(Instance *ip, char *value)
{
	WANDeviceWANConnectionDeviceWANPPPConnection *p = (WANDeviceWANConnectionDeviceWANPPPConnection *)ip->cpeData;
	if ( p ){
		p->nATEnabled=testBoolean(value);
	}
	return CPE_OK;
}
CPE_STATUS getWANDeviceWANConnectionDeviceWANPPPConnection_NATEnabled(Instance *ip, char **value)
{
	WANDeviceWANConnectionDeviceWANPPPConnection *p = (WANDeviceWANConnectionDeviceWANPPPConnection *)ip->cpeData;
	if ( p ){
		*value = GS_STRDUP(p->nATEnabled? "true": "false");
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANPPPConnection_Username                     **/
CPE_STATUS setWANDeviceWANConnectionDeviceWANPPPConnection_Username(Instance *ip, char *value)
{
	WANDeviceWANConnectionDeviceWANPPPConnection *p = (WANDeviceWANConnectionDeviceWANPPPConnection *)ip->cpeData;
	if ( p ){
		COPYSTR(p->username, value);
	}
	return CPE_OK;
}
CPE_STATUS getWANDeviceWANConnectionDeviceWANPPPConnection_Username(Instance *ip, char **value)
{
	WANDeviceWANConnectionDeviceWANPPPConnection *p = (WANDeviceWANConnectionDeviceWANPPPConnection *)ip->cpeData;
	if ( p ){
		if ( p->username )
			*value = GS_STRDUP(p->username);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANPPPConnection_Password                     **/
CPE_STATUS setWANDeviceWANConnectionDeviceWANPPPConnection_Password(Instance *ip, char *value)
{
	WANDeviceWANConnectionDeviceWANPPPConnection *p = (WANDeviceWANConnectionDeviceWANPPPConnection *)ip->cpeData;
	if ( p ){
		COPYSTR(p->password, value);
	}
	return CPE_OK;
}
CPE_STATUS getWANDeviceWANConnectionDeviceWANPPPConnection_Password(Instance *ip, char **value)
{
	WANDeviceWANConnectionDeviceWANPPPConnection *p = (WANDeviceWANConnectionDeviceWANPPPConnection *)ip->cpeData;
	if ( p ){
		if ( p->password )
			*value = GS_STRDUP(p->password);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANPPPConnection_ExternalIPAddress                     **/
CPE_STATUS setWANDeviceWANConnectionDeviceWANPPPConnection_ExternalIPAddress(Instance *ip, char *value)
{
	WANDeviceWANConnectionDeviceWANPPPConnection *p = (WANDeviceWANConnectionDeviceWANPPPConnection *)ip->cpeData;
	if ( p ){
		COPYSTR(p->externalIPAddress, value);
	}
	return CPE_OK;
}
CPE_STATUS getWANDeviceWANConnectionDeviceWANPPPConnection_ExternalIPAddress(Instance *ip, char **value)
{
	WANDeviceWANConnectionDeviceWANPPPConnection *p = (WANDeviceWANConnectionDeviceWANPPPConnection *)ip->cpeData;
	if ( p ){
		if ( p->externalIPAddress )
			*value = GS_STRDUP(p->externalIPAddress);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANPPPConnection_DNSEnabled                     **/
CPE_STATUS setWANDeviceWANConnectionDeviceWANPPPConnection_DNSEnabled(Instance *ip, char *value)
{
	WANDeviceWANConnectionDeviceWANPPPConnection *p = (WANDeviceWANConnectionDeviceWANPPPConnection *)ip->cpeData;
	if ( p ){
		p->dNSEnabled=testBoolean(value);
	}
	return CPE_OK;
}
CPE_STATUS getWANDeviceWANConnectionDeviceWANPPPConnection_DNSEnabled(Instance *ip, char **value)
{
	WANDeviceWANConnectionDeviceWANPPPConnection *p = (WANDeviceWANConnectionDeviceWANPPPConnection *)ip->cpeData;
	if ( p ){
		*value = GS_STRDUP(p->dNSEnabled? "true": "false");
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANPPPConnection_DNSOverrideAllowed                     **/
CPE_STATUS setWANDeviceWANConnectionDeviceWANPPPConnection_DNSOverrideAllowed(Instance *ip, char *value)
{
	WANDeviceWANConnectionDeviceWANPPPConnection *p = (WANDeviceWANConnectionDeviceWANPPPConnection *)ip->cpeData;
	if ( p ){
		p->dNSOverrideAllowed=testBoolean(value);
	}
	return CPE_OK;
}
CPE_STATUS getWANDeviceWANConnectionDeviceWANPPPConnection_DNSOverrideAllowed(Instance *ip, char **value)
{
	WANDeviceWANConnectionDeviceWANPPPConnection *p = (WANDeviceWANConnectionDeviceWANPPPConnection *)ip->cpeData;
	if ( p ){
		*value = GS_STRDUP(p->dNSOverrideAllowed? "true": "false");
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANPPPConnection_DNSServers                     **/
CPE_STATUS setWANDeviceWANConnectionDeviceWANPPPConnection_DNSServers(Instance *ip, char *value)
{
	WANDeviceWANConnectionDeviceWANPPPConnection *p = (WANDeviceWANConnectionDeviceWANPPPConnection *)ip->cpeData;
	if ( p ){
		COPYSTR(p->dNSServers, value);
	}
	return CPE_OK;
}
CPE_STATUS getWANDeviceWANConnectionDeviceWANPPPConnection_DNSServers(Instance *ip, char **value)
{
	WANDeviceWANConnectionDeviceWANPPPConnection *p = (WANDeviceWANConnectionDeviceWANPPPConnection *)ip->cpeData;
	if ( p ){
		if ( p->dNSServers )
			*value = GS_STRDUP(p->dNSServers);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANPPPConnection_MACAddress                     **/
CPE_STATUS setWANDeviceWANConnectionDeviceWANPPPConnection_MACAddress(Instance *ip, char *value)
{
	WANDeviceWANConnectionDeviceWANPPPConnection *p = (WANDeviceWANConnectionDeviceWANPPPConnection *)ip->cpeData;
	if ( p ){
		COPYSTR(p->mACAddress, value);
	}
	return CPE_OK;
}
CPE_STATUS getWANDeviceWANConnectionDeviceWANPPPConnection_MACAddress(Instance *ip, char **value)
{
	WANDeviceWANConnectionDeviceWANPPPConnection *p = (WANDeviceWANConnectionDeviceWANPPPConnection *)ip->cpeData;
	if ( p ){
		if ( p->mACAddress )
			*value = GS_STRDUP(p->mACAddress);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANPPPConnection_TransportType                     **/
CPE_STATUS getWANDeviceWANConnectionDeviceWANPPPConnection_TransportType(Instance *ip, char **value)
{
	WANDeviceWANConnectionDeviceWANPPPConnection *p = (WANDeviceWANConnectionDeviceWANPPPConnection *)ip->cpeData;
	if ( p ){
		if ( p->transportType )
			*value = GS_STRDUP(p->transportType);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANPPPConnection_PPPoEACName                     **/
CPE_STATUS setWANDeviceWANConnectionDeviceWANPPPConnection_PPPoEACName(Instance *ip, char *value)
{
	WANDeviceWANConnectionDeviceWANPPPConnection *p = (WANDeviceWANConnectionDeviceWANPPPConnection *)ip->cpeData;
	if ( p ){
		COPYSTR(p->pPPoEACName, value);
	}
	return CPE_OK;
}
CPE_STATUS getWANDeviceWANConnectionDeviceWANPPPConnection_PPPoEACName(Instance *ip, char **value)
{
	WANDeviceWANConnectionDeviceWANPPPConnection *p = (WANDeviceWANConnectionDeviceWANPPPConnection *)ip->cpeData;
	if ( p ){
		if ( p->pPPoEACName )
			*value = GS_STRDUP(p->pPPoEACName);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANPPPConnection_PPPoEServiceName                     **/
CPE_STATUS setWANDeviceWANConnectionDeviceWANPPPConnection_PPPoEServiceName(Instance *ip, char *value)
{
	WANDeviceWANConnectionDeviceWANPPPConnection *p = (WANDeviceWANConnectionDeviceWANPPPConnection *)ip->cpeData;
	if ( p ){
		COPYSTR(p->pPPoEServiceName, value);
	}
	return CPE_OK;
}
CPE_STATUS getWANDeviceWANConnectionDeviceWANPPPConnection_PPPoEServiceName(Instance *ip, char **value)
{
	WANDeviceWANConnectionDeviceWANPPPConnection *p = (WANDeviceWANConnectionDeviceWANPPPConnection *)ip->cpeData;
	if ( p ){
		if ( p->pPPoEServiceName )
			*value = GS_STRDUP(p->pPPoEServiceName);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANPPPConnection_ConnectionTrigger                     **/
CPE_STATUS setWANDeviceWANConnectionDeviceWANPPPConnection_ConnectionTrigger(Instance *ip, char *value)
{
	WANDeviceWANConnectionDeviceWANPPPConnection *p = (WANDeviceWANConnectionDeviceWANPPPConnection *)ip->cpeData;
	if ( p ){
		COPYSTR(p->connectionTrigger, value);
	}
	return CPE_OK;
}
CPE_STATUS getWANDeviceWANConnectionDeviceWANPPPConnection_ConnectionTrigger(Instance *ip, char **value)
{
	WANDeviceWANConnectionDeviceWANPPPConnection *p = (WANDeviceWANConnectionDeviceWANPPPConnection *)ip->cpeData;
	if ( p ){
		if ( p->connectionTrigger )
			*value = GS_STRDUP(p->connectionTrigger);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANPPPConnection_RouteProtocolRx                     **/
CPE_STATUS setWANDeviceWANConnectionDeviceWANPPPConnection_RouteProtocolRx(Instance *ip, char *value)
{
	WANDeviceWANConnectionDeviceWANPPPConnection *p = (WANDeviceWANConnectionDeviceWANPPPConnection *)ip->cpeData;
	if ( p ){
		COPYSTR(p->routeProtocolRx, value);
	}
	return CPE_OK;
}
CPE_STATUS getWANDeviceWANConnectionDeviceWANPPPConnection_RouteProtocolRx(Instance *ip, char **value)
{
	WANDeviceWANConnectionDeviceWANPPPConnection *p = (WANDeviceWANConnectionDeviceWANPPPConnection *)ip->cpeData;
	if ( p ){
		if ( p->routeProtocolRx )
			*value = GS_STRDUP(p->routeProtocolRx);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANPPPConnection_PortMappingNumberOfEntries                     **/
CPE_STATUS getWANDeviceWANConnectionDeviceWANPPPConnection_PortMappingNumberOfEntries(Instance *ip, char **value)
{
	cwmpGetObjInstanceCntStr("PortMapping", value);
	return CPE_OK;
}
/**@endparam                                               **/
/**@endobj WANDeviceWANConnectionDeviceWANPPPConnection **/

/**@obj WANDeviceWANConnectionDevice **/
CPE_STATUS  addWANDeviceWANConnectionDevice(CWMPObject *o, Instance *ip)
{
	/* add instance data */
	return CPE_OK;
}
CPE_STATUS  delWANDeviceWANConnectionDevice(CWMPObject *o, Instance *ip)
{
	/* delete instance data */
	return CPE_OK;
}

/**@param WANDeviceWANConnectionDevice_WANIPConnectionNumberOfEntries                     **/
CPE_STATUS getWANDeviceWANConnectionDevice_WANIPConnectionNumberOfEntries(Instance *ip, char **value)
{
	cwmpGetObjInstanceCntStr("WANIPConnection", value);
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDevice_WANPPPConnectionNumberOfEntries                     **/
CPE_STATUS getWANDeviceWANConnectionDevice_WANPPPConnectionNumberOfEntries(Instance *ip, char **value)
{
	cwmpGetObjInstanceCntStr("WANPPPConnection", value);
	return CPE_OK;
}
/**@endparam                                               **/
/**@endobj WANDeviceWANConnectionDevice **/

/**@obj WANDevice **/
CPE_STATUS  addWANDevice(CWMPObject *o, Instance *ip)
{
	/* add instance data */
	return CPE_OK;
}
CPE_STATUS  delWANDevice(CWMPObject *o, Instance *ip)
{
	/* delete instance data */
	return CPE_OK;
}

/**@param WANDevice_WANConnectionNumberOfEntries                     **/
CPE_STATUS getWANDevice_WANConnectionNumberOfEntries(Instance *ip, char **value)
{
	cwmpGetObjInstanceCntStr("WANConnectionDevice", value);
	return CPE_OK;
}
/**@endparam                                               **/
/**@endobj WANDevice **/





