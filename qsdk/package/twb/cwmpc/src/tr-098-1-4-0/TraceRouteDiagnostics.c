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
 * $Id: TraceRouteDiagnostics.c,v 1.1 2012/05/10 17:38:00 dmounday Exp $
 *----------------------------------------------------------------------*/

#include "paramTree.h"
#include "rpc.h"
#include "gslib/src/utils.h"
#include "soapRpc/rpcUtils.h"
#include "soapRpc/rpcMethods.h"
#include "soapRpc/cwmpSession.h"

#include "TraceRouteDiagnostics.h"

/**@obj TraceRouteDiagnosticsRouteHops **/
CPE_STATUS  addTraceRouteDiagnosticsRouteHops(CWMPObject *o, Instance *ip)
{
	/* add instance data */
	TraceRouteDiagnosticsRouteHops *p = (TraceRouteDiagnosticsRouteHops *)GS_MALLOC( sizeof(struct TraceRouteDiagnosticsRouteHops));
	memset(p, 0, sizeof(struct TraceRouteDiagnosticsRouteHops));
	ip->cpeData = (void *)p;
	return CPE_OK;
}
CPE_STATUS  delTraceRouteDiagnosticsRouteHops(CWMPObject *o, Instance *ip)
{
	/* delete instance data */
	TraceRouteDiagnosticsRouteHops *p = (TraceRouteDiagnosticsRouteHops *)ip->cpeData;
	if( p ){
		//TODO: free instance data
		GS_FREE(p);
	}
	return CPE_OK;
}

/**@param TraceRouteDiagnosticsRouteHops_HopHost                     **/
CPE_STATUS getTraceRouteDiagnosticsRouteHops_HopHost(Instance *ip, char **value)
{
	TraceRouteDiagnosticsRouteHops *p = (TraceRouteDiagnosticsRouteHops *)ip->cpeData;
	if ( p ){
		if ( p->hopHost )
			*value = GS_STRDUP(p->hopHost);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param TraceRouteDiagnosticsRouteHops_HopHostAddress                     **/
CPE_STATUS getTraceRouteDiagnosticsRouteHops_HopHostAddress(Instance *ip, char **value)
{
	TraceRouteDiagnosticsRouteHops *p = (TraceRouteDiagnosticsRouteHops *)ip->cpeData;
	if ( p ){
		if ( p->hopHostAddress )
			*value = GS_STRDUP(p->hopHostAddress);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param TraceRouteDiagnosticsRouteHops_HopErrorCode                     **/
CPE_STATUS getTraceRouteDiagnosticsRouteHops_HopErrorCode(Instance *ip, char **value)
{
	TraceRouteDiagnosticsRouteHops *p = (TraceRouteDiagnosticsRouteHops *)ip->cpeData;
	if ( p ){
		char    buf[10];
		snprintf(buf,sizeof(buf),"%u", p->hopErrorCode);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param TraceRouteDiagnosticsRouteHops_HopRTTimes                     **/
CPE_STATUS getTraceRouteDiagnosticsRouteHops_HopRTTimes(Instance *ip, char **value)
{
	TraceRouteDiagnosticsRouteHops *p = (TraceRouteDiagnosticsRouteHops *)ip->cpeData;
	if ( p ){
		char    buf[10];
		snprintf(buf,sizeof(buf),"%u", p->hopRTTimes);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@endobj TraceRouteDiagnosticsRouteHops **/

/**@obj TraceRouteDiagnostics **/
CPE_STATUS  initTraceRouteDiagnostics(CWMPObject *o, Instance *ip)
{
	/* initialize object */
	TraceRouteDiagnostics *p = (TraceRouteDiagnostics *)GS_MALLOC( sizeof(struct TraceRouteDiagnostics));
	memset(p, 0, sizeof(struct TraceRouteDiagnostics));
	ip->cpeData = (void *)p;
	return CPE_OK;
}
CPE_STATUS  commitTraceRouteDiagnostics(CWMPObject *o, Instance *ip, eCommitCBType cmt)
{
	/* commit object instance */
	return CPE_OK;
}

/**@param TraceRouteDiagnostics_DiagnosticsState                     **/
CPE_STATUS setTraceRouteDiagnostics_DiagnosticsState(Instance *ip, char *value)
{
	TraceRouteDiagnostics *p = (TraceRouteDiagnostics *)ip->cpeData;
	if ( p ){
		COPYSTR(p->diagnosticsState, value);
	}
	return CPE_OK;
}
CPE_STATUS getTraceRouteDiagnostics_DiagnosticsState(Instance *ip, char **value)
{
	TraceRouteDiagnostics *p = (TraceRouteDiagnostics *)ip->cpeData;
	if ( p ){
		if ( p->diagnosticsState )
			*value = GS_STRDUP(p->diagnosticsState);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param TraceRouteDiagnostics_Interface                     **/
CPE_STATUS setTraceRouteDiagnostics_Interface(Instance *ip, char *value)
{
	TraceRouteDiagnostics *p = (TraceRouteDiagnostics *)ip->cpeData;
	if ( p ){
		//TODO: resolve Instance pointers.
	}
	return CPE_OK;
}
CPE_STATUS getTraceRouteDiagnostics_Interface(Instance *ip, char **value)
{
	TraceRouteDiagnostics *p = (TraceRouteDiagnostics *)ip->cpeData;
	if ( p ){
		*value = cwmpGetInstancePathStr(p->interface);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param TraceRouteDiagnostics_Host                     **/
CPE_STATUS setTraceRouteDiagnostics_Host(Instance *ip, char *value)
{
	TraceRouteDiagnostics *p = (TraceRouteDiagnostics *)ip->cpeData;
	if ( p ){
		COPYSTR(p->host, value);
	}
	return CPE_OK;
}
CPE_STATUS getTraceRouteDiagnostics_Host(Instance *ip, char **value)
{
	TraceRouteDiagnostics *p = (TraceRouteDiagnostics *)ip->cpeData;
	if ( p ){
		if ( p->host )
			*value = GS_STRDUP(p->host);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param TraceRouteDiagnostics_NumberOfTries                     **/
CPE_STATUS setTraceRouteDiagnostics_NumberOfTries(Instance *ip, char *value)
{
	TraceRouteDiagnostics *p = (TraceRouteDiagnostics *)ip->cpeData;
	if ( p ){
		p->numberOfTries=atoi(value);
	}
	return CPE_OK;
}
CPE_STATUS getTraceRouteDiagnostics_NumberOfTries(Instance *ip, char **value)
{
	TraceRouteDiagnostics *p = (TraceRouteDiagnostics *)ip->cpeData;
	if ( p ){
		char    buf[10];
		snprintf(buf,sizeof(buf),"%u", p->numberOfTries);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param TraceRouteDiagnostics_Timeout                     **/
CPE_STATUS setTraceRouteDiagnostics_Timeout(Instance *ip, char *value)
{
	TraceRouteDiagnostics *p = (TraceRouteDiagnostics *)ip->cpeData;
	if ( p ){
		p->timeout=atoi(value);
	}
	return CPE_OK;
}
CPE_STATUS getTraceRouteDiagnostics_Timeout(Instance *ip, char **value)
{
	TraceRouteDiagnostics *p = (TraceRouteDiagnostics *)ip->cpeData;
	if ( p ){
		char    buf[10];
		snprintf(buf,sizeof(buf),"%u", p->timeout);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param TraceRouteDiagnostics_DataBlockSize                     **/
CPE_STATUS setTraceRouteDiagnostics_DataBlockSize(Instance *ip, char *value)
{
	TraceRouteDiagnostics *p = (TraceRouteDiagnostics *)ip->cpeData;
	if ( p ){
		p->dataBlockSize=atoi(value);
	}
	return CPE_OK;
}
CPE_STATUS getTraceRouteDiagnostics_DataBlockSize(Instance *ip, char **value)
{
	TraceRouteDiagnostics *p = (TraceRouteDiagnostics *)ip->cpeData;
	if ( p ){
		char    buf[10];
		snprintf(buf,sizeof(buf),"%u", p->dataBlockSize);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param TraceRouteDiagnostics_DSCP                     **/
CPE_STATUS setTraceRouteDiagnostics_DSCP(Instance *ip, char *value)
{
	TraceRouteDiagnostics *p = (TraceRouteDiagnostics *)ip->cpeData;
	if ( p ){
		p->dSCP=atoi(value);
	}
	return CPE_OK;
}
CPE_STATUS getTraceRouteDiagnostics_DSCP(Instance *ip, char **value)
{
	TraceRouteDiagnostics *p = (TraceRouteDiagnostics *)ip->cpeData;
	if ( p ){
		char    buf[10];
		snprintf(buf,sizeof(buf),"%u", p->dSCP);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param TraceRouteDiagnostics_MaxHopCount                     **/
CPE_STATUS setTraceRouteDiagnostics_MaxHopCount(Instance *ip, char *value)
{
	TraceRouteDiagnostics *p = (TraceRouteDiagnostics *)ip->cpeData;
	if ( p ){
		p->maxHopCount=atoi(value);
	}
	return CPE_OK;
}
CPE_STATUS getTraceRouteDiagnostics_MaxHopCount(Instance *ip, char **value)
{
	TraceRouteDiagnostics *p = (TraceRouteDiagnostics *)ip->cpeData;
	if ( p ){
		char    buf[10];
		snprintf(buf,sizeof(buf),"%u", p->maxHopCount);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param TraceRouteDiagnostics_ResponseTime                     **/
CPE_STATUS getTraceRouteDiagnostics_ResponseTime(Instance *ip, char **value)
{
	TraceRouteDiagnostics *p = (TraceRouteDiagnostics *)ip->cpeData;
	if ( p ){
		char    buf[10];
		snprintf(buf,sizeof(buf),"%u", p->responseTime);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param TraceRouteDiagnostics_RouteHopsNumberOfEntries                     **/
CPE_STATUS getTraceRouteDiagnostics_RouteHopsNumberOfEntries(Instance *ip, char **value)
{
	cwmpGetObjInstanceCntStr("RouteHops", value);
	return CPE_OK;
}
/**@endparam                                               **/
/**@endobj TraceRouteDiagnostics **/




