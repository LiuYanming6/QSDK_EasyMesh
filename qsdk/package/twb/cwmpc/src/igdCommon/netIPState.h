/*
 * netIPState.h
 *
 *  Created on: Feb 11, 2009
 *      Author: dmounday
 */

#ifndef NETIPSTATE_H_
#define NETIPSTATE_H_
typedef enum {
	eDHCPtype,
	eSTATICtype,
	eAutoIPtype
} enumADDRTYPE;

typedef struct NetIPState {
  char   mac[18];
  char   name[32];
  InAddr ip;
  InAddr mask;
  InAddr gw;
  enumADDRTYPE	  addrType;
  int	  macOverride;
  char	  *dnsServers;		/* may be null */
} NetIPState;

NetIPState *cpeGetNetIPInfo(int discovery);
const char *dfltWANIF(void);
const char *getMACAddrStr(void);
#endif /* NETIPSTATE_H_ */
