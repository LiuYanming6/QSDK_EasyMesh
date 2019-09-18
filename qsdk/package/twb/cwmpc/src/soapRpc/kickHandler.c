/*----------------------------------------------------------------------*
 * Gatespace
 * Copyright 2006 Gatespace Networks, Inc., All Rights Reserved.
 * Gatespace Networks, Inc. confidential material.
 *----------------------------------------------------------------------*
 * File Name  : kickHandler.c
 * Description:	Implementation of Kick RPC
 * This code is substantically similiar to the acsconnreq code. But is
 * maintained seperatly to aid in optional inclusion of this RPC
 * implementation.
 *---------------------------------------------------------------------*
 * $Revision: 1.5 $
 *
 * $Id: kickHandler.c,v 1.5 2009/09/03 15:04:36 dmounday Exp $
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
#include <time.h>
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
#include "../soapRpc/cwmpSession.h"
#include "targetsys.h"

#define DEBUG
#ifdef DEBUG
#define DBGPRINT(X) fprintf X
#else
#define DBGPRINT(X)
#endif


void cpeLog( int level, const char *fmt, ...);

extern CPEState cpeState;
extern EventPolicy eventPolicy[];
static void connReqCallback(void *handle);

typedef enum {
    sIdle,
    sAuthNotReq,
    sWaitingACS,
    sShutdown
} eLanConnState;

typedef struct KickConnection {
    eLanConnState cState; /* authentication state */
    tProtoCtx   *cpc;   /* so we can use wget proto functions */
    int         lfd;    /* listener socket */
    tHttpHdrs   *hdrs;
} KickConnection;

static KickConnection  kickConnection;
static char lanIFName[30];
static InAddr lanIPAddr;
/*
 * used to clean up and reset kick listener
 */

static void statusWriteComplete(void *handle)
{
	DBGPRINT((stderr, "statusWriteComplete()\n"));
    KickConnection *cd = (KickConnection*)handle;
    if (cd->hdrs){
        proto_FreeHttpHdrs(cd->hdrs);
		cd->hdrs = NULL;
	}
    proto_FreeCtx(cd->cpc);
    memset(cd, 0, sizeof(struct KickConnection));
    setTimer(connReqCallback, cd, CONNREQ_RESET_DELAY);
}

static void sendNoReply( void *handle)
{
	KickConnection *cd = (KickConnection*)handle;
    char    response[300];
    int     i;

    DBGPRINT((stderr, "sendNoReply()=\n"));
	if ( cd && cd->cpc ) {
	    i = snprintf(response, sizeof(response), "HTTP/1.1 %s\r\n",
					  "404 Not Found" );
	    i+= snprintf(response+i, sizeof(response)-i, "Content-Length: 0\r\n\r\n");
	    proto_Writen(cd->cpc, response, i);
	   	cd->cState = sShutdown;
		stopListener(cd->cpc->fd);
		setTimer(statusWriteComplete, cd, CONNREQ_RESET_DELAY);
	}
}
/*
 * writeKickRedirect( void *handle )
 */
void kickSendRedirect(const char *nextURL)
{
	char locHdr[1050];
	KickConnection *cd = &kickConnection;
	DBGPRINT((stderr, "kickSendRedirect( %s )\n", nextURL));
	stopTimer(sendNoReply, cd);
	snprintf(locHdr, sizeof(locHdr), "Location: %s", nextURL);
	proto_SendHeaders( cd->cpc, 302, "Redirect", locHdr, "text/html");
	cd->cState = sShutdown;
	stopListener(cd->cpc->fd);
	setTimer(statusWriteComplete, cd, CONNREQ_RESET_DELAY);
}

/**
 * A connected client is sending us data, our action is to setup and
 * ACS session and send a Kicked RPC. And then Redirect the client
 * to the URL in the KickedResponse RPC.

 **/
static void readData(void *handle)
{
    KickConnection *cd = (KickConnection *)handle;


    DBGPRINT((stderr, "LAN readData\n"));
    /* Free resources allocated earlier */
	if ( cpeState.kickNext ) {
		GS_FREE(cpeState.kickNext);
		cpeState.kickNext = NULL;
	}
	if ( cpeState.kickArg ) {
		GS_FREE(cpeState.kickArg);
		cpeState.kickArg = NULL;
	}
	if ( cpeState.kickCommand ) {
		GS_FREE(cpeState.kickCommand);
		cpeState.kickCommand = NULL;
	}
	if ( cpeState.kickReferer ) {
		GS_FREE(cpeState.kickReferer);
		cpeState.kickReferer = NULL;
	}

	if (cd->hdrs)
		proto_FreeHttpHdrs(cd->hdrs);
    cd->hdrs = proto_NewHttpHdrs();
    if ( proto_ParseRequest(cd->cpc, cd->hdrs) == 0 ) {
        proto_ParseHdrs(cd->cpc, cd->hdrs);
        if (strstr(cd->hdrs->path, LANKICK_PATH )) {
        	char *cp = strstr(cd->hdrs->path, "command=");
        	if ( cp ) {
        		char *argp = strstr(cd->hdrs->path, "&arg=");
        		char *nextp = strstr(cd->hdrs->path, "&next=");
        		cp += sizeof("command=")-1;
        		if ( argp ) {
        			cpeState.kickCommand = argp-cp>0? GS_STRNDUP( cp, argp-cp): GS_STRDUP("");
        			argp += sizeof("&arg=")-1;
        			if (nextp) {
        				cpeState.kickArg = nextp-argp>0? GS_STRNDUP(argp, nextp-argp):GS_STRDUP("");
        				nextp += sizeof("&next=")-1;
        				cpeState.kickNext = GS_STRDUP(nextp);
        				cpeState.kickReferer = cd->hdrs->Referer? GS_STRDUP(cd->hdrs->Referer)
        										: GS_STRDUP("");
        			}
        		}
        	}
			cwmpAddEvent(eEvtKicked);
			cwmpSetPending(PENDING_SENDKICKED);
			DBGPRINT((stderr, "notifyCallbacks(eventPolicy Kicked)\n"));
			notifyCallbacks(eventPolicy);
			cd->cState = sWaitingACS;
			setTimer(sendNoReply, cd, 15*1000);  /* set clean-up timer */
			return;
        }
    }
    sendNoReply( cd );  /* if failure then reply not found */
    DBGPRINT((stderr, "LAN Kick Listener Error reading response\n"));
}


/**
 * Something is trying to connect.
 */
static void connectLANClient(void *handle)
{
    int res;
    struct sockaddr_in addr;
    KickConnection *cd = (KickConnection *) handle;
    socklen_t sz = sizeof(struct sockaddr_in);
    int     flags=1;
    int     fd;

    stopListener(cd->lfd);
    memset(&addr, 0, sz);
    if (( fd = accept(cd->lfd, (struct sockaddr *)&addr, &sz)) < 0)
    {
        cpeLog(LOG_ERR, "connectLaNClient accept failed errno=%d.%s",errno, strerror(errno));
        close(cd->lfd);
        setTimer(connReqCallback, cd, 5000 ); /* reenable listen in 5 sec */
        return; /* return errno */
    }
    close(cd->lfd); /* close the listener socket - only one connection at a time */
    cd->lfd = 0;
    cd->cpc = proto_NewCtx(fd);
    if ( (res = setsockopt(cd->cpc->fd, SOL_SOCKET, SO_REUSEADDR, &flags,sizeof(flags)))<0)
        cpeLog(LOG_ERR, "conn req setsockopt error %d %d %s", cd->cpc->fd, errno, strerror(errno));

    setListener(cd->cpc->fd, readData, cd);
}
/*
* return -1: for error
*       != -1 is socket
*/
static int initSocket(InAddr *ip, int port)
{
    SockAddrStorage sa;
    struct sockaddr_in *sp = (struct sockaddr_in *)&sa;

    int port_sock = 0;
    int res, i = 1;


    memset(sp, 0, sizeof(SockAddrStorage));
    SET_SockADDR(sp, htons(port), ip);
    port_sock = socket(sp->sin_family, SOCK_STREAM, IPPROTO_TCP);
    if (port_sock < 0)
    {
        cpeLog(LOG_ERR, "kickHandler: init_listen_socket(port=%d), socket failed", port);
        return -1;
    }

    res = setsockopt(port_sock, SOL_SOCKET, SO_REUSEADDR, (char*) &i, sizeof(i));
    if (res < 0)
    {
        cpeLog(LOG_ERR,"kickHandler: %s", "Socket error");
        close(port_sock);
        return -1;
    }
    res = bind(port_sock, (struct sockaddr *)sp, SockADDRSZ(sp));
    if (res < 0)
    {
        cpeLog(LOG_ERR, "kickHandler bind failed errno=%d.%s",errno, strerror(errno));
        close(port_sock);
        return -1;
    }
    res = listen(port_sock,1);
    if (res < 0)
    {
        cpeLog(LOG_ERR, "kickHandler listen failed errno=%d.%s",errno, strerror(errno));
        close(port_sock);
        return -1;
    }
    return port_sock;
}

static void connReqCallback(void *handle) {
    KickConnection *cd = (KickConnection *)handle;
	if (cd->lfd)
		close(cd->lfd);
    if( (cd->lfd =initSocket(&lanIPAddr, LANKICK_PORT))==-1)
        setTimer(connReqCallback, cd, 5000 ); /* retry init_socket in 5 sec */
    else {
    	char kickURL[256];
    	snprintf(kickURL, sizeof(kickURL), "http://%s:%d%s",
    	  	writeInIPAddr(&lanIPAddr), LANKICK_PORT, LANKICK_PATH);
        cpeState.kickURL = GS_STRDUP(kickURL);
        setListener(cd->lfd, connectLANClient, cd );
    }
}

/*
* Start to listen on Kicked URL
*/
void cwmpStartKickListener(const char *lanIF, InAddr *lanIP)
{
    memset(&kickConnection,0, sizeof(struct KickConnection));
    strncpy( lanIFName, lanIF, sizeof(lanIFName));
    lanIPAddr = *lanIP;
    connReqCallback(&kickConnection);

}

