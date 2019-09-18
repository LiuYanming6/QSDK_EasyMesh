
/*----------------------------------------------------------------------*
 * Gatespace
 * Copyright 2005-2011 Gatespace. All Rights Reserved.
 * Gatespace Networks, Inc. confidential material.
 *----------------------------------------------------------------------*
 * File Name  :
 *
 * Description:
 * $Revision: 1.15 $
 * $Id: protocol.c,v 1.15 2012/06/13 11:11:54 dmounday Exp $
 *----------------------------------------------------------------------*/



#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <ctype.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <poll.h>
#include <syslog.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifdef USE_SSL
	#include <openssl/ssl.h>
	#include <openssl/err.h>
#endif
#ifdef DMALLOC
	#include <dmalloc.h>
#endif
#include "event.h"
#include "utils.h"
#include "protocol.h"
#include "www.h"


#define SERVER_NAME "gs-httpd"
#define HTTP_PROTOCOL "HTTP/1.1"
#define RFC1123FMT "%a, %d %b %Y %H:%M:%S GMT"

extern void cpeLog( int level, const char *fmt, ...);

//#define DEBUG 1  /* general debug log */
//#define DEBUGHDRS /* log html headers */
//#define WRITETRACE  /* prints everything written to socket */
//#define READTRACE*/  /* prints everything read to socket */
//#define DEBUGSSL  /* log ssl io and connections */
//#define DEBUGX509	/* log certificate validation status */
//#define DISABLE_CN_MATCHING /* define to disable checking of CN in certificates */
//#define DEBUGCIPHERLIST  /* log the openssl library's available cipher list */

#ifdef DEBUGSSL
/*		Define DBGSSLC(X) to debug SSL connection and SSL_shutdown*/
#define DBGSSLC(X) fprintf X
#else
#define DBGSSLC(X)
#endif


#ifdef DEBUGSSL
	#define mkstr(S) # S
	#define setListener(A,B,C) {fprintf(stderr,mkstr(%s setListener B fd=%d\n), getticks(), A);\
setListener( A,B,C);}

	#define setListenerType(A,B,C,E) {fprintf(stderr,mkstr(%s setListenerType B-E fd=%d\n), getticks(), A);\
setListenerType( A,B,C,E);}

	#define stopListener(A) {fprintf(stderr,"%s stopListener fd=%d\n", getticks(), A);\
stopListener( A );}
#endif
#ifdef DEBUGSSL
#include <time.h>
char timestr[40];
char *getticks(void)
{
	struct timeval now;
	gettimeofday( &now,NULL);
	sprintf(timestr,"%04ld.%06ld", now.tv_sec%1000, now.tv_usec);
	return timestr;
}

#endif

#define BUF_SIZE_MAX 4096

#ifdef USE_SSL
static SSL_CTX *ssl_ctx = NULL;
#endif

#ifdef DEBUGSSL
static void showSocketStatus( unsigned fd)
{

	struct pollfd fdl[1];
	int e;

	fdl[0].fd = fd;
	fdl[0].events = 0xff;
	if ( (e=poll(fdl,1,0))<0)
		fprintf(stderr,"*poll() error\n");
	else
		fprintf(stderr, "poll=%0x\n",fdl[0].revents);
}
#endif

#ifdef USE_CERTIFICATES
static char caLoaded;
/**
 * Check certificate CN matches host name. If no certificates are present then
 * return a 1.
 * return 1: match
 *        0: does not match.
 */
int proto_CheckCertificate(tProtoCtx *pc, const char *host){
#ifndef DISABLE_CN_MATCHING
	X509* cert;
	SSL *ssl = pc->ssl;
	char cn[257];
	cn[0] = '\0';
	if ( caLoaded ){
		if (ssl==NULL){
			cpeLog(LOG_ERR, "SSL Protocol error");
			return 0;
		}
		if ( (cert = SSL_get_peer_certificate(ssl))==NULL){
			cpeLog(LOG_ERR, "No certificate received from server");
			return 0;
		}
		long verifyStatus = SSL_get_verify_result(ssl);
		if (verifyStatus != X509_V_OK ){
			cpeLog(LOG_ERR, "Certificate not valid");
			return 0;
		}
		X509_NAME *x509Name = X509_get_subject_name(cert);
		if ( X509_NAME_get_text_by_NID( x509Name, NID_commonName, cn, sizeof(cn))== -1)
			return 1; // no name returned -- assume no certificate loaded.
		//Block By Narendra badhekar
		/*
		if ( strcasecmp(host, cn )!= 0 ){
			cpeLog(LOG_ERR, "Certificate CN does not match host name. CN=%s host=%s",
						cn, host);
			X509_free(cert);
			return 0;
		}
		*/
		cpeLog(LOG_DEBUG,"Certificate CN: %s matches host: %s", cn, host);
		X509_free(cert);
	}
#endif
	return 1;
}
/*
* verify server certificate	 see SSL_CTS_set_verify()
 * Returns: 0 - verification failed: stop verifying in failed state.
 *          1 - continue  verifying.
*/
static int verify_callback(int ok, X509_STORE_CTX *store)
{
	#ifdef DEBUG
	fprintf(stderr, "X509 CTX callback ok=%d store=%p\n", ok, store);
	#endif
	if ( !ok )
		cpeLog(LOG_ERROR, "Certificate verification error");
	// add more server verification logic here if the normal isn't enough
#ifdef DEBUGX509
	{
		X509	*cert;
		char	buf[257];
		int		err;
		cert = X509_STORE_CTX_get_current_cert( store );
		err = X509_STORE_CTX_get_error( store );
		X509_NAME_oneline(X509_get_subject_name(cert), buf, 256);
		cpeLog(LOG_DEBUG, buf);
		if ( !ok ){
			cpeLog(LOG_DEBUG, "verify_callback: depth=%d error: %s:(%d)",
					X509_STORE_CTX_get_error_depth(store),
					X509_verify_cert_error_string(err), err);
		}
	}
#endif
	return ok;
}
#endif
/*
 * Setup openSSL to for certificate verification.
 */
#ifdef USE_CERTIFICATES
static void setupSSLCerts(char *serverCerts, char *clientCerts) {
	caLoaded = 0;
	if (clientCerts) {
		struct stat fstat;
		if (lstat(clientCerts, &fstat)==0) {
			/* define the client certificate chain */
			if (!SSL_CTX_use_certificate_chain_file(ssl_ctx,
					clientCerts)) {
				cpeLog(LOG_ERROR,"Error loading certificate chain");
			} else
				cpeLog(LOG_ERROR, "Certificate chain file set");
			/* the key is contained in the client certificate file */
			if (!SSL_CTX_use_PrivateKey_file(ssl_ctx, clientCerts,
					SSL_FILETYPE_PEM)) {
				cpeLog(LOG_ERROR, "Unable to open PrivateKeys: %s", clientCerts);
			}
		} else {
			cpeLog(LOG_ERROR,"SSL Client Certificate chain file %s not found.", clientCerts);
		}
	}
	if (serverCerts){
		struct stat fstat;
		if (lstat(serverCerts, &fstat)==0) {
			/* if CERT_FILE is present then setup cert verification */
			/* verify locations for the server certificate */
			cpeLog(LOG_DEBUG,"Found certificate file %s size=%d", serverCerts, fstat.st_size);
			if (!(caLoaded = SSL_CTX_load_verify_locations(ssl_ctx, serverCerts, NULL))) {
				cpeLog(LOG_ERROR, "Could not load server Certificate");
			} else {
				SSL_CTX_set_verify(ssl_ctx, SSL_VERIFY_PEER, verify_callback);
			}
		} else
			cpeLog(LOG_ERROR,"SSL Certificate file %s not found. Server Certificate verification disabled",
					serverCerts);
	}
}
#endif /* USE_CERTIFICATES */
/*======================================================================*
 * Init
 *======================================================================*/
#define TLSv1_2	"TLSv1.2:"
#define TLSv1_1	"TLSv1.1:"
#define TLSv1_0	"TLSv1.0:"
#define SSLv3_0	"SSLv3.0"
void proto_Init(char *cipherList, char *serverCerts, char *clientCerts)
{
#ifdef USE_SSL
#ifdef DMALLOC
	CRYPTO_malloc_debug_init();
	CRYPTO_mem_ctrl(CRYPTO_MEM_CHECK_ON);
	CRYPTO_set_mem_debug_options(V_CRYPTO_MDEBUG_ALL);
#endif
	//#if OPENSSL_API_COMPAT < 0x10100000L, for openssl 1.1.0 support
	//OPENSSL_init_ssl(OPENSSL_INIT_LOAD_SSL_STRINGS 
	//		| OPENSSL_INIT_LOAD_CRYPTO_STRINGS, NULL);
	//OPENSSL_init_ssl(0, NULL);
	//#endif
	SSL_load_error_strings();
	SSL_library_init();
	if ( cipherList && (strncmp(cipherList, TLSv1_2, strlen(TLSv1_2))==0)){
		// If TLS is in cipher list then use the TLSv1_2_client_method
		 ssl_ctx = SSL_CTX_new(TLSv1_2_client_method());
		cipherList+= strlen(TLSv1_2);
	} else if ( cipherList && (strncmp(cipherList, TLSv1_1, strlen(TLSv1_1))==0)){
		ssl_ctx = SSL_CTX_new(TLSv1_1_client_method());
		cipherList+= strlen(TLSv1_1);
	} else if ( cipherList && (strncmp(cipherList, TLSv1_0, strlen(TLSv1_0))==0)){
		ssl_ctx = SSL_CTX_new(TLSv1_client_method());
		cipherList+=strlen(TLSv1_0);
	} else {
		// otherwise restrict to SSLv3 method.
		ssl_ctx = SSL_CTX_new(SSLv3_client_method());
		// strip prefix is present.
		if ( cipherList && (strncmp(cipherList, SSLv3_0, strlen(SSLv3_0))==0))
			cipherList+=strlen(SSLv3_0);
	}
	if (ssl_ctx == NULL) {
		cpeLog(LOG_ERROR,"Could not create SSL context");
		exit(1);
	}
	if (cipherList) {
#ifdef DEBUGCIPHERLIST
		STACK_OF(SSL_CIPHER) *stk;
		SSL_CIPHER *c;
		char	buf[129];
		SSL *ssl;
		int	i;
		if ( (ssl = SSL_new(ssl_ctx))){
			stk = SSL_get_ciphers( ssl );
			int num = sk_SSL_CIPHER_num( stk );
			cpeLog(LOG_DEBUG, "ciphers supported: %d", num);
			for ( i=0; i < num; i++) {
				c = sk_SSL_CIPHER_value(stk, i);
				if ( SSL_CIPHER_description( c, buf, sizeof(buf)))
					cpeLog(LOG_DEBUG, buf);
			}
			SSL_free(ssl);
		}

#endif
		cpeLog(LOG_INFO, "Setting cipher list to %s", cipherList);
		if (! SSL_CTX_set_cipher_list(ssl_ctx, cipherList) )
			cpeLog(LOG_ERROR, "Could not set cipher list for SSL");
#ifdef USE_CERTIFICATES
		else {
			setupSSLCerts( serverCerts, clientCerts);
		}
#endif
	}
	/*SSL_CTX_set_mode(ssl_ctx, SSL_MODE_AUTO_RETRY); */
	ERR_print_errors_fp(stderr);
	SSL_CTX_set_session_cache_mode(ssl_ctx, SSL_SESS_CACHE_OFF);
#endif
}

/*======================================================================*
 * Ctx
 *======================================================================*/
/*----------------------------------------------------------------------*/
tProtoCtx *proto_NewCtx(int fd)
{
	tProtoCtx *pc;

	if ( (pc = (tProtoCtx *) GS_MALLOC(sizeof(tProtoCtx)))){
		memset(pc, 0, sizeof(tProtoCtx));
		pc->type = iNormal;
		pc->fd = fd;
	}
	return pc;
}
#ifdef USE_SSL
/*----------------------------------------------------------------------*/
static void server_wait_for_ssl(void *handle) {
	tProtoCtx *pc = handle;
	int res;

#ifdef DEBUGSSL
	fprintf(stderr,"%s server_wait_for_ssl() SSL connect fd=%d ", getticks(), pc->fd);
	showSocketStatus(pc->fd);
#endif
	if ((res = SSL_connect(pc->ssl)) <= 0) {
		int sslres;
		sslres = SSL_get_error(pc->ssl, res);
		if (sslres == SSL_ERROR_WANT_READ) {
#ifdef DEBUGSSL
			fprintf(stderr,"%s SSL connection wants to read fd=%d\n", getticks(), pc->fd);
#endif
			setListener(pc->fd, server_wait_for_ssl, pc);
		} else if (sslres == SSL_ERROR_WANT_WRITE) {
#ifdef DEBUGSSL
			fprintf(stderr,"%s SSL connection wants to write fd=%d\n",getticks(), pc->fd);
#endif
			setListenerType(pc->fd, server_wait_for_ssl, pc, iListener_Write);
		} else {
			char errstr[256];
			unsigned long err;
			while ( (err = ERR_get_error())){
					ERR_error_string_n(err,errstr,256);
					cpeLog(LOG_DEBUG,"Error connecting to server: (err=%d) %s",
						   err, errstr );
				}
			(*(pc->cb))(pc->data, PROTO_ERROR_SSL);
			/* Note: pc may have been freed by the callback */
			return;
		}
	} else {
#ifdef DEBUGSSL
		fprintf(stderr,"%s SSL server_wait_for_ssl callback fd=%d\n",getticks(), pc->fd);
#endif
		(*(pc->cb))(pc->data, PROTO_OK);
	}
	/* Wait on read/write data for callback werver_wait_for_ssl to be called again.
	   Listener must be enabled */
}

/*----------------------------------------------------------------------*
 * callback errorcodes
 *  PROTO_OK          all ok
 *  PROTO_ERROR       generic error
 *  PROTO_ERROR_SSL   ssl error
 */
void proto_SetSslCtx(tProtoCtx *pc, tProtoHandler cb, void *data)
{

	pc->type = iSsl;
	if ( (pc->ssl = SSL_new(ssl_ctx))!= NULL) {
		DBGSSLC((stderr, "%s proto_SetSslCtx()ssl=%p fd=%d\n",getticks(),pc->ssl,pc->fd));
		if ( SSL_set_fd(pc->ssl, pc->fd)>0 ) {
			pc->cb = cb;
			pc->data = data;
			/* Start SSL connection to server  */
			server_wait_for_ssl(pc);
		} else
			cpeLog(LOG_ERROR, "SSL_set_fd failed");
	} else
		cpeLog(LOG_ERROR,"SSL_new failed");
}

static void postShutdownCleanUp( void *handle)
{
	tProtoCtx   *pc = (tProtoCtx *)handle;

	DBGSSLC( (stderr,"%s  postShutdownCleanUp() ssl=%p fd=%d\n", getticks(), pc->ssl, pc->fd) );
	stopTimer( postShutdownCleanUp, (void*)pc );
	stopListener(pc->fd);
	close(pc->fd);
	if (pc->ssl)
		SSL_free(pc->ssl);
	GS_FREE(pc);
	return;
}
static void wait_for_sslShutdown( void *handle)
{
	tProtoCtx   *pc = (tProtoCtx *)handle;
	int r;

	DBGSSLC((stderr, "%s  wait_for_sslShutdown()fd=%d\n", getticks(), pc->fd));
	r= SSL_shutdown(pc->ssl);
	DBGSSLC((stderr, "%s  SSL_shutdown= %d ssl=%p fd=%d\n", getticks(), r, pc->ssl, pc->fd ));
	if (r == 0) {
		/* started shutdown -- now call again */
		r= SSL_shutdown(pc->ssl);
		DBGSSLC((stderr, "%s  2nd SSL_shutdown= %d ssl=%p fd=%d\n", getticks(), r, pc->ssl, pc->fd) );
	}
	if ( r == 1 ) {
		postShutdownCleanUp(pc);
	} else if ( r == -1 ) {
		int sslres;
		sslres = SSL_get_error(pc->ssl, r );
		if (sslres == SSL_ERROR_WANT_READ) {
			DBGSSLC((stderr,"%s SSL_shutdown wants to read fd=%d\n", getticks(), pc->fd));
			setListener(pc->fd, wait_for_sslShutdown, pc);
		} else if (sslres == SSL_ERROR_WANT_WRITE) {
			DBGSSLC((stderr,"%s SSL_shutdown wants to write fd=%d\n",getticks(), pc->fd));
			setListenerType(pc->fd, wait_for_sslShutdown, pc, iListener_Write);
		} else {
			char errstr[256];
			ERR_error_string_n(sslres,errstr,256);
			cpeLog(LOG_ERROR, "SSL_shutdown server: (r=%d,sslres=%d) %s",
				   r, sslres, errstr );
			postShutdownCleanUp( pc );
			return;
		}
	} else {
		DBGSSLC((stderr, "%s  SSL_shutdown state error ssl=%p fd=%d\n", getticks(), pc->ssl, pc->fd));
		postShutdownCleanUp(pc);
	}
	return;
}
#endif

/*----------------------------------------------------------------------*/
/* For iNormal protoCtx stopListener and close the fd					*/
/* For iSSL start shutdown												*/

void proto_FreeCtx(tProtoCtx *pc)
{
	switch (pc->type) {
	case iNormal:
		stopListener(pc->fd);
		close(pc->fd);
		GS_FREE (pc);
		break;
#ifdef USE_SSL
	case iSsl:
		stopListener(pc->fd);
		DBGSSLC((stderr, "%s   proto_FreeCtx()ssl=%p fd=%d\n",getticks(),pc->ssl,pc->fd));
		setTimer(postShutdownCleanUp, (void *)pc, 3000);
		if (pc->ssl) {
			/* not completed */
			wait_for_sslShutdown(pc);
		} else
			postShutdownCleanUp(pc);
		break;
#endif
	default:
		cpeLog(LOG_ERROR,"Impossible error; proto_FreeCtx() illegal ProtoCtx type (%d)", pc->type);
		GS_FREE(pc);
		break;
	}
}

/*======================================================================*
 * Util
 *======================================================================*/
#ifdef USE_SSL
typedef struct {
	tProtoCtx *pc;
	tSSLIO    iofunc;
	char *ptr;
	int nbytes;
	tProtoHandler cb;
	void *userdata;
} SSL_io_ctx;
/*
* The following SSL io routines ensure that the parameters are saved
 * and restored in the subsequent call to SSL_read/write whenever the
 * functions return a -1 indicating non-blocking inprogress io.
 */
static void SSL_io_handler(void *handle) {
	SSL_io_ctx *rc = handle;
	int nresult;
	int sslres;
	if (rc->iofunc == sslRead ) {
		nresult = SSL_read(rc->pc->ssl, (void *)rc->ptr, rc->nbytes);
#ifdef DEBUGSSL
		fprintf(stderr, "%s SSL_io_handler read ssl=%x socket=%d nresult=%d\n", getticks(), rc->pc->ssl, rc->pc->fd, nresult);
#endif
	} else {
		nresult = SSL_write(rc->pc->ssl, (void *)rc->ptr, rc->nbytes);
#ifdef DEBUGSSL
		fprintf(stderr, "%s SSL_io_handler write ssl=%x socket=%d nresult=%d\n", getticks(),rc->pc->ssl, rc->pc->fd, nresult);
#endif
	}
	if (nresult < 0) {
		sslres = SSL_get_error(rc->pc->ssl, nresult);
		if (sslres == SSL_ERROR_WANT_READ) {
#ifdef DEBUGSSL
			fprintf(stderr, "%s SSL connection listen to read fd=%d\n", getticks(), rc->pc->fd);
#endif
			setListener(rc->pc->fd, SSL_io_handler, rc);
			return;
		} else if (sslres == SSL_ERROR_WANT_WRITE) {
#ifdef DEBUGSSL
			fprintf(stderr, "%s SSL connection listen to write fd=%d\n",getticks(), rc->pc->fd);
#endif
			setListenerType(rc->pc->fd, SSL_io_handler, rc, iListener_Write);
			return;
		}
		cpeLog(LOG_DEBUG,"SSL_io_handler %s error fd=%d errcode=%d",
			   rc->iofunc==sslRead? "read": "write", rc->pc->fd, sslres);
		return;
	}
	/* If we get here, we're done */

	stopListener(rc->pc->fd);
	(*(rc->cb))((void*)rc->userdata, nresult);
	GS_FREE(rc);
}

/*----------------------------------------------------------------------*/
int proto_SSL_IO(tSSLIO func, tProtoCtx *pc, char *ptr, int nbytes, tProtoHandler cb, void *data) {

	SSL_io_ctx *rc;
	int nresult;

	if (func == sslRead) {
		nresult = SSL_read(pc->ssl, ptr, nbytes);
#ifdef DEBUGSSL
		fprintf(stderr, "%s proto_SSL_io read fd=%d nresult=%d\n", getticks(), pc->fd,nresult);
#endif
	} else if (func == sslWrite) {
		nresult = SSL_write(pc->ssl, ptr, nbytes);
#ifdef DEBUGSSL
		fprintf(stderr, "%s proto_SSL_io write fd=%d nresult=%d\n",getticks(), pc->fd,nresult);
#endif
	} else
		nresult = -1;
	if (nresult < 0) {
		int sslres = SSL_get_error(pc->ssl, nresult);
		rc = (SSL_io_ctx *)GS_MALLOC(sizeof(SSL_io_ctx));
		if (!rc)
			return -2;
		memset(rc, 0, sizeof(SSL_io_ctx));
		rc->iofunc = func;
		rc->pc = pc;
		rc->ptr = ptr;
		rc->nbytes = nbytes;
		rc->cb = cb;
		rc->userdata = data;
		if (sslres == SSL_ERROR_WANT_READ) {
#ifdef DEBUGSSL
			fprintf(stderr, "%s SSL_IO connection listen to read fd=%d\n", getticks(),rc->pc->fd);
#endif
			setListenerType(rc->pc->fd, SSL_io_handler, rc,iListener_ReadWrite);
		} else if (sslres == SSL_ERROR_WANT_WRITE) {
#ifdef DEBUGSSL
			fprintf(stderr, "%s SSL_IO connection listen to write fd=%d\n", getticks(),rc->pc->fd);
#endif
			setListenerType(rc->pc->fd, SSL_io_handler, rc, iListener_Write);
		} else {
#ifdef DEBUGSSL
			fprintf(stderr, "%s SSL_IO fd=%d error=%d\n", getticks(),rc->pc->fd, sslres);
#endif
			GS_FREE (rc);
		}
	}
	return nresult;
}
#endif
/*----------------------------------------------------------------------*/
/* blocking read */
int proto_ReadWait(tProtoCtx *pc, char *ptr, int nbytes, int timeout)
{
	int nread=0;
	int flags, bflags;

	/* turn on synchroneous I/O, this call will block. */
	{
		flags = (long) fcntl(pc->fd, F_GETFL);
		bflags = flags & ~O_NONBLOCK; /* clear non-block flag, i.e. block */
		fcntl(pc->fd, F_SETFL, bflags);
	}

	errno = 0;
	switch (pc->type) {
	case iNormal:
		if (!proto_WaitSocketData(pc, timeout)) {
			cpeLog(LOG_ERROR, "******* proto_ReadWait blocking timeout - no data");
			nread =  -1;
			break;
		}
		nread = read(pc->fd, ptr, nbytes);
		break;
#ifdef USE_SSL
	case iSsl:
#ifdef xxDEBUGSSL
		fprintf(stderr, "%s read_SSL(%d, lth=%d)", getticks(),pc->fd, nbytes);
		fprintf(stderr, " result=%d\n", nread = SSL_read(pc->ssl, (void *) ptr, nbytes));
#else
		nread = SSL_read(pc->ssl, (void *) ptr, nbytes);
#endif
		break;
#endif
	default:
		cpeLog(LOG_ERROR, "Impossible error; readn() illegal ProtoCtx type (%d)", pc->type);
		break;
	}
	if (nread > nbytes) {
		cpeLog(LOG_ERROR, "proto_READ of %d returned %d", nbytes, nread);
	}

	fcntl(pc->fd, F_SETFL, flags); /* remove blocking flags */
#ifdef READTRACE
	{   int i;
		for (i=0; i<nread; ++i)
			fprintf(stderr, "%c", *(ptr+i));
		fprintf(stderr, "\n\n");
	}
#endif
	return nread;
}

/* */
/*----------------------------------------------------------------------*/
int proto_Read(tProtoCtx *pc, char *ptr, int nbytes)
{
	int nread;

	errno = 0;
	switch (pc->type) {
	case iNormal:
		nread = read(pc->fd, ptr, nbytes);
		break;
#ifdef USE_SSL
	case iSsl:
//#ifdef DEBUGSSL
//		fprintf(stderr, "%s read_SSL(%d, lth=%d)", getticks(),pc->fd, nbytes);
//		fprintf(stderr, " result=%d\n", nread = SSL_read(pc->ssl, (void *) ptr, nbytes));
//#else
		nread = SSL_read(pc->ssl, (void *) ptr, nbytes);
//#endif
		break;
#endif
	default:
		cpeLog(LOG_ERROR, "Impossible error; readn() illegal ProtoCtx type (%d)", pc->type);
		nread = -1;
		break;
	}
	if (nread > nbytes) {
		cpeLog(LOG_ERROR, "proto_READ of %d returned %d", nbytes, nread);
	}
	return nread;
}

/*----------------------------------------------------------------------*/
int proto_Readn(tProtoCtx *pc, char *ptr, int nbytes)
{
	int nleft, nread=0;
	int   errnoval;

	nleft = nbytes;
	while (nleft > 0) {
		errno =0;
		switch (pc->type) {
		case iNormal:
			nread = read(pc->fd, ptr, nleft);
			break;
#ifdef USE_SSL
		case iSsl:
#ifdef xxDEBUGSSL
			fprintf(stderr, "%s SSL_read(%d, lth=%d)", getticks(),pc->fd, nleft);
			nread = SSL_read(pc->ssl, (void *) ptr, nleft);
			fprintf(stderr, " result=%d\n", nread);
#else
			nread = SSL_read(pc->ssl, (void *) ptr, nleft);
#endif
			break;
#endif
		default:
			cpeLog(LOG_ERROR, "Impossible error; readn() illegal ProtoCtx type (%d)", pc->type);
			break;
		}

		if (nread < 0) {							/* This function will read until the byte cnt*/
			errnoval=errno;							/* is reached or the return is <0. In the case*/
			if (errnoval==EAGAIN )					/* of non-blocking reads this may happen after*/
				return nbytes-nleft;				/* some bytes have been retrieved. The EAGAIN*/
			else									/* status indicates that more are coming */
													/* Other possibilites are ECONNRESET indicating*/
				/* that the tcp connection is broken */
				fprintf(stderr,"!!!!!!!! read(fd=%d) error=%d\n",
						pc->fd, errnoval);
			return nread; /* error, return < 0 */

		} else if (nread == 0) {
			break; /* EOF */
		}

		nleft -= nread;
		ptr += nread;
	}

	return nbytes - nleft; /* return >= 0 */
}
/*
 * Return number of bytes written or -1.
 * If -1 check for errno for EAGAIN and recall.
 *----------------------------------------------------------------------*/
int proto_Writen(tProtoCtx *pc, const char *ptr, int nbytes)
{
	int  nwritten=0;
	errno = 0;
	switch (pc->type) {
	case iNormal:
		nwritten = write(pc->fd, ptr, nbytes);
		break;
#ifdef USE_SSL
	case iSsl:
#ifdef DEBUGSSL
		fprintf(stderr, "%s SSL_write(%d, lth=%d)", getticks(),pc->fd, nbytes);
		nwritten = SSL_write(pc->ssl, ptr, nbytes);
		fprintf(stderr, "result=%d\n", nwritten);
#else
		nwritten = SSL_write(pc->ssl, ptr, nbytes);
#endif
		break;
#endif
	default:
		cpeLog(LOG_ERROR, "Impossible error; writen() illegal ProtoCtx type (%d)", pc->type);
		break;
	}

#ifdef WRITETRACE
	if ( nwritten>0 ) {
		int  i;
		for (i=0;i<nwritten;++i)
			fprintf(stderr,"%02x ", *(ptr+i));
	}
#endif

	if (nwritten <= 0) {
		if (errno!=EAGAIN)
/*            fprintf(stderr,"proto_Writen() status = %d Error%s(%d)\n",nwritten,strerror(errno),errno);  */
			return nwritten;
		/*
		else
			fprintf(stderr,"proto_Writen() status = %d Error%s(%d)\n",nwritten,strerror(errno),errno);
		*/
	}
	/*
	if (nwritten != nbytes) {
		fprintf(stderr,"proto_Writen() short write rlth=%d actual=%d\n", nbytes, nwritten);
	}
	*/
	return nwritten;
}


/*
* return the poll status of read event if io type. Use timeout on poll
* based on io type.
*/
int proto_WaitSocketData(tProtoCtx *pc, int timeout)
{
	//int	ret; //-Werror=unused-but-set-variable
        struct pollfd fdl[1];
	fdl[0].fd = pc->fd;
	fdl[0].events = POLLIN;
	fdl[0].revents = 0;   /* initialize in case of poll returning error*/
	//ret = poll(fdl,1, timeout); //-Werror=unused-but-set-variable
	poll(fdl, 1, timeout); //-Werror=unused-but-set-variable
	//fprintf(stderr, "Poll rtn =%d, to=%d events  %x\n", ret, timeout, fdl[0].revents);
	return (int)fdl[0].revents&POLLIN;
}
/*----------------------------------------------------------------------*
 * Read a line from a descriptor. Read the line one byte at a time,
 * looking for the newline. We store the newline in the buffer,
 * then follow it with a \0 (the same as fgets).
 * We return the number of characters up to, but not including,
 * the \0 (the same as strlen).
 */
int proto_Readline(tProtoCtx *pc, char *ptr, int maxlen)
{
	int n, rc;
	char c;
	int flags = 0, bflags; //-Werror=maybe-uninitialized

	/* TBD: Temporary fix part 1, turn on synchronous I/O, this call will block. */
	if (pc->type != iNormal) {
		flags = (long) fcntl(pc->fd, F_GETFL);
		bflags = flags & ~O_NONBLOCK; /* clear non-block flag, i.e. block */
		fcntl(pc->fd, F_SETFL, bflags);
	}
	/* waitSocketStatus is called to make sure there is a byte to read*/
	n = 0;
	while( n < maxlen ) {
		if (pc->type==iNormal && !proto_WaitSocketData(pc, NORMAL_TIMEOUT)) {
			cpeLog(LOG_ERROR, "******* proto_Readline blocking timeout - no data");
			n =  -1;
			break;
		}
		rc = proto_Readn(pc, &c, 1);
		if (rc == 1) {
			*ptr++ = c;
			++n;
			if (c == '\n')
				break;
		} else if (rc == 0) {
			break;	  /* EOF, some data was read */
		} else {
//#ifdef DEBUG
			fprintf(stderr, "ERROR: proto_Readline fd=%d (%d)\n", pc->fd, errno);
//#endif
			n = -1;
			break;; /* ERROR */
		}
	}
	if ( n == maxlen ){
		cpeLog(LOG_ERROR, "******* proto_Readline Input exceeds buffer -- input discarded");
		n = -1;
	} else {
		*ptr = '\0';
	}
	if (pc->type != iNormal)
		fcntl(pc->fd, F_SETFL, flags); /* TBD: fix part2, remove blocking flags */
	return n;
}
/******************************************************************/
#if 0
/* this is the unprotected version of proto_Readline. */
/*----------------------------------------------------------------------*
 * Read a line from a descriptor. Read the line one byte at a time,
 * looking for the newline. We store the newline in the buffer,
 * then follow it with a \0 (the same as fgets).
 * We return the number of characters up to, but not including,
 * the \0 (the same as strlen).
 */

int proto_Readline(tProtoCtx *pc, char *buf, int maxlen)
{
	int n, rc;
	char   *ptr = buf;
	char c;
	int flags, bflags;

	/* turn on synchroneous I/O, this call will block. */
	{
		flags = (long) fcntl(pc->fd, F_GETFL);
		bflags = flags & ~O_NONBLOCK; /* clear non-block flag, i.e. block */
		fcntl(pc->fd, F_SETFL, bflags);
	}

	for (n = 1; n < maxlen; n++) {
		rc = proto_Readn(pc, &c, 1);
		if (rc == 1) {
			*ptr++ = c;
			if (c == '\n')
				break;
		} else if (rc == 0) {
			if (n == 1) {
				fcntl(pc->fd, F_SETFL, flags); /* TBD: fix part2, remove blocking flags */
				return 0; /* EOF, no data read */
			} else
				break;	  /* EOF, some data was read */
		} else {
#ifdef DEBUG
			fprintf(stderr, "ERROR: proto_Readline fd=%d (%d)\n", pc->fd, errno);
#endif
			fcntl(pc->fd, F_SETFL, flags); /* remove blocking flags */
			return -1; /* ERROR */
		}
	}

	*ptr = '\0';
#ifdef READTRACE
	fprintf(stderr, "%s", buf);
#endif
	fcntl(pc->fd, F_SETFL, flags); /* remove blocking flags */
	return n;
}
#endif
/************************************************************/

/*----------------------------------------------------------------------*/
void proto_Printline(tProtoCtx *pc, const char *fmt, ...)
{
	char *p;
	char *np;
	va_list ap;
	int n;
	int size;

	size = 1024;
	if ((p = GS_MALLOC(size)) == NULL) {
		cpeLog(LOG_ERROR, "failed to malloc(%d)", size);
		return;
	}

	while (1) {
		/* try to print in the allocated space */
		va_start(ap, fmt);
		n = vsnprintf(p, size, fmt, ap);
		va_end(ap);

		if (n < 0) {
			cpeLog(LOG_WARNING, "fdprintf() vsnprintf failed *%d): %s (%d) fmt=\"%s\"\n", n, strerror(errno), errno, fmt);
#if DEBUG
			fprintf(stderr, "vsnprintf failed (%d): %s (%d) fmt=\"%s\"\n", n, strerror(errno), errno, fmt);
#endif
			return;
		} else if (n >= 0 && n < size) {
			/* print succeeded, let's write it on outstream */
			proto_Writen(pc, p, n);
			GS_FREE(p);
			return;
		} else {
#if DEBUG
			fprintf(stderr, "vsnprintf, only wrote %d bytes, retrying: fmt=\"%s\" strlen(fmt)=%d size=%d\n",
					n, fmt, strlen(fmt), size);
#endif
			size *= 2;
			if ((np = GS_REALLOC(p, size)) == NULL) {
				GS_FREE(p);
				cpeLog(LOG_ERROR, "failed to realloc(%d)", size);
				return;
			}
			p = np;
		}
	}/*end while(1)*/
}

/*======================================================================*
 * Data
 *======================================================================*/
tHttpHdrs *proto_NewHttpHdrs()
{
	tHttpHdrs *p;

	p = (tHttpHdrs *) GS_MALLOC(sizeof(tHttpHdrs));
	if (p == NULL) {
		return NULL;
	}
	memset(p, 0, sizeof(tHttpHdrs));
	return p;
}

/*----------------------------------------------------------------------*/
void proto_FreeHttpHdrs(tHttpHdrs *p)
{
	CookieHdr   *cp;
	GS_FREE(p->content_type);
	GS_FREE(p->wwwAuthenticate);
	GS_FREE(p->Authorization);
	GS_FREE(p->Connection);
	GS_FREE(p->TransferEncoding);
	GS_FREE(p->host);
	GS_FREE(p->Referer);
	cp = p->setCookies;
	while (cp) {
		CookieHdr *last = cp;
		GS_FREE(cp->name);
		GS_FREE(cp->value);
		cp = cp->next;
		GS_FREE(last);
	}
	GS_FREE(p->message);
	GS_FREE(p);
}

/*======================================================================*
 * Sending
 *======================================================================*/
/*----------------------------------------------------------------------*/
void proto_SendRequest(tProtoCtx *pc, const char *method, const char *url)
{
	char buf[BUF_SIZE_MAX];
	int len;

	len = snprintf(buf,BUF_SIZE_MAX, "%s %s HTTP/1.1\r\n", method, url);
	if (len != proto_Writen(pc, buf, len)) {
		/* error in sending */
		;
	}
#ifdef DEBUG
	fprintf(stderr, "proto_SendRequest(%s)\n", buf);
#endif
}

/*----------------------------------------------------------------------*/
void proto_SendCookie(tProtoCtx *pc, CookieHdr *c)
{
	char buf[BUF_SIZE_MAX];
	int len;

	len = snprintf(buf,BUF_SIZE_MAX, "Cookie: %s=%s\r\n", c->name, c->value);
	if (len != proto_Writen(pc, buf, len)) {
		/* error in sending */
		;
	}

#ifdef DEBUGHDRS
	fprintf(stderr, "proto_SendCookie -> %s", buf);
#endif

}
/*----------------------------------------------------------------------*/
void proto_SendHeader(tProtoCtx *pc,  const char *header, const char *value)
{
	char buf[BUF_SIZE_MAX];
	int len;

	len = snprintf(buf,BUF_SIZE_MAX, "%s: %s\r\n", header, value);
	if (len != proto_Writen(pc, buf, len)) {
		/* error in sending */
		;
	}

#ifdef DEBUGHDRS
	fprintf(stderr, "proto_SendHeader: %s", buf);
#endif

}

/*----------------------------------------------------------------------*/
void proto_SendRaw(tProtoCtx *pc, const char *arg, int len)
{
	int wlth;
	int totWlth = 0;
#ifdef xxDEBUGHDRS
	fprintf(stderr, "proto_SendRaw(l=%d,\"%s\")", len, arg);
#endif
	while ( totWlth<len ) {
		if ( (wlth = proto_Writen(pc, arg+totWlth, len-totWlth)) >= 0) {
			/* some or all data sent*/
			totWlth += wlth;
			continue;
		} else {
			/* EWOULDBLOCK is included in the following test for porting purposes */
			if (wlth < 0 && (errno == EAGAIN || errno == EWOULDBLOCK) ) {
				/* can't send is all keep trying -- busy wait on writes */
				continue;
			}
			/* data send error */
			break;
		}
	}
}

/*----------------------------------------------------------------------*/
void proto_SendEndHeaders(tProtoCtx *pc)
{
#ifdef DEBUGHDRS
	fprintf(stderr, "proto_SendEndHeaders()\n");
#endif
	proto_Printline(pc, "\r\n");
}

/*----------------------------------------------------------------------*/
void proto_SendHeaders(tProtoCtx *pc, int status, const char* title, const char* extra_header, const char* content_type)
{
	time_t now;
	char timebuf[100];

	proto_Printline(pc, "%s %d %s\r\n", HTTP_PROTOCOL, status, title);
	now = time((time_t*) 0);
	(void) strftime(timebuf, sizeof(timebuf), RFC1123FMT, gmtime(&now));
	proto_Printline(pc, "Date: %s\r\n", timebuf);
	proto_Printline(pc, "MIME-Version: 1.0\r\n");
	proto_Printline(pc, "Server: %s\r\n", SERVER_NAME);
	proto_Printline(pc, "Connection: Close\r\n" );
	if (extra_header != NULL)
		proto_Printline(pc, "%s\r\n", extra_header );
	if (content_type != NULL)
		proto_Printline(pc, "Content-Type: %s\r\n", content_type);
	proto_Printline(pc, "\r\n" );
}

/*----------------------------------------------------------------------*/
void proto_SendRedirect(tProtoCtx *pc, const char *host, const char* location)
{
	char header[BUF_SIZE_MAX];
	char slash[2];

	if (location[0] == '/')
		strcpy(slash, "");
	else
		strcpy(slash, "/");
#ifdef DEBUG
	fprintf(stderr, "web: proto_SEndRedirect: host=%s location=%s\n",host, location);
#endif
	(void) snprintf(header, sizeof(header), "Location: http://%s%s%s", host, slash, location);
	proto_SendHeaders(pc, 307, "Redirect", header, "text/html");
#ifdef DEBUG
	fprintf(stderr, "web: proto_SEndRedirect: %s\n", header);
#endif
}


/*----------------------------------------------------------------------*/
void proto_SendRedirectProtoHost(tProtoCtx *pc, const char *protohost, const char* location)
{
	char header[BUF_SIZE_MAX];

	(void) snprintf(header, sizeof(header), "Location: %s%s", protohost, location);
	proto_SendHeaders(pc, 307, "Redirect", header, "text/html");
#ifdef DEBUG
	fprintf(stderr, "web: proto_SendRedirectProtoHost: %s\n", header);
#endif
}

/*----------------------------------------------------------------------*/
void proto_SendRedirectViaRefresh(tProtoCtx *pc, const char *host, const char* location)
{
	char slash[2];

	if (location[0] == '/')
		strcpy(slash, "");
	else
		strcpy(slash, "/");

	proto_SendHeaders(pc, 200, "Ok", NULL, "text/html");
	proto_Printline(pc, "<HTML><HEAD><TITLE>Redirecting to requested site...</TITLE>\n");
	proto_Printline(pc, "<meta http-equiv=\"refresh\" content=\"0;URL=http://%s%s%s\"></HEAD>\n",
					host, slash, location);
}

/*----------------------------------------------------------------------*/
void proto_SendError(tProtoCtx *pc, int status, const char* title, const char* extra_header, const char* text)
{
	proto_SendHeaders(pc, status, title, extra_header, "text/html");
	proto_Printline(pc, "<HTML><HEAD><TITLE>%d %s</TITLE></HEAD>\n", status, title);
	proto_Printline(pc, "<BODY BGCOLOR=\"#cc9999\"><H4>%d %s</H4>\n", status, title);
	proto_Printline(pc, "%s\n", text );
	proto_Printline(pc, "</BODY></HTML>\n");
}

/*======================================================================*
 * Receiving
 *======================================================================*/
/*----------------------------------------------------------------------*
 * return
 *   0 if ok
 *  -1 on failure
 */
int proto_ParseRequest(tProtoCtx *pc, tHttpHdrs *hdrs)
{
	char buf[BUF_SIZE_MAX];
	char	*p;

	if ( hdrs == NULL)
		return -1;
	/* Parse the first line of the request. */
	if (proto_Readline(pc, buf, BUF_SIZE_MAX) <= 0) {
#ifdef DEBUG
		fprintf(stderr, "DEBUG: error =%d proto_ParseRequest() proto_Readline() rtns empty\n",
				errno);
#endif
		return -1;
	}
	p = strtok(buf, " ");
	if ( p==NULL || strlen(p) <3 || strlen(p)> 8 )   /* GET ... CONNECT method strings */
		return -1;
	strncpy( hdrs->method, p, sizeof(hdrs->method)); /* GET, PUT, ... */
	p = strtok(NULL, " ");
	if ( p )
		strncpy( hdrs->path, p, sizeof(hdrs->path) ); /* scan the URI. Limit to sizeof path*/
	p = strtok(NULL, " \n\r");
	if ( p == NULL || strcmp(p, HTTP_PROTOCOL)) {
		cpeLog(LOG_ERROR, "Invalid HTTP protocol version");
		return -1;							/* invalid protocol */
	}
	strncpy( hdrs->protocol, p, sizeof(hdrs->protocol));

#ifdef DEBUGHDRS
	fprintf(stderr, "proto_ParseRequest method=\"%s\" path=\"%s\" protocol=\"%s\"\n",
			hdrs->method, hdrs->path, hdrs->protocol);
#endif
	return 0; /* OK */
}

/*----------------------------------------------------------------------*
 * return
 *   0 if ok
 *  -1 on failure
 */
int proto_ParseResponse(tProtoCtx *pc, tHttpHdrs *hdrs)
{
	char buf[BUF_SIZE_MAX];
	char *p;

#ifdef DEBUGSSL
	fprintf(stderr, "%s proto_ParseResponse()\n", getticks());
#endif
	/* Parse the first line of the request. */
	if (proto_Readline(pc, buf, BUF_SIZE_MAX) <= 0) {
		return -1;
	}
	p = strtok(buf, " ");
	if ( p == NULL || strcmp(p, HTTP_PROTOCOL)) {
		cpeLog(LOG_ERROR, "Invalid HTTP protocol version");
		return -1;							/* invalid protocol */
	}
	strncpy( hdrs->protocol, p, sizeof(hdrs->protocol));
	p = strtok(NULL, " \n\r");
	if ( p==NULL || !isdigit(*p) )           /* status value */
		return -1;
	hdrs->status_code = atoi(p);
	p = strtok(NULL, " \n\r");				/* reason phrase -- not required*/
	if ( p ) {
		hdrs->message = GS_STRDUP(p);
		if ( hdrs->message == NULL){
			cpeLog(LOG_ERROR, "protocol: memory exhausted");
			return -1;
		}
	}

#ifdef DEBUG
	fprintf(stderr, "proto_ParseResponse(protocol=\"%s\", status=%d message=\"%s\")\n",
			hdrs->protocol, hdrs->status_code, hdrs->message? hdrs->message: "");
#endif

	return 0; /* OK */
}

static char HostStr[]="Host:";
static char ConnectionStr[]="Connection:";
static char SetCookieStr[]= "Set-Cookie:";
static char SetCookieStr2[]="Set-Cookie2:";
static char ContentLthStr[]="Content-Length:";
static char ContentTypeStr[]="Content-Type:";
static char WWWAuthenticateStr[]="WWW-Authenticate:";
static char AuthorizationStr[]="Authorization:";
static char TransferEncoding[]="Transfer-Encoding:";
static char LocationStr[]="Location:";
static char RefererStr[]="Referer:";

static void addCookieHdr(CookieHdr **p, char *c) {
	CookieHdr *newCookie = (CookieHdr*) GS_MALLOC(sizeof(CookieHdr));
	char *cp;
	char *name = NULL;
	char *value = NULL;

	if (newCookie) {
		if ((cp = strchr(c, '='))) {
			newCookie->name = name = (char *) GS_STRNDUP(c, cp - c);
			newCookie->value = value = GS_STRDUP(cp + 1);

			if (name == NULL || value == NULL) {
				GS_FREE(name);
				GS_FREE(value);
				GS_FREE(newCookie);
				return;
			}
			newCookie->next = *p;
			*p = newCookie;
		} else
			GS_FREE(newCookie);
	}
}
/*----------------------------------------------------------------------*
 * hdrs->type needs to be initiated
 * Only read headers according to type
 * Reads all headers until an empty '\r\n" is found.
 */
void proto_ParseHdrs(tProtoCtx *pc, tHttpHdrs *hdrs)
{
	char buf[BUF_SIZE_MAX];
	char *cp;
	int n;

#ifdef DEBUGHDRS
	fprintf(stderr, "DEBUG: proto_ParseHdrs() pc=%p pc->type=%d\n", pc, pc->type);
#endif
	if ( hdrs == NULL )
		return;
	/* Parse the rest of the request headers. */
	while ((n = proto_Readline(pc, buf, BUF_SIZE_MAX)) > 0) {
		www_StripTail(buf);
#ifdef DEBUGHDRS
		fprintf(stderr, "  DEBUG: read \"%s\"\n", buf);
#endif
		if (strcmp(buf, "") == 0) {
			break;
		} else if (strncasecmp(buf, HostStr,sizeof(HostStr)-1) == 0) {
			cp = &buf[sizeof(HostStr)-1];
			cp += strspn(cp, " \t");
			GS_FREE(hdrs->host);
			hdrs->host = GS_STRDUP(cp);
		} else if (strncasecmp(buf, ContentLthStr,sizeof(ContentLthStr)-1) == 0) {
			cp = &buf[sizeof(ContentLthStr)-1];
			cp += strspn(cp, " \t");
			hdrs->content_length = atoi(cp);
		} else if (strncasecmp(buf, ContentTypeStr,sizeof(ContentTypeStr)-1) == 0) {
			cp = &buf[sizeof(ContentTypeStr)-1];
			cp += strspn(cp, " \t");
			GS_FREE(hdrs->content_type);
			hdrs->content_type = GS_STRDUP(cp);
		} else if (strncasecmp(buf, ConnectionStr,sizeof(ConnectionStr)-1) == 0) {
			cp = &buf[sizeof(ConnectionStr)-1];
			cp += strspn(cp, " \t");
			GS_FREE(hdrs->Connection);
			hdrs->Connection = GS_STRDUP(cp);
		} else if (strncasecmp(buf, WWWAuthenticateStr, sizeof(WWWAuthenticateStr)-1)==0) {
			cp =&buf[sizeof(WWWAuthenticateStr)-1];
			cp += strspn(cp, " \t");
			GS_FREE(hdrs->wwwAuthenticate);
			hdrs->wwwAuthenticate = GS_STRDUP(cp);
		} else if (strncasecmp(buf, AuthorizationStr, sizeof(AuthorizationStr)-1)==0) {
			cp =&buf[sizeof(AuthorizationStr)-1];
			cp += strspn(cp, " \t");
			GS_FREE(hdrs->Authorization);
			hdrs->Authorization = GS_STRDUP(cp);
		} else if (strncasecmp(buf, TransferEncoding, sizeof(TransferEncoding)-1)==0) {
			cp =&buf[sizeof(TransferEncoding)-1];
			cp += strspn(cp, " \t");
			GS_FREE(hdrs->TransferEncoding);
			hdrs->TransferEncoding = GS_STRDUP(cp);
		} else if (strncasecmp(buf, LocationStr, sizeof(LocationStr)-1)==0) {
			cp =&buf[sizeof(LocationStr)-1];
			cp += strspn(cp, " \t");
			GS_FREE(hdrs->locationHdr);
			hdrs->locationHdr = GS_STRDUP(cp);
		} else if (strncasecmp(buf, RefererStr, sizeof(RefererStr)-1)==0) {
			cp =&buf[sizeof(RefererStr)-1];
			cp += strspn(cp, " \t");
			GS_FREE(hdrs->Referer);
			hdrs->Referer = GS_STRDUP(cp);
		} else if ( (strncasecmp(buf, SetCookieStr, sizeof(SetCookieStr)-1)==0)
					|| (strncasecmp(buf, SetCookieStr2, sizeof(SetCookieStr2)-1)==0) ) {
			char *c;
			cp =&buf[sizeof(SetCookieStr)-1];
			cp += strspn(cp, " \t:");	/* colon is added to skip : in SetCookieStr2 str*/
			/* don't need anything after ";" if it exists */
			if ( (c = strstr(cp,";")))
				*c = '\0';
			addCookieHdr( &hdrs->setCookies, cp );
		}


	}

#ifdef DEBUGHDRS
	fprintf(stderr, "DEBUG: proto_ParseHdrs done.\n");
#endif
}

/*----------------------------------------------------------------------*
 * discard all there is to read on the in buffer
 * This is used since some stupid browsers (e.g. IE) sends more data
 * than specified in the content-length header
 * Returns result of last read():
 *  	0 - eof
 *     -1 - connection error.
 *      1 - no data, possibly more.
 */
int proto_Skip(tProtoCtx *pc)
{
	char c;
	int nread;
	int ret = 0;
	long flags, nbflags;

#ifdef DEBUG
	fprintf(stderr, "DEBUG: proto_Skip() read all from fd and ignore\n");
#endif

	flags = (long) fcntl(pc->fd, F_GETFL);
	nbflags = flags | O_NONBLOCK;
	fcntl(pc->fd, F_SETFL, nbflags);

	do {
		switch (pc->type) {
		case iNormal:
			nread = read(pc->fd, &c, 1);
			break;
#ifdef USE_SSL
		case iSsl:
			nread = SSL_read(pc->ssl, &c, 1);
			break;
#endif
		default:
			cpeLog(LOG_ERROR, "Impossible error; illegal ProtoCtx type (%d)", pc->type);
			nread = -1;
			break;
		}
		if (nread<0) {
			ret = errno == EAGAIN? 1: -1;
			break;
		}
	} while (nread>0);
	fcntl(pc->fd, F_SETFL, flags);

#ifdef DEBUG
	fprintf(stderr, "DEBUG: proto_Skip() done.ret=%d\n", ret);
#endif
	return ret;
}


