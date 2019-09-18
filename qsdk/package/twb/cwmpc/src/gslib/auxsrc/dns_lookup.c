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
 *  This implementation may not be applicable to all environments. The
 *  functions provided support the requirements of the DNS caching and
 *  IP address affinity recommendations of TR-069 section 3.1.
 *
 *  In an alternate implementation the caching may not be implemented,
 *  although the dns_get_next_ip() function should scan the list of IP
 *  addresses returned from the DNS lookup. The functions:
 *   dns_remove_cache()
 *   dns_set_cache_host()
 *   dns_set_cache_host_ip()
 *  may be implemented to just return a 0 value.
 *
 * $Revision: 1.7 $
 * $Id: dns_lookup.c,v 1.7 2009/12/21 12:58:46 dmounday Exp $
 *----------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <time.h>
#include <syslog.h>
#include <string.h>

#include <net/if.h>
#include <sys/ioctl.h>

/*
 * The macro USE_GETADDRINFO will compile the DNS functions to use the
 * getaddrinfo() library call instead of the gethostbyname() function.
 * The gethostbyname may not work for IPv6 addresses on all Linux systems.
 */
#ifdef USE_IPv6
#define USE_GETADDRINFO
#endif


#ifdef DMALLOC
	#include "dmalloc.h"
#endif

#include "../src/utils.h"
#include "cpelog.h"
//#define DEBUG
#ifdef DEBUG
#define DBGPRINT(X) fprintf X
#else
#define DBGPRINT(X)
#endif


typedef struct HostCache{
	struct HostCache	*next;
	char		hostname[257];	/* pointer to host name string */
	int			hListCnt;	/* number of address pointers in hostList */
	InAddr		*hostList;	/* pointer to array of host addresses */
	InAddr		lastIP;		/* last IP returned for this hostname*/
	int			startIdx;	/* starting hostList index */
	int			idx;		/* current hostList index */
} HostCache;

static HostCache	*hcache;
static HostCache	*lastHost;

static void freeHostList(HostCache *hc){
	InAddr *hlp = hc->hostList;
	if ( hlp ){
		GS_FREE (hlp);
	}
}

static void freeHostCache(HostCache *hc){
	freeHostList(hc);
	GS_FREE(hc);
}

static HostCache *findCache( const char *hostname){
	HostCache *hc = hcache;
	while (hc){
		if ( strcmp( hostname, hc->hostname )==0 )
			return hc;
		hc = hc->next;
	}
	return NULL;
}
/*
 * Removes the hostname from the DNS cache.
 * returns 1: found and removed.
 * 		   0: host cache not found.
 *
 * In alternate implementation this function may
 * at a minimum just return 0.
 */
int dns_remove_cache_host(const char *hostname){
	HostCache *hc = hcache;
	HostCache **last = &hcache;
	while (hc){
		if ( strcmp( hostname, hc->hostname )==0 ){
			*last = hc->next;
			freeHostCache(hc);
			return 1;
		}
		last = &hc->next;
		hc = hc->next;
	}
	return 0;
}
#ifdef USE_GETADDRINFO
/*
 * Updates the cached list of IP addresses from the addrinfo list.
 * If the lastIp address is present in the new addrinfo list it
 * is returned; otherwise, a random address is chosen.
 * Returns: <=0  error
 *           1  result set in *result.
 */
static int updateCache(InAddr *result, HostCache *hc, struct addrinfo *alist){
	struct  addrinfo *next;

	InAddr  *hlp = hc->hostList;
	int		hcnt = 0;

	if (hlp ){
		freeHostList(hc);
	}
	next = alist;
	/* count the addresses returned. */
	while ( next ){
		hcnt++;
		next = next->ai_next;
	}
	hc->hListCnt = hcnt;
	if ( (hlp = hc->hostList = (InAddr *)GS_MALLOC( sizeof(struct InAddr)*hcnt )) ==NULL){
		return -1;
	}
	memset( hlp, 0, sizeof(struct InAddr)*hcnt);
	hcnt = 0;
	next = alist;
	/* move addrinfo address list to the HostCache InAddr list */
	while (next!=NULL){
		hlp->inFamily = next->ai_family;
		if ( next->ai_family == AF_INET6 ){
			hlp->u.in6Addr = ((struct sockaddr_in6 *)(next->ai_addr))->sin6_addr;
		} else {
			hlp->u.inAddr = ((struct sockaddr_in *)(next->ai_addr))->sin_addr;
		}
		DBGPRINT((stdout, "Move to cache %d: %s\n", hcnt, writeInIPAddr(hlp)));
		++hcnt;
		next = next->ai_next;
		++hlp;
	}
	/* if lastIP try to find it in cache list */
	hlp = hc->hostList;
	if ( hc->lastIP.inFamily!= 0 ) {
		hcnt = 0;
		while ( hcnt<hc->hListCnt ){
			if ( eqInIPAddr( hlp+hcnt, &hc->lastIP )){
				hc->startIdx = hc->idx = hcnt;
				DBGPRINT((stdout, "fnd lastIP at %d\n", hcnt));
				*result = hc->lastIP;
				return 1;
			}
			++hcnt;
		}
	}
	/* no last IP set or not found so pick an ip from list */
	hcnt = rand()%hc->hListCnt;
	hc->startIdx = hc->idx = hcnt;
	hc->lastIP = *(hlp+hcnt);
	DBGPRINT((stdout, "lastIP not set, start at %d with %s\n", hcnt, writeInIPAddr(&hc->lastIP)));
	*result = hc->lastIP;
	return 1;
}
#else
/*
 * returns an IP address.
 * Updates the cached list of IP addresses.
 */
static int updateCache(InAddr *result, HostCache *hc, struct hostent *netent){
	InAddr  *hlp = hc->hostList;
	char	**nlp = netent->h_addr_list;
	int		hcnt = 0;

	if (hlp ){
		freeHostList(hc);
	}
	/* count the number of addresses in hostent h_addr_list */
	while ( *nlp++ ) hcnt++;
	hc->hListCnt = hcnt;
	DBGPRINT((stdout, "cache addr cnt=%d\n", hcnt));
	if ( (hlp = hc->hostList = (InAddr *)GS_MALLOC( sizeof(struct InAddr)*hcnt ))==NULL)
		return -1;
	memset(hlp, 0, sizeof(struct InAddr));
	nlp = netent->h_addr_list;
	hcnt = 0;
	/* move h_addr_list to cache array */
	while (hcnt < hc->hListCnt){
		struct in_addr *a = (struct in_addr *)*nlp;
		SET_InADDR(hlp, netent->h_addrtype, a);
		DBGPRINT((stdout, "Move to cache %d: %s\n", hcnt, writeInIPAddr(hlp)));
		++hcnt;
		++hlp;
		++nlp;
	}
	/* if lastIP try to find it in cache list */
	hlp = hc->hostList;
	if ( hc->lastIP.inFamily!= 0 ) {
		hcnt = 0;
		while ( hcnt<hc->hListCnt ){
			if ( eqInIPAddr( hlp+hcnt, &hc->lastIP) ){
				hc->startIdx = hc->idx = hcnt;
				DBGPRINT((stdout, "fnd lastIP at %d\n", hcnt));
				*result = hc->lastIP;
				return 1;
			}
			++hcnt;
		}
	}
	/* no last IP set or not found so pick an ip from list */
	hcnt = rand()%hc->hListCnt;
	hc->startIdx = hc->idx = hcnt;
	hc->lastIP = *(hlp+hcnt);
	DBGPRINT((stdout, "lastIP not set start at %d with %s\n", hcnt, writeInIPAddr(&hc->lastIP)));
	*result = hc->lastIP;
	return 1;
}
#endif

/*
 *
 */
static HostCache *createCacheEntry( const char *hostname, InAddr *ip){
	HostCache *hc;
	if ( (hc = (HostCache*)GS_MALLOC(sizeof(HostCache)))){
		memset(hc, 0, sizeof(HostCache));
		strncpy(hc->hostname, hostname, sizeof(hc->hostname));
		if ( ip != NULL)
			hc->lastIP = *ip;
		return hc;
	}
	return NULL;
}
/*
 * Add the host name and IP address to the DNS cache.
 * Return 1 if added successfully
 *        0 if failed.
 *
 * In alternate implementation this function may
 * at a minimum just return 0.
 */
int dns_set_cache_host_ip(const char *hostname, InAddr *ip){
	HostCache *hc;
	if ( findCache(hostname) )
		return 1;
	if ( (hc = createCacheEntry(hostname, ip))){
		hc->next = hcache;
		hcache = hc;
		return 1;
	}
	return 0;
}

/*
 * Add the hostname to the DNS cache. The IP addresses associated with
 * this hostname will be cached when resolved.
 * Return 1 if added successfully
 *        0 if failed.
 *
 * In alternate implementation this function may
 * at a minimum just return 0.
 */
int dns_set_cache_host(const char *hostname){
	InAddr zeroIP;
	clearInIPAddr(&zeroIP);
	return dns_set_cache_host_ip( hostname, &zeroIP );
}

/*
 * This returns the next IP address from the list cached from the
 * previous call to dns_lookup for the named host.
 * This is always a non-blocking call.
 *
 * Returns: 0 - No next IP address.
 *        !=0 - The next IP address was returned in *res.
 */
int dns_get_next_ip(const char *name, InAddr *res){
	HostCache *hc;
	InAddr    *hlp;
	if ( lastHost && ( strcmp(lastHost->hostname, name) == 0))
			hc = lastHost;
	else
		hc = findCache(name);
	if ( hc ){
		if ( (hlp = hc->hostList) ){
			hc->idx = (hc->idx+1)%hc->hListCnt;
			if ( hc->idx != hc->startIdx ){
				*res = hc->lastIP = *(hlp+hc->idx);
				cpeLog (LOG_DEBUG,"dns_get_next_ip(%s) = %s\n", name, writeInIPAddr(res));
				return 1;
			}
		}
	}
	clearInIPAddr(res);
	return 0;
}

#ifdef USE_GETADDRINFO
/*
 * dns lookup that uses getaddrinfo or gethostbyname -- this is blocking call.
 * Return value should always be true (1)
 * .
 * sockType maybe set to the socket type such as SOCK_STREAM.
 * This function may be replaced with a non-blocking implementation of
 * the dns_lookup. If the function is non-blocking and the lookup is in
 * progress then return a 0.
 *
 * This implementation is blocking and the lookup is complete when the
 * return is made. Thus the return must be a 1.
 *
 * The InAddr result->inFamily is set to 0 if the DNS lookup failed to
 * find a host.
 */
int dns_lookup(const char *name, int sockType, int family, InAddr *result)
{
	struct addrinfo *r;
	struct addrinfo hints;
	result->inFamily = 0;
	if ( readInIPAddr(result, name) > 0 ) {
		/* already a digit string */
		return 1;
	} else 
	{
		memset(&hints, 0, sizeof(struct addrinfo));
		hints.ai_flags = AI_ADDRCONFIG;
		hints.ai_family = family;
		hints.ai_socktype = sockType;
		if ( getaddrinfo( name, NULL, &hints, &r ) == 0 ){
			HostCache *hc;
			if ( (hc = findCache(name))){
				updateCache(result, hc, r);
			} else {
				if ( lastHost )
					freeHostCache(lastHost);
				if ( (lastHost = createCacheEntry(name, NULL)) ){
					updateCache(result, lastHost, r);
					DBGPRINT((stdout, "No cache set return %s\n", writeInIPAddr(result)));
				}
			}
			freeaddrinfo(r);
            return 1;
            
		} else {
			clearInIPAddr(result);
		}
	}
    return 0;
}
#else

extern int h_errno;

int dns_lookup(const char *name, int socktype, InAddr *result)
{
	struct hostent *netent;

	result->inFamily = 0;
	if ( readInIPAddr(result, name) > 0 ) {
		/* already a digit string */
		return 1;
	} else if ( (netent = gethostbyname( name ))) {
		HostCache *hc;
		if ( (hc = findCache(name))){
			updateCache(result, hc, netent);
		} else {
			if ( lastHost )
				freeHostCache(lastHost);
			if ( (lastHost = createCacheEntry(name, 0)) ){
				updateCache(result, lastHost, netent);
				DBGPRINT((stdout, "No cache set return %s\n", writeInIPAddr(result)));
			}
		}
		cpeLog (LOG_DEBUG,"dns_lookup(%s) = %s\n", name, writeInIPAddr(result));


	} else {
		cpeLog (LOG_ERR, "dns_lookup(%s) failed with (%d) %s", name, h_errno, hstrerror(h_errno));
		clearInIPAddr(result);
	}
	return 1;
}
#endif

int dns_lookup_auto(const char *name, int sockType, unsigned int port, int family, InAddr *result)
{
    int fd;
    SockAddrStorage sa;
    struct sockaddr_in *acsp=(struct sockaddr_in *)&sa;

    if(family == AF_UNSPEC)
    {
#ifdef USE_IPv6
        if( dns_lookup(name, sockType, AF_INET6, result) && result->inFamily != AF_INET )
        {
            SET_SockADDR(acsp, port, result);
            if ( (fd=socket(result->inFamily, SOCK_DGRAM, 0))>=0 ){
                /* since this is a DGRAM connection there shouldn't be any traffic */
                if ( connect(fd, (struct sockaddr*)acsp, SockADDRSZ(acsp))>=0 ){
                    close(fd);
		    return 1;
                }
                close(fd);
            }
            while(dns_get_next_ip(name, result))
            {
                SET_SockADDR(acsp, port, result);
                if ( (fd=socket(result->inFamily, SOCK_DGRAM, 0))>=0 ){
                    /* since this is a DGRAM connection there shouldn't be any traffic */
                    if ( connect(fd, (struct sockaddr*)acsp, SockADDRSZ(acsp))>=0 ){
                        close(fd);
                        return 1;
                    }
                    close(fd);
                }
            }
        }
#endif
        if (dns_lookup(name, sockType, AF_INET, result) && result->inFamily != AF_INET6 )
        {
            SET_SockADDR(acsp, port, result);
            if ( (fd=socket(result->inFamily, SOCK_DGRAM, 0))>=0 ){
                /* since this is a DGRAM connection there shouldn't be any traffic */
                if ( connect(fd, (struct sockaddr*)acsp, SockADDRSZ(acsp))>=0 ){
                    close(fd);
                    return 1;
                }
                close(fd);
            }
            while(dns_get_next_ip(name, result))
            {
                SET_SockADDR(acsp, port, result);
                if ( (fd=socket(result->inFamily, SOCK_DGRAM, 0))>=0 ){
                    /* since this is a DGRAM connection there shouldn't be any traffic */
                    if ( connect(fd, (struct sockaddr*)acsp, SockADDRSZ(acsp))>=0 ){
                        close(fd);
                        return 1;
                    }
                    close(fd);
                }
            }
        }
    }
    else
    {
        dns_lookup(name, sockType, family, result);
        SET_SockADDR(acsp, port, result);
        if ( (fd=socket(result->inFamily, SOCK_DGRAM, 0))>=0 ){
            /* since this is a DGRAM connection there shouldn't be any traffic */
            if ( connect(fd, (struct sockaddr*)acsp, SockADDRSZ(acsp))>=0 ){
                close(fd);
                return 1;
            }
            close(fd);
        }
        while(dns_get_next_ip(name, result))
        {
            SET_SockADDR(acsp, port, result);
            if ( (fd=socket(result->inFamily, SOCK_DGRAM, 0))>=0 ){
                /* since this is a DGRAM connection there shouldn't be any traffic */
                if ( connect(fd, (struct sockaddr*)acsp, SockADDRSZ(acsp))>=0 ){
                    close(fd);
                    return 1;
                }
                close(fd);
            }
        }
    }
    return 0;
}


#ifdef USE_GETADDRINFO
// Check the hostname Use v6 or v4
int check_v4_v6(char *hostname)
{

    struct addrinfo hint, *res, *p;
    int err;


    memset(&hint, 0, sizeof hint);
    hint.ai_family = AF_UNSPEC;
    hint.ai_socktype = SOCK_STREAM;


    err = getaddrinfo(hostname, "http", &hint, &res);
    if(err != 0) {
        DBG_MSG("ERR: %s", gai_strerror(err));
        return -1;
    }

    // list all ips
#if 0
     void *addr;
     struct sockaddr_in *ipv4;
     struct sockaddr_in6 *ipv6;
     char ipstr[INET6_ADDRSTRLEN];
#endif
     int i = 0;  //ipv6 count;
     int j = 0;  //ipv4 count;
    
     for(p = res ;p != NULL; p = p->ai_next) {

     if(p->ai_family == AF_INET6)
        i++;
     else
        j++;
#if 0
         // ipv4
         if(p->ai_family == AF_INET) {
             // convert generic addr to internet addr
             ipv4 = (struct sockaddr_in *)p->ai_addr;
    
             // point to binary addr for convert to str later
             addr = &(ipv4->sin_addr);
    
         // ipv6
         } else {
             // convert generic addr to internet addr
             ipv6 = (struct sockaddr_in6 *)p->ai_addr;
    
             // point to binary addr for convert to str later
             addr = &(ipv6->sin6_addr);
         }
    
         // convert binary addr to str
         inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr);
         printf("%s\n", ipstr);
#endif
     }
     // don't forget to free the addrinfo
     freeaddrinfo(res);

     if (i > 0)  // ipv6
        return 1;
     else // ipv4
        return 0;
}

#endif
