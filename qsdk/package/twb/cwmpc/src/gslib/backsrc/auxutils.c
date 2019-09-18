 /*----------------------------------------------------------------------*
 * Gatespace Networks, Inc.
 * Copyright 2004,2005 Gatespace. All Rights Reserved.
 * Gatespace Networks, Inc. confidential material.
 *----------------------------------------------------------------------*
 * File Name  :auxutils.c
 *
 * Description: auxiliary utility routines for GS application environment.
 *
 * Typically not required.
 *
 * $Revision: 1.2 $
 * $Id: auxutils.c,v 1.2 2009/09/03 14:49:22 dmounday Exp $
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
#include <sys/stat.h>

#include <dirent.h>


/**********************************************************************
 * Files
 **********************************************************************/
/*--------------------
 * returns 1 if files differ 0 otherwise
 */
int hasChanged(const char* new, const char* old)
{
	int ret = 0;
	FILE* fn = fopen(new, "r");
	FILE* fo = fopen(old, "r");
	char bufn[1024];
	char bufo[1024];

	if (!fn) {
		ret = 0;
	} else if (!fo) {
		ret = 1;
	} else {
		for (;;) {
			int nn = fread(bufn, 1, sizeof bufn, fn);
			int no = fread(bufo, 1, sizeof bufo, fo);
			if (nn != no) {
				ret = 1;
				break;
			}

			if (nn == 0) {
				break;
			}

			if (memcmp(bufn, bufo, nn)) {
				ret = 1;
				break;
			}
		}
	}
	if (fn) {
		fclose(fn);
	}
	if (fo) {
		fclose(fo);
	}
	return ret;
}

/*--------------------*/
int mkdirs(const char *path)
{
	struct stat sb;
	char buf[256];
	char* p;

	if (stat(path, &sb) && errno == ENOENT) {
		strcpy(buf, path);
		p = buf + strlen(buf) - 1;
		while (p > buf && *p=='/') p--;
		while (p >= buf && *p!='/')	p--;
		while (p > buf && *p=='/') p--;

		if (p < buf) {
			p = buf;
			*p = '.';
		}
		p[1] = 0;

		if (mkdirs(buf))
			return -1;

		if (mkdir(path, 0777))
			return -1;
	}
	return 0;
}

/*--------------------*/
static void rmrf1(char** p, int* max, int n)
{
	struct stat sb;

	(*p)[n] = 0;
	if (lstat(*p,&sb))
		return;

	if (S_ISDIR(sb.st_mode)) {
		DIR *dp = opendir(*p);
		if (dp) {
			struct dirent *de;
			int k = n;
			if (strcmp(*p,"/")) {
				(*p)[n] = '/';
				k = n + 1;
			}
			while ((de = readdir(dp))) {
				char* f = de->d_name;
				if (strcmp(f, ".") && strcmp(f, "..")) {
					int m = k + strlen(f);
					while (*max <= m) {
						*max *= 2;
						*p = GS_REALLOC(*p, *max);
					}
					strcpy(*p + k, f);
					rmrf1(p, max, m);
				}
			}
			closedir(dp);
		}
		(*p)[n] = 0;
		rmdir(*p);
	} else {
		unlink(*p);
	}
}

/*--------------------*/
void rmrf(const char* path)
{
	int n = strlen(path);
	int len = n > 254 ? 2*n : 256;
	char* buf=GS_MALLOC(len);

	strcpy(buf, path);
	while ( n > 1 && buf[n-1] == '/') {
		n--;
	}
	rmrf1(&buf, &len, n);
	GS_FREE(buf);
}

/*--------------------*/
int readProto(const char* val)
{
	if (!strcmp(val,"udp") || !strcmp(val, "UDP")) {
		return IPPROTO_UDP;
	}

	if (!strcmp(val,"icmp") || !strcmp(val, "ICMP")) {
		return IPPROTO_ICMP;
	}

	return IPPROTO_TCP;
}

/*--------------------*/
int readMask(const char *mask)
{
	int ip = readIp(mask);
	int count = 1;
	int bit = 1;

	while ((bit = (ip << 1))) {
		if (bit) count++;
		ip = (ip << 1);
	}
	return count;
}

/*--------------------*/
void writeIp_b(int ip, char *buf)
{
	sprintf(buf,"%d.%d.%d.%d",
			0xff&(ip>>24), 0xff&(ip>>16), 0xff&(ip>>8), 0xff&ip);
}
/*--------------------*/
char* writeIp(int ip)
{
	static char buf[24];
	writeIp_b(ip, buf);
	return buf;
}


/*--------------------*/
char* writeNet(int ip, int bits)
{
	static char buf[24];

	ip &= INADDR_BROADCAST << (32 - bits);
	sprintf(buf,"%d.%d.%d.%d",
			0xff&(ip>>24), 0xff&(ip>>16), 0xff&(ip>>8), 0xff&ip);
	return buf;
}

/*--------------------*/
char* writeBcast(int ip, int bits)
{
	static char buf[24];

	ip |= INADDR_BROADCAST >> bits;
	sprintf(buf,"%d.%d.%d.%d",
			0xff&(ip>>24), 0xff&(ip>>16), 0xff&(ip>>8), 0xff&ip);
	return buf;
}

/*--------------------*/
char* writeMask(int bits)
{
	static char buf[24];
	int ip = INADDR_BROADCAST << (32 - bits);

	sprintf(buf,"%d.%d.%d.%d",
			0xff&(ip>>24), 0xff&(ip>>16), 0xff&(ip>>8), 0xff&ip);
	return buf;
}

/*--------------------*/
char* writeRevNet(int ip, int bits)
{
	static char buf[24];
	char* p = buf;

	if (bits % 8) {
		cpeLog(LOG_ERROR,"Cannot split FQDN %s/%d", writeIp(ip), bits);
	}

	ip >>= 32 - bits;
	while (bits > 0) {
		if (p > buf) {
			*p++ = '.';
		}
		sprintf(p, "%d", 0xff&ip);
		while (*p) {
			p++;
		}
		ip >>= 8;
		bits -= 8;
	}
	return buf;
}

/*--------------------*/
char* writeRevHost(int ip, int bits)
{
	static char buf[24];
	char* p = buf;

	if (bits % 8) {
		cpeLog(LOG_ERROR ,"Cannot split FQDN %s/%d", writeIp(ip), bits);
	}

	bits = 32 - bits;
	while (bits > 0) {
		if (p > buf) {
			*p++ = '.';
		}
		sprintf(p, "%d", 0xff&ip);
		while (*p) p++;
		ip >>= 8;
		bits -= 8;
	}
	return buf;
}

/*--------------------*/
char* writeProto(int proto)
{
	switch (proto) {
	case IPPROTO_UDP:
		return "udp";
	case IPPROTO_ICMP:
		return "icmp";
	case IPPROTO_TCP:
	default:
		return "tcp";
	}
}

