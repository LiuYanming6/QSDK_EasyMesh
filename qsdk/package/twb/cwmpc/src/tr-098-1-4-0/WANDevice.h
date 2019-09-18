#ifndef _GS__WANDEVICE_H_
#define _GS__WANDEVICE_H_

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
	time_t	uptime;
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





#endif /* _GS__WANDEVICE_H_ */
