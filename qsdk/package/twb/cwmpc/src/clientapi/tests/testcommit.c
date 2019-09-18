/*
 * testcommit.c
 *
 *
 *  Created on: Jun 23, 2009
 *      Author: dmounday
 *  Test cpeCommit
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

#define DEBUG

#include "clientapilib.h"

void usage(void) {
	printf("Use: testcommit <host> <object-path> \n");
}

int main(int argc, char *argv[]) {

	char *hostname;
	char *pname;

	int	 stat;

	if (argc >= 3 ) {
		hostname = argv[1];
		pname = argv[2];
	} else {
			usage();
			exit(-1);
	}

	cpeLibInit( hostname, CPEAPI_PORT );
	cpeStartSession( 0);
	stat = cpeCommit(pname );
	if ( stat == 0 ) {
		fprintf(stdout, "%s=(%d)\n", pname, stat);
	} else
		fprintf(stdout, "APISTATUS return %d\n", stat);
	cpeEndSession();
	exit(0);
}




