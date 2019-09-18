/*
 * testadd.c
 *
 *  Created on: Jun 23, 2009
 *      Author: dmounday
 *  Test cpeAddObject
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
	printf("Use: testadd <host> <object-path-name> \n");
}

int main(int argc, char *argv[]) {

	char *hostname;
	char *pname;

	char *newInstance;
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
	stat = cpeAddObject(pname, &newInstance );
	if ( stat == 0 ) {
		fprintf(stdout, "%s=(%d) %s\n", pname, stat, newInstance);
		free( newInstance);
	} else
		fprintf(stdout, "APISTATUS return %d reading %s\n", stat, pname);
	cpeEndSession();
	exit(0);
}



