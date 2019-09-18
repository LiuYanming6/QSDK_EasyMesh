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
 * $Id: TraceRouteDiagnostics.c,v 1.1 2012/05/10 17:37:59 dmounday Exp $
 *----------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <errno.h>
#include <ctype.h>
#include "paramTree.h"
#include "rpc.h"
#include "gslib/src/utils.h"
#include "soapRpc/rpcUtils.h"
#include "soapRpc/rpcMethods.h"
#include "soapRpc/cwmpSession.h"


/**@obj TraceRouteDiagnosticsRouteHops **/
#include "includes/diagTraceRoute.h"
extern ACSSession acsSession;
extern TraceRtState cpeTRState;
/**@param TraceRouteDiagnosticsRouteHops_HopHost                     **/
CPE_STATUS getTraceRouteDiagnosticsRouteHops_HopHost(Instance *ip, char **value)
{
	/* get parameter */
	if ( ip && ip->cpeData ){
		*value = GS_STRDUP( ip->cpeData );
		return CPE_OK;
	}
	return CPE_ERR;
}
/**@endparam                                               **/
/**@param TraceRouteDiagnosticsRouteHops_HopHostAddress                     **/
CPE_STATUS getTraceRouteDiagnosticsRouteHops_HopHostAddress(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param TraceRouteDiagnosticsRouteHops_HopErrorCode                     **/
CPE_STATUS getTraceRouteDiagnosticsRouteHops_HopErrorCode(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param TraceRouteDiagnosticsRouteHops_HopRTTimes                     **/
CPE_STATUS getTraceRouteDiagnosticsRouteHops_HopRTTimes(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@endobj TraceRouteDiagnosticsRouteHops **/

/**@obj TraceRouteDiagnostics **/
/*
 * Commit is used to verify the correct parameter combination of values
 * and to schedule starting the traceroute.
 */
extern CWMPObject TraceRouteDiagnostics_Objs[];

CPE_STATUS commitTraceRouteDiagnostics(CWMPObject *o, Instance *ip, eCommitCBType cmt) {
	/* Set parameter */
	if ( cmt==COMMIT_SET && cpeTRState.pendingState == eRequested ) {
		if ( cpeTRState.host == NULL ){
			return CPE_ERR;
		}
		cpeTRState.parentObj = o;
		cpeTRState.hopObjs = cwmpSrchOList(TraceRouteDiagnostics_Objs, "RouteHops");
		if ( cpeTRState.state == eRequested ) {
			/* current state is Requested -- need to stop running trace route*/
			cpeStopTraceRt((void*)eRequested);
			cpeTRState.pendingState = eNone;
			cpeResetTRState();
			cpeStartTraceRt(NULL);
		} else {
			/* not running */
			cpeTRState.state = eRequested;
			cpeTRState.pendingState = eNone;
			setCallback( &acsSession, cpeStartTraceRt, NULL );
		}
	} else if ( cpeTRState.state == eRequested ){
		/* a writable parameter was set while traceroute is running */
		/* stop traceroute and set state to eNone */
		cpeStopTraceRt((void*)eNone);
		cpeTRState.pendingState = eNone;
	}
	return CPE_OK;
}
/**@param TraceRouteDiagnostics_DiagnosticsState                     **/
CPE_STATUS setTraceRouteDiagnostics_DiagnosticsState(Instance *ip, char *value)
{
	/* set parameter */
	if ( strcasecmp( value, "Requested") ==0 ) {
		cpeTRState.pendingState = eRequested;
		return CPE_OK;
	}
	return CPE_9003;
}
CPE_STATUS getTraceRouteDiagnostics_DiagnosticsState(Instance *ip, char **value)
{
	/* set parameter */
	switch (cpeTRState.state) {
		case eRequested:
			*value = GS_STRDUP("Requested");
			break;
		case eStateNone:
			*value = GS_STRDUP("None");
			break;
		case eHostError:
			*value = GS_STRDUP("Error_CannotResolveHostName");
			break;
		case eComplete:
			*value = GS_STRDUP("Completed");
			break;
		case eMaxHopExceeded:
			*value = GS_STRDUP("Error_MaxHopCountExceeded");
			break;
		default:
			break;
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param TraceRouteDiagnostics_Interface                     **/
CPE_STATUS setTraceRouteDiagnostics_Interface(Instance *ip, char *value)
{
	/* Set parameter */
	COPYSTR(cpeTRState.interface, value);
	return CPE_OK;
}
CPE_STATUS getTraceRouteDiagnostics_Interface(Instance *ip, char **value)
{
	/* get parameter */
	if ( cpeTRState.interface)
		value = GS_STRDUP(cpeTRState.interface);
	return CPE_OK;
}
/**@endparam                                               **/
/**@param TraceRouteDiagnostics_Host                     **/
CPE_STATUS setTraceRouteDiagnostics_Host(Instance *ip, char *value)
{
	/* Set parameter */
	if (cpeTRState.host)
		GS_FREE(cpeTRState.host);
	cpeTRState.host = GS_STRDUP(value);
	return CPE_OK;
}
CPE_STATUS getTraceRouteDiagnostics_Host(Instance *ip, char **value)
{
	if (cpeTRState.host )
	 	*value = GS_STRDUP(cpeTRState.host);
	return CPE_OK;
}
/**@endparam                                               **/
/**@param TraceRouteDiagnostics_NumberOfTries                     **/
CPE_STATUS setTraceRouteDiagnostics_NumberOfTries(Instance *ip, char *value)
{
	/* set parameter */
	return CPE_OK;
}
CPE_STATUS getTraceRouteDiagnostics_NumberOfTries(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param TraceRouteDiagnostics_Timeout                     **/
CPE_STATUS setTraceRouteDiagnostics_Timeout(Instance *ip, char *value)
{
	/* Set parameter */
	cpeTRState.timeout = atoi(value);
	return CPE_OK;
}
CPE_STATUS getTraceRouteDiagnostics_Timeout(Instance *ip, char **value)
{
	/* get parameter */
	*value = GS_STRDUP(itoa(cpeTRState.timeout));
	return CPE_OK;
}
/**@endparam                                               **/
/**@param TraceRouteDiagnostics_DataBlockSize                     **/
CPE_STATUS setTraceRouteDiagnostics_DataBlockSize(Instance *ip, char *value)
{
	/* set parameter */
	cpeTRState.dataBlkSz = atoi(value);
	return CPE_OK;
}
CPE_STATUS getTraceRouteDiagnostics_DataBlockSize(Instance *ip, char **value)
{
	/* get parameter */
	 *value = GS_STRDUP(itoa(cpeTRState.dataBlkSz));
	return CPE_OK;
}
/**@endparam                                               **/
/**@param TraceRouteDiagnostics_DSCP                     **/
CPE_STATUS setTraceRouteDiagnostics_DSCP(Instance *ip, char *value)
{
	/* Set parameter */
	cpeTRState.DSCP = atoi(value);
	return CPE_OK;
}
CPE_STATUS getTraceRouteDiagnostics_DSCP(Instance *ip, char **value)
{
	/* get parameter */
	 *value = GS_STRDUP(itoa(cpeTRState.DSCP));
	return CPE_OK;
}
/**@endparam                                               **/
/**@param TraceRouteDiagnostics_MaxHopCount                     **/
CPE_STATUS setTraceRouteDiagnostics_MaxHopCount(Instance *ip, char *value)
{
	/* set parameter */
	cpeTRState.maxTTL = atoi(value);
	return CPE_OK;
}
CPE_STATUS getTraceRouteDiagnostics_MaxHopCount(Instance *ip, char **value)
{
	/* get parameter */
	 *value = GS_STRDUP(itoa(cpeTRState.maxTTL));
	return CPE_OK;
}
/**@endparam                                               **/
/**@param TraceRouteDiagnostics_ResponseTime                     **/
CPE_STATUS getTraceRouteDiagnostics_ResponseTime(Instance *ip, char **value)
{
	/* get parameter */
	*value = GS_STRDUP(itoa(cpeTRState.responseTime));
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






