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
 * @file app_Client.cc
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

#define MAC_FILE    "/tmp/mac_client"
#define SCAN_FILE   "/tmp/mac_scan"

/*-------------------------------------------------------------------------*/
/*                           VARIABLES                                     */
/*-------------------------------------------------------------------------*/

static int client_wired = 0;
static int client_2g =0;
static int client_5g =0;
/*-------------------------------------------------------------------------*/
/*                           FUNCTIONS                                     */
/*-------------------------------------------------------------------------*/

struct clients
{
    char mac[18];
    char tx[5];
    char rx[5];
    char rssi[4];
};

struct clients clients_2g[256];
struct clients clients_5g[256];
struct clients clients_wired[256];

int init_Client()
{
    client_5g = 0;
    client_wired = 0;
    client_2g = 0;
    char buf[2048] = {0};
    char buf2[3]={0};
    char line_buf[256];
    int i = 0;
    int j = 0;
    int ret;
    char *pos = NULL;

    memset(&clients_2g , 0x0 , sizeof(clients_2g));
    memset(&clients_5g , 0x0 , sizeof(clients_5g));
    memset(&clients_wired , 0x0 , sizeof(clients_wired));

/*  For Client mac , tx , rx ,r ssi */
    sysinteract("" , buf , sizeof(buf) , "rm -rf %s && wlanconfig ath0 list | awk '!(NR==1){print $1 \" \" $4 \" \" $5 \" \" $6}'  > %s" , MAC_FILE, MAC_FILE );
    FILE *fp = NULL;
    fp = fopen(MAC_FILE, "r");
    if(fp == NULL)
    {
        DBG_MSG("Cannot read file, %s\n", MAC_FILE );
        return RESULT_ERROR;
    }
    while (fgets(line_buf, sizeof(line_buf), fp))
    {
        sscanf( line_buf , "%s %s %s %[^\n]" , clients_2g[i].mac , clients_2g[i].tx , clients_2g[i].rx , clients_2g[i].rssi );
        for(j = 0 ; j < strlen(clients_2g[i].mac)+1 ; j++ )
        {
            buf[j] = toupper(clients_2g[i].mac[j]);
        }
        snprintf(clients_2g[i].mac , strlen(clients_2g[i].mac)+1 , buf);
        //DBG_MSG ("%s %s %s %s", clients_2g[i].mac , clients_2g[i].tx , clients_2g[i].rx , clients_2g[i].rssi );
        i++;
    }
    fclose(fp);
    client_2g = i;
    i =0;


    sysinteract("" , buf , sizeof(buf) , "rm -rf %s && wlanconfig ath1 list | awk '!(NR==1){print $1 \" \" $4 \" \" $5 \" \" $6}'  > %s" , MAC_FILE, MAC_FILE );
    fp = fopen(MAC_FILE, "r");
    if(fp == NULL)
    {
        DBG_MSG("Cannot read file, %s\n", MAC_FILE );
        return RESULT_ERROR;
    }

    while (fgets(line_buf, sizeof(line_buf), fp))
    {
        sscanf( line_buf , "%s %s %s %[^\n]" , clients_5g[i].mac , clients_5g[i].tx , clients_5g[i].rx , clients_5g[i].rssi );
        DBG_MSG ("%s", clients_5g[i].mac  );
        for(j = 0 ; j < strlen(clients_5g[i].mac)+1 ; j++ )
        {
            buf[j] = toupper(clients_5g[i].mac[j]);
        }
        snprintf(clients_5g[i].mac , strlen(clients_5g[i].mac)+1 , buf);
        //DBG_MSG ("%s %s %s %s", clients_5g[i].mac , clients_5g[i].tx , clients_5g[i].rx , clients_5g[i].rssi );
        i++;
    }
    fclose(fp);
    client_5g = i;

/*  For Client ip */


/*  For RE wired client */
    cmd_popen("uci get repacd.repacd.DeviceRole", buf);
    cmd_popen("uci get repacd.repacd.IsEthBackhaul", buf2);
    if( strncmp(buf , "RE" , 2) == 0 && strncmp(buf2 , "0" , 1) == 0)
    {
        memset( buf , 0x0 , sizeof(buf));
	sysinteract("" , buf , sizeof(buf) , "rm -rf %s && arp-scan -l -I br-lan > %s" , SCAN_FILE, SCAN_FILE );
        
	memset( buf , 0x0 , sizeof(buf));
	if ((ret = ubox_get_qca_wifison_re_wired_client ( buf , 1 )) !=0)
            return RESULT_ERROR;
        
        i=0;
        pos = strtok(buf,"\n");
        while (pos != NULL)
        {
            sprintf(clients_wired[i].mac, pos);
            pos = strtok(NULL,"\n");
            i++;
        }
        client_wired = i;
    }

    //sysinteract("" , tmp , sizeof(tmp) , "wc -l %s | awk '{print $1}'" , MAC_FILE);
    return RESULT_OK;
}

int get_Client_MAC_max()
{ 
    return client_2g + client_5g + client_wired;
}

int get_Client_Numbers(char *token_nm, char *get_str, int x, int y)
{ 
    sprintf(get_str, "%d", client_2g + client_5g + client_wired );
    return RESULT_OK;
}

int get_Client_MAC(char *token_nm, char *get_str, int x, int y)
{
    //char client_mac[18] ={0}; 
    //sysinteract("" , client_mac , sizeof(client_mac) , "cat %s | awk '(NR==%d){print $1}'" , MAC_FILE , x+1 );
    if (client_2g-1 >= x)   /* 2g clients */
    {
        if (NULL != clients_2g[x].mac)
            sprintf(get_str, "%s", clients_2g[x].mac);
        else
            return RESULT_ERROR;
    }
    else if ( client_2g + client_5g -1 >=x)  /* 5g clients */
    {
        if (NULL != clients_5g[ x - client_2g].mac)
            sprintf(get_str, "%s", clients_5g[ x - client_2g ].mac);
        else
            return RESULT_ERROR;
    }
    else
    {
        if (NULL != clients_wired[ x - client_5g - client_2g].mac)
            sprintf(get_str, "%s", clients_wired[ x - client_5g - client_2g ].mac);
        else
            return RESULT_ERROR;
    }
    return RESULT_OK;
}

int get_Client_SignalStrength(char *token_nm, char *get_str, int x, int y)
{
    if (client_2g-1 >= x)   /* 2g clients */
    {
        if (NULL != clients_2g[x].rssi)
            sprintf(get_str, "%s", clients_2g[x].rssi);
        else
            return RESULT_ERROR;
    }
    else if ( client_2g + client_5g -1 >=x) /* 5g clients */
    {
        if (NULL != clients_5g[ x - client_2g].rssi)
            sprintf(get_str, "%s", clients_5g[ x - client_2g ].rssi);
        else
            return RESULT_ERROR;
    }
    else
    {
        if (NULL != clients_wired[ x - client_5g - client_2g].rssi)
            sprintf(get_str, "%s", clients_wired[ x - client_5g - client_2g ].rssi);
        else
            return RESULT_ERROR;
    }
    return RESULT_OK;
}

int get_Client_Interface(char *token_nm, char *get_str, int x, int y)
{
    if (client_2g-1 >= x)   /* 2g clients */
    {
        if (NULL != clients_2g[x].mac)
            sprintf(get_str, "%s", "WIFI_2.4G");
        else
            return RESULT_ERROR;
    }
    else if ( client_2g + client_5g -1 >=x)   /* 5g clients */
    {
        if (NULL != clients_5g[ x - client_2g].mac)
            sprintf(get_str, "%s", "WIFI_5G");
        else
            return RESULT_ERROR;
    }
    else
    {
        if (NULL != clients_wired[ x - client_5g - client_2g].mac)
            sprintf(get_str, "%s", "LAN");
        else
            return RESULT_ERROR;
    }

    return RESULT_OK;
}

int get_Client_ConnectionType(char *token_nm, char *get_str, int x, int y)
{
    if (client_2g-1 >= x)   /* 2g clients */
    {
        if (NULL != clients_2g[x].mac)
            sprintf(get_str, "%s", "wireless");
        else
            return RESULT_ERROR;
    }
    else if ( client_2g + client_5g -1 >=x)   /* 5g clients */
    {
        if (NULL != clients_5g[ x - client_2g].mac)
            sprintf(get_str, "%s", "wireless");
        else
            return RESULT_ERROR;
    }
    else
    {
        if (NULL != clients_wired[ x - client_5g - client_2g].mac)
            sprintf(get_str, "%s", "wired");
        else
            return RESULT_ERROR;
    }
    return RESULT_OK;
}

int get_Client_IPv4Address(char *token_nm, char *get_str, int x, int y)
{
    char client_ip[18] ={0};
    char *pos = NULL;
    if (client_2g-1 >= x)   /* 2g clients */
    {
        if (NULL != clients_2g[x].mac)
        {
            sysinteract("" , client_ip , sizeof(client_ip) , "cat %s | grep -ir '%s' | awk '{print $1}'" , SCAN_FILE ,  clients_2g[x].mac );
            if ((pos = strchr(client_ip, '\n')) != NULL)
                *pos = '\0';

            sprintf(get_str, "%s", client_ip);
        }
        else
            return RESULT_ERROR;
    }
    else if ( client_2g + client_5g -1 >=x)    /* 5g clients */
    {
        if (NULL != clients_5g[ x - client_2g].mac)
        {
            sysinteract("" , client_ip , sizeof(client_ip) , "cat %s | grep -ir '%s' | awk '{print $1}'" , SCAN_FILE ,  clients_5g[ x - client_2g ].mac );
            if ((pos = strchr(client_ip, '\n')) != NULL)
                *pos = '\0';

            sprintf(get_str, "%s", client_ip);
        }
        else
            return RESULT_ERROR;
    }
    else
    {
        if (NULL != clients_wired[ x - client_5g - client_2g].mac)
        {
            sysinteract("" , client_ip , sizeof(client_ip) , "cat %s | grep -ir '%s' | awk '{print $1}'" , SCAN_FILE ,  clients_wired[ x - client_5g - client_2g ].mac );
            if ((pos = strchr(client_ip, '\n')) != NULL)
                *pos = '\0';

            sprintf(get_str, "%s", client_ip);
        }
        else
            return RESULT_ERROR;
    }
    return RESULT_OK;
}

int get_Client_IPv6Address(char *token_nm, char *get_str, int x, int y)
{
    char client_ip[128] ={0};
    char *pos = NULL;
    if (client_2g-1 >= x)   /* 2g clients */
    {
        if (NULL != clients_2g[x].mac)
        {
            sysinteract("" , client_ip , sizeof(client_ip) , "ip -6 neigh show | grep -ir '%s' | awk '{print $1}'" , clients_2g[x].mac );
            if ((pos = strchr(client_ip, '\n')) != NULL)
                *pos = '\0';

            sprintf(get_str, "%s", client_ip);
        }
        else
            return RESULT_ERROR;
    }
    else if  ( client_2g + client_5g -1 >=x)    /* 5g clients */
    {
        if (NULL != clients_5g[ x - client_2g].mac)
        {
            sysinteract("" , client_ip , sizeof(client_ip) , "ip -6 neigh show | grep -ir '%s' | awk '{print $1}'" , clients_5g[ x - client_2g ].mac );
            if ((pos = strchr(client_ip, '\n')) != NULL)
                *pos = '\0';

            sprintf(get_str, "%s", client_ip);
        }
        else
            return RESULT_ERROR;
    }
    else
    {
        if (NULL != clients_wired[ x - client_5g - client_2g].mac)
        {
            sysinteract("" , client_ip , sizeof(client_ip) , "ip -6 neigh show | grep -ir '%s' | awk '{print $1}'" , clients_wired[ x - client_5g - client_2g ].mac );
            if ((pos = strchr(client_ip, '\n')) != NULL)
                *pos = '\0';

            sprintf(get_str, "%s", client_ip);
        }
        else
            return RESULT_ERROR;
    }
    return RESULT_OK;
}

int get_Client_LinkRateTx(char *token_nm, char *get_str, int x, int y)
{
    if (client_2g-1 >= x)   /* 2g clients */
    {
        if (NULL != clients_2g[x].tx)
            sprintf(get_str, "%s", clients_2g[x].tx);
        else
            return RESULT_ERROR;
    }
    else if ( client_2g + client_5g -1 >=x)  /* 5g clients */
    {
        if (NULL != clients_5g[ x - client_2g].tx)
            sprintf(get_str, "%s", clients_5g[ x - client_2g ].tx);
        else
            return RESULT_ERROR;
    }
    else
    {
        if (NULL != clients_wired[ x - client_5g - client_2g].tx)
            sprintf(get_str, "%s", clients_wired[ x - client_5g - client_2g ].tx);
        else
            return RESULT_ERROR;
    }
    return RESULT_OK;
}

int get_Client_LinkRateRx(char *token_nm, char *get_str, int x, int y)
{
    if (client_2g-1 >= x)   /* 2g clients */
    {
        if (NULL != clients_2g[x].rx)
            sprintf(get_str, "%s", clients_2g[x].rx);
        else
            return RESULT_ERROR;
    }
    else if ( client_2g + client_5g -1 >=x)  /* 5g clients */
    {
        if (NULL != clients_5g[ x - client_2g].rx)
            sprintf(get_str, "%s", clients_5g[ x - client_2g ].rx);
        else
            return RESULT_ERROR;
    }
    else
    {
        if (NULL != clients_wired[ x - client_5g - client_2g].rx)
            sprintf(get_str, "%s", clients_wired[ x - client_5g - client_2g ].rx);
        else
            return RESULT_ERROR;
    }
    return RESULT_OK;
}


/************************ END *********************************************/
