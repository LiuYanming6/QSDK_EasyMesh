/*----------------------------------------------------------------------*
 * Gatespace
 * Copyright 2011, 2012 Gatespace Networks, Inc., All Rights Reserved.
 * Gatespace Networks, Inc. confidential material.
 *----------------------------------------------------------------------*
 * File Name  :
 * Description: Auto-generated getter/setter stubs file.
 *----------------------------------------------------------------------*
 * $Revision: 1.2 $
 *
 * $Id: IP.c,v 1.2 2012/06/13 16:07:50 dmounday Exp $
 *----------------------------------------------------------------------*/

#include "paramTree.h"
#include "rpc.h"
#include "gslib/src/utils.h"
#include "soapRpc/rpcUtils.h"
#include "soapRpc/rpcMethods.h"
#include "soapRpc/cwmpSession.h"
#include <syslog.h>


#include "IP.h"

void cpeStopPing(void *);
void cpeStartPing(void *);
extern void *acsSession;
extern char *strptime(const char *, const char *, struct tm *);
extern CPEState cpeState;


/**@obj IPDiagnosticsIPPing **/
/* This function is called only once when the configuration is restored on
 * CWMPc startup. The CWMPObject for "IPPing" defines this function as the
 * addObj function. In the case of eObject types of CWMPObject it is only used
 * for initialization. The addObj function is not called by the framework for
 * this type of object.
 */
CPE_STATUS  initIPDiagnosticsIPPing(CWMPObject *o, Instance *ip)
{
	/* initialize object */
	IPDiagnosticsIPPing *p = (IPDiagnosticsIPPing *)GS_MALLOC(sizeof(struct IPDiagnosticsIPPing));
	memset(p, 0, sizeof(struct IPDiagnosticsIPPing));
	ip->cpeData = (void *)p;
	return CPE_OK;
}
CPE_STATUS  commitIPDiagnosticsIPPing(CWMPObject *o, Instance *ip, eCommitCBType cmt)
{
	IPDiagnosticsIPPing *p = (IPDiagnosticsIPPing*)ip->cpeData;
	if (cmt==COMMIT_SET && p->pendingState == eRequested) {
		if (p->host == NULL ){
			return CPE_ERR;
		}
		if ( p->state == eRequested ) {
			/* ping is running and a new request has been made */
			/* stop ping and restart */
			cpeStopPing((void*)eRequested);
			p->pendingState = eNone;
			setCallback(&acsSession, cpeStartPing, p );
		} else {
			/* No ping running */
			/* schedule callback to startPing when ACS session ends */
			p->state = eRequested;
			p->pendingState = eNone;
			setCallback(&acsSession, cpeStartPing, p );
			p->interface = NULL; // TODO: match up interface.
		}
	} else if (p->state == eRequested ){
		/* a writeable param was set while ping is running. */
		/* Stop ping and set state to eNone */
		cpeStopPing(eNone);
		p->pendingState = eNone;
	}
	return CPE_OK;
}

/**@param IPDiagnosticsIPPing_DiagnosticsState                     **/
CPE_STATUS setIPDiagnosticsIPPing_DiagnosticsState(Instance *ip, char *value)
{
	IPDiagnosticsIPPing *p = (IPDiagnosticsIPPing *)ip->cpeData;
	if ( p ){
		COPYSTR(p->diagnosticsState, value);
		if (strcasecmp(value, "requested")==0 ){
			p->pendingState = eRequested;
			return CPE_OK;
		}
	}
	return CPE_9003;
}
CPE_STATUS getIPDiagnosticsIPPing_DiagnosticsState(Instance *ip, char **value)
{
	IPDiagnosticsIPPing *p = (IPDiagnosticsIPPing *)ip->cpeData;
	if ( p ){
		switch (p->state) {
			case eRequested:
				*value = GS_STRDUP("Requested");
				break;
			case eStateNone:
				*value = GS_STRDUP("None");
				break;
			case eHostError:
				*value = GS_STRDUP("Error_CannotResolveHostName");
				break;
			case eComplete:
				*value = GS_STRDUP("Complete");
				break;
			case eErrorInternal:
				*value = GS_STRDUP("Error_Internal");
				break;
			case eErrorOther:
				*value = GS_STRDUP("Error_Other");
				break;
			default:
				break;
		}
		return CPE_OK;
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param IPDiagnosticsIPPing_Interface                     **/
CPE_STATUS setIPDiagnosticsIPPing_Interface(Instance *ip, char *value)
{
	IPDiagnosticsIPPing *p = (IPDiagnosticsIPPing *)ip->cpeData;
	if ( p ){
		// value points at a string containing the Interface path name.
		Instance *ifp = cwmpGetInstancePtr( value );
		if ( ifp ){
			p->interface = ifp;
			if (p->state == eRequested ){
				cpeStopPing((void*)eNone);
			}
		} else
			return CPE_9007;
	}
	return CPE_OK;
}
CPE_STATUS getIPDiagnosticsIPPing_Interface(Instance *ip, char **value)
{
	IPDiagnosticsIPPing *p = (IPDiagnosticsIPPing *)ip->cpeData;
	if ( p ){
		//*value = cwmpGetInstancePathStr(p->interface);
		*value = GS_STRDUP("Device.IP.Interface.1");
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param IPDiagnosticsIPPing_Host                     **/
CPE_STATUS setIPDiagnosticsIPPing_Host(Instance *ip, char *value)
{
	IPDiagnosticsIPPing *p = (IPDiagnosticsIPPing *)ip->cpeData;
	if ( p ){
		if (p->state == eRequested ){
			cpeStopPing((void*)eNone);
		}
		COPYSTR(p->host, value);
	}
	return CPE_OK;
}
CPE_STATUS getIPDiagnosticsIPPing_Host(Instance *ip, char **value)
{
	IPDiagnosticsIPPing *p = (IPDiagnosticsIPPing *)ip->cpeData;
    if ( p )
    {
        if ( p->host )
            *value = GS_STRDUP(p->host);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param IPDiagnosticsIPPing_NumberOfRepetitions                     **/
CPE_STATUS setIPDiagnosticsIPPing_NumberOfRepetitions(Instance *ip, char *value)
{
	IPDiagnosticsIPPing *p = (IPDiagnosticsIPPing *)ip->cpeData;
	if ( p ){
		int v = atoi(value);
		if ( v != 0) {
			if (p->state == eRequested ){
				cpeStopPing((void*)eNone);
			}
            else if (p->state == eComplete)
            {
                cpeStopPing((void*)eNone);
            }
			p->numberOfRepetitions = v;
			return CPE_OK;
		} else
			return CPE_9007;
	}
	return CPE_OK;
}
CPE_STATUS getIPDiagnosticsIPPing_NumberOfRepetitions(Instance *ip, char **value)
{
	IPDiagnosticsIPPing *p = (IPDiagnosticsIPPing *)ip->cpeData;
	if ( p ){
		char    buf[10];
		snprintf(buf,sizeof(buf),"%u", p->numberOfRepetitions);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param IPDiagnosticsIPPing_Timeout                     **/
CPE_STATUS setIPDiagnosticsIPPing_Timeout(Instance *ip, char *value)
{
	IPDiagnosticsIPPing *p = (IPDiagnosticsIPPing *)ip->cpeData;
	if ( p ){
		int v = atoi(value);
		if ( v != 0) {
			if (p->state == eRequested ){
				cpeStopPing((void*)eNone);
			}
            else if (p->state == eComplete)
            {
                cpeStopPing((void*)eNone);
            }
			p->timeout = v;
			return CPE_OK;
		} else
			return CPE_9007;
	}
	return CPE_OK;
}
CPE_STATUS getIPDiagnosticsIPPing_Timeout(Instance *ip, char **value)
{
	IPDiagnosticsIPPing *p = (IPDiagnosticsIPPing *)ip->cpeData;
	if ( p ){
		char    buf[10];
		snprintf(buf,sizeof(buf),"%u", p->timeout);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param IPDiagnosticsIPPing_DataBlockSize                     **/
CPE_STATUS setIPDiagnosticsIPPing_DataBlockSize(Instance *ip, char *value)
{
	IPDiagnosticsIPPing *p = (IPDiagnosticsIPPing *)ip->cpeData;
	if ( p ){
		if (p->state == eRequested ){
			cpeStopPing((void*)eNone);
		}
		p->dataBlockSize=atoi(value);
	}
	return CPE_OK;
}
CPE_STATUS getIPDiagnosticsIPPing_DataBlockSize(Instance *ip, char **value)
{
	IPDiagnosticsIPPing *p = (IPDiagnosticsIPPing *)ip->cpeData;
	if ( p ){
		char    buf[10];
		snprintf(buf,sizeof(buf),"%u", p->dataBlockSize);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param IPDiagnosticsIPPing_DSCP                     **/
CPE_STATUS setIPDiagnosticsIPPing_DSCP(Instance *ip, char *value)
{
	IPDiagnosticsIPPing *p = (IPDiagnosticsIPPing *)ip->cpeData;
	if ( p ){
		p->dSCP=atoi(value);
		if (p->state == eRequested ){
			cpeStopPing((void*)eNone);
		}
	}
	return CPE_OK;
}
CPE_STATUS getIPDiagnosticsIPPing_DSCP(Instance *ip, char **value)
{
	IPDiagnosticsIPPing *p = (IPDiagnosticsIPPing *)ip->cpeData;
	if ( p ){
		char    buf[10];
		snprintf(buf,sizeof(buf),"%u", p->dSCP);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param IPDiagnosticsIPPing_SuccessCount                     **/
CPE_STATUS getIPDiagnosticsIPPing_SuccessCount(Instance *ip, char **value)
{
	IPDiagnosticsIPPing *p = (IPDiagnosticsIPPing *)ip->cpeData;
	if ( p ){
		char    buf[10];
		snprintf(buf,sizeof(buf),"%u", p->successCount);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param IPDiagnosticsIPPing_FailureCount                     **/
CPE_STATUS getIPDiagnosticsIPPing_FailureCount(Instance *ip, char **value)
{
	IPDiagnosticsIPPing *p = (IPDiagnosticsIPPing *)ip->cpeData;
	if ( p ){
		char    buf[10];
		snprintf(buf,sizeof(buf),"%u", p->failureCount);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param IPDiagnosticsIPPing_AverageResponseTime                     **/
CPE_STATUS getIPDiagnosticsIPPing_AverageResponseTime(Instance *ip, char **value)
{
	IPDiagnosticsIPPing *p = (IPDiagnosticsIPPing *)ip->cpeData;
	if ( p ){
		char    buf[10];
		snprintf(buf,sizeof(buf),"%u", p->averageResponseTime);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param IPDiagnosticsIPPing_MinimumResponseTime                     **/
CPE_STATUS getIPDiagnosticsIPPing_MinimumResponseTime(Instance *ip, char **value)
{
	IPDiagnosticsIPPing *p = (IPDiagnosticsIPPing *)ip->cpeData;
	if ( p ){
		char    buf[10];
		snprintf(buf,sizeof(buf),"%u", p->minimumResponseTime);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param IPDiagnosticsIPPing_MaximumResponseTime                     **/
CPE_STATUS getIPDiagnosticsIPPing_MaximumResponseTime(Instance *ip, char **value)
{
	IPDiagnosticsIPPing *p = (IPDiagnosticsIPPing *)ip->cpeData;
	if ( p ){
		char    buf[10];
		snprintf(buf,sizeof(buf),"%u", p->maximumResponseTime);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param IPDiagnosticsIPPing_ProtocolVersion                     **/
CPE_STATUS setIPDiagnosticsIPPing_ProtocolVersion(Instance *ip, char *value)
{
    IPDiagnosticsIPPing *p = (IPDiagnosticsIPPing *)ip->cpeData;
    if ( p )
    {
        if (p->state == eRequested )
            cpeStopPing((void*)eNone);

        if(!strncmp(value, "IPv4", 4) || !strncmp(value, "IPv6", 4))
        {
            COPYSTR(p->protocolversion, value);
        }
        else if(!strncmp(value, "Any", 3))
        {
             if ( cpeState.acsIPAddress.inFamily == AF_INET6)
             {
                COPYSTR(p->protocolversion, "IPv6");
             }
             else
             {
                COPYSTR(p->protocolversion, "IPv4");
             }
        }
        else
            return CPE_ERR;

    }
	return CPE_OK;
}
CPE_STATUS getIPDiagnosticsIPPing_ProtocolVersion(Instance *ip, char **value)
{
    IPDiagnosticsIPPing *p = (IPDiagnosticsIPPing *)ip->cpeData;
    if ( p ){
        if(p->protocolversion)
            *value = GS_STRDUP(p->protocolversion);
        else
        {
           COPYSTR(p->protocolversion, "IPv4");
            *value = GS_STRDUP(p->protocolversion);
        }
    }
    return CPE_OK;
}
/**@endparam                                               **/

/**@endobj IPDiagnosticsIPPing **/

/**@obj IPInterfaceIPv4Address **/
CPE_STATUS  addIPInterfaceIPv4Address(CWMPObject *o, Instance *ip)
{
	/* add instance data */
		IPInterfaceIPv4Address *p = (IPInterfaceIPv4Address *)GS_MALLOC( sizeof(struct IPInterfaceIPv4Address));
		memset(p, 0, sizeof(struct IPInterfaceIPv4Address));
		ip->cpeData = (void *)p;
		p->status = GS_STRDUP("Disabled");
		p->addressingType = GS_STRDUP("DHCP");
		return CPE_OK;
	}
CPE_STATUS  commitIPInterfaceIPv4Address(CWMPObject *o, Instance *ip, eCommitCBType cmt);
CPE_STATUS  delIPInterfaceIPv4Address(CWMPObject *o, Instance *ip)
{
	/* delete instance data */
	IPInterfaceIPv4Address *p = (IPInterfaceIPv4Address *)ip->cpeData;
	if( p ){
		if ( p->enable ){
			p->enable = 0; // delete IP from interface if enabled.
			commitIPInterfaceIPv4Address( o, ip, COMMIT_SET);
		}
		GS_FREE(p->addressingType);
		GS_FREE(p->iPAddress);
		GS_FREE(p->status);
		GS_FREE(p->subnetMask);
		GS_FREE(p->pendingIPAddress);
		GS_FREE(p->pendingsubnetMask);
		GS_FREE(p);
	}
	return CPE_OK;
}
/*
 * This code is not complete with respect to managing the IP address on
 * the IP.Interface.
 */
CPE_STATUS  commitIPInterfaceIPv4Address(CWMPObject *o, Instance *ip, eCommitCBType cmt)
{
	/* commit object instance */
	//char	cmd[512];
	//FILE	*f;
	//IPInterfaceIPv4Address *p = (IPInterfaceIPv4Address *)ip->cpeData;
	//IPInterface *ipip = (IPInterface *)ip->parent->cpeData;
	fprintf(stderr, "commitIPInterfaceIPv4Address() called\n");
	/****
	if (  p->enable && p->pendingSet==eSetEnabled ){

		if ( ipip->enable && ipip->name ){
			snprintf(cmd, sizeof(cmd), "ip addr add local %s dev %s 2>&1\n",
				p->iPAddress, ipip->name );
			if ((f = popen(cmd, "r"))) {
				if ( fgets(cmd, sizeof(cmd), f) ){
					if ( strstr(cmd, "answers")){
						fclose(f);
						return CPE_9003;
					} // else ok
				}
			}else {
				cpeLog(LOG_ERR, "Could not run >%s<", cmd);
				return CPE_9003;
			}

		}
	}

	    else if ( !p->enable && p->iPAddress && p->subnetMask ){
		// disable by deleting IP address
		snprintf(cmd, sizeof(cmd), "ip addr delete local %s dev %s 2>&1\n",
			p->iPAddress, ipip->name );
		if ((f = popen(cmd, "r"))) {
			if ( fgets(cmd, sizeof(cmd), f) ){
				if ( strstr(cmd, "answers")){
					fclose(f);
					return CPE_9003;
				} // else ok
			}
		}else {
			cpeLog(LOG_ERR, "Could not run >%s<", cmd);
			return CPE_9003;
		}

	****/
	return CPE_OK;
}

/**@param IPInterfaceIPv4Address_Enable                     **/
CPE_STATUS setIPInterfaceIPv4Address_Enable(Instance *ip, char *value)
{
	IPInterfaceIPv4Address *p = (IPInterfaceIPv4Address *)ip->cpeData;
	if ( p ){
		p->pendingSet = testBoolean(value)? eSetEnabled: eSetDisabled;
	}
	return CPE_OK;
}
CPE_STATUS getIPInterfaceIPv4Address_Enable(Instance *ip, char **value)
{
	IPInterfaceIPv4Address *p = (IPInterfaceIPv4Address *)ip->cpeData;
	if ( p ){
		*value = GS_STRDUP(p->enable? "true": "false");
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param IPInterfaceIPv4Address_IPAddress                     **/
CPE_STATUS setIPInterfaceIPv4Address_IPAddress(Instance *ip, char *value)
{
	IPInterfaceIPv4Address *p = (IPInterfaceIPv4Address *)ip->cpeData;
	if ( p ){
		if ( !streq(p->iPAddress, value)){
			COPYSTR(p->pendingIPAddress, value);
		} /* else ignore if same address value */
	}
	return CPE_OK;
}
CPE_STATUS getIPInterfaceIPv4Address_IPAddress(Instance *ip, char **value)
{
	IPInterfaceIPv4Address *p = (IPInterfaceIPv4Address *)ip->cpeData;
	if ( p ){
		if ( p->pendingIPAddress )
			*value = GS_STRDUP(p->pendingIPAddress);
		else if (p->iPAddress)
			*value = GS_STRDUP(p->iPAddress);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param IPInterfaceIPv4Address_SubnetMask                     **/
CPE_STATUS setIPInterfaceIPv4Address_SubnetMask(Instance *ip, char *value)
{
	IPInterfaceIPv4Address *p = (IPInterfaceIPv4Address *)ip->cpeData;
	if ( p ){
		if ( !streq(p->subnetMask, value) ){
			if ( p->pendingsubnetMask)
				GS_FREE(p->pendingsubnetMask);
			p->pendingsubnetMask = GS_STRDUP(value);
		} /* else ignore if same address value */
	}
	return CPE_OK;
}
CPE_STATUS getIPInterfaceIPv4Address_SubnetMask(Instance *ip, char **value)
{
	IPInterfaceIPv4Address *p = (IPInterfaceIPv4Address *)ip->cpeData;
	if ( p ){
		if ( p->pendingsubnetMask )
			*value = GS_STRDUP(p->pendingsubnetMask);
		else if (p->subnetMask)
			*value = GS_STRDUP(p->subnetMask);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param IPInterfaceIPv4Address_AddressingType                     **/
CPE_STATUS getIPInterfaceIPv4Address_AddressingType(Instance *ip, char **value)
{
	IPInterfaceIPv4Address *p = (IPInterfaceIPv4Address *)ip->cpeData;
	if ( p ){
		if ( p->addressingType )
			*value = GS_STRDUP(p->addressingType);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param IPInterfaceIPv4Address_Status                     **/
CPE_STATUS getIPInterfaceIPv4Address_Status(Instance *ip, char **value)
{
	IPInterfaceIPv4Address *p = (IPInterfaceIPv4Address *)ip->cpeData;
	if ( p ){
		if ( p->status )
			*value = GS_STRDUP(p->status);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@endobj IPInterfaceIPv4Address **/

/**@obj IPInterfaceIPv6Address **/
CPE_STATUS  addIPInterfaceIPv6Address(CWMPObject *o, Instance *ip)
{
	/* add instance data */
		IPInterfaceIPv6Address *p = (IPInterfaceIPv6Address *)GS_MALLOC( sizeof(struct IPInterfaceIPv6Address));
		memset(p, 0, sizeof(struct IPInterfaceIPv6Address));
		ip->cpeData = (void *)p;
		p->status = GS_STRDUP("Disabled");
		p->iPAddressStatus = GS_STRDUP("Invalid");
		p->origin = GS_STRDUP("Static");
		p->preferredLifetime = -1;//GS_STRDUP("9999-12-31T23:59:59Z");
		p->validLifetime = -1; //GS_STRDUP("9999-12-31T23:59:59Z");
		return CPE_OK;
	}
CPE_STATUS  delIPInterfaceIPv6Address(CWMPObject *o, Instance *ip)
{
	/* delete instance data */
	IPInterfaceIPv6Address *p = (IPInterfaceIPv6Address *)ip->cpeData;
	if( p ){
		GS_FREE(p->iPAddress);
		GS_FREE(p->iPAddressStatus);
		GS_FREE(p->origin);
		GS_FREE(p->prefix);
		GS_FREE(p->status);
		GS_FREE(p);
	}
	return CPE_OK;
}
CPE_STATUS  commitIPInterfaceIPv6Address(CWMPObject *o, Instance *ip, eCommitCBType cmt)
{
	/* commit object instance */
	return CPE_OK;
}

/**@param IPInterfaceIPv6Address_Enable                     **/
CPE_STATUS setIPInterfaceIPv6Address_Enable(Instance *ip, char *value)
{
	IPInterfaceIPv6Address *p = (IPInterfaceIPv6Address *)ip->cpeData;
	if ( p ){
		p->enable=testBoolean(value);
	}
	return CPE_OK;
}
CPE_STATUS getIPInterfaceIPv6Address_Enable(Instance *ip, char **value)
{
	IPInterfaceIPv6Address *p = (IPInterfaceIPv6Address *)ip->cpeData;
	if ( p ){
		*value = GS_STRDUP(p->enable? "true": "false");
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param IPInterfaceIPv6Address_Status                     **/
CPE_STATUS getIPInterfaceIPv6Address_Status(Instance *ip, char **value)
{
	IPInterfaceIPv6Address *p = (IPInterfaceIPv6Address *)ip->cpeData;
	if ( p ){
		if ( p->status )
			*value = GS_STRDUP(p->status);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param IPInterfaceIPv6Address_IPAddressStatus                     **/
CPE_STATUS getIPInterfaceIPv6Address_IPAddressStatus(Instance *ip, char **value)
{
	IPInterfaceIPv6Address *p = (IPInterfaceIPv6Address *)ip->cpeData;
	if ( p ){
		if ( p->iPAddressStatus )
			*value = GS_STRDUP(p->iPAddressStatus);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param IPInterfaceIPv6Address_IPAddress                     **/
CPE_STATUS setIPInterfaceIPv6Address_IPAddress(Instance *ip, char *value)
{
	IPInterfaceIPv6Address *p = (IPInterfaceIPv6Address *)ip->cpeData;
	if ( p ){
		COPYSTR(p->iPAddress, value);
	}
	return CPE_OK;
}
CPE_STATUS getIPInterfaceIPv6Address_IPAddress(Instance *ip, char **value)
{
	IPInterfaceIPv6Address *p = (IPInterfaceIPv6Address *)ip->cpeData;
	if ( p ){
		if ( p->iPAddress )
			*value = GS_STRDUP(p->iPAddress);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param IPInterfaceIPv6Address_Origin                     **/
CPE_STATUS getIPInterfaceIPv6Address_Origin(Instance *ip, char **value)
{
	IPInterfaceIPv6Address *p = (IPInterfaceIPv6Address *)ip->cpeData;
	if ( p ){
		if ( p->origin )
			*value = GS_STRDUP(p->origin);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param IPInterfaceIPv6Address_Prefix                     **/
CPE_STATUS setIPInterfaceIPv6Address_Prefix(Instance *ip, char *value)
{
	IPInterfaceIPv6Address *p = (IPInterfaceIPv6Address *)ip->cpeData;
	if ( p ){
		//TODO: resolve Instance pointers.
	}
	return CPE_OK;
}
CPE_STATUS getIPInterfaceIPv6Address_Prefix(Instance *ip, char **value)
{
	IPInterfaceIPv6Address *p = (IPInterfaceIPv6Address *)ip->cpeData;
	if ( p ){
		*value = cwmpGetInstancePathStr(p->prefix);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param IPInterfaceIPv6Address_PreferredLifetime                     **/
CPE_STATUS setIPInterfaceIPv6Address_PreferredLifetime(Instance *ip, char *value)
{
	IPInterfaceIPv6Address *p = (IPInterfaceIPv6Address *)ip->cpeData;
	if ( p ){
		struct tm bt;
		strptime(value,"%Y-%m-%dT%H:%M:%S", &bt );
		p->preferredLifetime= mktime(&bt);
	}
	return CPE_OK;
}
CPE_STATUS getIPInterfaceIPv6Address_PreferredLifetime(Instance *ip, char **value)
{
	IPInterfaceIPv6Address *p = (IPInterfaceIPv6Address *)ip->cpeData;
	if ( p ){
		char buf[30];
		struct tm *bt=localtime(&p->preferredLifetime);
		strftime(buf,sizeof(buf),"%Y-%m-%dT%H:%M:%SZ",bt );
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param IPInterfaceIPv6Address_ValidLifetime                     **/
CPE_STATUS setIPInterfaceIPv6Address_ValidLifetime(Instance *ip, char *value)
{
	IPInterfaceIPv6Address *p = (IPInterfaceIPv6Address *)ip->cpeData;
	if ( p ){
		struct tm bt;
		strptime(value,"%Y-%m-%dT%H:%M:%S", &bt );
		p->validLifetime= mktime(&bt);
	}
	return CPE_OK;
}
CPE_STATUS getIPInterfaceIPv6Address_ValidLifetime(Instance *ip, char **value)
{
	IPInterfaceIPv6Address *p = (IPInterfaceIPv6Address *)ip->cpeData;
	if ( p ){
		char buf[30];
		struct tm *bt=localtime(&p->validLifetime);
		strftime(buf,sizeof(buf),"%Y-%m-%dT%H:%M:%SZ",bt );
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/

/**@param IPInterfaceIPv6Address_Anycast                     **/
CPE_STATUS setIPInterfaceIPv6Address_Anycast(Instance *ip, char *value)
{
	IPInterfaceIPv6Address *p = (IPInterfaceIPv6Address *)ip->cpeData;
	if ( p ){
        p->anycast=testBoolean(value);
	}
	return CPE_OK;
}
CPE_STATUS getIPInterfaceIPv6Address_Anycast(Instance *ip, char **value)
{
	IPInterfaceIPv6Address *p = (IPInterfaceIPv6Address *)ip->cpeData;
	if ( p ){
		*value = GS_STRDUP(p->anycast? "true": "false");
	}
	return CPE_OK;
}
/**@endparam                                               **/

/**@endobj IPInterfaceIPv6Address **/

/**@obj IPInterfaceIPv6Prefix **/
CPE_STATUS  addIPInterfaceIPv6Prefix(CWMPObject *o, Instance *ip)
{
	/* add instance data */
		IPInterfaceIPv6Prefix *p = (IPInterfaceIPv6Prefix *)GS_MALLOC( sizeof(struct IPInterfaceIPv6Prefix));
		memset(p, 0, sizeof(struct IPInterfaceIPv6Prefix));
		ip->cpeData = (void *)p;
		return CPE_OK;
	}
CPE_STATUS  delIPInterfaceIPv6Prefix(CWMPObject *o, Instance *ip)
{
	/* delete instance data */
	IPInterfaceIPv6Prefix *p = (IPInterfaceIPv6Prefix *)ip->cpeData;
	if( p ){
		//TODO: free instance data
		GS_FREE(p);
	}
	return CPE_OK;
}

/**@param IPInterfaceIPv6Prefix_Enable                     **/
CPE_STATUS setIPInterfaceIPv6Prefix_Enable(Instance *ip, char *value)
{
	IPInterfaceIPv6Prefix *p = (IPInterfaceIPv6Prefix *)ip->cpeData;
	if ( p ){
		p->enable=testBoolean(value);
	}
	return CPE_OK;
}
CPE_STATUS getIPInterfaceIPv6Prefix_Enable(Instance *ip, char **value)
{
	IPInterfaceIPv6Prefix *p = (IPInterfaceIPv6Prefix *)ip->cpeData;
	if ( p ){
		*value = GS_STRDUP(p->enable? "true": "false");
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param IPInterfaceIPv6Prefix_Status                     **/
CPE_STATUS getIPInterfaceIPv6Prefix_Status(Instance *ip, char **value)
{
	IPInterfaceIPv6Prefix *p = (IPInterfaceIPv6Prefix *)ip->cpeData;
	if ( p ){
		if ( p->status )
			*value = GS_STRDUP(p->status);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param IPInterfaceIPv6Prefix_PrefixStatus                     **/
CPE_STATUS getIPInterfaceIPv6Prefix_PrefixStatus(Instance *ip, char **value)
{
	IPInterfaceIPv6Prefix *p = (IPInterfaceIPv6Prefix *)ip->cpeData;
	if ( p ){
		if ( p->prefixStatus )
			*value = GS_STRDUP(p->prefixStatus);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param IPInterfaceIPv6Prefix_Prefix                     **/
CPE_STATUS setIPInterfaceIPv6Prefix_Prefix(Instance *ip, char *value)
{
	IPInterfaceIPv6Prefix *p = (IPInterfaceIPv6Prefix *)ip->cpeData;
	if ( p ){
		COPYSTR(p->prefix, value);
	}
	return CPE_OK;
}
CPE_STATUS getIPInterfaceIPv6Prefix_Prefix(Instance *ip, char **value)
{
	IPInterfaceIPv6Prefix *p = (IPInterfaceIPv6Prefix *)ip->cpeData;
	if ( p ){
		if ( p->prefix )
			*value = GS_STRDUP(p->prefix);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param IPInterfaceIPv6Prefix_Origin                     **/
CPE_STATUS getIPInterfaceIPv6Prefix_Origin(Instance *ip, char **value)
{
	IPInterfaceIPv6Prefix *p = (IPInterfaceIPv6Prefix *)ip->cpeData;
	if ( p ){
		if ( p->origin )
			*value = GS_STRDUP(p->origin);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param IPInterfaceIPv6Prefix_OnLink                     **/
CPE_STATUS setIPInterfaceIPv6Prefix_OnLink(Instance *ip, char *value)
{
	IPInterfaceIPv6Prefix *p = (IPInterfaceIPv6Prefix *)ip->cpeData;
	if ( p ){
		p->onLink=testBoolean(value);
	}
	return CPE_OK;
}
CPE_STATUS getIPInterfaceIPv6Prefix_OnLink(Instance *ip, char **value)
{
	IPInterfaceIPv6Prefix *p = (IPInterfaceIPv6Prefix *)ip->cpeData;
	if ( p ){
		*value = GS_STRDUP(p->onLink? "true": "false");
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param IPInterfaceIPv6Prefix_Autonomous                     **/
CPE_STATUS setIPInterfaceIPv6Prefix_Autonomous(Instance *ip, char *value)
{
	IPInterfaceIPv6Prefix *p = (IPInterfaceIPv6Prefix *)ip->cpeData;
	if ( p ){
		p->autonomous=testBoolean(value);
	}
	return CPE_OK;
}
CPE_STATUS getIPInterfaceIPv6Prefix_Autonomous(Instance *ip, char **value)
{
	IPInterfaceIPv6Prefix *p = (IPInterfaceIPv6Prefix *)ip->cpeData;
	if ( p ){
		*value = GS_STRDUP(p->autonomous? "true": "false");
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param IPInterfaceIPv6Prefix_PreferredLifetime                     **/
CPE_STATUS setIPInterfaceIPv6Prefix_PreferredLifetime(Instance *ip, char *value)
{
	IPInterfaceIPv6Prefix *p = (IPInterfaceIPv6Prefix *)ip->cpeData;
	if ( p ){
		struct tm bt;
		strptime(value,"%Y-%m-%dT%H:%M:%S", &bt );
		p->preferredLifetime= mktime(&bt);
	}
	return CPE_OK;
}
CPE_STATUS getIPInterfaceIPv6Prefix_PreferredLifetime(Instance *ip, char **value)
{
	IPInterfaceIPv6Prefix *p = (IPInterfaceIPv6Prefix *)ip->cpeData;
	if ( p ){
		char buf[30];
		struct tm *bt=localtime(&p->preferredLifetime);
		strftime(buf,sizeof(buf),"%Y-%m-%dT%H:%M:%S",bt );
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param IPInterfaceIPv6Prefix_ValidLifetime                     **/
CPE_STATUS setIPInterfaceIPv6Prefix_ValidLifetime(Instance *ip, char *value)
{
	IPInterfaceIPv6Prefix *p = (IPInterfaceIPv6Prefix *)ip->cpeData;
	if ( p ){
		struct tm bt;
		strptime(value,"%Y-%m-%dT%H:%M:%S", &bt );
		p->validLifetime= mktime(&bt);
	}
	return CPE_OK;
}
CPE_STATUS getIPInterfaceIPv6Prefix_ValidLifetime(Instance *ip, char **value)
{
	IPInterfaceIPv6Prefix *p = (IPInterfaceIPv6Prefix *)ip->cpeData;
	if ( p ){
		char buf[30];
		struct tm *bt=localtime(&p->validLifetime);
		strftime(buf,sizeof(buf),"%Y-%m-%dT%H:%M:%S",bt );
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@endobj IPInterfaceIPv6Prefix **/

/**@obj IPInterfaceStats **/

/**@param IPInterfaceStats_BytesSent                     **/
CPE_STATUS getIPInterfaceStats_BytesSent(Instance *ip, char **value)
{
	IPInterfaceStats *p = (IPInterfaceStats *)ip->cpeData;
	if ( p ){
		char    buf[30];
		snprintf(buf,sizeof(buf),"%lld", p->bytesSent);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param IPInterfaceStats_BytesReceived                     **/
CPE_STATUS getIPInterfaceStats_BytesReceived(Instance *ip, char **value)
{
	IPInterfaceStats *p = (IPInterfaceStats *)ip->cpeData;
	if ( p ){
		char    buf[30];
		snprintf(buf,sizeof(buf),"%lld", p->bytesReceived);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param IPInterfaceStats_PacketsSent                     **/
CPE_STATUS getIPInterfaceStats_PacketsSent(Instance *ip, char **value)
{
	IPInterfaceStats *p = (IPInterfaceStats *)ip->cpeData;
	if ( p ){
		char    buf[30];
		snprintf(buf,sizeof(buf),"%lld", p->packetsSent);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param IPInterfaceStats_PacketsReceived                     **/
CPE_STATUS getIPInterfaceStats_PacketsReceived(Instance *ip, char **value)
{
	IPInterfaceStats *p = (IPInterfaceStats *)ip->cpeData;
	if ( p ){
		char    buf[30];
		snprintf(buf,sizeof(buf),"%lld", p->packetsReceived);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param IPInterfaceStats_ErrorsSent                     **/
CPE_STATUS getIPInterfaceStats_ErrorsSent(Instance *ip, char **value)
{
	IPInterfaceStats *p = (IPInterfaceStats *)ip->cpeData;
	if ( p ){
		char    buf[10];
		snprintf(buf,sizeof(buf),"%u", p->errorsSent);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param IPInterfaceStats_ErrorsReceived                     **/
CPE_STATUS getIPInterfaceStats_ErrorsReceived(Instance *ip, char **value)
{
	IPInterfaceStats *p = (IPInterfaceStats *)ip->cpeData;
	if ( p ){
		char    buf[10];
		snprintf(buf,sizeof(buf),"%u", p->errorsReceived);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param IPInterfaceStats_UnicastPacketsSent                     **/
CPE_STATUS getIPInterfaceStats_UnicastPacketsSent(Instance *ip, char **value)
{
	IPInterfaceStats *p = (IPInterfaceStats *)ip->cpeData;
	if ( p ){
		char    buf[30];
		snprintf(buf,sizeof(buf),"%lld", p->unicastPacketsSent);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param IPInterfaceStats_UnicastPacketsReceived                     **/
CPE_STATUS getIPInterfaceStats_UnicastPacketsReceived(Instance *ip, char **value)
{
	IPInterfaceStats *p = (IPInterfaceStats *)ip->cpeData;
	if ( p ){
		char    buf[30];
		snprintf(buf,sizeof(buf),"%lld", p->unicastPacketsReceived);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param IPInterfaceStats_DiscardPacketsSent                     **/
CPE_STATUS getIPInterfaceStats_DiscardPacketsSent(Instance *ip, char **value)
{
	IPInterfaceStats *p = (IPInterfaceStats *)ip->cpeData;
	if ( p ){
		char    buf[10];
		snprintf(buf,sizeof(buf),"%u", p->discardPacketsSent);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param IPInterfaceStats_DiscardPacketsReceived                     **/
CPE_STATUS getIPInterfaceStats_DiscardPacketsReceived(Instance *ip, char **value)
{
	IPInterfaceStats *p = (IPInterfaceStats *)ip->cpeData;
	if ( p ){
		char    buf[10];
		snprintf(buf,sizeof(buf),"%u", p->discardPacketsReceived);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param IPInterfaceStats_MulticastPacketsSent                     **/
CPE_STATUS getIPInterfaceStats_MulticastPacketsSent(Instance *ip, char **value)
{
	IPInterfaceStats *p = (IPInterfaceStats *)ip->cpeData;
	if ( p ){
		char    buf[30];
		snprintf(buf,sizeof(buf),"%lld", p->multicastPacketsSent);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param IPInterfaceStats_MulticastPacketsReceived                     **/
CPE_STATUS getIPInterfaceStats_MulticastPacketsReceived(Instance *ip, char **value)
{
	IPInterfaceStats *p = (IPInterfaceStats *)ip->cpeData;
	if ( p ){
		char    buf[30];
		snprintf(buf,sizeof(buf),"%lld", p->multicastPacketsReceived);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param IPInterfaceStats_BroadcastPacketsSent                     **/
CPE_STATUS getIPInterfaceStats_BroadcastPacketsSent(Instance *ip, char **value)
{
	IPInterfaceStats *p = (IPInterfaceStats *)ip->cpeData;
	if ( p ){
		char    buf[30];
		snprintf(buf,sizeof(buf),"%lld", p->broadcastPacketsSent);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param IPInterfaceStats_BroadcastPacketsReceived                     **/
CPE_STATUS getIPInterfaceStats_BroadcastPacketsReceived(Instance *ip, char **value)
{
	IPInterfaceStats *p = (IPInterfaceStats *)ip->cpeData;
	if ( p ){
		char    buf[30];
		snprintf(buf,sizeof(buf),"%lld", p->broadcastPacketsReceived);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param IPInterfaceStats_UnknownProtoPacketsReceived                     **/
CPE_STATUS getIPInterfaceStats_UnknownProtoPacketsReceived(Instance *ip, char **value)
{
	IPInterfaceStats *p = (IPInterfaceStats *)ip->cpeData;
	if ( p ){
		char    buf[10];
		snprintf(buf,sizeof(buf),"%u", p->unknownProtoPacketsReceived);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@endobj IPInterfaceStats **/

/**@obj IPInterface **/
CPE_STATUS  addIPInterface(CWMPObject *o, Instance *ip)
{
	/* add instance data */
	IPInterface *p = (IPInterface *)GS_MALLOC( sizeof(struct IPInterface));
	memset(p, 0, sizeof(struct IPInterface));
	ip->cpeData = (void *)p;
	p->status = GS_STRDUP("Down");
	p->type = GS_STRDUP("Normal");
	return CPE_OK;
}
CPE_STATUS  delIPInterface(CWMPObject *o, Instance *ip)
{
	/* delete instance data */
	IPInterface *p = (IPInterface *)ip->cpeData;
	if( p ){
		GS_FREE(p->status);
		GS_FREE(p->type);
		GS_FREE(p->name);
		GS_FREE(p);
	}
	return CPE_OK;
}
CPE_STATUS  commitIPInterface(CWMPObject *o, Instance *ip, eCommitCBType cmt)
{
	IPInterface *p = (IPInterface *)ip->cpeData;
	CWMPObject *co = cwmpGetChildObject("IPv4Address");
	Instance *cip = cwmpGetChildInstance("IPv4Address", ip);
	while (cip) {
		IPInterfaceIPv4Address *ipv4p = (IPInterfaceIPv4Address *)cip->cpeData;
		if ( p->ipIFchange == ENABLE_PENDING ){
			// enable any IPv4 IP addresses
			if ( ipv4p->enable ) // re-enable IPv4
				commitIPInterfaceIPv4Address(co, cip, COMMIT_SET);
		} else if (p->ipIFchange == DISABLE_PENDING ){
			if ( ipv4p->enable ){
				ipv4p->enable = 0;
				commitIPInterfaceIPv4Address(co, cip, COMMIT_SET);
			}
		}
		cip = cwmpGetNextSiblingInstance( cip );
	}

	return CPE_OK;
}

/**@param IPInterface_Enable                     **/
CPE_STATUS setIPInterface_Enable(Instance *ip, char *value)
{
	IPInterface *p = (IPInterface *)ip->cpeData;
	if ( p ){
		if ((p->enable && testBoolean(value))
		   || (!p->enable && !testBoolean(value)))
			return CPE_9003;
		else
			p->ipIFchange = testBoolean(value)? ENABLE_PENDING: DISABLE_PENDING;
		return CPE_OK;
	}
	return CPE_9002;
}
CPE_STATUS getIPInterface_Enable(Instance *ip, char **value)
{
	IPInterface *p = (IPInterface *)ip->cpeData;
	if ( p ){
		*value = GS_STRDUP(p->enable? "true": "false");
	}
	return CPE_OK;
}
/**@endparam                                               **/
 /**@param IPInterface_IPv4Enable                     **/
CPE_STATUS getIPInterface_IPv4Enable(Instance *ip, char **value)
{
	IPInterface *p = (IPInterface *)ip->cpeData;
	if ( p ){
		*value = GS_STRDUP(p->iPv4Enable? "true": "false");
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param IPInterface_IPv6Enable                     **/
CPE_STATUS setIPInterface_IPv6Enable(Instance *ip, char *value)
{
	IPInterface *p = (IPInterface *)ip->cpeData;
	if ( p ){
		p->iPv6Enable=testBoolean(value);
	}
	return CPE_OK;
}
CPE_STATUS getIPInterface_IPv6Enable(Instance *ip, char **value)
{
	IPInterface *p = (IPInterface *)ip->cpeData;
	if ( p ){
		*value = GS_STRDUP(p->iPv6Enable? "true": "false");
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param IPInterface_ULAEnable                     **/
CPE_STATUS setIPInterface_ULAEnable(Instance *ip, char *value)
{
	IPInterface *p = (IPInterface *)ip->cpeData;
	if ( p ){
		p->uLAEnable=testBoolean(value);
	}
	return CPE_OK;
}
CPE_STATUS getIPInterface_ULAEnable(Instance *ip, char **value)
{
	IPInterface *p = (IPInterface *)ip->cpeData;
	if ( p ){
		*value = GS_STRDUP(p->uLAEnable? "true": "false");
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param IPInterface_Status                     **/
CPE_STATUS getIPInterface_Status(Instance *ip, char **value)
{
	IPInterface *p = (IPInterface *)ip->cpeData;
	if ( p ){
		if ( p->status )
			*value = GS_STRDUP(p->status);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param IPInterface_Name                     **/
CPE_STATUS getIPInterface_Name(Instance *ip, char **value)
{
	IPInterface *p = (IPInterface *)ip->cpeData;
	if ( p ){
		if ( p->name )
			*value = GS_STRDUP(p->name);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param IPInterface_LastChange                     **/
CPE_STATUS getIPInterface_LastChange(Instance *ip, char **value)
{
	IPInterface *p = (IPInterface *)ip->cpeData;
	if ( p ){
		char    buf[10];
		snprintf(buf,sizeof(buf),"%u", p->lastChange);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param IPInterface_LowerLayers                     **/
CPE_STATUS setIPInterface_LowerLayers(Instance *ip, char *value)
{
	IPInterface *p = (IPInterface *)ip->cpeData;
	if ( p ){
		// value should point to a string of comma separated instance paths.
		// TODO: handle more than a single instance path.
		p->lowerLayers[0] = cwmpGetInstancePtr(value);
		p->lowerLayers[1] = NULL;
	}
	return CPE_OK;
}
CPE_STATUS getIPInterface_LowerLayers(Instance *ip, char **value)
{
	IPInterface *p = (IPInterface *)ip->cpeData;
	if ( p ){
		*value = cwmpGetPathRefRowsStr(p->lowerLayers);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param IPInterface_Reset                     **/
CPE_STATUS setIPInterface_Reset(Instance *ip, char *value)
{
	IPInterface *p = (IPInterface *)ip->cpeData;
	if ( p ){
		p->reset=testBoolean(value);
	}
	return CPE_OK;
}
CPE_STATUS getIPInterface_Reset(Instance *ip, char **value)
{
	IPInterface *p = (IPInterface *)ip->cpeData;
	if ( p ){
		*value = GS_STRDUP(p->reset? "true": "false");
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param IPInterface_Type                     **/
CPE_STATUS getIPInterface_Type(Instance *ip, char **value)
{
	IPInterface *p = (IPInterface *)ip->cpeData;
	if ( p ){
		if ( p->type )
			*value = GS_STRDUP(p->type);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param IPInterface_IPv4AddressNumberOfEntries                     **/
CPE_STATUS getIPInterface_IPv4AddressNumberOfEntries(Instance *ip, char **value)
{
	cwmpGetObjInstanceCntStr("IPv4Address", value);
	return CPE_OK;
}
/**@endparam                                               **/
/**@param IPInterface_IPv6AddressNumberOfEntries                     **/
CPE_STATUS getIPInterface_IPv6AddressNumberOfEntries(Instance *ip, char **value)
{
	cwmpGetObjInstanceCntStr("IPv6Address", value);
	return CPE_OK;
}
/**@endparam                                               **/
/**@param IPInterface_IPv6PrefixNumberOfEntries                     **/
CPE_STATUS getIPInterface_IPv6PrefixNumberOfEntries(Instance *ip, char **value)
{
	cwmpGetObjInstanceCntStr("IPv6Prefix", value);
	return CPE_OK;
}
/**@endparam                                               **/

CPE_STATUS setIPInterface_AutoIPEnable(Instance *ip, char *value)
{
	IPInterface *p = (IPInterface *)ip->cpeData;
	if ( p ){
		p->autoenable=testBoolean(value);
        if ( !strcmp(p->name,"br-lan") && p->autoenable)
        {
            char cmd[128]={0};
            char cmd_result[10]={0};
            memset(cmd, 0x0 , sizeof(cmd));
            memset(cmd_result , 0x0 , sizeof(cmd_result));
            sprintf( cmd , "killall -SIGUSR1 udhcpc"); 
            cmd_popen( cmd , cmd_result);
            memset(cmd, 0x0 , sizeof(cmd));
            memset(cmd_result , 0x0 , sizeof(cmd_result));
            sprintf( cmd , "iwpriv ath0 kickmac FF:FF:FF:FF:FF:FF");
            cmd_popen( cmd , cmd_result);
            memset(cmd, 0x0 , sizeof(cmd));
            memset(cmd_result , 0x0 , sizeof(cmd_result));
            sprintf(cmd , "iwpriv ath1 kickmac FF:FF:FF:FF:FF:FF");
            cmd_popen( cmd , cmd_result);
            memset(cmd, 0x0 , sizeof(cmd));
            memset(cmd_result , 0x0 , sizeof(cmd_result));             
            sprintf(cmd , "/etc/init.d/mDNSResponder restart");
            cmd_popen( cmd , cmd_result);
        }
	}
	return CPE_OK;
}

CPE_STATUS getIPInterface_AutoIPEnable(Instance *ip, char **value)
{
    IPInterface *p = (IPInterface *)ip->cpeData;
    if ( p ){
        if (!strcmp(p->name,"br-lan"))
            p->autoenable = 1;

        *value = GS_STRDUP(p->autoenable? "true": "false");
    }
    return CPE_OK;
}
/**@endparam                                               **/
/**@endobj IPInterface **/

/**@obj IP **/
CPE_STATUS  initIP(CWMPObject *o, Instance *ip)
{
	/* initialize object */
	GS_IP *p = (GS_IP *)GS_MALLOC( sizeof(struct GS_IP));
	memset(p, 0, sizeof(struct GS_IP));
	p->iPv4Capable = 1;			// IPv4 Capable and Enable
	p->iPv4Enable = 1;			// Initialize Device.IP. object: IPv4 Enabled.
	p->iPv4Status = GS_STRDUP("Enabled");
	p->iPv6Capable = 1;			// IPv6 Capable and Enable
	p->iPv6Enable = 1;
	p->iPv6Status = GS_STRDUP("Enabled");
	ip->cpeData = (void *)p;
	return CPE_OK;
}

/**@param IP_IPv4Capable                     **/
CPE_STATUS getIP_IPv4Capable(Instance *ip, char **value)
{
	GS_IP *p = (GS_IP *)ip->cpeData;
	if ( p ){
		*value = GS_STRDUP(p->iPv4Capable? "true": "false");
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param IP_IPv4Enable                     **/
CPE_STATUS getIP_IPv4Enable(Instance *ip, char **value)
{
	GS_IP *p = (GS_IP *)ip->cpeData;
	if ( p ){
		*value = GS_STRDUP(p->iPv4Enable? "true": "false");
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param IP_IPv4Status                     **/
CPE_STATUS getIP_IPv4Status(Instance *ip, char **value)
{
	GS_IP *p = (GS_IP *)ip->cpeData;
	if ( p ){
		if ( p->iPv4Status )
			*value = GS_STRDUP(p->iPv4Status);
	}
	return CPE_OK;
}
/**@endparam                                               **/

/**@param IP_IPv6Capable                     **/
CPE_STATUS getIP_IPv6Capable(Instance *ip, char **value)
{
	GS_IP *p = (GS_IP *)ip->cpeData;
	if ( p ){
		*value = GS_STRDUP(p->iPv6Capable? "true": "false");
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param IP_IPv6Enable                     **/
CPE_STATUS setIP_IPv6Enable(Instance *ip, char *value)
{
	GS_IP *p = (GS_IP *)ip->cpeData;
	if ( p ){
		p->iPv6Enable=testBoolean(value);
	}
	return CPE_OK;
}
CPE_STATUS getIP_IPv6Enable(Instance *ip, char **value)
{
	GS_IP *p = (GS_IP *)ip->cpeData;
	if ( p ){
		*value = GS_STRDUP(p->iPv6Enable? "true": "false");
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param IP_IPv6Status                     **/
CPE_STATUS getIP_IPv6Status(Instance *ip, char **value)
{
	GS_IP *p = (GS_IP *)ip->cpeData;
	if ( p ){
		if ( p->iPv6Status )
			*value = GS_STRDUP(p->iPv6Status);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param IP_ULAPrefix                     **/
CPE_STATUS setIP_ULAPrefix(Instance *ip, char *value)
{
	GS_IP *p = (GS_IP *)ip->cpeData;
	if ( p ){
		COPYSTR(p->uLAPrefix, value);
	}
	return CPE_OK;
}
CPE_STATUS getIP_ULAPrefix(Instance *ip, char **value)
{
	GS_IP *p = (GS_IP *)ip->cpeData;
	if ( p ){
		if ( p->uLAPrefix )
			*value = GS_STRDUP(p->uLAPrefix);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param IP_InterfaceNumberOfEntries                     **/
CPE_STATUS getIP_InterfaceNumberOfEntries(Instance *ip, char **value)
{
	cwmpGetObjInstanceCntStr("Interface", value);
	return CPE_OK;
}
/**@endparam                                               **/

CPE_STATUS getIP_ActivePortNumberOfEntries(Instance *ip, char **value)
{
	cwmpGetObjInstanceCntStr("ActivePort", value);
	return CPE_OK;
}
/**@endparam                                               **/
/**@endobj IP **/

/**@obj IPDiagnosticsTraceRouteRouteHops **/
CPE_STATUS  addIPDiagnosticsTraceRouteRouteHops(CWMPObject *o, Instance *ip)
{
	/* add new instance data */
	IPDiagnosticsTraceRouteRouteHops *p = (IPDiagnosticsTraceRouteRouteHops *)GS_MALLOC( sizeof(struct IPDiagnosticsTraceRouteRouteHops));
	memset(p, 0, sizeof(struct IPDiagnosticsTraceRouteRouteHops));
	ip->cpeData = (void *)p;
	return CPE_OK;
}
CPE_STATUS delIPDiagnosticsTraceRouteRouteHops(CWMPObject *o, Instance *ip)
{
	/* delete instance data */
	IPDiagnosticsTraceRouteRouteHops *p = (IPDiagnosticsTraceRouteRouteHops *)ip->cpeData;
	if( p ){
		GS_FREE(p->host);
		GS_FREE(p->hostAddress);
		GS_FREE(p);
	}
	return CPE_OK;
}

/**@param IPDiagnosticsTraceRouteRouteHops_Host                     **/
CPE_STATUS getIPDiagnosticsTraceRouteRouteHops_Host(Instance *ip, char **value)
{
	IPDiagnosticsTraceRouteRouteHops *p = (IPDiagnosticsTraceRouteRouteHops *)ip->cpeData;
	if ( p ){
		if ( p->host )
			*value = GS_STRDUP(p->host);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param IPDiagnosticsTraceRouteRouteHops_HostAddress                     **/
CPE_STATUS getIPDiagnosticsTraceRouteRouteHops_HostAddress(Instance *ip, char **value)
{
	IPDiagnosticsTraceRouteRouteHops *p = (IPDiagnosticsTraceRouteRouteHops *)ip->cpeData;
	if ( p ){
		if ( p->hostAddress )
			*value = GS_STRDUP(p->hostAddress);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param IPDiagnosticsTraceRouteRouteHops_ErrorCode                     **/
CPE_STATUS getIPDiagnosticsTraceRouteRouteHops_ErrorCode(Instance *ip, char **value)
{
	IPDiagnosticsTraceRouteRouteHops *p = (IPDiagnosticsTraceRouteRouteHops *)ip->cpeData;
	if ( p ){
		char    buf[10];
		snprintf(buf,sizeof(buf),"%u", p->errorCode);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param IPDiagnosticsTraceRouteRouteHops_RTTimes                     **/
CPE_STATUS getIPDiagnosticsTraceRouteRouteHops_RTTimes(Instance *ip, char **value)
{
	IPDiagnosticsTraceRouteRouteHops *p = (IPDiagnosticsTraceRouteRouteHops *)ip->cpeData;
	if ( p ){
		char    buf[10];
		snprintf(buf,sizeof(buf),"%u", p->rTTimes);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@endobj IPDiagnosticsTraceRouteRouteHops **/

/**@obj IPDiagnosticsTraceRoute **/
void cpeStopTraceRt(void *handle);
void cpeStartTraceRt(void *handle);
void cpeRemoveRouteHops(void);

CPE_STATUS  initIPDiagnosticsTraceRoute(CWMPObject *o, Instance *ip)
{
	/* initialize object */
	IPDiagnosticsTraceRoute *p = (IPDiagnosticsTraceRoute*)GS_MALLOC(sizeof(struct IPDiagnosticsTraceRoute));
	memset(p, 0, sizeof(struct IPDiagnosticsTraceRoute));
	ip->cpeData = p;
	return CPE_OK;
}
CPE_STATUS  commitIPDiagnosticsTraceRoute(CWMPObject *o, Instance *ip, eCommitCBType cmt)
{
	IPDiagnosticsTraceRoute *p = (IPDiagnosticsTraceRoute*)ip->cpeData;
	if ( cmt==COMMIT_SET && p->pendingState == eRequested ) {
		if ( p->host == NULL ){
			return CPE_ERR;
		}
		if ( p->diagnosticState == eRequested ) {
			/* current state is Requested -- need to stop running trace route*/
			cpeStopTraceRt((void*)eRequested);
			p->pendingState = eNone;
			cpeRemoveRouteHops();
			setCallback( &acsSession, cpeStartTraceRt, p);
		} else {
			/* not running */
			p->diagnosticState = eRequested;
			p->pendingState = eNone;
			cpeRemoveRouteHops();
			setCallback( &acsSession, cpeStartTraceRt, p );
		}
	} else if ( p->diagnosticState == eRequested ){
		/* a writable parameter was set while traceroute is running */
		/* stop traceroute and set state to eNone */
		cpeStopTraceRt((void*)eNone);
		p->pendingState = eNone;
	}
	return CPE_OK;
}

/**@param IPDiagnosticsTraceRoute_DiagnosticsState                     **/
CPE_STATUS setIPDiagnosticsTraceRoute_DiagnosticsState(Instance *ip, char *value)
{
	IPDiagnosticsTraceRoute *p = (IPDiagnosticsTraceRoute *)ip->cpeData;
	if ( p ){
		if ( strcasecmp( value, "Requested") ==0 ) {
			p->pendingState = eRequested;
			return CPE_OK;
		}
	}
	return CPE_9003;
}
CPE_STATUS getIPDiagnosticsTraceRoute_DiagnosticsState(Instance *ip, char **value)
{
	IPDiagnosticsTraceRoute *p = (IPDiagnosticsTraceRoute *)ip->cpeData;
	if ( p ){
		switch (p->diagnosticState) {
			case eRequested:
				*value = GS_STRDUP("Requested");
				break;
			case eStateNone:
				*value = GS_STRDUP("None");
				break;
			case eHostError:
				*value = GS_STRDUP("Error_CannotResolveHostName");
				break;
			case eComplete:
				*value = GS_STRDUP("Complete");
				break;
			case eMaxHopExceeded:
				*value = GS_STRDUP("Error_MaxHopCountExceeded");
				break;
			default:
				break;
		}
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param IPDiagnosticsTraceRoute_Interface                     **/
CPE_STATUS setIPDiagnosticsTraceRoute_Interface(Instance *ip, char *value)
{
	IPDiagnosticsTraceRoute *p = (IPDiagnosticsTraceRoute *)ip->cpeData;
	if ( p ){
		// value points at a string containing the Interface path name.
		Instance *ifp = cwmpGetInstancePtr( value );
		if ( ifp )
			p->interface = ifp;
		else
			return CPE_9007;
	}
	return CPE_OK;
}
CPE_STATUS getIPDiagnosticsTraceRoute_Interface(Instance *ip, char **value)
{
	IPDiagnosticsTraceRoute *p = (IPDiagnosticsTraceRoute *)ip->cpeData;
	if ( p ){
		//*value = cwmpGetInstancePathStr(p->interface);
		*value = GS_STRDUP("Device.IP.Interface.1");
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param IPDiagnosticsTraceRoute_Host                     **/
CPE_STATUS setIPDiagnosticsTraceRoute_Host(Instance *ip, char *value)
{
	IPDiagnosticsTraceRoute *p = (IPDiagnosticsTraceRoute *)ip->cpeData;
	if ( p ){
		COPYSTR(p->host, value);
	}
	return CPE_OK;
}
CPE_STATUS getIPDiagnosticsTraceRoute_Host(Instance *ip, char **value)
{
	IPDiagnosticsTraceRoute *p = (IPDiagnosticsTraceRoute *)ip->cpeData;
	if ( p ){
		if ( p->host )
			*value = GS_STRDUP(p->host);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param IPDiagnosticsTraceRoute_NumberOfTries                     **/
CPE_STATUS setIPDiagnosticsTraceRoute_NumberOfTries(Instance *ip, char *value)
{
	IPDiagnosticsTraceRoute *p = (IPDiagnosticsTraceRoute *)ip->cpeData;
	if ( p ){
		p->numberOfTries=atoi(value);
	}
	return CPE_OK;
}
CPE_STATUS getIPDiagnosticsTraceRoute_NumberOfTries(Instance *ip, char **value)
{
	IPDiagnosticsTraceRoute *p = (IPDiagnosticsTraceRoute *)ip->cpeData;
	if ( p ){
		char    buf[10];
		snprintf(buf,sizeof(buf),"%u", p->numberOfTries);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param IPDiagnosticsTraceRoute_Timeout                     **/
CPE_STATUS setIPDiagnosticsTraceRoute_Timeout(Instance *ip, char *value)
{
    IPDiagnosticsTraceRoute *p = (IPDiagnosticsTraceRoute *)ip->cpeData;
    if ( p ){
        if (p->diagnosticState == eComplete)
        {
            p->diagnosticState = eNone;
            cpeRemoveRouteHops();
        }
        p->timeout=atoi(value);
    }
    return CPE_OK;
}
CPE_STATUS getIPDiagnosticsTraceRoute_Timeout(Instance *ip, char **value)
{
	IPDiagnosticsTraceRoute *p = (IPDiagnosticsTraceRoute *)ip->cpeData;
	if ( p ){
		char    buf[10];
		snprintf(buf,sizeof(buf),"%u", p->timeout);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param IPDiagnosticsTraceRoute_DataBlockSize                     **/
CPE_STATUS setIPDiagnosticsTraceRoute_DataBlockSize(Instance *ip, char *value)
{
    int datasize;
    IPDiagnosticsTraceRoute *p = (IPDiagnosticsTraceRoute *)ip->cpeData;
    if ( p ){
        datasize = atoi(value);
        if(datasize == 0)
            return CPE_ERR;
        else
            p->dataBlockSize=datasize;
    }
    return CPE_OK;
}
CPE_STATUS getIPDiagnosticsTraceRoute_DataBlockSize(Instance *ip, char **value)
{
    IPDiagnosticsTraceRoute *p = (IPDiagnosticsTraceRoute *)ip->cpeData;
    if ( p )
    {
        if(p->dataBlockSize == 0)
            p->dataBlockSize = 38;

        char    buf[10];
        snprintf(buf,sizeof(buf),"%u", p->dataBlockSize);
        *value = GS_STRDUP(buf);
    }
	return CPE_OK;
}
/**@endparam                                               **/
/**@param IPDiagnosticsTraceRoute_DSCP                     **/
CPE_STATUS setIPDiagnosticsTraceRoute_DSCP(Instance *ip, char *value)
{
	IPDiagnosticsTraceRoute *p = (IPDiagnosticsTraceRoute *)ip->cpeData;
	if ( p ){
		p->dSCP=atoi(value);
	}
	return CPE_OK;
}
CPE_STATUS getIPDiagnosticsTraceRoute_DSCP(Instance *ip, char **value)
{
	IPDiagnosticsTraceRoute *p = (IPDiagnosticsTraceRoute *)ip->cpeData;
	if ( p ){
		char    buf[10];
		snprintf(buf,sizeof(buf),"%u", p->dSCP);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param IPDiagnosticsTraceRoute_MaxHopCount                     **/
CPE_STATUS setIPDiagnosticsTraceRoute_MaxHopCount(Instance *ip, char *value)
{
	IPDiagnosticsTraceRoute *p = (IPDiagnosticsTraceRoute *)ip->cpeData;
	if ( p ){
		p->maxHopCount=atoi(value);
	}
	return CPE_OK;
}
CPE_STATUS getIPDiagnosticsTraceRoute_MaxHopCount(Instance *ip, char **value)
{
	IPDiagnosticsTraceRoute *p = (IPDiagnosticsTraceRoute *)ip->cpeData;
	if ( p ){
		char    buf[10];
		snprintf(buf,sizeof(buf),"%u", p->maxHopCount);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param IPDiagnosticsTraceRoute_ResponseTime                     **/
CPE_STATUS getIPDiagnosticsTraceRoute_ResponseTime(Instance *ip, char **value)
{
	IPDiagnosticsTraceRoute *p = (IPDiagnosticsTraceRoute *)ip->cpeData;
	if ( p ){
		char    buf[10];
		snprintf(buf,sizeof(buf),"%u", p->responseTime);
		*value = GS_STRDUP(buf);
	}
	return CPE_OK;
}
/**@endparam                                               **/
/**@param IPDiagnosticsTraceRoute_RouteHopsNumberOfEntries                     **/
CPE_STATUS getIPDiagnosticsTraceRoute_RouteHopsNumberOfEntries(Instance *ip, char **value)
{
	cwmpGetObjInstanceCntStr("RouteHops", value);
	return CPE_OK;
}
/**@endparam                                               **/
/**@endobj IPDiagnosticsTraceRoute **/

/**@param IPDiagnosticsTraceRoute_ProtocolVersion                     **/
CPE_STATUS setIPDiagnosticsTraceRoute_ProtocolVersion(Instance *ip, char *value)
{
    IPDiagnosticsTraceRoute *p = (IPDiagnosticsTraceRoute *)ip->cpeData;
    if ( p )
    {
        if(!strncmp(value, "IPv4", 4) || !strncmp(value, "IPv6", 4))
        {
            COPYSTR(p->protocolversion, value);
        }
        else if(!strncmp(value, "Any", 3))
        {
             if ( cpeState.acsIPAddress.inFamily == AF_INET6)
             {
                COPYSTR(p->protocolversion, "IPv6");
             }
             else
             {
                COPYSTR(p->protocolversion, "IPv4");
             }
        }
        else
            return CPE_ERR;
    }
	return CPE_OK;
}
CPE_STATUS getIPDiagnosticsTraceRoute_ProtocolVersion(Instance *ip, char **value)
{
    IPDiagnosticsTraceRoute *p = (IPDiagnosticsTraceRoute *)ip->cpeData;
    if ( p ){
        if(p->protocolversion)
            *value = GS_STRDUP(p->protocolversion);
        else
        {
            COPYSTR(p->protocolversion, "IPv4");
            *value = GS_STRDUP(p->protocolversion);
        }
    }
    return CPE_OK;
}
/**@endparam                                               **/

/**@obj IPDiagnostics **/
/**@endobj IPDiagnostics **/






























