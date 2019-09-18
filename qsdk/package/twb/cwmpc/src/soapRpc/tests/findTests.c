/*
 * findTests.c
 *
 *  Created on: Jan 4, 2012
 *      Author: dmounday
 */

#include <unistd.h>
#include <stdio.h>
#include <syslog.h>
#include <ctype.h>
#include "../../includes/cpemsgformat.h"

#include "../../includes/rpc.h"
#include "../cwmpSession.h"
#include "../../tr-181-2-4/targetsys.h"
#include "../../includes/CPEWrapper.h"
#include "../../soapRpc/rpcMethods.h"
#include "../rpcUtils.h"
#define DEBUG
#ifdef DEBUG
#define DBGPRINT(X) fprintf X
#else
#define DBGPRINT(X)
#endif



CPE_STATUS cpeChangeRequestedState(void *p,	const char *value) {
	return 0;
}

CPE_STATUS getterFunc(Instance *ip, char **value)
{
	return CPE_OK;
}
CPE_STATUS setterFunc(Instance *ip, const char *value)
{
	return CPE_OK;
}
CPE_STATUS addInstance(CWMPObject *o, Instance *ip){
	fprintf(stdout, " pAddObj() o=%s.%d .[%s]\n", o->name, ip->id, ip->alias? ip->alias: "" );
	return CPE_OK;
}

CWMPParam AA_Params[]={
	{ "AA_Pa",
		getterFunc,
		setterFunc,
		NULL,
		RPC_RW,
		eBoolean,
		0,
		0},
	{ "AA_Pb",
		getterFunc,
		NULL,
		NULL,
		RPC_R,
		eString,
		0,
		0},
		{"Alias",
			cwmpGetAliasParam,
			cwmpSetAliasParam,
			NULL,
			RPC_RW,
			eString,
			0,
			64
			},
	{NULL}
};

CWMPParam AB_Params[]={
	{ "AB_Pa",
		getterFunc,
		NULL,
		NULL,
		RPC_R,
		eString,
		0,
		64},
	{ "AB_Pb",
		getterFunc,
		NULL,
		NULL,
		RPC_R,
		eString,
		0,
		32},
	{"Alias",
		cwmpGetAliasParam,
		cwmpSetAliasParam,
		NULL,
		RPC_RW,
		eString,
		0,
		64
		},
	{NULL}
};
/** Device.SoftwareModules.ExecutionUnit.{i}.  */

CWMPObject AB_Objs[]={
	{"ABAB",
		NULL,
		addInstance,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		eObject},
	{NULL}
};

/** Device.SoftwareModules. */
CPE_STATUS getAANumberOfEntries(Instance *ip, char **value)
{
	cwmpGetObjInstanceCntStr("AA", value);
	fprintf(stdout, "...AA.AANumberOfEntries = %s\n", *value);
	return CPE_OK;
}
CPE_STATUS getABNumberOfEntries(Instance *ip, char **value)
{
	cwmpGetObjInstanceCntStr("AB", value);
	fprintf(stdout, "...AB.AANumberOfEntries = %s\n", *value);
	return CPE_OK;
}


CWMPParam A_Params[]={
	{ "AANumberOfEntries",
		getAANumberOfEntries,
		NULL,
		NULL,
		RPC_R,
		eUnsignedInt,
		0,
		0},
	{ "ABNumberOfEntries",
		getABNumberOfEntries,
		NULL,
		NULL,
		RPC_R,
		eUnsignedInt,
		0,
		0},
	{NULL}
};
/** Device.A.  */

CWMPObject A_Objs[]={
	{"AA",
		NULL,
		addInstance,
		NULL,
		NULL,
		NULL,
		AA_Params,
		NULL,
		eInstance},
	{"AB",
		NULL,
		addInstance,
		NULL,
		NULL,
		AB_Objs,
		AB_Params,
		NULL,
		eInstance},
	{NULL}
};

CWMPParam B_Params[]={
	{ "B_Pa",
		getterFunc,
		NULL,
		NULL,
		RPC_R,
		eUnsignedInt,
		0,
		0},
	{NULL}
};
/** Device.  */

CWMPObject Device_Objs[]={
	{"A",
		NULL,
		addInstance,
		NULL,
		NULL,
		A_Objs,
		A_Params,
		NULL,
		eObject},
	{"B",
		NULL,
		addInstance,
		NULL,
		NULL,
		NULL,
		B_Params,
		NULL,
		eInstance},
	{NULL}
};

/** CWMP ROOT Object Table  */
CPE_STATUS getBNumberOfEntries(Instance *ip, char **value)
{
	cwmpGetObjInstanceCntStr("B", value);
	fprintf(stdout, "Dev.B_NumberOfEntries = %s\n", *value);
	return CPE_OK;
}
CWMPParam Dev_Params[]={
	{ "B_NumberOfEntries",
		getBNumberOfEntries,
		NULL,
		NULL,
		RPC_R,
		eUnsignedInt,
		0,
		0},
	{NULL}
};
CWMPObject CWMP_RootObject[]={
	{"Dev",
		NULL,
		addInstance,
		NULL,
		NULL,
		Device_Objs,
		Dev_Params, //Device_Params,
		NULL,
		eObject},
	{NULL}
};

EventPolicy eventPolicy[1];
ACSSession acsSession;

/*
 * test flags
 */
int installUninstallOnly = 1;
int	unInstallRunning;
static int id = 100;


int cpeGetInstanceID(){
	return ++id;
}
CPEState cpeState;

void cpeSaveConfig(void){
	return;
}

/*
 * returns !=0 if the path ends in a instance sequence.
 * I.E.  LANDevice.1. or LANDevice.[landev].
 *   (last two chars are a digit followed by a .
 * or last two chars are '].'.
 */
static int isInstance( char *path ) {
    int	j = strlen(path);
    if (path[j-1]=='.') {
        /* last char is . */
        int i = j-2; // check char befor '.'
        return isdigit(path[i]) || (path[i]==']');
    }
    return 0;
}

static int isFragment( char *path ) {
    int j= strlen(path);
    return ( path[j-1]=='.');
}

/*
 * An object has been found. Need to retrieve all the parameter
 * values for it.
 */
static int walkFragment( int descend, CWMPObject *o, Instance *ip, XMLWriter *xp, WTCallBack func) {
    int	paramCnt = 0;
    cwmpWalkPTree(descend, o, ip, func, xp, &paramCnt, 0);
    return paramCnt;
}
int cwmpIsACSSessionActive(void){
	return 0;
}

void usage(void) {
	printf("Use: findTests\n");
	printf("\n");
}
extern char wpPathName[];
static int printOPName(CWMPObject *o, CWMPParam *p, Instance *ip,
									void *wxp, void *paramCnt, int unused ) {
	fprintf(stdout, ">>%s\t\t\t", wpPathName);
	if ( o ){
		fprintf(stdout, "%s ", o->name );
		if ( ip ){
			if (ip->id != 0 )
				fprintf(stdout, "%d ", ip->id);
		} else if ( o->oType == eInstance ){
			;
		} else {
			fprintf(stdout, "     *********** ERROR: ip set to null   ");
		}
		if ( p ){
			fprintf( stdout, "%s ", p->name);
			if ( p->pGetter ){
				char	*value=NULL;
				p->pGetter(ip, &value);
				if ( value ) {
					fprintf(stdout, "%s", value );
					GS_FREE(value);
				}

			}
		}
		fprintf(stdout, "\n");
	} else {
		fprintf(stdout, "Object pointer set to NULL\n");
	}
    return eOK;
}
unsigned writePStatus;

static int getWriteParamValue(CWMPObject *o, CWMPParam *p, Instance *ip,
									void *wxp, void *paramCnt, int unused ) {
    char	*val=NULL;
    if ( p && p->pGetter
    		&& (writePStatus = p->pGetter(ip, &val)) ==CPE_OK ) {
 		if (paramCnt) (*(int*)paramCnt)++;
        if (val) GS_FREE(val);
    } else {
        return eStopWalk; /* 9002 Internal fault here */
    }

    return eOK;
}

int main( int argc, char *argv[] ) {
	char buf[512];
	CWMPObject *o;
	int 	pCnt;
	Instance *ip, *instance;
	int			i;
	/* top level Instance initialization */
	if ( cwmpBootUpInstanceInit() == NULL ) {
		fprintf(stderr, "cwmpBootUpInstanceInit error\n");
		return -1;
	}
	fprintf(stdout, "walk test parameters only, no instances created yet: %s\n", "Dev.");
	o = CWMP_RootObject;
	ip = NULL;
    cwmpFrameworkReset();
    pCnt = walkFragment(DESCEND|CBPARAMS, o, ip, NULL, printOPName);
	fprintf(stdout, "\n**** walk test objects only, no instances created yet: %s\n", "Dev.");
    cwmpFrameworkReset();
	pCnt = walkFragment(DESCEND|CBOBJECTS, o, ip, NULL, printOPName);
	fprintf(stdout, "\n***** walk test objects+params, no instances created yet: %s\n", "Dev.");
    cwmpFrameworkReset();
	pCnt = walkFragment(DESCEND|CBOBJECTS|CBPARAMS, o, ip, NULL, printOPName);

	snprintf(buf, sizeof( buf ), "%s.B.1.", CWMP_RootObject[0].name );
	fprintf(stdout,"cwmpInitObjectInstance(%s)\n", buf);
	instance = cwmpInitObjectInstance(buf);
	//instance->alias = strdup("cpe-B1");
	snprintf(buf, sizeof( buf ), "%s.B.2.", CWMP_RootObject[0].name );
	fprintf(stdout,"cwmpInitObjectInstance(%s)\n", buf);
	instance = cwmpInitObjectInstance(buf);
	//instance->alias = strdup("cpe-B2");
	snprintf(buf, sizeof( buf ), "%s.A.AB.1.", CWMP_RootObject[0].name );
	fprintf(stdout,"cwmpInitObjectInstance(%s)\n", buf);
	instance = cwmpInitObjectInstance(buf);
	if ( instance==NULL)
		fprintf(stdout, "No instance .A.AB.1. created\n");
	//instance->alias = strdup("cpe-AAB1");
	fprintf(stdout, "\n****** walk test objects+params: %s\n", "Dev.");
    cwmpFrameworkReset();
    pCnt = walkFragment(DESCEND|CBOBJECTS|CBPARAMS, o, ip, NULL, printOPName);
    cwmpFrameworkReset();
    fprintf(stdout, "\nwalk test params only: %s\n", "Dev.");
    cwmpFrameworkReset();
    pCnt = walkFragment(DESCEND|CBPARAMS, o, ip, NULL, printOPName);

    fprintf(stdout, "\n******* walk and read parameter values\n");
    cwmpFrameworkReset();
    pCnt = walkFragment(DESCEND|CBPARAMS, o, ip, NULL, getWriteParamValue);

    fprintf(stdout, "\nLookup Root object name and walk and read parameter values\n");
    cwmpFrameworkReset();
    if ( (o = cwmpFindObject("Dev."))) {
		if ( isInstance("Dev.") && !ISINSTANCE(o) )
			fprintf(stdout, "fault = 9005\n");
		else {
			ip = isInstance("Dev.")? cwmpGetCurrentInstance(): NULL;
			pCnt = walkFragment(DESCEND|CBPARAMS, o, ip, NULL, getWriteParamValue);
		}
    } else {
    	fprintf(stdout, "cwmpFindObject(Dev. failed\n");
    }
    fprintf(stdout,"**********Look up alias instances\n");
	snprintf(buf, sizeof( buf ), "%s.A.AB.1.AB_Pa", CWMP_RootObject[0].name );
    CWMPParam *p = cwmpFindParameter(buf);
    if ( p )
    	fprintf(stdout, "Found parameter %s\n", buf);
    else
    	fprintf(stdout, "Error did not find parameter %s\n", buf);
	snprintf(buf, sizeof( buf ), "%s.A.AB.[cpe-1].Alias", CWMP_RootObject[0].name );
    p = cwmpFindParameter(buf);
    if ( p ) {
    	fprintf(stdout, "Found parameter %s\n", buf);
    	CWMPParam *ppp = cwmpFindParameter(buf);
    	if ( ppp ){
    		Instance *ip = cwmpGetCurrentInstance();
    		if ( ip ){
    			cwmpSetAliasParam(ip, "cpe-AAB1");
    		}
    	}
    } else
    	fprintf(stdout, "Error did not find parameter %s\n", buf);

    snprintf(buf, sizeof( buf ), "%s.A.AB.[cpe-AAB1].AB_Pa", CWMP_RootObject[0].name );
    p = cwmpFindParameter(buf);
    if ( p )
    	fprintf(stdout, "Found parameter %s\n", buf);
    else
    	fprintf(stdout, "Error did not find parameter %s\n", buf);

    snprintf(buf, sizeof( buf ), "%s.A.AB.", CWMP_RootObject[0].name );
	CPE_STATUS cpeStatus = cwmpAddObjectInstance(buf, &ip);
	if ( ip ){
		fprintf(stdout, "addObjectInstance id=%d alias=%s\n", ip->id, ip->alias);
	} else
		fprintf(stdout, "addobjectInstance FAILED *************\n");
    snprintf(buf, sizeof( buf ), "%s.A.AB.[aliasname].", CWMP_RootObject[0].name );
    cpeStatus = cwmpAddObjectInstance(buf, &ip);
	if ( ip ){
		fprintf(stdout, "addObjectInstance id=%d alias=%s\n", ip->id, ip->alias);
	} else
		fprintf(stdout, "addobjectInstance FAILED *************\n");
	fprintf(stdout, "\n****** walk tree %s  ******************\n", "Dev.");
    cwmpFrameworkReset();
    pCnt = walkFragment(DESCEND|CBOBJECTS|CBPARAMS, o, ip, NULL, printOPName);
    snprintf(buf, sizeof( buf ), "%s.A.AB.%d.", CWMP_RootObject[0].name, 101);
    o = cwmpFindObject(buf);
    fprintf(stdout, "DeleteObejct %s\n", buf);
    if (o) {
		CPE_STATUS cpeStatus;
		ip = cwmpGetCurrentInstance();
		cpeStatus = cwmpDeleteObjectInstance( o, ip);
		fprintf(stdout, "delete status = %d\n", cpeStatus);
    }
    snprintf(buf, sizeof( buf ), "%s.A.AB.%d.", CWMP_RootObject[0].name, 102);
   o = cwmpFindObject(buf);
   fprintf(stdout, "DeleteObejct %s\n", buf);
   if (o) {
		CPE_STATUS cpeStatus;
		ip = cwmpGetCurrentInstance();
		cpeStatus = cwmpDeleteObjectInstance( o, ip);
		fprintf(stdout, "delete status = %d\n", cpeStatus);
   }
   snprintf(buf, sizeof( buf ), "%s.A.AB.%d.", CWMP_RootObject[0].name, 1);
  o = cwmpFindObject(buf);
  fprintf(stdout, "DeleteObejct %s\n", buf);
  if (o) {
		CPE_STATUS cpeStatus;
		ip = cwmpGetCurrentInstance();
		cpeStatus = cwmpDeleteObjectInstance( o, ip);
		fprintf(stdout, "delete status = %d\n", cpeStatus);
  }
	fprintf(stdout, "\n****** walk tree %s  ******************\n", "Dev.");
    cwmpFrameworkReset();
	o = CWMP_RootObject;
	ip = NULL;
    pCnt = walkFragment(DESCEND|CBOBJECTS|CBPARAMS, o, ip, NULL, printOPName);
	return 0;
}

