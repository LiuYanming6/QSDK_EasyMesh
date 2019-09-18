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
 * $Id: LANConfigSecurity.c,v 1.1 2012/05/10 17:38:00 dmounday Exp $
 *----------------------------------------------------------------------*/

#include "paramTree.h"
#include "rpc.h"
#include "gslib/src/utils.h"
#include "soapRpc/rpcUtils.h"
#include "soapRpc/rpcMethods.h"
#include "soapRpc/cwmpSession.h"

#include "LANConfigSecurity.h"

/**@obj LANConfigSecurity **/

/**@param LANConfigSecurity_ConfigPassword                     **/
CPE_STATUS setLANConfigSecurity_ConfigPassword(Instance *ip, char *value)
{
	LANConfigSecurity *p = (LANConfigSecurity *)ip->cpeData;
	if ( p ){
		COPYSTR(p->configPassword, value);
	}
	return CPE_OK;
}
CPE_STATUS getLANConfigSecurity_ConfigPassword(Instance *ip, char **value)
{
	LANConfigSecurity *p = (LANConfigSecurity *)ip->cpeData;
	if ( p ){
		if ( p->configPassword )
			*value = GS_STRDUP(p->configPassword);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@endobj LANConfigSecurity **/





