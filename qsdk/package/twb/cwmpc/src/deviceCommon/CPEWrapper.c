/*----------------------------------------------------------------------*
 * Gatespace Networks, Inc.
 * Copyright 2009-2011 Gatespace Networks, Inc., All Rights Reserved.
 * Gatespace Networks, Inc. confidential material.
 *----------------------------------------------------------------------*
 * File Name  : CPEWrapper.c
 *
 * Description: Initial CPE dependent data:
 *              Specific to a Linux OS.
 *
 * $Revision: 1.3 $
 * $Id: CPEWrapper.c,v 1.3 2012/06/13 11:04:41 dmounday Exp $
 *----------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <time.h>
#include <string.h>
#include <netinet/in.h>
#include <errno.h>
#include <netdb.h>
#include <syslog.h>
#include <ctype.h>
#ifdef DMALLOC
#include "dmalloc.h"
#endif
#include <string.h>
#include "../includes/sys.h"
#include "../includes/paramTree.h"
#include "../includes/rpc.h"
#include "../soapRpc/rpcMethods.h"
#include "../soapRpc/rpcUtils.h"
#include "../soapRpc/notify.h"
#include "../includes/CPEWrapper.h"
#include "../gslib/src/utils.h"
#include "../gslib/src/xmlParserSM.h"
#include "../gslib/src/xmlWriter.h"
#include "targetsys.h"
#include "runstates.h"
#include "sysutils.h"
#include "netIPState.h"

#ifdef DEBUG
#define DBGPRINT(X) fprintf X
#else
#define DBGPRINT(X)
#endif
extern CWMPObject CWMP_RootObject[];
extern CWMPObject Services_Objs[];
extern CPEState	cpeState;

extern EthernetLink *cpeNetIF;

/*
* These are xml parser callback functions that are used to
* restore the cpeState structure values on a reboot.
*/
static XML_STATUS feventMask(void *user, const char *name, TOKEN_TYPE ttype, const char *value)
{
    if (ttype == TAGDATA)
        cpeState.eventMask = strtol(value, NULL, 16);
    return XML_STS_OK;
}
static XML_STATUS facsState(void *user, const char *name, TOKEN_TYPE ttype, const char *value)
{
    if (ttype == TAGDATA)
        cpeState.acsContactState = atoi(value);
    return XML_STS_OK;
}
static XML_STATUS frebootCommandKey(void *user, const char *name, TOKEN_TYPE ttype, const char *value)
{
    if (ttype == TAGDATA)
        strcpy(cpeState.rebootCommandKey, value);
    return XML_STS_OK;
}
static XML_STATUS fparameterKey(void *user, const char *name, TOKEN_TYPE ttype, const char *value)
{
    if (ttype == TAGDATA)
    	strcpy( cpeState.parameterKey, value);
    return XML_STS_OK;
}

static XML_STATUS facsURL(void *user, const char *name, TOKEN_TYPE ttype, const char *value)
{
    if (ttype == TAGDATA)
    	COPYSTR(cpeState.acsURL,value);

    return XML_STS_OK;
}
static XML_STATUS facsUser(void *user, const char *name, TOKEN_TYPE ttype, const char *value)
{
    if (ttype == TAGDATA)
    	COPYSTR(cpeState.acsUser,value);
    return XML_STS_OK;
}
static XML_STATUS facsPW(void *user, const char *name, TOKEN_TYPE ttype, const char *value)
{
    if (ttype == TAGDATA)
    	COPYSTR(cpeState.acsPW,value);
    return XML_STS_OK;
}
/*
static XML_STATUS fconnReqURL(void *user, const char *name, TOKEN_TYPE ttype, const char *value)
{
    if (ttype == TAGDATA)
        COPYSTR( cpeState.connReqURL, value);
    return XML_STS_OK;
}
*/
static XML_STATUS fconnReqUser(void *user, const char *name, TOKEN_TYPE ttype, const char *value)
{
    if (ttype == TAGDATA)
    	COPYSTR(cpeState.connReqUser,value);
    return XML_STS_OK;
}
static XML_STATUS fconnReqPW(void *user, const char *name, TOKEN_TYPE ttype, const char *value)
{
    if (ttype == TAGDATA)
    	COPYSTR(cpeState.connReqPW,value);
    return XML_STS_OK;
}
static XML_STATUS finformEnabled(void *user, const char *name, TOKEN_TYPE ttype, const char *value)
{
    if (ttype == TAGDATA)
        cpeState.informEnabled = atoi(value);
    return XML_STS_OK;
}
static XML_STATUS finformInterval(void *user, const char *name, TOKEN_TYPE ttype, const char *value)
{
    if (ttype == TAGDATA)
        cpeState.informInterval = atoi(value);
    return XML_STS_OK;
}
static XML_STATUS finformTime(void *user, const char *name, TOKEN_TYPE ttype, const char *value)
{
    if (ttype == TAGDATA)
        cpeState.informTime = atoi(value);
    return XML_STS_OK;
}
static XML_STATUS fupgradesManaged(void *user, const char *name, TOKEN_TYPE ttype, const char *value)
{
    if (ttype == TAGDATA)
        cpeState.upgradesManaged = atoi(value);
    return XML_STS_OK;
}
#ifdef CONFIG_RPCKICK
static XML_STATUS fkickURL(void *user, const char *name, TOKEN_TYPE ttype, const char *value)
{
    if (ttype == TAGDATA)
    	COPYSTR(cpeState.kickURL,value);
    return XML_STS_OK;
}
#endif

static XML_STATUS finstanceID(void *user, const char *name, TOKEN_TYPE ttype, const char *value)
{
    if (ttype == TAGDATA)
        cpeState.instanceID = atoi(value);
    return XML_STS_OK;
}
static XML_STATUS facshost(void *user, const char *name, TOKEN_TYPE ttype, const char *value)
{
    if (ttype == TAGDATA)
        COPYSTR( cpeState.acsHostName, value);
    return XML_STS_OK;
}
static XML_STATUS facsIPAddress(void *user, const char *name, TOKEN_TYPE ttype, const char *value)
{
    if (ttype == TAGDATA)
        readInIPAddr( &cpeState.acsIPAddress, value);
    return XML_STS_OK;
}
static XML_STATUS fdefaultACSURL(void *user, const char *name, TOKEN_TYPE ttype, const char *value)
{
    if (ttype == TAGDATA)
        COPYSTR( cpeState.defaultACSURL, value);
    return XML_STS_OK;
}
/*
 * added for V1.2
 */
static XML_STATUS fcwmpRetryMin(void *user, const char *name, TOKEN_TYPE ttype, const char *value)
{
    if (ttype == TAGDATA)
        cpeState.cwmpRetryMin = atoi(value);
    return XML_STS_OK;
}
static XML_STATUS fcwmpRetryMultiplier(void *user, const char *name, TOKEN_TYPE ttype, const char *value)
{
    if (ttype == TAGDATA)
        cpeState.cwmpRetryMultiplier = atoi(value);
    return XML_STS_OK;
}
/*
 * ************************************************
 * Conditional code for TR111 STUN parameters
 */
#ifdef CONFIG_TR111P2
#include "../tr111/tr111p2.h"

extern StunState stunState;
static XML_STATUS fconnReqNotifyLimit(void *user, const char *name, TOKEN_TYPE ttype, const char *value)
{
    if (ttype == TAGDATA)
        stunState.minNotifyLimit = atoi(value);
    return XML_STS_OK;
}
static XML_STATUS fSTUNEnable(void *user, const char *name, TOKEN_TYPE ttype, const char *value)
{
    if (ttype == TAGDATA)
        stunState.enable = testBoolean(value);
    return XML_STS_OK;
}
static XML_STATUS fSTUNServerAddress(void *user, const char *name, TOKEN_TYPE ttype, const char *value)
{
    if (ttype == TAGDATA)
    	COPYSTR(stunState.serverAddr,value);
    return XML_STS_OK;
}
static XML_STATUS fSTUNServerPort(void *user, const char *name, TOKEN_TYPE ttype, const char *value)
{
    if (ttype == TAGDATA)
        stunState.serverPort = atoi(value);
    return XML_STS_OK;
}
static XML_STATUS fSTUNUsername(void *user, const char *name, TOKEN_TYPE ttype, const char *value)
{
    if (ttype == TAGDATA)
    	COPYSTR(stunState.username, value);
    return XML_STS_OK;
}
static XML_STATUS fSTUNPassword(void *user, const char *name, TOKEN_TYPE ttype, const char *value)
{
    if (ttype == TAGDATA)
    	COPYSTR(stunState.password, value);
    return XML_STS_OK;
}
static XML_STATUS fSTUNMaxKeepAlive(void *user, const char *name, TOKEN_TYPE ttype, const char *value)
{
    if (ttype == TAGDATA)
        stunState.maxKeepAlive = atoi(value);
    return XML_STS_OK;
}
static XML_STATUS fSTUNMinKeepAlive(void *user, const char *name, TOKEN_TYPE ttype, const char *value)
{
    if (ttype == TAGDATA)
        stunState.minKeepAlive = atoi(value);
    return XML_STS_OK;
}
#endif /* CONFIG_TR111P2 */

#if defined(CONFIG_RPCUPLOAD) || defined(CONFIG_RPCDOWNLOAD) || defined(CONFIG_RPCSCHEDULEDOWNLOAD)
static RPCRequest *rpcReq;

static XML_STATUS fCommandKey(void *user, const char *name, TOKEN_TYPE ttype,
		const char *value) {
	if (ttype == TAGDATA ) {
		if ( strlen(value)<COMMANDKEY_SZ)
			strcpy(rpcReq->commandKey, value);
		else
			rpcReq->commandKey[0] = '\0';	/* discard value */
	}
	return XML_STS_OK;
}
static XML_STATUS fFileType(void *user, const char *name, TOKEN_TYPE ttype,
		const char *value) {
	if (ttype == TAGDATA)
		rpcReq->ud.downloadReq.fileType = GS_STRDUP(value);
	return XML_STS_OK;
}
static XML_STATUS fURL(void *user, const char *name, TOKEN_TYPE ttype,
		const char *value) {
	if (ttype == TAGDATA)
		rpcReq->ud.downloadReq.URL = GS_STRDUP(value);
	return XML_STS_OK;
}
static XML_STATUS fUsername(void *user, const char *name, TOKEN_TYPE ttype,
		const char *value) {
	if (ttype == TAGDATA)
		rpcReq->ud.downloadReq.userName = GS_STRDUP(value);
	return XML_STS_OK;
}
static XML_STATUS fPassword(void *user, const char *name, TOKEN_TYPE ttype,
		const char *value) {
	if (ttype == TAGDATA)
		rpcReq->ud.downloadReq.passWord = GS_STRDUP(value);
	return XML_STS_OK;
}
static XML_STATUS fFilesize(void *user, const char *name, TOKEN_TYPE ttype,
		const char *value) {
	if (ttype == TAGDATA)
		rpcReq->ud.downloadReq.fileSize = atoi(value);
	return XML_STS_OK;
}
static XML_STATUS fTargetFileName(void *user, const char *name, TOKEN_TYPE ttype,
		const char *value) {
	if (ttype == TAGDATA)
		rpcReq->ud.downloadReq.targetFileName = GS_STRDUP(value);
	return XML_STS_OK;
}
static XML_STATUS fDelaySeconds(void *user, const char *name, TOKEN_TYPE ttype,
		const char *value) {
	if (ttype == TAGDATA)
		rpcReq->ud.downloadReq.delaySeconds = atoi(value);
	return XML_STS_OK;
}
static XML_STATUS ftargetTime(void *user, const char *name, TOKEN_TYPE ttype,
		const char *value) {
	if (ttype == TAGDATA)
		rpcReq->DLMsg.targetTime = atoi(value);
	return XML_STS_OK;
}
static XML_STATUS fFailureURL(void *user, const char *name, TOKEN_TYPE ttype,
		const char *value) {
	if (ttype == TAGDATA)
		rpcReq->ud.downloadReq.failureURL = GS_STRDUP(value);
	return XML_STS_OK;
}
static XML_STATUS fdlStartTime(void *user, const char *name, TOKEN_TYPE ttype,
		const char *value) {
	if (ttype == TAGDATA)
		rpcReq->DLMsg.dlStartTime = atoi(value);
	return XML_STS_OK;
}
static XML_STATUS fdlEndTime(void *user, const char *name, TOKEN_TYPE ttype,
		const char *value) {
	if (ttype == TAGDATA)
		rpcReq->DLMsg.dlEndTime = atoi(value);
	return XML_STS_OK;
}

void cwmpAddEvent( eEventCode event );

static XML_STATUS fdlStatus(void *user, const char *name, TOKEN_TYPE ttype,
		const char *value) {
	if (ttype == TAGDATA) {
		rpcReq->DLMsg.dlStatus = atoi(value);
		/* update the EVENT FLAGS */
		if ( rpcReq->DLMsg.dlStatus!=2 && !(rpcReq->DLMsg.dlStatus<0) ){
			cwmpAddEvent( rpcReq->rpcMethod == eDownload? eEvtMDownload:
			    rpcReq->rpcMethod== eScheduleDownload? eEvtMScheduleDownload: eEvtMUpload );
			cwmpAddEvent(eEvtTransferComplete);
		}
	}
	return XML_STS_OK;
}
static XML_STATUS fdlFaultMsg(void *user, const char *name, TOKEN_TYPE ttype,
		const char *value) {
	if (ttype == TAGDATA)
		rpcReq->DLMsg.dlFaultMsg = atoi(value);
	return XML_STS_OK;
}
static XML_STATUS fupdownload( eRPCMethods rpc){
	if (rpcReq == NULL){
		rpcReq = GS_MALLOC(sizeof(RPCRequest));
		if ( rpcReq == NULL)
			return XML_STS_MEM;
	}
	memset(rpcReq, 0, sizeof(RPCRequest));
	rpcReq->rpcMethod = rpc;
	return XML_STS_OK;
}

void cwmpQueueDLRequest(RPCRequest *, int Q);

static XML_STATUS fDownload(void *user, const char *name, TOKEN_TYPE ttype,
		const char *value) {
	if (ttype == TAGBEGIN) {
		return fupdownload( eDownload );
	}
	if (ttype == TAGEND){
		cwmpQueueDLRequest(rpcReq, rpcReq->DLMsg.dlStatus == -2? WAITINGQ: ACTIVEQ );
		rpcReq = NULL;
	}

	return XML_STS_OK;
}
static XML_STATUS fUpload(void *user, const char *name, TOKEN_TYPE ttype,
		const char *value) {
	if (ttype == TAGBEGIN) {
		return fupdownload( eUpload );
	}
	if (ttype == TAGEND){
		cwmpQueueDLRequest(rpcReq, rpcReq->DLMsg.dlStatus == -2? WAITINGQ: ACTIVEQ );
		rpcReq = NULL;
	}
	return XML_STS_OK;
}
#ifdef CONFIG_RPCSCHEDULEDOWNLOAD
static XML_STATUS fdlWinStart(void *user, const char *name, TOKEN_TYPE ttype,
		const char *value) {
	if (ttype == TAGDATA)
		rpcReq->DLMsg.timeWindow[rpcReq->DLMsg.timeIndex].windowStart = atoi(value);
	return XML_STS_OK;
}
static XML_STATUS fdlWinEnd(void *user, const char *name, TOKEN_TYPE ttype,
		const char *value) {
	if (ttype == TAGDATA)
		rpcReq->DLMsg.timeWindow[rpcReq->DLMsg.timeIndex].windowEnd = atoi(value);
	return XML_STS_OK;
}
static XML_STATUS fdlWinMode(void *user, const char *name, TOKEN_TYPE ttype,
		const char *value) {
	if (ttype == TAGDATA)
		strncpy( rpcReq->DLMsg.timeWindow[rpcReq->DLMsg.timeIndex].windowMode, value, 65);
	return XML_STS_OK;
}
static XML_STATUS fdlWinUsrMsg(void *user, const char *name, TOKEN_TYPE ttype,
		const char *value) {
	if (ttype == TAGDATA)
		rpcReq->DLMsg.timeWindow[rpcReq->DLMsg.timeIndex].userMessage = strdup(value);
	return XML_STS_OK;
}
static XML_STATUS fdlWinMaxRetries(void *user, const char *name, TOKEN_TYPE ttype,
		const char *value) {
	if (ttype == TAGDATA)
		rpcReq->DLMsg.timeWindow[rpcReq->DLMsg.timeIndex].maxRetries = atoi(value);
	else if ( ttype == TAGEND)
		rpcReq->DLMsg.timeIndex++;  // order of tags is important.
	return XML_STS_OK;
}
static XML_STATUS fScheduleDownload(void *user, const char *name, TOKEN_TYPE ttype,
		const char *value) {
	if (ttype == TAGBEGIN) {
		return fupdownload( eScheduleDownload );
	}
	if (ttype == TAGEND){
		rpcReq->DLMsg.timeIndex = 0; // reset to 0
		cwmpQueueDLRequest(rpcReq, rpcReq->DLMsg.dlStatus == -2? WAITINGQ: ACTIVEQ );
		rpcReq = NULL;
	}

	return XML_STS_OK;
}
#endif

static XML_STATUS fAutoCreateInstance(void *user, const char *name, TOKEN_TYPE ttype,
		const char *value) {
	if (ttype == TAGDATA)
		cpeState.autoCreateInstances = testBoolean(value);
	return XML_STS_OK;
}
static XML_STATUS fInstanceMode(void *user, const char *name, TOKEN_TYPE ttype,
		const char *value) {
	if (ttype == TAGDATA)
		cpeState.instanceMode = testBoolean(value);
	return XML_STS_OK;
}
/* xml definition tables for saving cpeState */
static XmlNodeDesc xferDesc[] = {
		{ iNULL, "CommandKey", fCommandKey, NULL },
		{ iNULL, "FileType", fFileType, NULL },
		{ iNULL, "URL", fURL, NULL},
		{ iNULL, "Username", fUsername, NULL },
		{ iNULL, "Password", fPassword, NULL },
		{ iNULL, "Filesize", fFilesize, NULL },
		{ iNULL, "TargetFileName", fTargetFileName, NULL },
		{ iNULL, "DelaySeconds", fDelaySeconds, NULL },
		{ iNULL, "FailureURL", fFailureURL, NULL },
		{ iNULL, "targetTime", ftargetTime, NULL },
		{ iNULL, "dlStartTime", fdlStartTime, NULL },
		{ iNULL, "dlEndTime", fdlEndTime, NULL },
		{ iNULL, "dlStatus", fdlStatus, NULL },
		{ iNULL, "dlFaultMsg", fdlFaultMsg, NULL },
#ifdef CONFIG_RPCSCHEDULEDOWNLOAD
		{ iNULL, "wStart", fdlWinStart, NULL},
		{ iNULL, "wEnd", fdlWinEnd, NULL},
		{ iNULL, "wMode", fdlWinMode,NULL},
		{ iNULL, "wUsrMsg", fdlWinUsrMsg, NULL},
		{ iNULL, "wMaxRetries", fdlWinMaxRetries, NULL},
#endif

};
#endif /* defined(CONFIG_RPCDOWNLOAD || CONFIG_RPCUPLOAD) */

/* xml definition tables for saving cpeState */
static XmlNodeDesc cpeDesc[] = {
    {iNULL, "eventMask", feventMask, NULL},
    {iNULL, "acsState", facsState, NULL},
    {iNULL, "rebootCommandKey", frebootCommandKey, NULL},
    {iNULL, "parameterKey", fparameterKey, NULL},
    {iNULL, "acsURL", facsURL, NULL},
    {iNULL, "acsUser", facsUser, NULL},
    {iNULL, "acsPW", facsPW, NULL},
    {iNULL, "connReqUser", fconnReqUser, NULL},
    {iNULL, "connReqPW", fconnReqPW, NULL},
    {iNULL, "informEnabled", finformEnabled, NULL},
    {iNULL, "informInterval", finformInterval, NULL},
    {iNULL, "informTime", finformTime, NULL},
    {iNULL, "upgradesManaged", fupgradesManaged, NULL},
    {iNULL, "instanceID", finstanceID, NULL},
    {iNULL, "acshost", facshost, NULL},
    {iNULL, "acsIPAddress", facsIPAddress,NULL},
    {iNULL, "defaultACSURL", fdefaultACSURL, NULL},
    {iNULL, "cwmpRetryMin", fcwmpRetryMin, NULL},
    {iNULL, "cwmpRetryMultiplier", fcwmpRetryMultiplier, NULL},
#ifdef CONFIG_RPCDOWNLOAD
		{ iNULL, "Download", fDownload, xferDesc },
		{ iNULL, "Upload", fUpload, xferDesc },
#ifdef CONFIG_RPCSCHEDULEDOWNLOAD
		{ iNULL, "ScheduleDownload", fScheduleDownload, xferDesc},
#endif
#endif
#ifdef CONFIG_TR111P2
	{iNULL, "connReqNotifyLimit", fconnReqNotifyLimit, NULL},
	{iNULL, "STUNEnable", fSTUNEnable, NULL},
	{iNULL, "STUNServerAddress",fSTUNServerAddress,NULL},
	{iNULL, "STUNServerPort", fSTUNServerPort,NULL},
	{iNULL, "STUNUsername", fSTUNUsername,NULL},
	{iNULL, "STUNPassword", fSTUNPassword,NULL},
	{iNULL, "STUNMaxKeepAlive", fSTUNMaxKeepAlive,NULL},
	{iNULL, "STUNMinKeepAlive", fSTUNMinKeepAlive,NULL},
#endif
	{iNULL, "autoCreate", fAutoCreateInstance, NULL},
	{iNULL, "instanceMode", fInstanceMode, NULL},
    {NULL, NULL}
};

/* TopLevel node for CPEState xml*/
XmlNodeDesc cpeStateDesc[] = {
    {iNULL,"CPEState",NULL, cpeDesc}, /* */
    {NULL, NULL}
};
/* end of xml parsing tables and functions */

#ifdef CONFIG_TR111P2
int saveTR111Param( XMLWriter *xp )
{
	xmlPrintTaggedData(xp,"connReqNotifyLimit", "%d", stunState.minNotifyLimit);
	xmlPrintTaggedData(xp,"STUNEnable", "%d", stunState.enable);
	xmlPrintTaggedData(xp,"STUNServerAddress", "%s", stunState.serverAddr?
					 stunState.serverAddr:	"");
	xmlPrintTaggedData(xp,"STUNServerPort", "%d", stunState.serverPort);
	xmlPrintTaggedData(xp,"STUNUsername", "%s", stunState.username?
					 stunState.username: "");
	xmlPrintTaggedData(xp,"STUNPassword", "%s", stunState.password?
	 				 stunState.password: "");
	xmlPrintTaggedData(xp,"STUNMaxKeepAlive", "%d", stunState.maxKeepAlive);
	return	xmlPrintTaggedData(xp,"STUNMinKeepAlive", "%d", stunState.minKeepAlive);
}
#endif


#if defined(CONFIG_RPCUPLOAD) || defined(CONFIG_RPCDOWNLOAD)
static void saveQueue(RPCRequest *qHead, XMLWriter *xp){
    RPCRequest *r;
    int i = 0;
    r = qHead;
    while ( r ) {
#ifdef CONFIG_RPCSCHEDULEDOWNLOAD
    	if ( r->rpcMethod == eScheduleDownload)
    		xmlOpenTagGrp(xp, "ScheduleDownload");
    	else
#endif
    		xmlOpenTagGrp(xp, r->rpcMethod==eDownload? "Download": "Upload");
    	xmlPrintTaggedData(xp,"CommandKey", "%s", r->commandKey);
       	xmlPrintTaggedData(xp, "FileType", "%s", r->ud.downloadReq.fileType);
       	xmlPrintTaggedData(xp, "URL", "%s", r->ud.downloadReq.URL);
       	xmlPrintTaggedData(xp, "Username", "%s",
       			r->ud.downloadReq.userName? r->ud.downloadReq.userName:"");
       	xmlPrintTaggedData(xp, "Password", "%s",
       			r->ud.downloadReq.passWord? r->ud.downloadReq.passWord:"");
       	xmlPrintTaggedData(xp, "Filesize", "%d", r->ud.downloadReq.fileSize);
       	xmlPrintTaggedData(xp, "TargetFileName", "%s",
       			r->ud.downloadReq.targetFileName? r->ud.downloadReq.targetFileName:"");
       	xmlPrintTaggedData(xp, "DelaySeconds", "%d", r->ud.downloadReq.delaySeconds);
       	xmlPrintTaggedData(xp, "FailureURL", "%s",
       			r->ud.downloadReq.failureURL? r->ud.downloadReq.failureURL:"");
    	xmlPrintTaggedData(xp, "dlStatus", "%d", r->DLMsg.dlStatus);
    	xmlPrintTaggedData(xp, "dlStartTime", "%d", r->DLMsg.dlStartTime);
    	xmlPrintTaggedData(xp, "dlEndTime", "%d", r->DLMsg.dlEndTime);
    	xmlPrintTaggedData(xp, "dlFaultMsg", "%d", r->DLMsg.dlFaultMsg);
    	xmlPrintTaggedData(xp, "targetTime", "%d", r->DLMsg.targetTime);
#ifdef CONFIG_RPCSCHEDULEDOWNLOAD
    	while( i<MAX_TIMEWINDOWS && r->DLMsg.timeWindow[i].windowStart>0 ){
    		TimeWindow *tp = &r->DLMsg.timeWindow[i];
    		xmlPrintTaggedData(xp, "wStart", "%d", tp->windowStart);
    		xmlPrintTaggedData(xp, "wEnd", "%d", tp->windowEnd);
    		xmlPrintTaggedData(xp, "wMode", "%s", tp->windowMode);
    		xmlPrintTaggedData(xp, "wUsrMsg", "%s", tp->userMessage? tp->userMessage: "");
    		xmlPrintTaggedData(xp, "wMaxRetries", "%d", r->DLMsg.timeWindow[i].maxRetries);
    		++i;
    	}
#endif
       	xmlCloseTagGrp(xp);
       	r = r->next;
    }
}
static void saveDLQueue(XMLWriter *xp){
	saveQueue( cpeState.dlQ, xp);
	saveQueue( cpeState.dlQActive, xp);
}
#endif

/*
 * Restore the CPE CWMP client state. This should read a file or
 * other persistent data store.
 * Note that this implementation attempts to read the CPESTATE_FILENAME file which is
 * created by the framework and updated whenever any framework persistent state variables
 * change. If this file is not available, such as first boot time, the optional factory
 * CPESTATE_FILENAME_DEFAULT file, if it exists, is read to provide the initial configuration.
 * If the default file contains a acsURL definition it is used as the default ACS URL
 * definition. If it does not contain an acsURL definition then an attempt is made to
 * recover it from the DHCP vendor encapsulated options.
 *
 * A factory reset should remove the CPESTATE_FILENAME file; but not the
 * CPESTATE_FILENAME_DEFAULT file.
*/

void cpeRestoreCPEState(void)
{
	ParseHow    parseReq;
	cwmpSetRunState(RUNSTATE_RESTORE);
	parseReq.topLevel = cpeStateDesc;
	parseReq.nameSpace = NULL;

	char tmp_new[128]={0};
	char tmp_default[128]={0};
	cmd_popen("test -e "CPESTATE_FILENAME" && cat "CPESTATE_FILENAME" | grep acsURL | sed -r 's/ //g'", tmp_new);
	cmd_popen("test -e "CPESTATE_FILENAME_DEFAULT" && cat "CPESTATE_FILENAME_DEFAULT" | grep acsURL | sed -r 's/ //g'", tmp_default);
	if(tmp_new[0] != '\0' && tmp_default[0] != '\0' && strcmp(tmp_default, tmp_new) != 0)
		system("rm -rf "CPESTATE_FILENAME);

	if (xmlParseGeneric(NULL, CPESTATE_FILENAME,NULL,0, &parseReq)!=XML_STS_OK) {
		cpeLog(LOG_ERROR,"Unable to initialize from file: %s", CPESTATE_FILENAME);
		/* no configured cpestate file exists. Attempt to open preconfigured file */
		if (xmlParseGeneric(NULL, CPESTATE_FILENAME_DEFAULT,NULL,0, &parseReq)!=XML_STS_OK) {
			cpeLog(LOG_ERROR,"Unable to initialize from file: %s",  CPESTATE_FILENAME_DEFAULT);
			return;
		} else {
			cpeLog(LOG_INFO, "Initialized from: %s", CPESTATE_FILENAME_DEFAULT);	/* a default cpestate file exists */
			if ( EMPTYSTR(cpeState.acsURL) ) {
				cpeState.defaultACSURL = NULL;
			} else
				cpeState.defaultACSURL = GS_STRDUP(cpeState.acsURL); /* save default pre-config acs if specified */
		}
	} else
		cpeLog(LOG_INFO, "Initialized from: %s", CPESTATE_FILENAME);	/* a configured cpestate file exists */
}

/*
* This saves the current cpeState to a file in xml format
*/
void cpeSaveCPEState(void) {
	FILE *fs;
	char *sBuf;
	int n = 0;

	XMLWriter *xp = xmlOpenWriter(XML_DFLT_MALLOCSZ, XML_WRITE_FLGS);
	if (xp) {
		xmlOpenTagGrp(xp, "CPEState");
		xmlPrintTaggedData(xp, "eventMask", "0x%08x", cpeState.eventMask);
		xmlPrintTaggedData(xp,"acsState", "%d", cpeState.acsContactState);
		xmlPrintTaggedData(xp, "rebootCommandKey", "%s",cpeState.rebootCommandKey);
		xmlPrintTaggedData(xp, "parameterKey", "%s",cpeState.parameterKey );
		xmlPrintTaggedData(xp, "acsURL", "%s",
				cpeState.acsURL ? cpeState.acsURL : "");
		xmlPrintTaggedData(xp, "acsUser", "%s",
				cpeState.acsUser ? cpeState.acsUser : "");
		xmlPrintTaggedData(xp, "acsPW", "%s", cpeState.acsPW ? cpeState.acsPW
				: "");

		if ( ! (cpeState.cwmpRetryMin ==CWMPRETRYMINIMUMWAITINTERVAL
				&& cpeState.cwmpRetryMultiplier == CWMPRETRYINTERNALMULTIPLIER )){
			// These values are parameters in v1.2 so only save if they are not default values.
			xmlPrintTaggedData(xp, "cwmpReplyMin" ,"%d", cpeState.cwmpRetryMin);
			xmlPrintTaggedData(xp, "cwmpReplyMultiplier", "%d", cpeState.cwmpRetryMultiplier);
		}
		xmlPrintTaggedData(xp, "connReqUser", "%s",
				cpeState.connReqUser ? cpeState.connReqUser : "");
		xmlPrintTaggedData(xp, "connReqPW", "%s",
				cpeState.connReqPW ? cpeState.connReqPW : "");
		xmlPrintTaggedData(xp, "informEnabled", "%d", cpeState.informEnabled);
		xmlPrintTaggedData(xp, "informInterval", "%d", cpeState.informInterval);
		xmlPrintTaggedData(xp, "informTime", "%d", cpeState.informTime);
		xmlPrintTaggedData(xp, "upgradesManaged", "%d",
				cpeState.upgradesManaged);
#if defined(CONFIG_RPCUPLOAD) || defined(CONFIG_RPCDOWNLOAD)
		saveDLQueue(xp);
#endif
		xmlPrintTaggedData(xp, "instanceID", "%d", cpeState.instanceID);
		xmlPrintTaggedData(xp, "acshost", "%s", cpeState.acsHostName);
		xmlPrintTaggedData(xp, "acsIPAddress", "%s", writeInIPAddr(
				&cpeState.acsIPAddress));
		xmlPrintTaggedData(xp, "defaultACSURL", "%s",
				cpeState.defaultACSURL ? cpeState.defaultACSURL : "");
#ifdef CONFIG_TR111P2
		saveTR111Param( xp );
#endif
		xmlPrintTaggedData(xp, "autoCreate", "%d", cpeState.autoCreateInstances);
		xmlPrintTaggedData(xp, "instanceMode", "%d", cpeState.instanceMode);
		xmlCloseTagGrp(xp);
		if ((sBuf = xmlSaveCloseWriter(xp)) != NULL) {
			/* check that buffer is present */

			if ((fs = fopen(CPESTATE_FILENAME, "w"))) {
				n = fwrite(sBuf, strlen(sBuf), 1, fs);
				fclose(fs);
			} else
				cpeLog(LOG_ERR, "Unable to open %s", CPESTATE_FILENAME);
			GS_FREE(sBuf);
		}
	}
	if (n == 0)
		cpeLog(LOG_CRIT, "Unable to save CPEState data");
}



/* need to save in persistent memeory */
int cpeGetInstanceID(void)
{
	return ++cpeState.instanceID;
}

/*
 * Used by LAN.Stats to get the stats data.
 * field: specifies the field position of the data in the /proc/net/dev
 * line.
 * If no field empty string is returned.
 */
const char *cpeReadLANStats(int field){
	char *r = cpeReadFileField("/proc/net/dev", cpeNetIF->name, field );
	if ( r == NULL )
		return "";
	return r;
}
/* The following functions are called from the CWMPC framework to perform
*  configuration functions or request data.
*
* cpeRefreshCPEData( CPEState *cpe)
* refresh the CPEState components:
*     connReqURL with the current connection request URL.
*     connReqPath with the current connection request path
*     IPAddress with the IP address of the default WAN interface.
*     MACAddress with the current MAC address of the default WAN interface.
*    and update instances.
*/
void notifyCallbacks(void *);
extern InAddr wanIP;

void cpeRefreshCPEData(CPEState *cpe) {
	char buf[CRURL_SZ];
	const char *format;
	char mac[30]={0};
	char cmd[30]={0};
    int i;
	if ( cpeGetNetIPInfo(0) != NULL) {
		if (!eqInIPAddr(&cpe->ipAddress, &wanIP)) {
			/* IP Address has changed - notify those tasks watching for a change */
			/* This notification is necessary to force STUN to reset it's state. */
			notifyCallbacks(&cpe->ipAddress);

            /* If IP Address has change , need update ConnURL and Value Change */
            char rstring[CRPATH_SZ-1] = {0};
            randomstring(rstring, sizeof(rstring)-1);
            sprintf(cpeState.connReqPath, "/%s", rstring);
            cwmpAddEvent(eEvtValueChange);
		}
		cpe->ipAddress = wanIP;
		if ( cpe->ipAddress.inFamily != 0){
			if(cpeNetIF->mACAddress)
			{
				readMac(cpe->MACAddress, cpeNetIF->mACAddress);
		    }
			else
			{
                cmd_popen("state_cfg get ethaddr",mac);
                if (mac[strlen(mac)-1] =='\n')
                    mac[strlen(mac)-1]= '\0';
                for(i = 0 ; i < strlen(mac)+1 ; i++ )
                {
                    if(mac[i] == ':')
                    {
                        cmd[i] = ':';
                        continue;
                    }

                    cmd[i] = tolower(mac[i]);
                }
                COPYSTR(cpeNetIF->mACAddress, cmd);
                readMac(cpe->MACAddress, cpeNetIF->mACAddress);
#if 0
				cpeLog(LOG_INFO, "mACAddress is NULL, the config.save is wrong, remove it and exit");
				cpeLog(LOG_INFO, "Please run cwmpc again !");
				system("rm -rf "CONFIG_FILENAME);
				exit(0);
#endif
			}

			format = (cpe->ipAddress.inFamily == AF_INET6) ? "http://[%s]:%d%s"
					: "http://%s:%d%s";
			snprintf(buf, sizeof(buf), format, writeInIPAddr(&cpe->ipAddress),
					ACSCONNREQ_PORT, cpe->connReqPath); /* path has leading / */
			strcpy(cpe->connReqURL,buf);
		} else
			cpe->connReqURL[0] = '\0';
	}
}


/*
* Perform a reboot function. Return to CWMPC is not required.
*/

void CPEReboot(void *handle)
{
    cpeReboot();
}

void cpeReboot(void)
{
	cpeSaveCPEState();
	fprintf(stderr, "cpeReboot() called\n");
	system("led_ctrl LED_REBOOT");
	system("sync;sync;sync;reboot &");
	exit(0);
}

/*
* Reset the configuration to the factory defaults
* Return to CWMPC is not required.
*/
void cpeFactoryReset(void)
{
	/* clear the state of the CPE to factory defaults here */
	/* This should remove the CPESTATE_FILENAME file.      */
	fprintf(stderr, "cpeFactoryReset() called\n");

	system("led_ctrl LED_RESET_DEFAULT");
	system("jffs2reset -y && reboot &");
}

#ifndef CONFIG_RPCCHANGEDUSTATE
/* If building for SMM the lock synchronizing functions are in the cpeSMM.c file
 * in order to consolidate synchronizing functions in the same file.
 */
/*
* This function is called whenever the CWMPC initiates a session
 * with the ACS.
*/
static int cpeLockCnt = 0;
void cpeLockConfiguration(void)
{
	++cpeLockCnt;
	fprintf(stderr, "cpeLockConfiguration( LockCnt=%d)\n", cpeLockCnt);
}
/*
* This function is called whenever the CWMPC terminates a session
* with the ACS.
*/
void cpeSaveConfig(void);

void cpeUnlockConfiguration(int changeCfgCnt )
{
	--cpeLockCnt;
	fprintf(stderr, "cpeUnlockConfiguration( LockCnt=%d, changed=%s)\n",
			cpeLockCnt, changeCfgCnt? "True": "False");
	if ( changeCfgCnt )
		cpeSaveConfig();
}
#endif // CONFIG_RPCCHANGEDUSTATE

/*
* This function saves the compressed notification attributes to
 * persisent memory.
*/
void cpeSaveNotifyAttributes(void *p, int lth)
{
	int f, n;
	if ( (f=open(NOTIFYATTRIBFILE,
		 O_CREAT|O_WRONLY|O_TRUNC, S_IRWXU|S_IRWXO|S_IRWXO))!= -1  ) {
	   n = write(f, p, lth);
	   if (n!=lth)
		   fprintf(stderr, "Error writing attribute file\n");
	   close(f);
	}
}
/*
* Return a buffer pointer to the saved attribute buffer.
* *lth is to contain the lth of the buffer.
* Buffer should be malloced and will be freed by the
* caller.
*/

void *cpeRestoreNotifyAttributes(int *lth )
{
	int f, n, sn;
	struct stat filestat;
	char 	*p = NULL;

	sn = 0;
	if ( (f=open(NOTIFYATTRIBFILE, 0)) != -1  ) {
		if ( fstat( f, &filestat)==0 ) {
			n =  filestat.st_size;
			if ( (p=GS_MALLOC(n))!= NULL ) {
				sn = read(f, p, n);
			} else {
				cpeLog(LOG_ERR, "cpeRestoreNotifyAttributes: No Memory");
			}
		}
	   close(f);
	}
	*lth = sn;
	return p;
}
#ifdef OPENLOCALUDPPORT
/*
* The following are functions that are called by the CPE configuration
* functions that are running within the same address space as the CWMPc.
* They must not use the event.c event, timer and signaling
* handling functions since they are not thread safe. See cpelisten.c,
*/
static int udpLocalSend(short port, void *data, int len)
{
  int sockfd;
  struct sockaddr_in serv_addr;
  struct sockaddr_in lcl_addr;

  /* fill in local host -- server address */
  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family      = AF_INET;
  serv_addr.sin_addr.s_addr = readIp("127.0.0.1");
  serv_addr.sin_port        = htons(port);

  /* open udp socket */
  if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    cpeLog(LOG_ERR, "Could not open local host socket for send\n");
    return -1; /* could not open socket */
  }

  /* bind any local address for us */
  memset(&lcl_addr, 0, sizeof(lcl_addr));
  lcl_addr.sin_family      = AF_INET;
  lcl_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  lcl_addr.sin_port        = htons(0);

  if (bind(sockfd, (struct sockaddr *) &lcl_addr, sizeof(lcl_addr)) < 0) {
    cpeLog(LOG_ERR, "Could not bind client socket\n");
    return -2; /* could not bind client socket */
  }

  /* send the data */
  if (sendto(sockfd, data, len, 0, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) != len) {
    cpeLog(LOG_ERR,  "Could not sendto\n");
    return -3; /* could not sendto */
  }
  close(sockfd);
  return 0;
}
/*
* This function may be called by a CPE configuration function to
* signal the CWMPC that some change has occurred to the configuration
* database or some action is required. Also see cpelisten.c.
* The defined events are:
 * 	CPE_PARAMCHANGE:  One or more parameter values has been changed.
 *  CPE_DIAGNOSTICS_COMPLETE : A diagnostic process kicked off by a CWMPC call to a
 *            setter function has completed. The results are available to the
 *            corresponding getter function.
 *
 *  CPE_SENDINFORM; Force CWMPC to send an Inform RPC. Useful for testing.
 *            Similiar to a connection request.
 */
void signalCPEEvent( CPEEvent cpeEvent)
{
	udpLocalSend(CPELISTENPORT, (void*)&cpeEvent, sizeof(cpeEvent));
	return;
}

#endif /* OPENLOCALUDPPORT */

