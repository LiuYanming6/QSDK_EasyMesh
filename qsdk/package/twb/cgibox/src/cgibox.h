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
 * @file cgibox
 * @brief ...
 * @author T&W Technology co., Ltd.
 * @bug No known bugs
 */
#ifndef __CGIBOX_H__
#define __CGIBOX_H__

/*-------------------------------------------------------------------------*/
/*                        INCLUDE HEADER FILES                             */
/*-------------------------------------------------------------------------*/
#define MAX_BUF_SIZE    102400
#define USER_SESSION_ID_LENGTH 10

#if 1
#define LOGFILE     "/dev/console"
#define DBG_MSG(fmt, arg...) do { FILE *log_fp = fopen(LOGFILE, "w"); \
                                     fprintf(log_fp, "%s:%s:%d:" fmt "\n", __FILE__, __func__, __LINE__, ##arg); \
                                     fclose(log_fp); \
                                     } while(0)
#else
#define DBG_MSG(...)
#endif

/*-------------------------------------------------------------------------*/
/*                           DEFINITIONS                                   */
/*-------------------------------------------------------------------------*/

#define RESULT_STATUS_MASK      0x0000FFFF
typedef enum {
    // Action Status
    RESULT_OK                               = 0x00000001,
    RESULT_ERROR                            = 0x00000002,
    RESULT_REBOOT                           = 0x00000004,
    RESULT_BUSY                             = 0x00000008,
    RESULT_SUCCESS                          = 0x00000010,
	RESULT_AUTH_SUCCESS                     = 0x00000020,
	RESULT_AUTH_FAIL                        = 0x00000040,
    RESULT_RESTART                          = 0x00000080,
    // Following Action(s)
    RESULT_TOKEN_CHANGE                     = 0x00010000,    // Token Changed, sync with the sysifd and print the result
    RESULT_TOKEN_RESTORE                    = 0x00020000,    // Token Restored, sync with the sysifd and print the result
    RESULT_TOKEN_REBOOT                     = 0x00040000,    // Reboot
    RESULT_TOKEN_2_DEF                      = 0x00080000,    // Token Changed to factory default
    RESULT_FW_UPG_URL                       = 0x00100000,    // FW Upgrade by URL
    RESULT_FW_UPG_UPLOAD                    = 0x00200000,    // FW Upgrade by Upload
    RESULT_SYSTIME_SET                      = 0x00400000,    // Set System Time
    RESULT_TOKEN_MUITIACTION_FOR_APP        = 0x00800000,    // For Set Multiple Action in APP // TODO(SOMEONE): workaround for set muitiple action
    RESULT_TOKEN_MULTIPLE_FOR_APP_RESTART   = 0x01000000,    // For Set Multiple Action in APP // TODO(SOMEONE): workaround for set muitiple action
    RESULT_FW_UPG_URL_DL                    = 0x02000000,    // FW Upgrade by URL (start download fw)
    RESULT_FW_UPG_URL_ERROR                 = 0x04000000,    // RE FW Upgrade by URL error
    RESULT_TBD                              = 0x80000000,

    RESULT_ERROR_NODE_MISSING               = 0x10000000,
    RESULT_UNKNOWN                          = 0
} TYPE_RESULT;


#endif /* __CGIBOX_H__ */
