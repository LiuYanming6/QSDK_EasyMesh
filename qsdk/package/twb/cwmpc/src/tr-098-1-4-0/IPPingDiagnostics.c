/*----------------------------------------------------------------------*
 * Gatespace
 * Copyright 2011, 2012 Gatespace Networks, Inc., All Rights Reserved.
 * Gatespace Networks, Inc. confidential material.
 *----------------------------------------------------------------------*
 * File Name  :
 * Description: Auto-generated getter/setter stubs file.
 *----------------------------------------------------------------------*
 * $Revision: 1.1 $
 *
 * $Id: IPPingDiagnostics.c,v 1.1 2012/05/10 17:38:00 dmounday Exp $
 *----------------------------------------------------------------------*/

#include "paramTree.h"
#include "rpc.h"
#include "gslib/src/utils.h"
#include "soapRpc/rpcUtils.h"
#include "soapRpc/rpcMethods.h"
#include "soapRpc/cwmpSession.h"

#include "IPPingDiagnostics.h"

/**@obj IPPingDiagnostics **/
CPE_STATUS  initIPPingDiagnostics(CWMPObject *o, Instance *ip)
{
	/* initialize object */
	IPPingDiagnostics *p = (IPPingDiagnostics *)GS_MALLOC( sizeof(struct IPPingDiagnostics));
	memset(p, 0, sizeof(struct IPPingDiagnostics));
	ip->cpeData = (void *)p;
	return CPE_OK;
}
CPE_STATUS  commitIPPingDiagnostics(CWMPObject *o, Instance *ip, eCommitCBType cmt)
{
	/* commit object instance */
	return CPE_OK;
}

/**@param IPPingDiagnostics_DiagnosticsState                     **/
CPE_STATUS setIPPingDiagnostics_DiagnosticsState(Instance *ip, char *value)
{
	IPPingDiagnostics *p = (IPPingDiagnostics *)ip->cpeData;
	if ( p ){
		COPYSTR(p->diagnosticsState, value);
	}
	return CPE_OK;
}
CPE_STATUS getIPPingDiagnostics_DiagnosticsState(Instance *ip, char **value)
{
	IPPingDiagnostics *p = (IPPingDiagnostics *)ip->cpeData;
	if ( p ){
		if ( p->diagnosticsState )
			*value = GS_STRDUP(p->diagnosticsState);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param IPPingDiagnostics_Interface                     **/
CPE_STATUS setIPPingDiagnostics_Interface(Instance *ip, char *value)
{
	IPPingDiagnostics *p = (IPPingDiagnostics *)ip->cpeData;
	if ( p ){
		//TODO: resolve Instance pointers.
	}
	return CPE_OK;
}
CPE_STATUS getIPPingDiagnostics_Interface(Instance *ip, char **value)
{
	IPPingDiagnostics *p = (IPPingDiagnostics *)ip->cpeData;
	if ( p ){
		*value = cwmpGetInstancePathStr(p->interface);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param IPPingDiagnostics_Host                     **/
CPE_STATUS setIPPingDiagnostics_Host(Instance *ip, char *value)
{
	IPPingDiagnostics *p = (IPPingDiagnostics *)ip->cpeData;
	if ( p ){
		COPYSTR(p->host, value);
	}
	return CPE_OK;
}
CPE_STATUS getIPPingDiagnostics_Host(Instance *ip, char **value)
{
	IPPingDiagnostics *p = (IPPingDiagnostics *)ip->cpeData;
	if ( p ){
		if ( p->host )
			*value = GS_STRDUP(p->host);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param IPPingDiagnostics_NumberOfRepetitions                     **/
CPE_STATUS setIPPingDiagnostics_NumberOfRepetitions(Instance *ip, char *value)
{
	IPPingDiagnostics *p = (IPPingDiagnostics *)ip->cpeData;
	if ( p ){
		p->numberOfRepetitions=atoi(value);
	}
	return CPE_OK;
}
CPE_STATUS getIPPingDiagnostics_NumberOfRepetitions(Instance *ip, char **value)
{
	IPPingDiagnostics *p = (IPPingDiagnostics *)ip->cpeData;
	if ( p ){
		char    buf[10];
		snprintf(buf,sizeof(buf),"%u", p->numberOfRepetitions);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param IPPingDiagnostics_Timeout                     **/
CPE_STATUS setIPPingDiagnostics_Timeout(Instance *ip, char *value)
{
	IPPingDiagnostics *p = (IPPingDiagnostics *)ip->cpeData;
	if ( p ){
		p->timeout=atoi(value);
	}
	return CPE_OK;
}
CPE_STATUS getIPPingDiagnostics_Timeout(Instance *ip, char **value)
{
	IPPingDiagnostics *p = (IPPingDiagnostics *)ip->cpeData;
	if ( p ){
		char    buf[10];
		snprintf(buf,sizeof(buf),"%u", p->timeout);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param IPPingDiagnostics_DataBlockSize                     **/
CPE_STATUS setIPPingDiagnostics_DataBlockSize(Instance *ip, char *value)
{
	IPPingDiagnostics *p = (IPPingDiagnostics *)ip->cpeData;
	if ( p ){
		p->dataBlockSize=atoi(value);
	}
	return CPE_OK;
}
CPE_STATUS getIPPingDiagnostics_DataBlockSize(Instance *ip, char **value)
{
	IPPingDiagnostics *p = (IPPingDiagnostics *)ip->cpeData;
	if ( p ){
		char    buf[10];
		snprintf(buf,sizeof(buf),"%u", p->dataBlockSize);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param IPPingDiagnostics_DSCP                     **/
CPE_STATUS setIPPingDiagnostics_DSCP(Instance *ip, char *value)
{
	IPPingDiagnostics *p = (IPPingDiagnostics *)ip->cpeData;
	if ( p ){
		p->dSCP=atoi(value);
	}
	return CPE_OK;
}
CPE_STATUS getIPPingDiagnostics_DSCP(Instance *ip, char **value)
{
	IPPingDiagnostics *p = (IPPingDiagnostics *)ip->cpeData;
	if ( p ){
		char    buf[10];
		snprintf(buf,sizeof(buf),"%u", p->dSCP);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param IPPingDiagnostics_SuccessCount                     **/
CPE_STATUS getIPPingDiagnostics_SuccessCount(Instance *ip, char **value)
{
	IPPingDiagnostics *p = (IPPingDiagnostics *)ip->cpeData;
	if ( p ){
		char    buf[10];
		snprintf(buf,sizeof(buf),"%u", p->successCount);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param IPPingDiagnostics_FailureCount                     **/
CPE_STATUS getIPPingDiagnostics_FailureCount(Instance *ip, char **value)
{
	IPPingDiagnostics *p = (IPPingDiagnostics *)ip->cpeData;
	if ( p ){
		char    buf[10];
		snprintf(buf,sizeof(buf),"%u", p->failureCount);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param IPPingDiagnostics_AverageResponseTime                     **/
CPE_STATUS getIPPingDiagnostics_AverageResponseTime(Instance *ip, char **value)
{
	IPPingDiagnostics *p = (IPPingDiagnostics *)ip->cpeData;
	if ( p ){
		char    buf[10];
		snprintf(buf,sizeof(buf),"%u", p->averageResponseTime);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param IPPingDiagnostics_MinimumResponseTime                     **/
CPE_STATUS getIPPingDiagnostics_MinimumResponseTime(Instance *ip, char **value)
{
	IPPingDiagnostics *p = (IPPingDiagnostics *)ip->cpeData;
	if ( p ){
		char    buf[10];
		snprintf(buf,sizeof(buf),"%u", p->minimumResponseTime);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param IPPingDiagnostics_MaximumResponseTime                     **/
CPE_STATUS getIPPingDiagnostics_MaximumResponseTime(Instance *ip, char **value)
{
	IPPingDiagnostics *p = (IPPingDiagnostics *)ip->cpeData;
	if ( p ){
		char    buf[10];
		snprintf(buf,sizeof(buf),"%u", p->maximumResponseTime);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@endobj IPPingDiagnostics **/




