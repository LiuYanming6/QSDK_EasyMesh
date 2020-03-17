/*----------------------------------------------------------------------*
 * Gatespace Networks, Inc.
 * Copyright 2004,2005 Gatespace. All Rights Reserved.
 * Gatespace Networks, Inc. confidential material.
 *----------------------------------------------------------------------*
 * File Name  :utils.c
 *
 * Description: utility routines for GS application environment.
 *
 *
 * $Revision: 1.14 $
 * $Id: utils.c,v 1.14 2012/05/10 17:37:58 dmounday Exp $
 *----------------------------------------------------------------------*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <sys/types.h>
#include <net/if.h>
#include <sys/time.h>
#include <time.h>
#include <syslog.h>
#include "md5.h"
#include "utils.h"

/*
 * GNU prototypes
 * May need implementation of caseless strstr function if not supported
 * by C libraries.
 */
extern char *strcasestr (const char *, const char *);
/*
 * missing in some Posix versions
 */
#ifndef isblank
int isblank(int c);
#endif

/*#define DEBUG*/

#ifdef DMALLOC
	#include "dmalloc.h"
#endif

void cpeLog(int level, const char *fmt, ...);

/* forwards */
static void generateBasicAuth( SessionAuth *sa, char *user, char* pwd);
static void generateCnonce(char *cnonceBuf);


size_t b64_encode(const char *inp, size_t insize, char *outptr);

/*
 * bind a unix domain socket with the pathname
 * return socket
 * 	      -1   on error.
 */
int get_domain_socket( const char *name){
	int fd;
	struct sockaddr_un addr;
	unlink(name);		// remove existing domain name
	memset(&addr, 0, sizeof(struct sockaddr_un));
	addr.sun_family = AF_UNIX;
	strcpy(addr.sun_path,name);
	if ((fd = socket(AF_UNIX, SOCK_DGRAM, 0))< 0){
		cpeLog(LOG_ERR, "could not initiate UNIX_DOMAIN_SOCKET %s", name);
		return -1;
	}
	if (bind( fd,(const struct sockaddr *) &addr, sizeof(struct sockaddr_un))< 0){
		cpeLog(LOG_ERR, "Unable to bind UNIX_DOMAIN_SOCKET %s", name);
		close ( fd );
		return -1;
	}
	return fd;
}

int udp_listen(InAddr *ip, int port, char *inf, int broadcast)
{
	struct ifreq interface;
	int fd;
	SockAddrStorage sa;
	struct sockaddr_in *sp=(struct sockaddr_in*)&sa;
	int n = 1;


#ifdef DEBUG
	cpeLog(LOG_DEBUG,"Opening listen socket on 0x%08x:%d %s\n", ip, port,
		   inf == NULL ? "all ifcs" : inf);
#endif
	memset(sp, 0, sizeof(SockAddrStorage));
	SET_SockADDR( sp, htons(port), ip);
	if ((fd = socket(sp->sin_family, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
		return -1;
	}

	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char *) &n, sizeof(n)) == -1) {
		close(fd);
		return -1;
	}
	if (broadcast &&
		setsockopt(fd, SOL_SOCKET, SO_BROADCAST, (char *) &n, sizeof(n)) == -1) {
		close(fd);
		return -1;
	}
#if !defined(__APPLE__)
	if (inf != NULL) {
		strncpy(interface.ifr_ifrn.ifrn_name, inf, IFNAMSIZ);
		if (setsockopt(fd, SOL_SOCKET, SO_BINDTODEVICE,
					   (char *)&interface, sizeof(interface)) < 0) {
			close(fd);
			return -1;
		}
	}
#endif

	if (bind(fd, (struct sockaddr*)sp, SockADDRSZ(sp) ) == -1) {
		close(fd);
		return -1;
	}
	return fd;
}


/**********************************************************************
 * Time
 **********************************************************************/
/*--------------------*/
static int DSTflag;
static int  timeoffset;

time_t getCurrentTime()
{
	time_t now;
	struct tm *t;
	now = time(NULL);
	t=localtime(&now);
	DSTflag = t->tm_isdst;
	timeoffset=timezone;
	return now-timeoffset+(DSTflag==1?3600:0);
}

char *getXSIdateTime(const time_t *tp)
{
	char cmd[]="date +\"%Y-%m-%dT%T%z\"";
	static char cmd_result[30]={0};
	static int gettime = 0;
	if (gettime == 0)
	{
	    gettime =1;
            cmd_popen(cmd, cmd_result);
	    int count = strlen(cmd_result);
	    memmove(&cmd_result[count-1],&cmd_result[count-2],3);
	    cmd_result[count-2]=':';
	}
	static char    buf[30];
	int     c;
	struct tm   *tmp;
	tmp = localtime(tp);
	c = strftime(buf,sizeof(buf),"%Y-%m-%dT%H:%M:%S%z",tmp );
	/* fix missing : in time-zone offset-- change -500 to -5:00 */
	memmove(&buf[c-1],&buf[c-2],3);
	buf[c-2]=':';
	if (!strncmp(buf , cmd_result , c))
		return buf;
	else
		return cmd_result;
}

/*--------------------*/
int cmpTime(struct timeval* t1, struct timeval* t2)
{
	int n = t1->tv_sec - t2->tv_sec;
	if (n) {
		return n;
	}
	return t1->tv_usec - t2->tv_usec;
}

/*--------------------*/
/* add Milliseconds   */
void addMs(struct timeval* t1, struct timeval* t2, int ms)
{
	t1->tv_sec  = t2->tv_sec  + ms/1000;
	t1->tv_usec = t2->tv_usec + 1000*(ms%1000);
	if (t1->tv_usec >= 1000000) {
		t1->tv_sec  += 1;
		t1->tv_usec -= 1000000;
	}
}

/*--------------------*/
void subTime(struct timeval* t1, struct timeval* t2)
{
	t1->tv_sec  = t2->tv_sec  - t1->tv_sec;
	t1->tv_usec = t2->tv_usec - t1->tv_usec;
	if (t1->tv_usec < 0) {
		t1->tv_sec  -= 1;
		t1->tv_usec += 1000000;
	}
}



/**********************************************************************
 * Addresses
 **********************************************************************/
/*--------------------*/
static int hexChar(int c)
{
	if (c >= 'A') {
		if (c >= 'a') {
			return c <= 'f' ? c - 'a' + 10 : -1;
		} else {
			return c <= 'F' ? c - 'A' + 10 : -1;
		}
	} else {
		return c >= '0' && c <= '9' ? c - '0' : -1;
	}
}

/*--------------------*/
void readMac(u_char* mac, const char* val)
{
	if(!val)
	{
		cpeLog(LOG_ERR, "readMac input null MAC address.");
		return;
	}

	u_char* emac = mac + 6;
	int flag = 0;
	int b = 0;

	while (*val && mac < emac) {
		int c = hexChar(*val++);
		if (c >= 0) {
			b = (b << 4) | c;
			if (flag) {
				*mac++ = b;
			}
			flag = !flag;
		}
	}
}


/*--------------------*/
char* writeMac(const u_char* mac)
{
	static char buf[24];

	snprintf(buf, sizeof(buf),"%02x:%02x:%02x:%02x:%02x:%02x",
			mac[0], mac[1], mac[2] ,mac[3], mac[4], mac[5]);
	return buf;
}


/*--------------------
 * writes MAC in canonical form (mosquito definition),
 * 12 characters denoting macaddress, no colon or other delimiters, all lower case
 */
char* writeCanonicalMac(const u_char* mac)
{
	static char buf[24];
	//char *p; //-Werror=unused-but-set-variable

	snprintf(buf, sizeof(buf), "%02x%02x%02x%02x%02x%02x",
			mac[0], mac[1], mac[2] ,mac[3], mac[4], mac[5]);
	//p = buf; //-Werror=unused-but-set-variable
	/* while ((*p = tolower(*p++)));  The %x is lower case abcdef This is causing an error with BRCMSW */

	return buf;
}

char* writeCanonicalMacUCase(const u_char* mac)
{
	static char buf[24];
	//char *p; //-Werror=unused-but-set-variable

	snprintf(buf, sizeof(buf), "%02X%02X%02X%02X%02X%02X",
			mac[0], mac[1], mac[2] ,mac[3], mac[4], mac[5]);
	//p = buf; //-Werror=unused-but-set-variable
	/* while ((*p = tolower(*p++)));  The %x is lower case abcdef This is causing an error with BRCMSW */

	return buf;
}
/*--------------------*/
char* writeQMac(const u_char* mac)
{
	static char buf[24];

	snprintf(buf, sizeof(buf), "%02x\\:%02x\\:%02x\\:%02x\\:%02x\\:%02x",
			mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
	return buf;
}


/* caseless strcmp */
/* returns: if S1 and S2 are NULL then result is 0 */
/*    if S1 is NULL and S2 is not then result is -1 */
/*    if S2 is NULL and S1 is not then result is 1 */
/*    otherwise; result is -1, 0, 1 respective of */
/*    value of S1 compared to S2 begin less, equal, or more*/
int stricmp( const char *s1, const char *s2 )
{
	if (s1 == NULL && s2 == NULL)
		return 0;
	else if (s1 == NULL)
		return -1;
	else if (s2 == NULL)
		return 1;
	for (;(*s1 && *s2) && (tolower(*s1))==(tolower(*s2)); ++s1, ++s2);
	return(tolower(*s1))-(tolower(*s2));
}

int streq(const char *s0, const char *s1)
{
	if (s0 == NULL && s1 == NULL)
		return 1;
	else if (s0 == NULL || s1 == NULL)
		return 0;
	else
		return strcmp(s0, s1) == 0;
}
#ifdef __APPLE__
char *strndup( const char *s, size_t sz){
	char *ret;
	size_t lth = strlen(s);
	if ( sz < lth )
		lth = sz;
	ret = (char *)GS_MALLOC( lth + 1);
	if ( ret ){
		memcpy( ret, s, lth);
		*(ret+lth) = '\0';
	}
	return ret;
}
#endif
/**
 * Convert size bytes of buffer to the corresponding ASCII hex
 * characters into output. The output buffer must be 2 times size plus 1.
 *
 * return output pointer.
 */
char *toAsciiHex(const unsigned char* buffer, int size, char* output)
{	int i;
	static char hexmap[] = "0123456789abcdef";
	const char* p = (const char*)buffer; //-Werror=pointer-sign
	char* r = output;
	for (i=0; i < size; i++) {
		unsigned char temp = *p++;
		int hi = (temp & 0xf0)>>4;
		int low = (temp & 0xf);

		*r++ = hexmap[hi];
		*r++ = hexmap[low];
	}
	*r = 0;
	return output;
}

/*
 * Copy source string to destination string and append terminating '\0'.
 * The number of characters copied is limited by limit. The limit decreased
 * by 1 to allow for the terminating '\0' character. The limit includes the terminating character.
 * *dest   pointer to destination buffer
 * *src    pointer to source buffer.
 * limit   size of destination buffer.
 *
 * return: number of characters in dest buffer excluding null terminator.
 *
 */
size_t strCpyLimited( char *dest, const char *src, size_t limit) {
	char *d = dest;
	const char *s = src;
	int i = 0;
	while ( i<limit-1 && *s!='\0' ) {
		*d++ = *s++;
		++i;
	}
	*d = '\0';
	return i;
}

const char *itoa(int i)
{
	static char buf[128];
	snprintf(buf, sizeof(buf), "%d", i);
	return buf;
}


int testBoolean(const char *s)
{
	if ( s!= NULL) {
		if (stricmp(s,"true")==0)
			return 1;
		else if (stricmp(s,"on")==0)
			return 1;
		else if (stricmp(s,"1")==0)
			return 1;
		else if (stricmp(s, "yes")==0)
			return 1;
	}
	return 0;
}

/*
* digest authentication routines
*/


void  resetSessionAuth( SessionAuth *s)
{
	memset(s,0,sizeof(struct SessionAuth));
	s->nonceCnt = 1;
}
/*
*  Scan for "argname=value"
 * and copy value to to argval buffer, if lth less than arglth;
 * Return lth of argval or 0 if error. argval string is empty.
*/

int getArg(char *p, char *argname, char *argval, int arglth)
{
	int     nameLth = strlen(argname);
	int     lth;
	char    *s = p;

	argval[0] = '\0';
	do {
		if ((s=strcasestr(s, argname)) ) {
			if ( isalpha(*(s-1))) {
				s += nameLth;
				continue;
			}
			s += nameLth;
			while (*s && isblank(*s)) ++s;
			if ( *s == '=') {
				++s;
				while (*s && isblank(*s)) ++s;
				if (*s!='\"') {
					/* no quotes around value assume blank delimited or trailing , */
					char *e;
					if ( (e=strchr(s, ',')) || (e=strchr(s,' ')))
						lth = e-s;
					else /* assume hit \0 at end */
						lth	= strlen(s);
				} else { /* s at opening quote of string enclosed in quotes */
					char *e;
					++s;
					if ((e=strchr(s,'\"')))
						lth = e-s;
					else
						lth	= 0;		/* no closing quote-- ignore */
				}
				if (lth) {
					if ( lth > arglth-1 )
						lth = arglth-1;    /* make sure NULL is added to end */
					strncpy(argval, s, lth);
					*(argval+lth) = '\0';
				}
				return lth;
			}
		} else	/* no char sequence found -- return */
			return 0;

	} while (*s);
	return 0;
}

void md5ToAscii( unsigned char *s /*16bytes */, unsigned char *d /*33bytes*/)
{
	int i;

	for (i = 0; i < 16; i++)
		snprintf((char *)&d[i*2],3,"%02x", s[i]);
}
/*
* Fill buffer pointed to by *nonce with new nonce value.
* !!!! The algorithm requires that the nonce buffer be of sufficient size to contain the
* calculated nonce value. See b64_encode comments for details.
* The length of the new nonce is returned.
*/
static size_t makeNonce(char *nonce)
{
	struct timeval  tv;
	char    buf[30];

	gettimeofday( &tv, NULL);
	/* start with something odd but hardly random */
	srand(tv.tv_usec);
	snprintf(buf, sizeof(buf), "%8x:%x8:%8x",
			 (unsigned)tv.tv_usec*rand(), (unsigned)tv.tv_usec*rand(), (unsigned)tv.tv_usec*1551*rand());
	return b64_encode(buf, 0, nonce);
}

char *generateWWWAuthenticateHdr(SessionAuth *sa, char *realm, char *domain, char *method )
{
	char buf[256];

	resetSessionAuth(sa);

	makeNonce(sa->nonce);
	strncpy(sa->orignonce, sa->nonce, sizeof(sa->orignonce)); /* make copy for later test */
	strncpy(sa->realm, realm, sizeof(sa->realm));
	strncpy(sa->domain, domain, sizeof(sa->domain));
	strncpy(sa->method, method, sizeof(sa->method));
	sa->qopType = eAuth;
	snprintf(buf, sizeof(buf),
			 "WWW-Authenticate: Digest realm=\"%s\", domain=\"%s\", nonce=\"%s\", qop=\"auth\","
			 " algorithm=MD5",
			 sa->realm, sa->domain, sa->nonce);
	return GS_STRDUP(buf);
}
/*
* Returns value of calculated digest in sa->requestDigest
 * *
*/
static void generateRequestDigest( SessionAuth *sa, char *user, char* pwd)
{
	unsigned char md5inbuf[512];
	unsigned char md5buf[16];
	unsigned char HA1[33];
	unsigned char HA2[33];

	snprintf((char * __restrict__)md5inbuf, sizeof(md5inbuf), "%s:%s:%s", user, sa->realm, pwd); //-Werror=pointer-sign
	tr69_md5it(md5buf, (unsigned char*)md5inbuf);
	md5ToAscii(md5buf,HA1);
	/*if ( sa->algorithm && strcmp(sa->algorithm, "MD5-sess"))
		snprintf(tmpbuf, sizeof(tmpbuf), "%s:%s:%s", HA1, sa->nonce, cnonceBuf); */
	/* don't know how to do auth-int */
	snprintf((char * __restrict__)md5inbuf, sizeof(md5inbuf),"%s:%s", sa->method, sa->uri); //-Werror=pointer-sign
	tr69_md5it(md5buf,md5inbuf);
	md5ToAscii(md5buf,HA2);

	if (sa->qopType == eNoQop )
		snprintf((char * __restrict__)md5inbuf, sizeof(md5inbuf), "%s:%s:%s", HA1, sa->nonce, HA2); //-Werror=pointer-sign
	else
		snprintf((char * __restrict__)md5inbuf, sizeof(md5inbuf), "%s:%s:%08x:%s:%s:%s", HA1, sa->nonce,
				 sa->nonceCnt, sa->cnonce, sa->qop, HA2); //-Werror=pointer-sign
	tr69_md5it(md5buf, (unsigned char*)md5inbuf);
	md5ToAscii(md5buf, sa->requestDigest);
}

/*
* parse Authorization: header and test response against  requestDigest
 * returns : 1 passed authorization
 *         : 0 failed authorization
*/
int parseAuthorizationHdr(char *ahdr, SessionAuth *sa, char *username, char *password)
{
	char    *p;
	char	nc[15];

	if ( ahdr && (p=strcasestr(ahdr, "digest"))) {
		getArg(p,"realm", sa->realm, sizeof(sa->realm));
		getArg(p,"username", sa->user, sizeof(sa->user));
		getArg(p,"opaque", sa->opaque, sizeof(sa->opaque));
		getArg(p,"nonce", sa->nonce, sizeof(sa->nonce));
		getArg(p,"uri", sa->uri, sizeof(sa->uri));
		getArg(p,"response", sa->response, sizeof(sa->response));
		getArg(p,"cnonce", sa->cnonce, sizeof(sa->cnonce));
		getArg(p,"qop", sa->qop, sizeof(sa->qop));
		getArg(p,"nc", nc, sizeof(nc));
		if (strlen(nc))
			sa->nonceCnt = atoi(nc);
		if ( streq(sa->user, username) && streq(sa->nonce, sa->orignonce)) {
			generateRequestDigest(sa, sa->user, password);
			if (!memcmp(sa->response, sa->requestDigest, 32))
				return 1;
		}
	}
	/* else "basic" is not allowed on connectionRequest" */
	return 0;
}

/*
* parse the WWW-Authenticate header and generate response digest in SessionAuth
*/
eAuthentication parseWWWAuthenticate(char *ahdr, SessionAuth *sa)
{
	char    *p;
	eAuthentication auth = eNone;

	resetSessionAuth(sa);
	if ( ahdr ) {
		if ((p=strcasestr(ahdr, "digest"))) {
			getArg(p,"realm", sa->realm, sizeof(sa->realm));
			getArg(p,"nonce", sa->nonce, sizeof(sa->nonce));
			getArg(p,"domain", sa->domain, sizeof(sa->domain));
			getArg(p,"opaque", sa->opaque, sizeof(sa->opaque));
			getArg(p,"cnonce", sa->cnonce, sizeof(sa->cnonce));
			getArg(p,"algorithm", sa->algorithm, sizeof(sa->algorithm));
			getArg(p,"qop", sa->qop, sizeof(sa->qop));
			auth = eDigest;
			if ( strlen(sa->qop)>0) {
				if (strcmp(sa->qop, "auth")==0)
					sa->qopType = eAuth;
				else if (strcmp(sa->qop, "auth-int")==0)
					sa->qopType = eAuthInt;
				else
					sa->qopType	= eNoQop;
			} else
				sa->qopType	= eNoQop;
		} else if ((p=strcasestr(ahdr, "basic"))) {
			getArg(p,"realm", sa->realm, sizeof(sa->realm));
			auth = eBasic;
		} else
			auth = eNone;
	}
	return auth;
}
/*
* create formated digest string for Authorization header
*/

#define	HDRVALUESZ 512

static char *formatDigestParamStr( SessionAuth *sa, char *user)
{
	char xhdrbuf[256];
	char opaquebuf[256];
	char *hdrvalue;

	if (!(hdrvalue = (char *)GS_MALLOC(HDRVALUESZ)))
		return NULL;
	if (sa->qopType!=eNoQop && strlen(sa->cnonce)>0)
		snprintf(xhdrbuf, sizeof(xhdrbuf), "cnonce=\"%s\", nc=%08x, ",
				 sa->cnonce, sa->nonceCnt);
	else
		xhdrbuf[0]='\0';
	if (strlen(sa->opaque)>0)
		snprintf(opaquebuf, sizeof(opaquebuf), "opaque=\"%s\", ", sa->opaque);
	else
		opaquebuf[0]='\0';
	snprintf(hdrvalue, HDRVALUESZ,
			 "Digest username=\"%s\", realm=\"%s\", algorithm=MD5,%s"
			 " uri=\"%s\", nonce=\"%s\", %s%sresponse=\"%s\"",
			 user, sa->realm, sa->qopType!=eNoQop? " qop=auth,":"",
			 sa->uri, sa->nonce, xhdrbuf, opaquebuf, sa->requestDigest);
#ifdef DEBUG
	fprintf(stderr, "Authorization header value =%s\n", hdrvalue);
#endif
	return hdrvalue;
}
/*
* generate the Authorization header value for Digest
*       SessionAuth *sa: Session authorization struct to use.
* 		wwwAuth: pointer to WWWAuthenticate header value
*		method: "GET" or "POST" const string.
*		uri:	pointer to with uri.
*		user:  username for authentication
*       pwd:   password for authentication
* Return is pointer to malloced value: oK,
			NULL: Parse error on WWWAuthenticate header or malloc failure.
*/


char *generateAuthorizationHdrValue( SessionAuth *sa, char *wwwAuth, char *method,
									 char *uri, char *user, char *pwd)
{
	eAuthentication auth;
	char    *hdrvalue = NULL;

	if ( (auth = parseWWWAuthenticate(wwwAuth, sa))== eDigest) {
		strncpy( sa->method, method, sizeof(sa->method));
		strncpy( sa->uri ,uri, sizeof(sa->uri));
		generateCnonce(sa->cnonce);
		generateRequestDigest(sa, user, pwd);
		hdrvalue = formatDigestParamStr(sa, user);
	} else if (auth == eBasic) {
		if ((hdrvalue = (char *)GS_MALLOC(HDRVALUESZ))) {
			generateBasicAuth(sa, user, pwd);
			snprintf(hdrvalue, HDRVALUESZ, "Basic %s", sa->basic);
		}
	} /* eNone if not eDigest or eBasic */
	return hdrvalue;
}

/*
* regenerate the Authorization header digest with the next nonce-count (nc)
*/
char *generateNextAuthorizationHdrValue( SessionAuth *sa, char *user, char *pwd)
{
	char    *hdrvalue;

	++sa->nonceCnt;
	generateRequestDigest(sa, user, pwd);
	hdrvalue = formatDigestParamStr(sa, user);
	return hdrvalue;
}

/* ---- Base64 Encoding --- */
static const char table64[]=
"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/*
 * b64_encode()
 *
 * Returns the length of the newly created base64 string. The third argument
 * is a pointer to a area large enough to hold the base64 data. Required buffer
 * size is (insize*4)/3 + 4. If insize is 0 the str length of *inp is used.
 *
 */
size_t b64_encode(const char *inp, size_t insize, char *outptr)
{
	unsigned char ibuf[3];
	unsigned char obuf[4];
	int i;
	int inputparts;
	char *output;

	char *indata = (char *)inp;

	*outptr = '\0';	/* set to empty in case of failure before we reach the end */

	if (insize == 0)
		insize = strlen(indata);
	output = outptr;

	while (insize > 0) {
		for (i = inputparts = 0; i < 3; i++) {
			if (insize > 0) {
				inputparts++;
				ibuf[i] = *indata;
				indata++;
				insize--;
			} else
				ibuf[i]	= 0;
		}

		obuf [0] = (ibuf [0] & 0xFC) >> 2;
		obuf [1] = ((ibuf [0] & 0x03) << 4) | ((ibuf [1] & 0xF0) >> 4);
		obuf [2] = ((ibuf [1] & 0x0F) << 2) | ((ibuf [2] & 0xC0) >> 6);
		obuf [3] = ibuf [2] & 0x3F;

		switch (inputparts) {
		case 1:	/* only one byte read */
			snprintf(output, 5, "%c%c==",
					 table64[obuf[0]],
					 table64[obuf[1]]);
			break;
		case 2:	/* two bytes read */
			snprintf(output, 5, "%c%c%c=",
					 table64[obuf[0]],
					 table64[obuf[1]],
					 table64[obuf[2]]);
			break;
		default:
			snprintf(output, 5, "%c%c%c%c",
					 table64[obuf[0]],
					 table64[obuf[1]],
					 table64[obuf[2]],
					 table64[obuf[3]] );
			break;
		}
		output += 4;
	}
	return strlen(outptr); /* return the length of the new data */
}
/* ---- End of Base64 Encoding ---- */
static void generateCnonce(char *cnonceBuf)
{
	char    buf[7];
	time_t  now;
	now= time(NULL);
	snprintf(buf, sizeof(buf), "%06ld", now);
	b64_encode(buf, 0, cnonceBuf);
}

static void generateBasicAuth(SessionAuth *sa, char *user, char *pwd)
{
	char    raw[256];
	size_t  dataLen;

	snprintf(raw, sizeof(raw), "%s:%s", user, pwd);
	dataLen=strlen(raw);
	b64_encode(raw, dataLen, sa->basic);
}


/////////////////////////twb function



int cmd_packet_popen(char *cmd, char *cmd_result)
{
	FILE * fp;
	char buffer[33] = {0};
	fp = popen(cmd, "r");
	if( fp == NULL )
		return 0;

	while(fgets(buffer, sizeof(buffer), fp))
	{
		if(buffer[strlen(buffer)-1]=='\n')
			buffer[strlen(buffer)-1]='\0';

		if (strlen(buffer) <= 32)
		    strcat(cmd_result, buffer);

		memset(buffer, 0x0, sizeof(buffer));
	}
	pclose(fp);

	return 0;
}



int cmd_popen(char *cmd, char *cmd_result)
{
	FILE * fp;
	char buffer[256] = {0};
	fp = popen(cmd, "r");
	if( fp == NULL )
		return 0;

	while(fgets(buffer, sizeof(buffer), fp))
	{
		if(buffer[strlen(buffer)-1]=='\n')
			buffer[strlen(buffer)-1]='\0';
		strcat(cmd_result, buffer);
		memset(buffer, 0x0, sizeof(buffer));
	}
	pclose(fp);

	return 0;
}

/**
*  @brief util_check_string
*  @param char *srcstr
*  @param char *dststr
*  @param int flag
*  @return void
*/
int util_check_string(const char *srcstr, char *destStr)
{
    int len;

    if(srcstr == NULL || (len = strlen(srcstr))==0)
    {
        destStr[0] = '\0';
        return -1;
    }

    int i,j;
    int cnt = 0;
    char specialchar[]={'\"','\\'};

    for(i=0; i<=len; i++)
    {
        if ( (int)srcstr[i] > 0 && ((int)srcstr[i] > 126 || (int)srcstr[i] < 32))
        {
            return -1;
        }

        for(j=0;j<sizeof(specialchar);j++)
        {
            if(srcstr[i] == specialchar[j])
            {
                destStr[cnt++] = '\\';
                break;
            }
        }
        destStr[cnt++] = srcstr[i];
    }
    return 0;
}
#if 0
/*****************************************************************************/
/**
*  @brief util_check_string_on_web
*  @param char *src_str
*  @param char *dst_str
*  @return void
*/
void util_check_string_on_web(const char *srcstr, char *dststr)
{
    util_check_string(srcstr,dststr,0);
}
/*****************************************************************************/
/**
 *  @brief twbox_util_check_string_on_shell_cmd
 *  @param char *srcstr
 *  @param char *dststr
 *  @return void
 */
void util_check_string_on_shell_cmd(const char *srcstr, char *dststr)
{
    util_check_string(srcstr,dststr,1);
}
#endif
/*****************************************************************************/
/**
 *  @brief util_delete_space
 *  @param char *srcstr
 *  @return void
 */

char *ltrim(char *srcstr)   // first space
{
    if (srcstr == NULL || *srcstr == '\0')
    {
        return srcstr;
    }

    int len = 0;
    char *p = srcstr;
    
    while (*p != '\0' && isspace(*p))
    {
        ++p;
        ++len;
    }

    memmove(srcstr, p, strlen(srcstr) - len + 1);

    return srcstr;
}

char *rtrim(char *srcstr) // tail space
{
	if (srcstr == NULL || *srcstr == '\0')
	{
		return srcstr;
	}
 
	int len = strlen(srcstr);
	char *p = srcstr + len - 1;
	while (p >= srcstr  && isspace(*p))
	{
		*p = '\0';
		--p;
	}
    return srcstr;
}

char *util_delete_space(char *srcstr)
{
	srcstr = rtrim(srcstr);
	srcstr = ltrim(srcstr);
	
	return srcstr;
}


/*****************************************************************************/
/**
 *  @brief get_role
 *  @param int x
 *  @return char*
 *
*/

int get_role(void)
{
    static char cmd[128]={0};
    static char cmd_result[128]={0};
    
    sprintf(cmd ,"uci get repacd.repacd.Role 2>&1");
    cmd_popen(cmd , cmd_result);
    
    if(!strncmp(cmd_result,"CAP",3))
        return 1;
    else
        return 0;
}

/*TWB EAP: dynamic Wifi interface sorting*/
int get_device_map_bss(int index) {
    int mapbss;
    char cmd[128]={0};
    char cmd_result[128]={0};
    sprintf(cmd, "uci get wireless.@wifi-iface[%d].MapBSSType", index);
    cmd_popen(cmd, cmd_result);
    strtok(cmd_result, "\n");
    mapbss = atoi(cmd_result);
    
    return mapbss;
}

void get_device_radio_name(char *s, int index) {
    char cmd[128]={0};
    char cmd_result[128]={0};
    sprintf(cmd, "uci get wireless.@wifi-iface[%d].device", index);
    cmd_popen(cmd, cmd_result);
    strtok(cmd_result, "\n");
    strcpy(s, cmd_result);

    return;
}

int  get_uci_wifi_index(int *uci_idx, int idx) {

    int m;
    int bss;
    char dev[16] = {0};
    *uci_idx = -1;
    for (m = 0; m < 5; m++)  //go through wifi-iface[0] to [4]
    {
        memset(dev , 0 , sizeof(dev));
        get_device_radio_name(dev, m); //get radio name
        bss = get_device_map_bss(m);   //get MapBSS type

        switch (idx)
        {
            case 0: //2.4 fh
                if (bss == 32 && !strncmp(dev, "wifi0", 5))
                {
                    //DBG_MSG("dev: %s  ,bss: %d, m: %d ,idx : %d\n", dev, bss, m , idx);
                    *uci_idx = m;
                }
                break;
            case 1: //2.4 bh
                if (bss == 64 && !strncmp(dev, "wifi0", 5))
                {
                    //DBG_MSG("dev: %s  ,bss: %d, m: %d ,idx : %d\n", dev, bss, m , idx);
                    *uci_idx = m;
                }
                break;
            case 2: //5 fh
                if (bss == 32 && !strncmp(dev, "wifi1", 5))
                {
                    //DBG_MSG("dev: %s  ,bss: %d, m: %d ,idx : %d\n", dev, bss, m , idx);
                    *uci_idx = m;
                }
                break;
            case 3: //5 bh
                if (bss == 64 && !strncmp(dev, "wifi1", 5))
                {
                    //DBG_MSG("dev: %s  ,bss: %d, m: %d ,idx : %d\n", dev, bss, m , idx);
                    *uci_idx = m;
                }
                break;
            default:
                break;
        }
    }

    if (*uci_idx == -1)
        return -1;
    else
        return 1;
}

int get_uci_wifi_intf_name(char *ath, int idx, int role) {

    int m;
    int bss;
    char dev[16] = {0};

    for (m = 0; m < 5; m++)
    {
        memset(dev , 0 , sizeof(dev));
        get_device_radio_name(dev, m); //get radio name
        bss = get_device_map_bss(m);   //get MapBSS type

        switch (idx)
        {
            case 0: //2.4 fh
               if (role == 0 || role == 1) 
               {
                    if (bss == 32 && !strcmp(dev, "wifi0")) 
                    {
                        if (m > 0 && m < 3) 
                            strcpy(ath, "0");
                        else if (m > 2 && m < 5) 
                            strcpy(ath, "01");
                    }
                }
                //DBG_MSG("dev: %s  ,bss: %d, m: %d ,idx : %d ath: %s\n", dev, bss, m , idx , ath);
            break;

            case 1: //2.4 bh
               if (role == 0 || role == 1)
               {
                    if (bss == 64 && !strcmp(dev, "wifi0"))
                    {
                        if (m > 0 && m < 3)
                            strcpy(ath, "0");
                        else if (m > 2 && m < 5)
                            strcpy(ath, "01");
                    }
               }
#if 0
                if (role == 0) 
                { //wifi 
                    if (bss == 64 && !strcmp(dev, "wifi0"))
                        strcpy(ath, "0");
                }
                else if (role == 1) 
                {  //ethernet
                    strcpy(ath, "none");
                }
                //DBG_MSG("dev: %s  ,bss: %d, m: %d ,idx : %d ath: %s\n", dev, bss, m , idx , ath);
#endif
            break;

            case 2: //5 fh
                if (bss == 32 && !strcmp(dev, "wifi1")) 
                {
                    if (m > 0 && m < 3) 
                    {
                        sprintf(ath, "%d", (role==1?1:11));
                    }
                    else if (m > 2 && m < 5)
                        sprintf(ath, "%d", (role==1?11:1));
                }
                //DBG_MSG("dev: %s  ,bss: %d, m: %d ,idx : %d ath: %s\n", dev, bss, m , idx , ath);
            break;

            case 3: //5 bh
                if (bss == 64 && !strcmp(dev, "wifi1"))
                {
                    if (m > 0 && m < 3)
                    {
                        sprintf(ath, "%d", (role==1?1:11));
                    }
                    else if (m > 2 && m < 5)
                        sprintf(ath, "%d", (role==1?11:12));
                }
#if 0
                if (role == 0) 
                { 
                    if (bss == 128 && !strcmp(dev, "wifi1"))
                        strcpy(ath, "1");
                }
                else if (role == 1) 
                    strcpy(ath, "none");

                //DBG_MSG("dev: %s  ,bss: %d, m: %d ,idx : %d ath: %s\n", dev, bss, m , idx , ath);
#endif
            break;

            default:
            break;
        }
    }

    if(!strncmp(ath,"none",4))
        return -1;
    else
        return 1;
}
/**/


int get_bkhaul_iface(char *iface)
{
    static char cmd[128]={0};
    static char cmd_result[128]={0};

    sprintf(cmd ,"iwconfig %s | grep \"Mode:\" | awk -F' ' '{print $1}' | awk -F':' '{print $2}'", iface);
    cmd_popen(cmd , cmd_result);

    if(!strncmp(cmd_result,"Managed",7))
        return 2;
    else
        return 3;
}

/*********************************************************************************/

/**
 *  @brief get_topology_iface_name
 *  @param int x
 *  @return char*
 */
/*
 * 
Agent ethernet backhaul
ath   uci  channel            SSID Reference
0      1   2.4G Fronthaul        1
01     3   2.4G Backhaul         2
1      2   5G Fronthaul          3
11     4   5G Backhaul           4
*

WiFi backhaul
ath1 Managed

ath   uci  channel            SSID Reference
0     1    2.4G Fronthaul       1
01    3    2.4G Backhaul        2
11    2    5G Fronthaul         3
1     0    5G Backhaul          4

*
ath0 Managed
ath   uci  channel            SSID Reference
01    1    2.4G Fronthaul       1
0     0    2.4G Backhaul        2
1     2    5G Fronthaul         3
11    4    5G Backhaul          4



*/
char *get_topology_iface_name (int role , int id)
{
    static char ath[16] = {0};
    int index;

    index = get_uci_wifi_intf_name(ath, id, role);

    if(index == -1)
        return "-1";
    else
        return ath;

#if 0
    if ( role == 1 )
    {
        if(id==0)
            return "0";
        else if (id==1)
            return "01";
        else if (id==2)
            return "1";
        else if (id==3)
            return "11";
    }
    else if (role ==2)
    {
        if(id==0)
            return "0";
        else if(id==1)
            return "01";
        else if(id==2)
            return "11";
        else if(id ==3)
            return "1";
    }
    else if (role == 3)
    {
        if(id==0)
            return "01";
        else if(id==1)
            return "0";
        else if(id==2)
            return "1";
        else if(id ==3)
            return "11";
    }
    else
        DBG_MSG("ERROR : get_role: %d %d",role , id);

    return "-1";
#endif
}

/*****************************************************************************/
/**
 *  @brief get_uci_iface_name
 *  @param int x
 *  @return int
 */
/*
 * 
Agent ethernet backhaul
ath   uci  channel            SSID Reference
0      1   2.4G Fronthaul        1
01     3   2.4G Backhaul         2
1      2   5G Fronthaul          3
11     4   5G Backhaul           4
*

WiFi backhaul
ath1 Managed

ath   uci  channel            SSID Reference
01     1    2.4G Fronthaul       1
       3    2.4G Backhaul        2
12     2    5G Fronthaul         3
1      0    5G Backhaul          4

*
ath0 Managed
ath   uci  channel            SSID Reference
0     1    2.4G Fronthaul       1
01    0    2.4G Backhaul        2
11    2    5G Fronthaul         3
1     4    5G Backhaul          4
*

*/
int get_uci_iface_name (int role , int id)
{
    int uci;
    int index;

    index = get_uci_wifi_index(&uci, id);

    if(index == -1)
        return -1;
    else
        return uci;

#if 0
    if ( role == 1 )
    {
        if(id==0)
            return 1;
        else if(id==1)
            return 3;
        else if (id ==2)
            return 2;
        else if (id ==3)
            return 4;
    }
    else if ( role == 2)
    {
        if(id==0)
            return 1;
        else if(id==1)
            return 3;
        else if(id==2)
            return 2;
        else if (id==3)
            return 0;
    }
    else if ( role ==3 )
    {
        if(id==0)
            return 1;
        else if(id==1)
            return 0;
        else if(id==2)
            return 2;
        else if(id ==3)
            return 4;
    }
    else
        DBG_MSG("ERROR : get_role: %d %d",role , id);

    return -1;
#endif
}

int character_check (char *str)
{
    int len = strlen(str);
    int i;
    int digits = 0;
    int alphabets = 0;
    int special = 0;
    for(i=0; i < len; i++)
    {
        if( (str[i] >= 'a' && str[i] <= 'z') || (str[i] >= 'A' && str[i] <= 'Z') )
            alphabets ++;
        else if(str[i] >= '0' && str[i] <='9')
            digits ++;
        else
            special ++;
    }

    if(alphabets > 0 || special > 0)
        return 0;
    else
        return 1;
}

/*****************************************************************************/
unsigned int util_generate_random_num(void)
{
    unsigned int seed;
    FILE *urandom;

    if((urandom = fopen("/dev/urandom","r")) != NULL)
    {
        fread(&seed, sizeof(seed), 1, urandom);
        fclose(urandom);

        srand(seed);
        return (unsigned int)rand();
    }
    return 0;
}

void randomstring(char *rstring,int length)
{
    if(!length)
        return;
    int tmp_len = 0;
    while(tmp_len<length)
    {
        rstring[tmp_len] = 'a' + (util_generate_random_num() % 26);
        tmp_len++;
    }
}
