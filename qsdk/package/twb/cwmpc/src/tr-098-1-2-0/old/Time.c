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
 * $Id: Time.c,v 1.1 2012/05/10 17:38:08 dmounday Exp $
 *----------------------------------------------------------------------*/

#include "paramTree.h"
#include "rpc.h"
#include "gslib/src/utils.h"
#include "soapRpc/rpcUtils.h"
#include "soapRpc/rpcMethods.h"
#include "soapRpc/cwmpSession.h"
/** */
/** InternetGatewayDevice.Time. */

static char timeserver[80];
static char synctime[80];
static char status[80];
static void getSntpConf(){
    FILE *fp = fopen("/etc/sntp.conf", "r");
	if(fp != NULL)
	{
		char line[81];
		char *b;
	    if ( fgets(line,80,fp) ){
	    	if (strstr(line,"timeserver") ){
	    		b = (char *)strtok(line," ");
	    		b = (char *)strtok(NULL," \n\r");
	    		strcpy(timeserver, b);
	    	}
	    }
	    if ( fgets(line,80,fp)){
	    	if(strstr(line,"synctime") != NULL) {
	            b= (char *)strtok(line," ");
	            b = (char *)strtok(NULL," \n\r");
	            strcpy(synctime, b);
	            }
	    }
	    if ( fgets(line,80,fp)){
	    	if(strstr(line,"status") != NULL) {
	            b = (char *)strtok(line," ");
	            b = (char *)strtok(NULL," \n\r");
	            strcpy(status, b);
	            }
	    }
	    fclose(fp);
	}
}

static void setSntpConf(){
    FILE *fp = fopen("/etc/sntp.conf", "w");
	if(fp != NULL)
	{
		char line[255];
        snprintf(line, sizeof(line),
        		"timeserver %s\nsynctime %s\nstatus enable\n",
        		timeserver, synctime);
        fclose(fp);
        system("/bin/sntp >/dev/null 2>&1 &");
	}
}

/**@obj Time **/

/**@param Time_NTPServer1                     **/
CPE_STATUS setTime_NTPServer1(Instance *ip, char *value)
{
	/* Set parameter */
	getSntpConf();
	strCpyLimited(timeserver, value, sizeof timeserver );
	setSntpConf();
	return CPE_OK;
}
CPE_STATUS getTime_NTPServer1(Instance *ip, char **value)
{
	/* get parameter */
	getSntpConf();
	*value = strdup(timeserver);
	return CPE_OK;
}
/**@endparam                                               **/
/**@param Time_NTPServer2                     **/
CPE_STATUS setTime_NTPServer2(Instance *ip, char *value)
{
	/* set parameter */
	return CPE_OK;
}
CPE_STATUS getTime_NTPServer2(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param Time_CurrentLocalTime                     **/
CPE_STATUS getTime_CurrentLocalTime(Instance *ip, char **value)
{
	/* get parameter */
		time_t t = time(NULL);
	*value = strdup(getXSIdateTime(&t));
	return CPE_OK;
}
/**@endparam                                               **/
/**@param Time_LocalTimeZone                     **/
CPE_STATUS setTime_LocalTimeZone(Instance *ip, char *value)
{
	/* set parameter */
	return CPE_OK;
}
CPE_STATUS getTime_LocalTimeZone(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param Time_LocalTimeZoneName                     **/
CPE_STATUS setTime_LocalTimeZoneName(Instance *ip, char *value)
{
	/* set parameter */
	return CPE_OK;
}
CPE_STATUS getTime_LocalTimeZoneName(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param Time_DaylightSavingsUsed                     **/
CPE_STATUS setTime_DaylightSavingsUsed(Instance *ip, char *value)
{
	/* set parameter */
	return CPE_OK;
}
CPE_STATUS getTime_DaylightSavingsUsed(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param Time_DaylightSavingsStart                     **/
CPE_STATUS setTime_DaylightSavingsStart(Instance *ip, char *value)
{
	/* set parameter */
	return CPE_OK;
}
CPE_STATUS getTime_DaylightSavingsStart(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param Time_DaylightSavingsEnd                     **/
CPE_STATUS setTime_DaylightSavingsEnd(Instance *ip, char *value)
{
	/* set parameter */
	return CPE_OK;
}
CPE_STATUS getTime_DaylightSavingsEnd(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param Time_Enable                     **/
CPE_STATUS setTime_Enable(Instance *ip, char *value)
{
	/* set parameter */
	return CPE_OK;
}
CPE_STATUS getTime_Enable(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param Time_Status                     **/
CPE_STATUS getTime_Status(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@endobj Time **/





