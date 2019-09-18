/*
 * smmTest.c
 *
 *  Created on: Oct 17, 2011
 *      Author: dmounday
 */
#include <unistd.h>
#include <stdio.h>
#include <syslog.h>
#include "../../includes/cpemsgformat.h"

#include "../../includes/rpc.h"
#include "../cwmpSession.h"
#include "../../tr-181-2-4/targetsys.h"
#include "../../includes/CPEWrapper.h"
#include "../../soapRpc/rpcMethods.h"
#include "../smm.h"
#define DEBUG
#ifdef DEBUG
#define DBGPRINT(X) fprintf X
#else
#define DBGPRINT(X)
#endif
EventPolicy eventPolicy[1];
ACSSession acsSession;

static int fd;

void cpeSMMInit(void);

/** Device.SoftwareModules.DeploymentUnit.{i}. */

CPEGETFUNC(getSoftwareModulesDeploymentUnit_UUID);
CPEGETFUNC(getSoftwareModulesDeploymentUnit_DUID);
CPEGETFUNC(getSoftwareModulesDeploymentUnit_Name);
CPEGETFUNC(getSoftwareModulesDeploymentUnit_Status);
CPEGETFUNC(getSoftwareModulesDeploymentUnit_Resolved);
CPEGETFUNC(getSoftwareModulesDeploymentUnit_URL);
CPEGETFUNC(getSoftwareModulesDeploymentUnit_Vendor);
CPEGETFUNC(getSoftwareModulesDeploymentUnit_Version);
CPEGETFUNC(getSoftwareModulesDeploymentUnit_ExecutionUnitList);
CPEGETFUNC(getSoftwareModulesDeploymentUnit_ExecutionEnvRef);
CWMPParam SoftwareModulesDeploymentUnit_Params[]={
	{ "UUID",
		getSoftwareModulesDeploymentUnit_UUID,
		NULL,
		NULL,
		eString,
		0,
		36},
	{ "DUID",
		getSoftwareModulesDeploymentUnit_DUID,
		NULL,
		NULL,
		eString,
		0,
		64},
	{ "Name",
		getSoftwareModulesDeploymentUnit_Name,
		NULL,
		NULL,
		eString,
		0,
		64},
	{ "Status",
		getSoftwareModulesDeploymentUnit_Status,
		NULL,
		NULL,
		eString,
		NOACTIVENOTIFY,
		0},
	{ "Resolved",
		getSoftwareModulesDeploymentUnit_Resolved,
		NULL,
		NULL,
		eBoolean,
		0,
		0},
	{ "URL",
		getSoftwareModulesDeploymentUnit_URL,
		NULL,
		NULL,
		eString,
		0,
		1024},
	{ "Vendor",
		getSoftwareModulesDeploymentUnit_Vendor,
		NULL,
		NULL,
		eString,
		0,
		128},
	{ "Version",
		getSoftwareModulesDeploymentUnit_Version,
		NULL,
		NULL,
		eString,
		0,
		32},
	{ "ExecutionUnitList",
		getSoftwareModulesDeploymentUnit_ExecutionUnitList,
		NULL,
		NULL,
		eString,
		0,
		0},
	{ "ExecutionEnvRef",
		getSoftwareModulesDeploymentUnit_ExecutionEnvRef,
		NULL,
		NULL,
		eString,
		0,
		0},
	{NULL}
};
/** Device.SoftwareModules.ExecEnv.{i}. */

CPEGETFUNC(getSoftwareModulesExecEnv_Enable);
CPESETFUNC(setSoftwareModulesExecEnv_Enable);
CPEGETFUNC(getSoftwareModulesExecEnv_Status);
CPEGETFUNC(getSoftwareModulesExecEnv_Name);
CPEGETFUNC(getSoftwareModulesExecEnv_Type);
CPEGETFUNC(getSoftwareModulesExecEnv_Vendor);
CPEGETFUNC(getSoftwareModulesExecEnv_Version);
CPEGETFUNC(getSoftwareModulesExecEnv_ActiveExecutionUnits);
CWMPParam SoftwareModulesExecEnv_Params[]={
	{ "Enable",
		getSoftwareModulesExecEnv_Enable,
		setSoftwareModulesExecEnv_Enable,
		NULL,
		eBoolean,
		0,
		0},
	{ "Status",
		getSoftwareModulesExecEnv_Status,
		NULL,
		NULL,
		eString,
		0,
		0},
	{ "Name",
		getSoftwareModulesExecEnv_Name,
		NULL,
		NULL,
		eString,
		0,
		32},
	{ "Type",
		getSoftwareModulesExecEnv_Type,
		NULL,
		NULL,
		eString,
		0,
		64},
	{ "Vendor",
		getSoftwareModulesExecEnv_Vendor,
		NULL,
		NULL,
		eString,
		0,
		128},
	{ "Version",
		getSoftwareModulesExecEnv_Version,
		NULL,
		NULL,
		eString,
		0,
		32},
	{ "ActiveExecutionUnits",
		getSoftwareModulesExecEnv_ActiveExecutionUnits,
		NULL,
		NULL,
		eString,
		NOACTIVENOTIFY,
		0},
	{NULL}
};
/** Device.SoftwareModules.ExecutionUnit.{i}. */

CPEGETFUNC(getSoftwareModulesExecutionUnit_EUID);
CPEGETFUNC(getSoftwareModulesExecutionUnit_Name);
CPEGETFUNC(getSoftwareModulesExecutionUnit_ExecEnvLabel);
CPEGETFUNC(getSoftwareModulesExecutionUnit_Status);
CPEGETFUNC(getSoftwareModulesExecutionUnit_RequestedState);
CPESETFUNC(setSoftwareModulesExecutionUnit_RequestedState);
CPEGETFUNC(getSoftwareModulesExecutionUnit_ExecutionFaultCode);
CPEGETFUNC(getSoftwareModulesExecutionUnit_ExecutionFaultMessage);
CPEGETFUNC(getSoftwareModulesExecutionUnit_Vendor);
CPEGETFUNC(getSoftwareModulesExecutionUnit_Version);
CPEGETFUNC(getSoftwareModulesExecutionUnit_References);
CPEGETFUNC(getSoftwareModulesExecutionUnit_SupportedDataModelList);
CWMPParam SoftwareModulesExecutionUnit_Params[]={
	{ "EUID",
		getSoftwareModulesExecutionUnit_EUID,
		NULL,
		NULL,
		eString,
		0,
		64},
	{ "Name",
		getSoftwareModulesExecutionUnit_Name,
		NULL,
		NULL,
		eString,
		0,
		32},
	{ "ExecEnvLabel",
		getSoftwareModulesExecutionUnit_ExecEnvLabel,
		NULL,
		NULL,
		eString,
		0,
		64},
	{ "Status",
		getSoftwareModulesExecutionUnit_Status,
		NULL,
		NULL,
		eString,
		DEFAULT_ACTIVE,
		0},
	{ "RequestedState",
		getSoftwareModulesExecutionUnit_RequestedState,
		setSoftwareModulesExecutionUnit_RequestedState,
		NULL,
		eString,
		0,
		0},
	{ "ExecutionFaultCode",
		getSoftwareModulesExecutionUnit_ExecutionFaultCode,
		NULL,
		NULL,
		eString,
		0,
		0},
	{ "ExecutionFaultMessage",
		getSoftwareModulesExecutionUnit_ExecutionFaultMessage,
		NULL,
		NULL,
		eString,
		0,
		256},
	{ "Vendor",
		getSoftwareModulesExecutionUnit_Vendor,
		NULL,
		NULL,
		eString,
		0,
		128},
	{ "Version",
		getSoftwareModulesExecutionUnit_Version,
		NULL,
		NULL,
		eString,
		0,
		32},
	{ "References",
		getSoftwareModulesExecutionUnit_References,
		NULL,
		NULL,
		eString,
		0,
		0},
	{ "SupportedDataModelList",
		getSoftwareModulesExecutionUnit_SupportedDataModelList,
		NULL,
		NULL,
		eString,
		0,
		0},
	{NULL}
};
/** Device.SoftwareModules.ExecutionUnit.{i}.  */

CWMPObject SoftwareModulesExecutionUnit_Objs[]={
	{"Extensions",
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		eObject},
	{NULL}
};


/** Device.SoftwareModules. */

CPEGETFUNC(getSoftwareModules_ExecEnvNumberOfEntries);
CPEGETFUNC(getSoftwareModules_DeploymentUnitNumberOfEntries);
CPEGETFUNC(getSoftwareModules_ExecutionUnitNumberOfEntries);
CWMPParam SoftwareModules_Params[]={
	{ "ExecEnvNumberOfEntries",
		getSoftwareModules_ExecEnvNumberOfEntries,
		NULL,
		NULL,
		eUnsignedInt,
		0,
		0},
	{ "DeploymentUnitNumberOfEntries",
		getSoftwareModules_DeploymentUnitNumberOfEntries,
		NULL,
		NULL,
		eUnsignedInt,
		0,
		0},
	{ "ExecutionUnitNumberOfEntries",
		getSoftwareModules_ExecutionUnitNumberOfEntries,
		NULL,
		NULL,
		eUnsignedInt,
		0,
		0},
	{NULL}
};
/** Device.SoftwareModules.  */

CWMPObject SoftwareModules_Objs[]={
	{"DeploymentUnit",
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		SoftwareModulesDeploymentUnit_Params,
		NULL,
		eInstance},
	{"ExecEnv",
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		SoftwareModulesExecEnv_Params,
		NULL,
		eInstance},
	{"ExecutionUnit",
		NULL,
		NULL,
		NULL,
		NULL,
		SoftwareModulesExecutionUnit_Objs,
		SoftwareModulesExecutionUnit_Params,
		NULL,
		eInstance},
	{NULL}
};

CWMPObject Device_Objs[]={
	{"SoftwareModules",
		NULL,
		NULL,
		NULL,
		NULL,
		SoftwareModules_Objs,
		SoftwareModules_Params,
		NULL,
		eObject},
	{NULL}
};

CWMPObject CWMP_RootObject[]={
	{"Device",
		NULL,
		NULL,
		NULL,
		NULL,
		Device_Objs,
		NULL,
		NULL,
		eObject},
	{NULL}
};

/*
 * test flags
 */
int installUninstallOnly = 1;
int	unInstallRunning;
static int id = 0;


int cpeGetInstanceID(){
	return ++id;
}
CPEState cpeState;

#if 0
/* just return some memory for the Instance desc.
 *
 */

Instance *cwmpInitObjectInstance( char *path){
	Instance *i = (Instance *)malloc(sizeof (struct Instance));
	ips[id-1] = i;
	return i;
}
/*
 *
 */
const char* cwmpGetInstancePathPtr(const Instance *ip){
	return "Device.SoftwareModules.X.1.";
}

void cwmpGetObjInstanceCntStr(const char *oname, char **value) {
	*value = GS_STRDUP("1");
}
#endif

/** mock up functions */
static void cpeReadEvent(void *handle)
{
	int fd = (int)handle;
	CPEEvent    buf;
	DBGPRINT((stderr, "cpeReadEvent()\n"));
	if ( read(fd, &buf, sizeof(buf))>0 ) {
		DBGPRINT((stderr, "cpeReadEvent called: event=%0x\n", buf.eventCode));
		if ( buf.eventCode == CPE_SMM_EVENT ){
			cwmpSMMEventReceived();
		}
	}
	return;
}


void startCPEEventListener(void)
{   int fd;
#ifdef USE_UNIX_DOMAIN_SOCKET
	if ( (fd = get_domain_socket(CPELISTEN_NAME ))<0)
		return;
#else
	InAddr anyAddr= {AF_INET, {{htonl(INADDR_ANY)}}};

	if ((fd = udp_listen(&anyAddr, CPELISTENPORT, NULL, FALSE)) < 0) {
		cpeLog(LOG_ERR, "could not initiate UDP server socket (port=%d)", CPELISTENPORT);
		return;
	}
#endif /*USE_UNIX_DOMAIN_SOCKET */

	setListener( fd, cpeReadEvent, (void *)fd );
	return;
}
void stopCPEEventListener(void){
	stopListener(fd);
}

void cpeSaveConfig(void){
	fprintf(stdout,"cpeSaveConfig(()\n");
}

void changeDUStateRPC(void){
	RPCRequest	rpc;
	ChangeDUStateMsg	*dp = &rpc.ud.changeDUState;
	DUOperationStruct 	*op;
	DUOperationStruct 	**oList = &dp->opList;
	memset(&rpc, 0, sizeof(rpc));
	op = (DUOperationStruct *)malloc( sizeof(struct DUOperationStruct ));
	memset(op, 0, sizeof(struct DUOperationStruct));
	*oList = op;
	op->op = eInstallOp;
	//op->url = "http://192.168.3.8/download/knopflerfish_osgi_tiny_3.2.0.tgz";
	op->url = "http://acs-a3.gatespace.net/acsadmin/software_download/imageDownload.seam?fileName=knopflerfish_osgi_tiny_3.2.0.tgz";
	op->userName = "abc123";
	op->passWord = "abc123";
	op->version="1.0";
	cwmpQueueDUStateChange( &rpc );
	//startDUStateChange( NULL );
	// wait for smm thread to send back event.
}

void unInstallChangeDUStateRPC(void){
	RPCRequest	rpc;
	ChangeDUStateMsg	*dp = &rpc.ud.changeDUState;
	DUOperationStruct 	*op;
	DUOperationStruct 	**oList = &dp->opList;
	memset(&rpc, 0, sizeof(rpc));
	op = (DUOperationStruct *)malloc( sizeof(struct DUOperationStruct ));
	memset(op, 0, sizeof(struct DUOperationStruct));
	*oList = op;
	op->op = eUninstallOp;
	op->uuid="123456";
	op->version="1.0";
	cwmpQueueDUStateChange( &rpc );
}

void stopDelaying(void *handle){
	fprintf(stdout, "Stop delay complete\n");
}

int startCnt = 2;
void startChgCmpltSession( void *);

void unInstall(void *handle){
	fprintf(stdout, "unInstall\n");
	unInstallChangeDUStateRPC();
	notifyCallbacks(&acsSession); // should startDUStateChange().
	unInstallRunning =1;

}

void startDelaying( void *handle ){
	fprintf(stdout, "Stop EU\n");
	CWMPObject *o = cwmpFindObject("Device.SoftwareModules.ExecutionUnit.");
	if ( o ){
		Instance *ip = o->iList;  // only one for this test.
		setSoftwareModulesExecutionUnit_RequestedState(ip, "Idle");
		if ( --startCnt>0 ){
			setTimer( startChgCmpltSession, NULL, 5000);
			fprintf(stdout, "Delay 5 secs and set Active again.\n");
		} else {
			setTimer(unInstall, NULL, 10000);
			fprintf(stdout, "Delay 5 and uninstall.\n");
		}
	}else {
		fprintf(stderr, "Unable to locate SM EU object to seet Idle\n");
	}

}
void startChgCmpltSession( void *handle 	){
	fprintf(stdout, "Start ChangeDUComplete Session\n");
	if ( installUninstallOnly ){
		fprintf(stdout, "  testing install / uninstall only\n");
		unInstall(NULL);
		return;
	}
	CWMPObject *o = cwmpFindObject("Device.SoftwareModules.ExecutionUnit.");
	if ( o ){
		Instance *ip = o->iList;  // only one for this test.
		if ( ip!= NULL ){
			setSoftwareModulesExecutionUnit_RequestedState(ip, "Active");
			setTimer(startDelaying, NULL, 5000);
		} else {
			fprintf(stderr, "ERROR: Unable to find Device.SoftwareModules.ExecutionUnit. instance\n");
		}
	} else {
		fprintf(stderr, "Unable to locate SM EU object to set Active\n");
	}
}
void cwmpAddEvent ( eEventCode e ){
	fprintf(stdout, "cwmpAddEvent code= %d\n", e);
	if ( e == eEvtDUStateChangeComplete )
		fprintf(stdout, "        eEvtDUStateChangeComplete\n");
	if ( unInstallRunning ){ // done with uninstall...
		CPEChangeDUState *cpeDU = cwmpGetCompletedDUChange();
		if ( cpeDU ) {
			CPEOpResultStruct *r = cpeDU->results;
			while ( r ) {
				fprintf(stdout, "Uninstall result: %d\n", r->faultCode);
				fprintf(stdout, "UUID %s\n", r->uuid);
				fprintf(stdout, "DURef:  %s\n", r->duRef);
				fprintf(stdout, "EXRefList: %s\n",r->euRefList );
				r = r->next;
			}
		}

		setTimer(stopDelaying, NULL, 5000);
		stopCPEEventListener();
		cpeSMMStop();  // stop SMM thread.
	}
}
int cwmpIsACSSessionActive(void){
	return 0;
}

void usage(void) {
	printf("Use: smmTest\n");
	printf("\n");
}

int main( int argc, char *argv[] ) {

	initGSLib();		/* initialize the gslib timer and event handlers */
	cpeInitLog(0xff);
	/* top level Instance initialization */
	if ( cwmpBootUpInstanceInit() == NULL ) {
		cpeLog(LOG_ERR, "Unable to create initial instances");
		return -1;
	}
	setCallback(&eventPolicy, startChgCmpltSession, NULL);
	startCPEEventListener();
	cpeSMMInit();		/* Start the SMM thread */
	changeDUStateRPC();
	notifyCallbacks(&acsSession); // should startDUStateChange().
	eventLoop();		/* this returns when there are no timers or listeners */
	printf("eventLoop() terminated\n");
	return 0;
}
