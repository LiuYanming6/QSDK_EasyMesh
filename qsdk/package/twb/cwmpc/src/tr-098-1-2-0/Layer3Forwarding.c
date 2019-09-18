/*----------------------------------------------------------------------*
 * Gatespace
 * Copyright 2011, 2012 Gatespace Networks, Inc., All Rights Reserved.
 * Gatespace Networks, Inc. confidential material.
 *----------------------------------------------------------------------*
 * File Name  :
 * Description: Auto-generated getter/setter stubs file.
 *----------------------------------------------------------------------*
 * $Revision: 1.1 $
 *
 * $Id: Layer3Forwarding.c,v 1.1 2012/05/10 17:37:59 dmounday Exp $
 *----------------------------------------------------------------------*/
#include	<string.h>
#include 	<time.h>
#include 	<sys/types.h>
#include	<stdlib.h>
#include 	<arpa/inet.h>
#include 	<stdio.h>

#include "paramTree.h"
#include "rpc.h"
#include "gslib/src/utils.h"
#include "soapRpc/rpcUtils.h"
#include "soapRpc/rpcMethods.h"
#include "soapRpc/cwmpSession.h"
#include "sysutils.h"


/* InternetGatewayDevice.Layer3Forwarding.Forwarding.{i}. */
/* The following struct should be saved in persistent memory */
#define STATUS_DYN	    0x01
#define STATUS_ENABLEREQ  0x40    /* set by setXXEnable to enable     */
#define STATUS_DISABLEREQ 0x20  /* set by setXXEnable to disable    */
#define STATUS_ENABLED  0x80    /* the route has been added/enabled */

/* route flags ??
 */
#define R_UP	0x01
#define R_GW	0x02
#define R_HOST	0x04

typedef struct L3ForwardDesc {
	int     enable;
	char    status;			/* not writeable -- realtime status*/
	#if 0
	int    srcIP;
	int    srcMask;
	#endif
	char   *type;
	int    rFlags;
	InAddr destIP;
	InAddr destMask;
	InAddr gwIP;
	char    interface[33];
	int     metric;
    int     mtu;
} L3ForwardDesc;
/*
 * return:
 *      0: found copy of route
 * 	    1: added new L3 instance
 *     -1: internal error- couldn't find L3 object.
 */
static int mergeRoute(L3ForwardDesc *p, int enable){
	CWMPObject *o;
	Instance	*ip;

	if ( (o=cwmpFindObject("InternetGatewayDevice.Layer3Forwarding.Forwarding."))) {
		ip = o->iList;
		while( ip ){ /* walk down instance list looking for route */
			L3ForwardDesc *f = (L3ForwardDesc *)ip->cpeData;
			if ( f
			   && eqInIPAddr(&f->destIP, &p->destIP)    //TODO: IPv6 changes here....
			   && eqInIPAddr(&f->destMask, &p->destMask)
			   && eqInIPAddr(&f->gwIP, &p->gwIP)
			   && streq(f->interface, p->interface)) {
				f->enable = enable;
				GS_FREE( p );  /* already in Instance list */
				return 0;
			}
			ip = ip->next;
		}
		/* not in list add new instance */
		if ( (ip=cwmpCreateInstance( o, -1)) ){
			p->enable = enable;
			ip->cpeData = (L3ForwardDesc *)p;
			return 1;
		}
	}
	GS_FREE(p);
	return -1;
}

void updateL3Routing(void) {
	FILE *f;
	char line[256];
	char dIP[IP_ADDRSTRLEN];
	char gwIP[IP_ADDRSTRLEN];
	char dMask[IP_ADDRSTRLEN];
	unsigned refcnt;
	unsigned use;
	L3ForwardDesc *l3p;

	if ((f = fopen("/proc/net/route", "r"))) {
		while ((fgets(line, sizeof(line), f) !=NULL )) {
			/* iface destination gateway flags refcnt use metric mask mtu window irtt*/
			l3p = (L3ForwardDesc *) GS_MALLOC(sizeof(L3ForwardDesc));
			if (l3p != NULL) {
				memset(l3p, 0, sizeof(L3ForwardDesc));
				if (sscanf(line, "%s %s %s %x %x %x %x %s %x",
						l3p->interface, dIP, gwIP, &l3p->rFlags, &refcnt, &use,
						&l3p->metric, dMask, &l3p->mtu) == 9) {
					readInIPAddr(&l3p->destIP, dIP);
					readInIPAddr(&l3p->gwIP, gwIP);
					readInIPAddr(&l3p->destMask, dMask);
					if (l3p->rFlags & R_UP) {
						int r = mergeRoute(l3p, STATUS_ENABLED);
						if (r > 0) {
							fprintf(stderr, "Added Instance: %s", line);
						}
					} else
						GS_FREE(l3p);
				} else
					/* error free */
					GS_FREE(l3p);
			}
		}
		fclose(f);
	}
}
/*
 * It might be easier to handle the IP addresses and masks as
 * strings all the way thru. I left the conversions in to
 * support testing of the conversion routines, etc.
 */
static void manageRoute( L3ForwardDesc *l3p, int addDel ){
	char cmd[255];
	char mask[50];
	char ip[50];
	char gwIp[50];

	//strcpy(mask,writeIp(htonl(l3p->destMask)));
	strcpy(mask, writeInIPAddr( &l3p->destMask ));
	//strcpy(ip, writeIp(htonl(l3p->destIP)));
	strcpy(ip, writeInIPAddr( &l3p->destIP));
	//strcpy(gwIp, writeIp(htonl(l3p->gwIP)));
	strcpy(gwIp, writeInIPAddr( &l3p->gwIP));

	if ( l3p->destIP.inFamily==AF_INET
			&& l3p->destMask.inFamily==AF_INET
			&& l3p->gwIP.inFamily == 0 ){
		snprintf(cmd, sizeof(cmd), "-net %s netmask %s dev %s",
						ip, mask, l3p->interface);
	} else if ( l3p->destIP.inFamily==AF_INET
			&& l3p->gwIP.inFamily==AF_INET
			&& l3p->destMask.inFamily == AF_INET ){
		snprintf(cmd, sizeof(cmd), "-net %s netmask %s gw %s dev %s",
				ip, mask, gwIp, l3p->interface);
	} else if ( l3p->destIP.inFamily==0
			&& l3p->destMask.inFamily==0
			&& l3p->gwIP.inFamily==AF_INET ){
		snprintf(cmd, sizeof(cmd), "default gw %s dev %s",
						gwIp, l3p->interface);
	} else if ( l3p->gwIP.inFamily==0 ) {
		snprintf(cmd, sizeof(cmd), "-host %s", ip);
	} else {
		snprintf(cmd, sizeof(cmd), "-host %s gw %s dev %s",
				ip, gwIp, l3p->interface);
	}
	do_cmd(1, "route", "%s %s", addDel? "add": "del", cmd);
	// TODO:need to add code to redirect output of route to a file and */
	/* chk status of the command */
	return;
}

/**@obj Layer3ForwardingForwarding **/
CPE_STATUS  addLayer3ForwardingForwarding(CWMPObject *o, Instance *ip)
{
	L3ForwardDesc *l3fd;
	if ( (l3fd = (L3ForwardDesc *)GS_MALLOC(sizeof(struct L3ForwardDesc)))) {
		memset(l3fd, 0, sizeof(struct L3ForwardDesc));
		ip->cpeData = (void *)l3fd;	/* Link to cpe data */
		return CPE_OK;
	}
	return CPE_ERR;
}
CPE_STATUS  delLayer3ForwardingForwarding(CWMPObject *o, Instance *ip)
{
	L3ForwardDesc *l3fd;
	l3fd = (L3ForwardDesc *)ip->cpeData; /* find bcmDAta */
	manageRoute(l3fd, 0/*del*/);
	GS_FREE(l3fd->type);
	GS_FREE(l3fd);
	return CPE_OK;
}

CPE_STATUS  commitLayer3ForwardingForwarding(CWMPObject *o, Instance *ip, eCommitCBType cmt)
{
	L3ForwardDesc *l3fd = (L3ForwardDesc *)(ip->cpeData);
	if (cmt==COMMIT_SET && l3fd ){
		if ( l3fd->enable&STATUS_ENABLEREQ ){  /* enable request */
			l3fd->enable &= ~STATUS_ENABLEREQ; /* turn off req */
			if (! (l3fd->enable&STATUS_ENABLED) ) {
				manageRoute( l3fd, 1/*add*/);
				l3fd->enable |= STATUS_ENABLED;    /* Set enabled */
			}
		} else if ( l3fd->enable&STATUS_DISABLEREQ ) { /* DISABLE request*/
			l3fd->enable &= ~STATUS_DISABLEREQ;  /* turn off req */
			if ( l3fd->enable&STATUS_ENABLED ){
				manageRoute( l3fd, 0/*del*/);
				l3fd->enable &= ~STATUS_ENABLED; /* turn off*/
			}
		}
	}
	return CPE_OK;
}
/**@param Layer3ForwardingForwarding_Enable                     **/
CPE_STATUS setLayer3ForwardingForwarding_Enable(Instance *ip, char *value)
{
	L3ForwardDesc *l3fd;
	if ( (l3fd=(L3ForwardDesc *)ip->cpeData) ) {
		if ( testBoolean(value))
			l3fd->enable |= STATUS_ENABLEREQ;
		else
			l3fd->enable |= STATUS_DISABLEREQ;
		return CPE_OK;
	}
	return CPE_ERR;
}
CPE_STATUS getLayer3ForwardingForwarding_Enable(Instance *ip, char **value)
{
	L3ForwardDesc *l3fd;
	if ( (l3fd=(L3ForwardDesc *)ip->cpeData) ) {
		*value = GS_STRDUP(l3fd->enable&STATUS_ENABLED?"1":"0");
		return CPE_OK;
	}
	return CPE_ERR;
}
/**@endparam                                               **/
/**@param Layer3ForwardingForwarding_Status                     **/
CPE_STATUS getLayer3ForwardingForwarding_Status(Instance *ip, char **value)
{
	L3ForwardDesc *l3fd;
	if ( (l3fd=(L3ForwardDesc *)ip->cpeData)) {
            if ( l3fd->enable&STATUS_ENABLED )
            	*value = GS_STRDUP("Enabled");
            else
                *value = GS_STRDUP("Disabled");
            return CPE_OK;
	}
	return CPE_ERR;
}
/**@endparam                                               **/
/**@param Layer3ForwardingForwarding_Type                     **/
CPE_STATUS setLayer3ForwardingForwarding_Type(Instance *ip, char *value)
{
	/* set parameter */
	return CPE_OK;
}
CPE_STATUS getLayer3ForwardingForwarding_Type(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param Layer3ForwardingForwarding_DestIPAddress                     **/
CPE_STATUS setLayer3ForwardingForwarding_DestIPAddress(Instance *ip, char *value)
{
	L3ForwardDesc *l3fd;
	if ( (l3fd=(L3ForwardDesc *)ip->cpeData) ) {
		if ( strlen(value))
			readInIPAddr( &l3fd->destIP, value);
		else
			clearInIPAddr(&l3fd->destIP);
		return CPE_OK;
	}
	return CPE_ERR;
}
CPE_STATUS getLayer3ForwardingForwarding_DestIPAddress(Instance *ip, char **value)
{
	L3ForwardDesc *l3fd;
	if ( (l3fd=(L3ForwardDesc *)ip->cpeData) ){
		*value = GS_STRDUP(writeInIPAddr(&l3fd->destIP));
		return CPE_OK;
	}
	return CPE_ERR;
}
/**@endparam                                               **/
/**@param Layer3ForwardingForwarding_DestSubnetMask                     **/
CPE_STATUS setLayer3ForwardingForwarding_DestSubnetMask(Instance *ip, char *value)
{
	L3ForwardDesc *l3fd;
	if ( (l3fd=(L3ForwardDesc *)ip->cpeData) ){
		if (strlen(value))
			readInIPAddr( &l3fd->destMask, value);
		else
			clearInIPAddr(&l3fd->destMask);
		return CPE_OK;
	}
	return CPE_ERR;
}
CPE_STATUS getLayer3ForwardingForwarding_DestSubnetMask(Instance *ip, char **value)
{
	L3ForwardDesc *l3fd;
	if ( (l3fd=(L3ForwardDesc *)ip->cpeData) ) {
		*value = GS_STRDUP(writeInIPAddr(&l3fd->destMask));
		return CPE_OK;
	}
	return CPE_ERR;
}
/**@endparam                                               **/
/**@param Layer3ForwardingForwarding_SourceIPAddress                     **/
CPE_STATUS setLayer3ForwardingForwarding_SourceIPAddress(Instance *ip, char *value)
{
	/* set parameter */
	return CPE_OK;
}
CPE_STATUS getLayer3ForwardingForwarding_SourceIPAddress(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param Layer3ForwardingForwarding_SourceSubnetMask                     **/
CPE_STATUS setLayer3ForwardingForwarding_SourceSubnetMask(Instance *ip, char *value)
{
	/* set parameter */
	return CPE_OK;
}
CPE_STATUS getLayer3ForwardingForwarding_SourceSubnetMask(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param Layer3ForwardingForwarding_GatewayIPAddress                     **/
CPE_STATUS setLayer3ForwardingForwarding_GatewayIPAddress(Instance *ip, char *value)
{
	L3ForwardDesc *l3fd;
	if ( (l3fd=(L3ForwardDesc *)ip->cpeData) ){
		if(strlen(value))
			//l3fd->gwIP = ntohl(readIp(value));
			readInIPAddr( &l3fd->gwIP, value);
		else
			clearInIPAddr(&l3fd->gwIP);
		return CPE_OK;
	}
	return CPE_ERR;
}
CPE_STATUS getLayer3ForwardingForwarding_GatewayIPAddress(Instance *ip, char **value)
{
	L3ForwardDesc *l3fd;
	if ( (l3fd=(L3ForwardDesc *)ip->cpeData) ){
		*value = GS_STRDUP(writeInIPAddr(&l3fd->gwIP));
		return CPE_OK;
	}
	return CPE_ERR;
}
/**@endparam                                               **/
/**@param Layer3ForwardingForwarding_Interface                     **/
CPE_STATUS setLayer3ForwardingForwarding_Interface(Instance *ip, char *value)
{
	L3ForwardDesc *l3fd;
	if ( (l3fd=(L3ForwardDesc *)ip->cpeData) ) {
		// TODO: has to be set to the Paramter path name of interface
		strncpy(l3fd->interface, value, sizeof(l3fd->interface));
		return CPE_OK;
	}
	return CPE_OK;
	/*return CPE_ERR;*/
}
CPE_STATUS getLayer3ForwardingForwarding_Interface(Instance *ip, char **value)
{
	L3ForwardDesc *l3fd;
	if ( (l3fd=(L3ForwardDesc *)ip->cpeData) )
		*value = l3fd->interface? GS_STRDUP(l3fd->interface):NULL;
	else
		return CPE_ERR;
	return CPE_OK;
}
/**@endparam                                               **/
/**@param Layer3ForwardingForwarding_ForwardingMetric                     **/
CPE_STATUS setLayer3ForwardingForwarding_ForwardingMetric(Instance *ip, char *value)
{
	L3ForwardDesc *l3fd;
	if ( (l3fd=(L3ForwardDesc *)ip->cpeData) ){
			l3fd->metric=atoi(value);
			return CPE_OK;
	}
	return CPE_ERR;/* Set parameter */
}
CPE_STATUS getLayer3ForwardingForwarding_ForwardingMetric(Instance *ip, char **value)
{
	L3ForwardDesc *l3fd;
	if ( (l3fd=(L3ForwardDesc *)ip->cpeData) ){
	/* get parameter */
			*value = GS_STRDUP(itoa(l3fd->metric));
			return CPE_OK;
		}
	return CPE_ERR;
}
/**@endparam                                               **/
/**@param Layer3ForwardingForwarding_StaticRoute                     **/
CPE_STATUS getLayer3ForwardingForwarding_StaticRoute(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@endobj Layer3ForwardingForwarding **/

/**@obj Layer3Forwarding **/

/**@param Layer3Forwarding_DefaultConnectionService                     **/
CPE_STATUS setLayer3Forwarding_DefaultConnectionService(Instance *ip, char *value)
{
	/* set parameter */
	return CPE_OK;
}
CPE_STATUS getLayer3Forwarding_DefaultConnectionService(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@param Layer3Forwarding_ForwardNumberOfEntries                     **/
CPE_STATUS getLayer3Forwarding_ForwardNumberOfEntries(Instance *ip, char **value)
{
	cwmpGetObjInstanceCntStr("Forwarding", value);
	return CPE_OK;
}
/**@endparam                                               **/
/**@endobj Layer3Forwarding **/






