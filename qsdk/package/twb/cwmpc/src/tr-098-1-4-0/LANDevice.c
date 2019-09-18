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
 * $Id: LANDevice.c,v 1.1 2012/05/10 17:38:00 dmounday Exp $
 *----------------------------------------------------------------------*/

#include "paramTree.h"
#include "rpc.h"
#include "gslib/src/utils.h"
#include "soapRpc/rpcUtils.h"
#include "soapRpc/rpcMethods.h"
#include "soapRpc/cwmpSession.h"

#include "LANDevice.h"

/**@obj LANDeviceHostsHost **/
CPE_STATUS  addLANDeviceHostsHost(CWMPObject *o, Instance *ip)
{
	/* add instance data */
	LANDeviceHostsHost *p = (LANDeviceHostsHost *)GS_MALLOC( sizeof(struct LANDeviceHostsHost));
	memset(p, 0, sizeof(struct LANDeviceHostsHost));
	ip->cpeData = (void *)p;
	return CPE_OK;
}
CPE_STATUS  delLANDeviceHostsHost(CWMPObject *o, Instance *ip)
{
	/* delete instance data */
	LANDeviceHostsHost *p = (LANDeviceHostsHost *)ip->cpeData;
	if( p ){
		//TODO: free instance data
		GS_FREE(p);
	}
	return CPE_OK;
}

/**@param LANDeviceHostsHost_IPAddress                     **/
CPE_STATUS getLANDeviceHostsHost_IPAddress(Instance *ip, char **value)
{
	LANDeviceHostsHost *p = (LANDeviceHostsHost *)ip->cpeData;
	if ( p ){
		if ( p->iPAddress )
			*value = GS_STRDUP(p->iPAddress);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param LANDeviceHostsHost_AddressSource                     **/
CPE_STATUS getLANDeviceHostsHost_AddressSource(Instance *ip, char **value)
{
	LANDeviceHostsHost *p = (LANDeviceHostsHost *)ip->cpeData;
	if ( p ){
		if ( p->addressSource )
			*value = GS_STRDUP(p->addressSource);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param LANDeviceHostsHost_LeaseTimeRemaining                     **/
CPE_STATUS getLANDeviceHostsHost_LeaseTimeRemaining(Instance *ip, char **value)
{
	LANDeviceHostsHost *p = (LANDeviceHostsHost *)ip->cpeData;
	if ( p ){
		char    buf[10];
		snprintf(buf,sizeof(buf),"%d", p->leaseTimeRemaining);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param LANDeviceHostsHost_MACAddress                     **/
CPE_STATUS getLANDeviceHostsHost_MACAddress(Instance *ip, char **value)
{
	LANDeviceHostsHost *p = (LANDeviceHostsHost *)ip->cpeData;
	if ( p ){
		if ( p->mACAddress )
			*value = GS_STRDUP(p->mACAddress);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param LANDeviceHostsHost_HostName                     **/
CPE_STATUS getLANDeviceHostsHost_HostName(Instance *ip, char **value)
{
	LANDeviceHostsHost *p = (LANDeviceHostsHost *)ip->cpeData;
	if ( p ){
		if ( p->hostName )
			*value = GS_STRDUP(p->hostName);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param LANDeviceHostsHost_InterfaceType                     **/
CPE_STATUS getLANDeviceHostsHost_InterfaceType(Instance *ip, char **value)
{
	LANDeviceHostsHost *p = (LANDeviceHostsHost *)ip->cpeData;
	if ( p ){
		if ( p->interfaceType )
			*value = GS_STRDUP(p->interfaceType);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param LANDeviceHostsHost_Active                     **/
CPE_STATUS getLANDeviceHostsHost_Active(Instance *ip, char **value)
{
	LANDeviceHostsHost *p = (LANDeviceHostsHost *)ip->cpeData;
	if ( p ){
		*value = GS_STRDUP(p->active? "true": "false");
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@endobj LANDeviceHostsHost **/

/**@obj LANDeviceHosts **/
CPE_STATUS  initLANDeviceHosts(CWMPObject *o, Instance *ip)
{
	/* initialize object */
	return CPE_OK;
}

/**@param LANDeviceHosts_HostNumberOfEntries                     **/
CPE_STATUS getLANDeviceHosts_HostNumberOfEntries(Instance *ip, char **value)
{
	cwmpGetObjInstanceCntStr("Host", value);
	return CPE_OK;
}
/**@endparam                                               **/
/**@endobj LANDeviceHosts **/

/**@obj LANDeviceLANHostConfigManagementIPInterface **/
CPE_STATUS  addLANDeviceLANHostConfigManagementIPInterface(CWMPObject *o, Instance *ip)
{
	/* add instance data */
	LANDeviceLANHostConfigManagementIPInterface *p = (LANDeviceLANHostConfigManagementIPInterface *)GS_MALLOC( sizeof(struct LANDeviceLANHostConfigManagementIPInterface));
	memset(p, 0, sizeof(struct LANDeviceLANHostConfigManagementIPInterface));
	ip->cpeData = (void *)p;
	return CPE_OK;
}
CPE_STATUS  delLANDeviceLANHostConfigManagementIPInterface(CWMPObject *o, Instance *ip)
{
	/* delete instance data */
	LANDeviceLANHostConfigManagementIPInterface *p = (LANDeviceLANHostConfigManagementIPInterface *)ip->cpeData;
	if( p ){
		//TODO: free instance data
		GS_FREE(p);
	}
	return CPE_OK;
}
CPE_STATUS  commitLANDeviceLANHostConfigManagementIPInterface(CWMPObject *o, Instance *ip, eCommitCBType cmt)
{
	/* commit object instance */
	return CPE_OK;
}

/**@param LANDeviceLANHostConfigManagementIPInterface_Enable                     **/
CPE_STATUS setLANDeviceLANHostConfigManagementIPInterface_Enable(Instance *ip, char *value)
{
	LANDeviceLANHostConfigManagementIPInterface *p = (LANDeviceLANHostConfigManagementIPInterface *)ip->cpeData;
	if ( p ){
		p->enable=testBoolean(value);
	}
	return CPE_OK;
}
CPE_STATUS getLANDeviceLANHostConfigManagementIPInterface_Enable(Instance *ip, char **value)
{
	LANDeviceLANHostConfigManagementIPInterface *p = (LANDeviceLANHostConfigManagementIPInterface *)ip->cpeData;
	if ( p ){
		*value = GS_STRDUP(p->enable? "true": "false");
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param LANDeviceLANHostConfigManagementIPInterface_IPInterfaceIPAddress                     **/
CPE_STATUS setLANDeviceLANHostConfigManagementIPInterface_IPInterfaceIPAddress(Instance *ip, char *value)
{
	LANDeviceLANHostConfigManagementIPInterface *p = (LANDeviceLANHostConfigManagementIPInterface *)ip->cpeData;
	if ( p ){
		COPYSTR(p->iPInterfaceIPAddress, value);
	}
	return CPE_OK;
}
CPE_STATUS getLANDeviceLANHostConfigManagementIPInterface_IPInterfaceIPAddress(Instance *ip, char **value)
{
	LANDeviceLANHostConfigManagementIPInterface *p = (LANDeviceLANHostConfigManagementIPInterface *)ip->cpeData;
	if ( p ){
		if ( p->iPInterfaceIPAddress )
			*value = GS_STRDUP(p->iPInterfaceIPAddress);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param LANDeviceLANHostConfigManagementIPInterface_IPInterfaceSubnetMask                     **/
CPE_STATUS setLANDeviceLANHostConfigManagementIPInterface_IPInterfaceSubnetMask(Instance *ip, char *value)
{
	LANDeviceLANHostConfigManagementIPInterface *p = (LANDeviceLANHostConfigManagementIPInterface *)ip->cpeData;
	if ( p ){
		COPYSTR(p->iPInterfaceSubnetMask, value);
	}
	return CPE_OK;
}
CPE_STATUS getLANDeviceLANHostConfigManagementIPInterface_IPInterfaceSubnetMask(Instance *ip, char **value)
{
	LANDeviceLANHostConfigManagementIPInterface *p = (LANDeviceLANHostConfigManagementIPInterface *)ip->cpeData;
	if ( p ){
		if ( p->iPInterfaceSubnetMask )
			*value = GS_STRDUP(p->iPInterfaceSubnetMask);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param LANDeviceLANHostConfigManagementIPInterface_IPInterfaceAddressingType                     **/
CPE_STATUS setLANDeviceLANHostConfigManagementIPInterface_IPInterfaceAddressingType(Instance *ip, char *value)
{
	LANDeviceLANHostConfigManagementIPInterface *p = (LANDeviceLANHostConfigManagementIPInterface *)ip->cpeData;
	if ( p ){
		COPYSTR(p->iPInterfaceAddressingType, value);
	}
	return CPE_OK;
}
CPE_STATUS getLANDeviceLANHostConfigManagementIPInterface_IPInterfaceAddressingType(Instance *ip, char **value)
{
	LANDeviceLANHostConfigManagementIPInterface *p = (LANDeviceLANHostConfigManagementIPInterface *)ip->cpeData;
	if ( p ){
		if ( p->iPInterfaceAddressingType )
			*value = GS_STRDUP(p->iPInterfaceAddressingType);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@endobj LANDeviceLANHostConfigManagementIPInterface **/

/**@obj LANDevice **/
CPE_STATUS  addLANDevice(CWMPObject *o, Instance *ip)
{
	/* add instance data */
	GS_LANDevice *p = (GS_LANDevice *)GS_MALLOC( sizeof(struct GS_LANDevice));
	memset(p, 0, sizeof(struct GS_LANDevice));
	ip->cpeData = (void *)p;
	return CPE_OK;
}
CPE_STATUS  delLANDevice(CWMPObject *o, Instance *ip)
{
	/* delete instance data */
	GS_LANDevice *p = (GS_LANDevice *)ip->cpeData;
	if( p ){
		//TODO: free instance data
		GS_FREE(p);
	}
	return CPE_OK;
}

/**@param LANDevice_LANEthernetInterfaceNumberOfEntries                     **/
CPE_STATUS getLANDevice_LANEthernetInterfaceNumberOfEntries(Instance *ip, char **value)
{
	GS_LANDevice *p = (GS_LANDevice *)ip->cpeData;
	if ( p ){
		char    buf[10];
		snprintf(buf,sizeof(buf),"%u", p->lANEthernetInterfaceNumberOfEntries);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param LANDevice_LANUSBInterfaceNumberOfEntries                     **/
CPE_STATUS getLANDevice_LANUSBInterfaceNumberOfEntries(Instance *ip, char **value)
{
	GS_LANDevice *p = (GS_LANDevice *)ip->cpeData;
	if ( p ){
		char    buf[10];
		snprintf(buf,sizeof(buf),"%u", p->lANUSBInterfaceNumberOfEntries);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param LANDevice_LANWLANConfigurationNumberOfEntries                     **/
CPE_STATUS getLANDevice_LANWLANConfigurationNumberOfEntries(Instance *ip, char **value)
{
	GS_LANDevice *p = (GS_LANDevice *)ip->cpeData;
	if ( p ){
		char    buf[10];
		snprintf(buf,sizeof(buf),"%u", p->lANWLANConfigurationNumberOfEntries);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@endobj LANDevice **/

/**@obj LANDeviceLANHostConfigManagement **/

/**@param LANDeviceLANHostConfigManagement_DHCPServerConfigurable                     **/
CPE_STATUS setLANDeviceLANHostConfigManagement_DHCPServerConfigurable(Instance *ip, char *value)
{
	LANDeviceLANHostConfigManagement *p = (LANDeviceLANHostConfigManagement *)ip->cpeData;
	if ( p ){
		p->dHCPServerConfigurable=testBoolean(value);
	}
	return CPE_OK;
}
CPE_STATUS getLANDeviceLANHostConfigManagement_DHCPServerConfigurable(Instance *ip, char **value)
{
	LANDeviceLANHostConfigManagement *p = (LANDeviceLANHostConfigManagement *)ip->cpeData;
	if ( p ){
		*value = GS_STRDUP(p->dHCPServerConfigurable? "true": "false");
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param LANDeviceLANHostConfigManagement_DHCPServerEnable                     **/
CPE_STATUS setLANDeviceLANHostConfigManagement_DHCPServerEnable(Instance *ip, char *value)
{
	LANDeviceLANHostConfigManagement *p = (LANDeviceLANHostConfigManagement *)ip->cpeData;
	if ( p ){
		p->dHCPServerEnable=testBoolean(value);
	}
	return CPE_OK;
}
CPE_STATUS getLANDeviceLANHostConfigManagement_DHCPServerEnable(Instance *ip, char **value)
{
	LANDeviceLANHostConfigManagement *p = (LANDeviceLANHostConfigManagement *)ip->cpeData;
	if ( p ){
		*value = GS_STRDUP(p->dHCPServerEnable? "true": "false");
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param LANDeviceLANHostConfigManagement_DHCPRelay                     **/
CPE_STATUS getLANDeviceLANHostConfigManagement_DHCPRelay(Instance *ip, char **value)
{
	LANDeviceLANHostConfigManagement *p = (LANDeviceLANHostConfigManagement *)ip->cpeData;
	if ( p ){
		*value = GS_STRDUP(p->dHCPRelay? "true": "false");
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param LANDeviceLANHostConfigManagement_MinAddress                     **/
CPE_STATUS setLANDeviceLANHostConfigManagement_MinAddress(Instance *ip, char *value)
{
	LANDeviceLANHostConfigManagement *p = (LANDeviceLANHostConfigManagement *)ip->cpeData;
	if ( p ){
		COPYSTR(p->minAddress, value);
	}
	return CPE_OK;
}
CPE_STATUS getLANDeviceLANHostConfigManagement_MinAddress(Instance *ip, char **value)
{
	LANDeviceLANHostConfigManagement *p = (LANDeviceLANHostConfigManagement *)ip->cpeData;
	if ( p ){
		if ( p->minAddress )
			*value = GS_STRDUP(p->minAddress);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param LANDeviceLANHostConfigManagement_MaxAddress                     **/
CPE_STATUS setLANDeviceLANHostConfigManagement_MaxAddress(Instance *ip, char *value)
{
	LANDeviceLANHostConfigManagement *p = (LANDeviceLANHostConfigManagement *)ip->cpeData;
	if ( p ){
		COPYSTR(p->maxAddress, value);
	}
	return CPE_OK;
}
CPE_STATUS getLANDeviceLANHostConfigManagement_MaxAddress(Instance *ip, char **value)
{
	LANDeviceLANHostConfigManagement *p = (LANDeviceLANHostConfigManagement *)ip->cpeData;
	if ( p ){
		if ( p->maxAddress )
			*value = GS_STRDUP(p->maxAddress);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param LANDeviceLANHostConfigManagement_ReservedAddresses                     **/
CPE_STATUS setLANDeviceLANHostConfigManagement_ReservedAddresses(Instance *ip, char *value)
{
	LANDeviceLANHostConfigManagement *p = (LANDeviceLANHostConfigManagement *)ip->cpeData;
	if ( p ){
		COPYSTR(p->reservedAddresses, value);
	}
	return CPE_OK;
}
CPE_STATUS getLANDeviceLANHostConfigManagement_ReservedAddresses(Instance *ip, char **value)
{
	LANDeviceLANHostConfigManagement *p = (LANDeviceLANHostConfigManagement *)ip->cpeData;
	if ( p ){
		if ( p->reservedAddresses )
			*value = GS_STRDUP(p->reservedAddresses);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param LANDeviceLANHostConfigManagement_SubnetMask                     **/
CPE_STATUS setLANDeviceLANHostConfigManagement_SubnetMask(Instance *ip, char *value)
{
	LANDeviceLANHostConfigManagement *p = (LANDeviceLANHostConfigManagement *)ip->cpeData;
	if ( p ){
		COPYSTR(p->subnetMask, value);
	}
	return CPE_OK;
}
CPE_STATUS getLANDeviceLANHostConfigManagement_SubnetMask(Instance *ip, char **value)
{
	LANDeviceLANHostConfigManagement *p = (LANDeviceLANHostConfigManagement *)ip->cpeData;
	if ( p ){
		if ( p->subnetMask )
			*value = GS_STRDUP(p->subnetMask);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param LANDeviceLANHostConfigManagement_DNSServers                     **/
CPE_STATUS setLANDeviceLANHostConfigManagement_DNSServers(Instance *ip, char *value)
{
	LANDeviceLANHostConfigManagement *p = (LANDeviceLANHostConfigManagement *)ip->cpeData;
	if ( p ){
		COPYSTR(p->dNSServers, value);
	}
	return CPE_OK;
}
CPE_STATUS getLANDeviceLANHostConfigManagement_DNSServers(Instance *ip, char **value)
{
	LANDeviceLANHostConfigManagement *p = (LANDeviceLANHostConfigManagement *)ip->cpeData;
	if ( p ){
		if ( p->dNSServers )
			*value = GS_STRDUP(p->dNSServers);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param LANDeviceLANHostConfigManagement_DomainName                     **/
CPE_STATUS setLANDeviceLANHostConfigManagement_DomainName(Instance *ip, char *value)
{
	LANDeviceLANHostConfigManagement *p = (LANDeviceLANHostConfigManagement *)ip->cpeData;
	if ( p ){
		COPYSTR(p->domainName, value);
	}
	return CPE_OK;
}
CPE_STATUS getLANDeviceLANHostConfigManagement_DomainName(Instance *ip, char **value)
{
	LANDeviceLANHostConfigManagement *p = (LANDeviceLANHostConfigManagement *)ip->cpeData;
	if ( p ){
		if ( p->domainName )
			*value = GS_STRDUP(p->domainName);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param LANDeviceLANHostConfigManagement_IPRouters                     **/
CPE_STATUS setLANDeviceLANHostConfigManagement_IPRouters(Instance *ip, char *value)
{
	LANDeviceLANHostConfigManagement *p = (LANDeviceLANHostConfigManagement *)ip->cpeData;
	if ( p ){
		COPYSTR(p->iPRouters, value);
	}
	return CPE_OK;
}
CPE_STATUS getLANDeviceLANHostConfigManagement_IPRouters(Instance *ip, char **value)
{
	LANDeviceLANHostConfigManagement *p = (LANDeviceLANHostConfigManagement *)ip->cpeData;
	if ( p ){
		if ( p->iPRouters )
			*value = GS_STRDUP(p->iPRouters);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param LANDeviceLANHostConfigManagement_IPInterfaceNumberOfEntries                     **/
CPE_STATUS getLANDeviceLANHostConfigManagement_IPInterfaceNumberOfEntries(Instance *ip, char **value)
{
	cwmpGetObjInstanceCntStr("IPInterface", value);
	return CPE_OK;
}
/**@endparam                                               **/
/**@endobj LANDeviceLANHostConfigManagement **/





