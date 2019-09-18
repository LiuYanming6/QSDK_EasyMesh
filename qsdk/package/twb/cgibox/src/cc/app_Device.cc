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
 * @file app_Device.cc
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


/*-------------------------------------------------------------------------*/
/*                           FUNCTIONS                                     */
/*-------------------------------------------------------------------------*/

int get_Device_ModelNumber(char *token_nm, char *get_str, int x, int y)
{ 
	sprintf(get_str, "%s", "JCM0112");
	
	return RESULT_OK;
}

int get_Device_NickName(char *token_nm, char *get_str, int x, int y)
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

int set_Device_NickName(char *token_nm, char *set_str, int x, int y)
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


int get_Device_SN(char *token_nm, char *get_str, int x, int y)
{
    char *pos = NULL;
    char SN[64]={0}; 
    sysinteract("", SN , 64 , "state_cfg get sn");
    if((pos = strchr(SN, '\n')) != NULL)
        *pos = '\0';

    sprintf(get_str, "%s", SN);

    return RESULT_OK;
}

int get_Device_HWVersion(char *token_nm, char *get_str, int x, int y)
{
    //cat /etc/device_info | grep "DEVICE_REVISION | cut -c 18-19"
    char *pos = NULL;
    char HWVersion[64]={0}; 
    sysinteract("", HWVersion , 64 , "state_cfg get hver");
    if ((pos = strchr(HWVersion, '\n')) != NULL)
        *pos = '\0';

    sprintf(get_str, "%s", HWVersion);

    return RESULT_OK;
}

int get_Device_FWVersion(char *token_nm, char *get_str, int x, int y)
{
    char *pos = NULL;
    char version[64]={0};
    sysinteract("", version , 64 , "cat /etc/openwrt_version");
    if ((pos = strchr(version, '\n')) != NULL)
        *pos = '\0';

    sprintf(get_str, "%s", version);

    return RESULT_OK;
}

/************************ END *********************************************/
