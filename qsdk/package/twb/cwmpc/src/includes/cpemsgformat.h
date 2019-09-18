/*----------------------------------------------------------------------*
 * Gatespace Networks, Inc.
 * Copyright 2005 Gatespace Networks, Inc., All Rights Reserved.
 * Gatespace Networks, Inc. confidential material.
 *----------------------------------------------------------------------*
 * File Name  : cpemsgformat.h
 *
 * Description: Description of UDP packet message sent to the
 *              cpelisten function of the CWMPc. This is a sample
 *              used for testing and is not a complete implementation
 *              of the functionality that may be required for the native
 *              CPE configuration and management functions.
 *
 * $Revision: 1.3 $
 * $Id: cpemsgformat.h,v 1.3 2012/06/13 11:13:34 dmounday Exp $
 *----------------------------------------------------------------------*/

#ifndef CPEMSGFORMAT_H_
#define CPEMSGFORMAT_H_
#include <netinet/in.h>
//#include <arpa/inet.h>
/* UDP message format used to signal framework from CPE code */
/* */
#define CPE_PARAMCHANGE			1
#define CPE_DIAGNOSTICS_COMPLETE 2
#define CPE_SENDINFORM			 3    /* queues up GetRPCMethods */
#define CPE_ACSURLCHANGE         4
#define CPE_REQUESTDOWNLOAD      5
#define CPE_AUTOXFERCOMPLETE	 6
#define CPE_STOPCWMPC			 7
#define CPE_NEWLEASE			10
#define CPE_EXPIREDLEASE		11
#define	CPE_GATEWAYID			12
#define CPE_SMM_EVENT			20


#ifdef CONFIG_RPCAUTONOMOUSTRANSFERCOMPLETE
typedef struct AutoXferEvt {
	char	announceURL[1025];
	char	transferURL[1025];
	char	isDownload;
	char	fileType[65];
	int		fileSize;
	char	targetFileName[257];
	int		faultCode;
	int		faultMsg;		/* index in to DL messge table */
	time_t	startTime;
	time_t	completeTime;
}AutoXferEvt;
#endif

typedef struct UpLoadEvt {
    char    fileType[65];
    char    argName[65];
    char    argValue[257];
}UpLoadEvt;

typedef struct ACSChangeEvt {
	char	url[257];
}ACSChangeEvt;

/*
 * DHCP server event msg for ManageableDevices.
 */
typedef struct DHCPOptions {
	char	ipAddress[INET6_ADDRSTRLEN];			/* as a string */
	char	oui[7];
	char	serialNumber[65];
	char	productClass[65];
}DHCPOptions;

typedef struct CPEEvent {
	int		eventCode;
	union   {
		UpLoadEvt	 xLoadEvt;
		ACSChangeEvt acsChangeEvt;
		DHCPOptions	 dhcpOptions;
#ifdef CONFIG_RPCAUTONOMOUSTRANSFERCOMPLETE
		AutoXferEvt  autoXferEvt;
#endif
	} ud;
} CPEEvent;


#endif /* CPEMSGFORMAT_H_ */
