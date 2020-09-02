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
 * @file fwhandle.c
 * @brief
 * @author T&W Technology co., Ltd.
 * @bug No known bugs
 */

/*----------------------------------------------------------------------------*/
/*      INCLUDE HEADER FILES                                                  */
/*----------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "fwhandle.h"
#include "crc32.h"

/*----------------------------------------------------------------------------*/
/*      DEFINITIONS                                                           */
/*----------------------------------------------------------------------------*/
#define BE_TO_LE(x) (is_big_endian ? __bswap_32(x) : x) // i386 is little endian
#define LOGFILE     "/dev/console"
#define DBG_MSG(fmt, arg...) do { FILE *log_fp = fopen(LOGFILE, "w"); \
                                     fprintf(log_fp, fmt, ##arg); \
                                     fclose(log_fp); \
                                } while(0)

/*-------------------------------------------------------------------------*/
/*                           VARIABLES                                     */
/*-------------------------------------------------------------------------*/

static unsigned int encode_buf_size = 0;
static unsigned int decode_buf_size = 0;
/*-------------------------------------------------------------------------*/
/*                           FUNCTIONS                                     */
/*-------------------------------------------------------------------------*/

int main(int argc, char **argv)
{
    unsigned int pos;
    char *src_file = NULL;
    char *dest_file = NULL;
    unsigned char *buf = 0;
    fw_header_t header_empty = {0};
    fw_header_t *header = &header_empty;
    int y = 1;
    char *x = (char *) &y;
    int is_big_endian = (*x) ? 0 : 1;
    FILE *pFile;
    unsigned int   src_size = 0;
    unsigned int   buf_size = 0;
    int ret = -1;
    char *decode_src_file = NULL;

    while((pos = getopt(argc, argv, ":c:f:x:"))!= EOF)
    {
        switch(pos)
        {
            case 'c':
                src_file = optarg;
                break;
            case 'f':
                dest_file = optarg;
                break;
            case 'x':
                decode_src_file = optarg;
        }
    }

    if (src_file != NULL && dest_file != NULL)
    {
        pFile = fopen(src_file, "rb");
        if(pFile == NULL)
            goto ENCODE_END;

        fseek(pFile, 0L, SEEK_END);
        src_size = (unsigned int)ftell(pFile);
    
    buf_size = sizeof(fw_header_t) + src_size;
    buf = (unsigned char*)malloc(sizeof(unsigned char) * buf_size);
    fseek(pFile, 0L, SEEK_SET);
    fread(buf + sizeof(fw_header_t), 1, src_size, pFile);
    fclose(pFile);

    if (buf == NULL || buf == 0)
        goto ENCODE_END;

    header = (fw_header_t*)buf;
    header->twb_magic = TWB_MAGIC;
    
    if (src_size != 0)
        header->data_size = (uint32_t)src_size;

    header->data_crc = (uint32_t)crc32(0, buf + sizeof(fw_header_t), header->data_size);
    pFile = fopen(dest_file, "wb");
    if(pFile == NULL)
        goto ENCODE_END;

    fwrite(buf , 1, buf_size, pFile);
    fclose(pFile);
    ret = 0;

/*
    ------- RIL-AC1200 Info --------
    ------- Header   Info ----------
    Image Header Size      : 0x000c
    Image Header Magic Code: 0x031d6129
    Image Data Size        : 13369360
    Data CRC Checksum      : 0xa952bf30
    ------------------------------
    FWHANDLE, ENCODE PASS

 * */

ENCODE_END:
        printf("------- RIL-AC1200 Info --------\n");
        printf("------- Header   Info ----------\n");
        printf("Image Header Size      : 0x%04x\n", (unsigned int) (sizeof(fw_header_t)));
        printf("Image Header Magic Code: 0x%08x\n", BE_TO_LE(header->twb_magic));
        printf("Image Data Size        : %d\n", BE_TO_LE(buf_size));
        printf("Data CRC Checksum      : 0x%08x\n", BE_TO_LE(header->data_crc));
        printf("------------------------------\n");
        printf("FWHANDLE, ENCODE %s\n", ret == 0 ? "PASS" : "FAIL");
        goto END;

    }
    if (decode_src_file != NULL)
    {
        unsigned int decode_src_size = 0;
        unsigned int header_crc_decode = 0;
        char decode_dest_file[256] = {0};

        pFile = fopen(decode_src_file, "rb");
        if(pFile == NULL)
            goto DECODE_END;

        fseek(pFile, 0L, SEEK_END);
        decode_src_size = (unsigned int)ftell(pFile);
        buf = (unsigned char*)malloc(sizeof(unsigned char) * decode_src_size);
        if (buf == NULL || buf == 0)
        {
            fclose(pFile);
            goto DECODE_END;
        }
        memset(buf, 0, sizeof(unsigned char) * decode_src_size);
        header = (fw_header_t*)buf;
        
        fseek(pFile, 0L, SEEK_SET);
        fread(buf, 1, decode_src_size, pFile);
        fclose(pFile);

        if (header->twb_magic == IH_MAGIC)
        {
            ret=2;
            goto DECODE_END;
        }
        else if (header->twb_magic == RIL_MAGIC)
        {
            ret=3;
            goto DECODE_END;
        }
        if (BE_TO_LE(header->twb_magic) != TWB_MAGIC)
            goto DECODE_END;
    

        //DBG_MSG("------- TWB_MAGIC_CORRECT ------");
        header_crc_decode = (uint32_t)crc32(0, buf + sizeof(fw_header_t), BE_TO_LE(header->data_size));

        if (BE_TO_LE(header->data_crc) != header_crc_decode )
            goto DECODE_END;
        
        //DBG_MSG("------- TWB_DATA_CRC_CORRECT ------");
        //snprintf(decode_dest_file, sizeof(decode_dest_file), "%s.bin", decode_src_file);
        pFile = fopen(decode_src_file, "wb");
        if(pFile == NULL)
            goto DECODE_END;
        //remove(decode_src_file);
        fwrite(buf + sizeof(fw_header_t), 1, BE_TO_LE(header->data_size), pFile);
        fclose(pFile);
        ret = 0;

DECODE_END:
/*
        ------- RIL-AC1200 Info ------
        ------- Header Info ----------
        Image Header Size      : 0x000c
        Image Header Magic Code: 0x031d6129
        Image Data Size        : 13369360
        Original Data Size     : 13369348
        Data CRC Checksum      : 0x4432c5a2
        ------------------------------
        FWHANDLE, DECODE PASS

*/

        DBG_MSG("------- RIL-AC1200 Info ------\n");
        DBG_MSG("------- Header Info ----------\n");
        DBG_MSG("Image Header Size      : 0x%04x\n", (unsigned int)(sizeof(fw_header_t)));
        DBG_MSG("Image Header Magic Code: 0x%08x\n", BE_TO_LE(header->twb_magic));
        DBG_MSG("Image Data Size        : %d\n", decode_src_size);
        DBG_MSG("Original Data Size     : %d\n", BE_TO_LE(header->data_size));
        DBG_MSG("Data CRC Checksum      : 0x%08x\n", BE_TO_LE(header->data_crc));
        DBG_MSG("------------------------------\n");

        if (ret ==2)
        {
            DBG_MSG("FWHANDLE, DECODE %s\n", "PASSWITHNOENCODE");
            printf("FWHANDLE, DECODE %s\n", "PASSWITHNOENCODE");
        }
        else if (ret ==3)
        {
            DBG_MSG("FWHANDLE, DECODE %s\n", "RILPASSWITHNOENCODE");
            printf("FWHANDLE, DECODE %s\n", "RILPASSWITHNOENCODE");
        }
        else
        {
            DBG_MSG("FWHANDLE, DECODE %s\n", ret == 0 ? "PASS" : "FAIL");
            printf("FWHANDLE, DECODE %s\n", ret == 0 ? "PASS" : "FAIL");
            system("killall -SIGUSR1 cwmpc");
        }
        goto END; 
    }
    

END:
    if (buf != NULL && buf != 0)
        free(buf);
    return ret; 

}
