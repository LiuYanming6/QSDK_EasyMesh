/* */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/un.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <signal.h>
#include <ctype.h>
#include "../includes/sys.h"
#include "../includes/paramTree.h"
#include "../includes/rpc.h"
#include "../soapRpc/rpcMethods.h"
#include "../soapRpc/rpcUtils.h"
#include "../soapRpc/cwmpSession.h"
#include "../gslib/src/utils.h"
//#include "../gslib/auxsrc/cpelog.h" //error: conflicting types for 'cpeInitLog' with CPEWrapper.h
#include "targetsys.h"
//#define CONFIG_RPCAUTONOMOUSTRANSFERCOMPLETE
#include "../includes/cpemsgformat.h"

#include "../includes/CPEWrapper.h" //For FAULTNONE define

CPEEvent cevt;

int get_domain_socket( const char *name){
	int fd;
	struct sockaddr_un addr;
	unlink(name);		// remove existing domain name
	memset(&addr, 0, sizeof(struct sockaddr_un));
	addr.sun_family = AF_UNIX;
	strcpy(addr.sun_path,name);
	if ((fd = socket(AF_UNIX, SOCK_DGRAM, 0))< 0){
		return -1;
	}
	if (bind( fd,(const struct sockaddr *) &addr, sizeof(struct sockaddr_un))< 0){
		close ( fd );
		return -1;
	}
	return fd;
}
int nargs;
void usage(void) {
#ifndef USE_UNIX_DOMAIN_SOCKET
	nargs = 4;
	printf("Use: cpereq <host> <port> <sig> [args]\n"
#else
	nargs = 2;
	printf("Use: cpereq <sig> [args]\n"
#endif
		"       where <sig> is one of \n"
		"       cr ConnectionRequest/GetRPCMethods:\n"
		"       dc DiagnosticComplete\n"
		"       vc Value Change\n"
		"       rd RequestDownload RPC\n"
		"       at AutonomousTransferCompleter RPC\n"
		"       acs new ACS URL arg[4] is new URL\n"
		"       nl new lease arg[4..7] is IP OUI SN PClass\n"
		"       ex expire lease arg[4..7] as above\n"
		"       stop Stop the CWMPc\n");
}

/*
 * send the stdin to the arg[1] ip and socket
 */
char *hostname;
int port = 30005;
int main(int argc, char *argv[]) {
	int fd;
	size_t insize = {0}; //-Werror=maybe-uninitialized
#ifndef USE_UNIX_DOMAIN_SOCKET //-Werror=unused-variable
	struct hostent *netent;
	int ip;
#endif
	int outsz = 0;
	int data;
	int ix;
	socklen_t  slen;
	if (argc < nargs) {
		usage();
		exit(-1);
	}
#ifndef USE_UNIX_DOMAIN_SOCKET
	hostname = argv[1];
	port = atoi(argv[2]);
	if (argc >= 4) {
		ix = 3;
#else
	if (argc >=2 ) {
		ix = 1;
#endif
		if (strcmp(argv[ix], "cr") == 0) {
			data = CPE_SENDINFORM;
		} else if (strcmp(argv[ix], "dc") == 0) {
			data = CPE_DIAGNOSTICS_COMPLETE;
		} else if (strcmp(argv[ix], "vc") == 0) {
			data = CPE_PARAMCHANGE;
		} else if (strcmp(argv[ix], "rd") == 0) {
			data = CPE_REQUESTDOWNLOAD;
		} else if (strcmp(argv[ix], "at") == 0) {
			data = CPE_AUTOXFERCOMPLETE;
		} else if (strcmp(argv[ix], "acs") == 0) {
			data = CPE_ACSURLCHANGE;
		} else if ( strcmp(argv[ix], "nl") == 0) {
			data = CPE_NEWLEASE;
		} else if ( strcmp(argv[ix], "ex") == 0) {
			data = CPE_EXPIREDLEASE;
		} else if ( strcmp(argv[ix], "stop") == 0) {
			data = CPE_STOPCWMPC;
		} else {
			usage();
			exit(-1);
		}
	}
#ifndef USE_UNIX_DOMAIN_SOCKET
	struct sockaddr_in addr;
	if ((netent = gethostbyname(hostname))) {
		ip = ntohl((int) *(int *) *netent->h_addr_list);
	}

	fprintf(stderr, "IP address = %x:%d  data=%0x\n", ip, port, data);
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = htonl(ip);
	slen = sizeof(struct sockaddr_in);
	if ((fd = socket(PF_INET,SOCK_DGRAM, IPPROTO_UDP)) < 0)
		abort();
#else
	struct sockaddr_un addr;
	memset(&addr, 0, sizeof(struct sockaddr_un));
	addr.sun_family = AF_UNIX;
	strcpy(addr.sun_path,CPELISTEN_NAME);
	slen = sizeof(struct sockaddr_un);
	if ((fd = get_domain_socket( CPELISTEN_NAME "sndrtest")) <0){
		fprintf(stderr, "Unable to get domain socket\n");
		abort();
	}
#endif
	memset(&cevt, 0, sizeof(CPEEvent));
	if (data == CPE_REQUESTDOWNLOAD) {
		/* RequestDownload requires some more data */
		cevt.eventCode = data;
		strcpy(cevt.ud.xLoadEvt.fileType, "1 Firmware Upgrade");
		strcpy(cevt.ud.xLoadEvt.argName, "testACSargName");
		strcpy(cevt.ud.xLoadEvt.argValue, "testACSargValueDatagoeshere");
		outsz = sendto(fd, &cevt, sizeof(cevt), 0, (struct sockaddr *) &addr,
				slen);
#ifdef CONFIG_RPCAUTONOMOUSTRANSFERCOMPLETE
	} else if ( data == CPE_AUTOXFERCOMPLETE ){
		cevt.eventCode = data;
		strcpy(cevt.ud.autoXferEvt.announceURL, "http://acs.gatespace.net/announcement");
		strcpy(cevt.ud.autoXferEvt.transferURL, "http://acs.gatespace.net/transferurl");
		strcpy(cevt.ud.autoXferEvt.fileType,  "1 Firmware Upgrade");
		strcpy(cevt.ud.autoXferEvt.targetFileName,  "target-file-name");
		cevt.ud.autoXferEvt.isDownload=1;
		cevt.ud.autoXferEvt.fileSize = 100009;
		cevt.ud.autoXferEvt.faultCode = 0;
		cevt.ud.autoXferEvt.faultMsg = FAULTNONE;
		cevt.ud.autoXferEvt.startTime = time(NULL)-10000;
		cevt.ud.autoXferEvt.completeTime = time(NULL);
		outsz = sendto(fd, &cevt, sizeof(cevt), 0,
				(struct sockaddr *) &addr, slen);
#endif
	} else if (data == CPE_ACSURLCHANGE) {
		/* send new ACS URL */
		cevt.eventCode = data;
		if (argc > 4) {
			strcpy(cevt.ud.acsChangeEvt.url, argv[4]);
			outsz = sendto(fd, &cevt, sizeof(cevt), 0,
					(struct sockaddr *) &addr, slen);
		} else {
			fprintf(stderr,"ACS URL not specified in arg 4\n");
			return -1;
		}
	} else if ( data == CPE_NEWLEASE || data == CPE_EXPIREDLEASE ){
		cevt.eventCode = data;
		strcpy( cevt.ud.dhcpOptions.ipAddress, argv[4]);
		strcpy( cevt.ud.dhcpOptions.oui, argv[5]);
		strcpy( cevt.ud.dhcpOptions.serialNumber, argv[6]);
		strcpy( cevt.ud.dhcpOptions.productClass, argv[7]);
		outsz = sendto(fd, &cevt, sizeof(cevt), 0, (struct sockaddr *) &addr,
				slen);
	} else {
		/* send just the one data word */
		outsz = sendto(fd, &data, sizeof(data), 0, (struct sockaddr *) &addr,
				slen);
	}
	fprintf(stderr, "Sent %d bytes %s\n", outsz, strerror(errno));
	return insize != outsz;

}

