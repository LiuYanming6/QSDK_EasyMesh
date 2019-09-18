/*----------------------------------------------------------------------*
 * Gatespace
 * Copyright 2011, 2012 Gatespace Networks, Inc., All Rights Reserved.
 * Gatespace Networks, Inc. confidential material.
 *----------------------------------------------------------------------*
 * File Name  :
 * Description: Auto-generated getter/setter stubs file.
 *----------------------------------------------------------------------*
 * $Revision: 1.2 $
 *
 * $Id: SoftwareModules.c,v 1.2 2012/06/13 16:07:50 dmounday Exp $
 *----------------------------------------------------------------------*/

#include "paramTree.h"
#include "rpc.h"
#include "gslib/src/utils.h"
#include "soapRpc/rpcUtils.h"
#include "soapRpc/rpcMethods.h"
#include "soapRpc/cwmpSession.h"

#include "SoftwareModules.h"

/**@obj SoftwareModulesDeploymentUnit **/
CPE_STATUS  addSoftwareModulesDeploymentUnit(CWMPObject *o, Instance *ip)
{
	/* add instance data */
	SoftwareModulesDeploymentUnit *p = (SoftwareModulesDeploymentUnit *)GS_MALLOC( sizeof(struct SoftwareModulesDeploymentUnit));
	memset(p, 0, sizeof(struct SoftwareModulesDeploymentUnit));
	ip->cpeData = (void *)p;
	return CPE_OK;
}
CPE_STATUS  delSoftwareModulesDeploymentUnit(CWMPObject *o, Instance *ip)
{
	/* delete instance data */
	SoftwareModulesDeploymentUnit *p = (SoftwareModulesDeploymentUnit *)ip->cpeData;
	if( p ){
		//TODO: free instance data
		GS_FREE(p);
	}
	return CPE_OK;
}

/**@param SoftwareModulesDeploymentUnit_UUID                     **/
CPE_STATUS getSoftwareModulesDeploymentUnit_UUID(Instance *ip, char **value)
{
	SoftwareModulesDeploymentUnit *p = (SoftwareModulesDeploymentUnit *)ip->cpeData;
	if ( p ){
		if ( p->uUID )
			*value = GS_STRDUP(p->uUID);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param SoftwareModulesDeploymentUnit_DUID                     **/
CPE_STATUS getSoftwareModulesDeploymentUnit_DUID(Instance *ip, char **value)
{
	SoftwareModulesDeploymentUnit *p = (SoftwareModulesDeploymentUnit *)ip->cpeData;
	if ( p ){
		if ( p->dUID )
			*value = GS_STRDUP(p->dUID);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param SoftwareModulesDeploymentUnit_Name                     **/
CPE_STATUS getSoftwareModulesDeploymentUnit_Name(Instance *ip, char **value)
{
	SoftwareModulesDeploymentUnit *p = (SoftwareModulesDeploymentUnit *)ip->cpeData;
	if ( p ){
		if ( p->name )
			*value = GS_STRDUP(p->name);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param SoftwareModulesDeploymentUnit_Status                     **/
CPE_STATUS getSoftwareModulesDeploymentUnit_Status(Instance *ip, char **value)
{
	SoftwareModulesDeploymentUnit *p = (SoftwareModulesDeploymentUnit *)ip->cpeData;
	if ( p ){
		if ( p->status )
			*value = GS_STRDUP(p->status);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param SoftwareModulesDeploymentUnit_Resolved                     **/
CPE_STATUS getSoftwareModulesDeploymentUnit_Resolved(Instance *ip, char **value)
{
	SoftwareModulesDeploymentUnit *p = (SoftwareModulesDeploymentUnit *)ip->cpeData;
	if ( p ){
		*value = GS_STRDUP(p->resolved? "true": "false");
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param SoftwareModulesDeploymentUnit_URL                     **/
CPE_STATUS getSoftwareModulesDeploymentUnit_URL(Instance *ip, char **value)
{
	SoftwareModulesDeploymentUnit *p = (SoftwareModulesDeploymentUnit *)ip->cpeData;
	if ( p ){
		if ( p->uRL )
			*value = GS_STRDUP(p->uRL);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param SoftwareModulesDeploymentUnit_Vendor                     **/
CPE_STATUS getSoftwareModulesDeploymentUnit_Vendor(Instance *ip, char **value)
{
	SoftwareModulesDeploymentUnit *p = (SoftwareModulesDeploymentUnit *)ip->cpeData;
	if ( p ){
		if ( p->vendor )
			*value = GS_STRDUP(p->vendor);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param SoftwareModulesDeploymentUnit_Version                     **/
CPE_STATUS getSoftwareModulesDeploymentUnit_Version(Instance *ip, char **value)
{
	SoftwareModulesDeploymentUnit *p = (SoftwareModulesDeploymentUnit *)ip->cpeData;
	if ( p ){
		if ( p->version )
			*value = GS_STRDUP(p->version);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param SoftwareModulesDeploymentUnit_ExecutionUnitList                     **/
CPE_STATUS getSoftwareModulesDeploymentUnit_ExecutionUnitList(Instance *ip, char **value)
{
	SoftwareModulesDeploymentUnit *p = (SoftwareModulesDeploymentUnit *)ip->cpeData;
	if ( p ){
		*value = cwmpGetPathRefRowsStr(p->executionUnitList);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param SoftwareModulesDeploymentUnit_ExecutionEnvRef                     **/
CPE_STATUS getSoftwareModulesDeploymentUnit_ExecutionEnvRef(Instance *ip, char **value)
{
	SoftwareModulesDeploymentUnit *p = (SoftwareModulesDeploymentUnit *)ip->cpeData;
	if ( p ){
		*value = cwmpGetInstancePathStr(p->executionEnvRef);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@endobj SoftwareModulesDeploymentUnit **/

/**@obj SoftwareModulesExecEnv **/
CPE_STATUS  addSoftwareModulesExecEnv(CWMPObject *o, Instance *ip)
{
	/* add instance data */
	SoftwareModulesExecEnv *p = (SoftwareModulesExecEnv *)GS_MALLOC( sizeof(struct SoftwareModulesExecEnv));
	memset(p, 0, sizeof(struct SoftwareModulesExecEnv));
	ip->cpeData = (void *)p;
	return CPE_OK;
}
CPE_STATUS  delSoftwareModulesExecEnv(CWMPObject *o, Instance *ip)
{
	/* delete instance data */
	SoftwareModulesExecEnv *p = (SoftwareModulesExecEnv *)ip->cpeData;
	if( p ){
		//TODO: free instance data
		GS_FREE(p);
	}
	return CPE_OK;
}

/**@param SoftwareModulesExecEnv_Enable                     **/
CPE_STATUS setSoftwareModulesExecEnv_Enable(Instance *ip, char *value)
{
	SoftwareModulesExecEnv *p = (SoftwareModulesExecEnv *)ip->cpeData;
	if ( p ){
		p->enable=testBoolean(value);
	}
	return CPE_OK;
}
CPE_STATUS getSoftwareModulesExecEnv_Enable(Instance *ip, char **value)
{
	SoftwareModulesExecEnv *p = (SoftwareModulesExecEnv *)ip->cpeData;
	if ( p ){
		*value = GS_STRDUP(p->enable? "true": "false");
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param SoftwareModulesExecEnv_Status                     **/
CPE_STATUS getSoftwareModulesExecEnv_Status(Instance *ip, char **value)
{
	SoftwareModulesExecEnv *p = (SoftwareModulesExecEnv *)ip->cpeData;
	if ( p ){
		if ( p->status )
			*value = GS_STRDUP(p->status);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param SoftwareModulesExecEnv_Name                     **/
CPE_STATUS getSoftwareModulesExecEnv_Name(Instance *ip, char **value)
{
	SoftwareModulesExecEnv *p = (SoftwareModulesExecEnv *)ip->cpeData;
	if ( p ){
		if ( p->name )
			*value = GS_STRDUP(p->name);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param SoftwareModulesExecEnv_Type                     **/
CPE_STATUS getSoftwareModulesExecEnv_Type(Instance *ip, char **value)
{
	SoftwareModulesExecEnv *p = (SoftwareModulesExecEnv *)ip->cpeData;
	if ( p ){
		if ( p->type )
			*value = GS_STRDUP(p->type);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param SoftwareModulesExecEnv_Vendor                     **/
CPE_STATUS getSoftwareModulesExecEnv_Vendor(Instance *ip, char **value)
{
	SoftwareModulesExecEnv *p = (SoftwareModulesExecEnv *)ip->cpeData;
	if ( p ){
		if ( p->vendor )
			*value = GS_STRDUP(p->vendor);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param SoftwareModulesExecEnv_Version                     **/
CPE_STATUS getSoftwareModulesExecEnv_Version(Instance *ip, char **value)
{
	SoftwareModulesExecEnv *p = (SoftwareModulesExecEnv *)ip->cpeData;
	if ( p ){
		if ( p->version )
			*value = GS_STRDUP(p->version);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param SoftwareModulesExecEnv_ActiveExecutionUnits                     **/
CPE_STATUS getSoftwareModulesExecEnv_ActiveExecutionUnits(Instance *ip, char **value)
{
	SoftwareModulesExecEnv *p = (SoftwareModulesExecEnv *)ip->cpeData;
	if ( p ){
		*value = cwmpGetPathRefRowsStr(p->activeExecutionUnits);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@endobj SoftwareModulesExecEnv **/

/**@obj SoftwareModulesExecutionUnitExtensions **/
/**@endobj SoftwareModulesExecutionUnitExtensions **/

/**@obj SoftwareModulesExecutionUnit **/
CPE_STATUS  addSoftwareModulesExecutionUnit(CWMPObject *o, Instance *ip)
{
	/* add instance data */
	SoftwareModulesExecutionUnit *p = (SoftwareModulesExecutionUnit *)GS_MALLOC( sizeof(struct SoftwareModulesExecutionUnit));
	memset(p, 0, sizeof(struct SoftwareModulesExecutionUnit));
	ip->cpeData = (void *)p;
	return CPE_OK;
}
CPE_STATUS  delSoftwareModulesExecutionUnit(CWMPObject *o, Instance *ip)
{
	/* delete instance data */
	SoftwareModulesExecutionUnit *p = (SoftwareModulesExecutionUnit *)ip->cpeData;
	if( p ){
		//TODO: free instance data
		GS_FREE(p);
	}
	return CPE_OK;
}

/**@param SoftwareModulesExecutionUnit_EUID                     **/
CPE_STATUS getSoftwareModulesExecutionUnit_EUID(Instance *ip, char **value)
{
	SoftwareModulesExecutionUnit *p = (SoftwareModulesExecutionUnit *)ip->cpeData;
	if ( p ){
		if ( p->eUID )
			*value = GS_STRDUP(p->eUID);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param SoftwareModulesExecutionUnit_Name                     **/
CPE_STATUS getSoftwareModulesExecutionUnit_Name(Instance *ip, char **value)
{
	SoftwareModulesExecutionUnit *p = (SoftwareModulesExecutionUnit *)ip->cpeData;
	if ( p ){
		if ( p->name )
			*value = GS_STRDUP(p->name);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param SoftwareModulesExecutionUnit_ExecEnvLabel                     **/
CPE_STATUS getSoftwareModulesExecutionUnit_ExecEnvLabel(Instance *ip, char **value)
{
	SoftwareModulesExecutionUnit *p = (SoftwareModulesExecutionUnit *)ip->cpeData;
	if ( p ){
		if ( p->execEnvLabel )
			*value = GS_STRDUP(p->execEnvLabel);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param SoftwareModulesExecutionUnit_Status                     **/
CPE_STATUS getSoftwareModulesExecutionUnit_Status(Instance *ip, char **value)
{
	SoftwareModulesExecutionUnit *p = (SoftwareModulesExecutionUnit *)ip->cpeData;
	if ( p ){
		if ( p->status )
			*value = GS_STRDUP(p->status);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param SoftwareModulesExecutionUnit_RequestedState                     **/
CPE_STATUS cpeChangeRequestedState(SoftwareModulesExecutionUnit *p,	const char *value);

CPE_STATUS setSoftwareModulesExecutionUnit_RequestedState(Instance *ip, char *value)
{
	SoftwareModulesExecutionUnit *p = (SoftwareModulesExecutionUnit *)ip->cpeData;
	return cpeChangeRequestedState( p, value);
}
CPE_STATUS getSoftwareModulesExecutionUnit_RequestedState(Instance *ip, char **value)
{
	SoftwareModulesExecutionUnit *p = (SoftwareModulesExecutionUnit *)ip->cpeData;
	if ( p ){
		if ( p->requestedState )
			*value = GS_STRDUP(p->requestedState);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param SoftwareModulesExecutionUnit_ExecutionFaultCode                     **/
CPE_STATUS getSoftwareModulesExecutionUnit_ExecutionFaultCode(Instance *ip, char **value)
{
	SoftwareModulesExecutionUnit *p = (SoftwareModulesExecutionUnit *)ip->cpeData;
	if ( p ){
		if ( p->executionFaultCode )
			*value = GS_STRDUP(p->executionFaultCode);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param SoftwareModulesExecutionUnit_ExecutionFaultMessage                     **/
CPE_STATUS getSoftwareModulesExecutionUnit_ExecutionFaultMessage(Instance *ip, char **value)
{
	SoftwareModulesExecutionUnit *p = (SoftwareModulesExecutionUnit *)ip->cpeData;
	if ( p ){
		if ( p->executionFaultMessage )
			*value = GS_STRDUP(p->executionFaultMessage);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param SoftwareModulesExecutionUnit_Vendor                     **/
CPE_STATUS getSoftwareModulesExecutionUnit_Vendor(Instance *ip, char **value)
{
	SoftwareModulesExecutionUnit *p = (SoftwareModulesExecutionUnit *)ip->cpeData;
	if ( p ){
		if ( p->vendor )
			*value = GS_STRDUP(p->vendor);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param SoftwareModulesExecutionUnit_Version                     **/
CPE_STATUS getSoftwareModulesExecutionUnit_Version(Instance *ip, char **value)
{
	SoftwareModulesExecutionUnit *p = (SoftwareModulesExecutionUnit *)ip->cpeData;
	if ( p ){
		if ( p->version )
			*value = GS_STRDUP(p->version);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param SoftwareModulesExecutionUnit_References                     **/
CPE_STATUS getSoftwareModulesExecutionUnit_References(Instance *ip, char **value)
{
	SoftwareModulesExecutionUnit *p = (SoftwareModulesExecutionUnit *)ip->cpeData;
	if ( p ){
		*value = cwmpGetPathRefRowsStr(p->references);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param SoftwareModulesExecutionUnit_SupportedDataModelList                     **/
CPE_STATUS getSoftwareModulesExecutionUnit_SupportedDataModelList(Instance *ip, char **value)
{
	SoftwareModulesExecutionUnit *p = (SoftwareModulesExecutionUnit *)ip->cpeData;
	if ( p ){
		*value = cwmpGetPathRefRowsStr(p->supportedDataModelList);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@endobj SoftwareModulesExecutionUnit **/

/**@obj SoftwareModules **/

/**@param SoftwareModules_ExecEnvNumberOfEntries                     **/
CPE_STATUS getSoftwareModules_ExecEnvNumberOfEntries(Instance *ip, char **value)
{
	cwmpGetObjInstanceCntStr("ExecEnv", value);
	return CPE_OK;
}
/**@endparam                                               **/
/**@param SoftwareModules_DeploymentUnitNumberOfEntries                     **/
CPE_STATUS getSoftwareModules_DeploymentUnitNumberOfEntries(Instance *ip, char **value)
{
	cwmpGetObjInstanceCntStr("DeploymentUnit", value);
	return CPE_OK;
}
/**@endparam                                               **/
/**@param SoftwareModules_ExecutionUnitNumberOfEntries                     **/
CPE_STATUS getSoftwareModules_ExecutionUnitNumberOfEntries(Instance *ip, char **value)
{
	cwmpGetObjInstanceCntStr("ExecutionUnit", value);
	return CPE_OK;
}
/**@endparam                                               **/
/**@endobj SoftwareModules **/












