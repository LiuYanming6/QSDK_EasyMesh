
/*----------------------------------------------------------------------*
 * Gatespace
 * Copyright 2006 Gatespace Networks, Inc., All Rights Reserved.
 * Gatespace Networks, Inc. confidential material.
 *----------------------------------------------------------------------*
 * File Name  : notify.c
 * Description:	Implementation of notification, save and restore of
 *              object notification attributes.
 *----------------------------------------------------------------------*
 * $Revision: 1.16 $
 *
 * $Id: notify.c,v 1.16 2010/02/03 21:46:58 dmounday Exp $
 *----------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include <sys/select.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <syslog.h>
#include <string.h>

#ifdef DMALLOC
	#include "dmalloc.h"
#endif
#include "../includes/sys.h"
#include "../includes/paramTree.h"
#include "../includes/rpc.h"
#include "../includes/CPEWrapper.h"
#include "../gslib/src/utils.h"
#include "../gslib/src/event.h"
#include "../gslib/src/xmlWriter.h"
#include "../gslib/src/xmlParser.h"
#include "../gslib/src/xmlParserSM.h"
#include "../soapRpc/rpcUtils.h"
#include "../soapRpc/xmlTables.h"
#include "../gslib/src/event.h"
#include "../gslib/src/protocol.h"
#include "../gslib/src/wget.h"
#include "../soapRpc/rpcMethods.h"
#include "../soapRpc/cwmpSession.h"
#include "notify.h"


//#define DEBUG
#ifdef DEBUG
	#define DBGPRINT(X) fprintf X
#else
	#define DBGPRINT(X)
#endif

extern char wpPathName[];
extern CWMPObject CWMP_RootObject[];

void writeParamValues( eCWMPType pType, char *n, char *value, XMLWriter *xp);
/*
* Parameter Notification tracking functions
*/
static int numOfActPass;  /* number of parameters with active| passive attributes set */
typedef struct ParamNPtrs {
	int attribCnt;	/* number of parameters with passive or active attribute*/
	int	paramCnt;	/* number of parameter values in xmlWriter buffer */
	int changed;	/* number of parameter with value change */
	int activeCnt;	/* number of active Notification parameter with value change*/
} ParamNPtrs;

static int paramCopy(CWMPObject *o, CWMPParam *p, Instance *ip,
									 void *wxp, void *nPtr, int update )
{
	char	*val=NULL;
	XMLWriter *xp = (XMLWriter *) wxp;
	ParamNPtrs *np = (ParamNPtrs *)nPtr;
	IData 	*idp;

	if ( (idp = cwmpGetIData(p,ip)) ) {
		if ( idp->notify&(FORCED_INFORM|FORCED_ACTIVE|DEFAULT_ACTIVE|DEFAULT_PASSIVE|RPCNOTIFYMASK )) {
			if ( update&INIT_NOTIFY){
				/* initialize NOTIFY bits from Default bits */
				if ( idp->notify&DEFAULT_PASSIVE)
					idp->notify|= PASSIVE_NOTIFY;
				if ( idp->notify&DEFAULT_ACTIVE)
					idp->notify|= ACTIVE_NOTIFY;
			}
			if ( np && (idp->notify&(ACTIVE_NOTIFY|PASSIVE_NOTIFY)) )
				np->attribCnt++;
			if ( p->pGetter && p->pGetter(ip, &val)==CPE_OK){
				if ( (idp->notify&NOTIFY_ON)
					 && !streq(idp->dataCopy, val) ) {
					/* parameter is different*/
					DBGPRINT((stderr, "%s %s -> %s\n", p->name, idp->dataCopy, val));
					if (xp){
						writeParamValues( p->pType, wpPathName, val, xp);
						if (np) np->paramCnt++;
					}
					if (np){
						if (idp->notify &(ACTIVE_NOTIFY|FORCED_ACTIVE))
							np->activeCnt++;
						np->changed++;
					}
					if (update&UPDATE_COPY) {
						MOVESTR(idp->dataCopy, val);
					} else
						FREESTR(val);
					     /* the following clause is just used to generate the */
					     /* Forced inform parameters in the ParameterList */
				} else if (idp->notify&FORCED_INFORM && xp) {
					writeParamValues( p->pType, wpPathName, val, xp);
					if (np) np->paramCnt++;
					FREESTR(val);
				} else
					FREESTR(val);
			}
		}
	} /* this is an initialization error if no IData */
	return eOK;
}
/*
* void cwmpInitNotifyTracking(int mask)
* If mask is INIT_NOTIFY then walk the parameter tree and set the
* ACTIVE and PASSIVE attributes from the Default value.
*
* If mask is UPDATE_COPY then
 *  initialize the parameter change tracking by walking
*   the parameter tree and making a copy of any parameter that have
*   a notification attribute set. This function is called once following
*   a boot strap.
*
*/
void cwmpInitNotifyTracking(int mask)
{
	cwmpFrameworkReset();
	cwmpWalkPTree( DESCEND|CBPARAMS,CWMP_RootObject,NULL, paramCopy, NULL, NULL, mask);
}

/* int cwmpCheckNotification( XMLWriter *xp, int *paramCnt, int update);
*       Walks the parameter tree and returns the count of changed
*   parameters with an active, passive, or forced notification attribute. If the
*   *xp is not NULL the getWritParamVAlue function is called to create
*   a list of the changed parameter to be included in the Inform message.
*   If the notifyCnt parameter is not NULL the number of parameters in
*   the parameter list is written to int *paramCnt. If the update flag is
*   set any copies of the parameter values are updated to the current
*   values.
*/
int cwmpCheckNotification( XMLWriter *xp, int *notifyCnt, int update)
{

	ParamNPtrs nCntrs;
	nCntrs.activeCnt = nCntrs.paramCnt = nCntrs.changed = nCntrs.attribCnt = 0;
	cwmpFrameworkReset();
	cwmpWalkPTree( DESCEND|CBPARAMS,CWMP_RootObject,NULL, paramCopy, xp, &nCntrs, update);
	*notifyCnt = nCntrs.paramCnt;
	numOfActPass = nCntrs.attribCnt;
	return nCntrs.changed;
}
/* int cwmpAnyActiveNotification()
*       Walks the parameter list and returns the number of parameter
*  with active or forced active notification attributes that have changed.
*/
int cwmpAnyActiveNotifications(void)
{
	ParamNPtrs nCntrs;
	cwmpFrameworkReset();
	nCntrs.activeCnt = nCntrs.paramCnt = nCntrs.changed = nCntrs.attribCnt =0;
	cwmpWalkPTree( DESCEND|CBPARAMS,CWMP_RootObject,NULL, paramCopy, NULL, &nCntrs, 0);
	numOfActPass = nCntrs.attribCnt;
	return nCntrs.activeCnt;
}

/* int cwmpResetActiveNotification()
*       Walks the parameter list and returns the number of parameter
*  with active or forced active notification attributes that have changed while
*  also updating the IData copy of the parameter value. NOTE: This also
*  updates the IData copy for passive notification parameters.
*/
int cwmpResetActiveNotification(void)
{
	ParamNPtrs nCntrs;
	cwmpFrameworkReset();
	nCntrs.activeCnt = nCntrs.paramCnt = nCntrs.changed = nCntrs.attribCnt =0;
	cwmpWalkPTree( DESCEND|CBPARAMS,CWMP_RootObject,NULL, paramCopy, NULL, &nCntrs, UPDATE_COPY);
	numOfActPass = nCntrs.attribCnt;
	return nCntrs.activeCnt;
}

#ifdef COMPRESS_SAVED_ATTRIBUTES
/*************************************************************************************/
/* This will not work in all cases. If a internally created instance of an object is */
/* create the sequence count will change. These objects will not be present on reboot*/
/* and the sequence numbers will be off. Object example is the .LANDEvice.Hosts.Hosts.[i]*/

/* void saveNotifyAttributes(void)
*       Walks the parameter tree and creates a table of walkSeqIndex,attribute-value.
*  The walkSeqIndex is the sequence in which the cwmpWalkPTree visits the instance
* of the parameter.
*/
static int procAttrib(CWMPObject *o, CWMPParam *p, Instance *ip,
									 void *pi, void *seq, int restore )
{
	static int imageIndex;
	PAttribImage *pImage = (PAttribImage*)pi;
	PAttrib *pa;
	if ( *seq==0 )
		/* first call */
		imageIndex = 0;
	pa = &pImage->attrib[imageIndex];
	if ( restore ) {
		if (pa->walkSeq==*seq) {
			++imageIndex;
			IData *idp = cwmpGetIData(o,p,ip);
			if (idp)
				idp->notify = pa->savedAttr;
			else
				/* error-- */
				return eStopWalk;
		}
	} else {
		IData *idp;
		if ( (idp = cwmpGetIData(o,p,ip)) ) {
			if ( idp->notify&(ACTIVE_NOTIFY|PASSIVE_NOTIFY )){
				pa->savedAttr = idp->notify;
				pa->walkSeq = *seq;
				++imageIndex;
				++pImage->aCount;
			}
		} else
			return eStopWalk; /* error */
	}
	++*seq;
	return eOK;
}

void cwmpSaveNotifyAttributes(void)
{
	ParamNPtrs nCntrs;
    struct PAttribImage *pImage;
	int seq = 0;

	if ( numOfActPass ) {
		int iLth = sizeof(PAttribute)+sizeof(PAttrib)*numOfActPass);
		if ( (pImage= (PAttribute*) GS_MALLOC( iLth )) {
			strncpy(pImage->signature, "NotifyAttrib", sizeof(pImage->signature));
			cwmpFrameworkReset();
			if ( cwmpWalkPTree( DESCEND|CBPARAMS,CWMP_RootObject, NULL, procAttrib, pImage, &seq, 1)
				 == eOk)
				cpeSaveNotifyAttributes( p, iLth);
			else
				cpeLog(LOG_ERR, "Error saving notification attributes");
			return;

		}
	}
}


void cwmpRestoreNotifyAttributes( void )
{
	ParamNPtrs nCntrs;
    struct PAttribImage *pImage;
	int seq = 0;

	if ( numOfActPass ) {
		int iLth = sizeof(PAttribute)+sizeof(PAttrib)*numOfActPass);
		if ( (pImage= (PAttribute*) GS_MALLOC( iLth )) {
			memset( pImage, 0, iLth);
			if ( strncmp(pImage->signature, "NotifyAttrib", sizeof(pImage->signature))==0 ){
				cwmpFrameworkReset();
				if ( cwmpWalkPTree( DESCEND|CBPARAMS,CWMP_RootObject, NULL, procAttrib, pImage, &seq, 1)
					 != eOk)
					cpeLog(LOG_ERR, "Error restoring notification attributes");
			} else
				cpeLog(LOG_ERR, "Unable to restore notification attributes- bad signature");
			return;

		}
	}
}

#else

/*************************************************************************************/
/* This version writes out the full name of the parameter. */

/* void saveNotifyAttributes(void)
*       Walks the parameter tree and creates a table of walkSeqIndex,attribute-value.
*  The walkSeqIndex is the sequence in which the cwmpWalkPTree visits the instance
* of the parameter.
*/
static int procAttrib(CWMPObject *o, CWMPParam *p, Instance *ip,
									 void *vxp, void *cntPtr, int unused )
{
	IData *idp;
	if ( (idp = cwmpGetIData(p,ip)) ) {
		if ( idp->notify&(RPCNOTIFYMASK|DEFAULT_ACTIVE|DEFAULT_PASSIVE)){
			XMLWriter *xp = (XMLWriter *)vxp;
			xmlOpenTagGrp(xp, "SetParameterAttributesStruct");
			xmlPrintTaggedData(xp, "Name", "%s", wpPathName);
			xmlPrintTaggedData(xp, "Notification", "%d", idp->notify&RPCNOTIFYMASK);
			xmlCloseTagGrp(xp);
		}
	} else
		return eStopWalk; /* error */
	return eOK;
}

void cwmpSaveNotifyAttributes(void)
{
	XMLWriter *xp;
	int		*cnt;
	char	*buf;

	if ( (xp=xmlOpenWriter(2000, 0)) ) {
		cwmpFrameworkReset();
		xmlOpenTagGrp(xp, "ParameterList");
		if ( cwmpWalkPTree( DESCEND|CBPARAMS,CWMP_RootObject, NULL, procAttrib, xp, &cnt, 0)
			 == eOK) {
			int mlth;
			xmlCloseTagGrp(xp);
			mlth = xmlGetMsgSize(xp);
			if ( (buf = xmlSaveCloseWriter(xp)) != NULL) {
				cpeSaveNotifyAttributes( buf, mlth );
				GS_FREE(buf);
			} else {
				cpeLog(LOG_ERR,"No memory saving notification attributes");
			}
		} else {
			cpeLog(LOG_ERR, "Error saving notification attributes");
			xmlCloseWriter(xp);
		}
		return;
	}
}

extern XmlNodeDesc *cwmpAttributeListDesc;

void cwmpRestoreNotifyAttributes(void)
{
	char *buf;
	int   mlth;
	if ( (buf = cpeRestoreNotifyAttributes( &mlth )) ) {
		/* parse buffer and set attributes */
        XML_STATUS    status;
		ParseHow    parseReq;

		RPCRequest  *rp = GS_MALLOC( sizeof (RPCRequest) );
		memset( rp, 0 , sizeof(RPCRequest));
		parseReq.topLevel = cwmpAttributeListDesc;	 /* take a short-cut into the parser */
		parseReq.nameSpace = NULL;
		rp->rpcMethod = eSetParameterAttributes; /* force rpcMethod to SetParameterAttribute*/
		status = xmlParseGeneric( &rp, NULL, buf, mlth, &parseReq);
		if (status==XML_STS_OK) {
			ParameterAttributesStruct *pa = rp->ud.setPAttrReq.paList;
			while ( pa ) {
				CWMPParam *p;
				IData *idp;
				if ( (p = cwmpFindParameter(pa->pName))){
                    if ( (idp=cwmpGetIData(p, cwmpGetCurrentInstance())) )
                        idp->notify = (idp->notify&FWNOTIFYMASK)|(pa->notification&RPCNOTIFYMASK);
				} /* else ignore error */
				pa = pa->next;
			}
		}
		cwmpFreeRPCRequest( rp );
		GS_FREE(buf);
	}
	return;
}

#endif

