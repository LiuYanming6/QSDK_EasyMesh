/*
 * Gatespace
 * Copyright 2011 Gatespace. All Rights Reserved.
 * Gatespace Networks, Inc. confidential material.
 * ipv4ipv6.c : Utility functions for handling IPv4 and IPv6
 * 				addresses.
 *
 *  Created on: Jul 29, 2009
 *      Author: dmounday
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <arpa/inet.h>
#include "utils.h"

const char *writeIPAddr( SockAddrStorage *p ){
#ifdef USE_IPv6
	static char addrstr[INET6_ADDRSTRLEN];
	sa_family_t family = ((struct sockaddr *)p)->sa_family;

	if ( family == AF_INET6 ){
		inet_ntop( family, &((struct sockaddr_in6*)p)->sin6_addr,
				addrstr, INET6_ADDRSTRLEN);
	}else {
		inet_ntop( family, &((struct sockaddr_in*)p)->sin_addr,
				addrstr, INET6_ADDRSTRLEN);
	}
	return addrstr;
#else
	return inet_ntoa(((struct sockaddr_in*)p)->sin_addr);
#endif /* USE_IPv6 */
}
/*
 * If USE_IPv6:
 * If address family is 0 then return pointer to empty string.
 * otherwise return IPv4 or IPv6 address.
 */
const char *writeInIPAddr( InAddr *inp ){
#ifdef USE_IPv6
	static char addrstr[INET6_ADDRSTRLEN];
	sa_family_t family = inp->inFamily;
	if ( family == AF_INET || family==AF_INET6 )
		return inet_ntop( family, &inp->u.inAddr, addrstr, INET6_ADDRSTRLEN);
	else {
		addrstr[0]='\0';
		return addrstr;
	}
#else
	struct in_addr ip;
	ip = inp->u.inAddr;
	return inet_ntoa( ip );
#endif /* USE_IPv6 */
}

int readInIPAddr( InAddr *inp, const char *aStr ){
	if ( aStr == NULL)
		return -1;
#ifdef USE_IPv6
	if ( strchr(aStr, ':' )!=NULL ){
		inp->inFamily = AF_INET6;
	} else {
		inp->inFamily = AF_INET;
	}
	return inet_pton( inp->inFamily, aStr, &inp->u.inAddr );
#else
	inp->inFamily = AF_INET;
	return (inp->u.inAddr.s_addr=inet_addr( aStr ))==-1? -1: 1;
#endif
}
/*
 * Compare for equal. Addresses is limited to the length
 * of the family address.
 * returns: 1 if equal.
 *          0 if not equal.
 */
int eqInIPAddr( InAddr *in1, InAddr *in2){
	/* structure alignment requires multiple compares */
	int r = 0;
	sa_family_t fam1 = in1->inFamily;
	if ( fam1 != in2->inFamily )
		return 0;
#ifdef USE_IPV6
	int lth = fam1==AF_INET6? sizeof(struct in6_addr): sizeof(struct in_addr);
#else
	int lth = sizeof( struct in_addr );
#endif
	r = memcmp( &in1->u.inAddr, &in2->u.inAddr, lth);
	return r!=0? 0: 1;
}

void clearInIPAddr( InAddr *inp){
	memset( inp, 0, sizeof(struct InAddr));
}
