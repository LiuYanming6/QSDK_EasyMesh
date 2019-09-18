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
 * $Id: LANDevice.c,v 1.1 2012/05/10 17:37:59 dmounday Exp $
 *----------------------------------------------------------------------*/

#include "paramTree.h"
#include "rpc.h"
#include "gslib/src/utils.h"
#include "soapRpc/rpcUtils.h"
#include "soapRpc/rpcMethods.h"
#include "soapRpc/cwmpSession.h"


/**@obj LANDeviceHostsHost **/
#include "includes/cpedata.h"
extern HostLease hostLeases[256];

/**@param LANDeviceHostsHost_IPAddress                     **/
CPE_STATUS getLANDeviceHostsHost_IPAddress(Instance *ip, char **value)
{
	/* get parameter */
	*value = GS_STRDUP(inet_ntoa(*(struct in_addr*)(&hostLeases[(int)(ip->cpeData)].ip)));
	return CPE_OK;
}
/**@endparam                                               **/
/**@param LANDeviceHostsHost_AddressSource                     **/
CPE_STATUS getLANDeviceHostsHost_AddressSource(Instance *ip, char **value)
{
	/* get parameter */
	 *value = GS_STRDUP("DHCP");
	return CPE_OK;
}
/**@endparam                                               **/
/**@param LANDeviceHostsHost_LeaseTimeRemaining                     **/
CPE_STATUS getLANDeviceHostsHost_LeaseTimeRemaining(Instance *ip, char **value)
{
	/* get parameter */
	char buf[20];
	snprintf(buf, sizeof(buf), "%u",hostLeases[(int)ip->cpeData].expires);
	*value = GS_STRDUP(buf);
	return CPE_OK;
}
/**@endparam                                               **/
/**@param LANDeviceHostsHost_MACAddress                     **/
CPE_STATUS getLANDeviceHostsHost_MACAddress(Instance *ip, char **value)
{
	/* get parameter */
	char *p = hostLeases[(int)ip->cpeData].mac;
	 *value = GS_STRDUP(writeCanonicalMacUCase(p));
	return CPE_OK;
}
/**@endparam                                               **/
/**@param LANDeviceHostsHost_HostName                     **/
CPE_STATUS getLANDeviceHostsHost_HostName(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param LANDeviceHostsHost_InterfaceType                     **/
CPE_STATUS getLANDeviceHostsHost_InterfaceType(Instance *ip, char **value)
{
	/* get parameter */
	//TODO: Could look at wireless associations to get some of this.
	 *value = GS_STRDUP("Ethernet");
	return CPE_OK;
}
/**@endparam                                               **/
/**@param LANDeviceHostsHost_Active                     **/
CPE_STATUS getLANDeviceHostsHost_Active(Instance *ip, char **value)
{
	/* get parameter */
	// TODO: read ARP table and update based on MACs.
	 *value = GS_STRDUP("1");
	return CPE_OK;
}
/**@endparam                                               **/
/**@param LANDeviceHostsHost_Layer2Interface                     **/
CPE_STATUS getLANDeviceHostsHost_Layer2Interface(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@endobj LANDeviceHostsHost **/

/**@obj LANDeviceHosts **/

/**@param LANDeviceHosts_HostNumberOfEntries                     **/
CPE_STATUS getLANDeviceHosts_HostNumberOfEntries(Instance *ip, char **value)
{
	cwmpGetObjInstanceCntStr("Host", value);
	return CPE_OK;
}
/**@endparam                                               **/
/**@endobj LANDeviceHosts **/

/**@obj LANDeviceLANEthernetInterfaceConfigStats **/

extern AvailInterface availIF[MAXIFS];
/*
 * This is a pseudo-add function that is used to initialize the instance descriptor
 * of the static child object of the LANEthernetInterfaceConfig object or any other
 * data that may need to be initialized for the child object. This function
 * is called if it is defined (pAddObj) in the Stats object entry and the otype is set to
 * eObject. The instance has already been allocated and chained to the CWMPObject
 * structure pointed to by *o. Its instance id is set to 0 as required.
 */
CPE_STATUS addLANDeviceLANEthernetInterfaceConfigStats(CWMPObject *o, Instance *ip){
	/* The index of the availIF entry is in the ip->parent->cpeData component.
	 * This will copy it to the ip->cpeData component to avoid the extra addressing
	 * step in the Stats parameter getter function.
	 * To illustrate addressing the stats data thru both the Stats object instance and
	 * the parent instance are used below.
	 */
	ip->cpeData = ip->parent->cpeData;
	return CPE_OK;
}
/**@param LANDeviceLANEthernetInterfaceConfigStats_BytesSent                     **/
CPE_STATUS getLANDeviceLANEthernetInterfaceConfigStats_BytesSent(Instance *ip, char **value)
{
	/* get parameter */
	int ix = (int) ip->parent->cpeData;
	*value = GS_STRDUP(availIF[ix].txBytes);
	return CPE_OK;
}
/**@endparam                                               **/
/**@param LANDeviceLANEthernetInterfaceConfigStats_BytesReceived                     **/
CPE_STATUS getLANDeviceLANEthernetInterfaceConfigStats_BytesReceived(Instance *ip, char **value)
{
	/* get parameter */
	int ix = (int) ip->cpeData;
	 *value = GS_STRDUP(availIF[ix].rxBytes);
	return CPE_OK;
}
/**@endparam                                               **/
/**@param LANDeviceLANEthernetInterfaceConfigStats_PacketsSent                     **/
CPE_STATUS getLANDeviceLANEthernetInterfaceConfigStats_PacketsSent(Instance *ip, char **value)
{
	/* get parameter */
	int ix = (int) ip->parent->cpeData;
	*value = GS_STRDUP(availIF[ix].txPackets);
	return CPE_OK;
}
/**@endparam                                               **/
/**@param LANDeviceLANEthernetInterfaceConfigStats_PacketsReceived                     **/
CPE_STATUS getLANDeviceLANEthernetInterfaceConfigStats_PacketsReceived(Instance *ip, char **value)
{
	/* get parameter */
	int ix = (int) ip->cpeData;
	*value = GS_STRDUP(availIF[ix].rxPackets);
	return CPE_OK;
}
/**@endparam                                               **/
/**@param LANDeviceLANEthernetInterfaceConfigStats_ErrorsSent                     **/
CPE_STATUS getLANDeviceLANEthernetInterfaceConfigStats_ErrorsSent(Instance *ip, char **value)
{
	/* get parameter */
	int ix = (int) ip->parent->cpeData;
	*value = GS_STRDUP(availIF[ix].txErrors);
	return CPE_OK;
}
/**@endparam                                               **/
/**@param LANDeviceLANEthernetInterfaceConfigStats_ErrorsReceived                     **/
CPE_STATUS getLANDeviceLANEthernetInterfaceConfigStats_ErrorsReceived(Instance *ip, char **value)
{
	/* get parameter */
	int ix = (int) ip->parent->cpeData;
	*value = GS_STRDUP(availIF[ix].rxErrors);
	return CPE_OK;
}
/**@endparam                                               **/
/**@param LANDeviceLANEthernetInterfaceConfigStats_UnicastPacketsSent                     **/
CPE_STATUS getLANDeviceLANEthernetInterfaceConfigStats_UnicastPacketsSent(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param LANDeviceLANEthernetInterfaceConfigStats_UnicastPacketsReceived                     **/
CPE_STATUS getLANDeviceLANEthernetInterfaceConfigStats_UnicastPacketsReceived(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param LANDeviceLANEthernetInterfaceConfigStats_DiscardPacketsSent                     **/
CPE_STATUS getLANDeviceLANEthernetInterfaceConfigStats_DiscardPacketsSent(Instance *ip, char **value)
{
	/* get parameter */
	int ix = (int) ip->parent->cpeData;
	*value = GS_STRDUP(availIF[ix].txDropped);
	return CPE_OK;
}
/**@endparam                                               **/
/**@param LANDeviceLANEthernetInterfaceConfigStats_DiscardPacketsReceived                     **/
CPE_STATUS getLANDeviceLANEthernetInterfaceConfigStats_DiscardPacketsReceived(Instance *ip, char **value)
{
	/* get parameter */
	int ix = (int) ip->parent->cpeData;
	*value = GS_STRDUP(availIF[ix].rxDropped);
	return CPE_OK;
}
/**@endparam                                               **/
/**@param LANDeviceLANEthernetInterfaceConfigStats_MulticastPacketsSent                     **/
CPE_STATUS getLANDeviceLANEthernetInterfaceConfigStats_MulticastPacketsSent(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param LANDeviceLANEthernetInterfaceConfigStats_MulticastPacketsReceived                     **/
CPE_STATUS getLANDeviceLANEthernetInterfaceConfigStats_MulticastPacketsReceived(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param LANDeviceLANEthernetInterfaceConfigStats_BroadcastPacketsSent                     **/
CPE_STATUS getLANDeviceLANEthernetInterfaceConfigStats_BroadcastPacketsSent(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param LANDeviceLANEthernetInterfaceConfigStats_BroadcastPacketsReceived                     **/
CPE_STATUS getLANDeviceLANEthernetInterfaceConfigStats_BroadcastPacketsReceived(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param LANDeviceLANEthernetInterfaceConfigStats_UnknownProtoPacketsReceived                     **/
CPE_STATUS getLANDeviceLANEthernetInterfaceConfigStats_UnknownProtoPacketsReceived(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@endobj LANDeviceLANEthernetInterfaceConfigStats **/

/**@obj LANDeviceLANEthernetInterfaceConfig **/
/*
 * This add is used when the configuration is restored by the cpeRestoreObjectConfig
 * function. For the LANEthernetInterfaceConfig the instance number is the
 * index into the availIF table that contains the backing data for the
 * instance. The cpeData component is set to the instance id an used by the
 * getter and setter to access the availIF array entry for the interface.
 * The LANEthernetInterfaceConfig object otype must be set to eStaticInstance and
 * the pAddObj set to this function.
 */
CPE_STATUS addLANDeviceLANEthernetInterfaceConfig(CWMPObject *o, Instance *ip){

	ip->cpeData = (void*)(ip->id -1);  /* index to the avaiIF entry */
	return CPE_OK;
}
CPE_STATUS  delLANDeviceLANEthernetInterfaceConfig(CWMPObject *o, Instance *ip)
{
	/* delete instance data */
	return CPE_OK;
}

/**@param LANDeviceLANEthernetInterfaceConfig_Enable                     **/
CPE_STATUS setLANDeviceLANEthernetInterfaceConfig_Enable(Instance *ip, char *value)
{
	/* Set parameter */
	int ix = (int) ip->cpeData;
	availIF[ix].flag &= ~ENTRY_ENABLE;
	return CPE_OK;
}
CPE_STATUS getLANDeviceLANEthernetInterfaceConfig_Enable(Instance *ip, char **value)
{
	/* get parameter */
	int ix = (int) ip->cpeData;
	*value = GS_STRDUP(availIF[ix].flag&ENTRY_ENABLE? "1": "0");
	return CPE_OK;
}
/**@endparam                                               **/
/**@param LANDeviceLANEthernetInterfaceConfig_Status                     **/
CPE_STATUS getLANDeviceLANEthernetInterfaceConfig_Status(Instance *ip, char **value)
{
	/* get parameter */
	int ix = (int) ip->cpeData;
	if ( availIF[ix].ifStatus == eUP)
		*value = GS_STRDUP("Up");
	else
		*value = GS_STRDUP("NoLink");
	return CPE_OK;
}
/**@endparam                                               **/
/**@param LANDeviceLANEthernetInterfaceConfig_MACAddress                     **/
CPE_STATUS getLANDeviceLANEthernetInterfaceConfig_MACAddress(Instance *ip, char **value)
{
	/* get parameter */
	int ix = (int) ip->cpeData;
	*value = GS_STRDUP(availIF[ix].mac);
	return CPE_OK;
}
/**@endparam                                               **/
/**@param LANDeviceLANEthernetInterfaceConfig_MACAddressControlEnabled                     **/
CPE_STATUS setLANDeviceLANEthernetInterfaceConfig_MACAddressControlEnabled(Instance *ip, char *value)
{
	/* Set parameter */
	int ix = (int) ip->cpeData;
	availIF[ix].macAddCtrlEnabled = testBoolean(value);
	return CPE_OK;
}
CPE_STATUS getLANDeviceLANEthernetInterfaceConfig_MACAddressControlEnabled(Instance *ip, char **value)
{
	/* get parameter */
	int ix = (int) ip->cpeData;
	*value = GS_STRDUP(availIF[ix].macAddCtrlEnabled?"1":"0");
	return CPE_OK;
}
/**@endparam                                               **/
/**@param LANDeviceLANEthernetInterfaceConfig_MaxBitRate                     **/
CPE_STATUS setLANDeviceLANEthernetInterfaceConfig_MaxBitRate(Instance *ip, char *value)
{
	/* set parameter */
	return CPE_OK;
}
CPE_STATUS getLANDeviceLANEthernetInterfaceConfig_MaxBitRate(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param LANDeviceLANEthernetInterfaceConfig_DuplexMode                     **/
CPE_STATUS setLANDeviceLANEthernetInterfaceConfig_DuplexMode(Instance *ip, char *value)
{
	/* set parameter */
	return CPE_OK;
}
CPE_STATUS getLANDeviceLANEthernetInterfaceConfig_DuplexMode(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param LANDeviceLANEthernetInterfaceConfig_Name                     **/
CPE_STATUS getLANDeviceLANEthernetInterfaceConfig_Name(Instance *ip, char **value)
{
	/* get parameter */
	int ix = (int) ip->cpeData;
	*value = GS_STRDUP(availIF[ix].ifname);
	return CPE_OK;
}
/**@endparam                                               **/
/**@endobj LANDeviceLANEthernetInterfaceConfig **/

/**@obj LANDeviceLANHostConfigManagementDHCPConditionalServingPoolDHCPOption **/
CPE_STATUS  addLANDeviceLANHostConfigManagementDHCPConditionalServingPoolDHCPOption(CWMPObject *o, Instance *ip)
{
	/* add instance data */
	return CPE_OK;
}
CPE_STATUS  delLANDeviceLANHostConfigManagementDHCPConditionalServingPoolDHCPOption(CWMPObject *o, Instance *ip)
{
	/* delete instance data */
	return CPE_OK;
}

/**@param LANDeviceLANHostConfigManagementDHCPConditionalServingPoolDHCPOption_Enable                     **/
CPE_STATUS setLANDeviceLANHostConfigManagementDHCPConditionalServingPoolDHCPOption_Enable(Instance *ip, char *value)
{
	/* set parameter */
	return CPE_OK;
}
CPE_STATUS getLANDeviceLANHostConfigManagementDHCPConditionalServingPoolDHCPOption_Enable(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param LANDeviceLANHostConfigManagementDHCPConditionalServingPoolDHCPOption_Tag                     **/
CPE_STATUS setLANDeviceLANHostConfigManagementDHCPConditionalServingPoolDHCPOption_Tag(Instance *ip, char *value)
{
	/* set parameter */
	return CPE_OK;
}
CPE_STATUS getLANDeviceLANHostConfigManagementDHCPConditionalServingPoolDHCPOption_Tag(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param LANDeviceLANHostConfigManagementDHCPConditionalServingPoolDHCPOption_Value                     **/
CPE_STATUS setLANDeviceLANHostConfigManagementDHCPConditionalServingPoolDHCPOption_Value(Instance *ip, char *value)
{
	/* set parameter */
	return CPE_OK;
}
CPE_STATUS getLANDeviceLANHostConfigManagementDHCPConditionalServingPoolDHCPOption_Value(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@endobj LANDeviceLANHostConfigManagementDHCPConditionalServingPoolDHCPOption **/

/**@obj LANDeviceLANHostConfigManagementDHCPConditionalServingPool **/
CPE_STATUS  addLANDeviceLANHostConfigManagementDHCPConditionalServingPool(CWMPObject *o, Instance *ip)
{
	/* add instance data */
	return CPE_OK;
}
CPE_STATUS  delLANDeviceLANHostConfigManagementDHCPConditionalServingPool(CWMPObject *o, Instance *ip)
{
	/* delete instance data */
	return CPE_OK;
}

/**@param LANDeviceLANHostConfigManagementDHCPConditionalServingPool_DHCPOptionNumberOfEntries                     **/
CPE_STATUS getLANDeviceLANHostConfigManagementDHCPConditionalServingPool_DHCPOptionNumberOfEntries(Instance *ip, char **value)
{
	cwmpGetObjInstanceCntStr("DHCPOption", value);
	return CPE_OK;
}
/**@endparam                                               **/
/**@endobj LANDeviceLANHostConfigManagementDHCPConditionalServingPool **/

/**@obj LANDeviceLANHostConfigManagementDHCPOption **/
CPE_STATUS  addLANDeviceLANHostConfigManagementDHCPOption(CWMPObject *o, Instance *ip)
{
	/* add instance data */
	return CPE_OK;
}
CPE_STATUS  delLANDeviceLANHostConfigManagementDHCPOption(CWMPObject *o, Instance *ip)
{
	/* delete instance data */
	return CPE_OK;
}

/**@param LANDeviceLANHostConfigManagementDHCPOption_Enable                     **/
CPE_STATUS setLANDeviceLANHostConfigManagementDHCPOption_Enable(Instance *ip, char *value)
{
	/* set parameter */
	return CPE_OK;
}
CPE_STATUS getLANDeviceLANHostConfigManagementDHCPOption_Enable(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param LANDeviceLANHostConfigManagementDHCPOption_Tag                     **/
CPE_STATUS setLANDeviceLANHostConfigManagementDHCPOption_Tag(Instance *ip, char *value)
{
	/* set parameter */
	return CPE_OK;
}
CPE_STATUS getLANDeviceLANHostConfigManagementDHCPOption_Tag(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param LANDeviceLANHostConfigManagementDHCPOption_Value                     **/
CPE_STATUS setLANDeviceLANHostConfigManagementDHCPOption_Value(Instance *ip, char *value)
{
	/* set parameter */
	return CPE_OK;
}
CPE_STATUS getLANDeviceLANHostConfigManagementDHCPOption_Value(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@endobj LANDeviceLANHostConfigManagementDHCPOption **/

/**@obj LANDeviceLANHostConfigManagementIPInterface **/
extern DhcpDesc dhcpD;
CPE_STATUS  addLANDeviceLANHostConfigManagementIPInterface(CWMPObject *o, Instance *ip)
{
	/* add instance data */
	return CPE_OK;
}
CPE_STATUS  delLANDeviceLANHostConfigManagementIPInterface(CWMPObject *o, Instance *ip)
{
	/* delete instance data */
	return CPE_OK;
}
CPE_STATUS commitLANDeviceLANHostConfigManagement(CWMPObject *o, Instance *ip, eCommitCBType cmt){
	if ( dhcpD.flag&ENTRY_ENABLEREQ ){  /* enable request */
		dhcpD.flag &= ~ENTRY_ENABLEREQ; /* turn off req */
		if (! (dhcpD.flag&ENTRY_ENABLED) ) {
			setDhcpDesc(1);
			dhcpD.flag |= ENTRY_ENABLED;    /* Set enabled */
		}
	} else if ( dhcpD.flag&ENTRY_DISABLEREQ ) { /* DISABLE request*/
		dhcpD.flag &= ~ENTRY_DISABLEREQ;  /* turn off req */
		if ( dhcpD.flag&ENTRY_ENABLED ){
			setDhcpDesc(0);
			dhcpD.flag &= ~ENTRY_ENABLED; /* turn off*/
		}
	}
	return CPE_OK;
}
/**@param LANDeviceLANHostConfigManagementIPInterface_Enable                     **/
CPE_STATUS setLANDeviceLANHostConfigManagementIPInterface_Enable(Instance *ip, char *value)
{
	/* set parameter */
	return CPE_OK;
}
CPE_STATUS getLANDeviceLANHostConfigManagementIPInterface_Enable(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param LANDeviceLANHostConfigManagementIPInterface_IPInterfaceIPAddress                     **/
CPE_STATUS setLANDeviceLANHostConfigManagementIPInterface_IPInterfaceIPAddress(Instance *ip, char *value)
{
	/* set parameter */
	return CPE_OK;
}
CPE_STATUS getLANDeviceLANHostConfigManagementIPInterface_IPInterfaceIPAddress(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param LANDeviceLANHostConfigManagementIPInterface_IPInterfaceSubnetMask                     **/
CPE_STATUS setLANDeviceLANHostConfigManagementIPInterface_IPInterfaceSubnetMask(Instance *ip, char *value)
{
	/* Set parameter */
	int ix = (int)ip->cpeData;
	strncpy( availIF[ix].mask, value, sizeof(availIF[ix].mask));
	setLanIF(ix);
	return CPE_OK;
}
CPE_STATUS getLANDeviceLANHostConfigManagementIPInterface_IPInterfaceSubnetMask(Instance *ip, char **value)
{
	/* get parameter */
	int ix = (int)ip->cpeData;
	 *value = GS_STRDUP(availIF[ix].mask);
	return CPE_OK;
}
/**@endparam                                               **/
/**@param LANDeviceLANHostConfigManagementIPInterface_IPInterfaceAddressingType                     **/
CPE_STATUS setLANDeviceLANHostConfigManagementIPInterface_IPInterfaceAddressingType(Instance *ip, char *value)
{
	/* Set parameter */
	int ix = (int)ip->cpeData;
	if ( streq(value, "DHCP")){
		availIF[ix].addrType = eDHCP;
		//TODO: Start the dhcpc if this is set.
	} else if (streq(value, "Static"))
		availIF[ix].addrType = eStatic;
	else
		availIF[ix].addrType = eAutoIP;
	setLanIF(ix);
	return CPE_OK;
}
CPE_STATUS getLANDeviceLANHostConfigManagementIPInterface_IPInterfaceAddressingType(Instance *ip, char **value)
{
	/* get parameter */
	int ix = (int)ip->cpeData;
	if ( availIF[ix].addrType == eDHCP )
		*value = GS_STRDUP("DHCP");
	else if (availIF[ix].addrType == eStatic)
		*value = GS_STRDUP("Static");
	else
		*value = GS_STRDUP("AutoIP");
	return CPE_OK;
}
/**@endparam                                               **/
/**@endobj LANDeviceLANHostConfigManagementIPInterface **/

/**@obj LANDeviceLANHostConfigManagement **/

/**@param LANDeviceLANHostConfigManagement_DHCPServerConfigurable                     **/
CPE_STATUS setLANDeviceLANHostConfigManagement_DHCPServerConfigurable(Instance *ip, char *value)
{
	/* Set parameter */
	dhcpD.configurable = testBoolean(value);
	// TODO:Setting this to true should return the CPE to its default DHCP server settings
	return CPE_OK;
}
CPE_STATUS getLANDeviceLANHostConfigManagement_DHCPServerConfigurable(Instance *ip, char **value)
{
	/* get parameter */
	*value = GS_STRDUP(dhcpD.configurable? "1": "0");
	return CPE_OK;
}
/**@endparam                                               **/
/**@param LANDeviceLANHostConfigManagement_DHCPServerEnable                     **/
CPE_STATUS setLANDeviceLANHostConfigManagement_DHCPServerEnable(Instance *ip, char *value)
{
	/* Set parameter */
	if ( testBoolean(value))
		dhcpD.flag |= ENTRY_ENABLEREQ;
	else
		dhcpD.flag |= ENTRY_DISABLEREQ;
	return CPE_OK;
}
CPE_STATUS getLANDeviceLANHostConfigManagement_DHCPServerEnable(Instance *ip, char **value)
{
	/* get parameter */
	*value = GS_STRDUP(dhcpD.flag&ENTRY_ENABLE? "1": "0");
	return CPE_OK;
}
/**@endparam                                               **/
/**@param LANDeviceLANHostConfigManagement_DHCPRelay                     **/
CPE_STATUS getLANDeviceLANHostConfigManagement_DHCPRelay(Instance *ip, char **value)
{
	/* get parameter */
	 *value = GS_STRDUP("0");
	return CPE_OK;
}
/**@endparam                                               **/
/**@param LANDeviceLANHostConfigManagement_MinAddress                     **/
CPE_STATUS setLANDeviceLANHostConfigManagement_MinAddress(Instance *ip, char *value)
{
	/* Set parameter */
	strncpy(dhcpD.minIP, value, sizeof(dhcpD.minIP));
	return CPE_OK;
}
CPE_STATUS getLANDeviceLANHostConfigManagement_MinAddress(Instance *ip, char **value)
{
	/* get parameter */
	*value = GS_STRDUP(dhcpD.minIP);
	return CPE_OK;
}
/**@endparam                                               **/
/**@param LANDeviceLANHostConfigManagement_MaxAddress                     **/
CPE_STATUS setLANDeviceLANHostConfigManagement_MaxAddress(Instance *ip, char *value)
{
	/* Set parameter */
	strncpy(dhcpD.maxIP, value, sizeof(dhcpD.maxIP));
	return CPE_OK;
}
CPE_STATUS getLANDeviceLANHostConfigManagement_MaxAddress(Instance *ip, char **value)
{
	/* get parameter */
	 *value = GS_STRDUP(dhcpD.maxIP);
	return CPE_OK;
}
/**@endparam                                               **/
/**@param LANDeviceLANHostConfigManagement_ReservedAddresses                     **/
CPE_STATUS setLANDeviceLANHostConfigManagement_ReservedAddresses(Instance *ip, char *value)
{
	/* set parameter */
	return CPE_OK;
}
CPE_STATUS getLANDeviceLANHostConfigManagement_ReservedAddresses(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param LANDeviceLANHostConfigManagement_SubnetMask                     **/
CPE_STATUS setLANDeviceLANHostConfigManagement_SubnetMask(Instance *ip, char *value)
{
	/* Set parameter */
	strncpy(dhcpD.subnetMask, value, sizeof(dhcpD.subnetMask));
	return CPE_OK;
}
CPE_STATUS getLANDeviceLANHostConfigManagement_SubnetMask(Instance *ip, char **value)
{
	/* get parameter */
	 *value = GS_STRDUP(dhcpD.subnetMask);
	return CPE_OK;
}
/**@endparam                                               **/
/**@param LANDeviceLANHostConfigManagement_DNSServers                     **/
CPE_STATUS setLANDeviceLANHostConfigManagement_DNSServers(Instance *ip, char *value)
{
	/* Set parameter */
	strncpy(dhcpD.dnsServers, value, sizeof(dhcpD.dnsServers));
	return CPE_OK;
}
CPE_STATUS getLANDeviceLANHostConfigManagement_DNSServers(Instance *ip, char **value)
{
	/* get parameter */
	*value = GS_STRDUP(dhcpD.dnsServers);
	return CPE_OK;
}
/**@endparam                                               **/
/**@param LANDeviceLANHostConfigManagement_DomainName                     **/
CPE_STATUS setLANDeviceLANHostConfigManagement_DomainName(Instance *ip, char *value)
{
	/* Set parameter */
	strncpy(dhcpD.domainName, value, sizeof(dhcpD.domainName));
	return CPE_OK;
}
CPE_STATUS getLANDeviceLANHostConfigManagement_DomainName(Instance *ip, char **value)
{
	/* get parameter */
	 *value = GS_STRDUP(dhcpD.domainName);
	return CPE_OK;
}
/**@endparam                                               **/
/**@param LANDeviceLANHostConfigManagement_IPRouters                     **/
CPE_STATUS setLANDeviceLANHostConfigManagement_IPRouters(Instance *ip, char *value)
{
	/* Set parameter */
	strncpy(dhcpD.ipRouters, value, sizeof(dhcpD.ipRouters));
	return CPE_OK;
}
CPE_STATUS getLANDeviceLANHostConfigManagement_IPRouters(Instance *ip, char **value)
{
	/* get parameter */
	*value = GS_STRDUP(dhcpD.ipRouters);
	return CPE_OK;
}
/**@endparam                                               **/
/**@param LANDeviceLANHostConfigManagement_IPInterfaceNumberOfEntries                     **/
CPE_STATUS getLANDeviceLANHostConfigManagement_IPInterfaceNumberOfEntries(Instance *ip, char **value)
{
	cwmpGetObjInstanceCntStr("IPInterface", value);
	return CPE_OK;
}
/**@endparam                                               **/
/**@param LANDeviceLANHostConfigManagement_MACAddress                     **/
CPE_STATUS getLANDeviceLANHostConfigManagement_MACAddress(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param LANDeviceLANHostConfigManagement_DHCPOptionNumberOfEntries                     **/
CPE_STATUS getLANDeviceLANHostConfigManagement_DHCPOptionNumberOfEntries(Instance *ip, char **value)
{
	cwmpGetObjInstanceCntStr("DHCPOption", value);
	return CPE_OK;
}
/**@endparam                                               **/
/**@endobj LANDeviceLANHostConfigManagement **/

/**@obj LANDeviceWLANConfigurationAssociatedDevice **/

/**@param LANDeviceWLANConfigurationAssociatedDevice_AssociatedDeviceMACAddress                     **/
CPE_STATUS getLANDeviceWLANConfigurationAssociatedDevice_AssociatedDeviceMACAddress(Instance *ip, char **value)
{
	/* get parameter */
	AssociatedDev *ap = (AssociatedDev*)ip->cpeData;
	 *value = GS_STRDUP(ap->devMACAddress);
	return CPE_OK;
}
/**@endparam                                               **/
/**@param LANDeviceWLANConfigurationAssociatedDevice_AssociatedDeviceIPAddress                     **/
CPE_STATUS getLANDeviceWLANConfigurationAssociatedDevice_AssociatedDeviceIPAddress(Instance *ip, char **value)
{
	/* get parameter */
	AssociatedDev *ap = (AssociatedDev*)ip->cpeData;
	 *value = GS_STRDUP(ap->devIPAddress);
	return CPE_OK;
}
/**@endparam                                               **/
/**@param LANDeviceWLANConfigurationAssociatedDevice_AssociatedDeviceAuthenticationState                     **/
CPE_STATUS getLANDeviceWLANConfigurationAssociatedDevice_AssociatedDeviceAuthenticationState(Instance *ip, char **value)
{
	/* get parameter */
	AssociatedDev *ap = (AssociatedDev*)ip->cpeData;
	 *value = GS_STRDUP(ap->devAuthState? "true": "false");
	return CPE_OK;
}
/**@endparam                                               **/
/**@endobj LANDeviceWLANConfigurationAssociatedDevice **/

/**@obj LANDeviceWLANConfigurationPreSharedKey **/

/**@param LANDeviceWLANConfigurationPreSharedKey_PreSharedKey                     **/
CPE_STATUS setLANDeviceWLANConfigurationPreSharedKey_PreSharedKey(Instance *ip, char *value)
{
	/* Set parameter */
	PreSharedKey *psk = (PreSharedKey *)ip->cpeData;
	strCpyLimited(psk->psKey, value, sizeof(psk->psKey));
	// WlanState *wsp = (WlanState*)ip->parent->cpeData;
	// AvailInterface *a = wsp->aifp;
	// ????? do_cmd(DEBUGLOG, "iwconfig","%s preSharedKey [%d] %s", a->ifname, ip->id, value);
	return CPE_OK;
}
CPE_STATUS getLANDeviceWLANConfigurationPreSharedKey_PreSharedKey(Instance *ip, char **value)
{
	/* get parameter */
	PreSharedKey *psk = (PreSharedKey *)ip->cpeData;
	*value = GS_STRDUP(psk->psKey);
	return CPE_OK;
}
/**@endparam                                               **/
/**@param LANDeviceWLANConfigurationPreSharedKey_KeyPassphrase                     **/
CPE_STATUS setLANDeviceWLANConfigurationPreSharedKey_KeyPassphrase(Instance *ip, char *value)
{
	/* Set parameter */
	PreSharedKey *psk = (PreSharedKey *)ip->cpeData;
	strCpyLimited(psk->keyPassphrase, value, sizeof(psk->keyPassphrase));
	// WlanState *wsp = (WlanState*)ip->parent->cpeData;
	// AvailInterface *a = wsp->aifp;
	// ???do_cmd(DEBUGLOG, "iwconfig","%s preSharedKey passphrase [%d] %s", a->ifname, ip->id, value);
	return CPE_OK;
}
CPE_STATUS getLANDeviceWLANConfigurationPreSharedKey_KeyPassphrase(Instance *ip, char **value)
{
	/* get parameter */
	PreSharedKey *psk = (PreSharedKey *)ip->cpeData;
	*value = GS_STRDUP(psk->keyPassphrase);
	return CPE_OK;
}
/**@endparam                                               **/
/**@endobj LANDeviceWLANConfigurationPreSharedKey **/

/**@obj LANDeviceWLANConfigurationStats **/

/**@param LANDeviceWLANConfigurationStats_ErrorsSent                     **/
CPE_STATUS getLANDeviceWLANConfigurationStats_ErrorsSent(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param LANDeviceWLANConfigurationStats_ErrorsReceived                     **/
CPE_STATUS getLANDeviceWLANConfigurationStats_ErrorsReceived(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param LANDeviceWLANConfigurationStats_UnicastPacketsSent                     **/
CPE_STATUS getLANDeviceWLANConfigurationStats_UnicastPacketsSent(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param LANDeviceWLANConfigurationStats_UnicastPacketsReceived                     **/
CPE_STATUS getLANDeviceWLANConfigurationStats_UnicastPacketsReceived(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param LANDeviceWLANConfigurationStats_DiscardPacketsSent                     **/
CPE_STATUS getLANDeviceWLANConfigurationStats_DiscardPacketsSent(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param LANDeviceWLANConfigurationStats_DiscardPacketsReceived                     **/
CPE_STATUS getLANDeviceWLANConfigurationStats_DiscardPacketsReceived(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param LANDeviceWLANConfigurationStats_MulticastPacketsSent                     **/
CPE_STATUS getLANDeviceWLANConfigurationStats_MulticastPacketsSent(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param LANDeviceWLANConfigurationStats_MulticastPacketsReceived                     **/
CPE_STATUS getLANDeviceWLANConfigurationStats_MulticastPacketsReceived(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param LANDeviceWLANConfigurationStats_BroadcastPacketsSent                     **/
CPE_STATUS getLANDeviceWLANConfigurationStats_BroadcastPacketsSent(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param LANDeviceWLANConfigurationStats_BroadcastPacketsReceived                     **/
CPE_STATUS getLANDeviceWLANConfigurationStats_BroadcastPacketsReceived(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param LANDeviceWLANConfigurationStats_UnknownProtoPacketsReceived                     **/
CPE_STATUS getLANDeviceWLANConfigurationStats_UnknownProtoPacketsReceived(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@endobj LANDeviceWLANConfigurationStats **/

/**@obj LANDeviceWLANConfigurationWEPKey **/

/**@param LANDeviceWLANConfigurationWEPKey_WEPKey                     **/
CPE_STATUS setLANDeviceWLANConfigurationWEPKey_WEPKey(Instance *ip, char *value)
{
	/* The parent instance of a KEPKey instance is a WLANConfiguration instance
	 * which has a pointer to the WlanState.
	 */
	WlanState *wsp = (WlanState*)ip->parent->cpeData;
	AvailInterface *a = wsp->aifp;
	char *p = (char *)ip->cpeData;
	/* copy to wlanstate key buffer */
	strCpyLimited(p, value, 129);
	do_cmd(DEBUGLOG, "iwconfig","%s key [%d] %s", a->ifname, ip->id, value);
	return CPE_OK;
}
CPE_STATUS getLANDeviceWLANConfigurationWEPKey_WEPKey(Instance *ip, char **value)
{
	/* get parameter */
	char *p = (char *)ip->cpeData;
	if ( *p )
		*value = GS_STRDUP(p);
	return CPE_OK;
}
/**@endparam                                               **/
/**@endobj LANDeviceWLANConfigurationWEPKey **/

/**@obj LANDeviceWLANConfiguration **/

/**@param LANDeviceWLANConfiguration_Enable                     **/
CPE_STATUS setLANDeviceWLANConfiguration_Enable(Instance *ip, char *value)
{
	/* set parameter */
	return CPE_OK;
}
CPE_STATUS getLANDeviceWLANConfiguration_Enable(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param LANDeviceWLANConfiguration_Status                     **/
CPE_STATUS getLANDeviceWLANConfiguration_Status(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param LANDeviceWLANConfiguration_BSSID                     **/
CPE_STATUS getLANDeviceWLANConfiguration_BSSID(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param LANDeviceWLANConfiguration_MaxBitRate                     **/
CPE_STATUS setLANDeviceWLANConfiguration_MaxBitRate(Instance *ip, char *value)
{
	/* set parameter */
	WlanState *wsp = (WlanState*)ip->cpeData;
	AvailInterface *a = wsp->aifp;
	do_cmd(DEBUGLOG,"iwconfig", "%s rate %s", a->ifname, value);
	strCpyLimited(wsp->rate, value, sizeof(wsp->rate));
	return CPE_OK;
}
CPE_STATUS getLANDeviceWLANConfiguration_MaxBitRate(Instance *ip, char **value)
{
	/* get parameter */
	WlanState *wsp = (WlanState*)ip->cpeData;
	getWLanRate(wsp);
	*value = GS_STRDUP(wsp->rate);
	return CPE_OK;
}
/**@endparam                                               **/
/**@param LANDeviceWLANConfiguration_Channel                     **/
CPE_STATUS setLANDeviceWLANConfiguration_Channel(Instance *ip, char *value)
{
	/* set parameter */
	WlanState *wsp = (WlanState*)ip->cpeData;
	AvailInterface *a = wsp->aifp;
	strCpyLimited(wsp->channel, value, sizeof(wsp->channel));
	do_cmd( DEBUGLOG,"iwconfig", "%s channel %s", a->ifname, value);
	return CPE_OK;
}
CPE_STATUS getLANDeviceWLANConfiguration_Channel(Instance *ip, char **value)
{
	/* get parameter */
	WlanState *wsp = (WlanState*)ip->cpeData;
	//getWLanChannel(wsp);
	*value = GS_STRDUP(wsp->channel);
	return CPE_OK;
}
/**@endparam                                               **/
/**@param LANDeviceWLANConfiguration_SSID                     **/
CPE_STATUS setLANDeviceWLANConfiguration_SSID(Instance *ip, char *value)
{
	/* Set parameter */
	WlanState *wsp = (WlanState*)ip->cpeData;
	AvailInterface *a = wsp->aifp;
	strCpyLimited(wsp->ssid, value, sizeof(wsp->ssid));
	do_cmd(DEBUGLOG,"iwconfig", "%s essid %s", a->ifname, value);
	return CPE_OK;
}
CPE_STATUS getLANDeviceWLANConfiguration_SSID(Instance *ip, char **value)
{
	/* get parameter */
	WlanState *wsp = (WlanState*)ip->cpeData;
	 *value = GS_STRDUP(wsp->ssid);
	return CPE_OK;
}
/**@endparam                                               **/
/**@param LANDeviceWLANConfiguration_BeaconType                     **/
CPE_STATUS setLANDeviceWLANConfiguration_BeaconType(Instance *ip, char *value)
{
	/* Set parameter */
	WlanState *wsp = (WlanState*)ip->cpeData;
	strCpyLimited(wsp->beaconType, value, sizeof(wsp->beaconType));
	//TODO: how do we set in h/w???
	return CPE_OK;
}
CPE_STATUS getLANDeviceWLANConfiguration_BeaconType(Instance *ip, char **value)
{
	/* get parameter */
	WlanState *wsp = (WlanState*)ip->cpeData;
	 *value = GS_STRDUP(wsp->beaconType);
	return CPE_OK;
}
/**@endparam                                               **/
/**@param LANDeviceWLANConfiguration_MACAddressControlEnabled                     **/
CPE_STATUS setLANDeviceWLANConfiguration_MACAddressControlEnabled(Instance *ip, char *value)
{
	/* set parameter */
	return CPE_OK;
}
CPE_STATUS getLANDeviceWLANConfiguration_MACAddressControlEnabled(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param LANDeviceWLANConfiguration_Standard                     **/
CPE_STATUS getLANDeviceWLANConfiguration_Standard(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param LANDeviceWLANConfiguration_WEPKeyIndex                     **/
CPE_STATUS setLANDeviceWLANConfiguration_WEPKeyIndex(Instance *ip, char *value)
{
	/* Set parameter */
	WlanState *wsp = (WlanState*)ip->cpeData;
	AvailInterface *a = wsp->aifp;
	int k = atoi(value);
	if (k>0 && k<=4){
		do_cmd(DEBUGLOG, "iwconfig", "%s key [%s]", a->ifname, value);
		wsp->keyIndex = atoi(value);
		return CPE_OK;
	}
	return CPE_ERR;
}
CPE_STATUS getLANDeviceWLANConfiguration_WEPKeyIndex(Instance *ip, char **value)
{
	/* get parameter */
	WlanState *wsp = (WlanState*)ip->cpeData;
	 *value = GS_STRDUP(itoa(wsp->keyIndex));
	return CPE_OK;
}
/**@endparam                                               **/
/**@param LANDeviceWLANConfiguration_KeyPassphrase                     **/
CPE_STATUS setLANDeviceWLANConfiguration_KeyPassphrase(Instance *ip, char *value)
{
	/* set parameter */
	return CPE_OK;
}
CPE_STATUS getLANDeviceWLANConfiguration_KeyPassphrase(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param LANDeviceWLANConfiguration_WEPEncryptionLevel                     **/
CPE_STATUS getLANDeviceWLANConfiguration_WEPEncryptionLevel(Instance *ip, char **value)
{
	/* get parameter */
	 *value = GS_STRDUP("Disabled,40-bit,104bit");
	return CPE_OK;
}
/**@endparam                                               **/
/**@param LANDeviceWLANConfiguration_BasicEncryptionModes                     **/
CPE_STATUS setLANDeviceWLANConfiguration_BasicEncryptionModes(Instance *ip, char *value)
{
	/* set parameter */
	return CPE_OK;
}
CPE_STATUS getLANDeviceWLANConfiguration_BasicEncryptionModes(Instance *ip, char **value)
{
	/* get parameter */
	*value = GS_STRDUP("None,WEPEncryption");
	return CPE_OK;
}
/**@endparam                                               **/
/**@param LANDeviceWLANConfiguration_BasicAuthenticationMode                     **/
CPE_STATUS setLANDeviceWLANConfiguration_BasicAuthenticationMode(Instance *ip, char *value)
{
	/* set parameter */
	return CPE_OK;
}
CPE_STATUS getLANDeviceWLANConfiguration_BasicAuthenticationMode(Instance *ip, char **value)
{
	/* get parameter */
	 *value = GS_STRDUP("None,SharedAuthentication");
	return CPE_OK;
}
/**@endparam                                               **/
/**@param LANDeviceWLANConfiguration_WPAEncryptionModes                     **/
CPE_STATUS setLANDeviceWLANConfiguration_WPAEncryptionModes(Instance *ip, char *value)
{
	/* set parameter */
	return CPE_OK;
}
CPE_STATUS getLANDeviceWLANConfiguration_WPAEncryptionModes(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param LANDeviceWLANConfiguration_WPAAuthenticationMode                     **/
CPE_STATUS setLANDeviceWLANConfiguration_WPAAuthenticationMode(Instance *ip, char *value)
{
	/* set parameter */
	return CPE_OK;
}
CPE_STATUS getLANDeviceWLANConfiguration_WPAAuthenticationMode(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param LANDeviceWLANConfiguration_PossibleChannels                     **/
CPE_STATUS getLANDeviceWLANConfiguration_PossibleChannels(Instance *ip, char **value)
{
	/* get parameter */
	 *value = GS_STRDUP("1-13");
	return CPE_OK;
}
/**@endparam                                               **/
/**@param LANDeviceWLANConfiguration_BasicDataTransmitRates                     **/
CPE_STATUS setLANDeviceWLANConfiguration_BasicDataTransmitRates(Instance *ip, char *value)
{
	/* set parameter */
	return CPE_OK;
}
CPE_STATUS getLANDeviceWLANConfiguration_BasicDataTransmitRates(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param LANDeviceWLANConfiguration_OperationalDataTransmitRates                     **/
CPE_STATUS setLANDeviceWLANConfiguration_OperationalDataTransmitRates(Instance *ip, char *value)
{
	/* set parameter */
	return CPE_OK;
}
CPE_STATUS getLANDeviceWLANConfiguration_OperationalDataTransmitRates(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param LANDeviceWLANConfiguration_PossibleDataTransmitRates                     **/
CPE_STATUS getLANDeviceWLANConfiguration_PossibleDataTransmitRates(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param LANDeviceWLANConfiguration_RadioEnabled                     **/
CPE_STATUS setLANDeviceWLANConfiguration_RadioEnabled(Instance *ip, char *value)
{
	/* Set parameter */
	WlanState *wsp =(WlanState*)ip->cpeData;
	AvailInterface *ap = wsp->aifp;
	//do_cmd(DEBUGLOG, "ifconfig", "%s %s", ap->ifname, testBoolean(value)? "up": "down");
	cpeLog(DEBUGLOG, "ifconfig", "%s %s", ap->ifname, testBoolean(value)? "up": "down");
	return CPE_OK;
}
CPE_STATUS getLANDeviceWLANConfiguration_RadioEnabled(Instance *ip, char **value)
{
	/* get parameter */
	WlanState *wsp =(WlanState*)ip->cpeData;
	AvailInterface *ap = wsp->aifp;
	*value = GS_STRDUP(ap->ifStatus==eUP? "1": "0");
	return CPE_OK;
}
/**@endparam                                               **/
/**@param LANDeviceWLANConfiguration_AutoRateFallBackEnabled                     **/
CPE_STATUS setLANDeviceWLANConfiguration_AutoRateFallBackEnabled(Instance *ip, char *value)
{
	/* set parameter */
	return CPE_OK;
}
CPE_STATUS getLANDeviceWLANConfiguration_AutoRateFallBackEnabled(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param LANDeviceWLANConfiguration_TotalBytesSent                     **/
CPE_STATUS getLANDeviceWLANConfiguration_TotalBytesSent(Instance *ip, char **value)
{
	/* get parameter */
	WlanState *wsp =(WlanState*)ip->cpeData;
	AvailInterface *ap = wsp->aifp;
	*value = GS_STRDUP(ap->txBytes);
	return CPE_OK;
}
/**@endparam                                               **/
/**@param LANDeviceWLANConfiguration_TotalBytesReceived                     **/
CPE_STATUS getLANDeviceWLANConfiguration_TotalBytesReceived(Instance *ip, char **value)
{
	/* get parameter */
	WlanState *wsp =(WlanState*)ip->cpeData;
	AvailInterface *ap = wsp->aifp;
	*value = GS_STRDUP(ap->rxBytes);
	return CPE_OK;
}
/**@endparam                                               **/
/**@param LANDeviceWLANConfiguration_TotalPacketsSent                     **/
CPE_STATUS getLANDeviceWLANConfiguration_TotalPacketsSent(Instance *ip, char **value)
{
	/* get parameter */
	WlanState *wsp =(WlanState*)ip->cpeData;
	AvailInterface *ap = wsp->aifp;
	*value = GS_STRDUP(ap->txPackets);
	return CPE_OK;
}
/**@endparam                                               **/
/**@param LANDeviceWLANConfiguration_TotalPacketsReceived                     **/
CPE_STATUS getLANDeviceWLANConfiguration_TotalPacketsReceived(Instance *ip, char **value)
{
	/* get parameter */
	WlanState *wsp =(WlanState*)ip->cpeData;
	AvailInterface *ap = wsp->aifp;
	 *value = GS_STRDUP(ap->rxPackets);
	return CPE_OK;
}
/**@endparam                                               **/
/**@param LANDeviceWLANConfiguration_TotalAssociations                     **/
CPE_STATUS getLANDeviceWLANConfiguration_TotalAssociations(Instance *ip, char **value)
{
	cwmpGetObjInstanceCntStr("AssociatedDevice", value);
	return CPE_OK;
}
/**@endparam                                               **/
/**@param LANDeviceWLANConfiguration_Name                     **/
CPE_STATUS getLANDeviceWLANConfiguration_Name(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param LANDeviceWLANConfiguration_AutoChannelEnable                     **/
CPE_STATUS setLANDeviceWLANConfiguration_AutoChannelEnable(Instance *ip, char *value)
{
	/* set parameter */
	return CPE_OK;
}
CPE_STATUS getLANDeviceWLANConfiguration_AutoChannelEnable(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param LANDeviceWLANConfiguration_SSIDAdvertisementEnabled                     **/
CPE_STATUS setLANDeviceWLANConfiguration_SSIDAdvertisementEnabled(Instance *ip, char *value)
{
	/* set parameter */
	return CPE_OK;
}
CPE_STATUS getLANDeviceWLANConfiguration_SSIDAdvertisementEnabled(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param LANDeviceWLANConfiguration_TransmitPowerSupported                     **/
CPE_STATUS getLANDeviceWLANConfiguration_TransmitPowerSupported(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param LANDeviceWLANConfiguration_TransmitPower                     **/
CPE_STATUS setLANDeviceWLANConfiguration_TransmitPower(Instance *ip, char *value)
{
	/* set parameter */
	return CPE_OK;
}
CPE_STATUS getLANDeviceWLANConfiguration_TransmitPower(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@endobj LANDeviceWLANConfiguration **/

/**@obj LANDevice **/
CPE_STATUS  addLANDevice(CWMPObject *o, Instance *ip)
{
	/* add instance data */
	return CPE_OK;
}
CPE_STATUS  delLANDevice(CWMPObject *o, Instance *ip)
{
	/* delete instance data */
	return CPE_OK;
}

/**@param LANDevice_LANEthernetInterfaceNumberOfEntries                     **/
CPE_STATUS getLANDevice_LANEthernetInterfaceNumberOfEntries(Instance *ip, char **value)
{
	cwmpGetObjInstanceCntStr("LANEthernetInterfaceConfig", value);
	return CPE_OK;
}
/**@endparam                                               **/
/**@param LANDevice_LANUSBInterfaceNumberOfEntries                     **/
CPE_STATUS getLANDevice_LANUSBInterfaceNumberOfEntries(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param LANDevice_LANWLANConfigurationNumberOfEntries                     **/
CPE_STATUS getLANDevice_LANWLANConfigurationNumberOfEntries(Instance *ip, char **value)
{
	cwmpGetObjInstanceCntStr("WLANConfiguration", value);
	return CPE_OK;
}
/**@endparam                                               **/
/**@endobj LANDevice **/






