/*----------------------------------------------------------------------*
 * Gatespace
 * Copyright 2006-2011 Gatespace Networks, Inc., All Rights Reserved.
 * Gatespace Networks, Inc. confidential material.
 *----------------------------------------------------------------------*
 * File Name  : transfer.c
 * Description:	CWMP http upload/download implementation
 *----------------------------------------------------------------------*
 * $Revision: 1.2 $
 *
 * $Id: transfer.c,v 1.2 2012/05/10 17:38:08 dmounday Exp $
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
#include <signal.h>

#include "../includes/sys.h"
#include "../includes/paramTree.h"
#include "../includes/rpc.h"
#include "../includes/CPEWrapper.h"
#include "../gslib/src/utils.h"
#include "../gslib/src/event.h"
#include "../gslib/src/xmlWriter.h"
#include "../gslib/src/xmlParserSM.h"
#include "../gslib/src/www.h"
#include "../gslib/auxsrc/dns_lookup.h"
#include "../gslib/src/event.h"
#include "../gslib/src/protocol.h"
#include "../gslib/src/wget.h"
#include "../soapRpc/rpcUtils.h"
#include "../soapRpc/xmlTables.h"
#include "../soapRpc/cwmpSession.h"
#include "../soapRpc/rpcMethods.h"
#include "../soapRpc/notify.h"
#ifdef CONFIG_RPCKICK
#include "../soapRpc/kickHandler.h"
#endif
#include "targetsys.h"

#ifdef DMALLOC
#include "dmalloc.h"
unsigned long dmallocMark;
#endif
#ifdef DEBUGLOG
#define DBGPRINT(X) fprintf X
#else
#define DBGPRINT(X)
#endif

extern CPEState cpeState;
/************************************************************/
/* http upload/download session management                         */
/* The download mgmt allows one upload or download session at a time 	*/
/* Multiple download RPC requests may be pending and are    */
/* queued to the cpeState.dlQ.                              */
/*
 * steps in download
 * 1. verify protocol and url.
 * 2. Start connection to url
 * 3. Start get data from url
 * 4. read data - possibly multiple chunks into response buffer
 * 5. close connection.
 * 7. set transfer complete pending flag
 * 6. call CPE download end function
 * Buffer is allocated by the read data function and grown
 * as needed.
 *
 * steps in upload
 * 1. verify protocol and url.
 * 2. call CPE Upload setup function.
 * 2. Start connection to url.
 * 3. Call CPE upload get data function.
 * 4. http PUT data.
 * 5. close connection.
 * 6. call CPE upload complete function.
 * Buffer is allocated by the cpe get data function and freed by the
 * cpe complete function.
 *
 */
// forwards
static void startTransfer(void *handle);
static void nQTransferRequest(RPCRequest *r);
static void downloadWindowEnded(void *);

#if defined(CONFIG_RPCUPLOAD) || defined(CONFIG_RPCDOWNLOAD)

static void xfrConnected(void *handle);

ACSSession cwmpXfrSession;

#ifdef  CONFIG_RPCGETQUEUEDTRANSFERS
static void writeQueuedTags(XMLWriter *xp, char *cmdKey, int state) {
	xmlOpenTagGrp(xp, "QueuedTransferStruct");
	xmlPrintTaggedData(xp, "CommandKey", "%s", cmdKey ? cmdKey : "");
	xmlPrintTaggedData(xp, "State", "%d", state);
	xmlCloseTagGrp(xp);
}
int cwmpGetQueuedTransfersList(XMLWriter *xp) {
	RPCRequest *r = cpeState.dlQ;
	int state;
	int i = 0;

	while (r != NULL) {
		writeQueuedTags(xp, r->commandKey, 1);
		r = r->next;
		++i;
	}
	r = cpeState.dlQActive;
	while (r) {
		if (r->DLMsg.dlStatus == -1 || r->DLMsg.dlStatus == 2)
			state = 2;
		else
			state = 3;
		writeQueuedTags(xp, r->commandKey, state);
		r = r->next;
		++i;
	}
	return i;
}
#endif /* #ifdef CONFIG_RPCGETQUEUEDTRANSFERS */

#ifdef  CONFIG_RPCGETALLQUEUEDTRANSFERS
static void writeAllQueuedTags(XMLWriter *xp, RPCRequest *r, int state) {

	xmlOpenTagGrp(xp, "AllQueuedTransferStruct");

	xmlPrintTaggedData(xp, "CommandKey", "%s", r->commandKey);
	xmlPrintTaggedData(xp, "State", "%d", state);
	xmlPrintTaggedData(xp, "IsDownload", "%s", (r->rpcMethod == eDownload
			|| r->rpcMethod == eScheduleDownload) ? "1" : "0");
	xmlPrintTaggedData(xp, "FileType", "%s",
			r->DLMsg.fileType ? r->DLMsg.fileType : "");
	xmlPrintTaggedData(xp, "FileSize", "%d", r->DLMsg.fileSize);
	xmlPrintTaggedData(xp, "TargetFileName", "%s",
			r->DLMsg.targetFileName ? r->DLMsg.targetFileName : "");
	xmlCloseTagGrp(xp); /* AllQueuedTransferStruct */
}
/*
 * return: number of items in queue.
 */
int cwmpGetAllQueuedTransfersList(XMLWriter *xp) {
	RPCRequest *r = cpeState.dlQ;
	int state;
	int i = 0;

	while (r != NULL) {
		writeAllQueuedTags(xp, r, 1);
		r = r->next;
		++i;
	}
	r = cpeState.dlQActive;
	while (r) {
		if (r->DLMsg.dlStatus == -1 || r->DLMsg.dlStatus == 2)
			state = 2;
		else
			state = 3;
		writeAllQueuedTags(xp, r, state);
		r = r->next;
		++i;
	}
	return i;
}
#endif /* #ifdef CONFIG_RPCGETALLQUEUEDTRANSFERS */

/*
 * Dequeue RPCRequest *item from the waiting queue and
 * adjust the following delay timers.
 * Return dequeued RPCRequest * or NULL if not found.
 */
static RPCRequest *dQWaitingItem(RPCRequest *item) {
	RPCRequest *r = cpeState.dlQ;
	RPCRequest **last = &cpeState.dlQ;
	while (r != NULL) {
		if (r == item) {
			*last = r->next;
			if (r->next != NULL) {
				//need to adjust next delayTime
				time_t now = time(NULL);
				r->next->DLMsg.delaySeconds += r->next->DLMsg.targetTime - now;
			}
			return r;
		}
		last = &r->next;
		r = r->next;
	}
	return r;
}

/*
 * nQTransferRequest
 * Enqueue the transfer request on the waiting queue in the
 * order of delay time.
 * There may be multiple transfer requests before one completes( spec says 3)
 */

static void nQTransferRequest(RPCRequest *r) {
	RPCRequest **last = &cpeState.dlQ;
	RPCRequest *p = *last;
	time_t now = time(NULL);
	int when;

#ifdef CONFIG_RPCSCHEDULEDOWNLOAD
	// If a ScheduledDownload then move the WindowStart delay of first time window to
	// delaySeconds and queue request. Also set a timer for the WindowEnd time.
	if (r->rpcMethod == eScheduleDownload) {
		if (r->ud.downloadReq.timeIndex < MAX_TIMEWINDOWS) {
			TimeWindow *twp = &r->ud.downloadReq.timeWindow[r->ud.downloadReq.timeIndex];
			if (twp->windowStart > 0) {
				r->ud.downloadReq.delaySeconds = twp->windowStart;
				setTimer(downloadWindowEnded, (void *) r, twp->windowEnd * 1000);
			}
		}
	}
#endif
	r->DLMsg.targetTime = now + r->ud.downloadReq.delaySeconds;
	/* now add new item in order of delay time, Adjusting delay time by preceeding item's delay */
	if (p != NULL) {
		stopTimer(startTransfer, NULL);
	}
	r->next = NULL;
	while (*last) {
		if (r->ud.downloadReq.delaySeconds< (*last)->ud.downloadReq.delaySeconds) {
			/* queue new item, r here */
			r->next = *last;
			*last = r;
			last = &r->next;
			while (*last) { /* continue thru queue and adjust delay time */
				(*last)->ud.downloadReq.delaySeconds -= r->ud.downloadReq.delaySeconds;
				p = *last;
				last = &p->next;
			}
			when = cpeState.dlQ->ud.downloadReq.targetTime - now; /* calculate remaining interval */
			if (when < 0)
				when = 0;
			setTimer(r->rpcMethod==eScheduleDownload? cwmpStartWindow: startTransfer, NULL, when * 1000);
			cpeDbgLog(DBG_TRANSFER,
					"*******Insert Req SetTimer for %d******************\n",
					when * 1000);
			return;
		} else {/* else adjust delay on new item */
			if (r->ud.downloadReq.delaySeconds > 0)
				r->ud.downloadReq.delaySeconds-= (*last)->ud.downloadReq.delaySeconds;
		}
		p = *last;
		last = &p->next;
	} /* just add to end */
	*last = r;
	r->next = NULL;
	cpeDbgLog(DBG_TRANSFER,
			"****append req SetTimer for %d******************\n",
			cpeState.dlQ->ud.downloadReq.delaySeconds * 1000);
	setTimer(r->rpcMethod==eScheduleDownload? cwmpStartWindow: startTransfer, NULL,
			cpeState.dlQ->ud.downloadReq.delaySeconds * 1000);
	return;
}
/*
 * dequeue the expired transfer request and start the
 * delay timer for the next transfer request.
 * Returns: RPCRequest *
 * 	    or  NULL
 */
static RPCRequest *dQTransferRequest(void) {
	RPCRequest *r, *p;
	if ((p = cpeState.dlQ)) {
		cpeState.dlQ = p->next;
		if ((r = p->next)) {
			/* start time for next transfer */
			time_t now = time(NULL);
			int when = r->ud.downloadReq.targetTime - now;
			if (when < 0) /* adjust delay by time passed */
				when = 0;
			setTimer(startTransfer, NULL, when * 1000);
			cpeDbgLog(DBG_TRANSFER, "**** dQreq SetTimer for %d******************\n", when*1000);

		}
	}
	return p;
}
/*
 * Dequeue and cancel any waiting transfer requests that match the
 * commandKey. The RPCRequest is freed. Return number of items matching
 * key.
 */
int cwmpCancelWaiting(const char *cKey) {
	int cnt = 0;
	RPCRequest *r, **last;
	last = &cpeState.dlQ;
	r = cpeState.dlQ;
	while (r != NULL) {
		if (streq(r->commandKey, cKey)) {
			*last = r->next;
			RPCRequest *nxt = r->next;
			cwmpFreeRPCRequest(r);
			r = nxt;
			++cnt;
		} else {
			last = &r->next;
			r = r->next;
		}
	}
	return cnt;
}

static void nQActive(RPCRequest *r) {
	r->next = cpeState.dlQActive;
	cpeState.dlQActive = r;
}


void cwmpQueueDLRequest(RPCRequest *r, int activeQ) {
	if (activeQ) {
		/* Request is active, put on active queue */
		nQActive(r);
		return;
	}
	/* not yet active */
	r->DLMsg.dlStatus = -2;
	nQTransferRequest(r);

}
/*
 * Return true if there is a transfer ready to start
 */
static int isTransferReady(void) {
	RPCRequest *r = cpeState.dlQActive;
	return (r != NULL && r->ud.downloadReq.delaySeconds == 0
			&& r->ud.downloadReq.dlStatus == -1);
}

/*
 * This function is called when the Download/UPload
 * does not cause a reboot and when the cpeDownload
 * setup returns a status other than 2. The 2 status,
 * indicates the download is being performed
 * by some other entity than CWMPC. Then the transfer
 * complete is handled by sendPendingRPC.
 */
static void sendXferComplete(RPCRequest *r) {
#ifdef CONFIG_RPCSCHEDULEDOWNLOAD
	if (r->rpcMethod == eScheduleDownload )
		stopTimer(downloadWindowEnded, (void*)r);
#endif
	r->ud.downloadReq.dlEndTime = time(0);
	cwmpAddEvent(r->rpcMethod == eDownload ? eEvtMDownload
			        : r->rpcMethod== eScheduleDownload ? eEvtMScheduleDownload
			        : eEvtMUpload);
	cwmpSetPending(PENDING_XFERCOMPL);
	cwmpAddEvent(eEvtTransferComplete);
	/* don't send inform if the next transfer is ready to start */
	if (isTransferReady()) {
		DBGPRINT((stderr, "Next Transfer is ready\n"));
		return;
	}
	cwmpStartPending();
}

static void dlTransferQuit(ACSSession *s, int fault) {
	RPCRequest *r = s->rpc;
	if (s->wio) {
		wget_Disconnect(s->wio);
		s->wio = NULL;
	}
	if (s->authHdr)
		GS_FREE(s->authHdr);
	s->authHdr = NULL;
	r->ud.downloadReq.dlStatus = fault; /* download status */
	r->ud.downloadReq.dlFaultMsg = cpeGetFaultMsgIndex(fault);
#ifdef CONFIG_RPCUPLOAD
	if (r->rpcMethod == eUpload)
		cpeUploadComplete(&s->rpc->ud.downloadReq, s->postMsg);
#endif
	/* download errors do not call cpeDownloadComplete */
	s->postMsg = NULL;
	sendXferComplete(s->rpc);
	s->eHttpState = eClose;
	return;
}
/*
 * Save the transfer completion status and call the
 * cpeDownloadComplete wrapper function. If it returns an
 * error update the status. The cpeDownloadComplete may not
 * return if it is flashing firmware and rebooting so the
 * state must be saved prior to the call.
 *
 */
static int completeResponse(ACSSession *s, RPCRequest *r, char *buf, int lth){
	int status;
	r->ud.downloadReq.dlEndTime = time(0);
	cwmpSetPending(PENDING_XFERCOMPL);
	cpeState.eventMask |= EVT_DOWNLOAD;
	r->ud.downloadReq.dlStatus = lth>0? CPE_OK: CPE_9010;
	r->ud.downloadReq.dlFaultMsg = cpeGetFaultMsgIndex(r->ud.downloadReq.dlStatus);
	cpeSaveCPEState(); /* may not return so save state */
	status = cpeDownloadComplete(&r->ud.downloadReq, buf, lth);
	if ( status == 0)
		status = CPE_9010;   /* make general file transfer failure */
	else if (status == 1)
	{
		status = CPE_OK;
		while(1)
		{
			pause();
			if (cpeState.sigusr1==1)
			{
				dlTransferQuit(s, status);
				return status;
			}
		}
	}
	dlTransferQuit(s, status);
	return status;
}

static void getXferResponse(void *handle) {
	tWget *w = (tWget *) handle;
	ACSSession *s = (ACSSession*) w->handle;
	RPCRequest *r = s->rpc;

	DBGLOG((DBG_TRANSFER, "getXferResponse() http-status = %d\n", w->hdrs->status_code));
	if (w->status != 0) { /* catch connection failure here */
		cpeLog(LOG_ERR, "Transfer Server Status = %d %s", w->status, w->msg);
		dlTransferQuit(s, CPE_9015);
		return;
	}
	if (w->hdrs->status_code == 401 && s->eAuthState == eIdle) {
		/* authenticate requested by server */
		char *authValue;
		proto_Skip(w->pc);
		DBGLOG((DBG_TRANSFER, "WWW-Authenticate= %s\n", w->hdrs->wwwAuthenticate));
		if (EMPTYSTR(r->ud.downloadReq.userName)
				|| EMPTYSTR(r->ud.downloadReq.passWord)) {
			dlTransferQuit(s, CPE_9012);
			return;
		}
		if (!(authValue = generateAuthorizationHdrValue(&s->sessionAuth,
				w->hdrs->wwwAuthenticate, r->rpcMethod == eDownload
						|| r->rpcMethod == eScheduleDownload ? "GET" : "PUT",
				s->wio->uri, r->ud.downloadReq.userName,
				r->ud.downloadReq.passWord))) {
			cpeLog(LOG_ERR, "WWWAuthenticate header parse error: %s",
					w->hdrs->wwwAuthenticate);
			dlTransferQuit(s, CPE_9010);
		}
		/*
		 * Some servers send multiple packets of line data following the headers. Rather
		 * than read those packets its much simpler to just disconnect and reconnect
		 * with the Authorization header.
		 */
		s->authHdr = authValue; /* save auth header in session */
		wget_Disconnect(s->wio);
		s->wio = wget_Connect(r->ud.downloadReq.URL, xfrConnected, s);
		if (s->wio == NULL) {
			cpeLog(LOG_ERR, "transfer reconnect failed: %s",
					wget_LastErrorMsg());
			dlTransferQuit(s, CPE_9015);
		} else
			s->eAuthState = eAuthenticating;
	} else if (w->hdrs->status_code == 401 && s->eAuthState == eAuthenticating) {
		/* authentication failed */
		s->eAuthState = eAuthFailed;
		cpeLog(LOG_ERR, "upload/download: Server Authentication Failed %d",
				w->hdrs->status_code);
		dlTransferQuit(s, CPE_9012);
	} else if ((r->rpcMethod == eDownload || r->rpcMethod == eScheduleDownload)
			&& (w->hdrs->status_code >= 200 && w->hdrs->status_code <= 205)
			&& ((w->hdrs->content_length > 0) || (w->hdrs->TransferEncoding
					&& streq(w->hdrs->TransferEncoding, "chunked")))) {
		/* readin download data */
		int lth;
		char *buf = NULL;
		int status;//-Werror=unused-but-set-variable
        int count = 0;

		s->eAuthState = eAuthenticated;
		if (s->cpeDLBuf) {
			/* CPE wrapper is providing buffer and data handle functions.*/
            do
            {
                lth = wget_ReadBufResponse(w, s->cpeBufCB, (void *) r, s->cpeDLBuf,
                        s->cpeDLBufLth);
                DBGLOG((DBG_TRANSFER, "Download size into CPE buffer= %d\n", lth));
                status = completeResponse(s, r, buf, lth ); //-Werror=unused-but-set-variable
                if (status == CPE_9010)
                {
                    if (count != 1)
                        DBGLOG((DBG_TRANSFER, "Download failed,retry...\n", lth));

                    count ++ ;
                }
                else
                    break;
            }while (count != 2);
		} else {
			/* Buffer is allocated by CWMPc downloader functions */
			buf = wget_ReadResponse(w, &lth, MAXFILESIZE );
			DBGLOG((DBG_TRANSFER, "Download size = %d\n", lth));
			completeResponse(s, r, buf, lth);
			if ( buf )
				GS_FREE(buf);
		}

	} else if (w->hdrs->status_code >= 200 && w->hdrs->status_code <= 205) {
		if (r->rpcMethod == eDownload || r->rpcMethod == eScheduleDownload) {
			completeResponse(s, r, NULL, 0);
		} else if (r->rpcMethod == eUpload)
			/* Upload completed */
			dlTransferQuit(s, 0);
		else
			/* unclear how this could ever happen - but clean up anyway*/
			dlTransferQuit(s, 0);

	} else if (w->hdrs->status_code >= 100 && w->hdrs->status_code < 200) {
		; /* more data to come, ignore these status codes */
	} else {
		cpeLog(LOG_ERR, "Transfer: Error %d", w->hdrs->status_code);
		if (r->rpcMethod == eDownload || r->rpcMethod == eScheduleDownload)
			dlTransferQuit(s, CPE_9016);
		else
			dlTransferQuit(s, CPE_9011);
	}
	return;
}

static void xfrConnected(void *handle) {
	tWget *w = (tWget *) handle;
	ACSSession *s = (ACSSession*) w->handle;
	if (w->status != 0) {
		DBGLOG((DBG_TRANSFER, "xfrConnected() Failed to connect"));
		dlTransferQuit(s, CPE_9015);
		cpeLog(LOG_ERR, "Transfer Server Connect Status = %d %s", w->status,
				w->msg);
		return;
	} DBGLOG((DBG_TRANSFER, "xfrConnected()"));
	s->eHttpState = eConnected;
	wget_ClearPostHdrs(s->wio);
	if (s->authHdr) {
		wget_AddPostHdr(s->wio, "Authorization", s->authHdr);
		GS_FREE(s->authHdr);
		s->authHdr = NULL;
	}
	if (s->rpc->rpcMethod == eDownload || s->rpc->rpcMethod == eScheduleDownload) {
		/* send http get for download data */
		wget_GetData(s->wio, getXferResponse, (void *) s);
	}
#ifdef CONFIG_RPCUPLOAD
	else {
		/* send upLoad data */
		const char *ctype = s->rpc->ud.downloadReq.content_type;
		if (ctype == NULL)
			ctype = "text/xml";
		if (s->postMsg == NULL /* check if we already have the cpe data, then get it */
				&& cpeGetUploadData(&s->rpc->ud.downloadReq, &s->postMsg, &s->postLth)) {
			if (s->postMsg && s->postLth) {
				wget_PutData(s->wio, s->postMsg, s->postLth, ctype,
						getXferResponse, (void *) s);
				return;
			}
		} else {
			wget_PutData(s->wio, s->postMsg, s->postLth, ctype,
					getXferResponse, (void *) s);
			return;
		}

		/* cpe returned bad status or buffer */
		dlTransferQuit(s, CPE_9001);
		cpeLog(LOG_ERR, "cpeGetUploadData returned bad buffer or length");
	}
#endif /* CONFIG_RPCUPLOAD */
	return;
}

/*
 * download/upload request is queued to cpeState.dlQ.
 * This code is only called when delay timer expires on the first
 * item in the queue.
 * If more than one request is queued it is handled following
 * the completion of the first queued.
 */
static void startTransfer(void *handle) {
	ACSSession *s = &cwmpXfrSession;
	int setupStatus;
	RPCRequest *r;
	DownloadMsg *dr;
	if (s->wio != NULL) {
		 /* download already in progress or pending xfre complete, just return */
		DBGLOG((DBG_TRANSFER, "startTransfer() setPending PENDING_STARTXFER"));
		cwmpSetPending(PENDING_STARTXFER);
		// Set waiting queue item to 0 delay.
		if ( (r= cpeState.dlQ) )
			r->ud.downloadReq.delaySeconds = 0;
		return;
	} else
		cwmpClearPending(PENDING_STARTXFER);

	DBGLOG((DBG_TRANSFER, "startTransfer() clearPending PENDING_STARTXFER"));
	memset(s, 0, sizeof(ACSSession));
	if ((s->rpc = dQTransferRequest())) { /* dequeue DL request and start transfer */
		r = s->rpc;
		dr = &r->ud.downloadReq;
		dr->dlStartTime = time(NULL);
		dr->dlStatus = -1; /* set active status */
		nQActive(r);
		DBGLOG((DBG_TRANSFER, "startTransfer() %s\n", dr->URL ));

		if (r->rpcMethod == eDownload || r->rpcMethod == eScheduleDownload) {

			setupStatus = cpeDownloadSetup(&r->ud.downloadReq);
		}
#ifdef CONFIG_RPCUPLOAD
		else {
			/* eUpload */
			setupStatus = cpeUploadSetup(&r->ud.downloadReq);
		}
#endif
		if (setupStatus == 1) {
			if (!(strstr(dr->URL, "https:") != 0
					|| strstr(dr->URL, "http:") != 0)) {
				dr->dlStatus = CPE_9013;
				dr->dlFaultMsg = FAULT9013;
				sendXferComplete(r);
			}
			if ((s->wio = wget_Connect(dr->URL, xfrConnected, s)) == NULL) {
				if ( r->rpcMethod == eUpload ) {
					cpeLog(LOG_ERR, "Connect to download server %s failed: %s",
							dr->URL, wget_LastErrorMsg());

					dr->dlStatus = CPE_9015; /* Download failure */
					dr->dlFaultMsg = FAULT9015;
				} else {
					cpeLog(LOG_ERR, "Connect to upload server %s failed: %s",
							r->ud.downloadReq.URL, wget_LastErrorMsg());
					dr->dlStatus = CPE_9011; /* Upload load failure */
					dr->dlFaultMsg = FAULT9011;
				}
				sendXferComplete(r);
			} else {
				dr->dlStatus = -1;
				s->eHttpState = eStart;
				s->eAuthState = eIdle;
			}
		} else if (setupStatus == 2) {
			/* non-cwmp transfer is scheduled */
			dr->dlStatus = 2;
			/* if no remaining transfer requests then signal back cpe */
			if (cpeState.dlQ == NULL)
				notifyCallbacks(&cpeState.dlQActive);
		} else if (setupStatus == 0) {
			dr->dlStatus = CPE_9001;
			dr->dlFaultMsg = FAULT9001;
			sendXferComplete(r);
		} else {
			dr->dlStatus = setupStatus;
			setupStatus -= CPE_9000;   // calc fault msg index.
			if ( setupStatus >=FAULT9000 && setupStatus<=FAULTNONE )
				dr->dlFaultMsg = setupStatus;
			else
				dr->dlFaultMsg = FAULTNONE;
			sendXferComplete(r);
		}
	}

	return;
}
/*
 * Optional buffer handler function and buffer information is set by
 * CPE wrapper. Typically called back from the cpeDownloadSetup() function.
 */
void cwmpSetDLBufferHandler(int cb(void *, char *, int), char *cpeBuf,
		int bufLth) {
	ACSSession *s = &cwmpXfrSession;
	s->cpeBufCB = cb;
	s->cpeDLBuf = cpeBuf;
	s->cpeDLBufLth = bufLth;
}

#ifdef CONFIG_RPCSCHEDULEDOWNLOAD

/*
 * The current time window for the scheduled download has expired.
 * Cancel the current window and schedule the next window if specified.
 *
 */
static void downloadWindowEnded(void *handle) {
	RPCRequest *r = (RPCRequest *) handle;
	DBGLOG((DBG_TRANSFER, "downloadWindowEnded"));
	cpeDownloadWindowEnd( &r->ud.downloadReq );
	if (dQWaitingItem(r)) {
		// was on wait queue.
		if ( ++r->ud.downloadReq.timeIndex < MAX_TIMEWINDOWS) {
			if (r->ud.downloadReq.timeWindow[r->ud.downloadReq.timeIndex].windowStart > 0) {
				nQTransferRequest(r); // requeue for next window time.
			}
		} else {
			DBGLOG((DBG_TRANSFER, "downloadWindowEnded: ScheduledDownload failed to complete within windows\n"));
			r->ud.downloadReq.dlStatus = CPE_9020;
			nQActive(r);
			sendXferComplete(r);
		}
	} else {
		// must be an active transfer
		if ( cwmpXfrSession.wio ){
			dlTransferQuit(&cwmpXfrSession, CPE_9020);
			DBGLOG((DBG_TRANSFER, "downloadWidowEnded: stop transfer in progress\n"));
		} else {
			DBGLOG((DBG_TRANSFER, "downloadWidowEnded: no session in progress\n"));
		}
	}
}
/*
 * Called when a WindowStart time is active.
 * The call to cpeDownloadWindowStart() is made to retrieve the action status.
 * eSTARTTRANSFER: start the transfer,
 * eABORT:         terminate this window and any remaining windows and notify the
 *                 ACS with a fault.
 * eSKIPWINDOW:    skip this time window and schedule the next one or terminate
 *                 the attempt and notify the ACS.
 * eWAIT:          The CPE will signal when to resume this time window.
 *                 If the Window end time is reached the next window is scheduled
 *                 or the ACS is notified of a fault.
 *
 */
void cwmpStartWindow(void *handle){
	RPCRequest *r = cpeState.dlQ;
	DBGLOG((DBG_TRANSFER, "startWindow"));
	if ( r ){
		WINDOWACTION action = cpeDownloadWindowStart( &r->ud.downloadReq );
		if (action==eSTARTTRANSFER){
			startTransfer(NULL);
		} else if (action==eABORT){
			dQTransferRequest();
			r->ud.downloadReq.dlStartTime = time(NULL);
			r->ud.downloadReq.dlStatus = CPE_9020; /* set FAULT status */
			sendXferComplete(r);
		} else if (action==eSKIPWINDOW){
			if ( ++r->ud.downloadReq.timeIndex> MAX_TIMEWINDOWS ) {
				dQTransferRequest();
				r->ud.downloadReq.dlStartTime = time(NULL);
				r->ud.downloadReq.dlStatus = CPE_9020; /* set FAULT status */
				sendXferComplete(r);
			} else {
				stopTimer(downloadWindowEnded, (void*)r );
				// dequeue and enqueue request to schedule next time window.
				cwmpDQItem(r, &cpeState.dlQ);
				nQTransferRequest(r);
			}
		} else {
			DBGLOG((DBG_TRANSFER,"startWindow(): waiting for resume from CPE"));
		}
	} else {
		cpeLog( LOG_ERROR, "startWindow(): state error, empty dlQ" );
	}
}
/*
 * Attempt to start time window transfer
 */
void cwmpDownloadWindowResume(void){
	DBGLOG((DBG_TRANSFER, "cwmpDownloadWindowResume()"));
	notifyCallbacks( &cwmpXfrSession );;
}

void cwmpStopDownloadWindowTimers(RPCRequest *r){
	if (r->rpcMethod == eScheduleDownload )
		stopTimer(downloadWindowEnded, (void*)r);
}

#endif /* #ifdef CONFIG_RPCSCHEDULEDOWNLOAD */
#ifdef CONFIG_RPCCANCELTRANSFER
/*
 * Callback associated with &cpeState.dlQ.
 * Initiate any transfer request that have been held pending.
 */
int cwmpIsPending(int);

void cwmpCheckStartTransfer(void *handle){
	if ( cwmpIsPending(PENDING_STARTXFER)) {
		 startTransfer(NULL);
	}
}

#endif /* #ifdef CONFIG_RPCCANCELTRANSFER */
#endif /**** #if defined(CONFIG_RPCUPLOAD || CONFIG_RPCDOWNLOAD) ****/


