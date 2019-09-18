/*
 *----------------------------------------------------------------------*
 * Gatespace
 * Copyright 2006 Gatespace Networks, Inc., All Rights Reserved.
 * Gatespace Networks, Inc. confidential material.
 *----------------------------------------------------------------------*
 * File Name  :  voucherMgmt.c
 * Description:	 Implementation stubs for cpeSetVouchers and cpeGetOptions.
 *
 *----------------------------------------------------------------------*
 * $Revision: 1.3 $
 *
 * $Id: voucherMgmt.c,v 1.3 2009/09/03 14:46:19 dmounday Exp $
 *----------------------------------------------------------------------*/
#include	<string.h>
#include 	<time.h>
#include 	<sys/types.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <syslog.h>
#include "../gslib/src/utils.h"
#include "../includes/sys.h"
#include "../includes/paramTree.h"
#include "../includes/rpc.h"
#include "../soapRpc/rpcMethods.h"
#include "../soapRpc/rpcUtils.h"
#include "../soapRpc/notify.h"
#include "../includes/CPEWrapper.h"
/*
 * cpeSetVouchers( SetVoucherMsg * )
 * Returns:
 *          0: A internal error is returned
 *     >=9000: The fault code returned.
 *           1: Returns a valid SetVouchersResponse
 *
 */

int cpeSetVouchers( SetVouchersMsg *mp ){
	/*
	 * *mp points at the linked list of SetVoucherMsg.
	 */
	return 1;
}

/*
 * Returns a linked list of CPEOption structures that will be
 * freed by the framework. If optName is not NULL return the specific
 * option by that name; otherwise, return a linked list of all
 * options.
 * Returns NULL for error.
 */
CPEOption *cpeGetOptions( char *optName ){

	/* create an option of testing */
	CPEOption *p;
	if ( (p = (CPEOption *)GS_MALLOC( sizeof( CPEOption))) ){
		memset( p, 0, sizeof(CPEOption));
		strcpy(p->optionName, "SampleOption");
		p->voucherSN = 12345678;
		p->state = 0;
		p->startDate = getCurrentTime();
		p->expirationDate = p->startDate+84000;
		p->transferable = 1;
	}
	return p;
}
