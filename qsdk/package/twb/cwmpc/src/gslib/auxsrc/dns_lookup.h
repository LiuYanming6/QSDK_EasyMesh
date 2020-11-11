#ifndef DNS_LOOKUP_H
#define DNS_LOOKUP_H
/*----------------------------------------------------------------------*
 * Gatespace Networks, Inc.
 * Copyright 2004,2005,2006 Gatespace. All Rights Reserved.
 *----------------------------------------------------------------------*
 * File Name  :dns_lookup.c
 *
 * Description: This is blocking DNS lookup routine for use by
 *       applications that do not need the non-blocking lookup
 *       interface. May be linked to web protocol routines.
 *
 *
 * $Revision: 1.3 $
 * $Id: dns_lookup.h,v 1.3 2009/08/14 17:45:14 dmounday Exp $
 *----------------------------------------------------------------------*/

int dns_lookup(const char *name, int sockType, int family, InAddr *);
int dns_lookup2(const char *name, int sockType,  InAddr *);
int dns_lookup_auto(const char *name, int sockType, unsigned int port, int family, InAddr *);
void dns_set_cache_host(const char *name);
void dns_set_cache_host_ip(const char *name, InAddr *ip);
void dns_remove_cache_host(const char *name);
int dns_get_next_ip(const char *name, InAddr *res);
int check_v4_v6(char *hostname);
#endif
