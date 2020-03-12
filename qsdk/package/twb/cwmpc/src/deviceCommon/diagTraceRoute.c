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
 * $Revision: 1.3 $
 *
 * $Id: diagTraceRoute.c,v 1.3 2012/05/10 17:38:00 dmounday Exp $
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
#include "../gslib/auxsrc/dns_lookup.h"

#include "IP.h"

#define DEBUG
#ifdef DEBUG
#define DBGPRINT(X) fprintf X
#else
#define DBGPRINT(X)
#endif

IPDiagnosticsTraceRoute *cpeTR;
extern CPEState cpeState;
extern void *acsSession;
int checkstatus;

void cpeStopTraceRt(void *handle)
{
    static int state = 0;
    static int checkstatus;
    char cmd[32]={0};
    char cmd_result[32]={0};
    
    DiagState s = (DiagState)handle;
    DBGPRINT((stderr, "cpeStopTraceRt %d\n", s));

    checkstatus = check_v4_v6(cpeTR->host);
    if (cpeState.ipAddress.inFamily == AF_INET6  && 1 == checkstatus)
    {
        cmd_popen("pidof traceroute6",cmd);
        sprintf(cmd_result , "kill -9 %s", cmd);
        cmd_popen(cmd_result , cmd);
    }
    else
    {
        cmd_popen("pidof traceroute",cmd);
        sprintf(cmd_result , "kill -9 %s", cmd);
        cmd_popen(cmd_result , cmd);
    }

    if ( cpeTR->fp != 0 ){
        int fd = fileno(cpeTR->fp);
        stopTimer(cpeStopTraceRt, NULL);
        stopListener(fd);
        pclose(cpeTR->fp);
    }
    cpeTR->fp = 0;

    if (s == eHostError)
        state = 1;
    else if ( state == 1)
    {
        state = 0;
        cpeTR->diagnosticState = eHostError;
    }
    else
        cpeTR->diagnosticState = s;

    cwmpDiagnosticComplete(); /* setup for next Inform */
}


static void doTRRead6(void *arg)
{
    if (checkstatus == -1)
    {
        cpeStopTraceRt((void*)eHostError);
        DBGPRINT((stderr, "Invaild domain\n"));
        return;
    }

	char buf[1024];
	char oName[257];
	char *pos =NULL;
	memset(buf,0, sizeof(buf));
	memset(oName,0,sizeof(oName));
	if ( fgets(buf, 1024, cpeTR->fp) == NULL ) {
		/* EOF */
		cpeStopTraceRt((void*)eComplete);
		if ( cpeTR->hopCnt == cpeTR->maxHopCount )
			cpeTR->diagnosticState = eMaxHopExceeded;
	} else {
	    if(NULL != (pos = strchr(buf,'\n')));
	        *pos = '\0';

	    DBGPRINT((stderr, "v6 buf=%s\n", buf));

		if ( strncmp(buf, "traceroute",10)==0  ) {
			if ( strstr( buf, "unknown host" ) || strstr(buf, "bad address") || strstr(buf, "can't connect to remote host") ) {
				/* bad host name -- stop now */
				cpeStopTraceRt((void*)eHostError);
			}
			return; /* ignore information lines of input */
		}
		else if(NULL != strchr(buf,'*')){
            DBGPRINT((stderr, "Timeout...skipped\n"));
		}
		else {
			char *cp = buf;
			char *h, *tp;
			int	 id;
			Instance *rtInst;
			if ( (tp = strtok( cp, " "))) {
				if ( isdigit(*tp)) { /* found route line */
					/* ignore route number and use hopCnt as instance id */
					id = ++cpeTR->hopCnt;
					/* Now create the RouteHops.{i} instances for each host response. */
					/* This is necessary because the data model has the route host list */
					/* retrieved as instances of the RouteHops object.                 */
					snprintf(oName, sizeof(oName), "Device.IP.Diagnostics.TraceRoute.RouteHops.%d.", id);
					rtInst = cwmpInitObjectInstance(oName);
					IPDiagnosticsTraceRouteRouteHops *p = (IPDiagnosticsTraceRouteRouteHops *)rtInst->cpeData;
					if ( (h = strtok(NULL, "("))) {
						if(h[0] == ' ')
							h++;
						if(h[strlen(h)-1] == ' ')
							h[strlen(h)-1] = '\0';
						/* h points to Hop host name */;
						p->host = GS_STRDUP(h);
						p->hostAddress = GS_STRDUP(strtok(NULL, ")"));
						/*
						 * Find and accumulate the ResponseTime
						 */
						if ( (tp = strtok(NULL, " ")) ){
							/* Move past (xxx.xxx.xxx.xxx) ip address. */
							cpeTR->responseTime += atoi(tp);
							p->rTTimes = atoi(tp);
							cpeTR->timeout += atoi(tp);
						}

						return;
					}
				} 
				else if ( strstr(buf, "MPLS")){
					return;
				} 
				else{
					DBGPRINT((stderr, "Info line(skipped): %s", buf));
				}
			}
		}
		 /* everything else is a failure or noise */
		cpeLog(LOG_ERROR, "TraceRoute Parse error:%s\n", buf);
	}
}



static void doTRRead(void *arg)
{
    if (checkstatus == -1)
    {
        cpeStopTraceRt((void*)eHostError);
        DBGPRINT((stderr, "Invaild domain\n"));
        return;
    }
    char buf[1024];
    char oName[257];
    char *pos =NULL;
    memset(buf,0, sizeof(buf));
    memset(oName,0,sizeof(oName));

    if ( fgets(buf, 1024, cpeTR->fp) == NULL ) {
        /* EOF */
        cpeStopTraceRt((void*)eComplete);
        if ( cpeTR->hopCnt == cpeTR->maxHopCount )
            cpeTR->diagnosticState = eMaxHopExceeded;
    } else {

        if(NULL != (pos = strchr(buf,'\n')));
            *pos = '\0';


        DBGPRINT((stderr, "buf=%s\n", buf));
        if ( strncmp(buf, "traceroute",10)==0  ) {
            if ( strstr( buf, "unknown host" ) || strstr(buf, "bad address") || strstr(buf, "can't connect to remote host") ) {
                /* bad host name -- stop now */
                cpeStopTraceRt((void*)eHostError);
            }
            return; /* ignore information lines of input */
        }
        else if(NULL != strchr(buf,'*')){
            DBGPRINT((stderr, "Timeout...skipped\n"));
        }
        else {
            char *cp = buf;
            char *h, *tp;
            int	 id;
            Instance *rtInst;
            if ( (tp = strtok( cp, " "))) {
                if ( isdigit(*tp)) { /* found route line */
                    /* ignore route number and use hopCnt as instance id */
                    id = ++cpeTR->hopCnt;
                    /* Now create the RouteHops.{i} instances for each host response. */
                    /* This is necessary because the data model has the route host list */
                    /* retrieved as instances of the RouteHops object.                 */
                    snprintf(oName, sizeof(oName), "Device.IP.Diagnostics.TraceRoute.RouteHops.%d.", id);
                    rtInst = cwmpInitObjectInstance(oName);
                    IPDiagnosticsTraceRouteRouteHops *p = (IPDiagnosticsTraceRouteRouteHops *)rtInst->cpeData;
                    if ( (h = strtok(NULL, "("))) {
                        if(h[0] == ' ')
                            h++;
                        if(h[strlen(h)-1] == ' ')
                            h[strlen(h)-1] = '\0';
                        /* h points to Hop host name */;
                        p->host = GS_STRDUP(h);
                        p->hostAddress = GS_STRDUP(strtok(NULL, ")"));
                        /*
                         * Find and accumulate the ResponseTime
                         */
                        if ( (tp = strtok(NULL, " ")) ){
                            /* Move past (xxx.xxx.xxx.xxx) ip address. */
                            cpeTR->responseTime += atoi(tp);
                            p->rTTimes = atoi(tp);
                            cpeTR->timeout += atoi(tp);
                        }

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
/*
 * handle must point to IPDiagnosticsTraceRoute structure in Instance.
 */
void cpeStartTraceRt( void *handle )
{
    char cmd[1024];
    char ttlopt[64];
    memset(cmd,0,sizeof(cmd));
    memset (ttlopt,0,sizeof(ttlopt));
    int  fd;
    int	 toutSecs;

    cpeTR = (IPDiagnosticsTraceRoute*)handle;
    stopCallback(&acsSession, cpeStartTraceRt, NULL); /* stop callback */
    /* start the traceroute diagnostic here if pending Requested is set */
    if ( cpeTR->diagnosticState == eRequested && cpeTR->host ) {
        cpeTR->responseTime = cpeTR->hopCnt = 0;
        /* TODO: add code to map data model interface name to native interface name */
        /* block size not implemented in most traceroute utilities */
        /* TODO: add remaining supported parameters here. */
        if (cpeTR->maxHopCount==0)
            cpeTR->maxHopCount = TTLMAX;
        if (cpeTR->numberOfTries==0 )
            cpeTR->numberOfTries = 3;
        if (cpeTR->timeout>0 ){
            // round mesc up to next second
            toutSecs = (cpeTR->timeout+999)/1000;
        } else {
            toutSecs = 5;  // default
            cpeTR->timeout = 5000;
        }
        snprintf(ttlopt, sizeof(ttlopt), "-m %d -q %d -w %d",
                cpeTR->maxHopCount, cpeTR->numberOfTries, toutSecs);
        checkstatus = check_v4_v6(cpeTR->host);

        if ( cpeState.ipAddress.inFamily == AF_INET6  && (checkstatus == 1 || checkstatus == -1))
            snprintf(cmd, sizeof(cmd), "%s %s %s 2>&1\n", "traceroute6", ttlopt, cpeTR->host);
        else
            snprintf(cmd, sizeof(cmd), "%s %s %s 2>&1\n", TRACERTCMD, ttlopt, cpeTR->host);

        /* the 2>&1 also writes stderr into the stdout pipe */
        fprintf(stderr, "Start traceroute Diagnostic\n %s", cmd);


        if ((cpeTR->fp = popen(cmd, "r")) == NULL) {
            cpeLog(LOG_ERR, "Could not start traceroute>%s<", cmd);
            return;
        }
        if ((fd = fileno(cpeTR->fp)) < 0) {
            cpeLog(LOG_ERR, "Could not fileno popen stream %d(%s)",
                 errno, strerror(errno));
            return;
        }
        setTimer(cpeStopTraceRt, (void*)eComplete, cpeTR->timeout? (cpeTR->timeout/1000?cpeTR->timeout + 10000:cpeTR->timeout): 10*1000); /* 10000 msec default*/

        if (cpeState.ipAddress.inFamily == AF_INET6 && (checkstatus ==1 || checkstatus -1))
            setListener(fd, doTRRead6, (void*)fd);
        else
            setListener(fd, doTRRead, (void*)fd);
    }
    return;
}

/*
 * Remove RouteHop Instances
 */
void cpeRemoveRouteHops( void )
{
	CWMPObject *o = cwmpFindObject("Device.IP.Diagnostics.TraceRoute.RouteHops.");
	if ( o ){
		cwmpDeleteChildInstances(o, cwmpGetCurrentInstance());
	}
}
