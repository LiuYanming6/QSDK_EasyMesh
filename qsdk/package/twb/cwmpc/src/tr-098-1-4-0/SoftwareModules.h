#ifndef _GS__SOFTWAREMODULES_H_
#define _GS__SOFTWAREMODULES_H_

/**@obj SoftwareModulesDeploymentUnit **/
typedef struct SoftwareModulesDeploymentUnit {
	char *	uUID;
	char *	dUID;
	char *	name;
	char *	status;
	unsigned char	resolved;
	char *	uRL;
	char *	vendor;
	char *	version;
	Instance *	executionUnitList[10+1];
	Instance *	executionEnvRef;
} SoftwareModulesDeploymentUnit;
/**@endobj SoftwareModulesDeploymentUnit **/

/**@obj SoftwareModulesExecEnv **/
typedef struct SoftwareModulesExecEnv {
	unsigned char	enable;
	char *	status;
	char *	name;
	char *	type;
	char *	vendor;
	char *	version;
	Instance *	activeExecutionUnits[10+1];
} SoftwareModulesExecEnv;
/**@endobj SoftwareModulesExecEnv **/

/**@obj SoftwareModulesExecutionUnit **/
typedef struct SoftwareModulesExecutionUnit {
	char *	eUID;
	char *	name;
	char *	execEnvLabel;
	char *	status;
	char *	requestedState;
	char *	executionFaultCode;
	char *	executionFaultMessage;
	char *	vendor;
	char *	version;
	Instance *	references[10+1];
	Instance *	supportedDataModelList[10+1];
} SoftwareModulesExecutionUnit;
/**@endobj SoftwareModulesExecutionUnit **/


#endif /* _GS__SOFTWAREMODULES_H_ */
