/*
 * netIPState.c
 *
 *
 *      Author: dmounday
 */


#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <netdb.h>
#include <syslog.h>
#include <string.h>

#include "../includes/sys.h"
#include "../includes/paramTree.h"
#include "../includes/rpc.h"
#include "../soapRpc/rpcMethods.h"
#include "../soapRpc/rpcUtils.h"
#include "../soapRpc/notify.h"
#include "../includes/CPEWrapper.h"
#include "../gslib/src/utils.h"
#include "../gslib/src/xmlParserSM.h"
#include "../gslib/src/xmlWriter.h"
#include "../gslib/src/www.h"
#include "../gslib/auxsrc/dns_lookup.h"
#include "targetsys.h"
#include "netIPState.h"
#include "../gslib/src/utils_hyd.h"

/*-------------------------------------------------------------------------*/
/*                           VARIABLES                                     */
/*-------------------------------------------------------------------------*/

extern  CWMPObject CWMP_RootObject[];
extern  CPEState cpeState;

EthernetLink *cpeNetIF;
int          netIFCnt=0;
InAddr       wanIP;
char         wanIPString[80];
const char   NAMESERVER[]="nameserver";

/* wifi client */

static int role;
static int client_wired = 0;
static int client_2g =0;
static int client_5g =0;
static int client_2g_backhaul =0;
static int client_5g_backhaul =0;



struct clients
{
    char mac[18];
    char tx[5];
    char rx[5];
    char rssi[4];
    char idle;
    char state;
};

struct clients clients_2g[256];
struct clients clients_5g[256];
struct clients clients_wired[256];
struct clients clients_2g_backhaul[20];
struct clients clients_5g_backhaul[20];




/*-------------------------------------------------------------------------*/
/*                           DEFINITIONS                                   */
/*-------------------------------------------------------------------------*/
#define IP       "ip"
#define NETSTAT  "/usr/sbin/netstat -nr"
#define MAC_FILE    "/tmp/mac_client"
#define SCAN_FILE   "/tmp/mac_scan"

#if 0
/*
 * This configuration uses the default route interface.
 */
static void getDNSServerList(void){
	FILE *f;
	static char  line[256];
	char	*p;
	int		t;
	char	*lasts;

	if ((f=fopen("/etc/resolv.conf", "r"))) {
		if ( cpeNetIF->dnsServers ){
			GS_FREE(cpeNetIF->dnsServers);
			cpeNetIF->dnsServers = NULL;
		}
		if ( (cpeNetIF->dnsServers = (char *)GS_MALLOC(257)) == NULL )
		{
			fclose(f);
			return;
		}
		t = 0;
		while ( (fgets( line, sizeof(line), f)!=NULL) ) {
			p = line;
			p = strtok_r(p, " \n", &lasts);
			if ( p && (strcmp(p, NAMESERVER)== 0) ){
				/* found nameserver line */
				p = strtok_r( NULL, " \n", &lasts);
				if ( p ){
					if ( t> 0){
						strcpy(cpeNetIF->dnsServers+t,",");
						t+=1;
					}
					strcpy(cpeNetIF->dnsServers+t, p);
					t += strlen(p);
				}
			}
		}
		fclose(f);
	}
	return;
}
#endif

/*
* CPE specific utility functions
*/
#if 0
static int getMAC(char *interface, NetIPState *nState) {
	FILE *fp;
	char	buf[512];
	//fprintf(stderr, "Start: " IFCONFIG );
	if ((fp = popen(IP " link show", "r")) == NULL) {
		cpeLog(LOG_ERR, "Could not start " IP " link show");
		// set result;
		return -1;
	}
	// read pipe for results of transfer...
	while ( fgets(buf, sizeof(buf), fp) != NULL ){

		if ( strstr(buf, interface)  ) {
			//now look for HWaddr
			char *p = strstr(buf, "HWaddr ");
			if ( p ){
				// found mac
				readMac(nState->mac, p+strlen("HWaddr "));
				pclose(fp);
				return 1;
			} else {
				// see if "ether" is on next line
				fgets(buf, sizeof(buf), fp);
				if ( (p = strstr(buf, "ether")) ){
					readMac(nState->mac, p+strlen("ether "));
					pclose(fp);
					return 1;
				}
			}
		}
	}
	pclose(fp);
	return 0;
}

/*
 * Look in IPv4 route table
 * return 1 if found and cpeNetIPState is initialized.
 *        0 default route not found.
 */
static int findIPv4DefaultRoute(void){
	FILE *f;
	char  line[256];
	char  iface[30];
	char  dest[30];
	unsigned  gw;
	unsigned flags, refcnt, use, metric, mask, window, mtu, irtt;
	char gwStr[30];
	char flagStr[20];

	int		r = 0;

	cpeNetIPState.name[0]='\0';
	if ((f=fopen("/proc/net/route", "r"))) {
		while ( fgets(line, sizeof(line), f)!= NULL ) {
			/* iface destination gateway flags refcnt use metric mask mtu window irtt*/
			if ( sscanf(line, "%s %s %x %x %x %x %x %x %x %x %x",
						iface, dest, &gw, &flags, &refcnt, &use, &metric, &mask, &mtu, &window, &irtt)
				 == 11 ) {
				if (streq(dest,"00000000")) {
					struct in_addr ina = {gw};
					SET_InADDR( &cpeNetIPState.gw, AF_INET, &ina);
					strcpy(cpeNetIPState.name, iface);
					r = 1; /* result */
					break;
				}
			}
		}
		fclose(f);
	} else if ((f = popen(NETSTAT, "r"))) {
		// read pipe for results of transfer...
		while ( fgets(line, sizeof(line), f) != NULL ){
			if ( strstr(line, "default")  ) {
				if ( sscanf(line, "%s %s %s %d %d %s", dest, gwStr, flagStr, &refcnt, &use, iface) == 6) {
					readInIPAddr( &cpeNetIPState.gw, gwStr);
					strcpy(cpeNetIPState.name, iface);
					r = 1; /* result */
					break;
				}

			}
		}
		pclose(f);
	} else
		cpeLog(LOG_ERR, "Could not start " NETSTAT);
	return r;
}


/*
 * change IPv6 hex addr string into colon seperated string.
 */
static char *strToIPv6Addr( char *hexstr){
	static char addrstr[INET6_ADDRSTRLEN];
	int	i, j;

	if ( strlen(hexstr)!= 32)
		return NULL;
	for ( i=j=0; j<32; ++j){
		if ((j>0) && ((j%4) == 0))
			addrstr[i++]=':';
		addrstr[i++] = hexstr[j];
	}
	addrstr[i]='\0';
	return addrstr;
}
#endif
/*
 * isolate IP address string.
 * p points to string from 'ip addr show' that is of more 192.168.3.8/24
 *   or fe80::21e:4fff:fef5:751a/64
 */
static int maskSize;
static const char *getIPStr( const char *p ){
	static char s[INET6_ADDRSTRLEN];
	char *e = strchr(p, '/');
	if (e){
		strncpy(s, p, e-p);
		s[e-p] = '\0';
		maskSize = atoi(e+1);
		return s;
	}
	return NULL;
}
/*
 * Generate a dotted IPv4 subnet address string.
 */
static const char *getSubnetMask(int mask){
	int i;
	static char s[INET6_ADDRSTRLEN];
	unsigned int x=0x80000000;
	for (i=1; i<mask; ++i){
		x = x>>1;
		x |= 0x80000000;
	}
	snprintf(s, INET6_ADDRSTRLEN, "%d.%d.%d.%d",
			(x>>24)&0xff, (x>>16)&0xff, (x>>8)&0xff, x&0xff);
	return s;
}

static void createInterfaceStackEntry(Instance *lower, Instance *higher ){
	char oName[257];
	snprintf(oName, sizeof(oName), "%s.InterfaceStack.%d.", CWMP_RootObject[0].name, cpeGetInstanceID());
	cpeLog(LOG_DEBUG, "createInterfaceEntry: %s", oName);
	Instance *ifsp = cwmpInitObjectInstance(oName);
	if ( ifsp ){
		InterfaceStack *p = (InterfaceStack *)ifsp->cpeData;
		p->higherLayer = higher;
		p->lowerLayer = lower;
	}
}

void refreshAssociatedDeviceinstances(void)
{
    int i = 0;
    int j = 0;
    int ret;
    char *pos = NULL;
    char line_buf[256];
    char cmd[256];
    char cmd_result[256];
    char oName[257];
    Instance *associateddevice;
    Instance *associated;
    CWMPObject *o;
    FILE *fp = NULL;

    memset(&clients_2g , 0x0 , sizeof(clients_2g));
    memset(&clients_5g , 0x0 , sizeof(clients_5g));
    memset(&clients_wired , 0x0 , sizeof(clients_wired));
    memset(&clients_2g_backhaul , 0x0 , sizeof(clients_2g_backhaul));
    memset(&clients_5g_backhaul , 0x0 , sizeof(clients_5g_backhaul));
    memset(cmd,0x0,sizeof(cmd));
    memset(cmd_result,0x0,sizeof(cmd_result));


/* Clean all client exist instance first */
    for (i=1; i<=4 ; i++)
    {
        for (j=1; j<= ( client_2g + client_5g + client_2g_backhaul + client_5g_backhaul); j++)
        {
            memset(oName , 0x0 , sizeof(oName));
            snprintf(oName, sizeof(oName), "%s.WiFi.AccessPoint.%d.AssociatedDevice.%d.", CWMP_RootObject[0].name, i , j);
            o = cwmpFindObject(oName);
            if (o)
            {
               // DBG_MSG("clean");
                cwmpDeleteAllInstances(o);
            }
        }
    }

/* Clean all client wired with AccessPoint.5 object*/
    Instance *accesspoint;
    memset(oName , 0x0 , sizeof(oName));
    snprintf(oName, sizeof(oName), "%s.WiFi.AccessPoint.%d.", CWMP_RootObject[0].name, 5);
    o = cwmpFindObject(oName);
    if (o)
    {
       // DBG_MSG("clean");
        accesspoint = cwmpGetCurrentInstance();
        cwmpDeleteObjectInstance( o, accesspoint);
    }

    role = get_role();
/*  For Client mac , tx , rx ,rssi , state */

    if (!strncmp(get_topology_iface_name(role,0),"-1",2))
    {
        client_2g =0;
        i=0;
    }
    else
    {
        sprintf(cmd, "rm -rf %s && wlanconfig ath%s list | grep Minimum |  awk '{print $1 \" \" $4 \" \" $5 \" \" $6 \" \" $9 \" \" $16}' | tr -d \"M\"  > %s" , MAC_FILE , get_topology_iface_name(role,0) , MAC_FILE);
        cmd_popen(cmd, cmd_result);

        fp = fopen(MAC_FILE, "r");
        if(fp == NULL)
            DBG_MSG("Cannot read file, %s\n", MAC_FILE );

        memset(cmd,0x0,sizeof(cmd));
        i=0;
        while (fgets(line_buf, sizeof(line_buf), fp))
        {
            sscanf( line_buf , "%s %s %s %s %c %[^\n]" , clients_2g[i].mac , clients_2g[i].tx , clients_2g[i].rx , clients_2g[i].rssi , &clients_2g[i].idle , &clients_2g[i].state  );
            for(j = 0 ; j < strlen(clients_2g[i].mac)+1 ; j++ )
            {
                cmd[j] = toupper(clients_2g[i].mac[j]);
            }
            snprintf(clients_2g[i].mac , strlen(clients_2g[i].mac)+1 , cmd);
            //DBG_MSG ("%s %s %s %s", clients_2g[i].mac , clients_2g[i].tx , clients_2g[i].rx , clients_2g[i].rssi );
            i++;
        }
        fclose(fp);
        client_2g = i;
        i=0;
    }

    if (!strncmp(get_topology_iface_name(role,1),"-1",2))
    {
        client_2g_backhaul = 0;
        i=0;
    }
    else
    {
        memset(cmd,0x0,sizeof(cmd));
        memset(cmd_result,0x0,sizeof(cmd_result));
        sprintf(cmd, "rm -rf %s && wlanconfig ath%s list | grep Minimum |  awk '{print $1 \" \" $4 \" \" $5 \" \" $6 \" \" $9 \" \" $16}' | tr -d \"M\"  > %s" , MAC_FILE , get_topology_iface_name(role,1) , MAC_FILE);
        cmd_popen(cmd, cmd_result);

        fp = fopen(MAC_FILE, "r");
        if(fp == NULL)
        {
            DBG_MSG("Cannot read file, %s\n", MAC_FILE );
            return ;
        }

        memset(cmd,0x0,sizeof(cmd));
        memset(line_buf,0x0,sizeof(line_buf));
        while (fgets(line_buf, sizeof(line_buf), fp))
        {
            sscanf( line_buf , "%s %s %s %s %c %[^\n]" , clients_2g_backhaul[i].mac , clients_2g_backhaul[i].tx , clients_2g_backhaul[i].rx , clients_2g_backhaul[i].rssi , &clients_2g_backhaul[i].idle , &clients_2g_backhaul[i].state  );
            for(j = 0 ; j < strlen(clients_2g_backhaul[i].mac)+1 ; j++ )
            {
                cmd[j] = toupper(clients_2g_backhaul[i].mac[j]);
            }
            snprintf(clients_2g_backhaul[i].mac , strlen(clients_2g_backhaul[i].mac)+1 , cmd);
            //DBG_MSG ("%s %s %s %s", clients_2g[i].mac , clients_2g[i].tx , clients_2g[i].rx , clients_2g[i].rssi );
            i++;
        }
        fclose(fp);
        client_2g_backhaul = i;
        i=0;
    }

    if (!strncmp(get_topology_iface_name(role,2),"-1",2))
    {
        client_5g = 0;
        i=0;
    }
    else
    {
        memset(cmd,0x0,sizeof(cmd));
        memset(cmd_result,0x0,sizeof(cmd_result));
        sprintf(cmd, "rm -rf %s && wlanconfig ath%s list | grep Minimum |  awk '{print $1 \" \" $4 \" \" $5 \" \" $6 \" \" $9 \" \" $16}' | tr -d \"M\"  > %s" , MAC_FILE , get_topology_iface_name(role,2) , MAC_FILE);
        cmd_popen(cmd, cmd_result);

        fp = fopen(MAC_FILE, "r");
        if(fp == NULL)
        {
            DBG_MSG("Cannot read file, %s\n", MAC_FILE );
            return ;
        }

        memset(cmd,0x0,sizeof(cmd));
        memset(line_buf,0x0,sizeof(line_buf));
        while (fgets(line_buf, sizeof(line_buf), fp))
        {
            sscanf( line_buf , "%s %s %s %s %c %[^\n]" , clients_5g[i].mac , clients_5g[i].tx , clients_5g[i].rx , clients_5g[i].rssi , &clients_5g[i].idle , &clients_5g[i].state );
            //DBG_MSG ("%s", clients_5g[i].mac  );
            for(j = 0 ; j < strlen(clients_5g[i].mac)+1 ; j++ )
            {
                cmd[j] = toupper(clients_5g[i].mac[j]);
            }
            snprintf(clients_5g[i].mac , strlen(clients_5g[i].mac)+1 , cmd);
            //DBG_MSG ("%s %s %s %s", clients_5g[i].mac , clients_5g[i].tx , clients_5g[i].rx , clients_5g[i].rssi );
            i++;
        }
        fclose(fp);
        client_5g = i;
        i=0;
    }

    if (!strncmp(get_topology_iface_name(role,3),"-1",2))
    {
        client_5g_backhaul = 0;
        i=0;
    }
    else
    {
        memset(cmd,0x0,sizeof(cmd));
        memset(cmd_result,0x0,sizeof(cmd_result));
        sprintf(cmd, "rm -rf %s && wlanconfig ath%s list | grep Minimum |  awk '{print $1 \" \" $4 \" \" $5 \" \" $6 \" \" $9 \" \" $16}' | tr -d \"M\"  > %s" , MAC_FILE , get_topology_iface_name(role,3) , MAC_FILE);
        cmd_popen(cmd, cmd_result);

        fp = fopen(MAC_FILE, "r");
        if(fp == NULL)
        {
            DBG_MSG("Cannot read file, %s\n", MAC_FILE );
            return ;
        }

        memset(cmd,0x0,sizeof(cmd));
        memset(line_buf,0x0,sizeof(line_buf));
        while (fgets(line_buf, sizeof(line_buf), fp))
        {
            sscanf( line_buf , "%s %s %s %s %c %[^\n]" , clients_5g_backhaul[i].mac , clients_5g_backhaul[i].tx , clients_5g_backhaul[i].rx , clients_5g_backhaul[i].rssi , &clients_5g_backhaul[i].idle , &clients_5g_backhaul[i].state );
            //DBG_MSG ("%s", clients_5g[i].mac  );
            for(j = 0 ; j < strlen(clients_5g_backhaul[i].mac)+1 ; j++ )
            {
                cmd[j] = toupper(clients_5g_backhaul[i].mac[j]);
            }
            snprintf(clients_5g_backhaul[i].mac , strlen(clients_5g_backhaul[i].mac)+1 , cmd);
            //DBG_MSG ("%s %s %s %s", clients_5g[i].mac , clients_5g[i].tx , clients_5g[i].rx , clients_5g[i].rssi );
            i++;
        }
        fclose(fp);
        client_5g_backhaul = i;
        i=0;
    }



/*  For RE wired client */
    if ((ret = ubox_get_qca_wifison_re_wired_client ( cmd_result , 1 )) !=0)
    {
        DBG_MSG("No RE Wired Client");
        client_wired =0;
        i=0;
        //return ;
    }
    else
    {

        memset(oName , 0x0 , sizeof(oName));
        snprintf(oName, sizeof(oName), "%s.WiFi.AccessPoint.%d.", CWMP_RootObject[0].name, 5 );
        o = cwmpFindObject(oName);

        if ( NULL == o)
        {
           cwmpInitObjectInstance(oName);
           cpeLog(LOG_DEBUG, "createInterfaceEntry: %s", oName);
        }

        memset( cmd_result , 0x0 , sizeof(cmd_result));
        if ((ret = ubox_get_qca_wifison_re_wired_client ( cmd_result , 1 )) !=0)
        {
            DBG_MSG("No RE Wired Client");
            //return ;
        }
        i=0;
        pos = strtok(cmd_result,"\n");
        while (pos != NULL)
        {
            sprintf(clients_wired[i].mac, pos);
            pos = strtok(NULL,"\n");
            i++;
        }
        client_wired = i;
        i=0;
    }

    for (i=1,j=1; j<= client_2g; j++)
    {
        memset(oName , 0x0 , sizeof(oName));
        snprintf(oName, sizeof(oName), "%s.WiFi.AccessPoint.%d.AssociatedDevice.%d.", CWMP_RootObject[0].name, i , j);
        associateddevice = cwmpInitObjectInstance(oName);
        cpeLog(LOG_DEBUG, "createInterfaceEntry: %s", oName);
        //DBG_MSG("%s" , oName );
        if (associateddevice)
        {
            WiFiAccessPointAssociatedDevice *eassociateddevice = (WiFiAccessPointAssociatedDevice *)associateddevice->cpeData;
            //DBG_MSG("%s" , clients_2g[j-1].mac);
            if ( NULL != clients_2g[j-1].mac && '\0' != clients_2g[j-1].state && NULL != clients_2g[j-1].rx && NULL != clients_2g[j-1].tx && '\0' != clients_2g[j-1].idle &&  NULL != clients_2g[j-1].rssi )
            {
                COPYSTR(eassociateddevice->mACAddress, clients_2g[j-1].mac);
                eassociateddevice->authenticationState = (clients_2g[j-1].state == '1') ? 1:0;
                eassociateddevice->lastDataDownlinkRate = atoi(clients_2g[j-1].rx);
                eassociateddevice->lastDataUplinkRate = atoi(clients_2g[j-1].tx);
                eassociateddevice->active = (clients_2g[j-1].idle == '1') ? 1:0;
                eassociateddevice->signalStrength = clients_2g[j-1].rssi;
            }
        }
    }

    for (i=2,j=1; j<= client_2g_backhaul; j++)
    {
        memset(oName , 0x0 , sizeof(oName));
        snprintf(oName, sizeof(oName), "%s.WiFi.AccessPoint.%d.AssociatedDevice.%d.", CWMP_RootObject[0].name, i , j);
        associateddevice = cwmpInitObjectInstance(oName);
        cpeLog(LOG_DEBUG, "createInterfaceEntry: %s", oName);
        //DBG_MSG("%s" , oName );
        if (associateddevice)
        {
            WiFiAccessPointAssociatedDevice *eassociateddevice = (WiFiAccessPointAssociatedDevice *)associateddevice->cpeData;
            //DBG_MSG("%s" , clients_2g[j-1].mac);
            if ( NULL != clients_2g_backhaul[j-1].mac && '\0' != clients_2g_backhaul[j-1].state && NULL != clients_2g_backhaul[j-1].rx && NULL != clients_2g_backhaul[j-1].tx && '\0' != clients_2g_backhaul[j-1].idle &&  NULL != clients_2g_backhaul[j-1].rssi )
            {
                COPYSTR(eassociateddevice->mACAddress, clients_2g_backhaul[j-1].mac);
                eassociateddevice->authenticationState = (clients_2g_backhaul[j-1].state == '1') ? 1:0;
                eassociateddevice->lastDataDownlinkRate = atoi(clients_2g_backhaul[j-1].rx);
                eassociateddevice->lastDataUplinkRate = atoi(clients_2g_backhaul[j-1].tx);
                eassociateddevice->active = (clients_2g_backhaul[j-1].idle == '1') ? 1:0;
                eassociateddevice->signalStrength = clients_2g_backhaul[j-1].rssi;
            }
        }
    }



    for (i=3,j=1; j <= client_5g; j++)
    {
        memset(oName , 0x0 , sizeof(oName));
        snprintf(oName, sizeof(oName), "%s.WiFi.AccessPoint.%d.AssociatedDevice.%d.", CWMP_RootObject[0].name, i , j);
        associateddevice = cwmpInitObjectInstance(oName);
        cpeLog(LOG_DEBUG, "createInterfaceEntry: %s", oName);
        //DBG_MSG("%s" , oName );
        if (associateddevice)
        {
            WiFiAccessPointAssociatedDevice *eassociateddevice = (WiFiAccessPointAssociatedDevice *)associateddevice->cpeData;
            //DBG_MSG("%s" , clients_5g[j-1 ].mac);
            if ( NULL != clients_5g[j-1].mac && '\0' != clients_5g[j-1].state && NULL != clients_5g[j-1].rx && NULL != clients_5g[j-1].tx && '\0' != clients_5g[j-1].idle &&  NULL != clients_5g[j-1].rssi )
            {
                COPYSTR(eassociateddevice->mACAddress, clients_5g[j-1].mac);
                eassociateddevice->authenticationState = (clients_5g[j-1].state == '1') ? 1:0;
                eassociateddevice->lastDataDownlinkRate = atoi(clients_5g[j-1].rx);
                eassociateddevice->lastDataUplinkRate = atoi(clients_5g[j-1].tx);
                eassociateddevice->active = (clients_5g[j-1].idle == '1') ? 1:0;
                COPYSTR(eassociateddevice->signalStrength , clients_5g[j-1].rssi);
            }
        }
    }

    for (i=4,j=1; j <= client_5g_backhaul; j++)
    {
         memset(oName , 0x0 , sizeof(oName));
         snprintf(oName, sizeof(oName), "%s.WiFi.AccessPoint.%d.AssociatedDevice.%d.", CWMP_RootObject[0].name, i , j);
         associateddevice = cwmpInitObjectInstance(oName);
         cpeLog(LOG_DEBUG, "createInterfaceEntry: %s", oName);
         //DBG_MSG("%s" , oName );
        if (associateddevice)
        {
            WiFiAccessPointAssociatedDevice *eassociateddevice = (WiFiAccessPointAssociatedDevice *)associateddevice->cpeData;
            //DBG_MSG("%s" , clients_5g[j-1 ].mac);
            if ( NULL != clients_5g_backhaul[j-1].mac && '\0' != clients_5g_backhaul[j-1].state && NULL != clients_5g_backhaul[j-1].rx && NULL != clients_5g_backhaul[j-1].tx && '\0' != clients_5g_backhaul[j-1].idle &&  NULL != clients_5g_backhaul[j-1].rssi )
            {
                COPYSTR(eassociateddevice->mACAddress, clients_5g_backhaul[j-1].mac);
                eassociateddevice->authenticationState = (clients_5g_backhaul[j-1].state == '1') ? 1:0;
                eassociateddevice->lastDataDownlinkRate = atoi(clients_5g_backhaul[j-1].rx);
                eassociateddevice->lastDataUplinkRate = atoi(clients_5g_backhaul[j-1].tx);
                eassociateddevice->active = (clients_5g_backhaul[j-1].idle == '1') ? 1:0;
                COPYSTR(eassociateddevice->signalStrength , clients_5g_backhaul[j-1].rssi);
            }
        }
    }


    for (i=5,j=1; j <= client_wired ; j++)
    {
        memset(oName , 0x0 , sizeof(oName));
        snprintf(oName, sizeof(oName), "%s.WiFi.AccessPoint.%d.AssociatedDevice.%d.", CWMP_RootObject[0].name, i , j);
        associateddevice = cwmpInitObjectInstance(oName);
        cpeLog(LOG_DEBUG, "createInterfaceEntry: %s", oName);
        //DBG_MSG("%s" , oName );
        if (associateddevice)
        {
            WiFiAccessPointAssociatedDevice *eassociateddevice = (WiFiAccessPointAssociatedDevice *)associateddevice->cpeData;
            //DBG_MSG("%s" , clients_wired[j-1].mac);
            if (NULL != clients_wired[ j-1].mac )
                COPYSTR(eassociateddevice->mACAddress, clients_wired[ j-1].mac);
        }
    }

    for (i = 1; i<=5 ; i++)
    {
        memset(oName , 0x0 , sizeof(oName));
        snprintf(oName, sizeof(oName), "%s.WiFi.AccessPoint.%d.", CWMP_RootObject[0].name, i);
        o = cwmpFindObject(oName);
        if (o)
        {
            associated = cwmpGetCurrentInstance();
            WiFiAccessPoint *eassociated = (WiFiAccessPoint *)associated->cpeData;
            if (i == 1)
                eassociated->associatedDeviceNumberOfEntries =  client_2g;
            else if (i ==2 )
                eassociated->associatedDeviceNumberOfEntries =  client_2g_backhaul;
            else if (i == 3)
                eassociated->associatedDeviceNumberOfEntries =  client_5g;
            else if (i == 4)
                eassociated->associatedDeviceNumberOfEntries =  client_5g_backhaul;
            else if (i == 5)
                eassociated->associatedDeviceNumberOfEntries =  client_wired;
        }
    }

}
/*
 * * Refresh refreshDeviceMemory interface information.
*/

static void refreshDeviceMemory(void) {
    char oName[257];
    CWMPObject *o;

/* Create VendorLogFile */
    memset(oName , 0x0 , sizeof(oName));
    snprintf(oName, sizeof(oName), "%s.DeviceInfo.VendorLogFile.%d.", CWMP_RootObject[0].name, 1);
    o = cwmpFindObject(oName);
    if ( NULL == o)
    {
        cwmpInitObjectInstance(oName);
        cpeLog(LOG_DEBUG, "createInterfaceEntry: %s", oName);
    }

/* Create Device.Time */
    memset(oName , 0x0 , sizeof(oName));
    snprintf(oName, sizeof(oName), "%s.DeviceInfo.Time.", CWMP_RootObject[0].name);
    o = cwmpFindObject(oName);
    if ( NULL == o)
    {
        cwmpInitObjectInstance(oName);
        cpeLog(LOG_DEBUG, "createInterfaceEntry: %s", oName);
    }


    memset(oName , 0x0 , sizeof(oName));
    snprintf(oName, sizeof(oName), "%s.DeviceInfo.ProcessStatus.", CWMP_RootObject[0].name);
    o = cwmpFindObject(oName);
    if ( NULL == o)
    {
        cwmpInitObjectInstance(oName);
        cpeLog(LOG_DEBUG, "createInterfaceEntry: %s", oName);
    }

    memset(oName , 0x0 , sizeof(oName));
    snprintf(oName, sizeof(oName), "%s.DeviceInfo.MemoryStatus.", CWMP_RootObject[0].name);
    o = cwmpFindObject(oName);
    if ( NULL == o)
    {
        cwmpInitObjectInstance(oName);
        cpeLog(LOG_DEBUG, "createInterfaceEntry: %s", oName);
    }

}

/*
 * * Refresh refreshWiFinstances interface information.
*/

static void refreshWiFinstances(int discovery) {

    int i = 1;
    char oName[257];
    CWMPObject *o;


    for (i=1; i <= 4 ; i++)
    {
        memset(oName , 0x0 , sizeof(oName));
        snprintf(oName, sizeof(oName), "%s.WiFi.SSID.%d.", CWMP_RootObject[0].name, i );
        o = cwmpFindObject(oName);
        if ( NULL == o)
        {
            cwmpInitObjectInstance(oName);
            cpeLog(LOG_DEBUG, "createInterfaceEntry: %s", oName);
        }
        memset(oName , 0x0 , sizeof(oName));
        snprintf(oName, sizeof(oName), "%s.WiFi.SSID.%d.Stats.0.", CWMP_RootObject[0].name, i );
        o = cwmpFindObject(oName);
        if ( NULL == o)
        {
            cwmpInitObjectInstance(oName);
            cpeLog(LOG_DEBUG, "createInterfaceEntry: %s", oName);
        }
    }

    for(i=1; i <= 4 ; i++)
    {
        memset(oName , 0x0 , sizeof(oName));
        snprintf(oName, sizeof(oName), "%s.WiFi.AccessPoint.%d.", CWMP_RootObject[0].name, i );
        o = cwmpFindObject(oName);
        if ( NULL == o)
        {
            cwmpInitObjectInstance(oName);
            cpeLog(LOG_DEBUG, "createInterfaceEntry: %s", oName);
        }

        memset(oName , 0x0 , sizeof(oName));
        snprintf(oName, sizeof(oName), "%s.WiFi.AccessPoint.%d.Security.0.", CWMP_RootObject[0].name, i);
        o = cwmpFindObject(oName);
        if ( NULL == o)
        { 
            cwmpInitObjectInstance(oName);
            cpeLog(LOG_DEBUG, "createInterfaceEntry: %s", oName);
        }
        memset(oName , 0x0 , sizeof(oName));
        snprintf(oName, sizeof(oName), "%s.WiFi.AccessPoint.%d.WPS.0.", CWMP_RootObject[0].name, i);
        o = cwmpFindObject(oName);
        if ( NULL == o)
        {
            cwmpInitObjectInstance(oName);
            cpeLog(LOG_DEBUG, "createInterfaceEntry: %s", oName);
        }
    }

    for (i=1; i <= 2 ; i ++)
    {
        memset(oName , 0x0 , sizeof(oName));
        snprintf(oName, sizeof(oName), "%s.WiFi.Radio.%d.", CWMP_RootObject[0].name, i );
        o = cwmpFindObject(oName);
        if ( NULL == o)
        {
            cwmpInitObjectInstance(oName);
            cpeLog(LOG_DEBUG, "createInterfaceEntry: %s", oName);
        }
        memset(oName , 0x0 , sizeof(oName));
        snprintf(oName, sizeof(oName), "%s.WiFi.Radio.%d.Stats.0.", CWMP_RootObject[0].name, i );
        o = cwmpFindObject(oName);
        if ( NULL == o)
        {
            cwmpInitObjectInstance(oName);
            cpeLog(LOG_DEBUG, "createInterfaceEntry: %s", oName);
        }
    }

    memset(oName , 0x0 , sizeof(oName));
    snprintf(oName, sizeof(oName), "%s.X_RJIL_MeshNode.", CWMP_RootObject[0].name);
    o = cwmpFindObject(oName);
    if ( NULL == o)
    {
        cwmpInitObjectInstance(oName);
        cpeLog(LOG_DEBUG, "createInterfaceEntry: %s", oName);
    }

}
/*
 * Refresh Network interface information.
 * Read Interface list using "ip addr show" to get interfaces and IPv4 and IPv6
 * addresses. This is called both for initial discovery of the hardware interfaces
 * and to update the associated parameters.
 * 	int refreshNetworkInstances( int discover )
 *		discover: if 1 then attempt to discover the number
 *		of interfaces.
 * Return number of interfaces.
 *       -1: unable to run "ip addr show" utility.
 */
extern char *defaultNetworkIF;

static int refreshNetworkInstances(int discovery) {
	FILE *fp;
	int		ifCnt=0;
	char	buf[257];
	char 	oName[257];
	char	ifname[80];
	char	ifstate[20];
	u_char	mac[6];
	int		id;
	int		ipcnt;
	int     ipcnt6;

	Instance *eifip ={0};              // .Ether
	Instance *lkip ={0};
	Instance *ipif = {0};
	IPInterface *iip = {0};

	if ((fp = popen(IP " addr show", "r")) == NULL) {
		cpeLog(LOG_ERR, "Could not start " IP " addr show");
		// set result;
		return -1;
	}
	// read pipe for results of ip addr show ...
	if ( fgets(buf, sizeof(buf), fp) != NULL )
		do {
			// scan lines for "1:", "2:", ...
			if ( isdigit(buf[0])) {
				// found I/F name
				int 	inet;
				char 	*e;
				char	*p;
                memset(ifname,0x0 ,sizeof(ifname));
#if 0
				id = atoi(buf);			// The interface numbers are used for object instance numbers
										// which tie the instances to a specific interface.
#else
                id = 1;
#endif
				p = strchr(buf, ' '); // scan for space
				if ( p ){
					++p;
					e = strchr(p, ':');
					strncpy( ifname, p, e-p);
					ifname[e-p]= '\0';
					// more info on this line if needed.
					p = strstr(p, "state ");
					if ( p ){
						p+= strlen("state ");
						if ( strstr(p, "DOWN "))
							strcpy(ifstate, "Down");
						else if (strstr( p, "UP "))
							strcpy(ifstate, "Up");
						else
							strcpy(ifstate, "Down");
					}
				}
				if ( fgets(buf, sizeof(buf), fp)!=NULL ){
					// this is link/ether line. Expects the following format:
					// link/ether 00:1e:4f:f5:75:1a brd ff:ff:ff:ff:ff:ff
					p = strstr(buf, "link/ether");
					if ( p  && !strncmp( ifname, "br-lan", 6) ) {
						readMac(mac, p + strlen("link/ether "));
						snprintf(oName, sizeof(oName), "%s.Ethernet.Interface.%d.", CWMP_RootObject[0].name, id );
						// The pAddObj function of the .Interface. object is used to allocate and initialize the cpeData.
						// In this case the cpeData is the EthernetInterface structure.
						// If the instance is already present the Instance pointer is returned.
                        eifip = cwmpInitObjectInstance(oName);
						if ( eifip ){
							EthernetInterface *eip = (EthernetInterface *)eifip->cpeData;
                            if (NULL != ifname && NULL != ifstate && NULL != writeMac(mac) )
                            {
                                COPYSTR(eip->name, ifname);
                                COPYSTR(eip->status, ifstate);
                                COPYSTR(eip->mACAddress, writeMac(mac));
                            }
							eip->enable = streq(ifstate, "Up")? 1: 0;
						}
						// init Ethernet.Link This can be any thing that can carry an ethernet frame, but
						// the quick-start code limits it to Ethernet interfaces.
						snprintf(oName, sizeof(oName), "%s.Ethernet.Link.%d.", CWMP_RootObject[0].name, id );
                        lkip = cwmpInitObjectInstance(oName);
						if ( lkip ){
							// Default WAN interface
							EthernetLink *lp = (EthernetLink *)lkip->cpeData;
							if ( discovery ) {
								lp->lowerLayers[0] = eifip;
								// first time called: add InterfaceStack EthernetInterface.1. <--> Ethernet.Link.1.
								// The InterfaceStack instance is only created on the discovery call to refreshNetworkInstances.
								// Otherwise; the restored configuration is used. The RPC_R parameters of the InterfaceStack
								// are restored from the configuration using the setXXX parameter call backs.
#if 0
								createInterfaceStackEntry(eifip, lkip);
#endif
							}
                            if ( NULL != ifname && NULL != ifstate && NULL != writeMac(mac) )
                            {
                                COPYSTR(lp->name, ifname);
                                COPYSTR(lp->status, ifstate);
                                COPYSTR(lp->mACAddress, writeMac(mac));
                            }
							lp->enable = streq(ifstate, "Up")? 1: 0;
							// init IP Interface on each Ethernet.Link. May not have any IP addresses.
							snprintf(oName, sizeof(oName), "%s.IP.Interface.%d.", CWMP_RootObject[0].name, id );
                            ipif = cwmpInitObjectInstance(oName);
							if ( ipif ){
								iip = (IPInterface *)ipif->cpeData;
								if (discovery){
									iip->lowerLayers[0] = lkip;
#if 0
									createInterfaceStackEntry(lkip, ipif);
#endif
								}
								iip->enable = 1;
                                if ( NULL != ifstate && NULL != ifname)
                                {
                                    COPYSTR( iip->status, ifstate);
                                    COPYSTR( iip->name, ifname);
                                }
                                COPYSTR( iip->type, "Normal");
							}
							//
						}
						ipcnt = 0;
						ipcnt6 = 0;
                        do { // look for inet and inet6 records (inet6 has 2 lines).
                        	Instance *ipip ={0};
                        	if ( fgets(buf, sizeof(buf), fp)!=NULL )
                        	{
                        		p = strstr(buf, "inet6 ");
                        		if ( (inet = p!=NULL) )
                        		{
                        			++ipcnt6;
                        			iip->iPv6Enable = 1;
                        			p += strlen("inet6 ");
                        			snprintf(oName, sizeof(oName), "%s.IP.Interface.%d.IPv6Address.%d.",CWMP_RootObject[0].name, id, ipcnt6);
                                    ipip = cwmpInitObjectInstance(oName);
                        			if ( ipip )
                        			{
                        				IPInterfaceIPv6Address *ipdata= (IPInterfaceIPv6Address *)ipip->cpeData;
                        				COPYSTR( ipdata->status, "Enabled");
                                        if (!strncmp(ifname , "br-lan" , 6))
                                        {
                                            COPYSTR( ipdata->iPAddressStatus, "Preferred");
                                        }
                                        else
                                        {
                                            COPYSTR( ipdata->iPAddressStatus, "Inaccessible");
                                        }
                                        COPYSTR( ipdata->iPAddress, getIPStr(p) );
                        				ipdata->enable = 1;
                        			}
                        			fgets(buf, sizeof(buf), fp); // valid_lft forever ...
                        		} 
                        		else if ( (inet = (p = strstr(buf, "inet "))!=NULL))
                        		{
                        			p += strlen("inet ");
                        			++ipcnt;
                        			iip->iPv4Enable = 1;
                        			snprintf(oName, sizeof(oName), "%s.IP.Interface.%d.IPv4Address.%d.",CWMP_RootObject[0].name, id, ipcnt);
                                    ipip = cwmpInitObjectInstance(oName);
                        			if ( ipip )
                        			{
                        				IPInterfaceIPv4Address *ipdata= (IPInterfaceIPv4Address *)ipip->cpeData;
                                        if(NULL != getIPStr(p) && NULL !=  getSubnetMask(maskSize) )
                                        {
                                            COPYSTR( ipdata->iPAddress, getIPStr(p) );
                                            COPYSTR( ipdata->subnetMask, getSubnetMask(maskSize));
                                        }
                        				COPYSTR( ipdata->status, "Enabled");
                        				ipdata->enable = 1;
                        			}
                        			fgets(buf, sizeof(buf), fp); // valid_lft forever ...
                        		}
                        	} 
                        	else {
                        		inet = 0;
                        	}
                        	// read until get the "valid_..." line.
                        } while ( inet );
					} 
					else 
					{
						// flush to next interface record
						do {
							if ( fgets(buf, sizeof(buf), fp)==NULL)
								break;
						} while ( !isdigit(buf[0]));
					}
				}

			}
			++ifCnt;
		} while ( isdigit(buf[0]));
	pclose(fp);
    refreshWiFinstances(discovery);
    refreshDeviceMemory();
	return ifCnt;
}

/* this is callback that checks values */
static Instance *fndInstance;
static int chkParam(CWMPObject *o, CWMPParam *p, Instance *ip, void *up1,
		void *up2, int cbInt) {
	const char *target = (const char *)up1;
	const char *targetValue = up2;
	char *value = NULL;
	if (p && streq(target, p->name)) {
		/* found name-- get value */
		if (p->pGetter && p->pGetter(ip, &value)== CPE_OK) {
			if (streq(value, targetValue) ) {
				FREESTR(value);
				fndInstance = ip;
				return eStopWalk; /* stop walk here */
			}
			FREESTR(value);
		}
	}
	return eOK; /* keep walking */
}
#if 0
static EthernetLink *findELinkByIFName( const char *name){
	char oName[257];
	snprintf(oName, sizeof(oName), "%s.Ethernet.Link.", CWMP_RootObject[0].name);
	CWMPObject *o = cwmpFindObject( oName );
	if ( o ){
		if ( cwmpWalkDownTree( o, NULL, chkParam,(void*)"Name", (void*)name, 1)== eStopWalk
				&& fndInstance )
		{
			return (EthernetLink *)fndInstance->cpeData;
		}
	}
	return NULL;
}
#endif
/*
 * Find the Ethernet.Link.{i}. that has an IP address
 */
static EthernetLink *findELinkByIP( const char *ipAddr){
	char oName[257];
	fndInstance = NULL;
	snprintf(oName, sizeof(oName), "%s.IP.Interface.", CWMP_RootObject[0].name);
	if ( cwmpWalkSubTree( oName, chkParam, (void*)"IPAddress", (void*)ipAddr, 1)== eStopWalk
			&& fndInstance )
	{
		Instance *ipIF = fndInstance->parent; /* parent instance is .IP.Interface.{i}. */
		IPInterface *ipif = (IPInterface *)ipIF->cpeData;
		if ( ipif ){

                ipif->lowerLayers[0]=cwmpGetInstancePtr("Device.Ethernet.Link.1.");
                Instance *elip = ipif->lowerLayers[0];

                if( elip ){ //Fix Segmentation fault
                    EthernetLink *elp = (EthernetLink*)elip->cpeData;
                    return elp;
                }

				
		}
	}
	return NULL;
}

/*
 * This function determines the IP address of the outbound interface to the ACS by opening a
 * bogus DGRAM connection to the ACS and then calling getsockname() on the socket.
 * This should not cause any network traffic with the exception of the DNS access
 * to resolve the ACS IP address if necessary.
 *
 * This is a brain-dead way of guessing at the default route and the IP address
 * of the default route interface. The IP address of the default route is saved
 * in the cpeState.ipAddress and used to find the parameter name of the
 * default interface which is sent in the Inform RPC. This method does not return the
 * IPv6 address of the 6-to-4 tunnel.
 *
 * Other ways of finding the ExternalIPAddress parameter value should be considered in
 * a IDG CPE.
 *
 *
 */
extern char *defaultWANIP;

EthernetLink *cpeGetNetIPInfo(int discovery){
    char proto[PROTOCOL_SZ];
    char host[HOSTNAME_SZ];
    char uri[URI_SZ];
    int port;
	int fd;
	socklen_t alen;
    SockAddrStorage sa;
    struct sockaddr_in *acsp=(struct sockaddr_in *)&sa;
    SockAddrStorage na;
    struct sockaddr_in *np = (struct sockaddr_in *)&na;
    sa_family_t afamily;

    /*  */
	clearInIPAddr( &wanIP );
	if ( defaultWANIP==NULL || readInIPAddr( &wanIP, defaultWANIP)< 0 ){
		cpeLog(LOG_INFO, "Default WAN IP address not specified");
		/* try to determine a WAN IP address */
		//if ( cpeState.acsIPAddress.inFamily == AF_INET6 ){
			/* If IP address affinity not established then resolve ACS */
            if (www_ParseUrl(cpeState.acsURL, proto, host, &port, uri) < 0) {
                fprintf(stdout, "parserUrl fail\n");
                return NULL;
            }
		dns_lookup_auto( host, SOCK_DGRAM, htons(port), AF_UNSPEC, &cpeState.acsIPAddress );
        COPYSTR(cpeState.stunURL, writeInIPAddr(&cpeState.acsIPAddress));
		//}
		SET_SockADDR(acsp, htons(port), &cpeState.acsIPAddress);
		if ( (fd=socket(cpeState.acsIPAddress.inFamily, SOCK_DGRAM, 0))<0 ){
			cpeLog(LOG_ERR, "cpeGetNetIPInfo: Unable to create socket");
			return NULL;
		}
		/* since this is a DGRAM connection there shouldn't be any traffic */
		if ( connect(fd, (struct sockaddr*)acsp, SockADDRSZ(acsp))<0 ){
			cpeLog(LOG_ERR, "cpeGetNetIPInfo: Unable to connect to ACS");
			return NULL;
		}
		alen = sizeof(SockAddrStorage);
		if ( getsockname(fd, (struct sockaddr*)np, &alen) == -1){
			cpeLog(LOG_ERR, "cpeGetNetIPInfo: Unable to getsockname");
			close(fd);
			return NULL;
		}
		afamily = np->sin_family;
#ifdef USE_IPv6
		if ( afamily== AF_INET6) {
			SET_InADDR( &wanIP, AF_INET6, &((struct sockaddr_in6 *)np)->sin6_addr);
		} else {
			SET_InADDR( &wanIP, AF_INET, &((struct sockaddr_in *)np)->sin_addr);
		}
#else
		SET_InADDR( &wanIP, AF_INET, &((struct sockaddr_in *)np)->sin_addr);
#endif
		cpeLog(LOG_DEBUG, "CPE source IP address: %s", writeInIPAddr(&wanIP));
		close(fd);
	} else {
		readInIPAddr( &wanIP, defaultWANIP);
	}
	/* now try to find the interface name associated with the IP address */
	if ( (netIFCnt = refreshNetworkInstances(discovery))>0 ) {
		strcpy( wanIPString, writeInIPAddr(&wanIP));
		cpeNetIF = findELinkByIP( wanIPString );
	} else
		netIFCnt = 0;
	//getDNSServerList();
	
	return cpeNetIF;

}

void cpeNetInit(int discovery){
	cpeGetNetIPInfo(discovery);
}
