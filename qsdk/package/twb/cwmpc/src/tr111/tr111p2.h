#ifndef TR111P2_H_
#define TR111P2_H_
#include "stun.h"
#define UDPCONNECTIONREQPORT 7547    /* assigned by IANA */

typedef struct StunState {
	int		  pendingEnable;
	int       sfd;
	int       dfd;      /* discovery outgoing port */
	int       minNotifyLimit;
	int       enable;
	char      *serverAddr;
	int       serverPort;
	char      *username;
	char      *password;
	int       maxKeepAlive;
	int       minKeepAlive;
	int       keepAlive;
	int       natDetected;
	int       stunServerIP;
	int       cpeIP;   /* local IP address */
	int       natIP;
	int       natPort;
} StunState;

void startTR111p2(void);
void tr111p2ConnReq(void);
void tr111p2ActiveNotify(void);
#endif /*TR111P2_H_*/
