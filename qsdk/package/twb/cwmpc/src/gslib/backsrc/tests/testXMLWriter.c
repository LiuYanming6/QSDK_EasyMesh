
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
#include <time.h>
#include <string.h>
#include <syslog.h>

#include "../src/xmlWriter.h"


int main(int argc, char** argv)
{
	int verbose = 0;
	int	memory = 0;
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

	fprintf(stderr, "xmlMemPrintf = %d\n", xmlMemPrintf(xp, "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"));
	fprintf(stderr, "openTag = %d\n", xmlOpenTagGrp(xp, "%s:envelope", "SOAP"));
	fprintf(stderr, "openTag = %d\n", xmlOpenTagGrp(xp, "%s:Body", "SOAP"));
	fprintf(stderr, "memprint = %d\n", xmlMemPrintf(xp, "<AddObject name=\"%s\">\n", "object"));
	fprintf(stderr, "memprint = %d\n", xmlMemPrintf(xp, "</AddObject>\n"));
	fprintf(stderr, "xmlPrintTaggedData = %d\n",
			xmlPrintTaggedData(xp, "datatag", "%s %d", "ATTR1", 101));
	fprintf(stderr, "xmlPrintTaggedData = %d\n",
			xmlPrintTaggedAttrib(xp, "attrtag", "attr1=\"%s\" strattr=\"%s\" attr2=\"%d\"",
							 "ATTR1", "string", 101));
	fprintf(stderr, "CloseTag = %d\n", xmlCloseTagGrp(xp));	/* Body*/

	xmlStartTagGrp(xp, "device");
	xmlMemPrintf(xp, " id=\"%s\"", "xx:xx:xx:xx:xx" );
	xmlMemPrintf(xp, " name=\"%s\"", "client01" );
	xmlMemPrintf(xp, " defuser=\"%s\"", "testusernaem" );
	xmlMemPrintf(xp, " ip=\"%s\"", "192.168.10.105" );
	xmlMemPrintf(xp, " upnp=\"%s\"", "true" );
	xmlMemPrintf(xp, " firstuse=\"%d\"", 0);
	xmlEndTagGrp(xp); /* device */

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

	xmlEndTagGrp(xp);	/* parctl */
	xmlEndTagGrp(xp);  /* user */
	fprintf(stderr, "CloseTag = %d\n", xmlCloseTagGrp(xp));

	fprintf(stdout, "%s\n", xmlGetBufPtr(xp));
	xmlCloseWriter(xp);
	return 0;
}









