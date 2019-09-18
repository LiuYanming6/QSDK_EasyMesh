/*----------------------------------------------------------------------*
 * Gatespace
 * Copyright 2011, 2012 Gatespace Networks, Inc., All Rights Reserved.
 * Gatespace Networks, Inc. confidential material.
 *----------------------------------------------------------------------*
 * File Name  : 
 * Description: Auto-Generated C tables file.
 *----------------------------------------------------------------------*
 * $Revision: 1.1 $
 *
 * $Id: tables.c,v 1.1 2012/05/10 17:37:59 dmounday Exp $
 *----------------------------------------------------------------------*/

#include "paramTree.h"
#include "rpc.h"



/**  UDPConnReq:1, TraceRoute:1, EthernetLAN:2, EthernetLAN:1, IPPing:1, 
    ADSLWAN:1, DeviceAssociation:2, DHCPOption:1, DeviceAssociation:1, Time:1, 
    WiFiLAN:2, Time:2, EthernetWAN:1, WiFiLAN:1, Bridging:2, 
    Bridging:1, Baseline:2, Baseline:1, 
**/
/**@param InternetGatewayDevice.DeviceInfo. */
CPEGETFUNC(getDeviceInfo_Manufacturer);
CPEGETFUNC(getDeviceInfo_ManufacturerOUI);
CPEGETFUNC(getDeviceInfo_ModelName);
CPEGETFUNC(getDeviceInfo_Description);
CPEGETFUNC(getDeviceInfo_SerialNumber);
CPEGETFUNC(getDeviceInfo_HardwareVersion);
CPEGETFUNC(getDeviceInfo_SoftwareVersion);
CPEGETFUNC(getDeviceInfo_SpecVersion);
CPEGETFUNC(getDeviceInfo_ProvisioningCode);
CPESETFUNC(setDeviceInfo_ProvisioningCode);
CPEGETFUNC(getDeviceInfo_UpTime);
CPEGETFUNC(getDeviceInfo_DeviceLog);
CWMPParam DeviceInfo_Params[]={
	{ "Manufacturer", getDeviceInfo_Manufacturer, NULL, NULL, RPC_R, eString, 0, 64},
	{ "ManufacturerOUI", getDeviceInfo_ManufacturerOUI, NULL, NULL, RPC_R, eString, 0, 6},
	{ "ModelName", getDeviceInfo_ModelName, NULL, NULL, RPC_R, eString, 0, 64},
	{ "Description", getDeviceInfo_Description, NULL, NULL, RPC_R, eString, 0, 256},
	{ "SerialNumber", getDeviceInfo_SerialNumber, NULL, NULL, RPC_R, eString, 0, 64},
	{ "HardwareVersion", getDeviceInfo_HardwareVersion, NULL, NULL, RPC_R, eString, FORCED_INFORM, 64},
	{ "SoftwareVersion", getDeviceInfo_SoftwareVersion, NULL, NULL, RPC_R, eString, FORCED_INFORM|FORCED_ACTIVE, 64},
	{ "SpecVersion", getDeviceInfo_SpecVersion, NULL, NULL, RPC_R, eString, FORCED_INFORM, 0},
	{ "ProvisioningCode", getDeviceInfo_ProvisioningCode, setDeviceInfo_ProvisioningCode, NULL, RPC_RW, eString, FORCED_INFORM|FORCED_ACTIVE, 64},
	{ "UpTime", getDeviceInfo_UpTime, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "DeviceLog", getDeviceInfo_DeviceLog, NULL, NULL, RPC_R, eString, NOACTIVENOTIFY, 32768},
	{NULL}
};
/**@endparam InternetGatewayDevice.DeviceInfo.  */

/**@param InternetGatewayDevice.IPPingDiagnostics. */
CPEGETFUNC(getIPPingDiagnostics_DiagnosticsState);
CPESETFUNC(setIPPingDiagnostics_DiagnosticsState);
CPEGETFUNC(getIPPingDiagnostics_Interface);
CPESETFUNC(setIPPingDiagnostics_Interface);
CPEGETFUNC(getIPPingDiagnostics_Host);
CPESETFUNC(setIPPingDiagnostics_Host);
CPEGETFUNC(getIPPingDiagnostics_NumberOfRepetitions);
CPESETFUNC(setIPPingDiagnostics_NumberOfRepetitions);
CPEGETFUNC(getIPPingDiagnostics_Timeout);
CPESETFUNC(setIPPingDiagnostics_Timeout);
CPEGETFUNC(getIPPingDiagnostics_DataBlockSize);
CPESETFUNC(setIPPingDiagnostics_DataBlockSize);
CPEGETFUNC(getIPPingDiagnostics_DSCP);
CPESETFUNC(setIPPingDiagnostics_DSCP);
CPEGETFUNC(getIPPingDiagnostics_SuccessCount);
CPEGETFUNC(getIPPingDiagnostics_FailureCount);
CPEGETFUNC(getIPPingDiagnostics_AverageResponseTime);
CPEGETFUNC(getIPPingDiagnostics_MaximumResponseTime);
CPEGETFUNC(getIPPingDiagnostics_MinimumResponseTime);
CWMPParam IPPingDiagnostics_Params[]={
	{ "DiagnosticsState", getIPPingDiagnostics_DiagnosticsState, setIPPingDiagnostics_DiagnosticsState, NULL, RPC_RW, eString, NOACTIVENOTIFY, 0},
	{ "Interface", getIPPingDiagnostics_Interface, setIPPingDiagnostics_Interface, NULL, RPC_RW,  eString, 0, 0},
	{ "Host", getIPPingDiagnostics_Host, setIPPingDiagnostics_Host, NULL, RPC_RW, eString, 0, 256},
	{ "NumberOfRepetitions", getIPPingDiagnostics_NumberOfRepetitions, setIPPingDiagnostics_NumberOfRepetitions, NULL, RPC_RW, eUnsignedInt, 0, 0},
	{ "Timeout", getIPPingDiagnostics_Timeout, setIPPingDiagnostics_Timeout, NULL, RPC_RW, eUnsignedInt, 0, 0},
	{ "DataBlockSize", getIPPingDiagnostics_DataBlockSize, setIPPingDiagnostics_DataBlockSize, NULL, RPC_RW, eUnsignedInt, 0, 0},
	{ "DSCP", getIPPingDiagnostics_DSCP, setIPPingDiagnostics_DSCP, NULL, RPC_RW, eUnsignedInt, 0, 0},
	{ "SuccessCount", getIPPingDiagnostics_SuccessCount, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "FailureCount", getIPPingDiagnostics_FailureCount, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "AverageResponseTime", getIPPingDiagnostics_AverageResponseTime, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "MaximumResponseTime", getIPPingDiagnostics_MaximumResponseTime, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "MinimumResponseTime", getIPPingDiagnostics_MinimumResponseTime, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{NULL}
};
/**@endparam InternetGatewayDevice.IPPingDiagnostics.  */

/**@param InternetGatewayDevice.LANConfigSecurity. */
CPEGETFUNC(getLANConfigSecurity_ConfigPassword);
CPESETFUNC(setLANConfigSecurity_ConfigPassword);
CWMPParam LANConfigSecurity_Params[]={
	{ "ConfigPassword", getLANConfigSecurity_ConfigPassword, setLANConfigSecurity_ConfigPassword, NULL, RPC_RW, eStringSetOnly, 0, 64},
	{NULL}
};
/**@endparam InternetGatewayDevice.LANConfigSecurity.  */

/**@param InternetGatewayDevice.LANDevice.{i}.Hosts.Host.{i}. */
CPEGETFUNC(getLANDeviceHostsHost_IPAddress);
CPEGETFUNC(getLANDeviceHostsHost_AddressSource);
CPEGETFUNC(getLANDeviceHostsHost_LeaseTimeRemaining);
CPEGETFUNC(getLANDeviceHostsHost_MACAddress);
CPEGETFUNC(getLANDeviceHostsHost_HostName);
CPEGETFUNC(getLANDeviceHostsHost_InterfaceType);
CPEGETFUNC(getLANDeviceHostsHost_Active);
CPEGETFUNC(getLANDeviceHostsHost_Layer2Interface);
CWMPParam LANDeviceHostsHost_Params[]={
	{ "IPAddress", getLANDeviceHostsHost_IPAddress, NULL, NULL, RPC_R, eString, 0, 45},
	{ "AddressSource", getLANDeviceHostsHost_AddressSource, NULL, NULL, RPC_R, eString, 0, 0},
	{ "LeaseTimeRemaining", getLANDeviceHostsHost_LeaseTimeRemaining, NULL, NULL, RPC_R, eInt, NOACTIVENOTIFY, 0},
	{ "MACAddress", getLANDeviceHostsHost_MACAddress, NULL, NULL, RPC_R, eString, 0, 17},
	{ "HostName", getLANDeviceHostsHost_HostName, NULL, NULL, RPC_R, eString, 0, 64},
	{ "InterfaceType", getLANDeviceHostsHost_InterfaceType, NULL, NULL, RPC_R, eString, 0, 0},
	{ "Active", getLANDeviceHostsHost_Active, NULL, NULL, RPC_R, eBoolean, 0, 0},
	{ "Layer2Interface", getLANDeviceHostsHost_Layer2Interface, NULL, NULL, RPC_R, eString, 0, 256},
	{NULL}
};
/**@endparam InternetGatewayDevice.LANDevice.{i}.Hosts.Host.{i}.  */

/**@param InternetGatewayDevice.LANDevice.{i}.Hosts. */
CPEGETFUNC(getLANDeviceHosts_HostNumberOfEntries);
CWMPParam LANDeviceHosts_Params[]={
	{ "HostNumberOfEntries", getLANDeviceHosts_HostNumberOfEntries, NULL, NULL, RPC_R, eUnsignedInt, 0, 0},
	{NULL}
};
/**@endparam InternetGatewayDevice.LANDevice.{i}.Hosts.  */

/**@obj InternetGatewayDevice.LANDevice.{i}.Hosts.  */
CWMPObject LANDeviceHosts_Objs[]={
	{"Host", NULL, NULL, NULL, NULL, NULL, LANDeviceHostsHost_Params, NULL, eInstance,  0},
	{NULL}
};

/**@endobj InternetGatewayDevice.LANDevice.{i}.Hosts.  */
/**@param InternetGatewayDevice.LANDevice.{i}.LANEthernetInterfaceConfig.{i}.Stats. */
CPEGETFUNC(getLANDeviceLANEthernetInterfaceConfigStats_BytesSent);
CPEGETFUNC(getLANDeviceLANEthernetInterfaceConfigStats_BytesReceived);
CPEGETFUNC(getLANDeviceLANEthernetInterfaceConfigStats_PacketsSent);
CPEGETFUNC(getLANDeviceLANEthernetInterfaceConfigStats_PacketsReceived);
CPEGETFUNC(getLANDeviceLANEthernetInterfaceConfigStats_ErrorsSent);
CPEGETFUNC(getLANDeviceLANEthernetInterfaceConfigStats_ErrorsReceived);
CPEGETFUNC(getLANDeviceLANEthernetInterfaceConfigStats_UnicastPacketsSent);
CPEGETFUNC(getLANDeviceLANEthernetInterfaceConfigStats_UnicastPacketsReceived);
CPEGETFUNC(getLANDeviceLANEthernetInterfaceConfigStats_DiscardPacketsSent);
CPEGETFUNC(getLANDeviceLANEthernetInterfaceConfigStats_DiscardPacketsReceived);
CPEGETFUNC(getLANDeviceLANEthernetInterfaceConfigStats_MulticastPacketsSent);
CPEGETFUNC(getLANDeviceLANEthernetInterfaceConfigStats_MulticastPacketsReceived);
CPEGETFUNC(getLANDeviceLANEthernetInterfaceConfigStats_BroadcastPacketsSent);
CPEGETFUNC(getLANDeviceLANEthernetInterfaceConfigStats_BroadcastPacketsReceived);
CPEGETFUNC(getLANDeviceLANEthernetInterfaceConfigStats_UnknownProtoPacketsReceived);
CWMPParam LANDeviceLANEthernetInterfaceConfigStats_Params[]={
	{ "BytesSent", getLANDeviceLANEthernetInterfaceConfigStats_BytesSent, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "BytesReceived", getLANDeviceLANEthernetInterfaceConfigStats_BytesReceived, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "PacketsSent", getLANDeviceLANEthernetInterfaceConfigStats_PacketsSent, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "PacketsReceived", getLANDeviceLANEthernetInterfaceConfigStats_PacketsReceived, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "ErrorsSent", getLANDeviceLANEthernetInterfaceConfigStats_ErrorsSent, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "ErrorsReceived", getLANDeviceLANEthernetInterfaceConfigStats_ErrorsReceived, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "UnicastPacketsSent", getLANDeviceLANEthernetInterfaceConfigStats_UnicastPacketsSent, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "UnicastPacketsReceived", getLANDeviceLANEthernetInterfaceConfigStats_UnicastPacketsReceived, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "DiscardPacketsSent", getLANDeviceLANEthernetInterfaceConfigStats_DiscardPacketsSent, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "DiscardPacketsReceived", getLANDeviceLANEthernetInterfaceConfigStats_DiscardPacketsReceived, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "MulticastPacketsSent", getLANDeviceLANEthernetInterfaceConfigStats_MulticastPacketsSent, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "MulticastPacketsReceived", getLANDeviceLANEthernetInterfaceConfigStats_MulticastPacketsReceived, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "BroadcastPacketsSent", getLANDeviceLANEthernetInterfaceConfigStats_BroadcastPacketsSent, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "BroadcastPacketsReceived", getLANDeviceLANEthernetInterfaceConfigStats_BroadcastPacketsReceived, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "UnknownProtoPacketsReceived", getLANDeviceLANEthernetInterfaceConfigStats_UnknownProtoPacketsReceived, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{NULL}
};
/**@endparam InternetGatewayDevice.LANDevice.{i}.LANEthernetInterfaceConfig.{i}.Stats.  */

/**@param InternetGatewayDevice.LANDevice.{i}.LANEthernetInterfaceConfig.{i}. */
CPEGETFUNC(getLANDeviceLANEthernetInterfaceConfig_Enable);
CPESETFUNC(setLANDeviceLANEthernetInterfaceConfig_Enable);
CPEGETFUNC(getLANDeviceLANEthernetInterfaceConfig_Status);
CPEGETFUNC(getLANDeviceLANEthernetInterfaceConfig_MACAddress);
CPEGETFUNC(getLANDeviceLANEthernetInterfaceConfig_MACAddressControlEnabled);
CPESETFUNC(setLANDeviceLANEthernetInterfaceConfig_MACAddressControlEnabled);
CPEGETFUNC(getLANDeviceLANEthernetInterfaceConfig_MaxBitRate);
CPESETFUNC(setLANDeviceLANEthernetInterfaceConfig_MaxBitRate);
CPEGETFUNC(getLANDeviceLANEthernetInterfaceConfig_DuplexMode);
CPESETFUNC(setLANDeviceLANEthernetInterfaceConfig_DuplexMode);
CPEGETFUNC(getLANDeviceLANEthernetInterfaceConfig_Name);
CWMPParam LANDeviceLANEthernetInterfaceConfig_Params[]={
	{ "Enable", getLANDeviceLANEthernetInterfaceConfig_Enable, setLANDeviceLANEthernetInterfaceConfig_Enable, NULL, RPC_RW, eBoolean, 0, 0},
	{ "Status", getLANDeviceLANEthernetInterfaceConfig_Status, NULL, NULL, RPC_R, eString, 0, 0},
	{ "MACAddress", getLANDeviceLANEthernetInterfaceConfig_MACAddress, NULL, NULL, RPC_R, eString, 0, 17},
	{ "MACAddressControlEnabled", getLANDeviceLANEthernetInterfaceConfig_MACAddressControlEnabled, setLANDeviceLANEthernetInterfaceConfig_MACAddressControlEnabled, NULL, RPC_RW, eBoolean, 0, 0},
	{ "MaxBitRate", getLANDeviceLANEthernetInterfaceConfig_MaxBitRate, setLANDeviceLANEthernetInterfaceConfig_MaxBitRate, NULL, RPC_RW, eString, 0, 0},
	{ "DuplexMode", getLANDeviceLANEthernetInterfaceConfig_DuplexMode, setLANDeviceLANEthernetInterfaceConfig_DuplexMode, NULL, RPC_RW, eString, 0, 0},
	{ "Name", getLANDeviceLANEthernetInterfaceConfig_Name, NULL, NULL, RPC_R, eString, 0, 16},
	{NULL}
};
/**@endparam InternetGatewayDevice.LANDevice.{i}.LANEthernetInterfaceConfig.{i}.  */

/**@obj InternetGatewayDevice.LANDevice.{i}.LANEthernetInterfaceConfig.{i}.  */
CWMPObject LANDeviceLANEthernetInterfaceConfig_Objs[]={
	{"Stats", NULL, NULL, NULL, NULL, NULL, LANDeviceLANEthernetInterfaceConfigStats_Params, NULL, eObject,  0},
	{NULL}
};

/**@endobj InternetGatewayDevice.LANDevice.{i}.LANEthernetInterfaceConfig.{i}.  */
/**@param InternetGatewayDevice.LANDevice.{i}.LANHostConfigManagement.DHCPConditionalServingPool.{i}.DHCPOption.{i}. */
CPEGETFUNC(getLANDeviceLANHostConfigManagementDHCPConditionalServingPoolDHCPOption_Enable);
CPESETFUNC(setLANDeviceLANHostConfigManagementDHCPConditionalServingPoolDHCPOption_Enable);
CPEGETFUNC(getLANDeviceLANHostConfigManagementDHCPConditionalServingPoolDHCPOption_Tag);
CPESETFUNC(setLANDeviceLANHostConfigManagementDHCPConditionalServingPoolDHCPOption_Tag);
CPEGETFUNC(getLANDeviceLANHostConfigManagementDHCPConditionalServingPoolDHCPOption_Value);
CPESETFUNC(setLANDeviceLANHostConfigManagementDHCPConditionalServingPoolDHCPOption_Value);
CWMPParam LANDeviceLANHostConfigManagementDHCPConditionalServingPoolDHCPOption_Params[]={
	{ "Enable", getLANDeviceLANHostConfigManagementDHCPConditionalServingPoolDHCPOption_Enable, setLANDeviceLANHostConfigManagementDHCPConditionalServingPoolDHCPOption_Enable, NULL, RPC_RW, eBoolean, 0, 0},
	{ "Tag", getLANDeviceLANHostConfigManagementDHCPConditionalServingPoolDHCPOption_Tag, setLANDeviceLANHostConfigManagementDHCPConditionalServingPoolDHCPOption_Tag, NULL, RPC_RW, eUnsignedInt, 0, 0},
	{ "Value", getLANDeviceLANHostConfigManagementDHCPConditionalServingPoolDHCPOption_Value, setLANDeviceLANHostConfigManagementDHCPConditionalServingPoolDHCPOption_Value, NULL, RPC_RW, eBase64, 0, 255},
	{NULL}
};
/**@endparam InternetGatewayDevice.LANDevice.{i}.LANHostConfigManagement.DHCPConditionalServingPool.{i}.DHCPOption.{i}.  */

/**@param InternetGatewayDevice.LANDevice.{i}.LANHostConfigManagement.DHCPConditionalServingPool.{i}. */
CPEGETFUNC(getLANDeviceLANHostConfigManagementDHCPConditionalServingPool_DHCPOptionNumberOfEntries);
CWMPParam LANDeviceLANHostConfigManagementDHCPConditionalServingPool_Params[]={
	{ "DHCPOptionNumberOfEntries", getLANDeviceLANHostConfigManagementDHCPConditionalServingPool_DHCPOptionNumberOfEntries, NULL, NULL, RPC_R, eUnsignedInt, 0, 0},
	{NULL}
};
/**@endparam InternetGatewayDevice.LANDevice.{i}.LANHostConfigManagement.DHCPConditionalServingPool.{i}.  */

/**@obj InternetGatewayDevice.LANDevice.{i}.LANHostConfigManagement.DHCPConditionalServingPool.{i}.  */
CPEADDOBJ(addLANDeviceLANHostConfigManagementDHCPConditionalServingPoolDHCPOption);
CPEDELOBJ(delLANDeviceLANHostConfigManagementDHCPConditionalServingPoolDHCPOption);
CWMPObject LANDeviceLANHostConfigManagementDHCPConditionalServingPool_Objs[]={
	{"DHCPOption", delLANDeviceLANHostConfigManagementDHCPConditionalServingPoolDHCPOption, addLANDeviceLANHostConfigManagementDHCPConditionalServingPoolDHCPOption, NULL, NULL, NULL, LANDeviceLANHostConfigManagementDHCPConditionalServingPoolDHCPOption_Params, NULL, eInstance,  0},
	{NULL}
};

/**@endobj InternetGatewayDevice.LANDevice.{i}.LANHostConfigManagement.DHCPConditionalServingPool.{i}.  */
/**@param InternetGatewayDevice.LANDevice.{i}.LANHostConfigManagement.DHCPOption.{i}. */
CPEGETFUNC(getLANDeviceLANHostConfigManagementDHCPOption_Enable);
CPESETFUNC(setLANDeviceLANHostConfigManagementDHCPOption_Enable);
CPEGETFUNC(getLANDeviceLANHostConfigManagementDHCPOption_Tag);
CPESETFUNC(setLANDeviceLANHostConfigManagementDHCPOption_Tag);
CPEGETFUNC(getLANDeviceLANHostConfigManagementDHCPOption_Value);
CPESETFUNC(setLANDeviceLANHostConfigManagementDHCPOption_Value);
CWMPParam LANDeviceLANHostConfigManagementDHCPOption_Params[]={
	{ "Enable", getLANDeviceLANHostConfigManagementDHCPOption_Enable, setLANDeviceLANHostConfigManagementDHCPOption_Enable, NULL, RPC_RW, eBoolean, 0, 0},
	{ "Tag", getLANDeviceLANHostConfigManagementDHCPOption_Tag, setLANDeviceLANHostConfigManagementDHCPOption_Tag, NULL, RPC_RW, eUnsignedInt, 0, 0},
	{ "Value", getLANDeviceLANHostConfigManagementDHCPOption_Value, setLANDeviceLANHostConfigManagementDHCPOption_Value, NULL, RPC_RW, eBase64, 0, 255},
	{NULL}
};
/**@endparam InternetGatewayDevice.LANDevice.{i}.LANHostConfigManagement.DHCPOption.{i}.  */

/**@param InternetGatewayDevice.LANDevice.{i}.LANHostConfigManagement.IPInterface.{i}. */
CPEGETFUNC(getLANDeviceLANHostConfigManagementIPInterface_Enable);
CPESETFUNC(setLANDeviceLANHostConfigManagementIPInterface_Enable);
CPEGETFUNC(getLANDeviceLANHostConfigManagementIPInterface_IPInterfaceIPAddress);
CPESETFUNC(setLANDeviceLANHostConfigManagementIPInterface_IPInterfaceIPAddress);
CPEGETFUNC(getLANDeviceLANHostConfigManagementIPInterface_IPInterfaceSubnetMask);
CPESETFUNC(setLANDeviceLANHostConfigManagementIPInterface_IPInterfaceSubnetMask);
CPEGETFUNC(getLANDeviceLANHostConfigManagementIPInterface_IPInterfaceAddressingType);
CPESETFUNC(setLANDeviceLANHostConfigManagementIPInterface_IPInterfaceAddressingType);
CWMPParam LANDeviceLANHostConfigManagementIPInterface_Params[]={
	{ "Enable", getLANDeviceLANHostConfigManagementIPInterface_Enable, setLANDeviceLANHostConfigManagementIPInterface_Enable, NULL, RPC_RW, eBoolean, 0, 0},
	{ "IPInterfaceIPAddress", getLANDeviceLANHostConfigManagementIPInterface_IPInterfaceIPAddress, setLANDeviceLANHostConfigManagementIPInterface_IPInterfaceIPAddress, NULL, RPC_RW, eString, 0, 45},
	{ "IPInterfaceSubnetMask", getLANDeviceLANHostConfigManagementIPInterface_IPInterfaceSubnetMask, setLANDeviceLANHostConfigManagementIPInterface_IPInterfaceSubnetMask, NULL, RPC_RW, eString, 0, 45},
	{ "IPInterfaceAddressingType", getLANDeviceLANHostConfigManagementIPInterface_IPInterfaceAddressingType, setLANDeviceLANHostConfigManagementIPInterface_IPInterfaceAddressingType, NULL, RPC_RW, eString, 0, 0},
	{NULL}
};
/**@endparam InternetGatewayDevice.LANDevice.{i}.LANHostConfigManagement.IPInterface.{i}.  */

/**@param InternetGatewayDevice.LANDevice.{i}.LANHostConfigManagement. */
CPEGETFUNC(getLANDeviceLANHostConfigManagement_DHCPServerConfigurable);
CPESETFUNC(setLANDeviceLANHostConfigManagement_DHCPServerConfigurable);
CPEGETFUNC(getLANDeviceLANHostConfigManagement_DHCPServerEnable);
CPESETFUNC(setLANDeviceLANHostConfigManagement_DHCPServerEnable);
CPEGETFUNC(getLANDeviceLANHostConfigManagement_DHCPRelay);
CPEGETFUNC(getLANDeviceLANHostConfigManagement_MinAddress);
CPESETFUNC(setLANDeviceLANHostConfigManagement_MinAddress);
CPEGETFUNC(getLANDeviceLANHostConfigManagement_MaxAddress);
CPESETFUNC(setLANDeviceLANHostConfigManagement_MaxAddress);
CPEGETFUNC(getLANDeviceLANHostConfigManagement_ReservedAddresses);
CPESETFUNC(setLANDeviceLANHostConfigManagement_ReservedAddresses);
CPEGETFUNC(getLANDeviceLANHostConfigManagement_SubnetMask);
CPESETFUNC(setLANDeviceLANHostConfigManagement_SubnetMask);
CPEGETFUNC(getLANDeviceLANHostConfigManagement_DNSServers);
CPESETFUNC(setLANDeviceLANHostConfigManagement_DNSServers);
CPEGETFUNC(getLANDeviceLANHostConfigManagement_DomainName);
CPESETFUNC(setLANDeviceLANHostConfigManagement_DomainName);
CPEGETFUNC(getLANDeviceLANHostConfigManagement_IPRouters);
CPESETFUNC(setLANDeviceLANHostConfigManagement_IPRouters);
CPEGETFUNC(getLANDeviceLANHostConfigManagement_IPInterfaceNumberOfEntries);
CPEGETFUNC(getLANDeviceLANHostConfigManagement_MACAddress);
CPEGETFUNC(getLANDeviceLANHostConfigManagement_DHCPOptionNumberOfEntries);
CWMPParam LANDeviceLANHostConfigManagement_Params[]={
	{ "DHCPServerConfigurable", getLANDeviceLANHostConfigManagement_DHCPServerConfigurable, setLANDeviceLANHostConfigManagement_DHCPServerConfigurable, NULL, RPC_RW, eBoolean, 0, 0},
	{ "DHCPServerEnable", getLANDeviceLANHostConfigManagement_DHCPServerEnable, setLANDeviceLANHostConfigManagement_DHCPServerEnable, NULL, RPC_RW, eBoolean, 0, 0},
	{ "DHCPRelay", getLANDeviceLANHostConfigManagement_DHCPRelay, NULL, NULL, RPC_R, eBoolean, 0, 0},
	{ "MinAddress", getLANDeviceLANHostConfigManagement_MinAddress, setLANDeviceLANHostConfigManagement_MinAddress, NULL, RPC_RW, eString, 0, 45},
	{ "MaxAddress", getLANDeviceLANHostConfigManagement_MaxAddress, setLANDeviceLANHostConfigManagement_MaxAddress, NULL, RPC_RW, eString, 0, 45},
	{ "ReservedAddresses", getLANDeviceLANHostConfigManagement_ReservedAddresses, setLANDeviceLANHostConfigManagement_ReservedAddresses, NULL, RPC_RW, eString, 0, 45},
	{ "SubnetMask", getLANDeviceLANHostConfigManagement_SubnetMask, setLANDeviceLANHostConfigManagement_SubnetMask, NULL, RPC_RW, eString, 0, 45},
	{ "DNSServers", getLANDeviceLANHostConfigManagement_DNSServers, setLANDeviceLANHostConfigManagement_DNSServers, NULL, RPC_RW, eString, 0, 45},
	{ "DomainName", getLANDeviceLANHostConfigManagement_DomainName, setLANDeviceLANHostConfigManagement_DomainName, NULL, RPC_RW, eString, 0, 64},
	{ "IPRouters", getLANDeviceLANHostConfigManagement_IPRouters, setLANDeviceLANHostConfigManagement_IPRouters, NULL, RPC_RW, eString, 0, 45},
	{ "IPInterfaceNumberOfEntries", getLANDeviceLANHostConfigManagement_IPInterfaceNumberOfEntries, NULL, NULL, RPC_R, eUnsignedInt, 0, 0},
	{ "MACAddress", getLANDeviceLANHostConfigManagement_MACAddress, NULL, NULL, RPC_R, eString, 0, 17},
	{ "DHCPOptionNumberOfEntries", getLANDeviceLANHostConfigManagement_DHCPOptionNumberOfEntries, NULL, NULL, RPC_R, eUnsignedInt, 0, 0},
	{NULL}
};
/**@endparam InternetGatewayDevice.LANDevice.{i}.LANHostConfigManagement.  */

/**@obj InternetGatewayDevice.LANDevice.{i}.LANHostConfigManagement.  */
CPEADDOBJ(addLANDeviceLANHostConfigManagementDHCPConditionalServingPool);
CPEDELOBJ(delLANDeviceLANHostConfigManagementDHCPConditionalServingPool);
CPEADDOBJ(addLANDeviceLANHostConfigManagementDHCPOption);
CPEDELOBJ(delLANDeviceLANHostConfigManagementDHCPOption);
CPEADDOBJ(addLANDeviceLANHostConfigManagementIPInterface);
CPEDELOBJ(delLANDeviceLANHostConfigManagementIPInterface);
CWMPObject LANDeviceLANHostConfigManagement_Objs[]={
	{"DHCPConditionalServingPool", delLANDeviceLANHostConfigManagementDHCPConditionalServingPool, addLANDeviceLANHostConfigManagementDHCPConditionalServingPool, NULL, NULL, LANDeviceLANHostConfigManagementDHCPConditionalServingPool_Objs, LANDeviceLANHostConfigManagementDHCPConditionalServingPool_Params, NULL, eInstance,  0},
	{"DHCPOption", delLANDeviceLANHostConfigManagementDHCPOption, addLANDeviceLANHostConfigManagementDHCPOption, NULL, NULL, NULL, LANDeviceLANHostConfigManagementDHCPOption_Params, NULL, eInstance,  0},
	{"IPInterface", delLANDeviceLANHostConfigManagementIPInterface, addLANDeviceLANHostConfigManagementIPInterface, NULL, NULL, NULL, LANDeviceLANHostConfigManagementIPInterface_Params, NULL, eInstance,  0},
	{NULL}
};

/**@endobj InternetGatewayDevice.LANDevice.{i}.LANHostConfigManagement.  */
/**@param InternetGatewayDevice.LANDevice.{i}.WLANConfiguration.{i}.AssociatedDevice.{i}. */
CPEGETFUNC(getLANDeviceWLANConfigurationAssociatedDevice_AssociatedDeviceMACAddress);
CPEGETFUNC(getLANDeviceWLANConfigurationAssociatedDevice_AssociatedDeviceIPAddress);
CPEGETFUNC(getLANDeviceWLANConfigurationAssociatedDevice_AssociatedDeviceAuthenticationState);
CWMPParam LANDeviceWLANConfigurationAssociatedDevice_Params[]={
	{ "AssociatedDeviceMACAddress", getLANDeviceWLANConfigurationAssociatedDevice_AssociatedDeviceMACAddress, NULL, NULL, RPC_R, eString, NOACTIVENOTIFY, 17},
	{ "AssociatedDeviceIPAddress", getLANDeviceWLANConfigurationAssociatedDevice_AssociatedDeviceIPAddress, NULL, NULL, RPC_R, eString, NOACTIVENOTIFY, 64},
	{ "AssociatedDeviceAuthenticationState", getLANDeviceWLANConfigurationAssociatedDevice_AssociatedDeviceAuthenticationState, NULL, NULL, RPC_R, eBoolean, NOACTIVENOTIFY, 0},
	{NULL}
};
/**@endparam InternetGatewayDevice.LANDevice.{i}.WLANConfiguration.{i}.AssociatedDevice.{i}.  */

/**@param InternetGatewayDevice.LANDevice.{i}.WLANConfiguration.{i}.PreSharedKey.{i}. */
CPEGETFUNC(getLANDeviceWLANConfigurationPreSharedKey_PreSharedKey);
CPESETFUNC(setLANDeviceWLANConfigurationPreSharedKey_PreSharedKey);
CPEGETFUNC(getLANDeviceWLANConfigurationPreSharedKey_KeyPassphrase);
CPESETFUNC(setLANDeviceWLANConfigurationPreSharedKey_KeyPassphrase);
CWMPParam LANDeviceWLANConfigurationPreSharedKey_Params[]={
	{ "PreSharedKey", getLANDeviceWLANConfigurationPreSharedKey_PreSharedKey, setLANDeviceWLANConfigurationPreSharedKey_PreSharedKey, NULL, RPC_RW, eStringSetOnly, 0, 64},
	{ "KeyPassphrase", getLANDeviceWLANConfigurationPreSharedKey_KeyPassphrase, setLANDeviceWLANConfigurationPreSharedKey_KeyPassphrase, NULL, RPC_RW, eStringSetOnly, 0, 63},
	{NULL}
};
/**@endparam InternetGatewayDevice.LANDevice.{i}.WLANConfiguration.{i}.PreSharedKey.{i}.  */

/**@param InternetGatewayDevice.LANDevice.{i}.WLANConfiguration.{i}.Stats. */
CPEGETFUNC(getLANDeviceWLANConfigurationStats_ErrorsSent);
CPEGETFUNC(getLANDeviceWLANConfigurationStats_ErrorsReceived);
CPEGETFUNC(getLANDeviceWLANConfigurationStats_UnicastPacketsSent);
CPEGETFUNC(getLANDeviceWLANConfigurationStats_UnicastPacketsReceived);
CPEGETFUNC(getLANDeviceWLANConfigurationStats_DiscardPacketsSent);
CPEGETFUNC(getLANDeviceWLANConfigurationStats_DiscardPacketsReceived);
CPEGETFUNC(getLANDeviceWLANConfigurationStats_MulticastPacketsSent);
CPEGETFUNC(getLANDeviceWLANConfigurationStats_MulticastPacketsReceived);
CPEGETFUNC(getLANDeviceWLANConfigurationStats_BroadcastPacketsSent);
CPEGETFUNC(getLANDeviceWLANConfigurationStats_BroadcastPacketsReceived);
CPEGETFUNC(getLANDeviceWLANConfigurationStats_UnknownProtoPacketsReceived);
CWMPParam LANDeviceWLANConfigurationStats_Params[]={
	{ "ErrorsSent", getLANDeviceWLANConfigurationStats_ErrorsSent, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "ErrorsReceived", getLANDeviceWLANConfigurationStats_ErrorsReceived, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "UnicastPacketsSent", getLANDeviceWLANConfigurationStats_UnicastPacketsSent, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "UnicastPacketsReceived", getLANDeviceWLANConfigurationStats_UnicastPacketsReceived, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "DiscardPacketsSent", getLANDeviceWLANConfigurationStats_DiscardPacketsSent, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "DiscardPacketsReceived", getLANDeviceWLANConfigurationStats_DiscardPacketsReceived, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "MulticastPacketsSent", getLANDeviceWLANConfigurationStats_MulticastPacketsSent, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "MulticastPacketsReceived", getLANDeviceWLANConfigurationStats_MulticastPacketsReceived, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "BroadcastPacketsSent", getLANDeviceWLANConfigurationStats_BroadcastPacketsSent, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "BroadcastPacketsReceived", getLANDeviceWLANConfigurationStats_BroadcastPacketsReceived, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "UnknownProtoPacketsReceived", getLANDeviceWLANConfigurationStats_UnknownProtoPacketsReceived, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{NULL}
};
/**@endparam InternetGatewayDevice.LANDevice.{i}.WLANConfiguration.{i}.Stats.  */

/**@param InternetGatewayDevice.LANDevice.{i}.WLANConfiguration.{i}.WEPKey.{i}. */
CPEGETFUNC(getLANDeviceWLANConfigurationWEPKey_WEPKey);
CPESETFUNC(setLANDeviceWLANConfigurationWEPKey_WEPKey);
CWMPParam LANDeviceWLANConfigurationWEPKey_Params[]={
	{ "WEPKey", getLANDeviceWLANConfigurationWEPKey_WEPKey, setLANDeviceWLANConfigurationWEPKey_WEPKey, NULL, RPC_RW, eStringSetOnly, 0, 128},
	{NULL}
};
/**@endparam InternetGatewayDevice.LANDevice.{i}.WLANConfiguration.{i}.WEPKey.{i}.  */

/**@param InternetGatewayDevice.LANDevice.{i}.WLANConfiguration.{i}. */
CPEGETFUNC(getLANDeviceWLANConfiguration_Enable);
CPESETFUNC(setLANDeviceWLANConfiguration_Enable);
CPEGETFUNC(getLANDeviceWLANConfiguration_Status);
CPEGETFUNC(getLANDeviceWLANConfiguration_BSSID);
CPEGETFUNC(getLANDeviceWLANConfiguration_MaxBitRate);
CPESETFUNC(setLANDeviceWLANConfiguration_MaxBitRate);
CPEGETFUNC(getLANDeviceWLANConfiguration_Channel);
CPESETFUNC(setLANDeviceWLANConfiguration_Channel);
CPEGETFUNC(getLANDeviceWLANConfiguration_SSID);
CPESETFUNC(setLANDeviceWLANConfiguration_SSID);
CPEGETFUNC(getLANDeviceWLANConfiguration_BeaconType);
CPESETFUNC(setLANDeviceWLANConfiguration_BeaconType);
CPEGETFUNC(getLANDeviceWLANConfiguration_MACAddressControlEnabled);
CPESETFUNC(setLANDeviceWLANConfiguration_MACAddressControlEnabled);
CPEGETFUNC(getLANDeviceWLANConfiguration_Standard);
CPEGETFUNC(getLANDeviceWLANConfiguration_WEPKeyIndex);
CPESETFUNC(setLANDeviceWLANConfiguration_WEPKeyIndex);
CPEGETFUNC(getLANDeviceWLANConfiguration_KeyPassphrase);
CPESETFUNC(setLANDeviceWLANConfiguration_KeyPassphrase);
CPEGETFUNC(getLANDeviceWLANConfiguration_WEPEncryptionLevel);
CPEGETFUNC(getLANDeviceWLANConfiguration_BasicEncryptionModes);
CPESETFUNC(setLANDeviceWLANConfiguration_BasicEncryptionModes);
CPEGETFUNC(getLANDeviceWLANConfiguration_BasicAuthenticationMode);
CPESETFUNC(setLANDeviceWLANConfiguration_BasicAuthenticationMode);
CPEGETFUNC(getLANDeviceWLANConfiguration_WPAEncryptionModes);
CPESETFUNC(setLANDeviceWLANConfiguration_WPAEncryptionModes);
CPEGETFUNC(getLANDeviceWLANConfiguration_WPAAuthenticationMode);
CPESETFUNC(setLANDeviceWLANConfiguration_WPAAuthenticationMode);
CPEGETFUNC(getLANDeviceWLANConfiguration_PossibleChannels);
CPEGETFUNC(getLANDeviceWLANConfiguration_BasicDataTransmitRates);
CPESETFUNC(setLANDeviceWLANConfiguration_BasicDataTransmitRates);
CPEGETFUNC(getLANDeviceWLANConfiguration_OperationalDataTransmitRates);
CPESETFUNC(setLANDeviceWLANConfiguration_OperationalDataTransmitRates);
CPEGETFUNC(getLANDeviceWLANConfiguration_PossibleDataTransmitRates);
CPEGETFUNC(getLANDeviceWLANConfiguration_RadioEnabled);
CPESETFUNC(setLANDeviceWLANConfiguration_RadioEnabled);
CPEGETFUNC(getLANDeviceWLANConfiguration_AutoRateFallBackEnabled);
CPESETFUNC(setLANDeviceWLANConfiguration_AutoRateFallBackEnabled);
CPEGETFUNC(getLANDeviceWLANConfiguration_TotalBytesSent);
CPEGETFUNC(getLANDeviceWLANConfiguration_TotalBytesReceived);
CPEGETFUNC(getLANDeviceWLANConfiguration_TotalPacketsSent);
CPEGETFUNC(getLANDeviceWLANConfiguration_TotalPacketsReceived);
CPEGETFUNC(getLANDeviceWLANConfiguration_TotalAssociations);
CPEGETFUNC(getLANDeviceWLANConfiguration_Name);
CPEGETFUNC(getLANDeviceWLANConfiguration_AutoChannelEnable);
CPESETFUNC(setLANDeviceWLANConfiguration_AutoChannelEnable);
CPEGETFUNC(getLANDeviceWLANConfiguration_SSIDAdvertisementEnabled);
CPESETFUNC(setLANDeviceWLANConfiguration_SSIDAdvertisementEnabled);
CPEGETFUNC(getLANDeviceWLANConfiguration_TransmitPowerSupported);
CPEGETFUNC(getLANDeviceWLANConfiguration_TransmitPower);
CPESETFUNC(setLANDeviceWLANConfiguration_TransmitPower);
CWMPParam LANDeviceWLANConfiguration_Params[]={
	{ "Enable", getLANDeviceWLANConfiguration_Enable, setLANDeviceWLANConfiguration_Enable, NULL, RPC_RW, eBoolean, 0, 0},
	{ "Status", getLANDeviceWLANConfiguration_Status, NULL, NULL, RPC_R, eString, 0, 0},
	{ "BSSID", getLANDeviceWLANConfiguration_BSSID, NULL, NULL, RPC_R, eString, 0, 0},
	{ "MaxBitRate", getLANDeviceWLANConfiguration_MaxBitRate, setLANDeviceWLANConfiguration_MaxBitRate, NULL, RPC_RW, eString, 0, 4},
	{ "Channel", getLANDeviceWLANConfiguration_Channel, setLANDeviceWLANConfiguration_Channel, NULL, RPC_RW, eUnsignedInt, 0, 0},
	{ "SSID", getLANDeviceWLANConfiguration_SSID, setLANDeviceWLANConfiguration_SSID, NULL, RPC_RW, eString, 0, 32},
	{ "BeaconType", getLANDeviceWLANConfiguration_BeaconType, setLANDeviceWLANConfiguration_BeaconType, NULL, RPC_RW, eString, 0, 0},
	{ "MACAddressControlEnabled", getLANDeviceWLANConfiguration_MACAddressControlEnabled, setLANDeviceWLANConfiguration_MACAddressControlEnabled, NULL, RPC_RW, eBoolean, 0, 0},
	{ "Standard", getLANDeviceWLANConfiguration_Standard, NULL, NULL, RPC_R, eString, 0, 0},
	{ "WEPKeyIndex", getLANDeviceWLANConfiguration_WEPKeyIndex, setLANDeviceWLANConfiguration_WEPKeyIndex, NULL, RPC_RW, eUnsignedInt, 0, 0},
	{ "KeyPassphrase", getLANDeviceWLANConfiguration_KeyPassphrase, setLANDeviceWLANConfiguration_KeyPassphrase, NULL, RPC_RW, eStringSetOnly, 0, 63},
	{ "WEPEncryptionLevel", getLANDeviceWLANConfiguration_WEPEncryptionLevel, NULL, NULL, RPC_R, eString, 0, 0},
	{ "BasicEncryptionModes", getLANDeviceWLANConfiguration_BasicEncryptionModes, setLANDeviceWLANConfiguration_BasicEncryptionModes, NULL, RPC_RW, eString, 0, 31},
	{ "BasicAuthenticationMode", getLANDeviceWLANConfiguration_BasicAuthenticationMode, setLANDeviceWLANConfiguration_BasicAuthenticationMode, NULL, RPC_RW, eString, 0, 31},
	{ "WPAEncryptionModes", getLANDeviceWLANConfiguration_WPAEncryptionModes, setLANDeviceWLANConfiguration_WPAEncryptionModes, NULL, RPC_RW, eString, 0, 31},
	{ "WPAAuthenticationMode", getLANDeviceWLANConfiguration_WPAAuthenticationMode, setLANDeviceWLANConfiguration_WPAAuthenticationMode, NULL, RPC_RW, eString, 0, 31},
	{ "PossibleChannels", getLANDeviceWLANConfiguration_PossibleChannels, NULL, NULL, RPC_R, eString, 0, 0},
	{ "BasicDataTransmitRates", getLANDeviceWLANConfiguration_BasicDataTransmitRates, setLANDeviceWLANConfiguration_BasicDataTransmitRates, NULL, RPC_RW, eString, 0, 0},
	{ "OperationalDataTransmitRates", getLANDeviceWLANConfiguration_OperationalDataTransmitRates, setLANDeviceWLANConfiguration_OperationalDataTransmitRates, NULL, RPC_RW, eString, 0, 0},
	{ "PossibleDataTransmitRates", getLANDeviceWLANConfiguration_PossibleDataTransmitRates, NULL, NULL, RPC_R, eString, 0, 0},
	{ "RadioEnabled", getLANDeviceWLANConfiguration_RadioEnabled, setLANDeviceWLANConfiguration_RadioEnabled, NULL, RPC_RW, eBoolean, 0, 0},
	{ "AutoRateFallBackEnabled", getLANDeviceWLANConfiguration_AutoRateFallBackEnabled, setLANDeviceWLANConfiguration_AutoRateFallBackEnabled, NULL, RPC_RW, eBoolean, 0, 0},
	{ "TotalBytesSent", getLANDeviceWLANConfiguration_TotalBytesSent, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "TotalBytesReceived", getLANDeviceWLANConfiguration_TotalBytesReceived, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "TotalPacketsSent", getLANDeviceWLANConfiguration_TotalPacketsSent, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "TotalPacketsReceived", getLANDeviceWLANConfiguration_TotalPacketsReceived, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "TotalAssociations", getLANDeviceWLANConfiguration_TotalAssociations, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "Name", getLANDeviceWLANConfiguration_Name, NULL, NULL, RPC_R, eString, 0, 16},
	{ "AutoChannelEnable", getLANDeviceWLANConfiguration_AutoChannelEnable, setLANDeviceWLANConfiguration_AutoChannelEnable, NULL, RPC_RW, eBoolean, 0, 0},
	{ "SSIDAdvertisementEnabled", getLANDeviceWLANConfiguration_SSIDAdvertisementEnabled, setLANDeviceWLANConfiguration_SSIDAdvertisementEnabled, NULL, RPC_RW, eBoolean, 0, 0},
	{ "TransmitPowerSupported", getLANDeviceWLANConfiguration_TransmitPowerSupported, NULL, NULL, RPC_R, eUnsignedInt, 0, 0},
	{ "TransmitPower", getLANDeviceWLANConfiguration_TransmitPower, setLANDeviceWLANConfiguration_TransmitPower, NULL, RPC_RW, eUnsignedInt, 0, 0},
	{NULL}
};
/**@endparam InternetGatewayDevice.LANDevice.{i}.WLANConfiguration.{i}.  */

/**@obj InternetGatewayDevice.LANDevice.{i}.WLANConfiguration.{i}.  */
CWMPObject LANDeviceWLANConfiguration_Objs[]={
	{"AssociatedDevice", NULL, NULL, NULL, NULL, NULL, LANDeviceWLANConfigurationAssociatedDevice_Params, NULL, eInstance,  0},
	{"PreSharedKey", NULL, NULL, NULL, NULL, NULL, LANDeviceWLANConfigurationPreSharedKey_Params, NULL, eInstance,  0},
	{"Stats", NULL, NULL, NULL, NULL, NULL, LANDeviceWLANConfigurationStats_Params, NULL, eObject,  0},
	{"WEPKey", NULL, NULL, NULL, NULL, NULL, LANDeviceWLANConfigurationWEPKey_Params, NULL, eInstance,  0},
	{NULL}
};

/**@endobj InternetGatewayDevice.LANDevice.{i}.WLANConfiguration.{i}.  */
/**@param InternetGatewayDevice.LANDevice.{i}. */
CPEGETFUNC(getLANDevice_LANEthernetInterfaceNumberOfEntries);
CPEGETFUNC(getLANDevice_LANUSBInterfaceNumberOfEntries);
CPEGETFUNC(getLANDevice_LANWLANConfigurationNumberOfEntries);
CWMPParam LANDevice_Params[]={
	{ "LANEthernetInterfaceNumberOfEntries", getLANDevice_LANEthernetInterfaceNumberOfEntries, NULL, NULL, RPC_R, eUnsignedInt, 0, 0},
	{ "LANUSBInterfaceNumberOfEntries", getLANDevice_LANUSBInterfaceNumberOfEntries, NULL, NULL, RPC_R, eUnsignedInt, 0, 0},
	{ "LANWLANConfigurationNumberOfEntries", getLANDevice_LANWLANConfigurationNumberOfEntries, NULL, NULL, RPC_R, eUnsignedInt, 0, 0},
	{NULL}
};
/**@endparam InternetGatewayDevice.LANDevice.{i}.  */

/**@obj InternetGatewayDevice.LANDevice.{i}.  */
CPEADDOBJ(addLANDeviceLANEthernetInterfaceConfig);
CPEDELOBJ(delLANDeviceLANEthernetInterfaceConfig);
CPECOMMIT(commitLANDeviceLANHostConfigManagement);
CWMPObject LANDevice_Objs[]={
	{"Hosts", NULL, NULL, NULL, NULL, LANDeviceHosts_Objs, LANDeviceHosts_Params, NULL, eObject,  0},
	{"LANEthernetInterfaceConfig", delLANDeviceLANEthernetInterfaceConfig, addLANDeviceLANEthernetInterfaceConfig, NULL, NULL, LANDeviceLANEthernetInterfaceConfig_Objs, LANDeviceLANEthernetInterfaceConfig_Params, NULL, eCPEInstance,  0},
	{"LANHostConfigManagement", NULL, NULL, commitLANDeviceLANHostConfigManagement, NULL, LANDeviceLANHostConfigManagement_Objs, LANDeviceLANHostConfigManagement_Params, NULL, eObject,  0},
	{"WLANConfiguration", NULL, NULL, NULL, NULL, LANDeviceWLANConfiguration_Objs, LANDeviceWLANConfiguration_Params, NULL, eInstance,  0},
	{NULL}
};

/**@endobj InternetGatewayDevice.LANDevice.{i}.  */
/**@param InternetGatewayDevice.LANInterfaces. */
CPEGETFUNC(getLANInterfaces_LANEthernetInterfaceNumberOfEntries);
CPEGETFUNC(getLANInterfaces_LANUSBInterfaceNumberOfEntries);
CPEGETFUNC(getLANInterfaces_LANWLANConfigurationNumberOfEntries);
CWMPParam LANInterfaces_Params[]={
	{ "LANEthernetInterfaceNumberOfEntries", getLANInterfaces_LANEthernetInterfaceNumberOfEntries, NULL, NULL, RPC_R, eUnsignedInt, 0, 0},
	{ "LANUSBInterfaceNumberOfEntries", getLANInterfaces_LANUSBInterfaceNumberOfEntries, NULL, NULL, RPC_R, eUnsignedInt, 0, 0},
	{ "LANWLANConfigurationNumberOfEntries", getLANInterfaces_LANWLANConfigurationNumberOfEntries, NULL, NULL, RPC_R, eUnsignedInt, 0, 0},
	{NULL}
};
/**@endparam InternetGatewayDevice.LANInterfaces.  */

/**@param InternetGatewayDevice.Layer2Bridging.AvailableInterface.{i}. */
CPEGETFUNC(getLayer2BridgingAvailableInterface_AvailableInterfaceKey);
CPEGETFUNC(getLayer2BridgingAvailableInterface_InterfaceType);
CPEGETFUNC(getLayer2BridgingAvailableInterface_InterfaceReference);
CWMPParam Layer2BridgingAvailableInterface_Params[]={
	{ "AvailableInterfaceKey", getLayer2BridgingAvailableInterface_AvailableInterfaceKey, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "InterfaceType", getLayer2BridgingAvailableInterface_InterfaceType, NULL, NULL, RPC_R, eString, NOACTIVENOTIFY, 0},
	{ "InterfaceReference", getLayer2BridgingAvailableInterface_InterfaceReference, NULL, NULL, RPC_R,  eString, NOACTIVENOTIFY, 256},
	{NULL}
};
/**@endparam InternetGatewayDevice.Layer2Bridging.AvailableInterface.{i}.  */

/**@param InternetGatewayDevice.Layer2Bridging.Bridge.{i}. */
CPEGETFUNC(getLayer2BridgingBridge_BridgeKey);
CPEGETFUNC(getLayer2BridgingBridge_BridgeEnable);
CPESETFUNC(setLayer2BridgingBridge_BridgeEnable);
CPEGETFUNC(getLayer2BridgingBridge_BridgeStatus);
CPEGETFUNC(getLayer2BridgingBridge_BridgeName);
CPESETFUNC(setLayer2BridgingBridge_BridgeName);
CPEGETFUNC(getLayer2BridgingBridge_VLANID);
CPESETFUNC(setLayer2BridgingBridge_VLANID);
CPEGETFUNC(getLayer2BridgingBridge_BridgeStandard);
CPESETFUNC(setLayer2BridgingBridge_BridgeStandard);
CWMPParam Layer2BridgingBridge_Params[]={
	{ "BridgeKey", getLayer2BridgingBridge_BridgeKey, NULL, NULL, RPC_R, eUnsignedInt, 0, 0},
	{ "BridgeEnable", getLayer2BridgingBridge_BridgeEnable, setLayer2BridgingBridge_BridgeEnable, NULL, RPC_RW, eBoolean, 0, 0},
	{ "BridgeStatus", getLayer2BridgingBridge_BridgeStatus, NULL, NULL, RPC_R, eString, 0, 0},
	{ "BridgeName", getLayer2BridgingBridge_BridgeName, setLayer2BridgingBridge_BridgeName, NULL, RPC_RW, eString, 0, 64},
	{ "VLANID", getLayer2BridgingBridge_VLANID, setLayer2BridgingBridge_VLANID, NULL, RPC_RW, eUnsignedInt, 0, 0},
	{ "BridgeStandard", getLayer2BridgingBridge_BridgeStandard, setLayer2BridgingBridge_BridgeStandard, NULL, RPC_RW, eString, 0, 0},
	{NULL}
};
/**@endparam InternetGatewayDevice.Layer2Bridging.Bridge.{i}.  */

/**@param InternetGatewayDevice.Layer2Bridging.Filter.{i}. */
CPEGETFUNC(getLayer2BridgingFilter_FilterKey);
CPEGETFUNC(getLayer2BridgingFilter_FilterEnable);
CPESETFUNC(setLayer2BridgingFilter_FilterEnable);
CPEGETFUNC(getLayer2BridgingFilter_FilterStatus);
CPEGETFUNC(getLayer2BridgingFilter_FilterBridgeReference);
CPESETFUNC(setLayer2BridgingFilter_FilterBridgeReference);
CPEGETFUNC(getLayer2BridgingFilter_ExclusivityOrder);
CPESETFUNC(setLayer2BridgingFilter_ExclusivityOrder);
CPEGETFUNC(getLayer2BridgingFilter_FilterInterface);
CPESETFUNC(setLayer2BridgingFilter_FilterInterface);
CPEGETFUNC(getLayer2BridgingFilter_VLANIDFilter);
CPESETFUNC(setLayer2BridgingFilter_VLANIDFilter);
CPEGETFUNC(getLayer2BridgingFilter_AdmitOnlyVLANTagged);
CPESETFUNC(setLayer2BridgingFilter_AdmitOnlyVLANTagged);
CPEGETFUNC(getLayer2BridgingFilter_EthertypeFilterList);
CPESETFUNC(setLayer2BridgingFilter_EthertypeFilterList);
CPEGETFUNC(getLayer2BridgingFilter_EthertypeFilterExclude);
CPESETFUNC(setLayer2BridgingFilter_EthertypeFilterExclude);
CPEGETFUNC(getLayer2BridgingFilter_SourceMACAddressFilterList);
CPESETFUNC(setLayer2BridgingFilter_SourceMACAddressFilterList);
CPEGETFUNC(getLayer2BridgingFilter_SourceMACAddressFilterExclude);
CPESETFUNC(setLayer2BridgingFilter_SourceMACAddressFilterExclude);
CPEGETFUNC(getLayer2BridgingFilter_DestMACAddressFilterList);
CPESETFUNC(setLayer2BridgingFilter_DestMACAddressFilterList);
CPEGETFUNC(getLayer2BridgingFilter_DestMACAddressFilterExclude);
CPESETFUNC(setLayer2BridgingFilter_DestMACAddressFilterExclude);
CWMPParam Layer2BridgingFilter_Params[]={
	{ "FilterKey", getLayer2BridgingFilter_FilterKey, NULL, NULL, RPC_R, eUnsignedInt, 0, 0},
	{ "FilterEnable", getLayer2BridgingFilter_FilterEnable, setLayer2BridgingFilter_FilterEnable, NULL, RPC_RW, eBoolean, 0, 0},
	{ "FilterStatus", getLayer2BridgingFilter_FilterStatus, NULL, NULL, RPC_R, eString, 0, 0},
	{ "FilterBridgeReference", getLayer2BridgingFilter_FilterBridgeReference, setLayer2BridgingFilter_FilterBridgeReference, NULL, RPC_RW, eInt, 0, 0},
	{ "ExclusivityOrder", getLayer2BridgingFilter_ExclusivityOrder, setLayer2BridgingFilter_ExclusivityOrder, NULL, RPC_RW, eUnsignedInt, 0, 0},
	{ "FilterInterface", getLayer2BridgingFilter_FilterInterface, setLayer2BridgingFilter_FilterInterface, NULL, RPC_RW, eString, 0, 16},
	{ "VLANIDFilter", getLayer2BridgingFilter_VLANIDFilter, setLayer2BridgingFilter_VLANIDFilter, NULL, RPC_RW, eInt, 0, 0},
	{ "AdmitOnlyVLANTagged", getLayer2BridgingFilter_AdmitOnlyVLANTagged, setLayer2BridgingFilter_AdmitOnlyVLANTagged, NULL, RPC_RW, eBoolean, 0, 0},
	{ "EthertypeFilterList", getLayer2BridgingFilter_EthertypeFilterList, setLayer2BridgingFilter_EthertypeFilterList, NULL, RPC_RW, eUnsignedInt, 0, 0},
	{ "EthertypeFilterExclude", getLayer2BridgingFilter_EthertypeFilterExclude, setLayer2BridgingFilter_EthertypeFilterExclude, NULL, RPC_RW, eBoolean, 0, 0},
	{ "SourceMACAddressFilterList", getLayer2BridgingFilter_SourceMACAddressFilterList, setLayer2BridgingFilter_SourceMACAddressFilterList, NULL, RPC_RW, eString, 0, 0},
	{ "SourceMACAddressFilterExclude", getLayer2BridgingFilter_SourceMACAddressFilterExclude, setLayer2BridgingFilter_SourceMACAddressFilterExclude, NULL, RPC_RW, eBoolean, 0, 0},
	{ "DestMACAddressFilterList", getLayer2BridgingFilter_DestMACAddressFilterList, setLayer2BridgingFilter_DestMACAddressFilterList, NULL, RPC_RW, eString, 0, 0},
	{ "DestMACAddressFilterExclude", getLayer2BridgingFilter_DestMACAddressFilterExclude, setLayer2BridgingFilter_DestMACAddressFilterExclude, NULL, RPC_RW, eBoolean, 0, 0},
	{NULL}
};
/**@endparam InternetGatewayDevice.Layer2Bridging.Filter.{i}.  */

/**@param InternetGatewayDevice.Layer2Bridging.Marking.{i}. */
CPEGETFUNC(getLayer2BridgingMarking_MarkingKey);
CPEGETFUNC(getLayer2BridgingMarking_MarkingEnable);
CPESETFUNC(setLayer2BridgingMarking_MarkingEnable);
CPEGETFUNC(getLayer2BridgingMarking_MarkingStatus);
CPEGETFUNC(getLayer2BridgingMarking_MarkingBridgeReference);
CPESETFUNC(setLayer2BridgingMarking_MarkingBridgeReference);
CPEGETFUNC(getLayer2BridgingMarking_MarkingInterface);
CPESETFUNC(setLayer2BridgingMarking_MarkingInterface);
CPEGETFUNC(getLayer2BridgingMarking_VLANIDUntag);
CPESETFUNC(setLayer2BridgingMarking_VLANIDUntag);
CPEGETFUNC(getLayer2BridgingMarking_VLANIDMark);
CPESETFUNC(setLayer2BridgingMarking_VLANIDMark);
CPEGETFUNC(getLayer2BridgingMarking_EthernetPriorityMark);
CPESETFUNC(setLayer2BridgingMarking_EthernetPriorityMark);
CPEGETFUNC(getLayer2BridgingMarking_EthernetPriorityOverride);
CPESETFUNC(setLayer2BridgingMarking_EthernetPriorityOverride);
CPEGETFUNC(getLayer2BridgingMarking_VLANIDMarkOverride);
CPESETFUNC(setLayer2BridgingMarking_VLANIDMarkOverride);
CWMPParam Layer2BridgingMarking_Params[]={
	{ "MarkingKey", getLayer2BridgingMarking_MarkingKey, NULL, NULL, RPC_R, eUnsignedInt, 0, 0},
	{ "MarkingEnable", getLayer2BridgingMarking_MarkingEnable, setLayer2BridgingMarking_MarkingEnable, NULL, RPC_RW, eBoolean, 0, 0},
	{ "MarkingStatus", getLayer2BridgingMarking_MarkingStatus, NULL, NULL, RPC_R, eString, 0, 0},
	{ "MarkingBridgeReference", getLayer2BridgingMarking_MarkingBridgeReference, setLayer2BridgingMarking_MarkingBridgeReference, NULL, RPC_RW, eInt, 0, 0},
	{ "MarkingInterface", getLayer2BridgingMarking_MarkingInterface, setLayer2BridgingMarking_MarkingInterface, NULL, RPC_RW, eString, 0, 16},
	{ "VLANIDUntag", getLayer2BridgingMarking_VLANIDUntag, setLayer2BridgingMarking_VLANIDUntag, NULL, RPC_RW, eBoolean, 0, 0},
	{ "VLANIDMark", getLayer2BridgingMarking_VLANIDMark, setLayer2BridgingMarking_VLANIDMark, NULL, RPC_RW, eInt, 0, 0},
	{ "EthernetPriorityMark", getLayer2BridgingMarking_EthernetPriorityMark, setLayer2BridgingMarking_EthernetPriorityMark, NULL, RPC_RW, eInt, 0, 0},
	{ "EthernetPriorityOverride", getLayer2BridgingMarking_EthernetPriorityOverride, setLayer2BridgingMarking_EthernetPriorityOverride, NULL, RPC_RW, eBoolean, 0, 0},
	{ "VLANIDMarkOverride", getLayer2BridgingMarking_VLANIDMarkOverride, setLayer2BridgingMarking_VLANIDMarkOverride, NULL, RPC_RW, eBoolean, 0, 0},
	{NULL}
};
/**@endparam InternetGatewayDevice.Layer2Bridging.Marking.{i}.  */

/**@param InternetGatewayDevice.Layer2Bridging. */
CPEGETFUNC(getLayer2Bridging_MaxBridgeEntries);
CPEGETFUNC(getLayer2Bridging_MaxFilterEntries);
CPEGETFUNC(getLayer2Bridging_MaxMarkingEntries);
CPEGETFUNC(getLayer2Bridging_BridgeNumberOfEntries);
CPEGETFUNC(getLayer2Bridging_FilterNumberOfEntries);
CPEGETFUNC(getLayer2Bridging_MarkingNumberOfEntries);
CPEGETFUNC(getLayer2Bridging_AvailableInterfaceNumberOfEntries);
CPEGETFUNC(getLayer2Bridging_MaxDBridgeEntries);
CPEGETFUNC(getLayer2Bridging_MaxQBridgeEntries);
CWMPParam Layer2Bridging_Params[]={
	{ "MaxBridgeEntries", getLayer2Bridging_MaxBridgeEntries, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "MaxFilterEntries", getLayer2Bridging_MaxFilterEntries, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "MaxMarkingEntries", getLayer2Bridging_MaxMarkingEntries, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "BridgeNumberOfEntries", getLayer2Bridging_BridgeNumberOfEntries, NULL, NULL, RPC_R, eUnsignedInt, 0, 0},
	{ "FilterNumberOfEntries", getLayer2Bridging_FilterNumberOfEntries, NULL, NULL, RPC_R, eUnsignedInt, 0, 0},
	{ "MarkingNumberOfEntries", getLayer2Bridging_MarkingNumberOfEntries, NULL, NULL, RPC_R, eUnsignedInt, 0, 0},
	{ "AvailableInterfaceNumberOfEntries", getLayer2Bridging_AvailableInterfaceNumberOfEntries, NULL, NULL, RPC_R, eUnsignedInt, 0, 0},
	{ "MaxDBridgeEntries", getLayer2Bridging_MaxDBridgeEntries, NULL, NULL, RPC_R, eUnsignedInt, 0, 0},
	{ "MaxQBridgeEntries", getLayer2Bridging_MaxQBridgeEntries, NULL, NULL, RPC_R, eUnsignedInt, 0, 0},
	{NULL}
};
/**@endparam InternetGatewayDevice.Layer2Bridging.  */

/**@obj InternetGatewayDevice.Layer2Bridging.  */
CPEADDOBJ(addLayer2BridgingBridge);
CPEDELOBJ(delLayer2BridgingBridge);
CPEADDOBJ(addLayer2BridgingFilter);
CPEDELOBJ(delLayer2BridgingFilter);
CPEADDOBJ(addLayer2BridgingMarking);
CPEDELOBJ(delLayer2BridgingMarking);
CWMPObject Layer2Bridging_Objs[]={
	{"AvailableInterface", NULL, NULL, NULL, NULL, NULL, Layer2BridgingAvailableInterface_Params, NULL, eInstance,  0},
	{"Bridge", delLayer2BridgingBridge, addLayer2BridgingBridge, NULL, NULL, NULL, Layer2BridgingBridge_Params, NULL, eInstance,  0},
	{"Filter", delLayer2BridgingFilter, addLayer2BridgingFilter, NULL, NULL, NULL, Layer2BridgingFilter_Params, NULL, eInstance,  0},
	{"Marking", delLayer2BridgingMarking, addLayer2BridgingMarking, NULL, NULL, NULL, Layer2BridgingMarking_Params, NULL, eInstance,  0},
	{NULL}
};

/**@endobj InternetGatewayDevice.Layer2Bridging.  */
/**@param InternetGatewayDevice.Layer3Forwarding.Forwarding.{i}. */
CPEGETFUNC(getLayer3ForwardingForwarding_Enable);
CPESETFUNC(setLayer3ForwardingForwarding_Enable);
CPEGETFUNC(getLayer3ForwardingForwarding_Status);
CPEGETFUNC(getLayer3ForwardingForwarding_Type);
CPESETFUNC(setLayer3ForwardingForwarding_Type);
CPEGETFUNC(getLayer3ForwardingForwarding_DestIPAddress);
CPESETFUNC(setLayer3ForwardingForwarding_DestIPAddress);
CPEGETFUNC(getLayer3ForwardingForwarding_DestSubnetMask);
CPESETFUNC(setLayer3ForwardingForwarding_DestSubnetMask);
CPEGETFUNC(getLayer3ForwardingForwarding_SourceIPAddress);
CPESETFUNC(setLayer3ForwardingForwarding_SourceIPAddress);
CPEGETFUNC(getLayer3ForwardingForwarding_SourceSubnetMask);
CPESETFUNC(setLayer3ForwardingForwarding_SourceSubnetMask);
CPEGETFUNC(getLayer3ForwardingForwarding_GatewayIPAddress);
CPESETFUNC(setLayer3ForwardingForwarding_GatewayIPAddress);
CPEGETFUNC(getLayer3ForwardingForwarding_Interface);
CPESETFUNC(setLayer3ForwardingForwarding_Interface);
CPEGETFUNC(getLayer3ForwardingForwarding_ForwardingMetric);
CPESETFUNC(setLayer3ForwardingForwarding_ForwardingMetric);
CPEGETFUNC(getLayer3ForwardingForwarding_StaticRoute);
CWMPParam Layer3ForwardingForwarding_Params[]={
	{ "Enable", getLayer3ForwardingForwarding_Enable, setLayer3ForwardingForwarding_Enable, NULL, RPC_RW, eBoolean, 0, 0},
	{ "Status", getLayer3ForwardingForwarding_Status, NULL, NULL, RPC_R, eString, 0, 0},
	{ "Type", getLayer3ForwardingForwarding_Type, setLayer3ForwardingForwarding_Type, NULL, RPC_RW, eString, 0, 0},
	{ "DestIPAddress", getLayer3ForwardingForwarding_DestIPAddress, setLayer3ForwardingForwarding_DestIPAddress, NULL, RPC_RW, eString, 0, 45},
	{ "DestSubnetMask", getLayer3ForwardingForwarding_DestSubnetMask, setLayer3ForwardingForwarding_DestSubnetMask, NULL, RPC_RW, eString, 0, 45},
	{ "SourceIPAddress", getLayer3ForwardingForwarding_SourceIPAddress, setLayer3ForwardingForwarding_SourceIPAddress, NULL, RPC_RW, eString, 0, 45},
	{ "SourceSubnetMask", getLayer3ForwardingForwarding_SourceSubnetMask, setLayer3ForwardingForwarding_SourceSubnetMask, NULL, RPC_RW, eString, 0, 45},
	{ "GatewayIPAddress", getLayer3ForwardingForwarding_GatewayIPAddress, setLayer3ForwardingForwarding_GatewayIPAddress, NULL, RPC_RW, eString, 0, 45},
	{ "Interface", getLayer3ForwardingForwarding_Interface, setLayer3ForwardingForwarding_Interface, NULL, RPC_RW,  eString, 0, 0},
	{ "ForwardingMetric", getLayer3ForwardingForwarding_ForwardingMetric, setLayer3ForwardingForwarding_ForwardingMetric, NULL, RPC_RW, eInt, 0, 0},
	{ "StaticRoute", getLayer3ForwardingForwarding_StaticRoute, NULL, NULL, RPC_R, eBoolean, 0, 0},
	{NULL}
};
/**@endparam InternetGatewayDevice.Layer3Forwarding.Forwarding.{i}.  */

/**@param InternetGatewayDevice.Layer3Forwarding. */
CPEGETFUNC(getLayer3Forwarding_DefaultConnectionService);
CPESETFUNC(setLayer3Forwarding_DefaultConnectionService);
CPEGETFUNC(getLayer3Forwarding_ForwardNumberOfEntries);
CWMPParam Layer3Forwarding_Params[]={
	{ "DefaultConnectionService", getLayer3Forwarding_DefaultConnectionService, setLayer3Forwarding_DefaultConnectionService, NULL, RPC_RW, eString, 0, 256},
	{ "ForwardNumberOfEntries", getLayer3Forwarding_ForwardNumberOfEntries, NULL, NULL, RPC_R, eUnsignedInt, 0, 0},
	{NULL}
};
/**@endparam InternetGatewayDevice.Layer3Forwarding.  */

/**@obj InternetGatewayDevice.Layer3Forwarding.  */
CPEADDOBJ(addLayer3ForwardingForwarding);
CPEDELOBJ(delLayer3ForwardingForwarding);
CPECOMMIT(commitLayer3ForwardingForwarding);
CWMPObject Layer3Forwarding_Objs[]={
	{"Forwarding", delLayer3ForwardingForwarding, addLayer3ForwardingForwarding, commitLayer3ForwardingForwarding, NULL, NULL, Layer3ForwardingForwarding_Params, NULL, eInstance,  0},
	{NULL}
};

/**@endobj InternetGatewayDevice.Layer3Forwarding.  */
/**@param InternetGatewayDevice.ManagementServer.ManageableDevice.{i}. */
CPEGETFUNC(getManagementServerManageableDevice_ManufacturerOUI);
CPEGETFUNC(getManagementServerManageableDevice_SerialNumber);
CPEGETFUNC(getManagementServerManageableDevice_ProductClass);
CPEGETFUNC(getManagementServerManageableDevice_Host);
CWMPParam ManagementServerManageableDevice_Params[]={
	{ "ManufacturerOUI", getManagementServerManageableDevice_ManufacturerOUI, NULL, NULL, RPC_R, eString, NOACTIVENOTIFY, 6},
	{ "SerialNumber", getManagementServerManageableDevice_SerialNumber, NULL, NULL, RPC_R, eString, NOACTIVENOTIFY, 64},
	{ "ProductClass", getManagementServerManageableDevice_ProductClass, NULL, NULL, RPC_R, eString, NOACTIVENOTIFY, 64},
	{ "Host", getManagementServerManageableDevice_Host, NULL, NULL, RPC_R,  eString, 0, 0},
	{NULL}
};
/**@endparam InternetGatewayDevice.ManagementServer.ManageableDevice.{i}.  */

/**@param InternetGatewayDevice.ManagementServer. */
CPEGETFUNC(getManagementServer_URL);
CPESETFUNC(setManagementServer_URL);
CPEGETFUNC(getManagementServer_Username);
CPESETFUNC(setManagementServer_Username);
CPEGETFUNC(getManagementServer_Password);
CPESETFUNC(setManagementServer_Password);
CPEGETFUNC(getManagementServer_PeriodicInformEnable);
CPESETFUNC(setManagementServer_PeriodicInformEnable);
CPEGETFUNC(getManagementServer_PeriodicInformInterval);
CPESETFUNC(setManagementServer_PeriodicInformInterval);
CPEGETFUNC(getManagementServer_PeriodicInformTime);
CPESETFUNC(setManagementServer_PeriodicInformTime);
CPEGETFUNC(getManagementServer_ParameterKey);
CPEGETFUNC(getManagementServer_ConnectionRequestURL);
CPEGETFUNC(getManagementServer_ConnectionRequestUsername);
CPESETFUNC(setManagementServer_ConnectionRequestUsername);
CPEGETFUNC(getManagementServer_ConnectionRequestPassword);
CPESETFUNC(setManagementServer_ConnectionRequestPassword);
CPEGETFUNC(getManagementServer_UpgradesManaged);
CPESETFUNC(setManagementServer_UpgradesManaged);
CPEGETFUNC(getManagementServer_UDPConnectionRequestAddress);
CPEGETFUNC(getManagementServer_UDPConnectionRequestAddressNotificationLimit);
CPESETFUNC(setManagementServer_UDPConnectionRequestAddressNotificationLimit);
CPEGETFUNC(getManagementServer_STUNEnable);
CPESETFUNC(setManagementServer_STUNEnable);
CPEGETFUNC(getManagementServer_STUNServerAddress);
CPESETFUNC(setManagementServer_STUNServerAddress);
CPEGETFUNC(getManagementServer_STUNServerPort);
CPESETFUNC(setManagementServer_STUNServerPort);
CPEGETFUNC(getManagementServer_STUNUsername);
CPESETFUNC(setManagementServer_STUNUsername);
CPEGETFUNC(getManagementServer_STUNPassword);
CPESETFUNC(setManagementServer_STUNPassword);
CPEGETFUNC(getManagementServer_STUNMaximumKeepAlivePeriod);
CPESETFUNC(setManagementServer_STUNMaximumKeepAlivePeriod);
CPEGETFUNC(getManagementServer_STUNMinimumKeepAlivePeriod);
CPESETFUNC(setManagementServer_STUNMinimumKeepAlivePeriod);
CPEGETFUNC(getManagementServer_NATDetected);
CPEGETFUNC(getManagementServer_ManageableDeviceNumberOfEntries);
CWMPParam ManagementServer_Params[]={
	{ "URL", getManagementServer_URL, setManagementServer_URL, NULL, RPC_RW, eString, 0, 256},
	{ "Username", getManagementServer_Username, setManagementServer_Username, NULL, RPC_RW, eString, 0, 256},
	{ "Password", getManagementServer_Password, setManagementServer_Password, NULL, RPC_RW, eStringSetOnly, 0, 256},
	{ "PeriodicInformEnable", getManagementServer_PeriodicInformEnable, setManagementServer_PeriodicInformEnable, NULL, RPC_RW, eBoolean, 0, 0},
	{ "PeriodicInformInterval", getManagementServer_PeriodicInformInterval, setManagementServer_PeriodicInformInterval, NULL, RPC_RW, eUnsignedInt, 0, 0},
	{ "PeriodicInformTime", getManagementServer_PeriodicInformTime, setManagementServer_PeriodicInformTime, NULL, RPC_RW, eDateTime, 0, 0},
	{ "ParameterKey", getManagementServer_ParameterKey, NULL, NULL, RPC_R, eString, FORCED_INFORM|NOACTIVENOTIFY, 32},
	{ "ConnectionRequestURL", getManagementServer_ConnectionRequestURL, NULL, NULL, RPC_R, eString, FORCED_INFORM|DEFAULT_ACTIVE, 256},
	{ "ConnectionRequestUsername", getManagementServer_ConnectionRequestUsername, setManagementServer_ConnectionRequestUsername, NULL, RPC_RW, eString, 0, 256},
	{ "ConnectionRequestPassword", getManagementServer_ConnectionRequestPassword, setManagementServer_ConnectionRequestPassword, NULL, RPC_RW, eStringSetOnly, 0, 256},
	{ "UpgradesManaged", getManagementServer_UpgradesManaged, setManagementServer_UpgradesManaged, NULL, RPC_RW, eBoolean, 0, 0},
	{ "UDPConnectionRequestAddress", getManagementServer_UDPConnectionRequestAddress, NULL, NULL, RPC_R, eString, 0, 256},
	{ "UDPConnectionRequestAddressNotificationLimit", getManagementServer_UDPConnectionRequestAddressNotificationLimit, setManagementServer_UDPConnectionRequestAddressNotificationLimit, NULL, RPC_RW, eUnsignedInt, 0, 0},
	{ "STUNEnable", getManagementServer_STUNEnable, setManagementServer_STUNEnable, NULL, RPC_RW, eBoolean, 0, 0},
	{ "STUNServerAddress", getManagementServer_STUNServerAddress, setManagementServer_STUNServerAddress, NULL, RPC_RW, eString, 0, 256},
	{ "STUNServerPort", getManagementServer_STUNServerPort, setManagementServer_STUNServerPort, NULL, RPC_RW, eUnsignedInt, 0, 0},
	{ "STUNUsername", getManagementServer_STUNUsername, setManagementServer_STUNUsername, NULL, RPC_RW, eString, 0, 256},
	{ "STUNPassword", getManagementServer_STUNPassword, setManagementServer_STUNPassword, NULL, RPC_RW, eStringSetOnly, 0, 256},
	{ "STUNMaximumKeepAlivePeriod", getManagementServer_STUNMaximumKeepAlivePeriod, setManagementServer_STUNMaximumKeepAlivePeriod, NULL, RPC_RW, eInt, 0, 0},
	{ "STUNMinimumKeepAlivePeriod", getManagementServer_STUNMinimumKeepAlivePeriod, setManagementServer_STUNMinimumKeepAlivePeriod, NULL, RPC_RW, eUnsignedInt, 0, 0},
	{ "NATDetected", getManagementServer_NATDetected, NULL, NULL, RPC_R, eBoolean, 0, 0},
	{ "ManageableDeviceNumberOfEntries", getManagementServer_ManageableDeviceNumberOfEntries, NULL, NULL, RPC_R, eUnsignedInt, 0, 0},
	{NULL}
};
/**@endparam InternetGatewayDevice.ManagementServer.  */

/**@obj InternetGatewayDevice.ManagementServer.  */
CWMPObject ManagementServer_Objs[]={
	{"ManageableDevice", NULL, NULL, NULL, NULL, NULL, ManagementServerManageableDevice_Params, NULL, eInstance,  0},
	{NULL}
};

/**@endobj InternetGatewayDevice.ManagementServer.  */
/**@param InternetGatewayDevice.Time. */
CPEGETFUNC(getTime_NTPServer1);
CPESETFUNC(setTime_NTPServer1);
CPEGETFUNC(getTime_NTPServer2);
CPESETFUNC(setTime_NTPServer2);
CPEGETFUNC(getTime_CurrentLocalTime);
CPEGETFUNC(getTime_LocalTimeZoneName);
CPESETFUNC(setTime_LocalTimeZoneName);
CPEGETFUNC(getTime_Enable);
CPESETFUNC(setTime_Enable);
CPEGETFUNC(getTime_Status);
CWMPParam Time_Params[]={
	{ "NTPServer1", getTime_NTPServer1, setTime_NTPServer1, NULL, RPC_RW, eString, 0, 64},
	{ "NTPServer2", getTime_NTPServer2, setTime_NTPServer2, NULL, RPC_RW, eString, 0, 64},
	{ "CurrentLocalTime", getTime_CurrentLocalTime, NULL, NULL, RPC_R, eDateTime, NOACTIVENOTIFY, 0},
	{ "LocalTimeZoneName", getTime_LocalTimeZoneName, setTime_LocalTimeZoneName, NULL, RPC_RW, eString, 0, 64},
	{ "Enable", getTime_Enable, setTime_Enable, NULL, RPC_RW, eBoolean, 0, 0},
	{ "Status", getTime_Status, NULL, NULL, RPC_R, eString, 0, 0},
	{NULL}
};
/**@endparam InternetGatewayDevice.Time.  */

/**@param InternetGatewayDevice.TraceRouteDiagnostics.RouteHops.{i}. */
CPEGETFUNC(getTraceRouteDiagnosticsRouteHops_HopHost);
CPEGETFUNC(getTraceRouteDiagnosticsRouteHops_HopHostAddress);
CPEGETFUNC(getTraceRouteDiagnosticsRouteHops_HopErrorCode);
CPEGETFUNC(getTraceRouteDiagnosticsRouteHops_HopRTTimes);
CWMPParam TraceRouteDiagnosticsRouteHops_Params[]={
	{ "HopHost", getTraceRouteDiagnosticsRouteHops_HopHost, NULL, NULL, RPC_R, eString, NOACTIVENOTIFY, 256},
	{ "HopHostAddress", getTraceRouteDiagnosticsRouteHops_HopHostAddress, NULL, NULL, RPC_R, eString, NOACTIVENOTIFY, 0},
	{ "HopErrorCode", getTraceRouteDiagnosticsRouteHops_HopErrorCode, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "HopRTTimes", getTraceRouteDiagnosticsRouteHops_HopRTTimes, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{NULL}
};
/**@endparam InternetGatewayDevice.TraceRouteDiagnostics.RouteHops.{i}.  */

/**@param InternetGatewayDevice.TraceRouteDiagnostics. */
CPEGETFUNC(getTraceRouteDiagnostics_DiagnosticsState);
CPESETFUNC(setTraceRouteDiagnostics_DiagnosticsState);
CPEGETFUNC(getTraceRouteDiagnostics_Interface);
CPESETFUNC(setTraceRouteDiagnostics_Interface);
CPEGETFUNC(getTraceRouteDiagnostics_Host);
CPESETFUNC(setTraceRouteDiagnostics_Host);
CPEGETFUNC(getTraceRouteDiagnostics_NumberOfTries);
CPESETFUNC(setTraceRouteDiagnostics_NumberOfTries);
CPEGETFUNC(getTraceRouteDiagnostics_Timeout);
CPESETFUNC(setTraceRouteDiagnostics_Timeout);
CPEGETFUNC(getTraceRouteDiagnostics_DataBlockSize);
CPESETFUNC(setTraceRouteDiagnostics_DataBlockSize);
CPEGETFUNC(getTraceRouteDiagnostics_DSCP);
CPESETFUNC(setTraceRouteDiagnostics_DSCP);
CPEGETFUNC(getTraceRouteDiagnostics_MaxHopCount);
CPESETFUNC(setTraceRouteDiagnostics_MaxHopCount);
CPEGETFUNC(getTraceRouteDiagnostics_ResponseTime);
CPEGETFUNC(getTraceRouteDiagnostics_RouteHopsNumberOfEntries);
CWMPParam TraceRouteDiagnostics_Params[]={
	{ "DiagnosticsState", getTraceRouteDiagnostics_DiagnosticsState, setTraceRouteDiagnostics_DiagnosticsState, NULL, RPC_RW, eString, NOACTIVENOTIFY, 0},
	{ "Interface", getTraceRouteDiagnostics_Interface, setTraceRouteDiagnostics_Interface, NULL, RPC_RW,  eString, 0, 256},
	{ "Host", getTraceRouteDiagnostics_Host, setTraceRouteDiagnostics_Host, NULL, RPC_RW, eString, 0, 256},
	{ "NumberOfTries", getTraceRouteDiagnostics_NumberOfTries, setTraceRouteDiagnostics_NumberOfTries, NULL, RPC_RW, eUnsignedInt, 0, 0},
	{ "Timeout", getTraceRouteDiagnostics_Timeout, setTraceRouteDiagnostics_Timeout, NULL, RPC_RW, eUnsignedInt, 0, 0},
	{ "DataBlockSize", getTraceRouteDiagnostics_DataBlockSize, setTraceRouteDiagnostics_DataBlockSize, NULL, RPC_RW, eUnsignedInt, 0, 0},
	{ "DSCP", getTraceRouteDiagnostics_DSCP, setTraceRouteDiagnostics_DSCP, NULL, RPC_RW, eUnsignedInt, 0, 0},
	{ "MaxHopCount", getTraceRouteDiagnostics_MaxHopCount, setTraceRouteDiagnostics_MaxHopCount, NULL, RPC_RW, eUnsignedInt, 0, 0},
	{ "ResponseTime", getTraceRouteDiagnostics_ResponseTime, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "RouteHopsNumberOfEntries", getTraceRouteDiagnostics_RouteHopsNumberOfEntries, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{NULL}
};
/**@endparam InternetGatewayDevice.TraceRouteDiagnostics.  */

/**@obj InternetGatewayDevice.TraceRouteDiagnostics.  */
CWMPObject TraceRouteDiagnostics_Objs[]={
	{"RouteHops", NULL, NULL, NULL, NULL, NULL, TraceRouteDiagnosticsRouteHops_Params, NULL, eInstance,  0},
	{NULL}
};

/**@endobj InternetGatewayDevice.TraceRouteDiagnostics.  */
/**@param InternetGatewayDevice.WANDevice.{i}.WANCommonInterfaceConfig. */
CPEGETFUNC(getWANDeviceWANCommonInterfaceConfig_EnabledForInternet);
CPESETFUNC(setWANDeviceWANCommonInterfaceConfig_EnabledForInternet);
CPEGETFUNC(getWANDeviceWANCommonInterfaceConfig_WANAccessType);
CPEGETFUNC(getWANDeviceWANCommonInterfaceConfig_Layer1UpstreamMaxBitRate);
CPEGETFUNC(getWANDeviceWANCommonInterfaceConfig_Layer1DownstreamMaxBitRate);
CPEGETFUNC(getWANDeviceWANCommonInterfaceConfig_PhysicalLinkStatus);
CPEGETFUNC(getWANDeviceWANCommonInterfaceConfig_TotalBytesSent);
CPEGETFUNC(getWANDeviceWANCommonInterfaceConfig_TotalBytesReceived);
CPEGETFUNC(getWANDeviceWANCommonInterfaceConfig_TotalPacketsSent);
CPEGETFUNC(getWANDeviceWANCommonInterfaceConfig_TotalPacketsReceived);
CWMPParam WANDeviceWANCommonInterfaceConfig_Params[]={
	{ "EnabledForInternet", getWANDeviceWANCommonInterfaceConfig_EnabledForInternet, setWANDeviceWANCommonInterfaceConfig_EnabledForInternet, NULL, RPC_RW, eBoolean, 0, 0},
	{ "WANAccessType", getWANDeviceWANCommonInterfaceConfig_WANAccessType, NULL, NULL, RPC_R, eString, 0, 0},
	{ "Layer1UpstreamMaxBitRate", getWANDeviceWANCommonInterfaceConfig_Layer1UpstreamMaxBitRate, NULL, NULL, RPC_R, eUnsignedInt, 0, 0},
	{ "Layer1DownstreamMaxBitRate", getWANDeviceWANCommonInterfaceConfig_Layer1DownstreamMaxBitRate, NULL, NULL, RPC_R, eUnsignedInt, 0, 0},
	{ "PhysicalLinkStatus", getWANDeviceWANCommonInterfaceConfig_PhysicalLinkStatus, NULL, NULL, RPC_R, eString, 0, 0},
	{ "TotalBytesSent", getWANDeviceWANCommonInterfaceConfig_TotalBytesSent, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "TotalBytesReceived", getWANDeviceWANCommonInterfaceConfig_TotalBytesReceived, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "TotalPacketsSent", getWANDeviceWANCommonInterfaceConfig_TotalPacketsSent, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "TotalPacketsReceived", getWANDeviceWANCommonInterfaceConfig_TotalPacketsReceived, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{NULL}
};
/**@endparam InternetGatewayDevice.WANDevice.{i}.WANCommonInterfaceConfig.  */

/**@param InternetGatewayDevice.WANDevice.{i}.WANConnectionDevice.{i}.WANDSLLinkConfig. */
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANDSLLinkConfig_Enable);
CPESETFUNC(setWANDeviceWANConnectionDeviceWANDSLLinkConfig_Enable);
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANDSLLinkConfig_LinkStatus);
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANDSLLinkConfig_LinkType);
CPESETFUNC(setWANDeviceWANConnectionDeviceWANDSLLinkConfig_LinkType);
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANDSLLinkConfig_AutoConfig);
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANDSLLinkConfig_DestinationAddress);
CPESETFUNC(setWANDeviceWANConnectionDeviceWANDSLLinkConfig_DestinationAddress);
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANDSLLinkConfig_ATMTransmittedBlocks);
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANDSLLinkConfig_ATMReceivedBlocks);
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANDSLLinkConfig_AAL5CRCErrors);
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANDSLLinkConfig_ATMCRCErrors);
CWMPParam WANDeviceWANConnectionDeviceWANDSLLinkConfig_Params[]={
	{ "Enable", getWANDeviceWANConnectionDeviceWANDSLLinkConfig_Enable, setWANDeviceWANConnectionDeviceWANDSLLinkConfig_Enable, NULL, RPC_RW, eBoolean, 0, 0},
	{ "LinkStatus", getWANDeviceWANConnectionDeviceWANDSLLinkConfig_LinkStatus, NULL, NULL, RPC_R, eString, 0, 0},
	{ "LinkType", getWANDeviceWANConnectionDeviceWANDSLLinkConfig_LinkType, setWANDeviceWANConnectionDeviceWANDSLLinkConfig_LinkType, NULL, RPC_RW, eString, 0, 0},
	{ "AutoConfig", getWANDeviceWANConnectionDeviceWANDSLLinkConfig_AutoConfig, NULL, NULL, RPC_R, eBoolean, 0, 0},
	{ "DestinationAddress", getWANDeviceWANConnectionDeviceWANDSLLinkConfig_DestinationAddress, setWANDeviceWANConnectionDeviceWANDSLLinkConfig_DestinationAddress, NULL, RPC_RW, eString, 0, 256},
	{ "ATMTransmittedBlocks", getWANDeviceWANConnectionDeviceWANDSLLinkConfig_ATMTransmittedBlocks, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "ATMReceivedBlocks", getWANDeviceWANConnectionDeviceWANDSLLinkConfig_ATMReceivedBlocks, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "AAL5CRCErrors", getWANDeviceWANConnectionDeviceWANDSLLinkConfig_AAL5CRCErrors, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "ATMCRCErrors", getWANDeviceWANConnectionDeviceWANDSLLinkConfig_ATMCRCErrors, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{NULL}
};
/**@endparam InternetGatewayDevice.WANDevice.{i}.WANConnectionDevice.{i}.WANDSLLinkConfig.  */

/**@param InternetGatewayDevice.WANDevice.{i}.WANConnectionDevice.{i}.WANEthernetLinkConfig. */
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANEthernetLinkConfig_EthernetLinkStatus);
CWMPParam WANDeviceWANConnectionDeviceWANEthernetLinkConfig_Params[]={
	{ "EthernetLinkStatus", getWANDeviceWANConnectionDeviceWANEthernetLinkConfig_EthernetLinkStatus, NULL, NULL, RPC_R, eString, 0, 0},
	{NULL}
};
/**@endparam InternetGatewayDevice.WANDevice.{i}.WANConnectionDevice.{i}.WANEthernetLinkConfig.  */

/**@param InternetGatewayDevice.WANDevice.{i}.WANConnectionDevice.{i}.WANIPConnection.{i}.DHCPClient.ReqDHCPOption.{i}. */
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANIPConnectionDHCPClientReqDHCPOption_Enable);
CPESETFUNC(setWANDeviceWANConnectionDeviceWANIPConnectionDHCPClientReqDHCPOption_Enable);
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANIPConnectionDHCPClientReqDHCPOption_Tag);
CPESETFUNC(setWANDeviceWANConnectionDeviceWANIPConnectionDHCPClientReqDHCPOption_Tag);
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANIPConnectionDHCPClientReqDHCPOption_Value);
CWMPParam WANDeviceWANConnectionDeviceWANIPConnectionDHCPClientReqDHCPOption_Params[]={
	{ "Enable", getWANDeviceWANConnectionDeviceWANIPConnectionDHCPClientReqDHCPOption_Enable, setWANDeviceWANConnectionDeviceWANIPConnectionDHCPClientReqDHCPOption_Enable, NULL, RPC_RW, eBoolean, 0, 0},
	{ "Tag", getWANDeviceWANConnectionDeviceWANIPConnectionDHCPClientReqDHCPOption_Tag, setWANDeviceWANConnectionDeviceWANIPConnectionDHCPClientReqDHCPOption_Tag, NULL, RPC_RW, eUnsignedInt, 0, 0},
	{ "Value", getWANDeviceWANConnectionDeviceWANIPConnectionDHCPClientReqDHCPOption_Value, NULL, NULL, RPC_R, eBase64, 0, 255},
	{NULL}
};
/**@endparam InternetGatewayDevice.WANDevice.{i}.WANConnectionDevice.{i}.WANIPConnection.{i}.DHCPClient.ReqDHCPOption.{i}.  */

/**@param InternetGatewayDevice.WANDevice.{i}.WANConnectionDevice.{i}.WANIPConnection.{i}.DHCPClient.SentDHCPOption.{i}. */
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANIPConnectionDHCPClientSentDHCPOption_Enable);
CPESETFUNC(setWANDeviceWANConnectionDeviceWANIPConnectionDHCPClientSentDHCPOption_Enable);
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANIPConnectionDHCPClientSentDHCPOption_Tag);
CPESETFUNC(setWANDeviceWANConnectionDeviceWANIPConnectionDHCPClientSentDHCPOption_Tag);
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANIPConnectionDHCPClientSentDHCPOption_Value);
CPESETFUNC(setWANDeviceWANConnectionDeviceWANIPConnectionDHCPClientSentDHCPOption_Value);
CWMPParam WANDeviceWANConnectionDeviceWANIPConnectionDHCPClientSentDHCPOption_Params[]={
	{ "Enable", getWANDeviceWANConnectionDeviceWANIPConnectionDHCPClientSentDHCPOption_Enable, setWANDeviceWANConnectionDeviceWANIPConnectionDHCPClientSentDHCPOption_Enable, NULL, RPC_RW, eBoolean, 0, 0},
	{ "Tag", getWANDeviceWANConnectionDeviceWANIPConnectionDHCPClientSentDHCPOption_Tag, setWANDeviceWANConnectionDeviceWANIPConnectionDHCPClientSentDHCPOption_Tag, NULL, RPC_RW, eUnsignedInt, 0, 0},
	{ "Value", getWANDeviceWANConnectionDeviceWANIPConnectionDHCPClientSentDHCPOption_Value, setWANDeviceWANConnectionDeviceWANIPConnectionDHCPClientSentDHCPOption_Value, NULL, RPC_RW, eBase64, 0, 255},
	{NULL}
};
/**@endparam InternetGatewayDevice.WANDevice.{i}.WANConnectionDevice.{i}.WANIPConnection.{i}.DHCPClient.SentDHCPOption.{i}.  */

/**@param InternetGatewayDevice.WANDevice.{i}.WANConnectionDevice.{i}.WANIPConnection.{i}.DHCPClient. */
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANIPConnectionDHCPClient_SentDHCPOptionNumberOfEntries);
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANIPConnectionDHCPClient_ReqDHCPOptionNumberOfEntries);
CWMPParam WANDeviceWANConnectionDeviceWANIPConnectionDHCPClient_Params[]={
	{ "SentDHCPOptionNumberOfEntries", getWANDeviceWANConnectionDeviceWANIPConnectionDHCPClient_SentDHCPOptionNumberOfEntries, NULL, NULL, RPC_R, eUnsignedInt, 0, 0},
	{ "ReqDHCPOptionNumberOfEntries", getWANDeviceWANConnectionDeviceWANIPConnectionDHCPClient_ReqDHCPOptionNumberOfEntries, NULL, NULL, RPC_R, eUnsignedInt, 0, 0},
	{NULL}
};
/**@endparam InternetGatewayDevice.WANDevice.{i}.WANConnectionDevice.{i}.WANIPConnection.{i}.DHCPClient.  */

/**@obj InternetGatewayDevice.WANDevice.{i}.WANConnectionDevice.{i}.WANIPConnection.{i}.DHCPClient.  */
CPEADDOBJ(addWANDeviceWANConnectionDeviceWANIPConnectionDHCPClientReqDHCPOption);
CPEDELOBJ(delWANDeviceWANConnectionDeviceWANIPConnectionDHCPClientReqDHCPOption);
CPEADDOBJ(addWANDeviceWANConnectionDeviceWANIPConnectionDHCPClientSentDHCPOption);
CPEDELOBJ(delWANDeviceWANConnectionDeviceWANIPConnectionDHCPClientSentDHCPOption);
CWMPObject WANDeviceWANConnectionDeviceWANIPConnectionDHCPClient_Objs[]={
	{"ReqDHCPOption", delWANDeviceWANConnectionDeviceWANIPConnectionDHCPClientReqDHCPOption, addWANDeviceWANConnectionDeviceWANIPConnectionDHCPClientReqDHCPOption, NULL, NULL, NULL, WANDeviceWANConnectionDeviceWANIPConnectionDHCPClientReqDHCPOption_Params, NULL, eInstance,  0},
	{"SentDHCPOption", delWANDeviceWANConnectionDeviceWANIPConnectionDHCPClientSentDHCPOption, addWANDeviceWANConnectionDeviceWANIPConnectionDHCPClientSentDHCPOption, NULL, NULL, NULL, WANDeviceWANConnectionDeviceWANIPConnectionDHCPClientSentDHCPOption_Params, NULL, eInstance,  0},
	{NULL}
};

/**@endobj InternetGatewayDevice.WANDevice.{i}.WANConnectionDevice.{i}.WANIPConnection.{i}.DHCPClient.  */
/**@param InternetGatewayDevice.WANDevice.{i}.WANConnectionDevice.{i}.WANIPConnection.{i}.PortMapping.{i}. */
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_PortMappingEnabled);
CPESETFUNC(setWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_PortMappingEnabled);
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_PortMappingLeaseDuration);
CPESETFUNC(setWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_PortMappingLeaseDuration);
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_RemoteHost);
CPESETFUNC(setWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_RemoteHost);
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_ExternalPort);
CPESETFUNC(setWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_ExternalPort);
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_InternalPort);
CPESETFUNC(setWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_InternalPort);
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_PortMappingProtocol);
CPESETFUNC(setWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_PortMappingProtocol);
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_InternalClient);
CPESETFUNC(setWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_InternalClient);
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_PortMappingDescription);
CPESETFUNC(setWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_PortMappingDescription);
CWMPParam WANDeviceWANConnectionDeviceWANIPConnectionPortMapping_Params[]={
	{ "PortMappingEnabled", getWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_PortMappingEnabled, setWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_PortMappingEnabled, NULL, RPC_RW, eBoolean, 0, 0},
	{ "PortMappingLeaseDuration", getWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_PortMappingLeaseDuration, setWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_PortMappingLeaseDuration, NULL, RPC_RW, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "RemoteHost", getWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_RemoteHost, setWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_RemoteHost, NULL, RPC_RW, eString, 0, 0},
	{ "ExternalPort", getWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_ExternalPort, setWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_ExternalPort, NULL, RPC_RW, eUnsignedInt, 0, 0},
	{ "InternalPort", getWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_InternalPort, setWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_InternalPort, NULL, RPC_RW, eUnsignedInt, 0, 0},
	{ "PortMappingProtocol", getWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_PortMappingProtocol, setWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_PortMappingProtocol, NULL, RPC_RW, eString, 0, 0},
	{ "InternalClient", getWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_InternalClient, setWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_InternalClient, NULL, RPC_RW, eString, 0, 256},
	{ "PortMappingDescription", getWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_PortMappingDescription, setWANDeviceWANConnectionDeviceWANIPConnectionPortMapping_PortMappingDescription, NULL, RPC_RW, eString, 0, 256},
	{NULL}
};
/**@endparam InternetGatewayDevice.WANDevice.{i}.WANConnectionDevice.{i}.WANIPConnection.{i}.PortMapping.{i}.  */

/**@param InternetGatewayDevice.WANDevice.{i}.WANConnectionDevice.{i}.WANIPConnection.{i}.Stats. */
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANIPConnectionStats_EthernetBytesSent);
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANIPConnectionStats_EthernetBytesReceived);
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANIPConnectionStats_EthernetPacketsSent);
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANIPConnectionStats_EthernetPacketsReceived);
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANIPConnectionStats_EthernetErrorsSent);
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANIPConnectionStats_EthernetErrorsReceived);
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANIPConnectionStats_EthernetUnicastPacketsSent);
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANIPConnectionStats_EthernetUnicastPacketsReceived);
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANIPConnectionStats_EthernetDiscardPacketsSent);
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANIPConnectionStats_EthernetDiscardPacketsReceived);
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANIPConnectionStats_EthernetMulticastPacketsSent);
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANIPConnectionStats_EthernetMulticastPacketsReceived);
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANIPConnectionStats_EthernetBroadcastPacketsSent);
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANIPConnectionStats_EthernetBroadcastPacketsReceived);
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANIPConnectionStats_EthernetUnknownProtoPacketsReceived);
CWMPParam WANDeviceWANConnectionDeviceWANIPConnectionStats_Params[]={
	{ "EthernetBytesSent", getWANDeviceWANConnectionDeviceWANIPConnectionStats_EthernetBytesSent, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "EthernetBytesReceived", getWANDeviceWANConnectionDeviceWANIPConnectionStats_EthernetBytesReceived, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "EthernetPacketsSent", getWANDeviceWANConnectionDeviceWANIPConnectionStats_EthernetPacketsSent, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "EthernetPacketsReceived", getWANDeviceWANConnectionDeviceWANIPConnectionStats_EthernetPacketsReceived, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "EthernetErrorsSent", getWANDeviceWANConnectionDeviceWANIPConnectionStats_EthernetErrorsSent, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "EthernetErrorsReceived", getWANDeviceWANConnectionDeviceWANIPConnectionStats_EthernetErrorsReceived, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "EthernetUnicastPacketsSent", getWANDeviceWANConnectionDeviceWANIPConnectionStats_EthernetUnicastPacketsSent, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "EthernetUnicastPacketsReceived", getWANDeviceWANConnectionDeviceWANIPConnectionStats_EthernetUnicastPacketsReceived, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "EthernetDiscardPacketsSent", getWANDeviceWANConnectionDeviceWANIPConnectionStats_EthernetDiscardPacketsSent, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "EthernetDiscardPacketsReceived", getWANDeviceWANConnectionDeviceWANIPConnectionStats_EthernetDiscardPacketsReceived, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "EthernetMulticastPacketsSent", getWANDeviceWANConnectionDeviceWANIPConnectionStats_EthernetMulticastPacketsSent, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "EthernetMulticastPacketsReceived", getWANDeviceWANConnectionDeviceWANIPConnectionStats_EthernetMulticastPacketsReceived, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "EthernetBroadcastPacketsSent", getWANDeviceWANConnectionDeviceWANIPConnectionStats_EthernetBroadcastPacketsSent, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "EthernetBroadcastPacketsReceived", getWANDeviceWANConnectionDeviceWANIPConnectionStats_EthernetBroadcastPacketsReceived, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "EthernetUnknownProtoPacketsReceived", getWANDeviceWANConnectionDeviceWANIPConnectionStats_EthernetUnknownProtoPacketsReceived, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{NULL}
};
/**@endparam InternetGatewayDevice.WANDevice.{i}.WANConnectionDevice.{i}.WANIPConnection.{i}.Stats.  */

/**@param InternetGatewayDevice.WANDevice.{i}.WANConnectionDevice.{i}.WANIPConnection.{i}. */
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANIPConnection_Enable);
CPESETFUNC(setWANDeviceWANConnectionDeviceWANIPConnection_Enable);
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANIPConnection_ConnectionStatus);
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANIPConnection_PossibleConnectionTypes);
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANIPConnection_ConnectionType);
CPESETFUNC(setWANDeviceWANConnectionDeviceWANIPConnection_ConnectionType);
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANIPConnection_Name);
CPESETFUNC(setWANDeviceWANConnectionDeviceWANIPConnection_Name);
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANIPConnection_Uptime);
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANIPConnection_LastConnectionError);
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANIPConnection_RSIPAvailable);
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANIPConnection_NATEnabled);
CPESETFUNC(setWANDeviceWANConnectionDeviceWANIPConnection_NATEnabled);
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANIPConnection_AddressingType);
CPESETFUNC(setWANDeviceWANConnectionDeviceWANIPConnection_AddressingType);
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANIPConnection_ExternalIPAddress);
CPESETFUNC(setWANDeviceWANConnectionDeviceWANIPConnection_ExternalIPAddress);
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANIPConnection_SubnetMask);
CPESETFUNC(setWANDeviceWANConnectionDeviceWANIPConnection_SubnetMask);
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANIPConnection_DefaultGateway);
CPESETFUNC(setWANDeviceWANConnectionDeviceWANIPConnection_DefaultGateway);
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANIPConnection_DNSEnabled);
CPESETFUNC(setWANDeviceWANConnectionDeviceWANIPConnection_DNSEnabled);
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANIPConnection_DNSOverrideAllowed);
CPESETFUNC(setWANDeviceWANConnectionDeviceWANIPConnection_DNSOverrideAllowed);
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANIPConnection_DNSServers);
CPESETFUNC(setWANDeviceWANConnectionDeviceWANIPConnection_DNSServers);
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANIPConnection_MACAddress);
CPESETFUNC(setWANDeviceWANConnectionDeviceWANIPConnection_MACAddress);
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANIPConnection_ConnectionTrigger);
CPESETFUNC(setWANDeviceWANConnectionDeviceWANIPConnection_ConnectionTrigger);
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANIPConnection_RouteProtocolRx);
CPESETFUNC(setWANDeviceWANConnectionDeviceWANIPConnection_RouteProtocolRx);
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANIPConnection_PortMappingNumberOfEntries);
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANIPConnection_Reset);
CPESETFUNC(setWANDeviceWANConnectionDeviceWANIPConnection_Reset);
CWMPParam WANDeviceWANConnectionDeviceWANIPConnection_Params[]={
	{ "Enable", getWANDeviceWANConnectionDeviceWANIPConnection_Enable, setWANDeviceWANConnectionDeviceWANIPConnection_Enable, NULL, RPC_RW, eBoolean, 0, 0},
	{ "ConnectionStatus", getWANDeviceWANConnectionDeviceWANIPConnection_ConnectionStatus, NULL, NULL, RPC_R, eString, 0, 0},
	{ "PossibleConnectionTypes", getWANDeviceWANConnectionDeviceWANIPConnection_PossibleConnectionTypes, NULL, NULL, RPC_R, eString, 0, 0},
	{ "ConnectionType", getWANDeviceWANConnectionDeviceWANIPConnection_ConnectionType, setWANDeviceWANConnectionDeviceWANIPConnection_ConnectionType, NULL, RPC_RW, eString, 0, 0},
	{ "Name", getWANDeviceWANConnectionDeviceWANIPConnection_Name, setWANDeviceWANConnectionDeviceWANIPConnection_Name, NULL, RPC_RW, eString, 0, 256},
	{ "Uptime", getWANDeviceWANConnectionDeviceWANIPConnection_Uptime, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "LastConnectionError", getWANDeviceWANConnectionDeviceWANIPConnection_LastConnectionError, NULL, NULL, RPC_R, eString, 0, 0},
	{ "RSIPAvailable", getWANDeviceWANConnectionDeviceWANIPConnection_RSIPAvailable, NULL, NULL, RPC_R, eBoolean, 0, 0},
	{ "NATEnabled", getWANDeviceWANConnectionDeviceWANIPConnection_NATEnabled, setWANDeviceWANConnectionDeviceWANIPConnection_NATEnabled, NULL, RPC_RW, eBoolean, 0, 0},
	{ "AddressingType", getWANDeviceWANConnectionDeviceWANIPConnection_AddressingType, setWANDeviceWANConnectionDeviceWANIPConnection_AddressingType, NULL, RPC_RW, eString, 0, 0},
	{ "ExternalIPAddress", getWANDeviceWANConnectionDeviceWANIPConnection_ExternalIPAddress, setWANDeviceWANConnectionDeviceWANIPConnection_ExternalIPAddress, NULL, RPC_RW, eString, 0, 45},
	{ "SubnetMask", getWANDeviceWANConnectionDeviceWANIPConnection_SubnetMask, setWANDeviceWANConnectionDeviceWANIPConnection_SubnetMask, NULL, RPC_RW, eString, 0, 45},
	{ "DefaultGateway", getWANDeviceWANConnectionDeviceWANIPConnection_DefaultGateway, setWANDeviceWANConnectionDeviceWANIPConnection_DefaultGateway, NULL, RPC_RW, eString, 0, 0},
	{ "DNSEnabled", getWANDeviceWANConnectionDeviceWANIPConnection_DNSEnabled, setWANDeviceWANConnectionDeviceWANIPConnection_DNSEnabled, NULL, RPC_RW, eBoolean, 0, 0},
	{ "DNSOverrideAllowed", getWANDeviceWANConnectionDeviceWANIPConnection_DNSOverrideAllowed, setWANDeviceWANConnectionDeviceWANIPConnection_DNSOverrideAllowed, NULL, RPC_RW, eBoolean, 0, 0},
	{ "DNSServers", getWANDeviceWANConnectionDeviceWANIPConnection_DNSServers, setWANDeviceWANConnectionDeviceWANIPConnection_DNSServers, NULL, RPC_RW, eString, 0, 45},
	{ "MACAddress", getWANDeviceWANConnectionDeviceWANIPConnection_MACAddress, setWANDeviceWANConnectionDeviceWANIPConnection_MACAddress, NULL, RPC_RW, eString, 0, 17},
	{ "ConnectionTrigger", getWANDeviceWANConnectionDeviceWANIPConnection_ConnectionTrigger, setWANDeviceWANConnectionDeviceWANIPConnection_ConnectionTrigger, NULL, RPC_RW, eString, 0, 0},
	{ "RouteProtocolRx", getWANDeviceWANConnectionDeviceWANIPConnection_RouteProtocolRx, setWANDeviceWANConnectionDeviceWANIPConnection_RouteProtocolRx, NULL, RPC_RW, eString, 0, 0},
	{ "PortMappingNumberOfEntries", getWANDeviceWANConnectionDeviceWANIPConnection_PortMappingNumberOfEntries, NULL, NULL, RPC_R, eUnsignedInt, 0, 0},
	{ "Reset", getWANDeviceWANConnectionDeviceWANIPConnection_Reset, setWANDeviceWANConnectionDeviceWANIPConnection_Reset, NULL, RPC_RW, eBoolean, 0, 0},
	{NULL}
};
/**@endparam InternetGatewayDevice.WANDevice.{i}.WANConnectionDevice.{i}.WANIPConnection.{i}.  */

/**@obj InternetGatewayDevice.WANDevice.{i}.WANConnectionDevice.{i}.WANIPConnection.{i}.  */
CPEADDOBJ(addWANDeviceWANConnectionDeviceWANIPConnectionPortMapping);
CPEDELOBJ(delWANDeviceWANConnectionDeviceWANIPConnectionPortMapping);
CPECOMMIT(commitWANDeviceWANConnectionDeviceWANIPConnectionPortMapping);
CWMPObject WANDeviceWANConnectionDeviceWANIPConnection_Objs[]={
	{"DHCPClient", NULL, NULL, NULL, NULL, WANDeviceWANConnectionDeviceWANIPConnectionDHCPClient_Objs, WANDeviceWANConnectionDeviceWANIPConnectionDHCPClient_Params, NULL, eObject,  0},
	{"PortMapping", delWANDeviceWANConnectionDeviceWANIPConnectionPortMapping, addWANDeviceWANConnectionDeviceWANIPConnectionPortMapping, commitWANDeviceWANConnectionDeviceWANIPConnectionPortMapping, NULL, NULL, WANDeviceWANConnectionDeviceWANIPConnectionPortMapping_Params, NULL, eInstance,  0},
	{"Stats", NULL, NULL, NULL, NULL, NULL, WANDeviceWANConnectionDeviceWANIPConnectionStats_Params, NULL, eObject,  0},
	{NULL}
};

/**@endobj InternetGatewayDevice.WANDevice.{i}.WANConnectionDevice.{i}.WANIPConnection.{i}.  */
/**@param InternetGatewayDevice.WANDevice.{i}.WANConnectionDevice.{i}.WANPPPConnection.{i}.PortMapping.{i}. */
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_PortMappingEnabled);
CPESETFUNC(setWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_PortMappingEnabled);
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_PortMappingLeaseDuration);
CPESETFUNC(setWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_PortMappingLeaseDuration);
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_RemoteHost);
CPESETFUNC(setWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_RemoteHost);
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_ExternalPort);
CPESETFUNC(setWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_ExternalPort);
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_InternalPort);
CPESETFUNC(setWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_InternalPort);
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_PortMappingProtocol);
CPESETFUNC(setWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_PortMappingProtocol);
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_InternalClient);
CPESETFUNC(setWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_InternalClient);
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_PortMappingDescription);
CPESETFUNC(setWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_PortMappingDescription);
CWMPParam WANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_Params[]={
	{ "PortMappingEnabled", getWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_PortMappingEnabled, setWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_PortMappingEnabled, NULL, RPC_RW, eBoolean, 0, 0},
	{ "PortMappingLeaseDuration", getWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_PortMappingLeaseDuration, setWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_PortMappingLeaseDuration, NULL, RPC_RW, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "RemoteHost", getWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_RemoteHost, setWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_RemoteHost, NULL, RPC_RW, eString, 0, 0},
	{ "ExternalPort", getWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_ExternalPort, setWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_ExternalPort, NULL, RPC_RW, eUnsignedInt, 0, 0},
	{ "InternalPort", getWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_InternalPort, setWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_InternalPort, NULL, RPC_RW, eUnsignedInt, 0, 0},
	{ "PortMappingProtocol", getWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_PortMappingProtocol, setWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_PortMappingProtocol, NULL, RPC_RW, eString, 0, 0},
	{ "InternalClient", getWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_InternalClient, setWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_InternalClient, NULL, RPC_RW, eString, 0, 256},
	{ "PortMappingDescription", getWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_PortMappingDescription, setWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_PortMappingDescription, NULL, RPC_RW, eString, 0, 0},
	{NULL}
};
/**@endparam InternetGatewayDevice.WANDevice.{i}.WANConnectionDevice.{i}.WANPPPConnection.{i}.PortMapping.{i}.  */

/**@param InternetGatewayDevice.WANDevice.{i}.WANConnectionDevice.{i}.WANPPPConnection.{i}.Stats. */
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANPPPConnectionStats_EthernetBytesSent);
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANPPPConnectionStats_EthernetBytesReceived);
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANPPPConnectionStats_EthernetPacketsSent);
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANPPPConnectionStats_EthernetPacketsReceived);
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANPPPConnectionStats_EthernetErrorsSent);
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANPPPConnectionStats_EthernetErrorsReceived);
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANPPPConnectionStats_EthernetUnicastPacketsSent);
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANPPPConnectionStats_EthernetUnicastPacketsReceived);
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANPPPConnectionStats_EthernetDiscardPacketsSent);
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANPPPConnectionStats_EthernetDiscardPacketsReceived);
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANPPPConnectionStats_EthernetMulticastPacketsSent);
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANPPPConnectionStats_EthernetMulticastPacketsReceived);
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANPPPConnectionStats_EthernetBroadcastPacketsSent);
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANPPPConnectionStats_EthernetBroadcastPacketsReceived);
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANPPPConnectionStats_EthernetUnknownProtoPacketsReceived);
CWMPParam WANDeviceWANConnectionDeviceWANPPPConnectionStats_Params[]={
	{ "EthernetBytesSent", getWANDeviceWANConnectionDeviceWANPPPConnectionStats_EthernetBytesSent, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "EthernetBytesReceived", getWANDeviceWANConnectionDeviceWANPPPConnectionStats_EthernetBytesReceived, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "EthernetPacketsSent", getWANDeviceWANConnectionDeviceWANPPPConnectionStats_EthernetPacketsSent, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "EthernetPacketsReceived", getWANDeviceWANConnectionDeviceWANPPPConnectionStats_EthernetPacketsReceived, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "EthernetErrorsSent", getWANDeviceWANConnectionDeviceWANPPPConnectionStats_EthernetErrorsSent, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "EthernetErrorsReceived", getWANDeviceWANConnectionDeviceWANPPPConnectionStats_EthernetErrorsReceived, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "EthernetUnicastPacketsSent", getWANDeviceWANConnectionDeviceWANPPPConnectionStats_EthernetUnicastPacketsSent, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "EthernetUnicastPacketsReceived", getWANDeviceWANConnectionDeviceWANPPPConnectionStats_EthernetUnicastPacketsReceived, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "EthernetDiscardPacketsSent", getWANDeviceWANConnectionDeviceWANPPPConnectionStats_EthernetDiscardPacketsSent, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "EthernetDiscardPacketsReceived", getWANDeviceWANConnectionDeviceWANPPPConnectionStats_EthernetDiscardPacketsReceived, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "EthernetMulticastPacketsSent", getWANDeviceWANConnectionDeviceWANPPPConnectionStats_EthernetMulticastPacketsSent, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "EthernetMulticastPacketsReceived", getWANDeviceWANConnectionDeviceWANPPPConnectionStats_EthernetMulticastPacketsReceived, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "EthernetBroadcastPacketsSent", getWANDeviceWANConnectionDeviceWANPPPConnectionStats_EthernetBroadcastPacketsSent, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "EthernetBroadcastPacketsReceived", getWANDeviceWANConnectionDeviceWANPPPConnectionStats_EthernetBroadcastPacketsReceived, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "EthernetUnknownProtoPacketsReceived", getWANDeviceWANConnectionDeviceWANPPPConnectionStats_EthernetUnknownProtoPacketsReceived, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{NULL}
};
/**@endparam InternetGatewayDevice.WANDevice.{i}.WANConnectionDevice.{i}.WANPPPConnection.{i}.Stats.  */

/**@param InternetGatewayDevice.WANDevice.{i}.WANConnectionDevice.{i}.WANPPPConnection.{i}. */
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANPPPConnection_Enable);
CPESETFUNC(setWANDeviceWANConnectionDeviceWANPPPConnection_Enable);
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANPPPConnection_ConnectionStatus);
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANPPPConnection_PossibleConnectionTypes);
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANPPPConnection_ConnectionType);
CPESETFUNC(setWANDeviceWANConnectionDeviceWANPPPConnection_ConnectionType);
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANPPPConnection_Name);
CPESETFUNC(setWANDeviceWANConnectionDeviceWANPPPConnection_Name);
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANPPPConnection_Uptime);
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANPPPConnection_LastConnectionError);
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANPPPConnection_RSIPAvailable);
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANPPPConnection_NATEnabled);
CPESETFUNC(setWANDeviceWANConnectionDeviceWANPPPConnection_NATEnabled);
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANPPPConnection_Username);
CPESETFUNC(setWANDeviceWANConnectionDeviceWANPPPConnection_Username);
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANPPPConnection_Password);
CPESETFUNC(setWANDeviceWANConnectionDeviceWANPPPConnection_Password);
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANPPPConnection_ExternalIPAddress);
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANPPPConnection_DNSEnabled);
CPESETFUNC(setWANDeviceWANConnectionDeviceWANPPPConnection_DNSEnabled);
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANPPPConnection_DNSOverrideAllowed);
CPESETFUNC(setWANDeviceWANConnectionDeviceWANPPPConnection_DNSOverrideAllowed);
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANPPPConnection_DNSServers);
CPESETFUNC(setWANDeviceWANConnectionDeviceWANPPPConnection_DNSServers);
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANPPPConnection_MACAddress);
CPESETFUNC(setWANDeviceWANConnectionDeviceWANPPPConnection_MACAddress);
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANPPPConnection_TransportType);
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANPPPConnection_PPPoEACName);
CPESETFUNC(setWANDeviceWANConnectionDeviceWANPPPConnection_PPPoEACName);
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANPPPConnection_PPPoEServiceName);
CPESETFUNC(setWANDeviceWANConnectionDeviceWANPPPConnection_PPPoEServiceName);
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANPPPConnection_ConnectionTrigger);
CPESETFUNC(setWANDeviceWANConnectionDeviceWANPPPConnection_ConnectionTrigger);
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANPPPConnection_RouteProtocolRx);
CPESETFUNC(setWANDeviceWANConnectionDeviceWANPPPConnection_RouteProtocolRx);
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANPPPConnection_PortMappingNumberOfEntries);
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANPPPConnection_Reset);
CPESETFUNC(setWANDeviceWANConnectionDeviceWANPPPConnection_Reset);
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANPPPConnection_PPPoESessionID);
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANPPPConnection_DefaultGateway);
CWMPParam WANDeviceWANConnectionDeviceWANPPPConnection_Params[]={
	{ "Enable", getWANDeviceWANConnectionDeviceWANPPPConnection_Enable, setWANDeviceWANConnectionDeviceWANPPPConnection_Enable, NULL, RPC_RW, eBoolean, 0, 0},
	{ "ConnectionStatus", getWANDeviceWANConnectionDeviceWANPPPConnection_ConnectionStatus, NULL, NULL, RPC_R, eString, 0, 0},
	{ "PossibleConnectionTypes", getWANDeviceWANConnectionDeviceWANPPPConnection_PossibleConnectionTypes, NULL, NULL, RPC_R, eString, 0, 0},
	{ "ConnectionType", getWANDeviceWANConnectionDeviceWANPPPConnection_ConnectionType, setWANDeviceWANConnectionDeviceWANPPPConnection_ConnectionType, NULL, RPC_RW, eString, 0, 0},
	{ "Name", getWANDeviceWANConnectionDeviceWANPPPConnection_Name, setWANDeviceWANConnectionDeviceWANPPPConnection_Name, NULL, RPC_RW, eString, 0, 256},
	{ "Uptime", getWANDeviceWANConnectionDeviceWANPPPConnection_Uptime, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "LastConnectionError", getWANDeviceWANConnectionDeviceWANPPPConnection_LastConnectionError, NULL, NULL, RPC_R, eString, 0, 0},
	{ "RSIPAvailable", getWANDeviceWANConnectionDeviceWANPPPConnection_RSIPAvailable, NULL, NULL, RPC_R, eBoolean, 0, 0},
	{ "NATEnabled", getWANDeviceWANConnectionDeviceWANPPPConnection_NATEnabled, setWANDeviceWANConnectionDeviceWANPPPConnection_NATEnabled, NULL, RPC_RW, eBoolean, 0, 0},
	{ "Username", getWANDeviceWANConnectionDeviceWANPPPConnection_Username, setWANDeviceWANConnectionDeviceWANPPPConnection_Username, NULL, RPC_RW, eString, 0, 0},
	{ "Password", getWANDeviceWANConnectionDeviceWANPPPConnection_Password, setWANDeviceWANConnectionDeviceWANPPPConnection_Password, NULL, RPC_RW, eString, 0, 0},
	{ "ExternalIPAddress", getWANDeviceWANConnectionDeviceWANPPPConnection_ExternalIPAddress, NULL, NULL, RPC_R, eString, 0, 45},
	{ "DNSEnabled", getWANDeviceWANConnectionDeviceWANPPPConnection_DNSEnabled, setWANDeviceWANConnectionDeviceWANPPPConnection_DNSEnabled, NULL, RPC_RW, eBoolean, 0, 0},
	{ "DNSOverrideAllowed", getWANDeviceWANConnectionDeviceWANPPPConnection_DNSOverrideAllowed, setWANDeviceWANConnectionDeviceWANPPPConnection_DNSOverrideAllowed, NULL, RPC_RW, eBoolean, 0, 0},
	{ "DNSServers", getWANDeviceWANConnectionDeviceWANPPPConnection_DNSServers, setWANDeviceWANConnectionDeviceWANPPPConnection_DNSServers, NULL, RPC_RW, eString, 0, 45},
	{ "MACAddress", getWANDeviceWANConnectionDeviceWANPPPConnection_MACAddress, setWANDeviceWANConnectionDeviceWANPPPConnection_MACAddress, NULL, RPC_RW, eString, 0, 17},
	{ "TransportType", getWANDeviceWANConnectionDeviceWANPPPConnection_TransportType, NULL, NULL, RPC_R, eString, 0, 0},
	{ "PPPoEACName", getWANDeviceWANConnectionDeviceWANPPPConnection_PPPoEACName, setWANDeviceWANConnectionDeviceWANPPPConnection_PPPoEACName, NULL, RPC_RW, eString, 0, 0},
	{ "PPPoEServiceName", getWANDeviceWANConnectionDeviceWANPPPConnection_PPPoEServiceName, setWANDeviceWANConnectionDeviceWANPPPConnection_PPPoEServiceName, NULL, RPC_RW, eString, 0, 0},
	{ "ConnectionTrigger", getWANDeviceWANConnectionDeviceWANPPPConnection_ConnectionTrigger, setWANDeviceWANConnectionDeviceWANPPPConnection_ConnectionTrigger, NULL, RPC_RW, eString, 0, 0},
	{ "RouteProtocolRx", getWANDeviceWANConnectionDeviceWANPPPConnection_RouteProtocolRx, setWANDeviceWANConnectionDeviceWANPPPConnection_RouteProtocolRx, NULL, RPC_RW, eString, 0, 0},
	{ "PortMappingNumberOfEntries", getWANDeviceWANConnectionDeviceWANPPPConnection_PortMappingNumberOfEntries, NULL, NULL, RPC_R, eUnsignedInt, 0, 0},
	{ "Reset", getWANDeviceWANConnectionDeviceWANPPPConnection_Reset, setWANDeviceWANConnectionDeviceWANPPPConnection_Reset, NULL, RPC_RW, eBoolean, 0, 0},
	{ "PPPoESessionID", getWANDeviceWANConnectionDeviceWANPPPConnection_PPPoESessionID, NULL, NULL, RPC_R, eUnsignedInt, 0, 0},
	{ "DefaultGateway", getWANDeviceWANConnectionDeviceWANPPPConnection_DefaultGateway, NULL, NULL, RPC_R, eString, 0, 0},
	{NULL}
};
/**@endparam InternetGatewayDevice.WANDevice.{i}.WANConnectionDevice.{i}.WANPPPConnection.{i}.  */

/**@obj InternetGatewayDevice.WANDevice.{i}.WANConnectionDevice.{i}.WANPPPConnection.{i}.  */
CPEADDOBJ(addWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping);
CPEDELOBJ(delWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping);
CPECOMMIT(commitWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping);
CWMPObject WANDeviceWANConnectionDeviceWANPPPConnection_Objs[]={
	{"PortMapping", delWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping, addWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping, commitWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping, NULL, NULL, WANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_Params, NULL, eInstance,  0},
	{"Stats", NULL, NULL, NULL, NULL, NULL, WANDeviceWANConnectionDeviceWANPPPConnectionStats_Params, NULL, eObject,  0},
	{NULL}
};

/**@endobj InternetGatewayDevice.WANDevice.{i}.WANConnectionDevice.{i}.WANPPPConnection.{i}.  */
/**@param InternetGatewayDevice.WANDevice.{i}.WANConnectionDevice.{i}. */
CPEGETFUNC(getWANDeviceWANConnectionDevice_WANIPConnectionNumberOfEntries);
CPEGETFUNC(getWANDeviceWANConnectionDevice_WANPPPConnectionNumberOfEntries);
CWMPParam WANDeviceWANConnectionDevice_Params[]={
	{ "WANIPConnectionNumberOfEntries", getWANDeviceWANConnectionDevice_WANIPConnectionNumberOfEntries, NULL, NULL, RPC_R, eUnsignedInt, 0, 0},
	{ "WANPPPConnectionNumberOfEntries", getWANDeviceWANConnectionDevice_WANPPPConnectionNumberOfEntries, NULL, NULL, RPC_R, eUnsignedInt, 0, 0},
	{NULL}
};
/**@endparam InternetGatewayDevice.WANDevice.{i}.WANConnectionDevice.{i}.  */

/**@obj InternetGatewayDevice.WANDevice.{i}.WANConnectionDevice.{i}.  */
CPEADDOBJ(addWANDeviceWANConnectionDeviceWANIPConnection);
CPEDELOBJ(delWANDeviceWANConnectionDeviceWANIPConnection);
CPEADDOBJ(addWANDeviceWANConnectionDeviceWANPPPConnection);
CPEDELOBJ(delWANDeviceWANConnectionDeviceWANPPPConnection);
CWMPObject WANDeviceWANConnectionDevice_Objs[]={
	{"WANDSLLinkConfig", NULL, NULL, NULL, NULL, NULL, WANDeviceWANConnectionDeviceWANDSLLinkConfig_Params, NULL, eInstance,  0},
	{"WANEthernetLinkConfig", NULL, NULL, NULL, NULL, NULL, WANDeviceWANConnectionDeviceWANEthernetLinkConfig_Params, NULL, eInstance,  0},
	{"WANIPConnection", delWANDeviceWANConnectionDeviceWANIPConnection, addWANDeviceWANConnectionDeviceWANIPConnection, NULL, NULL, WANDeviceWANConnectionDeviceWANIPConnection_Objs, WANDeviceWANConnectionDeviceWANIPConnection_Params, NULL, eInstance,  0},
	{"WANPPPConnection", delWANDeviceWANConnectionDeviceWANPPPConnection, addWANDeviceWANConnectionDeviceWANPPPConnection, NULL, NULL, WANDeviceWANConnectionDeviceWANPPPConnection_Objs, WANDeviceWANConnectionDeviceWANPPPConnection_Params, NULL, eInstance,  0},
	{NULL}
};

/**@endobj InternetGatewayDevice.WANDevice.{i}.WANConnectionDevice.{i}.  */
/**@param InternetGatewayDevice.WANDevice.{i}.WANDSLConnectionManagement.ConnectionService.{i}. */
CWMPParam WANDeviceWANDSLConnectionManagementConnectionService_Params[]={
	{NULL}
};
/**@endparam InternetGatewayDevice.WANDevice.{i}.WANDSLConnectionManagement.ConnectionService.{i}.  */

/**@param InternetGatewayDevice.WANDevice.{i}.WANDSLConnectionManagement. */
CWMPParam WANDeviceWANDSLConnectionManagement_Params[]={
	{NULL}
};
/**@endparam InternetGatewayDevice.WANDevice.{i}.WANDSLConnectionManagement.  */

/**@obj InternetGatewayDevice.WANDevice.{i}.WANDSLConnectionManagement.  */
CWMPObject WANDeviceWANDSLConnectionManagement_Objs[]={
	{"ConnectionService", NULL, NULL, NULL, NULL, NULL, WANDeviceWANDSLConnectionManagementConnectionService_Params, NULL, eInstance,  0},
	{NULL}
};

/**@endobj InternetGatewayDevice.WANDevice.{i}.WANDSLConnectionManagement.  */
/**@param InternetGatewayDevice.WANDevice.{i}.WANDSLInterfaceConfig.Stats.Showtime. */
CPEGETFUNC(getWANDeviceWANDSLInterfaceConfigStatsShowtime_ReceiveBlocks);
CPEGETFUNC(getWANDeviceWANDSLInterfaceConfigStatsShowtime_TransmitBlocks);
CPEGETFUNC(getWANDeviceWANDSLInterfaceConfigStatsShowtime_CellDelin);
CPEGETFUNC(getWANDeviceWANDSLInterfaceConfigStatsShowtime_LinkRetrain);
CPEGETFUNC(getWANDeviceWANDSLInterfaceConfigStatsShowtime_InitErrors);
CPEGETFUNC(getWANDeviceWANDSLInterfaceConfigStatsShowtime_InitTimeouts);
CPEGETFUNC(getWANDeviceWANDSLInterfaceConfigStatsShowtime_LossOfFraming);
CPEGETFUNC(getWANDeviceWANDSLInterfaceConfigStatsShowtime_ErroredSecs);
CPEGETFUNC(getWANDeviceWANDSLInterfaceConfigStatsShowtime_SeverelyErroredSecs);
CPEGETFUNC(getWANDeviceWANDSLInterfaceConfigStatsShowtime_FECErrors);
CPEGETFUNC(getWANDeviceWANDSLInterfaceConfigStatsShowtime_ATUCFECErrors);
CPEGETFUNC(getWANDeviceWANDSLInterfaceConfigStatsShowtime_HECErrors);
CPEGETFUNC(getWANDeviceWANDSLInterfaceConfigStatsShowtime_ATUCHECErrors);
CPEGETFUNC(getWANDeviceWANDSLInterfaceConfigStatsShowtime_CRCErrors);
CPEGETFUNC(getWANDeviceWANDSLInterfaceConfigStatsShowtime_ATUCCRCErrors);
CWMPParam WANDeviceWANDSLInterfaceConfigStatsShowtime_Params[]={
	{ "ReceiveBlocks", getWANDeviceWANDSLInterfaceConfigStatsShowtime_ReceiveBlocks, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "TransmitBlocks", getWANDeviceWANDSLInterfaceConfigStatsShowtime_TransmitBlocks, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "CellDelin", getWANDeviceWANDSLInterfaceConfigStatsShowtime_CellDelin, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "LinkRetrain", getWANDeviceWANDSLInterfaceConfigStatsShowtime_LinkRetrain, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "InitErrors", getWANDeviceWANDSLInterfaceConfigStatsShowtime_InitErrors, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "InitTimeouts", getWANDeviceWANDSLInterfaceConfigStatsShowtime_InitTimeouts, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "LossOfFraming", getWANDeviceWANDSLInterfaceConfigStatsShowtime_LossOfFraming, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "ErroredSecs", getWANDeviceWANDSLInterfaceConfigStatsShowtime_ErroredSecs, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "SeverelyErroredSecs", getWANDeviceWANDSLInterfaceConfigStatsShowtime_SeverelyErroredSecs, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "FECErrors", getWANDeviceWANDSLInterfaceConfigStatsShowtime_FECErrors, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "ATUCFECErrors", getWANDeviceWANDSLInterfaceConfigStatsShowtime_ATUCFECErrors, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "HECErrors", getWANDeviceWANDSLInterfaceConfigStatsShowtime_HECErrors, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "ATUCHECErrors", getWANDeviceWANDSLInterfaceConfigStatsShowtime_ATUCHECErrors, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "CRCErrors", getWANDeviceWANDSLInterfaceConfigStatsShowtime_CRCErrors, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "ATUCCRCErrors", getWANDeviceWANDSLInterfaceConfigStatsShowtime_ATUCCRCErrors, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{NULL}
};
/**@endparam InternetGatewayDevice.WANDevice.{i}.WANDSLInterfaceConfig.Stats.Showtime.  */

/**@param InternetGatewayDevice.WANDevice.{i}.WANDSLInterfaceConfig.Stats.Total. */
CPEGETFUNC(getWANDeviceWANDSLInterfaceConfigStatsTotal_ReceiveBlocks);
CPEGETFUNC(getWANDeviceWANDSLInterfaceConfigStatsTotal_TransmitBlocks);
CPEGETFUNC(getWANDeviceWANDSLInterfaceConfigStatsTotal_CellDelin);
CPEGETFUNC(getWANDeviceWANDSLInterfaceConfigStatsTotal_LinkRetrain);
CPEGETFUNC(getWANDeviceWANDSLInterfaceConfigStatsTotal_InitErrors);
CPEGETFUNC(getWANDeviceWANDSLInterfaceConfigStatsTotal_InitTimeouts);
CPEGETFUNC(getWANDeviceWANDSLInterfaceConfigStatsTotal_LossOfFraming);
CPEGETFUNC(getWANDeviceWANDSLInterfaceConfigStatsTotal_ErroredSecs);
CPEGETFUNC(getWANDeviceWANDSLInterfaceConfigStatsTotal_SeverelyErroredSecs);
CPEGETFUNC(getWANDeviceWANDSLInterfaceConfigStatsTotal_FECErrors);
CPEGETFUNC(getWANDeviceWANDSLInterfaceConfigStatsTotal_ATUCFECErrors);
CPEGETFUNC(getWANDeviceWANDSLInterfaceConfigStatsTotal_HECErrors);
CPEGETFUNC(getWANDeviceWANDSLInterfaceConfigStatsTotal_ATUCHECErrors);
CPEGETFUNC(getWANDeviceWANDSLInterfaceConfigStatsTotal_CRCErrors);
CPEGETFUNC(getWANDeviceWANDSLInterfaceConfigStatsTotal_ATUCCRCErrors);
CWMPParam WANDeviceWANDSLInterfaceConfigStatsTotal_Params[]={
	{ "ReceiveBlocks", getWANDeviceWANDSLInterfaceConfigStatsTotal_ReceiveBlocks, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "TransmitBlocks", getWANDeviceWANDSLInterfaceConfigStatsTotal_TransmitBlocks, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "CellDelin", getWANDeviceWANDSLInterfaceConfigStatsTotal_CellDelin, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "LinkRetrain", getWANDeviceWANDSLInterfaceConfigStatsTotal_LinkRetrain, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "InitErrors", getWANDeviceWANDSLInterfaceConfigStatsTotal_InitErrors, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "InitTimeouts", getWANDeviceWANDSLInterfaceConfigStatsTotal_InitTimeouts, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "LossOfFraming", getWANDeviceWANDSLInterfaceConfigStatsTotal_LossOfFraming, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "ErroredSecs", getWANDeviceWANDSLInterfaceConfigStatsTotal_ErroredSecs, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "SeverelyErroredSecs", getWANDeviceWANDSLInterfaceConfigStatsTotal_SeverelyErroredSecs, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "FECErrors", getWANDeviceWANDSLInterfaceConfigStatsTotal_FECErrors, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "ATUCFECErrors", getWANDeviceWANDSLInterfaceConfigStatsTotal_ATUCFECErrors, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "HECErrors", getWANDeviceWANDSLInterfaceConfigStatsTotal_HECErrors, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "ATUCHECErrors", getWANDeviceWANDSLInterfaceConfigStatsTotal_ATUCHECErrors, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "CRCErrors", getWANDeviceWANDSLInterfaceConfigStatsTotal_CRCErrors, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "ATUCCRCErrors", getWANDeviceWANDSLInterfaceConfigStatsTotal_ATUCCRCErrors, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{NULL}
};
/**@endparam InternetGatewayDevice.WANDevice.{i}.WANDSLInterfaceConfig.Stats.Total.  */

/**@obj InternetGatewayDevice.WANDevice.{i}.WANDSLInterfaceConfig.Stats.  */
CWMPObject WANDeviceWANDSLInterfaceConfigStats_Objs[]={
	{"Showtime", NULL, NULL, NULL, NULL, NULL, WANDeviceWANDSLInterfaceConfigStatsShowtime_Params, NULL, eObject,  0},
	{"Total", NULL, NULL, NULL, NULL, NULL, WANDeviceWANDSLInterfaceConfigStatsTotal_Params, NULL, eObject,  0},
	{NULL}
};

/**@endobj InternetGatewayDevice.WANDevice.{i}.WANDSLInterfaceConfig.Stats.  */
/**@param InternetGatewayDevice.WANDevice.{i}.WANDSLInterfaceConfig. */
CPEGETFUNC(getWANDeviceWANDSLInterfaceConfig_Enable);
CPESETFUNC(setWANDeviceWANDSLInterfaceConfig_Enable);
CPEGETFUNC(getWANDeviceWANDSLInterfaceConfig_Status);
CPEGETFUNC(getWANDeviceWANDSLInterfaceConfig_UpstreamCurrRate);
CPEGETFUNC(getWANDeviceWANDSLInterfaceConfig_DownstreamCurrRate);
CPEGETFUNC(getWANDeviceWANDSLInterfaceConfig_UpstreamMaxRate);
CPEGETFUNC(getWANDeviceWANDSLInterfaceConfig_DownstreamMaxRate);
CPEGETFUNC(getWANDeviceWANDSLInterfaceConfig_UpstreamNoiseMargin);
CPEGETFUNC(getWANDeviceWANDSLInterfaceConfig_DownstreamNoiseMargin);
CPEGETFUNC(getWANDeviceWANDSLInterfaceConfig_UpstreamAttenuation);
CPEGETFUNC(getWANDeviceWANDSLInterfaceConfig_DownstreamAttenuation);
CPEGETFUNC(getWANDeviceWANDSLInterfaceConfig_UpstreamPower);
CPEGETFUNC(getWANDeviceWANDSLInterfaceConfig_DownstreamPower);
CPEGETFUNC(getWANDeviceWANDSLInterfaceConfig_ATURVendor);
CPEGETFUNC(getWANDeviceWANDSLInterfaceConfig_ATURCountry);
CPEGETFUNC(getWANDeviceWANDSLInterfaceConfig_ATUCVendor);
CPEGETFUNC(getWANDeviceWANDSLInterfaceConfig_ATUCCountry);
CPEGETFUNC(getWANDeviceWANDSLInterfaceConfig_TotalStart);
CPEGETFUNC(getWANDeviceWANDSLInterfaceConfig_ShowtimeStart);
CWMPParam WANDeviceWANDSLInterfaceConfig_Params[]={
	{ "Enable", getWANDeviceWANDSLInterfaceConfig_Enable, setWANDeviceWANDSLInterfaceConfig_Enable, NULL, RPC_RW, eBoolean, 0, 0},
	{ "Status", getWANDeviceWANDSLInterfaceConfig_Status, NULL, NULL, RPC_R, eString, 0, 0},
	{ "UpstreamCurrRate", getWANDeviceWANDSLInterfaceConfig_UpstreamCurrRate, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "DownstreamCurrRate", getWANDeviceWANDSLInterfaceConfig_DownstreamCurrRate, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "UpstreamMaxRate", getWANDeviceWANDSLInterfaceConfig_UpstreamMaxRate, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "DownstreamMaxRate", getWANDeviceWANDSLInterfaceConfig_DownstreamMaxRate, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "UpstreamNoiseMargin", getWANDeviceWANDSLInterfaceConfig_UpstreamNoiseMargin, NULL, NULL, RPC_R, eInt, NOACTIVENOTIFY, 0},
	{ "DownstreamNoiseMargin", getWANDeviceWANDSLInterfaceConfig_DownstreamNoiseMargin, NULL, NULL, RPC_R, eInt, NOACTIVENOTIFY, 0},
	{ "UpstreamAttenuation", getWANDeviceWANDSLInterfaceConfig_UpstreamAttenuation, NULL, NULL, RPC_R, eInt, NOACTIVENOTIFY, 0},
	{ "DownstreamAttenuation", getWANDeviceWANDSLInterfaceConfig_DownstreamAttenuation, NULL, NULL, RPC_R, eInt, NOACTIVENOTIFY, 0},
	{ "UpstreamPower", getWANDeviceWANDSLInterfaceConfig_UpstreamPower, NULL, NULL, RPC_R, eInt, NOACTIVENOTIFY, 0},
	{ "DownstreamPower", getWANDeviceWANDSLInterfaceConfig_DownstreamPower, NULL, NULL, RPC_R, eInt, NOACTIVENOTIFY, 0},
	{ "ATURVendor", getWANDeviceWANDSLInterfaceConfig_ATURVendor, NULL, NULL, RPC_R, eString, 0, 8},
	{ "ATURCountry", getWANDeviceWANDSLInterfaceConfig_ATURCountry, NULL, NULL, RPC_R, eString, 0, 4},
	{ "ATUCVendor", getWANDeviceWANDSLInterfaceConfig_ATUCVendor, NULL, NULL, RPC_R, eString, 0, 8},
	{ "ATUCCountry", getWANDeviceWANDSLInterfaceConfig_ATUCCountry, NULL, NULL, RPC_R, eString, 0, 4},
	{ "TotalStart", getWANDeviceWANDSLInterfaceConfig_TotalStart, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "ShowtimeStart", getWANDeviceWANDSLInterfaceConfig_ShowtimeStart, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{NULL}
};
/**@endparam InternetGatewayDevice.WANDevice.{i}.WANDSLInterfaceConfig.  */

/**@obj InternetGatewayDevice.WANDevice.{i}.WANDSLInterfaceConfig.  */
CWMPObject WANDeviceWANDSLInterfaceConfig_Objs[]={
	{"Stats", NULL, NULL, NULL, NULL, WANDeviceWANDSLInterfaceConfigStats_Objs, NULL, NULL, eObject,  0},
	{NULL}
};

/**@endobj InternetGatewayDevice.WANDevice.{i}.WANDSLInterfaceConfig.  */
/**@param InternetGatewayDevice.WANDevice.{i}.WANEthernetInterfaceConfig.Stats. */
CPEGETFUNC(getWANDeviceWANEthernetInterfaceConfigStats_BytesSent);
CPEGETFUNC(getWANDeviceWANEthernetInterfaceConfigStats_BytesReceived);
CPEGETFUNC(getWANDeviceWANEthernetInterfaceConfigStats_PacketsSent);
CPEGETFUNC(getWANDeviceWANEthernetInterfaceConfigStats_PacketsReceived);
CWMPParam WANDeviceWANEthernetInterfaceConfigStats_Params[]={
	{ "BytesSent", getWANDeviceWANEthernetInterfaceConfigStats_BytesSent, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "BytesReceived", getWANDeviceWANEthernetInterfaceConfigStats_BytesReceived, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "PacketsSent", getWANDeviceWANEthernetInterfaceConfigStats_PacketsSent, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "PacketsReceived", getWANDeviceWANEthernetInterfaceConfigStats_PacketsReceived, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{NULL}
};
/**@endparam InternetGatewayDevice.WANDevice.{i}.WANEthernetInterfaceConfig.Stats.  */

/**@param InternetGatewayDevice.WANDevice.{i}.WANEthernetInterfaceConfig. */
CPEGETFUNC(getWANDeviceWANEthernetInterfaceConfig_Enable);
CPESETFUNC(setWANDeviceWANEthernetInterfaceConfig_Enable);
CPEGETFUNC(getWANDeviceWANEthernetInterfaceConfig_Status);
CPEGETFUNC(getWANDeviceWANEthernetInterfaceConfig_MACAddress);
CPEGETFUNC(getWANDeviceWANEthernetInterfaceConfig_MaxBitRate);
CPESETFUNC(setWANDeviceWANEthernetInterfaceConfig_MaxBitRate);
CPEGETFUNC(getWANDeviceWANEthernetInterfaceConfig_DuplexMode);
CPESETFUNC(setWANDeviceWANEthernetInterfaceConfig_DuplexMode);
CWMPParam WANDeviceWANEthernetInterfaceConfig_Params[]={
	{ "Enable", getWANDeviceWANEthernetInterfaceConfig_Enable, setWANDeviceWANEthernetInterfaceConfig_Enable, NULL, RPC_RW, eBoolean, 0, 0},
	{ "Status", getWANDeviceWANEthernetInterfaceConfig_Status, NULL, NULL, RPC_R, eString, 0, 0},
	{ "MACAddress", getWANDeviceWANEthernetInterfaceConfig_MACAddress, NULL, NULL, RPC_R, eString, 0, 17},
	{ "MaxBitRate", getWANDeviceWANEthernetInterfaceConfig_MaxBitRate, setWANDeviceWANEthernetInterfaceConfig_MaxBitRate, NULL, RPC_RW, eString, 0, 0},
	{ "DuplexMode", getWANDeviceWANEthernetInterfaceConfig_DuplexMode, setWANDeviceWANEthernetInterfaceConfig_DuplexMode, NULL, RPC_RW, eString, 0, 0},
	{NULL}
};
/**@endparam InternetGatewayDevice.WANDevice.{i}.WANEthernetInterfaceConfig.  */

/**@obj InternetGatewayDevice.WANDevice.{i}.WANEthernetInterfaceConfig.  */
CWMPObject WANDeviceWANEthernetInterfaceConfig_Objs[]={
	{"Stats", NULL, NULL, NULL, NULL, NULL, WANDeviceWANEthernetInterfaceConfigStats_Params, NULL, eObject,  0},
	{NULL}
};

/**@endobj InternetGatewayDevice.WANDevice.{i}.WANEthernetInterfaceConfig.  */
/**@param InternetGatewayDevice.WANDevice.{i}. */
CPEGETFUNC(getWANDevice_WANConnectionNumberOfEntries);
CWMPParam WANDevice_Params[]={
	{ "WANConnectionNumberOfEntries", getWANDevice_WANConnectionNumberOfEntries, NULL, NULL, RPC_R, eUnsignedInt, 0, 0},
	{NULL}
};
/**@endparam InternetGatewayDevice.WANDevice.{i}.  */

/**@obj InternetGatewayDevice.WANDevice.{i}.  */
CPEADDOBJ(addWANDeviceWANConnectionDevice);
CPEDELOBJ(delWANDeviceWANConnectionDevice);
CPEADDOBJ(initWANDeviceWANDSLInterfaceConfig);
CPECOMMIT(commitWANDeviceWANDSLInterfaceConfig);
CPEADDOBJ(initWANDeviceWANEthernetInterfaceConfig);
CPECOMMIT(commitWANDeviceWANEthernetInterfaceConfig);
CWMPObject WANDevice_Objs[]={
	{"WANCommonInterfaceConfig", NULL, NULL, NULL, NULL, NULL, WANDeviceWANCommonInterfaceConfig_Params, NULL, eObject,  0},
	{"WANConnectionDevice", delWANDeviceWANConnectionDevice, addWANDeviceWANConnectionDevice, NULL, NULL, WANDeviceWANConnectionDevice_Objs, WANDeviceWANConnectionDevice_Params, NULL, eInstance,  0},
	{"WANDSLConnectionManagement", NULL, NULL, NULL, NULL, WANDeviceWANDSLConnectionManagement_Objs, WANDeviceWANDSLConnectionManagement_Params, NULL, eObject,  0},
	{"WANDSLInterfaceConfig", NULL, initWANDeviceWANDSLInterfaceConfig, commitWANDeviceWANDSLInterfaceConfig, NULL, WANDeviceWANDSLInterfaceConfig_Objs, WANDeviceWANDSLInterfaceConfig_Params, NULL, eObject,  0},
	{"WANEthernetInterfaceConfig", NULL, initWANDeviceWANEthernetInterfaceConfig, commitWANDeviceWANEthernetInterfaceConfig, NULL, WANDeviceWANEthernetInterfaceConfig_Objs, WANDeviceWANEthernetInterfaceConfig_Params, NULL, eObject,  0},
	{NULL}
};

/**@endobj InternetGatewayDevice.WANDevice.{i}.  */
/**@param InternetGatewayDevice. */
CPEGETFUNC(get_LANDeviceNumberOfEntries);
CPEGETFUNC(get_WANDeviceNumberOfEntries);
CPEGETFUNC(get_DeviceSummary);
CWMPParam InternetGatewayDevice_Params[]={
	{ "LANDeviceNumberOfEntries", get_LANDeviceNumberOfEntries, NULL, NULL, RPC_R, eUnsignedInt, 0, 0},
	{ "WANDeviceNumberOfEntries", get_WANDeviceNumberOfEntries, NULL, NULL, RPC_R, eUnsignedInt, 0, 0},
	{ "DeviceSummary", get_DeviceSummary, NULL, NULL, RPC_R, eString, FORCED_INFORM, 1024},
	{NULL}
};
/**@endparam InternetGatewayDevice.  */

/**@obj InternetGatewayDevice.  */
CPECOMMIT(commitIPPingDiagnostics);
CPEADDOBJ(addLANDevice);
CPEDELOBJ(delLANDevice);
CPECOMMIT(commitManagementServer);
CWMPObject InternetGatewayDevice_Objs[]={
	{"DeviceInfo", NULL, NULL, NULL, NULL, NULL, DeviceInfo_Params, NULL, eObject,  0},
	{"IPPingDiagnostics", NULL, NULL, commitIPPingDiagnostics, NULL, NULL, IPPingDiagnostics_Params, NULL, eObject,  0},
	{"LANConfigSecurity", NULL, NULL, NULL, NULL, NULL, LANConfigSecurity_Params, NULL, eObject,  0},
	{"LANDevice", delLANDevice, addLANDevice, NULL, NULL, LANDevice_Objs, LANDevice_Params, NULL, eInstance,  0},
	{"LANInterfaces", NULL, NULL, NULL, NULL, NULL, LANInterfaces_Params, NULL, eObject,  0},
	{"Layer2Bridging", NULL, NULL, NULL, NULL, Layer2Bridging_Objs, Layer2Bridging_Params, NULL, eObject,  0},
	{"Layer3Forwarding", NULL, NULL, NULL, NULL, Layer3Forwarding_Objs, Layer3Forwarding_Params, NULL, eObject,  0},
	{"ManagementServer", NULL, NULL, commitManagementServer, NULL, ManagementServer_Objs, ManagementServer_Params, NULL, eObject,  0},
	{"Time", NULL, NULL, NULL, NULL, NULL, Time_Params, NULL, eObject,  0},
	{"TraceRouteDiagnostics", NULL, NULL, NULL, NULL, TraceRouteDiagnostics_Objs, TraceRouteDiagnostics_Params, NULL, eObject,  0},
	{"WANDevice", NULL, NULL, NULL, NULL, WANDevice_Objs, WANDevice_Params, NULL, eInstance,  0},
	{NULL}
};

/**@endobj InternetGatewayDevice.  */
/** CWMP ROOT Object Table  */

CWMPObject CWMP_RootObject[]={
	{"InternetGatewayDevice", NULL, NULL, NULL, NULL, InternetGatewayDevice_Objs, InternetGatewayDevice_Params, NULL, eObject, 0},
	{NULL}
};

