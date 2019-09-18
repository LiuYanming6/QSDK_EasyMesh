#ifndef _UTILS_IPV6_H_
#define _UTILS_IPV6_H_

/** -------------------------------------------------------------------------
                          INCLUDE HEADER FILES                             
  -------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <linux/if.h>
#include <stdarg.h>
/** -------------------------------------------------------------------------
                          DEFINITIONS                             
  -------------------------------------------------------------------------*/
#define IPV6_ADDR_ANY         0x0000U
#define MAX_IPV6_STR_SIZE                         47 // INET6_ADDRSTRLEN (48)
#define MAX_IFNAME_STR_SIZE                 16

/** -------------------------------------------------------------------------
                          FUNCTIONS                             
  -------------------------------------------------------------------------*/
int ubox_ipv6_is_connected(void);
static char * ubox_ipv6_get_intf_address(char * ifname, int scope);
char * ubox_ipv6_get_default_gateway(void);

#endif

