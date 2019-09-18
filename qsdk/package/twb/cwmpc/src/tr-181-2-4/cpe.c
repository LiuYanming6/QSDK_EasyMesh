/*----------------------------------------------------------------------*
 * Gatespace Networks, Inc.
 * Copyright 2011 Gatespace Networks, Inc., All Rights Reserved.
 * Gatespace Networks, Inc. confidential material.
 *----------------------------------------------------------------------*
 * File Name  :
 *
 * Description: Instance initialization for xxx
 * 				data model.
 *              Specific to a Linux OS.
 *							*
 * $Revision: 1.1 $
 * $Id: cpe.c,v 1.1 2012/05/10 17:37:59 dmounday Exp $
 *----------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <sys/types.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <netinet/in.h>
#include <netdb.h>
#include <syslog.h>

#ifdef DMALLOC
	#include "dmalloc.h"
#endif
#include "sys.h"
#include "gslib/src/utils.h"
#include "gslib/src/event.h"
#include "gslib/src/protocol.h"
#include "paramTree.h"
#include "CPEWrapper.h"
#include "soapRpc/acsconnreq.h"
#include "soapRpc/rpcUtils.h"
#include "soapRpc/notify.h"
#include "soapRpc/cwmpSession.h"
#include "runstates.h"
#include "sysutils.h"
#include "netIPState.h"
#define DEBUG
#ifdef DEBUG
#define DBGPRINT(X) fprintf X
#define DEBUGLOG 1
#else
#define DBGPRINT(X)
#define DEBUGLOG 0
#endif

#include "targetsys.h"
#include "DeviceInfo.h"
#include "secret.h"

#define STRINGIZE(x) #x
#define STRINGIZE_VALUE_OF(x) STRINGIZE(x)


extern CWMPObject CWMP_RootObject[];
extern CPEState	cpeState;
extern void initEthernetInterface(void);
void initSoftwareModules(void);
void cpeSMMInit(void);
int cpeRestoreConfig(void);
void cpeRestoreCPEState(void);
void getDHCPVendorOptions(void);
void cpeSaveConfig(void);
/*
 * if there was no existing saved configuration the discovery
 * parameter is set to 1.
 */

static void createBasicInstances(int discovery) {

	cpeNetInit(discovery);
	#ifdef CONFIG_RPCCHANGEDUSTATE
	/* create initial SoftwareModules. Start the SMM thread and initialize the ExecEnv */
	cpeSMMInit();
	#endif
	//Instance *ip = initEthernetInterfaceLink();
	// The IP.Interface.1 interface is writeable so the config restore will create it.
}

/*
* The cpeRefreshInstances .
 * This function should create any new object instances that have been add as a result
 * of a new CPE configuration item being added. It should also delete any object
 * instances that no longer have corresponding CPE configuration items.
*/
#ifdef SERVICES
extern void servicesInstanceRefresh(void);
#endif

void cpeRefreshInstances(void) {
	/* Update any instances that have been modified by local CPE configuration */
	/* functions. */
	fprintf(stdout, "cpeRefreshInstances()\n");
	/* Update the CPE predefined instances here*/
    refreshAssociatedDeviceinstances();
}

/*
* This is called from the CWMPC main startup before any other initialization.

* The steps here should be:
* 1. restore the object and parameter data and cpeState from persistent memory.
* 2. If necessary create instances and children of the TR-069 objects to match the configuration data.
* 3. Restore any notification attributes associated with the instances.
*
*/
extern EthernetLink *cpeNetIF;
int cpeBootUpCPEFirst = 1;

void cpeBootUpCPEInit(void)
{
	int			discovery;
	cwmpSetRunState(RUNSTATE_CPEINIT);
#ifdef GEN_CONNREQ_PATH
	/* Connection Request URL formation.
	 * Add code here to generate the random path that is unique to
	 * the CPE device. See section 3.2.2.
	 * The length must be no more than CRPATH_SZ and must begin
	 * with a '/'.
	 */
	char rstring[CRPATH_SZ-1] = {0};
	randomstring(rstring, sizeof(rstring)-1);
    sprintf(cpeState.connReqPath, "/%s", rstring);
#else
	/*
	 *
	 * A fixed path is much better for testing.
	 */
	strcpy(cpeState.connReqPath, ACSCONNREQ_PATH);
#endif
	/*
	 * Set server timeout
	 */
	wget_SetServerTimeout( SERVER_CONNECT_TIMEOUT );

	/*
	 * Try to restore from configuration data.
	 */
	discovery = !cpeRestoreConfig();
	cpeRestoreCPEState();       /* need to get the ACS URL for cpeGetNetIFInfo() */
	createBasicInstances(discovery);		/* create basic object instances */
	/* try to restore saved object and parameter configuration */


	if ( EMPTYSTR(cpeState.acsURL)) {
		/* no ACS URL in configuration. Attempt to get it from DHCP vendor options */
		getDHCPVendorOptions();
	}
	cpeRefreshCPEData( &cpeState );

#if 0
	if ( EMPTYSTR(cpeState.acsUser)) {
		if ( (elp=cpeNetIF) ) {
			/* create username from the MAC */
			char mac[30];
			u_char umac[6];
			char buf[40];
			readMac(umac, elp->mACAddress);
			snprintf(mac, sizeof(mac), "%s", writeCanonicalMacUCase(umac));
			strncpy(buf, mac, 6);
			strncpy(buf+6, "-", 1);
			strcpy(buf+7, &mac[6]);
			cpeState.acsUser = GS_STRDUP(buf);
			if (cpeState.acsPW == NULL) {
				char pw[20];
				strcpy(pw, &mac[6]);
				cpeState.acsPW = GS_STRDUP(pw);
			}
		}
	}
#else
	if ( EMPTYSTR(cpeState.acsUser) )
	{
		char path[256];
		char *val = NULL;

		snprintf(path, sizeof(path), "%s.DeviceInfo.SerialNumber", CWMP_RootObject[0].name);
		CWMPParam *p = cwmpFindParameter(path);
		if(p)
		{
			if(p->pGetter && p->pGetter(cwmpGetCurrentInstance(), &val) == CPE_OK)
			{
				cpeState.acsUser = GS_STRDUP(val);
				if(cpeState.acsUser[strlen(cpeState.acsUser)-1] == '\n')
					cpeState.acsUser[strlen(cpeState.acsUser)-1] = '\0';
				if( cpeState.acsPW == NULL )
				{
					/* create password from the MAC */
					char mac[30] = {0};
					char cmd[128]  = {0};
					char pw[25]={0};
					char secret[20]={0};
					char prepassword[25]={0};
                    int i=0;
//					char prepassword[20]={0};
					strcpy(secret, TWB_SECRET);
/*
#if defined(AUTO_SECRET)
					strcpy(secret, STRINGIZE_VALUE_OF(AUTO_SECRET));
#endif

#if defined(AUTO_PREPASSWORD)
					strcpy(prepassword, STRINGIZE_VALUE_OF(AUTO_PREPASSWORD));
#endif
*/
                    cmd_popen("state_cfg get prepassword",prepassword);
                    i = strlen(prepassword);
                    if( i !=0 && prepassword[0] != '\n')
                    {
                        if (prepassword[i-1] == '\n')
                            prepassword[i-1] = '\0';

                        sprintf(cmd, "twbencode %s %s", prepassword , secret);
                    }
                    else
                    {
                        cmd_popen("state_cfg get ethaddr | sed -r 's/://g'", mac);
                        sprintf(cmd, "twbencode %s %s %s", mac, GS_STRDUP(val) , secret);
                    }
                    cmd_popen( cmd , pw );

                    cpeState.acsPW = GS_STRDUP(pw);
                    if(cpeState.acsPW[strlen(cpeState.acsPW)-1] == '\n')
                        cpeState.acsPW[strlen(cpeState.acsPW)-1] = '\0';
                }
                GS_FREE(val);
            }
        }
    }
	cpeLog(LOG_INFO, "acsUser:[%s], length:[%d]", cpeState.acsUser, cpeState.acsUser?strlen(cpeState.acsUser):0);
	cpeLog(LOG_INFO, "acsPW:[%s], length:[%d]", cpeState.acsPW, cpeState.acsPW?strlen(cpeState.acsPW):0);
#endif


	/* init Parameter Notification Tracking data */
	cwmpInitNotifyTracking(INIT_NOTIFY); /* first set default attributes */
	cwmpRestoreNotifyAttributes(); /* this will call cpeRestoreNotifyAttributes() to read */
	                               /* data saved by cpeSaveNotifyAttributes               */
	cwmpInitNotifyTracking(UPDATE_COPY); /* now make a tracking copy of all ACTIVE or PASSIVE param data*/

	cpeBootUpCPEFirst=0;
	cwmpCheckValueChange();
	cpeSaveConfig();

}
