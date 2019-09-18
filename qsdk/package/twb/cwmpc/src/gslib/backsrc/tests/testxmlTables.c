
/*----------------------------------------------------------------------*
 * Gatespace
 * Copyright 2005 Gatespace. All Rights Reserved.
 *----------------------------------------------------------------------*
 * File Name  : XMLTables.c
 *
 * Description: SOAP xmlTables and data structures
 * $Revision: 1.1 $
 * $Id: testxmlTables.c,v 1.1 2011/11/16 13:56:37 dmounday Exp $
 *----------------------------------------------------------------------*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/utsname.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/errno.h>
#include <net/if.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <time.h>
#include <syslog.h>

#ifdef DMALLOC
#include "dmalloc.h"
#endif
#include "../src/utils.h"
#include "../src/xmlParser.h"
#include "../../includes/rpc.h"
#include "testxmlTables.h"

#ifdef DEBUG
#define DBGPRINT(X) fprintf X
#else
#define DBGPRINT(X)
#endif
#define PARAMETERKEY_SZ 33				/* 32 plus 1 for null */
#define COMMANDKEY_SZ   33

/* List of namespaces recognized by this parser */
/* Prefixes are set by each message */
/* !!!!! Notice change MACROS if this table is changed */
NameSpace nameSpaces[] = {
    {NULL, "SOAP-ENV:", "http://schemas.xmlsoap.org/soap/envelope/"},
    {NULL, "SOAP-ENC:", "http://schemas.xmlsoap.org/soap/encoding/"},
    {NULL, "xsd:",      "http://www.w3.org/2001/XMLSchema"},
    {NULL, "xsi:",      "http://www.w3.org/2001/XMLSchema-instance"},
    {NULL, "cwmp:",     "urn:dslforum-org:cwmp-1-0"},
    {NULL, NULL,NULL}
};

/* MACROS for referencing the above namespace */
/* strings from xml node description tables   */
/* must match initializations in xmlTables */
#define iSOAP       &nameSpaces[0]
#define iSOAP_ENC   &nameSpaces[1]
#define iXSD        &nameSpaces[2]
#define iXSI        &nameSpaces[3]
#define iCWMP       &nameSpaces[4]
/* */
/* Node descriptors for SOAP Header */
static XmlNodeDesc mustDesc[] = {
    {iSOAP, "mustUnderstand",NULL,NULL},
    {iSOAP, "actor", NULL,NULL}, /* not sure about this-- ignore attr value */
    {iXSI,	"type", NULL,NULL},
	{NULL, NULL}
};

XMLFUNC(fIDValue);
XMLFUNC(fHoldRequest);
static XmlNodeDesc headerDesc[] = {
    {iCWMP, "ID", fIDValue, mustDesc},               /* data value */
    {iCWMP, "HoldRequests",fHoldRequest, mustDesc}, /* data value */
    {iCWMP, "NoMoreRequests",NULL,NULL}, 			/* Removed in PD69-v5 -- data value flag */
    {NULL, NULL}
};

/* Node descriptors for body */

/* fault codes from ACS */
XMLFUNC(fFaultCode);
XMLFUNC(fFaultString);
static XmlNodeDesc faultDesc[] = {
    {NULL,  "faultCode", fFaultCode, NULL},               /* data value */
    {NULL,  "faultString",fFaultString, NULL}, /* data value */
    {NULL, NULL}
};

/* <cmsp:Reboot>
*/
XMLFUNC(fMaxEnvelopes);
static XmlNodeDesc informResponseDesc[] = {
    {NULL,  "MaxEnvelopes", fMaxEnvelopes, NULL},               /*  */
    {NULL, NULL}
};
/* <cmsp:Reboot>
*/

XMLFUNC(fCommandKey);
static XmlNodeDesc rebootDesc[] = {
    {NULL,  "CommandKey", fCommandKey, NULL},               /*  */
    {NULL, NULL}
};
/* <cmwp:Download >
*/

XMLFUNC(fDownloadFileType);
XMLFUNC(fDLCommandKey);
XMLFUNC(fDownloadURL);
XMLFUNC(fDownloadUser);
XMLFUNC(fDownloadPwd);
XMLFUNC(fDownloadFileSize);
XMLFUNC(fTargetFileName);
XMLFUNC(fDownloadDelaySec);
static XmlNodeDesc downLoadDesc[] = {
    {NULL,  "CommandKey", fDLCommandKey,NULL},
    {NULL,  "FileType", fDownloadFileType,NULL},
    {NULL,  "URL", fDownloadURL,NULL},
    {NULL,  "Username", fDownloadUser,NULL},
    {NULL,  "Password", fDownloadPwd,NULL},
    {NULL,  "FileSize", fDownloadFileSize,NULL},
    {NULL,  "TargetFileName", fTargetFileName,NULL},
    {NULL,  "DelaySeconds", fDownloadDelaySec,NULL},
    {NULL,  "SuccessURL", NULL,NULL},
    {NULL,  "FailureURL", NULL,NULL},
    {NULL,NULL}
};
/* <cwmp:DeleteObject>
*  <cwmp:AddObject>
*/

XMLFUNC(fObjectName);
XMLFUNC(fSetParameterKey);
static XmlNodeDesc addDelObjectDesc[] = {
    {NULL,  "ObjectName", fObjectName,NULL},
    {NULL,  "ParameterKey", fSetParameterKey, NULL},
    {NULL,NULL}
};

/* <cwmp:GetParameterValues ..../> */
/*
 * NOTE: arrayType is used generically for determining array size.
* <GetParameterValues>
 *  <ParameterNames arrayType="xsd:string[x]>
 *      <xsd:string>name1</xsd:string>
 *      .....
 *  </ParameterNames>
 * </GetParameterValues>
 */

XMLFUNC(fParameterNameArraySize);
XMLFUNC(fParameterNames);
XMLFUNC(fGetParameterValues);
static XmlNodeDesc pNamesDesc[] = {
    {iSOAP, "arrayType", fParameterNameArraySize,NULL},
    {iXSD,  "string", fParameterNames, NULL },    /* name value is in TAGDATA */
    {NULL,NULL}
};
static XmlNodeDesc getParameterValuesDesc[] = {
    {NULL,  "ParameterNames", NULL, pNamesDesc},
    {NULL,NULL}
};

/* <SetParameterValue>
*   <ParameterList <ParameterValueStruct>
*                       <Name>xxxx</Name> <Value>yyyy</value>
*   </ParamterList>
*   <ParameterKey>kkkkk</ParameterKey>
*/
XMLFUNC(fParameterValue );
XMLFUNC(fNames );
XMLFUNC(fSetParameterValues );

static XmlNodeDesc valueDesc[] = {
    {iXSD, "type", NULL, NULL},         /* may need to keep and verify type with CPE parameters */
    {NULL,NULL}
};
static XmlNodeDesc parameterValueStructDesc[] = {
    {NULL, "Name", fNames, NULL},
    {NULL, "Value", fParameterValue, valueDesc},
    {NULL,NULL}
};


/* <SetParameterAttributes>
*  <ParameterList <SetParameterAttributesStruct>
*               <Name>xxx</Name>  ....
*/

XMLFUNC(fSetAttParameterName );
XMLFUNC(fNotification);
XMLFUNC(fNotificationChange );
XMLFUNC(fAcclistChange);
XMLFUNC(fAccessList);
static XmlNodeDesc accessListDesc[] = {
	{NULL, "string", fAccessList, NULL},	/* only subscriber defined */
	{NULL, "arrayType", NULL,NULL},
	{NULL,NULL}
};
static XmlNodeDesc setparameterAttriStructDesc[] = {
    {NULL, "Name", fSetAttParameterName, NULL},
    {NULL, "Notification", fNotification, NULL},
    {NULL, "NotificationChange", fNotificationChange, NULL},
    {NULL, "AccessListChange", fAcclistChange, NULL},
    {NULL, "AccessList", NULL, accessListDesc},
    {NULL,NULL}
};

XMLFUNC(fSetParameterAttributesList);
XMLFUNC(fSetParameterValueStruct);
/* this is being used by multiple complex types -- will not detect mixed element errors*/
static XmlNodeDesc parameterListDesc[] = {
    {iSOAP, "arrayType", fParameterNameArraySize, NULL},
    {NULL, "ParameterValueStruct", fSetParameterValueStruct, parameterValueStructDesc},
    {NULL, "SetParameterAttributesStruct",fSetParameterAttributesList,setparameterAttriStructDesc},
    {NULL,NULL}
};
static XmlNodeDesc setParameterValuesDesc[] = {
    {NULL, "ParameterList", NULL, parameterListDesc},
    {NULL, "ParameterKey", fSetParameterKey, NULL},
    {NULL,NULL}
};
/*
*/
static XmlNodeDesc setParameterAttributesDesc[] = {
    {NULL, "ParameterList", NULL, parameterListDesc},
    {NULL,NULL}
};
/*
*/
static XmlNodeDesc getParameterAttributesDesc[] = {
    {NULL, "ParameterNames", NULL, pNamesDesc},
    {NULL,NULL}
};

/*
*/
XMLFUNC(fParameterPath);
XMLFUNC( fNextLevel);
static XmlNodeDesc getParameterNamesDesc[] = {
    {NULL,  "ParameterPath", fParameterPath, NULL},               /* data value */
    {NULL,  "NextLevel",fNextLevel, NULL}, /* data value */
    {NULL, NULL}
};

XMLFUNC(fFault);
XMLFUNC(fGetRPCMethods);
XMLFUNC(fGetParameterNames);
XMLFUNC(fSetParameterAttributes);
XMLFUNC(fGetParameterAttributes);
XMLFUNC(fAddObject);
XMLFUNC(fDeleteObject);
XMLFUNC(fDownload);
XMLFUNC(fReboot);
XMLFUNC(fInformResponse);
XMLFUNC(fFactoryReset);
XMLFUNC(fTransferCompleteResponse);
XMLFUNC(fGetRPCMethodsResponse);
static XmlNodeDesc bodyDesc[] = {
    {iSOAP, "Fault", fFault, faultDesc},
    {iCWMP, "GetRPCMethods", fGetRPCMethods, NULL},
    {iCWMP, "GetParameterNames", fGetParameterNames, getParameterNamesDesc},
    {iCWMP, "GetParameterValues", fGetParameterValues, getParameterValuesDesc},
    {iCWMP, "SetParameterValues", fSetParameterValues, setParameterValuesDesc},
    {iCWMP, "SetParameterAttributes", fSetParameterAttributes, setParameterAttributesDesc},
    {iCWMP, "GetParameterAttributes", fGetParameterAttributes, getParameterAttributesDesc},
    {iCWMP, "AddObject", fAddObject, addDelObjectDesc},
    {iCWMP, "DeleteObject", fDeleteObject, addDelObjectDesc},
    {iCWMP, "Download", fDownload, downLoadDesc},
    {iCWMP, "Reboot", fReboot, rebootDesc},
    {iCWMP, "InformResponse", fInformResponse, informResponseDesc},
    {iCWMP, "FactoryReset", fFactoryReset, NULL},
    {iCWMP, "TransferCompleteResponse", fTransferCompleteResponse, faultDesc},
    {iCWMP, "GetRPCMethodsResponse", fGetRPCMethodsResponse, NULL},	/* ignore content*/
    {NULL, NULL}
};

//XMLFUNC(fHeader);
static XmlNodeDesc envDesc[] = {
    {iSOAP, "Header", NULL, headerDesc},
    {iSOAP, "Body", NULL /*fBody*/, bodyDesc},
    {NULL, NULL}
};
/* TopLevel node for a SOAP envelope */
XMLFUNC(fEnvelope);
XmlNodeDesc cwmpEnvelopeDesc[] = {
    {iNULL,"Envelope",fEnvelope, envDesc}, /* -1 is namespace exception flag*/
    {NULL, NULL}
};

static XML_STATUS fFaultString(void *user, const char *name, TOKEN_TYPE ttype, const char *value)
{
    return XML_STS_OK;
}
static XML_STATUS fFaultCode(void *user, const char *name, TOKEN_TYPE ttype, const char *value)
{
    return XML_STS_OK;
}
static XML_STATUS fFault(void *user, const char *name, TOKEN_TYPE ttype, const char *value)
{
	RPCRequest *r = *(RPCRequest **)user;
    if (ttype == TAGBEGIN && r)
        r->rpcMethod = eFault;
    return XML_STS_OK;
}
/* <Download> ??????????????????????????????????
*/

static XML_STATUS fDownload(void *user, const char *name, TOKEN_TYPE ttype, const char *value)
{
	RPCRequest *r = *(RPCRequest **)user;
	if (ttype == TAGBEGIN && r)
        r->rpcMethod = eDownload;
    return XML_STS_OK;
}

static XML_STATUS fDLCommandKey(void *user, const char *name, TOKEN_TYPE ttype, const char *value)
{
	RPCRequest *r = *(RPCRequest **)user;
	if (ttype == TAGDATA && r)
		if ( strlen(value)<COMMANDKEY_SZ )
			strcpy( r->commandKey, value);
		else
			return XML_STS_ERR;
    return XML_STS_OK;
}
static XML_STATUS fDownloadFileType(void *user, const char *name, TOKEN_TYPE ttype, const char *value)
{
	RPCRequest *r = *(RPCRequest **)user;
    if (ttype == TAGDATA && r)
        r->ud.downloadReq.fileType = GS_STRDUP(value);
    return XML_STS_OK;
}
static XML_STATUS fDownloadURL(void *user, const char *name, TOKEN_TYPE ttype, const char *value)
{
	RPCRequest *r = *(RPCRequest **)user;
    if (ttype == TAGDATA && r)
        r->ud.downloadReq.URL = GS_STRDUP(value);
    return XML_STS_OK;
}
static XML_STATUS fDownloadUser(void *user, const char *name, TOKEN_TYPE ttype, const char *value)
{
	RPCRequest *r = *(RPCRequest **)user;
    if (ttype == TAGDATA)
        r->ud.downloadReq.userName = GS_STRDUP(value);
    return XML_STS_OK;
}
static XML_STATUS fDownloadPwd(void *user, const char *name, TOKEN_TYPE ttype, const char *value)
{
	RPCRequest *r = *(RPCRequest **)user;
    if (ttype == TAGDATA)
        r->ud.downloadReq.passWord = GS_STRDUP(value);
    return XML_STS_OK;
}
static XML_STATUS fDownloadFileSize(void *user, const char *name, TOKEN_TYPE ttype, const char *value)
{
	RPCRequest *r = *(RPCRequest **)user;
    if (ttype == TAGDATA)
        r->ud.downloadReq.fileSize = atoi(value);
    return XML_STS_OK;
}
static XML_STATUS fTargetFileName(void *user, const char *name, TOKEN_TYPE ttype, const char *value)
{
	RPCRequest *r = *(RPCRequest **)user;
    if (ttype == TAGDATA)
        r->ud.downloadReq.targetFileName = GS_STRDUP(value);
    return XML_STS_OK;
}
static XML_STATUS fDownloadDelaySec(void *user, const char *name, TOKEN_TYPE ttype, const char *value)
{
	RPCRequest *r = *(RPCRequest **)user;
    if (ttype == TAGDATA)
        r->ud.downloadReq.delaySeconds = atoi(value);
    return XML_STS_OK;
}

/* <xxx:InformResponse>
*       <MaxEnvelopes>xx</MaxEnvelopes>
* Assumes that we only get this in an InformResponse msg.
*/

static XML_STATUS fMaxEnvelopes(void *user, const char *name, TOKEN_TYPE ttype, const char *value)
{
	RPCRequest *r = *(RPCRequest **)user;
    if (ttype == TAGDATA)
        r->maxEnvelopes= atoi(value);
    return XML_STS_OK;
}
/* <Reboot>
*/

static XML_STATUS fCommandKey(void *user, const char *name, TOKEN_TYPE ttype, const char *value)
{
	RPCRequest *r = *(RPCRequest **)user;
	if (ttype == TAGDATA && r)
		if ( strlen(value)<COMMANDKEY_SZ )
			strcpy( r->commandKey, value);
		else
			return XML_STS_ERR;
    return XML_STS_OK;
}
static XML_STATUS fReboot(void *user, const char *name, TOKEN_TYPE ttype, const char *value)
{
	RPCRequest *r = *(RPCRequest **)user;
    if (ttype == TAGBEGIN)
        r->rpcMethod = eReboot;
    return XML_STS_OK;
}
/*
* <FactoryReset/>
*/
static XML_STATUS fFactoryReset(void *user, const char *name, TOKEN_TYPE ttype, const char *value)
{
	RPCRequest *r = *(RPCRequest **)user;
    if (ttype == TAGBEGIN)
        r->rpcMethod = eFactoryReset;
    return XML_STS_OK;
}

static XML_STATUS fInformResponse(void *user, const char *name, TOKEN_TYPE ttype, const char *value)
{
	RPCRequest *r = *(RPCRequest **)user;
    if (ttype == TAGBEGIN)
        r->rpcMethod = eInformResponse;
    return XML_STS_OK;
}
static XML_STATUS fTransferCompleteResponse(void *user, const char *name, TOKEN_TYPE ttype, const char *value)
{
	RPCRequest *r = *(RPCRequest **)user;
    if (ttype == TAGBEGIN)
        r->rpcMethod = eTransferCompleteResponse;
    return XML_STS_OK;
}
/* <AddObject>
*  <DeleteObject>
*
*/

static XML_STATUS fObjectName(void *user, const char *name, TOKEN_TYPE ttype, const char *value)
{
	RPCRequest *r = *(RPCRequest **)user;
    if (ttype == TAGDATA && r){
        r->ud.addDelObjectReq.objectPath = GS_STRDUP(value);
    }
    return XML_STS_OK;
}

static XML_STATUS fAddObject(void *user, const char *name, TOKEN_TYPE ttype, const char *value)
{
	RPCRequest *r = *(RPCRequest **)user;
    if (ttype == TAGBEGIN && r)
        r->rpcMethod = eAddObject;
    return XML_STS_OK;
}
static XML_STATUS fDeleteObject(void *user, const char *name, TOKEN_TYPE ttype, const char *value)
{
	RPCRequest *r = *(RPCRequest **)user;
    if (ttype == TAGBEGIN && r){
        r->rpcMethod = eDeleteObject;
    }
    return XML_STS_OK;
}
/* <SetParameterValue>
*   <ParameterList <ParameterValueStruct>
*                       <Name>xxxx</Name> <Value>yyyy</value>
*   </ParamterList>
*   <ParameterKey>kkkkk</ParameterKey>
*/
/* alloc a new struct for each ParameterValueStruct scanned over */
static XML_STATUS fSetParameterValueStruct(void *user, const char *name, TOKEN_TYPE ttype, const char *value)
{
	RPCRequest *r = *(RPCRequest **)user;
    if (ttype == TAGBEGIN && r ){
		ParameterValueStruct *p = (ParameterValueStruct *)GS_MALLOC(sizeof (struct ParameterValueStruct));
        if (p) { /* link new item to RPCRequest */
			memset(p, 0, sizeof(ParameterValueStruct));
            ++r->ud.setPValuesReq.paramCnt;
            p->next = r->ud.setPValuesReq.pvList;
            r->ud.setPValuesReq.pvList = p;
            return XML_STS_OK;
        }
	}
    return XML_STS_ERR;
}
static XML_STATUS fParameterValue(void *user, const char *name, TOKEN_TYPE ttype, const char *value)
{
	RPCRequest *r = *(RPCRequest **)user;
    if (ttype == TAGDATA && r ){
        if(r->ud.setPValuesReq.pvList)
            r->ud.setPValuesReq.pvList->value = GS_STRDUP(value);
		return XML_STS_OK;
    }
    return XML_STS_OK;
}
static XML_STATUS fSetParameterKey(void *user, const char *name, TOKEN_TYPE ttype, const char *value)
{
	RPCRequest *r = *(RPCRequest **)user;
    if (ttype == TAGDATA && r) {
    	if ( strlen(value)<PARAMETERKEY_SZ)
    		strcpy(r->parameterKey, value);
    	else
    		return XML_STS_ERR;      /* key length error */
    }
    return XML_STS_OK;
}
static XML_STATUS fSetParameterValues(void *user, const char *name, TOKEN_TYPE ttype, const char *value)
{
	RPCRequest *r = *(RPCRequest **)user;
    if (ttype == TAGBEGIN && r)
        r->rpcMethod = eSetParameterValues;
    return XML_STS_OK;
}

static XML_STATUS fNames(void *user, const char *name, TOKEN_TYPE ttype, const char *value)
{
	RPCRequest *r = *(RPCRequest **)user;
    if (ttype==TAGDATA && r) {
			ParameterValueStruct *p = r->ud.setPValuesReq.pvList;
			p->pName = GS_STRDUP(value);
    }
    return XML_STS_OK;
}
/* <ns:GetParameterValues ....../>
*
*/
/* generic for multiple rpc messages: set arrayTypeSize=n  from the    enc:arrayType="cwmp:XXXXX[n]">  */
static XML_STATUS fParameterNameArraySize(void *user, const char *name, TOKEN_TYPE ttype, const char *value)
{
    RPCRequest *r = *(RPCRequest **)user;
    if (ttype == ATTRIBUTEVALUE && r ){
        char *s;
        s = strchr(value,'[');
        if (s)
            r->arrayTypeSize = atoi(s+1);
    }
    return XML_STS_OK;
}

static int tagBeginFlg;	/* used to detect empty parameter names, such as: */
                        /*  <string></string>                             */
static XML_STATUS fParameterNames(void *user, const char *name, TOKEN_TYPE ttype, const char *value)
{
	RPCRequest *r = *(RPCRequest **)user;
	if ( (ttype==TAGDATA || (ttype==TAGEND && tagBeginFlg)) && r) {
		ParameterNameStruct *p = (ParameterNameStruct *)GS_MALLOC(sizeof (ParameterNameStruct));
		if (p) {
			memset(p, 0, sizeof(ParameterNameStruct));
			if (value)
				p->pName = GS_STRDUP(value);
			p->next = r->ud.getPAVReq.pnameList;
			r->ud.getPAVReq.pnameList = p;
			++r->ud.getPAVReq.paramCnt;
			tagBeginFlg = 0;
		} else
			return XML_STS_ERR;
	} else if (ttype==TAGBEGIN) {
		tagBeginFlg = 1;
	} else
		tagBeginFlg = 0;
	return XML_STS_OK;
}

static XML_STATUS fGetParameterValues(void *user, const char *name, TOKEN_TYPE ttype, const char *value)
{
	RPCRequest *r = *(RPCRequest **)user;
    if (ttype == TAGBEGIN && r )
        r->rpcMethod = eGetParameterValues;
    return XML_STS_OK;
}

/* <SetParameterAttributes>
*  <ParameterList <SetParameterAttributesStruct>
*               <Name>xxx</Name>  ....
*/
static XML_STATUS fAccessList(void *user, const char *name, TOKEN_TYPE ttype, const char *value)
{   /* there's only one access list item today */
	RPCRequest *r = *(RPCRequest **)user;
    if (ttype == TAGDATA && r){
        if (r->ud.setPAttrReq.paList) {
            r->ud.setPAttrReq.paList->accessList = !strcasecmp(value,"subscriber");
            return XML_STS_OK;
        }
        return XML_STS_ERR;
    }
    return XML_STS_OK;
}
static XML_STATUS fAcclistChange(void *user, const char *name, TOKEN_TYPE ttype, const char *value)
{
	RPCRequest *r = *(RPCRequest **)user;
    if (ttype == TAGDATA && r){
        if (r->ud.setPAttrReq.paList) {
            r->ud.setPAttrReq.paList->accessListChange = testBoolean(value);
            return XML_STS_OK;
        }
        return XML_STS_ERR;
    }
    return XML_STS_OK;
}
static XML_STATUS fNotificationChange(void *user, const char *name, TOKEN_TYPE ttype, const char *value)
{
	RPCRequest *r = *(RPCRequest **)user;
    if (ttype == TAGDATA && r){
        if (r->ud.setPAttrReq.paList) {
            r->ud.setPAttrReq.paList->notifyChange = testBoolean(value);
            return XML_STS_OK;
        }
        return XML_STS_ERR;
    }
    return XML_STS_OK;
}
static XML_STATUS fNotification(void *user, const char *name, TOKEN_TYPE ttype, const char *value)
{
	RPCRequest *r = *(RPCRequest **)user;
    if (ttype == TAGDATA && r){
        if ( r->ud.setPAttrReq.paList ) {
            r->ud.setPAttrReq.paList->notification = atoi(value);
            return XML_STS_OK;
        }
        return XML_STS_ERR;
    }
    return XML_STS_OK;
}
static XML_STATUS fSetAttParameterName(void *user, const char *name, TOKEN_TYPE ttype, const char *value)
{
	RPCRequest *r = *(RPCRequest **)user;
    if (ttype == TAGDATA && r){
        if (r->ud.setPAttrReq.paList) {
            r->ud.setPAttrReq.paList->pName = GS_STRDUP(value);
            return XML_STS_OK;
        }
        return XML_STS_ERR;
    }
    return XML_STS_OK;
}

static XML_STATUS fSetParameterAttributesList(void *user, const char *name, TOKEN_TYPE ttype, const char *value)
{
	RPCRequest *r = *(RPCRequest **)user;
    if (ttype == TAGBEGIN && r){
        ParameterAttributesStruct *p = (ParameterAttributesStruct *)GS_MALLOC(sizeof (ParameterAttributesStruct));
        if (p) {
			memset(p, 0, sizeof(ParameterAttributesStruct));
			++r->ud.setPAttrReq.paramCnt;
            p->next = r->ud.setPAttrReq.paList;
            r->ud.setPAttrReq.paList = p;
            return XML_STS_OK;
        }
        return XML_STS_ERR;
    }
    return XML_STS_OK;
}

static XML_STATUS fSetParameterAttributes(void *user, const char *name, TOKEN_TYPE ttype, const char *value)
{
	RPCRequest *r = *(RPCRequest **)user;
    if (ttype == TAGBEGIN && r)
        r->rpcMethod = eSetParameterAttributes;
    return XML_STS_OK;
}
/* <GetParameterAttributes>
*  <ParameterNames SOAP-ENC:arrayType="xsd:string[xx]"
*               <xsd:string>xxx</xsd:string>  ....
*/
static XML_STATUS fGetParameterAttributes(void *user, const char *name, TOKEN_TYPE ttype, const char *value)
{
	RPCRequest *r = *(RPCRequest **)user;
    if (ttype == TAGBEGIN && r)
        r->rpcMethod = eGetParameterAttributes;
    return XML_STS_OK;
}


/* <ns:GetParameterNames>
*     <ParameterPath>parameterpath</ParameterPath>
*  Only one parameterPath is allowed in msg.
*/
static XML_STATUS fGetParameterNames(void *user, const char *name, TOKEN_TYPE ttype, const char *value)
{
	RPCRequest *r = *(RPCRequest **)user;
    if (ttype == TAGBEGIN && r )
        r->rpcMethod = eGetParameterNames;
    return XML_STS_OK;
}
static XML_STATUS fParameterPath(void *user, const char *name, TOKEN_TYPE ttype, const char *value)
{
	RPCRequest *r = *(RPCRequest **)user;
    if (ttype == TAGDATA && r )
        r->ud.getPNamesReq.pNamePath = GS_STRDUP(value);
    return XML_STS_OK;
}
static XML_STATUS fNextLevel(void *user, const char *name, TOKEN_TYPE ttype, const char *value)
{
	RPCRequest *r = *(RPCRequest **)user;
    if (ttype == TAGDATA && r)
        r->ud.getPNamesReq.nextLevel = testBoolean(value);
    return XML_STS_OK;
}
/* <SOAP_ENV:Body ..../> */


static XML_STATUS fGetRPCMethods(void *user, const char *name, TOKEN_TYPE ttype, const char *value)
{
	RPCRequest *r = *(RPCRequest **)user;
    if (ttype == TAGBEGIN && r )
        r->rpcMethod = eGetRPCMethods;
    return XML_STS_OK;
}


static XML_STATUS fGetRPCMethodsResponse(void *user, const char *name, TOKEN_TYPE ttype, const char *value)
{
	RPCRequest *r = *(RPCRequest **)user;
    if (ttype == TAGBEGIN && r )
        r->rpcMethod = eGetRPCMethodsResponse;
    return XML_STS_OK;
}

static XML_STATUS fHoldRequest(void *user, const char *name, TOKEN_TYPE ttype, const char *value)
{
	RPCRequest *r = *(RPCRequest **)user;
    if (ttype == TAGDATA)
        r->holdRequests = !strcmp(value,"1");

    return XML_STS_OK;
}
static XML_STATUS fIDValue(void *user, const char *name, TOKEN_TYPE ttype, const char *value)
{
	RPCRequest *r = *(RPCRequest **)user;
    if (ttype == TAGDATA) {
        if (r)
            r->ID = GS_STRDUP(value);
        else
            return XML_STS_ERR;
    }
    return XML_STS_OK;
}
/* <envelope .../>   */
static XML_STATUS fEnvelope(void *user, const char *name, TOKEN_TYPE ttype, const char *value)
{
	RPCRequest **req = (RPCRequest **)user;
	if (req) {
		if (ttype == TAGBEGIN) {
			DBGPRINT((stderr, "fEnvelope() TAGBEGIN ------\n"));
			/* allocate a new RPCRequest to hold request */
			if ( (*req=(RPCRequest *)GS_MALLOC(sizeof(struct RPCRequest))) )
				memset(*req, 0, sizeof(struct RPCRequest));
		} else {
			DBGPRINT((stderr, "fEnvelope() TAGEND -----\n"));
			/* end of envelope -- now run RPC */
			/* */
		}
		return XML_STS_OK;
	}
	return XML_STS_ERR;  /* no user context pointer */
}



