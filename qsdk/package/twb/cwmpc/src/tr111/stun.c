/*----------------------------------------------------------------------*
 * Gatespace
 * Copyright 2008 Gatespace Networks, Inc., All Rights Reserved.
 * Gatespace Networks, Inc. confidential material.
 *----------------------------------------------------------------------*
 * File Name  : stun.c
 * Description:	Implementation of stun client
 *----------------------------------------------------------------------*
 * $Revision: 1.10 $
 *
 * $Id: stun.c,v 1.10 2012/05/10 17:38:08 dmounday Exp $
 *----------------------------------------------------------------------*/
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/nameser.h>
#include <resolv.h>
#include <net/if.h>
#include <errno.h>
#include <syslog.h>

#ifdef DMALLOC
	#include "dmalloc.h"
#endif
#include "../includes/sys.h"
#include "../includes/CPEWrapper.h"
#include "../gslib/src/utils.h"
#include "../gslib/src/event.h"
#include "../soapRpc/cwmpSession.h"
#include "stun.h"
#include "tr111p2.h"
#include <openssl/hmac.h>

#define DEBUG
#ifdef DEBUG
#define DBGPRINT(X) fprintf X
#else
#define DBGPRINT(X)
#endif

void cpeSaveConfig(void);
extern CPEState cpeState;
extern char **cwmpConnReqUser;        /* linkage to Connection Request Credentials */
extern char **cwmpConnReqPW;
extern StunState stunState;
extern void *acsSession;
int dns_lookup(const char *name, int sockType, int family, InAddr *res);

static void sendBindRequest(void *handle);
static void sendDiscovery(void *);
static void sendDiscoveryTO(void *);
static void discoveryKeepAlive(void);
static void restartStun(void *handle);

typedef struct UDPConnReq {
	int		ts;	/*   */
	int		id;
	char	*cn;
	char 	*un;
	char	*sig;
} UDPConnReq;

static UDPConnReq udpReq;

#define STUN_BIND_SZ 256
static int reqBindRetries;
static StunResponse stunRsp;
static UInt8 stunMsg[STUN_BIND_SZ];
static int trialDelay;
static int hiDelay;
static int lowDelay;
static int monitorDelay;
static int pendingBindingChange;
static int pendingIntegrityRsp;
static int crInDiscovery;	/* set if Conn Req. in discovery phase*/

typedef enum {
	eDiscoveryInitial = 0,
	eDiscoveryStarted,
	eDiscoveryStopped,
	eDiscoveryDone
} DiscState;
static DiscState discState;

static enum {
	eNoMessage,
	eWaitingResponse,
	eResponseTO,
} discMsgState;

static UInt8 lastTid[16];
/*
 * Returns:
 *        0: No mapping change.
 *        1: NAT mapping changed.
 *
 */
static int updateStunState( StunResponse *sp )
{
	int changed = 0;
	if ( stunState.natIP != sp->mappedAddr.ipv4.addr
	   || stunState.natPort != sp->mappedAddr.ipv4.port ){
		/* mapping changed */
		changed =  1;
		stunState.natIP = sp->mappedAddr.ipv4.addr;
		stunState.natPort = sp->mappedAddr.ipv4.port;
		DBGPRINT((stderr, "NAT mapping changed\n"));
//		cwmpStartACSInform(); //Update Inform when changed
	}
	if ( sp->mappedAddr.ipv4.addr != stunState.cpeIP
	    || sp->mappedAddr.ipv4.port != UDPCONNECTIONREQPORT ) {
	    /* NAT is enabled */
	    changed |= (stunState.natDetected != 1);
	    stunState.natDetected = 1;
 		DBGPRINT((stderr, "NAT detected\n"));
	} else {
		changed = stunState.natDetected == 1;
		stunState.natDetected = 0;
		DBGPRINT((stderr, "No NAT detected\n"));
	}
    
	if(changed) /*if NAT mapping change save the confg file to mentatin the state of UDP connection accross the reboot */
	cpeSaveConfig();    
    
	return changed;
}
/*
 * Called by the framework following the end of ACS session.
 */
static void checkStunStateUpdate( void *handle ){
	stopCallback(&acsSession, checkStunStateUpdate, NULL);
	/* only allow changes of parameter data if no ACS session */
	if ( (pendingBindingChange = updateStunState( &stunRsp )) ){
		/* something changed */
		DBGPRINT((stderr, "%d: Binding changed\n", (int)time(NULL)));
		if ( stunState.natDetected )
			sendBindRequest(NULL);
		tr111p2ActiveNotify();    /* something changed - chk for active notify*/
	} else if ( stunState.natDetected )
		discoveryKeepAlive();
}

/*
 * Return 1 if calculated digest equal to sig
 */
static int verifySig( StunState *ss, char *text, int textLth, const char *sig )
{
    int rs;
    uint8_t msgDigest[20];
    char digBuf[41];
    unsigned int hmacSize;
    char *pw = *cwmpConnReqPW;

    if ( pw ) {
		HMAC(EVP_sha1(), pw, strlen(pw),
		     (const unsigned char*)text, textLth,
	                          msgDigest, &hmacSize);

		toAsciiHex(msgDigest, 20, digBuf);
		digBuf[40] = '\0';
		rs = ( strcasecmp( digBuf, sig)==0 );
	#ifdef DEBUG
	    DBGPRINT((stderr, "Message_Digest = %s\n", digBuf ));
	    DBGPRINT((stderr, "signature      = %s rs=%d\n", sig, rs));
	#endif
    } else {
    	DBGPRINT((stderr, "No Conn Request pw set - oK anything\n"));
    	rs = 1;
    }
    return rs;
}


char *getKeyValue( const char *qp, char *key ){
	char *vs;
	char *vcpy = NULL;
	int  lth;
	if ( (vs = strstr( qp, key)) !=NULL) {
		lth = strcspn( (vs=vs+ strlen(key)), " &");
		vcpy = GS_STRNDUP( vs, lth);
	}
	return vcpy;
}

/*
 * parseConnectionRequest
 * The spec says that the form of the request URI must be absolute form:
 *  absolute-URI  = scheme ":" hier-part [ "?" query ]
 *  GET http://xxx.xx.xx.xx:pppp?ts=nnnnnnn&un=nnnnnn&cn=xxxxxx&sig=aaaaaaaaaa HTTP1.1
 *
 * Returns: -1 Parse failed bad format.
 * 			0: Duplicate of previous request.
 * 			1: New validated connection request.
 *             Copy new UDPConnReq data over old saved data.
 *
 */

static int parseConnectionRequest(  StunState *ss, UDPConnReq *ur, UInt8 *sMsg, int lth )
{
	int  rstat = -1;
	char *cp, *qp, *ts, *id;
	char text[512];
	UDPConnReq udpReq;

	ts = id = NULL;
	memset( &udpReq, 0, sizeof(UDPConnReq));
	if ( strncmp((const char *)sMsg, "GET", 3)!= 0 ) //-Werror=pointer-sign
		return -1;
	if ( strstr((const char *)sMsg, "HTTP/1.1") == NULL) //-Werror=pointer-sign
		return -1;

	if ( (cp = strstr((const char *)sMsg, "http://")) == NULL) //-Werror=pointer-sign
		return -1;
	/* probably should look at authority value here */
	if ( (qp = strchr( cp, '?')) != NULL ) {
		/* found query portion */
		/* look for ts */
		if ( (ts = getKeyValue( qp, "ts=" )) ){
			udpReq.ts = atoi(ts);
		}
		if ( (id = getKeyValue( qp, "id=" )) ){
			udpReq.id = atoi(id);
		}
		udpReq.un = getKeyValue( qp, "un=" );
		udpReq.cn = getKeyValue( qp, "cn=" );
		udpReq.sig = getKeyValue(qp, "sig=");
		if (  (udpReq.id == ur->id)
			&&(udpReq.ts == ur->ts)
	 		&&( ur->cn && (strcmp(udpReq.cn, ur->cn )== 0)))
		{
				rstat = 0;  /* duplicate of previous request */
 		} else {
 			/* compare usernames */
 			char *crun = *cwmpConnReqUser;
 			if ( crun && udpReq.un && (strcmp(crun, udpReq.un)==0) ) {
 				/* Connection request user defined and equal to UDP packet un*/
	 			snprintf(text, sizeof(text), "%s%s%s%s", ts, id, udpReq.un, udpReq.cn);
	 			rstat = verifySig( ss, text, strlen(text), udpReq.sig );
	 		   	if (rstat == 1) {
	 		   		if ( ur->un ) GS_FREE (ur->un);
					if ( ur->cn ) GS_FREE (ur->cn);
					if ( ur->sig ) GS_FREE (ur->sig);
	 		   		/* good new connect request- update saved values */
	 		   		memcpy( ur, &udpReq, sizeof( UDPConnReq ));
	 		   	}
 			} else {
 				/* No match on username. If no Connection Request user allow all */
 				rstat = (crun == NULL);
 			}
 		}
	}
	GS_FREE(ts);
	GS_FREE(id);
	if ( rstat <= 0 ){
		if ( udpReq.un ) GS_FREE (udpReq.un);
		if ( udpReq.cn ) GS_FREE (udpReq.cn);
		if ( udpReq.sig ) GS_FREE (udpReq.sig);

	}
	return rstat;
}

static void parseIPAddr( StunAtrAddress4 *sa, UInt8 *p )
{
	sa->family = *(p+1);
	sa->ipv4.port = ntohs( *(UInt16 *)(p+2) );
	sa->ipv4.addr = ntohl( *(UInt32 *)(p+4) );

}
static void parseErrorCode( StunResponse *sr, UInt8 *p )
{
	int val = *(p+3);
	sr->errorCode = *(p+2)*100 + val;
}

/*
 * Returns
 *        1: Valid STUN msg type.
 *       -1: BindResponseErrorMessage or a message format error.
 *           errorCode is value of STUN error if not -1.
 *       -2: TID doesn't match expected message.
 *        0: Possible connection Request message.
 */

int parseStunMsg( StunResponse *sr, UInt8 *sMsg, int lth )
{
	StunMsgHdr *sh = (StunMsgHdr *)sMsg;

	memset( sr, 0, sizeof(StunResponse));
	if ( ntohs(sh->msgType) == BINDRESPONSEMSG
	   && ( ntohs(sh->msgLength)+sizeof(StunMsgHdr) <= lth )) {
		/* type and lth are ok */
		UInt8 *p = sMsg+ sizeof(StunMsgHdr);
		sr->errorCode = 0;
		if ( memcmp( &sh->tid, lastTid, 16) != 0 )
			return -2;
		while (p < sMsg+lth) {
			StunAtrHdr *ah = (StunAtrHdr *)p;
			UInt16 type = ntohs(ah->type);
			UInt16 alth = ntohs(ah->length);
			if ( type == MAPPEDADDRESS )
				parseIPAddr( &sr->mappedAddr, ah->octet );
			if ( type == SOURCEADDRESS )
				parseIPAddr( &sr->sourceAddr, ah->octet );
			if ( type == CHANGEDADDRESS )
				parseIPAddr( &sr->changedAddr, ah->octet );
			if ( type == REFLECTEDFROM )
				parseIPAddr( &sr->reflectFrom, ah->octet );
			/* increment ah to next attribute header */
			p = p + alth + sizeof(StunAtrHdr);
		}
		return 1;
	} else if ( ntohs(sh->msgType) == BINDERRORRESPONSEMSG ) {
		StunAtrHdr *ah = (StunAtrHdr *)(sMsg + sizeof(StunMsgHdr));
		parseErrorCode( sr, ah->octet );
		DBGPRINT((stderr, "STUN return error code = %d\n", sr->errorCode));
		return -1;
	} else if ( *sMsg == 'G' ) {
		sr->errorCode = 0;
		return 0;
	}
	sr->errorCode = -1;
	return -1;

}
static void createTransID( UInt128 *tid )
{
	UInt8 *p = (UInt8 *)tid;
	int i;
	for ( i=0; i<16; i=i+4 ) {
		int r = rand();
		p[i+0]= r>>0;
      	p[i+1]= r>>8;
		p[i+2]= r>>16;
		p[i+3]= r>>24;
   }
}
/*
 * Return total byte size of attribute.
 */

static void createAttrIntegrity( UInt8 *msg, int lth){
	UInt8 *p = msg + lth;
	UInt8 hmac[20];
	int   hmacSize;
	int	  multiple;

#ifdef DEBUG
	char	dbuf[256];
	toAsciiHex(msg, 64, dbuf);
	fprintf(stderr, "text: %s\n", dbuf);
	toAsciiHex((const unsigned char*)stunState.password, strlen(stunState.password), dbuf); //-Werror=pointer-sign
	fprintf(stderr, "key:  %s\n", dbuf);
#endif
	multiple = (lth+63)/64;
	/* stunMsg is zeroed prior to building so just use as is for padded 64 bytes*/
	HMAC(EVP_sha1(), stunState.password, strlen(stunState.password),
	     (const unsigned char*)msg, 64*multiple,
                          (unsigned char *)hmac, (unsigned int *)&hmacSize); //-Werror=pointer-sign
#ifdef DEBUG
	toAsciiHex(hmac, 20, dbuf);
	fprintf(stderr, "hmac: %s\n", dbuf);
#endif

	*(UInt16 *)p = htons( MESSAGEINTEGRITY );
	p += sizeof(UInt16);
	*(UInt16 *)p = htons(20);
	p += sizeof(UInt16);
	memcpy(p, hmac, 20);
	return;
}
/*
 * Return total byte size of attribute
 */
static int createAttrStr( UInt8 *bp, UInt16 attrType, char *aStr){
	int lth, ml, i;
	UInt8 *p = bp;

	*(UInt16 *)p = htons( attrType );
	p += sizeof(UInt16);
	lth = strlen(aStr);
	if ( (ml=lth%4) )
		ml = 4-ml+lth;
	else
		ml = lth;
	*(UInt16 *)p = htons(ml); /* round up lth */
	p += sizeof(UInt16);
	for (i=0; i<ml; ++i)
		*p++ = (i<lth)? aStr[i]: ' '; /* and pad out str */
	return ml+sizeof(StunAtrHdr);
}

static int createAttrAddr( UInt8 *bp, UInt16 attrType, int IP,
                          short int port){
	UInt8 *p = bp;

	*(UInt16 *)p = htons( attrType );
	p += sizeof(UInt16);
	*(UInt16 *)p = htons( sizeof (StunAddress4) );
	p += sizeof(UInt16);
	*(UInt16 *)p = htons(IPV4FAMILY);
	p += sizeof(UInt16);
	*(UInt16 *)p = htons(port);
	p += sizeof(UInt16);
	*(UInt32 *)p = htonl(IP);
	return sizeof(StunAddress4)+sizeof(StunAtrHdr);
}

int buildBindReq(StunState *ss, int attMask ) {
	int lth = sizeof( StunMsgHdr );

	memset( &stunMsg, 0, sizeof(stunMsg));
	StunMsgHdr *sh = (StunMsgHdr *)stunMsg;
	sh->msgType = htons( BINDREQUESTMSG );
	createTransID( &sh->tid );
	memcpy( lastTid, &sh->tid, 16);
	if (attMask&ADD_USERNAME &&	ss->username) {
		lth += createAttrStr( &stunMsg[lth], USERNAME, ss->username);
	}
	if (attMask&ADD_CONNREQ )
		lth += createAttrStr( &stunMsg[lth], CONNECTIONREQUESTBINDING,
				"dslforum.org/TR-111 ");
	if (attMask&ADD_RESPONSEADDR)
		lth += createAttrAddr( &stunMsg[lth], RESPONSEADDRESS, ss->natIP,
				ss->natPort);
	if (attMask&ADD_BINDINGCHANGE)
		lth += createAttrStr( &stunMsg[lth], BINDINGCHANGE, "");
	/* INTEGRITY must be the last attribute */
	if (attMask&ADD_INTEGRITY){
		/* set length so its calculated in the HMAC-SHA1 */
		int bindex = lth;
		lth += sizeof(StunAtrHdr)+20;
		sh->msgLength = htons(lth - sizeof(StunMsgHdr));
		createAttrIntegrity( stunMsg, bindex);
	} else
		sh->msgLength = htons(lth - sizeof(StunMsgHdr));
	return lth;
}


/*
 * Received a ReflectedFrom response to a sendDiscovery().
 * Increase the maximum delay between monitor
 * messages at this point. When timer expires try to
 * send another discovery bind request.
 */
static void updateRecvReflected(void)
{
	stopTimer(sendDiscoveryTO, NULL);
	if ( crInDiscovery>0){
		/* a CR was received and may have disrupted timing in NAT */
		/* retry last interval  time */
		crInDiscovery = 0;
		DBGPRINT((stderr, "%d: updateRecvReflected - CR msg replay= %d\n", (int)time(NULL), trialDelay));
		setTimer(sendDiscovery, NULL, trialDelay*1000 );
	}else if ( abs(trialDelay-hiDelay) < 5 ){
		/* we are done searching */
		monitorDelay = trialDelay;
		discState = eDiscoveryDone;
		DBGPRINT((stderr, "%d: updateRecvReflected - discovered delay = %d\n", (int)time(NULL), monitorDelay));
		setTimer( sendBindRequest, NULL, monitorDelay*1000);
	} else {
		lowDelay = trialDelay;
		trialDelay = (hiDelay-trialDelay)/2 + lowDelay;
		DBGPRINT((stderr, "%d: updateRecvReflected - wait %d\n", (int)time(NULL), trialDelay ));
		setTimer(sendDiscovery, NULL, trialDelay*1000);
	}
}

static void discoveryKeepAlive(void)
{
	if ( discState == eDiscoveryInitial ) {
		/* never stated yet */
		if ( stunState.maxKeepAlive != stunState.minKeepAlive ) {
			if ( stunState.dfd <= 0 ) { /* not started so start now */
				/* no port specified - this one just sends */
				InAddr anyAddr= {AF_INET, {{htonl(INADDR_ANY)}}};
				if ((stunState.dfd = udp_listen(&anyAddr, 0, NULL, 0)) < 0) {
					cpeLog(LOG_ERR, "Could not create socket for DiscoverKeepAlive()");
					return;
				}
				/* reset all search times */
				hiDelay = (stunState.maxKeepAlive==0 || stunState.maxKeepAlive==-1)?
				                MAXKEEPALIVE: stunState.maxKeepAlive;
				lowDelay = stunState.minKeepAlive>MIN_DISCOVERY_TIME? stunState.minKeepAlive:
								MIN_DISCOVERY_TIME;
				trialDelay = hiDelay;
				if ( (hiDelay-lowDelay) < MIN_DISCOVERY_DELTA ){
					monitorDelay = lowDelay;
					discState = eDiscoveryStopped;
					DBGPRINT((stderr, "no discovery possible hiDelay=%d lowDelay=%d\n", hiDelay, lowDelay));
					setTimer(sendBindRequest, NULL, monitorDelay*1000);
				} else {
					discState = eDiscoveryStarted;
					DBGPRINT((stderr, "%d: start discovery in %d\n", (int)time(NULL), hiDelay));
					setTimer(sendDiscovery, NULL, hiDelay*1000);
				}
			}
		} else if ( stunState.minKeepAlive ) {
			monitorDelay = stunState.minKeepAlive;
			setTimer(sendBindRequest, NULL, monitorDelay*1000);
		} else
			discState = eDiscoveryDone;
	} else if ( discState == eDiscoveryStarted ) {
		DBGPRINT((stderr, "%d: eDiscoveryStarted Next discovery msg in %d\n", (int)time(NULL), trialDelay));
		setTimer( sendDiscovery, NULL, trialDelay*1000 );
	} else if ( discState == eDiscoveryDone ) {
		DBGPRINT((stderr, "%d: eDiscoveryDone monitor time=%d\n", (int)time(NULL), monitorDelay));
		setTimer( sendBindRequest, NULL, monitorDelay*1000);
	}
}

/*
 * This is the rapid re-send that is a result of
 * no immediate response to the binding request.
 */

static int retryTO;
static void sendBindReqTO(void *handle)
{
	DBGPRINT((stderr, "%d: sendBindReqTO() retryTO=%d\n", (int)time(NULL), retryTO));
	if ( reqBindRetries == 9 ) {
		++reqBindRetries;     /* on last retry wait for 1.6 sec */
		setTimer( sendBindReqTO, NULL, 1600 /*1.6 sec*/ );
	} else if (reqBindRetries > 9 ) {
		reqBindRetries = retryTO = 0;  /* now wait for 10 minutes */
		stopStun();
		setTimer( restartStun, NULL, 60*5*1000 /* 5 min */);
	} else {
		++reqBindRetries;
		if (retryTO==0)
			retryTO = 100;
		retryTO += retryTO;
		sendBindRequest(handle);
	}
}
/**
 * Reset the Bind attempt state.
 */
static void resetBindState(void){
	reqBindRetries = retryTO = 0;
	stopTimer(sendBindReqTO, NULL);
	stopTimer(sendDiscoveryTO, NULL);
}
/*
 * Note that the error responses for a Discovery are returned on
 * the sending socket. stunState.dfd.
 */

static void rcvdResponse(void *handle)
{
	int sz;
	int pstat;
	int fd = (int) handle;

	DBGPRINT((stderr, "%d: rcvdResponse(%s)\n", (int)time(NULL), stunState.sfd == fd? "bind req": "discovery"));
	if ( (sz=read(fd, stunMsg, sizeof(stunMsg)))> sizeof(StunMsgHdr)){
		if ( (pstat = parseStunMsg( &stunRsp, stunMsg, sz)) == 1){
			resetBindState();
			pendingIntegrityRsp =0;
			DBGPRINT((stderr, "Mapped-address(network order)=%s:%d\n",
					inet_ntoa( (*(struct in_addr *)(&stunRsp.mappedAddr.ipv4.addr) )),
						stunRsp.mappedAddr.ipv4.port));
			DBGPRINT((stderr, "Reflected-From(network order)=%s:%d\n",
					inet_ntoa( (*(struct in_addr *)(&stunRsp.reflectFrom.ipv4.addr) )),
						stunRsp.reflectFrom.ipv4.port));
			if ( discMsgState == eWaitingResponse ) {
				/* this is a reflected response - do not update Stunstate. */
				discMsgState = eNoMessage;
				updateRecvReflected();
			} else {
				if ( !cwmpIsACSSessionActive() ){
				/* only allow changes of parameter data if no ACS session */
					if ( (pendingBindingChange = updateStunState( &stunRsp )) ){
						/* something changed */
						DBGPRINT((stderr, "%d: Binding changed\n", (int)time(NULL)));
						if ( stunState.natDetected )
							sendBindRequest(NULL);
						tr111p2ActiveNotify();    /* something changed - chk for active notify*/
					} else if ( stunState.natDetected )
						discoveryKeepAlive();
				} else {
					/* active session with ACS so delay until end-of-session */
					setCallback(&acsSession, checkStunStateUpdate, NULL);
				}
			}
		} else if (pstat == 0) {
			resetBindState();
			DBGPRINT((stderr, "UDP connection request received\n"));
			/* parseConnReq */
			crInDiscovery++;   /* set flag that a CR was received */
			if ( parseConnectionRequest( &stunState, &udpReq, stunMsg, sz )> 0)
				tr111p2ConnReq();
		} else if (pstat == -2) {
			DBGPRINT((stderr, "TID doesn't match expected response. Ignore packet.\n"));
		} else if (pstat == -1 && stunRsp.errorCode == 401 ) {
			DBGPRINT((stderr, "STUN server authentication request\n"));
			if ( stunState.username && stunState.password) {
				resetBindState();
				if (!pendingIntegrityRsp ){
					pendingIntegrityRsp = 1;
					sendBindRequest(NULL);
				} else {
					/* integrity chk failed on server - Use timeout delay seq. */
					pendingIntegrityRsp = 0;
					setTimer( sendBindReqTO, NULL, 5*1000 );
				}
			} else {
				cpeLog(LOG_ERR, "STUN Server is requesting authentication credentials\n");
				stopStun();
			}
		} else
			cpeLog(LOG_ERR, "STUN Binding-Response error code =%d", stunRsp.errorCode);
	} else
		cpeLog( LOG_ERR, "STUN socket read failed: %s", strerror(errno));

}

static void sendBindRequest(void *handle)
{
	int	attrMask = ADD_CONNREQ|ADD_USERNAME;
	struct sockaddr_in addr;
	int    sz, outsz;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(stunState.serverPort);
	addr.sin_addr.s_addr = htonl(stunState.stunServerIP);
	if ( pendingBindingChange )
		attrMask |= ADD_BINDINGCHANGE;
	if ( pendingIntegrityRsp )
		attrMask |= ADD_INTEGRITY;
	sz = buildBindReq( &stunState, attrMask );
	if ( (outsz = sendto( stunState.sfd, &stunMsg, sz, 0, (struct sockaddr *)&addr, sizeof (struct sockaddr_in)))<0){
		cpeLog(LOG_ERR,"Failed to send STUN BindRequest");
		DBGPRINT((stderr,"Failed to send STUN BindRequest\n"));
		stopStun();
		setTimer( restartStun, NULL, 60*5*1000 /* 5 min */);
	} else {
		DBGPRINT((stderr, "%d: sendBindRequest() Sent %d bytes\n", (int)time(NULL), outsz));
		setListener(stunState.sfd, rcvdResponse, (void *)stunState.sfd);
		setTimer(sendBindReqTO, NULL , (retryTO+300) /*ms*/);
	}
}

static int discRetryTO;
static int discRetries;
static void sendDiscoveryTO(void *handle)
{
	DBGPRINT((stderr, "%d: sendDiscoveryTO() discRetries = %d discRetryTO=%d\n", (int)time(NULL), discRetries, discRetryTO));
	if ( discRetries >= 4 ){ /* 0 .. 4 is 300ms to 1.6ms */
		/* Reflected Binding Timeout */
		/* update delays and send new bindReq */
		discRetries = discRetryTO = 0;
		hiDelay = trialDelay;
		trialDelay = trialDelay - (trialDelay-lowDelay+1)/2;
		if (trialDelay < stunState.minKeepAlive+2){
			cpeLog(LOG_ERR, "Discovery Time is less than Minimum Interval");
			discMsgState = eNoMessage;
			discState = eDiscoveryDone;
			monitorDelay = stunState.minKeepAlive;
		} else
			discMsgState = eResponseTO;
		sendBindRequest(NULL);
	} else {
		++discRetries;
		if (discRetryTO==0)
			discRetryTO = 100;
		discRetryTO += discRetryTO;
		sendDiscovery(NULL);
	}
	return;
}

static void sendDiscovery(void *handle)
{
	struct sockaddr_in addr;
	int    sz, outsz;
	int	attrMask = ADD_RESPONSEADDR;

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(stunState.serverPort);
	addr.sin_addr.s_addr = htonl(stunState.stunServerIP);

	sz = buildBindReq( &stunState, attrMask );
	if ( (outsz = sendto( stunState.dfd, &stunMsg, sz, 0, (struct sockaddr *)&addr, sizeof (struct sockaddr_in))) <0 ){
		cpeLog(LOG_ERR,"Failed to send STUN Discovery BindRequest");
		stopStun();
		setTimer( restartStun, NULL, 60*5*1000 /* 5 min */);
	} else {
		DBGPRINT((stderr, "%d: send Discovery BindRequest() Sent %d bytes\n", (int)time(NULL), outsz));
		discMsgState = eWaitingResponse;
		setTimer(sendDiscoveryTO, NULL, discRetryTO+300 /*ms*/);
		setListener(stunState.dfd, rcvdResponse, (void *)stunState.dfd);
	}
}

static void restartStun(void *handle){
    int ret = reStartStun();
    if ( 0 == ret)
    {
        DBGPRINT((stderr, "restartStun Timer : 5 min\n"));
        setTimer( restartStun, NULL, 60*5*1000 /* 5 min */);
    }
    else if ( 1 == ret)
    {
        DBGPRINT((stderr, "Start Stun.\n"));
    }
    else if ( -1 == ret)
    {
            DBGPRINT((stderr, "Detect IPv6 , Stun stop.\n"));
    }

}

/* called from a restart timer
 *
 */
int reStartStun(void)
{
	DBGPRINT((stderr,"restartStun()\n"));
	stopStun();
	return startStun();
}


void stopStun(void)
{
	DBGPRINT((stderr,"stopStun()\n"));
	/* cancel timers */
	stopTimer(sendDiscoveryTO, NULL);
	stopTimer(sendBindReqTO, NULL);
	stopTimer(sendDiscovery, NULL);
	stopTimer(sendBindRequest,NULL);

	if ( stunState.sfd >0 ) {
		closeListenerFD(stunState.sfd);
		stunState.sfd = -1;
	}
	if ( stunState.dfd >0 ) {
		closeListenerFD(stunState.dfd);
		stunState.dfd = -1;
	}

	stopCallback(&cpeState.ipAddress, restartStun, NULL);
}

int startStun(void)
{
    retryTO = 0;
    discState = eDiscoveryInitial;
    if ( stunState.sfd <= 0 ) { /* check if stun already running */
        if ( cpeState.ipAddress.inFamily != AF_INET ){
            DBGPRINT((stderr,"STUN does not support IPv6 addresses\n"));
            cpeLog(LOG_ERR, "STUN does not support IPv6 addresses");
            return -1;
        }
        DBGPRINT((stderr,"startStun()\n"));
        stunState.cpeIP = cpeState.ipAddress.u.inAddr.s_addr;
        COPYSTR(stunState.serverAddr, cpeState.stunURL);
//        stunState.serverPort = 3478;
//        stunState.minKeepAlive=30;
//        stunState.maxKeepAlive=45;
        /* setup to be notified if IP address changes */
        setCallback(&cpeState.ipAddress, restartStun, NULL);
        if ( stunState.cpeIP == 0 ){
            /* no local IP address yet wait until it changes to start */
            DBGPRINT((stderr,"startStun(): wait IP addr set\n"));
        } else 	if ( stunState.enable  && stunState.serverAddr ) {
            InAddr	srvrIP;
            dns_lookup( stunState.serverAddr, SOCK_DGRAM, AF_INET, &srvrIP);
            if ( srvrIP.inFamily!=0 ) {
                InAddr anyAddr= {AF_INET, {{htonl(INADDR_ANY)}}};
                stunState.stunServerIP = htonl( srvrIP.u.inAddr.s_addr);
                if ((stunState.sfd = udp_listen(&anyAddr, UDPCONNECTIONREQPORT, NULL, 0)) < 0) {
                    DBGPRINT((stderr,"Could not create socket for (port=%d)", UDPCONNECTIONREQPORT));
                    cpeLog(LOG_ERR, "Could not create socket for (port=%d)", UDPCONNECTIONREQPORT);
                    return 0;
                }
                DBGPRINT((stderr,"sendBindRequest()\n"));
                sendBindRequest(NULL);
                return 1;
            }
            else
                DBGPRINT((stderr,"Empty srvrIP.inFamily\n"));
        }
        else if ( !stunState.enable)
        {
            DBGPRINT((stderr,"Stun is not enable , enable Stun. \n"));
            stunState.enable = 1;
            if ( 1 == reStartStun())
                    return 1;
            else
                    return 0;
        }
        else
            DBGPRINT((stderr,"Empty StunState.serverAddr\n"));
    } else {
        DBGPRINT((stderr,"startStun(): stun already running\n"));
    }
    stopStun();
    return 0;
}

