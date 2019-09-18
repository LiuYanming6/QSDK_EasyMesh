/*
 * clientapilib.h
 *
 *  Created on: Jun 23, 2009
 *      Author: dmounday
 */

#ifndef CLIENTAPILIB_H_
#define CLIENTAPILIB_H_
#include "clientapi.h"
#include "arpa/inet.h"

#define REMOTE_HOST

typedef int APISTATUS;

#ifdef REMOTE_HOST
APISTATUS		  cpeLibInit(const char *hostname, unsigned port);
//#define USE_NETWORKORDER			/* USE network byte order for int */
#else
APISTATUS		  cpeLibInit( unsigned port );
#endif


#ifdef USE_NETWORKORDER
#define	NTOHL(x)		ntohl(x)
#define HTONL(x)		htonl(x)
#define	NTOHS(x)		ntohs(x)
#define HTONS(x)		htons(x)
#else
#define	NTOHL(x)		(x)
#define HTONL(x)		(x)
#define	NTOHS(x)		(x)
#define HTONS(x)		(x)
#endif

APISTATUS cpeStartSession(int maxSessionTime);
APISTATUS cpeEndSession(void);

APISTATUS cpeSetPath(const char *prefix);

APISTATUS cpeGetParameterValue( const char *path, char **value);
APISTATUS cpeSetParameterValue( const char *path, const char *value);
APISTATUS cpeCommit( const char *objPath);

APISTATUS cpeAddObject(const char *path, char **newObjPath);
APISTATUS cpeDeleteObject(const char *objPath);

APISTATUS cpeGetParameterNames( const char *path, int nextLevel, int *count);
APISTATUS cpeGetNextParameterName( char **value );

APISTATUS cpeFindObjectPath( const char *oname,
								const char *pname,
								const char *pvalue,
								char **path);

/* include these in their own header files to make conditional compilation easier*/
APISTATUS cpeReboot(void);
APISTATUS cpeReset(void);
APISTATUS cpeGetRPCMethods(void);
APISTATUS cpeStopCWMPc(void);
/****
APISTATUS cpeAutoTransComplete(void);
APISTATUS cpeRequestDownload(void);
*****/
APISTATUS cpeSetACSSessions(int);

#endif /* CLIENTAPILIB_H_ */
