/*----------------------------------------------------------------------*
 * Gatespace
 * Copyright 2011 Gatespace Networks, Inc., All Rights Reserved.
 * Gatespace Networks, Inc. confidential material.
 *----------------------------------------------------------------------*
 * File Name  : smm.h
 * Description:	Software Module Management
 *----------------------------------------------------------------------*
 * $Revision: 1.3 $
 *
 * $Id: smm.h,v 1.3 2012/05/10 17:38:07 dmounday Exp $
 *----------------------------------------------------------------------*/
#ifndef SMM_H_
#define SMM_H_
#include "../includes/paramTree.h"

/**
 * The CWMPc passes the ChangeDUState linked list of operation
 * requests and the list of result structures
 * to the cpeChangeDUState() function in the CPEChangeDUState.
 * This structure is passed back to the CWMPc via call to the
 * cwmpDUStateChangeComplete() when the operations are complete.
 * The results of each operation request in a DUOPerationStruct
 * are assigned to the associated CPEOPResultStruct.
 * Note that it is possible for multiple sets of DU change requests
 * to be outstanding at once.
 */
typedef enum {
	eDUChangeWait,
	eDUChangeStarted,
	eDUChangeCompleted
}eDUChangeState;

#define MAX_EU	5

typedef struct CPEOpResultStruct {
	struct CPEOpResultStruct *next;
	char		*uuid;
	char		*duRef;
	char		*version;
	char		*currentState;
	char		resolved;
	char	 	*euRefList;
	time_t		startTime;
	time_t		completeTime;
	unsigned	faultCode;
	char		*faultString;
}CPEOpResultStruct;

typedef struct CPEChangeDUState {
	struct CPEChangeDUState	*next;
	eDUChangeState 	state;
	char		commandKey[COMMANDKEY_SZ];
	struct DUOperationStruct *ops;		// defined in rpc.h
	struct CPEOpResultStruct *results;
}CPEChangeDUState;

int cpeChangeDUState( CPEChangeDUState *);
CPEChangeDUState *cwmpGetCompletedDUChange(void);
void cwmpFreeDUChangeStruct( CPEChangeDUState *);
void cwmpDUStateChangeComplete(CPEChangeDUState *);
int cwmpQueueDUStateChange( RPCRequest *r);
void cwmpDUStateChangeAcked(void);
void cwmpSMMEventReceived(void);

#endif /* SMM_H_ */
