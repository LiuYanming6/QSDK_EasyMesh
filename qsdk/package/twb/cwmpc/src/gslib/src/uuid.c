/*
 * uuid.c
 *
 *  Created on: Jan 3, 2012
 *      Author: dmounday
 *  These functions are translated from the Apache Java implementation of UUID.
 *  Only the ones needed for CWMPc have been translated.
 */
#include <stdint.h>
#include <openssl/sha.h>
#include "../gslib/src/utils.h"
#define UUID_SZ 20
#define UUID_BYTE_LENGTH 16
#define TIME_HI_AND_VERSION_BYTE_6 6
#define CLOCK_SEQ_HI_AND_RESERVED_BYTE_8 8

#define NAME_MAX_SZ	64+128+2+1
//typedef unsigned char UUID;

// From TR-069 Amendment 3, Annex H.2:
//#define NAMESPACE "6ba7b810-9dad-11d1-80b4-00c04fd430c8"
static uint8_t nameSpace[UUID_BYTE_LTH+1] = {
		0x6b, 0xa7, 0xb8, 0x10,
		0x9d, 0xad,
		0x11, 0xd1,
		0x80, 0xb4,
		0x00, 0xc0, 0x4f, 0xd4, 0x30, 0xc8
		};

/*
 * encoding is always SHA1_ENCODING for SMM UUID usage.
 * param name is the DeploymentUnit.{i}.Name + DeploymentUnit.{i}.Vendor
 * Such as "sample1.gatespace.com."
 */
const char *generateUUID( const char *name ){

	uint8_t cName[NAME_MAX_SZ];
	uint8_t msgDigest[20];
	int	lth;
    static char digBuf[UUID_BYTE_LTH*2+4];
    uint8_t raw[UUID_BYTE_LTH];
    int hmacSize;

    memcpy(cName, nameSpace, UUID_BYTE_LENGTH+1);
    lth = strlen(name);
    memcpy(&cName[UUID_BYTE_LTH], name, cNameLth );
    lth = lth + UUID_BYTE_LTH;
    SHA1( cName, lth, msgDigest );
    memcpy(raw, msgDigest, UUID_BYTE_LTH);
    raw[TIME_HI_AND_VERSION_BYTE_6]&= 0x0F;
    raw[TIME_HI_AND_VERSION_BYTE_6]|= (3<<4); //version 3
    raw[CLOCK_SEQ_HI_AND_RESERVED_BYTE_8] &= 0x3f;
    raw[CLOCK_SEQ_HI_AND_RESERVED_BYTE_8]|= 0x80;
	toAsciiHex(msgDigest, 4, digBuf);
	digBuf[8]='-';
	toAsciiHex(msgDigest+4, 2, digBuf+9);
	digBuf[13]='-';
	toAsciiHex(msgDigest+14, 2, digBuf+14);
	digBuf[18]='-';
	toAsciiHex(msgDigest+16, 2, digBuf+19);
	digBuf[23]='-';
	toAsciiHex(msgDigest+18, 6, digBuf+24);
	digBuf[36] = '\0';
	return digBuf;
}
