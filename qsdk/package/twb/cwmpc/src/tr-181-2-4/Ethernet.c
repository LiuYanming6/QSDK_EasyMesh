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
 * $Id: Ethernet.c,v 1.2 2012/06/13 16:07:50 dmounday Exp $
 *----------------------------------------------------------------------*/

#include "paramTree.h"
#include "rpc.h"
#include "gslib/src/utils.h"
#include "soapRpc/rpcUtils.h"
#include "soapRpc/rpcMethods.h"
#include "soapRpc/cwmpSession.h"

#include "Ethernet.h"

/**@obj EthernetInterfaceStats **/
CPE_STATUS  initEthernetInterfaceStats(CWMPObject *o, Instance *ip)
{
	/* initialize object */
	EthernetInterfaceStats *p = (EthernetInterfaceStats *)GS_MALLOC( sizeof(struct EthernetInterfaceStats));
	memset(p, 0, sizeof(struct EthernetInterfaceStats));
	ip->cpeData = (void *)p;
	return CPE_OK;
}

/**@param EthernetInterfaceStats_BytesSent                     **/
CPE_STATUS getEthernetInterfaceStats_BytesSent(Instance *ip, char **value)
{
	EthernetInterfaceStats *p = (EthernetInterfaceStats *)ip->cpeData;
	if ( p ){
		char    buf[30];
		snprintf(buf,sizeof(buf),"%lld", p->bytesSent);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param EthernetInterfaceStats_BytesReceived                     **/
CPE_STATUS getEthernetInterfaceStats_BytesReceived(Instance *ip, char **value)
{
	EthernetInterfaceStats *p = (EthernetInterfaceStats *)ip->cpeData;
	if ( p ){
		char    buf[30];
		snprintf(buf,sizeof(buf),"%lld", p->bytesReceived);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param EthernetInterfaceStats_PacketsSent                     **/
CPE_STATUS getEthernetInterfaceStats_PacketsSent(Instance *ip, char **value)
{
	EthernetInterfaceStats *p = (EthernetInterfaceStats *)ip->cpeData;
	if ( p ){
		char    buf[30];
		snprintf(buf,sizeof(buf),"%lld", p->packetsSent);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param EthernetInterfaceStats_PacketsReceived                     **/
CPE_STATUS getEthernetInterfaceStats_PacketsReceived(Instance *ip, char **value)
{
	EthernetInterfaceStats *p = (EthernetInterfaceStats *)ip->cpeData;
	if ( p ){
		char    buf[30];
		snprintf(buf,sizeof(buf),"%lld", p->packetsReceived);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param EthernetInterfaceStats_ErrorsSent                     **/
CPE_STATUS getEthernetInterfaceStats_ErrorsSent(Instance *ip, char **value)
{
	EthernetInterfaceStats *p = (EthernetInterfaceStats *)ip->cpeData;
	if ( p ){
		char    buf[10];
		snprintf(buf,sizeof(buf),"%u", p->errorsSent);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param EthernetInterfaceStats_ErrorsReceived                     **/
CPE_STATUS getEthernetInterfaceStats_ErrorsReceived(Instance *ip, char **value)
{
	EthernetInterfaceStats *p = (EthernetInterfaceStats *)ip->cpeData;
	if ( p ){
		char    buf[10];
		snprintf(buf,sizeof(buf),"%u", p->errorsReceived);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param EthernetInterfaceStats_UnicastPacketsSent                     **/
CPE_STATUS getEthernetInterfaceStats_UnicastPacketsSent(Instance *ip, char **value)
{
	EthernetInterfaceStats *p = (EthernetInterfaceStats *)ip->cpeData;
	if ( p ){
		char    buf[30];
		snprintf(buf,sizeof(buf),"%lld", p->unicastPacketsSent);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param EthernetInterfaceStats_UnicastPacketsReceived                     **/
CPE_STATUS getEthernetInterfaceStats_UnicastPacketsReceived(Instance *ip, char **value)
{
	EthernetInterfaceStats *p = (EthernetInterfaceStats *)ip->cpeData;
	if ( p ){
		char    buf[30];
		snprintf(buf,sizeof(buf),"%lld", p->unicastPacketsReceived);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param EthernetInterfaceStats_DiscardPacketsSent                     **/
CPE_STATUS getEthernetInterfaceStats_DiscardPacketsSent(Instance *ip, char **value)
{
	EthernetInterfaceStats *p = (EthernetInterfaceStats *)ip->cpeData;
	if ( p ){
		char    buf[10];
		snprintf(buf,sizeof(buf),"%u", p->discardPacketsSent);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param EthernetInterfaceStats_DiscardPacketsReceived                     **/
CPE_STATUS getEthernetInterfaceStats_DiscardPacketsReceived(Instance *ip, char **value)
{
	EthernetInterfaceStats *p = (EthernetInterfaceStats *)ip->cpeData;
	if ( p ){
		char    buf[10];
		snprintf(buf,sizeof(buf),"%u", p->discardPacketsReceived);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param EthernetInterfaceStats_MulticastPacketsSent                     **/
CPE_STATUS getEthernetInterfaceStats_MulticastPacketsSent(Instance *ip, char **value)
{
	EthernetInterfaceStats *p = (EthernetInterfaceStats *)ip->cpeData;
	if ( p ){
		char    buf[30];
		snprintf(buf,sizeof(buf),"%lld", p->multicastPacketsSent);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param EthernetInterfaceStats_MulticastPacketsReceived                     **/
CPE_STATUS getEthernetInterfaceStats_MulticastPacketsReceived(Instance *ip, char **value)
{
	EthernetInterfaceStats *p = (EthernetInterfaceStats *)ip->cpeData;
	if ( p ){
		char    buf[30];
		snprintf(buf,sizeof(buf),"%lld", p->multicastPacketsReceived);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param EthernetInterfaceStats_BroadcastPacketsSent                     **/
CPE_STATUS getEthernetInterfaceStats_BroadcastPacketsSent(Instance *ip, char **value)
{
	EthernetInterfaceStats *p = (EthernetInterfaceStats *)ip->cpeData;
	if ( p ){
		char    buf[30];
		snprintf(buf,sizeof(buf),"%lld", p->broadcastPacketsSent);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param EthernetInterfaceStats_BroadcastPacketsReceived                     **/
CPE_STATUS getEthernetInterfaceStats_BroadcastPacketsReceived(Instance *ip, char **value)
{
	EthernetInterfaceStats *p = (EthernetInterfaceStats *)ip->cpeData;
	if ( p ){
		char    buf[30];
		snprintf(buf,sizeof(buf),"%lld", p->broadcastPacketsReceived);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param EthernetInterfaceStats_UnknownProtoPacketsReceived                     **/
CPE_STATUS getEthernetInterfaceStats_UnknownProtoPacketsReceived(Instance *ip, char **value)
{
	EthernetInterfaceStats *p = (EthernetInterfaceStats *)ip->cpeData;
	if ( p ){
		char    buf[10];
		snprintf(buf,sizeof(buf),"%u", p->unknownProtoPacketsReceived);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@endobj EthernetInterfaceStats **/

/**@obj EthernetInterface **/
CPE_STATUS  addEthernetInterface(CWMPObject *o, Instance *ip)
{
	/* add instance data */
	EthernetInterface *p = (EthernetInterface *)GS_MALLOC( sizeof(struct EthernetInterface));
	memset(p, 0, sizeof(struct EthernetInterface));
	ip->cpeData = (void *)p;
	return CPE_OK;
}

/**@param EthernetInterface_Enable                     **/
CPE_STATUS setEthernetInterface_Enable(Instance *ip, char *value)
{
	EthernetInterface *p = (EthernetInterface *)ip->cpeData;
	if ( p ){
		p->enable=testBoolean(value);
	}
	return CPE_OK;
}
CPE_STATUS getEthernetInterface_Enable(Instance *ip, char **value)
{
	EthernetInterface *p = (EthernetInterface *)ip->cpeData;
	if ( p ){
		*value = GS_STRDUP(p->enable? "true": "false");
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param EthernetInterface_Status                     **/
CPE_STATUS getEthernetInterface_Status(Instance *ip, char **value)
{
	EthernetInterface *p = (EthernetInterface *)ip->cpeData;
	if ( p ){
		if ( p->status )
			*value = GS_STRDUP(p->status);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param EthernetInterface_Name                     **/
CPE_STATUS setEthernetInterface_Name(Instance *ip, char *value)
{
	EthernetInterface *p = (EthernetInterface *)ip->cpeData;
	if ( p ){
		COPYSTR(p->name, value);
	}
	return CPE_OK;
}
CPE_STATUS getEthernetInterface_Name(Instance *ip, char **value)
{
	EthernetInterface *p = (EthernetInterface *)ip->cpeData;
	if ( p ){
		if ( p->name )
			*value = GS_STRDUP(p->name);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param EthernetInterface_LastChange                     **/
CPE_STATUS getEthernetInterface_LastChange(Instance *ip, char **value)
{
	EthernetInterface *p = (EthernetInterface *)ip->cpeData;
	if ( p ){
		char    buf[10];
		snprintf(buf,sizeof(buf),"%u", p->lastChange);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param EthernetInterface_Upstream                     **/
CPE_STATUS getEthernetInterface_Upstream(Instance *ip, char **value)
{
	EthernetInterface *p = (EthernetInterface *)ip->cpeData;
	if ( p ){
		*value = GS_STRDUP(p->upstream? "true": "false");
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param EthernetInterface_MACAddress                     **/
CPE_STATUS getEthernetInterface_MACAddress(Instance *ip, char **value)
{
	EthernetInterface *p = (EthernetInterface *)ip->cpeData;
	if ( p ){
		if ( p->mACAddress )
			*value = GS_STRDUP(p->mACAddress);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param EthernetInterface_MaxBitRate                     **/
CPE_STATUS setEthernetInterface_MaxBitRate(Instance *ip, char *value)
{
	EthernetInterface *p = (EthernetInterface *)ip->cpeData;
	if ( p ){
		p->maxBitRate=atoi(value);
	}
	return CPE_OK;
}
CPE_STATUS getEthernetInterface_MaxBitRate(Instance *ip, char **value)
{
	EthernetInterface *p = (EthernetInterface *)ip->cpeData;
	if ( p ){
		char    buf[10];
		snprintf(buf,sizeof(buf),"%d", p->maxBitRate);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param EthernetInterface_DuplexMode                     **/
CPE_STATUS setEthernetInterface_DuplexMode(Instance *ip, char *value)
{
	EthernetInterface *p = (EthernetInterface *)ip->cpeData;
	if ( p ){
		COPYSTR(p->duplexMode, value);
	}
	return CPE_OK;
}
CPE_STATUS getEthernetInterface_DuplexMode(Instance *ip, char **value)
{
	EthernetInterface *p = (EthernetInterface *)ip->cpeData;
	if ( p ){
		if ( p->duplexMode )
			*value = GS_STRDUP(p->duplexMode);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@endobj EthernetInterface **/

/**@obj EthernetLinkStats **/
CPE_STATUS  initEthernetLinkStats(CWMPObject *o, Instance *ip)
{
	/* initialize object */
	EthernetLinkStats *p = (EthernetLinkStats *)GS_MALLOC( sizeof(struct EthernetLinkStats));
	memset(p, 0, sizeof(struct EthernetLinkStats));
	ip->cpeData = (void *)p;
	return CPE_OK;
}

/**@param EthernetLinkStats_BytesSent                     **/
CPE_STATUS getEthernetLinkStats_BytesSent(Instance *ip, char **value)
{
	EthernetLinkStats *p = (EthernetLinkStats *)ip->cpeData;
	if ( p ){
		char    buf[30];
		snprintf(buf,sizeof(buf),"%lld", p->bytesSent);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param EthernetLinkStats_BytesReceived                     **/
CPE_STATUS getEthernetLinkStats_BytesReceived(Instance *ip, char **value)
{
	EthernetLinkStats *p = (EthernetLinkStats *)ip->cpeData;
	if ( p ){
		char    buf[30];
		snprintf(buf,sizeof(buf),"%lld", p->bytesReceived);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param EthernetLinkStats_PacketsSent                     **/
CPE_STATUS getEthernetLinkStats_PacketsSent(Instance *ip, char **value)
{
	EthernetLinkStats *p = (EthernetLinkStats *)ip->cpeData;
	if ( p ){
		char    buf[30];
		snprintf(buf,sizeof(buf),"%lld", p->packetsSent);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param EthernetLinkStats_PacketsReceived                     **/
CPE_STATUS getEthernetLinkStats_PacketsReceived(Instance *ip, char **value)
{
	EthernetLinkStats *p = (EthernetLinkStats *)ip->cpeData;
	if ( p ){
		char    buf[30];
		snprintf(buf,sizeof(buf),"%lld", p->packetsReceived);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param EthernetLinkStats_ErrorsSent                     **/
CPE_STATUS getEthernetLinkStats_ErrorsSent(Instance *ip, char **value)
{
	EthernetLinkStats *p = (EthernetLinkStats *)ip->cpeData;
	if ( p ){
		char    buf[10];
		snprintf(buf,sizeof(buf),"%u", p->errorsSent);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param EthernetLinkStats_ErrorsReceived                     **/
CPE_STATUS getEthernetLinkStats_ErrorsReceived(Instance *ip, char **value)
{
	EthernetLinkStats *p = (EthernetLinkStats *)ip->cpeData;
	if ( p ){
		char    buf[10];
		snprintf(buf,sizeof(buf),"%u", p->errorsReceived);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param EthernetLinkStats_UnicastPacketsSent                     **/
CPE_STATUS getEthernetLinkStats_UnicastPacketsSent(Instance *ip, char **value)
{
	EthernetLinkStats *p = (EthernetLinkStats *)ip->cpeData;
	if ( p ){
		char    buf[30];
		snprintf(buf,sizeof(buf),"%lld", p->unicastPacketsSent);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param EthernetLinkStats_UnicastPacketsReceived                     **/
CPE_STATUS getEthernetLinkStats_UnicastPacketsReceived(Instance *ip, char **value)
{
	EthernetLinkStats *p = (EthernetLinkStats *)ip->cpeData;
	if ( p ){
		char    buf[30];
		snprintf(buf,sizeof(buf),"%lld", p->unicastPacketsReceived);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param EthernetLinkStats_DiscardPacketsSent                     **/
CPE_STATUS getEthernetLinkStats_DiscardPacketsSent(Instance *ip, char **value)
{
	EthernetLinkStats *p = (EthernetLinkStats *)ip->cpeData;
	if ( p ){
		char    buf[10];
		snprintf(buf,sizeof(buf),"%u", p->discardPacketsSent);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param EthernetLinkStats_DiscardPacketsReceived                     **/
CPE_STATUS getEthernetLinkStats_DiscardPacketsReceived(Instance *ip, char **value)
{
	EthernetLinkStats *p = (EthernetLinkStats *)ip->cpeData;
	if ( p ){
		char    buf[10];
		snprintf(buf,sizeof(buf),"%u", p->discardPacketsReceived);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param EthernetLinkStats_MulticastPacketsSent                     **/
CPE_STATUS getEthernetLinkStats_MulticastPacketsSent(Instance *ip, char **value)
{
	EthernetLinkStats *p = (EthernetLinkStats *)ip->cpeData;
	if ( p ){
		char    buf[30];
		snprintf(buf,sizeof(buf),"%lld", p->multicastPacketsSent);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param EthernetLinkStats_MulticastPacketsReceived                     **/
CPE_STATUS getEthernetLinkStats_MulticastPacketsReceived(Instance *ip, char **value)
{
	EthernetLinkStats *p = (EthernetLinkStats *)ip->cpeData;
	if ( p ){
		char    buf[30];
		snprintf(buf,sizeof(buf),"%lld", p->multicastPacketsReceived);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param EthernetLinkStats_BroadcastPacketsSent                     **/
CPE_STATUS getEthernetLinkStats_BroadcastPacketsSent(Instance *ip, char **value)
{
	EthernetLinkStats *p = (EthernetLinkStats *)ip->cpeData;
	if ( p ){
		char    buf[30];
		snprintf(buf,sizeof(buf),"%lld", p->broadcastPacketsSent);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param EthernetLinkStats_BroadcastPacketsReceived                     **/
CPE_STATUS getEthernetLinkStats_BroadcastPacketsReceived(Instance *ip, char **value)
{
	EthernetLinkStats *p = (EthernetLinkStats *)ip->cpeData;
	if ( p ){
		char    buf[30];
		snprintf(buf,sizeof(buf),"%lld", p->broadcastPacketsReceived);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param EthernetLinkStats_UnknownProtoPacketsReceived                     **/
CPE_STATUS getEthernetLinkStats_UnknownProtoPacketsReceived(Instance *ip, char **value)
{
	EthernetLinkStats *p = (EthernetLinkStats *)ip->cpeData;
	if ( p ){
		char    buf[10];
		snprintf(buf,sizeof(buf),"%u", p->unknownProtoPacketsReceived);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@endobj EthernetLinkStats **/

/**@obj EthernetLink **/
CPE_STATUS  addEthernetLink(CWMPObject *o, Instance *ip)
{
	/* add instance data */
	EthernetLink *p = (EthernetLink *)GS_MALLOC( sizeof(struct EthernetLink));
	memset(p, 0, sizeof(struct EthernetLink));
	ip->cpeData = (void *)p;
	return CPE_OK;
}
CPE_STATUS  delEthernetLink(CWMPObject *o, Instance *ip)
{
	/* delete instance data */
	EthernetLink *p = (EthernetLink *)ip->cpeData;
	if( p ){
		//TODO: free instance data
		GS_FREE(p);
	}
	return CPE_OK;
}

/**@param EthernetLink_Enable                     **/
CPE_STATUS setEthernetLink_Enable(Instance *ip, char *value)
{
	EthernetLink *p = (EthernetLink *)ip->cpeData;
	if ( p ){
		p->enable=testBoolean(value);
	}
	return CPE_OK;
}
CPE_STATUS getEthernetLink_Enable(Instance *ip, char **value)
{
	EthernetLink *p = (EthernetLink *)ip->cpeData;
	if ( p ){
		*value = GS_STRDUP(p->enable? "true": "false");
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param EthernetLink_Status                     **/
CPE_STATUS getEthernetLink_Status(Instance *ip, char **value)
{
	EthernetLink *p = (EthernetLink *)ip->cpeData;
	if ( p ){
		if ( p->status )
			*value = GS_STRDUP(p->status);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param EthernetLink_Name                     **/
CPE_STATUS setEthernetLink_Name(Instance *ip, char *value)
{
	EthernetLink *p = (EthernetLink *)ip->cpeData;
	if ( p ){
		COPYSTR(p->name, value);
	}
	return CPE_OK;
}
CPE_STATUS getEthernetLink_Name(Instance *ip, char **value)
{
	EthernetLink *p = (EthernetLink *)ip->cpeData;
	if ( p ){
		if ( p->name )
			*value = GS_STRDUP(p->name);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param EthernetLink_LastChange                     **/
CPE_STATUS getEthernetLink_LastChange(Instance *ip, char **value)
{
	EthernetLink *p = (EthernetLink *)ip->cpeData;
	if ( p ){
		char    buf[10];
		snprintf(buf,sizeof(buf),"%u", p->lastChange);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param EthernetLink_LowerLayers                     **/
CPE_STATUS setEthernetLink_LowerLayers(Instance *ip, char *value)
{
	EthernetLink *p = (EthernetLink *)ip->cpeData;
	if ( p ){
		p->lowerLayers[0] = cwmpGetInstancePtr(value);
		p->lowerLayers[1] = NULL;
	}
	return CPE_OK;
}
CPE_STATUS getEthernetLink_LowerLayers(Instance *ip, char **value)
{
	EthernetLink *p = (EthernetLink *)ip->cpeData;
	if ( p ){
		*value = cwmpGetPathRefRowsStr(p->lowerLayers);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param EthernetLink_MACAddress                     **/
CPE_STATUS getEthernetLink_MACAddress(Instance *ip, char **value)
{
	EthernetLink *p = (EthernetLink *)ip->cpeData;
	if ( p ){
		if ( p->mACAddress )
			*value = GS_STRDUP(p->mACAddress);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@endobj EthernetLink **/

/**@obj Ethernet **/

/**@param Ethernet_InterfaceNumberOfEntries                     **/
CPE_STATUS getEthernet_InterfaceNumberOfEntries(Instance *ip, char **value)
{
	cwmpGetObjInstanceCntStr("Interface", value);
	return CPE_OK;
}
/**@endparam                                               **/
/**@param Ethernet_LinkNumberOfEntries                     **/
CPE_STATUS getEthernet_LinkNumberOfEntries(Instance *ip, char **value)
{
	cwmpGetObjInstanceCntStr("Link", value);
	return CPE_OK;
}
/**@endparam                                               **/
/**@endobj Ethernet **/












