/*----------------------------------------------------------------------*
 * Gatespace
 * Copyright 2012 Gatespace Networks, Inc., All Rights Reserved.
 * Gatespace Networks, Inc. Confidential material.
 *----------------------------------------------------------------------*
 * File Name  : diagPing.c
 * Description:	Sample CPE implementation PingDiagnostics for Device.
 *              data models.
 *
 *
 *----------------------------------------------------------------------*
 * $Revision: 1.2 $
 *
 * $Id: diagPing.c,v 1.2 2012/05/10 17:38:00 dmounday Exp $
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

#include "IP.h"      /* should be in the target data model dir */
#include "Ethernet.h"/*   "           " */
#include "../gslib/auxsrc/dns_lookup.h"


#define DEBUG
#ifdef DEBUG
#define DBGPRINT(X) fprintf X
#else
#define DBGPRINT(X)
#endif

#define PINGCMD  "/bin/ping"


extern void *acsSession;
IPDiagnosticsIPPing *cpePingParam;
extern CPEState cpeState;
int checkstatus;

void cpeStopPing(void *handle)
{
	DiagState s = (DiagState)handle;
	DBGPRINT((stderr, "cpeStopPing %d\n", s));
	stopTimer(cpeStopPing, (void*)eComplete);
	if ( cpePingParam->fp ){
		stopListener(fileno(cpePingParam->fp));
		pclose(cpePingParam->fp);
	}
	cpePingParam->fp = 0;
	cpePingParam->state = s;
	cpePingParam->failureCount = cpePingParam->numberOfRepetitions - cpePingParam->successCount;
	cwmpDiagnosticComplete(); /* setup for next Inform with Diag Complete event */
}
static void doRead(void *arg)
{
    if (checkstatus == -1)
    {
        cpeStopPing((void*)eHostError);
        DBGPRINT((stderr, "Invaild domain\n"));
        return;
    }

	char buf[1024];
	IPDiagnosticsIPPing *pp = cpePingParam;
	char	  *p;
	if ( fgets(buf, 1024, cpePingParam->fp) == NULL ) {
		/* EOF */
		cpeStopPing((void*)eComplete);
	} else {

		DBGPRINT((stderr, "buf=>%s", buf));
		if ( strncmp(buf, "PING", 4)==0  ) {
			return; /* ignore first line of input */
		} else if ( strstr(buf, "unknown host") || strstr(buf, "bad address") || strstr(buf, "100% packet loss") ){
			/* ping: unknown host xxxxxxxxx */
			cpeStopPing((void*)eHostError);
		} else if ( strstr(buf, "packets transmitted") ){
			/* looking for:  n packets transmitted, x received, y errors ...*/
			sscanf(buf,"%*d packets transmitted, %d received", &pp->successCount);
			pp->failureCount = pp->numberOfRepetitions - pp->successCount;
		} else if ( strstr(buf, "rtt") || strstr(buf, "round-trip")){
			/* looking for min/avg/max */
			if ( (p = strchr(buf, '='))) {
				++p;
				pp->minimumResponseTime = atoi(p);
				if ( (p = strchr(p, '/'))){
					++p;
					pp->averageResponseTime = atoi(p);
					if ( (p=strchr(p, '/'))){
						++p;
						pp->maximumResponseTime = atoi(p);
					}
				}
			}
		} else if ( strstr(buf, "bytes ")) {
			/* count received packets */
			pp->successCount++;
		}

	}
}

void cpeStartPing( void *handle )
{
	char cmd[256];
	char blkstr[10];
	char qstr[10];
	char interface[64];
	int	 fd;

	IPDiagnosticsIPPing *pp = (IPDiagnosticsIPPing *)handle;
	cpePingParam = pp;
	stopCallback(&acsSession, cpeStartPing, NULL); /* stop callback */
	/* start the Ping diagnostic here if pending Requested is set */
	if ( pp->state == eRequested ) {
		pp->maximumResponseTime = pp->minimumResponseTime = pp->successCount =
								pp->totRsp = 0;
		if (pp->dataBlockSize)
			snprintf(blkstr, sizeof(blkstr), " -s %d", pp->dataBlockSize);
		else
			blkstr[0]='\0';
		if (pp->dSCP )
			snprintf(qstr, sizeof(qstr), " -q %d", pp->dSCP);
		else
			qstr[0] = '\0';
		interface[0] = '\0';
		if (pp->interface) {
			// get local interface name.
			EthernetInterface *eifp = (EthernetInterface*)(pp->interface->cpeData);
			if ( eifp )
				snprintf(interface, sizeof(interface), " -I %s", eifp->name);
		}
		if ( pp->numberOfRepetitions == 0)   /* default numberOfRepetitions to 4 if not specified */
			pp->numberOfRepetitions = 4;
#ifdef TEST_TARGET
		snprintf(cmd, sizeof(cmd), PINGCMD " -c %d %s %s %s %s >&1",
				pp->numberOfRepetitions, blkstr, interface, qstr, pp->host);
#else /* busybox ping cmdline */
        checkstatus = check_v4_v6(pp->host);

        if(strlen(pp->protocolversion) ==0)
            COPYSTR(pp->protocolversion, "IPv4");

        if(!strncmp(pp->protocolversion, "IPv6", 4))
        {
            snprintf(cmd, sizeof(cmd), "%s -6 -w %d -c %d%s %s 2>&1 &", PINGCMD,
            pp->timeout? pp->timeout/1000: pp->numberOfRepetitions + 10, pp->numberOfRepetitions, blkstr, pp->host);
        }
        else if (!strncmp(pp->protocolversion, "IPv4", 4))
        {
            snprintf(cmd, sizeof(cmd), "%s -4 -w %d -c %d%s %s 2>&1 &", PINGCMD,
            pp->timeout? pp->timeout/1000: pp->numberOfRepetitions + 10, pp->numberOfRepetitions, blkstr, pp->host);
        }
        else
        {
            pp->state = eErrorInternal;
            cwmpDiagnosticComplete(); /* setup for next Inform with Diag Complete event */
            return;
        }
#if 0
        if ( cpeState.ipAddress.inFamily == AF_INET6 && (1 == checkstatus || -1 == checkstatus) )
        {
            snprintf(cmd, sizeof(cmd), "%s -6 -w %d -c %d%s %s 2>&1 &", PINGCMD,
            pp->timeout? pp->timeout/1000: pp->numberOfRepetitions + 10, pp->numberOfRepetitions, blkstr, pp->host);
        }
        else
        {
            snprintf(cmd, sizeof(cmd), "%s -4 -w %d -c %d%s %s 2>&1 &", PINGCMD,
            pp->timeout? pp->timeout/1000: pp->numberOfRepetitions + 10, pp->numberOfRepetitions, blkstr, pp->host);
        }
#endif
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
		setTimer(cpeStopPing, (void*)eComplete, pp->timeout? (pp->timeout/1000?pp->timeout + 1000:pp->timeout): pp->numberOfRepetitions*1000 + 10*1000); /* 10000 msec default*/
		setListener(fd, doRead, (void*)fd);
	}
	return;
}
