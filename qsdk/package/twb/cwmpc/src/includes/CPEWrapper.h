/*----------------------------------------------------------------------*
 * Gatespace Networks, Inc.
 * Copyright 2005-2011 Gatespace Networks, Inc., All Rights Reserved.
 * Gatespace Networks, Inc. confidential material.
 *----------------------------------------------------------------------*
 * File Name  : CPEWrapper.h
 *
 * Description: Functions that wrap the CPE services:
 *																		*
 * $Revision: 1.15 $
 * $Id: CPEWrapper.h,v 1.15 2012/05/10 17:37:59 dmounday Exp $
 *----------------------------------------------------------------------*/
#include <stdarg.h>
#include "../gslib/src/utils.h"
#include "../includes/rpc.h"
#include "../soapRpc/rpcMethods.h"

/*
 * These manifests are used to index the
 * fault message table, cpeFaultTable[], which is defined in
 * faultMessages.c.
 *
 */

#define FAULT9000	0	/*Method not Supported */
#define FAULT9001	1	/*Request Denied	*/
#define FAULT9002	2	/*Internal Error*/
#define FAULT9003	3	/*Invalid Arguments*/
#define FAULT9004	4	/*Resources Exceeded*/
#define FAULT9005	5	/*Invalid Parameter Name*/
#define FAULT9006	6	/*Invalid Parameter Type*/
#define FAULT9007	7	/*Invalid Parameter Value*/
#define FAULT9008	8	/*Attempt to set a non-writable parameter*/
#define FAULT9009	9	/*Notification request rejected */
#define FAULT9010	10	/*Download failure*/
#define FAULT9011	11	/*Upload failure*/
#define FAULT9012	12	/*File transfer server authentication failure*/
#define FAULT9013	13	/*Unsupported protocol for file transfer*/
#define FAULT9014	14	/*Download failure, Unable to join multicast group*/
#define FAULT9015	15	/*Download failure, unable to contact file server*/
#define FAULT9016	16	/*Download failure, unable to access file*/
#define FAULT9017	17	/*Download failure, unable to complete download*/
#define FAULT9018	18	/*Download failure, file corrupted*/
#define FAULT9019	19	/*Download failure, file authentication failure*/
#define FAULT9020	20	/*File Transfer failure, Unable to download within spcified time windows */
#define FAULT9021	21	/*Cancelation of file transfer not permitted in current transfer state*/
#define FAULT9022	22	/*Invalid UUID Format*/
#define FAULT9023	23	/*Unknown Execution Environment*/
#define FAULT9024	24	/*Disabled Execution Environment*/
#define FAULT9025	25	/*Deployment Unit to Execution Environment Missmatch*/
#define FAULT9026	26	/*Duplicate Deployment Unit*/
#define FAULT9027	27	/*System Resources Exceeded*/
#define FAULT9028	28	/*Unknown Deployment Unite*/
#define FAULT9029	29	/*Invalid Deployment Unit State*/
#define FAULT9030	30	/*Invalid Deployment Unit Update -- Downgrade not permitted*/
#define FAULT9031	31	/*Invalid Deployment Unit Update -- Version not specified*/
#define FAULT9032	32	/*Invalid Deployment Unit Update -- Version already exists*/
#define FAULTNONE	33	/* no fault fault code is 0 */
#define FAULT_END	33	/* null table entry */



typedef enum {
	eFaultUnknown,
	eClient,
	eServer
} FAULTTYPE;

/* Notification Attributes buffer image definition
*/
typedef struct PAttrib {
	int walkSeq;
	int savedAttr;
} PAttrib;

typedef struct PAttribImage {
	char signature[14];	/* NotifyAttrib*/
	int  aCount;         /* number of PAttrib */
	PAttrib attrib[];
} PAttribImage;


typedef struct CPEOption {
	struct CPEOption	*next;
	char	optionName[65];
	int		voucherSN;
	int		state;
	int		mode;
	int		startDate;
	int		expirationDate;
	int		transferable;
} CPEOption;

typedef enum {
	eACS_ACCESSABLE,
	eNOACCESS
} ACSACCESS;

#ifdef CONFIG_RPCSETVOUCHERS
CPEOption *cpeGetOptions(char *name);
int		cpeSetVouchers( SetVouchersMsg *);
#endif

void cpeBootUpCPEInit(void);
void cpeSaveCPEState(void);
int  cpeGetInstanceID(void);
void cpeRefreshCPEData( CPEState *);
void cpeRefreshInstances(void);
void CPEReboot(void *handle);
void cpeReboot(void);
ACSACCESS cpeGetACSAccessStatus(void);
#ifdef CONFIG_RPCFACTORYRESET
void cpeFactoryReset(void);
#endif
#ifdef CONFIG_RPCDOWNLOAD
int  cpeDownloadSetup( DownloadMsg *r);
int  cpeDownloadComplete( DownloadMsg *r, char *buf, int lth);
#endif
#ifdef CONFIG_RPCUPLOAD
int  cpeUploadSetup( DownloadMsg *r);
int  cpeGetUploadData( DownloadMsg *r, char **buf, int *lth);
void  cpeUploadComplete( DownloadMsg *r, char *buf);
#endif
#ifdef CONFIG_RPCSCHEDULEDOWNLOAD
typedef enum {
	eSTARTTRANSFER,
	eABORT,
	eSKIPWINDOW,
	eWAIT
}WINDOWACTION;
WINDOWACTION cpeDownloadWindowStart( DownloadMsg *r);
void cpeDownloadWindowEnd( DownloadMsg *r);
#endif

#ifdef CONFIG_RPCCHANGEDUSTATE
#include "../soapRpc/smm.h"
int cpeChangeDUState( CPEChangeDUState *cpeDU);
#endif /* #ifdef CONFIG_RPCCHANGEDUSTATE */

void cpeUnlockConfiguration(int changeCfgCnt );
void cpeLockConfiguration(void);
void cpeSaveNotifyAttributes( void *, int lth);
void *cpeRestoreNotifyAttributes( int *lth );
/*
 * Function defined in cpeFaultMessages.c to return
 * CPE fault details.
 */
const char *cpeGetFaultMessage(int faultCode);
FAULTTYPE cpeGetFaultType(int faultCode);
const char *cpeGetFaultIndexedMessage( int index );
int cpeGetFaultMsgIndex( int faultCode );


/*
* system wrapper functions
*/


void cpeInitLog(int flag);
void cpeLog(int level, const char* fmt, ...);
void cpeVlog(int level, const char* fmt, va_list ap);
void cpeSetDbgLogMask( int mask );
void cpeDbgLog(int mask, const char* fmt, ...);
