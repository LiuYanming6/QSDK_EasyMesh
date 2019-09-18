#ifndef _GS__HOSTS_H_
#define _GS__HOSTS_H_

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












#endif /* _GS__HOSTS_H_ */
