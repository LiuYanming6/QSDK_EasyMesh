#ifndef _RPCMETHODS_H_
#define _RPCMETHODS_H_
/*----------------------------------------------------------------------*
 * Gatespace
 * Copyright 2006 Gatespace Networks, Inc., All Rights Reserved.
 * Gatespace Networks, Inc. confidential material.
 *----------------------------------------------------------------------*
 * File Name  : rpcMethods.h
 * Description:	rpcMethods implementation
 *----------------------------------------------------------------------*
 * $Revision: 1.17 $
 *
 * $Id: rpcMethods.h,v 1.17 2012/06/13 11:34:33 dmounday Exp $
 *----------------------------------------------------------------------*/
/* This is the status for the completion of an ACS session
 *  which may include multiple connections (eHttpState).
 */

#include "../gslib/src/xmlWriter.h"
#include "../gslib/src/utils.h"

typedef enum {
    eAuthError,
    eConnectError,
    ePostError,
    eRedirectError,
    eFaultResponse,
    eAcsDone,
    eInternalError,
    eSOAPError
} eSessionStatus;

typedef enum {
    eIdle,
    eAuthenticating,
    eAuthenticated,
    eAuthFailed,
    eShutdown
} eAUTHSTATE;
/* The eHttpState is the state of the currnet connection to
 * the ACS.
 */
typedef enum {
    eClosed,	/* connection is closed */
    eClose,		/* set Connection: close on next send */
    eStart,		/* connection is connecting */
    eConnected,	/* connection has completed */
} eHTTPSTATE;

typedef enum {
    eNoNullSent,
    eSentNull
} eNullMsgState;

#define CRPATH_SZ	12
#define CRURL_SZ  	IP_ADDRSTRLEN+CRPATH_SZ+14


/*
 * Data that needs to be persistent
 */
typedef struct CPEState {
    unsigned eventMask;		/* saved event mask */
    int		acsContactState;	/* enable/disable inform sessions */
    char    rebootCommandKey[COMMANDKEY_SZ];
    char    parameterKey[PARAMETERKEY_SZ];
    #ifdef  CONFIG_RPCSCHEDULEINFORM
    char    scheduleInformCommandKey[COMMANDKEY_SZ];
    #endif
    char    *acsURL;
    char    *acsUser;
    char    *acsPW;
    char	connReqURL[CRURL_SZ];
    char	connReqPath[CRPATH_SZ];
    char    *connReqUser;
    char    *connReqPW;
    int     informEnabled;
    time_t	basePTime;		/* time reference for Periodic Inform */
    time_t  informInterval;
    time_t  informTime;
    int		upgradesManaged;
    int     instanceID;	  /* last Instance ID generated */
    RPCRequest *dlQ;      /* queue of waiting download requests */
    RPCRequest *dlQActive;/* queue of active or completed requests*/
    InAddr	ipAddress;		/* IP of CPE default interface */
    InAddr	crIPAddress;	/* IP address for connection requests. same as ACS route */
    u_char	MACAddress[6];
    InAddr	acsIPAddress;	/* used for ACS IP affinity */
    char	*acsHostName;	/* Host name from acsURL */
    char	*defaultACSURL;  /* saved from the default pre-configuration if available */
    char	cwmpVersion;	/* 0, 1 or 2 */
    int		cwmpRetryMin;	/* CWMPRetryMinimumWaitInterval     v1.2 */
    int		cwmpRetryMultiplier; /* CWMPRetryIntervalMultiplier v1.2 */
#ifdef  CONFIG_RPCREQUESTDOWNLOAD
    char    *reqDLFileType;
    char    *reqDLArgName;
    char    *reqDLArgValue;
#endif
#ifdef  CONFIG_RPCKICK
   	char	*kickURL;
	char	*kickCommand;	/* from CGI arg */
	char	*kickArg;		/* from CGI arg */
	char	*kickNext;		/* from CGI arg */
	char	*kickReferer;	/* from http referer header */
#endif
	char	provisioningCodeStr[65];
	u_char	aliasAddressing; /* set true if AliasBasedAddressing supported */
	u_char	instanceMode;	 /* InstanceNumber=0 or InstanceAlias==1 */
	u_char	autoCreateInstances; /* Auto create object instances for SetPValues RPC */
	char	SoftwareVersion[33];
// TwinA
	int	sigusr1;  /* for sysupgrade use */
	char    OUI[7];
	char    SN[30];
	char    HVER[30];
	char    *stunURL;
} CPEState;


/* Inform Events
 * Notice!!!!!!!!!!!!!!!!!!!
 * enum order must match eventPolicy table order. */
typedef enum {
    eEvtBootstrap = 0,
    eEvtBoot,
    eEvtPeriodic,
    eEvtScheduled,
    eEvtValueChange,
    eEvtKicked,
    eEvtConnectRequest,
    eEvtTransferComplete,
    eEvtDiagComplete,
    eEvtRequestDownload,
    eEvtAutonomousTransferComplete,
    eEvtDUStateChangeComplete,
    eEvtAutonomousDUStateChangeComplete,
    eEvtMReboot,
    eEvtMScheduleInform,
    eEvtMDownload,
    eEvtMScheduleDownload,
    eEvtMUpload,
    eEvtMChangeDUState,
    eEvtMVendor,
    eEvtINVALID,
    eEvtArraySz
}eEventCode;
/*
 * event policy handling
 */
typedef enum {
    eOff,
    eActive,
    ePending
} eEState;
/*
 * An event may have evState set to eActive and
 * evPending set to ePending.
 */
typedef struct EventPolicy {
    eEventCode	evtCode;
    const char	*evtString;
    int			retryPolicy;
    int			currRetries;
    eRPCMethods	resetMethod;
    eEState		evState:8;			/* is eOff or eActive*/
    eEState		evPending:8;		/* is eOff or ePending */
} EventPolicy;

/*
 * return status of runRPC
 */
typedef enum {
    eRpcInit,      /* InformResponse RPC received session is initiated*/
    eRpcContinue,  /* SOAP msg posted to ACS waiting response continue session */
    eRpcEnd, 	   /* no CPE requests pending - send NULL */
    eRpcAbort,	   /* end session with fault to ACS, disconnect, and retry */
    eRpcFail,	   /* end session with failure */
    eRpcRetry,	   /* SOAP Fault response with 8005 Retry Request from ACS*/
    eRpcDone       /* nothing to do - last NULL sent, disconnect */
} eRPCStatus;

void initRPCMethods(void);
char *cwmpInformRPC(char *sessionID, int retryCnt, char *defaultIP );
int runGetRPCMethods(char *sessionID, RPCRequest *r, char **);
int runGetParameterValues( char *sessionID, RPCRequest *r, char **);
int runGetParameterNames( char *sessionID, RPCRequest *r, char **);
int runGetParameterAttributes( char *sessionID, RPCRequest *r, char **);
int runSetParameterAttributes( char *sessionID, RPCRequest *r, char **);
int runSetParameterValues( char *sessionID, RPCRequest *r, int *pending, char **bp);
int runDeleteObject( char *sessionID, RPCRequest *r, int *, char **);
int runAddObject( char *sessionID, RPCRequest *r, int *, char **);
#ifdef CONFIG_RPCUPLOAD
int runUpload( char *sessionID, RPCRequest *r, int *pending, char **bp);
#endif
#ifdef CONFIG_RPCDOWNLOAD
int runDownload( char *sessionID, RPCRequest *r, int *, char **);
#endif
#if defined(CONFIG_RPCDOWNLOAD) || defined(CONFIG_RPCUPLOAD)
int runCancelTransfer(char *sessionID, RPCRequest *r, int *, char **);
#endif
#ifdef CONFIG_RPCSCHEDULEDOWNLOAD
int runScheduleDownload(  char *sessionID, RPCRequest *r, int *, char **);
#endif
int sendTransferComplete( char *sessionID, RPCRequest *, char **bp);
int sendGetRPCMethods( char *sessionID, CPEState *, char **bp);
int runReboot( char *sessionID, RPCRequest *r, int *, char **);
#ifdef CONFIG_RPCAUTONOMOUSTRANSFERCOMPLETE
int sendAutonomousTransferComplete( char *sessionID, RPCRequest *r, char **bp);
#endif
#ifdef CONFIG_RPCFACTORYRESET
int runFactoryReset( char *sessionID, RPCRequest *r, int *, char **);
#endif
int runSendFault(char *sessionID, int fault, char **bp);
#ifdef CONFIG_RPCREQUESTDOWNLOAD
int sendRequestDownload( char *sessionID, CPEState *cpe, char **bp);
#endif
#ifdef CONFIG_RPCGETQUEUEDTRANSFERS
int  runGetQueuedTransfers(  char *sessionID, char ** );
#endif
#ifdef CONFIG_RPCGETALLQUEUEDTRANSFERS
int  runGetAllQueuedTransfers(  char *sessionID, char ** );
#endif
#ifdef CONFIG_RPCSCHEDULEINFORM
int runScheduleInform( char *sessionID, RPCRequest *r, char **bp);
#endif
#ifdef CONFIG_RPCKICK
int sendKickedRPC(char *sessionID, CPEState *cpe, char **bp);
#endif
#ifdef CONFIG_RPCSETVOUCHERS
int	runSetVouchers(char *sessionID, RPCRequest *, char **);
int	runGetOptions(char *sessionID, RPCRequest *, char **);
#endif
#ifdef CONFIG_RPCCHANGEDUSTATE
#include "smm.h"
int runChangeDUState( char *sessionID, RPCRequest *, char **);
int cwmpIsPendingChangeDUStateComplete(void);
int sendDUStateChangeComplete( char *sessionID, CPEChangeDUState *, char **bp);
#endif /* CONFIG_RPCCHANGEDUSTATE */

#endif /* _RPCMETHODS_H_ */
