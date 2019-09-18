/*----------------------------------------------------------------------*
 * Gatespace
 * Copyright 2006-2011 Gatespace. All Rights Reserved.
 * Gatespace Networks, Inc. confidential material.
 *----------------------------------------------------------------------*
 * File Name  : sys.h
 * Description:	Application and system level defines and conditional
 *      complitation flags.
 *             .
 *----------------------------------------------------------------------*
 * $Revision: 1.16 $
 * $Id:
 *----------------------------------------------------------------------*/
#ifndef _SYS_H_
#define _SYS_H_
/*
 *
 */
#ifdef CONFIG_CWMP_V1_0
#define CWMP_VERSION 0
#define DSLFORUM_NAMESPACE "urn:dslforum-org:cwmp-1-0"
#else
#ifdef CONFIG_CWMP_V1_1
#define DSLFORUM_NAMESPACE "urn:dslforum-org:cwmp-1-1"
#define CWMP_VERSION 1
#else
#define DSLFORUM_NAMESPACE "urn:dslforum-org:cwmp-1-2"
#define CWMP_VERSION 2
#endif
#endif
/*
* oemCWMPc compile time constant definitions.
*/
#define XML_WRITE_FLGS	0
#define UNKNOWN_TIME          "1970-01-01T00:00:00.000000+05:30"

/*
 * oemCWMPc ACS connection request listener
*/
#define ACSAUTHREALM	"cwmp-device"
#define ACSDOMAIN		"/cwmp"
#define ACSCONNREQ_PORT 7547				/* Defined by IANA for CWMP */
#define ACSCONNREQ_PATH "/creq"	            /* do include a leading "/" in path */

/*
*/
#define USER_AGENT_NAME	"GateSpace-cwmp-1.2"
#define CWMP_SPEC_VERSION	  "1.0"
#define SOAPCONTENTTYPE		"text/xml; charset=\"utf-8\""
/* was just  #define SOAPCONTENTTYPE        "text/xml"  */
/*
* Timer values:
*/
#define ACSRESPONSETIME (30*1000)			/* max time to wait on ACS to reply */
#define CWMPRETRYMINIMUMWAITINTERVAL 5	 	/* TR-069 Section 3.2.1.1 default m value */
#define CWMPRETRYINTERNALMULTIPLIER	2000 	/* TR-069 Section 3.2.1.1 default k value */
#define CONNREQ_RESET_DELAY	(2*1000)		/* time to delay following any conn req */
#define MIN_DOWNLOAD_DELAY      1       	/* time in secs to wait before starting a download */
											/* if a 0 or smaller delay specified */
/*
 * Other constants
 */
#define MAX_REDIRECTS		5

/**/
#ifdef GENERATE_STRCONST
#define STRCONST(X) const char sc_##X[]=#X
#else
#define STRCONST(X) extern const char sc_##X[]
#endif



#endif /* _SYS_H_  */
