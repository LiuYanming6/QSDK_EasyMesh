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
 * $Id: IPPingDiagnostics.c,v 1.1 2012/05/10 17:38:08 dmounday Exp $
 *----------------------------------------------------------------------*/

#include "paramTree.h"
#include "rpc.h"
#include "gslib/src/utils.h"
#include "soapRpc/rpcUtils.h"
#include "soapRpc/rpcMethods.h"
#include "soapRpc/cwmpSession.h"


/**@obj IPPingDiagnostics **/
#include "includes/diagPing.h"
extern PingParam cpePingParam;
extern void*     acsSession;

/*
 * Commit is used to verify the correct parameter combination of values
 * and to schedule starting ping.
 */
CPE_STATUS commitIPPingDiagnostics(CWMPObject *o, Instance *ip, eCommitCBType cmt){

	if (cmt==COMMIT_SET && cpePingParam.pendingState == eRequested) {
		if (cpePingParam.host == NULL ){
			return CPE_ERR;
		}
		if ( cpePingParam.state == eRequested ) {
			/* ping is running and a new request has been made */
			/* stop ping and restart */
			cpeStopPing((void*)eRequested);
			cpePingParam.pendingState = eNone;
			cpeStartPing(NULL);
		} else {
			/* No ping running */
			/* schedule callback to startPing when ACS session ends */
			cpePingParam.state = eRequested;
			cpePingParam.pendingState = eNone;
			setCallback(&acsSession, cpeStartPing, NULL );
			cpePingParam.interface = NULL; /* no interface in this data model */
		}
	} else if (cpePingParam.state == eRequested ){
		/* a writeable param was set while ping is running. */
		/* Stop ping and set state to eNone */
		cpeStopPing(eNone);
		cpePingParam.pendingState = eNone;
	}
	return CPE_OK;
}
/**@param IPPingDiagnostics_DiagnosticsState                     **/
CPE_STATUS setIPPingDiagnostics_DiagnosticsState(Instance *ip, char *value)
{
	if (strcasecmp(value, "requested") == 0 ){
		cpePingParam.pendingState = eRequested;
		return CPE_OK;
	}
	return CPE_9003;
}
CPE_STATUS getIPPingDiagnostics_DiagnosticsState(Instance *ip, char **value)
{
	switch (cpePingParam.state) {
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
			*value = GS_STRDUP("Complete");
			break;
		case eErrorInternal:
			*value = GS_STRDUP("Error_Internal");
			break;
		case eErrorOther:
			*value = GS_STRDUP("Error_Other");
			break;
		default:
			break;
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param IPPingDiagnostics_Interface                     **/
CPE_STATUS setIPPingDiagnostics_Interface(Instance *ip, char *value)
{
	if (cpePingParam.interface) GS_FREE(cpePingParam.interface);
	if (!EMPTYSTR(value))
		COPYSTR(cpePingParam.interface, value);
	return CPE_OK;
}
CPE_STATUS getIPPingDiagnostics_Interface(Instance *ip, char **value)
{
	/* get parameter */
	if (cpePingParam.interface)
		*value = GS_STRDUP(cpePingParam.interface);
	return CPE_OK;
}
/**@endparam                                               **/
/**@param IPPingDiagnostics_Host                     **/
CPE_STATUS setIPPingDiagnostics_Host(Instance *ip, char *value)
{
	if (cpePingParam.state == eRequested ){
		cpeStopPing((void*)eNone);
	} else {
		if (cpePingParam.host)
			GS_FREE(cpePingParam.host);
		cpePingParam.host = GS_STRDUP(value);
	}
	return CPE_OK;
}
CPE_STATUS getIPPingDiagnostics_Host(Instance *ip, char **value)
{
	/* get parameter */
	if (cpePingParam.host)
		*value = GS_STRDUP(cpePingParam.host);
	return CPE_OK;
}
/**@endparam                                               **/
/**@param IPPingDiagnostics_NumberOfRepetitions                     **/
CPE_STATUS setIPPingDiagnostics_NumberOfRepetitions(Instance *ip, char *value)
{
	int v = atoi(value);
	if ( v != 0) {
		if (cpePingParam.state == eRequested ){
			cpeStopPing((void*)eNone);
		}
		cpePingParam.reps = v;
		return CPE_OK;
	} else
		return CPE_9007;
}
CPE_STATUS getIPPingDiagnostics_NumberOfRepetitions(Instance *ip, char **value)
{
	/* get parameter */
		*value = GS_STRDUP(itoa(cpePingParam.reps));
	return CPE_OK;
}
/**@endparam                                               **/
/**@param IPPingDiagnostics_Timeout                     **/
CPE_STATUS setIPPingDiagnostics_Timeout(Instance *ip, char *value)
{
	int v = atoi(value);
	if ( v != 0) {
		if (cpePingParam.state == eRequested ){
			cpeStopPing((void*)eNone);
		}
		cpePingParam.timeout = v;
		return CPE_OK;
	} else
		return CPE_9007;
}
CPE_STATUS getIPPingDiagnostics_Timeout(Instance *ip, char **value)
{
	/* get parameter */
	*value = GS_STRDUP(itoa(cpePingParam.timeout));
	return CPE_OK;
}
/**@endparam                                               **/
/**@param IPPingDiagnostics_DataBlockSize                     **/
CPE_STATUS setIPPingDiagnostics_DataBlockSize(Instance *ip, char *value)
{
	int v = atoi(value);
	if ( v != 0) {
		if (cpePingParam.state == eRequested ){
			cpeStopPing((void*)eNone);
		}
		cpePingParam.blockSize = v;
		return CPE_OK;
	} else
		return CPE_9007;
}
CPE_STATUS getIPPingDiagnostics_DataBlockSize(Instance *ip, char **value)
{
	/* get parameter */
	*value = GS_STRDUP(itoa(cpePingParam.blockSize));
	return CPE_OK;
}
/**@endparam                                               **/
/**@param IPPingDiagnostics_DSCP                     **/
CPE_STATUS setIPPingDiagnostics_DSCP(Instance *ip, char *value)
{
	int v = atoi(value);
	if ( v >= 0 && v<=63) {
		if (cpePingParam.state == eRequested ){
			cpeStopPing((void*)eNone);
		}
		cpePingParam.dscp = v;
		return CPE_OK;
	} else
		return CPE_9007;
	return CPE_OK;
}
CPE_STATUS getIPPingDiagnostics_DSCP(Instance *ip, char **value)
{
	/* get parameter */
	 *value = GS_STRDUP(itoa(cpePingParam.dscp));
	return CPE_OK;
}
/**@endparam                                               **/
/**@param IPPingDiagnostics_SuccessCount                     **/
CPE_STATUS getIPPingDiagnostics_SuccessCount(Instance *ip, char **value)
{
	*value = GS_STRDUP( itoa( cpePingParam.rcvdCnt ));
	return CPE_OK;
}
/**@endparam                                               **/
/**@param IPPingDiagnostics_FailureCount                     **/
CPE_STATUS getIPPingDiagnostics_FailureCount(Instance *ip, char **value)
{
	*value = GS_STRDUP( itoa( cpePingParam.reps-cpePingParam.rcvdCnt ));
	return CPE_OK;
}
/**@endparam                                               **/
/**@param IPPingDiagnostics_AverageResponseTime                     **/
CPE_STATUS getIPPingDiagnostics_AverageResponseTime(Instance *ip, char **value)
{
	/* get parameter */
	 *value = GS_STRDUP(itoa( cpePingParam.avgRsp));
	return CPE_OK;
}
/**@endparam                                               **/
/**@param IPPingDiagnostics_MaximumResponseTime                     **/
CPE_STATUS getIPPingDiagnostics_MaximumResponseTime(Instance *ip, char **value)
{
	/* get parameter */
	 *value = GS_STRDUP(itoa(cpePingParam.maxRsp));
	return CPE_OK;
}
/**@endparam                                               **/
/**@param IPPingDiagnostics_MinimumResponseTime                     **/
CPE_STATUS getIPPingDiagnostics_MinimumResponseTime(Instance *ip, char **value)
{
	/* get parameter */
	 *value = GS_STRDUP(itoa( cpePingParam.minRsp ));
	return CPE_OK;
}
/**@endparam                                               **/
/**@endobj IPPingDiagnostics **/





