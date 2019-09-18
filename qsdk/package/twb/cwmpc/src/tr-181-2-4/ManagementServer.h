#ifndef _GS__MANAGEMENTSERVER_H_
#define _GS__MANAGEMENTSERVER_H_

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




























#endif /* _GS__MANAGEMENTSERVER_H_ */
