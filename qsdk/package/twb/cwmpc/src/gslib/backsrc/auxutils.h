/*----------------------------------------------------------------------*
 * Gatespace
 * Copyright 2004,2005,2006 Gatespace. All Rights Reserved.
 * Gatespace Networks, Inc. confidential material.
 *----------------------------------------------------------------------*
 * File Name  :
 *
 * Description: Auxiliary utility routines prototypes for gs apps.
 *
 *
 * $Revision: 1.1 $
 * $Id: auxutils.h,v 1.1 2009/08/14 17:43:01 dmounday Exp $
 *----------------------------------------------------------------------*/

#ifndef AUXUTILS_H_
#define AUXUTILS_H_
/* Files */
int  hasChanged(const char* new, const char* old);
int  mkdirs(const char *path);
void rmrf(const char* path);
int  readProto(const char* val);
int readMask(const char *mask);
char* writeNet(int ip, int bits);
char* writeBcast(int ip, int bits);
char* writeMask(int bits);
void  writeIp_b(int ip, char *buf);
char* writeIp(int ip);
char* writeRevNet(int ip, int bits);
char* writeRevHost(int ip, int bits);
char* writeProto(int proto);

#endif /* AUXUTILS_H_ */
