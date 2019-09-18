/*----------------------------------------------------------------------*
 * Gatespace
 * Copyright 2007 Gatespace Networks, Inc., All Rights Reserved.
 *----------------------------------------------------------------------*
 * File Name  : main.c
 * Description:	The main() function.
 *----------------------------------------------------------------------*
 * $Revision: 1.3 $
 *
 * $Id: stuntest.c,v 1.3 2011/11/16 14:09:56 dmounday Exp $
 *----------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/time.h>
#include <syslog.h>
#include <string.h>


#ifdef DMALLOC
	#include "dmalloc.h"
#endif
#include "../includes/sys.h"
#include "../gslib/src/utils.h"
#include "../gslib/src/event.h"
#include "../gslib/src/protocol.h"
#include "../includes/paramTree.h"
#include "../includes/CPEWrapper.h"
#include "../soapRpc/acsconnreq.h"
#include "../soapRpc/rpcUtils.h"
#include "../soapRpc/notify.h"
#include "../soapRpc/cwmpSession.h"

#include "../tr111/tr111p2.h"
#include "stun.h"

#define DEBUG
#ifdef DEBUG
#define DBGPRINT(X) fprintf X
#else
#define DBGPRINT(X)
#endif

extern StunState stunState;
unsigned	acsSession;      /* satisfy externs */

int cwmpIsACSSessionActive(){
	return 0;
}
EventPolicy eventPolicy[1];
CPEState cpeState;
void cwmpCheckValueChange(void)
{
	printf("cwmpCheckValueChange()\n");
	return;
}


static void initTest(void){
	// cpeIP and serverAddr set by main() getopt().
	//stunState.cpeIP = readIp("192.168.10.105");
	//stunState.serverAddr = strdup("66.179.112.189");
	//stunState.serverAddr = strdup("192.168.10.105");
	//stunState.serverAddr = strdup("stun.xten.com");
	stunState.serverPort = 3478;
	stunState.enable =1;

	stunState.minKeepAlive = 12;
	stunState.maxKeepAlive = 180;
}


CPEState cpeState;
static void initTasks(void)
{
	cpeState.connReqUser = "don";
	cpeState.connReqPW ="abc123";
	initTest();
	startTR111p2();

}

void helpmsg(){
	printf("Use: stuntest <options> \n");
	printf("\n");
	printf("  -l <localhost-IP>\n");
	printf("  -s <STUN-server-name>\n");
	printf("  [-u <STUN username>]\n");
	printf("  [-p <STUN password>]\n");
	printf("  -h This help\n");
}
int main(int argc, char** argv)
{
	char    *host = "192.168.10.105";

	extern int optind;
	extern char *optarg;
	char	c;

	stunState.username = NULL;
	stunState.password = NULL;

	while ((c=getopt(argc, argv, "hl:s:u:p:")) != -1) {
		switch (c) {

		case 'h':
			helpmsg();
			exit(0);
			break;
		case 'l':
			host = strdup( optarg );
			break;
		case 's':
			stunState.serverAddr = strdup ( optarg );
			break;
		case 'u':
			stunState.username = strdup( optarg );
			break;
		case 'p':
			stunState.password = strdup( optarg );
			break;
		default:
			break;
		}
	}

	readInIPAddr( &cpeState.ipAddress, host );
	initGSLib();	/* required to initialize gslib timer and event handlers */
	cpeInitLog(1);

	initTasks();
	eventLoop();		/* this returns when there are no timers or listeners */
	return 0;
}
