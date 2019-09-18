/*
 * diagTraceRoute.h
 *
 *  Created on: Feb 10, 2009
 *      Author: dmounday
 */

#ifndef DIAGTRACEROUTE_H_
#define DIAGTRACEROUTE_H_

#include "diagPing.h"

typedef struct TraceRtState {
	CWMPObject	*parentObj;		/* object to add RouteHops instances to */
	CWMPObject	*hopObjs;
	DiagState	state;
	DiagState	pendingState;
	char		*host;
	int			timeout;
	int			dataBlkSz;
	int			maxTTL;
	int			DSCP;
	int			responseTime;
	char		*interface;
	int			hopCnt; /* should be same as number of routeHops */
	FILE		*fp;
} TraceRtState;

#define TRACERTCMD  "/bin/traceroute"
#define TTLMAX		30

void cpeResetTRState(void);
void cpeStopTraceRt(void *handle);
void cpeStartTraceRt( void *handle );

#endif /* DIAGTRACEROUTE_H_ */
