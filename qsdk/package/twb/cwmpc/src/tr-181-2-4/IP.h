#ifndef _GS__IP_H_
#define _GS__IP_H_

/**@obj IPDiagnosticsIPPing **/
typedef enum DiagState {
	eStateNone = 0,
	eHostError,
	eMaxHopExceeded,
	eErrorInternal,
	eErrorOther,
	eComplete =10,
	eRequested
}DiagState;
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
	DiagState	state;
	DiagState	pendingState;
	unsigned	totRsp;
	FILE		*fp;

} IPDiagnosticsIPPing;
/**@endobj IPDiagnosticsIPPing **/

/**@obj IPInterfaceIPv4Address **/
typedef enum SetState {
	eSetNone = 0,
	eSetEnabled,
	eSetDisabled,
}SetState;
typedef struct IPInterfaceIPv4Address {
	unsigned char	enable;
	char *	status;
	char *	iPAddress;
	char *	subnetMask;
	char *	addressingType;
	/* pending values waiting commit */
	char *	pendingIPAddress;
	char *	pendingsubnetMask;
	SetState pendingSet;
} IPInterfaceIPv4Address;
/**@endobj IPInterfaceIPv4Address **/

/**@obj IPInterfaceIPv6Address **/
typedef struct IPInterfaceIPv6Address {
	unsigned char	enable;
	unsigned char	anycast;
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
typedef enum {
	ENABLE_PENDING,
	DISABLE_PENDING
}IPIFChange;
typedef struct IPInterface {
	unsigned char	enable;
	unsigned char	iPv4Enable;
	unsigned char	iPv6Enable;
	unsigned char	uLAEnable;
	unsigned char	autoenable;
	char *	status;
	char *	name;
	unsigned	lastChange;
	Instance *	lowerLayers[10+1];
	unsigned char	reset;
	char *	type;
	IPIFChange	ipIFchange;
} IPInterface;
/**@endobj IPInterface **/

/**@obj IP **/
typedef struct GS_IP {
	unsigned char	iPv4Capable;
	unsigned char	iPv4Enable;
	char *	iPv4Status;
	unsigned char	iPv6Capable;
	unsigned char	iPv6Enable;
	char *	iPv6Status;
	char *	uLAPrefix;
} GS_IP;
/**@endobj IP **/

/**@obj IPDiagnosticsTraceRouteRouteHops **/
typedef struct IPDiagnosticsTraceRouteRouteHops {
	char *	host;
	char *	hostAddress;
	unsigned	errorCode;
	unsigned	rTTimes;
} IPDiagnosticsTraceRouteRouteHops;
/**@endobj IPDiagnosticsTraceRouteRouteHops **/

/**@obj IPDiagnosticsTraceRoute **/
#define TRACERTCMD  "traceroute"
#define TTLMAX		30
typedef struct IPDiagnosticsTraceRoute {
	DiagState	diagnosticState;
	Instance *	interface;
	char *	host;
	unsigned	numberOfTries;
	unsigned	timeout;
	unsigned	dataBlockSize;
	unsigned	dSCP;
	unsigned	maxHopCount;
	unsigned	responseTime;
	DiagState	pendingState;
	int			hopCnt;
	FILE		*fp;
	unsigned	maxTTL;
} IPDiagnosticsTraceRoute;
/**@endobj IPDiagnosticsTraceRoute **/




























#endif /* _GS__IP_H_ */
