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
 * @file app_Admin.cc
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

int get_Admin_Name(char *token_nm, char *get_str, int x, int y)
{ 
    sprintf(get_str, "%s", "admin");

    return RESULT_OK;
}

int set_Admin_Name(char *token_nm, char *set_str, int x, int y)
{ 
    //sprintf(get_str, "%s", CFG_PRODUCT_NAME);

    return RESULT_OK;
}

int get_Admin_Password(char *token_nm, char *get_str, int x, int y)
{ 
    
    sprintf(get_str, "%s", "");

    return RESULT_OK;
}

int set_Admin_Password(char *token_nm, char *set_str, int x, int y)
{
    if(strlen(set_str) > 128 )
        return RESULT_ERROR;

    char salt[9] = {0};
    char pwd[260]={0};
    char encoding_pwd[260] = {0}, new_encoding_pwd[260] = {0};
    char cmd_result[1024]  = {0};
    char cmd[1024]  = {0};

    /*
    memset(cmd, 0x0, sizeof(cmd));
    memset(cmd_result, 0x0, sizeof(cmd_result));	
    sprintf(cmd, "grep -rw '/etc/shadow' -e 'root' | cut -d ':' -f2");
    cmd_popen(cmd, encoding_pwd);
    encoding_pwd[strlen(encoding_pwd)-1] = '\0';

    if (strcmp(encoding_pwd, "") == 0)
        return RESULT_ERROR;

    DBG_MSG("encoding_pwd = %s", encoding_pwd);	
*/
    memset(cmd, 0x0, sizeof(cmd));
    memset(cmd_result, 0x0, sizeof(cmd_result));
    sprintf(cmd, "sed -i '/admin:/d' /etc/shadow");
    cmd_popen(cmd, cmd_result);

    if(*set_str=='\0')
    {
        memset(cmd, 0x0, sizeof(cmd));
        memset(cmd_result, 0x0, sizeof(cmd_result));
        sprintf(cmd, "echo 'admin::0:0:99999:7:::' >> /etc/shadow");
        cmd_popen(cmd, cmd_result);
    }
    else
    {
        genRandomID(salt, 8);
        DBG_MSG("salt = %s", salt);
        
        memset(cmd, 0x0, sizeof(cmd));
        memset(cmd_result, 0x0, sizeof(cmd_result));
        memset(pwd, 0x0, sizeof(pwd));

        twbox_util_check_string_on_shell_cmd(set_str, pwd);
        sprintf(cmd, "echo \"%s\" | openssl passwd -1 -salt %s -stdin", pwd, salt);
        cmd_popen(cmd, new_encoding_pwd);
        new_encoding_pwd[strlen(new_encoding_pwd)-1] = '\0';
        DBG_MSG("new_encoding_pwd = %s", new_encoding_pwd);	

        memset(cmd, 0x0, sizeof(cmd));
        memset(cmd_result, 0x0, sizeof(cmd_result));
        sprintf(cmd, "echo 'admin:%s:17730:0:99999:7:::' >> /etc/shadow", new_encoding_pwd);	
        cmd_popen(cmd, cmd_result);
    }
    DBG_MSG("cmd = %s", cmd);
    return RESULT_OK;
}

/************************ END *********************************************/
