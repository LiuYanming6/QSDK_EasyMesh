/*
 * tableTest.c
 *
 *  Created on: Nov 8, 2011
 *      Author: dmounday
 */
#include <unistd.h>
#include <stdio.h>
#include <syslog.h>
#include "cpemsgformat.h"

#include "../../includes/rpc.h"
#include "../cwmpSession.h"
#include "../../tr181-2-2/targetsys.h"
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

/**  UDPConnReq:1, SM_Baseline:1, Baseline:2, Baseline:1,
**//** Device.DNS.Client.Server.{i}. */

CPEGETFUNC(getDNSClientServer_Enable);
CPESETFUNC(setDNSClientServer_Enable);
CPEGETFUNC(getDNSClientServer_Status);
CPEGETFUNC(getDNSClientServer_Alias);
CPESETFUNC(setDNSClientServer_Alias);
CPEGETFUNC(getDNSClientServer_DNSServer);
CPESETFUNC(setDNSClientServer_DNSServer);
CPEGETFUNC(getDNSClientServer_Interface);
CPESETFUNC(setDNSClientServer_Interface);
CPEGETFUNC(getDNSClientServer_Type);
CWMPParam DNSClientServer_Params[]={
	{ "Enable",
		getDNSClientServer_Enable,
		setDNSClientServer_Enable,
		NULL,
		eBoolean,
		0,
		0},
	{ "Status",
		getDNSClientServer_Status,
		NULL,
		NULL,
		eString,
		0,
		0},
	{ "Alias",
		getDNSClientServer_Alias,
		setDNSClientServer_Alias,
		NULL,
		eString,
		0,
		64},
	{ "DNSServer",
		getDNSClientServer_DNSServer,
		setDNSClientServer_DNSServer,
		NULL,
		eString,
		0,
		45},
	{ "Interface",
		getDNSClientServer_Interface,
		setDNSClientServer_Interface,
		NULL,
		eString,
		0,
		256},
	{ "Type",
		getDNSClientServer_Type,
		NULL,
		NULL,
		eString,
		0,
		0},
	{NULL}
};
CPEADDOBJ(addDNSClientServer);
CPEDELOBJ(delDNSClientServer);
/** Device.DNS.Client. */

CPEGETFUNC(getDNSClient_Enable);
CPESETFUNC(setDNSClient_Enable);
CPEGETFUNC(getDNSClient_Status);
CPEGETFUNC(getDNSClient_ServerNumberOfEntries);
CWMPParam DNSClient_Params[]={
	{ "Enable",
		getDNSClient_Enable,
		setDNSClient_Enable,
		NULL,
		eBoolean,
		0,
		0},
	{ "Status",
		getDNSClient_Status,
		NULL,
		NULL,
		eString,
		0,
		0},
	{ "ServerNumberOfEntries",
		getDNSClient_ServerNumberOfEntries,
		NULL,
		NULL,
		eUnsignedInt,
		0,
		0},
	{NULL}
};
/** Device.DNS.Client.  */

CWMPObject DNSClient_Objs[]={
	{"Server",
		delDNSClientServer,
		addDNSClientServer,
		NULL,
		NULL,
		DNSClientServer_Params,
		NULL,
		eInstance},
	{NULL}
};

/** Device.DNS. */

CPEGETFUNC(getDNS_SupportedRecordTypes);
CWMPParam DNS_Params[]={
	{ "SupportedRecordTypes",
		getDNS_SupportedRecordTypes,
		NULL,
		NULL,
		eString,
		0,
		0},
	{NULL}
};
/** Device.DNS.  */

CWMPObject DNS_Objs[]={
	{"Client",
		NULL,
		NULL,
		NULL,
		DNSClient_Objs,
		DNSClient_Params,
		NULL,
		eObject},
	{NULL}
};

/** Device.DeviceInfo. */

CPEGETFUNC(getDeviceInfo_Manufacturer);
CPEGETFUNC(getDeviceInfo_ManufacturerOUI);
CPEGETFUNC(getDeviceInfo_ModelName);
CPEGETFUNC(getDeviceInfo_Description);
CPEGETFUNC(getDeviceInfo_SerialNumber);
CPEGETFUNC(getDeviceInfo_HardwareVersion);
CPEGETFUNC(getDeviceInfo_SoftwareVersion);
CPEGETFUNC(getDeviceInfo_ProvisioningCode);
CPESETFUNC(setDeviceInfo_ProvisioningCode);
CPEGETFUNC(getDeviceInfo_UpTime);
CWMPParam DeviceInfo_Params[]={
	{ "Manufacturer",
		getDeviceInfo_Manufacturer,
		NULL,
		NULL,
		eString,
		0,
		64},
	{ "ManufacturerOUI",
		getDeviceInfo_ManufacturerOUI,
		NULL,
		NULL,
		eString,
		0,
		6},
	{ "ModelName",
		getDeviceInfo_ModelName,
		NULL,
		NULL,
		eString,
		NOACTIVENOTIFY,
		64},
	{ "ProductClass",
		getDeviceInfo_ProductClass,
		NULL,
		NULL,
		eString,
		NOACTIVENOTIFY,
		64},
	{ "Description",
		getDeviceInfo_Description,
		NULL,
		NULL,
		eString,
		NOACTIVENOTIFY,
		256},
	{ "SerialNumber",
		getDeviceInfo_SerialNumber,
		NULL,
		NULL,
		eString,
		0,
		64},
	{ "HardwareVersion",
		getDeviceInfo_HardwareVersion,
		NULL,
		NULL,
		eString,
		FORCED_INFORM,
		64},
	{ "SoftwareVersion",
		getDeviceInfo_SoftwareVersion,
		NULL,
		NULL,
		eString,
		FORCED_INFORM|FORCED_ACTIVE,
		64},
	{ "ProvisioningCode",
		getDeviceInfo_ProvisioningCode,
		setDeviceInfo_ProvisioningCode,
		NULL,
		eString,
		FORCED_INFORM|FORCED_ACTIVE,
		64},
	{ "UpTime",
		getDeviceInfo_UpTime,
		NULL,
		NULL,
		eUnsignedInt,
		NOACTIVENOTIFY,
		0},
	{NULL}
};
/** Device.InterfaceStack.{i}. */

CPEGETFUNC(getInterfaceStack_HigherLayer);
CPEGETFUNC(getInterfaceStack_LowerLayer);
CWMPParam InterfaceStack_Params[]={
	{ "HigherLayer",
		getInterfaceStack_HigherLayer,
		NULL,
		NULL,
		eString,
		0,
		256},
	{ "LowerLayer",
		getInterfaceStack_LowerLayer,
		NULL,
		NULL,
		eString,
		0,
		256},
	{NULL}
};
/** Device.LANConfigSecurity. */

CPEGETFUNC(getLANConfigSecurity_ConfigPassword);
CPESETFUNC(setLANConfigSecurity_ConfigPassword);
CWMPParam LANConfigSecurity_Params[]={
	{ "ConfigPassword",
		getLANConfigSecurity_ConfigPassword,
		setLANConfigSecurity_ConfigPassword,
		NULL,
		eStringSetOnly,
		0,
		64},
	{NULL}
};

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
		SoftwareModulesDeploymentUnit_Params,
		NULL,
		eInstance},
	{"ExecEnv",
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
		SoftwareModulesExecutionUnit_Objs,
		SoftwareModulesExecutionUnit_Params,
		NULL,
		eInstance},
	{NULL}
};

/** Device. */

CPEGETFUNC(get_InterfaceStackNumberOfEntries);
CWMPParam Device_Params[]={
	{ "InterfaceStackNumberOfEntries",
		get_InterfaceStackNumberOfEntries,
		NULL,
		NULL,
		eUnsignedInt,
		0,
		0},
	{NULL}
};
/** Device.  */

CWMPObject Device_Objs[]={
	{"DNS",
		NULL,
		NULL,
		NULL,
		DNS_Objs,
		DNS_Params,
		NULL,
		eObject},
	{"DeviceInfo",
		NULL,
		NULL,
		NULL,
		NULL,
		DeviceInfo_Params,
		NULL,
		eObject},
	{"InterfaceStack",
		NULL,
		NULL,
		NULL,
		NULL,
		InterfaceStack_Params,
		NULL,
		eInstance},
	{"LANConfigSecurity",
		NULL,
		NULL,
		NULL,
		NULL,
		LANConfigSecurity_Params,
		NULL,
		eObject},
	{"SoftwareModules",
		NULL,
		NULL,
		NULL,
		SoftwareModules_Objs,
		SoftwareModules_Params,
		NULL,
		eObject},
	{NULL}
};

/** CWMP ROOT Object Table  */

CWMPObject CWMP_RootObject[]={
	{"Device",
		NULL,
		NULL,
		NULL,
		Device_Objs,
		Device_Params,
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
static int id = 0;


int cpeGetInstanceID(){
	return ++id;
}
CPEState cpeState;


/*
 * returns !=0 if the path ends in a instance sequence.
 * I.E.  LANDevice.1.
 *   (last two chars are a digit followed by a . )
 */
static int isInstance( char *path ) {
    int	j = strlen(path);
    if (path[j-1]=='.') {
        /* last char is . */
        int i = j-1;
        /* find . before instance digits */
        while ( i-->0 && isdigit( path[i]) );
        if ( path[i]=='.')
            return atoi( &path[i+1]);
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
	printf("Use: tablesTest\n");
	printf("\n");
}
extern char wpPathName[];
static int printOPName(CWMPObject *o, CWMPParam *p, Instance *ip,
									void *wxp, void *paramCnt, int unused ) {
	if ( p == NULL )
		fprintf(stdout, "ERROR no parameter: %s\n", o->name);
	else
		fprintf(stdout, "cb: %s\n", wpPathName);
	/***
	else if (ip->id != 0 ){
		fprintf(stdout, "cb: %s.%d.%s\n", o->name, ip->id, p->name);
	} else
		fprintf(stdout, "cb: %s.%s\n", o->name, p->name);
		***/
    return eOK;
}

const char *smm = "Device.SoftwareModules.";
const char *smmEU1 = "Device.SoftwareModules.ExecutionUnit.1.";

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
	fprintf(stdout, "walk: %s\n", smm);
    if ( (o = cwmpFindObject(smm))) {
    	if ( isInstance(smm) && !ISINSTANCE(o) )
    		fprintf(stdout, "fault 9005\n");
    	else {
            ip = isInstance(smm)? cwmpGetCurrentInstance(): NULL;
            pCnt = walkFragment(DESCEND|CBPARAMS, o, ip, NULL, printOPName);
    	}
    } else
        fprintf(stdout, "Object not found\n");
    fprintf(stdout, "\n");
	snprintf(buf, sizeof( buf ), "%s.SoftwareModules.ExecEnv.1.", CWMP_RootObject[0].name );
	instance = cwmpInitObjectInstance(buf);
	snprintf(buf, sizeof( buf ), "%s.SoftwareModules.ExecEnv.2.", CWMP_RootObject[0].name );
	instance = cwmpInitObjectInstance(buf);

	i = cpeGetInstanceID();
	snprintf(buf, sizeof( buf ), "%s.SoftwareModules.ExecutionUnit.%d.", CWMP_RootObject[0].name, i );
	instance = cwmpInitObjectInstance(buf);
	fprintf(stdout, "walk: %s after create instance ExecutionUnit.i\n", smm);
    if ( (o = cwmpFindObject(smm))) {
    	if ( isInstance(smm) && !ISINSTANCE(o) )
    		fprintf(stdout, "fault 9005\n");
    	else {
            ip = isInstance(smm)? cwmpGetCurrentInstance(): NULL;
            pCnt = walkFragment(DESCEND|CBPARAMS, o, ip, NULL, printOPName);
    	}
    } else
        fprintf(stdout, "Object not found\n");
    fprintf(stdout, "\n");
    i = cpeGetInstanceID();
	snprintf(buf, sizeof( buf ), "%s.SoftwareModules.DeploymentUnit.%d.", CWMP_RootObject[0].name, i );
	instance = cwmpInitObjectInstance(buf);
	fprintf(stdout, "walk: %s after create instance DeploymentUnit.i\n", smm);
	if ( (o = cwmpFindObject(smm))) {
		if ( isInstance(smm) && !ISINSTANCE(o) )
			fprintf(stdout, "fault 9005\n");
		else {
			ip = isInstance(smm)? cwmpGetCurrentInstance(): NULL;
			pCnt = walkFragment(DESCEND|CBPARAMS, o, ip, NULL, printOPName);
		}
	} else
		fprintf(stdout, "Object not found\n");
	fprintf(stdout, "\n");
	fprintf(stdout, "walk: %s \n", smmEU1);
	if ( (o = cwmpFindObject(smmEU1))) {
			if ( isInstance(smmEU1) && !ISINSTANCE(o) )
				fprintf(stdout, "fault 9005\n");
			else {
				ip = isInstance(smmEU1)? cwmpGetCurrentInstance(): NULL;
				pCnt = walkFragment(DESCEND|CBPARAMS, o, ip, NULL, printOPName);
			}
		} else
			fprintf(stdout, "Object not found\n");
		fprintf(stdout, "\n");


	return 0;
}
