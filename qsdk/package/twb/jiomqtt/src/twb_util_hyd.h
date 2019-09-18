#ifndef _UBOX_HYD_H_
#define _UBOX_HYD_H_

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
#include <ctype.h>
#include <sys/select.h>


/** -------------------------------------------------------------------------
                          INCLUDE HEADER FILES                             
  -------------------------------------------------------------------------*/

/** -------------------------------------------------------------------------
                          DEFINITIONS                             
  -------------------------------------------------------------------------*/
typedef struct _hyd_wifison_dev
{
    int idx;
    char mac[32];
    char pmac[32];
    /* ... */

} hyd_wifison_dev;
/** -------------------------------------------------------------------------
                          FUNCTIONS                             
  -------------------------------------------------------------------------*/
int ubox_hyd_info_capture(char *addr, int port, char *fmt, ...);
int ubox_get_qca_wifison_dev_num(int *dev, int need_renew);
///int ubox_get_qca_wifison_dev_topology(hyd_wifison_dev *son_dev, T_BOOL need_renew);
int ubox_get_qca_wifison_dev_topology(int dev_num, hyd_wifison_dev *son_dev, int need_renew);
int ubox_if_qca_wifison_cap_exsit(int need_renew);
int ubox_get_qca_wifison_re_wired_client( char *mac , int need_renew );
#endif

