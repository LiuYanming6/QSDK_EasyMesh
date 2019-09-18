#ifndef CPEDATA_H_
/*----------------------------------------------------------------------*
 * Gatespace
 * Copyright 2009 Gatespace Networks, Inc., All Rights Reserved.
 * Gatespace Networks, Inc. confidential material.
 *----------------------------------------------------------------------*
 * File Name  :
 * Description:
 *----------------------------------------------------------------------*
 * $Revision: 1.1 $
 *
 * $Id: cpedata.h,v 1.1 2012/05/10 17:38:08 dmounday Exp $
 *----------------------------------------------------------------------*/

#define CPEDATA_H_

/* file name strings
 */
//
#define IFTEMP		"/tmp/iftemp"
#define BRTEMP		"/tmp/brtemp"
#define DHCPLEASES	"/var/udhcpd.leases"
// TARGET_ENV == X86 is a test configuration
#ifdef TEST_TARGET
#define DHCPCONF	"/tmp/udhcpd.conf"
#define WANIFNAME	"eth0"
#define WLANIFNAME	"eth0:2"
#else
#define WANIFNAME	"eth0.11"           /* set to "" if no WAN interface"*/
#define DHCPCONF	"/etc/udhcpd.conf"
#define WLANIFNAME	"wlan"
#endif

#define BRCTL 	"brctl"
#define ROUTE	"route"
#define TRACEROUTE "traceroute"

typedef enum {
	eDHCP,
	eStatic,
	eAutoIP
} AddrType;

typedef enum {
	eUP,
	eNoLink,
	eDisabled
}IFStatus;

typedef enum {
	eADD,
	eDELETE,
	eADDIF,
	eDELIF
}eManageOp;

/*
 * Interface configuration profile
 */
#define LAYER2	 0x00000001			/* supports Layer2 */
#define LAYER3   0x00000002			/* supports Layer 3 (IP addressing)*/
#define LANSIDE  0x00000004			/* I/F is on the LAN side of the GW*/
#define WANSIDE  0x00000008			/* I/F is on the WAN side of the GW*/
#define BRIF     0x00000010			/* I/F is a bridge interface (br0) */
#define WANDFLT  0x00000020			/* Default WAN interface */
#define WIFI     0x00000040			/* I/F is a wireless interface */

typedef struct IFConfig {
	const char	*prefix;		/* eth, ppp, br, ... */
	int		    cfMask;
} IFConfig;

/*
 * Available Interfaces: Not addable by ACS
 * Derived from ifconfig -a output.
 *
 */

/* flag definitions */
/* common flags */
#define	ENTRY_ACTIVE		0x00000001   /* just says the table entry is in use */
#define ENTRY_ENABLE		0x00000002   /* XXXXXEnable  parameter */
#define ENTRY_ENABLED 		ENTRY_ENABLE
#define ENTRY_ADDPENDN		0x00000004
#define ENTRY_OPPENDING		0x00000008	/* a commit operation is pending */
#define ENTRY_ENABLEREQ 	0x00000040 /* set by setXXXEnable to enable */
#define ENTRY_DISABLEREQ 	0x00000020 /* set by setXXXEnable to disable*/

#define	ENTRY_IGNORE		0x80000000

/* */
#define ENTRY_L2		0x40000000
#define ENTRY_L3		0x20000000
#define ENTRY_LAN		0x10000000
#define ENTRY_BRIDGE	0x08000000
#define ENTRY_WANDFLT	0x04000000

#define MAXIFS	32
typedef struct AvailInterface {
	char	ip[18];
	char	mask[18];
	char	ifname[33];
	AddrType addrType;
	IFStatus ifStatus;
	char	linkEncap[18];
	char	rxPackets[18];
	char	txPackets[18];
	char	rxBytes[18];
	char	txBytes[18];
	char	rxErrors[18];
	char	txErrors[18];
	char	rxDropped[18];
	char	txDropped[18];
	char	mac[20];
	int		flag;
	int		macAddCtrlEnabled;
	int		instanceId;
}AvailInterface;

/*
 * Filter interface for Bridges.
 *
 */
typedef struct BFilter {
	int		enabled;
	int		instanceId;
	char	ifname[33];
	int		flag;
	int		brRef;			/*use instance ID of bridge*/
	int		exclusiveOrder;
	int		vlanid;
	/* lots of stuff here */
}BFilter;

#define MAXFILTERS	32
/*
 * output of brctl show
 */
#define MAXBRS	8
#define MAXFILTER_IFS	16
typedef struct BridgeDesc {
	char		bname[65];
	char		ifname[MAXFILTER_IFS][33]; /* only used with brctl show */
	unsigned	vlanid;
	int			instanceId;
	int			flag;
} BridgeDesc;

/*
 */
typedef struct DhcpDesc {
	int		configurable;
	char	flag;
	char	minIP[18];
	char	maxIP[18];
	char	subnetMask[18];
	char	ipRouters[65];
	char	leaseTime[10];
	char	domainName[65];
	char	interface[10];
	char	dnsServers[65];
	char	reservedIP[256];
}DhcpDesc;

/*
 * Data from the leases table.
 */
typedef struct HostLease{
	unsigned char mac[16];
	unsigned	ip;
	unsigned	expires;
} HostLease;

#define WSTATUS_UP	0x1
#define WSTATUS_ERR	0x2
#define WSTATUS_DIS	0x0

#define MAXPSKEY	10
#define MAXWLANCONFIG 1
/* no limit on this structure repetition per WlanState*/
/* The AssociatedDev structure is allocated from the heap */
/* and linked to the Instance via the cpedata component */
typedef struct AssociatedDev {
	char	devMACAddress[8];
	char	devIPAddress[65];
	char	devAuthState;
	char	lastReqUnicastCipher[257];
	char	lastReqMulticastCipher[257];
	char	lastPMKId[257];
}AssociatedDev;

typedef struct PreSharedKey{
	char	psKey[65];
	char	keyPassphrase[64];
	char	assocMAC[8];
}PreSharedKey;

typedef struct WlanState{
	Instance *wlanIp;
	AvailInterface 	*aifp;		/* pointer to availIf of interface */
	int		status;
	char	ssid[33];
	char	beaconType[13];
	char	regDomain[4];
	char	locationDesc[1025];
	int		keyIndex;
	char	rate[6];
	char	channel[4];
	char	keys[4][129];
	PreSharedKey preSharedKey[MAXPSKEY];
}WlanState;


#define MAXMAPS 256
typedef struct PortMapping{
	Instance *cdIp;
	int		 flag;
	int		 leaseDuration;
	char	*remoteHost;
	int		externalPort;
	int		internalPort;
	char	*protocol;
	char	*internalClient;
	char	*description;
} PortMapping;

typedef enum ConnectionStatus{
	eUnconfigured,
	eConnecting,
	eConnectedStatus,
	ePendingDisconnect,
	eDisconnecting,
	eDisconnected
}ConnectionStatus;


/*
 * Only one of the following WANxxxInterfaceConfig should
 * be enabled.
 *
 * Maps to WANDevice.{i}.WANDSLInterfaceConfig.
 */
typedef struct WANDSLInterfaceConfig {
	int			flag;			/* enable/disable link */
	/* linkage to sub-objects for stats, etc. here */
}WANDSLInterfaceConfig;
/*
 * Maps to WANDevice.{i}.WANEnternetInterfaceConfig.
 */
typedef struct WANEthernetInterfaceConfig {
	int		flag;
	char	macAddr[16];
	char	maxBitRate[6];
	char	duplexMode[6];
}WANEthernetInterfaceConfig;

/*
 * Only one of the following WANxxxxLinkConfig should be
 * enabled at once.
 *
 * Maps to WANDevice.{i}.WANConnectionDevice.{i}.WANDSLLinkConfig.
 * Describes a ATM VC.
 */
typedef struct WANDSLLinkConfig {
	int			flag;
	char		linkType[16];
	char		destAddr[257];
	char		atmEncap[6];
	int			fcsPreserved;
	char		vcSearchList[65];
	char		atmQoS[9];
	int			peakCellRate;
	int			maxBurstSize;
	int			sustainCellRate;
	/* error counters here ? */
} WANDSLLinkConfig;

/*
 * Maps to WANDevice.{i}.WANConnectionDevice.{i}.WANEthernetLinkConfig.
 *
 * No settable parameters present.
 */
//typedef struct WANEthernetLinkConfig{
//	int			flag;
//}WANEthernetLinkConfig;

/*
 * Maps to WANDevice.{i}.WANConnectionDevice.{i}.IPConnection.{i}.
 *
 */
#define MAXWANIPCONN 4
typedef struct WANIPConnection {
	int		flag;
	AvailInterface	*ap;
	ConnectionStatus	connStatus;
	char	connType[15];
	char	name[257];
	int		natEnabled;
	AddrType	addrType;
	char	externalIP[IP_ADDRSTRLEN];
	char	subnetMask[IP_ADDRSTRLEN];
	char	defaultGW[IP_ADDRSTRLEN];
	int		dnsEnabled;
	char	dnsServers[64];
	int		maxMTU;
	Instance *connIp;	/* .WANConnectionDevice.{i}. pointer */
} WANIPConnection;

/*
 * Maps to WANDevice.{i}.WANConnectionDevice.{i}.WANPPPConnection.{i}.
 *
 */
#define MAXWANPPPCONN  4
typedef struct WANPPPConnection {
	int		flag;
	AvailInterface	*ap;   /* points to an interface descibed in the availIF table*/
	char	connType[15];
	char	name[257];
	int		autoDiscTime;
	int		idleDiscTime;
	int		natEnabled;
	char	username[65];
	char	password[65];
	int		maxMRU;
	int		dnsEnabled;
	char	dnsServers[64];
	int		dnsOverrideAllowed;
	char	macAddr[16];
	int		macAddrOveride;
	char	pppoeACName[257];
	char	pppoeServiceName[257];
	/* other parameters supported */
}WANPPPConnection;

/*
 * DHCP server event msg for ManageableDevices.
 */
typedef struct DHCPOptions {
	char	ipAddress[INET6_ADDRSTRLEN];			/* as a string */
	char	oui[7];
	char	serialNumber[65];
	char	productClass[65];
}DHCPOptions;

typedef struct ManageableDevice {
	Instance 	*ip;				/* Instance descriptor */
	DHCPOptions deviceData;
} ManageableDevice;

#endif /*CPEDATA_H_*/
