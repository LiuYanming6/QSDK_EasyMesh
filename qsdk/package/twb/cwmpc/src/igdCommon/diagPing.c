/*----------------------------------------------------------------------*
 * Gatespace
 * Copyright 2009 Gatespace Networks, Inc., All Rights Reserved.
 * Gatespace Networks, Inc. Confidential material.
 *----------------------------------------------------------------------*
 * File Name  : diagPing.c
 * Description:	Sample CPE implementation PingDiagnostics for TR-106.
 *
 *
 *----------------------------------------------------------------------*
 * $Revision: 1.1 $
 *
 * $Id: diagPing.c,v 1.1 2012/05/10 17:38:00 dmounday Exp $
 *----------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <unistd.h>
#include <fcntl.h>
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

#include "diagPing.h"

#define DEBUG
#ifdef DEBUG
#define DBGPRINT(X) fprintf X
#else
#define DBGPRINT(X)
#endif

#define PINGCMD  "/bin/ping"


extern void *acsSession;

PingParam cpePingParam;

void cpeStopPing(void *handle)
{
	DiagState s = (DiagState)handle;
	DBGPRINT((stderr, "cpeStopPing %d\n", s));
	stopTimer(cpeStopPing, (void*)eComplete);
	if ( cpePingParam.fp ){
		stopListener(fileno(cpePingParam.fp));
		pclose(cpePingParam.fp);
	}
	cpePingParam.fp = 0;
	cpePingParam.state = s;
	cpePingParam.failures = cpePingParam.reps - cpePingParam.rcvdCnt;
	cwmpDiagnosticComplete(); /* setup for next Inform with Diag Complete event */
}
static void doRead(void *arg)
{
	char buf[1024];
	PingParam *pp = &cpePingParam;
	char	  *p;
	if ( fgets(buf, 1024, cpePingParam.fp) == NULL ) {
		/* EOF */
		cpeStopPing((void*)eComplete);
	} else {

		DBGPRINT((stderr, "buf=>%s", buf));
		if ( strncmp(buf, "PING", 4)==0  ) {
			return; /* ignore first line of input */
		} else if ( strstr(buf, "unknown host") ){
			/* ping: unknown host xxxxxxxxx */
			cpeStopPing((void*)eHostError);
		} else if ( strstr(buf, "packets transmitted") ){
			/* looking for:  n packets transmitted, x received, y errors ...*/
			sscanf(buf,"%*d packets transmitted, %d received", &pp->rcvdCnt);
			pp->failures = pp->reps - pp->rcvdCnt;
		} else if ( strstr(buf, "rtt")){
			/* looking for min/avg/max */
			if ( (p = strchr(buf, '='))) {
				++p;
				pp->minRsp = atoi(p);
				if ( (p = strchr(p, '/'))){
					++p;
					pp->avgRsp = atoi(p);
					if ( (p=strchr(p, '/'))){
						++p;
						pp->maxRsp = atoi(p);
					}
				}
			}
		} else if ( strstr(buf, "bytes ")) {
			/* count received packets */
			pp->rcvdCnt++;
		}

	}
}

void cpeStartPing( void *handle )
{
	char cmd[256];
	char blkstr[10];
	char qstr[10];
	char ifname[64];
	int	 fd;
	PingParam *pp = &cpePingParam;
	stopCallback(&acsSession, cpeStartPing, NULL); /* stop callback */
	/* start the Ping diagnostic here if pending Requested is set */
	if ( pp->state == eRequested ) {
		pp->maxRsp = pp->minRsp = pp->rcvdCnt =
								pp->totRsp = 0;
		if (pp->blockSize)
			snprintf(blkstr, sizeof(blkstr), " -s %d", pp->blockSize);
		else
			blkstr[0]='\0';
		if (pp->dscp )
			snprintf(qstr, sizeof(qstr), " -q %d", pp->dscp);
		else
			qstr[0] = '\0';
		if (pp->ifname)
			snprintf(ifname, sizeof(ifname), " -I %s", pp->ifname);
		else
			ifname[0] = '\0';
		if ( pp->reps == 0)   /* default reps to 4 if not specified */
			pp->reps = 4;
#ifdef TEST_TARGET
		snprintf(cmd, sizeof(cmd), PINGCMD " -c %d %s %s %s %s >&1",
				pp->reps, blkstr, ifname, qstr, pp->host);
#else /* busybox ping cmdline */
		snprintf(cmd, sizeof(cmd), "%s -c %d%s %s 2>&1\n", PINGCMD,
		  		pp->reps, blkstr, pp->host);
#endif
		/* the 2>&1 also writes stderr into the stdout pipe */
		fprintf(stderr, "Start LAN Ping Diagnostic\n %s", cmd);
		if ((pp->fp = popen(cmd, "r")) == NULL) {
			cpeLog(LOG_ERR, "Could not start ping >%s<", cmd);
			pp->state = eErrorInternal;
			cwmpDiagnosticComplete(); /* setup for next Inform with Diag Complete event */
			return;
		}
		if ((fd = fileno(pp->fp)) < 0) {
			cpeLog(LOG_ERR, "Could not fileno popen stream %d(%s)",
				 errno, strerror(errno));
			pp->state = eErrorInternal;
			cwmpDiagnosticComplete(); /* setup for next Inform with Diag Complete event */
			return;
		}
		if ( fcntl( fd, F_SETFL, O_ASYNC)< 0){
			cpeLog(LOG_ERR, "Could not set O_ASYNC flag on popen fd: %s", strerror(errno));
			pp->state = eErrorInternal;
			cwmpDiagnosticComplete(); /* setup for next Inform with Diag Complete event */
			return;
		}
		setTimer(cpeStopPing, (void*)eComplete, pp->timeout? pp->timeout: pp->reps*1000 + 10*1000); /* 10000 msec default*/
		setListener(fd, doRead, (void*)fd);
	}
	return;
}
