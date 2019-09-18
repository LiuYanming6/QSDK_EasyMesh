/*----------------------------------------------------------------------*
 * Gatespace
 * Copyright 2011 Gatespace Networks, Inc., All Rights Reserved.
 * Gatespace Networks, Inc. confidential material.
 *----------------------------------------------------------------------*
 * File Name  :
 * Description: Auto-generated getter/setter stubs file.
 *----------------------------------------------------------------------*
 * $Revision: 1.2 $
 *
 * $Id: DeviceInfo.c,v 1.2 2012/06/13 16:07:50 dmounday Exp $
 *----------------------------------------------------------------------*/

#include "paramTree.h"
#include "rpc.h"
#include "gslib/src/utils.h"
#include "soapRpc/rpcUtils.h"
#include "soapRpc/rpcMethods.h"

#include "stdlib.h"
#include "targetsys.h"
/**@obj DeviceInfo **/
extern CPEState cpeState;
#define STRINGIZE(x) #x
#define STRINGIZE_VALUE_OF(x) STRINGIZE(x)

/**@param DeviceInfo_Manufacturer                     **/
CPE_STATUS getDeviceInfo_Manufacturer(Instance *ip, char **value) {
	/* get parameter */
	*value = GS_STRDUP(CWMP_MANUFACTURER);
	return CPE_OK;
}
/**@endparam                                                      **/
/**@param DeviceInfo_ManufacturerOUI                     **/
CPE_STATUS getDeviceInfo_ManufacturerOUI(Instance *ip, char **value) {
	char cmd[7]={0};
	memset(cmd,0x0,sizeof(cmd));
	cmd_popen("state_cfg get rmac | tr -d : | cut -c 1-6",cmd);
	if ( 0 != strlen(cmd))
	{
	    sprintf(cpeState.OUI, cmd);
	    *value = (char *) GS_STRNDUP(cmd, 6);
	}
	/* get parameter */
	else
	    *value = GS_STRDUP(cpeState.OUI);


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
/**@param DeviceInfo_ProductClass                     **/
CPE_STATUS getDeviceInfo_ProductClass(Instance *ip, char **value) {
	/* get parameter */
	*value = GS_STRDUP(CWMP_PRODUCT_CLASS);
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
    char snStr[30]={0};
    cmd_popen("state_cfg get sn", snStr);
    if( 0 != strlen(snStr))
    {
	sprintf(cpeState.SN , snStr);
	*value = GS_STRDUP(snStr); /* use all digits of MAC */
    }
    else
	*value = GS_STRDUP(cpeState.SN);

    return CPE_OK;
}
/**@endparam                                                      **/
/**@param DeviceInfo_HardwareVersion                     **/
CPE_STATUS getDeviceInfo_HardwareVersion(Instance *ip, char **value) {
    char hverStr[30]={0};
    cmd_popen("state_cfg get hver" , hverStr);
    if( 0 != strlen(hverStr))
    {
       sprintf(cpeState.HVER , hverStr);
       *value = GS_STRDUP(hverStr);
    }
    else
	*value = GS_STRDUP(cpeState.HVER);
    
    return CPE_OK;
}

extern int cpeBootUpCPEFirst;
/**@endparam                                                      **/
/**@param DeviceInfo_SoftwareVersion                     **/
CPE_STATUS getDeviceInfo_SoftwareVersion(Instance *ip, char **value) {
    char sverStr[30]={0};

#if defined(VERSION_NUM)
    strcpy(sverStr, STRINGIZE_VALUE_OF(VERSION_NUM));
#endif
    if(sverStr[0] == '\0')
        strcpy(sverStr, CWMP_HW_VERSION);

    if(cpeBootUpCPEFirst)
        *value = GS_STRDUP(cpeState.SoftwareVersion);
    else
        *value = GS_STRDUP(sverStr);
    return CPE_OK;
}
/**@endparam                                                      **/
/**@param DeviceInfo_SoftwareVersion                     **/
CPE_STATUS setDeviceInfo_SoftwareVersion(Instance *ip, char *value) {
	strncpy(cpeState.SoftwareVersion, value, 32);
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
/**@endobj DeviceInfo **/
