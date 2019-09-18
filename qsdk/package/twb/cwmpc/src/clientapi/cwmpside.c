/*
 * cwmpside.c
 *
 * This code implements the CWMPc side of the CPE API set that allows
 * CPE based applications to invoke CWMPc functions that can read and update
 * the data model.
 *
 *
 *  Created on: Jun 22, 2009
 *      Author: dmounday
 * $Revision: 1.8 $
 *
 * $Id: cwmpside.c,v 1.8 2012/06/13 10:52:55 dmounday Exp $
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
#include <ctype.h>

#ifdef DMALLOC
#include "dmalloc.h"
#endif
#include "../includes/sys.h"
#include "../gslib/src/utils.h"
#include "../gslib/src/event.h"
#include "../gslib/src/protocol.h"
#include "../includes/paramTree.h"
#include "../includes/CPEWrapper.h"
#include "../soapRpc/acsconnreq.h"
#include "../soapRpc/rpcUtils.h"
#include "../soapRpc/notify.h"
#include "../soapRpc/cwmpSession.h"
#include "targetsys.h"
#include "clientapi.h"

#define DEBUG
#ifdef DEBUG
#define DBGPRINT(X) fprintf X
#else
#define DBGPRINT(X)
#endif

extern CPEState cpeState;
extern char wpPathName[512];
extern CWMPObject CWMP_RootObject[];
typedef struct APIMsgDope {
	int fd;
	struct sockaddr from;
	int addrlen;
	CPEAPIMessage msg;
} APIMsgDope;

static int sessActive = 0;
static int sessionTime;
static int sessSeq;

static void replyMessage(APIMsgDope *);

typedef struct LinkedNames{
	struct LinkedNames	*next;
	APIParameterName	paramNameData;
}LinkedNames;
static LinkedNames	*getNames;

static void freeLinkedNames(void){
	LinkedNames *p = getNames;
	LinkedNames *t;
	while ( p ){
		t = p->next;
		GS_FREE ( p );
		p = t;
	}
	getNames = NULL;
}

static CPEAPIMessage *runGetNextParameterName(CPEAPIMessage *mp) {

	LinkedNames *p = getNames;
	/* this function uses the request buffer to write the response into */
	if ( p ){
		memcpy( &mp->ud.apiParameterName, &p->paramNameData, sizeof(struct APIParameterName));
		mp->msize = API_STATUS_SZ + sizeof(struct APIParameterName);
		getNames = p->next;
		GS_FREE( p );
	} else {
		mp->msize = API_STATUS_SZ;
	}
	mp->status = API_SESS_OK;
	DBGPRINT((stderr, "runGetNextParameterName API status=%d\n", mp->status));
	return mp;
}

static int getPNames(CWMPObject *o, CWMPParam *p, Instance *ip,
						void *wxp, void *paramCnt, int unused ) {
	LinkedNames *lnp = GS_MALLOC(sizeof (struct LinkedNames) );
	if ( lnp != NULL){
		strncpy(lnp->paramNameData.paramName, wpPathName, TR69NAMESZ);
		if (!p)
			lnp->paramNameData.writeable = o->pAddObj!=NULL && o->oType==eInstance;  /* must be an object */
		else
			lnp->paramNameData.writeable = p->setable==RPC_RW;
		lnp->next = getNames;
		getNames = lnp;
		if (paramCnt) (*(int*)paramCnt)++;
		return eOK;
	}
	return eStopWalk;
}

static CPEAPIMessage *runAPIGetParameterNames(CPEAPIMessage *mp) {
	CWMPParam *p;
	CWMPObject *o;
	Instance *ip;
	/* this function uses the request buffer to write the response into */
	const char *pPath = mp->ud.apiGetParameterNames.pathName;
	int nextLevel = mp->ud.apiGetParameterNames.nextLevel;
    CNTRL_MASK descend = nextLevel!=1?DESCEND|CBPARAMS|CBOBJECTS: CBPARAMS|CBOBJECTS;
    int		paramCnt = 0;	 /* count of parameters to return */
	int fault = 0;
	DBGPRINT((stderr, "runAPISetParameterValue API: %s\n", pPath ));
	freeLinkedNames();
    cwmpFrameworkReset();
    if (pPath == NULL || strlen(pPath)==0 ) {
        /* name is empty- use root of parameter tree */
        o = CWMP_RootObject;
        if ( !(descend&DESCEND) ){ /* only top level object if NextLevel=1*/
        	strcpy(wpPathName, o->name);
        	strcat(wpPathName, ".");
        	getPNames(o, NULL, NULL, NULL, &paramCnt, 0);
        } else
        	cwmpWalkPTree(descend, o, NULL, getPNames, NULL, &paramCnt, 0);
    } else if ( pPath[strlen(pPath)-1]=='.' ){ /* ends with . ? */
        if ( (o = cwmpFindObject(pPath)) ){
        	if ( isdigit(pPath[strlen(pPath)-2]) )
        		ip = cwmpGetCurrentInstance();
        	else
        		ip = NULL;
       		if ( descend&DESCEND )  /* if DESCEND (nextLevel=0) include this object*/
       			getPNames(o, NULL, ip, NULL, &paramCnt, 0);
            cwmpWalkPTree(descend, o, ip, getPNames, NULL, &paramCnt, 0);
        }
        else
            fault = 9005; /* parameter name not found */
    } else {/* must be a simple parameter */
        if ( (p = cwmpFindParameter(pPath)) ){
        	getPNames(NULL, p, NULL, NULL, &paramCnt, 0);
        } else
            fault = 9005; /* 9005 parameter name not found */
    }
	mp->msize = API_STATUS_SZ+sizeof(struct APIGetParameterNamesCnt);
	mp->status = fault;
	mp->ud.apiGetParameterNamesCnt.nameCount = paramCnt;
	DBGPRINT((stderr, "runGetParameterNames API status=%d, cnt=%d\n", mp->status, paramCnt));
	return mp;
}

static int fcbfunc(CWMPObject *o, CWMPParam *p, Instance *ip,
						void *targetPname, void *targetValue, int unusedInt )
{
	char *value;
	if (p && strcmp(targetPname, p->name)==0) {
		/* found name-- get value */
		//DBGPRINT((stderr, "fcbfunc found pname-%s\n", p->name));
		if (p->pGetter && p->pGetter(ip, &value)== CPE_OK) {
			if (streq(value, targetValue) ) {
				FREESTR(value);
				return eStopWalk; /* stop walk here */
			}
			FREESTR(value);
		}
	}
	return eOK; /* keep walking */
}

static CPEAPIMessage *runApiFindObjectPath(CPEAPIMessage *mp) {
	int fault = 0;
	char *oname = mp->ud.apiFindObjectPath.objName;
	char *pname = mp->ud.apiFindObjectPath.paramName;
	char *pvalue = mp->ud.apiFindObjectPath.paramValue;
	if (strcmp(oname,".")==0)
		oname = NULL;
	char *path = cwmpFindPath( oname, fcbfunc, pname, pvalue, 0);
	if ( path ) { /* a null return is valid */
		/* drop parameter name from path */
		char *e;
		e = strrchr(path, '.' );
		if ( e )
			*(e+1) = '\0';
		strcpy(mp->ud.apiObjectPath.path, path);
		GS_FREE(path);
		mp->msize = API_STATUS_SZ + sizeof(struct APIObjectPath);
	} else {
		fault = 9005;
		mp->msize = API_STATUS_SZ;
	}
	mp->status = fault;
	DBGPRINT((stderr, "runApiFindObjectPath API status=%d\n", mp->status));
	return mp;
}
/*
 * This queues a GetRPCMethods RPC for the next
 * ACS session. Used for testing ACS functionality.
 * Return is always ok.
 */

static CPEAPIMessage *runAPIGetRPCMethods( CPEAPIMessage *mp ) {
	cwmpAddEvent(eEvtConnectRequest);
	cwmpSetPending(PENDING_GETRPC);
	mp->msize = API_STATUS_SZ;
	mp->status = 0;
	DBGPRINT((stderr, "runAPIGetRPCMethods status=%d\n", mp->status));
	return mp;
}

/*
 * This is a callback from cwmpWalkPTree in runApiDeleteObject.
 * It is called for each sub-object of the object being deleted.
 * Call the objects CPE del object function if
 * it is not null.
 */
static int delSubObjects(CWMPObject *o, CWMPParam *p, Instance *unusedip,
void *unusedvp, void *unusedvp2, int unused ) {
	Instance *next, *tp;
	Instance *parent = cwmpGetCurrentInstance();
    tp = o->iList;
    while ( tp ){
    	next = tp->next;
    	if ( tp->parent == parent ){
    		if ( o->pDelObj )
    			o->pDelObj( o, tp);
    		else
    			cwmpDeleteInstance( o, tp);
    	}
    	tp = next;
    }
    return eOK;
}

static CPEAPIMessage *runApiDeleteObject(CPEAPIMessage *mp) {
	/* this function uses the request buffer to write the response into */
	const char *pn = mp->ud.apiAddDeleteObject.pathName;
	int fault = 0;
	if (pn && strlen(pn) > 0) {
		CWMPObject *o = cwmpFindObject(pn);
		if (o) {
			if (o->pDelObj) {
				Instance *ip = cwmpGetCurrentInstance();
				cwmpWalkPTree(DESCEND | CBOBJECTS, o, ip, delSubObjects, NULL,
						NULL, 0);
				fault = o->pDelObj(o, ip);
				if (fault == CPE_ERR)
					fault = 9002; /* internal fault */
			} else
				fault = 9003; /* invalid arguments */
		} else
			fault = 9005; /* invalid parameter name */
	} else
		fault = 9003; /* invalid arguments */
	mp->msize = API_STATUS_SZ;
	mp->status = fault;
	DBGPRINT((stderr, "runDeleteObject API status=%d\n", mp->status));
	return mp;
}

static CPEAPIMessage *runApiAddObject(CPEAPIMessage *mp) {
	/* this function uses the request buffer to write the response into */
	const char *pn = mp->ud.apiAddDeleteObject.pathName;
	int fault;
	Instance *ip = NULL;
	int lth;
	char buf[TR69NAMESZ];
	DBGPRINT((stderr, "runAddObject API pname=%s\n", pn ));
	lth = strlen(pn);
	if (pn && lth > 0) {
		CWMPObject *o = cwmpFindObject(pn);
		if (o) {
			if (o->pAddObj && o->oType == eInstance) {
				if ( (ip = cwmpCreateInstance(o, -1))){
					fault = o->pAddObj(o, ip);
					if (fault == CPE_ERR)
						fault = 9002; /* internal fault */
					else {
						/* add new instance number string to path */
						snprintf(buf, sizeof(buf), "%s%d.", mp->ud.apiAddDeleteObject.pathName, ip->id);
						strncpy(mp->ud.apiAddDeleteObject.pathName, buf, sizeof(TR69NAMESZ));
					}
				} else
					fault = 9004; /* resources exceeded */
			} else
				fault = 9005; /* non-writable not a valid object */
		} else
			fault = 9005; /* invalid parameter name */
	} else
		fault = 9003; /* invalid arguments */
	mp->msize = API_STATUS_SZ + sizeof(struct APIAddDeleteObject);
	mp->status = fault;
	DBGPRINT((stderr, "runAddObject API status=%d\n", mp->status));
	return mp;
}

static CPEAPIMessage *runApiCommit(CPEAPIMessage *mp) {
	CWMPObject *o;
	/* this function uses the request buffer to write the response into */
	const char *pn = mp->ud.apiCommit.objPath;
	int fault = 0;
	DBGPRINT((stderr, "runApiCommit API pname=%s\n", pn ));

	if ((o = cwmpFindObject(pn))) {
		if (o->pCommitObj) {
			fault = o->pCommitObj(o, cwmpGetCurrentInstance(), COMMIT_SET);
		} else
			fault = 9001; /* request denied */
	} else
		fault = 9005; /* invalid name */
	mp->msize = API_STATUS_SZ;
	mp->status = fault;
	DBGPRINT((stderr, "runCommit API status=%d\n", mp->status));
	return mp;
}

static CPEAPIMessage *runSetParameterValue(CPEAPIMessage *mp) {
	CWMPParam *p;
	/* this function uses the request buffer to write the response into */
	const char *pn = mp->ud.apiSetParameterValue.paramName;
	const char *pv = mp->ud.apiSetParameterValue.paramValue;
	int fault = 0;
	DBGPRINT((stderr, "runSetParameterValue API pname=%s\n", pn ));

	if ((p = cwmpFindParameter(pn))) {
		if (p->setable == RPC_RW) {
			fault = p->pSetter(cwmpGetCurrentInstance(), pv);
		} else
			fault = 9008; /* parameter not writeable */
	} else
		fault = 9005; /* invalid name */
	mp->msize = API_STATUS_SZ;
	mp->status = fault;
	DBGPRINT((stderr, "runSetParameterValue API status=%d\n", mp->status));
	return mp;
}

static CPEAPIMessage *runGetParameterValue(CPEAPIMessage *mp) {

	CWMPParam *p;
	/* this function uses the request buffer to write the response into */
	APIParameterName *pn = &mp->ud.apiParameterName;
	APIParameterValue *pv = &mp->ud.apiParameterValue;
	char *value = NULL;
	int fault = 0;
	DBGPRINT((stderr, "runGetParameterValue API pname=%s\n", pn->paramName));

	if ((p = cwmpFindParameter(pn->paramName))) {
		if (p->pGetter) {
			fault = p->pGetter(cwmpGetCurrentInstance(), &value);
			if (fault == CPE_OK) {
				if (value) { /* a null return is valid */
					strcpy(pv->paramValue, value);
					GS_FREE(value);
				} else
					pv->paramValue[0] = '\0';
				pv->ptype = p->pType;
				mp->msize = API_STATUS_SZ + sizeof(struct APIParameterValue);
			}
		} else
			fault = 9005; /* invalid parameter name */
	} else
		fault = 9005;
	if (fault != 0) {
		mp->msize = API_STATUS_SZ;
	}
	mp->status = fault;
	DBGPRINT((stderr, "runGetParameterValue API status=%d\n", mp->status));
	return mp;
}

void cpeReboot(void);

static CPEAPIMessage *runAPIReboot(CPEAPIMessage *mp) {
	mp->msize = API_STATUS_SZ;
	mp->status = 0;
	DBGPRINT((stderr, "runstatus=%d\n", mp->status));
	/* the following call does not return */
	cpeReboot();      /* This is implemented in CPEWrapper.c */
	return mp;

}

void cpeFactoryReset(void);

static CPEAPIMessage *runAPIReset(CPEAPIMessage *mp) {
	mp->msize = API_STATUS_SZ;
	mp->status = 0;
	DBGPRINT((stderr, "runAPIReset\n"));
	/* the following call does not return */
	cpeFactoryReset();      /* This is implemented in CPEWrapper.c */
	return mp;

}
/*
 * called following the end of a clientapi session.
 */
extern void *acsSession;
static void startInformSessions(void *handle){
	stopCallback(&acsSession, startInformSessions, handle);
	cwmpStartACSInform();
}

static CPEAPIMessage *runSetACSSession( CPEAPIMessage *mp ){
	int newstate = mp->ud.apiSetAcsSessions.enable;
	mp->msize = API_STATUS_SZ;
	mp->status = 0;
	DBGPRINT((stderr, "runSetACSSession: %s\n", newstate? "Enabled": "Disabled"));
	if ( newstate==1 && cpeState.acsContactState!=ACS_SESSION_ENABLED){
		/* enable CWMP sessions with ACS */
		cpeState.acsContactState = ACS_SESSION_ENABLED;
		setCallback(&acsSession, startInformSessions, NULL);
	} else if ( newstate ==0 && cpeState.acsContactState==ACS_SESSION_ENABLED){
		cpeState.acsContactState = ACS_SESSION_DISABLED;
	}
	return mp;
}

static void runAPIRequest(APIMsgDope *from) {
	CPEAPIMessage *cpeMp = &from->msg;
	CPEAPIMessage *top; /* to client message pointer */
	switch (cpeMp->mtype) {
	case API_GETPARAMETERVALUE:
		top = runGetParameterValue(cpeMp);
		break;
	case API_SETPARAMETERVALUE:
		top = runSetParameterValue(cpeMp);
		break;
	case API_COMMIT:
		top = runApiCommit(cpeMp);
		break;

	case API_ADDOBJECT:
		top = runApiAddObject(cpeMp);
		break;
	case API_DELETEOBJECT:
		top = runApiDeleteObject(cpeMp);
		break;
	case API_FINDOBJECTPATH:
		top = runApiFindObjectPath(cpeMp);
		break;
	case API_GETRPCMETHODS:
		top = runAPIGetRPCMethods( cpeMp );
		break;

	case API_GETPARAMETERNAMES:
		top = runAPIGetParameterNames( cpeMp );
		break;
	case API_GETNEXTPARAMNAME:
		top = runGetNextParameterName( cpeMp );
		break;
	case API_REBOOT:
		top = runAPIReboot( cpeMp );
		break;

	case API_RESET:
		top = runAPIReset( cpeMp );
		 break;
	/******
		 case API_AUTOTRANSFERCOMPLETE:
		 break;
		 case API_REQUESTDOWNLOAD:
		 break;
		 *****/
	case API_SETACSSESSIONS:
		top = runSetACSSession( cpeMp );
		break;
	default:
		top = cpeMp;
		top->status = API_SESS_MSGERR;
		top->msize = API_STATUS_SZ;
		break;
	}
	replyMessage(from);
}

static void replyMessage(APIMsgDope *from) {
	CPEAPIMessage *cpeMp = &from->msg;
	sendto(from->fd, cpeMp, cpeMp->msize, 0, &from->from, from->addrlen);
	return;
}
/*
 * build message and send to socket.
 */
static void replyStatus(APIMsgDope *from, int status) {

	CPEAPIMessage *cpeMp = &from->msg;
	cpeMp->status = status;
	cpeMp->msize = API_STATUS_SZ;
	sendto(from->fd, cpeMp, API_STATUS_SZ, 0, &from->from, from->addrlen);
	return;
}

extern void *acsSession;
static void sessionWrapup(void){
	freeLinkedNames();
	cwmpSetSessionBlock(UNBLOCK_NEW_SESSION);
	/* start any post session functions that need to run */
	notifyCallbacks( &acsSession );
	/* see if any parameters changed that have notification */
	cwmpCheckValueChange();    /* start session if any active value change */
	cwmpStartPending();        /* start session is any pending actions */
}


static void sessionTimeout(void *handle) {
	sessActive = 0;
	sessionWrapup();
	DBGPRINT((stderr, "API session timeout: session forced end\n"));
}

static void cpeAPIMsgRead(void *handle) {
	APIMsgDope apiMsgDope;
	apiMsgDope.fd = (int) handle;
	CPEAPIMessage *mp = &apiMsgDope.msg;

	apiMsgDope.addrlen = sizeof( apiMsgDope.from );

	if (recvfrom(apiMsgDope.fd, mp, sizeof(struct CPEAPIMessage), 0,
			&apiMsgDope.from, (socklen_t * __restrict__)&apiMsgDope.addrlen) > 0) { //-Werror=pointer-sign
		DBGPRINT((stderr, "cpeAPIMsgRead called: event=%0d\n", mp->mtype));
		if (mp->mtype == API_STARTSESSION) {
			if (sessActive != 0) {
				/* API session is already in progress */
				replyStatus(&apiMsgDope, API_SESS_ACTIVEERR);
				DBGPRINT((stderr, "API_STARTSESSION: API Session in progress\n"));
			} else if (cwmpIsACSSessionActive()) {
				/* ACS session is active */
				replyStatus(&apiMsgDope, API_SESS_ACSACTIVE);
				DBGPRINT((stderr, "API_STARTSESSION: ACS Session in progress\n"));
			} else if ( cwmpSetSessionBlock(BLOCK_NEW_SESSION) == -1){
				/* ACS session is in unknown state. */
				replyStatus(&apiMsgDope, API_SESS_ACSACTIVE);
				DBGPRINT((stderr, "API_STARTSESSION: ACS Session is in unknown state\n"));
			} else {
				/* start an API session */
				/* TODO: set the cwmpSession wio pointer to -1 here */
				sessionTime = mp->ud.apiStartSessionParam.maxSessionTime;
				if (sessionTime != 0)
					setTimer(sessionTimeout, NULL, sessionTime * 1000);
				sessActive = 1;
				sessSeq = mp->mseq;
				DBGPRINT((stderr, "API_STARTSESSION: API session started\n"));
				replyStatus(&apiMsgDope, API_SESS_OK);
			}
		} else if (mp->mtype == API_ENDSESSION) {
			sessActive = 0;
			/* TODO: schedule any pending sessions */
			replyStatus(&apiMsgDope, API_SESS_OK);
			if (sessionTime) {
				stopTimer(sessionTimeout, NULL);
				sessionTime = 0;
			}
			sessionWrapup();
			DBGPRINT((stderr, "API_ENDSESSION: API session ended\n"));
		} else if (mp->mtype == API_STOPCWMPC) {
			/* overrides active session if set */
			replyStatus(&apiMsgDope, API_SESS_OK);
			closeAllFds();
			stopAllTimers();
		}else {
			if (sessActive == 0) {
				replyStatus(&apiMsgDope, API_SESS_ACTIVEERR);
				DBGPRINT((stderr, "API session not active\n"));
			} else
				/* API session is active */
				runAPIRequest(&apiMsgDope);
		}

	} else {
		cpeLog(LOG_ERR, "recvfrom failed %s", strerror(errno));
	}
	return;
}
/*
 * Start listener on listenPort
 * Returns: -1: error
 *
 */
int cwmpStartCPEAPIListener(void) {
	int fd;
	InAddr anyAddr= {AF_INET, {{htonl(INADDR_ANY)}}};
	if ((fd = udp_listen(&anyAddr, CPE_CLIENTAPI_PORT, NULL, FALSE)) < 0) {
		cpeLog(
				LOG_ERR,
				"could not initiate CPEAPI listener on UDP server socket (port=%d):%s",
				CPE_CLIENTAPI_PORT, strerror(errno));
		return -1;
	}
	setListener(fd, cpeAPIMsgRead, (void *) fd);
	return 0;
}
