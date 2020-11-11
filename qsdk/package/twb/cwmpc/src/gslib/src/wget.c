
/*----------------------------------------------------------------------*
 * Gatespace
 * Copyright 2006 Gatespace Networks, Inc., All Rights Reserved.
 * Gatespace Networks, Inc. confidential material.
 *----------------------------------------------------------------------*
 * File Name  : wget.h
 * Description:	http get/post implementation
 *----------------------------------------------------------------------*
 * $Revision: 1.15 $
 *
 * $Id: wget.c,v 1.15 2012/05/10 17:37:58 dmounday Exp $
 *----------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <signal.h>
#include <ctype.h>
#include <syslog.h>

#ifdef DMALLOC
#include "dmalloc.h"
#endif
#ifdef USE_SSL
#include <openssl/ssl.h>
#endif
#include "utils.h"
#include "event.h"
#include "protocol.h"
#include "www.h"
#include "wget.h"


void cpeLog(int level, const char *fmt, ...);
int dns_lookup(const char *name, int sockType, int family, InAddr *res);
int dns_lookup2(const char *name, int sockType,  InAddr *);
int dns_lookup_auto(const char *name, int sockType, unsigned int port, int family, InAddr *);
int dns_get_next_ip( const char *name, InAddr *res);

static void do_resolve(void *handle);
#define BUF_SIZE 1024
//#define DEBUG

/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
#ifdef  DEBUG
        #define mkstr(S) # S
        #define setListener(A, B, C) {fprintf(stderr, mkstr(%s %s:%d setListener B fd=%d\n), getticks(), __FILE__, __LINE__, A);\
        setListener( A, B, C);}

        #define setListenerType(A, B, C, E) {fprintf(stderr, mkstr(%s %s:%d setListenerType B-E fd=%d\n), getticks(), __FILE__, __LINE__, A);\
        setListenerType( A, B, C, E);}

        #define stopListener(A) {fprintf(stderr, "%s %s:%d stopListener fd=%d\n", getticks(), __FILE__, __LINE__, A);\
        stopListener( A );}

        static char timestr[40];
        static char *getticks() {
            struct timeval now;
            gettimeofday( &now, NULL);
            sprintf(timestr, "%04ld.%06ld", now.tv_sec%1000, now.tv_usec);
            return timestr;
        }
#endif
#ifdef DEBUG
#define DBGPRINT(X) fprintf X
#else
#define DBGPRINT(X)
#endif

/*<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

/*----------------------------------------------------------------------*
 * forwards
 *----------------------------------------------------------------------*/
static void timer_connect(void *p);
static void do_connect(void *p);
static void timer_response(void *p);
static void do_response(void *p);
#ifdef USE_SSL
static void do_chk_cert_send_request(void *p, int errorcode);
#endif
static void do_send_request(void *p, int errorcode);
static const char noHostConnectMsg[] = "Could not establish connection to host %s(%s):%d";
static const char noHostResolve[] = "Could not resolve host %s";

/*
 * local variables
 */
static char userAgent[256]="GatespaceUA";
static char lastErrorMsg[256];
static int  serverTimeout = 60*2000;  /* default to 120 seconds */

static void freeHdrs( XtraPostHdr **p ) {
    XtraPostHdr *next = *p;
    while( next ){
        XtraPostHdr *temp;
        temp = next->next;
        GS_FREE(next->hdr);
        GS_FREE(next->value);
        GS_FREE(next);
        next = temp;
    }
}

void wget_freeCookies( CookieHdr **p ) {
    CookieHdr *next = *p;
    while( next ){
        CookieHdr *temp;
        temp = next->next;
        GS_FREE(next->name);
        GS_FREE(next->value);
        GS_FREE(next);
        next = temp;
    }
}
/*----------------------------------------------------------------------*/
static void freeData(tWgetInternal *wg) {
    if (wg != NULL) {
        if (wg->pc != NULL) {
            proto_FreeCtx(wg->pc);
            wg->pc = NULL;
        }
        freeHdrs( &wg->xtraPostHdrs );
        wget_freeCookies( &wg->cookieHdrs );
        if (wg->hdrs != NULL)
            proto_FreeHttpHdrs(wg->hdrs);
        GS_FREE(wg);
    }
}


typedef enum {
    REPLACE,
    ADDTOLIST
} eHdrOp;

static int addCookieHdr( CookieHdr **hdrQp, char *cookieName, char *value, eHdrOp replaceDups) {
    CookieHdr *xh;
    CookieHdr **p = hdrQp;
    xh= (CookieHdr *)GS_MALLOC( sizeof(struct CookieHdr));
    if (xh) {
    	memset(xh, 0, sizeof(struct CookieHdr));
        xh->name = GS_STRDUP(cookieName);
        xh->value = GS_STRDUP(value);
        if ( xh->name == NULL || xh->value==NULL ){
        	GS_FREE(xh->name);
        	GS_FREE(xh->value);
        	GS_FREE(xh);
        	return 0;
        }
        if (replaceDups == REPLACE) {
            while (*p) {
                CookieHdr *xp = *p;
                if ( strcasecmp(xp->name, xh->name)==0) {
                    /* replace header */
                    xh->next = xp->next;
                    GS_FREE(xp->name);
                    GS_FREE(xp->value);
                    GS_FREE(xp);
                    *p = xh;
                    return 1;
                }
                p = &xp->next;
            }
        }
        /* just stick it at beginning of list */
        xh->next = *hdrQp;
        *hdrQp = xh;
        return 1;
    }
    return 0;
}

static int addPostHdr( XtraPostHdr **hdrQp, char *xhdrname, char *value, eHdrOp replaceDups) {
    XtraPostHdr *xh;
    XtraPostHdr **p = hdrQp;
    xh= (XtraPostHdr *)GS_MALLOC( sizeof(struct XtraPostHdr));
    if (xh) {
        memset(xh, 0, sizeof(struct XtraPostHdr));
        xh->hdr = GS_STRDUP(xhdrname);
        xh->value = GS_STRDUP(value);
        if ( xh->hdr==NULL || xh->value==NULL ){
        	GS_FREE (xh->hdr);
        	GS_FREE (xh->value);
        	GS_FREE (xh);
        	return 0;
        }
        if (replaceDups == REPLACE) {
            while (*p) {
                XtraPostHdr *xp = *p;
                if ( strcmp(xp->hdr, xh->hdr)==0) {
                    /* replace header */
                    xh->next = xp->next;
                    GS_FREE(xp->hdr);
                    GS_FREE(xp->value);
                    GS_FREE(xp);
                    *p = xh;
                    return 1;
                }
                p = &xp->next;
            }
        }
        /* just stick it at beginning of list */
        xh->next = *hdrQp;
        *hdrQp = xh;
        return 1;
    }
    return 0;
}
/*----------------------------------------------------------------------*/
static void report_status(tWgetInternal *data, tWgetStatus status,
const char *msg) {
    tWget wg;
    /* internal error, call callback */
    wg.status = status;
    wg.pc = data->pc;
    wg.hdrs = data->hdrs;
    wg.msg = msg;
    wg.handle = data->handle;
    data->cbActive = 1;
    (*data->cb)(&wg);
    data->cbActive = 0;
    if (data->keepConnection==eCloseConnection)
        freeData(data);
}

/*----------------------------------------------------------------------*
 * returns
 *   0 if ok
 *  -1 if WAN interface is not active
 */
static int send_get_request(tWgetInternal *p, const char *host, int port, const char *uri) {
    tProtoCtx   *pc = p->pc;
    XtraPostHdr *next;
    char buf[HOSTNAME_SZ+10];

    proto_SendRequest(pc, "GET", uri);
    if ( strchr(host, ':')  ){
    	/* simple test if host is an IPv6 address string */
    	snprintf(buf, sizeof(buf), "[%s]:%d", host, port);
    } else {
    	snprintf(buf, sizeof(buf), "%s:%d", host, port);
    }
    proto_SendHeader(pc,  "Host", host);
    proto_SendHeader(pc,  "User-Agent", userAgent);
    if (p->keepConnection==eCloseConnection)
        proto_SendHeader(pc,  "Connection", "close");
    else
        proto_SendHeader(pc,  "Connection", "keep-alive");

    next = p->xtraPostHdrs;
    while (next) {
        proto_SendHeader(pc, next->hdr, next->value);
        next = next->next;
    }
    proto_SendEndHeaders(pc);

    return 0;
}

/*----------------------------------------------------------------------*
 * returns
 *   0 if ok
 *  -1 if WAN interface is not active
 *  arg_keys is a NULL terminated array of (char *)
 *  arg_values is a NULL terminated array of (char *) of same length as arg_keys
 */
static int send_post_request(tWgetInternal *p, tRequest putPost, const char *host, int port, const char *uri,
const char *data, int datalen, const char *content_type) {
    tProtoCtx   *pc = p->pc;
    char buf[HOSTNAME_SZ+10];
    XtraPostHdr *next;
    CookieHdr	*cookie;

    proto_SendRequest(pc, putPost== ePostData? "POST":  "PUT", uri);
    if ( strchr(host, ':') ){
    	/* simple test if host is an IPv6 address string */
    	snprintf(buf, sizeof(buf), "[%s]:%d", host, port);
    } else {
    	snprintf(buf, sizeof(buf), "%s:%d", host, port);
    }
    proto_SendHeader(pc,  "Host", buf);
    proto_SendHeader(pc,  "User-Agent", userAgent);
    if (p->keepConnection==eCloseConnection)
        proto_SendHeader(pc,  "Connection", "close");
    else
        proto_SendHeader(pc,  "Connection", "keep-alive");
    next = p->xtraPostHdrs;
    while (next) {
        proto_SendHeader(pc, next->hdr, next->value);
        next = next->next;
    }
    cookie = p->cookieHdrs;
    while(cookie) {
        proto_SendCookie(pc, cookie);
        cookie = cookie->next;
    }
    if ( content_type )
    	proto_SendHeader(pc,  "Content-Type", content_type);
    snprintf(buf, sizeof(buf), "%d", datalen);
    proto_SendHeader(pc,  "Content-Length", buf);

    proto_SendEndHeaders(pc);
    if (data && datalen)
        proto_SendRaw(pc, data, datalen);

    return 0;
}

/*----------------------------------------------------------------------
 * connect timeout
 */
static void timer_connect(void *p) {
    tWgetInternal *data = (tWgetInternal *) p;
    char buf[256];
    InAddr nxtIp;
    stopListener(data->pc->fd);
    /* try next IP if any */
    if ( dns_get_next_ip(data->host, &nxtIp) ) {
    	data->host_addr = nxtIp;
    	close( data->pc->fd);
    	data->pc->fd = 0;
    	do_resolve( (void*) data);
    	return;
    }
    snprintf(buf, sizeof(buf), "Connection timed out to host %s:%d", data->host, data->port);
    report_status(data, iWgetStatus_ConnectionError, buf);
}

/*----------------------------------------------------------------------*/
static void timer_response(void *p) {
    tWgetInternal *data = (tWgetInternal *) p;
    char buf[512];
    stopListener(data->pc->fd);
    snprintf(buf, sizeof(buf), "Host (%s:%d) is not responding, timeout", data->host, data->port);
    report_status(data, iWgetStatus_ConnectionError, buf);
}

/*----------------------------------------------------------------------*/
static void do_connect(void *p) {
    tWgetInternal *data = (tWgetInternal *) p;
    int err;
    u_int n;

    stopTimer(timer_connect, data);
    stopListener(data->pc->fd);

    /* check fd status */
    n = sizeof(int);
    if (getsockopt(data->pc->fd, SOL_SOCKET, SO_ERROR, &err, &n) < 0) {
        report_status(data, iWgetStatus_InternalError,
        "internal error: do_connect(): getsockopt failed");

        return;
    }

    if (err != 0) {
        /* connection not established */
        char buf[512];

        snprintf(buf, sizeof(buf), "Connection to host %s(%s):%d failed %d (%s)",
        		data->host, writeInIPAddr(&data->host_addr), data->port,
        err, strerror(err));
        report_status(data, iWgetStatus_ConnectionError, buf);
        return;
    }
    /* return at this point if function is connect only */
    if(data->request==eConnect){
        report_status(data, iWgetStatus_Ok, NULL);
        return;
    }

#ifdef USE_SSL
    /* init ssl if proto is https */
    if (strcmp(data->proto, "https") == 0) {
        proto_SetSslCtx(data->pc, do_chk_cert_send_request, data);
    } else {
        do_send_request(data, PROTO_OK);
    }
#else
    do_send_request(data, PROTO_OK);
#endif
}

#ifdef USE_SSL
/*
 * called immediately following a connect to an https server to check
 * the certificate.
 */
/*----------------------------------------------------------------------*/
static void do_chk_cert_send_request( void *p, int errorcode){
    tWgetInternal *data = (tWgetInternal *) p;
#ifdef DEBUG
    fprintf(stderr, "do_chk_cert_send_request\n");
#endif
    if (errorcode < 0) {
        report_status(data, iWgetStatus_ConnectionError, "Failed to establish SSL connection");
        return;
    }
	if (!proto_CheckCertificate( data->pc, data->host)){
		report_status(data, iWgetStatus_ConnectionError, "Certificate miss-match");
        return;
	}
	do_send_request(p, errorcode);
	return;
}
#endif

/*----------------------------------------------------------------------*/
static void do_send_request(void *p, int errorcode) {
    tWgetInternal *data = (tWgetInternal *) p;
    int res;
#ifdef DEBUG
    fprintf(stderr, "do_send_request keepConn=%d status=%d\n", data->keepConnection, data->status);
#endif
    if (errorcode < 0) {
        report_status(data, iWgetStatus_ConnectionError, "Failed to establish SSL connection");
        return;
    }

    /* send request */
    if (data->request == eGetData) {
        res = send_get_request(p, data->host, data->port, data->uri);
    } else { /* ePostData or ePutData */
        res = send_post_request(p, data->request, data->host, data->port, data->uri, data->postdata,
        data->datalen, data->content_type);
    }

    if (res < 0) {
        report_status(data, iWgetStatus_ConnectionError, "Failed to send request on connection");
        return;
    }

    /* wait for response */
    setListener(data->pc->fd, do_response, data);
    setTimer(timer_response, data, serverTimeout); /*  */
    return;
}

    /*----------------------------------------------------------------------*/
static void do_response(void *p) {
    CookieHdr	*cp;
    tWgetInternal *data = (tWgetInternal *) p;
    #ifdef DEBUG
        cpeLog(LOG_DEBUG, "do_response data->pc=%p", data->pc);
    #endif
    stopTimer(timer_response, data);

    if (data->pc == NULL) {
        cpeLog(LOG_ERROR, "wget %s", "Internal Error");
        report_status(data, iWgetStatus_InternalError, "internal error: no filedescriptor");
        return;
    }
    if (data->pc->fd <= 0) {
        report_status(data, iWgetStatus_InternalError, "internal error: no filedescriptor");
        return;
    }
    if (data->hdrs)
        proto_FreeHttpHdrs(data->hdrs);
    data->hdrs = proto_NewHttpHdrs();
    if (data->hdrs == NULL) {
        /* memory exhausted?!? */
        cpeLog(LOG_ERROR, "wget %s", "Memory exhausted");
        report_status(data, iWgetStatus_InternalError, "internal error: memory exhausted");
                return;
    }

    if (proto_ParseResponse(data->pc, data->hdrs) < 0) {
        report_status(data, iWgetStatus_Error,
        		"error: illegal http response or read failure");
        return;
    }

    proto_ParseHdrs(data->pc, data->hdrs);
    cp = data->hdrs->setCookies;
    while (cp) { /* save new cookies if present*/
        addCookieHdr( &data->cookieHdrs, cp->name, cp->value, REPLACE );
        cp = cp->next;
    }

    if (data->hdrs->status_code >= 100 && data->hdrs->status_code < 600 ) {
        report_status(data, iWgetStatus_Ok, NULL);
    } else {
        char buf[512];
        snprintf(buf, sizeof(buf), "Host %s returned error \"%s\"(%d)",
        		data->host, data->hdrs->message? data->hdrs->message: "", data->hdrs->status_code);
        report_status(data, iWgetStatus_HttpError, buf);
    }
}

/*----------------------------------------------------------------------*/
static void do_resolve(void *handle) {
    tWgetInternal *data = handle;
    int res, fd;
    char	buf[512];

    stopTimer(timer_connect, data);
    /* if callback does not exist, this is a non-op */
    stopCallback(&(data->host_addr), do_resolve, data);

    if (data->host_addr.inFamily==0) {
        snprintf(buf, sizeof(buf), noHostResolve, data->host);
		report_status(data, iWgetStatus_ConnectionError, buf);
    } else if ((res = www_EstablishConnection(&data->host_addr, data->port, &fd)) < 0) {
        if (res == -1) {
            report_status(data, iWgetStatus_InternalError, "Socket creation error");
        } else {
            snprintf(buf, sizeof(buf), noHostConnectMsg, data->host,
            		writeInIPAddr(&data->host_addr), data->port);
            report_status(data, iWgetStatus_ConnectionError, buf);
        }
    } else {
        data->pc = proto_NewCtx(fd);
	    setTimer(timer_connect, data, serverTimeout); /* response timeout is 60 sec */
        setListenerType(fd, do_connect, data, iListener_Write);
    }
}

/*
 * Connect to the specified url
 * Returns: NULL  failed allocate memory or immediate connection error.
 * 			     Call wget_LastErrorMsg() to retrieve last error msg.
 *         pointer to Connection descriptor tWgetInternal.
 */
tWgetInternal *wget_Connect(const char *url, EventHandler callback, void *handle) {
    tWgetInternal *wg;
    int port;

#ifdef DEBUG
        cpeLog(LOG_DEBUG, "wget_Connect(\"%s\", ...)", url);
#endif
	if ( (wg = (tWgetInternal*) GS_MALLOC(sizeof(tWgetInternal)))==NULL)
	    return NULL;
	memset(wg, 0, sizeof(tWgetInternal));
	lastErrorMsg[0] = '\0';
	wg->request = eConnect;
	wg->keepConnection = eKeepConnectionOpen;
	if (www_ParseUrl(url, wg->proto, wg->host, &port, wg->uri) < 0) {
	    wg->status = -5;
	    return wg;
	}

	if (port == 0) {
	    if (strcmp(wg->proto, "http") == 0) {
	        port = 80;
	    #ifdef USE_SSL
	    } else if (strcmp(wg->proto, "https") == 0) {
	        port = 443;
	    #endif
	    } else {
	        cpeLog(LOG_ERROR, "unsupported protocol in url \"%s\"", wg->proto);
	        port = 80; /* guess http and port 80 */
	    }
	}

	wg->pc = NULL;
	wg->cb = callback;
	wg->handle = handle;
	clearInIPAddr( &wg->host_addr);
	wg->port = port;
	if (strlen(wg->uri)== 0)
	    strcpy(wg->uri, "/");
	//if (dns_lookup_auto(wg->host, SOCK_STREAM, htons(wg->port), AF_UNSPEC, &wg->host_addr)) {
	//if (dns_lookup(wg->host, SOCK_STREAM, htons(wg->port), AF_UNSPEC, &wg->host_addr)) {
    if (dns_lookup2(wg->host, SOCK_STREAM,  &wg->host_addr)) {
		cpeLog(LOG_DEBUG, "Host IP address: %s", writeInIPAddr(&wg->host_addr));
	    /* immediate return requires special handling. */
	    int res; int fd;
	    if (wg->host_addr.inFamily == 0) {
	        snprintf(lastErrorMsg, sizeof(lastErrorMsg), noHostResolve, wg->host);
	        freeData(wg);
	        wg=NULL;
	    } else if ((res = www_EstablishConnection( &wg->host_addr, wg->port, &fd)) < 0) {
	        if (res == -1)
	            strncpy(lastErrorMsg, "Socket creation error", sizeof(lastErrorMsg));
	        else
	            snprintf(lastErrorMsg, sizeof(lastErrorMsg), noHostConnectMsg,
	            		wg->host, writeInIPAddr(&wg->host_addr), wg->port);
	        freeData(wg);
	        wg=NULL;
	    } else { /* connect started */
	        wg->pc = proto_NewCtx(fd);
		    setTimer(timer_connect, wg, serverTimeout); /* response timeout is 60 sec */
	        setListenerType(fd, do_connect, wg, iListener_Write);
	    }
	} else {
	    setTimer(timer_connect, wg, serverTimeout); /* response timeout is 60 sec */
	    setCallback(&(wg->host_addr), do_resolve, wg);
	}

	return wg;
}

int wget_GetData(tWgetInternal *wg, EventHandler callback, void *handle) {
    wg->request = eGetData;
    wg->handle = handle;
    wg->cb = callback;
    if (wg->hdrs) {
        wg->hdrs->status_code = 0; /* reset status_code */
        wg->hdrs->content_length = 0;
    }

#ifdef USE_SSL
    /* init ssl if proto is https */
    if ( (strcmp(wg->proto, "https") == 0) && wg->pc->ssl==NULL) {
        proto_SetSslCtx(wg->pc, do_chk_cert_send_request, wg);
    } else {
        do_send_request(wg, PROTO_OK);
    }
#else
    do_send_request(wg, PROTO_OK);
#endif
    return 0;
}

int wget_PostData(tWgetInternal *wg, char *postdata, int datalen, const char *content_type, EventHandler callback, void *handle) {
    wg->request = ePostData;
    wg->content_type = content_type;
    wg->postdata = postdata;
    wg->datalen = datalen;
    wg->handle = handle;
    wg->cb = callback;
    if (wg->hdrs) {
        wg->hdrs->status_code = 0; /* reset status_code */
        wg->hdrs->content_length = 0;
    }

#ifdef USE_SSL
    /* init ssl if proto is https */
    if ( (strcmp(wg->proto, "https") == 0) && wg->pc->ssl==NULL) {
        proto_SetSslCtx(wg->pc, do_chk_cert_send_request, wg);
    } else {
        do_send_request(wg, PROTO_OK);
    }
#else
    do_send_request(wg, PROTO_OK);
#endif
    return 0;
}

int wget_PostDataClose(tWgetInternal *wg, char *postdata, int datalen, const char *content_type, EventHandler callback, void *handle) {
    wg->request = ePostData;
    wg->content_type = content_type;
    wg->postdata = postdata;
    wg->datalen = datalen;
    wg->handle = handle;
    wg->cb = callback;
    if (wg->hdrs) {
        wg->hdrs->status_code = 0; /* reset status_code */
        wg->hdrs->content_length = 0;
    }
    wg->keepConnection = eCloseConnection;
#ifdef USE_SSL
    /* init ssl if proto is https */
    if ( (strcmp(wg->proto, "https") == 0) && wg->pc->ssl==NULL) {
        proto_SetSslCtx(wg->pc, do_chk_cert_send_request, wg);
    } else {
        do_send_request(wg, PROTO_OK);
    }
#else
    do_send_request(wg, PROTO_OK);
#endif
    return 0;
}

int wget_PutData(tWgetInternal *wg, char *postdata, int datalen, const char *content_type, EventHandler callback, void *handle) {
    wg->request = ePutData;
    wg->content_type = content_type;
    wg->postdata = postdata;
    wg->datalen = datalen;
    wg->handle = handle;
    wg->cb = callback;
    if (wg->hdrs) {
        wg->hdrs->status_code = 0; /* reset status_code */
        wg->hdrs->content_length = 0;
    }
#ifdef USE_SSL
    /* init ssl if proto is https */
    if ( (strcmp(wg->proto, "https") == 0) && wg->pc->ssl==NULL) {
        proto_SetSslCtx(wg->pc, do_chk_cert_send_request, wg);
    } else {
        do_send_request(wg, PROTO_OK);
    }
#else
    do_send_request(wg, PROTO_OK);
#endif
    return 0;
}
/*
 * Disconnect maybe called from within a callback called
 * by report_status. Don't freeData() if cbActive is set.
 * Setting cCloseConnection will cause report_status
 * to free up the data on return by the callback.
 *
 */

int wget_Disconnect(tWgetInternal *wio) {
    if (wio!=NULL) {
        stopTimer(timer_response, wio); /* may be running */
        wio->request = eDisconnect;
        wio->keepConnection = eCloseConnection;
        if(!wio->cbActive)
            freeData(wio);
    }
    return 0;
}

int wget_AddPostHdr( tWgetInternal *wio, char *xhdrname, char *value) {
    XtraPostHdr	**p = &wio->xtraPostHdrs;
    return addPostHdr(p, xhdrname, value, REPLACE );
}

void wget_ClearPostHdrs( tWgetInternal *wio) {
    XtraPostHdr *xh = wio->xtraPostHdrs;

    while (xh) {
        XtraPostHdr *nxt;
        GS_FREE( xh->hdr);
        GS_FREE(xh->value);
        nxt = xh->next;
        GS_FREE(xh);
        xh= nxt;
    }
    wio->xtraPostHdrs = NULL;
}

const char *wget_LastErrorMsg(void) {
    return lastErrorMsg;
}

static int readChunkSz( tWget *wg ){
    char cbuf[80];
    /* read length of first chunk*/
    if (proto_Readline(wg->pc, cbuf, sizeof(cbuf))>0 ) {
        int  chksz=0;
        sscanf(cbuf, "%x", &chksz);
        return chksz;
    }
    return -1;
}


/*
 * tWget *wg is an connected web descriptor,
 *      *mlth is pointer to location of result read length,
 *      maxBufferSize is maximum size to read if non-zero. No limit if maxSize is 0.
 * Returns:
 *     pointer to response buffer or NULL.
 *      *mlth contain size of buffer. Undefined if return is NULL.
 */
char *wget_ReadResponse(tWget *wg, int *mlth, int maxBufferSize) {
	char *rspBuf = NULL;
	int bufCnt = 0;
	int bufLth;
	int readSz;
	*mlth = 0;
	if (wg->hdrs->content_length > 0) {
		bufCnt = 0;
		bufLth = wg->hdrs->content_length;
		if (maxBufferSize == 0 || bufLth < maxBufferSize) {
			if ((rspBuf = (char *) GS_MALLOC(bufLth+1))) { /* add one for null */
				while (bufCnt < wg->hdrs->content_length) {
					if ((readSz = proto_ReadWait(wg->pc, rspBuf + bufCnt,
							bufLth, NORMAL_TIMEOUT)) > 0) {
						bufCnt += readSz;
						bufLth -= readSz;
					} else {
						/* read error */
						*mlth = 0;
						GS_FREE(rspBuf);
						return NULL;
					}
				} DBGPRINT((stderr, "rspBuf readCnt=%d ContentLth=%d\n" , bufCnt, wg->hdrs->content_length ));
				*mlth = bufCnt;
				rspBuf[bufCnt] = '\0';
				return rspBuf;
			}
		}
	} else if (wg->hdrs->TransferEncoding && !strcasecmp(
			wg->hdrs->TransferEncoding, "chunked")) {
		char cbuf[80];
		int chksz;
		/* read length of chunk*/
		while ((chksz = readChunkSz(wg)) > 0) {
			/* walk thru chunks and read all chunks into the realloc buffer */
			char *newBuf;
			if (maxBufferSize > 0 && ((*mlth + chksz) > maxBufferSize)) {
				GS_FREE(rspBuf);
				*mlth = 0;
				return NULL;
			}
			if ((newBuf = GS_REALLOC(rspBuf, chksz+*mlth )) == NULL) {
				/* no memory */
				GS_FREE(rspBuf);
				*mlth = 0;
				return NULL;
			}
			rspBuf = newBuf;
			bufCnt = *mlth;
			bufLth = chksz;
			while (bufLth > 0) {
				if ((readSz = proto_ReadWait(wg->pc, rspBuf + bufCnt, bufLth, NORMAL_TIMEOUT))
						> 0) {
					bufCnt += readSz;
					bufLth -= readSz;
				} else {
					/* read error */
					GS_FREE(rspBuf);
					*mlth = 0;
					return NULL;
				}
			}
			*mlth += chksz;
			/* read trailing CRLF */
			if (proto_Readline(wg->pc, cbuf, sizeof(cbuf)) <= 0) {
				/* error reading end of chunk CRLF */
				GS_FREE(rspBuf);
				*mlth = 0;
				return NULL;
			}
		}
		if (chksz < 0) {
			/* error reading chunk size */
			GS_FREE(rspBuf);
			*mlth = 0;
			return NULL;
		}

		proto_Readline(wg->pc, cbuf, sizeof(cbuf)); /* flush off trailing crlf */
		proto_Skip(wg->pc); /* skip(flush) anything else */
		return rspBuf;
	}
	return NULL;
}
/*
 * w - web connection
 * cb    - callback to handle the buffered data.
 * handle- context of data.
 * buf   - buffer pointer to use to read network data.
 * bufLth- size of the buffer.
 *
 * return:
 * 		>0  lth of data read from network.
 *      0   no data or error;
 */
int wget_ReadBufResponse(tWget *wg, CBGetBuf cb, void *handle, char *buf, int bufLth ){
    int	 	total =0;
    int		readSz;
    char	cbuf[80];

    if (wg->hdrs->content_length>0) {
    	int left = wg->hdrs->content_length;
		while ( left > 0 ) {
			if ( (readSz = proto_ReadWait(wg->pc, buf, left<bufLth? left: bufLth, NORMAL_TIMEOUT)) > 0 ) {
				left -= readSz;
				if ( cb(handle, buf, readSz)== -1)
					return 0;     /* handler returned an error */
				total += readSz;
			} else
				return 0; /* this is some sort of error */
		}
    } else if (wg->hdrs->TransferEncoding && !strcasecmp(wg->hdrs->TransferEncoding, "chunked")) {
    	int chksz;
        /* read length of first chunk*/
		while ( (chksz= readChunkSz( wg)) >0 ) {
			/* walk thru chunks  */
			int left = chksz;
			while ( left>0 ){
				/* read bufLth of this chunk's data */
				if ( (readSz = proto_ReadWait(wg->pc, buf, left<bufLth? left: bufLth, NORMAL_TIMEOUT)) >0 ){
					left -= readSz;
					if ( cb(handle, buf, readSz)== -1 )
						return 0; /* handler returned an error */
					total += readSz;
				} else
					return 0;   /* error */
			}
			/* read trailing CRLF of chunk */
			if (proto_Readline(wg->pc, cbuf, sizeof(cbuf)) <= 0 ) {
				return 0;
			}
		}
		if ( chksz<0)
			return 0;  /* error reading chunk size */
		/* else chksz == 0 is done reading */
		proto_Readline(wg->pc, cbuf, sizeof(cbuf));   /* flush off trailing crlf */
		proto_Skip(wg->pc);         /* skip(flush) anything else */
	}
	return total;;
}

void wget_SetUserAgent(const char *uaName) {
    strncpy(userAgent, uaName, sizeof(userAgent));
    return;
}
void wget_SetServerTimeout(int timeout) {
    serverTimeout = timeout;
}
