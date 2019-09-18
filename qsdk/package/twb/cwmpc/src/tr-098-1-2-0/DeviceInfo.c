/*----------------------------------------------------------------------*
 * Gatespace
 * Copyright 2011 Gatespace Networks, Inc., All Rights Reserved.
 * Gatespace Networks, Inc. confidential material.
 *----------------------------------------------------------------------*
 * File Name  :
 * Description: Auto-generated getter/setter stubs file.
 *----------------------------------------------------------------------*
 * $Revision: 1.1 $
 *
 * $Id: DeviceInfo.c,v 1.1 2012/05/10 17:37:59 dmounday Exp $
 *----------------------------------------------------------------------*/

#include "paramTree.h"
#include "rpc.h"
#include "gslib/src/utils.h"
#include "soapRpc/rpcUtils.h"
#include "soapRpc/rpcMethods.h"

#include "targetsys.h"
/**@obj DeviceInfo **/
extern CPEState cpeState;

/**@param DeviceInfo_Manufacturer                     **/
CPE_STATUS getDeviceInfo_Manufacturer(Instance *ip, char **value) {
	/* get parameter */
	*value = GS_STRDUP(CWMP_MANUFACTURER);
	return CPE_OK;
}
/**@endparam                                                      **/
/**@param DeviceInfo_ManufacturerOUI                     **/
CPE_STATUS getDeviceInfo_ManufacturerOUI(Instance *ip, char **value) {
	char macStr[30];
	strcpy(macStr, "720259");
	/* get parameter */
	*value = (char *) GS_STRNDUP(macStr, 6); /* use top six digits of MAC */
	return CPE_OK;
}
/**@endparam                                                      **/
/**@param DeviceInfo_ModelName                     **/
CPE_STATUS getDeviceInfo_ModelName(Instance *ip, char **value) {
	/* get parameter */
	*value = GS_STRDUP(CWMP_MODELNAME);
	return CPE_OK;
}
/**@endparam                                                      **/
/**@param DeviceInfo_Description                     **/
CPE_STATUS getDeviceInfo_Description(Instance *ip, char **value) {
	/* get parameter */
	*value = GS_STRDUP(CWMP_DEVICEDESCRIPTON);
	return CPE_OK;
}
/**@endparam                                                                    **/
/**@param DeviceInfo_SerialNumber                     **/
CPE_STATUS getDeviceInfo_SerialNumber(Instance *ip, char **value) {
	char macStr[30];
	strcpy(macStr, writeCanonicalMacUCase(cpeState.MACAddress));
	/* get parameter */
	*value = GS_STRDUP(macStr); /* use all digits of MAC */
	return CPE_OK;
}
/**@endparam                                                      **/
/**@param DeviceInfo_HardwareVersion                     **/
CPE_STATUS getDeviceInfo_HardwareVersion(Instance *ip, char **value) {
	*value = GS_STRDUP(CWMP_HW_VERSION);
	return CPE_OK;
}
/**@endparam                                                      **/
/**@param DeviceInfo_SoftwareVersion                     **/
CPE_STATUS getDeviceInfo_SoftwareVersion(Instance *ip, char **value) {
	*value = GS_STRDUP(CWMP_SOFTWARE_VERSION);
	return CPE_OK;
}
/**@endparam                                                      **/
/**@param DeviceInfo_ProvisioningCode                     **/
CPE_STATUS setDeviceInfo_ProvisioningCode(Instance *ip, char *value) {
	strncpy(cpeState.provisioningCodeStr, value, 64);
	return CPE_OK;
}
CPE_STATUS getDeviceInfo_ProvisioningCode(Instance *ip, char **value) {
	/* get parameter */
	*value = GS_STRDUP(cpeState.provisioningCodeStr);
	return CPE_OK;
}
/**@endparam                                                      **/
/**@param DeviceInfo_UpTime                     **/
CPE_STATUS getDeviceInfo_UpTime(Instance *ip, char **value) {
	FILE *f;
	size_t j;
	if ((f = fopen("/proc/uptime", "r"))) {
		char buf[80];
		fread(buf, sizeof(buf), 1, f);
		fclose(f);
		j = strcspn(buf, ". ");
		*value = (char *) GS_STRNDUP(buf, j);
	} else
		*value = GS_STRDUP("0");
	return CPE_OK;
}
/**@endparam                                                       **/
/**@param DeviceInfo_SpecVersion                     **/
CPE_STATUS getDeviceInfo_SpecVersion(Instance *ip, char **value)
{
	*value = GS_STRDUP(CWMP_SPEC_VERSION);
	return CPE_OK;
}
/**@endparam                                               **/
/**@param DeviceInfo_DeviceLog                     **/
CPE_STATUS getDeviceInfo_DeviceLog(Instance *ip, char **value)
{
	return CPE_OK;
}
/**@endparam                                               **/
/**@endobj DeviceInfo **/

























