/*
 * readdhcpoptions.c
 *
 *  Created on: Feb 25, 2009
 *      Author: dmounday
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>

#include <string.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <netdb.h>
#include <linux/if.h>
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
#include "../tr104/targetsys.h"

#ifdef DEBUG
#define DBGPRINT(X) fprintf X
#else
#define DBGPRINT(X)
#endif
extern void getDHCPVendorOptions(void);
CPEState cpeState;


static char *provCode;
CPE_STATUS setDeviceInfo_ProvisioningCode(void *ip, char *value)
{
	/* Set parameter */
	if ( provCode )
		GS_FREE(provCode);
	provCode = GS_STRDUP(value);
	return 0;
	/*return CPE_ERR;*/
}

int main(int argc, char *argv[]) {

	getDHCPVendorOptions();
	fprintf( stdout, "ACSURL:           %s\n", cpeState.acsURL);
	fprintf( stdout, "ProvisioningCode: %s\n", provCode);
}
