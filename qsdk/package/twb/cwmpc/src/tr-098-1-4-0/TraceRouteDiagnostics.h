#ifndef _GS__TRACEROUTEDIAGNOSTICS_H_
#define _GS__TRACEROUTEDIAGNOSTICS_H_

/**@obj TraceRouteDiagnosticsRouteHops **/
typedef struct TraceRouteDiagnosticsRouteHops {
	char *	hopHost;
	char *	hopHostAddress;
	unsigned	hopErrorCode;
	unsigned	hopRTTimes;
} TraceRouteDiagnosticsRouteHops;
/**@endobj TraceRouteDiagnosticsRouteHops **/

/**@obj TraceRouteDiagnostics **/
typedef struct TraceRouteDiagnostics {
	char *	diagnosticsState;
	Instance *	interface;
	char *	host;
	unsigned	numberOfTries;
	unsigned	timeout;
	unsigned	dataBlockSize;
	unsigned	dSCP;
	unsigned	maxHopCount;
	unsigned	responseTime;
} TraceRouteDiagnostics;
/**@endobj TraceRouteDiagnostics **/




#endif /* _GS__TRACEROUTEDIAGNOSTICS_H_ */
