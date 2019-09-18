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
 * @file ubox_socket.c
 * @brief ...
 * @author T&W Technology co., Ltd.
 * @bug No known bugs
 */

/*-------------------------------------------------------------------------*/
/*                        INCLUDE HEADER FILES                             */
/*-------------------------------------------------------------------------*/
#include <twb_util_hyd.h>

/*-------------------------------------------------------------------------*/
/*                           DEFINITIONS                                   */
/*-------------------------------------------------------------------------*/
#define FILE_HYD_READER "/tmp/jiomqtt-hyd-capture"
#define  CFG_WIFI_SON_HYD_IP "127.0.0.1"
#define CFG_WIFI_SON_HYD_PORT 7777
/*-------------------------------------------------------------------------*/
/*                           VARIABLES                                     */
/*-------------------------------------------------------------------------*/
#define LOGFILE     "/dev/console"
#define DBG_MSG(fmt, arg...) do { FILE *log_fp = fopen(LOGFILE, "w"); \
                                     fprintf(log_fp, "%s:%s:%d:" fmt "\n", __FILE__, __func__, __LINE__, ##arg); \
                                     fclose(log_fp); \
                                     } while(0)
/*-------------------------------------------------------------------------*/
/*                           FUNCTIONS                                     */
/*-------------------------------------------------------------------------*/
/*
* @param void
* * @return time_t
* */

time_t ubox_get_x_time(void)
{
    struct timeval tv;
    char str_buf[80];
    FILE *fp;
    tv.tv_sec=0;
    tv.tv_usec=0;

    fp = fopen("/proc/uptime", "r");
    if(fp == NULL) return 0;
    if (fgets(str_buf, sizeof(str_buf), fp) != NULL)
    {
        sscanf(str_buf,"%ld.%ld",&(tv.tv_sec),&(tv.tv_usec));
    }
    fclose(fp);

    return tv.tv_sec;

}

/**
 *  @brief twbox_util_delete_space
 *  @param char *srcstr
 *  @return void
 */
char *twbox_util_delete_space(char *srcstr)
{
    if (srcstr == NULL || *srcstr == '\0')
    {
        return srcstr;
    }

    int len = 0;
    char *p = srcstr;

    while (*p != '\0' && isspace(*p))
    {
        ++p;
        ++len;
    }

    memmove(srcstr, p, strlen(srcstr) - len + 1);

    return srcstr;
}

/**
* @brief dump the packet content
* @param ubox_socket_t *pkt
* @return T_BOOL
*/
int ubox_hyd_info_capture(char *addr, int port, char *fmt, ...)
{
    FILE *fp = NULL;
    int res, ret, result = -1;
    int sock = -1;
    int init = 0;
    int rec_len = 1;
    fd_set fds;
    struct sockaddr_in hyt;
    struct timeval ts;
    unsigned char buf[8];
    static char cmd[32];
    va_list ap;
    time_t start_time = ubox_get_x_time();
    int timeout = 5; //seconds

    va_start(ap, fmt);
    vsnprintf(cmd, 32, fmt, ap);
    va_end(ap);

    /*TODO file protection */
    if((fp = fopen(FILE_HYD_READER, "w")) == NULL )
    {
        return result; //error
    }

    /* create socket */
    if((sock = socket(AF_INET , SOCK_STREAM , 0)) < 0)
    {
        fclose(fp);
        return result; //error
    }

    hyt.sin_addr.s_addr = inet_addr(addr);
    hyt.sin_family = AF_INET;
    hyt.sin_port = htons(port);

    if(connect(sock , (struct sockaddr *)&hyt , sizeof(hyt)) < 0)
    {
        goto done;
    }

    ts.tv_sec = 1;
    ts.tv_usec = 0;

    while(ubox_get_x_time() < (start_time + timeout))
    {
        FD_ZERO(&fds);
        FD_SET(sock, &fds);

        res = select(sock + 1, &fds, (fd_set *) 0, (fd_set *) 0, &ts);
        if(res < 0)
        {
            result = -1;
            goto done; //error
        }
        else if(FD_ISSET(sock, &fds))
        {
            ret = recv(sock , buf , rec_len , 0);
            if(ret <= 0)
            {
                goto done; //error
            }
            else
            {
                buf[rec_len] = '\0';
                fflush(0);
            }
        }

        /* Wait utill hyt welcom message finished, symbol @ is the sign */
        if((buf[0] == '@') && init == 0)
        {
            init = 1;
            if(send(sock, cmd, strlen(cmd), 0) < 0)
            {
                goto done;//error
            }
        }
        /* End of the message */
        else if((buf[0] == '@') && init == 1)
        {
            result = 0; //success
            break;
        }

        /* Get hyd information */
        if(init == 1)
        {
            fprintf(fp, "%c", buf[0]);
        }
    } /* while*/

done:
    close(sock);
    fclose(fp);
    return result;
}

int ubox_get_qca_wifison_dev_num(int *dev, int need_renew)
{
    FILE *fp = NULL;
    char buf[256] = {0};
    char *line, *token;

    //if(need_renew && ubox_hyd_info_capture(CFG_WIFI_SON_HYD_IP, CFG_WIFI_SON_HYD_PORT, "%s %s%s", "td", "s2", "\n") != 0)
    if(need_renew)
    {
        system("(echo td s2;sleep 1)| hyt > /tmp/jiomqtt-hyd-capture");
    }

    if((fp = fopen(FILE_HYD_READER, "r")) == NULL)
    {
        return -1;
    }

    while(fgets(buf, 256, fp))
    {
        line = strtok(buf, "\n");
        if(!line)
        {
            continue;
        }

        /* Wi-Fi SON device number */
        if((token = strstr(line, "-- DB")) != NULL)
        {
            sscanf(token, "-- DB (%d entries):", dev);
        }
        //line = strtok(NULL, "\n");
    }
    fclose(fp);
    return 0;
}

int ubox_get_qca_wifison_dev_topology(int dev_num, hyd_wifison_dev *son_dev, int  need_renew)
{

    FILE *fp = NULL;
    char buf[128] = {0};
    char *line, *token;
    int i = 0;

    //if(need_renew && ubox_hyd_info_capture(CFG_WIFI_SON_HYD_IP, CFG_WIFI_SON_HYD_PORT, "%s %s%s", "td", "s2", "\n") != 0)
    if(need_renew)
    {
        system("(echo td s2;sleep 1)| hyt > /tmp/jiomqtt-hyd-capture");
    }

    if((fp = fopen(FILE_HYD_READER, "r")) == NULL)
    {
        return -1;
    }

    while(fgets(buf, 128, fp))
    {
        line = strtok(buf, "\n");
        if(!line)
        {
            continue;
        }
        /* Entry point for each WiFi-SON device */
        if((token = strstr(line, "QCA IEEE 1905.1 device:")) != NULL)
        {
            son_dev[i].idx = i+1;
            sscanf(token, "QCA IEEE 1905.1 device: %s", son_dev[i].mac);
            if(son_dev[i].mac[strlen(son_dev[i].mac)-1]==',')
            {
                son_dev[i].mac[strlen(son_dev[i].mac)-1] = '\0';
            }
        }

        if((token = strstr(line, "Upstream Device:")) != NULL)
        {
            sscanf(token, "Upstream Device: %s", son_dev[i].pmac);
            i++; /* next one */
        }
        if(dev_num == i)
        {
            break;
        }
    }
    fclose(fp);
    return 0;
}

int  ubox_if_qca_wifison_cap_exsit(int need_renew)
{

    FILE *fp = NULL;
    char buf[256] = {0};
    char *line, *token;

    //if(need_renew && ubox_hyd_info_capture(CFG_WIFI_SON_HYD_IP, CFG_WIFI_SON_HYD_PORT, "%s %s%s", "td", "s2", "\n") != 0)
    if(need_renew)
    {
        system("(echo td s2;sleep 1)| hyt > /tmp/jiomqtt-hyd-capture");
    }

    if((fp = fopen(FILE_HYD_READER, "r")) == NULL)
    {
        return -1;
    }

    while(fgets(buf, 256, fp))
    {
        line = strtok(buf, "\n");
        if(!line) {
            continue;
        }

        /* Find CAP */
        if(((token = strstr(line, "QCA IEEE 1905.1 device")) != NULL) && 
            ((token = strstr(line, "Network relaying device")) != NULL))
        {
            fclose(fp);
            return 0;
        }
    }
    fclose(fp);
    return -1;
}

int ubox_get_qca_wifison_re_wired_client( char *mac , int need_renew )
{
    FILE *fp = NULL;
    char buf[256] = {0};
    char *token;
    //if(need_renew && ubox_hyd_info_capture(CFG_WIFI_SON_HYD_IP, CFG_WIFI_SON_HYD_PORT, "%s %s%s", "td", "s2", "\n") != 0)
    if(need_renew)
    {
        system("(echo td s2;sleep 1)| hyt > /tmp/jiomqtt-hyd-capture");
    }

    if((fp = fopen(FILE_HYD_READER, "r")) == NULL)
    {
        return -1;
    }
    while(fgets(buf, 256, fp))
    {
        if((token = strstr(buf, "Interface eth0:")) != NULL)
        {
            while(fgets(buf, 256, fp)){
                if( '\n' == buf[0] ) {
                    fclose(fp);
                    return 0;
                }
                twbox_util_delete_space(buf);
                strcat(mac , buf);
            }
        }
    }
    fclose(fp);
    return -1;
}


/** ***********************  END  ********************************************/
