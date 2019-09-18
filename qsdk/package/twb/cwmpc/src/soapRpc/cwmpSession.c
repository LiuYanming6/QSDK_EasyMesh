
/*----------------------------------------------------------------------*
 * Gatespace
 * Copyright 2006-2011 Gatespace Networks, Inc., All Rights Reserved.
 * Gatespace Networks, Inc. confidential material.
 *----------------------------------------------------------------------*
 * File Name  : cwmpsession.c
 * Description:	CWMP http protocol implementation
 *----------------------------------------------------------------------*
 * $Revision: 1.53 $
 *
 * $Id: cwmpSession.c,v 1.53 2012/06/13 11:21:25 dmounday Exp $
 *----------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include <sys/select.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <syslog.h>

#include "../includes/CPEWrapper.h"
#include "../gslib/auxsrc/dns_lookup.h"
#include "targetsys.h"
#include "rpcUtils.h"
#include "cwmpSession.h"
#include "notify.h"


#ifdef DMALLOC
#include "dmalloc.h"
	unsigned long dmallocMark;
#endif
#ifdef DEBUGLOG
#define DBGPRINT(X) fprintf X
#else
#define DBGPRINT(X)
#endif

void setNameSpaceVersion(int);

/*
 * reflects state of session with ACS
 */
typedef enum {
	eDiscardCookies,
	eSaveCookies
}eCOOKIESTATE;


/*
 * forwards
 */

static int postToAcs(char *buf );
static void acsConnected(void *handle);
static void sessDisconnect(ACSSession *s, eSessionStatus acsStatus);
static int completeTransfer(eRPCMethods);
static RPCRequest *anyCompleteXfer(eRPCMethods);
static void setDNSCaching(int);
/*
 * table for handling events
 * NOTICE: eventPolicy event order must match eEventCode enum order.
 */
EventPolicy eventPolicy[eEvtArraySz] = {
    {eEvtBootstrap,         "0 BOOTSTRAP", -1, 0, eInformResponse},
    {eEvtBoot,              "1 BOOT"     , -1, 0, eInformResponse},
    {eEvtPeriodic,          "2 PERIODIC" , -1, 0, eInformResponse},
    {eEvtScheduled,         "3 SCHEDULED", -1, 0, eInformResponse},
    {eEvtValueChange,       "4 VALUE CHANGE", -1, 0, eInformResponse},
    {eEvtKicked,            "5 KICKED",     2, 0, eKickedResponse},
    {eEvtConnectRequest,    "6 CONNECTION REQUEST", 0, 0, eInformResponse},
    {eEvtTransferComplete,  "7 TRANSFER COMPLETE", -1, 0, eTransferCompleteResponse},
    {eEvtDiagComplete,      "8 DIAGNOSTICS COMPLETE", -1, 0, eInformResponse},
    {eEvtRequestDownload,   "9 REQUEST DOWNLOAD", 2, 0, eRequestDownloadResponse},
    {eEvtAutonomousTransferComplete,  "10 AUTONOMOUS TRANSFER COMPLETE", -1, 0, eAutonomousTransferCompleteResponse},
    {eEvtDUStateChangeComplete, "11 DU STATE CHANGE COMPLETE", -1, 0, eDUStateChangeCompleteResponse},
    {eEvtAutonomousDUStateChangeComplete, "12 AUTONOMOUS DU STATE CHANGE COMPLETE", -1, 0, eAutonomousDUStateChangeCompleteResponse},
    {eEvtMReboot,           "M Reboot", -1, 0, eInformResponse},
    {eEvtMScheduleInform,   "M ScheduleInform", -1, 0, eInformResponse},
    {eEvtMDownload,         "M Download", -1, 0, eInformResponse},
    {eEvtMScheduleDownload, "M ScheduleDownload", -1, 0, eInformResponse},
    {eEvtMUpload,           "M Upload", -1, 0, eInformResponse},
    {eEvtMChangeDUState,	"M ChangeDUState", -1, 0, eDUStateChangeCompleteResponse},
    {eEvtMVendor,           "M XXXXX", 0, 0},
    {eEvtINVALID}
};

CPEState cpeState;
ACSSession acsSession;

static int cwmpChangedCfg;			/* count of add/delete/set rpc's in session */
									/* also cwmpDiagnosticComplete() calls */

static int notifyChanged;	/* set if notification Attributes changed */

/*
 * Pending operation flags
 * These are operations that are pending until the disconnection
 * of the current session. For example: Reboot.
 */
static int pendingAct;

void cwmpSetPending( int actionMask ) {
    pendingAct |= actionMask;
    /* set eventMask for those PENDING actions that cause
     a reboot. The eventMask is saved across reboots. */
    if (actionMask&PENDING_REBOOT)
        cpeState.eventMask |= EVT_REBOOT;
    if (actionMask&PENDING_XFERCOMPL)
        cpeState.eventMask |= EVT_XFERCOMPL;
    if (actionMask&PENDING_AUTOXFRCMPT)
    	cpeState.eventMask |= EVT_AUTOXFRCMPL;
    if (actionMask&PENDING_ACSCHANGE) {
    	cpeState.eventMask = 0;           /* clear all pending events */
    	pendingAct = PENDING_ACSCHANGE;   /* and actions when switch to new ACS */
    	setDNSCaching(TRUE);
    	cwmpResetActiveNotification();    /* clear any pending value change events */
    }
}

void cwmpClearPending( int actionMask) {
    pendingAct = pendingAct&(~actionMask);
}
int cwmpIsPending( int actionMask ) {
    return pendingAct&actionMask;
}
static void disablePendingRPC( eEventCode e )
{
	switch (e) {
		case eEvtKicked:
			cwmpClearPending( PENDING_SENDKICKED );
			break;
		case eEvtRequestDownload:
			cwmpClearPending( PENDING_REQDOWNLOAD );
			break;
		case eEvtAutonomousTransferComplete:
			cwmpClearPending( PENDING_AUTOXFRCMPT );
			break;
		default:
			break;
	}
}

/*
 * Dequeue RPCRequest *item from the request queue pointed
 * to by **qHdr. If *item is NULL dequeue the first item in
 * the queue.
 * Return dequeued RPCRequest * or NULL if not found.
 */
RPCRequest *cwmpDQItem(RPCRequest *item, RPCRequest **qHdr) {
	RPCRequest *r = *qHdr;
	RPCRequest **last = qHdr;
	while (r != NULL) {
		if (r == item || item==NULL) {
			*last = r->next;
			return r;
		}
		last = &r->next;
		r = r->next;
	}
	return r;
}
/*
 * Remove the first completed activeItem from the Active transfer
 * queue. Items are not removed from the active transfer queue
 * until the ACS has send the TransferCompleteResponse to the associated
 * TransferComplete.
 * return 0: no more completed active items.
 *        1: more completed active items on queue.
 */
static int completeTransfer(eRPCMethods m) {
	RPCRequest *r = anyCompleteXfer(m);
	if (r) {
		if (r->rpcMethod == eDownload || r->rpcMethod == eScheduleDownload)
			cpeState.eventMask = cpeState.eventMask & (~EVT_DOWNLOAD);
		else if (r->rpcMethod == eUpload)
			cpeState.eventMask = cpeState.eventMask & (~EVT_UPLOAD);
		else if (r->rpcMethod == eAutonomousTransferComplete)
			cpeState.eventMask = cpeState.eventMask & (~EVT_AUTOXFRCMPL);
		cwmpDQItem(r, &cpeState.dlQActive);
		cwmpFreeRPCRequest(r);
		return anyCompleteXfer(m) != NULL;
	}
	return 0;
}
/*
 * Return the first RPCRequest that has completed.
 * If Requested method is eAutonomousTransferComplete return queued
 * request that is the same.
 * Otherwise only return eScheduledDownload, eDownload or eUpload.
 */
static RPCRequest *anyCompleteXfer(eRPCMethods m) {
	RPCRequest *r = cpeState.dlQActive;
	while (r != NULL) {
		if (r->ud.downloadReq.dlStatus != 2) {
			/* request has completed */
			if (m == eAutonomousTransferComplete) {
				if (r->rpcMethod == eAutonomousTransferComplete)
					break;
			} else if (r->rpcMethod == eUpload || r->rpcMethod == eDownload
					|| r->rpcMethod == eScheduleDownload)
				break;
			else if (r->rpcMethod == eUnspecified)
				break;
		}
		r = r->next;
	}
	return r;
}


/*
 * Called following a upload/download performed by
 * an entity other than the CWMP. Walks thru the cpeState.dlQActive
 * and initiates an Inform session if any requests in the
 * queue are complete.
 */
void cwmpSendXfersComplete(void) {
	if (anyCompleteXfer(eUpload/*or eDownload*/)) {
		cwmpSetPending(PENDING_XFERCOMPL);
		cwmpAddEvent(eEvtTransferComplete);
		cwmpStartPending();
	}
}
/*
 * Get acsHostname from ACS URL and set DNS caching for it.
 */
static void setDNSCaching(int changed){
	char	hostName[HOSTNAME_SZ];
	char	uri[URI_SZ];
	char	proto[PROTOCOL_SZ];
	int		port;
	/* if changed then reset acs hostname and acs ip address */
	if ( changed && cpeState.acsHostName ){
		/* remove old ACS host name if present*/
		dns_remove_cache_host( cpeState.acsHostName );
		GS_FREE(cpeState.acsHostName);
		cpeState.acsHostName = NULL;
		clearInIPAddr(&cpeState.acsIPAddress);
	}
	if ( www_ParseUrl(cpeState.acsURL, proto, hostName, &port, uri)>=0 ){
		COPYSTR(cpeState.acsHostName, hostName);
		dns_set_cache_host_ip(hostName, &cpeState.acsIPAddress);
	}
}
/*
 * setSessionURL
 * Replace the current session URL.
 */
static void setSessionURL(ACSSession *s, const char *newURL)
{
	if ( !EMPTYSTR(newURL) )
		strncpy(s->sessACSURL, newURL, sizeof(s->sessACSURL));
	else {
		cpeLog(LOG_ERR, "ACS URL not defined for session");
		if(cpeState.acsURL == NULL) //Segmentation fault
			return;
		strncpy(s->sessACSURL, cpeState.acsURL, sizeof(s->sessACSURL));   /* try to recover from orig */
	}
}

static int sessionID = 0;
/*
 * newSession()
 * Create a new session ID and
 * set the sessionACSURL to the value from the
 * Management object.
 * The sessionACSURL may be reset by a Redirect status
 * during the session
 */
static void newSession(ACSSession *s) {

    if (sessionID==0)
        sessionID = time(NULL);
    snprintf(s->sessID, sizeof(s->sessID), "%d", sessionID++);
    setSessionURL(s, cpeState.acsURL);
    s->eNullState = eNoNullSent;
    s->eHttpState = eStart;
    s->redirCount = 0;
}
/*
 * Return true if session with ACS active.
 */
int cwmpIsACSSessionActive(void){
	return acsSession.wio!=NULL;
}
/*
 * Block/unblock ACS session from starting.
 * block: 0 to unblock.
 *        -1 to block.
 * Return 0: session block or unblocked as requested.
 *       -1: Out-of-sync call to this function.
 */
int cwmpSetSessionBlock( int block ){
	if ( acsSession.wio == NULL || acsSession.wio == (void*)-1 ){
		acsSession.wio = (void*) block;
		return 0;
	}
	return -1;
}
/* called from the cwmpc process to indicate
 * that a diagnostic has completed.
 */
void cwmpDiagnosticComplete(void)
{
	cwmpChangedCfg++;   /* increment this to force configuration update */
						/* since the value of the State parameter changed */
	cwmpAddEvent(eEvtDiagComplete);
	if ( acsSession.wio==NULL)
		notifyCallbacks(eventPolicy);
	/* else waits for end of current session */
}
/*
 * Called from cwmpc process to check if a
 * value change occurred on a parameter with notification
 * set.
 */
void cwmpCheckValueChange(void){
	if (cwmpAnyActiveNotifications()){
		DBGPRINT((stderr, "Active Notification needed\n"));
		cwmpAddEvent(eEvtValueChange);
		if ( acsSession.wio==NULL)
			notifyCallbacks(eventPolicy);
	}
}
/*
 * Try pending Inform
 *
 */
void cwmpStartPending(void){
	if (acsSession.wio == NULL && cwmpIsPending(PENDING_ANY) )
		notifyCallbacks(eventPolicy);
}
/*
 * Add an event to the pending list if there is currently a connection
 * to the ACS. Otherwise; set the event to Active.
 */
void cwmpAddEvent( eEventCode event ) {
    if (acsSession.wio==NULL) /* set active if no session in progress*/
        eventPolicy[event].evState = eActive;
    else					  /* set Pending if session in progress */
        eventPolicy[event].evPending = ePending;
    eventPolicy[event].currRetries = 0;	 /* reset retry count */
}

void cwmpClearEvent( eEventCode event) {
    eventPolicy[event].evState = eOff;
}

static void resetAllEvents(void) {
    eEventCode e = eEvtBootstrap;
    while ( e<eEvtINVALID ) {
        eventPolicy[e].evState = eOff;
        eventPolicy[e].evPending = eOff;
        eventPolicy[e].currRetries = 0;
        ++e;
    }
}

/*
 * clears all active events if the response-method is
 * the same as method. Any pending events are changed
 * to active.
 */
static void clearAllEvents( eRPCMethods method ) {
    eEventCode e = eEvtBootstrap;
    while ( e<eEvtINVALID ) {
        if (eventPolicy[e].resetMethod == method) {
            eventPolicy[e].evState = eOff;
            eventPolicy[e].currRetries = 0;
        }
        ++e;
    }
}
/**
 * Change any pending events to active events.
 * Return the number of new active events.
 */
static int activatePendingEvents(void){
	int	i =0;
    eEventCode e = eEvtBootstrap;
    while ( e<eEvtINVALID ) {
    	if (eventPolicy[e].evPending == ePending) {
			eventPolicy[e].evState = eActive;
			eventPolicy[e].evPending = eOff;
			eventPolicy[e].currRetries = 0;
			++i;
        }
        ++e;
    }
    //fprintf(stderr, "activatePendingEvents() =%d\n", i);
    return i;
}
eEState cwmpGetEventState( eEventCode event) {
    return eventPolicy[event].evState;
}
const char *cwmpGetEventName( eEventCode event) {
    return eventPolicy[event].evtString;
}
int cwmpGetEventCount() {
    int i=0;
    eEventCode e = eEvtBootstrap;
    while ( e<eEvtINVALID ) {
        if ( eventPolicy[e].evState == eActive )
            ++i;
        ++e;
    }
    return i;
}

static int isEventActive( eEventCode event) {
	return eventPolicy[event].evState == eActive;
}
/*
 * Returns number of events still Active after applying
 * the retry policy.
 */
static int checkEventRetryPolicy(void) {
    int i=0;
    eEventCode e = eEvtBootstrap;
    while ( e<eEvtINVALID ) {
        if (eventPolicy[e].evState==eActive) {
            if ( eventPolicy[e].retryPolicy ==-1 )
                ++i;  /* never timeout any with -1 Policy count */
            else if (eventPolicy[e].retryPolicy >eventPolicy[e].currRetries ){
                ++i;
                ++eventPolicy[e].currRetries;
            } else { /* event delivery timed out */
                eventPolicy[e].evState = eOff;
                eventPolicy[e].currRetries = 0;
                /* also disable Pending RPC */
                disablePendingRPC( e );
            }
        }
        ++e;
    }
    return i;

}

static int startInformRetryTimer(ACSSession *);

static void retryACSInform(void *handle) {
    char    *infBuf;
    const char	*p;
    char	dfltIP[IP_ADDRSTRLEN];

    DBGLOG((DBG_ACSCONNECT, "retryACSInform\n"));
    #ifdef DMALLOC
    	dmallocMark = dmalloc_mark();
    #endif
    if (cpeState.acsContactState!=ACS_SESSION_ENABLED){
    	return; /* ignore any retries if session disabled */
    }
#ifdef POLL_ACSACCESS_STATUS
    if ( (cpeGetACSAccessStatus()== eNOACCESS) || cwmpIsACSSessionActive() ){
    	setTimer(retryACSInform, NULL, POLL_ACSACCESS_INTERVAL);
    	DBGLOG((DBG_ACSCONNECT, "retryACSInform: no ACS access or session active\n"));
    	return;
    }
#else
    /* return if a session is active. */
    if ( cwmpIsACSSessionActive() )
    	return;
#endif
    cpeLockConfiguration();
    activatePendingEvents();
    if ( cwmpIsPending(PENDING_ACSCHANGE)) {
    	/* if the ACS URL has changed then force a BOOTSTRAP event and clear all
    	 * other events.
    	 */
    	resetAllEvents();
    	cwmpAddEvent( eEvtBootstrap );
    } else {
	    /* clear pending of the possible events not set by RPC, i.e. Conn Req, Diag complete */
	    /* The new events have been added to the Inform event struct on retry */
	    /* Pending RPC flags should not be cleared */
	    cwmpClearPending( PENDING_CONNREQ|PENDING_DIAGINFORM );
    }
    cpeRefreshCPEData(&cpeState);
    cpeRefreshInstances();
    newSession(&acsSession);
    if ( (p = writeInIPAddr(&cpeState.ipAddress)))
    	strcpy(dfltIP, p);
    else
    	dfltIP[0]='\0';

    if ( (infBuf=cwmpInformRPC( acsSession.sessID, acsSession.retryCount, dfltIP))) {
        if ( postToAcs(infBuf)== -1){
            startInformRetryTimer(&acsSession); /* only if initial post call fails */
            cpeUnlockConfiguration(cwmpChangedCfg);
        }
        /* other retries within post code  */
    } else {
    	cpeLog(LOG_ERR, "Failed to start ACS session: No memory available.");
    	startInformRetryTimer(&acsSession); /* restart retry timer */
        cpeUnlockConfiguration(cwmpChangedCfg);
    }
}
/*
 * called with retryCount incremented
 * Returns -1: Timer to retryACSInform in retry time has been started.
 *         0: No retry necessary and no new pending events.
 *         1: Timer to retryACSInform for immediate Inform has been started.
 */
static int startInformRetryTimer(ACSSession *s) {
    int waitTime;

    cwmpClearPending(PENDING_GETRPC); /* Never retry an Inform for this pending RPC */
    if (checkEventRetryPolicy()>0) {
    	/* if no new active event then start retry timer */
    	if ( activatePendingEvents() == 0 ){
			if ( s->retryCount++ ) {
				if (s->retryCount<=10){
					s->delay = s->expVal * cpeState.cwmpRetryMin;
					// re-scale the exponentiation result to avoid overflow.
					// The order is important to retain some precision.
					s->expVal = (s->expVal * cpeState.cwmpRetryMultiplier)/1000;
				}
			} else {
				s->delay = cpeState.cwmpRetryMin*1000;
	    		s->expVal = cpeState.cwmpRetryMultiplier;
			}
			waitTime = s->delay+( rand() % (s->expVal*cpeState.cwmpRetryMin-s->delay));
			setTimer(retryACSInform, NULL, waitTime );
			cpeDbgLog(DBG_ACSCONNECT, "retry = %d waitTime = %d msec\n", s->retryCount, waitTime);
#if 0
			if (s->retryCount == 5 )
			{
				DBGPRINT((stderr,"TwinA : retry = 5 , connection failed , restart cwmpc"));
				exit(-1);
			}
#endif
			cpeState.basePTime = time(NULL)+waitTime;   /* reset periodic inform reference time on retries */
			return -1;   /* return that retry timer is running */
    	} else {
    		/* new active event */
			setTimer(retryACSInform, NULL, 200 /*millisec*/ );
			cpeDbgLog(DBG_ACSCONNECT, "New active event overrides retry time: Inform scheduled");
    		return 1;
    	}
    }
    s->delay = s->retryCount = 0;   /* no timer running */
    if ( activatePendingEvents()>0 ){
		/* new active event */
		setTimer(retryACSInform, NULL, 200 /*millisec*/ );
		cpeDbgLog(DBG_ACSCONNECT, "New active event Inform scheduled");
		return 1;
    }
    return 0;
}

static void startPeriodicInform(void *handle) {
    cwmpAddEvent(eEvtPeriodic);
    retryACSInform(NULL);
    
    time_t  now, next;
    if (cpeState.informEnabled && cpeState.informInterval) {
        now = time(NULL);
        if (cpeState.informTime && cpeState.informTime!=-1 ) 
        {
            time_t diff;
            diff = now-cpeState.informTime;
            next = abs(diff%cpeState.informInterval);
            if ( diff>0)
                next = cpeState.informInterval - next;
        } else 
        {
            next = cpeState.informInterval-(abs(now-cpeState.basePTime)%cpeState.informInterval);
        }
        cpeDbgLog(DBG_ACSCONNECT, "next periodic inform = %d\n", (int) next);
        setTimer(startPeriodicInform, NULL, next*1000);
    }
    return;
}

/*
 * Target of a notifyCallbacks() that is used by
 * the CPE msg/event handler or the client API msg handler
 * to start an Inform for a CPE notification.
 * Such as Value change, Diagnostic complete, Get RPC.
 * The PENDING flags and Inform events are set by the CPE msg handler (See main.c)
 * or the code that calls the signalCallbacks().
 */
static void startCPEEvtInform(void *handle) {
    retryACSInform(NULL);
    return;
}

/*
 * Target of a notifyCallback() used by the connection request listener
 * to start an Connection Request Inform
 */
static void startConnReqInform(void *handle) {
    cwmpAddEvent(eEvtConnectRequest);
    if (!cwmpIsACSSessionActive() ) {
        retryACSInform(NULL);   /* start now */
    } else
        cwmpSetPending(PENDING_CONNREQ);  /* wait for disconnect */
    return;
}

static void scheduleNextInform(void) {
    time_t  now, next;
    if (cpeState.informEnabled && cpeState.informInterval) {
    	now = time(NULL);
        if (cpeState.informTime && cpeState.informTime!=-1 ) {
        	time_t diff;
        	diff = now-cpeState.informTime;
        	next = abs(diff%cpeState.informInterval);
        	if ( diff>0)
        		next = cpeState.informInterval - next;
        } else {
            next = cpeState.informInterval-(abs(now-cpeState.basePTime)%cpeState.informInterval);
        }
        cpeDbgLog(DBG_ACSCONNECT, "next periodic inform = %d\n", (int) next);
        setTimer(startPeriodicInform, NULL, next*1000);
    }
    return;
}
/*
 * start sending Inform to ACS. Called once when CPE boots.
 */
void cwmpStartACSInform(void) {
    char    *infBuf;
    const char	*p;
    char firstboot[256]={0};
    char reboot[256]={0};
    char	dfltIP[IP_ADDRSTRLEN];
    /* set starting CWMP protocol version */
    cpeState.cwmpVersion = CWMP_VERSION;
    /* set retry defaults if not set by cpeWrapper */
    if ( cpeState.cwmpRetryMin == 0) {
        cpeState.cwmpRetryMin = CWMPRETRYMINIMUMWAITINTERVAL;
        cpeState.cwmpRetryMultiplier = CWMPRETRYINTERNALMULTIPLIER;
    } else if ( cpeState.cwmpRetryMultiplier < 1000 ){
        // force valid value.
        cpeState.cwmpRetryMultiplier = CWMPRETRYINTERNALMULTIPLIER;
    }
    cpeState.aliasAddressing = ALIAS_BASED_ADDRESSING;
    srand(time(0)); // seed rand function.

    /* init RPC Methods */
    initRPCMethods();
    
    /* set all Inform events from saved state */
    cmd_popen("uci get tr069.firstboot", firstboot);
    if ( !strncmp(firstboot,"0",1) && cpeState.eventMask == 0 )
    { /* EVT_BOOTSTRAP is 0 active */
        system("uci set tr069.firstboot='1'");
        system("uci commit tr069");
        cwmpAddEvent(eEvtBootstrap);
        cwmpAddEvent(eEvtBoot);
        cpeState.eventMask = 0;	        /* if BOOTSTRAP is 0 then this is first time */
                                        /* to attempt to inform this ACS. Thus all   */
                                        /* other pending events are forced to 0 */
        system("uci set tr069.reboot='1'");
    }
    else
    {
        cmd_popen("uci get tr069.reboot", reboot);
        if(!strncmp(reboot,"0",1) && cpeState.eventMask&EVT_BOOTSTRAP)
        {
            cwmpAddEvent(eEvtBoot);
            system("uci set tr069.reboot='1'");
        }
        if (cpeState.eventMask&EVT_VALUECHANGE)
            cwmpAddEvent(eEvtValueChange);
        if (anyCompleteXfer(eUpload) ) {
            cwmpAddEvent(eEvtTransferComplete);
            cwmpSetPending( PENDING_XFERCOMPL );
        }
        if (anyCompleteXfer(eAutonomousTransferComplete) ) {
            cwmpAddEvent(eEvtAutonomousTransferComplete);
            cwmpSetPending( PENDING_AUTOXFRCMPT );
        }

        if (cpeState.eventMask&EVT_REBOOT)
        {
            cwmpAddEvent(eEvtBoot);
            cwmpAddEvent(eEvtMReboot);
        }
    }
    cpeLockConfiguration();
    cpeRefreshCPEData(&cpeState);
    cpeRefreshInstances();
    setDNSCaching(FALSE);
    newSession(&acsSession);
    cwmpResetActiveNotification();

    if ( (p = writeInIPAddr(&cpeState.ipAddress)))
        strcpy(dfltIP, p);
    else
        dfltIP[0]='\0';
    cpeState.basePTime= time(NULL);
    if ( (infBuf=cwmpInformRPC( acsSession.sessID, acsSession.retryCount, dfltIP ))) {
        if ( postToAcs(infBuf)== -1){
            startInformRetryTimer(&acsSession); /* only if initial post call fails */
            cpeUnlockConfiguration(cwmpChangedCfg);
        }
        /* other retries within post code  */
    } else {
        cpeLog(LOG_ERR, "Failed to start ACS session: No memory available.");
        startInformRetryTimer(&acsSession);
        cpeUnlockConfiguration(cwmpChangedCfg);
    }
    setCallback(&cpeState, startConnReqInform, NULL);  /* callback is used for notification of */
    /* a ACS connection request */
    setCallback(eventPolicy, startCPEEvtInform, NULL); /* used by CPE msg event handler to start an*/
    /* Inform sequence */
    return;
    }
/*
 * Close the current connection to the ACS. If cookieOp is set to eSaveCookies
 * then move the cookie list from the connection to the sesion before closing.
 */
static void closeConnection( ACSSession *s, eCOOKIESTATE cookieOp ) {
    if (s->wio) {
    	DBGPRINT((stderr, "closeConnection %s\n", cookieOp==eSaveCookies?"save": "Discard"));
    	if ( cookieOp==eSaveCookies && s->wio->cookieHdrs) {
    		s->sessionCookie = s->wio->cookieHdrs;
    		s->wio->cookieHdrs = NULL;
    	}
    							 /* close connection. Also */
        wget_Disconnect(s->wio); /* discards the cookie list for the connection*/
        s->wio = NULL;
    }
    s->eHttpState = eClosed;
}

static void closeSession( ACSSession *s) {
    if (s->postMsg) {
        GS_FREE(s->postMsg); s->postMsg = NULL;
    }
    if (s->authHdr) {
        GS_FREE(s->authHdr); s->authHdr =NULL;
    }
    closeConnection(s, eDiscardCookies);
    s->eAuthState = eIdle;
    if (s->sessionCookie) {
    	wget_freeCookies( &s->sessionCookie );
    	s->sessionCookie = NULL;
    }
#ifdef GSLIB_COLLECT_EVENT_USAGE
    /* reports event, timer, registered callback usage in the event.c (enable in event.c)*/
    extern char *gsLibGetUsage();
    cpeLog(LOG_DEBUG, gsLibGetUsage());
#endif
}


static void emptyPostTimeout(void *handle) {
    ACSSession    *s = (ACSSession*)handle;
    DBGLOG((DBG_ACSCONNECT, "emptyPostTimeout\n"));
    sessDisconnect(s, ePostError);
    return;

}

static void sessDisconnect(ACSSession *s, eSessionStatus acsStatus) {
    static int nextsch = 0;
    DBGLOG((DBG_ACSCONNECT, "sessDisconnect: %d", acsStatus));
    stopTimer(emptyPostTimeout, (void *)s);
    //saveNotificationAttributes();

    closeSession(s);
    /* Unlock local configuration data here */
    cpeUnlockConfiguration(cwmpChangedCfg);
    cwmpChangedCfg = 0;						/* zero change count */
    switch (acsStatus) {
        case eAuthError:
            cpeLog(LOG_INFO, "ACS authentication failed");
            break;
        case eSOAPError:
            cpeLog(LOG_ERR, "ACS Disconnect: Received unrecognized SOAP msg");
            break;
        case ePostError:              /* HTTP status error, timeout, ... */
#if defined(CONFIG_CWMP_FALLBACK) && !defined(CONFIG_CWMP_V1_0)
        	/* only try to revert to V1.0 if BOOTSTRAP Event is pending or BOOT */
        	if ( ((!(cpeState.eventMask&EVT_BOOTSTRAP))
        			|| cwmpGetEventState(eEvtBoot)==eActive)
        			&& cpeState.cwmpVersion != 0 ){
        		/* revert to v1.0 if using v1.1 */
        		cpeState.cwmpVersion = 0;
        		setNameSpaceVersion(0);
        	}
#endif
        case eInternalError:
        case eRedirectError:
        case eFaultResponse:
        case eConnectError:
            cpeLog(LOG_ERR, "ACS Disconnect: error %d", acsStatus);
            break;
        case eAcsDone:
        default:
        	/* session was successful */

            DBGLOG((DBG_ACSCONNECT, "ACS Disconnect: ok"));
            if (notifyChanged){
                cwmpSaveNotifyAttributes();
                notifyChanged = 0;
            }
            cpeSaveCPEState();	   /* save any state changes */
            break;
    }
    #ifdef DMALLOC
    	dmalloc_log_changed(dmallocMark,1/*unfreed*/, 0/*no freed*/, 1/*log pnt */);
    #endif

    if ( cwmpIsPending(PENDING_ACSCHANGE) ) {
    	/* An ACS change has precedence over all other pending events */
    	resetAllEvents();
    	if ( acsStatus == eAcsDone )         /* if last session was successful */
    		notifyCallbacks( eventPolicy);   /* force startCPEEvtInform on call stack return*/
    } else {
    	/* first check all pending CPE actions requested by ACS */
	    if ( cwmpIsPending(PENDING_REBOOT)) {
	        cpeReboot();              /* no return expected */
	    }
#ifdef CONFIG_RPCFACTORYRESET
	    if ( cwmpIsPending(PENDING_RESET )) {
	        cpeFactoryReset();        /* no return expected */
	    }
#endif
#if defined(CONFIG_RPCUPLOAD) || defined(CONFIG_RPCDOWNLOAD)
	    if ( cwmpIsPending(PENDING_STARTXFER)) {
	        notifyCallbacks( &cpeState.dlQ );
	    }
#endif
		/* second check if we need to schedule a retry for Inform */
	    if ( startInformRetryTimer(s) == 0 ) {
	    	/* no retry scheduled  */
			/* Second check if there are any pending events or PENDING RPCs */
			/* so that a new session can be started. Event should already be in table */
		    if ( cwmpIsPending(PENDING_SENDKICKED|PENDING_DIAGINFORM|PENDING_CONNREQ
			         |PENDING_GETRPC|PENDING_REQDOWNLOAD|PENDING_XFERCOMPL
			         |PENDING_AUTOXFRCMPT|PENDING_VALUECHG)) {
		    	/* The eventPolicy callback will call startCPEEvtInform when the call */
		    	/* stack is popped and the event handler is back in control. */
		    	/* Not good to start a new Inform session inside of the IO     */
		    	/* completion callback chain.                                  */
		    	notifyCallbacks( eventPolicy);
		    } else {
                if (!nextsch)
                {
                    scheduleNextInform();	/* no pending RPC - schedule next Inform */
                    nextsch = 1;
                }
	    	    notifyCallbacks( &acsSession ); /* schedule a call to any CPE function*/
			}
	    } else {
	    	DBGLOG((DBG_ACSCONNECT, "sessDisconnect: Inform retry scheduled"));
	    }

    }
    return;
}

static void updateAuthorizationHdr( ACSSession *s ) {
    if (s->authHdr) {
        if ( s->sessionAuth.qopType==eAuth ) {
            GS_FREE(s->authHdr);
            s->authHdr = generateNextAuthorizationHdrValue( &s->sessionAuth, cpeState.acsUser, cpeState.acsPW );
        } /* update unconditionally if basic */
        wget_AddPostHdr(s->wio, "Authorization", s->authHdr);
    }
}

static void sendEmptyPost(ACSSession *s) {
    DBGLOG((DBG_ACSCONNECT, "sendEmptyPost(%s) to ACS", s->eHttpState==eClose? "close": "keepOpen"));
    if (s->postMsg) {
        GS_FREE(s->postMsg); s->postMsg = NULL;
    }
    postToAcs(NULL);
    setTimer(emptyPostTimeout, (void *)s, ACSRESPONSETIME/2);	/* half of max */
    return;
}

/*
 * look in SOAP message for the name-space and version.
 * If the message contains version is v1.0 then revert to it.
 */
#if defined(CONFIG_CWMP_FALLBACK) && !defined(CONFIG_CWMP_V1_0)
static void chkRevertCWMPVersion( char *soapBuf ){
	/* get the namespace version */
	if ( strstr(soapBuf, "urn:dslforum-org:cwmp-1-0") != NULL) {
		/* revert to v1.0 if using v1.1 */
		cpeState.cwmpVersion = 0;
		setNameSpaceVersion(0);
	}
}
#endif

/*
 * copy new parameter key to cpeState if present.
 * otherwise; free existing key.
 */
static void updateParameterKey( RPCRequest *r) {
    strcpy(cpeState.parameterKey, r->parameterKey);
    ++cwmpChangedCfg;			/* bump change count */
    return;
}
/*
 * run the RPC Request
 *This function runs the RPC method by calling the method implementation
 * function from the switch statement. A pointer to the response buffer
 is returned in rpcBuf.
 * Unimplemented RPC methods fall thru the switch to the default: and
 * return a fault response.
 */
static eRPCStatus runRPC( ACSSession *s ) {
    RPCRequest *r = s->rpc;
    char       *reqID = r->ID;   /* response contains the request ID */
    int        cpeError;		 /* 0 or RPC 9xxx value or -1 to force eRpcFail */
    int        pending =0 ;		 /* pending action following session end */
    eRPCStatus rs=eRpcContinue;
    char        *rpcBuf = NULL;	 /*buffer returned with response */
    cwmpDecodeRPCRequest(r);

    switch (r->rpcMethod) {
        case eGetRPCMethods:
            runGetRPCMethods(reqID, r, &rpcBuf);
            break;
        case eGetParameterNames:
            runGetParameterNames(reqID, r, &rpcBuf);
            break;
        case eGetParameterValues:
            runGetParameterValues(reqID, r, &rpcBuf);
            break;
        case eSetParameterValues:
            cpeError = runSetParameterValues(reqID, r, &pending, &rpcBuf);
            if (cpeError==0) {
                updateParameterKey( r );
                cwmpResetActiveNotification();
            } else if ( cpeError==-1)
            	rs = eRpcFail;
            break;
        case eGetParameterAttributes:
            runGetParameterAttributes(reqID, r, &rpcBuf);
            break;
        case eSetParameterAttributes:
            cpeError = runSetParameterAttributes(reqID, r, &rpcBuf );
            notifyChanged = (cpeError == 0);
            break;
        case eAddObject:
            cpeError = runAddObject(reqID, r, &pending, &rpcBuf );
            if (cpeError==0) {
                updateParameterKey( r );
                cwmpResetActiveNotification();
            }
            break;
        case eDeleteObject:
            cpeError = runDeleteObject(reqID, r, &pending, &rpcBuf );
            if (cpeError==0) {
                updateParameterKey( r );
                cwmpResetActiveNotification();
            }
            break;

#ifdef CONFIG_RPCDOWNLOAD
        case eDownload:
            if ( !(runDownload(reqID, r, &pending, &rpcBuf )))
                s->rpc = NULL; /* Request has been queued for later action */
            break;
#endif
#ifdef CONFIG_RPCSCHEDULEDOWNLOAD
        case eScheduleDownload:
            if ( !(runScheduleDownload(reqID, r, &pending, &rpcBuf )))
                s->rpc = NULL; /* Request has been queued for later action */
            break;
#endif
#if defined(CONFIG_RPCREQUESTDOWNLOAD)||defined(CONFIG_RPCDOWNLOAD)||defined(CONFIG_RPCUPLOAD)
        case eCancelTransfer:
        	runCancelTransfer(reqID, r, &pending, &rpcBuf);
        	break;
#endif

        case eReboot:
            runReboot(reqID, r, &pending, &rpcBuf );
            break;
        case eInformResponse:
            clearAllEvents(eInformResponse);
            cwmpResetActiveNotification();
            cwmpClearPending( PENDING_ACSCHANGE|PENDING_VALUECHG); /* force ACSCHANGE VALUECHG clear */
            cpeState.eventMask |= EVT_BOOTSTRAP;  /* force EVT_BOOTSTRAP complete, active 0 */
            rs = eRpcInit;
            break;

#ifdef CONFIG_RPCFACTORYRESET
        case eFactoryReset:
            runFactoryReset(reqID, r, &pending, &rpcBuf );
            break;
#endif
#ifdef CONFIG_RPCUPLOAD
        case eUpload:
            if ( !(cpeError = runUpload(reqID, r, &pending, &rpcBuf )))
                s->rpc = NULL; /* Request has been queued for later action */
            break;
#endif
#ifdef CONFIG_RPCGETQUEUEDTRANSFERS
        case eGetQueuedTransfers:
            runGetQueuedTransfers(reqID, &rpcBuf);
            break;
#endif
#ifdef CONFIG_RPCGETALLQUEUEDTRANSFERS
        case eGetAllQueuedTransfers:
            runGetAllQueuedTransfers(reqID, &rpcBuf);
            break;
#endif
#ifdef CONFIG_RPCSCHEDULEINFORM
        case eScheduleInform:
            runScheduleInform(reqID, r, &rpcBuf);
            break;
#endif
#ifdef CONFIG_RPCREQUESTDOWNLOAD
		case eRequestDownloadResponse:
			clearAllEvents(eRequestDownloadResponse);
			cwmpClearPending( PENDING_REQDOWNLOAD );
			rs = eRpcInit;
			break;
#endif
#ifdef CONFIG_RPCKICK
		case eKickedResponse:
			clearAllEvents(eKickedResponse);
			cwmpClearPending( PENDING_SENDKICKED );
			kickSendRedirect(r->ud.kickedResponseReq.nextURL);
            rs = eRpcContinue;
			break;
#endif

#ifdef CONFIG_RPCSETVOUCHERS
		case eSetVouchers:
			runSetVouchers(reqID, r, &rpcBuf);
			break;
		case eGetOptions:
			runGetOptions( reqID, r, &rpcBuf);
			break;

#endif

        case eTransferCompleteResponse:
        	if ( !completeTransfer(eUpload/*or eDownload*/) ) {
        		/* no more completed transfers */
        	    cwmpClearPending(PENDING_XFERCOMPL);
            	cpeState.eventMask = cpeState.eventMask & (~EVT_XFERCOMPL);
            	clearAllEvents(eTransferCompleteResponse);
            	rs = eRpcEnd;
        	} else
        		rs = eRpcInit;
            break;
#ifdef CONFIG_RPCAUTONOMOUSTRANSFERCOMPLETE
        case eAutonomousTransferCompleteResponse:
        	if ( !completeTransfer(eAutonomousTransferComplete) ) {
        		/* no more completed autonomous transfers */
        	    cwmpClearPending(PENDING_AUTOXFRCMPT);
            	cpeState.eventMask = cpeState.eventMask & (~EVT_AUTOXFRCMPL);
            	clearAllEvents(eAutonomousTransferCompleteResponse);
            	rs = eRpcEnd;
        	} else
        		rs = eRpcInit;
            break;
#endif
#ifdef CONFIG_RPCCHANGEDUSTATE
        case eChangeDUState:
        	runChangeDUState(reqID, r, &rpcBuf );
        	break;
        case eDUStateChangeCompleteResponse:
        	clearAllEvents(eDUStateChangeCompleteResponse);
    		cwmpDUStateChangeAcked();
    		rs = eRpcEnd;
        	break;

#endif
        case eGetRPCMethodsResponse:
            rs = eRpcInit;
            break;
        case eFault:
            if ( r->ud.faultMsg.faultCode == 8005 ) /* CWMP Retry request */
            	rs = eRpcRetry;
            else
            	rs = eRpcFail;
            break;
        default:
            runSendFault(reqID, 9000/*Unknown method*/, &rpcBuf);
            rs = eRpcAbort;
            break;
    }
    cwmpSetPending(pending);
    if ( rs == eRpcContinue || rs==eRpcAbort ) {
        postToAcs( rpcBuf );
    }
    return rs;
}
/*
 * sendPendingRPC
 * Any active event that indicates a pending RPC or pending action is checked and sent here.
 *
 */
static eRPCStatus sendPendingRPC( ACSSession *s) {
    eRPCStatus rs;
    char    *rpcBuf = NULL;
    RPCRequest *r;
    if ( isEventActive(eEvtTransferComplete)==eActive && (r=anyCompleteXfer(eUpload/*or eDownload*/)) ) {
    	sendTransferComplete( s->sessID, r, &rpcBuf	);
    	rs= eRpcContinue;
    } else if (cwmpIsPending(PENDING_GETRPC) ) {
        sendGetRPCMethods(s->sessID, &cpeState, &rpcBuf);
        cwmpClearPending(PENDING_GETRPC);  /* clear here because we don't care about the response*/
        rs = eRpcContinue;
#ifdef CONFIG_RPCREQUESTDOWNLOAD
    } else if ( isEventActive(eEvtRequestDownload) && pendingAct&PENDING_REQDOWNLOAD ){
        sendRequestDownload(s->sessID, &cpeState, &rpcBuf);
        rs = eRpcContinue;
#endif

#ifdef CONFIG_RPCAUTONOMOUSTRANSFERCOMPLETE
    } else if ( isEventActive(eEvtAutonomousTransferComplete) && (r=anyCompleteXfer(eAutonomousTransferComplete))) {
    	sendAutonomousTransferComplete(s->sessID, r, &rpcBuf);
    	rs = eRpcContinue;
#endif

#ifdef CONFIG_RPCKICK
   } else if ( isEventActive(eEvtKicked) && pendingAct&PENDING_SENDKICKED){
        sendKickedRPC(s->sessID, &cpeState, &rpcBuf);
        rs = eRpcContinue;
#endif

#ifdef CONFIG_RPCCHANGEDUSTATE
   } else if ( isEventActive(eEvtDUStateChangeComplete) ) {
	   CPEChangeDUState *cpeDU = cwmpGetCompletedDUChange();
	   if ( cpeDU )
		   sendDUStateChangeComplete( s->sessID, cpeDU, &rpcBuf);
	   rs = eRpcContinue;
#endif
    } else
        rs = eRpcEnd;
    if (rs == eRpcContinue && rpcBuf!=NULL)
        postToAcs(rpcBuf);
    return rs;
}

static void processResponse( ACSSession *s, char *soapMsg, int mlth ) {
    eRPCStatus rpcStatus;
    int		   keepPostMsg=0;
    if (soapMsg) {
        ParseHow    parseReq;
        DBGLOG((DBG_DUMPSOAP, "processResponse() Received Soap Msg=\n%s", soapMsg));
        parseReq.topLevel = cwmpEnvelopeDesc;
        parseReq.nameSpace = nameSpaces;
        if ( xmlParseGeneric(&s->rpc, NULL, soapMsg, mlth, &parseReq) !=XML_STS_OK ){
            cpeLog(LOG_DEBUG, "cwmpSession: ACS Msg. Parse Error %80s", soapMsg);
            rpcStatus = eRpcAbort;
        } else if ( s->rpc != NULL ){
            /* successful parse of SOAP - run the RPC */
#if defined(CONFIG_CWMP_FALLBACK) && !defined(CONFIG_CWMP_V1_0)
        	eEState bootEvt = cwmpGetEventState(eEvtBoot);  /* save BOOT event state across runRPC call */
#endif
            s->holdFlag = s->rpc->holdRequests;
            rpcStatus = runRPC(s);
#if defined(CONFIG_CWMP_FALLBACK) && !defined(CONFIG_CWMP_V1_0)
        	/* only check if need to revert to V1.0 if BOOTSTRAP Event is pending or BOOT
        	 * and the ACS returned a SOAP fault or a successful InformResponse.
        	*/
        	if ( ((!(cpeState.eventMask&EVT_BOOTSTRAP))	|| bootEvt==eActive)
        		 && cpeState.cwmpVersion != 0 ){
                if ( rpcStatus == eRpcFail || s->rpc->rpcMethod == eInformResponse ) {
                	chkRevertCWMPVersion(soapMsg);
                }
        	}
#endif
        } else {
            cpeLog(LOG_DEBUG, "cwmpSession: ACS Msg did not contain SOAP RPC");
            rpcStatus = eRpcFail;
        }
        if (s->rpc) {
            cwmpFreeRPCRequest(s->rpc);
            s->rpc = NULL;
        }
        GS_FREE(soapMsg);
    } else {
        /* received empty msg from ACS */
    	DBGLOG((DBG_DUMPSOAP, "processResponse() Received Empty Msg: %s", s->eNullState == eNoNullSent? "NoNullSent": "NullSent"));
        s->holdFlag = 0;
        if (s->eNullState == eNoNullSent)
            rpcStatus = eRpcEnd;
        else
            rpcStatus = eRpcDone;
    }

    DBGLOG((DBG_DUMPSOAP, "processResponse() rpcStatus: %d", rpcStatus));
    switch (rpcStatus) {
        case eRpcInit:
            /* received an InformResponse */
            if (s->holdFlag)
                sendEmptyPost(s);      /* send empty http to indicate CPE is ready */
            else {
                /* a session has been initiated - the response at this point */
                /* is either an empty message or a CPE RPC command if any    */
                /* actions are pending. */
                if ( sendPendingRPC(s) == eRpcEnd ){
                    s->eNullState = eSentNull;
                    sendEmptyPost(s);    /* nothing pending */
                } else
                	keepPostMsg = 1;
            }
            break;
        case eRpcContinue:
            /* next rpc has been posted to ACS - wait for response */
        	keepPostMsg = 1;
            break;
        case eRpcEnd:
            if ( sendPendingRPC(s) == eRpcEnd ){
            	/* send empty message to ACS to signal that CPE is ready */
                s->eNullState = eSentNull;
                sendEmptyPost(s);    /* nothing pending */
            } else
            	keepPostMsg = 1;     /* sent the pending msg */
            break;
        case eRpcFail:				 /* SOAP Fault */
            sessDisconnect(s, eFaultResponse);
            break;
        case eRpcRetry:
            postToAcs( s->postMsg );
            keepPostMsg = 1;
            break;
        case eRpcAbort:
        	sessDisconnect(s, eSOAPError);
        	break;
        case eRpcDone:
        default:
            sessDisconnect(s, eAcsDone);
            break;
    }
    if ( !keepPostMsg )
    	if (s->postMsg){
    		GS_FREE(s->postMsg); s->postMsg = NULL;
    	}
}
/*
 * Common clean up of a failed session.
 *
 */
static void failSession( ACSSession *s, eSessionStatus status){
    if (s->postMsg) {
        GS_FREE(s->postMsg); s->postMsg = NULL;
    }
    sessDisconnect(s, status);
    return;
}
/*
 * post to server has completed
 * .
 */

static void postToAcsComplete(void *handle) {
    tWget *w = (tWget *)handle;
    ACSSession  *s = (ACSSession*)w->handle;
    SessionAuth *sa = &s->sessionAuth;
    int         skip=1;

    DBGLOG((DBG_ACSCONNECT, "postToAcsComplete"));
    stopTimer(emptyPostTimeout, (void *)s);

    if (w->status==iWgetStatus_Ok) {
        if ( s->wio->hdrs->Connection && !strcasecmp(s->wio->hdrs->Connection, "close"))
            s->eHttpState = eClose;
        DBGLOG((DBG_ACSCONNECT, " Connection = %s", s->eHttpState==eClose?"close": "keep-alive"));

        if (w->hdrs->status_code== 100) {
            /* 100 Continue: Just ignore this status */
        	;
        } else if (w->hdrs->status_code== 401) {
            /* need to send authenticate */
            char *authHdr;
            if ( w->hdrs->content_length>0
                || (w->hdrs->TransferEncoding && streq(w->hdrs->TransferEncoding, "chunked"))) {
                int mlth;
                char *tmpBuf;
                if ( (tmpBuf = wget_ReadResponse(w, &mlth, MAXWEBBUFSZ)) )
                    GS_FREE(tmpBuf);
                /* it is possible to return a NULL if there is no data on with the 401 message */
            }
            //!!!!!!!s->eNullState = eNoNullSent;
            GS_FREE(s->authHdr); /* free in case of reauth requested during connection */
            s->authHdr = NULL;
            if ( s->eAuthState==eAuthenticating) {
                s->eAuthState = eAuthFailed;
                /* disconnect and delay */
                failSession( s, eAuthError);
                return;
            } else
                s->eAuthState = eAuthenticating;

            DBGLOG((DBG_ACSCONNECT, " WWW-Authenticate= %s\n", w->hdrs->wwwAuthenticate));
            if (!(authHdr = generateAuthorizationHdrValue(sa, w->hdrs->wwwAuthenticate,
									"POST", s->wio->uri, cpeState.acsUser, cpeState.acsPW))) {
                cpeLog(LOG_ERR, "WWWAuthenticate parsing error: %s", w->hdrs->wwwAuthenticate);
                failSession( s, eInternalError);
                return;
            }
            s->authHdr = authHdr;
            if (skip==0 || s->eHttpState==eClose ) { /* end of data on 401 skip_Proto() */
                /* close connection and reconnect with Authorization header*/
                closeConnection(s, eSaveCookies);
                s->wio=wget_Connect(s->sessACSURL, acsConnected, s);
                if (s->wio==NULL) {
                    cpeLog(LOG_ERR, "ACS Connect failed: %s", wget_LastErrorMsg());
                    GS_FREE(s->postMsg); s->postMsg = NULL;
                    GS_FREE(s->authHdr); s->authHdr = NULL;
                }
                return;
            } else {
                wget_AddPostHdr(s->wio, "Authorization", s->authHdr);
                /* now just resend the last data with the Authorization header */
                wget_PostData(s->wio, s->postMsg, s->postLth, "text/xml", postToAcsComplete, (void*)s);
            }
        } else {
            s->eAuthState = eAuthenticated;
            if (w->hdrs->status_code==200 && ( (w->hdrs->content_length>0)
            || (w->hdrs->TransferEncoding && streq(w->hdrs->TransferEncoding, "chunked")))) {
                /* allocate buffer and parse the response */
                int     mlth;
                char    *soapMsg;

                if ( (soapMsg = wget_ReadResponse(w, &mlth, MAXWEBBUFSZ)) ){
					if (s->eHttpState==eClose)
						closeConnection(s, eSaveCookies);
					if (soapMsg && mlth >0) {
						 ;//!!!!!!!s->eNullState = eNoNullSent;
					} else { /* should only happen if 200 OK with empty chunked.*/
						 DBGLOG((DBG_ACSCONNECT, "cwmpSession: HTTP status=200 empty response: ACS Protocol violation!" ));
					}
					processResponse(s, soapMsg, mlth);
                } else {
                	cpeLog(LOG_ERR, "ACS response corrupted or timed out");
                	failSession(s, eInternalError);
                }

            } else if ( (w->hdrs->status_code == 200 && !w->hdrs->TransferEncoding )
            				|| w->hdrs->status_code == 204 ) {
                /* empty ACS message -- ACS is done */
                DBGLOG((DBG_ACSCONNECT, " cwmpSession: recvd empty msg, eNullState=%s", s->eNullState == eNoNullSent?"eNoNullSent": "NullSent" ));
                if (s->eHttpState==eClose)
                    closeConnection(s, eSaveCookies);
                /* msg posted - free buffer */
                GS_FREE(s->postMsg); s->postMsg = NULL;
                //resetNotificationBuf();
                processResponse(s, NULL, 0);

            } else if ( (w->hdrs->status_code>=300 && w->hdrs->status_code<=307
            			&& w->hdrs->locationHdr )) {
                /* Redirect status with new location */
                /* repost msg to new URL */
                setSessionURL(s, w->hdrs->locationHdr);
                closeConnection(s, eSaveCookies);
                if ( s->authHdr ) {   /* authorization is controlled by new host */
                	GS_FREE(s->authHdr); /* discard current */
                	s->authHdr = NULL;
                }
                s->eAuthState = eIdle;
                s->eHttpState = eStart;
                cpeLog(LOG_DEBUG, "Redirect to %s", s->sessACSURL);
                if ( ++s->redirCount <= MAX_REDIRECTS ) {
	                s->wio=wget_Connect(s->sessACSURL, acsConnected, s);
	                if (s->wio==NULL) {
	                    cpeLog(LOG_ERR, "Redirect failed: %s", wget_LastErrorMsg());
	                    sessDisconnect(s, eRedirectError);
	                }
                } else {
                	cpeLog(LOG_INFO, "Session Redirects exceeded maximum");
                	failSession(s, eRedirectError);
	        	}
            } else if ( w->hdrs->status_code == 404 ){
            	cpeLog(LOG_ERR, "cwmpSession: service not found %d", w->hdrs->status_code);
            	failSession(s, eConnectError);
            } else {
                cpeLog(LOG_ERR, "cwmpSession: Unknown status_code=%d received from ACS", w->hdrs->status_code);
                failSession(s, ePostError);
            }
        }
    } else {
        cpeLog(LOG_ERR, "cwmpSession: Post to ACS failed, Status = %d %s", w->status, w->msg);
        failSession(s ,ePostError);
    }
    return;
}
/*
 * The connection to the ACS has been completed or
 * an error has ocurred.
 */
static void acsConnected(void *handle) {
    tWget *w = (tWget *)handle;
    ACSSession    *s = (ACSSession*)w->handle;
    if (w->status != 0) {
        GS_FREE(s->postMsg); s->postMsg = NULL;
        sessDisconnect(s, eConnectError);
        cpeLog(LOG_ERR, "acsConnect Status = %s(%d)", w->msg, w->status);
        return;
    }
    if (s->wio==NULL) {
        s->eHttpState = eClosed;
        cpeLog(LOG_CRIT, "Error -- pointer to wget IO desc is NULL");
        return;
    }
    if ( !eqInIPAddr( &cpeState.acsIPAddress, &s->wio->host_addr) ){
    	/* update dns caching and IP affinity */
    	dns_set_cache_host_ip( s->wio->host, &s->wio->host_addr);
    	cpeState.acsIPAddress = s->wio->host_addr;
    }
    s->eHttpState = eConnected;
    /* if the connection has no cookies and there are session cookies then */
    /* move the cookie list to the connection. The cookie list would have */
    /* been set by an ACS earlier in the session */
    if ( s->wio->cookieHdrs == NULL && s->sessionCookie ){
    	s->wio->cookieHdrs = s->sessionCookie;
    	s->sessionCookie = NULL;
    }
    wget_AddPostHdr(s->wio, "SOAPAction", "");
    if (s->authHdr) {
        if ( s->eAuthState==eAuthenticated)
            updateAuthorizationHdr(s);
        else
            wget_AddPostHdr(s->wio, "Authorization", s->authHdr);
    }
    DBGLOG((DBG_DUMPSOAP, "postToAcs(Connected) lth=%d:\n%s", s->postLth, s->postMsg? s->postMsg: "Empty Message"));
    wget_PostData(s->wio, s->postMsg, s->postLth, "text/xml", postToAcsComplete, (void *)s);
    return;
}
/*
 * Post the current buffer to the ACS. This is an async call. The
 * return status only indicates that the connection has started.
 *
 * Returs 0: oK - post in progress.
 *       -1: Error - no ACS URL defined.
 */

static int postToAcs(char *buf ){
    ACSSession  *s = &acsSession;

    if ( s->postMsg && s->postMsg!=buf ) {
    	/* Check if buf == postMsg as post may be a retry */
        GS_FREE(s->postMsg);
    }
    s->postMsg = buf;
    s->postLth = buf? strlen(buf): 0;
    //!!!!!!!!s->eNullState = s->postLth? eNoNullSent: eSentNull;
    if ( s->sessACSURL[0] != '\0' ) {
        if (s->wio == NULL) {
            /* make a new connection to the ACS */
            DBGLOG((DBG_ACSCONNECT, "Connect to ACS at %s\n", s->sessACSURL));
            s->eHttpState = eStart;
            s->wio=wget_Connect(s->sessACSURL, acsConnected, s);
#if 0
            while (s->wio==NULL || s->wio->status == -5) {
                s->wio=wget_Connect(s->sessACSURL, acsConnected, s);
                sleep(5);
            }
#else
        if (cpeState.sigusr1==1)
        {
            while (s->wio==NULL || s->wio->status == -5) {
                s->wio=wget_Connect(s->sessACSURL, acsConnected, s);
                sleep(5);
            }
        }
        else
        {
            if ( s->wio==NULL || s->wio->status == -5 ) {
                    cpeLog(LOG_ERR, "ACS Connect Failed: %s", wget_LastErrorMsg());
                    GS_FREE(buf); s->postMsg = NULL;
                    acsSession.wio = NULL;
                    return -1;
            }
        }
#endif
        } else { /* connection in place */
            wget_ClearPostHdrs(s->wio);
            if ( s->postMsg )  /* do not send SOAPACtion hdr on empty msg */
            	wget_AddPostHdr(s->wio, "SOAPAction", "");
            DBGLOG((DBG_DUMPSOAP, "postToAcs:\n%s", s->postMsg? s->postMsg:"(empty)"));
            updateAuthorizationHdr(s);
            if (s->eHttpState == eClose)
                wget_PostDataClose(s->wio, s->postMsg, s->postLth,
                		s->postMsg? SOAPCONTENTTYPE: NULL, postToAcsComplete, ( void*)s);
            else
                wget_PostData(s->wio, s->postMsg, s->postLth,
                		s->postMsg? SOAPCONTENTTYPE: NULL, postToAcsComplete, (void *)s);
        }
        return 0;
    }
    return -1;
}
void updateConnection(void *handle)
{
        char proto[PROTOCOL_SZ];
        char host[HOSTNAME_SZ];
        char uri[URI_SZ];
        int port;
        if (www_ParseUrl(cpeState.acsURL, proto, host, &port, uri) < 0) {
            fprintf(stdout, "parserUrl fail\n");
        }
        dns_lookup_auto( host, SOCK_DGRAM, htons(port), AF_UNSPEC, &cpeState.acsIPAddress );

        setTimer(retryACSInform, NULL, 1*1000);
}

#ifdef CONFIG_RPCSCHEDULEINFORM
void cwmpScheduleInform(void *handle) {
	DBGPRINT((stderr, "cwmpScheduleInform()\n"));
	cwmpAddEvent(eEvtScheduled);
	cwmpAddEvent(eEvtMScheduleInform);
	retryACSInform(NULL);
}
#endif /* #ifdef CONFIG_RPCSCHEDULEINFORM */


