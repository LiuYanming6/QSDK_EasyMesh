/*******************************************************************************
 *        Copyright (c) 2017 TWin Advanced Technology Corp., Ltd.
 *        All rights reserved.
 *
 *       THIS WORK IS AN UNPUBLISHED WORK AND CONTAINS CONFIDENTIAL,
 *       PROPRIETARY AND TRADESECRET INFORMATION OF TwinA INCORPORATED.
 *       ACCESS TO THIS WORK IS RESTRICTED TO (I) TwinA EMPLOYEES WHO HAVE A
 *       NEED TO KNOW TO PERFORM TASKS WITHIN THE SCOPE OF THEIR ASSIGNMENTS
 *       AND (II) ENTITIES OTHER THAN TwinA WHO HAVE ENTERED INTO APPROPRIATE
 *       LICENSE AGREEMENTS.  NO PART OF THIS WORK MAY BE USED, PRACTICED,
 *       PERFORMED, COPIED, DISTRIBUTED, REVISED, MODIFIED, TRANSLATED,
 *       ABBRIDGED, CONDENSED, EXPANDED, COLLECTED, COMPILED, LINKED, RECAST,
 *       TRANSFORMED OR ADAPTED WITHOUT THE PRIOR WRITTEN CONSENT OF TwinA.
 *       ANY USE OR EXPLOITATION OF THIS WORK WITHOUT AUTHORIZATION COULD
 *       SUBJECT THE PERPERTRATOR TO CRIMINAL AND CIVIL LIABILITY.
 ******************************************************************************/
 /**
 * @file ril_crypto.c
 * @brief ...
 * @author TwinA Technology co., Ltd.
 * @bug No known bugs
 */

/*-------------------------------------------------------------------------*/
/*                        INCLUDE HEADER FILES                             */
/*-------------------------------------------------------------------------*/
#include <twbencode.h>
/*-------------------------------------------------------------------------*/
/*                           DEFINITIONS                                   */
/*-------------------------------------------------------------------------*/
#define MAX_MAC_ADDR_STR_SIZE       17

/*-------------------------------------------------------------------------*/
/*                           VARIABLES                                     */
/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/*                           FUNCTIONS                                     */
/*-------------------------------------------------------------------------*/

void get_lsb_byte(char *input , char *output)
{
    char *p = NULL;  
    int i = strlen(input)/2;

    while( i > 0 )
    {
        p = &input[(i*2) - 2];

        strncat(output, p, 2);
        i = i - 1;
    }
}

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

unsigned int ary2hexstring (BYTE *input, char *output, unsigned int bytelength)
{
    unsigned int i=0;
    char tmp[3];
    output[0] = '\0';
    static char hex_list[16] = {'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};

    for(i=0;i<bytelength;i++)
    {
        //sprintf (tmp, "%02x", input[i]); /* need to increase speed */
        unsigned char c = input[i];
        tmp[0] = hex_list[(c >>4)];
        tmp[1] = hex_list[(c & 0xf)];
        tmp[2] = '\0';

        strcat(output,tmp);
    }

    return 0;
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
/* **
*  @brief This function is for reliance pre-password encode
*  @param 
*  @return int

*/

void sha256(char *string, char outputBuffer[65])
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


int ril_prepassword_encode(char *mac_str , char *sn, char *prepassword)
{
    unsigned char array2hex[128] = {0};
    unsigned char hex2array[128] = {0};
    unsigned char output[128] = {0};
    unsigned char output_lsb[128] = {0};
    char *base64EncodeOutput = NULL;
    char *pos = NULL;
    char mac[18]= {0};
    int i = 0 ;

//    sscanf (mac_str , "%2s%2s%2s%2s%2s%2s", mac_1 , mac_2 , mac_3 , mac_4 , mac_5 , mac_6);
    pos = strtok(mac_str,":");
    while(pos != NULL)
    {
        sprintf(mac+i, pos);
        pos = strtok(NULL,":");
        i = i + 2 ;
    }
    ary2hexstring(sn, array2hex , strlen(sn));
    strncat(array2hex,mac+2,2);
    strncat(array2hex,mac+6,2);
    strncat(array2hex,mac+10,2);
    strncat(array2hex,mac,2);
    hexstr2array(array2hex,hex2array);
    sha256(hex2array , output );
    get_lsb_byte(output,output_lsb);
    memset(output, 0 , sizeof(output));
    snprintf(output,33,output_lsb);

    Base64Encode(output, 33, &base64EncodeOutput);
    memset(hex2array, 0 , sizeof(hex2array));
    strncpy(hex2array, base64EncodeOutput, 16);
    ary2hexstring(hex2array, prepassword , strlen(hex2array));


    return TRUE;
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
    char input[128] = {0};
    char *base64EncodeOutput = NULL;
  
    strcat(input , prepassword);
    strcat(input , secret);

    hexstr2array(input, output_hex2array);
    sha256(output_hex2array , output_sha256 );

    snprintf(output_first128,33,output_sha256);
    hexstr2array(output_first128, output_128hex2array);

    Base64Encode(output_128hex2array, 16, &base64EncodeOutput);
    strncpy(output, base64EncodeOutput, strlen(base64EncodeOutput));
    
    return TRUE;
}

/*
 * example :
 *  twbencode MAC SN Secret
*/
int main(int argc, char *argv[])
{
    char prepassword[128]={0};
    char result[128]={0};
    unsigned char secrethex[128] = {0};
    if(argc == 4)
    {
        //PASSWORD: twbencode [MAC Address] [SN] [secret]
        ril_prepassword_encode(argv[1] , argv[2] , prepassword);
        //printf("[DBG] ,prepassword = [%s]\n",prepassword);
        ary2hexstring(argv[3], secrethex, strlen(argv[3]));
        ril_acs_encode(prepassword , secrethex, result);
        printf("%s", result);
        return 0;
    }
    else if(argc == 3)
    {
        //prepassword: twbencode [MAC Address] [SN]
        if(strlen(argv[1])==12)
        {
            ril_prepassword_encode(argv[1] , argv[2] , prepassword);
            printf("%s", prepassword);
        }
        else
        {
           //PASSWORD: twbencode [prepassword] [secret]
            ary2hexstring(argv[2], secrethex, strlen(argv[2]));
            ril_acs_encode(argv[1] , secrethex, result);
            printf("%s", result);
        }
        return 0;
    }
    return 1;
}

/*****************************************************************************/
