#ifndef _GS__DEVICEINFO_H_
#define _GS__DEVICEINFO_H_

/**@obj DeviceInfoVendorLogFile **/
typedef struct DeviceInfoVendorLogFile {
        char *  name;
        unsigned        maximumSize;
        unsigned char   persistent;
} DeviceInfoVendorLogFile;
/**@endobj  **/


/**@obj DeviceInfo **/
typedef struct DeviceInfo {
	char *	manufacturer;
	char *	manufacturerOUI;
	char *	modelName;
	char *	description;
	char *	serialNumber;
	char *	hardwareVersion;
	char *	softwareVersion;
	char *	provisioningCode;
	unsigned	upTime;
} DeviceInfo;
/**@endobj DeviceInfo **/




























#endif /* _GS__DEVICEINFO_H_ */
