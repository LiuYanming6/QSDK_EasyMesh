/*----------------------------------------------------------------------*
 * Gatespace
 * Copyright 2004,2005,2006 Gatespace. All Rights Reserved.
 * Gatespace Networks, Inc. confidential material.
 *----------------------------------------------------------------------*
 * File Name  :
 *
 * Description: text utility routines prototypes for gs apps.
 *
 *
 * $Revision: 1.2 $
 * $Id: textutil.c,v 1.2 2009/09/03 14:49:22 dmounday Exp $
 *----------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/utsname.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/errno.h>
//#include <net/if.h>
#include <sys/types.h>

/**********************************************************************
 * Text formatting
 *********************************************************************/
/*--------------------*/
void readHash(u_char* hash, const char* val)
{
	u_char* ehash = hash + MD5_DIGEST_LEN;
	int flag = 0;
	int b = 0;

	while (*val && hash < ehash) {
		int c = hexChar(*val++);
		if (c >= 0) {
			b = (b << 4) | c;
			if (flag) {
				*hash++ = b;
			}
			flag = !flag;
		}
	}
}

/*--------------------*/
char* writeQHash(const u_char* hash)
{
	int i;
	static char buf[128];
	char* p = buf;

	for (i=0; i < MD5_DIGEST_LEN; i++) {
		if (i > 0) {
			*p++ = '\\';
			*p++ = ':';
		}
		sprintf(p, "%02x", hash[i]);
		p += 2;
	}
	return buf;
}

/*--------------------*/
char* unquoteText(const char* t)
{
	int len = 0;
	char* t1;
	const char* p;
	char* p1;

	for (p = t; *p; p++) {
		if (*p != '\\') {
			len++;
		}
	}
	t1 = GS_MALLOC(len + 1);

	for (p = t, p1 = t1; *p; p++) {
		if (*p != '\\') {
			*p1++ = *p;
		}
	}
	*p1 = 0;

	return t1;
}

/*--------------------*/
char* quoteText(const char* t)
{
	int len = 0;
	char* t1;
	const char* p;
	char* p1;

	for (p = t; *p; p++) {
		len += *p == ':' ? 2 : 1;
	}
	t1 = GS_MALLOC(len + 1);

	for (p = t, p1 = t1; *p; p++) {
		if (*p == ':') {
			*p1++ = '\\';
		}
		*p1++ = *p;
	}
	*p1 = 0;

	return t1;
}

/**********************************************************************
 * hex
 **********************************************************************/
static char hex[] = {
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
};

const char *util_StringToHex(const char *s)
{
	static char buf[256];
	char *p;

	if (strlen(s) > 126) return NULL;
	p = buf;
	while (*s) {
		*p++ = hex[((*s & 0xF0) >> 4)];
		*p++ = hex[(*s & 0x0F)];
		s++;
	}
	*p = '\0';
	return buf;
}
