/*----------------------------------------------------------------------*
 * Gatespace
 * Copyright 2006-2011 Gatespace Networks, Inc., All Rights Reserved.
 * Gatespace Networks, Inc. confidential material.
 *----------------------------------------------------------------------*
 * File Name  : rpcMethods.c
 * Description:	RPC Method implementation
 *----------------------------------------------------------------------*
 * $Revision: 1.46 $
 *
 * $Id: rpcMethods.c,v 1.46 2012/06/13 15:46:06 dmounday Exp $
 *----------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include <sys/select.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <ctype.h>

#ifdef DMALLOC
#include "dmalloc.h"
#endif
#include "../includes/sys.h"
#include "../includes/paramTree.h"
#include "../includes/rpc.h"
#include "../includes/CPEWrapper.h"
#include "../gslib/src/utils.h"
#include "../gslib/src/event.h"
#include "../gslib/src/xmlWriter.h"
#include "../gslib/src/xmlParser.h"
#include "../soapRpc/rpcUtils.h"
#include "../soapRpc/xmlTables.h"
#include "../gslib/src/event.h"
#include "../gslib/src/protocol.h"
#include "../gslib/src/wget.h"
#include "../soapRpc/rpcMethods.h"
#include "../soapRpc/cwmpSession.h"
#include "../soapRpc/notify.h"
#include "targetsys.h"

//#define DEBUG
#ifdef DEBUG
#define DBGPRINT(X) fprintf X
#else
#define DBGPRINT(X)
#endif

extern CPEState cpeState;
extern CWMPObject CWMP_RootObject[];
extern char wpPathName[];

typedef enum {
	eTest, eSet, eStop
} eListPass;

/*
 * DeviceInfo parameters used in Inform DeviceIdStruct.
 *
 */
static const char *informDeviceIDParams[] = {
		"Manufacturer",
		"ManufacturerOUI",
		"ModelName",
		"ProductClass",
		"SerialNumber",
		NULL };
#define MANUFACTUREROUI 1		/* index of ManufacturerOUI in informDeviceIDParams */

static char *interfaceName;
/*
 * RPCMethods implmented by this build.
 * Also initialize any callback functions that are required by the RPC implementation.
 */
static char implementedRPC[eFault + 1];

void initRPCMethods(void) {
	/* clear all rpc methods */
	memset(implementedRPC, 0, sizeof(implementedRPC));
	/* set required rpc methods */
	implementedRPC[eGetRPCMethods] = 1;
	implementedRPC[eGetParameterNames] = 1;
	implementedRPC[eGetParameterValues] = 1;
	implementedRPC[eSetParameterValues] = 1;
	implementedRPC[eGetParameterAttributes] = 1;
	implementedRPC[eSetParameterAttributes] = 1;
	implementedRPC[eAddObject] = 1;
	implementedRPC[eDeleteObject] = 1;
	implementedRPC[eReboot] = 1;
#ifdef CONFIG_RPCDOWNLOAD
	implementedRPC[eDownload] = 1;
#endif
#ifdef CONFIG_RPCSCHEDULEDOWNLOAD
	extern ACSSession cwmpXfrSession;
	implementedRPC[eScheduleDownload] = 1;
	setCallback(&cwmpXfrSession, cwmpStartWindow, NULL);
#endif
#if defined(CONFIG_RPCDOWNLOAD) || defined(CONFIG_RPCUPLOAD)
#ifdef CONFIG_RPCCANCELTRANSFER
	implementedRPC[eCancelTransfer] = 1;
	setCallback(&cpeState.dlQ, cwmpCheckStartTransfer, NULL);
#endif
#endif
	/* set optional rpc methods from sys.h */
#ifdef CONFIG_RPCUPLOAD
	implementedRPC[eUpload] = 1;
#endif
#ifdef CONFIG_RPCFACTORYRESET
	implementedRPC[eFactoryReset] = 1;
#endif
#ifdef CONFIG_RPCGETQUEUEDTRANSFERS
	implementedRPC[eGetQueuedTransfers] = 1;
#endif
#ifdef CONFIG_RPCGETALLQUEUEDTRANSFERS
	implementedRPC[eGetAllQueuedTransfers] = 1;
#endif
#ifdef CONFIG_RPCSCHEDULEINFORM
	implementedRPC[eScheduleInform] = 1;
#endif

#ifdef CONFIG_RPCSETVOUCHERS
	implementedRPC[eSetVouchers ] = 1;
	implementedRPC[eGetOptions ] = 1;
#endif
#ifdef CONFIG_RPCCHANGEDUSTATE
	implementedRPC[eChangeDUState] = 1;
#endif
}

static int isRPC(eRPCMethods method) {
	return implementedRPC[method] != 0;
}
/*
 */
static void openEnvHdrBody(XMLWriter *xp, char *idstr) {
	NameSpace *ns;
	xmlMemPrintf(xp, "<%sEnvelope", nsSOAP);
	/* generate Namespace declarations */
	ns = nameSpaces;
	while (ns->sndPrefix) {
		char pbuf[256];
		char *e;
		strncpy(pbuf, ns->sndPrefix, sizeof(pbuf));
		e = strchr(pbuf, ':');
		if (e)
			*e = '\0'; /* find : in prefix */
		xmlMemPrintf(xp, "\n xmlns:%s=\"%s\"", pbuf, ns->nsURL);
		++ns;
	}
	xmlMemPrintf(xp, ">\n");
	if (idstr) {
		xmlOpenTagGrp(xp, "%sHeader", nsSOAP);
		xmlMemPrintf(xp, "<%sID %smustUnderstand=\"1\">%s</%sID>\n", nsCWMP,
				nsSOAP, idstr, nsCWMP);
		xmlCloseTagGrp(xp);
	}
	xmlOpenTagGrp(xp, "%sBody", nsSOAP);
}
static void closeEnvBody(XMLWriter *xp) {
	xmlCloseTagGrp(xp); /* </Body> */
	xmlMemPrintf(xp, "</%sEnvelope>\n", nsSOAP);
}

/* return SOAP type in a string */
static const char *getSOAPTypeStr(eCWMPType etype) {
	const char *s;
	switch (etype) {
	case eString:
	case eStringSetOnly:
		s = "string";
		break;
	case eInt:
		s = "int";
		break;
	case eUnsignedInt:
		s = "unsignedInt";
		break;
	case eBoolean:
		s = "boolean";
		break;
	case eDateTime:
		s = "dateTime";
		break;
	case eBase64:
		s = "base64";
		break;
	case eLong:
		s = "long";
		break;
	case eUnsignedLong:
		s = "unsignedLong";
		break;
	case eHexBinary:
		s = "hexBinary";
		break;
	case eAny:
		s = "any";
		break;
	default:
		s = "";
		break;
	}
	return s;
}

static void writeSOAPFault(XMLWriter *xp, RPCRequest *r, char *idStr, int fault) {

	openEnvHdrBody(xp, idStr);
	xmlOpenTagGrp(xp, "%sFault", nsSOAP);
	xmlMemPrintf(xp, "<faultcode>%s</faultcode>\n",
			cpeGetFaultType(fault) == eServer ? "Server" : "Client"); /* always Client???*/
	xmlMemPrintf(xp, "<faultstring>CWMP fault</faultstring>\n");
	xmlOpenTagGrp(xp, "detail");
	xmlOpenTagGrp(xp, "%sFault", nsCWMP);
	xmlMemPrintf(xp, "<FaultCode>%d</FaultCode>\n", fault);
	xmlMemPrintf(xp, "<FaultString>%s</FaultString>\n",
			cpeGetFaultMessage(fault));
	if (r && r->rpcMethod == eSetParameterValues) {
		ParameterValueStruct *p = r->ud.setPValuesReq.pvList;
		/* stop thru parameters find faults */
		while (p != NULL) {
			if (p->fault) {
				xmlOpenTagGrp(xp, "SetParameterValuesFault");
				xmlMemPrintf(xp, "<ParameterName>%s</ParameterName>\n",
						p->pName);
				xmlMemPrintf(xp, "<FaultCode>%d</FaultCode>\n", p->fault);
				xmlMemPrintf(xp, "<FaultString>%s</FaultString>\n",
						cpeGetFaultMessage(p->fault));
				xmlCloseTagGrp(xp); /* /SetParameterValuesFault */
			}
			p = p->next;
		}
	}
	xmlCloseTagGrp(xp); /* </Fault> */
	xmlCloseTagGrp(xp); /* </detail> */
	xmlCloseTagGrp(xp); /* </Fault>  */
	closeEnvBody(xp); /* </Body> </envelope> */
}

/*
 *  Write ParameterAttributeStruct with XMLWriter.
 * *p - pointer to parameter structure.
 * *n - pointer to parameter path name.
 * *ip - pointer to current Instance(from framework function)
 * *xp - xmlWriter
 * *paramCnt - pointer to return location for parameter count.
 * *unused - unused int in this function
 *
 *     NOTification value is forced to 2 if FORCED_ACTIVE is set. Otherwise;
 *     it is the value of ACTIVE OR PASSIVE notify.
 */
static int getWriteParamAttr(CWMPObject *o, CWMPParam *p, Instance *ip,
		void *vxp, void *paramCnt, int unused) {
	IData *idp;
	XMLWriter *xp = vxp;
	if (p && (idp = cwmpGetIData(p, ip))) {
		/* now fill in ParameterAttributeStruct in response */
		xmlOpenTagGrp(xp, "ParameterAttributeStruct");
		xmlMemPrintf(xp, "<Name>%s</Name>\n", wpPathName);
		xmlMemPrintf(
				xp,
				"<Notification>%d</Notification>\n",
				idp->notify & FORCED_ACTIVE ? ACTIVE_NOTIFY : idp->notify
						& (PASSIVE_NOTIFY | ACTIVE_NOTIFY));
		xmlOpenTagGrp(xp, "AccessList %sarrayType=\"%sstring[%d]\"",
				nsSOAP_ENC, nsXSD,
				idp->accessClients & SUBWRITE_INHIBIT ? 0 : 1);
		if (!(idp->accessClients & SUBWRITE_INHIBIT))
			xmlMemPrintf(xp, "<string>%s</string>\n", "Subscriber");
		xmlCloseTagGrp(xp); /* /AccessList*/
		xmlCloseTagGrp(xp); /* /ParameterAttributeStruct */
		if (paramCnt)
			(*(int*) paramCnt)++;
	}
	return eOK;
}
/*
 *  Write ParameterValueStruct with XMLWriter.
 * *p - pointer to parameter structure.
 * *n - pointer to parameter path name.
 * *value - pointer to value of parameter (null terminated string )
 * *xp - xmlWriter
 * *pcount - pointer to return location for parameter count.
 *
 */
void writeParamValues(eCWMPType pType, char *n, const char *value,
		XMLWriter *xp) {
	/* now fill in ParameterValueStruct in response */
	xmlOpenTagGrp(xp, "ParameterValueStruct");
	xmlMemPrintf(xp, "<Name>%s</Name>\n", n ? n : "");
	xmlMemPrintf(xp, "<Value %stype=\"%s%s\">", nsXSI, nsXSD,
			getSOAPTypeStr(pType));
	if (pType == eStringSetOnly)
		xmlPrintf(xp, "");
	else {
		if ((value == NULL || *value == '\0') && pType == eBoolean)
			xmlPrintf(xp, "0"); // some ACS dislike empty boolean values -- force 0
		else
			xmlPrintf(xp, value ? value : "");
	}
	xmlMemPrintf(xp, "</Value>\n");
	//DBGPRINT((stderr, "%s=%s %s\n", wpPathName, getSOAPTypeStr(pType), value==NULL?*value:"NULL"));
	xmlCloseTagGrp(xp);
	return;
}

static CPE_STATUS writePStatus;

static int getWriteParamValue(CWMPObject *o, CWMPParam *p, Instance *ip,
		void *wxp, void *paramCnt, int unused) {
	char *val = NULL;
	XMLWriter *xp = (XMLWriter *) wxp;
	if (p && p->pGetter && (writePStatus = p->pGetter(ip, &val)) == CPE_OK) {
		writeParamValues(p->pType, wpPathName, val, xp);
		if (paramCnt)
			(*(int*) paramCnt)++;
		if (val)
			GS_FREE(val);
	} else {
		return eStopWalk; /* 9002 Internal fault here */
	}

	return eOK;
}

/*
 * returns !=0 if the path ends in a instance sequence.
 * I.E.  LANDevice.1. or LANDevice.[landev].
 *   (last two chars are a digit followed by a .
 * or last two chars are '].'.
 */
static int isInstance( char *path ) {
	int	j = strlen(path);
	if (path[j-1]=='.') {
		/* last char is . */
		int i = j-2; // check char befor '.'
		return isdigit(path[i]) || (path[i]==']');
	}
	return 0;
}


static int isFragment(char *path) {
	int j = strlen(path);
	return (path[j - 1] == '.');
}

static void plugArrayCount(XMLWriter *xp, int pCnt) {
	char buf[5];
	int s, i;
	char *p = xmlGetBufPtr(xp);
	char *t;
	/* buffer may not be present in XMLWriter */
	if (p) {
		t = strstr(p, "!^^!]");
		if (t) {
			snprintf(buf, sizeof(buf), "%d", pCnt);
			s = strlen(buf);
			for (i = 0; i < 4; ++i) {
				if (i < s)
					*(t + i) = buf[i];
				else
					*(t + i) = ' ';
			}
		}
	}
}
/*
 * return the number of events added.
 */
static int addEvent(eEventCode eEvent, char *ckey, XMLWriter *xp) {
	xmlOpenTagGrp(xp, "EventStruct");
	xmlMemPrintf(xp, "<EventCode>%s</EventCode>\n", cwmpGetEventName(eEvent));
	xmlMemPrintf(xp, "<CommandKey>%s</CommandKey>\n", ckey ? ckey : "");
	xmlCloseTagGrp(xp); /* </EventStruct> */
	return 1;
}
/*
 * Add any Upload or Download transfer events on the Active queue that have completed.
 * Do not count AntonomousTransferComplete events here.
 * Return number of events added.
 */
static int addXferEvents(XMLWriter *xp) {
	RPCRequest *r = cpeState.dlQActive;
	int i = 0;
	while (r) {
		if (r->ud.downloadReq.dlStatus != 2 && r->ud.downloadReq.dlStatus != -1) {
			/* completed transfer found, it may be an AutonomousTransferComplete */
			if (r->rpcMethod == eDownload ) {
				if ( cpeState.sigusr1 ==1 )
				{
				    addEvent(eEvtMDownload, r->commandKey, xp);	
				    ++i;
				}
				else
				    break;
			} else if (r->rpcMethod == eScheduleDownload) {
				addEvent(eEvtMScheduleDownload, r->commandKey, xp);
				++i;
			} else if (r->rpcMethod == eUpload) {
				addEvent(eEvtMUpload, r->commandKey, xp);
				++i;
			}
		}
		r = r->next;
	}
	return i;
}

#ifdef CONFIG_RPCCHANGEDUSTATE
#include "smm.h"
extern CPEChangeDUState *cwmpDUStateChangeDoneQ;

static int addDUChangeEvents(XMLWriter *xp) {
	CPEChangeDUState *du = cwmpDUStateChangeDoneQ;
	int i = 0;
	while (du) {
		addEvent(eEvtMChangeDUState, du->commandKey, xp);
		++i;
		du = du->next;
	}
	return i;
}
#endif

static void addEventStruct(XMLWriter *xp) {
	eEventCode eEvent;
	int eCnt = 0;
	xmlOpenTagGrp(xp, "Event %sarrayType=\"%sEventStruct[!^^!]\"", nsSOAP_ENC,
			nsCWMP);
	for (eEvent = eEvtBootstrap; eEvent < eEvtArraySz; ++eEvent) {
		if (cwmpGetEventState(eEvent) != eOff) {
			if (eEvent == eEvtMReboot)
				eCnt += addEvent(eEvent, cpeState.rebootCommandKey, xp);
			else if (eEvent == eEvtMDownload || eEvent == eEvtMUpload || eEvent
					== eEvtMScheduleDownload)
				continue; /* transfer events added by addXferEvents() */
			else if (eEvent == eEvtMChangeDUState)
				continue; /* added by addDUChangeEvents() */
#ifdef CONFIG_RPCSCHEDULEINFORM
			else if (eEvent == eEvtMScheduleInform)
				eCnt += addEvent(eEvent, cpeState.scheduleInformCommandKey, xp);
#endif
			else
				eCnt += addEvent(eEvent, NULL, xp);
		}
	}
	eCnt += addXferEvents(xp);
#ifdef CONFIG_RPCCHANGEDUSTATE
	eCnt += addDUChangeEvents(xp);
#endif
	xmlCloseTagGrp(xp); /* </Event> */
	plugArrayCount(xp, eCnt);
}
/*
 * build the Inform RPC message
 * The defaultIP parameter is IP address of the interface used to connect to
 * the ACS. This is required to construct the objectname/parameter path for the
 * interface. This parameter is only required for IGD data model CPE devices.
 *
 * Return: NULL - failed,
 *      otherwise, pointer to buffer containing the RPC Method.
 *	   (body of http message to send to ACS).
 */
char *cwmpInformRPC(char *sessionID, int retryCnt, char *defaultIP) {
	int i;
	int paramCnt = 0;
	time_t curtime;
	char *buf;
	XMLWriter *xp = xmlOpenWriter(SOAP_SENDBUFSZ, XML_WRITER_FLG);
	XMLWriter *xParams = xmlOpenWriter(SOAP_SENDBUFSZ, XML_WRITER_FLG);

	if (xp && xParams) {

		openEnvHdrBody(xp, sessionID);
		xmlOpenTagGrp(xp, "%sInform", nsCWMP);
		/* create DeviceId structure */
		xmlOpenTagGrp(xp, "DeviceId");
		for (i = 0; informDeviceIDParams[i]; ++i) {
			char path[256];
			char *val = NULL;
			snprintf(path, sizeof(path), "%s.DeviceInfo.%s",
					CWMP_RootObject[0].name, informDeviceIDParams[i]);
			CWMPParam *p = cwmpFindParameter(path);
			if (p) {
				const char *np = p->name;
				if (i == MANUFACTUREROUI)
					np = "OUI";
				if (p->pGetter && p->pGetter(cwmpGetCurrentInstance(), &val)
						== CPE_OK) {
					xmlMemPrintf(xp, "<%s>%s</%s>\n", np, val ? val : "", np);
					GS_FREE(val);
				}
			}
		}
		xmlCloseTagGrp(xp); /*DeviceId */
		/* the following also returns the list of forced inform parameters */
		if (cwmpCheckNotification(xParams, &paramCnt, 0) > 0)
		{
			/* notification parameters have changed - add event */
			cwmpAddEvent(eEvtValueChange);
		}
		/* generate Event Struct */
		addEventStruct(xp);

		xmlMemPrintf(xp, "<MaxEnvelopes>1</MaxEnvelopes>\n");
		curtime = time(NULL);
		xmlMemPrintf(xp, "<CurrentTime>%s</CurrentTime>\n",
				getXSIdateTime(&curtime));
		xmlMemPrintf(xp, "<RetryCount>%d</RetryCount>\n", retryCnt);
		if (interfaceName != NULL)
			GS_FREE(interfaceName);
		/* This is now forced by data model flags in the parameter table entry. This is required for the TR-098 forced parameters - should fail on other data models*/
		//if ((interfaceName = findInterfaceNameWithIP(defaultIP))) {
		//	writeParamValues(eString, interfaceName, defaultIP, xParams);
		//	++paramCnt;
		//}
		xmlOpenTagGrp(xp,
				"ParameterList %sarrayType=\"%sParameterValueStruct[%d]\"",
				nsSOAP_ENC, nsCWMP, paramCnt);
		if (paramCnt > 0) {
			char *xpbuf = xmlGetBufPtr(xParams);
			if (xpbuf != NULL)
				xmlMemPrintf(xp, "%s", xmlGetBufPtr(xParams)); /* append parameter buffer to message buffer */
			else {
				/* no memory */
				xmlCloseWriter(xParams);
				xmlCloseWriter(xp);
				return NULL;
			}
		}
		xmlCloseWriter(xParams); /* release parameter xmlWriter */
		xmlCloseTagGrp(xp); /* ParameterList */
		xmlCloseTagGrp(xp); /* </Inform> */
		closeEnvBody(xp); /* </Body> </envelope> */
		buf = xmlSaveCloseWriter(xp);
	} else {
		xmlCloseWriter(xParams);
		xmlCloseWriter(xp);
		buf = NULL;
	}
	return buf;

}

int runGetRPCMethods(char *sessionID, RPCRequest *r, char **bp) {
	XMLWriter *xp = xmlOpenWriter(SOAP_SENDBUFSZ, XML_WRITER_FLG);
	eRPCMethods m;
	int num = 0;
	if (xp) {
		openEnvHdrBody(xp, sessionID);
		xmlOpenTagGrp(xp, "%sGetRPCMethodsResponse", nsCWMP);
		xmlOpenTagGrp(xp, "MethodList %sarrayType=\"%sstring[!^^!]\"",
				nsSOAP_ENC, nsXSD);

		for (m = eGetRPCMethods; m < eInformResponse; ++m) {
			if (isRPC(m)) {
				const char *s = cwmpGetRPCMethodNameString(m);
				if (s) {
					xmlMemPrintf(xp, "<string>%s</string>\n", s);
					num++;
				}
			}
		}
		plugArrayCount(xp, num);
		xmlCloseTagGrp(xp); /* /MethodList */
		xmlCloseTagGrp(xp); /* /GetRPCMethodsResponse */
		closeEnvBody(xp); /* </Body> </envelope> */
		*bp = xmlSaveCloseWriter(xp);
	} else
		*bp = NULL;
	return 0;
}

/*
 * An object has been found. Need to retrieve all the parameter
 * values for it.
 */
static int walkFragment(int descend, CWMPObject *o, Instance *ip,
		XMLWriter *xp, WTCallBack func) {
	int paramCnt = 0;
	cwmpWalkPTree(descend, o, ip, func, xp, &paramCnt, 0);
	return paramCnt;
}
/*
 * Build the GetParametersResponse
 * The goofiness with the ParameterValueStruct[!^^!] is to allow use of
 * only a single buffer to build the response. When the parameter count
 * is known the array value is substituted into the buffer. The !^^! is
 * a space holder and a target search pattern.
 *
 */
int runGetParameterValues(char *sessionID, RPCRequest *r, char **bp) {
	int fault = 0;
	Instance *ip;
	ParameterNameStruct *pn; /* pointer to current parameter name */
	int paramCnt = 0; /* count of parameters to return */
	XMLWriter *xp = xmlOpenWriter(SOAP_SENDBUFSZ, XML_WRITER_FLG);
	if (xp) {
		openEnvHdrBody(xp, sessionID);
		xmlOpenTagGrp(xp, "%sGetParameterValuesResponse", nsCWMP);
		xmlOpenTagGrp(xp,
				"ParameterList %sarrayType=\"%sParameterValueStruct[!^^!]\"",
				nsSOAP_ENC, nsCWMP);
		if ((pn = r->ud.getPAVReq.pnameList) == NULL)
			fault = 9003; /* at least one parameter name is required by schema*/
		cwmpSetRunState(RUNSTATE_GETPARAMETER);
		while (pn && !fault) {
			/* process list of names */
			CWMPObject *o;
			CWMPParam *p;
			int pCnt = 0;
			cwmpFrameworkReset();
			if (pn->pName == NULL || strlen(pn->pName) == 0) {
				/* name is empty- use root of parameter tree */
				o = CWMP_RootObject;
				pCnt = walkFragment(DESCEND | CBPARAMS, o, NULL, xp,
						getWriteParamValue);
				fault = writePStatus;
			} else if (isFragment(pn->pName)) { /* ends with . ? */
				if ((o = cwmpFindObject(pn->pName))) {
					if (isInstance(pn->pName) && !ISINSTANCE(o))
						fault = 9005;
					else {
						ip = isInstance(pn->pName) ? cwmpGetCurrentInstance()
								: NULL;
						pCnt = walkFragment(DESCEND | CBPARAMS, o, ip, xp,
								getWriteParamValue);
						fault = writePStatus;
					}
				} else
					fault = 9005;
			} else {/* must be a simple parameter */
				if ((p = cwmpFindParameter(pn->pName))) {
					getWriteParamValue(NULL, p, cwmpGetCurrentInstance(), xp,
							&pCnt, 0);
					fault = writePStatus;
				} else
					fault = 9005; /* 9005 param name not found */
			}
			pn = pn->next;
			paramCnt += pCnt;
		}
		if (!fault) {
			plugArrayCount(xp, paramCnt);
			xmlCloseTagGrp(xp); /* /ParameterList */
			xmlCloseTagGrp(xp); /* /GetParameterValuesResponse */
			closeEnvBody(xp); /* </Body> </envelope> */
			*bp = xmlSaveCloseWriter(xp);
			xp = NULL;
			if (*bp == NULL)
				fault = 9004;
		}
		if (fault) {
			/* discard current msg and build SOAP fault message response */
			/* if *bp is NULL then the xmlWriter failed - so try a fault msg */
			xmlCloseWriter(xp);
			if ((xp = xmlOpenWriter(SOAP_SENDBUFSZ, XML_WRITER_FLG))) {
				writeSOAPFault(xp, r, sessionID, fault);
				*bp = xmlSaveCloseWriter(xp);
			} else
				*bp = NULL;
		}
	} else {
		*bp = NULL;
		fault = 9004;
	}
	return fault;
}
/*
 * Normally called as a callback from the walk structure functions.
 * Used to write the ParameterInfoStruct with the Name and
 * attributes to the xml buffer. Increments paramCnt if the
 * pointer is not NULL.
 */

static int getWriteNames(CWMPObject *o, CWMPParam *p, Instance *ip, void *wxp,
		void *paramCnt, int unused) {
	int writeable;
	XMLWriter *xp = (XMLWriter *) wxp;
	xmlOpenTagGrp(xp, "ParameterInfoStruct");
	xmlMemPrintf(xp, "<Name>%s</Name>\n", wpPathName);
	if (!p)
		writeable = o->pAddObj != NULL && o->oType == eInstance; /* must be an object */
	else
		writeable = p->setable == RPC_RW;
	xmlMemPrintf(xp, "<Writable>%s</Writable>\n", writeable ? "1" : "0");
	xmlCloseTagGrp(xp);
	if (paramCnt)
		(*(int*) paramCnt)++;
	return eOK;
}

int runGetParameterNames(char *sessionID, RPCRequest *r, char **bp) {
	char *pPath = r->ud.getPNamesReq.pNamePath;
	CNTRL_MASK descend = r->ud.getPNamesReq.nextLevel != 1 ? DESCEND | CBPARAMS
			| CBOBJECTS : CBPARAMS | CBOBJECTS;
	int paramCnt = 0; /* count of parameters to return */
	int fault = 0;
	CWMPObject *o;
	CWMPParam *p;

	XMLWriter *xp = xmlOpenWriter(SOAP_SENDBUFSZ, XML_WRITER_FLG);

	if (xp) {
		openEnvHdrBody(xp, sessionID);
		xmlOpenTagGrp(xp, "%sGetParameterNamesResponse", nsCWMP);
		xmlOpenTagGrp(xp,
				"ParameterList %sarrayType=\"%sParameterInfoStruct[!^^!]\"",
				nsSOAP_ENC, nsCWMP);
		cwmpFrameworkReset();
		if (pPath == NULL || strlen(pPath) == 0) {
			/* name is empty- use root of parameter tree */
			o = CWMP_RootObject;
			strcpy(wpPathName, o->name);
			strcat(wpPathName, ".");
			getWriteNames(o, NULL, NULL, xp, &paramCnt, 0);
			/* only top level object if NextLevel=1*/
			if (descend & DESCEND) {
				cwmpWalkPTree(descend, o, NULL, getWriteNames, xp, &paramCnt, 0);
			}
		} else if (isFragment(pPath)) { /* ends with . ? */
			if ((o = cwmpFindObject(pPath))) {
				Instance *ip = isInstance(pPath) ? cwmpGetCurrentInstance()
						: NULL;
				if (descend & DESCEND) /* if DESCEND (nextLevel=0) include this object*/
					getWriteNames(o, NULL, ip, xp, &paramCnt, 0);
				cwmpWalkPTree(descend, o, ip, getWriteNames, xp, &paramCnt, 0);
			} else
				fault = 9005; /* parameter name not found */
		} else if (descend & DESCEND) {/* must be a simple parameter and NextLevel=0 */
			if ((p = cwmpFindParameter(pPath))) {
				getWriteNames(NULL, p, NULL, xp, &paramCnt, 0);
			} else
				fault = 9005; /* 9005 parameter name not found */
		} else
			fault = 9003;
		if (!fault) {
			plugArrayCount(xp, paramCnt);
			xmlCloseTagGrp(xp); /* /ParameterList */
			xmlCloseTagGrp(xp); /* /GetParameterValuesResponse */
			closeEnvBody(xp); /* </Body> </envelope> */
			*bp = xmlSaveCloseWriter(xp);
			xp = NULL;
			if (*bp == NULL)
				fault = 9004;
		}
		if (fault) {
			/* discard current msg and build SOAP fault message response */
			/* if *bp is NULL then the xmlWriter failed - so try a fault msg */
			xmlCloseWriter(xp);
			if ((xp = xmlOpenWriter(SOAP_SENDBUFSZ, XML_WRITER_FLG))) {
				writeSOAPFault(xp, r, sessionID, fault);
				*bp = xmlSaveCloseWriter(xp);
			} else
				*bp = NULL;
		}
	} else {
		*bp = NULL;
		fault = 9004;
	}
	return fault;
}

int runGetParameterAttributes(char *sessionID, RPCRequest *r, char **bp) {
	ParameterNameStruct *pn; /* pointer to current parameter name */
	int paramCnt = 0; /* count of parameters to return */
	int fault = 0;
	XMLWriter *xp = xmlOpenWriter(SOAP_SENDBUFSZ, XML_WRITER_FLG);
	if (xp) {
		openEnvHdrBody(xp, sessionID);
		xmlOpenTagGrp(xp, "%sGetParameterAttributesResponse", nsCWMP);
		xmlOpenTagGrp(
				xp,
				"ParameterList %sarrayType=\"%sParameterAttributeStruct[!^^!]\"",
				nsSOAP_ENC, nsCWMP);
		pn = r->ud.getPAVReq.pnameList;
		while (pn && !fault) {
			/* process list of names */
			CWMPObject *o;
			CWMPParam *p;
			int pCnt = 0;
			cwmpFrameworkReset();
			if (pn->pName == NULL || strlen(pn->pName) == 0) {
				/* name is empty- use root of parameter tree */
				o = CWMP_RootObject;
				pCnt = walkFragment(DESCEND | CBPARAMS, o, NULL, xp,
						getWriteParamAttr);
			} else if (isFragment(pn->pName)) { /* ends with . ? ;only fragments allows for obj names*/
				if ((o = cwmpFindObject(pn->pName))) {
					Instance *ip =
							isInstance(pn->pName) ? cwmpGetCurrentInstance()
									: NULL;
					pCnt = walkFragment(DESCEND | CBPARAMS, o, ip, xp,
							getWriteParamAttr);
				} else
					fault = 9005; /* 9005 param name not found */
			} else {/* must be a simple parameter */
				if ((p = cwmpFindParameter(pn->pName))) {
					o = cwmpGetCurrentObject();
					getWriteParamAttr(o, p, cwmpGetCurrentInstance(), xp,
							&pCnt, 0);
				} else
					fault = 9005; /* 9005 param name not found */
			}
			pn = pn->next;
			paramCnt += pCnt;
		}
		if (!fault) {
			plugArrayCount(xp, paramCnt);
			xmlCloseTagGrp(xp); /* /ParameterList */
			xmlCloseTagGrp(xp); /* /GetParameterValuesResponse */
			closeEnvBody(xp); /* </Body> </envelope> */
			*bp = xmlSaveCloseWriter(xp);
			xp = NULL;
			if (*bp == NULL)
				fault = 9004;
		}
		if (fault) {
			/* discard current msg and build SOAP fault message response */
			/* if *bp is NULL then the xmlWriter failed - so try a fault msg */
			xmlCloseWriter(xp);
			if ((xp = xmlOpenWriter(SOAP_SENDBUFSZ, XML_WRITER_FLG))) {
				writeSOAPFault(xp, r, sessionID, fault);
				*bp = xmlSaveCloseWriter(xp);
			} else
				*bp = NULL;
		}
	} else {
		*bp = NULL;
		fault = 9004;
	}
	return fault;
}

/*
 * runSetParameterAttributes
 * The only way this can fail is if a parameter path name is wrong.
 * This function makes 2 passes thru the ParameterList in the request.
 * The first pass verifies that the parameter name is ok and that
 * the notification attribute can be set if requested. Then
 * second pass applies the new attributes. If the first pass detects
 * a parameter name error the second pass is skipped and the response
 * error code is returned. The spec doesn't require a list of parameters
 * in error so we can stop on the first error.
 */

static int cbSetParamAttr(CWMPObject *o, CWMPParam *p, Instance *ip,
		void *pReq, void *pFault, int ePass) {
	ParameterAttributesStruct *pRA = (ParameterAttributesStruct *) pReq;
	IData *idp;
	int fault = 0;

	if ((idp = cwmpGetIData(p, ip))) {
		if (pRA->notifyChange) {
			if (pRA->notification < eNOTIFYOFF || pRA->notification
					> eNOTIFYACTIVE)
				fault = 9003; /* notification value out-of-range */
			else if (pRA->notification == eNOTIFYACTIVE && (p->notify
					& NOACTIVENOTIFY))
				fault = 9009; /* notification request rejected*/
			else if (ePass == eSet) {
				/* && !(p->notify & (FORCED_ACTIVE|FORCED_INFORM)) */
				/* use mask to avoid overwriting framework level notify flags */
				idp->notify = (idp->notify & FWNOTIFYMASK) | (pRA->notification
						& RPCNOTIFYMASK);
				if (idp->notify & NOTIFY_ON && idp->dataCopy == NULL) {
					/* Notify is on - need to setup data copy */
					char *val = NULL;
					if (p->pGetter && p->pGetter(ip, &val) == CPE_OK) {
						idp->dataCopy = val;
					}
				} else if ((!(idp->notify & NOTIFY_ON)) && idp->dataCopy
						!=NULL) { /* Notify is off */
					/* free any data copy that is no longer needed */GS_FREE(idp->dataCopy);
					idp->dataCopy = NULL;
				}
			}
		}
		if (ePass == eSet && pRA->accessListChange)
			idp->accessClients = pRA->accessList;
	} else
		fault = 9002; /* internal error */
	if (pFault)
		*(int *) pFault = fault;
	return fault ? eStopWalk : eOK;
}

int runSetParameterAttributes(char *sessionID, RPCRequest *r, char **bp) {
	eListPass ePass = eTest;
	int fault = 0;
	ParameterAttributesStruct *pa; /* pointer to current parameter name */
	XMLWriter *xp = xmlOpenWriter(SOAP_SENDBUFSZ, XML_WRITER_FLG);

	for (ePass = eTest; ePass < eStop; ++ePass) {
		pa = r->ud.setPAttrReq.paList;
		while (pa && ePass < eStop) {
			CWMPObject *o;
			CWMPParam *p;
			cwmpFrameworkReset();
			if (pa->pName == NULL || strlen(pa->pName) == 0) {
				/* name is empty- use root of parameter tree */
				o = CWMP_RootObject;
				cwmpWalkPTree(DESCEND | CBPARAMS, o, NULL, cbSetParamAttr, pa,
						&fault, ePass);
			} else if (isFragment(pa->pName)) { /* ends with . ? */
				if ((o = cwmpFindObject(pa->pName))) {
					Instance *ip =
							isInstance(pa->pName) ? cwmpGetCurrentInstance()
									: NULL;
					cwmpWalkPTree(DESCEND | CBPARAMS, o, ip, cbSetParamAttr,
							pa, &fault, ePass);
				} else
					fault = 9005;
			} else {/* must be a simple parameter */
				if ((p = cwmpFindParameter(pa->pName))) {
					o = cwmpGetCurrentObject();
					cbSetParamAttr(o, p, cwmpGetCurrentInstance(), pa, &fault,
							ePass);
				} else
					fault = 9005; /* 9005 param name not found */
			}
			pa = pa->next;
			if (fault)
				ePass = eStop;
		}
	}

	if (xp) {
		if (!fault) {
			openEnvHdrBody(xp, sessionID);
			xmlOpenTagGrp(xp, "%sSetParameterAttributesResponse", nsCWMP);
			xmlCloseTagGrp(xp); /* /SetParameterAttributesResponse */
			closeEnvBody(xp); /* </Body> </envelope> */
		} else
			writeSOAPFault(xp, r, sessionID, fault);
		*bp = xmlSaveCloseWriter(xp);
	} else {
		*bp = NULL;
		fault = 9004;
	}
	return fault;

}

/* This cwmpWalkPTree callback sets the commit flag */
static int markCommit(CWMPObject *o, CWMPParam *p, Instance *ip, void *up1,
		void *up2, int cbInt) {
	ParameterValueStruct *pv = (ParameterValueStruct *) up1;
	if (o->pCommitObj != NULL && ip != NULL) {
		if (strstr(pv->pName, wpPathName) == pv->pName) {
			// This object is in the path of the parameter Name */
			ip->flags |= COMMIT_PENDING;
			DBGPRINT((stderr, "Mark CommitObj: %s\n", wpPathName));
		}
	}
	return eOK;
}

/* This cwmpWalkPTree callback sets the commit flag */
/* cbInt contains the restore flag that forces the commit calls even if
 * there is an error. */
static int execCommit(CWMPObject *o, CWMPParam *p, Instance *ip, void *up1,
		void *up2, int cbInt) {
	int status;
	int *retStatus = (int *) up2;
	if (o->pCommitObj != NULL && ip != NULL) {
		if (ip->flags & COMMIT_PENDING) {
			ip->flags &= ~COMMIT_PENDING; /* clear pending flag */
			if (*retStatus < CPE_ERR || (cbInt && !(*retStatus & COMMIT_STOP))) {
				DBGPRINT((stderr, "ExecCommit at Obj: %s\n", wpPathName));
				status = o->pCommitObj(o, ip, cbInt);
				if ((status & COMMIT_STOP) || ((status & STATUS_MASK)
						>= CPE_ERR))
					*retStatus = status;
			} else {/* continue so pending flags are cleared */
				DBGPRINT((stderr, "ExecCommit skipped due to %s at %s\n",
								*retStatus&COMMIT_STOP? "COMMIT_STOP":"CPE_ERR", wpPathName));
			}
		}
	}
	return eOK;
}
/* This cwmpWalkPTree callback calls the Commit function at any object */
/* that has a commit flag set. The restore flag indicates that the call*/
/* has been made to commit restored values following a setter error return*/
static int runCommits(ParameterValueStruct *pv, int restore) {
	// First find the object/instances the are on the object path.
	ParameterValueStruct *p = pv;
	int status = CPE_OK;
	while (p) {
		cwmpFrameworkReset();
		cwmpWalkPTree(CBOBJECTS | DESCEND, CWMP_RootObject,
				CWMP_RootObject->iList, markCommit, p, NULL, restore);
		p = p->next;
	}
	// Now execute any Commit callbacks that have been set.
	cwmpFrameworkReset();
	cwmpWalkPTree(CBOBJECTS | DESCEND, CWMP_RootObject, CWMP_RootObject->iList,
			execCommit, p, &status, restore);
	return status & STATUS_MASK;
}

static void deleteAutoAddedInstance(SetParameterValuesMsg *rpc ){
	ParameterNameStruct *pns = rpc->autoAdd;
	ParameterNameStruct *last;
	CWMPObject *o;
	Instance	*ip;
	while ( pns ){
		o = cwmpFindObject(pns->pName);
		if (o) {
			ip = cwmpGetCurrentInstance();
			cwmpDeleteObjectInstance( o, ip);
		}
		last = pns;
		pns = pns->next;
		GS_FREE( last->pName);
		GS_FREE( last );
	}
	rpc->autoAdd = NULL;
}

static CWMPParam *autoAddInstance(SetParameterValuesMsg *rpc, ParameterValueStruct *pv ){
	char	ipath[257];
	CWMPObject *o;
	CPE_STATUS s;
	Instance 	*ip;
	char	*cp = pv->pName;
	cp = strstr( cp, "].");
	int	lth;
	while ( cp ){
		lth = cp-pv->pName+2;
		strncpy( ipath, pv->pName, lth);
		ipath[lth] = '\0';
		if ( (o=cwmpFindObject(ipath))==NULL){
			if ( (s = cwmpAddObjectInstance(ipath, &ip))== CPE_OK){
				/* keep path for cleanup is needed. */
				ParameterNameStruct *pns = (ParameterNameStruct*)GS_MALLOC(sizeof (struct ParameterNameStruct));
				if ( pns ){
					pns->pName = GS_STRDUP(ipath);
					pns->next = rpc->autoAdd;
					rpc->autoAdd = pns;
				}
			} else {
				// undo anything here?
			}
		}
		cp = strstr( pv->pName+lth, "].");
	}
	return cwmpFindParameter(pv->pName);
}
int runSetParameterValues(char *sessionID, RPCRequest *r, int *pending,
		char **bp) {
	XMLWriter *xp;
	int rebootFlag = 0;
	int commitStatus = 0;
	int error = 0;
	int nodups = 1;
	SetParameterValuesMsg *rpc = &r->ud.setPValuesReq;
	ParameterValueStruct *pv; /* pointer to current parameter  */
	/* insure not parameters are duplicated */
	pv = r->ud.setPValuesReq.pvList;
	while (error == 0 && pv) {
		ParameterValueStruct *p = pv->next;
		while (p) {
			/* validate RPC message */
			if (pv->pName == NULL || pv->value == NULL) {
				return -1;
			}
			if (streq(pv->pName, p->pName)) {
				error = 9003;
				nodups = 0;
				break;
			}
			p = p->next;
		}
		pv = pv->next;
	}
	if (nodups) {
		/* first get the current values for the parameters */
		pv = r->ud.setPValuesReq.pvList;
		cwmpSetRunState(RUNSTATE_COPYPARAMETER);
		while (pv) {
			CWMPParam *p;
			pv->origValue = NULL;
			p = cwmpFindParameter(pv->pName);
			if ( !p && cpeState.autoCreateInstances && cwmpGetFindStatus()==INSTANCE_NOT_FOUND)
				p = autoAddInstance(rpc, pv);
			if ( p ){
				if (p->setable == RPC_R) {
					pv->fault = 9008; /* non-writable parameter */
					error = 9003;
				} else if (p->pGetter && p->pGetter(cwmpGetCurrentInstance(),
						&pv->origValue) != CPE_OK) {
					pv->fault = 9002; /* internal error */
					error = 9003;
				} else
					pv->fault = 0;
			} else {
				pv->fault = 9005; /* 9005 param name not found */
				error = 9003;
			}
			pv = pv->next;
		}
		/* second: attempt to set the new parameter values */
		if (error == 0) {
			cwmpSetRunState(RUNSTATE_SETPARAMETER);
			pv = r->ud.setPValuesReq.pvList;
			while (pv && error == 0) {
				CWMPParam *p;
				if (pv->fault == 0) { /* process parameter that didn't get errors in first step */
					if ((p = cwmpFindParameter(pv->pName))) {
						pv->cpeStatus = p->pSetter(cwmpGetCurrentInstance(),
								pv->value ? pv->value : "");
						if (pv->cpeStatus == CPE_ERR) {
							pv->fault = 9002; /* internal error */
							error = 9003;
						} else if (pv->cpeStatus >= CPE_9000 && pv->cpeStatus
								<= CPE_VNDR_END) {
							pv->fault = pv->cpeStatus;
							error = 9003;
						} else if (pv->cpeStatus == CPE_REBOOT)
							rebootFlag |= 1;
						/* else if (pv->cpeStatus==CPE_OBJCOMMIT)
						 */
					} else {
						pv->fault = 9005; /* 9005 param name not found */
						error = 9003;
					}
				}
				pv = pv->next;
			}

			/* If no errors run the Commit callbacks on any parent objects */
			/* of the parameters just set. */
			if (!error) {
				commitStatus = runCommits(r->ud.setPValuesReq.pvList,
						COMMIT_SET);
				if (commitStatus == CPE_ERR) {
					error = 9003;
				} else if (commitStatus >= CPE_9000 && commitStatus
						<= CPE_VNDR_END) {
					error = 9003;
				} else if (commitStatus == CPE_REBOOT)
					rebootFlag |= 1;
			}

			if (error) {
				/* Fault so restore the original parameter values for the ones */
				/* we set so far */
				pv = r->ud.setPValuesReq.pvList;
				cwmpSetRunState(RUNSTATE_RESETPARAMETER);
				while (pv) {
					CWMPParam *p;
					//CPE_STATUS cstat; //-Werror=unused-but-set-variable
					if (pv->origValue && (p = cwmpFindParameter(pv->pName))
							&& p->pSetter != NULL) {
						//cstat = p->pSetter(cwmpGetCurrentInstance(),//-Werror=unused-but-set-variable
						p->pSetter(cwmpGetCurrentInstance(),//-Werror=unused-but-set-variable
								pv->origValue ? pv->origValue : "");
						if (pv->cpeStatus == CPE_REBOOT)
							rebootFlag |= 1;
						// ignore all other status returns in the recovery step.
					}
					pv = pv->next;
				}
				if ( cpeState.autoCreateInstances && rpc->autoAdd ){
					deleteAutoAddedInstance( rpc );
				}
				/* only call commit to restore is commit to set failed */
				if (commitStatus != CPE_REBOOT && commitStatus != CPE_OK)
					runCommits(r->ud.setPValuesReq.pvList, COMMIT_RESTORE);
			}
		}
	}
	/***  ***/
	if ((xp = xmlOpenWriter(SOAP_SENDBUFSZ, XML_WRITER_FLG))) {
		if (!error) {
			if (rebootFlag) {
				*pending = PENDING_REBOOT;
			} else
				*pending = 0;
			openEnvHdrBody(xp, sessionID);
			xmlOpenTagGrp(xp, "%sSetParameterValuesResponse", nsCWMP);
			xmlMemPrintf(xp, "<Status>%d</Status>\n", rebootFlag ? 1 : 0);
			xmlCloseTagGrp(xp); /* /SetParameterValuesResponse */
			closeEnvBody(xp); /* </Body> </envelope> */
		} else
			writeSOAPFault(xp, r, sessionID, error);
		*bp = xmlSaveCloseWriter(xp);
	} else {
		*bp = NULL;
		error = 9004;
	}
	return error;
}

int runAddObject(char *sessionID, RPCRequest *r, int *pending, char **bp) {
	XMLWriter *xp;
	int rebootFlag = 0;
	int error = 0;
	Instance *ip = NULL;
	AddDelObjectMsg *p; /*   */

	cwmpSetRunState(RUNSTATE_ADDOBJECT);
	p = &r->ud.addDelObjectReq;
	if (p->objectPath && strlen(p->objectPath) > 0) {
		if (isFragment(p->objectPath) ) {
			if ( !isInstance(p->objectPath)) {
				error = 9003;
			} else {
				CPE_STATUS cpeStatus;
				cpeStatus = cwmpAddObjectInstance(p->objectPath, &ip);
				if (cpeStatus == CPE_REBOOT)
					rebootFlag = 1;
				else if (cpeStatus == CPE_ERR)
					error = 9002; /* internal error */
				else if (cpeStatus >= CPE_9001 && cpeStatus <= CPE_VNDR_END)
					error = cpeStatus;
			}
		} else
			error = 9005;
	} else
		error = 9003; /* invalid arguments */

	/***  ***/
	if ((xp = xmlOpenWriter(SOAP_SENDBUFSZ, XML_WRITER_FLG))) {
		if (!error) {
			if (rebootFlag) {
				*pending = PENDING_REBOOT;
			} else
				*pending = 0;
			openEnvHdrBody(xp, sessionID);
			xmlOpenTagGrp(xp, "%sAddObjectResponse", nsCWMP);
			xmlPrintTaggedData(xp, "InstanceNumber", "%d", ip->id);
			xmlPrintTaggedData(xp, "Status", "%d", rebootFlag ? 1 : 0);
			xmlCloseTagGrp(xp); /* /AddObject */
			closeEnvBody(xp); /* </Body> </envelope> */
		} else {
			GS_FREE(ip);
			writeSOAPFault(xp, r, sessionID, error);
		}
		*bp = xmlSaveCloseWriter(xp);
	} else {
		*bp = NULL;
		error = 9004;
	}
	return error;
}


int runDeleteObject(char *sessionID, RPCRequest *r, int *pending, char **bp) {
	XMLWriter *xp;
	int rebootFlag = 0;
	int error = 0;
	AddDelObjectMsg *p; /*   */

	cwmpSetRunState(RUNSTATE_DELETEOBJECT);
	p = &r->ud.addDelObjectReq;
	if (p->objectPath && strlen(p->objectPath) > 0) {
		if (isInstance(p->objectPath)) {
			CWMPObject *o = cwmpFindObject(p->objectPath);
			if (o) {
				if (o->pDelObj && o->oType == eInstance) {
					CPE_STATUS cpeStatus;
					Instance *ip = cwmpGetCurrentInstance();
					cpeStatus = cwmpDeleteObjectInstance( o, ip);
					if (cpeStatus == CPE_REBOOT){
						rebootFlag = 1;
					} else if (cpeStatus == CPE_ERR)
						error = 9002; /* internal error */
					else if (cpeStatus >= CPE_9000 && cpeStatus <= CPE_VNDR_END)
						error = cpeStatus;
				} else
					error = 9003; /* invalid arguments */
			} else
				error = 9005; /* invalid parameter name */
		} else
			error = 9005;
	} else
		error = 9003; /* invalid arguments */
	/***  ***/
	if ((xp = xmlOpenWriter(SOAP_SENDBUFSZ, XML_WRITER_FLG))) {
		if (!error) {
			if (rebootFlag) {
				*pending = PENDING_REBOOT;
			} else
				*pending = 0;
			openEnvHdrBody(xp, sessionID);
			xmlOpenTagGrp(xp, "%sDeleteObjectResponse", nsCWMP);
			xmlPrintTaggedData(xp, "Status", "%d", rebootFlag ? 1 : 0);
			xmlCloseTagGrp(xp); /* /DeleteObjectResponse */
			closeEnvBody(xp); /* </Body> </envelope> */
		} else
			writeSOAPFault(xp, r, sessionID, error);
		*bp = xmlSaveCloseWriter(xp);
	} else {
		*bp = NULL;
		error = 9004;
	}
	return error;
}

#ifdef CONFIG_RPCUPLOAD
int runUpload(char *sessionID, RPCRequest *r, int *pending, char **bp) {
	XMLWriter *xp;
	int error = 0;

	/***  ***/
	if ((xp = xmlOpenWriter(SOAP_SENDBUFSZ, XML_WRITER_FLG))) {
		*pending = 0;
		cwmpQueueDLRequest(r, WAITINGQ);
		openEnvHdrBody(xp, sessionID);
		xmlOpenTagGrp(xp, "%sUploadResponse", nsCWMP);
		xmlPrintTaggedData(xp, "Status", "1");
		xmlPrintTaggedData(xp, "StartTime", UNKNOWN_TIME);
		xmlPrintTaggedData(xp, "CompleteTime", UNKNOWN_TIME);
		xmlCloseTagGrp(xp); /* /UploadResponse */
		closeEnvBody(xp); /* </Body> </envelope> */
		*bp = xmlSaveCloseWriter(xp);
	} else {
		*bp = NULL;
		error = 9002;
	}
	return error;
}
#endif
#ifdef CONFIG_RPCDOWNLOAD
int runDownload(char *sessionID, RPCRequest *r, int *pending, char **bp) {
	XMLWriter *xp;
	int error = 0;

	/***  ***/
	if ((xp = xmlOpenWriter(SOAP_SENDBUFSZ, XML_WRITER_FLG))) {
		*pending = 0;
		r->DLMsg.dlStatus = -2; /* waiting queue - not active */
		cwmpQueueDLRequest(r, WAITINGQ);
		openEnvHdrBody(xp, sessionID);
		xmlOpenTagGrp(xp, "%sDownloadResponse", nsCWMP);
		xmlPrintTaggedData(xp, "Status", "1");
		xmlPrintTaggedData(xp, "StartTime", UNKNOWN_TIME);
		xmlPrintTaggedData(xp, "CompleteTime", UNKNOWN_TIME);
		xmlCloseTagGrp(xp); /* /DownloadResponse */
		closeEnvBody(xp); /* </Body> </envelope> */
		*bp = xmlSaveCloseWriter(xp);
	} else {
		*bp = NULL;
		error = 9002;
	}
	return error;
}
#endif /* CONFIG_RPCDOWNLOAD */
#ifdef CONFIG_RPCSCHEDULEDOWNLOAD
int runScheduleDownload(char *sessionID, RPCRequest *r, int *pending, char **bp) {
	XMLWriter *xp;
	int error = 0;

	/***  ***/
	if ((xp = xmlOpenWriter(SOAP_SENDBUFSZ, XML_WRITER_FLG))) {
		*pending = 0;
		r->DLMsg.dlStatus = -2; /* waiting queue - not active */
		cwmpQueueDLRequest(r, WAITINGQ);
		openEnvHdrBody(xp, sessionID);
		xmlOpenTagGrp(xp, "%sScheduleDownloadResponse", nsCWMP);
		xmlCloseTagGrp(xp); /* /ScheduleDownloadResponse */
		closeEnvBody(xp); /* </Body> </envelope> */
		*bp = xmlSaveCloseWriter(xp);
	} else {
		*bp = NULL;
		error = 9002;
	}
	return error;
}
#endif /* CONFIG_RPCSCHEDULEDOWNLOAD */

#if defined(CONFIG_RPCREQUESTDOWNLOAD)||defined(CONFIG_RPCDOWNLOAD)||defined(CONFIG_RPCUPLOAD)
int runCancelTransfer(char *sessionID, RPCRequest *r, int *pending, char **bp) {
	XMLWriter *xp;
	int error = 0;

	/***  ***/
	if ((xp = xmlOpenWriter(SOAP_SENDBUFSZ, XML_WRITER_FLG))) {
		*pending = 0;
		cwmpCancelWaiting(r->commandKey);
		// TODO: check the active queue for one in-progress.
		openEnvHdrBody(xp, sessionID);
		xmlOpenTagGrp(xp, "%sCancelTransferResponse", nsCWMP);
		xmlCloseTagGrp(xp); /* /ScheduleDownloadResponse */
		closeEnvBody(xp); /* </Body> </envelope> */
		*bp = xmlSaveCloseWriter(xp);
	} else {
		*bp = NULL;
		error = 9002;
	}
	return error;
}
#endif /* runCancelTransfer */

#ifdef CONFIG_RPCREQUESTDOWNLOAD
int sendRequestDownload(char *sessionID, CPEState *cpe, char **bp) {
	XMLWriter *xp;
	int error = 0;

	/***  ***/
	if ((xp = xmlOpenWriter(SOAP_SENDBUFSZ, XML_WRITER_FLG))) {
		openEnvHdrBody(xp, sessionID);
		xmlOpenTagGrp(xp, "%sRequestDownload", nsCWMP);
		xmlPrintTaggedData(xp, "FileType", "%s", cpe->reqDLFileType);
		xmlOpenTagGrp(xp, "FileTypeArg", "%sarrayType=\"%sArgStruct[1]\"",
				nsSOAP_ENC, nsCWMP);
		xmlPrintTaggedData(xp, "Name", "%s", cpe->reqDLArgName);
		xmlPrintTaggedData(xp, "Value", "%s", cpe->reqDLArgValue);
		xmlCloseTagGrp(xp); /* </FileTypeArg> */
		xmlCloseTagGrp(xp); /* </RequestDownload> */
		closeEnvBody(xp); /* </Body> </envelope> */
		*bp = xmlSaveCloseWriter(xp);
	} else {
		*bp = NULL;
		error = 9002;
	}
	return error;
}
#endif /* CONFIG_RPCREQUESTDOWNLOAD */

#ifdef CONFIG_RPCAUTONOMOUSTRANSFERCOMPLETE
int sendAutonomousTransferComplete(char *sessionID, RPCRequest *r, char **bp) {
	XMLWriter *xp;
	int error = 0;

	/***  ***/
	if ((xp = xmlOpenWriter(SOAP_SENDBUFSZ, XML_WRITER_FLG))) {
		DownloadMsg *dl = &r->ud.downloadReq;
		openEnvHdrBody(xp, sessionID);
		xmlOpenTagGrp(xp, "%sAutonomousTransferComplete", nsCWMP);
		xmlPrintTaggedData(xp, "AnnounceURL", "%s",
				dl->successURL ? dl->successURL : "");
		xmlPrintTaggedData(xp, "TransferURL", "%s",
				dl->failureURL ? dl->failureURL : "");
		xmlPrintTaggedData(xp, "IsDownload", "%s",
				dl->delaySeconds ? "true" : "false");
		xmlPrintTaggedData(xp, "FileType", "%s",
				dl->fileType ? dl->fileType : "");
		xmlPrintTaggedData(xp, "FileSize", "%d", dl->fileSize);
		xmlPrintTaggedData(xp, "TargetFileName", "%s",
				dl->targetFileName ? dl->targetFileName : "");

		xmlOpenTagGrp(xp, "FaultStruct");
		xmlPrintTaggedData(xp, "FaultCode", "%d", dl->dlStatus);
		xmlPrintTaggedData(xp, "FaultString", "%s",
				cpeGetFaultIndexedMessage(dl->dlFaultMsg));
		xmlCloseTagGrp(xp); /* </FaultStruct> */
		xmlPrintTaggedData(xp, "StartTime", "%s",
				getXSIdateTime(&dl->dlStartTime));
		xmlPrintTaggedData(xp, "CompleteTime", "%s",
				getXSIdateTime(&dl->dlEndTime));
		xmlCloseTagGrp(xp); /* </AutonomousTransferComplete> */
		closeEnvBody(xp); /* </Body> </envelope> */
		*bp = xmlSaveCloseWriter(xp);
	} else {
		*bp = NULL;
		error = 9002;
	}
	return error;
}
#endif /* CONFIG_RPCAUTONOMOUSTRANSFERCOMPLETE */

#ifdef CONFIG_RPCKICK
int sendKickedRPC( char *sessionID, CPEState *cpe, char **bp) {
	XMLWriter *xp;
	int error = 0;

	/***  ***/
	if ( (xp = xmlOpenWriter(SOAP_SENDBUFSZ, XML_WRITER_FLG)) ) {
		openEnvHdrBody(xp, sessionID);
		xmlOpenTagGrp(xp, "%sKicked", nsCWMP);
		xmlPrintTaggedData(xp, "Command", "%s", cpe->kickCommand);
		xmlPrintTaggedData(xp, "Referer", "%s", cpe->kickReferer);
		xmlPrintTaggedData(xp, "Arg", "%s", cpe->kickArg);
		xmlPrintTaggedData(xp, "Next", "%s", cpe->kickNext);
		xmlCloseTagGrp( xp ); /* </Kicked> */
		closeEnvBody(xp); /* </Body> </envelope> */
		*bp = xmlSaveCloseWriter( xp);
	} else {
		*bp = NULL;
		error = 9002;
	}
	return error;
}
#endif /* CONFIG_RPCKICK */

int sendTransferComplete(char *sessionID, RPCRequest *r, char **bp) {
	XMLWriter *xp;
	int error = 0;

	/***  ***/
	if ((xp = xmlOpenWriter(SOAP_SENDBUFSZ, XML_WRITER_FLG))) {
		DownloadMsg *dl = &r->ud.downloadReq;
		openEnvHdrBody(xp, sessionID);
		xmlOpenTagGrp(xp, "%sTransferComplete", nsCWMP);
		xmlPrintTaggedData(xp, "CommandKey", "%s", r->commandKey);
		xmlOpenTagGrp(xp, "FaultStruct");
		xmlPrintTaggedData(xp, "FaultCode", "%d", dl->dlStatus);
		xmlPrintTaggedData(xp, "FaultString", "%s",
				cpeGetFaultIndexedMessage(dl->dlFaultMsg));
		xmlCloseTagGrp(xp); /* </FaultStruct> */
		xmlPrintTaggedData(xp, "StartTime", "%s",
				getXSIdateTime(&dl->dlStartTime));
		xmlPrintTaggedData(xp, "CompleteTime", "%s",
				getXSIdateTime(&dl->dlEndTime));
		xmlCloseTagGrp(xp); /* </TransferComplete> */
		closeEnvBody(xp); /* </Body> </envelope> */
		*bp = xmlSaveCloseWriter(xp);
	} else {
		*bp = NULL;
		error = 9002;
	}
	return error;
}

int sendGetRPCMethods(char *sessionID, CPEState *cpe, char **bp) {
	XMLWriter *xp;
	int error = 0;

	/***  ***/
	if ((xp = xmlOpenWriter(SOAP_SENDBUFSZ, XML_WRITER_FLG))) {
		openEnvHdrBody(xp, sessionID);
		xmlOpenTagGrp(xp, "%sGetRPCMethods", nsCWMP);
		xmlCloseTagGrp(xp); /* </GetRPCMethods> */
		closeEnvBody(xp); /* </Body> </envelope> */
		*bp = xmlSaveCloseWriter(xp);
	} else {
		*bp = NULL;
		error = 9002;
	}
	return error;
}

int runReboot(char *sessionID, RPCRequest *r, int *pending, char **bp) {
	XMLWriter *xp;
	int error = 0;

	/***  ***/
	if ((xp = xmlOpenWriter(SOAP_SENDBUFSZ, XML_WRITER_FLG))) {
		cwmpAddEvent(eEvtMReboot); /* need event */
		strcpy(cpeState.rebootCommandKey, r->commandKey);
		*pending = PENDING_REBOOT;
		openEnvHdrBody(xp, sessionID);
		xmlOpenTagGrp(xp, "%sRebootResponse", nsCWMP);
		xmlCloseTagGrp(xp); /* /RebootResponse */
		closeEnvBody(xp); /* </Body> </envelope> */
		*bp = xmlSaveCloseWriter(xp);
	} else {
		*bp = NULL;
		error = 9004;
	}
	return error;
}

int runFactoryReset(char *sessionID, RPCRequest *r, int *pending, char **bp) {
	XMLWriter *xp;
	int error = 0;
	/***  ***/
	if ((xp = xmlOpenWriter(SOAP_SENDBUFSZ, XML_WRITER_FLG))) {
		*pending = PENDING_RESET;
		openEnvHdrBody(xp, sessionID);
		xmlOpenTagGrp(xp, "%sFactoryResetResponse", nsCWMP);
		xmlCloseTagGrp(xp); /* /FactoryResetResponse */
		closeEnvBody(xp); /* </Body> </envelope> */
		*bp = xmlSaveCloseWriter(xp);
	} else {
		*bp = NULL;
		error = 9004;
	}
	return error;
}

int runSendFault(char *sessionID, int fault, char **bp) {
	XMLWriter *xp = xmlOpenWriter(SOAP_SENDFAULTSZ, XML_WRITER_FLG);
	if (xp) {
		writeSOAPFault(xp, NULL, sessionID, fault);
		*bp = xmlSaveCloseWriter(xp);
	}
	return 0;
}

#ifdef CONFIG_RPCGETQUEUEDTRANSFERS
int runGetQueuedTransfers(char *sessionID, char **bp) {
	int error = 0;
	int transferCnt = 0;
	XMLWriter *xp = xmlOpenWriter(SOAP_SENDBUFSZ, XML_WRITER_FLG);

	if (xp) {
		openEnvHdrBody(xp, sessionID);
		xmlOpenTagGrp(xp, "%sGetQueuedTransfersResponse", nsCWMP);
		xmlOpenTagGrp(xp,
				"TransferList %sarrayType=\"%sQueuedTransferStruct[!^^!]\"",
				nsSOAP_ENC, nsCWMP);
		transferCnt = cwmpGetQueuedTransfersList(xp);

		xmlCloseTagGrp(xp); /* /QueuedTransferStruct */
		plugArrayCount(xp, transferCnt);
		xmlCloseTagGrp(xp); /* /TransferList */
		xmlCloseTagGrp(xp); /* /GetQueuedTransferResponse */
		closeEnvBody(xp); /* </Body> </envelope> */
		*bp = xmlSaveCloseWriter(xp);
	} else {
		*bp = NULL;
		error = 9004;
	}
	return error;
}

#endif /*  CONFIG_RPCGETQUEUEDTRANSFERS */

#ifdef CONFIG_RPCGETALLQUEUEDTRANSFERS
int runGetAllQueuedTransfers(char *sessionID, char **bp) {
	int error = 0;
	int transferCnt = 0;
	XMLWriter *xp = xmlOpenWriter(SOAP_SENDBUFSZ, 0);

	if (xp) {
		openEnvHdrBody(xp, sessionID);
		xmlOpenTagGrp(xp, "%sGetAllQueuedTransfersResponse", nsCWMP);
		xmlOpenTagGrp(xp,
				"TransferList %sarrayType=\"%sAllQueuedTransferStruct[!^^!]\"",
				nsSOAP_ENC, nsCWMP);
		transferCnt = cwmpGetAllQueuedTransfersList(xp);

		xmlCloseTagGrp(xp); /* /AllQueuedTransferStruct */
		plugArrayCount(xp, transferCnt);
		xmlCloseTagGrp(xp); /* AllTransferList */
		xmlCloseTagGrp(xp); /* /GetAllQueuedTransferResponse */
		closeEnvBody(xp); /* </Body> </envelope> */
		*bp = xmlSaveCloseWriter(xp);
	} else {
		*bp = NULL;
		error = 9004;
	}
	return error;
}

#endif /*  CONFIG_RPCGETALLQUEUEDTRANSFERS */

#ifdef CONFIG_RPCSCHEDULEINFORM
int runScheduleInform(char *sessionID, RPCRequest *r, char **bp) {
	XMLWriter *xp;
	int when;
	int error = 0;
	/***  ***/
	strcpy(cpeState.scheduleInformCommandKey, r->commandKey);
	when = r->ud.scheduleInformReq.delaySeconds;
	setTimer(cwmpScheduleInform, NULL, when * 1000);
	if ((xp = xmlOpenWriter(SOAP_SENDBUFSZ, XML_WRITER_FLG))) {
		openEnvHdrBody(xp, sessionID);
		xmlOpenTagGrp(xp, "%sScheduleInformResponse", nsCWMP);
		xmlCloseTagGrp(xp); /* /ScheduleInformResponse */
		closeEnvBody(xp); /* </Body> </envelope> */
		*bp = xmlSaveCloseWriter(xp);
	} else {
		*bp = NULL;
		error = 9004;
	}
	return error;
}
#endif

#ifdef CONFIG_RPCSETVOUCHERS
int runSetVouchers( char *sessionID, RPCRequest *r, char **bp) {
	XMLWriter *xp;
	int error =0;
	/***  ***/
	if ( (error = cpeSetVouchers( &r->ud.setVouchersReq )) == 0) {
		return 9002;
	} else if (error >= 9000 ) {
		return error;
	}

	if ( (xp = xmlOpenWriter(SOAP_SENDBUFSZ, XML_WRITER_FLG)) ) {
		openEnvHdrBody(xp, sessionID);
		xmlOpenTagGrp(xp, "%sSetVouchersResponse", nsCWMP);
		xmlCloseTagGrp( xp ); /* SetVouchersResponse */
		closeEnvBody(xp); /* </Body> </envelope> */
		*bp= xmlSaveCloseWriter( xp);
	} else {
		*bp = NULL;
		error = 9004;
	}
	return error;
}
int runGetOptions( char *sessionID, RPCRequest *r, char **bp) {
	XMLWriter *xp;
	CPEOption *cpeOption;
	int optionCnt = 0;
	int error = 0;
	/***  ***/
	if ( (cpeOption = cpeGetOptions( r->ud.getOptionsReq.pOptionName )) == NULL ) {
		return 9002;
	}
	if ( (xp = xmlOpenWriter(SOAP_SENDBUFSZ, XML_WRITER_FLG)) ) {
		openEnvHdrBody(xp, sessionID);
		xmlOpenTagGrp(xp, "%sGetOptionsResponse", nsCWMP);
		xmlOpenTagGrp(xp, "OptionList %sarrayType=\"%sOptionStruct[!^^!]\"",
				nsSOAP_ENC, nsCWMP);
		while ( cpeOption != NULL) {
			CPEOption *tmp = cpeOption;
			++optionCnt;
			/* now fill in OptionsStruct in response */
			xmlOpenTagGrp(xp, "OptionStruct");
			xmlMemPrintf( xp, "<OptionName %stype=\"%s%s\">%s</OptionName>\n",
					nsXSI, nsXSD, getSOAPTypeStr(eString), cpeOption->optionName );
			xmlMemPrintf( xp, "<VoucherSN %stype=\"%s%s\">%s</VoucherSN>\n",
					nsXSI, nsXSD, getSOAPTypeStr(eUnsignedInt), cpeOption->voucherSN);
			xmlMemPrintf( xp, "<State %stype=\"%s%s\">%s</State>\n",
					nsXSI, nsXSD, getSOAPTypeStr(eUnsignedInt), cpeOption->state);
			xmlMemPrintf( xp, "<Mode %stype=\"%s%s\">%s</Mode>\n",
					nsXSI, nsXSD, getSOAPTypeStr(eInt), cpeOption->mode);
			xmlMemPrintf( xp, "<StartDate %stype=\"%s%s\">%s</StartDate>\n",
					nsXSI, nsXSD, getSOAPTypeStr(eDateTime), cpeOption->startDate);
			xmlMemPrintf( xp, "<ExpirationDate %stype=\"%s%s\">%s</ExpirationDate>\n",
					nsXSI, nsXSD, getSOAPTypeStr(eDateTime), cpeOption->expirationDate);
			xmlMemPrintf( xp, "<IsTransferable %stype=\"%s%s\">%s</IsTransferable>\n",
					nsXSI, nsXSD, getSOAPTypeStr(eBoolean), cpeOption->transferable);
			xmlCloseTagGrp(xp);
			cpeOption = cpeOption->next;
			GS_FREE( tmp );
		}
		plugArrayCount( xp, optionCnt );
		xmlCloseTagGrp( xp ); /* OptionList */
		xmlCloseTagGrp( xp ); /* GetOptionsResponse */
		closeEnvBody(xp); /* </Body> </envelope> */
		*bp= xmlSaveCloseWriter( xp);
	} else {
		*bp = NULL;
		error = 9004;
	}
	return error;
}
#endif  /* CONFIG_RPCSETVOUCHERS  */

#ifdef CONFIG_RPCCHANGEDUSTATE
#include "smm.h"

int cwmpQueueDUStateChange(RPCRequest *);

int runChangeDUState(char *sessionID, RPCRequest *r, char **bp) {

	int error = cwmpQueueDUStateChange(r);
	if (error == 0) {
		XMLWriter *xp;
		if ((xp = xmlOpenWriter(SOAP_SENDBUFSZ, XML_WRITER_FLG))) {
			openEnvHdrBody(xp, sessionID);
			xmlOpenTagGrp(xp, "%sChangeDUStateResponse", nsCWMP);
			xmlCloseTagGrp(xp); /* /ChangeDUStateResponse */
			closeEnvBody(xp); /* </Body> </envelope> */
			*bp = xmlSaveCloseWriter(xp);
		} else {
			*bp = NULL;
			error = 9004;
		}
	}
	return error;
}

int sendDUStateChangeComplete(char *sessionID, CPEChangeDUState *cpeDU,
		char **bp) {
	XMLWriter *xp;
	int error = 0;
	CPEOpResultStruct *rp = cpeDU->results;
	/***  ***/
	if ((rp != NULL) && (xp = xmlOpenWriter(SOAP_SENDBUFSZ, XML_WRITER_FLG))) {
		openEnvHdrBody(xp, sessionID);
		xmlOpenTagGrp(xp, "%sDUStateChangeComplete", nsCWMP);
		while (rp != NULL) {
			xmlOpenTagGrp(xp, "Results");
			xmlPrintTaggedData(xp, "UUID", "%s", rp->uuid? rp->uuid: "");
			xmlPrintTaggedData(xp, "DeploymentUnitRef", "%s", rp->duRef? rp->duRef: "");
			xmlPrintTaggedData(xp, "Version", "%s", rp->version? rp->version: "");
			xmlPrintTaggedData(xp, "CurrentState", "%s", rp->currentState? rp->currentState: "Failed");
			xmlPrintTaggedData(xp, "Resolved", "%s",
					rp->resolved ? "true" : "false");
			xmlPrintTaggedData(xp, "ExecutionUnitRefList", "%s",
					rp->euRefList ? rp->euRefList : "");
			xmlPrintTaggedData(xp, "StartTime", "%s",
					getXSIdateTime(&rp->startTime));
			xmlPrintTaggedData(xp, "CompleteTime", "%s",
					getXSIdateTime(&rp->completeTime));
			xmlOpenTagGrp(xp, "Fault");
			xmlPrintTaggedData(xp, "FaultCode", "%d", rp->faultCode);
			xmlPrintTaggedData(xp, "FaultString", "%s",
					rp->faultString ? rp->faultString : "");
			xmlCloseTagGrp(xp); /*Fault*/
			xmlCloseTagGrp(xp); /* Results*/
			rp = rp->next;
		}
		xmlPrintTaggedData(xp, "CommandKey", "%s", cpeDU->commandKey);
		xmlCloseTagGrp(xp); /* /DUStateChangeComplete> */
		closeEnvBody(xp); /* </Body> </envelope> */
		*bp = xmlSaveCloseWriter(xp);
	} else {
		*bp = NULL;
		error = 9004;
	}
	return error;
}

#endif /* CONFIG_RPCCHANGEDUSTATE */

