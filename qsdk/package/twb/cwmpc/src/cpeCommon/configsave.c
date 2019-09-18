/*----------------------------------------------------------------------*
 * Gatespace
 * Copyright 2011-2012 Gatespace Networks, Inc., All Rights Reserved.
 * Gatespace Networks, Inc. confidential material.
 *----------------------------------------------------------------------*
 * File Name  :configsave.c
 * Description: Saves the current configuration of the CPE to a text
 *              file and restores the configuration.
 *----------------------------------------------------------------------*
 * $Revision: 1.13 $
 *
 * $Id: configsave.c,v 1.13 2012/05/10 17:38:01 dmounday Exp $
 *----------------------------------------------------------------------*/


#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include <sys/select.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/time.h>
#include <syslog.h>
#include <string.h>
#include <ctype.h>

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
#include "runstates.h"
#include "targetsys.h"
//#define DEBUG
#ifdef DEBUG
#define DBGPRINT(X) fprintf X
#else
#define DBGPRINT(X)
#endif

extern char wpPathName[512];
extern CWMPObject CWMP_RootObject[];
void initAvailableInterface(Instance *ip);

#ifdef CONFIG_RPCCHANGEDUSTATE
void restoreDUConfig(const char *, const char *, char *);
void restoreEUConfig(const char *, const char *, char *);
void saveDUConfig(FILE *, CWMPObject *, Instance *);
void saveEUConfig(FILE *, CWMPObject *, Instance *);
#endif
/*
 * restore extra configuration data
 */

static void restoreExtraConfig( const char *oName, const char *pName, char *p){
#ifdef CONFIG_RPCCHANGEDUSTATE
	if ( strstr(oName, "DeploymentUnit")){
		restoreDUConfig(oName, pName, p);
	} else if ( strstr(oName, "ExecutionUnit")){
		restoreEUConfig(oName, pName, p);
	}
#endif
	// other extra config data ?
}
/*
 * save any parameter or other state data that must be restored for this
 * object.
 */
static void saveExtraConfig(FILE *cf, CWMPObject *o, Instance *ip){
#ifdef CONFIG_RPCCHANGEDUSTATE
	if ( streq(o->name, "DeploymentUnit")){
		saveDUConfig(cf, o, ip);
	} else if ( streq(o->name, "ExecutionUnit")){
		saveEUConfig(cf, o, ip);
	}
#endif
	// other extra config data checks here?
}

/*
 * scan down parameter list for the object to
 * check if any parameters are writable.
 */
static int anySettableP( CWMPObject *o){
	CWMPParam *p = o->paramList;
	int	r = 0;
	while ( p != NULL && p->name ){
		if ( p->pSetter ){
			// The Alias parameter is not supported by Amendment 3 and should be omitted
			// from the data models used with Amendment 3 CWMP.
			// If the Alias parameter is included as the only set-able parameter then
			// the object's parameters are not saved. This version of the CWMPc does not support
			// the Alias parameter. It will be supported for Amendment 4.
			if ( strcmp(p->name, "Alias")!=0)
				++r;
		}
		++p;
	}
	return r;
}
static FILE *cf;
/*
 * Callback from tree walk to write out the
 * object data.
 */
static int cb(CWMPObject *o, CWMPParam *p, Instance *ip,
									 void *w, void *n, int u )
{
	if ( p == NULL ){
		// Visiting an Object node.
		// check for any set-able parameters
		if ( (o->oType == eObject && anySettableP(o))
			 || ( ip!=NULL ) ) {
			fprintf(cf, "\n%s\n", wpPathName);
			// check if this object has non-writable parameter data that is needed to restore state.
			saveExtraConfig(cf, o, ip );
		}
	} else {
		// Visiting a parameter
		if ( p->pSetter ){
			char  *b = NULL;
			p->pGetter( ip, &b);
			fprintf(cf, "  %s = %s\n", p->name, b? b: "");
			GS_FREE(b);
		}
	}
	return eOK;
}
/*
 * Device parameter data is written to a file.
 * This could be written to any persistent memory
 */

void cpeSaveConfig(void)
{
	if ( (cf = fopen(CONFIG_FILENAME, "w")) ){
		cwmpSetRunState(RUNSTATE_SAVECONFIG);
		cwmpWalkSubTree( NULL, cb, NULL, NULL, 1);
		fprintf(cf,"\n");
		fclose(cf);
	}
}

static void setParameter( char *oname, char *pname, char *value){
	char	name[512];
	CWMPParam *p;
	Instance  *ip;
	strcpy(name, oname);
	strcat(name, pname);

	if ( (p=cwmpFindParameter(name)) ){
		ip = cwmpGetCurrentInstance();
		if ( p->pSetter ) {
			DBGPRINT((stderr,"set%s=%s\n",name,value));
			p->pSetter( ip, value);
		}
	}
}

static void configureObject( char *oname ){
	Instance *ip;
	//CWMPObject *o; //-Werror=unused-but-set-variable
	if ( (ip = cwmpInitObjectInstance( oname )) ){
		//o = cwmpGetCurrentObject();
		cwmpGetCurrentObject();
	}
}

static void runCommit( char *oname ){
	CWMPObject *o;
	if ( (o=cwmpFindObject(oname)) ){
		if ( o->pCommitObj ){
			Instance *ip = o->oType==eObject? o->iList: cwmpGetCurrentInstance();
			o->pCommitObj( o, ip, COMMIT_SET);
		}
	}
}

/*
 * Read the configuration data and create any specified
 * object instances.
 * Returns: 1 - OK
 * 			0 - Not restored from file.
 * This function makes the first passe thru the file and
 * creates the necessary instances. This is necessary because some
 * objects can not be created directly but are result of
 * other instance creation. For example the
 * WANCommonInterfaceConfig.Connection.{i}. is created when
 * the WANConnectionDevice.{i}. is created.
 */
int cpeRestoreObjectConfig(void){
	FILE	*cf;
	char	oName[512];
	char	buf[257];

	if ( (cf = fopen(CONFIG_FILENAME, "r" )) == NULL ){
		if ( (cf =fopen(DEFAULT_CONFIG, "r")) == NULL)
			return 0;
	}
	cwmpSetRunState(RUNSTATE_RESTORE_OBJ);
	while ( fgets(buf, sizeof(buf), cf)){
		if ( buf[0] == '#'){
			/* cpe config data */
			;// used for other config data if required.
		}
		if ( isalpha(buf[0]) ){ /* not a blank or eol - Object name*/
			/* copy object name */
			buf[ strlen(buf)-1] = '\0'; /* strip \n off of end */
			strncpy( oName, buf, sizeof( oName));
			configureObject( oName );

		} else if ( buf[0] == ' '){ /* blank maybe parameter name */
			;
		} else if ( buf[0]== '\n' ) {
			;// end of object's paraemter list
		}
	}

	fclose(cf);
	return 1;
}
/*
 * this is a list of ManagementServer parameters that should not be
 * over written from the config.save data.
 */


static const char *xPnames[]= {
		"URL",
		"PeriodicInformEnable",
		"PeriodicInformInterval",
		"Username",
		"Password",
		"ConnectionRequestUsername",
		"ConnectionRequestPassword",
#ifdef CONFIG_TR111P2
		"STUNEnable",
		"STUNServerAddress",
		"STUNServerPort",
		"STUNUsername",
		"STUNPassword",
#endif
		NULL
};
static int excludeParameter( const char *o, const char *p ){
	const char **xp = xPnames;
	if ( strstr( o, "ManagementServer")){
		while ( *xp ){
			if ( streq(p, *xp))
				return TRUE;
			++xp;
		}
	}
	return FALSE;
}
/*
 * Read the configuration data and
 * set the set-able parameters.
 * Returns: 1 - OK
 * 			0 - Not restored from file.
 * This function makes the second passe thru the file and
 * sets the parameter data.
 */
int cpeRestoreParameterConfig(void){
	FILE	*cf;
	char	oName[512];
	char	pName[256];
	char	buf[257];

	oName[0] = '\0';
	if ( (cf = fopen(CONFIG_FILENAME, "r" )) == NULL ){
		if ( (cf =fopen(DEFAULT_CONFIG, "r")) == NULL)
			return 0;
	}
	cwmpSetRunState(RUNSTATE_RESTORE_PARAM);
	while ( fgets(buf, sizeof(buf), cf)){
		memset(oName,0x0,sizeof(oName));
		memset(pName,0x0,sizeof(pName));
		if ( buf[0]=='#' ){
			/* flush off cpe parameters */
			while ( fgets(buf, sizeof(buf), cf)){
				if ( isalpha(buf[0]) )
					break;
			}
		}
		if ( isalpha(buf[0]) ){ /* not a blank or eol - Object name*/
			/* copy object name */
			buf[ strlen(buf)-1] = '\0'; /* strip \n off of end */
			strncpy( oName, buf, sizeof( oName));
			configureObject( oName);
		} else if ( buf[0] == ' '){ /* blank maybe parameter name */
			if ( strspn( buf, " ") == 2 ){
				char *p;
				if ( (p = strstr( &buf[2], " = "))) {
					buf[ strlen(buf)-1] = '\0'; /* strip \n off of end */
					strncpy( pName, &buf[2], p-&buf[2] );
					pName[p-&buf[2]] = '\0';
					if ( pName[0]=='@' ){
						restoreExtraConfig( oName, pName+1, p+3);
					} else if ( !excludeParameter( oName, pName) ){
						p += 3; /* start of value */
						setParameter( oName, pName, p );
					}
				}
			}
		} else if ( buf[0]== '\n' ) {
			if ( oName[0] )
				runCommit( oName );
			oName[0] = '\0';
			;// end of object's parameter list
		}
	}
	fclose(cf);
	return 1;
}
/*
 * Restore configuration if present
 * Returns:
 * 		0: no saved configuration. Nothing restored.
 * 		1: Saved configuration restored.
 */
int cpeRestoreConfig(void){
	if (cpeRestoreObjectConfig() == 0) {
		return 0;
	} else {
		/* May need to initialize the created objects or instances here */
		cpeRestoreParameterConfig();
		// ??
		// initialize convenience pointers, etc.
		cpeLog(LOG_INFO, "Data Model restored from config.save");
		return 1;
	}
}
