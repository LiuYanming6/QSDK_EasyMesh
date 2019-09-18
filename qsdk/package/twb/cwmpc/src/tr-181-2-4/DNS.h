#ifndef _GS__DNS_H_
#define _GS__DNS_H_

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












#endif /* _GS__DNS_H_ */
