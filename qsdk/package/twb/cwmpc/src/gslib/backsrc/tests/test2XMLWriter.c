
/*----------------------------------------------------------------------*
 * Gatespace
 * Copyright 2006 Gatespace. All Rights Reserved.
 *----------------------------------------------------------------------*
 * File Name  : testXMLWriter.h
 * Description:	test XML Write utility functions.
 *----------------------------------------------------------------------*
 * $Revision: 1.1 $
 * $Id:
 *----------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

#include <string.h>
#include <syslog.h>
#include <string.h>
#include "../src/xmlWriter.h"


int main(int argc, char** argv)
{
	int verbose = 0;
	char    c;
	XMLWriter	*xp;
	size_t  rallocSz=1024;
	int	flags = 0;

	while ((c=getopt(argc, argv, "vms:")) != -1) {
		switch (c) {
		case 'v':
			verbose = 1;
			break;
		case 'h':
			printf("Use: testXMLWriter [-v]\n");
			printf("  Tests the xmlWriter functions\n");
			printf("  -s <n> is size of reallocation\n");
			printf("  -f <flag> is xmlOpenWriter flags\n");
			exit(0);
			break;
		case 's':
			rallocSz = atoi(optarg);
			break;
		case 'f':
			flags = strtol(optarg,NULL, 16);
			break;
		default:
			break;
		}
	}
	xp = xmlOpenWriter( rallocSz, flags);
	if (xp==NULL) {
		fprintf(stderr, "xmlOpenWriter returns NULL\n");
	}

	xmlStartTagGrp(xp, "document");
	xmlStartTagGrp(xp, "user name=\"test\"");
	xmlStartTagGrp(xp, "parctl");
	xmlMemPrintf(xp, "contentActive=\"%s\" blackActive=\"%s\"",
            "true", "false");
	xmlMemPrintf(xp, " whiteActive=\"%s\" sizeActive=\"%s\" sizePeriod=\"%d\"",
            "true", "false", 1000);
	xmlMemPrintf(xp, " sizeLimit=\"%d\" timeActive=\"%s\" timePeriod=\"%d\"",
            1000, "yes", 2000);
	xmlPrintTaggedAttrib(xp, "timespec", "start=\"%d\" end=\"%d\"",
				3000, 4000);
	//xmlPrintTaggedAttrib(xp, "timespec", "start=\"%d\" end=\"%d\"",
	//			3000, 4000);

	xmlEndTagGrp(xp);	/* parctl */
	xmlEndTagGrp(xp);  /* user */

	xmlStartTagGrp(xp, "user name=\"test2\"");
	xmlStartTagGrp(xp, "parctl");
	xmlMemPrintf(xp, "contentActive=\"%s\" blackActive=\"%s\"",
            "true", "false");
	xmlMemPrintf(xp, " whiteActive=\"%s\" sizeActive=\"%s\" sizePeriod=\"%d\"",
            "true", "false", 1000);
	xmlMemPrintf(xp, " sizeLimit=\"%d\" timeActive=\"%s\" timePeriod=\"%d\"",
            1000, "yes", 2000);

	xmlEndTagGrp(xp);	/* parctl */
	xmlEndTagGrp(xp);  /* user */
	xmlCloseTagGrp(xp);
	//fprintf(stdout, "<!-- xml document =====?>\n" );
	fprintf(stdout, "%s",  xmlGetBufPtr(xp));
	//fprintf(stdout, "\n<!-- document end==== -->\n");
	xmlCloseWriter(xp);
	return 0;
}









