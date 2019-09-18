/*----------------------------------------------------------------------*
 * Gatespace
 * Copyright 2006 Gatespace. All Rights Reserved.
 * Gatespace Networks, Inc. confidential material.
 *----------------------------------------------------------------------*
 * File Name  : paramTree.h
 * Description:	structure, etc. definitions for parameter tree and
 *             object lists, parameter lists and instances.
 *----------------------------------------------------------------------*
 * $Revision: 1.10 $
 * $Id:
 *----------------------------------------------------------------------*/
#ifndef _PARAMTREE_H
#define _PARAMTREE_H
#include "rpc.h"
/* This enumerates the return status from the getter/setter and
*  addXXX/delXXX object functions. If a CPE_OBJCOMMIT status is returned
*  the framework will call the associated commit function with the
*  object instance.	If the status is CPE_COMMITREBOOT then the CPE is
*  rebooted following the disconnect with the ACS.
*/
typedef enum {
	eUnknown=0,
	eString,
	eInt,
	eUnsignedInt,
	eBoolean,
	eDateTime,
	eBase64,
	eLong,
	eUnsignedLong,
	eHexBinary,
	eAny,
	eStringSetOnly
} eCWMPType;

typedef enum {
	eObject,
	eInstance,			/* may be added/deleted with RPC if add/del functions defined*/
	eStaticInstance,	/* can not be added/deleted with RPC, such as HW instances. */
	eCPEInstance		/* created and deleted by CPE actions. Such as .Hosts.Host.{i}. */
} eObjType;

/*
 * The CWMPObject oFlags are intended to be used by the CPE integration code.
 * For example the CONFIG_NOSAVE flag is used by the configsave quick-start
 * functions to inhibit saving any of an object's parameter data.
 * These flags are not used by the CWMPc framework functions.
 */
#define	CONFIG_NOSAVE	0x01
// #define CONFIG_USERFLG1	0x02
// #define CONFIG_USERFLG2	0x04 // upto 8 bits for user defined flags.

/*
 * Commit flags are passed to the commit callback function to indicate the
 * state of the commit call. Either setting a new parameter value or restoring
 * an object's values because of a set parameter error.
 */
typedef enum {
	COMMIT_SET,          /* callback is for a ParameterSetValue set*/
	COMMIT_RESTORE		 /* callback is to restore following a set error */
} eCommitCBType;
/*
 * The CWMPParam.setable flag indicates if the parameter is writable by a
 * SetParameterValues RPC. The pSetter function may optionally be defined for
 * use by the CPE's object initialization routines. The CWMPParam.setable flag
 * must be initialized to RPC_RW for parameter defined as writable.
 */
typedef enum {
	RPC_R = 0,
	RPC_RW =1
}eRPCWrt;
/*
 * Instance.flags bitmask
 */
#define COMMIT_PENDING	0x1		/* A commit call is pending */


/* Each CWMP object has an instance. If the object is limited to
*  a single instance then the single Instance structure has an id=0.
*  If there are multiple instances then there is just
*  one for each of the real instances and no instance with ID==0.
*  instances are created by the CWMP framework and the CPE on
*  initialization. For example, the WANDevice instance is created
*  on initialization.
*/
typedef struct Instance {
	struct Instance *next;
	int				id;			/* instance ID 	*/
	struct Instance *parent;	/* parent Instance in parameter path */
	char			*alias;		/* pointer to alias name string. CWMP Amendment 4 */
	void			*cpeData;	/* pointer to generic CPE data used by setter/getters */
								/*   must be freed in delXXXXInstance callbacks */
	int				flags;		/* Commit callback flags */
} Instance;

struct CWMPObject;

typedef CPE_STATUS (*CPEAddObj)(struct CWMPObject *, struct Instance *);
#define CPEADDOBJ(XX) CPE_STATUS XX (struct CWMPObject *, struct Instance *)

typedef CPE_STATUS (*CPEDelObj)(struct CWMPObject *, Instance *);
#define CPEDELOBJ(XX) CPE_STATUS XX (struct CWMPObject *, Instance *)

typedef CPE_STATUS (*CPECommit)(struct CWMPObject *, Instance *, eCommitCBType );
#define CPECOMMIT(XX) CPE_STATUS XX (struct CWMPObject *, Instance *, eCommitCBType)

typedef CPE_STATUS (*CPEOpnObj)(struct CWMPObject *, Instance * );
#define CPEOPNOBJ(XX) CPE_STATUS XX (struct CWMPObject *, Instance *)

typedef CPE_STATUS (*CPESetFunc)(Instance *, const char *pval);
#define CPESETFUNC(XX) CPE_STATUS XX (Instance *, const char *)

typedef CPE_STATUS (*CPEGetFunc)(Instance *, char **ppval);
#define CPEGETFUNC(XX) CPE_STATUS XX (Instance *, char **)

/*
* Each parameter in an object instance has a IData structure to represent the
* attributes and possible a copy of the data. This allows unique notification
* attributes and access lists for each parameter instance.
*/
#define SUBWRITE_INHIBIT 1		/* Inhibit write by subscriber, default is write not inhibited */
#define DEFAULT_PASSIVE	0x40	/* Default Passive notify - copied to PASSIVE_NOTIFY on startup */
#define DEFAULT_ACTIVE	0x20	/* Default Active notify - copied to ACTIVE_NOTIFYon startup */
#define NOACTIVENOTIFY  0x10    /* This is a bit that prevents active notify from being set by ACS*/
#define FORCED_INFORM   0x8		/* value can not be changed by ACS */
#define FORCED_ACTIVE   0x4		/* value can not be changed by ACS */
#define ACTIVE_NOTIFY   0x2
#define PASSIVE_NOTIFY  0x1
#define NO_NOTIFY       0

#define FWNOTIFYMASK    (NOACTIVENOTIFY|FORCED_INFORM|FORCED_ACTIVE|DEFAULT_ACTIVE|DEFAULT_PASSIVE)
#define RPCNOTIFYMASK   (ACTIVE_NOTIFY|PASSIVE_NOTIFY)
#define NOTIFY_ON 		(FORCED_ACTIVE|ACTIVE_NOTIFY|PASSIVE_NOTIFY)

typedef struct IData{
	struct IData	*next;
	unsigned	notify:7;		/* type of notify 0-none,              */
	unsigned	accessClients:1;/* bit to inhibit access to parameter by subscriber */
	Instance	*instance;		/* pointer to which instance this data belongs */
	char		*dataCopy;		/* data copies -- if required */
} IData;

/* CWMPParam is a structure representing the list of parameters within each
*  object.
*/
typedef struct CWMPParam {
	const char 	*name;
	CPEGetFunc	pGetter;
	CPESetFunc	pSetter;
	IData 		*instanceData;
	eRPCWrt		setable:1;	/*writable by SetParameterValues RPC */
	eCWMPType	pType:4;
	unsigned	notify:7;	/*notify values for child instances */
	unsigned	pSize:16;
} CWMPParam;

typedef struct CWMPObject {
	const char	*name;
	CPEDelObj	pDelObj;	/* CPE function to delete an object instance */
	CPEAddObj	pAddObj;	/* CPE function to add a new object instance */
	CPECommit	pCommitObj;	/* CPE function to commit parameters of object- optional*/
	CPEOpnObj	pOpenObj;	/* reserved  */
	struct CWMPObject	*objList;	/* pointer to a list of child objects */
	CWMPParam	*paramList;	/* pointer to a list of parameters of this object */
	Instance	*iList;  	/* pointer to instances of this object */
	eObjType	oType:8;	/* simple object or instances allowed */
	unsigned	oFlags:8;	/* CPE integration flags */
} CWMPObject;

CPEGETFUNC(cwmpGetAliasParam);
CPESETFUNC(cwmpSetAliasParam);

#endif  /* _PARAMTREE_H */

