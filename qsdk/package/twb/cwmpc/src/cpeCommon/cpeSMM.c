/*----------------------------------------------------------------------*
 * Gatespace
 * Copyright 2011 Gatespace. All Rights Reserved.
 * Gatespace Networks, Inc. confidential material.
 *----------------------------------------------------------------------*
 * File Name  : cpeSSM.c
 *
 * Description: CPE Software Module Management.
 *    This file provides a sample implementation of the message/event
 *    passing between the CWMPc thread and a thread that provides the
 *    implementation of the software module management functions.
 *
 *    Since the CWMPc only blocks on a select waiting for a timer to
 *    expire or an IO to complete any messages send from the SMM thread
 *    to the CWMPc thread must be sent as a UDP packet to the localhost
 *    interface. The SMM thread is started by the CWMPc and the SMM then
 *    waits on work to be added to the workQ in a pthread_cond_wait().
 *    The CWMPc enqueues the work items on the workQ and signals the SMM
 *    with a pthread_cond_signal().
 *
 *    When the SMM thread completes any work or needs to communicate a
 *    change of state to the CWMPc it enqueues the event on the doneQ and
 *    sends a UDP packet containing the CPE_SMM_EVENT message to the
 *    CPELISTENPORT on the localhost. The CWMPc responds by calling the
 *    callback function associated with the event, cpeSMMEvent().
 *
 *    NOTE: The gslib libraries that are used by the CWMPc to handle
 *    asynchronous IO and timer management are not thread safe and should
 *    not be called by the SMM thread without close inspection of the
 *    function being called.
 *
 * $Revision: 1.4 $
 * $Id: cpeSMM.c,v 1.4 2012/06/13 15:43:40 dmounday Exp $
 *----------------------------------------------------------------------*/
#include <syslog.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include "pthread.h"
#include "CPEWrapper.h"
#include "cwmpSession.h"
#include "rpcMethods.h"
#include "paramTree.h"
#include "rpcUtils.h"
#include "cpemsgformat.h"
#include "smm.h"
#include "targetsys.h"
#include "runstates.h"

#define DEBUG
#ifdef DEBUG
#define DBGPRINT(X) fprintf X
#else
#define DBGPRINT(X)
#endif

#include "SoftwareModules.h"
/* forwards */
const char *cwmpGenerateUUID(const char *);
static void initDU( Instance *, const char *oName);

/*
 * Select on of following transfer utilities.
 */
#define USE_CURL
//#define USE_WGET

#define MAX_EXECENV		2
extern CWMPObject CWMP_RootObject[];

#define XFER_DIRECTORY	"/tmp/smm"
static pthread_t	smmThread;

typedef enum {
	WRK_CHANGEDUSTATE,
	WRK_INVALID
}eWORK_REQ;

typedef struct WQEntry {
	struct WQEntry	*next;
	eWORK_REQ req;
	void	*detail;
} WQEntry;
/*
 * workQ:
 * items added by CWMPc thread, removed by SMM thread.
 */
static WQEntry *workQ = NULL;

static pthread_cond_t workQCond;
static pthread_mutex_t workQLock;
/*
 * doneQ:
 * items added by SMM thread, removed by CWMPc thread.
 */
static WQEntry *doneQ = NULL;
static pthread_mutex_t doneQLock;

/*
 * Data model configuration data lock. The CWMPc obtains this lock at the beginning
 * of a session with the ACS and releases it at the end of the session.
 * The SMM thread should obtain a lock if it is modifying any of the Object/Parameter
 * data. After obtaining a data model lock the SMM thread may call cwmp functions
 * that access the data model object tree provided that those functions to not call
 * any of the gslib event/timer functions. The gslib functions are NOT thread-safe.
 */
static pthread_mutex_t dataModelLock = PTHREAD_MUTEX_INITIALIZER;

/*
 * Flag to stop the SMM thread.
 */
static int	stopSMM;
/*
 * socket file descriptor used to send event msgs to CWMPc.
 */
static int	smmfd = -1;

/**
 * enqueue the new work entry and
 * signal the workQCond.
 */
static void enQWork( WQEntry *e ){
	pthread_mutex_lock( &workQLock );
	e->next = workQ;
	workQ = e;
	pthread_mutex_unlock( &workQLock );
	if ( pthread_cond_signal( &workQCond )){
		cpeLog(LOG_ERR, "enQWork cond signal failed");
	}
}

/**
 * Initialize the ExecEnv instances that are supported by this CPE.
 */
Instance *exEnvInstance[MAX_EXECENV];

static void initSWModules(void){
	int i;
	char oName[257];
	Instance *exEnv;
	SoftwareModulesExecEnv *exEnvDescP;
	for ( i=0; i< MAX_EXECENV; ++i){
		snprintf(oName, sizeof(oName), "%s.SoftwareModules.ExecEnv.%d.", CWMP_RootObject[0].name, i+1 );
		exEnv = cwmpInitObjectInstance(oName);
		exEnvInstance[i] = exEnv;		// keep array of Instance * to ExecEnv.
		exEnvDescP = (SoftwareModulesExecEnv *)exEnv->cpeData; // allocated by pAddObj function addSoftwareModulesExecEnv
		if ( i == 0){
			// Init Linux Execution environment -- instance .1.
			memset( exEnvDescP, 0, sizeof(struct SoftwareModulesExecEnv));
			exEnvDescP->status = GS_STRDUP("Installed");
			exEnvDescP->enable = 1;
			exEnvDescP->name = GS_STRDUP("LinuxOSBase");
			exEnvDescP->type = GS_STRDUP("Linux 2.16.xx");
			exEnvDescP->vendor = GS_STRDUP("RedHat");
		} else {
			// Java ExecEnv -- assumes Java is installed. instance .2.
			memset( exEnvDescP, 0, sizeof(struct SoftwareModulesExecEnv));
			exEnvDescP->status = GS_STRDUP("Up");
			exEnvDescP->enable = 1;
			exEnvDescP->name = GS_STRDUP("Java");
			exEnvDescP->type = GS_STRDUP("jvm");
			exEnvDescP->vendor = GS_STRDUP("Sun");
		}
	}
}

#if 0
static Instance* findExecEnvType( const char *targetType ){
	int i;
	Instance *ip;
	for (i=0; i<MAX_EXECENV; ++i){
		ip = exEnvInstance[i];
		if ( ip != NULL){
			SoftwareModulesExecEnv *ep = (SoftwareModulesExecEnv*)ip->cpeData;
			if ( strcmp(ep->type, targetType )== 0)
				return ip;
		} else
			break;
	}
	return NULL;
}
#endif

static Instance *createDUInstance( DUOperationStruct *cDU ){
	char oName[257];
	Instance *instance;
	int i = cpeGetInstanceID();
	snprintf(oName, sizeof( oName ), "%s.SoftwareModules.DeploymentUnit.%d.", CWMP_RootObject[0].name, i );
	instance = cwmpInitObjectInstance(oName);
	return instance;
}

static Instance *findDUInstance( DUOperationStruct *cDU ){
	Instance *ip = NULL; //-Werror=maybe-uninitialized
	CWMPObject *o;
	char path[257];
	snprintf(path, sizeof(path),"%s.SoftwareModules.DeploymentUnit.", CWMP_RootObject[0].name );
	o = cwmpFindObject(path);
	if ( o!= NULL){
		ip = o->iList;
		while ( ip!=NULL ){
			SoftwareModulesDeploymentUnit *dup = (SoftwareModulesDeploymentUnit*)ip->cpeData;
			if (strcmp(cDU->uuid, dup->uUID )==0 ){
				return ip;
				//TODO: check Version and EE if specified.
			}
			ip = ip->next;
		}
	}
	return ip;
}

void freeDeploymentUnit(SoftwareModulesDeploymentUnit *dp ){
	GS_FREE(dp->uUID);
	GS_FREE(dp->dUID);
	GS_FREE(dp->name);
	GS_FREE(dp->status);
	GS_FREE(dp->uRL);
	GS_FREE(dp->vendor);
	GS_FREE(dp->version);
	GS_FREE(dp);
}


Instance *createExecUnitInstance( void ){
	char buf[257];
	Instance *instance;
	SoftwareModulesExecutionUnit *eu;
	int i = cpeGetInstanceID();
	snprintf(buf, sizeof( buf ), "%s.SoftwareModules.ExecutionUnit.%d.", CWMP_RootObject[0].name, i );
	instance = cwmpInitObjectInstance(buf);
	//eu = GS_MALLOC(sizeof (struct SoftwareModulesExecutionUnit));
	//memset( eu, 0, sizeof (struct SoftwareModulesExecutionUnit));
	//instance->cpeData = eu;
	eu = (SoftwareModulesExecutionUnit*)instance->cpeData;
	eu->eUID = GS_STRDUP(itoa(i));    // use instance ID for EUID
	snprintf(buf, sizeof(buf), "EU-%d", i);
	eu->name = GS_STRDUP(buf);
	eu->status = GS_STRDUP("Idle");
	return instance;
}

void freeExecutionUnit(SoftwareModulesExecutionUnit *eu){
	GS_FREE(eu->eUID);
	GS_FREE(eu->name);
	GS_FREE(eu->execEnvLabel);
	GS_FREE(eu->status);
	GS_FREE(eu->requestedState);
	GS_FREE(eu->executionFaultCode);
	GS_FREE(eu->executionFaultMessage);
	GS_FREE(eu->vendor);
	GS_FREE(eu->version);
	GS_FREE(eu);
}

static void removeEU(Instance **iList ){
	Instance *ip;
	CWMPObject *o;
	char path[257];

	ip = *iList++;
	while ( ip!=NULL ){
		snprintf(path, sizeof(path),"%s.SoftwareModules.ExecutionUnit.%d.",
				CWMP_RootObject[0].name,  ip->id);
		o = cwmpFindObject(path);
		if ( o!= NULL){
			Instance *euip = cwmpGetCurrentInstance();
			SoftwareModulesExecutionUnit *eu = (SoftwareModulesExecutionUnit*)euip->cpeData;
			freeExecutionUnit( eu );
			cwmpDeleteInstance(o, euip);
		}
		ip = *iList++;
	}
	return;
}
/**
 * remove the DeploymentUnit instance and backing data as
 * well as any associated ExecutionUnit.
 */
static void removeDU(const char *duInstancePath, SoftwareModulesDeploymentUnit *dup  ){
	Instance *ip;
	CWMPObject *o;
	o = cwmpFindObject( duInstancePath );
	if ( o!= NULL){
		ip = cwmpGetCurrentInstance();
		if ( dup == (SoftwareModulesDeploymentUnit*)ip->cpeData) {
			removeEU( dup->executionUnitList );
			freeDeploymentUnit( dup );
			cwmpDeleteInstance(o, ip);
		}
	}
	return;
}
/*
 * restoreDUConfig
 * 	oName string pointer to "Device.SoftwareModules.DeploymentUnit.17."
 * 	pName pointer to @name
 * 	p pointer to string following " = " (value).
 */
void restoreDUConfig( const char *oName, const char *pName, char *p){
	CWMPObject *o;
	Instance 	*ip, *eeip;
	int		i;
	char 	*name = NULL;
	SoftwareModulesDeploymentUnit *dup;
	if ( (o=cwmpFindObject(oName)) ) {
		ip = cwmpGetCurrentInstance();
		dup = (SoftwareModulesDeploymentUnit *)ip->cpeData;
		if ( streq(pName, "name")){
			name = p;
			initDU(ip, name);
		} else if (streq(pName, "eu")){
			char buf[128];
			snprintf(buf, sizeof(buf), "%s.SoftwareModules.ExecutionUnit.%s", CWMP_RootObject[0].name, p);
			if ( (o=cwmpFindObject(oName)) ) {
				dup->executionUnitList[0] = cwmpGetCurrentInstance();
				dup->executionUnitList[1] = NULL;
			}
		} else if ( streq(pName, "ee")){
			int id = atoi(p);
			for ( i=0; i<MAX_EXECENV; ++i){
				if ( (eeip = exEnvInstance[i]) )
					if ( eeip->id == id ){
						dup->executionEnvRef = eeip;
						break;
					}
			}
		}  else if ( streq(pName, "url")){
			COPYSTR(dup->uRL, p);
		}
	}
}
/*
 * saveDUConfig
 * write the config necessary to restore the DeploymentUnit instance on reboot.
 * data must be written as:
 *  "  @%s = %s\n"
 * Each line is 2 spaces followed by a '@', an ID string and a value. As many lines
 * as necessary may be written.
 */
void saveDUConfig(FILE *cf, CWMPObject *o, Instance *ip){
	Instance *euip;
	SoftwareModulesDeploymentUnit *dup = (SoftwareModulesDeploymentUnit *)ip->cpeData;
	fprintf(cf, "  @name = %s\n", dup->name);
	fprintf(cf, "  @url = %s\n", dup->uRL);
	if ( (euip = dup->executionUnitList[0]))
		fprintf(cf, "  @eu = %d\n", dup->executionUnitList[0]->id);  // only one EU.
	if ( (euip = dup->executionEnvRef ))
		fprintf(cf, "  @ee = %d\n", ip->id);	// save ExecEnv ref.
}
void restoreEUConfig(const char *oName, const char *pName, char *p){
	CWMPObject *o;
	Instance 	*ip;

	SoftwareModulesExecutionUnit *eu;
	if ( (o=cwmpFindObject(oName)) ) {
		ip = cwmpGetCurrentInstance();
		eu = (SoftwareModulesExecutionUnit *)ip->cpeData;
		if ( streq(pName, "name")){
			COPYSTR(eu->name, p);
			COPYSTR(eu->eUID, itoa( ip->id ));   // set EUID to instance ID.
		} else if ( streq(pName, "evl")){
			COPYSTR(eu->execEnvLabel, p);
		} else if ( streq(pName, "status")){
			COPYSTR(eu->status, p);
		}

	}
}
/*
 * Called by the setSoftwareModulesExecutionUnit_RequestedState function to
 * initiate the state change. This is called by the CWMPc thread within the
 * session.
 * TODO: make this asynchronous by passing the request to the SMM thread
 * and returning immediately.
 */
CPE_STATUS cpeChangeRequestedState(SoftwareModulesExecutionUnit *p,	const char *value) {
	char dir[512];
	char cwd[512];
	int ret = CPE_OK;
	int s;

	if (strcmp(value, "Active") == 0) {
		// config is locked should be able to do this from the CWMPc thread.
		if ( strcmp(p->status, "Idle")== 0 || strcmp(p->status, "Stopping")==0 ){
			// try to start.
			snprintf(dir, sizeof(dir), "%s", p->execEnvLabel); // location of DU
			getcwd(cwd, sizeof(cwd)); // save current pwd.
			chdir(dir); // change dir to expanded download
			DBGPRINT((stderr, "starting: %s\n", dir));
			s = system("./run.sh");
			chdir(cwd); // restore working dir.
			if ( s != 0 ){
				COPYSTR(p->executionFaultCode, "FailureOnStart");
			} else {
				COPYSTR(p->status, "Active");
				COPYSTR(p->executionFaultCode, "NoFault");
			}
		} else if ( strcmp(p->status, "Stopping") == 0){
			ret = CPE_9003;
		}
	} else if ( strcmp(value, "Idle")==0){
		if ( (strcmp(p->status, "Active")==0) || (strcmp(p->status, "Starting")==0)){
			// try to start.
			snprintf(dir, sizeof(dir), "%s", p->execEnvLabel); // location of DU
			getcwd(cwd, sizeof(cwd)); // save current pwd.
			chdir(dir); // change dir to expanded download
			DBGPRINT((stderr, "stopping: %s\n", dir));
			COPYSTR(p->status, "Stopping");
			s = system("./stop.sh");
			chdir(cwd); // restore working dir.
			if ( s != 0 ){
				COPYSTR( p->executionFaultCode, "FailureOnStop");
			} else {
				COPYSTR( p->status, "Idle");
				COPYSTR( p->executionFaultCode, "NoFault");
			}
		} // else ignore.
	} else
		// parameter error.
		ret = CPE_9003;
	return ret;
}

CPE_STATUS commitSoftwareModulesExecutionUnit(struct CWMPObject *o, Instance *ip, eCommitCBType ect){
	if (cwmpGetRunState()== RUNSTATE_RESTORE_PARAM){
		/* only execute this if the RUNSTATE is being restored from a re-start */
		SoftwareModulesExecutionUnit *eu = (SoftwareModulesExecutionUnit *)ip->cpeData;
		if ( streq(eu->status, "Active") ){
			// start the ExecutionUnit
			char status[8];
			strcpy(status, eu->status);   // change status so function call will work.
			COPYSTR(eu->status, "Idle");
			return cpeChangeRequestedState(eu, status);
		}
	}
	return CPE_OK;
}
/*
 * Saves ExecutionUnit configuration.
 */
void saveEUConfig(FILE *cf, CWMPObject *o, Instance *ip){
	SoftwareModulesExecutionUnit *eu = (SoftwareModulesExecutionUnit*)ip->cpeData;
	fprintf(cf, "  @name = %s\n", eu->name);
	fprintf(cf, "  @evl = %s\n", eu->execEnvLabel);
	fprintf(cf, "  @status = %s\n", eu->status);
}
/*
 * Read first line of file into line upto maxSz.
 * return 1: file opened and read
 *        0: unable to open or no data read.
 */
static int readFile( const char *fname, char *line, const int maxSz ){
	FILE *inf;
	int result = 0;
	if ( (inf = fopen( fname, "r" ))){
		result = fgets( line, maxSz, inf )!=NULL;
		int lth = strlen(line);
		if ( line[lth-1]=='\n')
			line[lth-1]='\0'; // suppress the ending newline.
		fclose(inf);
	}
	return result;
}
static void initDU( Instance *ip, const char *oName){
	char moduleVendor[129];
	char moduleVersion[33];
	char buf[512];
	SoftwareModulesDeploymentUnit *du = (SoftwareModulesDeploymentUnit *)ip->cpeData;
	snprintf(buf, sizeof(buf), "%s/VENDOR", oName);
	if ( !readFile(buf, moduleVendor, sizeof(moduleVendor)) )
		strcpy(moduleVendor, "none");
	snprintf(buf, sizeof(buf), "%s/VERSION", oName);
	if ( !readFile(buf, moduleVersion, sizeof(moduleVersion)))
		strcpy(moduleVersion, "");

	du->name = GS_STRDUP( oName );   //TODO:
	du->dUID = GS_STRDUP( itoa(ip->id));  // Instance id is used to uninstall.
	du->version = GS_STRDUP(moduleVersion);
	du->vendor = GS_STRDUP(moduleVendor);
	// uUID in result is set by CWMPc if a UUID is specified in the RPC.
	if ( du->uUID == NULL ) {
		//No UUID specified calculate from the name and vendor.
		char encodename[128];
		www_UrlEncode( du->name, encodename); // % escape any non-alpha
		snprintf(buf, sizeof(buf), "%s.%s.", encodename, moduleVendor);
		du->uUID = GS_STRDUP(cwmpGenerateUUID(buf));
	}
	du->resolved = 1;
}
/*
 * Install Deployment Unit
 * Return 0: OK
 * 		 -1: Error in deployment.
 */
int smmInstall( DUOperationStruct *cDU, CPEOpResultStruct *result){
	// Download using wget
	char cmd[512];
	char abuf[256];
	char buf[1024];
	char oName[128];
	FILE *fp;
	if ( cDU->userName && cDU->passWord ){
#ifdef USE_CURL
		snprintf(abuf , sizeof(abuf), " --user %s:%s ", cDU->userName, cDU->passWord);
#endif
#ifdef USE_WGET
		snprintf(abuf , sizeof(abuf), " --http-user=%s --http-passwd=%s ", cDU->userName, cDU->passWord);
#endif
	} else {
		abuf[0]='\0';
	}
	// Check if url is from Gatespace ACS and isolate filename for output.
	// ..../imageDownload.seam?fileName=knopflerfish_osgi_tiny_3.2.0.tgz
	char *p = strstr(cDU->url, "?fileName=");
	if ( p!=NULL){
		snprintf(oName, sizeof(oName), "%s/%s ", XFER_DIRECTORY, p+ strlen("?fileName="));
	} else {
		if ( (p = strrchr(cDU->url, '/')) ){
			p++;        // strip any leading directory names.
			snprintf(oName, sizeof(oName), "%s/%s ", XFER_DIRECTORY, p);
		} else
			oName[0] = '\0';
	}
#ifdef USE_CURL
	if ( strlen(oName) >0 )
		snprintf(cmd, sizeof(cmd), "/usr/bin/curl %s -o %s %s 2>&1\n", abuf, oName, cDU->url );
	else
		snprintf(cmd, sizeof(cmd), "/usr/bin/curl %s -o %s %s 2>&1\n", abuf, XFER_DIRECTORY, cDU->url);
#endif
#ifdef USE_WGET
	if ( strlen(oName)>0 )
		snprintf(cmd, sizeof(cmd), "/usr/bin/wget -r -nH --cut-dirs=1 --directory-prefix=%s -O %s %s %s  2>&1\n",
			                   XFER_DIRECTORY, oName, abuf, cDU->url);
	else
		snprintf(cmd, sizeof(cmd), "/usr/bin/wget -r -nH --cut-dirs=1 --directory-prefix=%s %s %s  2>&1\n",
			                   XFER_DIRECTORY, abuf, cDU->url);
#endif
	result->startTime = time(NULL);
	result->faultCode = 0;
	/* the 2>&1 also writes stderr into the stdout pipe */
	fprintf(stderr, "Start transfer: %s", cmd);
	if ((fp = popen(cmd, "r")) == NULL) {
		cpeLog(LOG_ERR, "Could not start >%s<", cmd);
		// set result;
	}

	// read pipe for results of transfer...
	while ( fgets(buf, sizeof(buf), fp) != NULL ){
		DBGPRINT((stderr, "buf=>%s<", buf));
#ifdef USE_CURL
		if ( strstr(buf, "No such") || strstr(buf, "Failed writing")){
			result->faultCode = 9017;
			result->faultString = GS_STRDUP(cpeGetFaultIndexedMessage(FAULT9017));
			break;
		} else if ( strstr(buf, "\r100 ") ){
			// if line begins with '100 ' then the file was transfered.
			result->faultCode = 0;
			break;
		} else if (strstr( buf, "not resolve host")){
			/* unknown host xxxxxxxxx */
			result->faultCode = 9015;
			result->faultString = GS_STRDUP( cpeGetFaultIndexedMessage(FAULT9015));
			break;
		} else {
			;// flush
		}
#endif
#ifdef USE_WGET
		if ( strstr(buf, "Length: ")  ) {
			// xfer complete
			result->faultCode = 0;
			break;
		} else if ( strstr(buf, "Resolving") && strstr(buf, "failed")){
			/* unknown host xxxxxxxxx */
			result->faultCode = 9015;
			result->faultString = GS_STRDUP( cpeGetFaultIndexedMessage(FAULT9015), sizeof(result->faultString));
			break;
		} else if ( strstr(buf, "ERROR") && strstr(buf, "Not Found")  ) {
			/* unable to access file */
			result->faultCode = 9016;
			result->faultString = GS_STRDUP(cpeGetFaultIndexedMessage(FAULT9016), sizeof(result->faultString));
			break;
		} else if ( strstr(buf, "Authorization failed")){
			/*  */
			result->faultCode = 9019;
			result->faultString = GS_STRDUP(cpeGetFaultIndexedMessage(FAULT9019), sizeof(result->faultString));
			break;
		}
		 /* other failures, noise, .....*/
#endif
	}
	/* EOF on pipe indicates the wget program has completed */
	DBGPRINT((stderr, "Transfer faultCode=%d faultString=%s\n", result->faultCode, result->faultString));
	result->completeTime = time(0);
	pclose(fp);
	if ( result->faultCode == 0 ){
		cpeLockConfiguration(); // >>>>>>>>>>>>>>>>> lock for changes to object/parameter tree.
		if ( strstr(oName, ".tgz") ){
			// unpack download. The unpacked code must be in a directory with the uri name.
			snprintf(buf, sizeof(buf), "tar -x --directory=%s -f  %s", XFER_DIRECTORY, oName);
			int s = system(buf);
			fprintf(stderr, "system return value = %d\n", s);
			if ( s!= 0) {
				result->faultCode = 9029;
				result->faultString = GS_STRDUP( cpeGetFaultIndexedMessage(FAULT9029));
			} else {
				char *p;
				// strip .tgz suffix from cmd
				if ( (p = strrchr( oName, '.')))
					*p = '\0';
				// Create the .SoftwareModules.DeploymentUnit. instance.
				Instance *duIp =createDUInstance( cDU );
				SoftwareModulesDeploymentUnit *du = (SoftwareModulesDeploymentUnit *)duIp->cpeData;
				result->duRef = cwmpGetInstancePathStr( duIp );
				result->currentState = GS_STRDUP("Installed");
				initDU( duIp, oName);
				// initialize DeploymentUnit parameters
				du->status = GS_STRDUP("Installed");
				du->uRL = GS_STRDUP(cDU->url);
				result->version = GS_STRDUP( du->version );
				result->resolved = du->resolved;
				result->uuid = GS_STRDUP(du->uUID);
				// If there is an sh script by the name run.sh in the unpacked dir then
				// create a ExecutionUnit instance.

				snprintf(buf, sizeof(buf), "%s/run.sh", oName);
				if ( access( buf, X_OK )==0 ){
					// the run.sh script is present - create the EU
					Instance *ip = createExecUnitInstance();
					SoftwareModulesExecutionUnit *eu = (SoftwareModulesExecutionUnit*)ip->cpeData;
					du->executionUnitList[0] = ip;
					du->executionEnvRef = exEnvInstance[1]; //TODO: choose more smartly.
					result->euRefList = cwmpGetPathRefRowsStr(du->executionUnitList);
					eu->execEnvLabel = GS_STRDUP( oName );  // location of EU files.
					eu->vendor = GS_STRDUP(du->vendor);
					eu->version = GS_STRDUP(du->version);
				} else {
					// no EU required.
				}

			}
		} else {
			result->currentState = GS_STRDUP("Failed");
			result->resolved = 0;
			result->faultCode = 9028;
			result->faultString = GS_STRDUP( cpeGetFaultIndexedMessage(FAULT9028));
		}
		cpeUnlockConfiguration(1); // <<<<<<<<<<<<<<<<<<<<<<<<< unlock
	}
	return result->faultCode==0? 0: -1;
}

int smmUpdate( DUOperationStruct *cDU, CPEOpResultStruct *result){

	DBGPRINT((stderr, "smmUpdate UUID: %s\n", cDU->uuid));
	cpeLockConfiguration();
	result->faultCode = 9000;
	result->faultString = GS_STRDUP( cpeGetFaultIndexedMessage(FAULT9000));
	cpeUnlockConfiguration(0);
	return -1;
}

int smmUninstall( DUOperationStruct *cDU, CPEOpResultStruct *result){

	char cwd[512];
	cpeLockConfiguration();
	DBGPRINT((stderr, "smmUninstall UUID: %s\n", cDU->uuid));
	Instance *duIp = findDUInstance( cDU );
	if ( duIp != NULL){
		SoftwareModulesDeploymentUnit *dup = (SoftwareModulesDeploymentUnit*)duIp->cpeData;
		result->duRef = cwmpGetInstancePathStr( duIp );
		result->version = GS_STRDUP(dup->version);
		COPYSTR(dup->status, "Uninstalling");
		result->euRefList = cwmpGetPathRefRowsStr(dup->executionUnitList);
		Instance *euIp = dup->executionUnitList[0];  // only one in this environment
		if ( euIp != NULL) {
			// stop anything that might be running.
			SoftwareModulesExecutionUnit *eu = (SoftwareModulesExecutionUnit*)euIp->cpeData;
			getcwd(cwd, sizeof(cwd)); // save current pwd.
			chdir( eu->execEnvLabel ); //
			DBGPRINT((stderr, "stopping EU for unInstall: %s\n", eu->execEnvLabel));
			system("./stop.sh");
			//TODO: remove DU file here. Not implemented in test code to avoid damage.
			chdir(cwd); // restore working directory
		}
		removeDU( result->duRef, dup );
		result->currentState = GS_STRDUP("Uninstalled");
		result->resolved = 1;
		result->faultCode = CPE_OK;
	} else {
		result->faultCode = 9000;
		result->faultString = GS_STRDUP( cpeGetFaultIndexedMessage(FAULT9000));
	}
	cpeUnlockConfiguration(1);
	return result->faultCode-1;
}
/*
 * Step thru the chained list of OPlist
 */
void smmChangeDUState( CPEChangeDUState *cDU ){
	DUOperationStruct *duOp = cDU->ops;
	CPEOpResultStruct *result = cDU->results; // result structures are preallocated by CWMPc.
	while ( duOp ){
		switch( duOp->op ){
		case eInstallOp:
			smmInstall(duOp, result);
			break;
		case eUpdateOp:
			smmUpdate(duOp, result);
			break;
		case eUninstallOp:
			smmUninstall(duOp, result);
			break;
		case eNoOp:
		default:
			break;
		}
		duOp = duOp->next;
		result = result->next;
	}
	return;
}
/**
 * smm thread interface functions
 */
/**
 * When the SMM functions need to notify the CWMPc of completed work
 * or a change of state they should queue the event on the doneQ and
 * send a CPE_SMM_Event via a UDP message.
 */
static void smmEnQWorkDone( WQEntry *e){
	fprintf(stdout, "smmEnqQWorkDone() \n");
	pthread_mutex_lock( &doneQLock );
	e->next = doneQ;  	// enqueue work entry
	doneQ = e;
	pthread_mutex_unlock( &doneQLock );
	if ( e ){
		CPEEvent evt;
		ssize_t sz;
		evt.eventCode = CPE_SMM_EVENT;
#ifndef USE_UNIX_DOMAIN_SOCKET
		struct sockaddr_in addr;
		memset(&addr, 0, sizeof(addr));
		addr.sin_family = AF_INET;
		addr.sin_port = htons(CPELISTENPORT);
		addr.sin_addr.s_addr = htonl(0);

		sz = sendto( smmfd, &evt, sizeof(evt.eventCode, 0, (struct sockaddr*)&addr, sizeof(struct sockaddr_in));
#else
		struct sockaddr_un addr;
		memset(&addr, 0, sizeof(struct sockaddr_un));
		addr.sun_family = AF_UNIX;
		strcpy(addr.sun_path,CPELISTEN_NAME);
		sz = sendto( smmfd, &evt, sizeof(evt.eventCode), 0, (struct sockaddr*)&addr, sizeof(struct sockaddr_un));
#endif
		if ( sz != sizeof(int))
			fprintf(stdout, "Unable to send event - error = %s\n", strerror(errno) );
	}
}
/**
 * The sample smm thread starts and wait on a request from the
 * CWMPc which is queued on the work queue.
 */
static void * cpeSMMStart(void *arg){
	WQEntry	*we;
	fprintf(stdout, "smmStart() SMM thread started\n");
	// get socket to send msgs to CWMPc thread.
#ifndef USE_UNIX_DOMAIN_SOCKET
	smmfd = socket(PF_INET,SOCK_DGRAM, IPPROTO_UDP);
#else
	smmfd = get_domain_socket( CPELISTEN_NAME "sndr" ); // append "sndr" to path name.
#endif

	while ( !stopSMM ){
		if ( pthread_mutex_lock( &workQLock )){
			cpeLog(LOG_ERR, "workQLock mutex failed");
			return NULL;
		}
		// wait on workQ
		while ( workQ == NULL ){
			if ( pthread_cond_wait( &workQCond, &workQLock)){
				cpeLog(LOG_ERR, "workQCond cond_wait failed");
				pthread_mutex_unlock( &workQLock );
				return NULL;
			}
			if (stopSMM){
				pthread_mutex_unlock( &workQLock );
				return NULL; // exit thread.
			}
		}
		// dequeue item
		we = workQ;
		workQ = we->next;
		pthread_mutex_unlock( &workQLock );
		// do work.....
		switch (we->req) {
		case WRK_CHANGEDUSTATE:
			smmChangeDUState( we->detail );
			smmEnQWorkDone(we);
			break;
		default:
			break;
		}
	}

	return NULL;
}
/*
 * SMM thread signaled CWMPc thread which callback is here.
 * check if an completed work on doneQ.
 */
void cpeSMMEvent(void){
	WQEntry *we;
	fprintf(stdout, "cpeSMMEvent() event received from SMM thread\n");
	if ( pthread_mutex_lock( &doneQLock )){
		cpeLog(LOG_ERR, "doneQ mutex lock failed %d", errno);
		return;
	}
	// Dequeue completed work or other info and apply to CWMPc state
	// if required.
	while ( (we = doneQ) ){
		// item found
		doneQ = we->next;
		pthread_mutex_unlock ( &doneQLock );
		switch (we->req) {
		case WRK_CHANGEDUSTATE:
			cwmpDUStateChangeComplete((CPEChangeDUState *) we->detail );
			break;
		// extended req here....
		default:
			break;
		}
		GS_FREE(we);	// free work entry. Any detail data must be freed in response handlers.
		pthread_mutex_lock( &doneQLock );
	}
	//
	pthread_mutex_unlock ( &doneQLock );
}
/**
 * Called on CWMPc startup.
 *
 */
void cpeSMMInit(void){
	static pthread_attr_t smmAttr;
	// Add Linux ExecEnv instance.
#if 0
	initSWModules();		// create ExecEnv instances.
#endif
	// This example code starts the smm thread.
	fprintf(stdout, "Start SMM thred and initialize pthread locks\n");
	stopSMM = 0;
	pthread_cond_init(&workQCond, NULL);
	pthread_mutex_init(&workQLock, NULL);
	pthread_mutex_init(&doneQLock, NULL);
	pthread_attr_init(&smmAttr);
	if ( pthread_create( &smmThread, &smmAttr, cpeSMMStart, NULL)){
		cpeLog(LOG_ERR, "Unable to start smm Thread");
		return;
	}

}
/**
 * optional clean up routine to stop the smm thread and
 * clean up.
 */
void cpeSMMStop(void) {
	void *rs;
	stopSMM = 1;
	pthread_cond_signal(&workQCond);
	pthread_join( smmThread, &rs);

}

/*
* This function is called whenever the CWMPC initiates a session
 * with the ACS to obtain a dataModelLock. May be called by SMM thread to
 * lock configuration for updates.
*/
static int cpeLockCnt = 0;
void cpeLockConfiguration(void)
{
	++cpeLockCnt;
	pthread_mutex_lock( &dataModelLock );
	fprintf(stderr, "cpeLockConfiguration( LockCnt=%d)\n", cpeLockCnt);
}
/*
* This function is called whenever the CWMPC terminates a session
* with the ACS to release the dataModelLock.
*/
void cpeSaveConfig(void);
void cpeUnlockConfiguration(int changeCfgCnt )
{
	--cpeLockCnt;
	fprintf(stderr, "cpeUnlockConfiguration( LockCnt=%d, changed=%s)\n",
			cpeLockCnt, changeCfgCnt? "True": "False");
	if ( changeCfgCnt ) // save before unlocking.
		cpeSaveConfig();
	pthread_mutex_unlock( &dataModelLock );

}

/**
 * Put the ChangeDUSTate on the SMM work queue. This is called by
 * the CWMPc after the ACS session completes to initiate the state changes.
 * Return a 0 or a CWMP Fault code if unable to start the
 * state change requests.
 *
 */
int cpeChangeDUState( CPEChangeDUState *cpeDU ){
	WQEntry *e;
	cpeDbgLog(DBG_CHANGEDU, "cpeChangeDUState()");
	// pass to the SMM thread via the workQ.
	if ( (e=(WQEntry*)GS_MALLOC(sizeof (struct WQEntry )))){
		e->req = WRK_CHANGEDUSTATE;
		e->detail = cpeDU;
		enQWork( e );
		return 0;
	}
	return 9004;

}
