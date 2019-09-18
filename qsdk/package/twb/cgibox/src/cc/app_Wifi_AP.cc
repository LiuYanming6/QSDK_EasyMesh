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
 * @file app_Wifi_APP.cc
 * @brief
 * @author T&W Technology co., Ltd.
 * @bug No known bugs
 */

/*-------------------------------------------------------------------------*/
/*                        INCLUDE HEADER FILES                             */
/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/*                           DEFINITIONS                                   */
/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/*                           VARIABLES                                     */
/*-------------------------------------------------------------------------*/
static char g_role[4]={0};
static char g_ethbackhaul[4]={0};

/*-------------------------------------------------------------------------*/
/*                           FUNCTIONS                                     */
/*-------------------------------------------------------------------------*/

/*
 * 
RE
ath   uci
0      0
01     2
02     4
1      1
11     3
12     5
* 

CAP

ath   uci
0     0
01    4
1     1
11    5

*/
int check_hex(char *input , int ret)
{
    int i = 0;
    int j;
    char ssidinvaildchar[]={'|',';',']'};
    char pwinvaildchar[]={' ','|',';',']'};
    while(i < strlen(input))
    {
        if(!((int)input[i] >=32 && (int)input[i] <= 126) )
        {
            return 0;
        }
        if (ret == 1)
        {
            for (j=0; j<sizeof(ssidinvaildchar);j++)
            {
                if (input[i] == ssidinvaildchar[j])
                    return 0;
            }
        }
        else
        {
            for (j=0; j<sizeof(pwinvaildchar);j++)
            {
                if (input[i] == pwinvaildchar[j])
                    return 0;
            }
        }
        i++;
    }
    return 1;
}

char* get_uci_iface_name (int x)
{
    const char default_role[] = "CAP";
    if (strncmp(g_role, default_role, strlen(default_role)) == 0) /* CAP */
    {
        if(x==0)
            return "0";
        else if(x==1)
            return "4";
        else if(x==2)
            return "1";
        else if(x==3)
            return "5";
    }
    else
    {/*   RE   */
        if(g_ethbackhaul[0]=='0')
        {
            if(x==0)
                return "0";
            else if(x==1)
                return "4";
            else if(x==2)
                return "1";
            else if(x==3)
                return "5";
        }
        else
        {
            if(x==0)
                return "0";
            else if(x==1)
                return "4";
            else if(x==2)
                return "1";
            else if(x==3)
                return "5";
        }
            
    }
}

char* get_iface_name(int x)
{
    const char default_role[] = "CAP";
    if (strncmp(g_role, default_role, strlen(default_role)) == 0) /* CAP */
    {
        if(x==0)
            return "0";
        else if(x==1)
            return "01";
        else if(x==2)
            return "1";
        else if(x==3)
            return "11";
    }
    else
    {
        if(g_ethbackhaul[0]=='0')
        {
            if(x==0)
                return "0";
            else if(x==1)
                return "02";
            else if(x==2)
                return "1";
            else if(x==3)
                return "12";
        }
        else
        {
            if(x==0)
                return "0";
            else if(x==1)
                return "01";
            else if(x==2)
                return "1";
            else if(x==3)
                return "11";
        }
    }
}

int init_App_Wifi_AP()
{
    sysinteract("" , g_role , sizeof(g_role), "uci get repacd.repacd.DeviceRole");
    sysinteract("" , g_ethbackhaul , sizeof(g_ethbackhaul) , "uci get repacd.repacd.IsEthBackhaul");
    return RESULT_OK;
}


int apply_App_Wifi_AP()
{
	system("uci commit wireless");
    char lock[]="0";
    sysinteract("",lock, 1 , "uci get system.lock 2>&1");
    
    if (!strcmp(lock , "1"))
    {
        system("uci set system.reload=1");
        return;
    }
    system("uci set system.lock=1");
    system("killall /etc/cgibox.sh");
    system("/etc/cgibox.sh &");

    return RESULT_OK;
}

int get_WiFi_AP_max()
{
    char default_role[]="CAP";
    if (strncmp( g_role, default_role , strlen(default_role)) ==0)
        return 4;
    else
        if(g_ethbackhaul[0] =='0')
            return 6;
        else
            return 4;
}
/* example */
int get_App_Wifi_AP(char *token_nm, char *get_str, int x, int y)
{
    return RESULT_OK;
}

int set_App_Wifi_AP(char *token_nm, char *get_str, int x, int y)
{
    return RESULT_OK;
}

/* WiFi_AP_Numbers */
int get_WIFI_AP_VAP_NUM(char *token_nm, char *get_str, int x, int y)
{
    char default_role[]="CAP";
    if (strncmp( g_role, default_role , strlen(default_role)) ==0)
        sprintf(get_str, "%d", 2);
    else
        sprintf(get_str, "%d", 3);

    return RESULT_OK;
}

/* WiFi_AP_x_Enable */
int get_WIFI_AP_VAP_STAT(char *token_nm, char *get_str, int x, int y)
{
    int rval =0;
    char command_str[162] = {0};
    char access_point[64] = {0};
    int result=0;
    const char default_status[] = "Not-Associated";
    
    sprintf(command_str, "iwconfig ath%s | grep \"Access Point\" | awk -F\" \" '{print $6}'", get_iface_name(x));
    rval = sysinteract("00:AA:BB:CC:DD:11", access_point, sizeof(access_point), command_str);
    if(rval >= 0 && strncmp(access_point, default_status, strlen(default_status)) != 0)
    {
        sprintf(get_str, "%s", "True");
    }
    else
    {
        sprintf(get_str, "%s", "False");
    }

    return RESULT_OK;
}

/* WiFi_AP_x_InternetAccessOnly */
int get_WIFI_AP_GUEST(char *token_nm, char *get_str, int x, int y)
{
#if 0  // THe guest network function not work.
    int rval =0;
    char command_str[162] = {0};
    char access_point[64] = {0};
    const char default_status[] = "Not-Associated";
    
    sprintf(command_str, "iwconfig ath%s | grep \"Access Point\" | awk -F\" \" '{print $6}'", get_iface_name(x));
    rval = sysinteract("00:AA:BB:CC:DD:11", access_point, sizeof(access_point), command_str);
    if(rval >= 0 && strncmp(access_point, default_status, strlen(default_status)) != 0)
        sprintf(get_str, "%s", "True");
    else
#endif
    sprintf(get_str, "%s", "False");

    return RESULT_OK;
}

/* WiFi_AP_x_SSID */
int get_WIFI_AP_VAP_SSID(char *token_nm, char *get_str, int x, int y)
{
    char *pos = NULL;
    char vap_ssid[64]= {0};

    sysinteract("", vap_ssid, 64, "uci get wireless.@wifi-iface[%s].ssid",get_uci_iface_name(x));
    
    if ((pos = strchr(vap_ssid, '\n')) != NULL)
        *pos = '\0';

    twbox_util_check_string_on_web (vap_ssid , get_str);
    //sprintf(get_str,"%s",vap_ssid);

    return RESULT_OK;
}

int set_WIFI_AP_VAP_SSID(char *token_nm, char *set_str, int x, int y)
{
    char vap_ssid[128]= {0};
    char result[128]={0};
    char default_role[]="CAP";
    
    if (strncmp( g_role, default_role , strlen(default_role)) ==0)  // Only Can change CAP
    {
        if(strlen(set_str) == 0 || strlen(set_str) > 32)
            return RESULT_ERROR;
        else if (!check_hex(set_str,1))
            return RESULT_ERROR;

        twbox_util_check_string_on_shell_cmd(set_str, vap_ssid);

    /* Make sure the consistency of WiFi configuration on both radios */
        if(x == 0) 
        {
            sysinteract("",result, 128 , "uci set wireless.@wifi-iface[%s].ssid=%s", get_uci_iface_name(x) , vap_ssid);
            sysinteract("",result, 128 , "uci set wireless.@wifi-iface[%s].ssid=%s", get_uci_iface_name(x+2) , vap_ssid);
        }
        return RESULT_TOKEN_CHANGE;
    }

    return RESULT_OK;
}

/* WiFi_AP_x_Password */
int get_WIFI_AP_VAP_PSK(char *token_nm, char *get_str, int x, int y)
{
    char *pos = NULL;
    char vap_key[64]= {0};

    sysinteract("", vap_key, 64, "uci get wireless.@wifi-iface[%s].key",get_uci_iface_name(x));
    
    if ((pos = strchr(vap_key, '\n')) != NULL)
        *pos = '\0';

    twbox_util_check_string_on_web (vap_key , get_str);
    //sprintf(get_str,"%s",vap_key);
    return RESULT_OK;

}

int set_WIFI_AP_VAP_PSK(char *token_nm, char *set_str, int x, int y)
{
    char default_role[]="CAP";
    char vap_key[256]= {0};
    char result[128]={0};

    if (strncmp( g_role, default_role , strlen(default_role)) ==0)  // Only Can change CAP
    {
        if(strlen(set_str) == 64 || !check_hex(set_str,0))
            return RESULT_ERROR;
        else if(strlen(set_str) < 8 || strlen(set_str) > 63)
            return RESULT_ERROR;

        twbox_util_check_string_on_shell_cmd(set_str, vap_key);

        /* Make sure the consistency of WiFi configuration on both radios */
        if(x == 0) {
            sysinteract("",result, 128 , "uci set wireless.@wifi-iface[%s].key=%s", get_uci_iface_name(x) , vap_key);
            sysinteract("",result, 128 , "uci set wireless.@wifi-iface[%s].key=%s", get_uci_iface_name(x+2) , vap_key);
        }
        return RESULT_TOKEN_CHANGE;
    }

    return RESULT_OK;

}



/*********************** END *********************************************/
