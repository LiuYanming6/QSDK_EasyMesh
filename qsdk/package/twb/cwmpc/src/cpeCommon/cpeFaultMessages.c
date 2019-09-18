/*
 * The CPEFaultTable defines the text descriptions and
 * the fault type for each CPE fault defined in
 * section A.5.1 CPE Fault Codes.
 *
 * Vender specific entries may be added at the end
 * of this table and assigned a fault code in the range
 * 9800..9999.
 *
 * The CPEFaultTable entries are ordered as defined by the
 * message index definitions (FAULT9000..FAULT9019) in CPEWrapper.h.
 *
 * The indexes may be saved as state data for queued transfers
 * if required. Of course, the text referenced by the indexes
 * must remain the same across firmware changes.
 *
 * Any changes in the entry positions or number of
 * entries in this table must be reflected by
 * the CPEWrapper.h FAULTxxxx definitions.
 *
 */
#include "../includes/CPEWrapper.h"
struct CPEFaultTable {
	int			faultCode;
	FAULTTYPE	faultType;
	const char  *faultMessage;
};

static struct CPEFaultTable cpeFaultTable[]= {
		{ 9000, eServer, "Method not Supported"} , /* 9000 */
		{ 9001, eServer, "Request Denied"},  		/* 9001 Request Denied */
		{ 9002, eServer, "Internal Error"},		/* 9002 Internal error */
		{ 9003, eClient, "Invalid Arguments"},	/* 9003 */
		{ 9004, eServer, "Resources Exceeded"},	/* 9004		*/
		{ 9005, eClient, "Invalid Parameter Name"},	/*9005 */
		{ 9006, eClient, "Invalid Parameter Type"},	/*9006 */
		{ 9007, eClient, "Invalid Parameter Value"},	/*9007 */
		{ 9008, eClient, "Attempt to set a non-writable parameter"},	/*9008*/
		{ 9009, eServer, "Notification request rejected"},		/* 9009 */
		{ 9010, eServer, "Download failure"},			/* 9010 */
		{ 9011, eServer, "Upload failure"},			/* 9011 */
		{ 9012, eServer, "File transfer server authentication failure"}, /* 9012 */
		{ 9013, eServer, "Unsupported protocol for file transfer"},	   /* 9013 */
		{ 9014, eServer, "File transfer failure: Unable to join multicast group"}, /* 9014 */
		{ 9015, eServer, "File transfer failure: unable to contact file server"},  /* 9015 */
		{ 9016, eServer, "File transfer failure: unable to access file"},			/* 9016 */
		{ 9017, eServer, "File transfer failure: unable to complete download"},	/* 9017 */
		{ 9018, eServer, "File transfer failure: file corrupted"},					/* 9018 */
		{ 9019, eServer, "File transfer failure: file authentication failure"},	/* 9019 */
		{ 9020, eClient, "File transfer failure: unable to complete download with specified time windows"},
		{ 9021, eClient, "Cancelation of file transfer not permitted in current transfer state"},
		{ 9022, eServer, "Invalid UUID Format"},
		{ 9023, eServer, "Unknown Execution Environment"},
		{ 9024, eServer, "Disabled Execution Environment"},
		{ 9025, eServer, "Deployment Unit to Execution Environment Mismatch"},
		{ 9026, eServer, "Duplicate Deployment Unit"},
		{ 9027, eServer, "System Resources Exceeded"},
		{ 9028, eServer, "Unknown Deployment Unit"},
		{ 9029, eServer, "Invalid Deployment Unit state"},
		{ 9030, eServer, "Invalid Deployment Unit Update - Downgrade not permitted"},
		{ 9031, eServer, "Invalid Deployment Unit Update - Version not specified"},
		{ 9032, eServer, "Invalid Deployment Unit Update - Version alread exists"},
		/* vender fault messages here */
		{ 0, eFaultUnknown, (char *)0 }					/* End of table, also FAULTNONE */
};
/*
 * cpeGetFaultMsgIndex returns the index of the fault message.
 * The message index is used to save the state of Transfer Complete faults.
 */
int cpeGetFaultMsgIndex( int faultCode ){
	if ( faultCode>=CPE_9000 && faultCode <= CPE_9032 )
		return faultCode-9000;
	return sizeof(cpeFaultTable);
}
const char *cpeGetFaultMessage( int faultCode ){
	int		i;
	for ( i=0; cpeFaultTable[i].faultType != eFaultUnknown; ++i){
		if ( faultCode == cpeFaultTable[i].faultCode )
			return cpeFaultTable[i].faultMessage;
	}
	return "";
}

FAULTTYPE cpeGetFaultType( int faultCode ){
	int		i;
	for ( i=0; cpeFaultTable[i].faultType != eFaultUnknown; ++i){
		if ( faultCode == cpeFaultTable[i].faultCode )
			return cpeFaultTable[i].faultType;
	}
	return eFaultUnknown;
}
/*
 * Backward compatibility function for transfer functions
 * saving the message table index into the cpestate.xml
 * persistent data. The index is used to retrieve the fault code and
 * associated text following restarts of the framework.
 */
const char *cpeGetFaultIndexedMessage( int index ){
	if ( index < FAULT_END )
		return cpeFaultTable[index].faultMessage;
	return "";
}

