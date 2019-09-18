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
 * $Id: InterfaceStack.c,v 1.2 2012/06/13 16:07:50 dmounday Exp $
 *----------------------------------------------------------------------*/

#include "paramTree.h"
#include "rpc.h"
#include "gslib/src/utils.h"
#include "soapRpc/rpcUtils.h"
#include "soapRpc/rpcMethods.h"
#include "soapRpc/cwmpSession.h"

#include "InterfaceStack.h"

/**@obj InterfaceStack **/
/* The InterfaceStack is a eCPEInstance type.
 * The add/del functions can not be called by the RPC execution framework.
 * There are used here to aid in restoring the configuration and clean-up when
 * the interface stack is changed.
 */
CPE_STATUS  addInterfaceStack(CWMPObject *o, Instance *ip)
{
	/* add instance data */
	InterfaceStack *p = (InterfaceStack *)GS_MALLOC( sizeof(struct InterfaceStack));
	memset(p, 0, sizeof(struct InterfaceStack));
	ip->cpeData = (void *)p;
	return CPE_OK;
}
CPE_STATUS  delInterfaceStack(CWMPObject *o, Instance *ip)
{
	/* delete instance data */
	InterfaceStack *p = (InterfaceStack *)ip->cpeData;
	if( p ){
		GS_FREE(p);
	}
	return CPE_OK;
}

/**@param InterfaceStack_HigherLayer                     **/
CPE_STATUS setInterfaceStack_HigherLayer(Instance *ip, char *value)
{
	InterfaceStack *p = (InterfaceStack *)ip->cpeData;
	if ( p ){
		p->higherLayer = cwmpGetInstancePtr(value);
	}
	return CPE_OK;
}
CPE_STATUS getInterfaceStack_HigherLayer(Instance *ip, char **value)
{
	InterfaceStack *p = (InterfaceStack *)ip->cpeData;
	if ( p ){
		*value = cwmpGetInstancePathStr(p->higherLayer);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param InterfaceStack_LowerLayer                     **/
CPE_STATUS setInterfaceStack_LowerLayer(Instance *ip, char *value)
{
	InterfaceStack *p = (InterfaceStack *)ip->cpeData;
	if ( p ){
		p->lowerLayer = cwmpGetInstancePtr(value);
	}
	return CPE_OK;
}
CPE_STATUS getInterfaceStack_LowerLayer(Instance *ip, char **value)
{
	InterfaceStack *p = (InterfaceStack *)ip->cpeData;
	if ( p ){
		*value = cwmpGetInstancePathStr(p->lowerLayer);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@endobj InterfaceStack **/












