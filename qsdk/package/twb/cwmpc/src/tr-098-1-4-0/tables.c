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
 * $Id: tables.c,v 1.1 2012/05/10 17:38:00 dmounday Exp $
 *----------------------------------------------------------------------*/

#include "paramTree.h"
#include "rpc.h"



/**  TraceRoute:1, UDPConnReq:1, SM_Baseline:1, Time:1, IPPing:1, 
    Baseline:1, 
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
	{ "SpecVersion", getDeviceInfo_SpecVersion, NULL, NULL, RPC_R, eString, FORCED_INFORM, 16},
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
CPEGETFUNC(getIPPingDiagnostics_MinimumResponseTime);
CPEGETFUNC(getIPPingDiagnostics_MaximumResponseTime);
CWMPParam IPPingDiagnostics_Params[]={
	{ "DiagnosticsState", getIPPingDiagnostics_DiagnosticsState, setIPPingDiagnostics_DiagnosticsState, NULL, RPC_RW, eString, NOACTIVENOTIFY, 0},
	{ "Interface", getIPPingDiagnostics_Interface, setIPPingDiagnostics_Interface, NULL, RPC_RW,  eString, 0, 256},
	{ "Host", getIPPingDiagnostics_Host, setIPPingDiagnostics_Host, NULL, RPC_RW, eString, 0, 256},
	{ "NumberOfRepetitions", getIPPingDiagnostics_NumberOfRepetitions, setIPPingDiagnostics_NumberOfRepetitions, NULL, RPC_RW, eUnsignedInt, 0, 0},
	{ "Timeout", getIPPingDiagnostics_Timeout, setIPPingDiagnostics_Timeout, NULL, RPC_RW, eUnsignedInt, 0, 0},
	{ "DataBlockSize", getIPPingDiagnostics_DataBlockSize, setIPPingDiagnostics_DataBlockSize, NULL, RPC_RW, eUnsignedInt, 0, 0},
	{ "DSCP", getIPPingDiagnostics_DSCP, setIPPingDiagnostics_DSCP, NULL, RPC_RW, eUnsignedInt, 0, 0},
	{ "SuccessCount", getIPPingDiagnostics_SuccessCount, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "FailureCount", getIPPingDiagnostics_FailureCount, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "AverageResponseTime", getIPPingDiagnostics_AverageResponseTime, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "MinimumResponseTime", getIPPingDiagnostics_MinimumResponseTime, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "MaximumResponseTime", getIPPingDiagnostics_MaximumResponseTime, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{NULL}
};
/**@endparam InternetGatewayDevice.IPPingDiagnostics.  */

/**@param InternetGatewayDevice.LANConfigSecurity. */
CPEGETFUNC(getLANConfigSecurity_ConfigPassword);
CPESETFUNC(setLANConfigSecurity_ConfigPassword);
CWMPParam LANConfigSecurity_Params[]={
	{ "ConfigPassword", getLANConfigSecurity_ConfigPassword, setLANConfigSecurity_ConfigPassword, NULL, RPC_RW, eString, 0, 64},
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
CWMPParam LANDeviceHostsHost_Params[]={
	{ "IPAddress", getLANDeviceHostsHost_IPAddress, NULL, NULL, RPC_R, eString, 0, 45},
	{ "AddressSource", getLANDeviceHostsHost_AddressSource, NULL, NULL, RPC_R, eString, 0, 0},
	{ "LeaseTimeRemaining", getLANDeviceHostsHost_LeaseTimeRemaining, NULL, NULL, RPC_R, eInt, NOACTIVENOTIFY, 0},
	{ "MACAddress", getLANDeviceHostsHost_MACAddress, NULL, NULL, RPC_R, eString, 0, 17},
	{ "HostName", getLANDeviceHostsHost_HostName, NULL, NULL, RPC_R, eString, 0, 64},
	{ "InterfaceType", getLANDeviceHostsHost_InterfaceType, NULL, NULL, RPC_R, eString, 0, 0},
	{ "Active", getLANDeviceHostsHost_Active, NULL, NULL, RPC_R, eBoolean, 0, 0},
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
CPEADDOBJ(addLANDeviceHostsHost);
CPEDELOBJ(delLANDeviceHostsHost);
CWMPObject LANDeviceHosts_Objs[]={
	{"Host", delLANDeviceHostsHost, addLANDeviceHostsHost, NULL, NULL, NULL, LANDeviceHostsHost_Params, NULL, eCPEInstance,  0},
	{NULL}
};

/**@endobj InternetGatewayDevice.LANDevice.{i}.Hosts.  */
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
	{NULL}
};
/**@endparam InternetGatewayDevice.LANDevice.{i}.LANHostConfigManagement.  */

/**@obj InternetGatewayDevice.LANDevice.{i}.LANHostConfigManagement.  */
CPEADDOBJ(addLANDeviceLANHostConfigManagementIPInterface);
CPEDELOBJ(delLANDeviceLANHostConfigManagementIPInterface);
CPECOMMIT(commitLANDeviceLANHostConfigManagementIPInterface);
CWMPObject LANDeviceLANHostConfigManagement_Objs[]={
	{"IPInterface", delLANDeviceLANHostConfigManagementIPInterface, addLANDeviceLANHostConfigManagementIPInterface, commitLANDeviceLANHostConfigManagementIPInterface, NULL, NULL, LANDeviceLANHostConfigManagementIPInterface_Params, NULL, eInstance,  0},
	{NULL}
};

/**@endobj InternetGatewayDevice.LANDevice.{i}.LANHostConfigManagement.  */
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
CPEADDOBJ(initLANDeviceHosts);
CWMPObject LANDevice_Objs[]={
	{"Hosts", NULL, initLANDeviceHosts, NULL, NULL, LANDeviceHosts_Objs, LANDeviceHosts_Params, NULL, eObject,  0},
	{"LANHostConfigManagement", NULL, NULL, NULL, NULL, LANDeviceLANHostConfigManagement_Objs, LANDeviceLANHostConfigManagement_Params, NULL, eObject,  0},
	{NULL}
};

/**@endobj InternetGatewayDevice.LANDevice.{i}.  */
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
CWMPParam Layer3ForwardingForwarding_Params[]={
	{ "Enable", getLayer3ForwardingForwarding_Enable, setLayer3ForwardingForwarding_Enable, NULL, RPC_RW, eBoolean, 0, 0},
	{ "Status", getLayer3ForwardingForwarding_Status, NULL, NULL, RPC_R, eString, 0, 0},
	{ "Type", getLayer3ForwardingForwarding_Type, setLayer3ForwardingForwarding_Type, NULL, RPC_RW, eString, 0, 0},
	{ "DestIPAddress", getLayer3ForwardingForwarding_DestIPAddress, setLayer3ForwardingForwarding_DestIPAddress, NULL, RPC_RW, eString, 0, 45},
	{ "DestSubnetMask", getLayer3ForwardingForwarding_DestSubnetMask, setLayer3ForwardingForwarding_DestSubnetMask, NULL, RPC_RW, eString, 0, 45},
	{ "SourceIPAddress", getLayer3ForwardingForwarding_SourceIPAddress, setLayer3ForwardingForwarding_SourceIPAddress, NULL, RPC_RW, eString, 0, 45},
	{ "SourceSubnetMask", getLayer3ForwardingForwarding_SourceSubnetMask, setLayer3ForwardingForwarding_SourceSubnetMask, NULL, RPC_RW, eString, 0, 45},
	{ "GatewayIPAddress", getLayer3ForwardingForwarding_GatewayIPAddress, setLayer3ForwardingForwarding_GatewayIPAddress, NULL, RPC_RW, eString, 0, 45},
	{ "Interface", getLayer3ForwardingForwarding_Interface, setLayer3ForwardingForwarding_Interface, NULL, RPC_RW,  eString, 0, 256},
	{ "ForwardingMetric", getLayer3ForwardingForwarding_ForwardingMetric, setLayer3ForwardingForwarding_ForwardingMetric, NULL, RPC_RW, eInt, 0, 0},
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
CPEGETFUNC(getManagementServer_CWMPRetryMinimumWaitInterval);
CPESETFUNC(setManagementServer_CWMPRetryMinimumWaitInterval);
CPEGETFUNC(getManagementServer_CWMPRetryIntervalMultiplier);
CPESETFUNC(setManagementServer_CWMPRetryIntervalMultiplier);
CWMPParam ManagementServer_Params[]={
	{ "URL", getManagementServer_URL, setManagementServer_URL, NULL, RPC_RW, eString, 0, 256},
	{ "Username", getManagementServer_Username, setManagementServer_Username, NULL, RPC_RW, eString, 0, 256},
	{ "Password", getManagementServer_Password, setManagementServer_Password, NULL, RPC_RW, eString, 0, 256},
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
	{ "CWMPRetryMinimumWaitInterval", getManagementServer_CWMPRetryMinimumWaitInterval, setManagementServer_CWMPRetryMinimumWaitInterval, NULL, RPC_RW, eUnsignedInt, 0, 0},
	{ "CWMPRetryIntervalMultiplier", getManagementServer_CWMPRetryIntervalMultiplier, setManagementServer_CWMPRetryIntervalMultiplier, NULL, RPC_RW, eUnsignedInt, 0, 0},
	{NULL}
};
/**@endparam InternetGatewayDevice.ManagementServer.  */

/**@param InternetGatewayDevice.SoftwareModules.DeploymentUnit.{i}. */
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
/**@endparam InternetGatewayDevice.SoftwareModules.DeploymentUnit.{i}.  */

/**@param InternetGatewayDevice.SoftwareModules.ExecEnv.{i}. */
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
	{ "Name", getSoftwareModulesExecEnv_Name, NULL, NULL, RPC_R, eString, 0, 32},
	{ "Type", getSoftwareModulesExecEnv_Type, NULL, NULL, RPC_R, eString, 0, 64},
	{ "Vendor", getSoftwareModulesExecEnv_Vendor, NULL, NULL, RPC_R, eString, 0, 128},
	{ "Version", getSoftwareModulesExecEnv_Version, NULL, NULL, RPC_R, eString, 0, 32},
	{ "ActiveExecutionUnits", getSoftwareModulesExecEnv_ActiveExecutionUnits, NULL, NULL, RPC_R,  eString, NOACTIVENOTIFY, 0},
	{NULL}
};
/**@endparam InternetGatewayDevice.SoftwareModules.ExecEnv.{i}.  */

/**@param InternetGatewayDevice.SoftwareModules.ExecutionUnit.{i}. */
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
/**@endparam InternetGatewayDevice.SoftwareModules.ExecutionUnit.{i}.  */

/**@obj InternetGatewayDevice.SoftwareModules.ExecutionUnit.{i}.  */
CWMPObject SoftwareModulesExecutionUnit_Objs[]={
	{"Extensions", NULL, NULL, NULL, NULL, NULL, NULL, NULL, eObject,  0},
	{NULL}
};

/**@endobj InternetGatewayDevice.SoftwareModules.ExecutionUnit.{i}.  */
/**@param InternetGatewayDevice.SoftwareModules. */
CPEGETFUNC(getSoftwareModules_ExecEnvNumberOfEntries);
CPEGETFUNC(getSoftwareModules_DeploymentUnitNumberOfEntries);
CPEGETFUNC(getSoftwareModules_ExecutionUnitNumberOfEntries);
CWMPParam SoftwareModules_Params[]={
	{ "ExecEnvNumberOfEntries", getSoftwareModules_ExecEnvNumberOfEntries, NULL, NULL, RPC_R, eUnsignedInt, 0, 0},
	{ "DeploymentUnitNumberOfEntries", getSoftwareModules_DeploymentUnitNumberOfEntries, NULL, NULL, RPC_R, eUnsignedInt, 0, 0},
	{ "ExecutionUnitNumberOfEntries", getSoftwareModules_ExecutionUnitNumberOfEntries, NULL, NULL, RPC_R, eUnsignedInt, 0, 0},
	{NULL}
};
/**@endparam InternetGatewayDevice.SoftwareModules.  */

/**@obj InternetGatewayDevice.SoftwareModules.  */
CPEADDOBJ(addSoftwareModulesDeploymentUnit);
CPEDELOBJ(delSoftwareModulesDeploymentUnit);
CPEADDOBJ(addSoftwareModulesExecEnv);
CPEDELOBJ(delSoftwareModulesExecEnv);
CPEADDOBJ(addSoftwareModulesExecutionUnit);
CPEDELOBJ(delSoftwareModulesExecutionUnit);
CWMPObject SoftwareModules_Objs[]={
	{"DeploymentUnit", delSoftwareModulesDeploymentUnit, addSoftwareModulesDeploymentUnit, NULL, NULL, NULL, SoftwareModulesDeploymentUnit_Params, NULL, eCPEInstance,  0},
	{"ExecEnv", delSoftwareModulesExecEnv, addSoftwareModulesExecEnv, NULL, NULL, NULL, SoftwareModulesExecEnv_Params, NULL, eCPEInstance,  0},
	{"ExecutionUnit", delSoftwareModulesExecutionUnit, addSoftwareModulesExecutionUnit, NULL, NULL, SoftwareModulesExecutionUnit_Objs, SoftwareModulesExecutionUnit_Params, NULL, eCPEInstance,  0},
	{NULL}
};

/**@endobj InternetGatewayDevice.SoftwareModules.  */
/**@param InternetGatewayDevice.Time. */
CPEGETFUNC(getTime_NTPServer1);
CPESETFUNC(setTime_NTPServer1);
CPEGETFUNC(getTime_NTPServer2);
CPESETFUNC(setTime_NTPServer2);
CPEGETFUNC(getTime_CurrentLocalTime);
CPEGETFUNC(getTime_LocalTimeZoneName);
CPESETFUNC(setTime_LocalTimeZoneName);
CWMPParam Time_Params[]={
	{ "NTPServer1", getTime_NTPServer1, setTime_NTPServer1, NULL, RPC_RW, eString, 0, 64},
	{ "NTPServer2", getTime_NTPServer2, setTime_NTPServer2, NULL, RPC_RW, eString, 0, 64},
	{ "CurrentLocalTime", getTime_CurrentLocalTime, NULL, NULL, RPC_R, eDateTime, NOACTIVENOTIFY, 0},
	{ "LocalTimeZoneName", getTime_LocalTimeZoneName, setTime_LocalTimeZoneName, NULL, RPC_RW, eString, 0, 64},
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
CPEADDOBJ(addTraceRouteDiagnosticsRouteHops);
CPEDELOBJ(delTraceRouteDiagnosticsRouteHops);
CWMPObject TraceRouteDiagnostics_Objs[]={
	{"RouteHops", delTraceRouteDiagnosticsRouteHops, addTraceRouteDiagnosticsRouteHops, NULL, NULL, NULL, TraceRouteDiagnosticsRouteHops_Params, NULL, eCPEInstance,  0},
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
CWMPParam WANDeviceWANConnectionDeviceWANIPConnectionStats_Params[]={
	{ "EthernetBytesSent", getWANDeviceWANConnectionDeviceWANIPConnectionStats_EthernetBytesSent, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "EthernetBytesReceived", getWANDeviceWANConnectionDeviceWANIPConnectionStats_EthernetBytesReceived, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "EthernetPacketsSent", getWANDeviceWANConnectionDeviceWANIPConnectionStats_EthernetPacketsSent, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "EthernetPacketsReceived", getWANDeviceWANConnectionDeviceWANIPConnectionStats_EthernetPacketsReceived, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
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
	{ "ExternalIPAddress", getWANDeviceWANConnectionDeviceWANIPConnection_ExternalIPAddress, setWANDeviceWANConnectionDeviceWANIPConnection_ExternalIPAddress, NULL, RPC_RW, eString, FORCED_INFORM|DEFAULT_ACTIVE, 45},
	{ "SubnetMask", getWANDeviceWANConnectionDeviceWANIPConnection_SubnetMask, setWANDeviceWANConnectionDeviceWANIPConnection_SubnetMask, NULL, RPC_RW, eString, 0, 45},
	{ "DefaultGateway", getWANDeviceWANConnectionDeviceWANIPConnection_DefaultGateway, setWANDeviceWANConnectionDeviceWANIPConnection_DefaultGateway, NULL, RPC_RW, eString, 0, 0},
	{ "DNSEnabled", getWANDeviceWANConnectionDeviceWANIPConnection_DNSEnabled, setWANDeviceWANConnectionDeviceWANIPConnection_DNSEnabled, NULL, RPC_RW, eBoolean, 0, 0},
	{ "DNSOverrideAllowed", getWANDeviceWANConnectionDeviceWANIPConnection_DNSOverrideAllowed, setWANDeviceWANConnectionDeviceWANIPConnection_DNSOverrideAllowed, NULL, RPC_RW, eBoolean, 0, 0},
	{ "DNSServers", getWANDeviceWANConnectionDeviceWANIPConnection_DNSServers, setWANDeviceWANConnectionDeviceWANIPConnection_DNSServers, NULL, RPC_RW, eString, 0, 45},
	{ "MACAddress", getWANDeviceWANConnectionDeviceWANIPConnection_MACAddress, setWANDeviceWANConnectionDeviceWANIPConnection_MACAddress, NULL, RPC_RW, eString, 0, 17},
	{ "ConnectionTrigger", getWANDeviceWANConnectionDeviceWANIPConnection_ConnectionTrigger, setWANDeviceWANConnectionDeviceWANIPConnection_ConnectionTrigger, NULL, RPC_RW, eString, 0, 0},
	{ "RouteProtocolRx", getWANDeviceWANConnectionDeviceWANIPConnection_RouteProtocolRx, setWANDeviceWANConnectionDeviceWANIPConnection_RouteProtocolRx, NULL, RPC_RW, eString, 0, 0},
	{ "PortMappingNumberOfEntries", getWANDeviceWANConnectionDeviceWANIPConnection_PortMappingNumberOfEntries, NULL, NULL, RPC_R, eUnsignedInt, 0, 0},
	{NULL}
};
/**@endparam InternetGatewayDevice.WANDevice.{i}.WANConnectionDevice.{i}.WANIPConnection.{i}.  */

/**@obj InternetGatewayDevice.WANDevice.{i}.WANConnectionDevice.{i}.WANIPConnection.{i}.  */
CPEADDOBJ(addWANDeviceWANConnectionDeviceWANIPConnectionPortMapping);
CPEDELOBJ(delWANDeviceWANConnectionDeviceWANIPConnectionPortMapping);
CWMPObject WANDeviceWANConnectionDeviceWANIPConnection_Objs[]={
	{"PortMapping", delWANDeviceWANConnectionDeviceWANIPConnectionPortMapping, addWANDeviceWANConnectionDeviceWANIPConnectionPortMapping, NULL, NULL, NULL, WANDeviceWANConnectionDeviceWANIPConnectionPortMapping_Params, NULL, eInstance,  0},
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
	{ "PortMappingDescription", getWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_PortMappingDescription, setWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_PortMappingDescription, NULL, RPC_RW, eString, 0, 256},
	{NULL}
};
/**@endparam InternetGatewayDevice.WANDevice.{i}.WANConnectionDevice.{i}.WANPPPConnection.{i}.PortMapping.{i}.  */

/**@param InternetGatewayDevice.WANDevice.{i}.WANConnectionDevice.{i}.WANPPPConnection.{i}.Stats. */
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANPPPConnectionStats_EthernetBytesSent);
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANPPPConnectionStats_EthernetBytesReceived);
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANPPPConnectionStats_EthernetPacketsSent);
CPEGETFUNC(getWANDeviceWANConnectionDeviceWANPPPConnectionStats_EthernetPacketsReceived);
CWMPParam WANDeviceWANConnectionDeviceWANPPPConnectionStats_Params[]={
	{ "EthernetBytesSent", getWANDeviceWANConnectionDeviceWANPPPConnectionStats_EthernetBytesSent, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "EthernetBytesReceived", getWANDeviceWANConnectionDeviceWANPPPConnectionStats_EthernetBytesReceived, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "EthernetPacketsSent", getWANDeviceWANConnectionDeviceWANPPPConnectionStats_EthernetPacketsSent, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
	{ "EthernetPacketsReceived", getWANDeviceWANConnectionDeviceWANPPPConnectionStats_EthernetPacketsReceived, NULL, NULL, RPC_R, eUnsignedInt, NOACTIVENOTIFY, 0},
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
CPESETFUNC(setWANDeviceWANConnectionDeviceWANPPPConnection_ExternalIPAddress);
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
	{ "Username", getWANDeviceWANConnectionDeviceWANPPPConnection_Username, setWANDeviceWANConnectionDeviceWANPPPConnection_Username, NULL, RPC_RW, eString, 0, 64},
	{ "Password", getWANDeviceWANConnectionDeviceWANPPPConnection_Password, setWANDeviceWANConnectionDeviceWANPPPConnection_Password, NULL, RPC_RW, eString, 0, 64},
	{ "ExternalIPAddress", getWANDeviceWANConnectionDeviceWANPPPConnection_ExternalIPAddress, setWANDeviceWANConnectionDeviceWANPPPConnection_ExternalIPAddress, NULL, RPC_RW, eString, FORCED_INFORM|DEFAULT_ACTIVE, 45},
	{ "DNSEnabled", getWANDeviceWANConnectionDeviceWANPPPConnection_DNSEnabled, setWANDeviceWANConnectionDeviceWANPPPConnection_DNSEnabled, NULL, RPC_RW, eBoolean, 0, 0},
	{ "DNSOverrideAllowed", getWANDeviceWANConnectionDeviceWANPPPConnection_DNSOverrideAllowed, setWANDeviceWANConnectionDeviceWANPPPConnection_DNSOverrideAllowed, NULL, RPC_RW, eBoolean, 0, 0},
	{ "DNSServers", getWANDeviceWANConnectionDeviceWANPPPConnection_DNSServers, setWANDeviceWANConnectionDeviceWANPPPConnection_DNSServers, NULL, RPC_RW, eString, 0, 45},
	{ "MACAddress", getWANDeviceWANConnectionDeviceWANPPPConnection_MACAddress, setWANDeviceWANConnectionDeviceWANPPPConnection_MACAddress, NULL, RPC_RW, eString, 0, 17},
	{ "TransportType", getWANDeviceWANConnectionDeviceWANPPPConnection_TransportType, NULL, NULL, RPC_R, eString, 0, 0},
	{ "PPPoEACName", getWANDeviceWANConnectionDeviceWANPPPConnection_PPPoEACName, setWANDeviceWANConnectionDeviceWANPPPConnection_PPPoEACName, NULL, RPC_RW, eString, 0, 256},
	{ "PPPoEServiceName", getWANDeviceWANConnectionDeviceWANPPPConnection_PPPoEServiceName, setWANDeviceWANConnectionDeviceWANPPPConnection_PPPoEServiceName, NULL, RPC_RW, eString, 0, 256},
	{ "ConnectionTrigger", getWANDeviceWANConnectionDeviceWANPPPConnection_ConnectionTrigger, setWANDeviceWANConnectionDeviceWANPPPConnection_ConnectionTrigger, NULL, RPC_RW, eString, 0, 0},
	{ "RouteProtocolRx", getWANDeviceWANConnectionDeviceWANPPPConnection_RouteProtocolRx, setWANDeviceWANConnectionDeviceWANPPPConnection_RouteProtocolRx, NULL, RPC_RW, eString, 0, 0},
	{ "PortMappingNumberOfEntries", getWANDeviceWANConnectionDeviceWANPPPConnection_PortMappingNumberOfEntries, NULL, NULL, RPC_R, eUnsignedInt, 0, 0},
	{NULL}
};
/**@endparam InternetGatewayDevice.WANDevice.{i}.WANConnectionDevice.{i}.WANPPPConnection.{i}.  */

/**@obj InternetGatewayDevice.WANDevice.{i}.WANConnectionDevice.{i}.WANPPPConnection.{i}.  */
CPEADDOBJ(addWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping);
CPEDELOBJ(delWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping);
CWMPObject WANDeviceWANConnectionDeviceWANPPPConnection_Objs[]={
	{"PortMapping", delWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping, addWANDeviceWANConnectionDeviceWANPPPConnectionPortMapping, NULL, NULL, NULL, WANDeviceWANConnectionDeviceWANPPPConnectionPortMapping_Params, NULL, eInstance,  0},
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
	{"WANIPConnection", delWANDeviceWANConnectionDeviceWANIPConnection, addWANDeviceWANConnectionDeviceWANIPConnection, NULL, NULL, WANDeviceWANConnectionDeviceWANIPConnection_Objs, WANDeviceWANConnectionDeviceWANIPConnection_Params, NULL, eInstance,  0},
	{"WANPPPConnection", delWANDeviceWANConnectionDeviceWANPPPConnection, addWANDeviceWANConnectionDeviceWANPPPConnection, NULL, NULL, WANDeviceWANConnectionDeviceWANPPPConnection_Objs, WANDeviceWANConnectionDeviceWANPPPConnection_Params, NULL, eInstance,  0},
	{NULL}
};

/**@endobj InternetGatewayDevice.WANDevice.{i}.WANConnectionDevice.{i}.  */
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
CWMPObject WANDevice_Objs[]={
	{"WANCommonInterfaceConfig", NULL, NULL, NULL, NULL, NULL, WANDeviceWANCommonInterfaceConfig_Params, NULL, eObject,  0},
	{"WANConnectionDevice", delWANDeviceWANConnectionDevice, addWANDeviceWANConnectionDevice, NULL, NULL, WANDeviceWANConnectionDevice_Objs, WANDeviceWANConnectionDevice_Params, NULL, eInstance,  0},
	{NULL}
};

/**@endobj InternetGatewayDevice.WANDevice.{i}.  */
/**@param InternetGatewayDevice. */
CPEGETFUNC(get_DeviceSummary);
CPEGETFUNC(get_LANDeviceNumberOfEntries);
CPEGETFUNC(get_WANDeviceNumberOfEntries);
CWMPParam InternetGatewayDevice_Params[]={
	{ "DeviceSummary", get_DeviceSummary, NULL, NULL, RPC_R, eString, FORCED_INFORM, 1024},
	{ "LANDeviceNumberOfEntries", get_LANDeviceNumberOfEntries, NULL, NULL, RPC_R, eUnsignedInt, 0, 0},
	{ "WANDeviceNumberOfEntries", get_WANDeviceNumberOfEntries, NULL, NULL, RPC_R, eUnsignedInt, 0, 0},
	{NULL}
};
/**@endparam InternetGatewayDevice.  */

/**@obj InternetGatewayDevice.  */
CPEADDOBJ(initIPPingDiagnostics);
CPECOMMIT(commitIPPingDiagnostics);
CPEADDOBJ(addLANDevice);
CPEDELOBJ(delLANDevice);
CPEADDOBJ(initLayer3Forwarding);
CPECOMMIT(commitManagementServer);
CPEADDOBJ(initSoftwareModules);
CPEADDOBJ(initTraceRouteDiagnostics);
CPECOMMIT(commitTraceRouteDiagnostics);
CPEADDOBJ(addWANDevice);
CPEDELOBJ(delWANDevice);
CWMPObject InternetGatewayDevice_Objs[]={
	{"DeviceInfo", NULL, NULL, NULL, NULL, NULL, DeviceInfo_Params, NULL, eObject,  0},
	{"IPPingDiagnostics", NULL, initIPPingDiagnostics, commitIPPingDiagnostics, NULL, NULL, IPPingDiagnostics_Params, NULL, eObject,  0},
	{"LANConfigSecurity", NULL, NULL, NULL, NULL, NULL, LANConfigSecurity_Params, NULL, eObject,  0},
	{"LANDevice", delLANDevice, addLANDevice, NULL, NULL, LANDevice_Objs, LANDevice_Params, NULL, eInstance,  0},
	{"Layer3Forwarding", NULL, initLayer3Forwarding, NULL, NULL, Layer3Forwarding_Objs, Layer3Forwarding_Params, NULL, eObject,  0},
	{"ManagementServer", NULL, NULL, commitManagementServer, NULL, NULL, ManagementServer_Params, NULL, eObject,  0},
	{"SoftwareModules", NULL, initSoftwareModules, NULL, NULL, SoftwareModules_Objs, SoftwareModules_Params, NULL, eObject,  0},
	{"Time", NULL, NULL, NULL, NULL, NULL, Time_Params, NULL, eObject,  0},
	{"TraceRouteDiagnostics", NULL, initTraceRouteDiagnostics, commitTraceRouteDiagnostics, NULL, TraceRouteDiagnostics_Objs, TraceRouteDiagnostics_Params, NULL, eObject,  0},
	{"WANDevice", delWANDevice, addWANDevice, NULL, NULL, WANDevice_Objs, WANDevice_Params, NULL, eCPEInstance,  0},
	{NULL}
};

/**@endobj InternetGatewayDevice.  */
/** CWMP ROOT Object Table  */

CWMPObject CWMP_RootObject[]={
	{"InternetGatewayDevice", NULL, NULL, NULL, NULL, InternetGatewayDevice_Objs, InternetGatewayDevice_Params, NULL, eObject, 0},
	{NULL}
};

