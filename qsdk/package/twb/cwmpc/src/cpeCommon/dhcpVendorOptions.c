/*----------------------------------------------------------------------*
 * Gatespace
 * Copyright 2006 Gatespace Networks, Inc., All Rights Reserved.
 * Gatespace Networks, Inc. confidential material.
 *----------------------------------------------------------------------*
 * File Name  : dhcpVendorOptions.c
 * Description:	A sample methods for accessing the DHCP Encapsulated
 *			Vendor Options that may contain the URL of the ACS and
 *			a Provisioning code.
 *----------------------------------------------------------------------*
 * $Revision: 1.5 $
 *
 * $Id: dhcpVendorOptions.c,v 1.5 2012/05/10 17:38:01 dmounday Exp $
 *----------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <time.h>
#include <string.h>
#include <netinet/in.h>
#include <errno.h>
#include <netdb.h>
#include <syslog.h>

#ifdef DMALLOC
#include "dmalloc.h"
#endif
#include <string.h>
#include "../includes/sys.h"
#include "../includes/paramTree.h"
#include "../includes/rpc.h"
#include "../soapRpc/rpcMethods.h"
#include "../soapRpc/rpcUtils.h"
#include "../soapRpc/notify.h"
#include "../includes/CPEWrapper.h"
#include "targetsys.h"
#include "sysutils.h"

#ifdef DEBUG
#define DBGPRINT(X) fprintf X
#else
#define DBGPRINT(X)
#endif
extern CPEState	cpeState;
CPE_STATUS setDeviceInfo_ProvisioningCode(Instance *ip, char *value);
//CPE_STATUS setManagementServer_CWMPRetryIntervalMultiplier(Instance *ip, char *value);
//CPE_STATUS setManagementServer_CWMPRetryMinimumWaitInterval(Instance *ip, char *value);

/*
 * For this example to function the dhclient.conf must be
 * configured to send the vendor-class-identifier of "dslforum.org"
 * and the /etc/dhclient-exit-hooks shell script must be defined to
 * write the new_vendor_encapsulted_options environment variable to
 * the file /tmp/vendor-options.
 * Contents could be:
 *    echo ${new_vendor_encapsulted_options} >/tmp/vendor-options
 *
 * Busybox and dnsmasq dhcp have their own interfaces to which this
 * code can be adapted.
 */
static char *getOptData( char *buf, int lth, char *p){
	int i = 0;
	while ( i< lth){
		buf[i] = strtol(p, NULL, 16);
		p = strchr(p, ':');
		if ( p )
			++p;
		else {
			break;
		}
		++i;
	}
	return p;
}
void getDHCPVendorOptions(void){
	FILE	*vf;
	char	buf[1024];
	char 	*p;
	char	opt1[257];
	char	opt2[257];
	char	opt3[257];
	char	opt4[257];
	char	tmp[257];
	unsigned char	opt;
	unsigned char	lth;

	memset( opt1, 0, sizeof(opt1));
	memset( opt2, 0, sizeof(opt2));
	memset( opt3, 0, sizeof(opt2));
	memset( opt4, 0, sizeof(opt2));
	if ( (vf = fopen(DHCP_VENDOR_OPT_FILE, "r" )) == NULL ){
		return;
	}
	if ( fgets(buf, sizeof(buf), vf )){
		/* now parse off vendor options */
		/* look for option number 1 and 2 */
		p = buf;
		while ( p ) {
			/* get option number */
			opt = (unsigned char)strtol( p, NULL, 16);
			if ( (p = strchr(p, ':')) ){
				/* get length */
				++p;
				lth = (unsigned char)strtol( p, NULL, 16);
				p = strchr(p, ':');
				if ( p ){
					++p;
					if ( opt == 1)
						p = getOptData( opt1, lth, p);
					else if (opt == 2)
						p = getOptData( opt2, lth, p);
					else if (opt == 3)
						p = getOptData( opt3, lth, p);
					else if (opt == 4)
						p = getOptData( opt4, lth, p);
					else
						p = getOptData( tmp, lth, p	); /* scan past it */
				}
			}
		}
	}
	fclose(vf);

	if ( opt1[0] ){
		if ( cpeState.acsURL ) GS_FREE(cpeState.acsURL);
		cpeState.acsURL = GS_STRDUP( opt1 );
	}
	if ( opt2[0] ){
		strncpy(cpeState.provisioningCodeStr, opt2, 64);
		//setDeviceInfo_ProvisioningCode(NULL, opt2);
	}
	if ( opt3[0] ){
		cpeState.cwmpRetryMin = atoi(opt3);
		//setManagementServer_CWMPRetryMinimumWaitInterval(NULL, opt3);
	}
	if ( opt4[0] ){
		cpeState.cwmpRetryMultiplier = atoi(opt4);
		//setManagementServer_CWMPRetryIntervalMultiplier(NULL, opt4);
	}
}
