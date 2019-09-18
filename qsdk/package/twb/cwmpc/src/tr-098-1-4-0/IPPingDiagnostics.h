#ifndef _GS__IPPINGDIAGNOSTICS_H_
#define _GS__IPPINGDIAGNOSTICS_H_

/**@obj IPPingDiagnostics **/
typedef struct IPPingDiagnostics {
	char *	diagnosticsState;
	Instance *	interface;
	char *	host;
	unsigned	numberOfRepetitions;
	unsigned	timeout;
	unsigned	dataBlockSize;
	unsigned	dSCP;
	unsigned	successCount;
	unsigned	failureCount;
	unsigned	averageResponseTime;
	unsigned	minimumResponseTime;
	unsigned	maximumResponseTime;
} IPPingDiagnostics;
/**@endobj IPPingDiagnostics **/




#endif /* _GS__IPPINGDIAGNOSTICS_H_ */
