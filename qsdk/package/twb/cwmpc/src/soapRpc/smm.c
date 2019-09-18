/*----------------------------------------------------------------------*
 * Gatespace
 * Copyright 2011 Gatespace Networks, Inc., All Rights Reserved.
 * Gatespace Networks, Inc. confidential material.
 *----------------------------------------------------------------------*
 * File Name  : smm.c
 * Description:	Software Module Management
 *----------------------------------------------------------------------*
 * $Revision: 1.3 $
 *
 * $Id: smm.c,v 1.3 2012/05/10 17:38:07 dmounday Exp $
 *----------------------------------------------------------------------*/

#include <syslog.h>
#include "../includes/rpc.h"
#include "../includes/CPEWrapper.h"
#include "../gslib/src/utils.h"
#include "../gslib/src/event.h"

#include "cwmpSession.h"
#include "smm.h"

void cpeSMMEvent(void);//-Werror=implicit-function-declaration

extern EventPolicy eventPolicy[];
extern ACSSession acsSession;
/* Queue of pending changes. */
CPEChangeDUState *cwmpDUStateChangeQ;

/* Queue of completed changes */
CPEChangeDUState *cwmpDUStateChangeDoneQ;

typedef struct QItem {
	struct QItem *next;
}QItem;

/*
 * enqueue qItem at end of queue qHdr.
 */
static void enQueue( void *qHdr, void *qItem){
	if ( ((QItem *)qHdr)->next == NULL )
		((QItem *)qHdr)->next = (void *)qItem;
	else {
		QItem *nxt = *((QItem **)qHdr);
		QItem **link;
		while ( nxt!= NULL ){
			link = &nxt->next;
			nxt = nxt->next;
		}
		*link = qItem;
	}
	((QItem *)qItem)->next = NULL;
}



static QItem* deQueue( void *qHdr ) {
	QItem *item = ((QItem *)qHdr)->next;
	if ( item != NULL) {
		((QItem *)qHdr)->next = item->next;
		item->next = NULL;
	}
	return item;
}


/*
 * callback at end of acs session to start any change request if
 * not already started.
 */
static void startDUStateChange(void *handle){
	CPEChangeDUState *cDU = cwmpDUStateChangeQ;
	if ( cDU!= NULL && cDU->state==eDUChangeWait){
		cDU->state = eDUChangeStarted;
		cpeChangeDUState( cDU );
	}
	return;
}
/*
 * Make a copy of the RPC DU change info and
 * enqueue the DU state change request.
 */
static int callbackset = 0;

int cwmpQueueDUStateChange( RPCRequest *r ){
	/* copy DUOperationStruct to CPEChangeDUState and allocate and
	 * initialize the CPEOpResultStruct. Keep the linked list ordered.
	 */
	int error = 0;
	cpeDbgLog(DBG_CHANGEDU, "cwmpQueueDUStateChange()");
	CPEChangeDUState *cpeDU = (CPEChangeDUState *)GS_MALLOC(sizeof (struct CPEChangeDUState));
	DUOperationStruct **lastcp = &cpeDU->ops;
	CPEOpResultStruct *rp, **lastrp = &cpeDU->results;
	DUOperationStruct *duOp = r->ud.changeDUState.opList;
	while ( error == 0 && duOp != NULL){
		// allocate the DUChangeComplete results structure to enqueue with the request.
		if ( (rp = (CPEOpResultStruct *)GS_MALLOC(sizeof( struct CPEOpResultStruct )))){
			memset( rp, 0, sizeof(struct CPEOpResultStruct ));
			*lastrp = rp;
			lastrp = &rp->next;
			*lastcp = duOp;
			lastcp = &duOp->next;
			if ( duOp->uuid ) // init uuid if present.
				rp->uuid = GS_STRDUP( duOp->uuid );
				// if not present the uuid is calculated after DU is loaded and initialized.
			duOp = duOp->next;
		} else {
			error = 9004;
			rp = cpeDU->results;
			CPEOpResultStruct *nxt;
			while( rp ){  // clean up memory.
				nxt = rp->next;
				GS_FREE( rp );
				rp = nxt;
			}
			GS_FREE ( cpeDU );
			cpeDU = NULL;
		}
	}
	if ( error == 0 ) {
		r->ud.changeDUState.opList = NULL; // unlink from original request.
		strncpy( cpeDU->commandKey, r->commandKey, COMMANDKEY_SZ);
		cpeDU->state = eDUChangeWait;
		enQueue( &cwmpDUStateChangeQ, (QItem*)cpeDU);
		// requests are queued and then acted upon when session ends.
		if (!callbackset ) {
			setCallback(&acsSession, startDUStateChange, NULL );
			callbackset = 1;
		}
	}
	cpeDbgLog(DBG_CHANGEDU, "cwmpQueueDUStateChange(): change queued error =%d", error);
	return error;
}

/**
 * Called by the CPE when the ChangeDUState has completed. The CPEChangeDUState
 * is passed back to the CWMPc framework with the CPEOpResultStruct values set
 * for each requested operation.
 */


void cwmpDUStateChangeComplete(CPEChangeDUState *cpeDU ){
	cpeDbgLog(DBG_CHANGEDU, "cwmpDUStateChangeComplete()");
	if ( cpeDU == cwmpDUStateChangeQ){
		CPEChangeDUState *item = (CPEChangeDUState *)deQueue( &cwmpDUStateChangeQ );
		item->state = eDUChangeCompleted;
		enQueue( &cwmpDUStateChangeDoneQ, item);
		cwmpAddEvent(eEvtDUStateChangeComplete);
		if (!cwmpIsACSSessionActive())
			notifyCallbacks(eventPolicy);
	} else
		cpeLog(LOG_ERR, "Incorrect CPEChangeDUState structure returned");
}
/*
 * Return first completed item from done queue. Caller must call cwmpFreeDUChangeStruct
 * to free the memory
 */
CPEChangeDUState *cwmpGetCompletedDUChange(void){
	CPEChangeDUState *item = cwmpDUStateChangeDoneQ;
	return item;
}
/**
 * free the memory allocated for the ChangeDUState request and the associated
 * results.
 */
void cwmpFreeDUChangeStruct( CPEChangeDUState *item ){
	DUOperationStruct *duOp = item->ops;
	while( duOp ){
		DUOperationStruct *nxt = duOp->next;
		GS_FREE( duOp->uuid);
		GS_FREE( duOp->version);
		GS_FREE( duOp->execEnvRef);
		GS_FREE( duOp->url);
		GS_FREE( duOp->userName);
		GS_FREE( duOp->passWord);
		GS_FREE( duOp);
		duOp = nxt;
	}
	CPEOpResultStruct *rp = item->results;
	while ( rp ){
		CPEOpResultStruct *nxt = rp->next;
		GS_FREE(rp->euRefList);
		GS_FREE(rp->uuid);
		GS_FREE(rp->duRef);
		GS_FREE(rp->version);
		GS_FREE(rp->currentState);
		GS_FREE(rp->faultString);
		GS_FREE ( rp );
		rp = nxt;
	}
	GS_FREE ( item );
}
/**
 * called by session when the DUStateChangeCompleteResponse is received.
 */
void cwmpDUStateChangeAcked(void){
	CPEChangeDUState *item = (CPEChangeDUState *) deQueue( &cwmpDUStateChangeDoneQ );
	if ( item )
		cwmpFreeDUChangeStruct( item );
	else
		cpeLog(LOG_ERR, "cwmpDUStateChangeAcked: No CPEChangeDUState on done queue");
}

/**
 * This is called by the cpe event listener when a CPE_SMM_EVENT is received
 * from the CPE's SMM management processes.
 */
void cwmpSMMEventReceived(void){
	cpeDbgLog(DBG_CHANGEDU,"cwmpSMMEventReceived()\n");
	// TODO: check if session is in progress and use the notifyCallback of acsSession if busy.
	cpeSMMEvent();
}
/*
*  These functions are translated from the Apache Java implementation of UUID.
*  Only the ones needed for CWMPc have been translated.
*/
#include <stdint.h>
#include <openssl/sha.h>

#define UUID_SZ 20
#define UUID_BYTE_LTH 16
#define TIME_HI_AND_VERSION_BYTE_6 6
#define CLOCK_SEQ_HI_AND_RESERVED_BYTE_8 8

#define NAME_MAX_SZ	64+128+2+1
//typedef unsigned char UUID;

// From TR-069 Amendment 3, Annex H.2:
//#define NAMESPACE "6ba7b810-9dad-11d1-80b4-00c04fd430c8"
static uint8_t nameSpace[UUID_BYTE_LTH+1] = {
		0x6b, 0xa7, 0xb8, 0x10,
		0x9d, 0xad,
		0x11, 0xd1,
		0x80, 0xb4,
		0x00, 0xc0, 0x4f, 0xd4, 0x30, 0xc8
		};

/*
 * encoding is always SHA1_ENCODING for SMM UUID usage.
 * param name is the DeploymentUnit.{i}.Name + DeploymentUnit.{i}.Vendor
 * Such as "sample1.gatespace.com."
 */
const char *cwmpGenerateUUID( const char *name ){

	uint8_t cName[NAME_MAX_SZ];
	uint8_t msgDigest[20];
	int	lth;
    static char digBuf[UUID_BYTE_LTH*2+4];
    uint8_t raw[UUID_BYTE_LTH];
    fprintf(stderr, "cwmpGenerateUUID: name=%s\n", name);
    memcpy(cName, nameSpace, UUID_BYTE_LTH+1);
    lth = strlen(name);
    memcpy(&cName[UUID_BYTE_LTH], name, lth );
    lth = lth + UUID_BYTE_LTH;
    SHA1( cName, lth, msgDigest );
    memcpy(raw, msgDigest, UUID_BYTE_LTH);
    raw[TIME_HI_AND_VERSION_BYTE_6]&= 0x0F;
    raw[TIME_HI_AND_VERSION_BYTE_6]|= (3<<4); //version 3
    raw[CLOCK_SEQ_HI_AND_RESERVED_BYTE_8] &= 0x3f;
    raw[CLOCK_SEQ_HI_AND_RESERVED_BYTE_8]|= 0x80;
	toAsciiHex(msgDigest, 4, digBuf);
	digBuf[8]='-';
	toAsciiHex(msgDigest+4, 2, digBuf+9);
	digBuf[13]='-';
	toAsciiHex(msgDigest+6, 2, digBuf+14);
	digBuf[18]='-';
	toAsciiHex(msgDigest+8, 2, digBuf+19);
	digBuf[23]='-';
	toAsciiHex(msgDigest+10, 6, digBuf+24);
	digBuf[35] = '\0'; //-Werror=array-bounds
	return digBuf;
}

