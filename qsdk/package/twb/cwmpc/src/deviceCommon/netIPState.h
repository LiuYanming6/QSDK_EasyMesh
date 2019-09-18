/*
 * netIPState.h
 *
 *  Created on: Feb 11, 2009
 *      Author: dmounday
 */

#ifndef NETIPSTATE_H_
#define NETIPSTATE_H_
/*
 * Include headers from target directory.
 * May require add or delete of header files if code generator changes
 * objects generated.
 */
#include "Ethernet.h"
#include "IP.h"
#include "InterfaceStack.h"
#include "WiFi.h"
#if 0
typedef enum {
	eDHCPtype,
	eSTATICtype,
	eAutoIPtype
} enumADDRTYPE;

typedef enum {
	eLOOPBACK,
	eETHER,
	eOTHER
}enumLINKTYPE;

typedef enum {
	eLinkUP,
	eLinkDOWN,
	eLinkUNKNOWN
}enumLINKSTATE;

/*
 * This structure defines the interfaces and IP information discovered on
 * the CPE device.
 */
#define		MAX_IPADDRESSES	10
typedef struct NetIPState {
	Instance			*etherIF;
	EthernetInterface 	*etherIFData;
	Instance			*etherLink;
	EthernetLink		*etherLinkData;
	Instance			*ipIF;
	IPInterface			*ipIFData;
	Instance			*ipIPv4[MAX_IPADDRESSES];
	IPInterfaceIPv4Address	*ipIPv4Data[MAX_IPADDRESSES];

  u_char mac[6];
  char   name[32];
  InAddr ip4;
  InAddr mask;
  InAddr gw;
  InAddr ip6;
  enumADDRTYPE	  addrType;
  enumLINKTYPE	  linkType;
  enumLINKSTATE	  state;
  int	  macOverride;
  char	  *dnsServers;		/* may be null */
} NetIPState;
#endif

EthernetLink *cpeGetNetIPInfo(int discovery);
const char *dfltWANIF(void);
void cpeNetInit(int);
void refreshAssociatedDeviceinstances(void);

#endif /* NETIPSTATE_H_ */
