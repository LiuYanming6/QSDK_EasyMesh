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
 * @file twbox_crypto.c
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

unsigned int hexstr2array (char *input, BYTE *output)
{
        unsigned int length = strlen(input);
        unsigned int i=0;
        char tmp[3];

        if((length % 2) != 0)
        {
                fprintf(stderr, "[ERROR] input length error:%u\n", length);
                return 0;
        }

        for(i=0;i<length;i++)
        {
                char c = input[i];
                if (c < '0' ||
                        ((c > '9') && (c < 'A')) ||
                        ((c > 'F') && (c < 'a')) ||
                        (c > 'f'))
                        {
                                fprintf(stderr, "[ERROR] input string error: %s\n", input);
                                return 0;
                        }
        }

        for(i=0;i<length;i+=2)
        {
                tmp[0] = input[i];
                tmp[1] = input[i+1];
                tmp[2] = '\0';
                output[i/2] = strtol (tmp, NULL, 16);
        }

        return length/2;
}

void ary2sha256(char *string, char outputBuffer[65])
{
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, string, strlen(string));
    SHA256_Final(hash, &sha256);
    int i = 0;
    for(i = 0; i < SHA256_DIGEST_LENGTH; i++)
    {
        sprintf(outputBuffer + (i * 2), "%02x", hash[i]);
    }
    outputBuffer[64] = 0;
}

/**
*  @brief This function is for reliance ACS Encode
*  @param 
*  @return int
*  @emample :
*  prepassword , secret , output
*  password = base64 ( first_128_bits_of ( sha256( concatenate( pre-password, secret ) ) )
*  input = concatenate( pre-password, secret ) 
*/

int ril_acs_encode(char *prepassword , char *secret , char *output)
{
    unsigned char output_hex2array[128] = {0};
    unsigned char output_sha256[128] = {0};
    unsigned char output_first128[128]={0};
    unsigned char output_128hex2array[128]={0};
    char *input = NULL;
    char *base64EncodeOutput = NULL;
    char *delim="0x";
    
    prepassword = strdup(prepassword);
    secret = strdup(secret);
    input = strtok(prepassword,delim);
    strcat(input , strtok(secret,delim));
    
    hexstr2array(input, output_hex2array);
    sha256(output_hex2array , output_sha256 );
    snprintf(output_first128,33,output_sha256);
    hexstr2array(output_first128, output_128hex2array);
    Base64Encode(output_128hex2array, 16, &base64EncodeOutput);
    strncpy(output, base64EncodeOutput, strlen(base64EncodeOutput));
    
    return TRUE;
}

/*****************************************************************************/
