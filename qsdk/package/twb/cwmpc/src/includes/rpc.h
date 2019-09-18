/*----------------------------------------------------------------------*
 * Gatespace
 * Copyright 2005-2011 Gatespace. All Rights Reserved.
 * Gatespace Networks, Inc. confidential material.
 *----------------------------------------------------------------------*
 * File Name  : rpc.h
 *
 * Description: CWMP RPC definitions and data structures
 * $Revision: 1.15 $
 * $Id: rpc.h,v 1.15 2012/06/13 11:16:10 dmounday Exp $
 *----------------------------------------------------------------------*/
#ifndef _RPC_H_
#define _RPC_H_
#include "time.h"
#include "sys.h"
/*
 * ACS Session state
 */
#define ACS_SESSION_ENABLED		1
#define ACS_SESSION_DISABLED	0

/* CPEState.eventMask - one or more may be present */
#define EVT_BOOTSTRAP	0x0000001	/* all zero bit is bootstrap */
#define EVT_BOOT		0X0000002	/*        */
#define EVT_VALUECHANGE	0x0000004
#define EVT_XFERCOMPL	0x0000008
#define EVT_ADDOBJECT	0x0000010
#define EVT_DELOBJECT	0x0000020
#define EVT_REBOOT		0x0000040
#define EVT_DOWNLOAD	0x0000080
#define EVT_UPLOAD		0x0000100
#define EVT_SETPARAMVAL 0x0000200
#define EVT_AUTOXFRCMPL	0x0000800

typedef enum {
	eNOTIFYOFF = 0,
	eNOTIFYPASSIVE =1,
	eNOTIFYACTIVE=2
}eNOTIFICATION;

/* The return value from the getter/setter functions are the values
 * CPE_OK thru CPE_REBOOT.
 * The Object Commit functions can return an additional indicator to
 * stop the post-order traversal of commit calls. This is a mask bit
 * that is or'ed with the CPE_xxx status value return by the
 * Commit function. It is masked off by the execCommit() function.
 */
#define COMMIT_STOP	0x04000000
#define STATUS_MASK	0x0000ffff
typedef enum {
	CPE_OK = 0,
	CPE_REBOOT,
	CPE_ERR,
    CPE_9000 = 9000,
    CPE_9001,
    CPE_9002,
    CPE_9003,
    CPE_9004,
    CPE_9005,
    CPE_9006,
    CPE_9007,
    CPE_9008,
    CPE_9009,
    CPE_9010,
    CPE_9011,
    CPE_9012,
    CPE_9013,
    CPE_9014,				/* 9014.. 9019 added in v1.1 schema*/
    CPE_9015,
    CPE_9016,
    CPE_9017,
    CPE_9018,
    CPE_9019,
    CPE_9020,
    CPE_9021,
    CPE_9022,
    CPE_9023,
    CPE_9024,
    CPE_9025,
    CPE_9026,
    CPE_9027,
    CPE_9028,
    CPE_9029,
    CPE_9030,
    CPE_9031,
    CPE_9032,
    CPE_VNDR_START = 9800,
    CPE_VNDR_END = 9899
} CPE_STATUS;

/* TR-69 RPC methods */
						/* note methods returned in the GetRPCMethods*/
						/* response are between eGetRPCMethods and   */
						/* eFactoryReset inclusive*/
typedef enum {
	eUnspecified=0,
	eGetRPCMethods,
	eGetParameterNames,
	eGetParameterValues,
    eSetParameterValues,
	eGetParameterAttributes,
	eSetParameterAttributes,
	eAddObject,               /*      ...                           */
	eDeleteObject,            /*      ...                           */
	eDownload,                /*      ...                           */
    eUpload,
	eReboot,                  /*      ...                           */
    eFactoryReset,
    eGetQueuedTransfers,
    eScheduleInform,
    eSetVouchers,
    eGetOptions,
    eGetAllQueuedTransfers,
    eScheduleDownload,
    eCancelTransfer,
    eChangeDUState,
    /**********************************************************************/
	eInformResponse,          /* Responses, all responses must follow here*/
	eTransferCompleteResponse,
	eGetRPCMethodsResponse,
	eRequestDownloadResponse,
	eKicked,
    eKickedResponse,
    eAutonomousTransferComplete,
    eAutonomousTransferCompleteResponse,
    eDUStateChangeCompleteResponse,
    eAutonomousDUStateChangeCompleteResponse,
    eScheduleDownloadResponse,
    eCancelTransferResponse,
    eChangeDUStateResponse,
	eFault					/* catches faults from ACS */
} eRPCMethods;

typedef struct ParameterNameStruct {
	struct ParameterNameStruct  *next;
	char    *pName;
} ParameterNameStruct;

typedef struct ParameterValueStruct {		/* linked list element of parameter name */
	struct ParameterValueStruct *next;		/* value pairs */
	char    *pName;
	char    *value;
	char    *origValue;
	int		fault;						/* fault value on setter call */
	CPE_STATUS	cpeStatus;				/* save CPE_STATUS here on bad setter return */
} ParameterValueStruct;

typedef struct ParameterAttributesStruct {
	struct ParameterAttributesStruct    *next;
	char    *pName;
	int     notifyChange;
	eNOTIFICATION     notification;
	int     accessListChange;
	int     accessList;
} ParameterAttributesStruct;

typedef struct GetRPCMethodsMsg {
} GetRPCMethodsMsg;


typedef struct SetParameterValuesMsg {
	int                     paramCnt; /* actual count */
	ParameterValueStruct    *pvList;
	ParameterNameStruct     *autoAdd;	/* auto added instances */
} SetParameterValuesMsg;

typedef struct GetParameterAVMsg {
	int                 paramCnt;
	ParameterNameStruct *pnameList;
} GetParameterAVMsg;

typedef struct GetParameterNamesMsg {
	char    *pNamePath;
	int     nextLevel;
} GetParameterNamesMsg;

typedef struct SetParameterAttributesMsg {
	int                         paramCnt; /* actual count */
	ParameterAttributesStruct   *paList;
}SetParameterAttributesMsg;

typedef struct AddDelObjectMsg {
	char    *objectPath;
} AddDelObjectMsg;

// TimeWindow for ScheduledDownload
typedef struct TimeWindow {
	unsigned	windowStart;
	unsigned	windowEnd;
	char		windowMode[65];
	char		*userMessage;
	int			maxRetries;
}TimeWindow;

#define MAX_TIMEWINDOWS		2
#define WAITINGQ 0				/* flags for queue destination */
#define ACTIVEQ	1
#define DLMsg ud.downloadReq
typedef struct DownloadMsg {            /* also used for Upload */
	char	*fileType;
	char    *URL;
	char    *userName;
	char    *passWord;
	int     fileSize;		/* only a suggested size */
	char    *targetFileName;
	int     delaySeconds;	 /* also isDownLoad */
	char    *successURL;     /* also AnnounceURL */
	char    *failureURL;     /* also TransferURL */
	time_t	targetTime;
    time_t  dlStartTime;
    time_t  dlEndTime;
    int     dlStatus;
    int		dlFaultMsg;
#ifdef CONFIG_RPCUPLOAD
    const char *content_type;	/* used to specify upload content-type*/
#endif
#ifdef CONFIG_RPCSCHEDULEDOWNLOAD
    int		timeIndex;
    TimeWindow timeWindow[MAX_TIMEWINDOWS];
#endif
} DownloadMsg;

#ifdef CONFIG_RPCCHANGEDUSTATE
typedef enum {
	eNoOp,
	eInstallOp,
	eUpdateOp,
	eUninstallOp
}eDUCHANGEOP;

typedef struct DUOperationStruct {
	struct 	DUOperationStruct	*next;
	eDUCHANGEOP	op;			// Operation determines struct component usage.
	char		*uuid;
	char		*version;
	char		*execEnvRef;
	char		*url;
	char		*userName;
	char		*passWord;
} DUOperationStruct;

typedef struct ChangeDUStateMsg {
	int			operationCnt;
	DUOperationStruct	*opList;
}ChangeDUStateMsg;

#endif /* CONFIG_RPCCHANGEDUSTATE */

typedef struct RebootMsg {
} RebootMsg;

typedef struct ScheduleInformMsg {
	int     delaySeconds;
} ScheduleInformMsg;

typedef struct FactoryResetMsg {
}FactoryResetMsg;

typedef struct KickedResponseMsg {
    char    *nextURL;
}KickedResponseMsg;

typedef struct FaultMsg {
	int     faultCode;
	char    *faultString;
}FaultMsg;

typedef struct Voucher {
	struct Voucher		*next;
	char		*pVoucher;
} Voucher;

typedef struct SetVouchersMsg {
	int			voucherCnt;
	Voucher		*voucherList;
} SetVouchersMsg;

typedef struct GetOptionsMsg {
	char		*pOptionName;
} GetOptionsMsg;

typedef struct CancelTransferMsg {
} CancelTransferMsg;

#define PARAMETERKEY_SZ 33				/* 32 plus 1 for null */
#define COMMANDKEY_SZ   33

typedef struct RPCRequest {
	struct RPCRequest  *next;			/* only used if queueing required */
	char        *ID;					/* SOAP request ID */
	int         holdRequests;
	int         maxEnvelopes;
	char        commandKey[COMMANDKEY_SZ];
	char        parameterKey[PARAMETERKEY_SZ];
	int         arrayTypeSize;
	eRPCMethods rpcMethod;
	union {
		SetParameterValuesMsg       setPValuesReq;
		GetParameterAVMsg           getPAVReq;		/* attr or value */
		GetParameterNamesMsg        getPNamesReq;
		SetParameterAttributesMsg   setPAttrReq;
		AddDelObjectMsg             addDelObjectReq;
		DownloadMsg                 downloadReq;
		RebootMsg                   rebootReq;
		ScheduleInformMsg           scheduleInformReq;
		FactoryResetMsg             factoryResetReq;
        KickedResponseMsg           kickedResponseReq;
		FaultMsg                    faultMsg;
		SetVouchersMsg				setVouchersReq;
		GetOptionsMsg				getOptionsReq;
		CancelTransferMsg			cancelTransferMsg;
#ifdef	CONFIG_RPCCHANGEDUSTATE
		ChangeDUStateMsg			changeDUState;
#endif
	} ud;
} RPCRequest;








#endif   /*** _RPC_H_******/
