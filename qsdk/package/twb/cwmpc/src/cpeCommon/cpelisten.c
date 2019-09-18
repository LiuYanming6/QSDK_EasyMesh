/*----------------------------------------------------------------------*
 * Gatespace
 * Copyright 2006 Gatespace Networks, Inc., All Rights Reserved.
 *----------------------------------------------------------------------*
 * File Name  : cpelisten.c
 * Description:	Listen for udp messages from native cpe configuration and
 * 	management functions that request or notify of changes to the
 *  CPE configuration.
 *----------------------------------------------------------------------*
 * $Revision: 1.10 $
 *
 * $Id: cpelisten.c,v 1.10 2012/06/13 11:01:47 dmounday Exp $
 *----------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <sys/time.h>
#include <syslog.h>
#include <string.h>


#ifdef DMALLOC
	#include "dmalloc.h"
#endif
#include "../includes/sys.h"
#include "../gslib/src/utils.h"
#include "../gslib/src/event.h"
#include "../gslib/src/protocol.h"
#include "../includes/paramTree.h"
#include "../includes/CPEWrapper.h"
#include "../soapRpc/acsconnreq.h"
#include "../soapRpc/rpcUtils.h"
#include "../soapRpc/notify.h"
#include "../soapRpc/cwmpSession.h"
#include "targetsys.h"
//#include "cpedata.h"

#include "../includes/cpemsgformat.h"

#define DEBUG
#ifdef DEBUG
#define DBGPRINT(X) fprintf X
#else
#define DBGPRINT(X)
#endif

#ifdef IGD_DATAMODEL
void addNewManageableDevice( DHCPOptions *);
void deleteManageableDevice( DHCPOptions *);
#endif

#define CFMKICKDELAY	2000		/* wait 2 secs before processing event --- may be more pending*/

extern CPEState cpeState;
static int pendingMask;

static void cfmDelayTimeOut(void *handle)
{
	if (pendingMask==0) {
		/* must be a Value Change signal: Only sends an Inform if something changed. */
		cpeRefreshCPEData( &cpeState );
		cwmpCheckValueChange();
		/* else nothing to do */
	} else {
		/* Set the pending actions in the framework and schedule action. */
		cwmpSetPending( pendingMask );
		cwmpStartPending();
		pendingMask = 0;
	}
	return;
}
static void cpeReadEvent(void *handle)
{
	int fd = (int)handle;
	CPEEvent    buf;

	if ( read(fd, &buf, sizeof(buf))>0 ) {
		DBGPRINT((stderr, "cpeReadEvent called: event=%0x\n", buf.eventCode));
		if ( buf.eventCode == CPE_SENDINFORM){
			cwmpAddEvent(eEvtConnectRequest);  /* make the event a connection request */
			pendingMask |= PENDING_GETRPC;
		} else if (buf.eventCode == CPE_PARAMCHANGE ){
			cwmpCheckValueChange();
		} else if (buf.eventCode == CPE_ACSURLCHANGE
				&& strcmp(cpeState.acsURL, buf.ud.acsChangeEvt.url)!=0 ) {
			/* Check that acsURL is different than the current copy. */
			/* ACS change event thru the framework. If the cpeState.acsURL */
			/* is updated from a local source then this strcpy is not necessary*/
			GS_FREE(cpeState.acsURL);
			cpeState.acsURL = GS_STRDUP(buf.ud.acsChangeEvt.url);
			/* set the local static pendingMask to start a session when the */
			/* cfm timeout completes */
			pendingMask |= PENDING_ACSCHANGE;
			/* On the delay timeout the PENDING_ACSCHANGE is set in the framework.*/
			/* The PENDING_ACSCHANGE will set the next Inform event to "Bootstrap" */
			/* and schedule an Inform */
		} else if ( buf.eventCode == CPE_DIAGNOSTICS_COMPLETE ) {
			cwmpDiagnosticComplete();
#ifdef 	CONFIG_RPCCHANGEDUSTATE
		} else if ( buf.eventCode == CPE_SMM_EVENT ){
			cwmpSMMEventReceived();
#endif
#ifdef IGD_DATAMODEL
		} else if ( buf.eventCode == CPE_NEWLEASE ){
			addNewManageableDevice( &buf.ud.dhcpOptions );
		} else if ( buf.eventCode == CPE_EXPIREDLEASE ){
			deleteManageableDevice( &buf.ud.dhcpOptions);
#else
		} else if (buf.eventCode == CPE_GATEWAYID ) {
			;
#endif
#ifdef CONFIG_RPCREQUESTDOWNLOAD
		} else if (buf.eventCode == CPE_REQUESTDOWNLOAD ) {
            cpeState.reqDLFileType = GS_STRDUP(buf.ud.xLoadEvt.fileType);
            cpeState.reqDLArgName = GS_STRDUP(buf.ud.xLoadEvt.argName);
            cpeState.reqDLArgValue = GS_STRDUP(buf.ud.xLoadEvt.argValue);
            cwmpAddEvent(eEvtRequestDownload);
            pendingMask |= PENDING_REQDOWNLOAD; /* pending action */
#endif
#ifdef CONFIG_RPCAUTONOMOUSTRANSFERCOMPLETE
		} else if (buf.eventCode == CPE_AUTOXFERCOMPLETE ) {
			/* Create a RPCRequest entry that can be queued to the
			 * active transfer queue with a completed status.
			 */
			AutoXferEvt *axe = &buf.ud.autoXferEvt;
			RPCRequest *req = (RPCRequest*)GS_MALLOC(sizeof(RPCRequest));
			if (req){
				memset(req, 0, sizeof(RPCRequest));
				req->rpcMethod = eAutonomousTransferComplete;
				DownloadMsg *atc = &req->DLMsg;
				if ( axe->announceURL )
					atc->successURL = GS_STRDUP(axe->announceURL);
				if ( axe->transferURL )
					atc->failureURL = GS_STRDUP(axe->transferURL);
				atc->delaySeconds = axe->isDownload;
				if ( axe->fileType )
					atc->fileType = GS_STRDUP(axe->fileType);
				atc->fileSize = axe->fileSize;
				if ( axe->targetFileName )
					atc->targetFileName = GS_STRDUP( axe->targetFileName);
				/* dlFalutMsg is an index into the message table */
				atc->dlFaultMsg = axe->faultMsg;
				atc->dlStatus = axe->faultCode;     /* this status must be >= 0 */
				atc->dlStartTime = axe->startTime;
				atc->dlEndTime = axe->completeTime;
				/* enqueue on the ACTIVE transfer queue with completed status*/
				cwmpQueueDLRequest(req, ACTIVEQ);
	            cwmpAddEvent(eEvtAutonomousTransferComplete);
	            pendingMask |= PENDING_AUTOXFRCMPT; /* pending action */
			}
#endif
		} else if (buf.eventCode == CPE_STOPCWMPC) {
			/* closing all FDs and stopping all timers should cause the evenLoop to exit.*/
			closeAllFds();
			stopAllTimers();
			return;
        }
		/* other option is PARAM_CHANGE which we always check */
		stopTimer(cfmDelayTimeOut, NULL);		/* restart timer on each cfm msg */
		setTimer(cfmDelayTimeOut,NULL, CFMKICKDELAY );
	}
	return;
}

void startCPEEventListener(void)
{   int fd;
#ifdef USE_UNIX_DOMAIN_SOCKET
	if ( (fd = get_domain_socket(CPELISTEN_NAME ))<0)
		return;
#else
	InAddr anyAddr= {AF_INET, {{htonl(INADDR_ANY)}}};

	if ((fd = udp_listen(&anyAddr, CPELISTENPORT, NULL, FALSE)) < 0) {
		cpeLog(LOG_ERR, "could not initiate UDP server socket (port=%d)", CPELISTENPORT);
		return;
	}
#endif /*USE_UNIX_DOMAIN_SOCKET */

	setListener( fd, cpeReadEvent, (void *)fd );
	return;
}

