/*----------------------------------------------------------------------*
 * Gatespace
 * Copyright 2006 Gatespace. All Rights Reserved.
 *----------------------------------------------------------------------*
 * File Name  : testParser.c
 * Description:	test the parser with the xmlTables for RPC.
 * 		Parses a file and displays the contents of the RPCRequest
 * 		structure.
 *             .
 *----------------------------------------------------------------------*
 * $Revision: 1.2 $
 * $Id:
 *----------------------------------------------------------------------*/


#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <sys/types.h>
#include <unistd.h>


#include <time.h>
#include <string.h>
#include <syslog.h>

#include "../src/xmlParser.h"
#include "../src/xmlParserSM.h"
#include "testrpc.h"
#include "testxmlTables.h"

#ifdef DMALLOC
#include "dmalloc.h"
#endif
#ifdef DEBUG
#define DBGPRINT(X) fprintf X
#else
#define DBGPRINT(X)
#endif
char	*memBuf;
RPCRequest	*rpc;
ParseHow	parseHow;

static const char    *getRPCMethodName(eRPCMethods m)
{
        const char    *t;
        switch (m) {
        case eGetRPCMethods:
                t="GetRPCMethods";
                break;
        case eGetParameterNames:
                t="GetParamterNames";
                break;
        case eGetParameterValues:
                t="GetParameterValues";
                break;
        case eSetParameterValues:
                t = "SetParameterValues";
                break;
        case eGetParameterAttributes:
                t="GetParameterAttributes";
                break;
        case eSetParameterAttributes:
                t="SetParameterAttributes";
                break;
        case eAddObject:
                t="AddObject";
                break;
        case eDeleteObject:
                t="DeleteObject";
                break;
        case eReboot:
                t="Reboot";
                break;
        case eDownload:
                t="Download";
                break;
        case eInformResponse:
                t="InformResponse";
                break;
        case eFactoryReset:
                t="FactoryReset";
                break;
        default:
                t="no RPC method";
                break;
        }
        return t;
}


void decodeRPCRequest(RPCRequest *r)
{
	fprintf(stdout, "ID = %s\n", r->ID? r->ID: "<unspecified>");
	fprintf(stdout, "holdRequests = %d\n", r->holdRequests);
	fprintf(stdout, "maxEnvelopes = %d\n", r->maxEnvelopes);
	fprintf(stdout, "commandKey = %s\n", r->commandKey);
	fprintf(stdout, "parmeterKey = %s\n", r->parameterKey);
	fprintf(stdout, "RPC Method = %s\n", getRPCMethodName(r->rpcMethod));

	switch (r->rpcMethod) {
	case eGetParameterNames:
			fprintf(stdout, "ParameterPathName=%s\n", r->ud.getPNamesReq.pNamePath);
			fprintf(stdout, "NextLevel=%d\n", r->ud.getPNamesReq.nextLevel);
			break;
	case eGetParameterValues:
	case eGetParameterAttributes:
		{
			ParameterNameStruct	*p = r->ud.getPAVReq.pnameList;
			fprintf(stdout, "arrayTypeSize = %d pnames=%d\n", r->arrayTypeSize, r->ud.getPAVReq.paramCnt);
			while ( p ) {
				fprintf(stdout, "ParameterPathName=%s\n", p->pName);
				p = p->next;
			}
		break;
		}
	case eSetParameterValues:
		{
			ParameterValueStruct	*p = r->ud.setPValuesReq.pvList;
			fprintf(stdout, "arrayTypeSize = %d pnames=%d\n", r->arrayTypeSize, r->ud.setPValuesReq.paramCnt);
			while ( p ) {
				fprintf(stdout, "ParameterPathName=%s value=%s\n",p->pName, p->value);
				p = p->next;
			}
			break;
		}
	case eSetParameterAttributes:
		{
			ParameterAttributesStruct	*p = r->ud.setPAttrReq.paList;
			fprintf(stdout, "arrayTypeSize = %d pnames=%d\n", r->arrayTypeSize, r->ud.setPAttrReq.paramCnt);
			while ( p ) {
				fprintf(stdout,
				 "ParameterName=%s\nnotifyChange=%d notify=%d accessChange=%d accessList=%d\n",p->pName, p->notifyChange,
						p->notification, p->accessListChange, p->accessList);
				p = p->next;
			}
			break;
		}
	case eAddObject:
	case eDeleteObject:
		fprintf(stdout, "ObjectPathName=%s\n", r->ud.addDelObjectReq.objectPath);
		break;
	case eDownload:
		fprintf(stdout, "FileType = %s, URL=%s, UserName=%s password=%s\n",
				r->ud.downloadReq.fileType, r->ud.downloadReq.URL, r->ud.downloadReq.userName, r->ud.downloadReq.passWord);
		fprintf(stdout, "FileSize =%d, targetFile=%s, delaySeconds=%d\n",
				r->ud.downloadReq.fileSize, r->ud.downloadReq.targetFileName, r->ud.downloadReq.delaySeconds);
		fprintf(stdout, "SuccessURL=%s FailureURL=%s\n",
				r->ud.downloadReq.successURL, r->ud.downloadReq.failureURL);
			break;
	case eInformResponse:
			break;
	case eFactoryReset:
			break;
	default:
			break;
	}
}

int main(int argc, char** argv)
{
	int verbose = 0;
	int	memory = 0;
	char    c;
	FILE    *fin;
	struct	stat fstat;
	XML_STATUS xs;

	while ((c=getopt(argc, argv, "vhm")) != -1) {
		switch (c) {
		case 'v':
			verbose = 1;
			break;
		case 'h':
			printf("Use: testParser [-v] [-m] <soapinput.xml>\n");
			printf("  -m: specifies a parse from memory, otherwise, the parser reads the file\n");
			exit(0);
			break;
		case 'm':
			memory = 1;
		default:
			break;
		}
	}
	if ( lstat(argv[optind],&fstat) == -1) {
		fprintf(stderr, "Unable to stat %s\n", argv[optind]);
		exit(-1);
	} else {
		parseHow.nameSpace = nameSpaces;	    /* name spaces supported */
		parseHow.topLevel = cwmpEnvelopeDesc;		/* where to start */
		/**if (memory) { **/
			if ( (memBuf = (char *)malloc(fstat.st_size))) {
				int	fd;
				if ( (fd=open(argv[optind],O_RDONLY))!=-1) {
					read(fd, memBuf, fstat.st_size);
					if ( (xs=xmlParseGeneric(&rpc,NULL, memBuf, fstat.st_size, &parseHow))==XML_STS_OK){
						cwmpDecodeRPCRequest( rpc );
						//cwmpFreeRPCRequest(rpc);
					}
					else
						printf("xmlParseGeneric memory parse retured an error: %d\n", xs);
					close(fd);
				}
			}
		} /***else {
			if ( xmlParseGeneric(&rpc,argv[optind], NULL,0, &parseHow)==NO_ERROR){
				decodeRPCRequest( rpc );
				//cwmpFreeRPCRequest(rpc);
			}
			else
				printf("xmlParseGeneric file parse retured an error\n");

		}
		***/
	return 0;
}









