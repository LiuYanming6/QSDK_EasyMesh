/*
 * cpeapilib.c
 *
 * This code implements the CPE's application side of the CPE API set that allows
 * CPE based applications to invoke CWMPc functions that can read and update
 * the data model.
 *
 *
 *  Created on: Jun 22, 2009
 *      Author: dmounday
 * $Revision: 1.6 $
 *
 * $Id: clientapilib.c,v 1.6 2012/06/13 10:52:55 dmounday Exp $
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
#include <sys/time.h>
#include <syslog.h>
#include <string.h>


//#define DEBUG
#ifdef DEBUG
#define DBGPRINT(X) fprintf X
#else
#define DBGPRINT(X)
#endif

#include "clientapilib.h"

static int fd = 0;
static struct hostent *netent;
static struct sockaddr_in cwmpaddr;
static int mseq = 1;
static char prefixstr[257];

#ifdef REMOTE_HOST

#include <netdb.h>
/*
 *
 */
APISTATUS cpeLibInit(const char *hostname, unsigned port ) {
	unsigned ip = 0;
	if ((netent = gethostbyname(hostname))) {
		ip = ntohl((int) *(int *) *netent->h_addr_list);
	}
#else
APISTATUS cpeLibInit( unsigned port ) {
	unsigned ip = 0;
#endif
	int		status = API_SESS_OK;
	fprintf(stderr, "IP address = %x:%d\n", ip, port);
	memset(&cwmpaddr, 0, sizeof(cwmpaddr));
	cwmpaddr.sin_family = AF_INET;
	cwmpaddr.sin_port = htons(port);
	cwmpaddr.sin_addr.s_addr = htonl(ip);

	return status;
}

/*
 * create the socket on each startSession to flush any
 * pending datagrams.
 */
static int connectServer(void){
	if ((fd = socket(PF_INET,SOCK_DGRAM, IPPROTO_UDP)) < 0) {
		fprintf(stderr, "Unable to open socket on port %d\n", ntohs(cwmpaddr.sin_port));
		return API_SESS_ERROR;
	}
	return 0;
}
/*
* close the socket on each startSession to flush any
* pending datagrams.
*/
static void disconnectServer(void){
	close(fd);
	fd = 0;
}
/*
 * return -1 if error.
 */
static int apisendmsg( CPEAPIMessage *data){
	int outsz;
	data->mseq = mseq++;

	outsz = sendto(fd, data, data->msize, 0, (struct sockaddr *) &cwmpaddr,
		sizeof(struct sockaddr_in));
	return outsz==data->msize? 0: -1;
}

static int receivemsg(CPEAPIMessage *mp ){
	ssize_t		msize;

	if ( (msize= recv(fd, (void *)mp, sizeof(struct CPEAPIMessage), 0))<= 0) {
		fprintf( stderr, "receivemsg error (%d)\n", errno);
		return -1;
	}
	DBGPRINT((stderr, "receivemsg completed event=%0x\n", mp->mtype));
	return msize;
}


static void setPName(char *d, const char *pn){
	int	lth;
	if ( strlen(prefixstr)==0 )
		strcpy( d, pn );
	else if ( strstr(pn, prefixstr)==NULL ){
		strcpy( d, prefixstr);
		lth = strlen(d);
		if ( lth + strlen(pn )<= TR69NAMESZ)
			strcpy( d+lth, pn);
	}
	return;
}

APISTATUS cpeSetPath(const char *prefix){
	if ( prefix == NULL ){
		prefixstr[0]='\0';
		return API_SESS_OK;
	}
	if ( strlen(prefix)> sizeof(prefixstr))
		return API_SESS_MSGERR;
	if ( strchr(prefix, '.')== 0)
		return API_SESS_MSGERR;
	strcpy(prefixstr, prefix);
	return API_SESS_OK;
}

APISTATUS cpeStartSession(int maxSessionTime ){
	CPEAPIMessage apiMsg;
	connectServer();
	if ( fd != 0 ){
		prefixstr[0]='\0';
		apiMsg.mtype = API_STARTSESSION;
		apiMsg.msize = API_STATUS_SZ + sizeof( struct APIStartSessionParam );
		apiMsg.ud.apiStartSessionParam.maxSessionTime = maxSessionTime;
		if ( apisendmsg(&apiMsg)>= 0 ){
			if (receivemsg( &apiMsg )>= 0){
				return apiMsg.status;
			}
		}
	}
	return API_SESS_MSGERR;
}

APISTATUS cpeEndSession(void){
	CPEAPIMessage apiMsg;
	if ( fd != 0 ){
		apiMsg.mtype = API_ENDSESSION;
		apiMsg.msize = API_STATUS_SZ;
		if ( apisendmsg(&apiMsg)>= 0 ){
			if (receivemsg( &apiMsg )>= 0){
				disconnectServer();
				return apiMsg.status;
			}
		}
		disconnectServer();
	}
	return API_SESS_MSGERR;
}

APISTATUS cpeGetParameterValue( const char *pname, char **value ){
	CPEAPIMessage apiMsg;
	if ( fd != 0 ){
		apiMsg.mtype = API_GETPARAMETERVALUE;
		apiMsg.msize = sizeof(apiMsg);
		setPName( apiMsg.ud.apiParameterName.paramName, pname);
		if ( apisendmsg(&apiMsg)>= 0 ){
			if (receivemsg( &apiMsg )>= 0){
				*value = strdup(apiMsg.ud.apiParameterValue.paramValue);
				return apiMsg.status;
			}
		}
	}
	return API_SESS_MSGERR;
}

APISTATUS cpeSetParameterValue( const char *pname, const char *value ){
	CPEAPIMessage apiMsg;
	if ( fd != 0 ){
		apiMsg.mtype = API_SETPARAMETERVALUE;
		apiMsg.msize = API_STATUS_SZ + sizeof(struct APISetParameterValue);
		setPName( apiMsg.ud.apiSetParameterValue.paramName, pname);
		strcpy( apiMsg.ud.apiSetParameterValue.paramValue, value);
		if ( apisendmsg(&apiMsg)>= 0 ){
			if (receivemsg( &apiMsg )>= 0){
				return apiMsg.status;
			}
		}
	}
	return API_SESS_MSGERR;
}

APISTATUS cpeAddObject( const char *path, char **ret){
	CPEAPIMessage apiMsg;
	if ( fd != 0 ){
		apiMsg.mtype = API_ADDOBJECT;
		apiMsg.msize = API_STATUS_SZ + sizeof(struct APIAddDeleteObject);
		setPName( apiMsg.ud.apiAddDeleteObject.pathName, path);
		if ( apisendmsg(&apiMsg)>= 0 ){
			if (receivemsg( &apiMsg )>= 0){
				if ( apiMsg.status == 0 ){
					*ret = strdup(apiMsg.ud.apiAddDeleteObject.pathName);
				}
				return apiMsg.status;
			}
		}
	}
	return API_SESS_MSGERR;
}

APISTATUS cpeDeleteObject( const char *path ){
	CPEAPIMessage apiMsg;
	if ( fd != 0 ){
		apiMsg.mtype = API_DELETEOBJECT;
		apiMsg.msize = API_STATUS_SZ + sizeof(struct APIAddDeleteObject);
		setPName( apiMsg.ud.apiAddDeleteObject.pathName, path);
		if ( apisendmsg(&apiMsg)>= 0 ){
			if (receivemsg( &apiMsg )>= 0){
				return apiMsg.status;
			}
		}
	}
	return API_SESS_MSGERR;
}

APISTATUS cpeCommit( const char *path ){
	CPEAPIMessage apiMsg;
	if ( fd != 0 ){
		apiMsg.mtype = API_COMMIT;
		apiMsg.msize = API_STATUS_SZ + sizeof(struct APICommit);
		setPName( apiMsg.ud.apiCommit.objPath, path);
		if ( apisendmsg(&apiMsg)>= 0 ){
			if (receivemsg( &apiMsg )>= 0){
				return apiMsg.status;
			}
		}
	}
	return API_SESS_MSGERR;
}

APISTATUS cpeGetRPCMethods(){
	CPEAPIMessage apiMsg;
	if ( fd != 0 ){
		apiMsg.mtype = API_GETRPCMETHODS;
		apiMsg.msize = API_STATUS_SZ;
		if ( apisendmsg(&apiMsg)>= 0 ){
			if (receivemsg( &apiMsg )>= 0){
				return apiMsg.status;
			}
		}
	}
	return API_SESS_MSGERR;
}

APISTATUS cpeFindObjectPath( const char *oname,
								const char *pname,
								const char *pvalue,
								char **path) {
	CPEAPIMessage apiMsg;
	if ( fd != 0 ){
		apiMsg.mtype = API_FINDOBJECTPATH;
		apiMsg.msize = API_STATUS_SZ + sizeof(struct APIFindObjectPath);
		strcpy( apiMsg.ud.apiFindObjectPath.objName, oname);
		strcpy( apiMsg.ud.apiFindObjectPath.paramName, pname);
		strcpy( apiMsg.ud.apiFindObjectPath.paramValue, pvalue);

		if ( apisendmsg(&apiMsg)>= 0 ){
			if (receivemsg( &apiMsg )>= 0){
				*path = strdup(apiMsg.ud.apiObjectPath.path);
				return apiMsg.status;
			}
		}
	}
	return API_SESS_MSGERR;
}

APISTATUS cpeGetParameterNames( const char *path, int nextLevel, int *paramCount) {
	CPEAPIMessage apiMsg;
	if ( fd != 0 ){
		apiMsg.mtype = API_GETPARAMETERNAMES;
		apiMsg.msize = API_STATUS_SZ + sizeof(struct APIGetParameterNames);
		setPName( apiMsg.ud.apiGetParameterNames.pathName, path);
		apiMsg.ud.apiGetParameterNames.nextLevel =  nextLevel;
		if ( apisendmsg(&apiMsg)>= 0 ){
			if (receivemsg( &apiMsg )>= 0){
				*paramCount = apiMsg.ud.apiGetParameterNamesCnt.nameCount;
				return apiMsg.status;
			}
		}
	}
	return API_SESS_MSGERR;
}

APISTATUS cpeGetNextParameterName(char **pname ) {
	CPEAPIMessage apiMsg;
	if ( fd != 0 ){
		apiMsg.mtype = API_GETNEXTPARAMNAME;
		apiMsg.msize = API_STATUS_SZ;
		if ( apisendmsg(&apiMsg)>= 0 ){
			if (receivemsg( &apiMsg )>= 0){
				if ( apiMsg.msize > API_STATUS_SZ ){
					*pname = strdup( apiMsg.ud.apiParameterName.paramName);
				} else {
					*pname = NULL;
				}
				return apiMsg.status;
			}
		}
	}
	return API_SESS_MSGERR;
}

/*
 * There is no reply from the CWMPc to these messages.
 */
APISTATUS cpeReboot(void) {
	CPEAPIMessage apiMsg;
	if ( fd != 0 ){
		apiMsg.mtype = API_REBOOT;
		apiMsg.msize = API_STATUS_SZ;
		apisendmsg(&apiMsg);
		return 0;
	}
	return API_SESS_MSGERR;
}

APISTATUS cpeReset(void) {
	CPEAPIMessage apiMsg;
	if ( fd != 0 ){
		apiMsg.mtype = API_RESET;
		apiMsg.msize = API_STATUS_SZ;
		apisendmsg(&apiMsg);
		return 0;
	}
	return API_SESS_MSGERR;
}

APISTATUS cpeStopCWMPc(void) {
	CPEAPIMessage apiMsg;
	if ( fd != 0 ){
		apiMsg.mtype = API_STOPCWMPC;
		apiMsg.msize = API_STATUS_SZ;
		apisendmsg(&apiMsg);
		return 0;
	}
	return API_SESS_MSGERR;
}

APISTATUS cpeSetACSSessions( int enable) {
	CPEAPIMessage apiMsg;
	if ( fd != 0 ){
		apiMsg.mtype = HTONL(API_SETACSSESSIONS);
		apiMsg.msize = HTONL(API_STATUS_SZ + sizeof(struct APISetACSSessions));
		apiMsg.ud.apiSetAcsSessions.enable = HTONL(enable);
		if ( apisendmsg(&apiMsg)>= 0 ){
			if (receivemsg( &apiMsg )>= 0){
				return NTOHL(apiMsg.status);
			}
		}
	}
	return API_SESS_MSGERR;
}

