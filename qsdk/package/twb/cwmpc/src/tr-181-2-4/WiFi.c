/*******************************************************************************
 *        Copyright (c) 2018 TWin Advanced Technology Corp., Ltd.
 *        All rights reserved.
 *
 *       THIS WORK IS AN UNPUBLISHED WORK AND CONTAINS CONFIDENTIAL,
 *       PROPRIETARY AND TRADESECRET INFORMATION OF T&W INCORPORATED.
 *       ACCESS TO THIS WORK IS RESTRICTED TO (I) T&W EMPLOYEES WHO HAVE A
 *       NEED TO KNOW TO PERFORM TASKS WITHIN THE SCOPE OF THEIR ASSIGNMENTS
 *       AND (II) ENTITIES OTHER THAN T&W WHO HAVE ENTERED INTO APPROPRIATE
 *       LICENSE AGREEMENTS.  NO PART OF THIS WORK MAY BE USED, PRACTICED,
 *       PERFORMED, COPIED, DISTRIBUTED, REVISED, MODIFIED, TRANSLATED,
 *       ABBRIDGED, CONDENSED, EXPANDED, COLLECTED, COMPILED, LINKED, RECAST,
 *       TRANSFORMED OR ADAPTED WITHOUT THE PRIOR WRITTEN CONSENT OF T&W.
 *       ANY USE OR EXPLOITATION OF THIS WORK WITHOUT AUTHORIZATION COULD
 *       SUBJECT THE PERPERTRATOR TO CRIMINAL AND CIVIL LIABILITY.
 ******************************************************************************/
 /**
 * @file WiFi.c
 * @brief
 * @author T&W Technology co., Ltd.
 * @bug No known bugs
 */

/*----------------------------------------------------------------------*
 * Gatespace
 * Copyright 2011, 2012 Gatespace Networks, Inc., All Rights Reserved.
 * Gatespace Networks, Inc. confidential material.
 *----------------------------------------------------------------------*
 * File Name  :
 * Description: Auto-generated getter/setter stubs file.
 *----------------------------------------------------------------------*
 * $Revision: 1.2 $
 *
 * $Id: stubgenPre,v 1.2 2012/05/10 17:38:08 dmounday Exp $
 *----------------------------------------------------------------------*/
/*-------------------------------------------------------------------------*/
/*                        INCLUDE HEADER FILES                             */
/*-------------------------------------------------------------------------*/
#include "paramTree.h"
#include "rpc.h"
#include "gslib/src/utils.h"
#include "gslib/src/utils_hyd.h"
#include "soapRpc/rpcUtils.h"
#include "soapRpc/rpcMethods.h"
#include "soapRpc/cwmpSession.h"

#include "WiFi.h"

/*-------------------------------------------------------------------------*/
/*                           DEFINITIONS                                   */
/*-------------------------------------------------------------------------*/
#define GB_TO_BYTE (long long int)1024*1024*1024
/*-------------------------------------------------------------------------*/
/*                           VARIABLES                                     */
/*-------------------------------------------------------------------------*/
extern  CWMPObject CWMP_RootObject[];
/*-------------------------------------------------------------------------*/
/*                           FUNCTIONS                                     */
/*-------------------------------------------------------------------------*/
void reload_query(void){
    char lock[2]={0};
    cmd_popen("uci get system.lock 2>&1" , lock);
    
    if (!strncmp(lock ,"1" ,1 ))
    {
        system("uci set system.reload=1");
        return;
    }
    system("uci set system.lock=1");
    system("killall /etc/reload.sh");
    system("/etc/reload.sh &"); /* sleep 5  , /etc/init.d/repacd restart */
    return;
}

/**@obj WiFiAccessPointAssociatedDevice **/
CPE_STATUS  addWiFiAccessPointAssociatedDevice(CWMPObject *o, Instance *ip)
{
    /* add instance data */
    WiFiAccessPointAssociatedDevice *p = (WiFiAccessPointAssociatedDevice *)GS_MALLOC( sizeof(struct WiFiAccessPointAssociatedDevice));
    memset(p, 0, sizeof(struct WiFiAccessPointAssociatedDevice));
    ip->cpeData = (void *)p;

    return CPE_OK;
}

CPE_STATUS  delWiFiAccessPointAssociatedDevice(CWMPObject *o, Instance *ip)
{
	/* delete instance data */
	WiFiAccessPointAssociatedDevice *p = (WiFiAccessPointAssociatedDevice *)ip->cpeData;
	if( p ){
		//TODO: free instance data
		GS_FREE(p);
	}
	return CPE_OK;
}

/**@param WiFiAccessPointAssociatedDevice_MACAddress                     **/
CPE_STATUS getWiFiAccessPointAssociatedDevice_MACAddress(Instance *ip, char **value)
{

	WiFiAccessPointAssociatedDevice *p = (WiFiAccessPointAssociatedDevice *)ip->cpeData;
	if ( p ){
		if ( p->mACAddress )
			*value = GS_STRDUP(p->mACAddress);
	}
	return CPE_OK;
}
/**@endparam                                               **/

/**@param WiFiAccessPointAssociatedDevice_AuthenticationState                     **/
CPE_STATUS getWiFiAccessPointAssociatedDevice_AuthenticationState(Instance *ip, char **value)
{
	WiFiAccessPointAssociatedDevice *p = (WiFiAccessPointAssociatedDevice *)ip->cpeData;
	if ( p ){
		*value = GS_STRDUP(p->authenticationState? "true": "false");
	}
	return CPE_OK;
}
/**@endparam                                               **/

/**@param WiFiAccessPointAssociatedDevice_LastDataDownlinkRate                     **/
CPE_STATUS getWiFiAccessPointAssociatedDevice_LastDataDownlinkRate(Instance *ip, char **value)
{
	WiFiAccessPointAssociatedDevice *p = (WiFiAccessPointAssociatedDevice *)ip->cpeData;
	if ( p ){
		char    buf[10];
		snprintf(buf,sizeof(buf),"%u", p->lastDataDownlinkRate);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/

/**@param WiFiAccessPointAssociatedDevice_LastDataUplinkRate                     **/
CPE_STATUS getWiFiAccessPointAssociatedDevice_LastDataUplinkRate(Instance *ip, char **value)
{
	WiFiAccessPointAssociatedDevice *p = (WiFiAccessPointAssociatedDevice *)ip->cpeData;
	if ( p ){
		char    buf[10];
		snprintf(buf,sizeof(buf),"%u", p->lastDataUplinkRate);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                                                 **/

/**@obj WiFiAccessPointSecurity **/
CPE_STATUS  initWiFiAccessPointSecurity(CWMPObject *o, Instance *ip)
{
	/* initialize object */
	WiFiAccessPointSecurity *p = (WiFiAccessPointSecurity *)GS_MALLOC( sizeof(struct WiFiAccessPointSecurity));
	memset(p, 0, sizeof(struct WiFiAccessPointSecurity));
	ip->cpeData = (void *)p;
	return CPE_OK;
}

/**@param WiFiAccessPointAssociatedDevice_SignalStrength                     **/
CPE_STATUS getWiFiAccessPointAssociatedDevice_SignalStrength(Instance *ip, char **value)
{
	WiFiAccessPointAssociatedDevice *p = (WiFiAccessPointAssociatedDevice *)ip->cpeData;
	if ( p ){
		if ( p->signalStrength )
		{
			int signal;
            char buf[10] = {0};
			signal = atoi(p->signalStrength) - 95;
            sprintf(buf , "%d" , signal);
			*value = GS_STRDUP(buf);
		}
	}
	return CPE_OK;
}
/**@endparam                                                                  **/

/**@param WiFiAccessPointAssociatedDevice_Retransmissions                     **/
CPE_STATUS getWiFiAccessPointAssociatedDevice_Retransmissions(Instance *ip, char **value)
{
	WiFiAccessPointAssociatedDevice *p = (WiFiAccessPointAssociatedDevice *)ip->cpeData;
	if ( p ){
		char    buf[10];
		snprintf(buf,sizeof(buf),"%u", p->retransmissions);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                                                **/

/**@param WiFiAccessPointAssociatedDevice_Active                            **/
CPE_STATUS getWiFiAccessPointAssociatedDevice_Active(Instance *ip, char **value)
{
	WiFiAccessPointAssociatedDevice *p = (WiFiAccessPointAssociatedDevice *)ip->cpeData;
	if ( p ){
		*value = GS_STRDUP(p->active? "false": "true");
	}
	return CPE_OK;
}
/**@endparam                                                                 **/

/**@endobj WiFiAccessPointAssociatedDevice **/

/**@obj WiFiAccessPointSecurity **/

/**@param WiFiAccessPointSecurity_Reset                         **/
CPE_STATUS getWiFiAccessPointSecurity_Reset(Instance *ip, char **value)
{
	WiFiAccessPointSecurity *p = (WiFiAccessPointSecurity *)ip->cpeData;
	if ( p ){
        //The value of this parameter is not part of the device configuration and is always false when read.
        p->reset = 0;
		*value = GS_STRDUP(p->reset? "true": "false");
	}
	return CPE_OK;
}

CPE_STATUS setWiFiAccessPointSecurity_Reset(Instance *ip, char *value)
{
    WiFiAccessPointSecurity *p = (WiFiAccessPointSecurity *)ip->cpeData;
    if ( p ){
        p->reset=testBoolean(value); // default values
        if(p->reset)
        {
            char cmd[128]={0};
            char cmd_result[128]={0};
            int wpa =0;
            char *pos =NULL;

            if ( get_role() ==1 && 0!= strcmp("01", get_topology_iface_name(ip->parent->id-1) ) && 0!= strcmp("11", get_topology_iface_name(ip->parent->id-1) ) )  // Only Can change CAP
            {
                sprintf(cmd, "cat /var/run/hostapd-ath%s.conf | grep wpa= | awk -F '=' '{print $2}'", get_topology_iface_name(ip->parent->id-1));
                cmd_popen(cmd , cmd_result );
                if ((pos = strchr(cmd_result, '\n')) != NULL)   *pos = '\0';
                if ( NULL != cmd_result)
                    wpa = atoi(cmd_result);

                memset(cmd , 0x0 , sizeof(cmd));
                memset(cmd_result , 0x0 , sizeof(cmd_result));
                sprintf(cmd, "sed -i 's/wpa=%d/wpa=%d/g' /var/run/hostapd-ath%s.conf", wpa , 2 , get_topology_iface_name(ip->parent->id-1));

                memset(cmd , 0x0 , sizeof(cmd));
                memset(cmd_result, 0x0 , sizeof(cmd_result));
                sprintf(cmd, "state_cfg get rkey");
                cmd_popen(cmd,cmd_result);
                if ((pos = strchr(cmd_result, '\n')) != NULL)   *pos = '\0';

                memset(cmd , 0x0 , sizeof(cmd));
                sprintf(cmd, "uci set wireless.@wifi-iface[%d].key=%s", get_uci_iface_name(ip->parent->id-1), cmd_result);
                cmd_popen(cmd,cmd_result);

                memset(cmd , 0x0 , sizeof(cmd));
                sprintf(cmd, "uci set wireless.@wifi-iface[%d].key=%s", get_uci_iface_name(ip->parent->id+1), cmd_result);
                cmd_popen(cmd,cmd_result);

                system("uci commit wireless");
                reload_query();
            }

        }
    }
    return CPE_ERR;
}
/**@endparam                                                      **/

/**@param WiFiAccessPointSecurity_ModesSupported                     **/
CPE_STATUS getWiFiAccessPointSecurity_ModesSupported(Instance *ip, char **value)
{
	WiFiAccessPointSecurity *p = (WiFiAccessPointSecurity *)ip->cpeData;
	if ( p ){
        COPYSTR(p->modesSupported,"WPA2-Personal");
		if ( p->modesSupported )
			*value = GS_STRDUP(p->modesSupported);
	}
	return CPE_OK;
}
/**@endparam                                                      **/

/**@param WiFiAccessPointSecurity_ModeEnabled                     **/
CPE_STATUS setWiFiAccessPointSecurity_ModeEnabled(Instance *ip, char *value)
{
    WiFiAccessPointSecurity *p = (WiFiAccessPointSecurity *)ip->cpeData;
    if ( p ){
        COPYSTR(p->modeEnabled, value);
        char cmd[128]={0};
        char cmd_result[128]={0};
        int wpa_a =0 , wpa_b =0;
        char *pos =NULL;

        if ( get_role() ==1 && 0 != strcmp("01", get_topology_iface_name(ip->parent->id-1) ) && 0 != strcmp("11", get_topology_iface_name(ip->parent->id-1) ) )  // Only Can change CAP
        {
            sprintf(cmd, "cat /var/run/hostapd-ath%s.conf | grep wpa= | awk -F '=' '{print $2}'", get_topology_iface_name(ip->parent->id-1));
            cmd_popen(cmd , cmd_result );
            if ((pos = strchr(cmd_result, '\n')) != NULL)   *pos = '\0';
            if ( NULL != cmd_result)
                wpa_a = atoi(cmd_result);


            if (!strcmp(p->modeEnabled , "WPA-Personal"))
                wpa_b=1;
            else if (!strcmp(p->modeEnabled , "WPA2-Personal"))
                wpa_b=2;
            else if (!strcmp(p->modeEnabled , "WPA-WPA2-Personal"))
                wpa_b=3;

            memset(cmd , 0x0 , sizeof(cmd));
            memset(cmd_result , 0x0 , sizeof(cmd_result));
            sprintf(cmd, "sed -i 's/wpa=%d/wpa=%d/g' /var/run/hostapd-ath%s.conf", wpa_a , wpa_b , get_topology_iface_name(ip->parent->id-1));
            cmd_popen(cmd , cmd_result );
            memset(cmd , 0x0 , sizeof(cmd));
            memset(cmd_result , 0x0 , sizeof(cmd_result));
            sprintf(cmd, "hostapd_cli -i ath%s -p var/run/hostapd-wifi%d reload", get_topology_iface_name(ip->parent->id-1) ,ip->parent->id-1);
            cmd_popen(cmd , cmd_result );
        }
    }
    return CPE_OK;
}
CPE_STATUS getWiFiAccessPointSecurity_ModeEnabled(Instance *ip, char **value)
{
    WiFiAccessPointSecurity *p = (WiFiAccessPointSecurity *)ip->cpeData;
    if ( p )
    {
        char cmd[128]={0};
        char cmd_result[128]={0};
        int ret;
        char *pos =NULL;

        sprintf(cmd, "cat /var/run/hostapd-ath%s.conf | grep wpa= | awk -F '=' '{print $2}'", get_topology_iface_name(ip->parent->id-1));
        cmd_popen(cmd , cmd_result );
        if ((pos = strchr(cmd_result, '\n')) != NULL)   *pos = '\0';
        if (NULL != cmd_result)     ret = atoi(cmd_result);
        
        if(ret == 1)
        {
            COPYSTR(p->modeEnabled ,"WPA-Personal" );
        }
        else if(ret == 2)
        {
            COPYSTR(p->modeEnabled ,"WPA2-Personal" );
        }
        else if(ret == 3)  
        {
            COPYSTR(p->modeEnabled ,"WPA-WPA2-Personal" );
        }
        if ( p->modeEnabled )
            *value = GS_STRDUP(p->modeEnabled);
    }
    return CPE_OK;
}
/**@endparam                                               **/


/**@param WiFiAccessPointSecurity_PreSharedKey                     **/
CPE_STATUS setWiFiAccessPointSecurity_PreSharedKey(Instance *ip, char *value)
{

    WiFiAccessPointSecurity *p = (WiFiAccessPointSecurity *)ip->cpeData;
    if ( p ){
        if ( get_role() ==1 && ip->parent->id != 2 && ip->parent->id !=4 )  // Only Can change CAP
        {
            char cmd[128]={0};
            char cmd_result[128]={0};
            cmd_popen(cmd , cmd_result );
            sprintf(cmd, "uci get wireless.@wifi-iface[%d].key", 0);
            if (0 == strncmp(cmd_result , value , strlen(value)))
                return CPE_OK;
            
            if(strlen(value) >=8  && strlen(value) < 64)
            {
                char cmd[128]={0};
                char cmd_result[128]={0};

                int ret = util_check_string(value , cmd);
                if (ret == -1)
                    return CPE_ERR;

                COPYSTR(p->preSharedKey, cmd);

                memset(cmd , 0x0 , sizeof(cmd));
                memset(cmd_result, 0x0 , sizeof(cmd_result));
                sprintf(cmd, "uci set wireless.@wifi-iface[%d].key=\"%s\"", 0, p->preSharedKey);
                cmd_popen(cmd,cmd_result);

                memset(cmd , 0x0 , sizeof(cmd));
                memset(cmd_result, 0x0 , sizeof(cmd_result));
                sprintf(cmd, "uci set wireless.@wifi-iface[%d].key=\"%s\"", 1, p->preSharedKey);
                cmd_popen(cmd,cmd_result);

                system("uci commit wireless");
                reload_query();
                return CPE_OK;
            }
        }
    }
    return CPE_ERR;
}
CPE_STATUS getWiFiAccessPointSecurity_PreSharedKey(Instance *ip, char **value)
{
    WiFiAccessPointSecurity *p = (WiFiAccessPointSecurity *)ip->cpeData;
    if ( p )
    {
#if 0
        if (ip->parent->id == 1)
        {
            char cmd[128]={0};
            char cmd_result[128]={0};
            char *pos = NULL;

            if ( get_role() ==1)  // Only Can change CAP
            {
                sprintf(cmd, "uci get wireless.@wifi-iface[%d].key", get_uci_iface_name(ip->parent->id-1));
                cmd_popen(cmd,cmd_result);
                if ((pos = strchr(cmd_result, '\n')) != NULL)   *pos = '\0';
            }
        }
#endif
        // When read, this parameter returns an empty string, regardless of the actual value.
        if ( p->preSharedKey )
        {
            *value = GS_STRDUP(p->preSharedKey);
        }
    }
    return CPE_OK;
}
/**@endparam                                               **/

/**@param WiFiAccessPointSecurity_KeyPassphrase                     **/
CPE_STATUS setWiFiAccessPointSecurity_KeyPassphrase(Instance *ip, char *value)
{
    WiFiAccessPointSecurity *p = (WiFiAccessPointSecurity *)ip->cpeData;
    if ( p ){


        if ( get_role() ==1 && ip->parent->id != 2 && ip->parent->id !=4 )  // Only Can change CAP
        {
            char cmd[128]={0};
            char cmd_result[128]={0};
            cmd_popen(cmd , cmd_result );
            sprintf(cmd, "uci get wireless.@wifi-iface[%d].key", 0);

            if (0 == strncmp(cmd_result , value , strlen(value)))
                return CPE_OK;

            if( strlen(value) >= 8 && strlen(value) < 64)
            {

                int ret = util_check_string(value , cmd);
                if (ret == -1)
                    return CPE_ERR;

                COPYSTR(p->keyPassphrase, cmd);

                memset(cmd , 0x0 , sizeof(cmd));
                memset(cmd_result, 0x0 , sizeof(cmd_result));
                sprintf(cmd, "uci set wireless.@wifi-iface[%d].key=\"%s\"", 0, p->keyPassphrase);
                cmd_popen(cmd,cmd_result);

                memset(cmd , 0x0 , sizeof(cmd));
                memset(cmd_result, 0x0 , sizeof(cmd_result));
                sprintf(cmd, "uci set wireless.@wifi-iface[%d].key=\"%s\"", 1, p->keyPassphrase);
                cmd_popen(cmd,cmd_result);

                system("uci commit wireless");
                reload_query();
                return CPE_OK;
            }
        }
    }
    return CPE_ERR;
}
CPE_STATUS getWiFiAccessPointSecurity_KeyPassphrase(Instance *ip, char **value)
{
    WiFiAccessPointSecurity *p = (WiFiAccessPointSecurity *)ip->cpeData;
    if ( p ){
        if ( p->keyPassphrase )
            *value = GS_STRDUP(p->keyPassphrase);
    }
    return CPE_OK;
}
/**@endparam                                               **/
/**@endobj WiFiAccessPointSecurity **/

/**@obj WiFiAccessPointWPS **/
CPE_STATUS  initWiFiAccessPointWPS(CWMPObject *o, Instance *ip)
{
	/* initialize object */
	WiFiAccessPointWPS *p = (WiFiAccessPointWPS *)GS_MALLOC( sizeof(struct WiFiAccessPointWPS));
	memset(p, 0, sizeof(struct WiFiAccessPointWPS));
	ip->cpeData = (void *)p;
	return CPE_OK;
}

/**@param WiFiAccessPointWPS_Enable                     **/
CPE_STATUS setWiFiAccessPointWPS_Enable(Instance *ip, char *value)
{
	WiFiAccessPointWPS *p = (WiFiAccessPointWPS *)ip->cpeData;
	if ( p ){
		p->enable=testBoolean(value);
        char cmd[128]={0};
        char cmd_result[128]={0};

        sprintf(cmd, "iwpriv ath%s wps mode %d", get_topology_iface_name(ip->parent->id-1), p->enable );
        cmd_popen(cmd , cmd_result );
 
	}
	return CPE_OK;
}
CPE_STATUS getWiFiAccessPointWPS_Enable(Instance *ip, char **value)
{
	WiFiAccessPointWPS *p = (WiFiAccessPointWPS *)ip->cpeData;
	if ( p ){
        char cmd[128]={0};
        char cmd_result[128]={0};
        char *pos = NULL;

        sprintf(cmd, "iwpriv ath%s get_wps | awk -F \":\" '{print $2}'", get_topology_iface_name(ip->parent->id-1));
        cmd_popen(cmd , cmd_result );
        if ((pos = strchr(cmd_result, '\n')) != NULL)   *pos = '\0';
        if (NULL != cmd_result)     p->enable = atoi(cmd_result);
		*value = GS_STRDUP(p->enable? "true": "false");
	}
	return CPE_OK;
}
/**@endparam                                               **/

/**@param WiFiAccessPointWPS_ConfigMethodsSupported                     **/
CPE_STATUS getWiFiAccessPointWPS_ConfigMethodsSupported(Instance *ip, char **value)
{
	WiFiAccessPointWPS *p = (WiFiAccessPointWPS *)ip->cpeData;
	if ( p ){
		if ( p->configMethodsSupported )
			*value = GS_STRDUP(p->configMethodsSupported);
	}
	return CPE_OK;
}
/**@endparam                                               **/

/**@param WiFiAccessPointWPS_ConfigMethodsEnabled                     **/
CPE_STATUS setWiFiAccessPointWPS_ConfigMethodsEnabled(Instance *ip, char *value)
{
	WiFiAccessPointWPS *p = (WiFiAccessPointWPS *)ip->cpeData;
	if ( p ){
		COPYSTR(p->configMethodsEnabled, value);
	}
	return CPE_OK;
}
CPE_STATUS getWiFiAccessPointWPS_ConfigMethodsEnabled(Instance *ip, char **value)
{
	WiFiAccessPointWPS *p = (WiFiAccessPointWPS *)ip->cpeData;
	if ( p ){
		if ( p->configMethodsEnabled )
			*value = GS_STRDUP(p->configMethodsEnabled);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@endobj WiFiAccessPointWPS **/

/**@obj WiFiAccessPoint **/
CPE_STATUS  addWiFiAccessPoint(CWMPObject *o, Instance *ip)
{
	/* add instance data */
	WiFiAccessPoint *p = (WiFiAccessPoint *)GS_MALLOC( sizeof(struct WiFiAccessPoint));
	memset(p, 0, sizeof(struct WiFiAccessPoint));
	ip->cpeData = (void *)p;
	return CPE_OK;
}
CPE_STATUS  delWiFiAccessPoint(CWMPObject *o, Instance *ip)
{
	/* delete instance data */
	WiFiAccessPoint *p = (WiFiAccessPoint *)ip->cpeData;
	if( p ){
		//TODO: free instance data
		GS_FREE(p);
	}
	return CPE_OK;
}

/**@param WiFiAccessPoint_Enable                     **/
CPE_STATUS setWiFiAccessPoint_Enable(Instance *ip, char *value)
{
    WiFiAccessPoint *p = (WiFiAccessPoint *)ip->cpeData;
    if ( p ){
        char cmd[128]={0};
        char cmd_result[128]={0};

        p->enable=testBoolean(value);
        if ( p->enable)
        {
            sprintf(cmd, "ifconfig ath%s up", get_topology_iface_name(ip->id-1) );
            cmd_popen(cmd, cmd_result);
        }
        else
        {
            sprintf(cmd, "ifconfig ath%s down", get_topology_iface_name(ip->id-1) );
            cmd_popen(cmd, cmd_result);
        }
    }
    return CPE_OK;
}
CPE_STATUS getWiFiAccessPoint_Enable(Instance *ip, char **value)
{
    WiFiAccessPoint *p = (WiFiAccessPoint *)ip->cpeData;
    if ( p ){
        char cmd[128]={0};
        char cmd_result[128]={0};
        const char default_status[]="UP";
        char *pos = NULL;

        sprintf(cmd, "ifconfig ath%s | grep UP | awk -F \" \" '{print $1}'" , get_topology_iface_name(ip->id-1));
        cmd_popen(cmd , cmd_result);
        if ( NULL != cmd_result)
        {
            if ((pos = strchr(cmd_result, '\n')) != NULL) 
            { 
                    *pos = '\0';
            }
            if(!strncmp(cmd_result, default_status, strlen(default_status)))    
                p->enable = 1;
            else
                p->enable = 0;
        }
        *value = GS_STRDUP(p->enable? "true": "false");
    }
    return CPE_OK;
    }
/**@endparam                                               **/

/**@param WiFiAccessPoint_Status                     **/
CPE_STATUS getWiFiAccessPoint_Status(Instance *ip, char **value)
{
    WiFiAccessPoint *p = (WiFiAccessPoint *)ip->cpeData;
    if ( p ){

        char cmd[128]={0};
        char cmd_result[128]={0};
        const char default_status[] = "Not-Associated";

        sprintf(cmd, "iwconfig ath%s | grep \"Access Point\" | awk -F\" \" '{print $6}'", get_topology_iface_name(ip->id-1));
        cmd_popen(cmd, cmd_result);
        if(strncmp(cmd_result, default_status, strlen(default_status)) != 0)
        {
            //COPYSTR(p->status , "Connected");
            *value = GS_STRDUP("Enabled");
        }
        else
        {
            *value = GS_STRDUP("Disabled");
            //COPYSTR(p->status , "Disconnected");
        }

        if ( p->status )
            *value = GS_STRDUP(p->status);
    }
    return CPE_OK;
}
/**@endparam                                               **/

/**@param WiFiAccessPoint_SSIDReference                     **/
CPE_STATUS setWiFiAccessPoint_SSIDReference(Instance *ip, char *value)
{
	WiFiAccessPoint *p = (WiFiAccessPoint *)ip->cpeData;
	if ( p ){
		//TODO: resolve Instance pointers.
	}
	return CPE_OK;
}
CPE_STATUS getWiFiAccessPoint_SSIDReference(Instance *ip, char **value)
{
    WiFiAccessPoint *p = (WiFiAccessPoint *)ip->cpeData;
    if ( p ){
        //Instance *ssid;
        char oName[257];
        memset(oName , 0x0 , sizeof(oName));
        snprintf(oName, sizeof(oName), "%s.WiFi.SSID.%d", CWMP_RootObject[0].name, ip->id );
        *value = GS_STRDUP(oName);
        //*value = cwmpGetInstancePathStr(p->sSIDReference);
    }
    return CPE_OK;
}
/**@endparam                                               **/

/**@param WiFiAccessPoint_SSIDAdvertisementEnabled                     **/
CPE_STATUS setWiFiAccessPoint_SSIDAdvertisementEnabled(Instance *ip, char *value)
{
    WiFiAccessPoint *p = (WiFiAccessPoint *)ip->cpeData;
    if ( p ){
        p->sSIDAdvertisementEnabled=testBoolean(value);
        char cmd[128]={0};
        char cmd_result[128]={0};

        sprintf(cmd, "iwpriv ath%s hide_ssid %d", get_topology_iface_name(ip->id-1), p->sSIDAdvertisementEnabled ? 0:1 );
        cmd_popen(cmd , cmd_result );
    }
    return CPE_OK;
}
CPE_STATUS getWiFiAccessPoint_SSIDAdvertisementEnabled(Instance *ip, char **value)
{
//  iwpriv ath0 get_hide_ssid | awk -F ":" '{print $2}'
    WiFiAccessPoint *p = (WiFiAccessPoint *)ip->cpeData;
    if ( p ){

        char cmd[128]={0};
        char cmd_result[128]={0};
        char *pos = NULL;

        sprintf(cmd, "iwpriv ath%s get_hide_ssid | awk -F \":\" '{print $2}'", get_topology_iface_name(ip->id-1));
        cmd_popen(cmd , cmd_result );
        if ((pos = strchr(cmd_result, '\n')) != NULL)   *pos = '\0';
        if (NULL != cmd_result)     p->sSIDAdvertisementEnabled = atoi(cmd_result);
        

        *value = GS_STRDUP(p->sSIDAdvertisementEnabled? "false": "true");
    }
    return CPE_OK;
}
/**@endparam                                               **/

/**@param WiFiAccessPoint_AssociatedDeviceNumberOfEntries                     **/
CPE_STATUS getWiFiAccessPoint_AssociatedDeviceNumberOfEntries(Instance *ip, char **value)
{
	cwmpGetObjInstanceCntStr("AssociatedDevice", value);
	return CPE_OK;
}
/**@endparam                                               **/

/**@endobj WiFiAccessPoint **/

/**@obj WiFiRadioStats **/
CPE_STATUS  initWiFiRadioStats(CWMPObject *o, Instance *ip)
{
	/* initialize object */
	WiFiRadioStats *p = (WiFiRadioStats *)GS_MALLOC( sizeof(struct WiFiRadioStats));
	memset(p, 0, sizeof(struct WiFiRadioStats));
	ip->cpeData = (void *)p;
	return CPE_OK;
}

/**@param WiFiRadioStats_BytesSent                     **/
CPE_STATUS getWiFiRadioStats_BytesSent(Instance *ip, char **value)
{
    WiFiRadioStats *p = (WiFiRadioStats *)ip->cpeData;
    if ( p ){
        //char    buf[32];
        char cmd[128]={0};
        char cmd_result[128]={0};
        char *pos = NULL;

        sprintf(cmd, "ifconfig wifi%d| grep -r \"RX bytes\" | awk -F \":\" '{print $3}' | awk -F \" \" '{print $1 %% %llu}'"  , ip->parent->id-1 , 4*GB_TO_BYTE );
        cmd_packet_popen(cmd , cmd_result);
        if ((pos = strchr(cmd_result, '\n')) != NULL)   *pos = '\0';
        //if( NULL != cmd_result)     p->bytesSent = atoi(cmd_result);
        //snprintf(buf,sizeof(buf),"%lld", p->bytesSent);
        *value = GS_STRDUP(cmd_result);
    }
    return CPE_OK;
}
/**@endparam                                               **/

/**@param WiFiRadioStats_BytesReceived                     **/
CPE_STATUS getWiFiRadioStats_BytesReceived(Instance *ip, char **value)
{
    WiFiRadioStats *p = (WiFiRadioStats *)ip->cpeData;
    if ( p ){
        //char    buf[32];
        char cmd[128]={0};
        char cmd_result[128]={0};
        char *pos = NULL;

        sprintf(cmd, "ifconfig wifi%d| grep -r \"RX bytes\" | awk -F \":\" '{print $2}' | awk -F \" \" '{print $1 %% %llu}'" , ip->parent->id-1 , 4*GB_TO_BYTE  );
        cmd_packet_popen(cmd , cmd_result);
        if ((pos = strchr(cmd_result, '\n')) != NULL)   *pos = '\0';
        //if( NULL != cmd_result)     p->bytesReceived = atoi(cmd_result);
       // snprintf(buf,sizeof(buf),"%lld", p->bytesReceived);
        *value = GS_STRDUP(cmd_result);
    }
    return CPE_OK;
}
/**@endparam                                               **/

/**@param WiFiRadioStats_PacketsSent                     **/
CPE_STATUS getWiFiRadioStats_PacketsSent(Instance *ip, char **value)
{
    WiFiRadioStats *p = (WiFiRadioStats *)ip->cpeData;
    if ( p ){
        //char    buf[32];
        char cmd[128]={0};
        char cmd_result[128]={0};
        char *pos = NULL;

        sprintf(cmd, "ifconfig wifi%d | grep -r \"TX packets\" | awk -F \":\" '{print $2}' | awk -F \" \" '{print $1 %% %llu}'" ,ip->parent->id-1 ,4*GB_TO_BYTE );
        cmd_packet_popen(cmd , cmd_result);
        if ((pos = strchr(cmd_result, '\n')) != NULL)   *pos = '\0';
       // if( NULL != cmd_result)     p->packetsSent = atoi(cmd_result);
        //snprintf(buf,sizeof(buf),"%lld", p->packetsSent);
        *value = GS_STRDUP(cmd_result);
    }
    return CPE_OK;
}
/**@endparam                                               **/

/**@param WiFiRadioStats_PacketsReceived                     **/
CPE_STATUS getWiFiRadioStats_PacketsReceived(Instance *ip, char **value)
{
    WiFiRadioStats *p = (WiFiRadioStats *)ip->cpeData;
    if ( p ){
        //char    buf[32];
        char cmd[128]={0};
        char cmd_result[128]={0};
        char *pos = NULL;

        sprintf(cmd, "ifconfig wifi%d| grep -r \"RX packets\" | awk -F \":\" '{print $2}' | awk -F \" \" '{print $1 %% %llu}'" , ip->parent->id-1 , 4*GB_TO_BYTE );
        cmd_packet_popen(cmd , cmd_result);
        if ((pos = strchr(cmd_result, '\n')) != NULL)   *pos = '\0';
        //if( NULL != cmd_result)     p->packetsReceived = atoi(cmd_result);
        //snprintf(buf,sizeof(buf),"%lld", p->packetsReceived);
        *value = GS_STRDUP(cmd_result);
    }
    return CPE_OK;
}
/**@endparam                                               **/

/**@param WiFiRadioStats_ErrorsSent                     **/
CPE_STATUS getWiFiRadioStats_ErrorsSent(Instance *ip, char **value)
{
    WiFiRadioStats *p = (WiFiRadioStats *)ip->cpeData;
    if ( p ){
        //char    buf[32];
        char cmd[128]={0};
        char cmd_result[128]={0};
        char *pos = NULL;

        sprintf(cmd, "ifconfig wifi%d| grep -r \"TX packets\" | awk -F \":\" '{print $3}' | awk -F \" \" '{print $1 %% %llu}'" , ip->parent->id-1 , 4*GB_TO_BYTE);
        cmd_packet_popen(cmd , cmd_result);
        if ((pos = strchr(cmd_result, '\n')) != NULL)   *pos = '\0';
       // if( NULL != cmd_result)     p->errorsSent = atoi(cmd_result);
        //snprintf(buf,sizeof(buf),"%u", p->errorsSent);
        *value = GS_STRDUP(cmd_result);
    }
    return CPE_OK;
}
/**@endparam                                               **/

/**@param WiFiRadioStats_ErrorsReceived                     **/
CPE_STATUS getWiFiRadioStats_ErrorsReceived(Instance *ip, char **value)
{
    WiFiRadioStats *p = (WiFiRadioStats *)ip->cpeData;
    if ( p ){
        //char    buf[32];
        char cmd[128]={0};
        char cmd_result[128]={0};
        char *pos = NULL;

        sprintf(cmd, "ifconfig wifi%d| grep -r \"RX packets\" | awk -F \":\" '{print $3}' | awk -F \" \" '{print $1 %% %llu}'" , ip->parent->id-1 , 4*GB_TO_BYTE);
        cmd_packet_popen(cmd , cmd_result);
        if ((pos = strchr(cmd_result, '\n')) != NULL)   *pos = '\0';
        //if( NULL != cmd_result)     p->errorsReceived = atoi(cmd_result);
        //snprintf(buf,sizeof(buf),"%u", p->errorsReceived);
        *value = GS_STRDUP(cmd_result);
    }
    return CPE_OK;
}
/**@endparam                                               **/


/**@param WiFiRadioStats_DiscardPacketsSent                    **/
CPE_STATUS getWiFiRadioStats_DiscardPacketsSent(Instance *ip, char **value)
{
//  ifconfig wifi0 | grep -r "TX packets" | awk -F ":" '{print $4}'| awk -F " " '{print $1}'
    WiFiRadioStats *p = (WiFiRadioStats *)ip->cpeData;
    if ( p ){
        //char    buf[32];
        char cmd[128]={0};
        char cmd_result[128]={0};
        char *pos = NULL;

        sprintf(cmd, "ifconfig wifi%d | grep -r \"TX packets\" | awk -F \":\" '{print $4}'| awk -F \" \" '{print $1 %% %llu }'" , ip->parent->id-1 , 4*GB_TO_BYTE);
        cmd_packet_popen(cmd , cmd_result);
        if ((pos = strchr(cmd_result, '\n')) != NULL)   *pos = '\0';
        //if( NULL != cmd_result)     p->discardPacketsSent = atoi(cmd_result);
        //snprintf(buf,sizeof(buf),"%u", p->discardPacketsSent);
        *value = GS_STRDUP(cmd_result);
    }
    return CPE_OK;
}
/**@endparam                                               **/

/**@param WiFiRadioStats_DiscardPacketsReceived                    **/
CPE_STATUS getWiFiRadioStats_DiscardPacketsReceived(Instance *ip, char **value)
{
//  ifconfig wifiX | grep -r "RX packets" | awk -F ":" '{print $4}'| awk -F " " '{print $1}'
    WiFiRadioStats *p = (WiFiRadioStats *)ip->cpeData;
    if ( p ){
        //char    buf[32];
        char cmd[128]={0};
        char cmd_result[128]={0};
        char *pos = NULL;

        sprintf(cmd, "ifconfig wifi%d | grep -r \"RX packets\" | awk -F \":\" '{print $4}'| awk -F \" \" '{print $1 %% %llu}'" , ip->parent->id-1 , 4*GB_TO_BYTE);
        cmd_packet_popen(cmd , cmd_result);
        if ((pos = strchr(cmd_result, '\n')) != NULL)   *pos = '\0';
        //if( NULL != cmd_result)     p->discardPacketsReceived = atoi(cmd_result);
        //snprintf(buf,sizeof(buf),"%u", p->discardPacketsReceived);
        *value = GS_STRDUP(cmd_result);
    }
    return CPE_OK;
}
/**@endparam                                               **/
/**@endobj WiFiRadioStats **/

/**@obj WiFiRadio **/
CPE_STATUS  addWiFiRadio(CWMPObject *o, Instance *ip)
{
        /* add instance data */
        GS_WiFiRadio *p = (GS_WiFiRadio *)GS_MALLOC( sizeof(struct GS_WiFiRadio));
        memset(p, 0, sizeof(struct GS_WiFiRadio));
        ip->cpeData = (void *)p;
        return CPE_OK;
}
/**@param WiFiRadio_Enable                     **/
CPE_STATUS setWiFiRadio_Enable(Instance *ip, char *value)
{

    GS_WiFiRadio *p = (GS_WiFiRadio *)ip->cpeData;
    if ( p ){
        char cmd[128]={0};
        char cmd_result[128]={0};

        p->enable=testBoolean(value);
        if ( p->enable)
        {
            sprintf(cmd, "ifconfig wifi%d up", ip->id-1 );
            cmd_popen(cmd, cmd_result);
        }
        else
        {
            sprintf(cmd, "ifconfig wifi%d down", ip->id-1 );
            cmd_popen(cmd, cmd_result);
        }
    }
    return CPE_OK;
}
CPE_STATUS getWiFiRadio_Enable(Instance *ip, char **value)
{

    GS_WiFiRadio *p = (GS_WiFiRadio *)ip->cpeData;
    if ( p ){
        char cmd[128]={0};
        char cmd_result[128]={0};
        const char default_status[]="UP";
        char *pos = NULL;

        sprintf(cmd, "ifconfig wifi%d | grep UP | awk -F \" \" '{print $1}'" , ip->id-1);
        cmd_popen(cmd , cmd_result);
        if ( NULL != cmd_result)
        {
            if ((pos = strchr(cmd_result, '\n')) != NULL)
                *pos = '\0';

            if(!strncmp(cmd_result, default_status, strlen(default_status)))
                p->enable = 1;
            else
                p->enable = 0;
        }
        *value = GS_STRDUP(p->enable? "true": "false");
    }
    return CPE_OK;
}
/**@endparam                                               **/

/**@param WiFiRadio_Status                     **/
CPE_STATUS getWiFiRadio_Status(Instance *ip, char **value)
{
    GS_WiFiRadio *p = (GS_WiFiRadio *)ip->cpeData;
    if ( p )
    {
        char cmd[128]={0};
        char cmd_result[128]={0};
        const char default_status[]="UP";
        char *pos = NULL;

        sprintf(cmd, "ifconfig wifi%d | grep UP | awk -F \" \" '{print $1}'" , ip->id-1);
        cmd_popen(cmd , cmd_result);
        if ( NULL != cmd_result)
        {
            if ((pos = strchr(cmd_result, '\n')) != NULL)       
                *pos = '\0';

            if(!strncmp(cmd_result, default_status, strlen(default_status)))
            {
                COPYSTR( p->status, "UP");
            }
            else
            {
                COPYSTR( p->status, "Down");
            }
        }
        if ( p->status )
            *value = GS_STRDUP(p->status);
    }
    return CPE_OK;
}
/**@endparam                                               **/


/**@param WiFiRadio_Name                     **/
CPE_STATUS getWiFiRadio_Name(Instance *ip, char **value)
{
    GS_WiFiRadio *p = (GS_WiFiRadio *)ip->cpeData;
    if ( p ){
            char cmd[128]={0};
            sprintf(cmd, "ath%d", ip->id-1);
            if( NULL != cmd)    COPYSTR( p->name, cmd );
        if ( p->name )
            *value = GS_STRDUP(p->name);
    }
    return CPE_OK;
}
/**@endparam                                               **/


/**@param WiFiRadio_MaxBitRate                     **/
CPE_STATUS getWiFiRadio_MaxBitRate(Instance *ip, char **value)
{
// iwconfig athx | grep "Bit Rate" | awk -F ":" '{print $2}' | awk -F " " '{print $1 " " $2}'
    GS_WiFiRadio *p = (GS_WiFiRadio *)ip->cpeData;
    if ( p ){
        char cmd[128]={0};
        char cmd_result[128]={0};
        char *pos = NULL;

        sprintf(cmd, "iwconfig ath%d | grep \"Bit Rate\" | awk -F \":\" '{print $2}' | awk -F \" \" '{print $1 \" \" $2}'", ip->id-1);
        cmd_popen(cmd , cmd_result );
        if ((pos = strchr(cmd_result, '\n')) != NULL)   *pos = '\0';
        if (NULL != cmd_result)     p->maxBitRate = atoi(cmd_result);
        char    buf[10];
        snprintf(buf,sizeof(buf),"%u", p->maxBitRate);
        *value = GS_STRDUP(buf);
    }
    return CPE_OK;
}
/**@endparam                                               **/

/**@param WiFiRadio_SupportedFrequencyBands                     **/
CPE_STATUS getWiFiRadio_SupportedFrequencyBands(Instance *ip, char **value)
{
	GS_WiFiRadio *p = (GS_WiFiRadio *)ip->cpeData;
	if ( p ){
        char cmd[128]={0};
        char cmd_result[128]={0};
        char *pos = NULL;

        sprintf(cmd, "wifitool ath%d supported_freq | awk -F \" \" '{print $5}'", ip->id-1);
        cmd_popen(cmd , cmd_result );
        if ((pos = strchr(cmd_result, '\n')) != NULL)   *pos = '\0';
        if (NULL != cmd_result)     COPYSTR(p->supportedFrequencyBands ,cmd_result );
		if ( p->supportedFrequencyBands )
			*value = GS_STRDUP(p->supportedFrequencyBands);
	}
	return CPE_OK;
}
/**@endparam                                               **/

/**@param WiFiRadio_OperatingFrequencyBand                     **/
CPE_STATUS setWiFiRadio_OperatingFrequencyBand(Instance *ip, char *value)
{
    GS_WiFiRadio *p = (GS_WiFiRadio *)ip->cpeData;
    if ( p ){
        COPYSTR(p->operatingFrequencyBand, value);
        char cmd[128]={0};
        char cmd_result[128]={0};

        if (!strcmp(p->operatingFrequencyBand, "2.4GHz"))
            sprintf(cmd, "iwpriv ath%d freq %s", ip->id-1 , "2.412G" );
        else if (!strcmp(p->operatingFrequencyBand, "5GHz"))
            sprintf(cmd, "iwpriv ath%d freq %s", ip->id-1 , "5G" );

        cmd_popen(cmd, cmd_result);    
    }
    return CPE_OK;
}
CPE_STATUS getWiFiRadio_OperatingFrequencyBand(Instance *ip, char **value)
{
    GS_WiFiRadio *p = (GS_WiFiRadio *)ip->cpeData;
    if ( p ){
        char cmd[128]={0};
        char cmd_result[128]={0};
        char *pos = NULL;


        sprintf(cmd, "iwconfig ath%d | grep Frequency | awk -F ':' '{print $3}' | awk -F \" \" '{print $1 \" \" $2}'", ip->id-1);
        cmd_popen(cmd , cmd_result );
        if ((pos = strchr(cmd_result, '\n')) != NULL)   *pos = '\0';
        if (NULL != cmd_result)     COPYSTR(p->operatingFrequencyBand ,cmd_result );

        if ( p->operatingFrequencyBand )
            *value = GS_STRDUP(p->operatingFrequencyBand);
    }
	return CPE_OK;
}
/**@endparam                                               **/

/**@param WiFiRadio_SupportedStandards                     **/
CPE_STATUS getWiFiRadio_SupportedStandards(Instance *ip, char **value)
{
    GS_WiFiRadio *p = (GS_WiFiRadio *)ip->cpeData;
    if ( p ){

        if (ip->id  == 1)
        {
            COPYSTR(p->supportedStandards ,"AUTO\n11B\n11G\n11NGHT20\n11NGHT40PLUS\n11NGHT40MINUS");
        }
        else if (ip->id ==2)
        {
            COPYSTR(p->supportedStandards ,"AUTO\n11A\n11NAHT20\n11NAHT40PLUS\n11NAHT40MINUS\n11ACVHT20\n11ACVHT40PLUS\n11ACVHT40MINUS\n11ACVHT80\n11ACVHT160\n11ACVHT80_80");
        }
        if ( p->supportedStandards )
            *value = GS_STRDUP(p->supportedStandards);
    }
    return CPE_OK;
}
/**@endparam                                               **/

/**@param WiFiRadio_OperatingStandards                     **/
CPE_STATUS setWiFiRadio_OperatingStandards(Instance *ip, char *value)
{
    GS_WiFiRadio *p = (GS_WiFiRadio *)ip->cpeData;
    if ( p ){
        COPYSTR(p->operatingStandards, value);
        char cmd[128]={0};
        char cmd_result[128]={0};

        if (ip->id == 1)
        {
            if ( !strcmp(p->operatingStandards , "11B") || !strcmp(p->operatingStandards , "11G")  || !strcmp(p->operatingStandards , "11NGHT40PLUS") || !strcmp(p->operatingStandards , "11NGHT40MINUS")   )
            {
                sprintf(cmd,"ifconfig ath0 down; ifconfig ath01 down;iwpriv ath%d mode %s", ip->id-1 , p->operatingStandards);
                cmd_popen(cmd, cmd_result); 
                memset(cmd , 0x0 , sizeof(cmd));
                sprintf(cmd,"iwpriv ath0 disablecoext 1; iwpriv ath01 disablecoext 1; ifconfig ath0 up; ifconfig ath01 up");
                cmd_popen(cmd, cmd_result);
            }
            else if ( !strcmp(p->operatingStandards , "AUTO")  || !strcmp(p->operatingStandards , "11NGHT20") )
            {
                sprintf(cmd,"ifconfig ath0 down; ifconfig ath01 down;iwpriv ath%d mode %s", ip->id-1 , p->operatingStandards);
                cmd_popen(cmd, cmd_result); 
                memset(cmd , 0x0 , sizeof(cmd));
                sprintf(cmd,"iwpriv ath0 disablecoext 0; iwpriv ath01 disablecoext 0; ifconfig ath0 up; ifconfig ath01 up");
                cmd_popen(cmd, cmd_result);
            }

        }
        else if (ip->id ==2)
        {
            if ( !strcmp(p->operatingStandards , "AUTO") || !strcmp(p->operatingStandards , "11A") || !strcmp(p->operatingStandards , "11NAHT20") || !strcmp(p->operatingStandards , "11NAHT40PLUS") || !strcmp(p->operatingStandards , "11NAHT40MINUS") || !strcmp(p->operatingStandards , "11ACVHT20") || !strcmp(p->operatingStandards , "11ACVHT40PLUS") || !strcmp(p->operatingStandards , "11ACVHT40MINUS") || !strcmp(p->operatingStandards , "11ACVHT80") || !strcmp(p->operatingStandards , "11ACVHT160") || !strcmp(p->operatingStandards , "11ACVHT80_80" ))
            {
                sprintf(cmd,"ifconfig ath1 down; ifconfig ath11 down;iwpriv ath%d mode %s", ip->id-1 , p->operatingStandards);
                cmd_popen(cmd, cmd_result);
                memset(cmd , 0x0 , sizeof(cmd));
                sprintf(cmd,"ifconfig ath1 up; ifconfig ath11 up");
                cmd_popen(cmd, cmd_result);
            }
            else if ( !strcmp(p->operatingStandards , "AUTO")  || !strcmp(p->operatingStandards , "11NAHT20") )
            {
                sprintf(cmd,"ifconfig ath1 down; ifconfig ath11 down;iwpriv ath%d mode %s", ip->id-1 , p->operatingStandards);
                cmd_popen(cmd, cmd_result); 
                memset(cmd , 0x0 , sizeof(cmd));
                sprintf(cmd,"iwpriv ath1 disablecoext 0; iwpriv ath11 disablecoext 0; ifconfig ath1 up; ifconfig ath11 up");
                cmd_popen(cmd, cmd_result);
            }
        }
    }
    return CPE_OK;
}
CPE_STATUS getWiFiRadio_OperatingStandards(Instance *ip, char **value)
{
    GS_WiFiRadio *p = (GS_WiFiRadio *)ip->cpeData;
    if ( p ){
        char cmd[128]={0};
        char cmd_result[128]={0};
        char *pos = NULL;

        sprintf(cmd, "iwpriv ath%d get_mode | awk -F \":\" '{print $2}'", ip->id-1);
        cmd_popen(cmd , cmd_result );
        if ((pos = strchr(cmd_result, '\n')) != NULL)   *pos = '\0';
        if (NULL != cmd_result)     COPYSTR(p->operatingStandards ,cmd_result );
        if ( p->operatingStandards )
            *value = GS_STRDUP(p->operatingStandards);
    }
    return CPE_OK;
}
/**@endparam                                               **/

/**@param WiFiRadio_PossibleChannels                     **/
CPE_STATUS getWiFiRadio_PossibleChannels(Instance *ip, char **value)
{
//  iwlist ath1 chan | grep -r " : " | awk -F " " '{print $2}' | tr -s "\n" ','
    GS_WiFiRadio *p = (GS_WiFiRadio *)ip->cpeData;
    if ( p ){
        char cmd[128]={0};
        char cmd_result[128]={0};

        sprintf(cmd, "iwlist ath%d chan | grep -r \" : \" | awk -F \" \" '{print $2}' | tr -s \"\n\" ','", ip->id-1);
        cmd_popen(cmd , cmd_result );
        cmd_result[strlen(cmd_result)-1]='\0';
        if (NULL != cmd_result)     COPYSTR(p->possibleChannels ,cmd_result );
        if ( p->possibleChannels )
            *value = GS_STRDUP(p->possibleChannels);
    }
    return CPE_OK;
}
/**@endparam                                               **/

/**@param WiFiRadio_ChannelsInUse                     **/
CPE_STATUS getWiFiRadio_ChannelsInUse(Instance *ip, char **value)
{
    GS_WiFiRadio *p = (GS_WiFiRadio *)ip->cpeData;
    if ( p ){
        char cmd[128]={0};
        char cmd_result[128]={0};

        sprintf(cmd, "iwlist ath%d chan | grep -r \" : \" | awk -F \" \" '{print $2}' | tr -s \"\n\" ','", ip->id-1);
        cmd_popen(cmd , cmd_result );
        cmd_result[strlen(cmd_result)-1]='\0';

        if (NULL != cmd_result)     COPYSTR(p->channelsInUse ,cmd_result );
        if ( p->channelsInUse )
            *value = GS_STRDUP(p->channelsInUse);
    }
    return CPE_OK;
}
/**@endparam                                               **/

/**@param WiFiRadio_Channel                     **/
CPE_STATUS setWiFiRadio_Channel(Instance *ip, char *value)
{
	GS_WiFiRadio *p = (GS_WiFiRadio *)ip->cpeData;
	if ( p ){
		p->channel=atoi(value);
        char cmd[128]={0};
        char cmd_result[128]={0};

        sprintf(cmd, "iwconfig ath%d chan %d", ip->id-1 , p->channel);
        cmd_popen(cmd , cmd_result );
	}
	return CPE_OK;
}
CPE_STATUS getWiFiRadio_Channel(Instance *ip, char **value)
{
//  iwlist ath0 chan | grep Current | awk -F ":" '{ print $2 }' | awk -F "(" '{ print $2}' | awk -F ")" '{print $1}' | awk -F " " '{print $2}'
	GS_WiFiRadio *p = (GS_WiFiRadio *)ip->cpeData;
	if ( p ){
        char cmd[128]={0};
        char cmd_result[128]={0};
        int c;
        sprintf(cmd,"iwlist ath%d chan | grep '(Channel' | awk -F ' ' '{print $5}' | tr -d ')'", ip->id-1);
        //sprintf(cmd, "iwlist ath%d chan | grep Current | awk -F \":\" '{ print $2 }' | awk -F \"(\" '{ print $2}' | awk -F \")\" '{print $1}' | awk -F \" \" '{print $2}'", ip->id-1);
        cmd_popen(cmd , cmd_result );
        c = strlen(cmd_result);
        if (cmd_result[c-1] == '\n')
            cmd_result[c-1] = '\0';
        if (c != 0)     
            p->channel = atoi(cmd_result);

		char    buf[10];
		snprintf(buf,sizeof(buf),"%u", p->channel);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/

/**@param WiFiRadio_AutoChannelSupported                     **/
CPE_STATUS getWiFiRadio_AutoChannelSupported(Instance *ip, char **value)
{
	GS_WiFiRadio *p = (GS_WiFiRadio *)ip->cpeData;
	if ( p ){
        p->autoChannelSupported = 1;
		*value = GS_STRDUP(p->autoChannelSupported? "true": "false");
	}
	return CPE_OK;
}
/**@endparam                                               **/

/**@param WiFiRadio_AutoChannelEnable                     **/
CPE_STATUS setWiFiRadio_AutoChannelEnable(Instance *ip, char *value)
{
    GS_WiFiRadio *p = (GS_WiFiRadio *)ip->cpeData;
    if ( p ){
        p->autoChannelEnable=testBoolean(value);
        char cmd[128]={0};
        char cmd_result[128]={0};

        sprintf(cmd, "iwpriv ath%d acsreport %d", ip->id-1, p->autoChannelEnable );
        cmd_popen(cmd , cmd_result );
    }
    return CPE_OK;
}
CPE_STATUS getWiFiRadio_AutoChannelEnable(Instance *ip, char **value)
{
//  iwpriv ath0 get_acsreport | awk -F ":" '{printf $2}'
    GS_WiFiRadio *p = (GS_WiFiRadio *)ip->cpeData;
    if ( p ){
        char cmd[128]={0};
        char cmd_result[128]={0};
        char *pos = NULL;

        sprintf(cmd, "iwpriv ath%d get_acsreport | awk -F \":\" '{printf $2}'", ip->id-1);
        cmd_popen(cmd , cmd_result );
        if ((pos = strchr(cmd_result, '\n')) != NULL)   *pos = '\0';
        if (NULL != cmd_result)     p->autoChannelEnable = atoi(cmd_result);
        *value = GS_STRDUP(p->autoChannelEnable? "true": "false");
    }
    return CPE_OK;
}
/**@endparam                                               **/
/**@param WiFiRadio_AutoChannelRefreshPeriod                     **/
CPE_STATUS getWiFiRadio_AutoChannelRefreshPeriod(Instance *ip, char **value)
{
    GS_WiFiRadio *p = (GS_WiFiRadio *)ip->cpeData;
    if ( p ){
        if ( p->autoChannelRefreshPeriod )
            *value = GS_STRDUP(p->autoChannelRefreshPeriod);
    }
    return CPE_OK;
}

CPE_STATUS setWiFiRadio_AutoChannelRefreshPeriod(Instance *ip, char *value)
{
	GS_WiFiRadio *p = (GS_WiFiRadio *)ip->cpeData;
	if ( p ){
		COPYSTR(p->autoChannelRefreshPeriod, value);
	}
	return CPE_OK;
}
/**@endparam                                               **/

/**@param WiFiRadio_OperatingChannelBandwidth                     **/
CPE_STATUS getWiFiRadio_OperatingChannelBandwidth(Instance *ip, char **value)
{
    GS_WiFiRadio *p = (GS_WiFiRadio *)ip->cpeData;
    if ( p ){
        char cmd[128]={0};
        char cmd_result[128]={0};
        char *pos = NULL;
        int ret = -1;

        sprintf(cmd, "iwpriv ath%d get_chwidth | awk -F \":\" '{printf $2}'", ip->id-1);
        cmd_popen(cmd , cmd_result );
        if ((pos = strchr(cmd_result, '\n')) != NULL)   *pos = '\0';
        if ( NULL != cmd_result)   ret = atoi(cmd_result);
        if(ret == 0)
        {
            COPYSTR(p->operatingChannelBandwidth , "20 MHz");
        }
        else if ( ret == 1)
        {
            COPYSTR(p->operatingChannelBandwidth , "40 MHz");
        }
        else if ( ret ==2 )
        {
            COPYSTR(p->operatingChannelBandwidth , "80 MHz");
        }

        if ( p->operatingChannelBandwidth )
            *value = GS_STRDUP(p->operatingChannelBandwidth);
    }
    return CPE_OK;
}

CPE_STATUS setWiFiRadio_OperatingChannelBandwidth(Instance *ip, char *value)
{
    GS_WiFiRadio *p = (GS_WiFiRadio *)ip->cpeData;
    if ( p ){
        COPYSTR(p->operatingChannelBandwidth, value);
        char cmd[128]={0};
        char cmd_result[128]={0};

        if (p->operatingChannelBandwidth)
        {
            if ( !strcmp(p->operatingChannelBandwidth,"(HT)20 MHz") )
            {
                sprintf(cmd, "iwpriv ath%d chwidth %d", ip->id-1 , 0 );
            }
            else if ( !strcmp(p->operatingChannelBandwidth,"20 MHz") )
            {
                sprintf(cmd, "iwpriv ath%d chwidth %d", ip->id-1 , 1 );
            }
            else if ( !strcmp(p->operatingChannelBandwidth,"40 MHz") )
            {
                sprintf(cmd, "iwpriv ath%d chwidth %d", ip->id-1 , 2 );
            }
            else if ( !strcmp(p->operatingChannelBandwidth,"Auto") )
            {
                sprintf(cmd, "iwpriv ath%d chwidth %d", ip->id-1 , 3 );
            }
            cmd_popen(cmd , cmd_result );
        }
    }
    return CPE_OK;
}
/**@endparam                                               **/

/**@param WiFiRadio_ExtensionChannel                     **/
CPE_STATUS setWiFiRadio_ExtensionChannel(Instance *ip, char *value)
{
    GS_WiFiRadio *p = (GS_WiFiRadio *)ip->cpeData;
    if ( p ){
        COPYSTR(p->extensionChannel, value);
        char cmd[128]={0};
        char cmd_result[128]={0};

        if (p->extensionChannel)
        {
            if ( !strcmp(p->operatingChannelBandwidth,"Auto") )
                sprintf(cmd, "iwpriv ath%d chextoffset %d", ip->id-1 , 0 );
            else if ( !strcmp(p->operatingChannelBandwidth,"None") )
                sprintf(cmd, "iwpriv ath%d chextoffset %d", ip->id-1 , 1 );
            else if ( !strcmp(p->operatingChannelBandwidth,"AboveControlChannel") )
                sprintf(cmd, "iwpriv ath%d chextoffset %d", ip->id-1 , 2 );
            else if ( !strcmp(p->operatingChannelBandwidth,"BelowControlChannel") )
                sprintf(cmd, "iwpriv ath%d chextoffset %d", ip->id-1 , 3 );

            cmd_popen(cmd , cmd_result );
        }
    }
    return CPE_OK;
}
CPE_STATUS getWiFiRadio_ExtensionChannel(Instance *ip, char **value)
{
    GS_WiFiRadio *p = (GS_WiFiRadio *)ip->cpeData;
    if ( p )
    {
        char cmd[128]={0};
        char cmd_result[128]={0};
        char *pos = NULL;
        int ret = -1;

        sprintf(cmd, "iwpriv ath%d get_chextoffset | awk -F \":\" '{printf $2}'", ip->id-1);
        cmd_popen(cmd , cmd_result );
        if ((pos = strchr(cmd_result, '\n')) != NULL)   *pos = '\0';
        if (NULL != cmd_result)     ret = atoi(cmd_result);
        
        if( ret == 0)
        {
            COPYSTR(p->extensionChannel , "Auto");
        }
        else if( ret == 1)
        {
            COPYSTR(p->extensionChannel , "None");
        }
        else if( ret == 2)
        {
            COPYSTR(p->extensionChannel , "AboveControlChannel");
        }
        else if( ret == 3)
        {
            COPYSTR(p->extensionChannel , "BelowControlChannel");
        }
        if ( p->extensionChannel )
            *value = GS_STRDUP(p->extensionChannel);
    }
    return CPE_OK;
}
    /**@endparam                                               **/

/**@param WiFiRadio_GuardInterval                     **/
CPE_STATUS setWiFiRadio_GuardInterval(Instance *ip, char *value)
{
    GS_WiFiRadio *p = (GS_WiFiRadio *)ip->cpeData;
    if ( p ){
        COPYSTR(p->guardInterval, value);
        char cmd[128]={0};
        char cmd_result[128]={0};
        
        if (p->guardInterval)
        {
            if( !strcmp(p->guardInterval , "400nsec"))
                sprintf(cmd, "iwpriv ath%d shortgi %d", ip->id-1 , 0 );
            else if( !strcmp(p->guardInterval , "800nsec"))
                sprintf(cmd, "iwpriv ath%d shortgi %d", ip->id-1 , 1 );

            cmd_popen(cmd , cmd_result );
        }
    }
    return CPE_OK;
}
CPE_STATUS getWiFiRadio_GuardInterval(Instance *ip, char **value)
{
//iwpriv athX get_shortgi | awk -F ":" '{printf $2'} 
    GS_WiFiRadio *p = (GS_WiFiRadio *)ip->cpeData;
    if ( p ){
        char cmd[128]={0};
        char cmd_result[128]={0};
        char *pos = NULL;
        int ret = -1;

        sprintf(cmd, "iwpriv ath%d get_shortgi | awk -F \":\" '{printf $2}'", ip->id-1);
        cmd_popen(cmd , cmd_result );
        if ((pos = strchr(cmd_result, '\n')) != NULL)   *pos = '\0';
        if (NULL != cmd_result)     ret = atoi(cmd_result);
        
        if ( ret == 0)
        {
            COPYSTR(p->guardInterval ,"400nsec" );
        }
        else if ( ret == 1)
        {
            COPYSTR(p->guardInterval ,"800nsec" );
        }
        if ( p->guardInterval )
            *value = GS_STRDUP(p->guardInterval);
    }
    return CPE_OK;
}
/**@endparam                                               **/

/**@param WiFiRadio_MCS                     **/
CPE_STATUS setWiFiRadio_MCS(Instance *ip, char *value)
{
    GS_WiFiRadio *p = (GS_WiFiRadio *)ip->cpeData;
    if ( p ){
        COPYSTR(p->mCS , value);
        char cmd[128]={0};
        char cmd_result[128]={0};
        int rates;

        if (ip->id == 1)
        {
            if ( NULL != p->mCS && atoi(p->mCS) >= 0)
            {
                if( 1 == strlen(p->mCS) )
                {
                    rates = p->mCS[0];
                    if ( rates >=65 && rates <=70 )
                        rates = rates - 55 ; 
                    else if ( rates >= 48 && rates <= 57)
                        rates = rates - 48 ;

                    sprintf(cmd, "iwpriv ath%d set11NRates 0x8%d8%d8%d8%d", ip->id-1 , rates , rates ,rates , rates);
                    cmd_popen(cmd , cmd_result );
                    memset(cmd , 0x0 , sizeof(cmd));
                    memset(cmd_result , 0x0 , sizeof(cmd_result));
                    sprintf(cmd, "iwpriv ath%d set11NRetries %s", ip->id-1, "0x04040404");
                    cmd_popen(cmd , cmd_result );
                }
            }
            else if ( NULL != p->mCS && atoi(p->mCS) == -1)   
            {
                sprintf(cmd, "iwpriv ath%d set11NRates %d", ip->id-1 , 0);
                cmd_popen(cmd , cmd_result );
                memset(cmd , 0x0 , sizeof(cmd));
                memset(cmd_result , 0x0 , sizeof(cmd_result));
                sprintf(cmd, "iwpriv ath%d set11NRetries %d", ip->id-1, 0);
                cmd_popen(cmd , cmd_result );
            }
        }
        else if (ip->id ==2) // > 9  disable fix rate
        {
            if ( NULL != p->mCS) 
                sprintf(cmd, "iwpriv ath%d vhtmcs %s", ip->id-1 , (atoi(p->mCS) == -1) ? "10":p->mCS);
 

            cmd_popen(cmd , cmd_result );
        }
        else{}
    }
    return CPE_OK;
}
CPE_STATUS getWiFiRadio_MCS(Instance *ip, char **value)
{
    GS_WiFiRadio *p = (GS_WiFiRadio *)ip->cpeData;
    if ( p ){

        char cmd[128]={0};
        char cmd_result[128]={0};
        char result[128]={0};
        int mcs_result=0;
        char *pos = NULL;

        if (ip->id == 1)
        {
            sprintf(cmd, "iwpriv ath%d get11NRates | awk -F \":\" '{printf $2}'", ip->id-1);
            cmd_popen(cmd , cmd_result );
            if ((pos = strchr(cmd_result, '\n')) != NULL)   *pos = '\0';
            if (NULL != cmd_result)    sprintf(result, "%08x" , atoi(cmd_result));
            
            if (NULL != cmd_result && atoi(cmd_result) == 0)
            {
                mcs_result = -1;
            }
            else if ( 8 == strlen(result))
            {
                mcs_result = result[1];
            }
            else if ( 16 == strlen(result))
            {
                mcs_result = result[9];
            }

            
            if ( mcs_result >=65 && mcs_result <=70 )
                mcs_result = mcs_result - 55 ; 
            else if ( mcs_result >= 48 && mcs_result <= 57)
                mcs_result = mcs_result - 48 ; 

            sprintf(result , "%d" , mcs_result);
            if(NULL != result)  COPYSTR(p->mCS , result);
        }

        if (ip->id == 2) // > 9  disable fix rate
        {
            sprintf(cmd, "iwpriv ath%d get_vhtmcs | awk -F \":\" '{printf $2}'", ip->id-1);
            cmd_popen(cmd , cmd_result );
            if ((pos = strchr(cmd_result, '\n')) != NULL)   *pos = '\0';
            
            sprintf(result , cmd_result);
            if ( NULL != result && atoi(result) == 0 )
                sprintf(result , "-1");
        }

        if (NULL != result)     COPYSTR(p->mCS , result);

        if ( p->mCS )
        *value = GS_STRDUP(p->mCS);
    }
    return CPE_OK;
}
/**@endparam                                               **/

/**@param WiFiRadio_TransmitPowerSupported                     **/
CPE_STATUS getWiFiRadio_TransmitPowerSupported(Instance *ip, char **value)
{
	GS_WiFiRadio *p = (GS_WiFiRadio *)ip->cpeData;
	if ( p ){
        char cmd[128]={0};

        sprintf(cmd, "0,25,50,75,100");
        if (NULL != cmd)     COPYSTR(p->transmitPowerSupported , cmd);
		*value = GS_STRDUP(p->transmitPowerSupported);
	}
	return CPE_OK;
}
/**@endparam                                               **/

/**@param WiFiRadio_TransmitPower                     **/
CPE_STATUS setWiFiRadio_TransmitPower(Instance *ip, char *value)
{
    GS_WiFiRadio *p = (GS_WiFiRadio *)ip->cpeData;
    if ( p ){
        p->transmitPower=atoi(value);
        char cmd[128]={0};
        char cmd_result[128]={0};
        //wireless.wifi0.tpscale='1'

        if (p->transmitPower || p->transmitPower == 0)
        {
            int transmitpower = (100 - p->transmitPower) / 25 ;

            sprintf(cmd, "iwpriv wifi%d tpscale %d", ip->id-1 , transmitpower);
            cmd_popen(cmd , cmd_result );

            memset(cmd , 0x0 , sizeof(cmd));
            sprintf(cmd, "uci set wireless.wifi%d.tpscale='%d'", ip->id-1 , transmitpower);
            system(cmd);
            system("uci commit wireless");
        }
    }

    return CPE_OK;

}
CPE_STATUS getWiFiRadio_TransmitPower(Instance *ip, char **value)
{
//  iwlist ath0 txpower | grep Current | awk -F ":" '{printf $2}' | awk -F " " '{print $1}'
    GS_WiFiRadio *p = (GS_WiFiRadio *)ip->cpeData;
    if ( p ){
        char cmd[128]={0};
        char cmd_result[128]={0};
        char *pos = NULL;
        char    buf[10];

        sprintf(cmd, "iwpriv wifi%d get_tpscale | awk -F \":\" '{print $2}'", ip->id-1);
        cmd_popen(cmd , cmd_result );
        if ((pos = strchr(cmd_result, '\n')) != NULL)   *pos = '\0';
        if ( NULL != cmd_result)
        {
            p->transmitPower = atoi(cmd_result);

            if (p->transmitPower == 0)
                snprintf(buf,sizeof(buf),"%d", 100);
            else if (p->transmitPower == 1)
                snprintf(buf,sizeof(buf),"%d", 75);
            else if (p->transmitPower == 2)
                snprintf(buf,sizeof(buf),"%d", 50);
            else if (p->transmitPower == 3)
                snprintf(buf,sizeof(buf),"%d", 25);
            else if (p->transmitPower == 4)
                snprintf(buf,sizeof(buf),"%d", 0);
            *value = GS_STRDUP(buf);
        }
    }
    return CPE_OK;
}
/**@endparam                                               **/

/**@param WiFiRadio_IEEE80211hSupported                     **/
CPE_STATUS getWiFiRadio_IEEE80211hSupported(Instance *ip, char **value)
{
	GS_WiFiRadio *p = (GS_WiFiRadio *)ip->cpeData;
	if ( p ){
        p->iEEE80211hSupported = 1;
		*value = GS_STRDUP(p->iEEE80211hSupported? "true": "false");
	}
	return CPE_OK;
}
/**@endparam                                               **/

/**@param WiFiRadio_IEEE80211hEnabled                     **/
CPE_STATUS setWiFiRadio_IEEE80211hEnabled(Instance *ip, char *value)
{
    GS_WiFiRadio *p = (GS_WiFiRadio *)ip->cpeData;
    if ( p ){
        p->iEEE80211hEnabled=testBoolean(value);
        char cmd[128]={0};
        char cmd_result[128]={0};

        sprintf(cmd, "iwpriv ath%d doth %d" , ip->id-1 , p->iEEE80211hEnabled );
        cmd_popen(cmd , cmd_result );
    }
    return CPE_OK;
}
CPE_STATUS getWiFiRadio_IEEE80211hEnabled(Instance *ip, char **value)
{
	GS_WiFiRadio *p = (GS_WiFiRadio *)ip->cpeData;
	if ( p ){
        char cmd[128]={0};
        char cmd_result[128]={0};


        sprintf(cmd, "iwpriv ath%d get_doth | awk -F ':' '{print $2}'", ip->id-1);
        cmd_popen(cmd , cmd_result );
        p->iEEE80211hEnabled = atoi(cmd_result);
        *value = GS_STRDUP(p->iEEE80211hEnabled? "true": "false");
    }
    return CPE_OK;
}
/**@endparam                                               **/

/**@param WiFiRadio_RegulatoryDomain                     **/
CPE_STATUS setWiFiRadio_RegulatoryDomain(Instance *ip, char *value)
{
    GS_WiFiRadio *p = (GS_WiFiRadio *)ip->cpeData;
    if ( p ){
        COPYSTR(p->regulatoryDomain, value);
        char cmd[128]={0};
        char cmd_result[128]={0};


        sprintf(cmd, "iwpriv wifi%d setCountry %s", ip->id-1 , p->regulatoryDomain);
        cmd_popen(cmd , cmd_result );
    }
    return CPE_OK;
}
CPE_STATUS getWiFiRadio_RegulatoryDomain(Instance *ip, char **value)
{
    GS_WiFiRadio *p = (GS_WiFiRadio *)ip->cpeData;
    if ( p ){
        char cmd[128]={0};
        char cmd_result[128]={0};
        char *pos = NULL;

        sprintf(cmd, "iwpriv wifi%d getCountry | awk -F ':' '{print $2}'", ip->id-1);
        cmd_popen(cmd , cmd_result );
        if ((pos = strchr(cmd_result, '\n')) != NULL)   *pos = '\0';
        if ( NULL != cmd_result)       COPYSTR( p->regulatoryDomain , cmd_result);
        if ( p->regulatoryDomain )
            *value = GS_STRDUP(p->regulatoryDomain);
    }
    return CPE_OK;
}
/**@endparam                                               **/
/**@endobj WiFiRadio **/

/**@obj WiFiSSIDStats **/
CPE_STATUS  initWiFiSSIDStats(CWMPObject *o, Instance *ip)
{
	/* initialize object */
	WiFiSSIDStats *p = (WiFiSSIDStats *)GS_MALLOC( sizeof(struct WiFiSSIDStats));
	memset(p, 0, sizeof(struct WiFiSSIDStats));
	ip->cpeData = (void *)p;

	return CPE_OK;
}

/**@param WiFiSSIDStats_BytesSent                     **/
CPE_STATUS getWiFiSSIDStats_BytesSent(Instance *ip, char **value)
{
//  ifconfig br-lan | grep -r "RX bytes" | awk -F ":" '{print $3}' | awk -F " " '{print $1}'

    WiFiSSIDStats *p = (WiFiSSIDStats *)ip->cpeData;
    if ( p ){
        char    buf[30];
        char cmd[128]={0};
        char cmd_result[128]={0};
        char *pos = NULL;

        sprintf(cmd, "ifconfig ath%s| grep -r \"RX bytes\" | awk -F \":\" '{print $3}' | awk -F \" \" '{print $1}'"  ,get_topology_iface_name(ip->parent->id-1) );
        cmd_popen(cmd , cmd_result);
        if ((pos = strchr(cmd_result, '\n')) != NULL)   *pos = '\0';
        if( NULL != cmd_result)
            p->bytesSent = atoi(cmd_result);
        
            snprintf(buf,sizeof(buf),"%lld", p->bytesSent);
            *value = GS_STRDUP(buf);
    }
    return CPE_OK;
}
/**@endparam                                               **/

/**@param WiFiSSIDStats_BytesReceived                     **/
CPE_STATUS getWiFiSSIDStats_BytesReceived(Instance *ip, char **value)
{
//  ifconfig br-lan | grep -r "RX bytes" | awk -F ":" '{print $2}' | awk -F " " '{print $1}'

    WiFiSSIDStats *p = (WiFiSSIDStats *)ip->cpeData;
    if ( p ){
        char    buf[30];
        char cmd[128]={0};
        char cmd_result[128]={0};
        char *pos = NULL;

        sprintf(cmd, "ifconfig ath%s| grep -r \"RX bytes\" | awk -F \":\" '{print $2}' | awk -F \" \" '{print $1}'" , get_topology_iface_name(ip->parent->id-1));
        cmd_popen(cmd , cmd_result);
        if ((pos = strchr(cmd_result, '\n')) != NULL)   *pos = '\0';
        if( NULL != cmd_result)
            p->bytesReceived = atoi(cmd_result);

        snprintf(buf,sizeof(buf),"%lld", p->bytesReceived);
        *value = GS_STRDUP(buf);
    }
    return CPE_OK;
}
/**@endparam                                               **/

/**@param WiFiSSIDStats_PacketsSent                     **/
CPE_STATUS getWiFiSSIDStats_PacketsSent(Instance *ip, char **value)
{
//  ifconfig br-lan | grep -r "TX packets" | awk -F ":" '{print $2}' | awk -F " " '{print $1}'

    WiFiSSIDStats *p = (WiFiSSIDStats *)ip->cpeData;
    if ( p ){
        char    buf[30];
        char cmd[128]={0};
        char cmd_result[128]={0};
        char *pos = NULL;
 
        sprintf(cmd, "ifconfig ath%s | grep -r \"TX packets\" | awk -F \":\" '{print $2}' | awk -F \" \" '{print $1}'" ,get_topology_iface_name(ip->parent->id-1) );
        cmd_popen(cmd , cmd_result);
        if ((pos = strchr(cmd_result, '\n')) != NULL)   *pos = '\0';
        if( NULL != cmd_result)
            p->packetsSent = atoi(cmd_result);

        snprintf(buf,sizeof(buf),"%lld", p->packetsSent);
        *value = GS_STRDUP(buf);
    }
    return CPE_OK;
}
/**@endparam                                               **/

/**@param WiFiSSIDStats_PacketsReceived                     **/
CPE_STATUS getWiFiSSIDStats_PacketsReceived(Instance *ip, char **value)
{
//  ifconfig br-lan | grep -r "RX packets" | awk -F ":" '{print $2}' | awk -F " " '{print $1}'
	WiFiSSIDStats *p = (WiFiSSIDStats *)ip->cpeData;
	if ( p ){
        char    buf[30];
        char cmd[128]={0};
        char cmd_result[128]={0};
        char *pos = NULL;

        sprintf(cmd, "ifconfig ath%s| grep -r \"RX packets\" | awk -F \":\" '{print $2}' | awk -F \" \" '{print $1}'" , get_topology_iface_name(ip->parent->id-1));
        cmd_popen(cmd , cmd_result);
        if ((pos = strchr(cmd_result, '\n')) != NULL)   *pos = '\0';
        if( NULL != cmd_result)
            p->packetsReceived = atoi(cmd_result);

		snprintf(buf,sizeof(buf),"%lld", p->packetsReceived);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/

/**@param WiFiSSIDStats_ErrorsSent                     **/
CPE_STATUS getWiFiSSIDStats_ErrorsSent(Instance *ip, char **value)
{
// ifconfig br-lan | grep -r "TX packets" | awk -F ":" '{print $3}'| awk -F " " '{print $1}'
	WiFiSSIDStats *p = (WiFiSSIDStats *)ip->cpeData;
	if ( p ){
        char    buf[30];
        char cmd[128]={0};
        char cmd_result[128]={0};
        char *pos = NULL;

        sprintf(cmd, "ifconfig ath%s| grep -r \"TX packets\" | awk -F \":\" '{print $3}' | awk -F \" \" '{print $1}'" , get_topology_iface_name(ip->parent->id-1));
        cmd_popen(cmd , cmd_result);
        if ((pos = strchr(cmd_result, '\n')) != NULL)   *pos = '\0';
        if( NULL != cmd_result)
            p->errorsSent = atoi(cmd_result);

        snprintf(buf,sizeof(buf),"%u", p->errorsSent);
        *value = GS_STRDUP(buf);
    }
    return CPE_OK;
}
/**@endparam                                               **/

/**@param WiFiSSIDStats_ErrorsReceived                     **/
CPE_STATUS getWiFiSSIDStats_ErrorsReceived(Instance *ip, char **value)
{
// ifconfig br-lan | grep -r "RX packets" | awk -F ":" '{print $3}'| awk -F " " '{print $1}'
	WiFiSSIDStats *p = (WiFiSSIDStats *)ip->cpeData;
	if ( p ){
        char    buf[30];
        char cmd[128]={0};
        char cmd_result[128]={0};
        char *pos = NULL;

        sprintf(cmd, "ifconfig ath%s| grep -r \"RX packets\" | awk -F \":\" '{print $3}' | awk -F \" \" '{print $1}'" , get_topology_iface_name(ip->parent->id-1));
        cmd_popen(cmd , cmd_result);
        if ((pos = strchr(cmd_result, '\n')) != NULL)   *pos = '\0';
        if( NULL != cmd_result)
            p->packetsReceived = atoi(cmd_result);

        snprintf(buf,sizeof(buf),"%u", p->errorsReceived);
        *value = GS_STRDUP(buf);
    }
    return CPE_OK;
}
/**@endparam                                               **/
/**@endobj WiFiSSIDStats **/

/**@obj WiFiSSID **/
CPE_STATUS  addWiFiSSID(CWMPObject *o, Instance *ip)
{
	/* add instance data */
	GS_WiFiSSID *p = (GS_WiFiSSID *)GS_MALLOC( sizeof(struct GS_WiFiSSID));
	memset(p, 0, sizeof(struct GS_WiFiSSID));
	ip->cpeData = (void *)p;
	return CPE_OK;
}
CPE_STATUS  delWiFiSSID(CWMPObject *o, Instance *ip)
{
    /* delete instance data */
    GS_WiFiSSID *p = (GS_WiFiSSID *)ip->cpeData;
    if( p ){
        //TODO: free instance data
        GS_FREE(p);
    }
    return CPE_OK;
}

/**@param WiFiSSID_Enable                     **/
CPE_STATUS setWiFiSSID_Enable(Instance *ip, char *value)
{
    GS_WiFiSSID *p = (GS_WiFiSSID *)ip->cpeData;
    if ( p )
    {
        char cmd[128]={0};
        char cmd_result[128]={0};

        p->enable=testBoolean(value);
        if ( p->enable)
        {
            sprintf(cmd, "ifconfig ath%s up", get_topology_iface_name(ip->id-1) );
            cmd_popen(cmd, cmd_result);
        }
        else
        {
            sprintf(cmd, "ifconfig ath%s down", get_topology_iface_name(ip->id-1) );
            cmd_popen(cmd, cmd_result);
        }
    }
    return CPE_OK;
}
CPE_STATUS getWiFiSSID_Enable(Instance *ip, char **value)
{
    GS_WiFiSSID *p = (GS_WiFiSSID *)ip->cpeData;
    if ( p )
    {
        char cmd[128]={0};
        char cmd_result[128]={0};
        const char default_status[]="UP";
        char *pos = NULL;

        sprintf(cmd, "ifconfig ath%s | grep UP | awk -F \" \" '{print $1}'" , get_topology_iface_name(ip->id-1));
        cmd_popen(cmd , cmd_result);
        if ( NULL != cmd_result){
            if ((pos = strchr(cmd_result, '\n')) != NULL)
                *pos = '\0';

            if(!strncmp(cmd_result, default_status, strlen(default_status)))
                p->enable = 1;
            else
                p->enable = 0;
        }
        *value = GS_STRDUP(p->enable? "true": "false");
    }
    return CPE_OK;
}
/**@endparam                                               **/

/**@param WiFiSSID_Status                     **/
CPE_STATUS getWiFiSSID_Status(Instance *ip, char **value)
{
    GS_WiFiSSID *p = (GS_WiFiSSID *)ip->cpeData;
    if ( p ){
        char cmd[128]={0};
        char cmd_result[128]={0};
        const char default_status[] = "Not-Associated";

        sprintf(cmd, "iwconfig ath%s | grep \"Access Point\" | awk -F\" \" '{print $6}'", get_topology_iface_name(ip->id-1));
        cmd_popen(cmd, cmd_result);
        if(strncmp(cmd_result, default_status, strlen(default_status)) != 0)
        {
            COPYSTR(p->status , "UP");
        }
        else
        {
            COPYSTR(p->status , "Down");
        }
        if ( p->status )
            *value = GS_STRDUP(p->status);
    }
    return CPE_OK;
}
/**@endparam                                               **/

/**@param WiFiSSID_Name                     **/
CPE_STATUS getWiFiSSID_Name(Instance *ip, char **value)
{
    GS_WiFiSSID *p = (GS_WiFiSSID *)ip->cpeData;
    if ( p ){
            char cmd[128]={0};
            char cmd_result[128]={0};
            char *pos = NULL;

            sprintf(cmd, "uci get wireless.@wifi-iface[%d].ssid", get_uci_iface_name(ip->id-1));
            cmd_popen(cmd , cmd_result );
            if ((pos = strchr(cmd_result, '\n')) != NULL)   *pos = '\0';

            if (NULL != cmd_result) 
            {
                COPYSTR( p->name, cmd_result );
                //util_check_string_on_web(p->name , p->name);
            }

        if ( p->name )
            *value = GS_STRDUP(p->name);
    }
    return CPE_OK;
}
/**@endparam                                               **/

/**@param WiFiSSID_BSSID                     **/
CPE_STATUS getWiFiSSID_BSSID(Instance *ip, char **value)
{
    GS_WiFiSSID *p = (GS_WiFiSSID *)ip->cpeData;
    if ( p ){
            char cmd[128]={0};
            char cmd_result[128]={0};
            char *pos = NULL;

            sprintf(cmd, "iwconfig ath%s | grep \"Access Point\" | awk -F\" \" '{print $6}'", get_topology_iface_name(ip->id-1));
            cmd_popen(cmd, cmd_result);
            if ((pos = strchr(cmd_result, '\n')) != NULL)   *pos = '\0';

            if(NULL != cmd_result)  COPYSTR(p->bSSID , cmd_result);

        if ( p->bSSID )
            *value = GS_STRDUP(p->bSSID);
    }
    return CPE_OK;
}
/**@endparam                                               **/

/**@param WiFiSSID_MACAddress                     **/
CPE_STATUS getWiFiSSID_MACAddress(Instance *ip, char **value)
{
    GS_WiFiSSID *p = (GS_WiFiSSID *)ip->cpeData;
    if ( p ){
        char cmd[128]={0};
        char cmd_result[128]={0};
        char *pos = NULL;

        sprintf(cmd, "ifconfig ath%s | grep -r \"HWaddr\" | awk -F \" \" '{print $5}'"  , get_topology_iface_name(ip->id-1) );
        cmd_popen(cmd , cmd_result);
        if ((pos = strchr(cmd_result, '\n')) != NULL)   *pos = '\0';
        if( NULL != cmd_result) 
            COPYSTR(p->mACAddress , cmd_result);
        if ( p->mACAddress )
            *value = GS_STRDUP(p->mACAddress);
    }
    return CPE_OK;
}
/**@endparam                                               **/

/**@param WiFiSSID_SSID                     **/
CPE_STATUS setWiFiSSID_SSID(Instance *ip, char *value)
{
    GS_WiFiSSID *p = (GS_WiFiSSID *)ip->cpeData;
    if ( p )
    {
        if ( get_role() ==1 &&  ip->id != 2 && ip->id != 4 )  // Only Can change CAP
        {
            char cmd[128]={0};
            char cmd_result[128] = {0};

            sprintf(cmd, "uci get wireless.@wifi-iface[%d].ssid", 0  );
            cmd_popen(cmd , cmd_result );
            if (0 == strncmp(cmd_result , value , strlen(value)))
                return CPE_OK;

            if ( strlen(value) > 0 && strlen(value) <=32)
            {
                int ret  = util_check_string( value , cmd);
                if (ret == -1)
                    return CPE_ERR;

                COPYSTR(p->sSID, cmd);

                memset(cmd , 0x0 , sizeof(cmd));
                memset(cmd_result , 0x0 , sizeof(cmd_result));
                sprintf(cmd, "uci set wireless.@wifi-iface[%d].ssid=\"%s\"", 0 , p->sSID );
                cmd_popen(cmd , cmd_result );
                
                memset(cmd , 0x0 , sizeof(cmd));
                memset(cmd_result , 0x0 , sizeof(cmd_result));
                sprintf(cmd, "uci set wireless.@wifi-iface[%d].ssid=\"%s\"", 1 , p->sSID );
                cmd_popen(cmd , cmd_result );
                
                system("uci commit wireless");
                reload_query();
                return CPE_OK;
            }
        }
    }
    return CPE_ERR;
}
CPE_STATUS getWiFiSSID_SSID(Instance *ip, char **value)
{
    GS_WiFiSSID *p = (GS_WiFiSSID *)ip->cpeData;
    if ( p )
    {
        char cmd[128]={0};
        char cmd_result[128]={0};
        char *pos = NULL;



        sprintf(cmd, "uci get wireless.@wifi-iface[%d].ssid", get_uci_iface_name(ip->id-1));
        cmd_popen(cmd , cmd_result );
        if ((pos = strchr(cmd_result, '\n')) != NULL)   *pos = '\0';

        if ( NULL != cmd_result)
        {
            //memset(cmd , 0x0 , sizeof(cmd));
            //util_check_string_on_web(cmd_result , cmd);
            COPYSTR( p->sSID, cmd_result );
        }

        if ( p->sSID ){
            *value = GS_STRDUP(p->sSID);
        }
    }

    return CPE_OK;
}
/**@endparam                                               **/
/**@endobj WiFiSSID **/

/**@obj WiFi **/
CPE_STATUS  initWiFi(CWMPObject *o, Instance *ip)
{
    
    return CPE_OK;
}
/**@param WiFi_RadioNumberOfEntries                     **/
CPE_STATUS getWiFi_RadioNumberOfEntries(Instance *ip, char **value)
{

    cwmpGetObjInstanceCntStr("Radio", value);
    return CPE_OK;
}
/**@endparam                                               **/

/**@param WiFi_SSIDNumberOfEntries                     **/
CPE_STATUS getWiFi_SSIDNumberOfEntries(Instance *ip, char **value)
{

    cwmpGetObjInstanceCntStr("SSID", value);
    return CPE_OK;
}
/**@endparam                                               **/

/**@param WiFi_AccessPointNumberOfEntries                     **/
CPE_STATUS getWiFi_AccessPointNumberOfEntries(Instance *ip, char **value)
{
    cwmpGetObjInstanceCntStr("AccessPoint", value);
    return CPE_OK;
}
/**@endparam                                               **/
/**@endobj WiFi **/
