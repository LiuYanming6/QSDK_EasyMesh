#ifndef UTILS_H_MAIN
#define UTILS_H_MAIN
/*----------------------------------------------------------------------*
 * Gatespace
 * Copyright 2004,2005,2006-2011 Gatespace. All Rights Reserved.
 * Gatespace Networks, Inc. confidential material.
 *----------------------------------------------------------------------*
 * File Name  :
 *
 * Description: utility routines prototypes for gs apps.
 *
 *
 * $Revision: 1.13 $
 * $Id: utils.h,v 1.13 2012/05/10 17:37:58 dmounday Exp $
 *----------------------------------------------------------------------*/
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>

/* a DBGLOG(x) should appear as
*    DBGLOG((DBG_MASK, "format string %s\n", datapointer));
*  where DBG_MASK is defined below.
*/
#ifdef DEBUGLOG
#define DBGLOG(X) cpeDbgLog X
#else
#define DBGLOG(X)
#endif

#define	DBG_DUMPSOAP	0x00000001
#define DBG_ACSCONNECT	0x00000002
#define DBG_TRANSFER	0x00000004
#define DBG_DUMPRPC		0x00000008
#define DBG_CHANGEDU	0x00000010

#define LOG_ERROR LOG_ERR

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

/*
 *TwinA Debug
 *
 * */


#define LOGFILE     "/dev/console"
#define DBG_MSG(fmt, arg...) do { FILE *log_fp = fopen(LOGFILE, "w"); \
                                     fprintf(log_fp, "%s:%s:%d:" fmt "\n", __FILE__, __func__, __LINE__, ##arg); \
                                     fclose(log_fp); \
                                     } while(0)



/*
 * Memory allocation function definitions:
 * The following macros are used to enable over-ride of the
 * system library memory allocation functions to enable testing
 * of boundary conditions and ease of replacement of allocation
 * functions with custom allocators.
 */
#ifdef USE_GSMEMWRAPPER
#define GS_STRDUP(SS) gs_strdup_wrapper(SS)
#define GS_STRNDUP(SS,XX) gs_strndup_wrapper(SS,XX)
#define GS_REALLOC(XX,SS) gs_realloc_wrapper(XX,SS)
#define GS_MALLOC(SS) gs_malloc_wrapper(SS)
#define GS_FREE(XX) gs_free_wrapper(XX)
char *gs_strdup_wrapper(const char *);
char *gs_strndup_wrapper(const char *, size_t );
void *gs_realloc_wrapper(void *, size_t);
void *gs_malloc_wrapper(size_t);
void gs_free_wrapper(void *);

#define COPYSTR(x,y) {if(x)gs_free_wrapper(x);x=gs_strdup_wrapper(y);}
#define MOVESTR(x,y) {if(x)gs_free_wrapper(x);x=y;y=NULL;}
#define FREESTR(x) {if(x){gs_free_wrapper(x);x=NULL;}}
#define DUPSTR(x,y) {if(y) x=gs_strdup_wrapper(y);}
/* EMPTYSTR used in if cond clause only */
#define EMPTYSTR(x) ((x==NULL)||(strlen(x)==0))

#else

#define GS_STRDUP(SS) strdup(SS)
#define GS_STRNDUP(SS,XX) strndup(SS,XX)
#define GS_REALLOC(XX,SS) realloc(XX,SS)
#define GS_MALLOC(SS) malloc(SS)
#define GS_FREE(XX) free(XX)

#define COPYSTR(x,y) {if(x)free(x);x=strdup(y);}
#define MOVESTR(x,y) {if(x)free(x);x=y;y=NULL;}
#define FREESTR(x) {if(x){free(x);x=NULL;}}
#define DUPSTR(x,y) {if(y) x=strdup(y);}
/* EMPTYSTR used in if cond clause only */
#define EMPTYSTR(x) ((x==NULL)||(strlen(x)==0))
#endif
#ifdef __APPLE__
char *strndup(const char*, size_t );
#endif

/*
 * The InAddr is used to retain the address family and
 * the address.
 */
typedef struct InAddr {
	sa_family_t inFamily;
	union{
		struct in_addr inAddr;
#ifdef USE_IPv6
		struct in6_addr in6Addr;
#endif
	}u;
}InAddr;


#ifdef USE_IPv6
/* IPv6 and IPv4 support:
 * SockAddrStorage is used when other information related to
 * the address is necessary.
 */
#define IP_ADDRSTRLEN INET6_ADDRSTRLEN
typedef struct sockaddr_storage SockAddrStorage;
#define SET_InADDR(X, XF, XAP) {(X)->inFamily=(XF); \
							  if (XF == AF_INET6) {\
								  (X)->u.in6Addr = *(struct in6_addr *)(XAP); \
							  } else {\
								  (X)->u.inAddr = *(struct in_addr *)(XAP);\
							  }}

#define SET_SockADDR(S, XPORT, INA)\
	{ (S)->sin_family = (INA)->inFamily;\
	  (S)->sin_port = (XPORT);\
	  if ((INA)->inFamily == AF_INET6){\
		((struct sockaddr_in6 *)(S))->sin6_addr = (INA)->u.in6Addr;\
		((struct sockaddr_in6 *)(S))->sin6_flowinfo = 0;\
		((struct sockaddr_in6 *)(S))->sin6_scope_id = 0;\
	 } else {\
		((struct sockaddr_in *)(S))->sin_addr = (INA)->u.inAddr;\
	  }\
	}

#define SockADDRSZ(S) \
	(S)->sin_family==AF_INET6?sizeof( struct sockaddr_in6):sizeof(struct sockaddr_in)

#else
/* IPv4 support only:
 *
 */
#define IP_ADDRSTRLEN INET_ADDRSTRLEN
typedef struct sockaddr SockAddrStorage;

#define SET_InADDR(X, XF, XA) {(X)->inFamily=XF;\
	                                  (X)->u.inAddr = *(XA);}

#define SET_SockADDR(S, XPORT, XINA) \
		{((struct sockaddr_in *)(S))->sin_port = (XPORT);\
		 ((struct sockaddr_in *)(S))->sin_family = (XINA)->inFamily;\
		 ((struct sockaddr_in *)(S))->sin_addr = (XINA)->u.inAddr;}

#define SockADDRSZ(sock) sizeof(struct sockaddr_in)
#endif

int get_domain_socket( const char *name);
int udp_listen(InAddr *ip, int port, char *inf, int broadcast);

/* Time */
time_t getCurrentTime(void);
int  cmpTime(struct timeval* t1, struct timeval* t2);
void addMs(struct timeval* t1, struct timeval* t2, int ms);
void subTime(struct timeval* t1, struct timeval* t2);
char *getXSIdateTime(const time_t *tp);


/* Addresses */
int  readInIPAddr( InAddr *, const char *);
const char *writeInIPAddr( InAddr *);
const char *writeIPAddr( SockAddrStorage *p );
int  eqInIPAddr( InAddr *ip1, InAddr *ip2 );
void clearInIPAddr( InAddr *);

void readMac(u_char* mac, const char* val);
char* writeMac(const u_char* mac);
char* writeCanonicalMac(const u_char* mac);
char* writeCanonicalMacUCase(const u_char* mac);
char* writeQMac(const u_char* mac);


/* Text handling and formatting */

int streq(const char *s0, const char *s1);
int stricmp( const char *s1, const char *s2 );
size_t strCpyLimited( char *dest, const char *src, size_t limit);
const char *itoa(int i);
int testBoolean(const char *s);
char *toAsciiHex(const unsigned char* buffer, int size, char* output);

typedef enum {
    eNone,
    eDigest,
    eBasic
} eAuthentication;
typedef enum {
	eNoQop,
	eAuth,
	eAuthInt
} eQop;

#define CNONCELTH   129
#define NONCELTH	129

/* Used for both server/client */
typedef struct SessionAuth  {
	eQop	qopType;
	int		nonceCnt;
    char    nonce[NONCELTH];       /* if makeNonce is modified the size of this */
    char    orignonce[NONCELTH];   /* need to be checked */
    char    realm[256];
    char    domain[512];
    char    method[10];
    char    cnonce[CNONCELTH];
	char 	opaque[512];
	char	qop[12];
    char    user[257];
    char    uri[512];
	char	algorithm[9];
    char    response[33];
    char    basic[684];
    unsigned    char    requestDigest[33];
} SessionAuth;

char *generateWWWAuthenticateHdr(SessionAuth *sa, char *realm, char *domain, char* method);

int parseAuthorizationHdr(char *ahdr, SessionAuth *sa, char *username, char *password);

char *generateAuthorizationHdrValue( SessionAuth *sa, char *wwwAuth, char *method,
									  char *uri, char *user, char *pwd);
char *generateNextAuthorizationHdrValue(SessionAuth *, char *user, char *pwd );
eAuthentication parseWWWAuthenticate(char *ahdr, SessionAuth *sa);

/////////////////////////twb function
int cmd_packet_popen(char *cmd, char *cmd_result);
int cmd_popen(char *cmd, char *cmd_result);
int util_check_string(const char *srcstr, char *destStr);
void util_check_string_on_shell_cmd(const char *srcstr, char *dststr);
void util_check_string_on_web(const char *srcstr, char *dststr);
char *util_delete_space(char *srcstr);
int get_role(void);
int get_uci_iface_name (int x);
char *get_topology_iface_name (int x);
unsigned int util_generate_random_num(void);
void randomstring(char *rstring,int length);
#endif

