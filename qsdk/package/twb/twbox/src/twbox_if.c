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
 * @file twbox_if.c
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
*  @brief This function check whether the "interface" is up
*  @param interface
*  @return T_BOOL
*/
bool ubox_if_is_up(const char *interface)
{
        struct ifreq    ifrq;
        int fd;

    if(interface == NULL)
    {
        printf("interface is NULL!\n");
        return FALSE;
    }
     /** create socket for ioctls */
    if((fd = socket(AF_INET, SOCK_DGRAM, 0)) >= 0)
    {
        memset(&ifrq, 0, sizeof(ifrq));
        strcpy(ifrq.ifr_name, interface);
        if(ioctl(fd, SIOCGIFFLAGS, &ifrq) == 0)
            close(fd);
        else
        {
            close(fd);
            return FALSE;
        }
    }
    else
        return FALSE;

    return (ifrq.ifr_flags & IFF_UP)? TRUE:FALSE;
}

/*****************************************************************************/
