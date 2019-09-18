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
 * @file app_Login.cc
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

int get_Login(char *token_nm, char *get_str, int x, int y)
{ 
    sprintf(get_str, ",,%s", "");

    return RESULT_OK;
}

int set_Login(char *token_nm, char *set_str, int x, int y)
{

    int ret = RESULT_AUTH_FAIL;
    char salt[128] = {0};
    char encoding_pwd[260] = {0};
    char cmd_result[1024]  = {0};
    char cmd[1024]  = {0};
    char pwd[260] ={0};
    char *p = NULL, *s = NULL;
    
    p = strchr(set_str, ',');
    if (p == NULL)
        goto done;

    *p = '\0';


    DBG_MSG("app login = %s, default username = %s", set_str, "admin");	
    if (strcmp("admin", set_str) != 0)
        goto done;

    p++;
    s = strchr(p, ',');
    if (s == NULL)
        goto done;

    *s = '\0';

    sprintf(cmd, "grep -rw '/etc/shadow' -e '%s::0'", set_str);
    cmd_popen(cmd, cmd_result);

    if (*p == '\0' && strcmp(cmd_result, "") != 0)
    {
        //Pass
    } 
    else
    {
        //DBG_MSG("app password = %s, pssword = %s", p, tok_get_str_value(TOK_HTTP_PASSWD));
        memset(cmd, 0x0, sizeof(cmd));
        memset(cmd_result, 0x0, sizeof(cmd_result));
        sprintf(cmd, "grep -rw '/etc/shadow' -e '%s' | cut -d '$' -f3", set_str);
        cmd_popen(cmd, salt);
        DBG_MSG("cmd = %s, salt = %s", cmd, salt);
        if (strcmp(salt, "") == 0)
            goto done;

        
        memset(cmd, 0x0, sizeof(cmd));
        memset(cmd_result, 0x0, sizeof(cmd_result));
        memset(pwd ,0x0, sizeof(pwd));

        twbox_util_check_string_on_shell_cmd(p, pwd);
        //echo "password" | openssl passwd -1 -salt 3EZtCdZc -stdin	
        sprintf(cmd, "echo \"%s\" | openssl passwd -1 -salt %s -stdin", pwd, salt);
        cmd_popen(cmd, encoding_pwd);
        DBG_MSG("cmd = %s, encoding_pwd = %s|||", cmd, encoding_pwd);
        encoding_pwd[strlen(encoding_pwd)-1] = '\0';
        
        
        memset(cmd, 0x0, sizeof(cmd));
        memset(cmd_result, 0x0, sizeof(cmd_result));
        sprintf(cmd, "grep -rw '/etc/shadow' -e '%s:%s'", set_str, encoding_pwd);
        cmd_popen(cmd, cmd_result);
        DBG_MSG("cmd = %s, cmd_result = %s", cmd, cmd_result);
        if (strcmp(cmd_result, "") == 0)
            goto done;
    }

    ret = RESULT_OK | RESULT_AUTH_SUCCESS;

done:
//DBG_MSG("Done");
    return ret;
}

/************************ END *********************************************/
