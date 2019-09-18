/*----------------------------------------------------------------------*
 * Gatespace
 * Copyright 2005 Gatespace. All Rights Reserved.
 * Gatespace Networks, Inc. confidential material.
 *----------------------------------------------------------------------*
 * File Name  : rpcUtils.h
 *
 * Description: SOAP RPC utility routines for handling data structures
 * $Revision: 1.17 $
 * $Id: rpcUtils.h,v 1.17 2012/06/13 12:01:34 dmounday Exp $
 *----------------------------------------------------------------------*/
#ifndef _RPCUTILS_H_
#define _RPCUTILS_H_
#include "../includes/sys.h"
#include "../gslib/src/utils.h"
#include "../includes/rpc.h"
#include "../includes/paramTree.h"

void cwmpFreeRPCRequest(RPCRequest *);

/*
 * findParameter status.
 * Retrieved by cwmpGetFindStatus(void).
 */
#define	INSTANCE_NOT_FOUND	1
#define	OBJECT_NOT_FOUND	2
#define	PARAM_NOT_FOUND		3
#define	PATH_FORMAT_ERROR	4
int cwmpGetFindStatus(void);    /* return status of last findXXX call */

Instance *cwmpCreateInstance(CWMPObject *, int id);

/* findParameter */
CWMPParam *cwmpFindParameter(const char *pPath);


/* findObject */
CWMPObject *cwmpFindObject(const char *oPath);

/* findParameterValue : such as looking for object path of ExternalIP == xxx*/

/* walkTree and execute a callback at each node until return from callback is STOP_WALK */

typedef enum {
	eOK,
	eStopWalk
} eWalkStatus;

typedef int CNTRL_MASK; 	/* walkTree control mask */
#define 	DESCEND   1		/* descend tree in order to traverse the complete tree */
#define 	CBOBJECTS 2		/* callback on objects traversed*/
#define 	CBPARAMS  4	    /* callback on parameters traversed */

/* Define for object paths size */
#define MAX_PATH_BUF_SZ	2047

typedef int (*WTCallBack)(CWMPObject *, CWMPParam *, Instance *, void *, void *, int );

int cwmpWalkPTree(CNTRL_MASK cntrl, CWMPObject *, Instance *, WTCallBack, void *, void *, int);
int cwmpWalkSubTree( const char *startPath, WTCallBack, void *, void *, int);
char *cwmpFindPath(const char *startPath, WTCallBack cBack, void *cbp1, void *cbp2, int cbInt);

const char    *cwmpGetRPCMethodNameString(eRPCMethods m);

Instance *cwmpBootUpInstanceInit(void);
void cwmpPushInstance(Instance *);
Instance *cwmpPopInstance(void);
void cwmpFrameworkReset(void);
char *findInterfaceNameWithIP( const char * );
Instance *cwmpGetCurrentInstance(void);
Instance *cwmpCreateInitInstance( CWMPObject *start, Instance *path, CWMPObject *target, int id);
Instance *cwmpInitObjectInstance( char *name );
Instance *cwmpFindInstance(CWMPObject *start, Instance *path, const char* iId);
Instance *cwmpGetInstancePtr(const char *path);
Instance *cwmpGetNextInstance( CWMPObject *, Instance *);
int cwmpIsInstance(CWMPObject *o);
CWMPObject *cwmpGetCurrentObject(void);
CWMPObject *cwmpSrchOList( CWMPObject *obj, const char *name);
CWMPParam *cwmpSrchPList(CWMPObject *obj, const char *pname);
CWMPParam *cwmpGetCurrentParam(void);
IData *cwmpGetIData(CWMPParam *p, Instance *ip);
int cwmpGetNumberOfInstances(CWMPObject *o, Instance *path);
void  cwmpGetInstanceCntStr(CWMPObject *o, char **value);
void cwmpGetObjInstanceCntStr(const char *oname, char **value);
char *cwmpGetInstancePathStr(const Instance *);
char *cwmpGetPathRefRowsStr(Instance *p[]);
void cwmpDecodeRPCRequest(RPCRequest *r);
void cwmpDeleteInstance( CWMPObject *o, Instance *ip );
void cwmpDeleteAllInstances(CWMPObject *o);
void cwmpDeleteChildInstances(CWMPObject *o, Instance *parent);
CPE_STATUS cwmpAddObjectInstance( const char *p, Instance **ipp );
CPE_STATUS cwmpDeleteObjectInstance(CWMPObject *start, Instance *ip);
CWMPObject *cwmpGetChildObject(const char *oName);
Instance *cwmpGetChildInstance(const char *oName, const Instance *parent);
Instance *cwmpGetNextSiblingInstance(const Instance *instance);
#define ISINSTANCE(x)   (x->oType == eInstance || x->oType == eStaticInstance || x->oType==eCPEInstance)
CPE_STATUS cwmpGetAliasParam(Instance *ip, char **value);
CPE_STATUS cwmpSetAliasParam(Instance *ip, const char *value);

/*
 * cwmpRunState CWMPc framework state values. All state value set by the
 * framework are negative.
 * The CPE functions may set other values.
 */
#define	RUNSTATE_BOOTUP_INIT	-1
#define	RUNSTATE_ADDOBJECT		-2
#define	RUNSTATE_DELETEOBJECT	-3
#define	RUNSTATE_GETPARAMETER	-4
#define RUNSTATE_SETPARAMETER	-5
#define	RUNSTATE_RESETPARAMETER	-6
#define	RUNSTATE_COPYPARAMETER	-7
void cwmpSetRunState(int state);
int cwmpGetRunState(void);



#endif  /* _RPCUTILS_H_*/

