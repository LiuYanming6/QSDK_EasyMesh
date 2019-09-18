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
 * $Id: Time.c,v 1.1 2012/05/10 17:38:00 dmounday Exp $
 *----------------------------------------------------------------------*/

#include "paramTree.h"
#include "rpc.h"
#include "gslib/src/utils.h"
#include "soapRpc/rpcUtils.h"
#include "soapRpc/rpcMethods.h"
#include "soapRpc/cwmpSession.h"

#include "Time.h"

/**@obj Time **/
CPE_STATUS  initTime(CWMPObject *o, Instance *ip)
{
	/* initialize object */
	GS_Time *p = (GS_Time *)GS_MALLOC( sizeof(struct GS_Time));
	memset(p, 0, sizeof(struct GS_Time));
	ip->cpeData = (void *)p;
	return CPE_OK;
}
CPE_STATUS  commitTime(CWMPObject *o, Instance *ip, eCommitCBType cmt)
{
	/* commit object instance */
	return CPE_OK;
}

/**@param Time_NTPServer1                     **/
CPE_STATUS setTime_NTPServer1(Instance *ip, char *value)
{
	GS_Time *p = (GS_Time *)ip->cpeData;
	if ( p ){
		COPYSTR(p->nTPServer1, value);
	}
	return CPE_OK;
}
CPE_STATUS getTime_NTPServer1(Instance *ip, char **value)
{
	GS_Time *p = (GS_Time *)ip->cpeData;
	if ( p ){
		if ( p->nTPServer1 )
			*value = GS_STRDUP(p->nTPServer1);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param Time_NTPServer2                     **/
CPE_STATUS setTime_NTPServer2(Instance *ip, char *value)
{
	GS_Time *p = (GS_Time *)ip->cpeData;
	if ( p ){
		COPYSTR(p->nTPServer2, value);
	}
	return CPE_OK;
}
CPE_STATUS getTime_NTPServer2(Instance *ip, char **value)
{
	GS_Time *p = (GS_Time *)ip->cpeData;
	if ( p ){
		if ( p->nTPServer2 )
			*value = GS_STRDUP(p->nTPServer2);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param Time_CurrentLocalTime                     **/
CPE_STATUS getTime_CurrentLocalTime(Instance *ip, char **value)
{
	GS_Time *p = (GS_Time *)ip->cpeData;
	if ( p ){
		char buf[30];
		struct tm *bt=localtime(&p->currentLocalTime);
		strftime(buf,sizeof(buf),"%Y-%m-%dT%H:%M:%S",bt );
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param Time_LocalTimeZone                     **/
CPE_STATUS setTime_LocalTimeZone(Instance *ip, char *value)
{
	GS_Time *p = (GS_Time *)ip->cpeData;
	if ( p ){
		COPYSTR(p->localTimeZone, value);
	}
	return CPE_OK;
}
CPE_STATUS getTime_LocalTimeZone(Instance *ip, char **value)
{
	GS_Time *p = (GS_Time *)ip->cpeData;
	if ( p ){
		if ( p->localTimeZone )
			*value = GS_STRDUP(p->localTimeZone);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param Time_LocalTimeZoneName                     **/
CPE_STATUS setTime_LocalTimeZoneName(Instance *ip, char *value)
{
	GS_Time *p = (GS_Time *)ip->cpeData;
	if ( p ){
		COPYSTR(p->localTimeZoneName, value);
	}
	return CPE_OK;
}
CPE_STATUS getTime_LocalTimeZoneName(Instance *ip, char **value)
{
	GS_Time *p = (GS_Time *)ip->cpeData;
	if ( p ){
		if ( p->localTimeZoneName )
			*value = GS_STRDUP(p->localTimeZoneName);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param Time_DaylightSavingsUsed                     **/
CPE_STATUS setTime_DaylightSavingsUsed(Instance *ip, char *value)
{
	GS_Time *p = (GS_Time *)ip->cpeData;
	if ( p ){
		p->daylightSavingsUsed=testBoolean(value);
	}
	return CPE_OK;
}
CPE_STATUS getTime_DaylightSavingsUsed(Instance *ip, char **value)
{
	GS_Time *p = (GS_Time *)ip->cpeData;
	if ( p ){
		*value = GS_STRDUP(p->daylightSavingsUsed? "true": "false");
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param Time_DaylightSavingsStart                     **/
CPE_STATUS setTime_DaylightSavingsStart(Instance *ip, char *value)
{
	GS_Time *p = (GS_Time *)ip->cpeData;
	if ( p ){
		struct tm bt;
		strptime(value,"%Y-%m-%dT%H:%M:%S", &bt );
		p->daylightSavingsStart= mktime(&bt);
	}
	return CPE_OK;
}
CPE_STATUS getTime_DaylightSavingsStart(Instance *ip, char **value)
{
	GS_Time *p = (GS_Time *)ip->cpeData;
	if ( p ){
		char buf[30];
		struct tm *bt=localtime(&p->daylightSavingsStart);
		strftime(buf,sizeof(buf),"%Y-%m-%dT%H:%M:%S",bt );
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param Time_DaylightSavingsEnd                     **/
CPE_STATUS setTime_DaylightSavingsEnd(Instance *ip, char *value)
{
	GS_Time *p = (GS_Time *)ip->cpeData;
	if ( p ){
		struct tm bt;
		strptime(value,"%Y-%m-%dT%H:%M:%S", &bt );
		p->daylightSavingsEnd= mktime(&bt);
	}
	return CPE_OK;
}
CPE_STATUS getTime_DaylightSavingsEnd(Instance *ip, char **value)
{
	GS_Time *p = (GS_Time *)ip->cpeData;
	if ( p ){
		char buf[30];
		struct tm *bt=localtime(&p->daylightSavingsEnd);
		strftime(buf,sizeof(buf),"%Y-%m-%dT%H:%M:%S",bt );
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@endobj Time **/




