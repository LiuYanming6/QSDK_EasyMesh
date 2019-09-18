/*----------------------------------------------------------------------*
 * Gatespace Networks, Inc.
 * Copyright 2012 Gatespace Networks, Inc., All Rights Reserved.
 * Gatespace Networks, Inc. confidential material.
 *----------------------------------------------------------------------*
 * File Name  : cpe.c
 *
 * Description: Instance initialization for TR-098 Amendment 2-1-0.
 *              Specific to a Linux OS.
 *				Examples are WANDevice and LANDevice instances			*
 * $Revision: 1.2 $
 * $Id: cpe.c,v 1.2 2012/06/14 11:20:46 dmounday Exp $
 *----------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <sys/types.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <netinet/in.h>
#include <netdb.h>
#include <syslog.h>

#ifdef DMALLOC
	#include "dmalloc.h"
#endif
#include "../includes/sys.h"
#include "../gslib/src/utils.h"
#include "../gslib/src/event.h"
#include "../gslib/src/protocol.h"
#include "../includes/paramTree.h"
#include "../includes/CPEWrapper.h"
#include "../soapRpc/acsconnreq.h"
#include "../soapRpc/rpcUtils.h"
#include "../soapRpc/notify.h"
#include "../soapRpc/cwmpSession.h"

#define DEBUG
#ifdef DEBUG
#define DBGPRINT(X) fprintf X
#define DEBUGLOG 1
#else
#define DBGPRINT(X)
#define DEBUGLOG 0
#endif

#include "sysutils.h"
#include "targetsys.h"
#include "includes/netIPState.h"
#include "includes/cpedata.h"


extern CWMPObject InternetGatewayDevice_Objs[];
extern CWMPObject LANDeviceHosts_Objs[];
extern CWMPObject Layer2Bridging_Objs[];
extern CWMPObject CWMP_RootObject[];
extern CPEState	cpeState;

void updateL3Routing(void);
void updateL2Bridging(void);
void updateAvailIFInstances(void);
void createWLANConfiguration(const char *name);
void createWEPKeys(const WlanState *, const char *wlanName);
void createPreSharedKeys(const WlanState *wsp, const char *wlanName);
void getLanConfigData(void);
void refreshHosts_Host(void);
void refreshWLANAssocDevices(void);
void updateWANConnections(void);
void refreshHosts_Host(void);
void refresh_ManageableDevice(void);
void updateDefaultWANIPConn(void);

extern int cpeRestoreObjectConfig(void);
extern int cpeRestoreParameterConfig(void);
extern void getDHCPVendorOptions(void);
extern void cpeRestoreCPEState(void);
extern int cpeSaveConfig(void);
void wget_SetServerTimeout(int timeout);

NetIPState *cpeGetNetIPInfo(void);
const char *dfltWANIF(void);

/* prototypes */
int read_if(char *interface, u_int32_t *addr, u_char *mac);


/***********************************************************************/
#ifdef TEST_TARGET
/* Linux Workstation with single physical ethernet interface */
/****
IFConfig ifConfig[] = {
		{"eth0:1", LAYER2|LAYER3|LANSIDE},
		{"eth0:2", LAYER2|LANSIDE|WIFI},
		{"eth0", LAYER2|LAYER3|WANSIDE|WANDFLT},
		//{"br0",	 LAYER3|LANSIDE|BRIF},
		//{"br",   LAYER3|BRIF},
		{NULL, 0},
	};
	*****/

 /*  test platform with extra ethernet interfaces */
IFConfig ifConfig[] = {
		{"eth0", LAYER2|LAYER3|WANSIDE|WANDFLT},
		{"eth1", LAYER2|LAYER3|LANSIDE},
		//{"eth2", LAYER2|LAYER3|LANSIDE},
		//{"eth3", LAYER2|LAYER3|LANSIDE},
		//{"eth2", LAYER2|LANSIDE|WIFI},
		{"br0",	 LAYER3|LANSIDE|BRIF},
		{"br",   LAYER3|BRIF},
		{NULL, 0},
	};

#else
// A typical wireless IGD configuration:
/* The following configuration assumes that the LAN side interfaces
 * are bridged to the "br0" interface and the eth0 ethernet switch
 * has been configured to provide one or more ports on interface
 * "eth0.10".
 * The WAN IP interface can be on "eth0.11".
 *
 * Table order in important. The specific interfaces must appear
 * prior to the prefixes. For example: "br0" must appear before
 * "br". This will cause "br0" to be given a LAYER3 property while
 * any other "brX" will be limited to LAYER2.
 */
IFConfig ifConfig[] = {
		{"eth0.10", LAYER2|LANSIDE},
		{"eth0.11", LAYER2|LAYER3|WANSIDE},
		{"eth1", LAYER2|LAYER3|LANSIDE},
		{"wifi", LAYER2|LANSIDE|WIFI},
		{"wlan", LAYER2|LANSIDE|WIFI},
		{"br0",	 LAYER3|LANSIDE|BRIF},
		{"br",   LAYER3|BRIF},
		{"nas_", LAYER2|WANSIDE}, /* DSL Interface */
		{"ppp",  LAYER3|WANSIDE}, /* a PPP interface */
		{NULL, 0},
	};
#endif

/* The availIF table contains an entry for each network
 * interface and bridge. It is initially populated with
 * an "ifconfig -a" if there is no configuration saved.
 * The IFConfig table is used to describe each type of
 * interface and its direction.
 * e
 * Need to allocate one of the following for each interface
 *
 */
AvailInterface availIF[MAXIFS];

/* need one of these for each LAN side subnet */
/* assume one on the br0 bridge interface */

DhcpDesc dhcpD;



extern CPEState cpeState;
NetIPState cpeNetIPState;
const char NAMESERVER[]="nameserver";

static void getDNSServerList(void){
	FILE *f;
	static char  line[256];
	char	*p;
	int		t;
	char	*lasts;

	if ((f=fopen("/etc/resolv.conf", "r"))) {
		if ( cpeNetIPState.dnsServers ){
			GS_FREE(cpeNetIPState.dnsServers);
			cpeNetIPState.dnsServers = NULL;
		}
		if ( (cpeNetIPState.dnsServers = (char *)GS_MALLOC(257)) == NULL )
			return;
		t = 0;
		while ( (fgets( line, sizeof(line), f)!=NULL) ) {
			p = line;
			p = strtok_r(p, " \n", &lasts);
			if ( p && (strcmp(p, NAMESERVER)== 0) ){
				/* found nameserver line */
				p = strtok_r( NULL, " \n", &lasts);
				if ( p ){
					if ( t> 0){
						strcpy(cpeNetIPState.dnsServers+t,",");
						t+=1;
					}
					strcpy(cpeNetIPState.dnsServers+t, p);
					t += strlen(p);
				}
			}
		}
		fclose(f);
	}
	return;
}

const char *dfltWANIF(void){
	return cpeNetIPState.name;
}

/*
* CPE specific utility functions
*/


#ifdef USE_IPv6
/*
* CPE specific utility functions
*/
static int readMAC(char *interface, NetIPState *nState) {
	FILE 	*fp;
	char	buf[257];
	char	*p;
	if ( (fp=popen("/sbin/ifconfig", "r"))){
		while ( fgets(buf, sizeof(buf), fp)!=NULL) {
			if ( strstr(buf, interface)){   // should be "eth0   Link encap: ..."
				p = strstr(buf, "HWaddr "); // scan for mac string.
				strncpy(nState->mac, p+sizeof("HWaddr "), sizeof(nState->mac));
				break;
			}
		}
		pclose(fp);
	} else
		return -1;
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
	}
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

/*
 * findInterface
 * retrieve the IP address and MAC address from the
 * interface.
 * The ioctls do not work for IPv6 so look in file for IP and
 * interface name.
 *
 */
extern char *defaultNetworkIF;
static void findInterface(NetIPState *nState) {
	FILE *f;
	char line[80];
	char iface[30];
	char iphex[33];
	int prefix;
	int ifx;
	int scope;
	int ifflags;
	int found = 0;
	char *addrstr;
	InAddr a2;

	/* if IF is specified use it */
	if (!EMPTYSTR(defaultNetworkIF)) {
		strcpy(nState->name, defaultNetworkIF);

	} else {
		if ((f = fopen("/proc/net/if_inet6", "r"))) {
			while (!found && (fgets(line, sizeof(line), f) != NULL)) {
				/* iface destination gateway flags refcnt use metric mask mtu window irtt*/
				if (sscanf(line, "%s %x %x %x %x %s", iphex, &ifx, &prefix,
						&scope, &ifflags, iface) == 6) {
					/* convert IP to an InAddr structure and compare */
					addrstr = strToIPv6Addr(iphex);
					readInIPAddr(&a2, addrstr);
					if ((found = eqInIPAddr(&nState->ip, &a2))) {
						fprintf(stdout, "Interface:%s\n", iface);
						strcpy(nState->name, iface);
						break;
					}

				}
			}
			fclose(f);
		}
		if (!found) {
			/* No IPv6 interface, try to find an IPv4 default route */
			if (!findIPv4DefaultRoute()) {
				cpeLog(LOG_ERR,
						"Unable to determine network interface name. Trying eth0");
				strcpy(nState->name, "eth0");

			}
		}
	}
	/* need the MAC address of the interface to form the serial number */
	readMAC(nState->name, nState);
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
 */
extern char *defaultWANIP;

NetIPState *cpeGetNetIPInfo(void){
    char proto[PROTOCOL_SZ];
    char host[HOSTNAME_SZ];
    char uri[URI_SZ];
    int port;
	int fd;
	int	alen;
    SockAddrStorage sa;
    struct sockaddr_in *acsp=(struct sockaddr_in *)&sa;
    SockAddrStorage na;
    struct sockaddr_in *np = (struct sockaddr_in *)&na;
    sa_family_t afamily;

    /*  */
	cpeNetIPState.name[0]='\0';
	clearInIPAddr( &cpeNetIPState.ip);
	if ( readInIPAddr( &cpeNetIPState.ip, defaultWANIP)< 0 ){
		cpeLog(LOG_INFO, "Default WAN IP address not specified");
		/* try to determine a WAN IP address */
		if ( cpeState.acsIPAddress.inFamily == 0 ){
			/* If IP address affinity not established then resolve ACS */
			if (www_ParseUrl(cpeState.acsURL, proto, host, &port, uri) < 0) {
				return NULL;
			}
			dns_lookup( host, SOCK_DGRAM, &cpeState.acsIPAddress );
		}
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
		if ( afamily== AF_INET6) {
			SET_InADDR( &cpeNetIPState.ip, AF_INET6, &((struct sockaddr_in6 *)np)->sin6_addr);
		} else {
			SET_InADDR( &cpeNetIPState.ip, AF_INET, &((struct sockaddr_in *)np)->sin_addr);
		}
		cpeLog(LOG_DEBUG, "CPE source IP address: %s", writeInIPAddr(&cpeNetIPState.ip));
		close(fd);
		} else {
			if ( readInIPAddr( &cpeNetIPState.ip, defaultWANIP)< 0 ){

			}
	}
	/* now try to find the interface name associated with the IP address */
	findInterface(&cpeNetIPState);

	getDNSServerList();
	return &cpeNetIPState;

}
#else
/*
 * read_if
 * retrieve the IP address and MAC address from the
 * interface.
 * TODO: This is no longer valid on 2.6 kernels with IPv6 enabled. Each
 * interface may have multiple IP addresses associated with it. The MAC will
 * be correct, if present.
 */
static int read_if(char *interface, NetIPState *nState) {
	int fd;
	struct ifreq ifr;
	struct sockaddr_in *our_ip;

	memset(&ifr, 0, sizeof(struct ifreq));
	if ((fd = socket(AF_INET,SOCK_RAW, IPPROTO_RAW)) >= 0) {
		ifr.ifr_addr.sa_family = AF_INET;
		strcpy(ifr.ifr_name, interface);
		if (ioctl(fd, SIOCGIFADDR, &ifr) != -1) {
			our_ip = (struct sockaddr_in *) &ifr.ifr_addr;
			SET_InADDR(&nState->ip, AF_INET, &our_ip->sin_addr);
		} else {
			cpeLog(LOG_ERROR, "SIOCGIFADDR failed, interface %s: %s",
					ifr.ifr_name, strerror(errno));
		}
		if (ioctl(fd, SIOCGIFHWADDR, &ifr) != -1)
			memcpy(nState->mac, ifr.ifr_hwaddr.sa_data, 6);
		if (ioctl(fd, SIOCGIFNETMASK, &ifr) != -1) {
			our_ip = (struct sockaddr_in *) &ifr.ifr_addr;
			//nState->mask.inFamily = AF_INET;
			//nState->mask.inAddr  = our_ip->sin_addr;
			SET_InADDR(&nState->mask, AF_INET, &our_ip->sin_addr);
		}
	} else {
		cpeLog(LOG_ERROR, "interface %s socket failed!: %s", ifr.ifr_name,
				strerror(errno));
		return -1;
	}
	close(fd);
	return 0;
}
/* This function looks in the /proc/net/route table to find the current
*  default route and its associated IP address, network interface name, etc.
*  Other methods may be more appropriate for some CPE environments.
*
*/
NetIPState *cpeGetNetIPInfo(void)
{
	FILE *f;
	char  line[256];
	char  iface[30];
	char  dest[30];
	unsigned  gw;
	unsigned flags, refcnt, use, metric, mask, window, mtu, irtt;

	cpeNetIPState.name[0]='\0';
	clearInIPAddr( &cpeNetIPState.ip);
	if ((f=fopen("/proc/net/route", "r"))) {
		while ( (fgets( line, sizeof(line), f)) ) {
			/* iface destination gateway flags refcnt use metric mask mtu window irtt*/
			if ( sscanf(line, "%s %s %x %x %x %x %x %x %x %x %x",
						iface, dest, &gw, &flags, &refcnt, &use, &metric, &mask, &mtu, &window, &irtt)
				 == 11 ) {
				if (streq(dest,"00000000")) {
					struct in_addr ina = {gw};
					//cpeNetIPState.gw.inFamily = AF_INET;
					//cpeNetIPState.gw.inAddr = ina;
					SET_InADDR( &cpeNetIPState.gw, AF_INET, &ina);
					strcpy(cpeNetIPState.name, iface);
					read_if(cpeNetIPState.name, &cpeNetIPState);
					cpeLog(LOG_DEBUG, "WanIF up %s %s", cpeNetIPState.name, writeInIPAddr(&cpeNetIPState.ip));
					break;
				}
			}
		}
		fclose(f);
		getDNSServerList();
	}
	return &cpeNetIPState;
}
#endif


#define PINGCMD  "/bin/ping"
#include "includes/diagPing.h"
#include "includes/diagTraceRoute.h"

TraceRtState cpeTRState;
extern void *acsSession;

void cpeStopTraceRt(void *handle)
{
	DiagState s = (DiagState)handle;
	DBGPRINT((stderr, "cpeStopTraceRt %d\n", s));

	if ( cpeTRState.fp != 0 ){
		int fd = fileno(cpeTRState.fp);
		stopTimer(cpeStopTraceRt, NULL);
		stopListener(fd);
		pclose(cpeTRState.fp);
	}
	cpeTRState.fp = 0;
	cpeTRState.state = s;
	cwmpDiagnosticComplete(); /* setup for next Inform */
}

extern CWMPObject Device_Objs[];
extern CWMPObject LANTraceRouteDiagnostics_Objs[];

static void doTRRead(void *arg)
{
	char buf[1024];

	if ( fgets(buf, 1024, cpeTRState.fp) == NULL ) {
		/* EOF */
		cpeStopTraceRt((void*)eComplete);
		if ( cpeTRState.hopCnt == cpeTRState.maxTTL )
			cpeTRState.state = eMaxHopExceeded;
	} else {
		DBGPRINT((stderr, "buf=>%s<", buf));
		if ( strncmp(buf, "traceroute",10)==0  ) {
			if ( strstr( buf, "unknown host" ) ) {
				/* bad host name -- stop now */
				cpeStopTraceRt((void*)eHostError);
			}
			return; /* ignore information lines of input */
		} else {
			char *cp = buf;
			char *h, *p;
			int	 id;
			Instance *rtInst;
			if ( (p = strtok( cp, " "))) {
				if ( isdigit(*p)) { /* found route line */
					/* ignore route number and use hopCnt as instance id */
					id = ++cpeTRState.hopCnt;
					if ( (h = strtok(NULL, " "))) {
						/* h points to Hop host name */;
						/*
						 * Find and accumulate the ResponseTime
						 */
						if ( (p = strtok(NULL, " ")) ){
							/* Move past (xxx.xxx.xxx.xxx) ip address. */
							cpeTRState.timeout += atoi(p);
						}
						/* Now create the RouteHops.{i} instances for each host response. */
						/* This is necessary because the data model has the route host list */
						/* retrieved as instances of the RouteHops object.                 */
						/* */
						/* The following cwmpcCreateInstance only works because there are*/
						/* not containing objects with instances. Otherwise; the instance stack within*/
						/* the framework would need to be initialized. */
						CWMPObject *targetObject;
						CWMPObject *parentObject = cpeTRState.parentObj;
						/* create instance of .RouteHops.i. */
						targetObject = cpeTRState.hopObjs;
        				rtInst = cwmpCreateInitInstance( parentObject, parentObject->iList, targetObject, id);
			        	if (rtInst) {
			        		rtInst->cpeData = (void*)GS_STRDUP( h );
			        	}
						/* finish parsing line to get times if needed */
						return;
					}
				} else if ( strstr(buf, "MPLS")){
					return;
				} else
					DBGPRINT((stderr, "Info line(skipped): %s", buf));
			}
		}
		 /* everything else is a failure or noise */
		cpeLog(LOG_ERROR, "TraceRoute Parse error:%s\n", buf);
	}
}

void cpeStartTraceRt( void *handle )
{
	char cmd[256];
	char ttlopt[20];
	int  fd;
	stopCallback(&acsSession, cpeStartTraceRt, NULL); /* stop callback */
	/* start the traceroute diagnostic here if pending Requested is set */
	if ( cpeTRState.state == eRequested && cpeTRState.host ) {
		cpeTRState.responseTime = cpeTRState.hopCnt = 0;
		/* TODO: add code to map data model interface name to native interface name */
		/* block size not implemented in most traceroute utilities */
		/* TODO: add remaining supported parameters here. */
		if (cpeTRState.maxTTL==0)
			cpeTRState.maxTTL = TTLMAX;
		snprintf(ttlopt, sizeof(ttlopt), " -m %d", cpeTRState.maxTTL);

		snprintf(cmd, sizeof(cmd), TRACERTCMD "%s%s 2>&1\n",
		  		ttlopt, cpeTRState.host);
		/* the 2>&1 also writes stderr into the stdout pipe */
		fprintf(stderr, "Start traceroute Diagnostic\n %s", cmd);
		if ((cpeTRState.fp = popen(cmd, "r")) == NULL) {
			cpeLog(LOG_ERR, "Could not start traceroute>%s<", cmd);
			return;
		}
		if ((fd = fileno(cpeTRState.fp)) < 0) {
			cpeLog(LOG_ERR, "Could not fileno popen stream %d(%s)",
				 errno, strerror(errno));
			return;
		}
		setTimer(cpeStopTraceRt, NULL, cpeTRState.timeout? cpeTRState.timeout: 10*1000); /* 10000 msec default*/
		setListener(fd, doTRRead, (void*)fd);
	}
	return;
}

/*
 * Remove RouteHop Instances
 */
void cpeResetTRState( void )
{
	CWMPObject *hopObj = cpeTRState.hopObjs;
	Instance *ip = hopObj->iList;
	while ( ip ){
		/* free host name linked to hop instance */
		GS_FREE( ip->cpeData );
		ip = ip->next;
	}
	cwmpDeleteAllInstances( hopObj );
	cpeTRState.state = eNone;
}


PingParam cpePingParam;

void cpeStopPing(void *handle)
{
	DiagState s = (DiagState)handle;
	DBGPRINT((stderr, "cpeStopPing %d\n", s));
	stopTimer(cpeStopPing, (void*)eComplete);
	if ( cpePingParam.fp ){
		stopListener(fileno(cpePingParam.fp));
		pclose(cpePingParam.fp);
	}
	cpePingParam.fp = 0;
	cpePingParam.state = s;
	cpePingParam.failures = cpePingParam.reps - cpePingParam.rcvdCnt;
	cwmpDiagnosticComplete(); /* setup for next Inform with Diag Complete event */
}
static void doRead(void *arg)
{
	char buf[1024];
	PingParam *pp = &cpePingParam;
	char	  *p;
	if ( fgets(buf, 1024, cpePingParam.fp) == NULL ) {
		/* EOF */
		cpeStopPing((void*)eComplete);
	} else {

		DBGPRINT((stderr, "buf=>%s", buf));
		if ( strncmp(buf, "PING", 4)==0  ) {
			return; /* ignore first line of input */
		} else if ( strstr(buf, "unknown host") ){
			/* ping: unknown host xxxxxxxxx */
			cpeStopPing((void*)eHostError);
		} else if ( strstr(buf, "packets transmitted") ){
			/* looking for:  n packets transmitted, x received, y errors ...*/
			sscanf(buf,"%*d packets transmitted, %d received", &pp->rcvdCnt);
			pp->failures = pp->reps - pp->rcvdCnt;
		} else if ( strstr(buf, "rtt")){
			/* looking for min/avg/max */
			if ( (p = strchr(buf, '='))) {
				++p;
				pp->minRsp = atoi(p);
				if ( (p = strchr(p, '/'))){
					++p;
					pp->avgRsp = atoi(p);
					if ( (p=strchr(p, '/'))){
						++p;
						pp->maxRsp = atoi(p);
					}
				}
			}
		} else if ( strstr(buf, "bytes ")) {
			/* count received packets */
			pp->rcvdCnt++;
		}

	}
}

void cpeStartPing( void *handle )
{
	char cmd[256];
	char blkstr[10];
	char qstr[10];
	char ifname[64];
	int	 fd;
	PingParam *pp = &cpePingParam;
	stopCallback(&acsSession, cpeStartPing, NULL); /* stop callback */
	/* start the Ping diagnostic here if pending Requested is set */
	if ( pp->state == eRequested ) {
		pp->maxRsp = pp->minRsp = pp->rcvdCnt =
								pp->totRsp = 0;
		if (pp->blockSize)
			snprintf(blkstr, sizeof(blkstr), " -s %d", pp->blockSize);
		else
			blkstr[0]='\0';
		if (pp->dscp )
			snprintf(qstr, sizeof(qstr), " -q %d", pp->dscp);
		else
			qstr[0] = '\0';
		if (pp->ifname)
			snprintf(ifname, sizeof(ifname), " -I %s", pp->ifname);
		else
			ifname[0] = '\0';
		if ( pp->reps == 0)   /* default reps to 4 if not specified */
			pp->reps = 4;
#ifdef TEST_TARGET
		snprintf(cmd, sizeof(cmd), PINGCMD " -c %d %s %s %s %s >&1",
				pp->reps, blkstr, ifname, qstr, pp->host);
#else /* busybox ping cmdline */
		snprintf(cmd, sizeof(cmd), "%s -c %d%s %s 2>&1\n", PINGCMD,
		  		pp->reps, blkstr, pp->host);
#endif
		/* the 2>&1 also writes stderr into the stdout pipe */
		fprintf(stderr, "Start LAN Ping Diagnostic\n %s", cmd);
		if ((pp->fp = popen(cmd, "r")) == NULL) {
			cpeLog(LOG_ERR, "Could not start ping >%s<", cmd);
			pp->state = eErrorInternal;
			cwmpDiagnosticComplete(); /* setup for next Inform with Diag Complete event */
			return;
		}
		if ((fd = fileno(pp->fp)) < 0) {
			cpeLog(LOG_ERR, "Could not fileno popen stream %d(%s)",
				 errno, strerror(errno));
			pp->state = eErrorInternal;
			cwmpDiagnosticComplete(); /* setup for next Inform with Diag Complete event */
			return;
		}
		if ( fcntl( fd, F_SETFL, O_ASYNC)< 0){
			cpeLog(LOG_ERR, "Could not set O_ASYNC flag on popen fd: %s", strerror(errno));
			pp->state = eErrorInternal;
			cwmpDiagnosticComplete(); /* setup for next Inform with Diag Complete event */
			return;
		}
		setTimer(cpeStopPing, (void*)eComplete, pp->timeout? pp->timeout: pp->reps*1000 + 10*1000); /* 10000 msec default*/
		setListener(fd, doRead, (void*)fd);
	}
	return;
}

/*********************************************************
 * Read the /var/udhpc.leases file to find lan hosts.
 * This is required to be done prior to the start of a
 * session so that the GetParameterNames, GetParameterValue
 * work correctly. The number of Hosts.Host.{i}. can changes
 * Independently of any management activity.
 *
 * The same is true of the WLAN AssociatedDevice.{i}.
 *
 */

HostLease hostLeases[256];

void refreshHosts_Host(){
	int active = 0;
	int i;
	int	fd;
	int	lth;
	CWMPObject *o;
	char path[100];

	if ( (fd=open(DHCPLEASES, O_RDONLY))!= -1) {
		while (active < 256 &&
			( (lth = read( fd, &hostLeases[active], sizeof(HostLease)))>0 )){
				++active;
			}
		close(fd);

		/* now create an Instance for each host */
		/* need to first delete existing instances */
		o = cwmpFindObject("InternetGatewayDevice.LANDevice.1.Hosts.Host.");
		/* delete all the old ones */
		cwmpDeleteAllInstances(o);

		for (i=0; i<active; ++i){
			Instance *hostIp;
			char hostPath[100];
			snprintf(hostPath, sizeof(hostPath), "%s%d.", path, i+1);
			hostIp = cwmpInitObjectInstance( hostPath );
			if (hostIp){
				hostIp->cpeData = (void *)i; /* cpeData is index into hostLeases table */
				fprintf(stderr,"Create Hosts.Host instance %d\n", i+1);
			}
		}
	}
}


/*
 * Save interface configuration. May want to combine this with
 * object/parameter configuration in fielded applications.
 */
/*
 * Save the interface configuration to the open
 * file pointed to by cf.
 */
AvailInterface *findIF(char *ifname);
static void saveInterfaceConfig(FILE *cf){
	int i=0;
	fprintf(cf, "#InterfaceConfig\n");
	for (i=0; i<MAXIFS; ++i){
		if ( availIF[i].flag&ENTRY_ACTIVE ){
			if ( !EMPTYSTR(availIF[i].ifname) ) fprintf(cf, "  if = %s\n", availIF[i].ifname);
			if ( !EMPTYSTR(availIF[i].ip) ) fprintf(cf, "  ip = %s\n", availIF[i].ip);
			if ( !EMPTYSTR(availIF[i].mask) ) fprintf(cf, "  mask = %s\n", availIF[i].mask);
			fprintf(cf, "  addrtype = %s\n", availIF[i].addrType==eDHCP? "DHCP": "Static");
			fprintf(cf, "  iftype = 0x%x\n", availIF[i].flag );
			fprintf(cf, "  iID = %d\n", availIF[i].instanceId);
		}
	}


}

static void restoreInterfaceConfig(FILE *cf ){
	char	pName[256];
	char	b[256];
	AvailInterface *ap=NULL;
	while ( fgets(b, 256, cf)){
		if ( b[0] == '#'){
			;  /* first line is #InterfaceConfig */
		} else if ( b[0] == ' '){ /* blank maybe parameter name */
			if ( strspn( b, " ") == 2 ){
				char *p;
				if ( (p= strstr( &b[2], " = "))) {
					b[ strlen(b)-1] = '\0'; /* strip \n off of end */
					strncpy( pName, &b[2], p-&b[2] );
					pName[p-&b[2]] = '\0';
					p += 3; /* start of value */
					if (streq(pName,"if")) {
						ap=findIF(p); /* find next entry */
					} else if ( ap && streq(pName,"ip")) strcpy(ap->ip, p);
					else if ( ap && streq(pName,"mask")) strcpy(ap->mask, p);
					else if ( ap && streq(pName,"addrtype")) {
						ap->addrType = streq(p, "DHCP")? eDHCP: eStatic;
					} else if (  ap && streq(pName,"iftype")) {
						int mask = strtol( p, NULL, 16);
						ap->flag |= mask;
					} else if ( ap && streq(pName, "iID")){
						ap->instanceId = atoi(p);
					}
				} /* else
				   * File is corrupt if = is missing.
				   */
			}
		} else if ( isalpha(*b))
			/* found next title line */
			return;
	}
	return;
}
int cpeRestoreInterfaceConfig(){
	FILE *cf;
	if ( (cf = fopen(INTERFACECONFIG_FILENAME, "r")) ){
		restoreInterfaceConfig(cf);
		fclose(cf);
		return 1;
	}
	return 0;
}
void cpeSaveInterfaceConfig(void)
{
	FILE *cf;
	if ( (cf = fopen(INTERFACECONFIG_FILENAME, "w")) ){
		saveInterfaceConfig(cf);
		fclose(cf);
	} else
		fprintf(stderr, "Unable to open: " INTERFACECONFIG_FILENAME);
}

/*
 * get labeled value
 */
static int getValue(char *b, char *label, char *dest, int size){
	char *p, *d;
	if ( (p = strstr( b, label)) ){
		p += strlen(label);
		while ( isspace(*p)) ++p;   /* scan off and white space */
		d =p;
		while ( !isspace(*d) && !iscntrl(*d)) ++d;  /* scan for end of token*/
		int lth = d-p;
		if (lth-1> size) lth = size;
		strncpy( dest, p, lth);
		*(dest+lth+1) = '\0';
		return lth+1;
	}
	return 0;
}

int getAvailIFKey( char *ifname ){
	int	i = 0;
	while ( i < MAXIFS	){
		if ( strcmp( ifname, availIF[i].ifname)==0 ){
			return availIF[i].instanceId;
		}
		++i;
	}
	return -1;
}

AvailInterface *findAvailIF( const char *ifname){
	int	i;
	for(i=0; i < MAXIFS; ++i){
		if ( availIF[i].flag& ENTRY_ACTIVE
		  && strcmp( ifname, availIF[i].ifname)==0 ){
			return &availIF[i];
		}
	}
	return NULL;
}

AvailInterface *findIFByKey( int key ){
	int	i;
	for(i=0; i < MAXIFS; ++i){
		if ( availIF[i].flag& ENTRY_ACTIVE && availIF[i].instanceId == key ){
			return &availIF[i];
		}
	}
	return NULL;
}

AvailInterface *findIF(char *ifname){
	int i = 0;
	AvailInterface *a;
	if ( (a = findAvailIF(ifname)) == NULL) {
		i = 0;
		while ( i< MAXIFS ){
			if ( !(availIF[i].flag& ENTRY_ACTIVE) ){
				/* add new interface name here */
				memset( &availIF[i], 0, sizeof( AvailInterface));
				strcpy( availIF[i].ifname, ifname);
				availIF[i].flag = ENTRY_ACTIVE;
				return &availIF[i];
			}
			++i;
		}
	}
	return NULL;
}

static void resetAvailFlag(){
	int	i = 0;
	while ( i < MAXIFS	){
		availIF[i].flag &= ~ENTRY_ACTIVE; /* mark all as unused */
		++i;
	}

}

static void setLANWANFlags(){
	int	i = 0;
	while ( i < MAXIFS	){
		if ( availIF[i].flag&ENTRY_ACTIVE ){
			int k = 0;
			while ( ifConfig[k].prefix != NULL) {  /* find config */
				if ( strstr(availIF[i].ifname, ifConfig[k].prefix ) == availIF[i].ifname ){
					if ( ifConfig[k].cfMask&LANSIDE)
						availIF[i].flag |= ENTRY_LAN;
					if ( ifConfig[k].cfMask&LAYER2)
						availIF[i].flag |= ENTRY_L2;
					if ( ifConfig[k].cfMask&LAYER3)
						availIF[i].flag |= ENTRY_L3;
					if ( ifConfig[k].cfMask&BRIF)
						availIF[i].flag |= ENTRY_BRIDGE;
					if ( ifConfig[k].cfMask&WANDFLT)
						availIF[i].flag |= ENTRY_WANDFLT;
					break;
				}
				++k;
			}

		}
		++i;
	}
}
static void clearUnusedAvail(){
	int	i = 0;
	while ( i < MAXIFS	){
		if ( !(availIF[i].flag & ENTRY_ACTIVE) )
			memset( &availIF[i], 0, sizeof( AvailInterface));
		++i;
	}
}

static void getDhcpDescs(AvailInterface *ap){
	char	buf[1024];
	int		fd;
	int		lth;
	char	flag;
// Assume that the LAN is on br0 interface.
	snprintf(buf, sizeof(buf), DHCPCONF ".%s", "br0");
	ap->addrType = eStatic;
	memset(&dhcpD, 0, sizeof(DhcpDesc));
	if ( (fd =open(buf, O_RDONLY)) != -1) {
		lth = read( fd, buf, sizeof(buf));
		close(fd);
		if ( lth > 0){
			getValue(buf, "Status", &flag, 1);
			if (flag == '1') {
				dhcpD.flag = ENTRY_ENABLE|ENTRY_ACTIVE;
			} else
				dhcpD.flag = ENTRY_ACTIVE;
			dhcpD.reservedIP[0]='\0';
			getValue(buf, "start", dhcpD.minIP, sizeof(dhcpD.minIP));
			getValue(buf, "end", dhcpD.maxIP, sizeof(dhcpD.maxIP));
			getValue(buf, "interface", dhcpD.interface, sizeof(dhcpD.interface));
			getValue(buf, "router", dhcpD.ipRouters, sizeof(dhcpD.ipRouters));
			getValue(buf, "domain", dhcpD.domainName, sizeof(dhcpD.domainName));
			getValue(buf, "lease", dhcpD.leaseTime, sizeof(dhcpD.leaseTime));
			getValue(buf, "dns", dhcpD.dnsServers, sizeof(dhcpD.dnsServers));
			getValue(buf, "subnet", dhcpD.subnetMask, sizeof(dhcpD.subnetMask));
			getValue(buf, "static_lease", dhcpD.reservedIP, sizeof(dhcpD.reservedIP));
		}
	}
}

void setDhcpDesc(int enable){
	FILE	*f;
	char	buf[80];

	fprintf(stderr, "DHCP set to %s\n", enable? "ENABLED": "DISABLE");
	snprintf(buf, sizeof(buf), DHCPCONF ".%s", "br0");
	if ( (f = fopen(buf, "w"))){
		fprintf(f, "start\t%s\n", dhcpD.minIP);
		fprintf(f, "end\t%s\n", dhcpD.maxIP);
		fprintf(f, "interface\t%s\n", dhcpD.interface);
		fprintf(f, "opt\tdns\t%s\n", dhcpD.dnsServers);
		fprintf(f, "option\tsubnet\t%s\n", dhcpD.subnetMask);
		fprintf(f, "opt\trouter\t%s\n", dhcpD.ipRouters);
		fprintf(f, "option\tdomain\t%s\n", dhcpD.domainName);
		fprintf(f, "option\tlease\t%s\n", dhcpD.leaseTime);
		if ( strlen(dhcpD.reservedIP ))
			fprintf(f, "reserved-lease\t%s\n", dhcpD.reservedIP);
		fprintf(f, "#Status%c\n", enable?'1': '0');
		fclose(f);
	} else {
		fprintf(stderr, "Unable to open %s", buf);
	}
	/* */
	killDaemon(buf);
	if ( enable ) {
		do_cmd(DEBUGLOG, "udhcpd", "%s", buf);
	}
}
/*
 * initialize the WLANConfiguration instances
 * These functions have been changed to now support multiple
 * WLANConfiguration instances. The instances are created statically when the
 * framework is initialized and always have instance numbers
 * 1..MAXWLANCONFIG.
 */

WlanState wlanState[MAXWLANCONFIG];

/*
 * Creation of WLANConfiguration instances and children.
 * There are always 4 instances of this object, 1..4.
 * Instance.cpedata points to the key[i] buffer in WlanState.
 */

void createWEPKeys(const WlanState *wsp, const char *wlanName){
	int k;
	char keyName[257];
	Instance *ikp;
	/* create InternetGatewayDevice.LANDevice.1.WLANConfiguration.1.WEPKey.{i}. */
	for ( k=0; k<4; ++k ){
		snprintf(keyName, sizeof(keyName), "%s%s.%d.", wlanName, "WEPKey", k+1);
		if ( (ikp = cwmpInitObjectInstance(keyName))){
			// point cpeData at the key buffer
			ikp->cpeData = (void *)wsp->keys[k];
		}
	}
}

/*
 * Create the PreShared key for the WLAN configuration instance.
 * There are always 10 instances, 1..10 of this object.
 * Instance.cpedata points to the preSharedKey buffer in WlanState
 */
void createPreSharedKeys(const WlanState *wsp, const char *wlanName){
	int k;
	char keyName[257];
	Instance *ikp;
	/* create InternetGatewayDevice.LANDevice.1.WLANConfiguration.1.PreSharedKey.{i}. */
	for ( k=0; k<MAXPSKEY; ++k ){
		snprintf(keyName, sizeof(keyName), "%s%s.%d.", wlanName, "PreSharedKey", k+1);
		if ( (ikp = cwmpInitObjectInstance(keyName))){
			// point cpeData in Instance descriptor at the PreSharedKey structure for instance.
			ikp->cpeData = (void *)&(wsp->preSharedKey[k-1]);
		}
	}
}
/*
 * update the AvailInterface entry with any data from buffer.
 */
static void getLanIF(AvailInterface *aif, char *buf ){

		getValue(buf, "inet addr:", aif->ip, sizeof(aif->ip) );
		getValue(buf, "Mask:", aif->mask, sizeof(aif->mask));
		if ( getValue(buf, "RX packets:", aif->rxPackets, sizeof(aif->rxPackets))){
			getValue(buf, "errors:", aif->rxErrors, sizeof(aif->rxErrors));
			getValue(buf, "dropped:", aif->rxDropped, sizeof(aif->rxDropped));
		}
		if ( getValue(buf, "TX packets:", aif->txPackets, sizeof(aif->txPackets))) {
			/* reading the TX line */
			getValue(buf, "errors:", aif->txErrors, sizeof(aif->txErrors));
			getValue(buf, "dropped:", aif->txDropped, sizeof(aif->txDropped));
		}
		getValue(buf, "RX bytes:", aif->rxBytes, sizeof(aif->rxBytes));
		getValue(buf, "TX bytes:", aif->txBytes, sizeof(aif->txBytes));
		getValue(buf, "Link encap:", aif->linkEncap, sizeof(aif->linkEncap));
		getValue(buf, "HWaddr", aif->mac, sizeof(aif->mac));
		if (strstr(buf, "UP " ))
			aif->ifStatus= eUP;
		aif->flag |= ENTRY_ENABLE;
}

void setLanIF(int ix){
	char buf[80];
	AvailInterface *aif = &availIF[ix];
	snprintf(buf, sizeof(buf), "ifconfig %s %s netmask %s",
			aif->ifname, aif->ip, aif->mask);
/* TODO: dhcp addrType */
}

void getAvailableIF(void){
	FILE	*f;
	char 	buf[80];

	char	ifname[30];
	AvailInterface *ap;
	snprintf(buf, sizeof(buf), "ifconfig -a >" IFTEMP);
	system(buf);
	if ( (f = fopen(IFTEMP, "r"))) {
		/* first set the unused flag in each availIF entry */
		resetAvailFlag();
		while ( fgets(buf, sizeof(buf), f)){
			//if (buf[0]!=' '){ /* not a blank - then its a interface name. */
			// changed test for systems with non-English localization */
			if ((isascii(buf[0])!=0)&& (buf[0]!=' ')&&(!iscntrl(buf[0]))){
				char *p = strchr(buf, ' ');
				char	*b;
				strncpy(ifname, buf, p-buf);
				ifname[p-buf]='\0';
				if ( (ap = findIF(ifname)) ){
					ap->flag |= ENTRY_ACTIVE;
					do {
						getLanIF( ap, buf);  /* extract any data */
						b = fgets( buf, sizeof(buf), f);
					} while( b != NULL && b[0]==' ');
				} else
					fprintf(stderr, "Internal error on getting interface data");
			}
		}
		setLANWANFlags();
		fclose(f);
		unlink(IFTEMP);
	}
	clearUnusedAvail();
}
/*
 * call from the configRestore to complete AvailableInterface
 * instance initialization.
 */
void initAvailableInterface(Instance *ip){
	int i;
	for ( i=0; i<MAXIFS; ++i){
		if ( availIF[i].instanceId == ip->id )
			ip->cpeData = (void *)i+1;   /* cross link from instance to availIF entry*/
	}
	return;
}
/*
 * Refresh AvailableInterface instance list by first passing thru
 * the Instance list and checking if an availIf is present. If not then the
 * flag is marked unused then delete the Instance.
 * Then pass over the availIF list and check if each has an number present. If
 * not then add the new instance.
 */
void updateAvailIFInstances(){
	Instance *ip;
	Instance *next;
	CWMPObject *o = cwmpSrchOList( Layer2Bridging_Objs, "AvailableInterface");
	CWMPObject *parent = cwmpSrchOList(InternetGatewayDevice_Objs, "Layer2Bridging");
	Instance *path = parent->iList;
	getAvailableIF();
	ip = o->iList;
	while ( ip ) {
		next = ip->next;
		if (ip->parent == path ){ // check that this Instance is in the Instance path.
			int	ai = ((int)ip->cpeData-1); /* index into availIF table */
			if ( ai >= 0 ){
				if ( (availIF[ai].flag&(ENTRY_ACTIVE|ENTRY_L2))== (ENTRY_ACTIVE|ENTRY_L2)) { // still in use
					;
				} else {
					cwmpDeleteInstance(o, ip);
				}
			}
		}
		ip = next;
	}
	int i = 0;
	while ( i<MAXIFS){
		if (( availIF[i].flag&(ENTRY_ACTIVE|ENTRY_L2))== (ENTRY_ACTIVE|ENTRY_L2) ){
			// only create active L2 instances
			int id = availIF[i].instanceId;
			if ( (id == 0) || (ip = cwmpFindInstance(o, path, itoa(id))) == NULL ){
				ip = cwmpCreateInitInstance(parent, path, o, cpeGetInstanceID());
				ip->cpeData = (void *)(i+1);
				availIF[i].instanceId = ip->id;   // x-link
			}
		}
		++i;
	}
}


static void refreshWLANConfigurationAssocDevices(char *wLanPath, int wLanId) {
	int active = 3;         /* create 3 test associations */
	int i;
	Instance *aDevIp;
	char path[200];
	AssociatedDev *ap;

	/* get the wireless data here */

	for (i = 0; i < active; ++i) {
		/* for each item create an AssociatedDevice instance */
		snprintf(path, sizeof(path), "%s%d.", wLanPath,	i + 1);
		fprintf(stderr, "WLAN path is %s\n", path);

		aDevIp = cwmpInitObjectInstance(path);
		if (aDevIp) {
			/* allocate some data for the parameters */
			aDevIp->cpeData = ap = (void *) GS_MALLOC(sizeof(AssociatedDev));
			if ( ap!= NULL ){
				memset(ap, 0, sizeof(AssociatedDev));
				/* now fill in the data */
				/* this is test data - need to use data from the wireless */
				ap->devAuthState = 1;
				strcpy(ap->devIPAddress, "ip addr instance1");
				strcpy(ap->devMACAddress, "mac ");
				strcpy(ap->lastPMKId, "pmk-id");
			}
		}
	}
}
/*
 * The AssociatedDevice instances are managed by the CPE device. Replace the
 * old list of instances with the current list.
 */
void refreshWLANAssocDevices(void) {
	int i;
	CWMPObject *assocDev;
	Instance *wLanIp;
	Instance *aDevIp;

	char path[100];
	/* Loop thru the InternetGatewayDevice.LANDevice.1.WLANConfiguration.{i}.  */
	/* WLANConfigurations.{i}. are 1..n    */
	/* One of there per SSID               */

	for (i = 0; i < MAXWLANCONFIG; ++i) {
		snprintf(path, sizeof(path),
				"InternetGatewayDevice.LANDevice.1.WLANConfiguration.%d.AssociatedDevice.", i + 1);
		if ((assocDev = cwmpFindObject(path))) {
			/* wLanIp is the parent in the instance path */
			wLanIp = cwmpGetCurrentInstance();
			if (assocDev) {
				int ai = 1;
				do { /* walk down Instances and delete them */
					if ((aDevIp = cwmpFindInstance(assocDev, wLanIp, itoa(ai)))) {
						if (aDevIp->cpeData) /* delete user data */
							GS_FREE(aDevIp->cpeData);
						cwmpDeleteInstance(assocDev, aDevIp);
					}
				} while (aDevIp);
			}
			/* create InternetGatewayDevice.LANDevice.1.WLANConfiguration.{i].
			 * 									AssociatedDevice.{ai}.           */
			refreshWLANConfigurationAssocDevices(path, i);
		} else
			break;
	}
}
/*
 * refresh all the LAN configuration data
 */
void getLanConfigData(){
	int		i;
	AvailInterface *ap;

	i=0;
	while (i<MAXIFS){
		ap = &availIF[i];
		if ( ap->flag& ENTRY_LAN ) {
			// see if there is a udhcpd config file for the I/F
			getDhcpDescs( ap );

		}
		++i;
	}
	refreshHosts_Host();
}


void getWLanChannel(WlanState *wsp){
	FILE *f;
	char	buf[80];
	AvailInterface *a = wsp->aifp;
	do_cmd(DEBUGLOG, "iwlist","%s channel >" IFTEMP, a->ifname);
	if ( (f = fopen(IFTEMP, "r"))) {
		while ( fgets(buf, sizeof(buf), f) ){
			if (strstr(buf, "Current F") != NULL){
				// found Current Freq line
				getValue(buf, "Channel ", wsp->channel, sizeof(wsp->channel));
			}
		}
		fclose(f);
	}

}

void getWLanRate(WlanState *wsp){
	FILE *f;
	char buf[80];
	AvailInterface *a = wsp->aifp;
	snprintf(buf, sizeof(buf), "iwlist %s rate >" IFTEMP, a->ifname);
	system(buf);
	if ( (f = fopen(IFTEMP, "r"))) {
		while ( fgets(buf, sizeof(buf), f) ){
			if (strstr(buf, "Current Bit") != NULL){
				// found Current Freq line
				getValue(buf, "Rate:", wsp->rate, sizeof(wsp->rate));
			}
		}
		fclose(f);
	}
}

extern AvailInterface availIF[MAXIFS];
extern WlanState wlanState[];

/* Simpler integrated version */
/*
 * These are addInstance functions for the eStaticInstance object types.
 * The eStaticInstance type allows the initialization procedures to use the
 * traversal of the object/parameter tables to initialize instances that
 * are not added by the framework executing an RPC.
 *
 * The *value data may be left as is.
 */
extern char wpPathName[];

CPE_STATUS addLANDeviceWLANConfiguration(CWMPObject *o, char **value){
	/* This is adding a WLANConfiguration instance to the object tree */
	/* that corresponds to a wireless h/w configuration. */
	Instance *ip;
	int id;
	AvailInterface *ap = NULL;
	char lanNames[257];
	/* Check if the instance ID is passed in? */
	if ( *value != NULL ){
		id = atoi( *value );
		fprintf(stderr, "addLANDeviceWLANConfiguration id = %d\n", id);
		if (!EMPTYSTR(WLANIFNAME)){
			ap = findAvailIF(WLANIFNAME);
		}
		if ( (ip= cwmpCreateInstance( o, id)) ) {
			/* */
			for( id=0; id<MAXWLANCONFIG; ++id){
				if ( wlanState[id].wlanIp==NULL){
					wlanState[id].wlanIp = ip;
					ip->cpeData = (void *)&wlanState[id];
					wlanState[id].aifp = ap;
					snprintf(lanNames, sizeof(lanNames),"%s%s.", wpPathName, *value);
					createWEPKeys(&wlanState[id], lanNames);
					/* create the 10 PreSharedKey instances when we figure out how is works*/
					createPreSharedKeys(&wlanState[id], lanNames);
					return CPE_OK;
				}
			}
		}
	}
	fprintf(stderr, "Unable to create static instance for WLANConfiguration.%s\n", *value);
	return CPE_ERR;
}
/*
 * This function is called to add the interfaces and create the static instances
 * from the compiled in configuration tables.
 */
static void createLANChildInstances(void){
	char	lanNames[257];
	int		i, id;
	Instance *ip;

	/* create                                                          */
	/* InternetGatewayDevice.LANDevice.{i}.LANEthernetInterfaceConfig.1. */
	// This is mapped to eth1, there is one of these per ethernet MAC address  */
	for ( i= 0; i<MAXIFS; ++i ){
		if ( availIF[i].flag&ENTRY_ACTIVE
				&& strcmp( availIF[i].ifname, dfltWANIF() )  /* not eq to wan interface */
				&& strcmp( availIF[i].ifname, WLANIFNAME)  /* not equ to WLAN I/F */
				&&	strstr( availIF[i].ifname, "eth")){
			snprintf(lanNames, sizeof(lanNames),
				"InternetGatewayDevice.LANDevice.1.LANEthernetInterfaceConfig.%d.", i+1);
			if ( (ip = cwmpInitObjectInstance(lanNames))) {
				ip->cpeData = (void *)i;			/* point at availIF entry */
			} else {
				cpeLog(LOG_ERR, "Unable to create LAN instances");
			}
		}
	}

	/* create LANUSBInterfaceConfig if required here                 */

	/* create InternetGatewayDevice.LANDevice.1.WLANConfiguration.{i}.  */
	/* WLANConfigurations.{i}. are 1..n    */
	/* One of there per SSID               */
	for ( i= id =0; i<MAXIFS && id<MAXWLANCONFIG; ++i ){
		if ( availIF[i].flag&ENTRY_ACTIVE
				&& (strstr( availIF[i].ifname, WLANIFNAME)!=NULL) ){
			/* create InternetGatewayDevice.LANDevice.1.WLANConfiguration.{id}.  */
			snprintf(lanNames, sizeof(lanNames),
					"InternetGatewayDevice.LANDevice.1.WLANConfiguration.%d.", i);
			if ( (ip = cwmpInitObjectInstance(lanNames))) {
				wlanState[id].wlanIp = ip;
				ip->cpeData = &wlanState[id];  /* cross link to instance */
				wlanState[id].status = 0;
				wlanState[id].aifp = &availIF[i];				/* link to availIF entry */
				createWEPKeys(&wlanState[id], lanNames);
				/* create the 10 PreSharedKey instances when we figure out how is works*/
				createPreSharedKeys(&wlanState[id], lanNames);
				++id;
			}
		}
	}

}

static void createBasicInstances(void) {
	/* create or update LANDDevice and WANDevice Instances and child Instances */
	char	oName[100];
	char	lanNames[257];

	/****************************** LANDevice..... ********************/
	/* For this implementation this corresponds to br0 with an IP    */
	/* subnet defined.                                                */
	/* create InternetGatewayDevice.LANDevice.1. */
	cwmpInitObjectInstance( "InternetGatewayDevice.LANDevice.1." );

	/*
	 * InternetGatewayDevice.LANDevice.1.LANHostConfigManagement.IPInterface.1. */
	snprintf(lanNames, sizeof(lanNames),
			"%s%s", oName, "LANHostConfigManagement.IPInterface.1.");
	cwmpInitObjectInstance(lanNames);

	/******************************** WANDevice..... *****************/
	/* create InternetGatewayDevice.WANDevice.1. instance */
	cwmpInitObjectInstance("InternetGatewayDevice.WANDevice.1.");
#if 0
	/* Create InternetGatewayDevice.WANDevice.1.WANCommonInterfaceConfig.Connection.1. */
	cwmpInitObjectInstance("InternetGatewayDevice.WANDevice.1.WANCommonInterfaceConfig.Connection.1.");
	/* Create  InternetGatewayDevice.WANDevice.1.WANConnectionDevice.1. */
	cwmpInitObjectInstance("InternetGatewayDevice.WANDevice.1.WANConnectionDevice.1.");
	/*  InternetGatewayDevice.WANDevice.1.WANConnectionDevice.11.WANIPConnection.1. */
	cwmpInitObjectInstance("InternetGatewayDevice.WANDevice.1.WANConnectionDevice.11.WANIPConnection.1.");
#endif
}

static void restoreConfigData(void){
	/* check if there is a saved configuration */
	cpeRestoreInterfaceConfig();
	createBasicInstances();
	if (cpeRestoreObjectConfig() == 0) {
		/* No saved config or default. */
		/* no configuration -- attempt to create the instances, etc. from */
		/* the running environment */
		updateAvailIFInstances();
		//updateL2Bridging();
		/* other objects to create */
		createLANChildInstances();
		updateWANConnections();

		// refresh hosts config data.
		getLanConfigData();
		updateL3Routing();
	} else {
		/* May need to initialize the created objects or instances here */
		cpeRestoreParameterConfig();
		updateWANConnections();
		updateL3Routing();
		// initialize convenience pointers, etc.
		cpeLog(LOG_INFO, "Data Model restored from config.save");

	}

}

/*
* The cpeRefreshInstances .
 * This function should create any new object instances that have been added as a result
 * of a new CPE configuration item being added. It should also delete any object
 * instances that no longer have corresponding CPE configuration items. This function
 * is called by the CWMPc framework prior to establishing a session with the ACS.
*/
#ifdef SERVICES
extern void servicesInstanceRefresh(void);
#endif

void cpeRefreshInstances(void) {
	/* Update any instances that have been modified by local CPE configuration */
	/* functions. */
	fprintf(stdout, "cpeRefreshInstances()\n");
	/* Update the CPE predefined instances here*/
	refreshHosts_Host();
	refresh_ManageableDevice();
	refreshWLANAssocDevices();
	updateAvailIFInstances();
	updateDefaultWANIPConn();
#ifdef SERVICES
	servicesInstancesRefresh();
#endif
}

/*
 * When using the clientapi interface it may be necessary to
 * ensure that the necessary configuration parameters required to
 * access the ACS are defined before attempting to start an ACS session.
 * This function checks the parameters and returns eNOACCESS or eACS_ACCESSABLE.
 * If there is no WAN configuration or connectivity this function should return
 * eNOACCESS.
 */
#ifdef POLL_ACSACCESS_STATUS
extern NetIPState cpeNetIPState;
ACSACCESS cpeGetACSAccessStatus(void){

	if ( cpeState.acsURL==NULL || cpeState.acsURL[0]=='\0' )
		return eNOACCESS;
	if ( cpeNetIPState.ip.inFamily == 0)
		/* if no IPAddress then no access */
		return eNOACCESS;
	/* check DNS here */
	/* check route */
	return eACS_ACCESSABLE;
}
#endif
/*
* This is called from the CWMPC main startup before any other initialization.

* The steps here should be:
* 1. restore the object and parameter data and cpeState from persistent memory.
* 2. If necessary create instances and children of the TR-069 objects to match the configuration data.
* 3. Restore any notification attributes associated with the instances.
*
*/

void cpeBootUpCPEInit(void)
{
	NetIPState *ips;
#ifdef GEN_CONNREQ_PATH
	/* Connection Request URL formation.
	 * Add code here to generate the random path that is unique to
	 * the CPE device. See section 3.2.2.
	 * The length must be no more than CRPATH_SZ and must begin
	 * with a '/'.
	 */
#else
	/*
	 *
	 * A fixed path is much better for testing.
	 */
	strcpy(cpeState.connReqPath, ACSCONNREQ_PATH);
#endif
	/*
	 * Set server timeout
	 */
	wget_SetServerTimeout( SERVER_CONNECT_TIMEOUT );

	/*
	 * Try to restore from configuration data.
	 */
	cpeRestoreCPEState();       /* need to get the ACS URL for cpeGetNetIFInfo() */
	ips = cpeGetNetIPInfo();
	restoreConfigData();		/* Restore all obj/parameter data before its over written by cpestate.xml */
	/*
	 * Get ACS URL and previous state data from cpestate.xml
	 *
	 */
	//cpeRestoreCPEState();       /* some ManagementServer data may have been overwritten - reread  the cpestate*/
	if ( EMPTYSTR(cpeState.acsURL)) {
		/* no ACS URL in configuration. Attempt to get it from DHCP vendor options */
		getDHCPVendorOptions();
	}
	cpeRefreshCPEData( &cpeState );

	if ( EMPTYSTR(cpeState.acsUser)) {
		if ( (ips) ) {
			/* create username from the MAC */
			char mac[30];
			char buf[40];
			snprintf(mac, sizeof(mac), "%s", writeCanonicalMacUCase(ips->mac));
			strncpy(buf, mac, 6);
			strncpy(buf+6, "-", 1);
			strcpy(buf+7, &mac[6]);
			cpeState.acsUser = GS_STRDUP(buf);
			if (cpeState.acsPW == NULL) {
				char pw[20];
				strcpy(pw, &mac[6]);
				cpeState.acsPW = GS_STRDUP(pw);
			}
		}
	}

	/* init Parameter Notification Tracking data */
	cwmpInitNotifyTracking(INIT_NOTIFY); /* first set default attributes */
	cwmpRestoreNotifyAttributes(); /* this will call cpeRestoreNotifyAttributes() to read */
	                               /* data saved by cpeSaveNotifyAttributes               */
	cwmpInitNotifyTracking(UPDATE_COPY); /* now make a tracking copy of all ACTIVE or PASSIVE param data*/
	cpeSaveInterfaceConfig();
	cpeSaveConfig();

#ifdef CONFIG_RPCKICK
	extern void cwmpStartKickListener(const char *, InAddr *);
	/*
	 *Need to define the interface and IP address of where to listen for
	 * kick redirects. This requires local http target to handle the
	 * redirects send to the a browser on the LAN.
	 */
	const char lanInterface[]="eth0"; /* use */
	InAddr lanIP;
	readInIPAddr( &lanIP, "192.168.10.105");
	cwmpStartKickListener(lanInterface, &lanIP);
#endif

}
