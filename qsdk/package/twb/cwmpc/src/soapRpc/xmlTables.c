
/*----------------------------------------------------------------------*
 * Gatespace
 * Copyright 2005-2011 Gatespace. All Rights Reserved.
 * Gatespace Networks, Inc. confidential material.
 *----------------------------------------------------------------------*
 * File Name  : XMLTables.c
 *
 * Description: SOAP xmlTables and data structures
 * $Revision: 1.15 $
 * $Id: xmlTables.c,v 1.15 2011/11/16 14:00:27 dmounday Exp $
 *----------------------------------------------------------------------*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <ctype.h>
#include <errno.h>
#include <time.h>
#include <syslog.h>

#ifdef DMALLOC
#include "dmalloc.h"
#endif
#include "../gslib/src/utils.h"
#include "../gslib/src/xmlParser.h"
#include "../includes/rpc.h"
#include "../includes/sys.h"
#include "xmlTables.h"

#ifdef DEBUG
#define DBGPRINT(X) fprintf X
#else
#define DBGPRINT(X)
#endif

/* List of namespaces recognized by this parser */
static char dslNS[] = DSLFORUM_NAMESPACE;
/* Prefixes are set by each message */
/* !!!!! Notice change MACROS if this table is changed */
NameSpace nameSpaces[] = {
    {NULL, "SOAP-ENV:", "http://schemas.xmlsoap.org/soap/envelope/"},
    {NULL, "SOAP-ENC:", "http://schemas.xmlsoap.org/soap/encoding/"},
    {NULL, "xsd:",      "http://www.w3.org/2001/XMLSchema"},
    {NULL, "xsi:",      "http://www.w3.org/2001/XMLSchema-instance"},
    {NULL, "cwmp:",     dslNS},                /* urn:dslforum-org:cwmp-1-? */
    {NULL, NULL, NULL}
};

/* MACROS for referencing the above namespace */
/* strings from xml node description tables   */
/* must match initializations in xmlTables */
#define iSOAP       &nameSpaces[0]
#define iSOAP_ENC   &nameSpaces[1]
#define iXSD        &nameSpaces[2]
#define iXSI        &nameSpaces[3]
#define iCWMP       &nameSpaces[4]
/*
 * Overwrite the CWMP namespace url with the CWMP version number.
 *
 */
void setNameSpaceVersion( int cwmpVer ){
	char	v = cwmpVer+0x30;
	char	*p = dslNS + strlen(dslNS)-1;
	*p = v;		/* over write last char in namespace url with new version */
}

/* */
/* Node descriptors for SOAP Header */
static XmlNodeDesc mustDesc[] = {
    {iSOAP, "mustUnderstand", NULL, NULL},
    {iSOAP, "actor", NULL, NULL}, /* not sure about this-- ignore attr value */
    {iXSI,	"type", NULL, NULL},
    {NULL, NULL}
};

XMLFUNC(fIDValue);
XMLFUNC(fHoldRequest);
static XmlNodeDesc headerDesc[] = {
    {iDEFAULT, "ID", fIDValue, mustDesc},               /* data value */
    {iDEFAULT, "HoldRequests", fHoldRequest, mustDesc}, /* data value */
    {iDEFAULT, "NoMoreRequests", NULL, NULL}, 			/* Removed in PD69-v5 -- data value flag */
    {NULL, NULL}
};

/* Node descriptors for body */

XMLFUNC(fFaultCode);
XMLFUNC(fFaultString);
static XmlNodeDesc faultDetailDesc[] = {
    {NULL,  "FaultCode", fFaultCode, NULL},               /* data value */
    {NULL,  "FaultString", fFaultString, NULL},             /* data value */
    {NULL, NULL}
};

static XmlNodeDesc detailDesc[] = {
    {iDEFAULT, "Fault", NULL, faultDetailDesc},
    {NULL, NULL}
};
/* fault codes from ACS */
static XmlNodeDesc faultDesc[] = {
    {NULL,  "faultcode", NULL, NULL},               /* data value */
    {NULL,  "faultstring", NULL, NULL}, /* data value */
    {NULL,  "detail", NULL, detailDesc},
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

#ifdef CONFIG_RPCCHANGEDUSTATE
/********
 * Sample ChangeDUState RPC with allowed operation types.
<soapenv:Envelope xmlns:xsd="http://www.w3.org/2001/XMLSchema"
 xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
 xmlns:soapenv="http://schemas.xmlsoap.org/soap/envelope/"
 xmlns:soapenc="http://schemas.xmlsoap.org/soap/encoding/"
 xmlns:urn="urn:dslforum-org:cwmp-1-2">
  <soapenv:Header/>
  <soapenv:Body>
    <urn:ChangeDUState>
      <Operations xsi:type="urn:InstallOpStruct">
        <URL>http://xyz.com/software/pgm1</URL>
        <UUID>12345678-1234-1234-1234-1234567890ab</UUID>
        <Username>abc</Username>
        <ExecutionEnvRef>InternetGatewayDevice.SoftwareModules.ExecEnv.1.</ExecutionEnvRef>
      </Operations>
      <Operations xsi:type="urn:InstallOpStruct">
        <URL>http://xyz.com/software/pgmXYZ</URL>
        <UUID>12345678-1234-1234-1234-1234567890ab</UUID>
        <Username>abc</Username>
        <ExecutionEnvRef/>
      </Operations>
      <Operations xsi:type="urn:UpdateOpStruct">
        <UUID>12345678-1234-1234-1234-1234567890ab</UUID>
        <Version>1</Version>
        <URL>http://acs.gatespace.net/download/xyz</URL>
        <Username>abc</Username>
        <Password>xyz</Password>
      </Operations>
      <Operations xsi:type="urn:UninstallOpStruct">
        <UUID>12345678-1234-1234-1234-1234567890ab</UUID>
        <Version/>
        <ExecutionEnvRef>InternetGatewayDevice.SoftwareModules.ExecEnv.1.</ExecutionEnvRef>
      </Operations>
      <CommandKey>ckey</CommandKey>
    </urn:ChangeDUState>
  </soapenv:Body>
</soapenv:Envelope>
******/
XMLFUNC(fChangeDUState);
XMLFUNC(fDUStateChangeCompleteResponse);
XMLFUNC(fAutonmousDUStateChangeCompleteResponse);
XMLFUNC(fOperations);
XMLFUNC(fDUOpURL);
XMLFUNC(fDUOpUUID);
XMLFUNC(fDUOpUsername);
XMLFUNC(fDUOpPassword);
XMLFUNC(fDUOpExecutionEnvRef);
XMLFUNC(fDUOpType);
XMLFUNC(fDUOpVersion);

static XML_STATUS fDUStateChangeCompleteResponse( void *user, const char *name, TOKEN_TYPE ttype, const char *value) {
    RPCRequest *r = *(RPCRequest **)user;
    if (ttype == TAGBEGIN && r)
        r->rpcMethod = eDUStateChangeCompleteResponse;
    else if (ttype == TAGEND )
    	;
    return XML_STS_OK;
}
static XML_STATUS fAutonmousDUStateChangeCompleteResponse( void *user, const char *name, TOKEN_TYPE ttype, const char *value) {
    RPCRequest *r = *(RPCRequest **)user;
    if (ttype == TAGBEGIN && r)
        r->rpcMethod = eAutonomousDUStateChangeCompleteResponse;
    else if (ttype == TAGEND )
    	;
    return XML_STS_OK;
}

static XmlNodeDesc operationsDesc[] = {
		{NULL, "type", fDUOpType, NULL},
		{NULL, "URL", fDUOpURL, NULL},
		{NULL, "UUID", fDUOpUUID, NULL},
		{NULL, "Username", fDUOpUsername, NULL},
		{NULL, "Password", fDUOpPassword, NULL},
		{NULL, "Version", fDUOpVersion, NULL},
		{NULL, "ExecutionEnvRef", fDUOpExecutionEnvRef, NULL},
		{NULL, NULL}
};
static XmlNodeDesc changeDUStateDesc[] = {
	{NULL, "Operations", fOperations, operationsDesc},
	{NULL, "CommandKey", fCommandKey, NULL},
	{NULL, NULL}
};
/**
 * Note: The fDUOpXXXX functions to not track the receipt of empty elements. There
 * is no status of having received for example, <UUID/>.
 * The fDUOpUUID function will be called with TOKEN_TYPEs
 *    TAG_BEGIN, TAG_END.
 *
 */
static XML_STATUS fDUOpUUID(void *user, const char *name, TOKEN_TYPE ttype, const char *value) {
    RPCRequest *r = *(RPCRequest **)user;
    DUOperationStruct *op = r->ud.changeDUState.opList;
    if (ttype == TAGDATA ){
    	if ( op && op->uuid==NULL ) {
    		op->uuid = GS_STRDUP(value);
    	} else
    		return XML_STS_ERR;
    }
    return XML_STS_OK;
}
static XML_STATUS fDUOpUsername(void *user, const char *name, TOKEN_TYPE ttype,
		const char *value) {
	RPCRequest *r = *(RPCRequest **) user;
	DUOperationStruct *op = r->ud.changeDUState.opList;
	if (ttype == TAGDATA) {
		if (op && op->userName == NULL) {
			op->userName = GS_STRDUP(value);
		} else
			return XML_STS_ERR;
	}
	return XML_STS_OK;
}
static XML_STATUS fDUOpPassword(void *user, const char *name, TOKEN_TYPE ttype, const char *value) {
    RPCRequest *r = *(RPCRequest **)user;
    DUOperationStruct *op = r->ud.changeDUState.opList;
	if (ttype == TAGDATA) {
		if (op && op->passWord == NULL) {
			op->passWord = GS_STRDUP(value);
		} else
			return XML_STS_ERR;
	}
	return XML_STS_OK;
}

static XML_STATUS fDUOpVersion(void *user, const char *name, TOKEN_TYPE ttype, const char *value) {
    RPCRequest *r = *(RPCRequest **)user;
    DUOperationStruct *op = r->ud.changeDUState.opList;
	if (ttype == TAGDATA) {
		if (op && op->version == NULL) {
			op->version = GS_STRDUP(value);
		} else
			return XML_STS_ERR;
	}
	return XML_STS_OK;
}
static XML_STATUS fDUOpExecutionEnvRef(void *user, const char *name, TOKEN_TYPE ttype, const char *value) {
    RPCRequest *r = *(RPCRequest **)user;
    DUOperationStruct *op = r->ud.changeDUState.opList;
	if (ttype == TAGDATA) {
		if (op && op->execEnvRef == NULL) {
			op->execEnvRef = GS_STRDUP(value);
		} else
			return XML_STS_ERR;
	}
	return XML_STS_OK;
}
static XML_STATUS fDUOpURL(void *user, const char *name, TOKEN_TYPE ttype, const char *value) {
    RPCRequest *r = *(RPCRequest **)user;
    DUOperationStruct *op = r->ud.changeDUState.opList;
	if (ttype == TAGDATA) {
		if (op && op->url == NULL) {
			op->url = GS_STRDUP(value);
		} else
			return XML_STS_ERR;
	}
	return XML_STS_OK;
}

static XML_STATUS fDUOpType(void *user, const char *name, TOKEN_TYPE ttype, const char *value) {
    RPCRequest *r = *(RPCRequest **)user;
    DUOperationStruct *op = r->ud.changeDUState.opList;
    if (ttype == ATTRIBUTEVALUE &&  op ) {
    	if ( strstr( value, "InstallOpStruct"))
    		op->op = eInstallOp;
    	else if ( strstr( value, "UpdateOpStruct"))
    		op->op = eUpdateOp;
    	else if ( strstr( value, "UninstallOpStruct"))
    		op->op = eUninstallOp;
    	else
    		return XML_STS_ERR;
    }
    return XML_STS_OK;
}
/*
 * This queuing function is used to place the new item at the end of the list.
 * Required as the spec says that the order or the DUStateChangeComplete OpResultStruct must be
 * the same as the order of the DUOperations in the the ChangeDUState.
 */
static void qDUOp(DUOperationStruct **qHdr, DUOperationStruct *r) {
	DUOperationStruct **last = qHdr;
	DUOperationStruct *p;
    while ( *last ) {
        p = *last;
        last = &p->next;
    }
    *last = r;
    r->next = NULL;
}

static XML_STATUS fOperations(void *user, const char *name, TOKEN_TYPE ttype, const char *value) {
    RPCRequest *r = *(RPCRequest **)user;
    DUOperationStruct *p;
    if (ttype == TAGBEGIN && r ){
        p = (DUOperationStruct *)GS_MALLOC(sizeof (struct DUOperationStruct));
        if (p) { /* link new item to RPCRequest at beginning of opList so above functions point to it.*/
            memset(p, 0, sizeof(DUOperationStruct));
            p->next = r->ud.changeDUState.opList;
            r->ud.changeDUState.opList =  p;
            ++r->ud.changeDUState.operationCnt;
        } else
            return XML_STS_MEM;
    } else if (ttype == TAGEND && r && (p = r->ud.changeDUState.opList) ) {
    	/* verify some required parameters */
    	if ((p->op == eNoOp)
    	  ||(p->op == eUninstallOp && p->uuid==NULL )){
    		return XML_STS_ERR;
    	}
        /* move the item at head of opList to the end of the list if more than one item */
        if ( p->next ) {
            r->ud.changeDUState.opList = p->next;
            p->next = NULL;
            qDUOp( &r->ud.changeDUState.opList, p);
        }
    }
    return XML_STS_OK;
}
static XML_STATUS fChangeDUState(void *user, const char *name, TOKEN_TYPE ttype, const char *value) {
    RPCRequest *r = *(RPCRequest **)user;
    if (ttype == TAGBEGIN && r)
        r->rpcMethod = eChangeDUState;
    else if (ttype == TAGEND )
    	;
    return XML_STS_OK;
}
#endif


#ifdef CONFIG_RPCSCHEDULEDOWNLOAD
XMLFUNC(fWindowStart);
XMLFUNC(fWindowEnd);
XMLFUNC(fWindowMode);
XMLFUNC(fUserMessage);
XMLFUNC(fMaxRetries);
XMLFUNC(fTimeWindowList);
XMLFUNC(fTimeWindowStruct);

static XmlNodeDesc timeWindowStructDesc[] = {
    {NULL,  "WindowStart", fWindowStart, NULL},               /*  */
    {NULL,  "WindowEnd", fWindowEnd, NULL},               /*  */
    {NULL,  "WindowMode", fWindowMode, NULL},               /*  */
    {NULL,  "UserMessage", fUserMessage, NULL},               /*  */
    {NULL,  "MaxRetries", fMaxRetries, NULL},               /*  */
    {NULL, NULL}
};
static XmlNodeDesc timeWindowListDesc[] = {
	{NULL,  "arrayType", NULL, NULL },
    {NULL,  "TimeWindowStruct", fTimeWindowStruct, timeWindowStructDesc},               /*  */
    {NULL, NULL}
};

static XmlNodeDesc cancelTransferDesc[] = {
    {NULL,  "CommandKey", fCommandKey, NULL},
    {NULL, NULL}
};

#endif

/* <cmwp:Download >
 */

XMLFUNC(fDownloadFileType);
XMLFUNC(fDownloadURL);
XMLFUNC(fDownloadUser);
XMLFUNC(fDownloadPwd);
XMLFUNC(fDownloadFileSize);
XMLFUNC(fTargetFileName);
XMLFUNC(fDownloadDelaySec);
XMLFUNC(fSuccessURL);
XMLFUNC(fFailureURL);

static XmlNodeDesc downLoadDesc[] = {
    {NULL,  "CommandKey", fCommandKey, NULL},
    {NULL,  "FileType", fDownloadFileType, NULL},
    {NULL,  "URL", fDownloadURL, NULL},
    {NULL,  "Username", fDownloadUser, NULL},
    {NULL,  "Password", fDownloadPwd, NULL},
    {NULL,  "FileSize", fDownloadFileSize, NULL},
    {NULL,  "TargetFileName", fTargetFileName, NULL},
    {NULL,  "DelaySeconds", fDownloadDelaySec, NULL},
    {NULL,  "SuccessURL", fSuccessURL, NULL},
    {NULL,  "FailureURL", fFailureURL, NULL},
#ifdef CONFIG_RPCSCHEDULEDOWNLOAD
    {NULL,  "TimeWindowList", fTimeWindowList, timeWindowListDesc},
#endif
    {NULL, NULL}
};
/* <cwmp:DeleteObject>
 *  <cwmp:AddObject>
 */

XMLFUNC(fObjectName);
XMLFUNC(fSetParameterKey);
static XmlNodeDesc addDelObjectDesc[] = {
    {NULL,  "ObjectName", fObjectName, NULL},
    {NULL,  "ParameterKey", fSetParameterKey, NULL},
    {NULL, NULL}
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
    {iSOAP, "arrayType", fParameterNameArraySize, NULL},
    {iXSD,  "string", fParameterNames, NULL },    /* name value is in TAGDATA */
    {NULL,  "ParameterName", fParameterNames, NULL}, /* Not part of schema: !!!!!!! kludge for KT ACS */
    {NULL, NULL}                                     /* !!!!!!!!!!!!!!!!!!!*/
};
static XmlNodeDesc getParameterValuesDesc[] = {
    {NULL,  "ParameterNames", NULL, pNamesDesc},
    {NULL, NULL}
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
    {NULL, NULL}
};
static XmlNodeDesc parameterValueStructDesc[] = {
    {NULL, "Name", fNames, NULL},
    {NULL, "Value", fParameterValue, valueDesc},
    {NULL, NULL}
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
    {NULL, "arrayType", NULL, NULL},
    {NULL, NULL}
};
static XmlNodeDesc setparameterAttriStructDesc[] = {
    {NULL, "Name", fSetAttParameterName, NULL},
    {NULL, "Notification", fNotification, NULL},
    {NULL, "NotificationChange", fNotificationChange, NULL},
    {NULL, "AccessListChange", fAcclistChange, NULL},
    {NULL, "AccessList", NULL, accessListDesc},
    {NULL, NULL}
};

XMLFUNC(fSetParameterAttributesList);
XMLFUNC(fSetParameterValueStruct);
/* this is being used by multiple complex types -- will not detect mixed element errors*/
static XmlNodeDesc parameterListDesc[] = {
    {iSOAP, "arrayType", fParameterNameArraySize, NULL},
    {NULL, "ParameterValueStruct", fSetParameterValueStruct, parameterValueStructDesc},
    {NULL, "SetParameterAttributesStruct", fSetParameterAttributesList, setparameterAttriStructDesc},
    {NULL, NULL}
};

static XmlNodeDesc setParameterValuesDesc[] = {
    {NULL, "ParameterList", NULL, parameterListDesc},
    {NULL, "ParameterKey", fSetParameterKey, NULL},
    {NULL, NULL}
};
/*
 */
static XmlNodeDesc setParameterAttributesDesc[] = {
    {NULL, "ParameterList", NULL, parameterListDesc},
    {NULL, NULL}
};
/* added to enable start of parsing at this node */
XmlNodeDesc *cwmpAttributeListDesc=&setParameterAttributesDesc[0];
/*
 */
static XmlNodeDesc getParameterAttributesDesc[] = {
    {NULL, "ParameterNames", NULL, pNamesDesc},
    {NULL, NULL}
};

/*
 */
XMLFUNC(fParameterPath);
XMLFUNC( fNextLevel);
static XmlNodeDesc getParameterNamesDesc[] = {
    {NULL,  "ParameterPath", fParameterPath, NULL},               /* data value */
    {NULL,  "NextLevel", fNextLevel, NULL}, /* data value */
    {NULL, NULL}
};

#ifdef CONFIG_RPCSCHEDULEINFORM
XMLFUNC(fDelaySeconds);
XMLFUNC(fScheduleInform);
static XmlNodeDesc scheduleInformDesc[] = {
    {NULL,  "DelaySeconds", fDelaySeconds, NULL},               /* data value */
    {NULL,  "CommandKey", fCommandKey, NULL},                   /* data value */
    {NULL, NULL}
};
#endif
XMLFUNC(fFault);
XMLFUNC(fGetRPCMethods);
XMLFUNC(fGetParameterNames);
XMLFUNC(fSetParameterAttributes);
XMLFUNC(fGetParameterAttributes);
XMLFUNC(fAddObject);
XMLFUNC(fDeleteObject);
#ifdef CONFIG_RPCDOWNLOAD
XMLFUNC(fDownload);
#endif
XMLFUNC(fReboot);
XMLFUNC(fInformResponse);
XMLFUNC(fTransferCompleteResponse);
XMLFUNC(fGetRPCMethodsResponse);
#ifdef CONFIG_RPCUPLOAD
XMLFUNC(fUpload);
#endif
#ifdef CONFIG_RPCFACTORYRESET
XMLFUNC(fFactoryReset);
#endif

#ifdef CONFIG_RPCGETQUEUEDTRANSFERS
XMLFUNC(fGetQueuedTransfers);
#endif
#ifdef CONFIG_RPCGETALLQUEUEDTRANSFERS
XMLFUNC(fGetAllQueuedTransfers);
#endif

#ifdef CONFIG_RPCKICK
XMLFUNC(fKickedResponse);
XMLFUNC(fKickedNextURL);
static XmlNodeDesc kickedRespDesc[] = {
    {iDEFAULT, "NextURL", fKickedNextURL, NULL},
    {NULL,NULL}
};
#endif

#ifdef CONFIG_RPCREQUESTDOWNLOAD
XMLFUNC(fRequestDownloadResponse);
#endif
#ifdef CONFIG_RPCAUTONOMOUSTRANSFERCOMPLETE
XMLFUNC(fAutonomousTransferCompleteResponse);
#endif
#ifdef CONFIG_RPCSETVOUCHERS
XMLFUNC(fVoucherList);
XMLFUNC(fOptionName);
XMLFUNC(fSetVouchers);
XMLFUNC(fGetOptions);
static XmlNodeDesc voucherListDesc[] = {
    {iSOAP, "arrayType", fParameterNameArraySize, NULL},
    {iXSD,  "base64", fVoucherList, NULL },    /* name value is in TAGDATA */
    {NULL,  "VoucherList", fVoucherList, NULL}, /* Not part of schema: !!!!!!! kludge for KT ACS */
    {NULL, NULL}                                     /* !!!!!!!!!!!!!!!!!!!*/
};
static XmlNodeDesc setVouchersDesc[] = {
    {iDEFAULT, "VoucherList", NULL, voucherListDesc},
    {NULL,NULL}
};
static XmlNodeDesc getOptionsDesc[] = {
    {iDEFAULT, "OptionName", fOptionName, NULL},
    {NULL,NULL}
};
#endif

#ifdef CONFIG_RPCSCHEDULEDOWNLOAD
XMLFUNC(fScheduleDownload);
XMLFUNC(fCancelTransfer);
#endif

static XmlNodeDesc bodyDesc[] = {
    {iSOAP, "Fault", fFault, faultDesc},
    {iDEFAULT, "InformResponse", fInformResponse, informResponseDesc},
    {iDEFAULT, "GetRPCMethods", fGetRPCMethods, NULL},
    {iDEFAULT, "GetParameterNames", fGetParameterNames, getParameterNamesDesc},
    {iDEFAULT, "GetParameterValues", fGetParameterValues, getParameterValuesDesc},
    {iDEFAULT, "SetParameterValues", fSetParameterValues, setParameterValuesDesc},
    {iDEFAULT, "GetParameterAttributes", fGetParameterAttributes, getParameterAttributesDesc},
    {iDEFAULT, "SetParameterAttributes", fSetParameterAttributes, setParameterAttributesDesc},
    {iDEFAULT, "TransferCompleteResponse", fTransferCompleteResponse, faultDesc},
    {iDEFAULT, "GetRPCMethodsResponse", fGetRPCMethodsResponse, NULL},	/* ignore content*/
    {iDEFAULT, "AddObject", fAddObject, addDelObjectDesc},
    {iDEFAULT, "DeleteObject", fDeleteObject, addDelObjectDesc},
#ifdef CONFIG_RPCDOWNLOAD
    {iDEFAULT, "Download", fDownload, downLoadDesc},
#endif
    {iDEFAULT, "Reboot", fReboot, rebootDesc},
#ifdef CONFIG_RPCFACTORYRESET
    {iDEFAULT, "FactoryReset", fFactoryReset, NULL},
#endif
#ifdef CONFIG_RPCUPLOAD
    {iDEFAULT, "Upload", fUpload, downLoadDesc},
#endif
#ifdef CONFIG_RPCSCHEDULEINFORM
    {iDEFAULT, "ScheduleInform", fScheduleInform, scheduleInformDesc},
#endif
#ifdef CONFIG_RPCGETQUEUEDTRANSFERS
    {iDEFAULT, "GetQueuedTransfers", fGetQueuedTransfers, NULL},
#endif
#ifdef CONFIG_RPCGETALLQUEUEDTRANSFERS
    {iDEFAULT, "GetAllQueuedTransfers", fGetAllQueuedTransfers, NULL},
#endif
#ifdef CONFIG_RPCKICK
    {iDEFAULT, "KickedResponse", fKickedResponse, kickedRespDesc},
#endif
#ifdef CONFIG_RPCREQUESTDOWNLOAD
    {iDEFAULT, "RequestDownloadResponse", fRequestDownloadResponse, NULL},
#endif
#ifdef CONFIG_RPCAUTONOMOUSTRANSFERCOMPLETE
    {iDEFAULT, "AutonomousTransferCompleteResponse", fAutonomousTransferCompleteResponse, NULL},
#endif
#ifdef CONFIG_RPCSETVOUCHERS
    {iDEFAULT, "SetVouchers", fSetVouchers, setVouchersDesc},
    {iDEFAULT, "GetOptions", fGetOptions, getOptionsDesc},
#endif
#ifdef CONFIG_RPCSCHEDULEDOWNLOAD
    {iDEFAULT, "ScheduleDownload", fScheduleDownload, downLoadDesc},
    {iDEFAULT, "CancelTransfer", fCancelTransfer, cancelTransferDesc},
#endif
#ifdef CONFIG_RPCCHANGEDUSTATE
    {iDEFAULT, "ChangeDUState", fChangeDUState, changeDUStateDesc},
    {iDEFAULT, "DUStateChangeCompleteResponse", fDUStateChangeCompleteResponse, NULL},
    {iDEFAULT, "AutonomousDUStateChangeCompleteResponse", fAutonmousDUStateChangeCompleteResponse, NULL},
#endif
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
    {iNULL, "Envelope", fEnvelope, envDesc}, /* -1 is namespace exception flag*/
    {NULL, NULL}
};

static XML_STATUS fFaultString(void *user, const char *name, TOKEN_TYPE ttype, const char *value) {
    RPCRequest *r = *(RPCRequest **)user;
    if (ttype == TAGDATA && r) {
    	FaultMsg *p = &r->ud.faultMsg;
    	if ( p->faultString==NULL )
    		p->faultString = GS_STRDUP(value);
    	else
    		return XML_STS_ERR;
    }
    return XML_STS_OK;
}
static XML_STATUS fFaultCode(void *user, const char *name, TOKEN_TYPE ttype, const char *value) {
    RPCRequest *r = *(RPCRequest **)user;
    if (ttype == TAGDATA && r)
        r->ud.faultMsg.faultCode = atoi(value);
    return XML_STS_OK;
}
static XML_STATUS fFault(void *user, const char *name, TOKEN_TYPE ttype, const char *value) {
    RPCRequest *r = *(RPCRequest **)user;
    if (ttype == TAGBEGIN && r)
        r->rpcMethod = eFault;
    return XML_STS_OK;
}

/*
* ScheduleInform
*/
#ifdef CONFIG_RPCSCHEDULEINFORM
static XML_STATUS fDelaySeconds(void *user, const char *name, TOKEN_TYPE ttype, const char *value) {
    RPCRequest *r = *(RPCRequest **)user;
    if (ttype == TAGDATA && r)
        r->ud.scheduleInformReq.delaySeconds = atoi(value);
    return XML_STS_OK;
}
static XML_STATUS fScheduleInform(void *user, const char *name, TOKEN_TYPE ttype, const char *value) {
    RPCRequest *r = *(RPCRequest **)user;
    if (ttype == TAGBEGIN && r)
        r->rpcMethod = eScheduleInform;
    return XML_STS_OK;
}

#endif /*CONFIG_RPCSCHEDULEINFORM*/

#ifdef CONFIG_RPCREQUESTDOWNLOAD
static XML_STATUS fRequestDownloadResponse(void *user, const char *name, TOKEN_TYPE ttype, const char *value) {
    RPCRequest *r = *(RPCRequest **)user;
    if (ttype == TAGBEGIN && r)
        r->rpcMethod = eRequestDownloadResponse;
    return XML_STS_OK;
}
#endif  /*  CONFIG_RPCREQUESTDOWNLOAD*/

#ifdef CONFIG_RPCGETQUEUEDTRANSFERS
static XML_STATUS fGetQueuedTransfers(void *user, const char *name, TOKEN_TYPE ttype, const char *value) {
    RPCRequest *r = *(RPCRequest **)user;
    if (ttype == TAGBEGIN && r)
        r->rpcMethod = eGetQueuedTransfers;
    return XML_STS_OK;
}
#endif
#ifdef CONFIG_RPCGETALLQUEUEDTRANSFERS
static XML_STATUS fGetAllQueuedTransfers(void *user, const char *name, TOKEN_TYPE ttype, const char *value) {
    RPCRequest *r = *(RPCRequest **)user;
    if (ttype == TAGBEGIN && r)
        r->rpcMethod = eGetAllQueuedTransfers;
    return XML_STS_OK;
}
#endif
/* <Download> / <Upload>
 */
#ifdef CONFIG_RPCUPLOAD
static XML_STATUS fUpload(void *user, const char *name, TOKEN_TYPE ttype, const char *value) {
    RPCRequest *r = *(RPCRequest **)user;
    if (ttype == TAGBEGIN && r)
        r->rpcMethod = eUpload;
    return XML_STS_OK;
}
#endif
#ifdef CONFIG_RPCDOWNLOAD
static XML_STATUS fDownload(void *user, const char *name, TOKEN_TYPE ttype, const char *value) {
    RPCRequest *r = *(RPCRequest **)user;
    if (ttype == TAGBEGIN && r)
        r->rpcMethod = eDownload;
    return XML_STS_OK;
}
#endif

#ifdef CONFIG_RPCSCHEDULEDOWNLOAD

/* <ScheduleDownload>
 *          ...
 *          <TimeWindowList>
 *             <TimeWindowStruct>
 *                <WindowStart>ttt</WindowStart>
 *                ...
 *             <TimeWindowStrut>
 *             	...
 */

static XML_STATUS fScheduleDownload(void *user, const char *name, TOKEN_TYPE ttype, const char *value) {
    RPCRequest *r = *(RPCRequest **)user;
    if (ttype == TAGBEGIN && r)
        r->rpcMethod = eScheduleDownload;
    else if (ttype == TAGEND )
    	r->ud.downloadReq.timeIndex = 0;   /* use timeIndex to count TimeWindowStruct in array */
    return XML_STS_OK;
}
static XML_STATUS fTimeWindowStruct(void *user, const char *name, TOKEN_TYPE ttype, const char *value) {
    RPCRequest *r = *(RPCRequest **)user;
    if (ttype == TAGEND && r)
        r->ud.downloadReq.timeIndex++;
    else if (ttype == TAGBEGIN && r->ud.downloadReq.timeIndex >= MAX_TIMEWINDOWS)
    	return XML_STS_ERR;         /* schema allows maximum of 2 TimeWindowStruct */
    return XML_STS_OK;
}
static XML_STATUS fTimeWindowList(void *user, const char *name, TOKEN_TYPE ttype, const char *value) {
    return XML_STS_OK;
}
static XML_STATUS fWindowStart(void *user, const char *name, TOKEN_TYPE ttype, const char *value) {
    RPCRequest *r = *(RPCRequest **)user;
    if (ttype == TAGDATA)
        r->ud.downloadReq.timeWindow[r->ud.downloadReq.timeIndex].windowStart = atoi(value);
    return XML_STS_OK;
}
static XML_STATUS fWindowEnd(void *user, const char *name, TOKEN_TYPE ttype, const char *value) {
    RPCRequest *r = *(RPCRequest **)user;
    if (ttype == TAGDATA)
        r->ud.downloadReq.timeWindow[r->ud.downloadReq.timeIndex].windowEnd = atoi(value);
    return XML_STS_OK;
}
static XML_STATUS fWindowMode(void *user, const char *name, TOKEN_TYPE ttype, const char *value) {
    RPCRequest *r = *(RPCRequest **)user;
    if (ttype == TAGDATA)
    	strncpy( r->ud.downloadReq.timeWindow[r->ud.downloadReq.timeIndex].windowMode, value, 65);
    return XML_STS_OK;
}
static XML_STATUS fMaxRetries(void *user, const char *name, TOKEN_TYPE ttype, const char *value) {
    RPCRequest *r = *(RPCRequest **)user;
    if (ttype == TAGDATA)
        r->ud.downloadReq.timeWindow[r->ud.downloadReq.timeIndex].maxRetries = atoi(value);
    return XML_STS_OK;
}
static XML_STATUS fUserMessage(void *user, const char *name, TOKEN_TYPE ttype, const char *value) {
    RPCRequest *r = *(RPCRequest **)user;
    if (ttype == TAGDATA)
        r->ud.downloadReq.timeWindow[r->ud.downloadReq.timeIndex].userMessage = strdup(value);
    return XML_STS_OK;
}
static XML_STATUS fCancelTransfer(void *user, const char *name, TOKEN_TYPE ttype, const char *value) {
    RPCRequest *r = *(RPCRequest **)user;
    if (ttype == TAGBEGIN && r)
        r->rpcMethod = eCancelTransfer;
    return XML_STS_OK;
}
#endif

static XML_STATUS fDownloadFileType(void *user, const char *name, TOKEN_TYPE ttype, const char *value) {
    RPCRequest *r = *(RPCRequest **)user;
    if (ttype == TAGDATA) {
    	if (r && r->ud.downloadReq.fileType==NULL )
    		r->ud.downloadReq.fileType = GS_STRDUP(value);
    	else
    		return XML_STS_ERR;
    }
    return XML_STS_OK;
}
static XML_STATUS fDownloadURL(void *user, const char *name, TOKEN_TYPE ttype, const char *value) {
    RPCRequest *r = *(RPCRequest **)user;
    if (ttype == TAGDATA ){
    	if (r && r->ud.downloadReq.URL==NULL )
    		r->ud.downloadReq.URL = GS_STRDUP(value);
    	else
    		return XML_STS_ERR;
    }
    return XML_STS_OK;
}
static XML_STATUS fDownloadUser(void *user, const char *name, TOKEN_TYPE ttype, const char *value) {
    RPCRequest *r = *(RPCRequest **)user;
    if (ttype == TAGDATA ){
        if ( r && r->ud.downloadReq.userName==NULL )
        	r->ud.downloadReq.userName = GS_STRDUP(value);
        else
        	return XML_STS_ERR;
    }
    return XML_STS_OK;
}
static XML_STATUS fDownloadPwd(void *user, const char *name, TOKEN_TYPE ttype, const char *value) {
    RPCRequest *r = *(RPCRequest **)user;
    if (ttype == TAGDATA ){
    	if ( r && r->ud.downloadReq.passWord==NULL)
    		r->ud.downloadReq.passWord = GS_STRDUP(value);
    	else
    		return XML_STS_ERR;
    }
    return XML_STS_OK;
}
static XML_STATUS fDownloadFileSize(void *user, const char *name, TOKEN_TYPE ttype, const char *value) {
    RPCRequest *r = *(RPCRequest **)user;
    if (ttype == TAGDATA && r)
        r->ud.downloadReq.fileSize = atoi(value);
    return XML_STS_OK;
}
static XML_STATUS fTargetFileName(void *user, const char *name, TOKEN_TYPE ttype, const char *value) {
    RPCRequest *r = *(RPCRequest **)user;
    if (ttype == TAGDATA){
    	if ( r && r->ud.downloadReq.targetFileName==NULL )
    		r->ud.downloadReq.targetFileName = GS_STRDUP(value);
    	else
    		return XML_STS_ERR;
    }
    return XML_STS_OK;
}
static XML_STATUS fDownloadDelaySec(void *user, const char *name, TOKEN_TYPE ttype, const char *value) {
    RPCRequest *r = *(RPCRequest **)user;
    if (ttype == TAGDATA && r)
        r->ud.downloadReq.delaySeconds = atoi(value);
    return XML_STS_OK;
}
static XML_STATUS fSuccessURL(void *user, const char *name, TOKEN_TYPE ttype, const char *value) {
    RPCRequest *r = *(RPCRequest **)user;
    if (ttype == TAGDATA){
    	if ( r && r->ud.downloadReq.successURL==NULL )
    		r->ud.downloadReq.successURL = GS_STRDUP(value);
    	else
    		return XML_STS_ERR;
    }
    return XML_STS_OK;
}
static XML_STATUS fFailureURL(void *user, const char *name, TOKEN_TYPE ttype, const char *value) {
    RPCRequest *r = *(RPCRequest **)user;
    if (ttype == TAGDATA){
    	if ( r && r->ud.downloadReq.failureURL==NULL )
    		r->ud.downloadReq.failureURL = GS_STRDUP(value);
    	else
    		return XML_STS_ERR;
    }
    return XML_STS_OK;
}
/* <xxx:InformResponse>
 *       <MaxEnvelopes>xx</MaxEnvelopes>
 * Assumes that we only get this in an InformResponse msg.
 */

static XML_STATUS fMaxEnvelopes(void *user, const char *name, TOKEN_TYPE ttype, const char *value) {
    RPCRequest *r = *(RPCRequest **)user;
    if (ttype == TAGDATA)
        r->maxEnvelopes= atoi(value);
    return XML_STS_OK;
}
/* <Reboot>
 */

static XML_STATUS fCommandKey(void *user, const char *name, TOKEN_TYPE ttype, const char *value) {
    RPCRequest *r = *(RPCRequest **)user;
    if (ttype == TAGDATA){
    	if ( r && strlen(value)< COMMANDKEY_SZ)
    		strcpy(r->commandKey, value);
    	else
    		return XML_STS_ERR;
    }
    return XML_STS_OK;
}
static XML_STATUS fReboot(void *user, const char *name, TOKEN_TYPE ttype, const char *value) {
    RPCRequest *r = *(RPCRequest **)user;
    if (r && ttype == TAGBEGIN)
        r->rpcMethod = eReboot;
    return XML_STS_OK;
}
/*
 * <FactoryReset/>
 */
#ifdef CONFIG_RPCFACTORYRESET
static XML_STATUS fFactoryReset(void *user, const char *name, TOKEN_TYPE ttype, const char *value) {
    RPCRequest *r = *(RPCRequest **)user;
    if (r && ttype == TAGBEGIN)
        r->rpcMethod = eFactoryReset;
    return XML_STS_OK;
}
#endif

static XML_STATUS fInformResponse(void *user, const char *name, TOKEN_TYPE ttype, const char *value) {
    RPCRequest *r = *(RPCRequest **)user;
    if (ttype == TAGBEGIN && r)
        r->rpcMethod = eInformResponse;
    return XML_STS_OK;
}
static XML_STATUS fTransferCompleteResponse(void *user, const char *name, TOKEN_TYPE ttype, const char *value) {
    RPCRequest *r = *(RPCRequest **)user;
    if (r && ttype == TAGBEGIN)
        r->rpcMethod = eTransferCompleteResponse;
	return XML_STS_OK;
}
/* <AddObject>
 *  <DeleteObject>
 *
 */

static XML_STATUS fObjectName(void *user, const char *name, TOKEN_TYPE ttype, const char *value) {
    RPCRequest *r = *(RPCRequest **)user;
    if (ttype == TAGDATA){
    	if (r && r->ud.addDelObjectReq.objectPath==NULL )
    		r->ud.addDelObjectReq.objectPath = GS_STRDUP(value);
    	else
    		return XML_STS_ERR;
    }
    return XML_STS_OK;
}

static XML_STATUS fAddObject(void *user, const char *name, TOKEN_TYPE ttype, const char *value) {
    RPCRequest *r = *(RPCRequest **)user;
    if (ttype == TAGBEGIN && r)
        r->rpcMethod = eAddObject;
    return XML_STS_OK;
}
static XML_STATUS fDeleteObject(void *user, const char *name, TOKEN_TYPE ttype, const char *value) {
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
static XML_STATUS fSetParameterValueStruct(void *user, const char *name, TOKEN_TYPE ttype, const char *value) {
    RPCRequest *r = *(RPCRequest **)user;
    if (ttype == TAGBEGIN && r ){
        ParameterValueStruct *p = (ParameterValueStruct *)GS_MALLOC(sizeof (struct ParameterValueStruct));
        if (p) { /* link new item to RPCRequest */
            memset(p, 0, sizeof(ParameterValueStruct));
            ++r->ud.setPValuesReq.paramCnt;
            p->next = r->ud.setPValuesReq.pvList;
            r->ud.setPValuesReq.pvList =  p;
        } else
            return XML_STS_MEM;
    }
    return XML_STS_OK;
}
static XML_STATUS fParameterValue(void *user, const char *name, TOKEN_TYPE ttype, const char *value) {
    RPCRequest *r = *(RPCRequest **)user;
    if (ttype == TAGDATA && r ){
    	ParameterValueStruct *p = r->ud.setPValuesReq.pvList;
        if( p!=NULL && p->value==NULL )
            p->value = GS_STRDUP(value);
        else
        	return XML_STS_ERR;
    }
    return XML_STS_OK;
}
static XML_STATUS fSetParameterKey(void *user, const char *name, TOKEN_TYPE ttype, const char *value) {
    RPCRequest *r = *(RPCRequest **)user;
    if (ttype == TAGDATA && r) {
    	if ( strlen(value)<PARAMETERKEY_SZ)
    		strcpy(r->parameterKey, value);
    	else
    		return XML_STS_ERR;      /* key length error */
    }
    return XML_STS_OK;
}
static XML_STATUS fSetParameterValues(void *user, const char *name, TOKEN_TYPE ttype, const char *value) {
    RPCRequest *r = *(RPCRequest **)user;
    if (ttype == TAGBEGIN && r)
        r->rpcMethod = eSetParameterValues;
    else if (ttype == TAGEND ){
    	if ( r->arrayTypeSize>0 && r->ud.setPValuesReq.pvList==NULL)
    		return XML_STS_ERR;
    	/* don't test much here since some ACS don't send array size */
    }
    return XML_STS_OK;
}

static XML_STATUS fNames(void *user, const char *name, TOKEN_TYPE ttype, const char *value) {
    RPCRequest *r = *(RPCRequest **)user;
    if (ttype==TAGDATA && r) {
        ParameterValueStruct *p = r->ud.setPValuesReq.pvList;
        if ( p!=NULL && p->pName==NULL  )
        	p->pName = GS_STRDUP(value);
        else
        	return XML_STS_ERR;  /* illegal ParameterValueStruct */
    }
    return XML_STS_OK;
}
/* <ns:GetParameterValues ....../>
 *
 */
/* generic for multiple rpc messages: set arrayTypeSize=n  from the    enc:arrayType="cwmp:XXXXX[n]">  */
static XML_STATUS fParameterNameArraySize(void *user, const char *name, TOKEN_TYPE ttype, const char *value) {
    RPCRequest *r = *(RPCRequest **)user;
    if (ttype == ATTRIBUTEVALUE && r ){
        char *s;
        s = strchr(value, '[');
        if (s)
            r->arrayTypeSize = atoi(s+1);
    }
    return XML_STS_OK;
}

static int tagBeginFlg;	/* used to detect empty parameter names, such as: */
/*  <string></string>                             */
static XML_STATUS fParameterNames(void *user, const char *name, TOKEN_TYPE ttype, const char *value) {
    RPCRequest *r = *(RPCRequest **)user;
    if ( (ttype==TAGDATA || (ttype==TAGEND && tagBeginFlg)) && r) {
        ParameterNameStruct *p = (ParameterNameStruct *)GS_MALLOC(sizeof (ParameterNameStruct));
        if (p) {
            memset(p, 0, sizeof(ParameterNameStruct));
            if (value)
                if ( (p->pName = GS_STRDUP(value)) == NULL )
                	return XML_STS_MEM;
            p->next = r->ud.getPAVReq.pnameList;
            r->ud.getPAVReq.pnameList = p;
            ++r->ud.getPAVReq.paramCnt;
            tagBeginFlg = 0;
        } else
            return XML_STS_MEM;
    } else if (ttype==TAGBEGIN) {
        tagBeginFlg = 1;
    } else
        tagBeginFlg = 0;
    return XML_STS_OK;
}

static XML_STATUS fGetParameterValues(void *user, const char *name, TOKEN_TYPE ttype, const char *value) {
    RPCRequest *r = *(RPCRequest **)user;
    if (ttype == TAGBEGIN && r )
        r->rpcMethod = eGetParameterValues;
    return XML_STS_OK;
}

/* <SetParameterAttributes>
 *  <ParameterList <SetParameterAttributesStruct>
 *               <Name>xxx</Name>  ....
 */
static XML_STATUS fAccessList(void *user, const char *name, TOKEN_TYPE ttype, const char *value) {   /* there's only one access list item today */
    RPCRequest *r = *(RPCRequest **)user;
    if (ttype == TAGDATA && r){
        if (r->ud.setPAttrReq.paList) {
            r->ud.setPAttrReq.paList->accessList = !strcasecmp(value, "subscriber");
            return XML_STS_OK;
        }
        return XML_STS_ERR;
    }
    return XML_STS_OK;
}
static XML_STATUS fAcclistChange(void *user, const char *name, TOKEN_TYPE ttype, const char *value) {
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
static XML_STATUS fNotificationChange(void *user, const char *name, TOKEN_TYPE ttype, const char *value) {
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
static XML_STATUS fNotification(void *user, const char *name, TOKEN_TYPE ttype, const char *value) {
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
static XML_STATUS fSetAttParameterName(void *user, const char *name, TOKEN_TYPE ttype, const char *value) {
    RPCRequest *r = *(RPCRequest **)user;
    if (ttype == TAGDATA && r){
    	ParameterAttributesStruct *p = r->ud.setPAttrReq.paList;
        if (p && p->pName==NULL) {
            p->pName = GS_STRDUP(value);
        } else
        	return XML_STS_ERR;
    }
    return XML_STS_OK;
}

/*
 * This queuing function is used to place the new item at the end of the list.
 * Required as the spec says that the order that attributes are applied must be
 * the same as the order in the soap message.
 */
static void qAttributeReq(ParameterAttributesStruct **qHdr, ParameterAttributesStruct *r) {
    ParameterAttributesStruct **last = qHdr;
    ParameterAttributesStruct *p;
    while ( *last ) {
        p = *last;
        last = &p->next;
    }
    *last = r;
    r->next = NULL;
}

static XML_STATUS fSetParameterAttributesList(void *user, const char *name, TOKEN_TYPE ttype, const char *value) {
    RPCRequest *r = *(RPCRequest **)user;
    ParameterAttributesStruct *pp;
    if (ttype == TAGBEGIN && r){
        ParameterAttributesStruct *p = (ParameterAttributesStruct *)GS_MALLOC(sizeof (ParameterAttributesStruct));
        if (p) {
            /* enqueue at beginning of list so that  fxxx above work */
            memset(p, 0, sizeof(ParameterAttributesStruct));
            ++r->ud.setPAttrReq.paramCnt;
            p->next = r->ud.setPAttrReq.paList;
            r->ud.setPAttrReq.paList = p;
            return XML_STS_OK;
        }
        return XML_STS_MEM;
    } else if (ttype == TAGEND && r && (pp = r->ud.setPAttrReq.paList) ) {
        /* move the item at paList to the end of the list if more than one item */
        if ( pp->next ) {
            r->ud.setPAttrReq.paList = pp->next;
            pp->next = NULL;
            qAttributeReq( &r->ud.setPAttrReq.paList, pp);
        }
    }
    return XML_STS_OK;
}

static XML_STATUS fSetParameterAttributes(void *user, const char *name, TOKEN_TYPE ttype, const char *value) {
    RPCRequest *r = *(RPCRequest **)user;
    if (ttype == TAGBEGIN && r)
        r->rpcMethod = eSetParameterAttributes;
    return XML_STS_OK;
}
/* <GetParameterAttributes>
 *  <ParameterNames SOAP-ENC:arrayType="xsd:string[xx]"
 *               <xsd:string>xxx</xsd:string>  ....
 */
static XML_STATUS fGetParameterAttributes(void *user, const char *name, TOKEN_TYPE ttype, const char *value) {
    RPCRequest *r = *(RPCRequest **)user;
    if (ttype == TAGBEGIN && r)
        r->rpcMethod = eGetParameterAttributes;
    return XML_STS_OK;
}


/* <ns:GetParameterNames>
 *     <ParameterPath>parameterpath</ParameterPath>
 *  Only one parameterPath is allowed in msg.
 */
static XML_STATUS fGetParameterNames(void *user, const char *name, TOKEN_TYPE ttype, const char *value) {
    RPCRequest *r = *(RPCRequest **)user;
    if (ttype == TAGBEGIN && r )
        r->rpcMethod = eGetParameterNames;
    return XML_STS_OK;
}
static XML_STATUS fParameterPath(void *user, const char *name, TOKEN_TYPE ttype, const char *value) {
    RPCRequest *r = *(RPCRequest **)user;
    if (ttype == TAGDATA ){
    	if (r && r->ud.getPNamesReq.pNamePath == NULL )
    		r->ud.getPNamesReq.pNamePath = GS_STRDUP(value);
    	else
    		return XML_STS_ERR;
    }
    return XML_STS_OK;
}
static XML_STATUS fNextLevel(void *user, const char *name, TOKEN_TYPE ttype, const char *value) {
    RPCRequest *r = *(RPCRequest **)user;
    if (ttype == TAGDATA && r)
        r->ud.getPNamesReq.nextLevel = testBoolean(value);
    return XML_STS_OK;
}

/* <KickedResponse */

#ifdef CONFIG_RPCKICK

static XML_STATUS fKickedNextURL(void *user, const char *name, TOKEN_TYPE ttype, const char *value) {
    RPCRequest *r = *(RPCRequest **)user;
    if (ttype == TAGDATA && r){
        if ( r->ud.kickedResponseReq.nextURL)
            GS_FREE(r->ud.kickedResponseReq.nextURL);
        r->ud.kickedResponseReq.nextURL = GS_STRDUP(value);
    }
    return XML_STS_OK;
}

static XML_STATUS fKickedResponse(void *user, const char *name, TOKEN_TYPE ttype, const char *value) {
    RPCRequest *r = *(RPCRequest **)user;
    if (ttype == TAGBEGIN && r ){
        r->rpcMethod = eKickedResponse;
    }
    return XML_STS_OK;
}
#endif
#ifdef CONFIG_RPCAUTONOMOUSTRANSFERCOMPLETE
static XML_STATUS fAutonomousTransferCompleteResponse(void *user, const char *name, TOKEN_TYPE ttype, const char *value) {
    RPCRequest *r = *(RPCRequest **)user;
    if (ttype == TAGBEGIN && r )
        r->rpcMethod = eAutonomousTransferCompleteResponse;
    return XML_STS_OK;
}
#endif

#ifdef CONFIG_RPCSETVOUCHERS
/* <SetVouchers ... */

/*  <base64></base64>                             */
static XML_STATUS fVoucherList(void *user, const char *name, TOKEN_TYPE ttype, const char *value) {
    RPCRequest *r = *(RPCRequest **)user;
    if ( (ttype==TAGDATA || (ttype==TAGEND && tagBeginFlg)) && r) {
        Voucher *p = (Voucher *)GS_MALLOC(sizeof (Voucher));
        if (p) {
            memset(p, 0, sizeof(Voucher));
            if (value)
                p->pVoucher = GS_STRDUP(value);
            p->next = r->ud.setVouchersReq.voucherList;
            r->ud.setVouchersReq.voucherList = p;
            ++r->ud.setVouchersReq.voucherCnt;
            tagBeginFlg = 0;
        } else
            return XML_STS_MEM;
    } else if (ttype==TAGBEGIN) {
        tagBeginFlg = 1;
    } else
        tagBeginFlg = 0;
    return XML_STS_OK;
}

static XML_STATUS fSetVouchers(void *user, const char *name, TOKEN_TYPE ttype, const char *value) {
    RPCRequest *r = *(RPCRequest **)user;
    if (ttype == TAGBEGIN && r )
        r->rpcMethod = eSetVouchers;
    return XML_STS_OK;
}

static XML_STATUS fOptionName(void *user, const char *name, TOKEN_TYPE ttype, const char *value) {
    RPCRequest *r = *(RPCRequest **)user;
    if (ttype == TAGDATA && r )
    	if (ud.getOptionsReq.pOptionName == NULL )
    		r->ud.getOptionsReq.pOptionName = GS_STRDUP(value);
    	else
    		return XML_STS_ERR;
    return XML_STS_OK;
}
static XML_STATUS fGetOptions(void *user, const char *name, TOKEN_TYPE ttype, const char *value) {
    RPCRequest *r = *(RPCRequest **)user;
    if (ttype == TAGBEGIN && r )
        r->rpcMethod = eGetOptions;
    return XML_STS_OK;
}

#endif
/* <SOAP_ENV:Body ..../> */


static XML_STATUS fGetRPCMethods(void *user, const char *name, TOKEN_TYPE ttype, const char *value) {
    RPCRequest *r = *(RPCRequest **)user;
    if (ttype == TAGBEGIN && r )
        r->rpcMethod = eGetRPCMethods;
    return XML_STS_OK;
}


static XML_STATUS fGetRPCMethodsResponse(void *user, const char *name, TOKEN_TYPE ttype, const char *value) {
    RPCRequest *r = *(RPCRequest **)user;
    if (ttype == TAGBEGIN && r )
        r->rpcMethod = eGetRPCMethodsResponse;
    return XML_STS_OK;
}

static XML_STATUS fHoldRequest(void *user, const char *name, TOKEN_TYPE ttype, const char *value) {
    RPCRequest *r = *(RPCRequest **)user;
    if (ttype == TAGDATA)
        r->holdRequests = !strcmp(value, "1");

    return XML_STS_OK;
}
static XML_STATUS fIDValue(void *user, const char *name, TOKEN_TYPE ttype, const char *value) {
    RPCRequest *r = *(RPCRequest **)user;
    if (ttype == TAGDATA) {
        if (r && r->ID==NULL)
            r->ID = GS_STRDUP(value);
        else
            return XML_STS_ERR;
    }
    return XML_STS_OK;
}
/* <envelope .../>   */
static XML_STATUS fEnvelope(void *user, const char *name, TOKEN_TYPE ttype, const char *value) {
    RPCRequest **req = (RPCRequest **)user;
    if (req) {
        if (ttype == TAGBEGIN) {
            DBGPRINT((stderr, "fEnvelope() TAGBEGIN ------\n"));
            /* allocate a new RPCRequest to hold request */
            if ( (*req=(RPCRequest *)GS_MALLOC(sizeof(struct RPCRequest))) )
                memset(*req, 0, sizeof(struct RPCRequest));
            else
            	return XML_STS_MEM;
        } else {
            DBGPRINT((stderr, "fEnvelope() TAGEND -----\n"));
            /* end of envelope -- now run RPC */
            /* */
        }
        return XML_STS_OK;
    }
    return XML_STS_ERR;  /* no user context pointer */
}



