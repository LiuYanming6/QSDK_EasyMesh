/*******************************************************************************
 *        Copyright (c) 2017 TWin Advanced Technology Corp., Ltd.
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
 * @file app_WiFiSON.cc
 * @brief
 * @author T&W Technology co., Ltd.
 * @bug No known bugs
 */

/*-------------------------------------------------------------------------*/
/*                        INCLUDE HEADER FILES                             */
/*-------------------------------------------------------------------------*/
#include <twbox_hyd.h>

/*-------------------------------------------------------------------------*/
/*                           DEFINITIONS                                   */
/*-------------------------------------------------------------------------*/

enum{
    RADIO_2G=0,
    RADIO_5G
};

typedef enum{
    SIGNAL_ERROR=0,
    SIGNAL_FAR,
    SIGNAL_NEAR,
    SIGNAL_SUITABLE,
    SIGNAL_DISCONNECT,
} SIGNAL_DEF;
/*-------------------------------------------------------------------------*/
/*                           VARIABLES                                     */
/*-------------------------------------------------------------------------*/

static int max_number = 0;
/*-------------------------------------------------------------------------*/
/*                           FUNCTIONS                                     */
/*-------------------------------------------------------------------------*/

int init_App_WiFiSON()
{
    int ret;
    sysinteract("" , g_ethbackhaul , sizeof(g_ethbackhaul) , "uci get repacd.repacd.IsEthBackhaul");
    sysinteract("" , g_role , sizeof(g_role), "uci get repacd.repacd.DeviceRole");
    
    if ((ret = ubox_get_qca_wifison_dev_num(&max_number, 1)) != 0)
        return RESULT_ERROR;
    else
        return RESULT_OK;
}

int get_WiFiSON_Number_MAX()
{
    return max_number+1;
}

int get_WiFiSON_SignalStrength_Number_MAX()
{
    return 2;
}

int get_WiFiSON_BackhaulLinkIndicator_Number_MAX()
{
    return 2;
}

int get_WiFiSON_Number(char *token_nm, char *get_str, int x, int y)
{
    sprintf (get_str, "%d" , max_number+1);
    return RESULT_OK;

}

int get_WiFiSON_x_MAC(char *token_nm, char *get_str, int x, int y)
{
    int ret;
    int  dev = 0;
    char addr[32] = {0};
    hyd_wifison_dev son_dev[12] = {0};
    //hyd_wifison_dev *son_dev;

    if ((ret = ubox_get_qca_wifison_dev_num(&dev, 0)) != 0)
    {
        return RESULT_ERROR;
    }
    else
    {
        dev++;
        //hyd_wifison_dev *son_dev = malloc(dev+1 * sizeof(hyd_wifison_dev)); //TODO init structure
        //son_dev = malloc(sizeof(hyd_wifison_dev));
        memset(son_dev, 0, sizeof(hyd_wifison_dev));
        ubox_get_qca_wifison_dev_topology(dev, son_dev, 0);
        strcpy(addr, son_dev[x].mac);

        //if (son_dev != NULL)
            //  free(son_dev);
        sprintf(get_str, "%s", addr);
        return RESULT_OK;
    }

}

int get_WiFiSON_x_ParentAPMAC(char *token_nm, char *get_str, int x, int y)
{

    int ret;
    int dev = 0;
    char addr[32] = {0};
    hyd_wifison_dev son_dev[12] = {0};

    if((ret = ubox_get_qca_wifison_dev_num(&dev, 0)) != 0)
    {
        return RESULT_ERROR;
    }
    else
    {
        //hyd_wifison_dev *son_dev = malloc(dev+1 * sizeof(hyd_wifison_dev)); //TODO init structure
        ubox_get_qca_wifison_dev_topology(dev+1, son_dev, 0);
        strcpy(addr, son_dev[x].pmac);
        //free(son_dev);
        sprintf(get_str, "%s", addr);
        return RESULT_OK;
    }

}

int get_WiFiSON_x_SignalStrength(char *token_nm, char *get_str, int x, int y)
{
    char default_role[]="CAP";
    if (strncmp( g_role, default_role , strlen(default_role)) ==0)
    {
    }
    else
    {
        if (g_ethbackhaul[0]=='0')
        {
            char access_point[64]={0};
            const char default_status[] = "Not-Associated";
            char rssi[4] = {0};
            int ret;
            //iwconfig ath01 | grep 'Signal level' | awk -F'=' '{print $3}' | awk '{print $1}'
            
            switch(x)
            {
                case RADIO_2G:
                    ret = sysinteract("00:AA:BB:CC:DD:11",access_point, sizeof(access_point), "iwconfig ath01 | grep \"Access Point\" | awk -F\" \" '{print $6}'");
                    if(ret >= 0 && strncmp(access_point, default_status, strlen(default_status)) != 0)
                    {
                        ret = sysinteract( "-95" , rssi , sizeof(rssi) , "iwconfig ath01 | grep 'Signal level' | awk -F'=' '{print $3}' | awk '{print $1}'");
                    }
                    break;
                case RADIO_5G:
                    ret = sysinteract("00:AA:BB:CC:DD:11",access_point, sizeof(access_point), "iwconfig ath11 | grep \"Access Point\" | awk -F\" \" '{print $6}'");
                    if(ret >= 0 && strncmp(access_point, default_status, strlen(default_status)) != 0)
                    {
                        ret = sysinteract( "-95" , rssi , sizeof(rssi) , "iwconfig ath11 | grep 'Signal level' | awk -F'=' '{print $3}' | awk '{print $1}'");
                    }
                    break;
                default:
                    return RESULT_ERROR;
            }
            if (ret >= 0)
            {
                if( NULL != rssi )
                {
                    ret = atoi(rssi);
                }
                if ( ret > -95 && ret !=0)
                {
                    sprintf(get_str, "%d", ret+95);
                }
                else
                {
                    sprintf(get_str, "%d", -95);
                }
            }
        }
        else
            sprintf(get_str, "%s", "Ethernet Backhaul");
    }
    return RESULT_OK;
}

int get_WiFiSON_x_BackhaulLinkIndicator(char *token_nm, char *get_str, int x, int y)
{
    char default_role[]="CAP";
    if (strncmp( g_role, default_role , strlen(default_role)) ==0)
    {
    }
    else
    {
        if(g_ethbackhaul[0]=='0')
        {
            SIGNAL_DEF strength;
            char rssi_2g[4] = {0};
            char rssi_5g[4] = {0};
            int rssi =0;
            int ret;
            char access_point[64]={0};
            const char default_status[] = "Not-Associated";

            switch(x)
            {
                case RADIO_2G:
                    ret = sysinteract("00:AA:BB:CC:DD:11",access_point, sizeof(access_point), "iwconfig ath01 | grep \"Access Point\" | awk -F\" \" '{print $6}'");
                    if(ret >= 0 && strncmp(access_point, default_status, strlen(default_status)) != 0)
                    {
                            sysinteract( "" , rssi_2g , sizeof(rssi_2g) , "iwconfig ath01 | grep 'Signal level' | awk -F'=' '{print $3}' | awk '{print $1}'");
                            rssi = atoi(rssi_2g);
                    }
                    break;
                case RADIO_5G:
                    ret = sysinteract("00:AA:BB:CC:DD:11",access_point, sizeof(access_point), "iwconfig ath11 | grep \"Access Point\" | awk -F\" \" '{print $6}'"); 
                    if(ret >= 0 && strncmp(access_point, default_status, strlen(default_status)) != 0)
                    {
                        sysinteract( "" , rssi_5g , sizeof(rssi_5g) , "iwconfig ath11 | grep 'Signal level' | awk -F'=' '{print $3}' | awk '{print $1}'");
                        rssi = atoi(rssi_5g);
                    }
                    break;
                default:
                    return RESULT_ERROR;
            }

            if ( ret >=0 && rssi < 0)
            {
                rssi += 95;
                if ( rssi < 25 && rssi > 0 )
                    strength = SIGNAL_FAR;
                else if ( rssi >= 25 && rssi < 95 )
                    strength = SIGNAL_SUITABLE;
                else
                    strength = SIGNAL_DISCONNECT;

                switch(strength)
                {
                    case SIGNAL_DISCONNECT:
                        sprintf(get_str, "%s", "Red");
                        break;
                    case SIGNAL_FAR:
                    case SIGNAL_NEAR:
                        sprintf(get_str, "%s", "Amber");
                        break;
                    case SIGNAL_SUITABLE:
                        sprintf(get_str, "%s", "Green");
                        break;
                }
            }
            else
                sprintf(get_str, "%s", "Red");
        }
        else
        {
            sprintf(get_str, "%s", "Green");
        }
    }
    return RESULT_OK;
}

int get_WiFiSON_NickName(char *token_nm, char *get_str, int x, int y)
{
/* device.nickiname */
    const char default_status[] = "JCM0112";
    char *pos = NULL;
    char nickname[64]={0};
    char buf[10]={0};
    sysinteract("", nickname , sizeof(nickname) , "uci get device.nickname" );
    if( strlen(nickname) == 0 )
    {
        sprintf(nickname,default_status);
    }
    else
    {
        if ((pos = strchr(nickname, '\n')) != NULL)
        *pos = '\0';
    }
    
    twbox_util_check_string_on_web (nickname , get_str);
    //sprintf(get_str, "%s", nickname);


    return RESULT_OK;
}

int set_WiFiSON_NickName(char *token_nm, char *set_str, int x, int y)
{
/* device.nickiname */

    char buf[10]={0};
    char nickname[64] = {0};
    
    if(strlen(set_str) > 30 )
        return RESULT_ERROR;

    int ret = twbox_util_check_char(set_str, nickname);

    if (ret == -1)
    {
    	return RESULT_ERROR;
    }

    sysinteract("", buf , sizeof(buf) , "uci set device.nickname=\"%s\"" , nickname );
    system("uci commit device");
    system("/etc/init.d/mDNSResponder restart &");

    return RESULT_OK;
}
/************************ END *********************************************/
