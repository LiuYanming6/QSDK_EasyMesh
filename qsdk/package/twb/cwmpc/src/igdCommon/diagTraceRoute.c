/*----------------------------------------------------------------------*
 * Gatespace
 * Copyright 2009 Gatespace Networks, Inc., All Rights Reserved.
 * Gatespace Networks, Inc. Confidential material.
 *----------------------------------------------------------------------*
 * File Name  : diagTraceRoute.c
 * Description:	Sample CPE implementation of TraceRouteDiagnostics
 *              for TR-106.
 *
 *
 *----------------------------------------------------------------------*
 * $Revision: 1.2 $
 *
 * $Id: diagTraceRoute.c,v 1.2 2012/05/10 18:02:54 dmounday Exp $
 *----------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include	<string.h>
#include 	<time.h>
#include 	<sys/types.h>
#include <errno.h>
#include <ctype.h>

#ifdef DMALLOC
	#include "dmalloc.h"
#endif
#include "../includes/sys.h"
#include "../includes/paramTree.h"
#include "../includes/rpc.h"
#include "../includes/CPEWrapper.h"
#include "../gslib/src/utils.h"
#include "../gslib/src/event.h"
#include "../gslib/src/xmlWriter.h"
#include "../gslib/src/xmlParserSM.h"
#include "../soapRpc/rpcUtils.h"
#include "../soapRpc/xmlTables.h"
#include "../soapRpc/cwmpSession.h"
#include "diagTraceRoute.h"

#define DEBUG
#ifdef DEBUG
#define DBGPRINT(X) fprintf X
#else
#define DBGPRINT(X)
#endif

TraceRtState cpeTRState;
extern void *acsSession;

void cpeStopTraceRt(void *handle)
{
	DiagState s = (DiagState)handle;
	DBGPRINT((stderr, "cpeStopTraceRt %d\n", s));

	if ( cpeTRState.fp != 0 ){
		int fd = fileno(cpeTRState.fp);
		stopTimer(cpeStopTraceRt, NULL);
		stopListener(fd);
		pclose(cpeTRState.fp);
	}
	cpeTRState.fp = 0;
	cpeTRState.state = s;
	cwmpDiagnosticComplete(); /* setup for next Inform */
}

extern CWMPObject Device_Objs[];
extern CWMPObject LANTraceRouteDiagnostics_Objs[];
extern CWMPObject CWMP_RootObject[];

static void doTRRead(void *arg)
{
	char buf[1024];

	if ( fgets(buf, 1024, cpeTRState.fp) == NULL ) {
		/* EOF */
		cpeStopTraceRt((void*)eComplete);
		if ( cpeTRState.hopCnt == cpeTRState.maxTTL )
			cpeTRState.state = eMaxHopExceeded;
	} else {
		DBGPRINT((stderr, "buf=>%s<", buf));
		if ( strncmp(buf, "traceroute",10)==0  ) {
			if ( strstr( buf, "unknown host" ) ) {
				/* bad host name -- stop now */
				cpeStopTraceRt((void*)eHostError);
			}
			return; /* ignore information lines of input */
		} else {
			char *cp = buf;
			char *h, *p;
			int	 id;
			Instance *rtInst;
			if ( (p = strtok( cp, " "))) {
				if ( isdigit(*p)) { /* found route line */
					/* ignore route number and use hopCnt as instance id */
					id = ++cpeTRState.hopCnt;
					if ( (h = strtok(NULL, " "))) {
						/* h points to Hop host name */;
						/*
						 * Find and accumulate the ResponseTime
						 */
						if ( (p = strtok(NULL, " ")) ){
							/* Move past (xxx.xxx.xxx.xxx) ip address. */
							cpeTRState.timeout += atoi(p);
						}
						/* Now create the RouteHops.{i} instances for each host response. */
						/* This is necessary because the data model has the route host list */
						/* retrieved as instances of the RouteHops object.                 */
						/* */
						/* The following cwmpcCreateInstance only works because there are*/
						/* not containing objects with instances. Otherwise; the instance stack within*/
						/* the framework would need to be initialized. */
						CWMPObject *targetObject;
						CWMPObject *parentObject = cpeTRState.parentObj;
						/* create instance of .RouteHops.i. */
						targetObject = cpeTRState.hopObjs;
        				rtInst = cwmpCreateInitInstance( parentObject, parentObject->iList, targetObject, id);
			        	if (rtInst) {
			        		rtInst->cpeData = (void*)GS_STRDUP( h );
			        	}
						/* finish parsing line to get times if needed */
						return;
					}
				} else if ( strstr(buf, "MPLS")){
					return;
				} else
					DBGPRINT((stderr, "Info line(skipped): %s", buf));
			}
		}
		 /* everything else is a failure or noise */
		cpeLog(LOG_ERROR, "TraceRoute Parse error:%s\n", buf);
	}
}

void cpeStartTraceRt( void *handle )
{
	char cmd[256];
	char ttlopt[20];
	int  fd;
	stopCallback(&acsSession, cpeStartTraceRt, NULL); /* stop callback */
	/* start the traceroute diagnostic here if pending Requested is set */
	if ( cpeTRState.state == eRequested && cpeTRState.host ) {
		cpeTRState.responseTime = cpeTRState.hopCnt = 0;
		/* TODO: add code to map data model interface name to native interface name */
		/* block size not implemented in most traceroute utilities */
		/* TODO: add remaining supported parameters here. */
		if (cpeTRState.maxTTL==0)
			cpeTRState.maxTTL = TTLMAX;
		snprintf(ttlopt, sizeof(ttlopt), " -m %d", cpeTRState.maxTTL);

		snprintf(cmd, sizeof(cmd), TRACERTCMD "%s%s 2>&1\n",
		  		ttlopt, cpeTRState.host);
		/* the 2>&1 also writes stderr into the stdout pipe */
		fprintf(stderr, "Start traceroute Diagnostic\n %s", cmd);
		if ((cpeTRState.fp = popen(cmd, "r")) == NULL) {
			cpeLog(LOG_ERR, "Could not start traceroute>%s<", cmd);
			return;
		}
		if ((fd = fileno(cpeTRState.fp)) < 0) {
			cpeLog(LOG_ERR, "Could not fileno popen stream %d(%s)",
				 errno, strerror(errno));
			return;
		}
		setTimer(cpeStopTraceRt, NULL, cpeTRState.timeout? cpeTRState.timeout: 10*1000); /* 10000 msec default*/
		setListener(fd, doTRRead, (void*)fd);
	}
	return;
}

/*
 * Remove RouteHop Instances
 */
void cpeResetTRState( void )
{
	CWMPObject *hopObj = cpeTRState.hopObjs;
	Instance *ip = hopObj->iList;
	while ( ip ){
		/* free host name linked to hop instance */
		GS_FREE( ip->cpeData );
		ip = ip->next;
	}
	cwmpDeleteAllInstances( hopObj );
	cpeTRState.state = eNone;
}
