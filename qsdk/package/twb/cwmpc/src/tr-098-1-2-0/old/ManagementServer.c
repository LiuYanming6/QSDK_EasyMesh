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
 * $Id: ManagementServer.c,v 1.1 2012/05/10 17:38:08 dmounday Exp $
 *----------------------------------------------------------------------*/
#include "syslog.h"
#include "paramTree.h"
#include "rpc.h"
#include "gslib/src/utils.h"
#include "soapRpc/rpcUtils.h"
#include "soapRpc/rpcMethods.h"
#include "soapRpc/cwmpSession.h"
#include "../includes/CPEWrapper.h"

/**@obj ManagementServerManageableDevice **/
#include "includes/cpedata.h"
#define MAX_M_DEVICES   50

static ManageableDevice mDevs[MAX_M_DEVICES];

static int findUnused(void){
	int i=0;
	while ( i<MAX_M_DEVICES ){
		if ( mDevs[i].deviceData.ipAddress[0]=='\0' ){
			break;
		}
		++i;
	}
	return i;
}
/*
 * Search mDevs for the device. The IP address may have
 * changed so just compare device data.
 */
static int findDuplicateDevice(DHCPOptions *d){
	int i=0;
	while ( i<MAX_M_DEVICES  ){
		if ( mDevs[i].deviceData.ipAddress[0]!='\0' ){
			DHCPOptions *op = &mDevs[i].deviceData;
			if (   streq( d->oui, op->oui)
				&& streq( d->productClass, op->productClass)
				&& streq( d->serialNumber, op->serialNumber)) {
				return i;
			}
		}
		++i;
	}
	return i;
}
/*
 * New Lease awarded.
 */
void addNewManageableDevice( DHCPOptions *d){
	int		idx;

	if ( (idx = findDuplicateDevice( d )) < MAX_M_DEVICES ){
		/* just update the IP address */
		strcpy( mDevs[idx].deviceData.ipAddress, d->ipAddress);
		return; /* ignore duplicates */
	}

	idx = findUnused();
	if ( idx < MAX_M_DEVICES ){
		mDevs[idx].deviceData = *d;
		return;
	}
	cpeLog(LOG_ERR, "Unable to create Manageable Device instances");
}
/*
 * Lease expired or device removed.
 */
void deleteManageableDevice(DHCPOptions *d) {
	int i = 0;
	while (i < MAX_M_DEVICES) {
		if (streq(d->ipAddress, mDevs[i].deviceData.ipAddress)) {
			memset(&mDevs[i], 0, sizeof(struct ManageableDevice));
			break;
		}
		++i;
	}
}

void refresh_ManageableDevice(void){
	int i;
	CWMPObject *o;
	/* need to first delete existing instances */
	o = cwmpFindObject("InternetGatewayDevice.ManagementServer.ManageableDevice.");
	/* delete all the old ones */
	cwmpDeleteAllInstances(o);
	/* create */
	for (i=0; i<MAX_M_DEVICES; ++i){
		Instance *deviceIp;
		char devicePath[100];
		if ( mDevs[i].deviceData.ipAddress[0] ){
			snprintf(devicePath, sizeof(devicePath), "%s%d.", "InternetGatewayDevice.ManagementServer.ManageableDevice.", i+1);
			deviceIp = cwmpInitObjectInstance( devicePath );
			if (deviceIp){
				deviceIp->cpeData = (void *)i+1; /* cpeData is index+1 into mDevs table */
				mDevs[i].ip = deviceIp;
				fprintf(stderr,"Create ManageableDevice.%d.\n", i+1);
			}
		}
	}
}

/*
 * cwmpFindPath callback to locate all .Hosts.Hosts{i}. instances with the
 * IPaddress of the current mDevs IPaddress.
 */
extern CWMPObject CWMP_RootObject[];
extern char wpPathName[];
#define	MAX_HOSTPLTH 1024
#define IPADDRESS_NL    10
static int hostsCB(CWMPObject *o, CWMPParam *p, Instance *ip,
						void *hosts, void *unused, int mdevIdx )
{
	int		hlth;
	char	*h = (char *)hosts;
	int		plth;
	if ( streq( o->name, "Host") && p!=NULL && streq( p->name, "IPAddress")){
		/* if .Host.{i}.IPAddress */
		char *ipValue;
		fprintf(stdout, "hostCB at %s.%d %s\n", o->name, ip->id, p->name);

		if ( p->pGetter(ip, &ipValue)== CPE_OK){
			if ( streq( mDevs[mdevIdx].deviceData.ipAddress, ipValue)){
				fprintf(stdout, "Found Host.%d.IPAddress\n", ip->id );
				/* concat the current path to accumulated paths */
				hlth = strlen( h);
				if ( hlth ){
					/* comma separated */
					*(h+hlth) = ',';
					++hlth;
				}
				/* isolate the object path name from the IPAddress parameter name*/
				plth = strlen(wpPathName)-IPADDRESS_NL;
				if ( plth < MAX_HOSTPLTH-hlth ){
					strncpy(h+hlth, wpPathName, plth);
					*(h+hlth+plth) = '\0';
				}

			}
			GS_FREE( ipValue );
		}
	}
	return eOK;
}
/**@param ManagementServerManageableDevice_ManufacturerOUI                     **/
CPE_STATUS getManagementServerManageableDevice_ManufacturerOUI(Instance *ip, char **value)
{
	/* get parameter */
	int	idx =(int)ip->cpeData -1;    /* mDevs array index is biased by 1 */
	*value = GS_STRDUP( mDevs[idx].deviceData.oui );
	return CPE_OK;
}
/**@endparam                                               **/
/**@param ManagementServerManageableDevice_SerialNumber                     **/
CPE_STATUS getManagementServerManageableDevice_SerialNumber(Instance *ip, char **value)
{
	/* get parameter */
	int	idx =(int)ip->cpeData -1;    /* mDevs array index is biased by 1 */
	*value = GS_STRDUP( mDevs[idx].deviceData.serialNumber );
	return CPE_OK;
}
/**@endparam                                               **/
/**@param ManagementServerManageableDevice_ProductClass                     **/
CPE_STATUS getManagementServerManageableDevice_ProductClass(Instance *ip, char **value)
{
	/* get parameter */
	int	idx =(int)ip->cpeData -1;    /* mDevs array index is biased by 1 */
	*value = GS_STRDUP( mDevs[idx].deviceData.productClass );
	return CPE_OK;
}
/**@endparam                                               **/
/**@param ManagementServerManageableDevice_Host                     **/
CPE_STATUS getManagementServerManageableDevice_Host(Instance *ip, char **value)
{
	/* get parameter */
	return CPE_OK;
}
/**@endparam                                               **/
/**@endobj ManagementServerManageableDevice **/

/**@obj ManagementServer **/
extern CPEState cpeState;

/**@param ManagementServer_URL                                                      **/
CPE_STATUS setManagementServer_URL(Instance *ip, char *value)
{
        if ( !streq(cpeState.acsURL, value) ){
                if (cpeState.acsURL) GS_FREE(cpeState.acsURL);
                cpeState.acsURL = GS_STRDUP(value);
        if ( cwmpIsACSSessionActive() )   /* only set this if a session is active. May be initializing. */
                cwmpSetPending(PENDING_ACSCHANGE);
    }
    return CPE_OK;
}
CPE_STATUS getManagementServer_URL(Instance *ip, char **value)
{
        if ( cpeState.acsURL)
                *value = GS_STRDUP(cpeState.acsURL);
    return CPE_OK;
}
/**@endparam                                                      **/
/**@param ManagementServer_Username                                                      **/
CPE_STATUS setManagementServer_Username(Instance *ip, char *value)
{
    COPYSTR( cpeState.acsUser,value);
    return CPE_OK;
}
CPE_STATUS getManagementServer_Username(Instance *ip, char **value)
{
        if ( cpeState.acsUser)
                *value = GS_STRDUP(cpeState.acsUser);
    return CPE_OK;
}
/**@endparam                                                      **/
/**@param ManagementServer_Password                                                       **/
CPE_STATUS setManagementServer_Password(Instance *ip, char *value)
{
    COPYSTR(cpeState.acsPW, value);
    return CPE_OK;
}
CPE_STATUS getManagementServer_Password(Instance *ip, char **value)
{
        if (cpeState.acsPW)
                *value = GS_STRDUP(cpeState.acsPW);
    return CPE_OK;
}
/**@endparam                                                      **/
/**@param ManagementServer_PeriodicInformEnable                                                      **/
CPE_STATUS setManagementServer_PeriodicInformEnable(Instance *ip, char *value)
{
    cpeState.informEnabled = testBoolean(value);
    return CPE_OK;
}
CPE_STATUS getManagementServer_PeriodicInformEnable(Instance *ip, char **value)
{
    *value = GS_STRDUP(cpeState.informEnabled? "1": "0");
    return CPE_OK;
}
/**@endparam                                                      **/
/**@param ManagementServer_PeriodicInformInterval                                                      **/
CPE_STATUS setManagementServer_PeriodicInformInterval(Instance *ip, char *value)
{
    cpeState.informInterval = atoi(value);
    return CPE_OK;
}
CPE_STATUS getManagementServer_PeriodicInformInterval(Instance *ip, char **value)
{
    char    buf[10];
    snprintf(buf,sizeof(buf),"%d", (unsigned)cpeState.informInterval);
    *value = GS_STRDUP(buf);
    return CPE_OK;
}
/**@endparam                                                      **/
/**@param ManagementServer_PeriodicInformTime                                                       **/
CPE_STATUS setManagementServer_PeriodicInformTime(Instance *ip, char *value)
{
        extern char *strptime(const char *, const char *, struct tm *);
    struct tm bt;
    strptime(value,"%Y-%m-%dT%H:%M:%S", &bt );
        cpeState.informTime = mktime(&bt);
        return CPE_OK;
}
CPE_STATUS getManagementServer_PeriodicInformTime(Instance *ip, char **value)
{
    char    buf[30];
        if (cpeState.informTime != 0 && cpeState.informTime!= -1) {
                struct tm *bt=localtime(&cpeState.informTime);
                strftime(buf,sizeof(buf),"%Y-%m-%dT%H:%M:%S",bt );
                *value = GS_STRDUP(buf);
        }
        else
                *value = GS_STRDUP(UNKNOWN_TIME);
    return CPE_OK;
}
/**@endparam                                                      **/
/**@param ManagementServer_ParameterKey                                                       **/
CPE_STATUS getManagementServer_ParameterKey(Instance *ip, char **value)
{
    *value = GS_STRDUP(cpeState.parameterKey);
    return CPE_OK;
}
/**@endparam                                                      **/
/**@param ManagementServer_ConnectionRequestURL                                                      **/
CPE_STATUS getManagementServer_ConnectionRequestURL(Instance *ip, char **value)
{
        *value = GS_STRDUP(cpeState.connReqURL);
    return CPE_OK;
}
/**@endparam                                                      **/
/**@param ManagementServer_ConnectionRequestUsername                                                       **/
CPE_STATUS setManagementServer_ConnectionRequestUsername(Instance *ip, char *value)
{
    COPYSTR(cpeState.connReqUser,value);
    return CPE_OK;
}
CPE_STATUS getManagementServer_ConnectionRequestUsername(Instance *ip, char **value)
{
        if (cpeState.connReqUser)
                *value = GS_STRDUP(cpeState.connReqUser);
    return CPE_OK;
}
/**@endparam                                                      **/
/**@param ManagementServer_ConnectionRequestPassword                                                      **/
CPE_STATUS setManagementServer_ConnectionRequestPassword(Instance *ip, char *value)
{
    COPYSTR(cpeState.connReqPW, value);
    return CPE_OK;
}
CPE_STATUS getManagementServer_ConnectionRequestPassword(Instance *ip, char **value)
{
        if ( cpeState.connReqPW)
                *value = GS_STRDUP(cpeState.connReqPW);
    return CPE_OK;
}
/**@endparam                                                      **/
/**@param ManagementServer_UpgradesManaged                                                       **/
CPE_STATUS setManagementServer_UpgradesManaged(Instance *ip, char *value)
{
    cpeState.upgradesManaged = testBoolean(value);
    /* call manufacture function here */
    return CPE_OK;
}
CPE_STATUS getManagementServer_UpgradesManaged(Instance *ip, char **value)
{
    *value = GS_STRDUP(itoa(cpeState.upgradesManaged));
    return CPE_OK;
}
/**@endparam                                                      **/
/**@param ManagementServer_UDPConnectionRequestAddress                     **/
#ifdef CONFIG_TR111P2
#include "../tr111/tr111p2.h"
extern StunState stunState;
CPE_STATUS commitManagementServer(CWMPObject *o, Instance *ip, eCommitCBType cmt ){
	fprintf(stderr,"commitManagementServer() pendingEnable=%d\n", stunState.pendingEnable);
	if ( stunState.pendingEnable ){
		stunState.pendingEnable = 0;
		stunState.enable = 1;
		if ( reStartStun() ) {
			return CPE_OK;
		} else {
			stunState.enable = 0;
			return CPE_ERR;
		}
	}
	/* may be setting some other ManagementServer parameters so return OK */
	return CPE_OK;
}
CPE_STATUS getManagementServer_UDPConnectionRequestAddress(Instance *ip, char **value)
{
	/* get parameter */
	if ( stunState.natIP != 0 ) {
		char *buf = (char *) GS_MALLOC(258);
		struct in_addr n;
		n.s_addr = ntohl(stunState.natIP);
		snprintf(buf, 258, "%s:%d", inet_ntoa(n), stunState.natPort);
		*value = buf;      /* buf is freed by framework */
	} else
		*value = NULL;
	return CPE_OK;
}
/**@endparam                                               **/
/**@param ManagementServer_UDPConnectionRequestAddressNotificationLimit                     **/
CPE_STATUS setManagementServer_UDPConnectionRequestAddressNotificationLimit(Instance *ip, char *value)
{
	return CPE_OK;
}
CPE_STATUS getManagementServer_UDPConnectionRequestAddressNotificationLimit(Instance *ip, char **value)
{
	return CPE_OK;
}
/**@endparam                                               **/
/**@param ManagementServer_STUNEnable                     **/
CPE_STATUS setManagementServer_STUNEnable(Instance *ip, char *value)
{
	/* Set parameter */
	if ( testBoolean(value) ) {
		/* restart stun if already running */
		stunState.pendingEnable = 1;
		/* started by commitManagementServer */
	} else { /* stop it if its running */
		if ( stunState.enable ) {
			stunState.pendingEnable = stunState.enable = 0;
			/* stop it here */
			stopStun();
		}
		stunState.natDetected = 0;
	}
	return CPE_OK;
}
CPE_STATUS getManagementServer_STUNEnable(Instance *ip, char **value)
{
	 *value = GS_STRDUP( stunState.enable? "1": "0");
	return CPE_OK;
}
/**@endparam                                               **/
/**@param ManagementServer_STUNServerAddress                     **/
CPE_STATUS setManagementServer_STUNServerAddress(Instance *ip, char *value)
{
	if ( stunState.serverAddr ) GS_FREE (stunState.serverAddr);
	stunState.serverAddr = GS_STRDUP(value);
	return CPE_OK;
}
CPE_STATUS getManagementServer_STUNServerAddress(Instance *ip, char **value)
{
	if (stunState.serverAddr)
		*value = GS_STRDUP(stunState.serverAddr);
	return CPE_OK;
}
/**@endparam                                               **/
/**@param ManagementServer_STUNServerPort                     **/
CPE_STATUS setManagementServer_STUNServerPort(Instance *ip, char *value)
{
	stunState.serverPort = atoi(value);
	return CPE_OK;
}
CPE_STATUS getManagementServer_STUNServerPort(Instance *ip, char **value)
{
	*value = GS_STRDUP( itoa(stunState.serverPort) );
	return CPE_OK;
}
/**@endparam                                               **/
/**@param ManagementServer_STUNUsername                     **/
CPE_STATUS setManagementServer_STUNUsername(Instance *ip, char *value)
{
	if ( stunState.username ) GS_FREE (stunState.username);
	if (*value)
		stunState.username = GS_STRDUP(value);
	else
		stunState.username = NULL;
	return CPE_OK;
}
CPE_STATUS getManagementServer_STUNUsername(Instance *ip, char **value)
{
	if ( stunState.username )
		*value = GS_STRDUP(stunState.username);
	return CPE_OK;
}
/**@endparam                                               **/
/**@param ManagementServer_STUNPassword                     **/
CPE_STATUS setManagementServer_STUNPassword(Instance *ip, char *value)
{
	if ( stunState.password ) GS_FREE (stunState.password);
	if (*value)
		stunState.password = GS_STRDUP(value);
	else
		stunState.password = NULL;
	return CPE_OK;
}
CPE_STATUS getManagementServer_STUNPassword(Instance *ip, char **value)
{
	if (stunState.password)
		*value = GS_STRDUP(stunState.password);
	return CPE_OK;
}
/**@endparam                                               **/
/**@param ManagementServer_STUNMaximumKeepAlivePeriod                     **/
CPE_STATUS setManagementServer_STUNMaximumKeepAlivePeriod(Instance *ip, char *value)
{
	stunState.maxKeepAlive = atoi( value );
	return CPE_OK;
}
CPE_STATUS getManagementServer_STUNMaximumKeepAlivePeriod(Instance *ip, char **value)
{
	*value = GS_STRDUP(itoa(stunState.maxKeepAlive));
	return CPE_OK;
}
/**@endparam                                               **/
/**@param ManagementServer_STUNMinimumKeepAlivePeriod                     **/
CPE_STATUS setManagementServer_STUNMinimumKeepAlivePeriod(Instance *ip, char *value)
{
	stunState.minKeepAlive = atoi( value );
	return CPE_OK;
}
CPE_STATUS getManagementServer_STUNMinimumKeepAlivePeriod(Instance *ip, char **value)
{
	*value = GS_STRDUP(itoa(stunState.minKeepAlive));
	return CPE_OK;
}
/**@endparam                                               **/
/**@param ManagementServer_NATDetected                     **/
CPE_STATUS getManagementServer_NATDetected(Instance *ip, char **value)
{
	*value = GS_STRDUP(stunState.natDetected?"1": "0");
	return CPE_OK;
}
#endif // CONFIG_TR111P2
/**@endparam                                               **/
/**@param ManagementServer_ManageableDeviceNumberOfEntries                     **/
CPE_STATUS getManagementServer_ManageableDeviceNumberOfEntries(Instance *ip, char **value)
{
	cwmpGetObjInstanceCntStr("ManageableDevice", value);
	return CPE_OK;
}
/**@endparam
/**@endobj ManagementServer **/







