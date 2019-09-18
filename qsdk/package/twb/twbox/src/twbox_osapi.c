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
 * @file ubox_socket.c
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

/**
*  @brief sysinteract
*  @param output output.
*  @param outputlen output of len.
*  @param fmt format
*  @return The results: cmd status
*/
int sysinteract(char *def, char *output, int outputlen, char *fmt, ...)
{
    static char buf[1024];
    FILE *pipe;
    int i;
    int p;
    va_list ap;

    va_start(ap, fmt);
    vsnprintf(buf, 1024, fmt, ap);
    va_end(ap);

    memset(output, 0, outputlen);

    if((pipe = popen(buf, "r")) == NULL)
    {
        goto error;
    }

    for(i = 0; ((p = fgetc(pipe)) != EOF) && (i < outputlen - 1); i++)
    {
        output[i] = (char) p;
    }
    output[i] = '\0';

    pclose(pipe);

    if(strlen(output) == 0)
    {
        goto error;
    }

    return 0;

error:
    if (def) {
        // NOTICE(SOMEONE): no expose the error cmd to users (debug usage only)
        printf("CMD[%s] has no response, setting to default (%s)\n", buf, def);
        strcpy(output, def);
    }

    return -1;
}


/** ***********************  END  ********************************************/
