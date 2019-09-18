#ifndef _GS__DEVICE_H_
#define _GS__DEVICE_H_

/**@obj DNSClientServer **/
typedef struct DNSClientServer {
	unsigned char	enable;
	char *	status;
	char *	dNSServer;
	Instance *	interface;
	char *	type;
} DNSClientServer;
/**@endobj DNSClientServer **/

/**@obj DNSClient **/
typedef struct GS_DNSClient {
	unsigned char	enable;
	char *	status;
} GS_DNSClient;
/**@endobj DNSClient **/

/**@obj DNS **/
typedef struct GS_DNS {
	char *	supportedRecordTypes;
} GS_DNS;
/**@endobj DNS **/

/**@obj DeviceInfo **/
typedef struct DeviceInfo {
	char *	manufacturer;
	char *	manufacturerOUI;
	char *	modelName;
	char *	description;
	char *	serialNumber;
	char *	hardwareVersion;
	char *	softwareVersion;
	char *	provisioningCode;
	unsigned	upTime;
} DeviceInfo;
/**@endobj DeviceInfo **/

/**@obj EthernetInterfaceStats **/
typedef struct EthernetInterfaceStats {
	long long	bytesSent;
	long long	bytesReceived;
	long long	packetsSent;
	long long	packetsReceived;
	unsigned	errorsSent;
	unsigned	errorsReceived;
	long long	unicastPacketsSent;
	long long	unicastPacketsReceived;
	unsigned	discardPacketsSent;
	unsigned	discardPacketsReceived;
	long long	multicastPacketsSent;
	long long	multicastPacketsReceived;
	long long	broadcastPacketsSent;
	long long	broadcastPacketsReceived;
	unsigned	unknownProtoPacketsReceived;
} EthernetInterfaceStats;
/**@endobj EthernetInterfaceStats **/

/**@obj EthernetInterface **/
typedef struct EthernetInterface {
	unsigned char	enable;
	char *	status;
	char *	name;
	unsigned	lastChange;
	unsigned char	upstream;
	char *	mACAddress;
	int	maxBitRate;
	char *	duplexMode;
} EthernetInterface;
/**@endobj EthernetInterface **/

/**@obj EthernetLinkStats **/
typedef struct EthernetLinkStats {
	long long	bytesSent;
	long long	bytesReceived;
	long long	packetsSent;
	long long	packetsReceived;
	unsigned	errorsSent;
	unsigned	errorsReceived;
	long long	unicastPacketsSent;
	long long	unicastPacketsReceived;
	unsigned	discardPacketsSent;
	unsigned	discardPacketsReceived;
	long long	multicastPacketsSent;
	long long	multicastPacketsReceived;
	long long	broadcastPacketsSent;
	long long	broadcastPacketsReceived;
	unsigned	unknownProtoPacketsReceived;
} EthernetLinkStats;
/**@endobj EthernetLinkStats **/

/**@obj EthernetLink **/
typedef struct EthernetLink {
	unsigned char	enable;
	char *	status;
	char *	name;
	unsigned	lastChange;
	Instance *	lowerLayers[10+1];
	char *	mACAddress;
} EthernetLink;
/**@endobj EthernetLink **/

/**@obj HostsHost **/
typedef struct GS_HostsHost {
	char *	physAddress;
	char *	iPAddress;
	char *	addressSource;
	int	leaseTimeRemaining;
	Instance *	layer1Interface;
	Instance *	layer3Interface;
	char *	hostName;
	unsigned char	active;
} GS_HostsHost;
/**@endobj HostsHost **/

/**@obj IPDiagnosticsIPPing **/
typedef struct IPDiagnosticsIPPing {
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
} IPDiagnosticsIPPing;
/**@endobj IPDiagnosticsIPPing **/

/**@obj IPDiagnosticsTraceRouteRouteHops **/
typedef struct IPDiagnosticsTraceRouteRouteHops {
	char *	host;
	char *	hostAddress;
	unsigned	errorCode;
	unsigned	rTTimes;
} IPDiagnosticsTraceRouteRouteHops;
/**@endobj IPDiagnosticsTraceRouteRouteHops **/

/**@obj IPDiagnosticsTraceRoute **/
typedef struct IPDiagnosticsTraceRoute {
	char *	diagnosticsState;
	Instance *	interface;
	char *	host;
	unsigned	numberOfTries;
	unsigned	timeout;
	unsigned	dataBlockSize;
	unsigned	dSCP;
	unsigned	maxHopCount;
	unsigned	responseTime;
} IPDiagnosticsTraceRoute;
/**@endobj IPDiagnosticsTraceRoute **/

/**@obj IPInterfaceIPv4Address **/
typedef struct IPInterfaceIPv4Address {
	unsigned char	enable;
	char *	iPAddress;
	char *	subnetMask;
	char *	addressingType;
} IPInterfaceIPv4Address;
/**@endobj IPInterfaceIPv4Address **/

/**@obj IPInterfaceIPv6Address **/
typedef struct IPInterfaceIPv6Address {
	unsigned char	enable;
	char *	status;
	char *	iPAddressStatus;
	char *	iPAddress;
	char *	origin;
	Instance *	prefix;
	time_t	preferredLifetime;
	time_t	validLifetime;
} IPInterfaceIPv6Address;
/**@endobj IPInterfaceIPv6Address **/

/**@obj IPInterfaceIPv6Prefix **/
typedef struct IPInterfaceIPv6Prefix {
	unsigned char	enable;
	char *	status;
	char *	prefixStatus;
	char *	prefix;
	char *	origin;
	unsigned char	onLink;
	unsigned char	autonomous;
	time_t	preferredLifetime;
	time_t	validLifetime;
} IPInterfaceIPv6Prefix;
/**@endobj IPInterfaceIPv6Prefix **/

/**@obj IPInterfaceStats **/
typedef struct IPInterfaceStats {
	long long	bytesSent;
	long long	bytesReceived;
	long long	packetsSent;
	long long	packetsReceived;
	unsigned	errorsSent;
	unsigned	errorsReceived;
	long long	unicastPacketsSent;
	long long	unicastPacketsReceived;
	unsigned	discardPacketsSent;
	unsigned	discardPacketsReceived;
	long long	multicastPacketsSent;
	long long	multicastPacketsReceived;
	long long	broadcastPacketsSent;
	long long	broadcastPacketsReceived;
	unsigned	unknownProtoPacketsReceived;
} IPInterfaceStats;
/**@endobj IPInterfaceStats **/

/**@obj IPInterface **/
typedef struct IPInterface {
	unsigned char	enable;
	unsigned char	iPv4Enable;
	unsigned char	iPv6Enable;
	unsigned char	uLAEnable;
	char *	status;
	char *	name;
	unsigned	lastChange;
	Instance *	lowerLayers[10+1];
	unsigned char	reset;
	char *	type;
} IPInterface;
/**@endobj IPInterface **/

/**@obj IP **/
typedef struct GS_IP {
	unsigned char	iPv6Capable;
	unsigned char	iPv6Enable;
	char *	iPv6Status;
	char *	uLAPrefix;
} GS_IP;
/**@endobj IP **/

/**@obj InterfaceStack **/
typedef struct InterfaceStack {
	Instance *	higherLayer;
	Instance *	lowerLayer;
} InterfaceStack;
/**@endobj InterfaceStack **/

/**@obj LANConfigSecurity **/
typedef struct LANConfigSecurity {
	char *	configPassword;
} LANConfigSecurity;
/**@endobj LANConfigSecurity **/

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
	char *	uDPConnectionRequestAddress;
	unsigned char	sTUNEnable;
	char *	sTUNServerAddress;
	unsigned	sTUNServerPort;
	char *	sTUNUsername;
	char *	sTUNPassword;
	int	sTUNMaximumKeepAlivePeriod;
	unsigned	sTUNMinimumKeepAlivePeriod;
	unsigned char	nATDetected;
} ManagementServer;
/**@endobj ManagementServer **/

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

/**@obj WiFiAccessPointAssociatedDevice **/
typedef struct WiFiAccessPointAssociatedDevice {
    char *          mACAddress;
    unsigned char   authenticationState;
    unsigned        lastDataDownlinkRate;
    unsigned        lastDataUplinkRate;
    char *          signalStrength;
    unsigned        retransmissions;
    unsigned char   active;
} WiFiAccessPointAssociatedDevice;
/**@endobj  **/

/**@obj WiFiAccessPointSecurity **/
typedef struct WiFiAccessPointSecurity {
    unsigned char   reset;
    char *          modesSupported;
    char *          modeEnabled;
    char *          preSharedKey;
    char *          keyPassphrase;
} WiFiAccessPointSecurity;
/**@endobj  **/

/**@obj WiFiAccessPointWPS **/
typedef struct WiFiAccessPointWPS {
    unsigned char   enable;
    char *          configMethodsSupported;
    char *          configMethodsEnabled;
} WiFiAccessPointWPS;
/**@endobj  **/

/**@obj WiFiAccessPoint **/
typedef struct WiFiAccessPoint {
    unsigned char   enable;
    char *          status;
    Instance *      sSIDReference;
    unsigned char   sSIDAdvertisementEnabled;
    unsigned        associatedDeviceNumberOfEntries;
    char *          parentAPMAC;
    unsigned char   cAPMODE;
} WiFiAccessPoint;
/**@endobj  **/

/**@obj WiFiRadioStats **/
typedef struct WiFiRadioStats {
    long long   bytesSent;
    long long   bytesReceived;
    long long   packetsSent;
    long long   packetsReceived;
    unsigned    errorsSent;
    unsigned    errorsReceived;
    unsigned    discardPacketsSent;
    unsigned    discardPacketsReceived;
} WiFiRadioStats;
/**@endobj  **/

/**@obj WiFiRadio **/
typedef struct GS_WiFiRadio {
    unsigned char   enable;
    char *          status;
    char *          name;
    unsigned        maxBitRate;
    char *          supportedFrequencyBands;
    char *          operatingFrequencyBand;
    char *          supportedStandards;
    char *          operatingStandards;
    char *          possibleChannels;
    char *          channelsInUse;
    unsigned        channel;
    unsigned char   autoChannelSupported;
    unsigned char   autoChannelEnable;
    char *          autoChannelRefreshPeriod;
    char *          operatingChannelBandwidth;
    char *          extensionChannel;
    char *          guardInterval;
    int             mCS;
    int             transmitPowerSupported;
    int             transmitPower;
    unsigned char   iEEE80211hSupported;
    unsigned char   iEEE80211hEnabled;
    char *          regulatoryDomain;
} GS_WiFiRadio;
/**@endobj  **/

/**@obj WiFiSSIDStats **/
typedef struct WiFiSSIDStats {
    long long   bytesSent;
    long long   bytesReceived;
    long long   packetsSent;
    long long   packetsReceived;
    unsigned    errorsSent;
    unsigned    errorsReceived;
} WiFiSSIDStats;
/**@endobj  **/

/**@obj WiFiSSID **/
typedef struct GS_WiFiSSID {
    unsigned char   enable;
    char *          status;
    char *          name;
    char *          bSSID;
    char *          mACAddress;
    char *          sSID;
} GS_WiFiSSID;
/**@endobj  **/

/**@obj XRJILMeshNode **/
typedef struct XRJILMeshNode {
    char *          parentAPMAC;
    unsigned char   cAPMODE;
} XRJILMeshNode;
/**@endobj  **/

#endif /* _GS__DEVICE_H_ */
