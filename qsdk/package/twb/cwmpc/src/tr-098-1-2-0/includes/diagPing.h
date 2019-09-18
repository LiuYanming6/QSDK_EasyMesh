/*
 * diagPing.h
 *
 *  Created on: Feb 10, 2009
 *      Author: dmounday
 */

#ifndef DIAGPING_H_
#define DIAGPING_H_

typedef enum DiagState {
	eStateNone = 0,
	eHostError,
	eMaxHopExceeded,
	eErrorInternal,
	eErrorOther,
	eComplete =10,
	eRequested
}DiagState;

typedef struct PingParam{
	DiagState  state;
	DiagState	pendingState;
	char       *host;
	char	   *interface;	/* Data model object.parameter interface name */
	char       *ifname;		/* local CPE I/F name */
	int		   blockSize;
	int        timeout;
	int        reps;
	unsigned char   dscp;
	int        rcvdCnt;
	int        failures;
	int        minRsp;
	int        maxRsp;
	int		   avgRsp;
	int        totRsp;
	FILE       *fp;
} PingParam;

void cpeStopPing( void * );
void cpeStartPing(void *);

#endif /* DIAGPING_H_ */
