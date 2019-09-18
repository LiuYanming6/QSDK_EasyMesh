/*
 * netIPState.c
 *
 *  Created on: Feb 11, 2009
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
//#include <stropts.h>
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

#include "WANDevice.h"

extern CPEState cpeState;
NetIPState cpeNetIPState;
const char NAMESERVER[]="nameserver";

const char *getMACAddrStr(void){
	return cpeNetIPState.mac;
}
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
static int getMACValue(char *interface, NetIPState *nState) {
	FILE 	*fp;
	char	buf[257];
	char	*p;
	if ( (fp=popen("/sbin/ifconfig", "r"))){
		while ( fgets(buf, sizeof(buf), fp)!=NULL) {
			if ( strstr(buf, interface)){   // should be "eth0   Link encap: ..."
				p = strstr(buf, "HWaddr "); // scan for mac string.
				readMac(nState->mac, p+7);
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
extern CWMPObject CWMP_RootObject[];

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


	if ((fp = popen("ip addr show", "r")) == NULL) {
		cpeLog(LOG_ERR, "Could not start 'ip addr show'");
		// set result;
		return -1;
	}
	// read pipe for results of ip addr show ...
	if ( fgets(buf, sizeof(buf), fp) != NULL )
		do {
			// scan lines for "1: lo: ", "2: eth0:", ...
			if ( isdigit(buf[0])) {
				// found I/F name
				int 	inet;
				char 	*e;
				char	*p;

				id = atoi(buf);			// The interface numbers are used for object instance numbers
										// which tie the instances to a specific interface.
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
							strcpy(ifstate, "Unknown");
					}
				}
				if ( fgets(buf, sizeof(buf), fp)!=NULL ){
					// this is link/ether line. Expects the following format:
					// link/ether 00:1e:4f:f5:75:1a brd ff:ff:ff:ff:ff:ff
					p = strstr(buf, "link/ether");
					if ( p ) {
						readMac(mac, p + strlen("link/ether "));
						ipcnt = 0;
						do { // look for inet and inet6 records (inet6 has 2 lines).
							// if IP address is equal to wanAddr then create WANConnectionDevice.{i}.
							// otherwise create a LANDevice.1.LANEthernetInterfaceConfig.{i}.
							Instance *ipip;
							if ( fgets(buf, sizeof(buf), fp)!=NULL ){
								p = strstr(buf, "inet6 ");
								if ( (inet = p!=NULL) ){
									++ipcnt;

									fgets(buf, sizeof(buf), fp); // valid_lft forever ...
								} else if ( (inet = (p = strstr(buf, "inet "))!=NULL)){
									p += strlen("inet ");
									++ipcnt;

								}
							} else {
								inet = 0;
							}
							// read until get the "valid_..." line.
						} while ( inet );
					} else {
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
	return ifCnt;
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
	getMACValue(nState->name, nState);
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

NetIPState *cpeGetNetIPInfo(int discovery){
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


void cpeNetInit(int discovery){
	char oName[257];
	Instance *wanIp, *lanIp, *wanConDevIp, *wanIPIp;
	if ( discovery ){
		snprintf(oName, sizeof(oName), "%s.LANDevice.1.", CWMP_RootObject[0].name );
		lanIp = cwmpInitObjectInstance(oName);
		snprintf(oName, sizeof(oName), "%s.WANDevice.1.", CWMP_RootObject[0].name );
		wanIp = cwmpInitObjectInstance(oName);
		cpeGetNetIPInfo(discovery);
		if ( !EMPTYSTR(cpeNetIPState.name)){
			snprintf(oName, sizeof(oName), "%s.WANDevice.1.WANConnectionDevice.1.", CWMP_RootObject[0].name );
			wanConDevIp = cwmpInitObjectInstance(oName);
			snprintf(oName, sizeof(oName),
				"%s.WANDevice.1.WANConnectionDevice.1.WANIPConnection.1.", CWMP_RootObject[0].name );
			wanIPIp = cwmpInitObjectInstance(oName);
			WANDeviceWANConnectionDeviceWANIPConnection *p = (WANDeviceWANConnectionDeviceWANIPConnection*)wanIPIp->cpeData;
			p->enable = 1;
			COPYSTR(p->connectionStatus, "Connected");
			COPYSTR(p->possibleConnectionTypes, "IP_Routed");
			COPYSTR(p->connectionType, "IP_Routed");
			COPYSTR(p->name, cpeNetIPState.name);
			p->uptime = time(NULL);
			COPYSTR(p->externalIPAddress, writeInIPAddr(&cpeNetIPState.ip) );
			COPYSTR(p->subnetMask, writeInIPAddr( &cpeNetIPState.mask) );
			/* Add LANDevice child objects if other interfaces are present. */
		}

	}
}

