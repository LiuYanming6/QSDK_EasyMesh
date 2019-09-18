/*
 * testgpv.c
 *
 *  Created on: Jun 23, 2009
 *      Author: dmounday
 *  Test cpeSetParameterValue
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
	printf("Use: testspv <host> <parametername> <value>\n");
}

int main(int argc, char *argv[]) {

	char *hostname;
	char *pname;
	char *pvalue;
	int	 stat;

	if (argc >= 4 ) {
		hostname = argv[1];
		pname = argv[2];
		pvalue = argv[3];
	} else {
			usage();
			exit(-1);
	}

	cpeLibInit( hostname, CPEAPI_PORT );
	cpeStartSession( 0);
	stat = cpeSetParameterValue(pname, pvalue );
	if ( stat == 0 )
		fprintf(stdout, "%s=(%d) %s\n", pname, stat, pvalue);
	else
		fprintf(stdout, "APISTATUS return %d\n", stat);
	cpeEndSession();
	exit(0);
}


