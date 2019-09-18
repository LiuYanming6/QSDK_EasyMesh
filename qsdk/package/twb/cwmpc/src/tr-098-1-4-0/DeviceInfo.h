#ifndef _GS__DEVICEINFO_H_
#define _GS__DEVICEINFO_H_

/**@obj DeviceInfo **/
typedef struct DeviceInfo {
	char *	manufacturer;
	char *	manufacturerOUI;
	char *	modelName;
	char *	description;
	char *	serialNumber;
	char *	hardwareVersion;
	char *	softwareVersion;
	char *	specVersion;
	char *	provisioningCode;
	unsigned	upTime;
	char *	deviceLog;
} DeviceInfo;
/**@endobj DeviceInfo **/





#endif /* _GS__DEVICEINFO_H_ */
