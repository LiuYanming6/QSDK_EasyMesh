#ifndef _GS__INTERNETGATEWAYDEVICE_H_
#define _GS__INTERNETGATEWAYDEVICE_H_

/**@obj DeviceInfo **/
typedef struct DeviceInfo {
	char *	manufacturer;
	char *	manufacturerOUI;
	char *	modelName;
	char *	description;
	char *	serialNumber;
	char *	hardwareVersion;
	char *	softwareVersion;
	char *	specVersion;
	char *	provisioningCode;
	unsigned	upTime;
	char *	deviceLog;
} DeviceInfo;
/**@endobj DeviceInfo **/

/**@obj LANConfigSecurity **/
typedef struct LANConfigSecurity {
	char *	configPassword;
} LANConfigSecurity;
/**@endobj LANConfigSecurity **/

/**@obj LANDeviceHostsHost **/
typedef struct LANDeviceHostsHost {
	char *	iPAddress;
	char *	addressSource;
	int	leaseTimeRemaining;
	char *	mACAddress;
	char *	hostName;
	char *	interfaceType;
	unsigned char	active;
} LANDeviceHostsHost;
/**@endobj LANDeviceHostsHost **/

/**@obj LANDeviceLANHostConfigManagementIPInterface **/
typedef struct LANDeviceLANHostConfigManagementIPInterface {
	unsigned char	enable;
	char *	iPInterfaceIPAddress;
	char *	iPInterfaceSubnetMask;
	char *	iPInterfaceAddressingType;
} LANDeviceLANHostConfigManagementIPInterface;
/**@endobj LANDeviceLANHostConfigManagementIPInterface **/

/**@obj LANDevice **/
typedef struct GS_LANDevice {
	unsigned	lANEthernetInterfaceNumberOfEntries;
	unsigned	lANUSBInterfaceNumberOfEntries;
	unsigned	lANWLANConfigurationNumberOfEntries;
} GS_LANDevice;
/**@endobj LANDevice **/

/**@obj Layer3ForwardingForwarding **/
typedef struct Layer3ForwardingForwarding {
	unsigned char	enable;
	char *	status;
	char *	type;
	char *	destIPAddress;
	char *	destSubnetMask;
	char *	sourceIPAddress;
	char *	sourceSubnetMask;
	char *	gatewayIPAddress;
	Instance *	interface;
	int	forwardingMetric;
} Layer3ForwardingForwarding;
/**@endobj Layer3ForwardingForwarding **/

/**@obj Layer3Forwarding **/
typedef struct Layer3Forwarding {
	char *	defaultConnectionService;
} Layer3Forwarding;
/**@endobj Layer3Forwarding **/

/**@obj ManagementServer **/
typedef struct ManagementServer {
	char *	uRL;
	char *	username;
	char *	password;
	unsigned char	periodicInformEnable;
	unsigned	periodicInformInterval;
	time_t	periodicInformTime;
	char *	parameterKey;
	char *	connectionRequestURL;
	char *	connectionRequestUsername;
	char *	connectionRequestPassword;
	unsigned char	upgradesManaged;
} ManagementServer;
/**@endobj ManagementServer **/

/**@obj WANDeviceWANCommonInterfaceConfig **/
typedef struct WANDeviceWANCommonInterfaceConfig {
	unsigned char	enabledForInternet;
	char *	wANAccessType;
	unsigned	layer1UpstreamMaxBitRate;
	unsigned	layer1DownstreamMaxBitRate;
	char *	physicalLinkStatus;
	unsigned	totalBytesSent;
	unsigned	totalBytesReceived;
	unsigned	totalPacketsSent;
	unsigned	totalPacketsReceived;
} WANDeviceWANCommonInterfaceConfig;
/**@endobj WANDeviceWANCommonInterfaceConfig **/

/**@obj WANDeviceWANConnectionDeviceWANIPConnectionPortMapping **/
typedef struct WANDeviceWANConnectionDeviceWANIPConnectionPortMapping {
	unsigned char	portMappingEnabled;
	unsigned	portMappingLeaseDuration;
	char *	remoteHost;
	unsigned	externalPort;
	unsigned	internalPort;
	char *	portMappingProtocol;
	char *	internalClient;
	char *	portMappingDescription;
} WANDeviceWANConnectionDeviceWANIPConnectionPortMapping;
/**@endobj WANDeviceWANConnectionDeviceWANIPConnectionPortMapping **/

/**@obj WANDeviceWANConnectionDeviceWANIPConnectionStats **/
typedef struct WANDeviceWANConnectionDeviceWANIPConnectionStats {
	unsigned	ethernetBytesSent;
	unsigned	ethernetBytesReceived;
	unsigned	ethernetPacketsSent;
	unsigned	ethernetPacketsReceived;
} WANDeviceWANConnectionDeviceWANIPConnectionStats;
/**@endobj WANDeviceWANConnectionDeviceWANIPConnectionStats **/

/**@obj WANDeviceWANConnectionDeviceWANIPConnection **/
typedef struct WANDeviceWANConnectionDeviceWANIPConnection {
	unsigned char	enable;
	char *	connectionStatus;
	char *	possibleConnectionTypes;
	char *	connectionType;
	char *	name;
	unsigned	uptime;
	char *	lastConnectionError;
	unsigned char	rSIPAvailable;
	unsigned char	nATEnabled;
	char *	addressingType;
	char *	externalIPAddress;
	char *	subnetMask;
	char *	defaultGateway;
	unsigned char	dNSEnabled;
	unsigned char	dNSOverrideAllowed;
	char *	dNSServers;
	char *	mACAddress;
	char *	connectionTrigger;
	char *	routeProtocolRx;
} WANDeviceWANConnectionDeviceWANIPConnection;
/**@endobj WANDeviceWANConnectionDeviceWANIPConnection **/

/**@obj WANDeviceWANConnectionDeviceWANPPPConnectionPortMapping **/
typedef struct WANDeviceWANConnectionDeviceWANPPPConnectionPortMapping {
	unsigned char	portMappingEnabled;
	unsigned	portMappingLeaseDuration;
	char *	remoteHost;
	unsigned	externalPort;
	unsigned	internalPort;
	char *	portMappingProtocol;
	char *	internalClient;
	char *	portMappingDescription;
} WANDeviceWANConnectionDeviceWANPPPConnectionPortMapping;
/**@endobj WANDeviceWANConnectionDeviceWANPPPConnectionPortMapping **/

/**@obj WANDeviceWANConnectionDeviceWANPPPConnectionStats **/
typedef struct WANDeviceWANConnectionDeviceWANPPPConnectionStats {
	unsigned	ethernetBytesSent;
	unsigned	ethernetBytesReceived;
	unsigned	ethernetPacketsSent;
	unsigned	ethernetPacketsReceived;
} WANDeviceWANConnectionDeviceWANPPPConnectionStats;
/**@endobj WANDeviceWANConnectionDeviceWANPPPConnectionStats **/

/**@obj WANDeviceWANConnectionDeviceWANPPPConnection **/
typedef struct WANDeviceWANConnectionDeviceWANPPPConnection {
	unsigned char	enable;
	char *	connectionStatus;
	char *	possibleConnectionTypes;
	char *	connectionType;
	char *	name;
	unsigned	uptime;
	char *	lastConnectionError;
	unsigned char	rSIPAvailable;
	unsigned char	nATEnabled;
	char *	username;
	char *	password;
	char *	externalIPAddress;
	unsigned char	dNSEnabled;
	unsigned char	dNSOverrideAllowed;
	char *	dNSServers;
	char *	mACAddress;
	char *	transportType;
	char *	pPPoEACName;
	char *	pPPoEServiceName;
	char *	connectionTrigger;
	char *	routeProtocolRx;
} WANDeviceWANConnectionDeviceWANPPPConnection;
/**@endobj WANDeviceWANConnectionDeviceWANPPPConnection **/

/**@obj  **/
typedef struct GS_ {
	char *	deviceSummary;
} GS_;
/**@endobj  **/

/**@obj IPPingDiagnostics **/
typedef struct IPPingDiagnostics {
	char *	diagnosticsState;
	Instance *	interface;
	char *	host;
	unsigned	numberOfRepetitions;
	unsigned	timeout;
	unsigned	dataBlockSize;
	unsigned	dSCP;
	unsigned	successCount;
	unsigned	failureCount;
	unsigned	averageResponseTime;
	unsigned	minimumResponseTime;
	unsigned	maximumResponseTime;
} IPPingDiagnostics;
/**@endobj IPPingDiagnostics **/

/**@obj Time **/
typedef struct GS_Time {
	char *	nTPServer1;
	char *	nTPServer2;
	time_t	currentLocalTime;
	char *	localTimeZone;
	char *	localTimeZoneName;
	unsigned char	daylightSavingsUsed;
	time_t	daylightSavingsStart;
	time_t	daylightSavingsEnd;
} GS_Time;
/**@endobj Time **/

/**@obj TraceRouteDiagnosticsRouteHops **/
typedef struct TraceRouteDiagnosticsRouteHops {
	char *	hopHost;
	char *	hopHostAddress;
	unsigned	hopErrorCode;
	unsigned	hopRTTimes;
} TraceRouteDiagnosticsRouteHops;
/**@endobj TraceRouteDiagnosticsRouteHops **/

/**@obj TraceRouteDiagnostics **/
typedef struct TraceRouteDiagnostics {
	char *	diagnosticsState;
	Instance *	interface;
	char *	host;
	unsigned	numberOfTries;
	unsigned	timeout;
	unsigned	dataBlockSize;
	unsigned	dSCP;
	unsigned	maxHopCount;
	unsigned	responseTime;
} TraceRouteDiagnostics;
/**@endobj TraceRouteDiagnostics **/

/**@obj LANDeviceLANHostConfigManagement **/
typedef struct LANDeviceLANHostConfigManagement {
	unsigned char	dHCPServerConfigurable;
	unsigned char	dHCPServerEnable;
	unsigned char	dHCPRelay;
	char *	minAddress;
	char *	maxAddress;
	char *	reservedAddresses;
	char *	subnetMask;
	char *	dNSServers;
	char *	domainName;
	char *	iPRouters;
} LANDeviceLANHostConfigManagement;
/**@endobj LANDeviceLANHostConfigManagement **/

/**@obj SoftwareModulesDeploymentUnit **/
typedef struct SoftwareModulesDeploymentUnit {
	char *	uUID;
	char *	dUID;
	char *	name;
	char *	status;
	unsigned char	resolved;
	char *	uRL;
	char *	vendor;
	char *	version;
	Instance *	executionUnitList[10+1];
	Instance *	executionEnvRef;
} SoftwareModulesDeploymentUnit;
/**@endobj SoftwareModulesDeploymentUnit **/

/**@obj SoftwareModulesExecEnv **/
typedef struct SoftwareModulesExecEnv {
	unsigned char	enable;
	char *	status;
	char *	name;
	char *	type;
	char *	vendor;
	char *	version;
	Instance *	activeExecutionUnits[10+1];
} SoftwareModulesExecEnv;
/**@endobj SoftwareModulesExecEnv **/

/**@obj SoftwareModulesExecutionUnit **/
typedef struct SoftwareModulesExecutionUnit {
	char *	eUID;
	char *	name;
	char *	execEnvLabel;
	char *	status;
	char *	requestedState;
	char *	executionFaultCode;
	char *	executionFaultMessage;
	char *	vendor;
	char *	version;
	Instance *	references[10+1];
	Instance *	supportedDataModelList[10+1];
} SoftwareModulesExecutionUnit;
/**@endobj SoftwareModulesExecutionUnit **/





#endif /* _GS__INTERNETGATEWAYDEVICE_H_ */
