/*******************************************************************************
 *        Copyright (c) 2018 TWin Advanced Technology Corp., Ltd.
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
 * @file ril_crypto.h
 * @brief ...
 * @author T&W Technology co., Ltd.
 * @bug No known bugs
 */

#ifndef __TWBOX_CRYPTO_H__
#define __TWBOX_CRYPTO_H__

/*-------------------------------------------------------------------------*/
/*                        INCLUDE HEADER FILES                             */
/*-------------------------------------------------------------------------*/
#include <openssl/sha.h>
#include <ctype.h>



/*-------------------------------------------------------------------------*/
/*                           DEFINITIONS                                   */
/*-------------------------------------------------------------------------*/

#define BYTE unsigned char

/*-------------------------------------------------------------------------*/
/*                           VARIABLES                                     */
/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/*                           FUNCTIONS                                     */
/*-------------------------------------------------------------------------*/
void get_lsb_byte(char *input , char *output);
unsigned int hexstr2array (char *input, BYTE *output);
unsigned int ary2hexstring (BYTE *input, char *output, unsigned int bytelength);
void ary2sha256(char *string, char outputBuffer[65]);
void sha256(char *string, char outputBuffer[65]);
int ril_prepassword_encode(char mac_str[] , char sn[], char prepassword[]);
int ril_acs_encode(char *prepassword , char *secret , char *output);



#endif /* __TWBOX_IF_H__ */
