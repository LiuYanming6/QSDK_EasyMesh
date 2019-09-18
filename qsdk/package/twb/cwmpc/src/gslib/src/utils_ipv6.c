/*******************************************************************************
 *        Copyright (c) 2017 TWin Advanced Technology Corp., Ltd.
 *        All rights reserved.
 *
 *       THIS WORK IS AN UNPUBLISHED WORK AND CONTAINS CONFIDENTIAL,
 *       PROPRIETARY AND TRADESECRET INFORMATION OF T&W INCORPORATED.
 *       ACCESS TO THIS WORK IS RESTRICTED TO (I) T&W EMPLOYEES WHO HAVE A
 *       NEED TO KNOW TO PERFORM TASKS WITHIN THE SCOPE OF THEIR ASSIGNMENTS
 *       AND (II) ENTITIES OTHER THAN T&W WHO HAVE ENTERED INTO APPROPRIATE
 *       LICENSE AGREEMENTS.  NO PART OF THIS WORK MAY BE USED, PRACTICED,
 *       PERFORMED, COPIED, DISTRIBUTED, REVISED, MODIFIED, TRANSLATED,
 *       ABBRIDGED, CONDENSED, EXPANDED, COLLECTED, COMPILED, LINKED, RECAST,
 *       TRANSFORMED OR ADAPTED WITHOUT THE PRIOR WRITTEN CONSENT OF T&W.
 *       ANY USE OR EXPLOITATION OF THIS WORK WITHOUT AUTHORIZATION COULD
 *       SUBJECT THE PERPERTRATOR TO CRIMINAL AND CIVIL LIABILITY.
 ******************************************************************************/
 /**
 * @file utils_ipv6.c
 * @brief ...
 * @author T&W Technology co., Ltd.
 * @bug No known bugs
 */

/*-------------------------------------------------------------------------*/
/*                        INCLUDE HEADER FILES                             */
/*-------------------------------------------------------------------------*/
#include "utils_ipv6.h"
#include "utils.h"
/*-------------------------------------------------------------------------*/
/*                           DEFINITIONS                                   */
/*-------------------------------------------------------------------------*/
#define FILE_HYD_READER "/tmp/hyd-capture"
#define  CFG_WIFI_SON_HYD_IP "127.0.0.1"
#define CFG_WIFI_SON_HYD_PORT 7777
/*-------------------------------------------------------------------------*/
/*                           VARIABLES                                     */
/*-------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------*/
/*                           FUNCTIONS                                     */
/*-------------------------------------------------------------------------*/

int ubox_ipv6_is_connected(void)
{
        return strcmp(ubox_ipv6_get_intf_address("br-lan", IPV6_ADDR_ANY), "") != 0
                && strcmp(ubox_ipv6_get_default_gateway(), "") != 0;
}


static char * ubox_ipv6_get_intf_address(char * ifname, int scope)
{
        static char address[MAX_IPV6_STR_SIZE+1] = {0};
        FILE *f;
        int scope_tmp, prefix;
        unsigned char ipv6[16];
        char dname[MAX_IFNAME_STR_SIZE+1];

        memset(address, 0, sizeof(address));

        if (ifname == NULL || strlen(ifname) == 0)
                return address;

    f = fopen("/proc/net/if_inet6", "r");
    if (f == NULL) {
        return address;
    }

    while (19 == fscanf(f,
                    " %2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx %*x %x %x %*x %s"
                    , &ipv6[0], &ipv6[1], &ipv6[2], &ipv6[3], &ipv6[4], &ipv6[5], &ipv6[6], &ipv6[7]
                    , &ipv6[8], &ipv6[9], &ipv6[10], &ipv6[11], &ipv6[12], &ipv6[13], &ipv6[14], &ipv6[15]
                    , &prefix,
                    &scope_tmp,
                    dname))
        {
        if (strcmp(ifname, dname) != 0)
                {
            continue;
        }

                if ((scope_tmp == scope) && (inet_ntop(AF_INET6, &ipv6, address, INET6_ADDRSTRLEN) != NULL))
                {
                        goto ret;
                }
    }

ret:
        fclose (f);
        return address;
}

char * ubox_ipv6_get_default_gateway(void)
{
    static char address[INET6_ADDRSTRLEN] = {0};
    FILE *f;
    char dest_addr[33] = {0};
    int plength = 0;
    unsigned char next_hop[16] = {0};
    char dname[MAX_IFNAME_STR_SIZE+1] = {0};

    f = fopen("/proc/net/ipv6_route", "r");
    if (f == NULL) {
        return address;
    }
        while (19 == fscanf(f,
                "%32s %x " //dest addr and plength
                "%*x %*x " // source addr and plength
                "%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx " // nexthop
                "%*x " //Metric
                "%*x " //Reference counter
                "%*x " //Use counter
                "%*x " //Flags
                "%s " // devname
                , dest_addr , &plength
                , &next_hop[0], &next_hop[1], &next_hop[2], &next_hop[3], &next_hop[4], &next_hop[5], &next_hop[6], &next_hop[7]
                , &next_hop[8], &next_hop[9], &next_hop[10], &next_hop[11], &next_hop[12], &next_hop[13], &next_hop[14], &next_hop[15]
                , dname))
        {
        if(!strcmp(dest_addr, "00000000000000000000000000000000") && plength == 0
            && !strcmp(dname, "br-lan")
        )
        {
            memset(address, 0, sizeof(address));
            inet_ntop(AF_INET6, next_hop, address, sizeof(address));
        }
        //DO NOT break to get last gateway
        }
        fclose(f);

        return address;
}

/** ***********************  END  ********************************************/
