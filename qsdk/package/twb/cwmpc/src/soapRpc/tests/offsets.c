/*
 * offsets.c
 *
 *  Created on: Jan 6, 2012
 *      Author: dmounday
 */
#include <stdio.h>
#include <stddef.h>

#include "../../includes/paramTree.h"
#define DEBUG
#ifdef DEBUG
#define DBGPRINT(X) fprintf X
#else
#define DBGPRINT(X)
#endif

int main( int argc, char *argv[] ) {

	CWMPParam	param;
	CWMPObject	obj;

	fprintf(stdout, "CWMPObject.name\t\t%d\n", offsetof(CWMPObject,name));
	fprintf(stdout, "CWMPObject.pDelObj\t\t%d\n", offsetof(CWMPObject,pDelObj));
	fprintf(stdout, "CWMPObject.pAddObj\t\t%d\n", offsetof(CWMPObject,pAddObj));
	fprintf(stdout, "CWMPObject.pCommitObj\t\t%d\n", offsetof(CWMPObject,pCommitObj));
	fprintf(stdout, "CWMPObject.objList\t\t%d\n", offsetof(CWMPObject,objList));
	fprintf(stdout, "CWMPObject.paramList\t\t%d\n", offsetof(CWMPObject,paramList));
	fprintf(stdout, "CWMPObject.iList\t\t%d\n", offsetof(CWMPObject,iList));
	fprintf(stdout, "CWMPObject size\t\t%d\n", sizeof( struct CWMPObject));
	fprintf(stdout, "\n\n");
	fprintf(stdout, "CWMPParam.name\t\t%d\n", offsetof(CWMPParam,name));
	fprintf(stdout, "CWMPParam.pGetter\t\t%d\n", offsetof(CWMPParam,pGetter));
	fprintf(stdout, "CWMPParam.pSetter\t\t%d\n", offsetof(CWMPParam,pSetter));
	fprintf(stdout, "CWMPParam.instanceData\t\t%d\n", offsetof(CWMPParam,instanceData));

	fprintf(stdout, "CWMPParam size\t\t%d\n", sizeof( struct CWMPParam));
	return 0;
}

