#ifndef SYSUTILS_H_
/*----------------------------------------------------------------------*
 * Gatespace
 * Copyright 2008 Gatespace Networks, Inc., All Rights Reserved.
 * Gatespace Networks, Inc. confidential material.
 *----------------------------------------------------------------------*
 * File Name  :
 * Description:
 *----------------------------------------------------------------------*
 * $Revision: 1.2 $
 *
 * $Id: sysutils.h,v 1.2 2012/05/10 17:38:01 dmounday Exp $
 *----------------------------------------------------------------------*/

#define SYSUTILS_H_

unsigned findProc(char *pattern);
void signalDaemon(const char *name, int sig);
void stopDaemon(const char* name);
void killDaemon(const char* name);
int  do_cmd(int logon, const char *cmd, char *fmt, ...);
char *cpeReadFileField( const char *fname, const char *prefix, int field);

#endif /*SYSUTILS_H_*/
