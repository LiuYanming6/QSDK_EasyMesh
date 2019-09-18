/*----------------------------------------------------------------------*
 * Gatespace Networks, Inc.
 * Copyright 2005-2012 Gatespace Networks, Inc., All Rights Reserved.
 * Gatespace Networks, Inc. confidential material.
 *----------------------------------------------------------------------*
 * File Name  : rpcUtils.c
 *
 * Description: SOAP RPC utility routines for handling data structures
 * $Revision: 1.39 $
 * $Id: rpcUtils.c,v 1.39 2012/06/13 12:01:34 dmounday Exp $
 *----------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>
#include <net/if.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <netdb.h>
#include <time.h>
#include <syslog.h>

#ifdef DMALLOC
#include "dmalloc.h"
#endif

#include "rpcUtils.h"

#include "../includes/CPEWrapper.h"
//#include "../testCPE/strconst.h"

//#define DEBUG
#ifdef DEBUG
#define DBGPRINT(X) fprintf X
#else
#define DBGPRINT(X)
#endif
#define MAX_INSTANCE_DEPTH 20
extern CWMPObject CWMP_RootObject[];
extern CPEState cpeState;

static void createIData(CWMPObject *start, Instance *ip);
/*
 * cwmpRunState
 */
int	cwmpRunState;
void cwmpSetRunState(int state){
	cwmpRunState = state;
}
int cwmpGetRunState(void){
	return cwmpRunState;
}

static int findStatus;

/* The instance stack is part of the cwmp framework state*/
/* and is maintained by the parameter tree searching and */
/* traversal functions */
/* bottom of stack is NULL */
static Instance *iStack[MAX_INSTANCE_DEPTH]= { NULL };
static int iStackTop=0;

void cwmpPushInstance(Instance *ip) {
	iStack[++iStackTop] = ip;
}

Instance *cwmpPopInstance(void) {
	if (iStackTop==0) { /* test for underflow */
		cpeLog(LOG_ERR,"cwmpPopInstance stack underrun\n");
		return NULL;
	} else	if ( iStack[iStackTop] == (void *)(-1))
		cpeLog(LOG_ERR, "Stack Marker popped from stack");
	return iStack[iStackTop--];
}

Instance *cwmpTopInstance(void) {
	return iStack[iStackTop];
}

CPE_STATUS cwmpGetAliasParam(Instance *ip, char **value){
	if ( ip->alias )
		*value = GS_STRDUP(ip->alias);
	return CPE_OK;
}
CPE_STATUS cwmpSetAliasParam(Instance *ip, const char *value){
	COPYSTR(ip->alias, value);
	return CPE_OK;
}
/*
 * resets all the framework state
 */
void cwmpFrameworkReset(void) {
	findStatus = 0;
	iStackTop=0; // underflow marker.
	iStack[0]=NULL;
}

Instance *cwmpGetCurrentInstance(void) {
	return cwmpTopInstance();
}

static CWMPObject *cwmpCurrentObject;
CWMPObject *cwmpGetCurrentObject(void) {
	return cwmpCurrentObject;
}
static CWMPParam *cwmpCurrentParam;

CWMPParam *cwmpGetCurrentParam(void) {
	return cwmpCurrentParam;
}
/*
 * Search CWMPObject *obj for the object by name.
 * Returns CWMPObject *
 *        NULL:         Not found
 */
CWMPObject *cwmpSrchOList(CWMPObject *obj, const char *name) {
	CWMPObject *o = obj;

	while ( o->name) {
		if (strcmp(name, o->name)==0)
			return o;
		++o; /* increment to next parameter */
	}
	return NULL;
}
/*
 * assumes that it is called from a framework
 * callback and that the cwmpCurrentObject is
 * set.
 */
int cwmpGetNumberOfInstances(CWMPObject *o, Instance *path) {
	int n = 0;
	if (o) {
		Instance *ip = o->iList;
		while (ip) {
			if (ip->id!=0 && ip->parent==path)
				++n;
			ip = ip->next;
		}
	}
	return n;
}

/*
 * May only be called from getter that is called from
 * within the execution of the framework.
 * Used by callbacks to retrieve the instance count string.
 * If not called by callbacks from within framework the
 * allocated string (*value) must be freed by caller.
 */
void cwmpGetObjInstanceCntStr(const char *oname, char **value) {
	int			n=0;
	CWMPObject *o = cwmpGetCurrentObject();
	o = o->objList;  /* search in current obj's objList */
	if (o && (o = cwmpSrchOList(o, oname))) {
		n = cwmpGetNumberOfInstances(o, cwmpTopInstance());
	}
	*value = GS_STRDUP( itoa(n));
}
/*
 * Get the IData for a parameter. There is a IData structure
 * for each valid instance path to this parameter.
 * I.e. A.1.B.2.p
 *      A.2.B.2.p
 *
 */
IData *cwmpGetIData(CWMPParam *p, Instance *ip) {
	IData *idp;
	if (p) {
		idp = p->instanceData;
		while (idp) {
			if (ip==NULL || idp->instance == ip)
				return idp;
			idp = idp->next;
		}
	}
	return NULL;
}

/*
 * Global string for walkTree; contains the current parameter path name upon callback.
 *
 */
char wpPathName[512];

void cwmpFreeRPCRequest(RPCRequest *r) {
	if (r->ID)
		GS_FREE(r->ID);

	switch (r->rpcMethod) {
	case eGetParameterNames:
		GS_FREE(r->ud.getPNamesReq.pNamePath);
		break;
	case eGetParameterValues:
	case eGetParameterAttributes: {
		ParameterNameStruct *p = r->ud.getPAVReq.pnameList;
		while (p) {
			ParameterNameStruct *tmp = p->next;
			GS_FREE(p->pName);
			GS_FREE(p);
			p = tmp;
		}
		break;
	}
	case eSetParameterValues: {
		SetParameterValuesMsg *spvm = &r->ud.setPValuesReq;
		ParameterValueStruct *p = r->ud.setPValuesReq.pvList;
		while (p) {
			ParameterValueStruct *tmp = p->next;
			GS_FREE(p->pName);
			GS_FREE(p->value);
			if (p->origValue)
				GS_FREE(p->origValue);
			GS_FREE(p);
			p = tmp;
		}
		ParameterNameStruct *pn = spvm->autoAdd;
		while ( pn ){
			ParameterNameStruct *nxt = pn->next;
			GS_FREE(pn->pName);
			GS_FREE(pn);
			pn = nxt;
		}
		break;
	}
	case eSetParameterAttributes: {
		ParameterAttributesStruct *p = r->ud.setPAttrReq.paList;
		while (p) {
			ParameterAttributesStruct *tmp = p->next;
			GS_FREE(p->pName);
			GS_FREE(p);
			p = tmp;
		}
		break;
	}
	case eAddObject:
	case eDeleteObject:
		GS_FREE(r->ud.addDelObjectReq.objectPath);
		break;
	case eUpload:
	case eDownload:
	case eScheduleDownload:
		if (r->ud.downloadReq.fileType)
			GS_FREE(r->ud.downloadReq.fileType);
		if (r->ud.downloadReq.URL)
			GS_FREE(r->ud.downloadReq.URL);
		if (r->ud.downloadReq.userName)
			GS_FREE(r->ud.downloadReq.userName);
		if (r->ud.downloadReq.passWord)
			GS_FREE(r->ud.downloadReq.passWord);
		if (r->ud.downloadReq.targetFileName)
			GS_FREE(r->ud.downloadReq.targetFileName);
		if (r->ud.downloadReq.successURL)
			GS_FREE(r->ud.downloadReq.successURL);
		if (r->ud.downloadReq.failureURL)
			GS_FREE(r->ud.downloadReq.failureURL);
#ifdef CONFIG_RPCSCHEDULEDOWNLOAD
		if (r->ud.downloadReq.timeWindow[0].userMessage)
			GS_FREE(r->ud.downloadReq.timeWindow[0].userMessage);
		if (r->ud.downloadReq.timeWindow[1].userMessage)
			GS_FREE(r->ud.downloadReq.timeWindow[1].userMessage);
#endif

		break;
#ifdef CONFIG_RPCCHANGEDUSTATE
	case eChangeDUState: {
		DUOperationStruct *p = r->ud.changeDUState.opList;
		while (p) {
			DUOperationStruct *tmp = p->next;
			GS_FREE(p->uuid);
			GS_FREE(p->version);
			GS_FREE(p->execEnvRef);
			GS_FREE(p->url);
			GS_FREE(p->userName);
			GS_FREE(p->passWord);
			GS_FREE(p);
			p = tmp;
		}
		break;
	}
#endif /* CONFIG_RPCCHANGEDUSTATE */
	case eInformResponse:
	case eFactoryReset:
	case eFault:
		if (r->ud.faultMsg.faultString)
			GS_FREE(r->ud.faultMsg.faultString);
		break;
	case eTransferCompleteResponse:
		break;
#ifdef CONFIG_RPCKICK
	case eKickedResponse:
		if (r->ud.kickedResponseReq.nextURL)
			GS_FREE(r->ud.kickedResponseReq.nextURL);
		break;
#endif
#ifdef CONFIG_SETVOUCHERS
	case eSetVouchers:
		Voucher *p = r->ud.setVouchersReq.voucherList;
		while (p) {
			Voucher *tmp = p->next;
			GS_FREE(p->pVoucher);
			GS_FREE(p);
			p = tmp;
		}
		break;
	case GetOptions:
		GS_FREE(r->ud.getOptionsReq.pOptionName);
		break;

#endif
	case eUnspecified:
	default:
		break;
	}
	GS_FREE(r);
	return;
}
/*
 * deleteIData
 *   Delete the IData associated with each instance of a parameter for this instance.
 */
static void deleteIData(CWMPObject *o, Instance *ip) {
	IData *idp;
	CWMPParam *pp = o ? o->paramList : NULL;

	while (pp && pp->name) {
		if ( (idp=pp->instanceData)) {
			IData **lastIdp = &pp->instanceData;
			while (idp) {
				if (idp->instance == ip) {
					*lastIdp = idp->next;
					if (idp->dataCopy)
						GS_FREE(idp->dataCopy);
					GS_FREE(idp);
					break;
				}
				lastIdp = &idp->next;
				idp = idp->next;
			}
		}
		++pp;
	}
	return;
}
/*
 * Scan CWMPObject paramList and return CWMPParam pointer
 * if parameter name is "Alias";
 * Otherwise return NULL.
 */
static CWMPParam *findAliasParam(const CWMPObject *o){
	CWMPParam *p = o->paramList;
	while ( p && p->name ){
		if ( streq(p->name, "Alias"))
			return p;
		p++;
	}
	return NULL;
}

static void createChildObjectInstance(CWMPObject *start, Instance *iPath);

static Instance *initInstance(CWMPObject *start, Instance *iPath, int id){
	Instance *ip;
	if ( (ip = (Instance *)GS_MALLOC(sizeof(Instance)))) {
		memset(ip, 0, sizeof(Instance));
		ip->next = start->iList; /* link up new instance to object iList */
		start->iList = ip;
	} else
		return NULL;    /* this is an error */
	ip->id = id;
	ip->parent = iPath;
	createIData(start, ip);
	return ip;
}
/*
 * createChildObjectInstances
 * 	start is the object to check for child objects.
 * 	iPath is the parent Instance.
 * 	This only creates the un-named instances of CWMPObject(s).
 */
static void createChildObjectInstance(CWMPObject *start, Instance *iPath){
	CWMPObject *o = start->objList;
	while (o && o->name){
		if ( o->oType == eObject ){
			Instance *ip = initInstance(o, iPath, 0);
			if ( ip && o->pAddObj ){     /* call the instance addObj function if defined*/
				o->pAddObj(o, ip);
				DBGPRINT((stderr,"pAddObj(): %s%d.\n", o->n, ip->id));
			}
			createChildObjectInstance(o, ip);
		}
		++o;
	}
}
/*
 * createInstance
 *	Initialize all the initial instance for all the eObjects and
 *   parameter IData in the subtree identified by start. eInstance
 *   sub-objects of start are not traversed.
 *
 *
 * CWMPObject *o: pointer to object to link to instance.
 * Instance   *iPath: pointer to parent instance.
 * int        id:  id value for new instance.
 * 					if id == -1 then generate a new instance id number
 */
static Instance *createInstance(CWMPObject *o, Instance *iPath, int id, CPE_STATUS *sp) {

	Instance *ip;
	if ( (ip = initInstance( o, iPath, id==-1? cpeGetInstanceID(): id))== NULL ){
		if (sp) *sp = CPE_9004;
		return NULL; /* this is an error */
	}
	createChildObjectInstance(o, ip);
	if ( findAliasParam(o) ){
		char buf[65];
		snprintf(buf, sizeof(buf), "cpe-%d", ip->id);
		cwmpSetAliasParam(ip, buf);
	}
	if ( o->pAddObj ){     /* call the instance addObj function if defined*/
		CPE_STATUS s = o->pAddObj(o, ip);
		if ( sp ) *sp = s;
		DBGPRINT((stderr,"pAddObj(): %s%d.\n", o->n, ip->id));
		if ( s > CPE_REBOOT ){
			/* error need to clean up */
			cwmpDeleteObjectInstance(o, ip);
		}
	}
	return ip;
}

/*
 * cwmpCreateInstance
 *  Must be called from within framework. Typically by a CPE addXXXX
 *  callback function. Gets Instance path from top of Instance stack.
 *	Initializes all the initial instance for all the eObjects and
 *   parameter IData in the subtree identified by start. eInstance
 *   sub-objects of start are not traversed.
 *
 *
 * CWMPObject *o: pointer to object to link instance.
 * int        id:  id value for new instance.
 * 					if id == -1 then generate a new instance id number
 */
Instance *cwmpCreateInstance(CWMPObject *o, int id) {
	return createInstance(o, cwmpTopInstance(), id, NULL);
}

/*
 * isolate object path and alias if present.
 * return true if alias present. object is not set if false.
 */
static int isAliasPath( const char *path, char *object, char *alias ){
	int	j = strlen(path);
	if (path[j-1]=='.') {
		/* last char is . */
		int i = j-2; // check char before '.'
		if ( path[i] == ']'){
			/* alias present */
			int k;
			for (k = i; k>0; --k)
				if ( path[k]=='[')
					break;
			strncpy( object, path, k);
			object[k]='\0';
			strncpy( alias, path+k+1, i-k-1);
			alias[i-k-1]='\0';
			return 1;
		}
	}
	return 0;
}
/*
 * cwmpAddObjectInstance
 * param const char *p points to the object fragment such as A.B. or
 * to an Alias that should be used to create the Instance, A.B.[wan].
 * The instance is created and assigned the next instance ID. If the
 * alias is specified it is assigned to Instance.alias.
 *
 * If any pAddObj errors occur the instance is removed.
 * Return is the CPE_STATUS of the pAddObj function.
 */
CPE_STATUS cwmpAddObjectInstance( const char *p, Instance **ipp ){
	CPE_STATUS s;
	char opath[257];
	char alias[65];
	CWMPObject *o;
	int	 b;
	if ( (b = isAliasPath(p, opath, alias ))){
		o = cwmpFindObject( opath );
		if ( cwmpSrchPList(o, "Alias")==NULL){ // check for Alias parameter
			o = NULL; // force return status to 9005.
		}
	} else {
		o = cwmpFindObject( p );
	}
	if ( o && o->pAddObj && o->oType== eInstance ){
		*ipp = createInstance(o, cwmpTopInstance(), -1, &s);
		if ( *ipp && b ){
			cwmpSetAliasParam(*ipp, alias);
		}
	} else {
		s = CPE_9005;
		*ipp = NULL;
	}
	return s;
}

/* cwmpDeleteInstance
 *  Delete the instance pointed to by *ip and any
 *  associated IData for any of the parameter instances.
 */
void cwmpDeleteInstance(CWMPObject *o, Instance *ip) {
	Instance *p;
	Instance **last = &o->iList;

	p = o->iList;
	while (p) {
		if (p == ip) {
			*last = p->next;
			deleteIData(o, p);
			GS_FREE(p->alias);
			GS_FREE(p);
			break;
		}
		last = &p->next;
		p = p->next;
	}
}
/*
 * cwmpDeleteAllInstances
 * Delete all the child instances of an object.
 */
void cwmpDeleteAllInstances(CWMPObject *o) {
	Instance *p;
	if ( ISINSTANCE(o) ){
		while ( (p=o->iList )){
			cwmpDeleteInstance(o, p);
		}
	}
}
/*
 * cwmpDeleteChildInstances
 * Delete child instance for this objects current path.
 */
void cwmpDeleteChildInstances(CWMPObject *o, Instance *parent){
	Instance *p;
	if ( ISINSTANCE(o) ){
		p = o->iList;
		while ( p ) {
			if ( p->parent == parent ){
				Instance *next = p->next;
				if ( o->pDelObj )
					o->pDelObj(o, p);
				cwmpDeleteInstance(o, p);
				p = next;
			} else {
				p = p->next;
			}
		}
	}
}
/*
 * This is a callback from cwmpWalkPTree in runDeleteObject.
 * It is called for each sub-object of the object being deleted.
 * Call the objects CPE del object function if
 * it is not null.
 */
static int delChildObjInstances(CWMPObject *o, CWMPParam *p, Instance *unusedip,
		void *unusedvp, void *unusedvp2, int unused) {
	Instance *next, *tp;
	Instance *parent = cwmpGetCurrentInstance();
	tp = o->iList;
	while (tp) {
		next = tp->next;
		if (tp->parent == parent) {
			if (o->pDelObj)
				o->pDelObj(o, tp);
			cwmpDeleteInstance(o, tp);
		}
		tp = next;
	}
	return eOK;
}
/*
 * Delete the CWMPObject instance identified by
 * the CWMPObject pointer 'start' and Instance 'ip'.
 * All child objects and instances are also removed.
 * Each pDelObj function is called if present in the child object CWMPObject.
 */
CPE_STATUS cwmpDeleteObjectInstance(CWMPObject *start, Instance *ip){
	CPE_STATUS cpeStatus = CPE_OK;
	cwmpWalkPTree(DESCEND | CBOBJECTS, start, ip, delChildObjInstances,
			NULL, NULL, 0);
	if ( start->pDelObj ) {
		cpeStatus = start->pDelObj(start, ip);
	}
	cwmpDeleteInstance(start, ip);
	return cpeStatus;
}
/*
 * oName: pointer to child object name.
 * Return pointer to child object CWMPObject structure.
 * Uses cwmpCurrentObject as the parent object.
 */
CWMPObject *cwmpGetChildObject(const char *oName){
	CWMPObject *o = cwmpCurrentObject->objList;
	while ( o && o->name){
		if (strcmp(oName,o->name)==0 ){
			return o;
		}
		++o;
	}
	return NULL;
}
/*
 * cwmpGetChildInstance
 * Return the child object instance for the named object and parent Instance.
 */
Instance *cwmpGetChildInstance(const char *oName, const Instance *parent){
	CWMPObject *o = cwmpCurrentObject->objList;
	Instance *ip;
	while ( o && o->name){
		if (strcmp(oName,o->name)==0 ){
			ip = o->iList;
			while ( ip ){
				if (ip->parent==parent)
					return ip;
				ip = ip->next;
			}
			break;
		}
		++o;
	}
	return NULL;
}
/*
 * cwmpGetNextSiblingInstance
 * Return the next sibling Instance of the instance. Used following
 * cwmpGetChildInstance(). The instance list must not be changed between calls.
 */
Instance *cwmpGetNextSiblingInstance(const Instance *instance){
	Instance *parent = instance->parent;
	Instance *ip = instance->next;
	while ( ip ){
		if (ip->parent==parent)
			return ip;
		ip = ip->next;
	}
	return NULL;
}
/*
 * Create and initialize a new instance of the target object.
 * The path is not necessarily the direct parent but may be a
 * grand parent, etc. It is an instance of the starting object.
 * The object tree is walked starting at the start
 * object to find and build an instance path to the new
 * target object.
 * start specifies the object that contains the list of
 * objects to search for the parent object.
 */
Instance *cwmpCreateInitInstance(CWMPObject *start, Instance *path,
		CWMPObject *target, int id) {
	CWMPObject *o;
	Instance *ip;

	o = start->objList;
	while (o && o->name) {
		if (o==target) {
			Instance *p = o->iList;
			while (p) {
				if (p->id == id)
					return p; /* already created, just return pointer*/
				p = p->next;
			}
			/* didn't find it, create a new instance */
			return createInstance(o, path, id, NULL);
		} else {
			Instance *p = o->iList;
			while (p) {
				if (p->parent == path)
					if ( (ip = cwmpCreateInitInstance(o, p, target, id)))
						return ip;
				p = p->next;
			}
		}
		++o;
	}
	return NULL;
}
/*
 * checkInstanceID to make sure that it is larger than the
 * one requested. If not change the InstanceID. We don't want
 * to allow a subsequent AddObject of the current object to
 * get a duplicate ID later on.
 */
static void checkInstanceID(int id){
	if ( id > cpeState.instanceID )
		cpeState.instanceID = id;
}
/*
 * cwmpInitObjectInstance searches for the name of the object. The
 * name is assumed to end with an instance number in the
 * form "XXX.<i>." If the name and instance is found the
 * instance pointer is returned and cwmpGetCurrentObject() will
 * return the object pointer. If not found the Instance is created
 * and the pAddObj() function is called to perform any initialization of
 * instance data. The function looks up the newly create object and
 * instance in order to leave the framework state representing
 * the new object instance.
 */
Instance *cwmpInitObjectInstance( char *name ){
	CWMPObject *o;
	int			id;
	char		base[256];

	if ( (cwmpFindObject(name)))
		return cwmpGetCurrentInstance();
	else if ( isdigit( name[strlen(name)-2] )){
		/* need to create an object instance */
		char *p = name + strlen(name)-2;
		while ( isdigit( *p )) --p;
		id = atoi( p+1);
		checkInstanceID(id);
		strncpy( base, name, p-name+1); /* isolate base name */
		base[p-name+1]='\0';
		if ( (o = cwmpFindObject(base)) ){
			createInstance(o, cwmpTopInstance(), id, NULL);
			DBGPRINT((stderr, "Create static instance: %s%d.\n", base, id));
			cwmpFindObject(name); 		/* this sets the top instance to the */
											/* new instance, etc. */
			return cwmpGetCurrentInstance();
		}
	}
	return NULL;
}
/*
 * Return pointer to instance with id==iId or
 * alias == iId that is on the current instance path.
 * iId points to a substring of the form
 *       <digits>.       such as '123.'
 *    or [alphanumeric chars]. such as '[cpe123].'
 * It should be terminated with a '.'.
 * Return NULL if no instance or invalid instance path.
 */
static Instance *findInstance(CWMPObject *obj, const char *iId) {
	Instance *ip = obj->iList;
	const char *p = NULL;
	int	lth;
	if (iId && *iId == '['){
		// alias found
		p = iId +1;
		lth = index(p, ']')-p;
	}
	while (ip) {
		if ( ip->parent == cwmpTopInstance()){
			if ( iId==NULL ){ // looking for unnamed Instance: id==0
				return (ip->id == 0)? ip: NULL;
			}
			if (p && ip->alias ){  // compare alias
				if ( (strncmp(p, ip->alias, lth)==0) && (strlen(ip->alias)== lth))
					return ip;
			} else if ( isdigit(*iId)){
				if ( ip->id== atoi(iId))
					return ip;
			}
		}
		ip = ip->next;
	}
	return NULL;
}

Instance *cwmpFindInstance(CWMPObject *obj, Instance *parent, const char *iId) {
	Instance *ri;
	cwmpPushInstance(parent);
	ri = findInstance(obj, iId);
	cwmpPopInstance();
	return ri;
}
/**
 * Return pointer to Instance for path.
 */
Instance *cwmpGetInstancePtr(const char *path){
	CWMPObject *o = cwmpFindObject(path);
	if ( o && ISINSTANCE(o)){
		return cwmpTopInstance();
	}
	return NULL;
}
/*
 * return true if object is an eInstance or eStaticInstance.
 */
int cwmpIsInstance(CWMPObject *o){
	return ISINSTANCE(o);
}
/*
 * Check if instance ip is linked to CWMPObject o
 */
static int instanceHere(CWMPObject *o, Instance *ip) {
	Instance *p = o->iList;
	while (p) {
		if (p == ip)
			return 1;
		p=p->next;
	}
	return 0;
}
/*
 * *ip is a pointer to the current Instance structure
 * return the next valid Instance based on the instance path
 */
Instance *cwmpGetNextInstance(CWMPObject *o, Instance *ip) {
	Instance *ci = {0}; //-Werror=maybe-uninitialized

	if (o==NULL && ip==NULL)
		return NULL;
	if (ip!=NULL)
		ci = ip->next;
	else if (o !=NULL)
		ci = o->iList;
	if (ci) { /* check instance path of this instance */
		while (ci && ci->parent!=cwmpTopInstance() )
			ci = ci->next;
	}
	return ci;
}

CWMPParam *cwmpSrchPList(CWMPObject *obj, const char *pname) {
	CWMPParam *pL = obj->paramList;

	while (pL->name) {
		if (strcmp(pname, pL->name)==0)
			return pL;
		++pL; /* increment to next parameter */
	}
	return NULL;
}

int cwmpGetFindStatus(void){
	return findStatus;
}

static CWMPParam *findParameter(CWMPObject *start, const char *pPath) {
	char *cp;
	CWMPObject *obj = start;
	int oLth;

	cp = strchr(pPath, '.'); /* find lth of obj Name */
	if (cp==NULL) {
		/* end of path - now search for parameter */
		return cwmpSrchPList(obj, pPath);
	} else {
		++cp; /* move past . */
		/* search Object list for path fragment */
		oLth = cp - pPath -1;
		while (obj && obj->name) {
			if (strncmp(obj->name, pPath, oLth)==0) {
				/* found object name */
				cwmpCurrentObject = obj;
				/* test if pPath is at an instance */
				if ( isdigit(*cp)||(*cp=='[')) {
					/* there is an instance number in the path ? */
					Instance *ip;
					if (!ISINSTANCE(obj)) {
						findStatus = PATH_FORMAT_ERROR;
						return NULL; /* Not an instance object -- return NULL */
					}
					if ( (ip = findInstance(obj, cp))) {
						/* found instance with correct path - step down to next obj */
						if ( (cp = strchr(cp, '.'))!=NULL) {
							CWMPParam *p;
							cwmpPushInstance(ip); /* update instance Path */
							if (*++cp) { /* check if end of object-path name and step past . */
								char *pp = strchr(cp, '.');
								if (pp == NULL){
									p = cwmpSrchPList(obj, cp);
									findStatus = PARAM_NOT_FOUND;
								} else
									p = findParameter(obj->objList, cp);
							} else
								p = NULL; /* end of obj- no parameter  */
							if (!p) {/* if found then leave instance path on stack */
								cwmpPopInstance();
							}
							return p;
						} else {
							findStatus = PATH_FORMAT_ERROR;
							return NULL; /* missing . following instance number */
						}
					} else {
						findStatus = INSTANCE_NOT_FOUND;
						return NULL;
					}
				} else if ( *cp) { /* more path */
					CWMPParam *p;
					cp = strchr(cp, '.'); /* find lth of obj Name */
					cwmpPushInstance(findInstance(obj, NULL));
					if (cp==NULL) {
						/* end of path - now search for parameter */
						p = cwmpSrchPList(obj, &pPath[oLth+1]);
						if (!p)
							findStatus = PARAM_NOT_FOUND;
					} else {
						/* more path remaining -- stop down to next node */
						p = findParameter(obj->objList, &pPath[oLth+1]);
					}
					if (!p) {
						cwmpPopInstance();
					}
					return p;
				} else
					return NULL; /* */
			}
			++obj;
		}
		findStatus = OBJECT_NOT_FOUND;
	}
	return NULL;
}

CWMPParam *cwmpFindParameter(const char *pPath) {
	cwmpFrameworkReset();
	if ( pPath ) {
		strncpy(wpPathName, pPath, sizeof(wpPathName));
		return cwmpCurrentParam = findParameter(CWMP_RootObject, pPath);
	}
	return NULL;
}
/*
 * start: where in tree to start search.
 * oPath: Object path - must match start node in tree and be terminated with a '.'
 *
 * Returns: pointer to object.
 * 		iStack[iStackTop]: current instance, also cwmpTopInstance() value.
 * */

static CWMPObject *findObject(CWMPObject *start, const char *oPath) {
	char *cp;
	CWMPObject *obj = start;
	int oLth;

	cp = strchr(oPath, '.'); /* find lth of obj Name */
	if (cp==NULL) {
		/* end of path - object path not terminated with '.' */
		return NULL;
	} else {
		++cp; /* move past . */
		/* search Object list for path fragment */
		oLth = cp - oPath -1;
		while (obj && obj->name) {
			if (strncmp(obj->name, oPath, oLth)==0) {
				/* found object name */
				cwmpCurrentObject = obj;
				/* test if pPath is at an instance */
				if ( isdigit(*cp)||(*cp=='[')) {
					/* there is an instance number in the path ? */
					Instance *ip;
					if (!ISINSTANCE(obj))
						return NULL; /* Not an instance object -- return NULL */
					if ( (ip = findInstance(obj, cp))) {
						/* found instance with correct path - step down to next obj */
						if ( (cp = strchr(cp, '.'))) {
							CWMPObject *o;
							cwmpPushInstance(ip); /* update instance Path  */
							if (*(cp+1)) /* check if end of object-path name */
								o = findObject(obj->objList, cp+1);
							else
								o = obj; /* end - found it */
							if (!o) /* if found leave instance path on stack */
								cwmpPopInstance();
							return o;
						} else
							return NULL; /* missing . following instance number */
					} else
						return NULL;
				} else if (oPath[oLth+1]) {
					CWMPObject *o;
					/* more path remaining -- step down to next node */
					cwmpPushInstance(findInstance(obj, NULL));
					o = findObject(obj->objList, &oPath[oLth+1]);
					if (!o)
						cwmpPopInstance();
					return o;
				} else
					return obj; /* found object */
			}
			++obj;
		}
	}
	return NULL;
}

CWMPObject *cwmpFindObject(const char *oPath) {

	cwmpFrameworkReset();
	if ( oPath ){
		strncpy(wpPathName, oPath, sizeof(wpPathName));
		return findObject(CWMP_RootObject, oPath);
	}
	return NULL;
}
/*
 * This walks the CWMPObject tree and visits each CWMPObject in a post-order
 * traversal of the tree and optionally calls the function specified by the
 * cBack parameter. It is intended that this function be called following
 * a cwmpFindObject or cwmpFindParameter call with the iStack set as left
 * by those functions.
 * The CTNRL_MASK controls the traversal and callbacks and
 * may have one or more of the following bits set:
 *   DESCEND: The traversal descends down the tree until a callback returns
 *            a eStopWalk. If this is not set the traversal only performs
 *            callbacks at the current level.
 * CBPARAMS:  The call back is called for each parameter traversed.
 *
 * CBOBJECTS:  The callback is called for each object traversed.
 *
 * For each instance of an object the traversal visits each object in
 * the object list and each parameter in the parameter list for each
 * object.
 *
 * Other parameters are:
 * CWMPObject *start: The object at which to start the traversal.
 * Instance   *startIp: pointer to Instance of the start object.
 *               Otherwise it should be set to NULL.
 *
 *       If the start is other than the CWMP_RootObject it is assumed that
 *       the wpPathName is set to the complete path name of the starting
 *       object and instance.
 * wtCallBack: Callback proc to be called for each parameter.
 * void *cbp1: void pointer passed thru to callback.
 * void *cbv2: void pointer passed thru to callback.
 * int  cbInt: integer passed thru to callback.
 *
 *
 * returns:eOK       - call backs executed and returned eOK status for each
 *                     parameter call back..
 *         eStopWalk - a Call back returned a eStopWalk and the walkPTree
 *                     stopped at the instance,object or paraemeter where the
 *                     callback returned the eStopWalk.
 *
 * Other global variables:
 * cwmpCurrentObject: Set to the current visited CWMPObject.
 * cwmpCurrentParameter: Set to the current visited CWMPParam
 */
int cwmpWalkPTree(CNTRL_MASK cntrl, CWMPObject *start, Instance *startIp,
		WTCallBack cBack, void *cbp1, void *cbp2, int cbInt) {
	CWMPParam *p;
	int wpLth, rootLth;
	Instance *ip;

	if (start==CWMP_RootObject) { /* reset wpPathName if start is at root */
		wpLth = rootLth =snprintf(wpPathName, sizeof(wpPathName), "%s.",
				start->name);
		//cwmpFrameworkReset();
	} else {
		wpLth = rootLth = strlen(wpPathName);
	}
	ip = startIp;
	if (ip && ISINSTANCE(start) && instanceHere(start, ip) ) { /* if starting instance- start from instance */
		CWMPObject *o = start->objList; /* wpPathName should contain instance id ".n." at end */
		cwmpCurrentParam = NULL; /* Starting instance pushed on iStack by cwmpFindXXX */
		cwmpCurrentObject = o;
		while (o && o->name) {
			int pLth;
			pLth = wpLth+snprintf(wpPathName+wpLth, sizeof(wpPathName)-wpLth,
					"%s.", o->name);
			DBGPRINT((stderr, "ObjectName=%s\n", wpPathName));
			if ( (cntrl&DESCEND) && (o->objList || o->paramList)
					&& cwmpWalkPTree(cntrl, o, ip, cBack, cbp1, cbp2, cbInt)
							== eStopWalk)
				return eStopWalk;
			wpPathName[pLth]='\0';
			if (cntrl&CBOBJECTS)
				if (cBack(o, NULL, NULL, cbp1, cbp2, cbInt)== eStopWalk)
					return eStopWalk;
			++o;
			cwmpCurrentObject = o;
		}
		p = start->paramList;
		while ( (cntrl&CBPARAMS) && p && p->name) {
			snprintf(wpPathName+wpLth, sizeof(wpPathName)-wpLth, "%s", p->name);
			DBGPRINT((stderr, "paramName=%s\n", wpPathName));
			cwmpCurrentParam = p;
			cwmpCurrentObject = start;
			if (cBack(start, p, ip, cbp1, cbp2, cbInt)== eStopWalk)
				return eStopWalk;
			++p;
		}
	} else if ( (ip = cwmpGetNextInstance(start, NULL))) { /* walk thru all instances in object- */
		while (ip) {
			CWMPObject *o = start->objList;
			cwmpCurrentObject = o;
			if (ip->id) {
				if ( cpeState.instanceMode && ip->alias ){
					/* use alias */
					wpLth = rootLth + snprintf(wpPathName+rootLth,
											sizeof(wpPathName)-rootLth, "[%s].", ip->alias);
				} else {
					wpLth = rootLth + snprintf(wpPathName+rootLth,
							sizeof(wpPathName)-rootLth, "%d.", ip->id);
				}
			} else
				wpLth = rootLth;
			cwmpPushInstance(ip); //fprintf(stderr, "Push Instance=%s\n", wpPathName);
			cwmpCurrentParam = NULL;
			while (o && o->name) {
				int pLth;
				pLth = wpLth+snprintf(wpPathName+wpLth, sizeof(wpPathName)
						-wpLth, "%s.", o->name);
				DBGPRINT((stderr, "Object=%s\n", wpPathName));
				if ( (cntrl&DESCEND) && (o->objList || o->paramList)
						&& cwmpWalkPTree(cntrl, o, ip, cBack, cbp1, cbp2, cbInt)
								== eStopWalk) {
					return eStopWalk;
				}
				wpPathName[pLth]='\0';
				if (cntrl&CBOBJECTS) {
					if (cntrl&DESCEND || start->oType==eObject) { /* if object then cBack for this level of objects*/
						Instance *myInst;
						cwmpCurrentObject = o;
						myInst = cwmpFindInstance(o, ip, NULL);
						if (cBack(o, NULL, myInst, cbp1, cbp2, cbInt)== eStopWalk) {
							return eStopWalk;
						}
					}
				}
				++o;
				cwmpCurrentObject = o;
			}
			if (cntrl&CBPARAMS) {
				if (cntrl&DESCEND || start->oType==eObject) { /* if object then cBack for this level of params*/
					p = start->paramList;
					while ( (cntrl&CBPARAMS) && p && p->name) {
						snprintf(wpPathName+wpLth, sizeof(wpPathName)-wpLth,
								"%s", p->name);
						DBGPRINT((stderr, "ParamName=%s\n", wpPathName));
						cwmpCurrentParam = p;
						cwmpCurrentObject = start;
						if (cBack(start, p, ip, cbp1, cbp2, cbInt)== eStopWalk) {
							return eStopWalk;
						}
						++p;
					}
				}
			}
			if ( (cntrl&CBOBJECTS)== CBOBJECTS && ISINSTANCE(start)) {
				/* print any instances at the start level */
				wpPathName[wpLth]='\0';
				cwmpCurrentParam = NULL;
				cwmpCurrentObject = start;
				if (cBack(start, NULL, ip, cbp1, cbp2, cbInt)== eStopWalk){
					return eStopWalk;
				}
			}
			cwmpPopInstance();   //fprintf(stderr, "Pop Instance=%s\n", wpPathName);
			ip = cwmpGetNextInstance( NULL, ip);
		}
	} else if ( ISINSTANCE(start) && startIp==NULL && cntrl&CBOBJECTS
			&& !(cntrl&DESCEND)) { /* Don't cback if DESCEND is set */
		/* no instances in list, startIP is NULL, and object is an eInstance */
		/* just handle the empty instance object. Such as .LANDevice. */
		cwmpCurrentObject = start;
		if (cBack(start, NULL, ip, cbp1, cbp2, cbInt)== eStopWalk)
			return eStopWalk;
	}

	return eOK;
}

static int walkDownTree(CWMPObject *start, Instance *startIp, WTCallBack cBack,
		void *cbp1, void *cbp2, int cbInt );

/**
 * functions to save and restore framework walk path state.
 */
typedef struct FWState {
	char pathSave[512];
	int	 stackTop;
}FWState;

static void saveFWState( FWState *fwState ){
	strncpy(fwState->pathSave, wpPathName, sizeof(fwState->pathSave));
	wpPathName[0]='\0';
	cwmpPushInstance(NULL);
	fwState->stackTop = iStackTop;
	return;
}

static void restoreFWState( FWState *fwState){
	strncpy(wpPathName, fwState->pathSave, sizeof(wpPathName));    /* restore wpPath */
	iStackTop = fwState->stackTop;
	if ( cwmpTopInstance() != NULL)
		cpeLog(LOG_ERR, "Stack Marker miss-match on framework state restore");
	cwmpPopInstance();               /* remove stack marker */
}
/*
 * cwmpFindPath() is used to locate the path of an object or parameter based
 * on the tests made by the callback function. If the callback function returns
 * an eStopWalk status the return is a pointer to a buffer containing the
 * path of the object or parameter. The buffer has been allocated from the heap
 * and must be freed by the calling functions.
 *
 * This function is safe to call from a cpe getter/setter level function as it
 * saves the state of the framework and restores it prior to returning to the
 * caller.
 *
 * Return: NULL - no path found ( cb function did not return an eStopWalk status).
 *         buffer - heap buffer containing the object or parameter path.
 */
char *cwmpFindPath(const char *startPath,
		WTCallBack cBack, void *cbp1, void *cbp2, int cbInt) {
	FWState fwState;
	char *p = NULL;
	eWalkStatus s;
	CWMPObject *o;

	saveFWState( &fwState );
	if ( startPath ){
		strncpy(wpPathName, startPath, sizeof(wpPathName));
		o = findObject(CWMP_RootObject, startPath);
	} else {
		o = CWMP_RootObject;
	}
	if ( o ){
		s = walkDownTree( o, NULL, cBack, cbp1, cbp2, cbInt);
		if ( s == eStopWalk )
			p = GS_STRDUP(wpPathName);
	}
	restoreFWState( &fwState );
	return p;
}
/* cwmpWalkDownTree():
 * This walks the CWMPObject tree and visits each CWMPObject in a pre-order
 * traversal of the tree and calls the function specified by the
 * cBack parameter for each node visited.
 * For each instance of an object the traversal visits each object in
 * the object list and each parameter in the parameter list for each
 * object.
 *
 * Other parameters are:
 * CWMPObject *start: The object at which to start the traversal.
 * Instance   *startIp: pointer to Instance of the start object.
 *               Otherwise it should be set to NULL.
 *
 *       If the start is other than the CWMP_RootObject it is assumed that
 *       the wpPathName is set to the complete path name of the starting
 *       object and instance.
 * wtCallBack: Callback proc to be called for each parameter.
 * void *cbp1: void pointer passed thru to callback.
 * void *cbv2: void pointer passed thru to callback.
 * int  cbInt: integer passed thru to callback.
 *
 *
 * returns:eOK       - call backs executed and returned eOK status for each
 *                     parameter call back..
 *         eStopWalk - a Call back returned a eStopWalk and the walkPTree
 *                     stopped at the instance,object or paraemeter where the
 *                     callback returned the eStopWalk.
 *
 * Other global variables:
 * cwmpCurrentObject: Set to the current visited CWMPObject.
 * cwmpCurrentParameter: Set to the current visited CWMPParam
 */

static int walkDownTree(CWMPObject *start, Instance *startIp, WTCallBack cBack,
		void *cbp1, void *cbp2, int cbInt ) {
	CWMPParam *p;
	int wpLth, rootLth;
	Instance *ip;

	if (start==CWMP_RootObject) { /* reset wpPathName if start is at root */
		wpLth = rootLth =snprintf(wpPathName, sizeof(wpPathName), "%s.", start->name);
		ip = CWMP_RootObject->iList;
		if (cBack(start, NULL, ip, cbp1, cbp2, cbInt)== eStopWalk)
			return eStopWalk;
	} else {
		ip = startIp;
		wpLth = rootLth = strlen(wpPathName);
	}
	if (ISINSTANCE(start) && ip==NULL) { /*  */
		/* no instances in list, startIP is NULL, and object is an eInstance */
		/* just handle the empty instance object. Such as .LANDevice. */
		if (cBack(start, NULL, ip, cbp1, cbp2, cbInt)== eStopWalk)
			return eStopWalk;
	}
	if (ip && ISINSTANCE(start) && instanceHere(start, ip) ) {/* if starting instance- start from instance */
		CWMPObject *o = start->objList; /* wpPathName should contain instance id ".n." at end */
		p = start->paramList;
		while (p && p->name) {
			snprintf(wpPathName+wpLth, sizeof(wpPathName)-wpLth, "%s", p->name);
			DBGPRINT((stderr, "i-paramName=%s\n", wpPathName));
			if (cBack(start, p, ip, cbp1, cbp2, cbInt)== eStopWalk)
				return eStopWalk;
			++p;
		}
		while (o && o->name) {
			int pLth;
			pLth = wpLth+snprintf(wpPathName+wpLth, sizeof(wpPathName)-wpLth,
					"%s.", o->name);
			DBGPRINT((stderr, "i-ObjectName=%s\n", wpPathName));
			if (cBack(o, NULL, NULL, cbp1, cbp2, cbInt)== eStopWalk)
				return eStopWalk;
			if ((o->objList||o->paramList) && walkDownTree(o, ip, cBack, cbp1, cbp2, cbInt) == eStopWalk)
				return eStopWalk;
			wpPathName[pLth]='\0';
			++o;
		}

	} else if ( (ip = cwmpGetNextInstance(start, NULL))) { /* walk thru all instances in object- */
		while (ip) {
			CWMPObject *o = start->objList;
			cwmpPushInstance(ip);
			if (ip->id) {
				wpLth = rootLth + snprintf(wpPathName+rootLth,
						sizeof(wpPathName)-rootLth, "%d.", ip->id);
			} else
				wpLth = rootLth;
			DBGPRINT((stderr, "Instance=%s\n", wpPathName));
			if (ISINSTANCE(start)) {
				/* print any instances at the start level */
				wpPathName[wpLth]='\0';
				if (cBack(start, NULL, ip, cbp1, cbp2, cbInt)== eStopWalk){
					cwmpPopInstance();
					return eStopWalk;
				}
				if (walkDownTree(start, ip, cBack, cbp1, cbp2, cbInt) == eStopWalk){
					cwmpPopInstance();
					return eStopWalk;
				}
			} else {
				if (start->oType==eObject) { /* if object then cBack for this level of params*/
					p = start->paramList;
					while (p && p->name) {
						snprintf(wpPathName+wpLth, sizeof(wpPathName)-wpLth,
								"%s", p->name);
						DBGPRINT((stderr, "ParamName=%s\n", wpPathName));
						cwmpCurrentParam = p;
						if (cBack(start, p, ip, cbp1, cbp2, cbInt)== eStopWalk){
							cwmpPopInstance();
							return eStopWalk;
						}
						++p;
					}
				}
				cwmpCurrentObject = o;
				while (o && o->name) {
					int pLth;
					pLth = wpLth+snprintf(wpPathName+wpLth, sizeof(wpPathName)
							-wpLth, "%s.", o->name);
					DBGPRINT((stderr, "Object=%s\n", wpPathName));
					if (start->oType==eObject) { /* if object then cBack for this level of objects*/
						//cwmpCurrentObject = o;
						if (cBack(o, NULL, NULL, cbp1, cbp2, cbInt)== eStopWalk){
							cwmpPopInstance();
							return eStopWalk;
						}
					}
					if ( (o->objList || o->paramList)
					  && walkDownTree(o, ip, cBack, cbp1, cbp2, cbInt) == eStopWalk) {
						cwmpPopInstance();
						return eStopWalk;
					}
					wpPathName[pLth]='\0';
					++o;
				}
			}
			cwmpPopInstance();
			ip = cwmpGetNextInstance( NULL, ip);
		}
	}

	return eOK;
}
/*
 * Walk the object/parameter tree starting at the node identified by
 * 	startPath.
 * Safe to call from getter/setter as the framework state is saved.
 */
int cwmpWalkSubTree(const char *startPath, WTCallBack cBack, void *cbp1,
		            void *cbp2, int cbInt) {
	FWState fwState;
	eWalkStatus s = {0}; //-Werror=maybe-uninitialized
	CWMPObject *o;

	saveFWState( &fwState );
	if ( startPath ){
		o = findObject(CWMP_RootObject, startPath);
	} else {
		o = CWMP_RootObject;
	}
	if ( o ){
		s = walkDownTree( o, NULL, cBack, cbp1, cbp2, cbInt);
	}
	restoreFWState( &fwState );
	return s;
}
/**
 *
 */
int cwmpWalkDownTree(CWMPObject *start, Instance *startIp, WTCallBack cBack,
		void *cbp1, void *cbp2, int cbInt) {
	int rs;
	cwmpPushInstance(NULL);
	rs = walkDownTree(start, startIp, cBack, cbp1, cbp2, cbInt);
	cwmpPopInstance();
	return rs;
}

/*
 * walk the tree looking for a parameter named "ExternalIPAddress" that
 * has the value of IP and return the parameter path name.
 */
/* this is callback that checks values */
static int chkParam(CWMPObject *o, CWMPParam *p, Instance *ip, void *up1,
		void *up2, int cbInt) {
	const char *target = (const char *)up1;
	const char *targetValue = up2;
	char *value;
	if (strcmp(target, p->name)==0) {
		/* found name-- get value */
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

char *findInterfaceNameWithIP(const char *IP) {
	const char *targetPname = "ExternalIPAddress";

	cwmpFrameworkReset();
	if (cwmpWalkPTree(DESCEND|CBPARAMS, CWMP_RootObject, NULL, chkParam,
					     (void*)targetPname, (void*)IP, 0) == eStopWalk) {
		return GS_STRDUP(wpPathName);
	}
	cpeLog(LOG_DEBUG, "Unable to find default route interface name");
	return NULL;
}

/*
 * Return pointer to allocated string containing the object path to the Instance *ip.
 * Caller must free the string.
 * 		NULL if not found or the Instance parameter is NULL.
 */
static int chkInstance(CWMPObject *o, CWMPParam *p, Instance *ip, void *up1,
		void *up2, int cbInt ){
	return (ip==(Instance*)up1)? eStopWalk: eOK;
}
char *cwmpGetInstancePathStr( const Instance *ip){
	FWState fwState;
	char *retStr = NULL;
	if ( ip != NULL ){
		saveFWState( &fwState );
		if (cwmpWalkPTree(DESCEND|CBOBJECTS, CWMP_RootObject, NULL, chkInstance,
											  (void*)ip, NULL, 0) == eStopWalk) {
			retStr = GS_STRDUP(wpPathName);
		} else
			cpeLog(LOG_DEBUG, "Instance pointer not found in tree.");
		restoreFWState( &fwState );
	}
	return retStr;
}

/**
 * Returns a heap allocated string containing the Object path name instances.
 * Path names are comma separated strings.
 */
char *cwmpGetPathRefRowsStr(Instance *p[] ){
	Instance *ip = *p;
	char *bp = NULL;
	int	 i = 0;
	if ( ip ){
		bp = GS_MALLOC(MAX_PATH_BUF_SZ);
		do {
			char *path = cwmpGetInstancePathStr( ip );
			if ( path ){
				if ( i>0 && i< MAX_PATH_BUF_SZ-2 ){
					// add ',' following previous
					*(bp+i++) = ',';
					*(bp+i++) = '\0';
				}
				int lth = strlen( path );
				strncpy ( bp+i, path, MAX_PATH_BUF_SZ - i);
				i += lth;
				*(bp+i) = '\0';
				GS_FREE(path);
			}
			ip = *++p;
		} while (ip);
	}
	return bp;
}
/*
 * Create the IData structure for each of the parameter in the
 * subtree defined by the Object start. eInstance objects are
 * not traversed.
 */
static void createIData(CWMPObject *start, Instance *ip) {
	CWMPParam *p;

	p = start->paramList;
	while (p && p->name) {
		/* verify that one isn't already there */
		IData *idp = p->instanceData;
		while (idp) {
			if (idp->instance == ip)
				break;
			idp = idp->next;
		}
		if (idp==NULL) {
			if ( (idp = (IData *)GS_MALLOC(sizeof(IData)))) {
				memset(idp, 0, sizeof(IData));
				idp->next = p->instanceData;
				p->instanceData = idp;
				idp->instance = ip;
				idp->notify = p->notify;
			}
		}
		++p;
	}
	return;
}
/*
 *
 * create an Instance for each of the eObject types of objects
 * that are statically defined by the root.
 */
Instance *cwmpBootUpInstanceInit(void) {
	CWMPObject *o = CWMP_RootObject;
	Instance *ip;
	cwmpRunState = RUNSTATE_BOOTUP_INIT;
	snprintf(wpPathName, sizeof(wpPathName), "%s.", o->name);
	ip = createInstance(o, NULL, 0, NULL); /* create root instance */
	return ip;
}

const char *cwmpGetRPCMethodNameString(eRPCMethods m) {
	const char *t;
	switch (m) {
	case eGetRPCMethods:
		t="GetRPCMethods";
		break;
	case eGetParameterNames:
		t="GetParameterNames";
		break;
	case eGetParameterValues:
		t="GetParameterValues";
		break;
	case eSetParameterValues:
		t = "SetParameterValues";
		break;
	case eGetParameterAttributes:
		t="GetParameterAttributes";
		break;
	case eSetParameterAttributes:
		t="SetParameterAttributes";
		break;
	case eAddObject:
		t="AddObject";
		break;
	case eDeleteObject:
		t="DeleteObject";
		break;
	case eReboot:
		t="Reboot";
		break;
#ifdef CONFIG_RPCDOWNLOAD
	case eDownload:
		t="Download";
		break;
	case eTransferCompleteResponse:
		t="TransferCompleteResponse";
		break;
#endif
#ifdef CONFIG_RPCSCHEDULEDOWNLOAD
	case eScheduleDownload:
		t="ScheduleDownload";
		break;
#endif
#ifdef CONFIG_RPCCANCELTRANSFER
	case eCancelTransfer:
		t="CancelTransfer";
		break;
#endif
#ifdef CONFIG_RPCFACTORYRESET
	case eFactoryReset:
		t="FactoryReset";
		break;
#endif
#ifdef CONFIG_RPCUPLOAD
	case eUpload:
		t = "Upload";
		break;
#endif
#ifdef CONFIG_RPCGETQUEUEDTRANSFERS
	case eGetQueuedTransfers:
		t = "GetQueuedTransfers";
		break;
#endif
#ifdef CONFIG_RPCGETALLQUEUEDTRANSFERS
	case eGetAllQueuedTransfers:
		t = "GetAllQueuedTransfers";
		break;
#endif
#ifdef CONFIG_RPCSCHEDULEINFORM
	case eScheduleInform:
		t = "ScheduleInform";
		break;
#endif
#ifdef CONFIG_RPCKICK
	case eKickedResponse:
		t = "KickedResponse";
		break;
#endif
#ifdef CONFIG_RPCREQUESTDOWNLOAD
	case eRequestDownloadResponse:
		t = "RequestDownloadResponse";
		break;
#endif
#ifdef CONFIG_RPCSETVOUCHERS
	case eSetVouchers:
		t = "SetVouchers";
		break;
	case eGetOptions:
		t = "GetOptions";
		break;
#endif
	case eInformResponse:
		t="InformResponse";
		break;
	case eGetRPCMethodsResponse:
		t="GetRPCMethodsResponse";
		break;
	case eFault:
		t="Fault";
		break;
#ifdef CONFIG_RPCAUTONOMOUSTRANSFERCOMPLETE
	case eAutonomousTransferCompleteResponse:
		t="AutonomousTransferCompleteResponse";
		break;
#endif
#ifdef CONFIG_RPCCHANGEDUSTATE
	case eChangeDUState:
		t="ChangeDUState";
		break;
	case eChangeDUStateResponse:
		t="DUStateChangeComplete";
		break;
	case eDUStateChangeCompleteResponse:
		t="DUStateChangeCompleteResponse";
		break;
#endif

	default:
		t="Unknown RPC Request";
		break;
	}
	return t;
}

void cwmpDecodeRPCRequest(RPCRequest *r) {
	cpeDbgLog(DBG_DUMPRPC, "ID = %s", r->ID? r->ID: "<unspecified>");
	cpeDbgLog(DBG_DUMPRPC, "holdRequests = %d", r->holdRequests);
	cpeDbgLog(DBG_DUMPRPC, "maxEnvelopes = %d", r->maxEnvelopes);
	cpeDbgLog(DBG_DUMPRPC, "commandKey = %s", r->commandKey[0]? r->commandKey: "<unspecified>");
	cpeDbgLog(DBG_DUMPRPC, "parmeterKey = %s", r->parameterKey[0]? r->parameterKey: "<unspecified>");
	cpeDbgLog(DBG_DUMPRPC, "RPC Method = %s", cwmpGetRPCMethodNameString(r->rpcMethod));

	switch (r->rpcMethod) {
	case eGetParameterNames:
		cpeDbgLog(DBG_DUMPRPC, "ParameterPathName=%s", r->ud.getPNamesReq.pNamePath);
		cpeDbgLog(DBG_DUMPRPC, "NextLevel=%d", r->ud.getPNamesReq.nextLevel);
		break;
	case eGetParameterValues:
	case eGetParameterAttributes: {
		ParameterNameStruct *p = r->ud.getPAVReq.pnameList;
		cpeDbgLog(DBG_DUMPRPC, "arrayTypeSize = %d pnames=%d", r->arrayTypeSize, r->ud.getPAVReq.paramCnt);
		while (p) {
			cpeDbgLog(DBG_DUMPRPC, "ParameterPathName=%s", p->pName);
			p = p->next;
		}
		break;
	}
	case eSetParameterValues: {
		ParameterValueStruct *p = r->ud.setPValuesReq.pvList;
		cpeDbgLog(DBG_DUMPRPC, "arrayTypeSize = %d pnames=%d", r->arrayTypeSize, r->ud.setPValuesReq.paramCnt);
		while (p) {
			cpeDbgLog(DBG_DUMPRPC, "ParameterPathName=%s value=%s", p->pName, p->value);
			p = p->next;
		}
		break;
	}
	case eSetParameterAttributes: {
		ParameterAttributesStruct *p = r->ud.setPAttrReq.paList;
		cpeDbgLog(DBG_DUMPRPC, "arrayTypeSize = %d pnames=%d", r->arrayTypeSize, r->ud.setPAttrReq.paramCnt);
		while (p) {
			cpeDbgLog(DBG_DUMPRPC,
			"ParameterName=%s\nnotifyChange=%d notify=%d accessChange=%d accessList=%d", p->pName, p->notifyChange,
			p->notification, p->accessListChange, p->accessList);
			p = p->next;
		}
		break;
	}
	case eAddObject:
	case eDeleteObject:
		cpeDbgLog(DBG_DUMPRPC, "ObjectPathName=%s", r->ud.addDelObjectReq.objectPath);
		break;
	case eUpload:
	case eDownload:
	case eScheduleDownload:
		cpeDbgLog(DBG_DUMPRPC, "FileType = %s, URL=%s, UserName=%s password=%s",
		r->ud.downloadReq.fileType, r->ud.downloadReq.URL, r->ud.downloadReq.userName, r->ud.downloadReq.passWord);
		cpeDbgLog(DBG_DUMPRPC, "FileSize =%d, targetFile=%s, delaySeconds=%d",
		r->ud.downloadReq.fileSize, r->ud.downloadReq.targetFileName, r->ud.downloadReq.delaySeconds);
		cpeDbgLog(DBG_DUMPRPC, "SuccessURL=%s FailureURL=%s",
		r->ud.downloadReq.successURL, r->ud.downloadReq.failureURL);
#ifdef CONFIG_RPCSCHEDULEDOWNLOAD
		if (r->rpcMethod == eScheduleDownload ){
			int i;
			TimeWindow *twp = r->ud.downloadReq.timeWindow;
			for (i=0; twp->windowStart>0 && i<MAX_TIMEWINDOWS; ++i, ++twp){
				cpeDbgLog(DBG_DUMPRPC, "WindowStart=%d WindowEnd=%d WindowMode=%s UserMessage=%s MaxRetires=%d",
				 twp->windowStart, twp->windowEnd, twp->windowMode,
				 twp->userMessage? twp->userMessage: "", twp->maxRetries);
			}
		}
#endif
		break;
	case eInformResponse:
		break;
	case eGetRPCMethods:
	case eGetRPCMethodsResponse:
	case eTransferCompleteResponse:
	case eRequestDownloadResponse:
	case eFactoryReset:
	case eGetQueuedTransfers:
	case eGetAllQueuedTransfers:
	case eAutonomousTransferCompleteResponse:
	case eChangeDUState:
	case eChangeDUStateResponse:
	case eDUStateChangeCompleteResponse:
		break;
	case eFault:
		cpeDbgLog(DBG_DUMPRPC, "FaultCode=%d FaultString=%s", r->ud.faultMsg.faultCode,
				r->ud.faultMsg.faultString);
		break;
#ifdef CONFIG_RPCKICK
	case eKickedResponse:
		cpeDbgLog(DBG_DUMPRPC, "NextURL = %s",
		r->ud.kickedResponseReq.nextURL);
		break;
#endif
	default:
		cpeDbgLog(DBG_DUMPRPC, "RPC Method is not recognized");
		break;
	}
}

