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
 * $Id: WANDevice.c,v 1.1 2012/05/10 17:37:59 dmounday Exp $
 *----------------------------------------------------------------------*/

#include "paramTree.h"
#include "rpc.h"
#include "gslib/src/utils.h"
#include "soapRpc/rpcUtils.h"
#include "soapRpc/rpcMethods.h"
#include "soapRpc/cwmpSession.h"


/**@obj WANDeviceWANCommonInterfaceConfig **/
/*
 * Create WAN connections.
 * Called by the initialization code to create the WAN
 * object instances.
 *
 */
#include "includes/cpedata.h"
#include "includes/netIPState.h"
extern NetIPState cpeNetIPState;
const char* dfltWANIF(void);

AvailInterface *findAvailIF( const char *ifname);

static AvailInterface *wanIF;

static int internetEnabled = 1;		/* default enabled */

static WANIPConnection wanIPConn[MAXWANIPCONN];


/*
 * Find an available wanIPConn element.
 * Mark is as active and return the array index.
 */
static int findAvailWANIPConn(void){
	int	i;
	for (i=0; i<MAXWANIPCONN; ++i){
		if ( !(wanIPConn[i].flag&ENTRY_ACTIVE) ){
			wanIPConn[i].flag = ENTRY_ACTIVE;
			return i;
		}
	}
	return -1;
}
/*
 * Free the wanIPConn element indexed by i.
 */
static void freeWANIPConn(int i){
	memset(&wanIPConn[i], 0, sizeof(WANIPConnection));
}

static WANPPPConnection wanPPPConn[MAXWANPPPCONN];
/*
 * Find an available wanIPConn element.
 * Mark is as active and return the array index.
 */
static int findAvailWANPPPConn(void){
	int	i;
	for (i=0; i<MAXWANPPPCONN; ++i){
		if ( !(wanPPPConn[i].flag&ENTRY_ACTIVE) ){
			wanPPPConn[i].flag = ENTRY_ACTIVE;
			return i;
		}
	}
	return -1;
}
/*
 * Free the wanIPConn element indexed by i.
 */
static void freeWANPPPConn(int i){
	memset(&wanPPPConn[i], 0, sizeof(WANPPPConnection));
}
/***********************************************************
 *  P O R T   M A P P I N G
 *
 *********************************************************/

static PortMapping portMap[MAXMAPS];
/*
 * Find an available portMap element and
 * return the index.
 */
static int findAvailPortMap(void){
	int	i;
	for (i=0; i<MAXMAPS; ++i){
		if ( !(portMap[i].flag&ENTRY_ACTIVE) ){
			portMap[i].flag = ENTRY_ACTIVE;
			return i;
		}
	}
	return -1;
}

/*
 * Free the portMap element indexed by i.
 */
static void freePortMapEntry(int i){
	if ( portMap[i].flag&ENTRY_ACTIVE ){
		GS_FREE(portMap[i].remoteHost);
		GS_FREE(portMap[i].internalClient);
		GS_FREE(portMap[i].description);
		GS_FREE(portMap[i].protocol);
		memset(&portMap[i], 0, sizeof(PortMapping));
	}
}
/*
 * Create WAN connections.
 * Called by the initialization code to create the WAN
 * object instances.
 *
 */

void updateWANConnections(void){

	wanIF=findAvailIF(dfltWANIF());
	/* Use ethernet since we don't have a DSL interface to test any
	 * of this.
	 */
	if ( wanIF ){
		/* create Ethernet connection instances....
		 * InternetGatewayDevice.WANDevice.{i}.WANConnectionDevice.{i}. */
		/* The WANConnectionDevice has a child object that represents the ethernet */
		/* link config for the connection. All of this info should come from   */
		/* the saved configuration. */
		cwmpInitObjectInstance("InternetGatewayDevice.WANDevice.1.WANConnectionDevice.1.");
		/* For each WANConnectionDevice the pAddObj() function automatically creates */
		/* InternetGatewayDevice.WANDevice.{i}.WANCommonInterfaceConfig.Connection.{i}. instance */
		/* This instance is deleted automatically by the CPE. */

		/* create InternetGatewayDevice.WANDevice.{i}.WANConnectionDevice.{i}.WANIPConnection.{i}. */
		cwmpInitObjectInstance("InternetGatewayDevice.WANDevice.1.WANConnectionDevice.1.WANIPConnection.1.");

	}

}
/*
 * Called update the wanIPConn for default WAN device data.
 */
void updateDefaultWANIPConn(void){
	int	i;
	AvailInterface *ap;
	for (i=0; i<MAXWANIPCONN; ++i){
		if ( (wanIPConn[i].flag&ENTRY_ACTIVE)
			&& (ap = wanIPConn[i].ap )) {
				if ( streq(ap->ifname, dfltWANIF())) {
					/* use the IP discovered by cpeGetNetIPInfo() */
					strcpy( wanIPConn[i].externalIP, writeInIPAddr(&cpeNetIPState.ip));
					strcpy( wanIPConn[i].subnetMask, writeInIPAddr(&cpeNetIPState.mask));
				}
		}
	}
	return;
}
/**@param WANDeviceWANCommonInterfaceConfig_EnabledForInternet                     **/
CPE_STATUS setWANDeviceWANCommonInterfaceConfig_EnabledForInternet(Instance *ip, char *value)
{
	/* set parameter */
	internetEnabled = testBoolean(value);
	return CPE_OK;
}
CPE_STATUS getWANDeviceWANCommonInterfaceConfig_EnabledForInternet(Instance *ip, char **value)
{
	/* get parameter */
	*value = GS_STRDUP(internetEnabled? "1": "0");
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANCommonInterfaceConfig_WANAccessType                     **/
CPE_STATUS getWANDeviceWANCommonInterfaceConfig_WANAccessType(Instance *ip, char **value)
{
	/* get parameter */
	// TODO: Determine this from data
	*value = GS_STRDUP(strstr(wanIF->ifname, "eth")? "Ethernet": "DSL");
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANCommonInterfaceConfig_Layer1UpstreamMaxBitRate                     **/
CPE_STATUS getWANDeviceWANCommonInterfaceConfig_Layer1UpstreamMaxBitRate(Instance *ip, char **value)
{
	/* get parameter */
	*value = GS_STRDUP("100000000");
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANCommonInterfaceConfig_Layer1DownstreamMaxBitRate                     **/
CPE_STATUS getWANDeviceWANCommonInterfaceConfig_Layer1DownstreamMaxBitRate(Instance *ip, char **value)
{
	/* get parameter */
	*value = GS_STRDUP("100000000");
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANCommonInterfaceConfig_PhysicalLinkStatus                     **/
CPE_STATUS getWANDeviceWANCommonInterfaceConfig_PhysicalLinkStatus(Instance *ip, char **value)
{
	/* get parameter */
	*value = GS_STRDUP(wanIF->ifStatus== eUP? "Up": "Down");
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANCommonInterfaceConfig_TotalBytesSent                     **/
CPE_STATUS getWANDeviceWANCommonInterfaceConfig_TotalBytesSent(Instance *ip, char **value)
{
	/* get parameter */
	*value = GS_STRDUP(wanIF->txBytes);
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANCommonInterfaceConfig_TotalBytesReceived                     **/
CPE_STATUS getWANDeviceWANCommonInterfaceConfig_TotalBytesReceived(Instance *ip, char **value)
{
	/* get parameter */
	*value = GS_STRDUP(wanIF->rxBytes);
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANCommonInterfaceConfig_TotalPacketsSent                     **/
CPE_STATUS getWANDeviceWANCommonInterfaceConfig_TotalPacketsSent(Instance *ip, char **value)
{
	/* get parameter */
	 *value = GS_STRDUP(wanIF->txPackets);
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANCommonInterfaceConfig_TotalPacketsReceived                     **/
CPE_STATUS getWANDeviceWANCommonInterfaceConfig_TotalPacketsReceived(Instance *ip, char **value)
{
	/* get parameter */
	 *value = GS_STRDUP(wanIF->rxPackets);
	return CPE_OK;
}
/**@endparam                                               **/
/**@endobj WANDeviceWANCommonInterfaceConfig **/

/**@obj WANDeviceWANConnectionDeviceWANDSLLinkConfig **/

/**@param WANDeviceWANConnectionDeviceWANDSLLinkConfig_Enable                     **/
CPE_STATUS setWANDeviceWANConnectionDeviceWANDSLLinkConfig_Enable(Instance *ip, char *value)
{
	/* set parameter */
	return CPE_OK;
}
CPE_STATUS getWANDeviceWANConnectionDeviceWANDSLLinkConfig_Enable(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANDSLLinkConfig_LinkStatus                     **/
CPE_STATUS getWANDeviceWANConnectionDeviceWANDSLLinkConfig_LinkStatus(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANDSLLinkConfig_LinkType                     **/
CPE_STATUS setWANDeviceWANConnectionDeviceWANDSLLinkConfig_LinkType(Instance *ip, char *value)
{
	/* set parameter */
	return CPE_OK;
}
CPE_STATUS getWANDeviceWANConnectionDeviceWANDSLLinkConfig_LinkType(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANDSLLinkConfig_AutoConfig                     **/
CPE_STATUS getWANDeviceWANConnectionDeviceWANDSLLinkConfig_AutoConfig(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANDSLLinkConfig_DestinationAddress                     **/
CPE_STATUS setWANDeviceWANConnectionDeviceWANDSLLinkConfig_DestinationAddress(Instance *ip, char *value)
{
	/* set parameter */
	return CPE_OK;
}
CPE_STATUS getWANDeviceWANConnectionDeviceWANDSLLinkConfig_DestinationAddress(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANDSLLinkConfig_ATMTransmittedBlocks                     **/
CPE_STATUS getWANDeviceWANConnectionDeviceWANDSLLinkConfig_ATMTransmittedBlocks(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANDSLLinkConfig_ATMReceivedBlocks                     **/
CPE_STATUS getWANDeviceWANConnectionDeviceWANDSLLinkConfig_ATMReceivedBlocks(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANDSLLinkConfig_AAL5CRCErrors                     **/
CPE_STATUS getWANDeviceWANConnectionDeviceWANDSLLinkConfig_AAL5CRCErrors(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANDSLLinkConfig_ATMCRCErrors                     **/
CPE_STATUS getWANDeviceWANConnectionDeviceWANDSLLinkConfig_ATMCRCErrors(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@endobj WANDeviceWANConnectionDeviceWANDSLLinkConfig **/

/**@obj WANDeviceWANConnectionDeviceWANEthernetLinkConfig **/

/**@param WANDeviceWANConnectionDeviceWANEthernetLinkConfig_EthernetLinkStatus                     **/
CPE_STATUS getWANDeviceWANConnectionDeviceWANEthernetLinkConfig_EthernetLinkStatus(Instance *ip, char **value)
{
	/* get parameter */
	*value = GS_STRDUP(wanIF->ifStatus==eUP? "Up": "Down");
	return CPE_OK;
}
/**@endparam                                               **/
/**@endobj WANDeviceWANConnectionDeviceWANEthernetLinkConfig **/

/**@obj WANDeviceWANConnectionDeviceWANIPConnectionDHCPClientReqDHCPOption **/
CPE_STATUS  addWANDeviceWANConnectionDeviceWANIPConnectionDHCPClientReqDHCPOption(CWMPObject *o, Instance *ip)
{
	/* add instance data */
	return CPE_OK;
}
CPE_STATUS  delWANDeviceWANConnectionDeviceWANIPConnectionDHCPClientReqDHCPOption(CWMPObject *o, Instance *ip)
{
	/* delete instance data */
	return CPE_OK;
}

/**@param WANDeviceWANConnectionDeviceWANIPConnectionDHCPClientReqDHCPOption_Enable                     **/
CPE_STATUS setWANDeviceWANConnectionDeviceWANIPConnectionDHCPClientReqDHCPOption_Enable(Instance *ip, char *value)
{
	/* set parameter */
	return CPE_OK;
}
CPE_STATUS getWANDeviceWANConnectionDeviceWANIPConnectionDHCPClientReqDHCPOption_Enable(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANIPConnectionDHCPClientReqDHCPOption_Tag                     **/
CPE_STATUS setWANDeviceWANConnectionDeviceWANIPConnectionDHCPClientReqDHCPOption_Tag(Instance *ip, char *value)
{
	/* set parameter */
	return CPE_OK;
}
CPE_STATUS getWANDeviceWANConnectionDeviceWANIPConnectionDHCPClientReqDHCPOption_Tag(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANIPConnectionDHCPClientReqDHCPOption_Value                     **/
CPE_STATUS getWANDeviceWANConnectionDeviceWANIPConnectionDHCPClientReqDHCPOption_Value(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@endobj WANDeviceWANConnectionDeviceWANIPConnectionDHCPClientReqDHCPOption **/

/**@obj WANDeviceWANConnectionDeviceWANIPConnectionDHCPClientSentDHCPOption **/
CPE_STATUS  addWANDeviceWANConnectionDeviceWANIPConnectionDHCPClientSentDHCPOption(CWMPObject *o, Instance *ip)
{
	/* add instance data */
	return CPE_OK;
}
CPE_STATUS  delWANDeviceWANConnectionDeviceWANIPConnectionDHCPClientSentDHCPOption(CWMPObject *o, Instance *ip)
{
	/* delete instance data */
	return CPE_OK;
}

/**@param WANDeviceWANConnectionDeviceWANIPConnectionDHCPClientSentDHCPOption_Enable                     **/
CPE_STATUS setWANDeviceWANConnectionDeviceWANIPConnectionDHCPClientSentDHCPOption_Enable(Instance *ip, char *value)
{
	/* set parameter */
	return CPE_OK;
}
CPE_STATUS getWANDeviceWANConnectionDeviceWANIPConnectionDHCPClientSentDHCPOption_Enable(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANIPConnectionDHCPClientSentDHCPOption_Tag                     **/
CPE_STATUS setWANDeviceWANConnectionDeviceWANIPConnectionDHCPClientSentDHCPOption_Tag(Instance *ip, char *value)
{
	/* set parameter */
	return CPE_OK;
}
CPE_STATUS getWANDeviceWANConnectionDeviceWANIPConnectionDHCPClientSentDHCPOption_Tag(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANIPConnectionDHCPClientSentDHCPOption_Value                     **/
CPE_STATUS setWANDeviceWANConnectionDeviceWANIPConnectionDHCPClientSentDHCPOption_Value(Instance *ip, char *value)
{
	/* set parameter */
	return CPE_OK;
}
CPE_STATUS getWANDeviceWANConnectionDeviceWANIPConnectionDHCPClientSentDHCPOption_Value(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@endobj WANDeviceWANConnectionDeviceWANIPConnectionDHCPClientSentDHCPOption **/

/**@obj WANDeviceWANConnectionDeviceWANIPConnectionDHCPClient **/

/**@param WANDeviceWANConnectionDeviceWANIPConnectionDHCPClient_SentDHCPOptionNumberOfEntries                     **/
CPE_STATUS getWANDeviceWANConnectionDeviceWANIPConnectionDHCPClient_SentDHCPOptionNumberOfEntries(Instance *ip, char **value)
{
	cwmpGetObjInstanceCntStr("SentDHCPOption", value);
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANIPConnectionDHCPClient_ReqDHCPOptionNumberOfEntries                     **/
CPE_STATUS getWANDeviceWANConnectionDeviceWANIPConnectionDHCPClient_ReqDHCPOptionNumberOfEntries(Instance *ip, char **value)
{
	cwmpGetObjInstanceCntStr("ReqDHCPOption", value);
	return CPE_OK;
}
/**@endparam                                               **/
/**@endobj WANDeviceWANConnectionDeviceWANIPConnectionDHCPClient **/

/**@obj WANDeviceWANConnectionDeviceWANIPConnectionPortMapping **/
CPE_STATUS  addWANDeviceWANConnectionDeviceWANIPConnectionPortMapping(CWMPObject *o, Instance *ip)
{
	int		pMap;
	if ( (pMap=findAvailPortMap())>=0){
		ip->cpeData = (void *)pMap;
		return CPE_OK;
	}
	return CPE_ERR;
}
CPE_STATUS commitPortMapping(CWMPObject *o, Instance *ip, eCommitCBType cmt);
CPE_STATUS  delWANDeviceWANConnectionDeviceWANIPConnectionPortMapping(CWMPObject *o, Instance *ip)
{

	int pMap = (int) ip->cpeData;
	if ( portMap[pMap].flag&ENTRY_ENABLE ){
		portMap[pMap].flag &= ~ENTRY_ENABLE;
		portMap[pMap].flag |= ENTRY_OPPENDING;
		commitPortMapping( o, ip, COMMIT_SET);
	}
	freePortMapEntry(pMap);
	return CPE_OK;
}
CPE_STATUS commitPortMapping(CWMPObject *o, Instance *ip, eCommitCBType cmt){

	int pMap = (int)ip->cpeData;
	if ( portMap[pMap].flag& ENTRY_OPPENDING){
		if ( portMap[pMap].flag == ENTRY_ENABLE)
			fprintf(stderr, "Enable port mapping\n");
		else
			fprintf(stderr," Disable port mapping\n");
	}
	portMap[pMap].flag&= ~ENTRY_OPPENDING;
	return CPE_OK;
}
CPE_STATUS  commitWANDeviceWANConnectionDeviceWANIPConnectionPortMapping(CWMPObject *o, Instance *ip, eCommitCBType cmt)
{
	/* commit object instance */
	return commitPortMapping(o, ip, cmt);
}
/**@param WANDeviceWANConnectionDeviceWANIPConnectionPortMapping_PortMappingEnabled                     **/
CPE_STATUS setWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_PortMappingEnabled(Instance *ip, char *value)
{
	/* set parameter */
	/* Set parameter */
	int pMap = (int) ip->cpeData;
	portMap[pMap].flag&= ~ENTRY_OPPENDING;
	if ( testBoolean(value))
		portMap[pMap].flag |=ENTRY_ENABLE;
	else
		portMap[pMap].flag &=~ENTRY_ENABLE;
	return CPE_OK;
}
CPE_STATUS getWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_PortMappingEnabled(Instance *ip, char **value)
{
	/* get parameter */
	int pMap = (int) ip->cpeData;
	*value = GS_STRDUP(portMap[pMap].flag&ENTRY_ENABLE? "1": "0");
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANIPConnectionPortMapping_PortMappingLeaseDuration                     **/
CPE_STATUS setWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_PortMappingLeaseDuration(Instance *ip, char *value)
{
	/* set parameter */
	int pMap = (int)ip->cpeData;
	portMap[pMap].leaseDuration = atoi(value);
	return CPE_OK;
}
CPE_STATUS getWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_PortMappingLeaseDuration(Instance *ip, char **value)
{
	/* get parameter */
	int pMap = (int)ip->cpeData;
	 *value = GS_STRDUP(itoa(portMap[pMap].leaseDuration));
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANIPConnectionPortMapping_RemoteHost                     **/
CPE_STATUS setWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_RemoteHost(Instance *ip, char *value)
{
	/* set parameter */
	int pMap = (int)ip->cpeData;
	COPYSTR(portMap[pMap].remoteHost,value);
	return CPE_OK;
}
CPE_STATUS getWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_RemoteHost(Instance *ip, char **value)
{
	/* get parameter */
	int pMap = (int)ip->cpeData;
	if ( portMap[pMap].remoteHost )
		*value = GS_STRDUP(portMap[pMap].remoteHost);
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANIPConnectionPortMapping_ExternalPort                     **/
CPE_STATUS setWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_ExternalPort(Instance *ip, char *value)
{
	/* set parameter */
	int pMap = (int)ip->cpeData;
	portMap[pMap].externalPort = atoi(value);
	return CPE_OK;
}
CPE_STATUS getWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_ExternalPort(Instance *ip, char **value)
{
	/* get parameter */
	int pMap = (int)ip->cpeData;
	 *value = GS_STRDUP(itoa(portMap[pMap].externalPort));
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANIPConnectionPortMapping_InternalPort                     **/
CPE_STATUS setWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_InternalPort(Instance *ip, char *value)
{
	/* set parameter */
	int pMap = (int)ip->cpeData;
	portMap[pMap].internalPort = atoi(value);
	return CPE_OK;
}
CPE_STATUS getWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_InternalPort(Instance *ip, char **value)
{
	/* get parameter */
	int pMap = (int)ip->cpeData;
	*value = GS_STRDUP(itoa(portMap[pMap].internalPort));
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANIPConnectionPortMapping_PortMappingProtocol                     **/
CPE_STATUS setWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_PortMappingProtocol(Instance *ip, char *value)
{
	/* set parameter */
	int pMap = (int)ip->cpeData;
	COPYSTR(portMap[pMap].protocol, value);
	return CPE_OK;
}
CPE_STATUS getWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_PortMappingProtocol(Instance *ip, char **value)
{
	/* get parameter */
	int pMap = (int)ip->cpeData;
	if ( portMap[pMap].protocol )
		*value = GS_STRDUP(portMap[pMap].protocol);
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANIPConnectionPortMapping_InternalClient                     **/
CPE_STATUS setWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_InternalClient(Instance *ip, char *value)
{
	/* set parameter */
	int pMap = (int)ip->cpeData;
	COPYSTR(portMap[pMap].internalClient, value);
	return CPE_OK;
}
CPE_STATUS getWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_InternalClient(Instance *ip, char **value)
{
	/* get parameter */
	int pMap = (int)ip->cpeData;
	if ( portMap[pMap].internalClient )
		*value = GS_STRDUP(portMap[pMap].internalClient);
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANIPConnectionPortMapping_PortMappingDescription                     **/
CPE_STATUS setWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_PortMappingDescription(Instance *ip, char *value)
{
	/* set parameter */
	int pMap = (int)ip->cpeData;
	COPYSTR(portMap[pMap].description, value);
	return CPE_OK;
}
CPE_STATUS getWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_PortMappingDescription(Instance *ip, char **value)
{
	/* get parameter */
	int pMap = (int)ip->cpeData;
	if (portMap[pMap].description )
		*value = GS_STRDUP(portMap[pMap].description);
	return CPE_OK;
}
/**@endparam                                               **/
/**@endobj WANDeviceWANConnectionDeviceWANIPConnectionPortMapping **/

/**@obj WANDeviceWANConnectionDeviceWANIPConnectionStats **/

/**@param WANDeviceWANConnectionDeviceWANIPConnectionStats_EthernetBytesSent                     **/
CPE_STATUS getWANDeviceWANConnectionDeviceWANIPConnectionStats_EthernetBytesSent(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANIPConnectionStats_EthernetBytesReceived                     **/
CPE_STATUS getWANDeviceWANConnectionDeviceWANIPConnectionStats_EthernetBytesReceived(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANIPConnectionStats_EthernetPacketsSent                     **/
CPE_STATUS getWANDeviceWANConnectionDeviceWANIPConnectionStats_EthernetPacketsSent(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANIPConnectionStats_EthernetPacketsReceived                     **/
CPE_STATUS getWANDeviceWANConnectionDeviceWANIPConnectionStats_EthernetPacketsReceived(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANIPConnectionStats_EthernetErrorsSent                     **/
CPE_STATUS getWANDeviceWANConnectionDeviceWANIPConnectionStats_EthernetErrorsSent(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANIPConnectionStats_EthernetErrorsReceived                     **/
CPE_STATUS getWANDeviceWANConnectionDeviceWANIPConnectionStats_EthernetErrorsReceived(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANIPConnectionStats_EthernetUnicastPacketsSent                     **/
CPE_STATUS getWANDeviceWANConnectionDeviceWANIPConnectionStats_EthernetUnicastPacketsSent(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANIPConnectionStats_EthernetUnicastPacketsReceived                     **/
CPE_STATUS getWANDeviceWANConnectionDeviceWANIPConnectionStats_EthernetUnicastPacketsReceived(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANIPConnectionStats_EthernetDiscardPacketsSent                     **/
CPE_STATUS getWANDeviceWANConnectionDeviceWANIPConnectionStats_EthernetDiscardPacketsSent(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANIPConnectionStats_EthernetDiscardPacketsReceived                     **/
CPE_STATUS getWANDeviceWANConnectionDeviceWANIPConnectionStats_EthernetDiscardPacketsReceived(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANIPConnectionStats_EthernetMulticastPacketsSent                     **/
CPE_STATUS getWANDeviceWANConnectionDeviceWANIPConnectionStats_EthernetMulticastPacketsSent(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANIPConnectionStats_EthernetMulticastPacketsReceived                     **/
CPE_STATUS getWANDeviceWANConnectionDeviceWANIPConnectionStats_EthernetMulticastPacketsReceived(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANIPConnectionStats_EthernetBroadcastPacketsSent                     **/
CPE_STATUS getWANDeviceWANConnectionDeviceWANIPConnectionStats_EthernetBroadcastPacketsSent(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANIPConnectionStats_EthernetBroadcastPacketsReceived                     **/
CPE_STATUS getWANDeviceWANConnectionDeviceWANIPConnectionStats_EthernetBroadcastPacketsReceived(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANIPConnectionStats_EthernetUnknownProtoPacketsReceived                     **/
CPE_STATUS getWANDeviceWANConnectionDeviceWANIPConnectionStats_EthernetUnknownProtoPacketsReceived(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@endobj WANDeviceWANConnectionDeviceWANIPConnectionStats **/

/**@obj WANDeviceWANConnectionDeviceWANIPConnection **/
CPE_STATUS  addWANDeviceWANConnectionDeviceWANIPConnection(CWMPObject *o, Instance *ip)
{
	Instance *cdIp;      /* parent ConnectionDevice instance ptr*/
	AvailInterface *ap;
	int wi;

	wi = findAvailWANIPConn();
	if ( wi!=-1 ) {
		/* */
		cdIp = ip->parent;
		if ( (ap= (AvailInterface *)cdIp->cpeData)){
			wanIPConn[wi].ap = ap;		/* point at AvailIF */
			if ( streq(ap->ifname, dfltWANIF())) {
				/* use the IP discovered by cpeGetNetIPInfo() */
				strcpy( wanIPConn[wi].externalIP, writeInIPAddr(&cpeNetIPState.ip));
				strcpy( wanIPConn[wi].subnetMask, writeInIPAddr(&cpeNetIPState.mask));
			} else {
				/* get the IP info from the availIF entry */
				strcpy(wanIPConn[wi].externalIP, ap->ip);
			}
			wanIPConn[wi].addrType = ap->addrType;
			strcpy(wanIPConn[wi].subnetMask, ap->mask);
			strcpy(wanIPConn[wi].connType, "IP_Routed");
		}
		ip->cpeData = (void *)wi;
		wanIPConn[wi].dnsEnabled = 1;
		wanIPConn[wi].connIp = cdIp;   /* WANConnectDevice.{i}. pointer */
		return CPE_OK;
	}
	return CPE_ERR;
}
CPE_STATUS  delWANDeviceWANConnectionDeviceWANIPConnection(CWMPObject *o, Instance *ip)
{
	/* Delete Instance */
	int wi = (int)ip->cpeData;
	if ( wanIPConn[wi].flag&ENTRY_WANDFLT )
		return CPE_ERR;             /* don't allow delete of WAN Default instance */
	freeWANIPConn(wi);
	return CPE_OK;
}

/**@param WANDeviceWANConnectionDeviceWANIPConnection_Enable                     **/
CPE_STATUS setWANDeviceWANConnectionDeviceWANIPConnection_Enable(Instance *ip, char *value)
{
	/* set parameter */
	return CPE_OK;
}
CPE_STATUS getWANDeviceWANConnectionDeviceWANIPConnection_Enable(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANIPConnection_ConnectionStatus                     **/
CPE_STATUS getWANDeviceWANConnectionDeviceWANIPConnection_ConnectionStatus(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANIPConnection_PossibleConnectionTypes                     **/
CPE_STATUS getWANDeviceWANConnectionDeviceWANIPConnection_PossibleConnectionTypes(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANIPConnection_ConnectionType                     **/
CPE_STATUS setWANDeviceWANConnectionDeviceWANIPConnection_ConnectionType(Instance *ip, char *value)
{
	/* set parameter */
	return CPE_OK;
}
CPE_STATUS getWANDeviceWANConnectionDeviceWANIPConnection_ConnectionType(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANIPConnection_Name                     **/
CPE_STATUS setWANDeviceWANConnectionDeviceWANIPConnection_Name(Instance *ip, char *value)
{
	/* set parameter */
	return CPE_OK;
}
CPE_STATUS getWANDeviceWANConnectionDeviceWANIPConnection_Name(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANIPConnection_Uptime                     **/
CPE_STATUS getWANDeviceWANConnectionDeviceWANIPConnection_Uptime(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANIPConnection_LastConnectionError                     **/
CPE_STATUS getWANDeviceWANConnectionDeviceWANIPConnection_LastConnectionError(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANIPConnection_RSIPAvailable                     **/
CPE_STATUS getWANDeviceWANConnectionDeviceWANIPConnection_RSIPAvailable(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANIPConnection_NATEnabled                     **/
CPE_STATUS setWANDeviceWANConnectionDeviceWANIPConnection_NATEnabled(Instance *ip, char *value)
{
	/* set parameter */
	return CPE_OK;
}
CPE_STATUS getWANDeviceWANConnectionDeviceWANIPConnection_NATEnabled(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANIPConnection_AddressingType                     **/
CPE_STATUS setWANDeviceWANConnectionDeviceWANIPConnection_AddressingType(Instance *ip, char *value)
{
	/* set parameter */
	return CPE_OK;
}
CPE_STATUS getWANDeviceWANConnectionDeviceWANIPConnection_AddressingType(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANIPConnection_ExternalIPAddress                     **/
CPE_STATUS setWANDeviceWANConnectionDeviceWANIPConnection_ExternalIPAddress(Instance *ip, char *value)
{
	/* set parameter */
	return CPE_OK;
}
CPE_STATUS getWANDeviceWANConnectionDeviceWANIPConnection_ExternalIPAddress(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANIPConnection_SubnetMask                     **/
CPE_STATUS setWANDeviceWANConnectionDeviceWANIPConnection_SubnetMask(Instance *ip, char *value)
{
	/* set parameter */
	return CPE_OK;
}
CPE_STATUS getWANDeviceWANConnectionDeviceWANIPConnection_SubnetMask(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANIPConnection_DefaultGateway                     **/
CPE_STATUS setWANDeviceWANConnectionDeviceWANIPConnection_DefaultGateway(Instance *ip, char *value)
{
	/* set parameter */
	return CPE_OK;
}
CPE_STATUS getWANDeviceWANConnectionDeviceWANIPConnection_DefaultGateway(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANIPConnection_DNSEnabled                     **/
CPE_STATUS setWANDeviceWANConnectionDeviceWANIPConnection_DNSEnabled(Instance *ip, char *value)
{
	/* set parameter */
	return CPE_OK;
}
CPE_STATUS getWANDeviceWANConnectionDeviceWANIPConnection_DNSEnabled(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANIPConnection_DNSOverrideAllowed                     **/
CPE_STATUS setWANDeviceWANConnectionDeviceWANIPConnection_DNSOverrideAllowed(Instance *ip, char *value)
{
	/* set parameter */
	return CPE_OK;
}
CPE_STATUS getWANDeviceWANConnectionDeviceWANIPConnection_DNSOverrideAllowed(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANIPConnection_DNSServers                     **/
CPE_STATUS setWANDeviceWANConnectionDeviceWANIPConnection_DNSServers(Instance *ip, char *value)
{
	/* set parameter */
	return CPE_OK;
}
CPE_STATUS getWANDeviceWANConnectionDeviceWANIPConnection_DNSServers(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANIPConnection_MACAddress                     **/
CPE_STATUS setWANDeviceWANConnectionDeviceWANIPConnection_MACAddress(Instance *ip, char *value)
{
	/* set parameter */
	return CPE_OK;
}
CPE_STATUS getWANDeviceWANConnectionDeviceWANIPConnection_MACAddress(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANIPConnection_ConnectionTrigger                     **/
CPE_STATUS setWANDeviceWANConnectionDeviceWANIPConnection_ConnectionTrigger(Instance *ip, char *value)
{
	/* set parameter */
	return CPE_OK;
}
CPE_STATUS getWANDeviceWANConnectionDeviceWANIPConnection_ConnectionTrigger(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANIPConnection_RouteProtocolRx                     **/
CPE_STATUS setWANDeviceWANConnectionDeviceWANIPConnection_RouteProtocolRx(Instance *ip, char *value)
{
	/* set parameter */
	return CPE_OK;
}
CPE_STATUS getWANDeviceWANConnectionDeviceWANIPConnection_RouteProtocolRx(Instance *ip, char **value)
{
	/* get parameter */
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
/**@param WANDeviceWANConnectionDeviceWANIPConnection_Reset                     **/
CPE_STATUS setWANDeviceWANConnectionDeviceWANIPConnection_Reset(Instance *ip, char *value)
{
	/* set parameter */
	return CPE_OK;
}
CPE_STATUS getWANDeviceWANConnectionDeviceWANIPConnection_Reset(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@endobj WANDeviceWANConnectionDeviceWANIPConnection **/

/**@obj WANDeviceWANConnectionDeviceWANPPPConnectionPortMapping **/
CPE_STATUS  addWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping(CWMPObject *o, Instance *ip)
{
	int pMap;
	if ( (pMap=findAvailPortMap())>=0){
		ip->cpeData = (void *)pMap;
		return CPE_OK;
	}
	return CPE_ERR;
}
CPE_STATUS  delWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping(CWMPObject *o, Instance *ip)
{
	/* Delete Instance */
	int pMap = (int) ip->cpeData;
	if ( portMap[pMap].flag&ENTRY_ENABLE ){
		portMap[pMap].flag &= ~ENTRY_ENABLE;
		portMap[pMap].flag |= ENTRY_OPPENDING;
		commitPortMapping( o, ip, COMMIT_SET);
	}
	freePortMapEntry(pMap);
	return CPE_OK;
}
CPE_STATUS  commitWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping(CWMPObject *o, Instance *ip, eCommitCBType cmt)
{
	/* commit object instance */
	return commitPortMapping(o, ip, cmt);
}
/**@param WANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_PortMappingEnabled                     **/
CPE_STATUS setWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_PortMappingEnabled(Instance *ip, char *value)
{
	/* Set parameter */
	int pMap = (int) ip->cpeData;
	portMap[pMap].flag&= ~ENTRY_OPPENDING;
	if ( testBoolean(value))
		portMap[pMap].flag |=ENTRY_ENABLE;
	else
		portMap[pMap].flag &=~ENTRY_ENABLE;
	return CPE_OK;
}
CPE_STATUS getWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_PortMappingEnabled(Instance *ip, char **value)
{
	/* get parameter */
	int pMap = (int) ip->cpeData;
	*value = GS_STRDUP(portMap[pMap].flag&ENTRY_ENABLE? "1": "0");
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_PortMappingLeaseDuration                     **/
CPE_STATUS setWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_PortMappingLeaseDuration(Instance *ip, char *value)
{
	/* Set parameter */
	int pMap = (int)ip->cpeData;
	portMap[pMap].leaseDuration = atoi(value);
	return CPE_OK;
}
CPE_STATUS getWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_PortMappingLeaseDuration(Instance *ip, char **value)
{
	/* get parameter */
	int pMap = (int)ip->cpeData;
	 *value = GS_STRDUP(itoa(portMap[pMap].leaseDuration));
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_RemoteHost                     **/
CPE_STATUS setWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_RemoteHost(Instance *ip, char *value)
{
	/* Set parameter */
	int pMap = (int)ip->cpeData;
	COPYSTR(portMap[pMap].remoteHost,value);
	return CPE_OK;
}
CPE_STATUS getWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_RemoteHost(Instance *ip, char **value)
{
	/* get parameter */
	int pMap = (int)ip->cpeData;
	if (portMap[pMap].remoteHost )
	 *value = GS_STRDUP(portMap[pMap].remoteHost);
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_ExternalPort                     **/
CPE_STATUS setWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_ExternalPort(Instance *ip, char *value)
{
	/* Set parameter */
	int pMap = (int)ip->cpeData;
	portMap[pMap].externalPort = atoi(value);
	return CPE_OK;
}
CPE_STATUS getWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_ExternalPort(Instance *ip, char **value)
{
	/* get parameter */
	int pMap = (int)ip->cpeData;
	 *value = GS_STRDUP(itoa(portMap[pMap].externalPort));
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_InternalPort                     **/
CPE_STATUS setWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_InternalPort(Instance *ip, char *value)
{
	/* Set parameter */
	int pMap = (int)ip->cpeData;
	portMap[pMap].internalPort = atoi(value);
	return CPE_OK;
}
CPE_STATUS getWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_InternalPort(Instance *ip, char **value)
{
	/* get parameter */
	int pMap = (int)ip->cpeData;
	*value = GS_STRDUP(itoa(portMap[pMap].internalPort));
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_PortMappingProtocol                     **/
CPE_STATUS setWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_PortMappingProtocol(Instance *ip, char *value)
{
	/* set parameter */
	int pMap = (int)ip->cpeData;
	COPYSTR(portMap[pMap].protocol, value);
	return CPE_OK;
}
CPE_STATUS getWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_PortMappingProtocol(Instance *ip, char **value)
{
	/* get parameter */
	int pMap = (int)ip->cpeData;
	if ( portMap[pMap].protocol )
	 *value = GS_STRDUP(portMap[pMap].protocol);
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_InternalClient                     **/
CPE_STATUS setWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_InternalClient(Instance *ip, char *value)
{
	/* set parameter */
	int pMap = (int)ip->cpeData;
	COPYSTR(portMap[pMap].internalClient, value);
	return CPE_OK;
}
CPE_STATUS getWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_InternalClient(Instance *ip, char **value)
{
	/* get parameter */
	int pMap = (int)ip->cpeData;
	if (portMap[pMap].internalClient)
	 *value = GS_STRDUP(portMap[pMap].internalClient);
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_PortMappingDescription                     **/
CPE_STATUS setWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_PortMappingDescription(Instance *ip, char *value)
{
	/* set parameter */
	int pMap = (int)ip->cpeData;
	COPYSTR(portMap[pMap].description, value);
	return CPE_OK;
}
CPE_STATUS getWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_PortMappingDescription(Instance *ip, char **value)
{
	/* get parameter */
	int pMap = (int)ip->cpeData;
	if ( portMap[pMap].description )
	 *value = GS_STRDUP(portMap[pMap].description);
	return CPE_OK;
}
/**@endparam                                               **/
/**@endobj WANDeviceWANConnectionDeviceWANPPPConnectionPortMapping **/

/**@obj WANDeviceWANConnectionDeviceWANPPPConnectionStats **/

/**@param WANDeviceWANConnectionDeviceWANPPPConnectionStats_EthernetBytesSent                     **/
CPE_STATUS getWANDeviceWANConnectionDeviceWANPPPConnectionStats_EthernetBytesSent(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANPPPConnectionStats_EthernetBytesReceived                     **/
CPE_STATUS getWANDeviceWANConnectionDeviceWANPPPConnectionStats_EthernetBytesReceived(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANPPPConnectionStats_EthernetPacketsSent                     **/
CPE_STATUS getWANDeviceWANConnectionDeviceWANPPPConnectionStats_EthernetPacketsSent(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANPPPConnectionStats_EthernetPacketsReceived                     **/
CPE_STATUS getWANDeviceWANConnectionDeviceWANPPPConnectionStats_EthernetPacketsReceived(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANPPPConnectionStats_EthernetErrorsSent                     **/
CPE_STATUS getWANDeviceWANConnectionDeviceWANPPPConnectionStats_EthernetErrorsSent(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANPPPConnectionStats_EthernetErrorsReceived                     **/
CPE_STATUS getWANDeviceWANConnectionDeviceWANPPPConnectionStats_EthernetErrorsReceived(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANPPPConnectionStats_EthernetUnicastPacketsSent                     **/
CPE_STATUS getWANDeviceWANConnectionDeviceWANPPPConnectionStats_EthernetUnicastPacketsSent(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANPPPConnectionStats_EthernetUnicastPacketsReceived                     **/
CPE_STATUS getWANDeviceWANConnectionDeviceWANPPPConnectionStats_EthernetUnicastPacketsReceived(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANPPPConnectionStats_EthernetDiscardPacketsSent                     **/
CPE_STATUS getWANDeviceWANConnectionDeviceWANPPPConnectionStats_EthernetDiscardPacketsSent(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANPPPConnectionStats_EthernetDiscardPacketsReceived                     **/
CPE_STATUS getWANDeviceWANConnectionDeviceWANPPPConnectionStats_EthernetDiscardPacketsReceived(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANPPPConnectionStats_EthernetMulticastPacketsSent                     **/
CPE_STATUS getWANDeviceWANConnectionDeviceWANPPPConnectionStats_EthernetMulticastPacketsSent(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANPPPConnectionStats_EthernetMulticastPacketsReceived                     **/
CPE_STATUS getWANDeviceWANConnectionDeviceWANPPPConnectionStats_EthernetMulticastPacketsReceived(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANPPPConnectionStats_EthernetBroadcastPacketsSent                     **/
CPE_STATUS getWANDeviceWANConnectionDeviceWANPPPConnectionStats_EthernetBroadcastPacketsSent(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANPPPConnectionStats_EthernetBroadcastPacketsReceived                     **/
CPE_STATUS getWANDeviceWANConnectionDeviceWANPPPConnectionStats_EthernetBroadcastPacketsReceived(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANPPPConnectionStats_EthernetUnknownProtoPacketsReceived                     **/
CPE_STATUS getWANDeviceWANConnectionDeviceWANPPPConnectionStats_EthernetUnknownProtoPacketsReceived(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@endobj WANDeviceWANConnectionDeviceWANPPPConnectionStats **/

/**@obj WANDeviceWANConnectionDeviceWANPPPConnection **/
CPE_STATUS  addWANDeviceWANConnectionDeviceWANPPPConnection(CWMPObject *o, Instance *ip)
{
	Instance *cdIp;      /* parent ConnectionDevice instance ptr*/
	int	wi;
	AvailInterface *ap;

	wi = findAvailWANPPPConn();
	if ( (wi!=-1) ) {
		cdIp = ip->parent;
		/* The interface is in the AvailIF table. If DSL it must be added */
		/* when the VPC is created */
		if ( (ap= (AvailInterface *)cdIp->cpeData)){
			wanPPPConn[wi].ap = ap;		/* point at AvailIF */
		}
		ip->cpeData = (void *)wi;       /* link Instance to the wanPPPConn entry*/
		wanIPConn[wi].connIp = cdIp;    /* WANConnectDevice.{i}. pointer */
		return CPE_OK;
	}
	return CPE_ERR;
}
CPE_STATUS  delWANDeviceWANConnectionDeviceWANPPPConnection(CWMPObject *o, Instance *ip)
{
	/* Delete Instance */
	int wi = (int)ip->cpeData;
	if ( wanPPPConn[wi].flag&ENTRY_WANDFLT )
		return CPE_ERR;             /* don't allow delete of WAN Default instance */
	freeWANPPPConn(wi);
	return CPE_OK;
}

/**@param WANDeviceWANConnectionDeviceWANPPPConnection_Enable                     **/
CPE_STATUS setWANDeviceWANConnectionDeviceWANPPPConnection_Enable(Instance *ip, char *value)
{
	/* set parameter */
	int wi = (int)ip->cpeData;
	if ( testBoolean(value))
		wanPPPConn[wi].flag |= ENTRY_ENABLE|ENTRY_OPPENDING;
	else {
		wanPPPConn[wi].flag |= ENTRY_OPPENDING;
		wanPPPConn[wi].flag &= ~ENTRY_ENABLE;
	}
	return CPE_OK;
}
CPE_STATUS getWANDeviceWANConnectionDeviceWANPPPConnection_Enable(Instance *ip, char **value)
{
	/* get parameter */
	int wi = (int)ip->cpeData;
	*value = GS_STRDUP(wanPPPConn[wi].flag&ENTRY_ENABLED ? "1": "0");
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANPPPConnection_ConnectionStatus                     **/
CPE_STATUS getWANDeviceWANConnectionDeviceWANPPPConnection_ConnectionStatus(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANPPPConnection_PossibleConnectionTypes                     **/
CPE_STATUS getWANDeviceWANConnectionDeviceWANPPPConnection_PossibleConnectionTypes(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANPPPConnection_ConnectionType                     **/
CPE_STATUS setWANDeviceWANConnectionDeviceWANPPPConnection_ConnectionType(Instance *ip, char *value)
{
	/* set parameter */
	int wi = (int)ip->cpeData;
	strCpyLimited(wanPPPConn[wi].connType, value, sizeof(wanPPPConn[wi].connType));
	return CPE_OK;
}
CPE_STATUS getWANDeviceWANConnectionDeviceWANPPPConnection_ConnectionType(Instance *ip, char **value)
{
	int wi = (int)ip->cpeData;
	*value = GS_STRDUP(wanPPPConn[wi].connType);
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANPPPConnection_Name                     **/
CPE_STATUS setWANDeviceWANConnectionDeviceWANPPPConnection_Name(Instance *ip, char *value)
{
	/* set parameter */
	int wi = (int)ip->cpeData;
	strCpyLimited(wanPPPConn[wi].name, value, sizeof( wanPPPConn[wi].name));
	return CPE_OK;
}
CPE_STATUS getWANDeviceWANConnectionDeviceWANPPPConnection_Name(Instance *ip, char **value)
{
	/* get parameter */
	int wi = (int)ip->cpeData;
	*value = GS_STRDUP(wanPPPConn[wi].name);
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANPPPConnection_Uptime                     **/
CPE_STATUS getWANDeviceWANConnectionDeviceWANPPPConnection_Uptime(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANPPPConnection_LastConnectionError                     **/
CPE_STATUS getWANDeviceWANConnectionDeviceWANPPPConnection_LastConnectionError(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANPPPConnection_RSIPAvailable                     **/
CPE_STATUS getWANDeviceWANConnectionDeviceWANPPPConnection_RSIPAvailable(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANPPPConnection_NATEnabled                     **/
CPE_STATUS setWANDeviceWANConnectionDeviceWANPPPConnection_NATEnabled(Instance *ip, char *value)
{
	/* set parameter */
	int wi = (int)ip->cpeData;
	wanPPPConn[wi].natEnabled = testBoolean(value);
	return CPE_OK;
}
CPE_STATUS getWANDeviceWANConnectionDeviceWANPPPConnection_NATEnabled(Instance *ip, char **value)
{
	/* get parameter */
	int wi = (int)ip->cpeData;
	 *value = GS_STRDUP(wanPPPConn[wi].natEnabled? "1":"0");
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANPPPConnection_Username                     **/
CPE_STATUS setWANDeviceWANConnectionDeviceWANPPPConnection_Username(Instance *ip, char *value)
{
	/* set parameter */
	int wi = (int)ip->cpeData;
	strncpy(wanPPPConn[wi].username, value, sizeof(wanPPPConn[wi].username));
	return CPE_OK;
}
CPE_STATUS getWANDeviceWANConnectionDeviceWANPPPConnection_Username(Instance *ip, char **value)
{
	/* get parameter */
	int wi = (int)ip->cpeData;
	*value = GS_STRDUP(wanPPPConn[wi].username);
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANPPPConnection_Password                     **/
CPE_STATUS setWANDeviceWANConnectionDeviceWANPPPConnection_Password(Instance *ip, char *value)
{
	/* set parameter */
	int wi = (int)ip->cpeData;
	strncpy(wanPPPConn[wi].password, value, sizeof(wanPPPConn[wi].password));
	return CPE_OK;
}
CPE_STATUS getWANDeviceWANConnectionDeviceWANPPPConnection_Password(Instance *ip, char **value)
{
	/* get parameter */
	int wi = (int)ip->cpeData;
	*value = GS_STRDUP(wanPPPConn[wi].password);
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANPPPConnection_ExternalIPAddress                     **/
CPE_STATUS getWANDeviceWANConnectionDeviceWANPPPConnection_ExternalIPAddress(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANPPPConnection_DNSEnabled                     **/
CPE_STATUS setWANDeviceWANConnectionDeviceWANPPPConnection_DNSEnabled(Instance *ip, char *value)
{
	/* set parameter */
	int wi = (int)ip->cpeData;
	wanPPPConn[wi].dnsEnabled = testBoolean(value);
	return CPE_OK;
}
CPE_STATUS getWANDeviceWANConnectionDeviceWANPPPConnection_DNSEnabled(Instance *ip, char **value)
{
	/* get parameter */
	int wi = (int)ip->cpeData;
	*value = GS_STRDUP(wanPPPConn[wi].dnsEnabled?"1":"0");
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANPPPConnection_DNSOverrideAllowed                     **/
CPE_STATUS setWANDeviceWANConnectionDeviceWANPPPConnection_DNSOverrideAllowed(Instance *ip, char *value)
{
	/* set parameter */
	int wi = (int)ip->cpeData;
	wanPPPConn[wi].dnsOverrideAllowed = testBoolean(value);
	return CPE_OK;
}
CPE_STATUS getWANDeviceWANConnectionDeviceWANPPPConnection_DNSOverrideAllowed(Instance *ip, char **value)
{
	/* get parameter */
	int wi = (int)ip->cpeData;
	*value = GS_STRDUP(wanPPPConn[wi].dnsOverrideAllowed?"1":"0");
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANPPPConnection_DNSServers                     **/
CPE_STATUS setWANDeviceWANConnectionDeviceWANPPPConnection_DNSServers(Instance *ip, char *value)
{
	/* set parameter */
	int wi = (int)ip->cpeData;
	strncpy(wanPPPConn[wi].dnsServers, value, sizeof(wanPPPConn[wi].dnsServers));
	return CPE_OK;
}
CPE_STATUS getWANDeviceWANConnectionDeviceWANPPPConnection_DNSServers(Instance *ip, char **value)
{
	/* get parameter */
	int wi = (int)ip->cpeData;
	*value = GS_STRDUP(wanPPPConn[wi].dnsServers);
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANPPPConnection_MACAddress                     **/
CPE_STATUS setWANDeviceWANConnectionDeviceWANPPPConnection_MACAddress(Instance *ip, char *value)
{
	/* set parameter */
	int wi = (int)ip->cpeData;
	strncpy(wanPPPConn[wi].macAddr, value, sizeof(wanPPPConn[wi].macAddr));
	return CPE_OK;
}
CPE_STATUS getWANDeviceWANConnectionDeviceWANPPPConnection_MACAddress(Instance *ip, char **value)
{
	/* get parameter */
	int wi = (int)ip->cpeData;
	*value = GS_STRDUP(wanPPPConn[wi].macAddr);
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANPPPConnection_TransportType                     **/
CPE_STATUS getWANDeviceWANConnectionDeviceWANPPPConnection_TransportType(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANPPPConnection_PPPoEACName                     **/
CPE_STATUS setWANDeviceWANConnectionDeviceWANPPPConnection_PPPoEACName(Instance *ip, char *value)
{
	/* set parameter */
	return CPE_OK;
}
CPE_STATUS getWANDeviceWANConnectionDeviceWANPPPConnection_PPPoEACName(Instance *ip, char **value)
{
	/* get parameter */
	int wi = (int)ip->cpeData;
	*value = GS_STRDUP(wanPPPConn[wi].pppoeACName);
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANPPPConnection_PPPoEServiceName                     **/
CPE_STATUS setWANDeviceWANConnectionDeviceWANPPPConnection_PPPoEServiceName(Instance *ip, char *value)
{
	/* set parameter */
	int wi = (int)ip->cpeData;
	strncpy(wanPPPConn[wi].pppoeServiceName, value, sizeof(wanPPPConn[wi].pppoeServiceName));
	return CPE_OK;
}
CPE_STATUS getWANDeviceWANConnectionDeviceWANPPPConnection_PPPoEServiceName(Instance *ip, char **value)
{
	/* get parameter */
	int wi = (int)ip->cpeData;
	*value = GS_STRDUP(wanPPPConn[wi].pppoeServiceName);
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANPPPConnection_ConnectionTrigger                     **/
CPE_STATUS setWANDeviceWANConnectionDeviceWANPPPConnection_ConnectionTrigger(Instance *ip, char *value)
{
	/* set parameter */
	return CPE_OK;
}
CPE_STATUS getWANDeviceWANConnectionDeviceWANPPPConnection_ConnectionTrigger(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANPPPConnection_RouteProtocolRx                     **/
CPE_STATUS setWANDeviceWANConnectionDeviceWANPPPConnection_RouteProtocolRx(Instance *ip, char *value)
{
	/* set parameter */
	return CPE_OK;
}
CPE_STATUS getWANDeviceWANConnectionDeviceWANPPPConnection_RouteProtocolRx(Instance *ip, char **value)
{
	/* get parameter */
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
/**@param WANDeviceWANConnectionDeviceWANPPPConnection_Reset                     **/
CPE_STATUS setWANDeviceWANConnectionDeviceWANPPPConnection_Reset(Instance *ip, char *value)
{
	/* set parameter */
	return CPE_OK;
}
CPE_STATUS getWANDeviceWANConnectionDeviceWANPPPConnection_Reset(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANPPPConnection_PPPoESessionID                     **/
CPE_STATUS getWANDeviceWANConnectionDeviceWANPPPConnection_PPPoESessionID(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANConnectionDeviceWANPPPConnection_DefaultGateway                     **/
CPE_STATUS getWANDeviceWANConnectionDeviceWANPPPConnection_DefaultGateway(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@endobj WANDeviceWANConnectionDeviceWANPPPConnection **/

/**@obj WANDeviceWANConnectionDevice **/
CPE_STATUS  addWANDeviceWANConnectionDevice(CWMPObject *o, Instance *ip)
{
	/* Add new instance: This should be called each time a ATM VC is created*/
	/* or when a Ethernet WAN connection is created */
	int id;
	AvailInterface *ap = NULL;
	char name[257];
	Instance *cip;
	/*  */
	if ( cwmpGetRunState() == RUNSTATE_BOOTUP_INIT ){
		// call is from boot-up configuration restore functions.
		fprintf(stderr, "addWANDeviceWANConnectionDevice id = %d\n", ip->id);
		if (!EMPTYSTR(dfltWANIF())){
			ap = findAvailIF(dfltWANIF());
		}
	}
	/* */
	ip->cpeData = ap;			/* point instance at interface data */
	/* Add corresponding WANCommonInterfaceConfig.Connection.{i}.    */
	/*    Use id from the WANDConnectionDevice instance              */
	snprintf(name, sizeof(name),
		"InternetGatewayDevice.WANDevice.1.WANCommonInterfaceConfig.Connection.%d.",
		ip->id);
	if ( cip = cwmpInitObjectInstance(name) ){;
		cip->cpeData = (void *)ip; /* point Connection instance at ConnectionDevice.{i}.*/
	} // if NULL the .Connection. object was not included in the tables.
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

/**@obj WANDeviceWANDSLConnectionManagementConnectionService **/

/**@param WANDeviceWANDSLConnectionManagementConnectionService_WANConnectionDevice                     **/
CPE_STATUS getWANDeviceWANDSLConnectionManagementConnectionService_WANConnectionDevice(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANDSLConnectionManagementConnectionService_WANConnectionService                     **/
CPE_STATUS getWANDeviceWANDSLConnectionManagementConnectionService_WANConnectionService(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANDSLConnectionManagementConnectionService_DestinationAddress                     **/
CPE_STATUS getWANDeviceWANDSLConnectionManagementConnectionService_DestinationAddress(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANDSLConnectionManagementConnectionService_LinkType                     **/
CPE_STATUS getWANDeviceWANDSLConnectionManagementConnectionService_LinkType(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANDSLConnectionManagementConnectionService_ConnectionType                     **/
CPE_STATUS getWANDeviceWANDSLConnectionManagementConnectionService_ConnectionType(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANDSLConnectionManagementConnectionService_Name                     **/
CPE_STATUS getWANDeviceWANDSLConnectionManagementConnectionService_Name(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@endobj WANDeviceWANDSLConnectionManagementConnectionService **/

/**@obj WANDeviceWANDSLConnectionManagement **/

/**@param WANDeviceWANDSLConnectionManagement_ConnectionServiceNumberOfEntries                     **/
CPE_STATUS getWANDeviceWANDSLConnectionManagement_ConnectionServiceNumberOfEntries(Instance *ip, char **value)
{
	cwmpGetObjInstanceCntStr("ConnectionService", value);
	return CPE_OK;
}
/**@endparam                                               **/
/**@endobj WANDeviceWANDSLConnectionManagement **/

/**@obj WANDeviceWANDSLInterfaceConfigStatsShowtime **/

/**@param WANDeviceWANDSLInterfaceConfigStatsShowtime_ReceiveBlocks                     **/
CPE_STATUS getWANDeviceWANDSLInterfaceConfigStatsShowtime_ReceiveBlocks(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANDSLInterfaceConfigStatsShowtime_TransmitBlocks                     **/
CPE_STATUS getWANDeviceWANDSLInterfaceConfigStatsShowtime_TransmitBlocks(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANDSLInterfaceConfigStatsShowtime_CellDelin                     **/
CPE_STATUS getWANDeviceWANDSLInterfaceConfigStatsShowtime_CellDelin(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANDSLInterfaceConfigStatsShowtime_LinkRetrain                     **/
CPE_STATUS getWANDeviceWANDSLInterfaceConfigStatsShowtime_LinkRetrain(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANDSLInterfaceConfigStatsShowtime_InitErrors                     **/
CPE_STATUS getWANDeviceWANDSLInterfaceConfigStatsShowtime_InitErrors(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANDSLInterfaceConfigStatsShowtime_InitTimeouts                     **/
CPE_STATUS getWANDeviceWANDSLInterfaceConfigStatsShowtime_InitTimeouts(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANDSLInterfaceConfigStatsShowtime_LossOfFraming                     **/
CPE_STATUS getWANDeviceWANDSLInterfaceConfigStatsShowtime_LossOfFraming(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANDSLInterfaceConfigStatsShowtime_ErroredSecs                     **/
CPE_STATUS getWANDeviceWANDSLInterfaceConfigStatsShowtime_ErroredSecs(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANDSLInterfaceConfigStatsShowtime_SeverelyErroredSecs                     **/
CPE_STATUS getWANDeviceWANDSLInterfaceConfigStatsShowtime_SeverelyErroredSecs(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANDSLInterfaceConfigStatsShowtime_FECErrors                     **/
CPE_STATUS getWANDeviceWANDSLInterfaceConfigStatsShowtime_FECErrors(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANDSLInterfaceConfigStatsShowtime_ATUCFECErrors                     **/
CPE_STATUS getWANDeviceWANDSLInterfaceConfigStatsShowtime_ATUCFECErrors(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANDSLInterfaceConfigStatsShowtime_HECErrors                     **/
CPE_STATUS getWANDeviceWANDSLInterfaceConfigStatsShowtime_HECErrors(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANDSLInterfaceConfigStatsShowtime_ATUCHECErrors                     **/
CPE_STATUS getWANDeviceWANDSLInterfaceConfigStatsShowtime_ATUCHECErrors(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANDSLInterfaceConfigStatsShowtime_CRCErrors                     **/
CPE_STATUS getWANDeviceWANDSLInterfaceConfigStatsShowtime_CRCErrors(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANDSLInterfaceConfigStatsShowtime_ATUCCRCErrors                     **/
CPE_STATUS getWANDeviceWANDSLInterfaceConfigStatsShowtime_ATUCCRCErrors(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@endobj WANDeviceWANDSLInterfaceConfigStatsShowtime **/

/**@obj WANDeviceWANDSLInterfaceConfigStatsTotal **/

/**@param WANDeviceWANDSLInterfaceConfigStatsTotal_ReceiveBlocks                     **/
CPE_STATUS getWANDeviceWANDSLInterfaceConfigStatsTotal_ReceiveBlocks(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANDSLInterfaceConfigStatsTotal_TransmitBlocks                     **/
CPE_STATUS getWANDeviceWANDSLInterfaceConfigStatsTotal_TransmitBlocks(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANDSLInterfaceConfigStatsTotal_CellDelin                     **/
CPE_STATUS getWANDeviceWANDSLInterfaceConfigStatsTotal_CellDelin(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANDSLInterfaceConfigStatsTotal_LinkRetrain                     **/
CPE_STATUS getWANDeviceWANDSLInterfaceConfigStatsTotal_LinkRetrain(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANDSLInterfaceConfigStatsTotal_InitErrors                     **/
CPE_STATUS getWANDeviceWANDSLInterfaceConfigStatsTotal_InitErrors(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANDSLInterfaceConfigStatsTotal_InitTimeouts                     **/
CPE_STATUS getWANDeviceWANDSLInterfaceConfigStatsTotal_InitTimeouts(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANDSLInterfaceConfigStatsTotal_LossOfFraming                     **/
CPE_STATUS getWANDeviceWANDSLInterfaceConfigStatsTotal_LossOfFraming(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANDSLInterfaceConfigStatsTotal_ErroredSecs                     **/
CPE_STATUS getWANDeviceWANDSLInterfaceConfigStatsTotal_ErroredSecs(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANDSLInterfaceConfigStatsTotal_SeverelyErroredSecs                     **/
CPE_STATUS getWANDeviceWANDSLInterfaceConfigStatsTotal_SeverelyErroredSecs(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANDSLInterfaceConfigStatsTotal_FECErrors                     **/
CPE_STATUS getWANDeviceWANDSLInterfaceConfigStatsTotal_FECErrors(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANDSLInterfaceConfigStatsTotal_ATUCFECErrors                     **/
CPE_STATUS getWANDeviceWANDSLInterfaceConfigStatsTotal_ATUCFECErrors(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANDSLInterfaceConfigStatsTotal_HECErrors                     **/
CPE_STATUS getWANDeviceWANDSLInterfaceConfigStatsTotal_HECErrors(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANDSLInterfaceConfigStatsTotal_ATUCHECErrors                     **/
CPE_STATUS getWANDeviceWANDSLInterfaceConfigStatsTotal_ATUCHECErrors(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANDSLInterfaceConfigStatsTotal_CRCErrors                     **/
CPE_STATUS getWANDeviceWANDSLInterfaceConfigStatsTotal_CRCErrors(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANDSLInterfaceConfigStatsTotal_ATUCCRCErrors                     **/
CPE_STATUS getWANDeviceWANDSLInterfaceConfigStatsTotal_ATUCCRCErrors(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@endobj WANDeviceWANDSLInterfaceConfigStatsTotal **/

/**@obj WANDeviceWANDSLInterfaceConfigStats **/
/**@endobj WANDeviceWANDSLInterfaceConfigStats **/

/**@obj WANDeviceWANDSLInterfaceConfig **/
CPE_STATUS  initWANDeviceWANDSLInterfaceConfig(CWMPObject *o, Instance *ip)
{
	/* initialize object */
	return CPE_OK;
}
CPE_STATUS  commitWANDeviceWANDSLInterfaceConfig(CWMPObject *o, Instance *ip, eCommitCBType cmt)
{
	/* commit object instance */
	return CPE_OK;
}

/**@param WANDeviceWANDSLInterfaceConfig_Enable                     **/
CPE_STATUS setWANDeviceWANDSLInterfaceConfig_Enable(Instance *ip, char *value)
{
	/* set parameter */
	return CPE_OK;
}
CPE_STATUS getWANDeviceWANDSLInterfaceConfig_Enable(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANDSLInterfaceConfig_Status                     **/
CPE_STATUS getWANDeviceWANDSLInterfaceConfig_Status(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANDSLInterfaceConfig_UpstreamCurrRate                     **/
CPE_STATUS getWANDeviceWANDSLInterfaceConfig_UpstreamCurrRate(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANDSLInterfaceConfig_DownstreamCurrRate                     **/
CPE_STATUS getWANDeviceWANDSLInterfaceConfig_DownstreamCurrRate(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANDSLInterfaceConfig_UpstreamMaxRate                     **/
CPE_STATUS getWANDeviceWANDSLInterfaceConfig_UpstreamMaxRate(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANDSLInterfaceConfig_DownstreamMaxRate                     **/
CPE_STATUS getWANDeviceWANDSLInterfaceConfig_DownstreamMaxRate(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANDSLInterfaceConfig_UpstreamNoiseMargin                     **/
CPE_STATUS getWANDeviceWANDSLInterfaceConfig_UpstreamNoiseMargin(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANDSLInterfaceConfig_DownstreamNoiseMargin                     **/
CPE_STATUS getWANDeviceWANDSLInterfaceConfig_DownstreamNoiseMargin(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANDSLInterfaceConfig_UpstreamAttenuation                     **/
CPE_STATUS getWANDeviceWANDSLInterfaceConfig_UpstreamAttenuation(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANDSLInterfaceConfig_DownstreamAttenuation                     **/
CPE_STATUS getWANDeviceWANDSLInterfaceConfig_DownstreamAttenuation(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANDSLInterfaceConfig_UpstreamPower                     **/
CPE_STATUS getWANDeviceWANDSLInterfaceConfig_UpstreamPower(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANDSLInterfaceConfig_DownstreamPower                     **/
CPE_STATUS getWANDeviceWANDSLInterfaceConfig_DownstreamPower(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANDSLInterfaceConfig_ATURVendor                     **/
CPE_STATUS getWANDeviceWANDSLInterfaceConfig_ATURVendor(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANDSLInterfaceConfig_ATURCountry                     **/
CPE_STATUS getWANDeviceWANDSLInterfaceConfig_ATURCountry(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANDSLInterfaceConfig_ATUCVendor                     **/
CPE_STATUS getWANDeviceWANDSLInterfaceConfig_ATUCVendor(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANDSLInterfaceConfig_ATUCCountry                     **/
CPE_STATUS getWANDeviceWANDSLInterfaceConfig_ATUCCountry(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANDSLInterfaceConfig_TotalStart                     **/
CPE_STATUS getWANDeviceWANDSLInterfaceConfig_TotalStart(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANDSLInterfaceConfig_ShowtimeStart                     **/
CPE_STATUS getWANDeviceWANDSLInterfaceConfig_ShowtimeStart(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@endobj WANDeviceWANDSLInterfaceConfig **/

/**@obj WANDeviceWANEthernetInterfaceConfigStats **/

/**@param WANDeviceWANEthernetInterfaceConfigStats_BytesSent                     **/
CPE_STATUS getWANDeviceWANEthernetInterfaceConfigStats_BytesSent(Instance *ip, char **value)
{
	/* get parameter */
	 *value = GS_STRDUP(wanIF->txBytes);
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANEthernetInterfaceConfigStats_BytesReceived                     **/
CPE_STATUS getWANDeviceWANEthernetInterfaceConfigStats_BytesReceived(Instance *ip, char **value)
{
	/* get parameter */
	 *value = GS_STRDUP(wanIF->rxBytes);
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANEthernetInterfaceConfigStats_PacketsSent                     **/
CPE_STATUS getWANDeviceWANEthernetInterfaceConfigStats_PacketsSent(Instance *ip, char **value)
{
	/* get parameter */
	 *value = GS_STRDUP(wanIF->txPackets);
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANEthernetInterfaceConfigStats_PacketsReceived                     **/
CPE_STATUS getWANDeviceWANEthernetInterfaceConfigStats_PacketsReceived(Instance *ip, char **value)
{
	/* get parameter */
	 *value = GS_STRDUP(wanIF->rxPackets);
	return CPE_OK;
}
/**@endparam                                               **/
/**@endobj WANDeviceWANEthernetInterfaceConfigStats **/

/**@obj WANDeviceWANEthernetInterfaceConfig **/
static WANEthernetInterfaceConfig wanEthIFCnf;
CPE_STATUS initWANDeviceWANEthernetInterfaceConfig(CWMPObject *o, Instance *ip){
	AvailInterface *ap = findAvailIF(dfltWANIF());
	if (ap){ /* initialize from the availIF table. */
		wanEthIFCnf.flag = ENTRY_ENABLE|ENTRY_ACTIVE;
		strCpyLimited( wanEthIFCnf.macAddr, ap->mac, sizeof(wanEthIFCnf.macAddr));
		strCpyLimited( wanEthIFCnf.maxBitRate, "Auto", sizeof(wanEthIFCnf.maxBitRate));
		strCpyLimited( wanEthIFCnf.duplexMode, "Auto", sizeof(wanEthIFCnf.duplexMode));
	}
	ip->cpeData = (void *)ap;   /* may be NULL */
	return CPE_OK;
}

CPE_STATUS  commitWANDeviceWANEthernetInterfaceConfig(CWMPObject *o, Instance *ip, eCommitCBType cmt)
{	//TODO: ifconfig xxx up/down here
	fprintf(stderr, "commitWANEthernetInterfaceConfig\n");
	return CPE_OK;
}
/**@param WANDeviceWANEthernetInterfaceConfig_Enable                     **/
CPE_STATUS setWANDeviceWANEthernetInterfaceConfig_Enable(Instance *ip, char *value)
{
	/* Set parameter */
	if ( testBoolean(value))
		wanEthIFCnf.flag |= ENTRY_ENABLE;
	else
		wanEthIFCnf.flag &= ~ENTRY_ENABLE;
	return CPE_OK;
}
CPE_STATUS getWANDeviceWANEthernetInterfaceConfig_Enable(Instance *ip, char **value)
{
	/* get parameter */
	*value = GS_STRDUP(wanEthIFCnf.flag&ENTRY_ENABLE? "1": "0");
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANEthernetInterfaceConfig_Status                     **/
CPE_STATUS getWANDeviceWANEthernetInterfaceConfig_Status(Instance *ip, char **value)
{
	/* get parameter */
	// TODO: real men would use the ethctl to find link status, etc.
	AvailInterface *ap = (AvailInterface *)ip->cpeData;
	if ( !(wanEthIFCnf.flag&ENTRY_ENABLE) )
		*value = GS_STRDUP("Disabled");
	else
		*value = GS_STRDUP(ap && ap->ifStatus==eUP? "Up": "NoLink");
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANEthernetInterfaceConfig_MACAddress                     **/
CPE_STATUS getWANDeviceWANEthernetInterfaceConfig_MACAddress(Instance *ip, char **value)
{
	/* get parameter */
	*value = GS_STRDUP(writeCanonicalMacUCase(wanEthIFCnf.macAddr));
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANEthernetInterfaceConfig_MaxBitRate                     **/
CPE_STATUS setWANDeviceWANEthernetInterfaceConfig_MaxBitRate(Instance *ip, char *value)
{
	/* set parameter */
	strCpyLimited(wanEthIFCnf.maxBitRate, value, sizeof(wanEthIFCnf.maxBitRate));
	return CPE_OK;
}
CPE_STATUS getWANDeviceWANEthernetInterfaceConfig_MaxBitRate(Instance *ip, char **value)
{
	/* get parameter */
	*value = GS_STRDUP(wanEthIFCnf.maxBitRate);
	return CPE_OK;
}
/**@endparam                                               **/
/**@param WANDeviceWANEthernetInterfaceConfig_DuplexMode                     **/
CPE_STATUS setWANDeviceWANEthernetInterfaceConfig_DuplexMode(Instance *ip, char *value)
{
	/* set parameter */
	strCpyLimited(wanEthIFCnf.duplexMode, value, sizeof(wanEthIFCnf.duplexMode));
	return CPE_OK;
}
CPE_STATUS getWANDeviceWANEthernetInterfaceConfig_DuplexMode(Instance *ip, char **value)
{
	/* get parameter */
	 *value = GS_STRDUP(wanEthIFCnf.duplexMode);
	return CPE_OK;
}
/**@endparam                                               **/
/**@endobj WANDeviceWANEthernetInterfaceConfig **/

/**@obj WANDevice **/

/**@param WANDevice_WANConnectionNumberOfEntries                     **/
CPE_STATUS getWANDevice_WANConnectionNumberOfEntries(Instance *ip, char **value)
{
	cwmpGetObjInstanceCntStr("WANConnectionDevice", value);
	return CPE_OK;
}
/**@endparam                                               **/
/**@endobj WANDevice **/






