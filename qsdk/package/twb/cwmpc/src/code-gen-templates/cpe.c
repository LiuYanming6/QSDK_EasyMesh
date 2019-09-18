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
 * $Revision: 1.2 $
 * $Id: cpe.c,v 1.2 2012/05/10 17:38:08 dmounday Exp $
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



extern CWMPObject CWMP_RootObject[];
extern CPEState	cpeState;
extern void initEthernetInterface(void);
void initSoftwareModules(void);
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

void cpeBootUpCPEInit(void)
{
	int			discovery;
	EthernetLink *elp;
	cwmpSetRunState(RUNSTATE_CPEINIT);
#ifdef GEN_CONNREQ_PATH
	/* Connection Request URL formation.
	 * Add code here to generate the random path that is unique to
	 * the CPE device. See section 3.2.2.
	 * The length must be no more than CRPATH_SZ and must begin
	 * with a '/'.
	 */
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

	/* init Parameter Notification Tracking data */
	cwmpInitNotifyTracking(INIT_NOTIFY); /* first set default attributes */
	cwmpRestoreNotifyAttributes(); /* this will call cpeRestoreNotifyAttributes() to read */
	                               /* data saved by cpeSaveNotifyAttributes               */
	cwmpInitNotifyTracking(UPDATE_COPY); /* now make a tracking copy of all ACTIVE or PASSIVE param data*/

	cpeSaveConfig();

}
