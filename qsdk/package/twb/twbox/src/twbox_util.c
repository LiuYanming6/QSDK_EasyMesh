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
 * @file twbox_util.c
 * @brief ...
 * @author T&W Technology co., Ltd.
 * @bug No known bugs
 */

/*-------------------------------------------------------------------------*/
/*                        INCLUDE HEADER FILES                             */
/*-------------------------------------------------------------------------*/

#include <twbox.h>
/*-------------------------------------------------------------------------*/
/*                           DEFINITIONS                                   */
/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/*                           VARIABLES                                     */
/*-------------------------------------------------------------------------*/


/*-------------------------------------------------------------------------*/
/*                           FUNCTIONS                                     */
/*-------------------------------------------------------------------------*/
twbox_util_check_char(const char *srcstr, char *destStr)
{
    int len;
    char *pos = NULL;
    if(srcstr == NULL || (len = strlen(srcstr))==0)
    {
        destStr[0] = '\0';
        return -1;
    }

    int i,j;
    int cnt = 0;
    char specialchar[]={'\"','\\'};

    if ( (pos = strchr(srcstr,' ') )!= NULL)
        return -1;

    for(i=0; i<=len; i++)
    {
        if ( (int)srcstr[i] > 0 && ((int)srcstr[i] > 126 || (int)srcstr[i] < 32))
        {
            return -1;
        }

        for(j=0;j<sizeof(specialchar);j++)
        {
            if(srcstr[i] == specialchar[j])
            {
                destStr[cnt++] = '\\';
                break;
            }
        }
        destStr[cnt++] = srcstr[i];
    }
    return 0;

}


/**
*  @brief twbox_util_check_string
*  @param char *srcstr
*  @param char *dststr
*  @param int flag
*  @return void
*/
void twbox_util_check_string(const char *srcstr, char *destStr, int flag)
{
    int len;

    if(srcstr == NULL || (len = strlen(srcstr))==0)
    {
        destStr[0] = '\0';
        return;
    }

    int i,j;
    int cnt = 0;
    char specialchar[]={'\"', '#', '$', '&', '\'', '(', ')', '*', ';', '<', '>', '\\', '`', '|', '~', ' '};

    for(i=0; i<=len; i++)
    {
        if(flag)
        {
            for(j=0;j<sizeof(specialchar);j++)
            {
                if(srcstr[i] == specialchar[j])
                {
                    destStr[cnt++] = '\\';
                    break;
                }
            }
            destStr[cnt++] = srcstr[i];
        }
        else
        {
            if(srcstr[i] == '\"' || srcstr[i] == '\\')
            {
                destStr[cnt++] = '\\';
            }
            destStr[cnt++] = srcstr[i];
        }
    }
}
/*****************************************************************************/
/**
*  @brief twbox_util_check_string_on_web
*  @param char *src_str
*  @param char *dst_str
*  @return void
*/
void twbox_util_check_string_on_web(const char *srcstr, char *dststr)
{
    twbox_util_check_string(srcstr,dststr,0);
}
/*****************************************************************************/
/**
 *  @brief twbox_util_check_string_on_shell_cmd
 *  @param char *srcstr
 *  @param char *dststr
 *  @return void
 */
void twbox_util_check_string_on_shell_cmd(const char *srcstr, char *dststr)
{
    twbox_util_check_string(srcstr,dststr,1);
}

/*****************************************************************************/
/**
 *  @brief twbox_util_delete_space
 *  @param char *srcstr
 *  @return void
 */
char *twbox_util_delete_space(char *srcstr)
{
    if (srcstr == NULL || *srcstr == '\0')
    {
        return srcstr;
    }

    int len = 0;
    char *p = srcstr;
    
    while (*p != '\0' && isspace(*p))
    {
        ++p;
        ++len;
    }

    memmove(srcstr, p, strlen(srcstr) - len + 1);

    return srcstr;
}

/*****************************************************************************/
