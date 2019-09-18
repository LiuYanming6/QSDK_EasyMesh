
/*----------------------------------------------------------------------*
 * Gatespace
 * Copyright 2006 Gatespace Networks, Inc., All Rights Reserved.
 * Gatespace Networks, Inc. confidential material.
 *----------------------------------------------------------------------*
 * File Name  : acsconnreq.c
 * Description:	Implementation of ACS connection request listener
 *----------------------------------------------------------------------*
 * $Revision: 1.12 $
 *
 * $Id: acsconnreq.c,v 1.12 2011/05/10 14:12:31 dmounday Exp $
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
#include "../includes/CPEWrapper.h"


#define DEBUG


void cpeLog( int level, const char *fmt, ...);

extern CPEState cpeState;

static void connReqCallback(void *handle);

typedef enum {
	sInitializing,
    sIdle,
    sAuthenticating,
    sAuthNotReq,
    sAuthFailed,
    sShutdown
} eACSConnState;

typedef struct ACSConnection {
    eACSConnState cState; /* authentication state */
    tProtoCtx   *cpc;   /* so we can use wget proto functions */
    int         lfd;    /* listener socket */
    tHttpHdrs   *hdrs;
    InAddr		ipAddr;  /* IP address to bind listener to */
} ACSConnection;

static ACSConnection  acsConnection;
static SessionAuth acsConnSess;


static int testChallenge( ACSConnection *cd)
{
    return ( parseAuthorizationHdr(cd->hdrs->Authorization, &acsConnSess,
                                    cpeState.connReqUser, cpeState.connReqPW));
}

static void statusWriteComplete(void *handle)
{
    ACSConnection *cd = (ACSConnection*)handle;
    if (cd->hdrs){
        proto_FreeHttpHdrs(cd->hdrs);
		cd->hdrs = NULL;
	}
    proto_FreeCtx(cd->cpc);
    memset(cd, 0, sizeof(struct ACSConnection));
    setTimer(connReqCallback, cd, CONNREQ_RESET_DELAY);
}

static int sendReply( ACSConnection *cd, int status )
{
    char    response[300];
    int     i;

    i = snprintf(response, sizeof(response), "HTTP/1.1 %s\r\n",
				  status==200?"200 OK": "401 Unauthorized" );
    i+= snprintf(response+i, sizeof(response)-i, "Content-Length: 0\r\n\r\n");
    if ( proto_Writen(cd->cpc, response, i) < i)
        return 0;
    return 1;
}

static int sendChallenge( ACSConnection *cd )
{
    char    response[300];
    char    *wwwHdr;
    int     i;

    i = snprintf(response, sizeof(response), "HTTP/1.1 401 Unauthorized\r\n");
    i+= snprintf(response+i, sizeof(response)-i, "Content-Length: 0\r\n");
    if ( (wwwHdr = generateWWWAuthenticateHdr( &acsConnSess, ACSAUTHREALM, ACSDOMAIN, cd->hdrs->method))){
		i+= snprintf(response+i, sizeof(response)-i,"%s\r\n\r\n", wwwHdr);
		GS_FREE(wwwHdr);
    }
    if ( proto_Writen(cd->cpc, response, i) < i)
        return 0;
    return 1;
}
/**** unused *****/
#if 0
static void trimPath(char *p)
{
	if ( p == NULL)
		return;
	char *c = p;
	while ( *c ){
		if ( *c == '?'){
			/* trim any arguments */
			*c = '\0';
			break;
		}
		++c;
	}
	if (*(c-1) == '/')
		*(c-1) = '\0';
	return;
}
#endif

/**
 * A connected ACS is sending us data,
 * Our action is to generate a digest authentication challange
 * with a 401 Unauthorized status code and
 * wait for the response to the challange. Then  send a
 * 200 OK or a 401 Unauthorized. */
static void acsReadData(void *handle)
{
    ACSConnection *cd = (ACSConnection *)handle;

#ifdef DEBUG
    cpeDbgLog(DBG_ACSCONNECT, "acsReadData\n");
#endif

    /* Free resources allocated earlier */
	if (cd->hdrs)
		proto_FreeHttpHdrs(cd->hdrs);
    cd->hdrs = proto_NewHttpHdrs();
    if (cd->hdrs == NULL )
    	cpeLog(LOG_ERR, "Connection Request failed: exhausted memory");
    if ( proto_ParseRequest(cd->cpc, cd->hdrs) == 0 ){
        proto_ParseHdrs(cd->cpc, cd->hdrs);
        if ( streq(cpeState.connReqPath, cd->hdrs->path) )
			{
                /* path is correct proceed with authorization */
				if (cpeState.connReqUser==NULL || cpeState.connReqUser[0]=='\0'){
					/* authentication not required */
					sendReply(cd, 200);
					notifyCallbacks(&cpeState);  /* this will start a inform rpc */
					cd->cState = sShutdown;
					setListenerType(cd->cpc->fd, statusWriteComplete,cd, iListener_Write);
				} else if (cd->cState == sIdle) {
                    /* send 401 with digest challenge */
                    sendChallenge( cd );
                    cd->cState = sAuthenticating;
                    setListener(cd->cpc->fd, acsReadData, (void *)cd);
                    return;
                } else if (cd->cState == sAuthenticating) {
                    if ( testChallenge( cd ) ) {
                        sendReply(cd, 200);
						notifyCallbacks(&cpeState);  /* this will start a inform rpc */
                        cd->cState = sShutdown;
                    }
                    else {
						cpeLog(LOG_DEBUG, "ConnectRequest authentication error");
                        sendReply(cd, 401);
                        cd->cState = sShutdown;
                    }
                    setListenerType(cd->cpc->fd, statusWriteComplete,cd, iListener_Write);
                } else {
					cd->cState = sShutdown;
					stopListener(cd->cpc->fd);
					setTimer(statusWriteComplete, cd, CONNREQ_RESET_DELAY);
				}
            } else {
				cd->cState = sShutdown;
				stopListener(cd->cpc->fd);
				setTimer(statusWriteComplete, cd, CONNREQ_RESET_DELAY);
			}
    } else {
        #ifdef DEBUG
        cpeDbgLog(DBG_ACSCONNECT, "acsListener Error reading response\n");
        #endif
		cd->cState = sShutdown;
		stopListener(cd->cpc->fd);
		setTimer(statusWriteComplete, cd, CONNREQ_RESET_DELAY);
    }
}


/**
 * Something is trying to connect.
 */
static void connectACS(void *handle)
{
    int res;
    struct sockaddr_in addr;
    ACSConnection *cd = (ACSConnection *) handle;
    socklen_t sz = sizeof(struct sockaddr_in);
    int     flags=1;
    int     fd;

    stopListener(cd->lfd);
    memset(&addr, 0, sz);
    if (( fd = accept(cd->lfd, (struct sockaddr *)&addr, &sz)) < 0)
    {
        cpeLog(LOG_ERR, "connectACS accept failed errno=%d.%s",errno, strerror(errno));
        close(cd->lfd);
        setTimer(connReqCallback, cd, 5000 ); /* reenable listen in 5 sec */
        return; /* return errno */
    }
    close(cd->lfd); /* close the listener socket - only one connection at a time */
    cd->lfd = 0;
    cd->cpc = proto_NewCtx(fd);
    if ( (res = setsockopt(cd->cpc->fd, SOL_SOCKET, SO_REUSEADDR, &flags,sizeof(flags)))<0)
        cpeLog(LOG_ERR, "conn req setsockopt error %d %d %s", cd->cpc->fd, errno, strerror(errno));

    setListener(cd->cpc->fd, acsReadData, cd);
}
/*
* return -1: for error
*       != -1 is socket
*/
static int initSocket(ACSConnection *cp, int port)
{
    SockAddrStorage sa;
    struct sockaddr_in *sp = (struct sockaddr_in *)&sa;

    int port_sock = 0;
    int res, i = 1;

    memset(sp, 0, sizeof(SockAddrStorage));
    SET_SockADDR(sp, htons(port), &cp->ipAddr);

    port_sock = socket(sp->sin_family, SOCK_STREAM, IPPROTO_TCP);
    if (port_sock < 0)
    {
        cpeLog(LOG_ERR, "ConnReq: init_socket(port=%d), socket failed: %s", port, strerror(errno));
        return -1;
    }

    res = setsockopt(port_sock, SOL_SOCKET, SO_REUSEADDR, (char*) &i, sizeof(i));
    if (res < 0)
    {
        cpeLog(LOG_ERR, "ConnReq: Socket error initializing");
        close(port_sock);
        return -1;
    }

    res = bind(port_sock, (struct sockaddr *)sp, SockADDRSZ(sp));
    if (res < 0)
    {
        cpeLog(LOG_ERR, "ConnReq: bind failed errno=%d.%s",errno, strerror(errno));
        close(port_sock);
        return -1;
    }

    res = listen(port_sock,1);
    if (res < 0)
    {
        cpeLog(LOG_ERR, "ConnReq: listen failed errno=%d.%s",errno, strerror(errno));
        close(port_sock);
        return -1;
    }
    return port_sock;
}

static void connReqCallback(void *handle) {
    ACSConnection *cd = (ACSConnection *)handle;
	if (cd->lfd) {
		close(cd->lfd);
	    stopListener(cd->lfd);
	}
	/* The cpeState.ipAddress is the address used to create the .ConnectionRequestURL. */
	cd->ipAddr = cpeState.ipAddress;
	cd->cState = sIdle;	
    if( (cd->lfd =initSocket( cd, ACSCONNREQ_PORT))==-1)
        setTimer(connReqCallback, cd, 5000 ); /* retry init_socket in 5 sec */
    else {
        setListener(cd->lfd, connectACS, cd );
    }
}


/*
* Setup the callback to start the ACS connection request
* listener when the first ACS session completes. At this time the
* network address family is known.
*/
void cwmpStartACSConnReqListener(void)
{
    memset(&acsConnection,0, sizeof(struct ACSConnection));
    acsConnection.cState = sInitializing;
    setCallback(&cpeState.ipAddress, connReqCallback, &acsConnection );
}
