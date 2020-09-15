/*----------------------------------------------------------------------*
 * Gatespace
 * Copyright 2011 Gatespace Networks, Inc., All Rights Reserved.
 * Gatespace Networks, Inc. confidential material.
 *----------------------------------------------------------------------*
 * File Name  : ManagementServer.c
 * Description: Auto-generated getter/setter stubs file.
 *----------------------------------------------------------------------*
 * $Revision: 1.2 $
 *
 * $Id: ManagementServer.c,v 1.2 2012/06/13 16:07:50 dmounday Exp $
 *----------------------------------------------------------------------*/

#include "paramTree.h"
#include "rpc.h"
#include "gslib/src/utils.h"
#include "soapRpc/rpcUtils.h"
#include "soapRpc/rpcMethods.h"
#include "soapRpc/cwmpSession.h"
#include "targetsys.h"
#include "CPEWrapper.h"

#define LOGFILE     "/dev/console"
#define DBG_MSG(fmt, arg...) do { FILE *log_fp = fopen(LOGFILE, "w"); \
                                     fprintf(log_fp, "%s:%s:%d:" fmt "\n", __FILE__, __func__, __LINE__, ##arg); \
                                     fclose(log_fp); \
                                     } while(0)

extern CPEState cpeState;
/**@obj ManagementServer **/

/**@param ManagementServer_URL                                                      **/
CPE_STATUS setManagementServer_URL(Instance *ip, char *value)
{
    if ( strlen(value) > 0 && !streq(cpeState.acsURL, value) )
    {
        if (cpeState.acsURL)
        {
            char cmd[512]={0};
            char cmd_result[64]={0};

            char *str1 = NULL;
            char *str2 = NULL;

            if(strchr(cpeState.acsURL,'['))
            {
                str1 = str_replace(cpeState.acsURL, "[","\\[");
                str2 = str_replace(str1, "]", "\\]");
                sprintf(cmd, "sed -i \'s~%s~%s~g\' %s", str2 , value , CPESTATE_FILENAME_DEFAULT );
            }

            else
            {
                sprintf(cmd, "sed -i \'s~%s~%s~g\' %s", cpeState.acsURL , value , CPESTATE_FILENAME_DEFAULT );
            }
            cmd_popen(cmd,cmd_result);
            GS_FREE(cpeState.acsURL);
            cpeState.acsURL = GS_STRDUP(value);
            system("uci set tr069.firstboot='0'");
            system("uci set tr069.reboot='0'");
            system("uci commit tr069");

            setTimer(CPEReboot, NULL, 60*1000);
        }

        if ( cwmpIsACSSessionActive() )   /* only set this if a session is active. May be initializing. */
            cwmpSetPending(PENDING_ACSCHANGE);
    }
    return CPE_OK;
}
CPE_STATUS getManagementServer_URL(Instance *ip, char **value)
{
        if ( cpeState.acsURL)
                *value = GS_STRDUP(cpeState.acsURL);
    return CPE_OK;
}
/**@endparam                                                      **/
/**@param ManagementServer_Username                                                      **/
CPE_STATUS setManagementServer_Username(Instance *ip, char *value)
{
    char cmd[128]="";
    char cmd_result[128]="";

    sprintf(cmd, "sed -i \"s/<acsUser>%s</<acsUser>%s</g\" %s", cpeState.acsUser , value , CPESTATE_FILENAME_DEFAULT );
    cmd_popen(cmd, cmd_result);

    COPYSTR( cpeState.acsUser,value);
    return CPE_OK;
}
CPE_STATUS getManagementServer_Username(Instance *ip, char **value)
{
        if ( cpeState.acsUser)
                *value = GS_STRDUP(cpeState.acsUser);
    return CPE_OK;
}
/**@endparam                                                      **/
/**@param ManagementServer_Password                                                       **/
CPE_STATUS setManagementServer_Password(Instance *ip, char *value)
{

    char cmd[128]="";
    char cmd_result[128]="";

    sprintf(cmd, "sed -i \"s/<acsPW>%s</<acsPW>%s</g\" %s", cpeState.acsPW , value , CPESTATE_FILENAME_DEFAULT );
    cmd_popen(cmd, cmd_result);

    COPYSTR(cpeState.acsPW, value);
    return CPE_OK;
}
CPE_STATUS getManagementServer_Password(Instance *ip, char **value)
{
        if (cpeState.acsPW)
                *value = GS_STRDUP(cpeState.acsPW);
    return CPE_OK;
}
/**@endparam                                                      **/
/**@param ManagementServer_PeriodicInformEnable                                                      **/
CPE_STATUS setManagementServer_PeriodicInformEnable(Instance *ip, char *value)
{
    if(testBoolean(value) == 1 || (stricmp(value, "false")==0  ))
    {
        char cmd[128]="";
        char cmd_result[128]="";

        sprintf(cmd, "sed -i \"s/<informEnabled>%d</<informEnabled>%d</g\" %s", cpeState.informEnabled , testBoolean(value) , CPESTATE_FILENAME_DEFAULT );
        cmd_popen(cmd, cmd_result);

        cpeState.informEnabled = testBoolean(value);

        return CPE_OK;
    }
    else
        return CPE_9007;
}
CPE_STATUS getManagementServer_PeriodicInformEnable(Instance *ip, char **value)
{
    *value = GS_STRDUP(cpeState.informEnabled? "true": "false");
    return CPE_OK;
}
/**@endparam                                                      **/
/**@param ManagementServer_PeriodicInformInterval                                                      **/
CPE_STATUS setManagementServer_PeriodicInformInterval(Instance *ip, char *value)
{
    int ret = character_check(value);
    if(ret)
    {
        if(value[0] =='0')
            return CPE_9007;

        cpeState.informInterval = atoi(value);
        
        char cmd[128]={0};
        char cmd_result[128]={0};
        char *pos =NULL;
        int interval = -1;
        memset(cmd , 0x0 , sizeof(cmd));
        memset(cmd_result , 0x0 , sizeof(cmd_result));
        sprintf(cmd, "cat /etc/cpestate-default.xml | grep Interval | awk -F '>' '{print $2}' | awk -F '<' '{print $1}'");
        cmd_popen(cmd, cmd_result);
        if ((pos = strchr(cmd_result, '\n')) != NULL)   *pos = '\0';
        if ( NULL != cmd_result)    interval = atoi(cmd_result);
        
        memset(cmd , 0x0 , sizeof(cmd));
        memset(cmd_result , 0x0 , sizeof(cmd_result));
        sprintf(cmd, "sed -i \"s/<informInterval>%d</<informInterval>%d</g\" %s", interval , atoi(value) , CPESTATE_FILENAME_DEFAULT );
        cmd_popen(cmd, cmd_result);
        return CPE_OK;
    }
    else
        return CPE_9007;
}
CPE_STATUS getManagementServer_PeriodicInformInterval(Instance *ip, char **value)
{
    char cmd[128]={0};
    char cmd_result[128]={0};
    char *pos =NULL;
    FILE *fp;
    memset(cmd , 0x0 , sizeof(cmd));
    memset(cmd_result , 0x0 , sizeof(cmd_result));
    if( (fp = fopen("/etc/keep/cpestate.xml", "r" )) == NULL )
    {
        char    buf[10];
        snprintf(buf,sizeof(buf),"%d", (unsigned)cpeState.informInterval);
        *value = GS_STRDUP(buf);
    }
    else
    {
        sprintf(cmd, "cat %s | grep Interval | awk -F '>' '{print $2}' | awk -F '<' '{print $1}'", CPESTATE_FILENAME_DEFAULT);
        cmd_popen(cmd, cmd_result);
        if ((pos = strchr(cmd_result, '\n')) != NULL)   *pos = '\0';
        if ( NULL != cmd_result)    cpeState.informInterval = atoi(cmd_result);
        char    buf[10];
        snprintf(buf,sizeof(buf),"%d", (unsigned)cpeState.informInterval);
        *value = GS_STRDUP(buf);
    }

    return CPE_OK;
}
/**@endparam                                                      **/
/**@param ManagementServer_PeriodicInformTime                                                       **/
CPE_STATUS setManagementServer_PeriodicInformTime(Instance *ip, char *value)
{
        extern char *strptime(const char *, const char *, struct tm *);
    struct tm bt;
    strptime(value,"%Y-%m-%dT%H:%M:%S", &bt );
        cpeState.informTime = mktime(&bt);
        return CPE_OK;
}
CPE_STATUS getManagementServer_PeriodicInformTime(Instance *ip, char **value)
{
    char    buf[30];
        if (cpeState.informTime != 0 && cpeState.informTime!= -1) {
                struct tm *bt=localtime(&cpeState.informTime);
                strftime(buf,sizeof(buf),"%Y-%m-%dT%H:%M:%S",bt );
                *value = GS_STRDUP(buf);
        }
        else
                *value = GS_STRDUP(UNKNOWN_TIME);
    return CPE_OK;
}
/**@endparam                                                      **/
/**@param ManagementServer_ParameterKey                                                       **/
CPE_STATUS getManagementServer_ParameterKey(Instance *ip, char **value)
{
    *value = GS_STRDUP(cpeState.parameterKey);
    return CPE_OK;
}
/**@endparam                                                      **/
/**@param ManagementServer_ConnectionRequestURL                                                      **/
CPE_STATUS getManagementServer_ConnectionRequestURL(Instance *ip, char **value)
{
        *value = GS_STRDUP(cpeState.connReqURL);
    return CPE_OK;
}
/**@endparam                                                      **/
/**@param ManagementServer_ConnectionRequestUsername                                                       **/
CPE_STATUS setManagementServer_ConnectionRequestUsername(Instance *ip, char *value)
{
    COPYSTR(cpeState.connReqUser,value);
    return CPE_OK;
}
CPE_STATUS getManagementServer_ConnectionRequestUsername(Instance *ip, char **value)
{
        if (cpeState.connReqUser)
                *value = GS_STRDUP(cpeState.connReqUser);
    return CPE_OK;
}
/**@endparam                                                      **/
/**@param ManagementServer_ConnectionRequestPassword                                                      **/
CPE_STATUS setManagementServer_ConnectionRequestPassword(Instance *ip, char *value)
{
    COPYSTR(cpeState.connReqPW, value);
    return CPE_OK;
}
CPE_STATUS getManagementServer_ConnectionRequestPassword(Instance *ip, char **value)
{
        if ( cpeState.connReqPW)
                *value = GS_STRDUP(cpeState.connReqPW);
    return CPE_OK;
}
/**@endparam                                                      **/
/**@param ManagementServer_UpgradesManaged                                                       **/
CPE_STATUS setManagementServer_UpgradesManaged(Instance *ip, char *value)
{
    cpeState.upgradesManaged = testBoolean(value);
    /* call manufacture function here */
    return CPE_OK;
}
CPE_STATUS getManagementServer_UpgradesManaged(Instance *ip, char **value)
{
    *value = GS_STRDUP(cpeState.upgradesManaged? "true":"false");
    return CPE_OK;
}
/**@endparam                                                      **/
/**@param ManagementServer_UDPConnectionRequestAddress                     **/
#ifdef CONFIG_TR111P2
#include "../tr111/tr111p2.h"
extern StunState stunState;
CPE_STATUS commitManagementServer(CWMPObject *o, Instance *ip, eCommitCBType cmt ){
	fprintf(stderr,"commitManagementServer() pendingEnable=%d\n", stunState.pendingEnable);
	if ( stunState.pendingEnable ){
		stunState.pendingEnable = 0;
		stunState.enable = 1;
		if ( reStartStun() ) {
			return CPE_OK;
		} else {
			stunState.enable = 0;
			return CPE_ERR;
		}
	}
	/* may be setting some other ManagementServer parameters so return OK */
	return CPE_OK;
}
CPE_STATUS getManagementServer_UDPConnectionRequestAddress(Instance *ip, char **value)
{
	/* get parameter */
	if ( stunState.natIP != 0 ) {
		char *buf = (char *) GS_MALLOC(258);
		struct in_addr n;
		n.s_addr = ntohl(stunState.natIP);
		snprintf(buf, 258, "%s:%d", inet_ntoa(n), stunState.natPort);
		*value = buf;      /* buf is freed by framework */
	} else
		*value = NULL;
	return CPE_OK;
}

int str_last_occurance(char delim, char *str)
{
    int i=0;
//    int index_occurance;
    int last = 0;
    char *strptr = str;
    while (*strptr != '\0') {
        if (*strptr == delim){
            last = i;
        }
        i++;
        strptr++;
    }
    return last;
}

CPE_STATUS setManagementServer_UDPConnectionRequestAddress(Instance *ip, char *value)
{
    /* get parameter */
    int length_occurance;
    char *cp_natIP=NULL,*cp_natPort ;
    struct in_addr n;
    if (*value ) {
        if(strstr(value,":")!= NULL){
            //length_occurance=CUSTOM_CALLBACK_FUNCTION(FIND_LAST_OCCURANCE_STRING,':',value,NULL);
            length_occurance = str_last_occurance(':',value);            
            cp_natIP=value;
            cp_natPort=cp_natIP+length_occurance+1;
            *(cp_natIP+length_occurance)='\0';
            inet_pton(AF_INET, cp_natIP, &n.s_addr);
            stunState.natIP=htonl(n.s_addr);
            stunState.natPort= atoi(cp_natPort);
        }
    }
    return CPE_OK;
}

/**@endparam                                               **/
/**@param ManagementServer_STUNEnable                     **/
CPE_STATUS setManagementServer_STUNEnable(Instance *ip, char *value)
{
	/* Set parameter */
	if ( testBoolean(value) ) {
		/* restart stun if already running */
		stunState.pendingEnable = 1;
		stunState.enable = 1;
		reStartStun();
		/* started by commitManagementServer */
	} else { /* stop it if its running */
		if ( stunState.enable ) {
			stunState.pendingEnable = stunState.enable = 0;
			/* stop it here */
			stopStun();
		}
		stunState.natDetected = 0;
	}
	return CPE_OK;
}
CPE_STATUS getManagementServer_STUNEnable(Instance *ip, char **value)
{
	 *value = GS_STRDUP( stunState.enable? "true": "false");
	return CPE_OK;
}
/**@endparam                                               **/
/**@param ManagementServer_STUNServerAddress                     **/
CPE_STATUS setManagementServer_STUNServerAddress(Instance *ip, char *value)
{
    char cmd[128]="";
    char cmd_result[128]="";

    if(strlen(value) > 0)
    {
        sprintf(cmd, "sed -i \"s/<STUNServerAddress>%s</<STUNServerAddress>%s</g\" %s", stunState.serverAddr , value , CPESTATE_FILENAME_DEFAULT );
        cmd_popen(cmd, cmd_result);
    }

    if(stunState.serverAddr)
        GS_FREE (stunState.serverAddr);

    stunState.serverAddr = GS_STRDUP(value);

	
	return CPE_OK;
}
CPE_STATUS getManagementServer_STUNServerAddress(Instance *ip, char **value)
{
	if (stunState.serverAddr)
		*value = GS_STRDUP(stunState.serverAddr);
	return CPE_OK;
}
/**@endparam                                               **/
/**@param ManagementServer_STUNServerPort                     **/
CPE_STATUS setManagementServer_STUNServerPort(Instance *ip, char *value)
{
    char cmd[128]="";
    char cmd_result[128]="";

    sprintf(cmd, "sed -i \"s/<STUNServerPort>%d</<STUNServerPort>%s</g\" %s", stunState.serverPort , value , CPESTATE_FILENAME_DEFAULT );
    cmd_popen(cmd, cmd_result);

    stunState.serverPort = atoi(value);
    return CPE_OK;
}
CPE_STATUS getManagementServer_STUNServerPort(Instance *ip, char **value)
{
	*value = GS_STRDUP( itoa(stunState.serverPort) );
	return CPE_OK;
}
/**@endparam                                               **/
/**@param ManagementServer_STUNUsername                     **/
CPE_STATUS setManagementServer_STUNUsername(Instance *ip, char *value)
{

    char cmd[128]="";
    char cmd_result[128]="";

    sprintf(cmd, "sed -i \"s/<STUNUsername>%s</<STUNUsername>%s</g\" %s", stunState.username?stunState.username:"" , *value? GS_STRDUP(value):"", CPESTATE_FILENAME_DEFAULT );
    cmd_popen(cmd, cmd_result);

	if ( stunState.username ) GS_FREE (stunState.username);
	if (*value)
		stunState.username = GS_STRDUP(value);
	else
		stunState.username = NULL;
	return CPE_OK;
}
CPE_STATUS getManagementServer_STUNUsername(Instance *ip, char **value)
{
	if ( stunState.username )
		*value = GS_STRDUP(stunState.username);
	return CPE_OK;
}
/**@endparam                                               **/
/**@param ManagementServer_STUNPassword                     **/
CPE_STATUS setManagementServer_STUNPassword(Instance *ip, char *value)
{

    char cmd[128]="";
    char cmd_result[128]="";

    sprintf(cmd, "sed -i \"s/<STUNPassword>%s</<STUNPassword>%s</g\" %s", stunState.password?stunState.password:"" , *value? GS_STRDUP(value):"", CPESTATE_FILENAME_DEFAULT );
    cmd_popen(cmd, cmd_result);


	if ( stunState.password ) GS_FREE (stunState.password);
	if (*value)
		stunState.password = GS_STRDUP(value);
	else
		stunState.password = NULL;
	return CPE_OK;
}
CPE_STATUS getManagementServer_STUNPassword(Instance *ip, char **value)
{
	if (stunState.password)
		*value = GS_STRDUP(stunState.password);
	return CPE_OK;
}
/**@endparam                                               **/
/**@param ManagementServer_STUNMaximumKeepAlivePeriod                     **/
CPE_STATUS setManagementServer_STUNMaximumKeepAlivePeriod(Instance *ip, char *value)
{
    char cmd[128]="";
    char cmd_result[128]="";

    sprintf(cmd, "sed -i \"s/<STUNMaxKeepAlive>%d</<STUNMaxKeepAlive>%s</g\" %s", stunState.maxKeepAlive , value , CPESTATE_FILENAME_DEFAULT );
    cmd_popen(cmd, cmd_result);

    stunState.maxKeepAlive = atoi( value );
    return CPE_OK;
}
CPE_STATUS getManagementServer_STUNMaximumKeepAlivePeriod(Instance *ip, char **value)
{
	*value = GS_STRDUP(itoa(stunState.maxKeepAlive));
	return CPE_OK;
}
/**@endparam                                               **/
/**@param ManagementServer_STUNMinimumKeepAlivePeriod                     **/
CPE_STATUS setManagementServer_STUNMinimumKeepAlivePeriod(Instance *ip, char *value)
{
    char cmd[128]="";
    char cmd_result[128]="";
    sprintf(cmd, "sed -i \"s/<STUNMinKeepAlive>%d</<STUNMinKeepAlive>%s</g\" %s", stunState.minKeepAlive , value , CPESTATE_FILENAME_DEFAULT );
    cmd_popen(cmd, cmd_result);

    stunState.minKeepAlive = atoi( value );
    return CPE_OK;
}
CPE_STATUS getManagementServer_STUNMinimumKeepAlivePeriod(Instance *ip, char **value)
{
	*value = GS_STRDUP(itoa(stunState.minKeepAlive));
	return CPE_OK;
}
/**@endparam                                               **/
/**@param ManagementServer_NATDetected                     **/
CPE_STATUS getManagementServer_NATDetected(Instance *ip, char **value)
{
	*value = GS_STRDUP(stunState.natDetected?"true": "false");
	return CPE_OK;
}
CPE_STATUS setManagementServer_NATDetected(Instance *ip, char *value)
{
    if(testBoolean(value)){
        stunState.natDetected=1;
    }else
    stunState.natDetected=0;
    return CPE_OK;
}
#endif // CONFIG_TR111P2
/**@endparam                                               **/
/**@param ManagementServer_AliasBasedAddressing                     **/
CPE_STATUS getManagementServer_AliasBasedAddressing(Instance *ip, char **value)
{
	*value = GS_STRDUP(cpeState.aliasAddressing?"true": "false");
	return CPE_OK;
}
/**@endparam                                               **/
/**@param ManagementServer_InstanceMode                     **/
CPE_STATUS setManagementServer_InstanceMode(Instance *ip, char *value)
{
	if ( cpeState.aliasAddressing )
		cpeState.instanceMode = streq(value, "InstanceAlias");
	else
		return CPE_9007;
	return CPE_OK;
}
CPE_STATUS getManagementServer_InstanceMode(Instance *ip, char **value)
{
	*value = GS_STRDUP(cpeState.instanceMode? "InstanceAlias": "InstanceNumber");
	return CPE_OK;
}
/**@endparam                                               **/
/**@param ManagementServer_AutoCreateInstances                     **/
CPE_STATUS setManagementServer_AutoCreateInstances(Instance *ip, char *value)
{
	if ( cpeState.aliasAddressing )
		cpeState.autoCreateInstances = testBoolean(value);
	else
		return CPE_9007;
	return CPE_OK;
}
CPE_STATUS getManagementServer_AutoCreateInstances(Instance *ip, char **value)
{
	*value = GS_STRDUP(cpeState.autoCreateInstances? "true": "false");
	return CPE_OK;
}
/**@endparam                                               **/
/**@param ManagementServer_CWMPRetryMinimumWaitInterval                     **/
CPE_STATUS setManagementServer_CWMPRetryMinimumWaitInterval(Instance *ip, char *value)
{
    if (value)
        cpeState.cwmpRetryMin = atoi(value);
    else
        return CPE_9007;
    return CPE_OK;
}
CPE_STATUS getManagementServer_CWMPRetryMinimumWaitInterval(Instance *ip, char **value)
{
    char    buf[10];
    snprintf(buf,sizeof(buf),"%d", (unsigned)cpeState.cwmpRetryMin);
    *value = GS_STRDUP(buf);
    return CPE_OK;
}
/**@endparam                                               **/

/**@param ManagementServer_CWMPRetryIntervalMultiplier                    **/
CPE_STATUS setManagementServer_CWMPRetryIntervalMultiplier(Instance *ip, char *value)
{
    if (value)
        cpeState.cwmpRetryMultiplier = atoi(value);
    else
        return CPE_9007;
    return CPE_OK;
}
CPE_STATUS getManagementServer_CWMPRetryIntervalMultiplier(Instance *ip, char **value)
{
    char    buf[10];
    snprintf(buf,sizeof(buf),"%d", (unsigned)cpeState.cwmpRetryMultiplier);
    *value = GS_STRDUP(buf);
    return CPE_OK;
}
/**@endparam                                               **/
/**@endobj ManagementServer **/



























