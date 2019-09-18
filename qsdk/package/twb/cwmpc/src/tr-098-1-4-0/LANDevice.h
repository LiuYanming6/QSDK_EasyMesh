#ifndef _GS__LANDEVICE_H_
#define _GS__LANDEVICE_H_

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





#endif /* _GS__LANDEVICE_H_ */
