/*----------------------------------------------------------------------*
 * Gatespace Networks, Inc.
 * Copyright 2005 Gatespace Networks, Inc., All Rights Reserved.
 * Gatespace Networks, Inc. confidential material.
 *----------------------------------------------------------------------*
 * File Name  : sysWrapper.c
 *
 * Description: Functions that wrap the CPE system services
 *																		* 
 * $Revision: 1.2 $
 * $Id: sysWrapper.c,v 1.2 2010/07/30 14:45:50 dmounday Exp $
 *----------------------------------------------------------------------*/
#include <stdio.h>
#include <stdarg.h>
#include <syslog.h>


static int dbgMask;

static int  verbose;
void cpeSetDbgLogMask( int mask )
{
	dbgMask = mask;
}

void cpeDbgLog( int mask, const char *fmt, ...)
{
	if (dbgMask&mask) {
		va_list ap;
		va_start(ap,fmt);
		vfprintf(stderr, fmt, ap);
		fprintf(stderr, "\n");
		va_end(ap);
	}
}

void cpeLog(int level, const char* fmt, ...)
{
	va_list ap;
	if (verbose) {
		va_start(ap,fmt);
		vfprintf(stderr, fmt, ap);
		va_end(ap);
		fprintf(stderr, "\n");
	}
	va_start(ap,fmt);
	vsyslog(level, fmt, ap);
	va_end(ap);
}

void cpeVlog(int level, const char* fmt, va_list ap)
{
	if (verbose) {
		vfprintf(stderr, fmt, ap);
	} else
		vsyslog(level, fmt, ap);
}

void cpeInitLog(int flag)
{
	verbose = flag;
	openlog( "user", LOG_CONS,LOG_DAEMON);
}
