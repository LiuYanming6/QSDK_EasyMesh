/*
 * netIPState.h
 *
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

NetIPState *cpeGetNetIPInfo(void);
const char *dfltWANIF(void);

#endif /* NETIPSTATE_H_ */
