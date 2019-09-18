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
 * $Revision: 1.1 $
 * $Id: textutils.h,v 1.1 2009/08/14 17:43:01 dmounday Exp $
 *----------------------------------------------------------------------*/

#ifndef TEXTUTILS_H_
#define TEXTUTILS_H_
void  readHash(u_char* hash, const char* val);
char* writeQHash(const u_char* mac);
char* unquoteText(const char* text);
char* quoteText(const char* text);
/* hex */
const char *util_StringToHex(const char *s);
#endif /* TEXTUTILS_H_ */
