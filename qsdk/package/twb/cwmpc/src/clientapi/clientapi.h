/*
 * clientapi.h
 *
 *  Created on: May 28, 2009
 *      Author: dmounday
 */

#ifndef CLIENTAPI_H_
#define CLIENTAPI_H_
#include <stddef.h>
#include "../includes/paramTree.h"
#include "targetsys.h"
#define	CPEAPI_PORT	30007

#define   TR69NAMESZ  257


/* library implementation details */
/* APISTATUS status */
#define API_SESS_OK				0
#define API_SESS_MSGERR			-4  /* Invalid request message */
#define API_SESS_ACSACTIVE		-2  /* ACS active session */
#define API_SESS_ACTIVEERR		-3  /* API session already active */
#define API_SESS_ERROR			-1  /* Messaging error */
/* mtype request values */
#define API_STARTSESSION	1
#define API_ENDSESSION		2
#define	API_GETPARAMETERVALUE	3
#define API_SETPARAMETERVALUE	4
#define	API_COMMIT				5
#define	API_ADDOBJECT			6
#define API_DELETEOBJECT		7
#define API_GETPARAMETERNAMES	8
#define	API_FINDOBJECTPATH		9
#define	API_REBOOT				20
#define	API_RESET				21
#define	API_GETRPCMETHODS		22
#define	API_AUTOTRANSFERCOMPLETE 23
#define API_REQUESTDOWNLOAD		24
#define API_GETNEXTPARAMNAME	25
#define API_SETACSSESSIONS		26
#define API_STOPCWMPC			27

typedef struct APICommit {
	char   objPath[TR69NAMESZ];
}APICommit;

typedef struct APISetParameterValue {
	char	paramName[TR69NAMESZ];
	char	paramValue[TR69NAMESZ];
} APISetParameterValue;

typedef struct APIFindObjectPath {
	char	objName[TR69NAMESZ];
	char	paramName[TR69NAMESZ];
	char	paramValue[TR69NAMESZ];
} APIFindObjectPath;

/* response to APIFindObjectPath */
typedef struct APIObjectPath {
	char	path[TR69NAMESZ];
}APIObjectPath;

typedef struct APIParameterName {
	char	writeable;
	char	paramName[TR69NAMESZ];
} APIParameterName;

typedef struct APIGetParameterNames {
	int		nextLevel;
	char	pathName[TR69NAMESZ];
} APIGetParameterNames;

typedef struct APIParameterValue {
	eCWMPType ptype;
	char		paramValue[TR69NAMESZ];
}APIParameterValue;

typedef struct APIStartSessionParam {
	int		maxWait;
	int		maxSessionTime;
} APIStartSessionParam;

typedef struct APIAddDeleteObject {
	char	pathName[TR69NAMESZ];
}APIAddDeleteObject;

typedef struct APIGetParameterNamesCnt {
	int		nameCount;

} APIGetParameterNamesCnt;

/*
 * Enable/disable ACS session scheduling.
 */
#define ACS_ENABLE_SESSIONS		1
#define ACS_DISABLE_SESSIONS 	0
typedef struct APISetACSSessions {
	int		enable;
}APISetACSSessions;

/*
 * API message template
 */

typedef struct CPEAPIMessage {
	int		mtype;
	int		mseq;
	int		msize;
	int		status;
	union {
		unsigned char mdata[TR69NAMESZ];
		APIFindObjectPath apiFindObjectPath;
		APIObjectPath apiObjectPath;
		APIGetParameterNames apiGetParameterNames;
		APIGetParameterNamesCnt apiGetParameterNamesCnt;
		APIParameterName apiParameterName;
		APIParameterValue apiParameterValue;
		APIStartSessionParam apiStartSessionParam;
		APISetParameterValue apiSetParameterValue;
		APICommit apiCommit;
		APIAddDeleteObject apiAddDeleteObject;
		APISetACSSessions apiSetAcsSessions;
	}ud;
} CPEAPIMessage;

#define API_STATUS_SZ	offsetof(CPEAPIMessage, ud)
#endif /* CLIENTAPI_H_ */
