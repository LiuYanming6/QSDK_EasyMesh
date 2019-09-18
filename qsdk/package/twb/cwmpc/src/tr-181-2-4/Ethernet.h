#ifndef _GS__ETHERNET_H_
#define _GS__ETHERNET_H_

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












#endif /* _GS__ETHERNET_H_ */
