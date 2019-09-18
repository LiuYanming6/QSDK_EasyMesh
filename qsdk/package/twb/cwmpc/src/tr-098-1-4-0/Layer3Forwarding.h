#ifndef _GS__LAYER3FORWARDING_H_
#define _GS__LAYER3FORWARDING_H_

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





#endif /* _GS__LAYER3FORWARDING_H_ */
