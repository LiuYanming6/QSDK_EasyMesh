/*----------------------------------------------------------------------*
 * Gatespace
 * Copyright 2011, 2012 Gatespace Networks, Inc., All Rights Reserved.
 * Gatespace Networks, Inc. confidential material.
 *----------------------------------------------------------------------*
 * File Name  : 
 * Description: Auto-Generated C tables file.
 *----------------------------------------------------------------------*
 * $Revision: 1.2 $
 *
 * $Id: tables.c,v 1.2 2012/06/13 16:07:50 dmounday Exp $
 *----------------------------------------------------------------------*/

#include "paramTree.h"
#include "rpc.h"



/**  TraceRoute:1, UDPConnReq:1, Hosts:1, SM_Baseline:1, EthernetLink:1, 
    EthernetInterface:1, IPInterface:1, IPPing:1, IPv6Interface:1, Baseline:2, 
    Baseline:1, 
**/
#if 0
/**@param Device.DNS.Client.Server.{i}. */
CPEGETFUNC(getDNSClientServer_Enable);
CPESETFUNC(setDNSClientServer_Enable);
CPEGETFUNC(getDNSClientServer_Status);
CPEGETFUNC(getDNSClientServer_DNSServer);
CPESETFUNC(setDNSClientServer_DNSServer);
CPEGETFUNC(getDNSClientServer_Interface);
CPESETFUNC(setDNSClientServer_Interface);
CPEGETFUNC(getDNSClientServer_Type);
CWMPParam DNSClientServer_Params[]={
	{ "Enable", getDNSClientServer_Enable, setDNSClientServer_Enable, NULL, RPC_RW, eBoolean, 0, 0},
	{ "Status", getDNSClientServer_Status, NULL, NULL, RPC_R, eString, 0, 0},
	{ "Alias", cwmpGetAliasParam, cwmpSetAliasParam, NULL, RPC_RW, eString, NOACTIVENOTIFY, 64},
	{ "DNSServer", getDNSClientServer_DNSServer, setDNSClientServer_DNSServer, NULL, RPC_RW, eString, 0, 45},
	{ "Interface", getDNSClientServer_Interface, setDNSClientServer_Interface, NULL, RPC_RW,  eString, 0, 256},
	{ "Type", getDNSClientServer_Type, NULL, NULL, RPC_R, eString, 0, 0},
	{NULL}
};
/**@endparam Device.DNS.Client.Server.{i}.  */

/**@param Device.DNS.Client. */
CPEGETFUNC(getDNSClient_Enable);
CPESETFUNC(setDNSClient_Enable);
CPEGETFUNC(getDNSClient_Status);
CPEGETFUNC(getDNSClient_ServerNumberOfEntries);
CWMPParam DNSClient_Params[]={
	{ "Enable", getDNSClient_Enable, setDNSClient_Enable, NULL, RPC_RW, eBoolean, 0, 0},
	{ "Status", getDNSClient_Status, NULL, NULL, RPC_R, eString, 0, 0},
	{ "ServerNumberOfEntries", getDNSClient_ServerNumberOfEntries, NULL, NULL, RPC_R, eUnsignedInt, 0, 0},
	{NULL}
};
/**@endparam Device.DNS.Client.  */

/**@obj Device.DNS.Client.  */
CPEADDOBJ(addDNSClientServer);
CPEDELOBJ(delDNSClientServer);
CWMPObject DNSClient_Objs[]={
	{"Server", delDNSClientServer, addDNSClientServer, NULL, NULL, NULL, DNSClientServer_Params, NULL, eInstance,  0},
	{NULL}
};

/**@endobj Device.DNS.Client.  */
/**@param Device.DNS. */
CPEGETFUNC(getDNS_SupportedRecordTypes);
CWMPParam DNS_Params[]={
	{ "SupportedRecordTypes", getDNS_SupportedRecordTypes, NULL, NULL, RPC_R, eString, 0, 0},
	{NULL}
};
/**@endparam Device.DNS.  */

/**@obj Device.DNS.  */
CWMPObject DNS_Objs[]={
	{"Client", NULL, NULL, NULL, NULL, DNSClient_Objs, DNSClient_Params, NULL, eObject,  0},
	{NULL}
};

/**@endobj Device.DNS.  */
#endif


/**@endobj Device.DeviceInfo.ProcessStatus.  */
/**@param Device.DeviceInfo.VendorLogFile.{i}. */
CPEGETFUNC(getDeviceInfoVendorLogFile_Name);
CPEGETFUNC(getDeviceInfoVendorLogFile_MaximumSize);
CPEGETFUNC(getDeviceInfoVendorLogFile_Persistent);
CWMPParam DeviceInfoVendorLogFile_Params[]={
        { "Name", getDeviceInfoVendorLogFile_Name, NULL, NULL, RPC_R, eString, 0, 64},
        { "MaximumSize", getDeviceInfoVendorLogFile_MaximumSize, NULL, NULL, RPC_R, eUnsignedInt, 0, 0},
        { "Persistent", getDeviceInfoVendorLogFile_Persistent, NULL, NULL, RPC_R, eBoolean, 0, 0},
        {NULL}
};
/**@endparam Device.DeviceInfo.VendorLogFile.{i}.  */


/**@param Device.DeviceInfo. */
CPEGETFUNC(getDeviceInfo_Manufacturer);
CPEGETFUNC(getDeviceInfo_ManufacturerOUI);
CPEGETFUNC(getDeviceInfo_ModelName);
CPEGETFUNC(getDeviceInfo_ProductClass);
CPEGETFUNC(getDeviceInfo_Description);
CPEGETFUNC(getDeviceInfo_SerialNumber);
CPEGETFUNC(getDeviceInfo_HardwareVersion);
CPEGETFUNC(getDeviceInfo_SoftwareVersion);
CPESETFUNC(setDeviceInfo_SoftwareVersion);
CPEGETFUNC(getDeviceInfo_ProvisioningCode);
CPESETFUNC(setDeviceInfo_ProvisioningCode);
CPEGETFUNC(getDeviceInfo_UpTime);
CWMPParam DeviceInfo_Params[]={
	{ "Manufacturer", getDeviceInfo_Manufacturer, NULL, NULL, RPC_R, eString, 0, 64},
	{ "ManufacturerOUI", getDeviceInfo_ManufacturerOUI, NULL, NULL, RPC_R, eString, 0, 6},
	{ "ModelName", getDeviceInfo_ModelName, NULL, NULL, RPC_R, eString, NOACTIVENOTIFY, 64},
	{ "ProductClass", getDeviceInfo_ProductClass, NULL, NULL, RPC_R, eString, NOACTIVENOTIFY, 64},
	{ "Description", getDeviceInfo_Description, NULL, NULL, RPC_R, eString, NOACTIVENOTIFY, 256},
	{ "SerialNumber", getDeviceInfo_SerialNumber, NULL, NULL, RPC_R, eString, 0, 64},
	{ "HardwareVersion", getDeviceInfo_HardwareVersion, NULL, NULL, RPC_R, eString, FORCED_INFORM, 64},
	{ "SoftwareVersion", getDeviceInfo_SoftwareVersion, setDeviceInfo_SoftwareVersion, NULL, RPC_R, eString, FORCED_INFORM|FORCED_ACTIVE, 64},
	{ "ProvisioningCode", getDeviceInfo_ProvisioningCode, setDeviceInfo_ProvisioningCode, NULL, RPC_RW, eString, FORCED_INFORM|FORCED_ACTIVE, 64},
	{ "UpTime", getDeviceInfo_UpTime, NULL, NULL, RPC_R, eUnsignedInt, 0, 0},
	{NULL}
};
/**@endparam Device.DeviceInfo.  */

/**@obj Device.DeviceInfo.  */
CWMPObject DeviceInfo_Objs[]={
        {"VendorLogFile", NULL, NULL, NULL, NULL, NULL, DeviceInfoVendorLogFile_Params, NULL, eInstance,  0},
        {NULL}
};
/**@endobj Device.DeviceInfo.  */


/**@param Device.Ethernet.Interface.{i}.Stats. */
CPEGETFUNC(getEthernetInterfaceStats_BytesSent);
CPEGETFUNC(getEthernetInterfaceStats_BytesReceived);
CPEGETFUNC(getEthernetInterfaceStats_PacketsSent);
CPEGETFUNC(getEthernetInterfaceStats_PacketsReceived);
CPEGETFUNC(getEthernetInterfaceStats_ErrorsSent);
CPEGETFUNC(getEthernetInterfaceStats_ErrorsReceived);
CPEGETFUNC(getEthernetInterfaceStats_UnicastPacketsSent);
CPEGETFUNC(getEthernetInterfaceStats_UnicastPacketsReceived);
CPEGETFUNC(getEthernetInterfaceStats_DiscardPacketsSent);
CPEGETFUNC(getEthernetInterfaceStats_DiscardPacketsReceived);
CPEGETFUNC(getEthernetInterfaceStats_MulticastPacketsSent);
CPEGETFUNC(getEthernetInterfaceStats_MulticastPacketsReceived);
CPEGETFUNC(getEthernetInterfaceStats_BroadcastPacketsSent);
CPEGETFUNC(getEthernetInterfaceStats_BroadcastPacketsReceived);
CPEGETFUNC(getEthernetInterfaceStats_UnknownProtoPacketsReceived);
CWMPParam EthernetInterfaceStats_Params[]={
	{ "BytesSent", getEthernetInterfaceStats_BytesSent, NULL, NULL, RPC_R, eUnsignedLong, NOACTIVENOTIFY, 0},
	{ "BytesReceived", getEthernetInterfaceStats_BytesReceived, NULL, NULL, RPC_R, eUnsignedLong, NOACTIVENOTIFY, 0},
	{ "PacketsSent", getEthernetInterfaceStats_PacketsSent, NULL, NULL, RPC_R, eUnsignedLong, NOACTIVENOTIFY, 0},
	{ "PacketsReceived", getEthernetInterfaceStats_PacketsReceived, NULL, NULL, RPC_R, eUnsignedLong, NOACTIVENOTIFY, 0},
	{ "ErrorsSent", getEthernetInterfaceStats_ErrorsSent, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "ErrorsReceived", getEthernetInterfaceStats_ErrorsReceived, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "UnicastPacketsSent", getEthernetInterfaceStats_UnicastPacketsSent, NULL, NULL, RPC_R, eUnsignedLong, NOACTIVENOTIFY, 0},
	{ "UnicastPacketsReceived", getEthernetInterfaceStats_UnicastPacketsReceived, NULL, NULL, RPC_R, eUnsignedLong, NOACTIVENOTIFY, 0},
	{ "DiscardPacketsSent", getEthernetInterfaceStats_DiscardPacketsSent, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "DiscardPacketsReceived", getEthernetInterfaceStats_DiscardPacketsReceived, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "MulticastPacketsSent", getEthernetInterfaceStats_MulticastPacketsSent, NULL, NULL, RPC_R, eUnsignedLong, NOACTIVENOTIFY, 0},
	{ "MulticastPacketsReceived", getEthernetInterfaceStats_MulticastPacketsReceived, NULL, NULL, RPC_R, eUnsignedLong, NOACTIVENOTIFY, 0},
	{ "BroadcastPacketsSent", getEthernetInterfaceStats_BroadcastPacketsSent, NULL, NULL, RPC_R, eUnsignedLong, NOACTIVENOTIFY, 0},
	{ "BroadcastPacketsReceived", getEthernetInterfaceStats_BroadcastPacketsReceived, NULL, NULL, RPC_R, eUnsignedLong, NOACTIVENOTIFY, 0},
	{ "UnknownProtoPacketsReceived", getEthernetInterfaceStats_UnknownProtoPacketsReceived, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{NULL}
};
/**@endparam Device.Ethernet.Interface.{i}.Stats.  */

/**@param Device.Ethernet.Interface.{i}. */
CPEGETFUNC(getEthernetInterface_Enable);
CPESETFUNC(setEthernetInterface_Enable);
CPEGETFUNC(getEthernetInterface_Status);
CPEGETFUNC(getEthernetInterface_Name);
CPEGETFUNC(getEthernetInterface_LastChange);
CPEGETFUNC(getEthernetInterface_Upstream);
CPEGETFUNC(getEthernetInterface_MACAddress);
CPEGETFUNC(getEthernetInterface_MaxBitRate);
CPESETFUNC(setEthernetInterface_MaxBitRate);
CPEGETFUNC(getEthernetInterface_DuplexMode);
CPESETFUNC(setEthernetInterface_DuplexMode);
CWMPParam EthernetInterface_Params[]={
	{ "Enable", getEthernetInterface_Enable, setEthernetInterface_Enable, NULL, RPC_RW, eBoolean, 0, 0},
	{ "Status", getEthernetInterface_Status, NULL, NULL, RPC_R, eString, 0, 0},
	{ "Alias", cwmpGetAliasParam, cwmpSetAliasParam, NULL, RPC_RW, eString, NOACTIVENOTIFY, 64},
	{ "Name", getEthernetInterface_Name, NULL, NULL, RPC_R, eString, 0, 64},
	{ "LastChange", getEthernetInterface_LastChange, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "Upstream", getEthernetInterface_Upstream, NULL, NULL, RPC_R, eBoolean, 0, 0},
	{ "MACAddress", getEthernetInterface_MACAddress, NULL, NULL, RPC_R, eString, 0, 17},
	{ "MaxBitRate", getEthernetInterface_MaxBitRate, setEthernetInterface_MaxBitRate, NULL, RPC_RW, eInt, 0, 0},
	{ "DuplexMode", getEthernetInterface_DuplexMode, setEthernetInterface_DuplexMode, NULL, RPC_RW, eString, 0, 0},
	{NULL}
};
/**@endparam Device.Ethernet.Interface.{i}.  */
#if 0
/**@obj Device.Ethernet.Interface.{i}.  */
CPEADDOBJ(initEthernetInterfaceStats);
CWMPObject EthernetInterface_Objs[]={
	{"Stats", NULL, initEthernetInterfaceStats, NULL, NULL, NULL, EthernetInterfaceStats_Params, NULL, eObject,  0},
	{NULL}
};
#endif
/**@endobj Device.Ethernet.Interface.{i}.  */
/**@param Device.Ethernet.Link.{i}.Stats. */
CPEGETFUNC(getEthernetLinkStats_BytesSent);
CPEGETFUNC(getEthernetLinkStats_BytesReceived);
CPEGETFUNC(getEthernetLinkStats_PacketsSent);
CPEGETFUNC(getEthernetLinkStats_PacketsReceived);
CPEGETFUNC(getEthernetLinkStats_ErrorsSent);
CPEGETFUNC(getEthernetLinkStats_ErrorsReceived);
CPEGETFUNC(getEthernetLinkStats_UnicastPacketsSent);
CPEGETFUNC(getEthernetLinkStats_UnicastPacketsReceived);
CPEGETFUNC(getEthernetLinkStats_DiscardPacketsSent);
CPEGETFUNC(getEthernetLinkStats_DiscardPacketsReceived);
CPEGETFUNC(getEthernetLinkStats_MulticastPacketsSent);
CPEGETFUNC(getEthernetLinkStats_MulticastPacketsReceived);
CPEGETFUNC(getEthernetLinkStats_BroadcastPacketsSent);
CPEGETFUNC(getEthernetLinkStats_BroadcastPacketsReceived);
CPEGETFUNC(getEthernetLinkStats_UnknownProtoPacketsReceived);
CWMPParam EthernetLinkStats_Params[]={
	{ "BytesSent", getEthernetLinkStats_BytesSent, NULL, NULL, RPC_R, eUnsignedLong, NOACTIVENOTIFY, 0},
	{ "BytesReceived", getEthernetLinkStats_BytesReceived, NULL, NULL, RPC_R, eUnsignedLong, NOACTIVENOTIFY, 0},
	{ "PacketsSent", getEthernetLinkStats_PacketsSent, NULL, NULL, RPC_R, eUnsignedLong, NOACTIVENOTIFY, 0},
	{ "PacketsReceived", getEthernetLinkStats_PacketsReceived, NULL, NULL, RPC_R, eUnsignedLong, NOACTIVENOTIFY, 0},
	{ "ErrorsSent", getEthernetLinkStats_ErrorsSent, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "ErrorsReceived", getEthernetLinkStats_ErrorsReceived, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "UnicastPacketsSent", getEthernetLinkStats_UnicastPacketsSent, NULL, NULL, RPC_R, eUnsignedLong, NOACTIVENOTIFY, 0},
	{ "UnicastPacketsReceived", getEthernetLinkStats_UnicastPacketsReceived, NULL, NULL, RPC_R, eUnsignedLong, NOACTIVENOTIFY, 0},
	{ "DiscardPacketsSent", getEthernetLinkStats_DiscardPacketsSent, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "DiscardPacketsReceived", getEthernetLinkStats_DiscardPacketsReceived, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "MulticastPacketsSent", getEthernetLinkStats_MulticastPacketsSent, NULL, NULL, RPC_R, eUnsignedLong, NOACTIVENOTIFY, 0},
	{ "MulticastPacketsReceived", getEthernetLinkStats_MulticastPacketsReceived, NULL, NULL, RPC_R, eUnsignedLong, NOACTIVENOTIFY, 0},
	{ "BroadcastPacketsSent", getEthernetLinkStats_BroadcastPacketsSent, NULL, NULL, RPC_R, eUnsignedLong, NOACTIVENOTIFY, 0},
	{ "BroadcastPacketsReceived", getEthernetLinkStats_BroadcastPacketsReceived, NULL, NULL, RPC_R, eUnsignedLong, NOACTIVENOTIFY, 0},
	{ "UnknownProtoPacketsReceived", getEthernetLinkStats_UnknownProtoPacketsReceived, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{NULL}
};
/**@endparam Device.Ethernet.Link.{i}.Stats.  */

/**@param Device.Ethernet.Link.{i}. */
CPEGETFUNC(getEthernetLink_Enable);
//CPESETFUNC(setEthernetLink_Enable);
CPEGETFUNC(getEthernetLink_Status);
CPEGETFUNC(getEthernetLink_Name);
CPEGETFUNC(getEthernetLink_LastChange);
CPEGETFUNC(getEthernetLink_LowerLayers);
CPESETFUNC(setEthernetLink_LowerLayers);
CPEGETFUNC(getEthernetLink_MACAddress);
CWMPParam EthernetLink_Params[]={
	{ "Enable", getEthernetLink_Enable, NULL, NULL, RPC_R, eBoolean, 0, 0},
	{ "Status", getEthernetLink_Status, NULL, NULL, RPC_R, eString, 0, 0},
	{ "Alias", cwmpGetAliasParam, cwmpSetAliasParam, NULL, RPC_RW, eString, NOACTIVENOTIFY, 64},
	{ "Name", getEthernetLink_Name, NULL, NULL, RPC_R, eString, 0, 64},
	{ "LastChange", getEthernetLink_LastChange, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "LowerLayers", getEthernetLink_LowerLayers, setEthernetLink_LowerLayers, NULL, RPC_RW,  eString, 0, 0},
	{ "MACAddress", getEthernetLink_MACAddress, NULL, NULL, RPC_R, eString, 0, 17},
	{NULL}
};
/**@endparam Device.Ethernet.Link.{i}.  */

#if 0
/**@obj Device.Ethernet.Link.{i}.  */
CPEADDOBJ(initEthernetLinkStats);
CWMPObject EthernetLink_Objs[]={
	{"Stats", NULL, initEthernetLinkStats, NULL, NULL, NULL, EthernetLinkStats_Params, NULL, eObject,  0},
	{NULL}
};
#endif
/**@endobj Device.Ethernet.Link.{i}.  */
/**@param Device.Ethernet. */
CPEGETFUNC(getEthernet_InterfaceNumberOfEntries);
CPEGETFUNC(getEthernet_LinkNumberOfEntries);
CWMPParam Ethernet_Params[]={
	{ "InterfaceNumberOfEntries", getEthernet_InterfaceNumberOfEntries, NULL, NULL, RPC_R, eUnsignedInt, 0, 0},
	{ "LinkNumberOfEntries", getEthernet_LinkNumberOfEntries, NULL, NULL, RPC_R, eUnsignedInt, 0, 0},
	{NULL}
};
/**@endparam Device.Ethernet.  */

/**@obj Device.Ethernet.  */
CPEADDOBJ(addEthernetInterface);
CPEADDOBJ(addEthernetLink);
CPEDELOBJ(delEthernetLink);
CWMPObject Ethernet_Objs[]={
	{"Interface", NULL, addEthernetInterface, NULL, NULL, NULL, EthernetInterface_Params, NULL, eStaticInstance,  0},
	{"Link", delEthernetLink, addEthernetLink, NULL, NULL, NULL, EthernetLink_Params, NULL, eInstance,  0},
	{NULL}
};

/**@endobj Device.Ethernet.  */
#if 0
/**@param Device.Hosts.Host.{i}. */
CPEGETFUNC(getHostsHost_PhysAddress);
CPEGETFUNC(getHostsHost_IPAddress);
CPEGETFUNC(getHostsHost_AddressSource);
CPEGETFUNC(getHostsHost_LeaseTimeRemaining);
CPEGETFUNC(getHostsHost_Layer1Interface);
CPEGETFUNC(getHostsHost_Layer3Interface);
CPEGETFUNC(getHostsHost_HostName);
CPEGETFUNC(getHostsHost_Active);
CWMPParam HostsHost_Params[]={
	{ "PhysAddress", getHostsHost_PhysAddress, NULL, NULL, RPC_R, eString, 0, 64},
	{ "IPAddress", getHostsHost_IPAddress, NULL, NULL, RPC_R, eString, 0, 45},
	{ "AddressSource", getHostsHost_AddressSource, NULL, NULL, RPC_R, eString, 0, 0},
	{ "LeaseTimeRemaining", getHostsHost_LeaseTimeRemaining, NULL, NULL, RPC_R, eInt, 0, 0},
	{ "Layer1Interface", getHostsHost_Layer1Interface, NULL, NULL, RPC_R,  eString, 0, 256},
	{ "Layer3Interface", getHostsHost_Layer3Interface, NULL, NULL, RPC_R,  eString, 0, 256},
	{ "HostName", getHostsHost_HostName, NULL, NULL, RPC_R, eString, 0, 64},
	{ "Active", getHostsHost_Active, NULL, NULL, RPC_R, eBoolean, 0, 0},
	{NULL}
};
/**@endparam Device.Hosts.Host.{i}.  */
#endif
#if 0
/**@param Device.Hosts. */
CPEGETFUNC(getHosts_HostNumberOfEntries);
CWMPParam Hosts_Params[]={
	{ "HostNumberOfEntries", getHosts_HostNumberOfEntries, NULL, NULL, RPC_R, eUnsignedInt, 0, 0},
	{NULL}
};
/**@endparam Device.Hosts.  */
#endif

#if 0
/**@obj Device.Hosts.  */
CPEADDOBJ(addHostsHost);
CPEDELOBJ(delHostsHost);
CWMPObject Hosts_Objs[]={
	{"Host", delHostsHost, addHostsHost, NULL, NULL, NULL, HostsHost_Params, NULL, eCPEInstance,  0},
	{NULL}
};
#endif

/**@endobj Device.Hosts.  */
/**@param Device.IP.Diagnostics.IPPing. */
CPEGETFUNC(getIPDiagnosticsIPPing_DiagnosticsState);
CPESETFUNC(setIPDiagnosticsIPPing_DiagnosticsState);
CPEGETFUNC(getIPDiagnosticsIPPing_Interface);
CPESETFUNC(setIPDiagnosticsIPPing_Interface);
CPEGETFUNC(getIPDiagnosticsIPPing_Host);
CPESETFUNC(setIPDiagnosticsIPPing_Host);
CPEGETFUNC(getIPDiagnosticsIPPing_NumberOfRepetitions);
CPESETFUNC(setIPDiagnosticsIPPing_NumberOfRepetitions);
CPEGETFUNC(getIPDiagnosticsIPPing_Timeout);
CPESETFUNC(setIPDiagnosticsIPPing_Timeout);
CPEGETFUNC(getIPDiagnosticsIPPing_DataBlockSize);
CPESETFUNC(setIPDiagnosticsIPPing_DataBlockSize);
CPEGETFUNC(getIPDiagnosticsIPPing_DSCP);
CPESETFUNC(setIPDiagnosticsIPPing_DSCP);
CPEGETFUNC(getIPDiagnosticsIPPing_SuccessCount);
CPEGETFUNC(getIPDiagnosticsIPPing_FailureCount);
CPEGETFUNC(getIPDiagnosticsIPPing_AverageResponseTime);
CPEGETFUNC(getIPDiagnosticsIPPing_MinimumResponseTime);
CPEGETFUNC(getIPDiagnosticsIPPing_MaximumResponseTime);
CWMPParam IPDiagnosticsIPPing_Params[]={
	{ "DiagnosticsState", getIPDiagnosticsIPPing_DiagnosticsState, setIPDiagnosticsIPPing_DiagnosticsState, NULL, RPC_RW, eString, NOACTIVENOTIFY, 0},
	{ "Interface", getIPDiagnosticsIPPing_Interface, setIPDiagnosticsIPPing_Interface, NULL, RPC_RW,  eString, 0, 256},
	{ "Host", getIPDiagnosticsIPPing_Host, setIPDiagnosticsIPPing_Host, NULL, RPC_RW, eString, 0, 256},
	{ "NumberOfRepetitions", getIPDiagnosticsIPPing_NumberOfRepetitions, setIPDiagnosticsIPPing_NumberOfRepetitions, NULL, RPC_RW, eUnsignedInt, 0, 0},
	{ "Timeout", getIPDiagnosticsIPPing_Timeout, setIPDiagnosticsIPPing_Timeout, NULL, RPC_RW, eUnsignedInt, 0, 0},
	{ "DataBlockSize", getIPDiagnosticsIPPing_DataBlockSize, setIPDiagnosticsIPPing_DataBlockSize, NULL, RPC_RW, eUnsignedInt, 0, 0},
	{ "DSCP", getIPDiagnosticsIPPing_DSCP, setIPDiagnosticsIPPing_DSCP, NULL, RPC_RW, eUnsignedInt, 0, 0},
	{ "SuccessCount", getIPDiagnosticsIPPing_SuccessCount, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "FailureCount", getIPDiagnosticsIPPing_FailureCount, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "AverageResponseTime", getIPDiagnosticsIPPing_AverageResponseTime, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "MinimumResponseTime", getIPDiagnosticsIPPing_MinimumResponseTime, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "MaximumResponseTime", getIPDiagnosticsIPPing_MaximumResponseTime, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{NULL}
};
/**@endparam Device.IP.Diagnostics.IPPing.  */

/**@param Device.IP.Diagnostics.TraceRoute.RouteHops.{i}. */
CPEGETFUNC(getIPDiagnosticsTraceRouteRouteHops_Host);
CPEGETFUNC(getIPDiagnosticsTraceRouteRouteHops_HostAddress);
CPEGETFUNC(getIPDiagnosticsTraceRouteRouteHops_ErrorCode);
CPEGETFUNC(getIPDiagnosticsTraceRouteRouteHops_RTTimes);
CWMPParam IPDiagnosticsTraceRouteRouteHops_Params[]={
	{ "Host", getIPDiagnosticsTraceRouteRouteHops_Host, NULL, NULL, RPC_R, eString, NOACTIVENOTIFY, 256},
	{ "HostAddress", getIPDiagnosticsTraceRouteRouteHops_HostAddress, NULL, NULL, RPC_R, eString, NOACTIVENOTIFY, 0},
	{ "ErrorCode", getIPDiagnosticsTraceRouteRouteHops_ErrorCode, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "RTTimes", getIPDiagnosticsTraceRouteRouteHops_RTTimes, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{NULL}
};
/**@endparam Device.IP.Diagnostics.TraceRoute.RouteHops.{i}.  */

/**@param Device.IP.Diagnostics.TraceRoute. */
CPEGETFUNC(getIPDiagnosticsTraceRoute_DiagnosticsState);
CPESETFUNC(setIPDiagnosticsTraceRoute_DiagnosticsState);
CPEGETFUNC(getIPDiagnosticsTraceRoute_Interface);
CPESETFUNC(setIPDiagnosticsTraceRoute_Interface);
CPEGETFUNC(getIPDiagnosticsTraceRoute_Host);
CPESETFUNC(setIPDiagnosticsTraceRoute_Host);
CPEGETFUNC(getIPDiagnosticsTraceRoute_NumberOfTries);
CPESETFUNC(setIPDiagnosticsTraceRoute_NumberOfTries);
CPEGETFUNC(getIPDiagnosticsTraceRoute_Timeout);
CPESETFUNC(setIPDiagnosticsTraceRoute_Timeout);
#if 0
CPEGETFUNC(getIPDiagnosticsTraceRoute_DataBlockSize);
CPESETFUNC(setIPDiagnosticsTraceRoute_DataBlockSize);
CPEGETFUNC(getIPDiagnosticsTraceRoute_DSCP);
CPESETFUNC(setIPDiagnosticsTraceRoute_DSCP);
#endif
CPEGETFUNC(getIPDiagnosticsTraceRoute_MaxHopCount);
CPESETFUNC(setIPDiagnosticsTraceRoute_MaxHopCount);
CPEGETFUNC(getIPDiagnosticsTraceRoute_ResponseTime);
CPEGETFUNC(getIPDiagnosticsTraceRoute_RouteHopsNumberOfEntries);
CWMPParam IPDiagnosticsTraceRoute_Params[]={
	{ "DiagnosticsState", getIPDiagnosticsTraceRoute_DiagnosticsState, setIPDiagnosticsTraceRoute_DiagnosticsState, NULL, RPC_RW, eString, NOACTIVENOTIFY, 0},
	{ "Interface", getIPDiagnosticsTraceRoute_Interface, setIPDiagnosticsTraceRoute_Interface, NULL, RPC_RW,  eString, 0, 256},
	{ "Host", getIPDiagnosticsTraceRoute_Host, setIPDiagnosticsTraceRoute_Host, NULL, RPC_RW, eString, 0, 256},
	{ "NumberOfTries", getIPDiagnosticsTraceRoute_NumberOfTries, setIPDiagnosticsTraceRoute_NumberOfTries, NULL, RPC_RW, eUnsignedInt, 0, 0},
	{ "Timeout", getIPDiagnosticsTraceRoute_Timeout, setIPDiagnosticsTraceRoute_Timeout, NULL, RPC_RW, eUnsignedInt, 0, 0},
#if 0
	{ "DataBlockSize", getIPDiagnosticsTraceRoute_DataBlockSize, setIPDiagnosticsTraceRoute_DataBlockSize, NULL, RPC_RW, eUnsignedInt, 0, 0},
	{ "DSCP", getIPDiagnosticsTraceRoute_DSCP, setIPDiagnosticsTraceRoute_DSCP, NULL, RPC_RW, eUnsignedInt, 0, 0},
#endif
	{ "MaxHopCount", getIPDiagnosticsTraceRoute_MaxHopCount, setIPDiagnosticsTraceRoute_MaxHopCount, NULL, RPC_RW, eUnsignedInt, 0, 0},
	{ "ResponseTime", getIPDiagnosticsTraceRoute_ResponseTime, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "RouteHopsNumberOfEntries", getIPDiagnosticsTraceRoute_RouteHopsNumberOfEntries, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{NULL}
};
/**@endparam Device.IP.Diagnostics.TraceRoute.  */

/**@obj Device.IP.Diagnostics.TraceRoute.  */
CPEADDOBJ(addIPDiagnosticsTraceRouteRouteHops);
CPEDELOBJ(delIPDiagnosticsTraceRouteRouteHops);
CWMPObject IPDiagnosticsTraceRoute_Objs[]={
	{"RouteHops", delIPDiagnosticsTraceRouteRouteHops, addIPDiagnosticsTraceRouteRouteHops, NULL, NULL, NULL, IPDiagnosticsTraceRouteRouteHops_Params, NULL, eCPEInstance,  0},
	{NULL}
};

/**@endobj Device.IP.Diagnostics.TraceRoute.  */
/**@obj Device.IP.Diagnostics.  */
CPEADDOBJ(initIPDiagnosticsIPPing);
CPECOMMIT(commitIPDiagnosticsIPPing);
CPEADDOBJ(initIPDiagnosticsTraceRoute);
CPECOMMIT(commitIPDiagnosticsTraceRoute);
CWMPObject IPDiagnostics_Objs[]={
	{"IPPing", NULL, initIPDiagnosticsIPPing, commitIPDiagnosticsIPPing, NULL, NULL, IPDiagnosticsIPPing_Params, NULL, eObject,  0},
	{"TraceRoute", NULL, initIPDiagnosticsTraceRoute, commitIPDiagnosticsTraceRoute, NULL, IPDiagnosticsTraceRoute_Objs, IPDiagnosticsTraceRoute_Params, NULL, eObject,  0},
	{NULL}
};

/**@endobj Device.IP.Diagnostics.  */
/**@param Device.IP.Interface.{i}.IPv4Address.{i}. */
CPEGETFUNC(getIPInterfaceIPv4Address_Enable);
CPEGETFUNC(getIPInterfaceIPv4Address_Status);
CPEGETFUNC(getIPInterfaceIPv4Address_IPAddress);
CPEGETFUNC(getIPInterfaceIPv4Address_SubnetMask);
CPEGETFUNC(getIPInterfaceIPv4Address_AddressingType);
CWMPParam IPInterfaceIPv4Address_Params[]={
	{ "Enable", getIPInterfaceIPv4Address_Enable, NULL, NULL, RPC_R, eBoolean, 0, 0},
	{ "Status", getIPInterfaceIPv4Address_Status, NULL, NULL, RPC_R, eString, 0, 0},
//	{ "Alias", cwmpGetAliasParam, cwmpSetAliasParam, NULL, RPC_RW, eString, NOACTIVENOTIFY, 64},
	{ "IPAddress", getIPInterfaceIPv4Address_IPAddress, NULL, NULL, RPC_R, eString, 0, 45},
	{ "SubnetMask", getIPInterfaceIPv4Address_SubnetMask, NULL, NULL, RPC_R, eString, 0, 45},
	{ "AddressingType", getIPInterfaceIPv4Address_AddressingType, NULL, NULL, RPC_R, eString, 0, 0},
	{NULL}
};
/**@endparam Device.IP.Interface.{i}.IPv4Address.{i}.  */

/**@param Device.IP.Interface.{i}.IPv6Address.{i}. */
CPEGETFUNC(getIPInterfaceIPv6Address_Enable);
CPEGETFUNC(getIPInterfaceIPv6Address_Status);
CPEGETFUNC(getIPInterfaceIPv6Address_IPAddressStatus);
CPEGETFUNC(getIPInterfaceIPv6Address_IPAddress);
//CPEGETFUNC(getIPInterfaceIPv6Address_Origin);
//CPEGETFUNC(getIPInterfaceIPv6Address_Prefix);
//CPESETFUNC(setIPInterfaceIPv6Address_Prefix);
//CPEGETFUNC(getIPInterfaceIPv6Address_PreferredLifetime);
//CPESETFUNC(setIPInterfaceIPv6Address_PreferredLifetime);
//CPEGETFUNC(getIPInterfaceIPv6Address_ValidLifetime);
//CPESETFUNC(setIPInterfaceIPv6Address_ValidLifetime);
//CPEGETFUNC(getIPInterfaceIPv6Address_Anycast);
//CPESETFUNC(setIPInterfaceIPv6Address_Anycast);
CWMPParam IPInterfaceIPv6Address_Params[]={
	{ "Enable", getIPInterfaceIPv6Address_Enable, NULL, NULL, RPC_R, eBoolean, 0, 0},
	{ "Status", getIPInterfaceIPv6Address_Status, NULL, NULL, RPC_R, eString, 0, 0},
	{ "IPAddressStatus", getIPInterfaceIPv6Address_IPAddressStatus, NULL, NULL, RPC_R, eString, 0, 0},
//	{ "Alias", cwmpGetAliasParam, cwmpSetAliasParam, NULL, RPC_RW, eString, NOACTIVENOTIFY, 64},
	{ "IPAddress", getIPInterfaceIPv6Address_IPAddress, NULL, NULL, RPC_R, eString, 0, 45},
//	{ "Origin", getIPInterfaceIPv6Address_Origin, NULL, NULL, RPC_R, eString, 0, 0},
//	{ "Prefix", getIPInterfaceIPv6Address_Prefix, setIPInterfaceIPv6Address_Prefix, NULL, RPC_RW,  eString, 0, 0},
//	{ "PreferredLifetime", getIPInterfaceIPv6Address_PreferredLifetime, setIPInterfaceIPv6Address_PreferredLifetime, NULL, RPC_RW, eDateTime, 0, 0},
//	{ "ValidLifetime", getIPInterfaceIPv6Address_ValidLifetime, setIPInterfaceIPv6Address_ValidLifetime, NULL, RPC_RW, eDateTime, 0, 0},
//	{ "Anycast", getIPInterfaceIPv6Address_Anycast, setIPInterfaceIPv6Address_Anycast, NULL, RPC_RW, eBoolean, 0, 0},
	{NULL}
};
/**@endparam Device.IP.Interface.{i}.IPv6Address.{i}.  */

/**@param Device.IP.Interface.{i}.IPv6Prefix.{i}. */
CPEGETFUNC(getIPInterfaceIPv6Prefix_Enable);
CPEGETFUNC(getIPInterfaceIPv6Prefix_Status);
CPEGETFUNC(getIPInterfaceIPv6Prefix_PrefixStatus);
CPEGETFUNC(getIPInterfaceIPv6Prefix_Prefix);
CPESETFUNC(setIPInterfaceIPv6Prefix_Prefix);
CPEGETFUNC(getIPInterfaceIPv6Prefix_Origin);
CPEGETFUNC(getIPInterfaceIPv6Prefix_OnLink);
CPESETFUNC(setIPInterfaceIPv6Prefix_OnLink);
CPEGETFUNC(getIPInterfaceIPv6Prefix_Autonomous);
CPESETFUNC(setIPInterfaceIPv6Prefix_Autonomous);
CPEGETFUNC(getIPInterfaceIPv6Prefix_PreferredLifetime);
CPESETFUNC(setIPInterfaceIPv6Prefix_PreferredLifetime);
CPEGETFUNC(getIPInterfaceIPv6Prefix_ValidLifetime);
CPESETFUNC(setIPInterfaceIPv6Prefix_ValidLifetime);
CWMPParam IPInterfaceIPv6Prefix_Params[]={
	{ "Enable", getIPInterfaceIPv6Prefix_Enable, NULL, NULL, RPC_R, eBoolean, 0, 0},
	{ "Status", getIPInterfaceIPv6Prefix_Status, NULL, NULL, RPC_R, eString, 0, 0},
	{ "PrefixStatus", getIPInterfaceIPv6Prefix_PrefixStatus, NULL, NULL, RPC_R, eString, 0, 0},
	{ "Alias", cwmpGetAliasParam, cwmpSetAliasParam, NULL, RPC_RW, eString, NOACTIVENOTIFY, 64},
	{ "Prefix", getIPInterfaceIPv6Prefix_Prefix, setIPInterfaceIPv6Prefix_Prefix, NULL, RPC_RW, eString, 0, 49},
	{ "Origin", getIPInterfaceIPv6Prefix_Origin, NULL, NULL, RPC_R, eString, 0, 0},
	{ "OnLink", getIPInterfaceIPv6Prefix_OnLink, setIPInterfaceIPv6Prefix_OnLink, NULL, RPC_RW, eBoolean, 0, 0},
	{ "Autonomous", getIPInterfaceIPv6Prefix_Autonomous, setIPInterfaceIPv6Prefix_Autonomous, NULL, RPC_RW, eBoolean, 0, 0},
	{ "PreferredLifetime", getIPInterfaceIPv6Prefix_PreferredLifetime, setIPInterfaceIPv6Prefix_PreferredLifetime, NULL, RPC_RW, eDateTime, 0, 0},
	{ "ValidLifetime", getIPInterfaceIPv6Prefix_ValidLifetime, setIPInterfaceIPv6Prefix_ValidLifetime, NULL, RPC_RW, eDateTime, 0, 0},
	{NULL}
};
/**@endparam Device.IP.Interface.{i}.IPv6Prefix.{i}.  */

/**@param Device.IP.Interface.{i}.Stats. */
CPEGETFUNC(getIPInterfaceStats_BytesSent);
CPEGETFUNC(getIPInterfaceStats_BytesReceived);
CPEGETFUNC(getIPInterfaceStats_PacketsSent);
CPEGETFUNC(getIPInterfaceStats_PacketsReceived);
CPEGETFUNC(getIPInterfaceStats_ErrorsSent);
CPEGETFUNC(getIPInterfaceStats_ErrorsReceived);
CPEGETFUNC(getIPInterfaceStats_UnicastPacketsSent);
CPEGETFUNC(getIPInterfaceStats_UnicastPacketsReceived);
CPEGETFUNC(getIPInterfaceStats_DiscardPacketsSent);
CPEGETFUNC(getIPInterfaceStats_DiscardPacketsReceived);
CPEGETFUNC(getIPInterfaceStats_MulticastPacketsSent);
CPEGETFUNC(getIPInterfaceStats_MulticastPacketsReceived);
CPEGETFUNC(getIPInterfaceStats_BroadcastPacketsSent);
CPEGETFUNC(getIPInterfaceStats_BroadcastPacketsReceived);
CPEGETFUNC(getIPInterfaceStats_UnknownProtoPacketsReceived);
CWMPParam IPInterfaceStats_Params[]={
	{ "BytesSent", getIPInterfaceStats_BytesSent, NULL, NULL, RPC_R, eUnsignedLong, NOACTIVENOTIFY, 0},
	{ "BytesReceived", getIPInterfaceStats_BytesReceived, NULL, NULL, RPC_R, eUnsignedLong, NOACTIVENOTIFY, 0},
	{ "PacketsSent", getIPInterfaceStats_PacketsSent, NULL, NULL, RPC_R, eUnsignedLong, NOACTIVENOTIFY, 0},
	{ "PacketsReceived", getIPInterfaceStats_PacketsReceived, NULL, NULL, RPC_R, eUnsignedLong, NOACTIVENOTIFY, 0},
	{ "ErrorsSent", getIPInterfaceStats_ErrorsSent, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "ErrorsReceived", getIPInterfaceStats_ErrorsReceived, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "UnicastPacketsSent", getIPInterfaceStats_UnicastPacketsSent, NULL, NULL, RPC_R, eUnsignedLong, NOACTIVENOTIFY, 0},
	{ "UnicastPacketsReceived", getIPInterfaceStats_UnicastPacketsReceived, NULL, NULL, RPC_R, eUnsignedLong, NOACTIVENOTIFY, 0},
	{ "DiscardPacketsSent", getIPInterfaceStats_DiscardPacketsSent, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "DiscardPacketsReceived", getIPInterfaceStats_DiscardPacketsReceived, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "MulticastPacketsSent", getIPInterfaceStats_MulticastPacketsSent, NULL, NULL, RPC_R, eUnsignedLong, NOACTIVENOTIFY, 0},
	{ "MulticastPacketsReceived", getIPInterfaceStats_MulticastPacketsReceived, NULL, NULL, RPC_R, eUnsignedLong, NOACTIVENOTIFY, 0},
	{ "BroadcastPacketsSent", getIPInterfaceStats_BroadcastPacketsSent, NULL, NULL, RPC_R, eUnsignedLong, 0, 0},
	{ "BroadcastPacketsReceived", getIPInterfaceStats_BroadcastPacketsReceived, NULL, NULL, RPC_R, eUnsignedLong, 0, 0},
	{ "UnknownProtoPacketsReceived", getIPInterfaceStats_UnknownProtoPacketsReceived, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{NULL}
};
/**@endparam Device.IP.Interface.{i}.Stats.  */

/**@param Device.IP.Interface.{i}. */
CPEGETFUNC(getIPInterface_Enable);
CPESETFUNC(setIPInterface_Enable);
CPEGETFUNC(getIPInterface_IPv4Enable);
CPEGETFUNC(getIPInterface_IPv6Enable);
CPEGETFUNC(getIPInterface_ULAEnable);
CPESETFUNC(setIPInterface_ULAEnable);
CPEGETFUNC(getIPInterface_Status);
CPEGETFUNC(getIPInterface_Name);
CPEGETFUNC(getIPInterface_LastChange);
CPEGETFUNC(getIPInterface_LowerLayers);
CPESETFUNC(setIPInterface_LowerLayers);
CPEGETFUNC(getIPInterface_Reset);
CPESETFUNC(setIPInterface_Reset);
CPEGETFUNC(getIPInterface_Type);
CPEGETFUNC(getIPInterface_IPv4AddressNumberOfEntries);
CPEGETFUNC(getIPInterface_IPv6AddressNumberOfEntries);
CPEGETFUNC(getIPInterface_IPv6PrefixNumberOfEntries);
//CPEGETFUNC(getIPInterface_AutoIPEnable);
//CPESETFUNC(setIPInterface_AutoIPEnable);
CWMPParam IPInterface_Params[]={
	{ "Enable", getIPInterface_Enable, setIPInterface_Enable, NULL, RPC_R, eBoolean, 0, 0},
	{ "IPv4Enable", getIPInterface_IPv4Enable, NULL, NULL, RPC_R, eBoolean, 0, 0},
	{ "IPv6Enable", getIPInterface_IPv6Enable, NULL, NULL, RPC_R, eBoolean, 0, 0},
	{ "ULAEnable", getIPInterface_ULAEnable, setIPInterface_ULAEnable, NULL, RPC_RW, eBoolean, 0, 0},
	{ "Status", getIPInterface_Status, NULL, NULL, RPC_R, eString, 0, 0},
	{ "Alias", cwmpGetAliasParam, cwmpSetAliasParam, NULL, RPC_RW, eString, NOACTIVENOTIFY, 64},
	{ "Name", getIPInterface_Name, NULL, NULL, RPC_R, eString, 0, 64},
	{ "LastChange", getIPInterface_LastChange, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "LowerLayers", getIPInterface_LowerLayers, setIPInterface_LowerLayers, NULL, RPC_RW,  eString, 0, 0},
	{ "Reset", getIPInterface_Reset, setIPInterface_Reset, NULL, RPC_RW, eBoolean, 0, 0},
	{ "Type", getIPInterface_Type, NULL, NULL, RPC_R, eString, 0, 0},
	{ "IPv4AddressNumberOfEntries", getIPInterface_IPv4AddressNumberOfEntries, NULL, NULL, RPC_R, eUnsignedInt, 0, 0},
	{ "IPv6AddressNumberOfEntries", getIPInterface_IPv6AddressNumberOfEntries, NULL, NULL, RPC_R, eUnsignedInt, 0, 0},
	{ "IPv6PrefixNumberOfEntries", getIPInterface_IPv6PrefixNumberOfEntries, NULL, NULL, RPC_R, eUnsignedInt, 0, 0},
//	{ "AutoIPEnable", getIPInterface_AutoIPEnable, setIPInterface_AutoIPEnable, NULL, RPC_RW, eBoolean, 0, 0},
	{NULL}
};
/**@endparam Device.IP.Interface.{i}.  */

/**@obj Device.IP.Interface.{i}.  */
CPEADDOBJ(addIPInterfaceIPv4Address);
CPEDELOBJ(delIPInterfaceIPv4Address);
CPECOMMIT(commitIPInterfaceIPv4Address);
CPEADDOBJ(addIPInterfaceIPv6Address);
CPEDELOBJ(delIPInterfaceIPv6Address);
CPECOMMIT(commitIPInterfaceIPv6Address);
//CPEADDOBJ(addIPInterfaceIPv6Prefix);
//CPEDELOBJ(delIPInterfaceIPv6Prefix);
CWMPObject IPInterface_Objs[]={
	{"IPv4Address", delIPInterfaceIPv4Address, addIPInterfaceIPv4Address, commitIPInterfaceIPv4Address, NULL, NULL, IPInterfaceIPv4Address_Params, NULL, eInstance,  0},
	{"IPv6Address", delIPInterfaceIPv6Address, addIPInterfaceIPv6Address, commitIPInterfaceIPv6Address, NULL, NULL, IPInterfaceIPv6Address_Params, NULL, eInstance,  0},
//	{"IPv6Prefix", delIPInterfaceIPv6Prefix, addIPInterfaceIPv6Prefix, NULL, NULL, NULL, IPInterfaceIPv6Prefix_Params, NULL, eInstance,  0},
//	{"Stats", NULL, NULL, NULL, NULL, NULL, IPInterfaceStats_Params, NULL, eObject,  0},
	{NULL}
};

/**@endobj Device.IP.Interface.{i}.  */
/**@param Device.IP. */
CPEGETFUNC(getIP_IPv4Capable);
CPEGETFUNC(getIP_IPv4Enable);
CPEGETFUNC(getIP_IPv4Status);
CPEGETFUNC(getIP_IPv6Capable);
CPEGETFUNC(getIP_IPv6Enable);
CPEGETFUNC(getIP_IPv6Status);
CPEGETFUNC(getIP_ULAPrefix);
CPESETFUNC(setIP_ULAPrefix);
CPEGETFUNC(getIP_InterfaceNumberOfEntries);
//CPEGETFUNC(getIP_ActivePortNumberOfEntries);
CWMPParam IP_Params[]={
	{ "IPv4Capable", getIP_IPv4Capable, NULL, NULL, RPC_R, eBoolean, 0, 0},
	{ "IPv4Enable", getIP_IPv4Enable, NULL, NULL, RPC_R, eBoolean, 0, 0},
	{ "IPv4Status", getIP_IPv4Status, NULL, NULL, RPC_R, eString, 0, 0},
	{ "IPv6Capable", getIP_IPv6Capable, NULL, NULL, RPC_R, eBoolean, 0, 0},
	{ "IPv6Enable", getIP_IPv6Enable, NULL, NULL, RPC_R, eBoolean, 0, 0},
	{ "IPv6Status", getIP_IPv6Status, NULL, NULL, RPC_R, eString, 0, 0},
	{ "ULAPrefix", getIP_ULAPrefix, setIP_ULAPrefix, NULL, RPC_RW, eString, 0, 49},
	{ "InterfaceNumberOfEntries", getIP_InterfaceNumberOfEntries, NULL, NULL, RPC_R, eUnsignedInt, 0, 0},
//	{ "ActivePortNumberOfEntries", getIP_ActivePortNumberOfEntries, NULL, NULL, RPC_R, eUnsignedInt, 0, 0},
	{NULL}
};
/**@endparam Device.IP.  */

/**@obj Device.IP.  */
CPEADDOBJ(addIPInterface);
CPEDELOBJ(delIPInterface);
CPECOMMIT(commitIPInterface);
CWMPObject IP_Objs[]={
	{"Diagnostics", NULL, NULL, NULL, NULL, IPDiagnostics_Objs, NULL, NULL, eObject,  0},
	{"Interface", delIPInterface, addIPInterface, commitIPInterface, NULL, IPInterface_Objs, IPInterface_Params, NULL, eInstance,  0},
	{NULL}
};

/**@endobj Device.IP.  */
#if 0
/**@param Device.InterfaceStack.{i}. */
CPEGETFUNC(getInterfaceStack_HigherLayer);
CPESETFUNC(setInterfaceStack_HigherLayer);
CPEGETFUNC(getInterfaceStack_LowerLayer);
CPESETFUNC(setInterfaceStack_LowerLayer);
CWMPParam InterfaceStack_Params[]={
	{ "Alias", cwmpGetAliasParam, cwmpSetAliasParam, NULL, RPC_RW, eString, NOACTIVENOTIFY, 64},
	{ "HigherLayer", getInterfaceStack_HigherLayer, setInterfaceStack_HigherLayer, NULL, RPC_R,  eString, 0, 256},
	{ "LowerLayer", getInterfaceStack_LowerLayer, setInterfaceStack_LowerLayer, NULL, RPC_R,  eString, 0, 256},
	{NULL}
};
/**@endparam Device.InterfaceStack.{i}.  */
#endif
/**@param Device.LANConfigSecurity. */
CPEGETFUNC(getLANConfigSecurity_ConfigPassword);
CPESETFUNC(setLANConfigSecurity_ConfigPassword);
CWMPParam LANConfigSecurity_Params[]={
	{ "ConfigPassword", getLANConfigSecurity_ConfigPassword, setLANConfigSecurity_ConfigPassword, NULL, RPC_RW, eStringSetOnly, 0, 64},
	{NULL}
};
/**@endparam Device.LANConfigSecurity.  */

/**@param Device.ManagementServer. */
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
CPEGETFUNC(getManagementServer_AliasBasedAddressing);
CPEGETFUNC(getManagementServer_InstanceMode);
CPESETFUNC(setManagementServer_InstanceMode);
CPEGETFUNC(getManagementServer_AutoCreateInstances);
CPESETFUNC(setManagementServer_AutoCreateInstances);
CPEGETFUNC(getManagementServer_CWMPRetryMinimumWaitInterval);
CPESETFUNC(setManagementServer_CWMPRetryMinimumWaitInterval);
CPEGETFUNC(getManagementServer_CWMPRetryIntervalMultiplier);
CPESETFUNC(setManagementServer_CWMPRetryIntervalMultiplier);
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
	{ "UDPConnectionRequestAddress", getManagementServer_UDPConnectionRequestAddress, NULL, NULL, RPC_R, eString, FORCED_INFORM|DEFAULT_ACTIVE, 256},
	{ "STUNEnable", getManagementServer_STUNEnable, setManagementServer_STUNEnable, NULL, RPC_RW, eBoolean, 0, 0},
	{ "STUNServerAddress", getManagementServer_STUNServerAddress, setManagementServer_STUNServerAddress, NULL, RPC_RW, eString, 0, 256},
	{ "STUNServerPort", getManagementServer_STUNServerPort, setManagementServer_STUNServerPort, NULL, RPC_RW, eUnsignedInt, 0, 0},
	{ "STUNUsername", getManagementServer_STUNUsername, setManagementServer_STUNUsername, NULL, RPC_RW, eString, 0, 256},
	{ "STUNPassword", getManagementServer_STUNPassword, setManagementServer_STUNPassword, NULL, RPC_RW, eStringSetOnly, 0, 256},
	{ "STUNMaximumKeepAlivePeriod", getManagementServer_STUNMaximumKeepAlivePeriod, setManagementServer_STUNMaximumKeepAlivePeriod, NULL, RPC_RW, eInt, 0, 0},
	{ "STUNMinimumKeepAlivePeriod", getManagementServer_STUNMinimumKeepAlivePeriod, setManagementServer_STUNMinimumKeepAlivePeriod, NULL, RPC_RW, eUnsignedInt, 0, 0},
	{ "NATDetected", getManagementServer_NATDetected, NULL, NULL, RPC_R, eBoolean, FORCED_INFORM|DEFAULT_ACTIVE, 0},
	{ "AliasBasedAddressing", getManagementServer_AliasBasedAddressing, NULL, NULL, RPC_R, eBoolean, FORCED_INFORM, 0},
	{ "InstanceMode", getManagementServer_InstanceMode, setManagementServer_InstanceMode, NULL, RPC_RW, eString, 0, 0},
	{ "AutoCreateInstances", getManagementServer_AutoCreateInstances, setManagementServer_AutoCreateInstances, NULL, RPC_RW, eBoolean, 0, 0},
	{ "CWMPRetryMinimumWaitInterval", getManagementServer_CWMPRetryMinimumWaitInterval, setManagementServer_CWMPRetryMinimumWaitInterval, NULL, RPC_RW, eUnsignedInt, 0, 0},
	{ "CWMPRetryIntervalMultiplier", getManagementServer_CWMPRetryIntervalMultiplier, setManagementServer_CWMPRetryIntervalMultiplier, NULL, RPC_RW, eUnsignedInt, 0, 0},
	{NULL}
};
/**@endparam Device.ManagementServer.  */
#if 0
/**@param Device.SoftwareModules.DeploymentUnit.{i}. */
CPEGETFUNC(getSoftwareModulesDeploymentUnit_UUID);
CPEGETFUNC(getSoftwareModulesDeploymentUnit_DUID);
CPEGETFUNC(getSoftwareModulesDeploymentUnit_Name);
CPEGETFUNC(getSoftwareModulesDeploymentUnit_Status);
CPEGETFUNC(getSoftwareModulesDeploymentUnit_Resolved);
CPEGETFUNC(getSoftwareModulesDeploymentUnit_URL);
CPEGETFUNC(getSoftwareModulesDeploymentUnit_Vendor);
CPEGETFUNC(getSoftwareModulesDeploymentUnit_Version);
CPEGETFUNC(getSoftwareModulesDeploymentUnit_ExecutionUnitList);
CPEGETFUNC(getSoftwareModulesDeploymentUnit_ExecutionEnvRef);
CWMPParam SoftwareModulesDeploymentUnit_Params[]={
	{ "UUID", getSoftwareModulesDeploymentUnit_UUID, NULL, NULL, RPC_R, eString, 0, 36},
	{ "DUID", getSoftwareModulesDeploymentUnit_DUID, NULL, NULL, RPC_R, eString, 0, 64},
	{ "Alias", cwmpGetAliasParam, cwmpSetAliasParam, NULL, RPC_RW, eString, NOACTIVENOTIFY, 64},
	{ "Name", getSoftwareModulesDeploymentUnit_Name, NULL, NULL, RPC_R, eString, 0, 64},
	{ "Status", getSoftwareModulesDeploymentUnit_Status, NULL, NULL, RPC_R, eString, NOACTIVENOTIFY, 0},
	{ "Resolved", getSoftwareModulesDeploymentUnit_Resolved, NULL, NULL, RPC_R, eBoolean, 0, 0},
	{ "URL", getSoftwareModulesDeploymentUnit_URL, NULL, NULL, RPC_R, eString, 0, 1024},
	{ "Vendor", getSoftwareModulesDeploymentUnit_Vendor, NULL, NULL, RPC_R, eString, 0, 128},
	{ "Version", getSoftwareModulesDeploymentUnit_Version, NULL, NULL, RPC_R, eString, 0, 32},
	{ "ExecutionUnitList", getSoftwareModulesDeploymentUnit_ExecutionUnitList, NULL, NULL, RPC_R,  eString, 0, 0},
	{ "ExecutionEnvRef", getSoftwareModulesDeploymentUnit_ExecutionEnvRef, NULL, NULL, RPC_R,  eString, 0, 0},
	{NULL}
};
/**@endparam Device.SoftwareModules.DeploymentUnit.{i}.  */

/**@param Device.SoftwareModules.ExecEnv.{i}. */
CPEGETFUNC(getSoftwareModulesExecEnv_Enable);
CPESETFUNC(setSoftwareModulesExecEnv_Enable);
CPEGETFUNC(getSoftwareModulesExecEnv_Status);
CPEGETFUNC(getSoftwareModulesExecEnv_Name);
CPEGETFUNC(getSoftwareModulesExecEnv_Type);
CPEGETFUNC(getSoftwareModulesExecEnv_Vendor);
CPEGETFUNC(getSoftwareModulesExecEnv_Version);
CPEGETFUNC(getSoftwareModulesExecEnv_ActiveExecutionUnits);
CWMPParam SoftwareModulesExecEnv_Params[]={
	{ "Enable", getSoftwareModulesExecEnv_Enable, setSoftwareModulesExecEnv_Enable, NULL, RPC_RW, eBoolean, 0, 0},
	{ "Status", getSoftwareModulesExecEnv_Status, NULL, NULL, RPC_R, eString, 0, 0},
	{ "Alias", cwmpGetAliasParam, cwmpSetAliasParam, NULL, RPC_RW, eString, NOACTIVENOTIFY, 64},
	{ "Name", getSoftwareModulesExecEnv_Name, NULL, NULL, RPC_R, eString, 0, 32},
	{ "Type", getSoftwareModulesExecEnv_Type, NULL, NULL, RPC_R, eString, 0, 64},
	{ "Vendor", getSoftwareModulesExecEnv_Vendor, NULL, NULL, RPC_R, eString, 0, 128},
	{ "Version", getSoftwareModulesExecEnv_Version, NULL, NULL, RPC_R, eString, 0, 32},
	{ "ActiveExecutionUnits", getSoftwareModulesExecEnv_ActiveExecutionUnits, NULL, NULL, RPC_R,  eString, NOACTIVENOTIFY, 0},
	{NULL}
};
/**@endparam Device.SoftwareModules.ExecEnv.{i}.  */

/**@param Device.SoftwareModules.ExecutionUnit.{i}. */
CPEGETFUNC(getSoftwareModulesExecutionUnit_EUID);
CPEGETFUNC(getSoftwareModulesExecutionUnit_Name);
CPEGETFUNC(getSoftwareModulesExecutionUnit_ExecEnvLabel);
CPEGETFUNC(getSoftwareModulesExecutionUnit_Status);
CPEGETFUNC(getSoftwareModulesExecutionUnit_RequestedState);
CPESETFUNC(setSoftwareModulesExecutionUnit_RequestedState);
CPEGETFUNC(getSoftwareModulesExecutionUnit_ExecutionFaultCode);
CPEGETFUNC(getSoftwareModulesExecutionUnit_ExecutionFaultMessage);
CPEGETFUNC(getSoftwareModulesExecutionUnit_Vendor);
CPEGETFUNC(getSoftwareModulesExecutionUnit_Version);
CPEGETFUNC(getSoftwareModulesExecutionUnit_References);
CPEGETFUNC(getSoftwareModulesExecutionUnit_SupportedDataModelList);
CWMPParam SoftwareModulesExecutionUnit_Params[]={
	{ "EUID", getSoftwareModulesExecutionUnit_EUID, NULL, NULL, RPC_R, eString, 0, 64},
	{ "Alias", cwmpGetAliasParam, cwmpSetAliasParam, NULL, RPC_RW, eString, NOACTIVENOTIFY, 64},
	{ "Name", getSoftwareModulesExecutionUnit_Name, NULL, NULL, RPC_R, eString, 0, 32},
	{ "ExecEnvLabel", getSoftwareModulesExecutionUnit_ExecEnvLabel, NULL, NULL, RPC_R, eString, 0, 64},
	{ "Status", getSoftwareModulesExecutionUnit_Status, NULL, NULL, RPC_R, eString, DEFAULT_ACTIVE, 0},
	{ "RequestedState", getSoftwareModulesExecutionUnit_RequestedState, setSoftwareModulesExecutionUnit_RequestedState, NULL, RPC_RW, eString, 0, 0},
	{ "ExecutionFaultCode", getSoftwareModulesExecutionUnit_ExecutionFaultCode, NULL, NULL, RPC_R, eString, 0, 0},
	{ "ExecutionFaultMessage", getSoftwareModulesExecutionUnit_ExecutionFaultMessage, NULL, NULL, RPC_R, eString, 0, 256},
	{ "Vendor", getSoftwareModulesExecutionUnit_Vendor, NULL, NULL, RPC_R, eString, 0, 128},
	{ "Version", getSoftwareModulesExecutionUnit_Version, NULL, NULL, RPC_R, eString, 0, 32},
	{ "References", getSoftwareModulesExecutionUnit_References, NULL, NULL, RPC_R,  eString, 0, 0},
	{ "SupportedDataModelList", getSoftwareModulesExecutionUnit_SupportedDataModelList, NULL, NULL, RPC_R,  eString, 0, 0},
	{NULL}
};
/**@endparam Device.SoftwareModules.ExecutionUnit.{i}.  */

/**@obj Device.SoftwareModules.ExecutionUnit.{i}.  */
CWMPObject SoftwareModulesExecutionUnit_Objs[]={
	{"Extensions", NULL, NULL, NULL, NULL, NULL, NULL, NULL, eObject,  0},
	{NULL}
};

/**@endobj Device.SoftwareModules.ExecutionUnit.{i}.  */
/**@param Device.SoftwareModules. */
CPEGETFUNC(getSoftwareModules_ExecEnvNumberOfEntries);
CPEGETFUNC(getSoftwareModules_DeploymentUnitNumberOfEntries);
CPEGETFUNC(getSoftwareModules_ExecutionUnitNumberOfEntries);
CWMPParam SoftwareModules_Params[]={
	{ "ExecEnvNumberOfEntries", getSoftwareModules_ExecEnvNumberOfEntries, NULL, NULL, RPC_R, eUnsignedInt, 0, 0},
	{ "DeploymentUnitNumberOfEntries", getSoftwareModules_DeploymentUnitNumberOfEntries, NULL, NULL, RPC_R, eUnsignedInt, 0, 0},
	{ "ExecutionUnitNumberOfEntries", getSoftwareModules_ExecutionUnitNumberOfEntries, NULL, NULL, RPC_R, eUnsignedInt, 0, 0},
	{NULL}
};
/**@endparam Device.SoftwareModules.  */

/**@obj Device.SoftwareModules.  */
CPEADDOBJ(addSoftwareModulesDeploymentUnit);
CPEADDOBJ(addSoftwareModulesExecEnv);
CPEADDOBJ(addSoftwareModulesExecutionUnit);
CWMPObject SoftwareModules_Objs[]={
	{"DeploymentUnit", NULL, addSoftwareModulesDeploymentUnit, NULL, NULL, NULL, SoftwareModulesDeploymentUnit_Params, NULL, eStaticInstance,  0},
	{"ExecEnv", NULL, addSoftwareModulesExecEnv, NULL, NULL, NULL, SoftwareModulesExecEnv_Params, NULL, eStaticInstance,  0},
	{"ExecutionUnit", NULL, addSoftwareModulesExecutionUnit, NULL, NULL, SoftwareModulesExecutionUnit_Objs, SoftwareModulesExecutionUnit_Params, NULL, eStaticInstance,  0},
	{NULL}
};

/**@endobj Device.SoftwareModules.  */
#endif
/**  WiFiAccessPoint:1, WiFiSSID:1, WiFiRadio:1, 
**/
/**@param Device.WiFi.AccessPoint.{i}.AssociatedDevice.{i}. */
CPEGETFUNC(getWiFiAccessPointAssociatedDevice_MACAddress);
CPEGETFUNC(getWiFiAccessPointAssociatedDevice_AuthenticationState);
CPEGETFUNC(getWiFiAccessPointAssociatedDevice_LastDataDownlinkRate);
CPEGETFUNC(getWiFiAccessPointAssociatedDevice_LastDataUplinkRate);
CPEGETFUNC(getWiFiAccessPointAssociatedDevice_SignalStrength);
//CPEGETFUNC(getWiFiAccessPointAssociatedDevice_Retransmissions);
CPEGETFUNC(getWiFiAccessPointAssociatedDevice_Active);
CWMPParam WiFiAccessPointAssociatedDevice_Params[]={
	{ "MACAddress", getWiFiAccessPointAssociatedDevice_MACAddress, NULL, NULL, RPC_R, eString, 0, 0},
	{ "AuthenticationState", getWiFiAccessPointAssociatedDevice_AuthenticationState, NULL, NULL, RPC_R, eBoolean, 0, 0},
	{ "LastDataDownlinkRate", getWiFiAccessPointAssociatedDevice_LastDataDownlinkRate, NULL, NULL, RPC_R, eUnsignedInt, 0, 0},
	{ "LastDataUplinkRate", getWiFiAccessPointAssociatedDevice_LastDataUplinkRate, NULL, NULL, RPC_R, eUnsignedInt, 0, 0},
	{ "SignalStrength", getWiFiAccessPointAssociatedDevice_SignalStrength, NULL, NULL, RPC_R, eString, 0, 0},
//	{ "Retransmissions", getWiFiAccessPointAssociatedDevice_Retransmissions, NULL, NULL, RPC_R, eUnsignedInt, 0, 0},
	{ "Active", getWiFiAccessPointAssociatedDevice_Active, NULL, NULL, RPC_R, eBoolean, 0, 0},
	{NULL}
};
/**@endparam Device.WiFi.AccessPoint.{i}.AssociatedDevice.{i}.  */

/**@param Device.WiFi.AccessPoint.{i}.Security. */
CPEGETFUNC(getWiFiAccessPointSecurity_Reset);
CPEGETFUNC(getWiFiAccessPointSecurity_ModesSupported);
CPEGETFUNC(getWiFiAccessPointSecurity_ModeEnabled);
CPEGETFUNC(getWiFiAccessPointSecurity_PreSharedKey);
CPEGETFUNC(getWiFiAccessPointSecurity_KeyPassphrase);

#if 0
CPESETFUNC(setWiFiAccessPointSecurity_Reset);
CPESETFUNC(setWiFiAccessPointSecurity_ModeEnabled);
CPESETFUNC(setWiFiAccessPointSecurity_PreSharedKey);
CPESETFUNC(setWiFiAccessPointSecurity_KeyPassphrase);
#endif

CWMPParam WiFiAccessPointSecurity_Params[]={
	{ "Reset", getWiFiAccessPointSecurity_Reset, NULL, NULL, RPC_R, eBoolean, 0, 0},
	{ "ModesSupported", getWiFiAccessPointSecurity_ModesSupported, NULL, NULL, RPC_R, eString, 0, 0},
	{ "ModeEnabled", getWiFiAccessPointSecurity_ModeEnabled, NULL, NULL, RPC_R, eString, 0, 0},
	{ "PreSharedKey", getWiFiAccessPointSecurity_PreSharedKey, NULL, NULL, RPC_R, eHexBinary, 0, 32},
	{ "KeyPassphrase", getWiFiAccessPointSecurity_KeyPassphrase, NULL, NULL, RPC_R, eStringSetOnly, 0, 63},
	{NULL}
};
/**@endparam Device.WiFi.AccessPoint.{i}.Security.  */

/**@param Device.WiFi.AccessPoint.{i}.WPS. */
CPEGETFUNC(getWiFiAccessPointWPS_Enable);
CPEGETFUNC(getWiFiAccessPointWPS_ConfigMethodsSupported);
CPEGETFUNC(getWiFiAccessPointWPS_ConfigMethodsEnabled);

#if 0
CPESETFUNC(setWiFiAccessPointWPS_Enable);
CPESETFUNC(setWiFiAccessPointWPS_ConfigMethodsEnabled);
#endif

CWMPParam WiFiAccessPointWPS_Params[]={
	{ "Enable", getWiFiAccessPointWPS_Enable, NULL, NULL, RPC_R, eBoolean, 0, 0},
	{ "ConfigMethodsSupported", getWiFiAccessPointWPS_ConfigMethodsSupported, NULL, NULL, RPC_R, eString, 0, 0},
	{ "ConfigMethodsEnabled", getWiFiAccessPointWPS_ConfigMethodsEnabled, NULL, NULL, RPC_R, eString, 0, 0},
	{NULL}
};
/**@endparam Device.WiFi.AccessPoint.{i}.WPS.  */

/**@param Device.WiFi.AccessPoint.{i}. */
CPEGETFUNC(getWiFiAccessPoint_Enable);
CPEGETFUNC(getWiFiAccessPoint_Status);
CPEGETFUNC(getWiFiAccessPoint_SSIDReference);
CPEGETFUNC(getWiFiAccessPoint_SSIDAdvertisementEnabled);
CPEGETFUNC(getWiFiAccessPoint_AssociatedDeviceNumberOfEntries);
CPEGETFUNC(getWiFiAccessPoint_ParentAPMAC);
CPEGETFUNC(getWiFiAccessPoint_CAPMODE);

#if 0
CPESETFUNC(setWiFiAccessPoint_Enable);
CPESETFUNC(setWiFiAccessPoint_SSIDAdvertisementEnabled);
#endif

CWMPParam WiFiAccessPoint_Params[]={
	{ "Enable", getWiFiAccessPoint_Enable, NULL, NULL, RPC_R, eBoolean, 0, 0},
	{ "Status", getWiFiAccessPoint_Status, NULL, NULL, RPC_R, eString, 0, 0},
	{ "SSIDReference", getWiFiAccessPoint_SSIDReference, NULL, NULL, RPC_R,  eString, 0, 256},
	{ "SSIDAdvertisementEnabled", getWiFiAccessPoint_SSIDAdvertisementEnabled, NULL, NULL, RPC_R, eBoolean, 0, 0},
	{ "AssociatedDeviceNumberOfEntries", getWiFiAccessPoint_AssociatedDeviceNumberOfEntries, NULL, NULL, RPC_R, eUnsignedInt, 0, 0},
	{NULL}
};
/**@endparam Device.WiFi.AccessPoint.{i}.  */

/**@obj Device.WiFi.AccessPoint.{i}.  */
CPEADDOBJ(addWiFiAccessPointAssociatedDevice);
CPEDELOBJ(delWiFiAccessPointAssociatedDevice);
CPEADDOBJ(initWiFiAccessPointSecurity);
CPEADDOBJ(initWiFiAccessPointWPS);
CWMPObject WiFiAccessPoint_Objs[]={
	{"AssociatedDevice", delWiFiAccessPointAssociatedDevice, addWiFiAccessPointAssociatedDevice, NULL, NULL, NULL, WiFiAccessPointAssociatedDevice_Params, NULL, eStaticInstance,  0},
	{"Security", NULL, initWiFiAccessPointSecurity, NULL, NULL, NULL, WiFiAccessPointSecurity_Params, NULL, eStaticInstance,  0},
	{"WPS", NULL, initWiFiAccessPointWPS, NULL, NULL, NULL, WiFiAccessPointWPS_Params, NULL, eStaticInstance,  0},
	{NULL}
};

/**@endobj Device.WiFi.AccessPoint.{i}.  */
/**@param Device.WiFi.Radio.{i}.Stats. */
CPEGETFUNC(getWiFiRadioStats_BytesSent);
CPEGETFUNC(getWiFiRadioStats_BytesReceived);
CPEGETFUNC(getWiFiRadioStats_PacketsSent);
CPEGETFUNC(getWiFiRadioStats_PacketsReceived);
CPEGETFUNC(getWiFiRadioStats_ErrorsSent);
CPEGETFUNC(getWiFiRadioStats_ErrorsReceived);
CPEGETFUNC(getWiFiRadioStats_DiscardPacketsSent);
CPEGETFUNC(getWiFiRadioStats_DiscardPacketsReceived);
CWMPParam WiFiRadioStats_Params[]={
	{ "BytesSent", getWiFiRadioStats_BytesSent, NULL, NULL, RPC_R, eUnsignedLong, NOACTIVENOTIFY, 0},
	{ "BytesReceived", getWiFiRadioStats_BytesReceived, NULL, NULL, RPC_R, eUnsignedLong, NOACTIVENOTIFY, 0},
	{ "PacketsSent", getWiFiRadioStats_PacketsSent, NULL, NULL, RPC_R, eUnsignedLong, NOACTIVENOTIFY, 0},
	{ "PacketsReceived", getWiFiRadioStats_PacketsReceived, NULL, NULL, RPC_R, eUnsignedLong, NOACTIVENOTIFY, 0},
	{ "ErrorsSent", getWiFiRadioStats_ErrorsSent, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "ErrorsReceived", getWiFiRadioStats_ErrorsReceived, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "DiscardPacketsSent", getWiFiRadioStats_DiscardPacketsSent, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "DiscardPacketsReceived", getWiFiRadioStats_DiscardPacketsReceived, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{NULL}
};
/**@endparam Device.WiFi.Radio.{i}.Stats.  */

/**@param Device.WiFi.Radio.{i}. */
CPEGETFUNC(getWiFiRadio_Enable);
CPEGETFUNC(getWiFiRadio_Status);
CPEGETFUNC(getWiFiRadio_Name);
CPEGETFUNC(getWiFiRadio_MaxBitRate);
CPEGETFUNC(getWiFiRadio_SupportedFrequencyBands);
CPEGETFUNC(getWiFiRadio_OperatingFrequencyBand);
CPEGETFUNC(getWiFiRadio_SupportedStandards);
CPEGETFUNC(getWiFiRadio_OperatingStandards);
CPEGETFUNC(getWiFiRadio_PossibleChannels);
CPEGETFUNC(getWiFiRadio_ChannelsInUse);
CPEGETFUNC(getWiFiRadio_Channel);
CPEGETFUNC(getWiFiRadio_AutoChannelSupported);
CPEGETFUNC(getWiFiRadio_AutoChannelEnable);
//CPEGETFUNC(getWiFiRadio_AutoChannelRefreshPeriod);
CPEGETFUNC(getWiFiRadio_OperatingChannelBandwidth);
//CPEGETFUNC(getWiFiRadio_ExtensionChannel);
CPEGETFUNC(getWiFiRadio_GuardInterval);
CPEGETFUNC(getWiFiRadio_MCS);
CPEGETFUNC(getWiFiRadio_TransmitPowerSupported);
CPEGETFUNC(getWiFiRadio_TransmitPower);
CPEGETFUNC(getWiFiRadio_IEEE80211hSupported);
CPEGETFUNC(getWiFiRadio_IEEE80211hEnabled);
CPEGETFUNC(getWiFiRadio_RegulatoryDomain);

#if 0
CPESETFUNC(setWiFiRadio_Enable);
CPESETFUNC(setWiFiRadio_RegulatoryDomain);
CPESETFUNC(setWiFiRadio_AutoChannelEnable);
CPESETFUNC(setWiFiRadio_TransmitPower);
CPESETFUNC(setWiFiRadio_GuardInterval);
CPESETFUNC(setWiFiRadio_IEEE80211hEnabled);
CPESETFUNC(setWiFiRadio_MCS);
CPESETFUNC(setWiFiRadio_ExtensionChannel);
CPESETFUNC(setWiFiRadio_AutoChannelRefreshPeriod);
CPESETFUNC(setWiFiRadio_OperatingFrequencyBand);
CPESETFUNC(setWiFiRadio_OperatingChannelBandwidth);
CPESETFUNC(setWiFiRadio_Channel);
CPESETFUNC(setWiFiRadio_OperatingStandards);
#endif

CWMPParam WiFiRadio_Params[]={
	{ "Enable", getWiFiRadio_Enable, NULL, NULL, RPC_R, eBoolean, 0, 0},
	{ "Status", getWiFiRadio_Status, NULL, NULL, RPC_R, eString, 0, 0},
	{ "Name", getWiFiRadio_Name, NULL, NULL, RPC_R, eString, 0, 64},
	{ "MaxBitRate", getWiFiRadio_MaxBitRate, NULL, NULL, RPC_R, eUnsignedInt, 0, 0},
	{ "SupportedFrequencyBands", getWiFiRadio_SupportedFrequencyBands, NULL, NULL, RPC_R, eString, 0, 0},
	{ "OperatingFrequencyBand", getWiFiRadio_OperatingFrequencyBand, NULL, NULL, RPC_R, eString, 0, 0},
	{ "SupportedStandards", getWiFiRadio_SupportedStandards, NULL, NULL, RPC_R, eString, 0, 0},
	{ "OperatingStandards", getWiFiRadio_OperatingStandards, NULL, NULL, RPC_R, eString, 0, 0},
	{ "PossibleChannels", getWiFiRadio_PossibleChannels, NULL, NULL, RPC_R, eString, 0, 0},
	{ "ChannelsInUse", getWiFiRadio_ChannelsInUse, NULL, NULL, RPC_R, eString, NOACTIVENOTIFY, 0},
	{ "Channel", getWiFiRadio_Channel, NULL, NULL, RPC_RW, eUnsignedInt, 0, 0},
	{ "AutoChannelSupported", getWiFiRadio_AutoChannelSupported, NULL, NULL, RPC_R, eBoolean, 0, 0},
	{ "AutoChannelEnable", getWiFiRadio_AutoChannelEnable, NULL, NULL, RPC_R, eBoolean, 0, 0},
//	{ "AutoChannelRefreshPeriod", getWiFiRadio_AutoChannelRefreshPeriod, setWiFiRadio_AutoChannelRefreshPeriod, NULL, RPC_RW, eString, 0, 0},
	{ "OperatingChannelBandwidth", getWiFiRadio_OperatingChannelBandwidth, NULL, NULL, RPC_RW, eString, 0, 0},
//	{ "ExtensionChannel", getWiFiRadio_ExtensionChannel, setWiFiRadio_ExtensionChannel, NULL, RPC_RW, eString, 0, 0},
	{ "GuardInterval", getWiFiRadio_GuardInterval, NULL, NULL, RPC_R, eString, 0, 0},
	{ "MCS", getWiFiRadio_MCS, NULL, NULL, RPC_R, eString, 0, 0},
	{ "TransmitPowerSupported", getWiFiRadio_TransmitPowerSupported, NULL, NULL, RPC_R, eInt, 0, 0},
	{ "TransmitPower", getWiFiRadio_TransmitPower, NULL, NULL, RPC_RW, eInt, 0, 0},
	{ "IEEE80211hSupported", getWiFiRadio_IEEE80211hSupported, NULL, NULL, RPC_R, eBoolean, 0, 0},
	{ "IEEE80211hEnabled", getWiFiRadio_IEEE80211hEnabled, NULL, NULL, RPC_R, eBoolean, 0, 0},
	{ "RegulatoryDomain", getWiFiRadio_RegulatoryDomain, NULL, NULL, RPC_R, eString, 0, 3},
	{NULL}
};
/**@endparam Device.WiFi.Radio.{i}.  */

/**@obj Device.WiFi.Radio.{i}.  */
CPEADDOBJ(initWiFiRadioStats);
CWMPObject WiFiRadio_Objs[]={
	{"Stats", NULL, initWiFiRadioStats, NULL, NULL, NULL, WiFiRadioStats_Params, NULL, eStaticInstance,  0},
	{NULL}
};

/**@endobj Device.WiFi.Radio.{i}.  */
/**@param Device.WiFi.SSID.{i}.Stats. */
CPEGETFUNC(getWiFiSSIDStats_BytesSent);
CPEGETFUNC(getWiFiSSIDStats_BytesReceived);
CPEGETFUNC(getWiFiSSIDStats_PacketsSent);
CPEGETFUNC(getWiFiSSIDStats_PacketsReceived);
CPEGETFUNC(getWiFiSSIDStats_ErrorsSent);
CPEGETFUNC(getWiFiSSIDStats_ErrorsReceived);
CWMPParam WiFiSSIDStats_Params[]={
	{ "BytesSent", getWiFiSSIDStats_BytesSent, NULL, NULL, RPC_R, eUnsignedLong, 0, 0},
	{ "BytesReceived", getWiFiSSIDStats_BytesReceived, NULL, NULL, RPC_R, eUnsignedLong, 0, 0},
	{ "PacketsSent", getWiFiSSIDStats_PacketsSent, NULL, NULL, RPC_R, eUnsignedLong, 0, 0},
	{ "PacketsReceived", getWiFiSSIDStats_PacketsReceived, NULL, NULL, RPC_R, eUnsignedLong, 0, 0},
	{ "ErrorsSent", getWiFiSSIDStats_ErrorsSent, NULL, NULL, RPC_R, eUnsignedInt, 0, 0},
	{ "ErrorsReceived", getWiFiSSIDStats_ErrorsReceived, NULL, NULL, RPC_R, eUnsignedInt, 0, 0},
	{NULL}
};
/**@endparam Device.WiFi.SSID.{i}.Stats.  */

/**@param Device.WiFi.SSID.{i}. */
CPEGETFUNC(getWiFiSSID_Enable);
CPEGETFUNC(getWiFiSSID_Status);
CPEGETFUNC(getWiFiSSID_Name);
CPEGETFUNC(getWiFiSSID_BSSID);
CPEGETFUNC(getWiFiSSID_MACAddress);
CPEGETFUNC(getWiFiSSID_SSID);

#if 0
CPESETFUNC(setWiFiSSID_SSID);
CPESETFUNC(setWiFiSSID_Enable);
#endif

CWMPParam WiFiSSID_Params[]={
	{ "Enable", getWiFiSSID_Enable, NULL, NULL, RPC_R, eBoolean, 0, 0},
	{ "Status", getWiFiSSID_Status, NULL, NULL, RPC_R, eString, 0, 0},
	{ "Name", getWiFiSSID_Name, NULL, NULL, RPC_R, eString, 0, 64},
	{ "BSSID", getWiFiSSID_BSSID, NULL, NULL, RPC_R, eString, 0, 17},
	{ "MACAddress", getWiFiSSID_MACAddress, NULL, NULL, RPC_R, eString, 0, 17},
	{ "SSID", getWiFiSSID_SSID, NULL, NULL, RPC_R, eString, FORCED_INFORM | DEFAULT_ACTIVE , 32},
	{NULL}
};
/**@endparam Device.WiFi.SSID.{i}.  */

/**@obj Device.WiFi.SSID.{i}.  */
CPEADDOBJ(initWiFiSSIDStats);
CWMPObject WiFiSSID_Objs[]={
	{"Stats", NULL, initWiFiSSIDStats, NULL, NULL, NULL, WiFiSSIDStats_Params, NULL, eStaticInstance,  0},
	{NULL}
};

/**@endobj Device.WiFi.SSID.{i}.  */
/**@param Device.WiFi. */
CPEGETFUNC(getWiFi_RadioNumberOfEntries);
CPEGETFUNC(getWiFi_SSIDNumberOfEntries);
CPEGETFUNC(getWiFi_AccessPointNumberOfEntries);
CWMPParam WiFi_Params[]={
	{ "RadioNumberOfEntries", getWiFi_RadioNumberOfEntries, NULL, NULL, RPC_R, eUnsignedInt, 0, 0},
	{ "SSIDNumberOfEntries", getWiFi_SSIDNumberOfEntries, NULL, NULL, RPC_R, eUnsignedInt, 0, 0},
	{ "AccessPointNumberOfEntries", getWiFi_AccessPointNumberOfEntries, NULL, NULL, RPC_R, eUnsignedInt, 0, 0},
	{NULL}
};
/**@endparam Device.WiFi.  */

/**@obj Device.WiFi.  */
CPEDELOBJ(delWiFiAccessPoint);
CPEADDOBJ(addWiFiAccessPoint);
CPEADDOBJ(addWiFiRadio);
CPEADDOBJ(addWiFiSSID);
CWMPObject WiFi_Objs[]={
	{"AccessPoint", delWiFiAccessPoint, addWiFiAccessPoint, NULL, NULL, WiFiAccessPoint_Objs, WiFiAccessPoint_Params, NULL, eStaticInstance,  0},
	{"Radio", NULL, addWiFiRadio, NULL, NULL, WiFiRadio_Objs, WiFiRadio_Params, NULL, eStaticInstance,  0},
	{"SSID", NULL, addWiFiSSID, NULL, NULL, WiFiSSID_Objs, WiFiSSID_Params, NULL, eStaticInstance,  0},
	{NULL}
};
/**@endobj Device.WiFi.  */

/**@param Device.X_RJIL_MeshNode.  */
CPEGETFUNC(getX_RJIL_MeshNode_ParentAPMAC);
CPEGETFUNC(getX_RJIL_MeshNode_CAPMODE);
CWMPParam X_RJIL_MeshNode_Params[]={
	{ "ParentAPMAC", getX_RJIL_MeshNode_ParentAPMAC, NULL, NULL, RPC_R, eString, 0, 0},
	{ "CAPMode", getX_RJIL_MeshNode_CAPMODE, NULL, NULL, RPC_R, eBoolean, 0, 0},
	{NULL}
};
/**@endparam Device.X_RJIL_MeshNode.  */

/**@param Device.DeviceInfo.MemoryStatus. */
CPEGETFUNC(getDeviceInfoMemoryStatus_Total);
CPEGETFUNC(getDeviceInfoMemoryStatus_Free);
CWMPParam DeviceInfoMemoryStatus_Params[]={
        { "Total", getDeviceInfoMemoryStatus_Total, NULL, NULL, RPC_R, eUnsignedInt, 0 , 0},
        { "Free", getDeviceInfoMemoryStatus_Free, NULL, NULL, RPC_R, eUnsignedInt, 0 , 0},
        {NULL}
};
/**@endparam Device.DeviceInfo.MemoryStatus.  */

/**@param Device.DeviceInfo.ProcessStatus. */
CPEGETFUNC(getDeviceInfoProcessStatus_CPUUsage);
CWMPParam DeviceInfoProcessStatus_Params[]={
        { "CPUUsage", getDeviceInfoProcessStatus_CPUUsage, NULL, NULL, RPC_R, eUnsignedInt, 0 , 0},
        {NULL}
};
/**@endparam Device.DeviceInfo.ProcessStatus.  */

/**@param Device. */
//CPEGETFUNC(get_InterfaceStackNumberOfEntries);
CWMPParam Device_Params[]={
//	{ "InterfaceStackNumberOfEntries", get_InterfaceStackNumberOfEntries, NULL, NULL, RPC_R, eUnsignedInt, 0, 0},
	{NULL}
};
/**@endparam Device.  */

/**@obj Device.  */
CPEADDOBJ(initIP);
CPEADDOBJ(initWiFi);
//CPEADDOBJ(addInterfaceStack);
CPEADDOBJ(initX_RJIL_MeshNode);
CPEADDOBJ(initDeviceInfoMemoryStatus);
CPEADDOBJ(initDeviceInfoProcessStatus);
CWMPObject Device_Objs[]={
//	{"DNS", NULL, NULL, NULL, NULL, DNS_Objs, DNS_Params, NULL, eObject,  0},
	{"DeviceInfo", NULL, NULL, NULL, NULL, DeviceInfo_Objs, DeviceInfo_Params, NULL, eObject,  0},
	{"Ethernet", NULL, NULL, NULL, NULL, Ethernet_Objs, Ethernet_Params, NULL, eObject,  0},
//	{"Hosts", NULL, NULL, NULL, NULL, Hosts_Objs, Hosts_Params, NULL, eObject,  0},
	{"IP", NULL, initIP, NULL, NULL, IP_Objs, IP_Params, NULL, eObject,  0},
//	{"InterfaceStack", NULL, addInterfaceStack, NULL, NULL, NULL, InterfaceStack_ParamsInterfaceStack_Params, NULL, eStaticInstance,  0},
	{"LANConfigSecurity", NULL, NULL, NULL, NULL, NULL, LANConfigSecurity_Params, NULL, eObject,  0},
	{"ManagementServer", NULL, NULL, NULL, NULL, NULL, ManagementServer_Params, NULL, eObject,  0},
//	{"SoftwareModules", NULL, NULL, NULL, NULL, SoftwareModules_Objs, SoftwareModules_Params, NULL, eObject,  0},
	{"MemoryStatus", NULL, initDeviceInfoMemoryStatus, NULL, NULL, NULL, DeviceInfoMemoryStatus_Params, NULL, eObject,  0},
	{"ProcessStatus", NULL, initDeviceInfoProcessStatus, NULL, NULL, NULL, DeviceInfoProcessStatus_Params, NULL, eObject,  0},
	{"WiFi", NULL, initWiFi, NULL, NULL, WiFi_Objs, WiFi_Params, NULL, eObject,  0},
	{"X_RJIL_MeshNode", NULL, initX_RJIL_MeshNode, NULL, NULL, NULL, X_RJIL_MeshNode_Params, NULL, eObject,  0},
	{NULL}
};

/**@endobj Device.  */
/** CWMP ROOT Object Table  */

CWMPObject CWMP_RootObject[]={
	{"Device", NULL, NULL, NULL, NULL, Device_Objs, Device_Params, NULL, eObject, 0},
	{NULL}
};

