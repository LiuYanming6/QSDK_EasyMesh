/*----------------------------------------------------------------------*
 * Gatespace
 * Copyright 2007 Gatespace. All Rights Reserved.
 * Gatespace Networks, Inc. confidential material.
 *----------------------------------------------------------------------*
 * File Name  : tr111p2.h
 *
 * Description: Implementation for TR-111 Part 2.
 * $Revision: 1.4 $
 * $Id: tr111p2.c,v 1.4 2011/01/12 18:58:13 dmounday Exp $
 *----------------------------------------------------------------------*/
 #include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/time.h>
#include <syslog.h>
#include <string.h>

#ifdef DMALLOC
	#include "dmalloc.h"
#endif
#include "../gslib/src/utils.h"
#include "../gslib/src/event.h"
#include "../includes/sys.h"
#include "../gslib/src/protocol.h"
#include "../gslib/src/wget.h"
#include "../includes/rpc.h"
#include "../soapRpc/rpcMethods.h"
#include "../soapRpc/notify.h"
#include "tr111p2.h"
#include "stun.h"

#define	CRDELAY		2000			// Delay this long to debounce multiple CR packets.
									// Some ACS send multiple UDP packets when attempting
									// a Connection Request via UDP.

#define DEBUG

#define DEBUG
#ifdef DEBUG
#define DBGPRINT(X) fprintf X
#else
#define DBGPRINT(X)
#endif

void cpeLog( int level, const char *fmt, ...);

char **cwmpConnReqUser;        /* linkage to Connection Request Credentials */
char **cwmpConnReqPW;
StunState stunState;

static int timerOn;

extern CPEState cpeState;
extern EventPolicy eventPolicy[];
extern void cwmpCheckValueChange(void);


/*
 * Check of active notifcations and start
 * the inform sequence if any.
 */

void tr111p2ActiveNotify(void)
{
//	DBGPRINT((stderr, "Wait for 60 seconds delay before cwmpCheckValueChange()\n"));
//	sleep(60);
	DBGPRINT((stderr, "tr111ActiveNotify - value change check\n"));
	cwmpCheckValueChange();
}

static void debounceCReq(void *handle){
	DBGPRINT((stderr, "tr111ConnReq Debounced\n"));
	notifyCallbacks(&cpeState);
}

void tr111p2ConnReq( void )
{
	if ( timerOn )
		stopTimer( debounceCReq, NULL );
	DBGPRINT((stderr, "tr111ConnReq()\n"));
	// restart timer.
	timerOn = setTimer( debounceCReq, NULL, CRDELAY);

}
/*
 * The parameters are pointers to the pointers for the
 * connection request credentials.
 */
void startTR111p2(void){

	cwmpConnReqUser = &cpeState.connReqUser; /* init linkage to credentials */
	cwmpConnReqPW = &cpeState.connReqPW;
	startStun();
	return;
}
