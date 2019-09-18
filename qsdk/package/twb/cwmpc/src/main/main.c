/*----------------------------------------------------------------------*
 * Gatespace
 * Copyright 2006 Gatespace Networks, Inc., All Rights Reserved.
 *----------------------------------------------------------------------*
 * File Name  : main.c
 * Description:	The main() function.
 *----------------------------------------------------------------------*
 * $Revision: 1.20 $
 *
 * $Id: main.c,v 1.20 2012/06/13 11:18:47 dmounday Exp $
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
#include <time.h>
#include <syslog.h>
#include <string.h>
#include <signal.h>

#ifdef DMALLOC
	#include "dmalloc.h"
#endif
#include "targetsys.h"
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

#ifdef CONFIG_TR111P2
#include "../tr111/tr111p2.h"
#endif
#define DEBUG
#ifdef DEBUG
#define DBGPRINT(X) fprintf X
#else
#define DBGPRINT(X)
#endif
int dns_lookup_auto(const char *name, int sockType, unsigned int port, int family, InAddr *);
void startCPEEventListener(void);

#ifdef CONFIG_CLIENTAPI
int cwmpStartCPEAPIListener(void);
#endif

/* */
extern CPEState cpeState;
/*
 * Global default network interface name
 * and WAN side IP address.
 * If using IPv6 for an IGD the WAN side IP address used
 * to contact the ACS should be included as a program parameter in
 * order to resolve the parameter path name of the External IP address
 * in the Inform parameter list.
 */
char	*defaultNetworkIF = "eth0";
char	*defaultWANIP;
static char *cipherList = NULL;
int		no_ca = 0;

static void initCpeState(void)
{
	memset(&cpeState, 0, sizeof (CPEState));
	cpeState.informEnabled = 1;		/* periodic inform */
	cpeState.informInterval = 3600; /* default to once per hour */
	/* The following initial session state is defined in the */
	/* targetsys.h header file. */
	cpeState.acsContactState = ACS_INITIAL_SESSION_STATE;
}
static void initTasks(void)
{
	/* Init cpeState variables to their default init values */
	/* May be over-written by cpeBootUpCPEInit              */
	initCpeState();
	/* top level Instance initialization */
	if ( cwmpBootUpInstanceInit() == NULL ) {
		cpeLog(LOG_ERR, "Unable to create initial instances");
		return;
	}
	/* init CPE configuration interface */
	cpeBootUpCPEInit();
#ifndef CONFIG_CLIENTAPI
	if ( EMPTYSTR(cpeState.acsURL)){
		/* if no ACS URL then we can not start the cwmpc */
		cpeLog(LOG_ERR,"Cannot start with empty ACS URL");
		return;
	}
#else
	/* or the clientapi may set the ACS URL and other configuration */
	/* data and then start the ACS sessions. */

	/* setup to listen for CPE config events */
	cwmpStartCPEAPIListener();
#endif

	/* INIT Protocol http, ssl */
	if ( cipherList == NULL )
		cipherList = ACS_CIPHERS;
	proto_Init(no_ca?cipherList:"DEFAULT", no_ca?"":SERVER_CERT, CLIENT_CERT);

	/* start listening for event messages from native CPE processes. */
	startCPEEventListener();

	/* STUN client startup */
	#ifdef CONFIG_TR111P2
	startTR111p2();
	#endif
	/* listen for ACS connection requests */
	cwmpStartACSConnReqListener();
	/* start ACS communications */
	cwmpStartACSInform();
}

static void daemonize(void) {

	cpeLog(LOG_DEBUG,"Daemonizing process ");
	if (fork()!=0) exit(0);
	setsid();

}
void signal_handler(int signum)
{
	if (signum == SIGUSR1)
	{
		cpeState.sigusr1 = 1;
	}
}

void reconnect(int signum)
{
    if (signum == SIGUSR2)
    {
        setTimer(updateConnection, NULL, 120*1000);
    }
}

#ifdef USE_CWMP_MAIN

int main(int argc, char** argv)
{
	int verbose = 0;
	int no_daemonize=1;
	/*
	FILE *pidfile;
	*/
	int     i;
	#ifdef DMALLOC
		dmalloc_debug_setup("debug=0x4f47d03,log=log.%d");
	#endif
	/* getopt doesn't work in snapgear env */
	for (i=1; i< argc; ++i) {
		if ( strcmp( argv[i],"-v")==0) {
			verbose = 1;
		} else if (strcmp(argv[i], "-i")==0) {
			defaultNetworkIF = argv[++i];
		} else if (strcmp(argv[i], "-a")==0){
			defaultWANIP = argv[++i];
		} else if (strcmp(argv[i], "-d")==0) {
			no_daemonize = 1;
		} else if (strcmp(argv[i], "-b")==0) {
			no_daemonize = 0;
		} else if (strcmp(argv[i], "-D")==0) {
			cpeSetDbgLogMask( strtol(argv[++i], NULL, 16));
		} else if (strcmp(argv[i], "-c")==0) {
			cipherList = argv[++i];
		} else if (strcmp(argv[i], "-n")==0) {
			no_ca = 1;
		} else if (strcmp(argv[i], "-h")==0) {
			printf("Options:\n");
			printf("  -i <network interface name>");
			printf("  -a <WAN IP address>");
			printf("  -d - don't daemonize\n");
			printf("  -b run in background");
			printf("  -n - No CA SSL connection\n");
			printf("  -D <debugMask-in-hex i.e. 0x00000003>\n");
			printf("  -v - verbose output to stderr\n");
			printf("  -h - this help\n");
			exit(-1);
		}
	}
	signal(SIGUSR1,signal_handler);
	signal(SIGUSR2,reconnect);
	/*
	mkdirs(CWMP_DIR);
	pidfile = fopen(CWMP_PID_FILE, "w");
	if (pidfile != NULL) {
			fprintf(pidfile, "%d\n", getpid());
			fclose(pidfile);
	}
	*/
	initGSLib();		/* initialize the gslib timer and event handlers */
	cpeInitLog(verbose);
	if (!no_daemonize)
		daemonize();
	initTasks();
	eventLoop();		/* this returns when there are no timers or listeners */
	cpeLog(LOG_DEBUG, "eventLoop() exited");
	return 0;
}

#else

int cwmpMain(void)
{
	initGSLib();		/* initialize the gslib timer and event handlers */
	cpeInitLog( 0 );
	initTasks();
	eventLoop();		/* this returns when there are no timers or listeners */
	return 0;
}


#endif
