/*
 * runtest.c
 *
 *	Read lines from stdin. Each line contains the name of an API function and
 *	it's arguments. In the form:
 *	           <api-name> [argument [[argument] ...] ] ]
 *	Such as:
 *	           SetParameterValue InternetGatewayDevice.ManagementServer.Username xyz
 *
 *	The pseudo functions are
 *	           delay seconds
 *
 *  Created on: Jun 24, 2009
 *      Author: dmounday
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

#define DEBUG

#include "clientapilib.h"
static char	api[257];
static int 	argcnt;
static char	arg[4][257];

void runAPI(void){
	int	i;
	int	maxSTime;
	int status;

	fprintf(stdout, "%s ", api);
	for(i=0; i<argcnt; ++i	){
		fprintf(stdout, " %s", arg[i]);
	}
	fprintf(stdout, "\n");
	if ( !strcmp(api, "StartSession")){
		if ( argcnt != 1){
			fprintf(stderr, "StartSession requires 1 argument");
		} else {
			maxSTime = atoi(arg[0]);
			status = cpeStartSession( maxSTime);
			fprintf(stdout, "Status =%d\n", status);
		}
	} else if (!strcmp(api, "SetPath")){
		if ( argcnt != 1){
			fprintf(stderr, "SetPath require 1 argument");
		} else {
			status = cpeSetPath(arg[0]);
			fprintf(stdout, "Status =%d\n", status);
		}
	} else if (!strcmp(api, "SetParameterValue")){
		if ( argcnt != 2){
			fprintf(stderr, "SetParameterValue requires 2 arguments");
		} else {
			status = cpeSetParameterValue(arg[0], arg[1]);
			fprintf(stdout, "Status =%d\n", status);
		}
	} else if (!strcmp(api, "GetParameterValue")){
		if ( argcnt != 1){
			fprintf(stderr, "GeParameterValue requires 1 argument");
		} else {
			char *value;
			status = cpeGetParameterValue(arg[0], &value );
			if ( status )
				fprintf(stdout, "Status =%d\n", status);
			else {
				fprintf(stdout, "Status =%d value=%s\n", status, value);
				free(value);
			}
		}
	} else if (!strcmp(api, "EndSession")){
		if ( argcnt != 0){
			fprintf(stderr, "EndSession has no arguments");
		} else {
			status = cpeEndSession();
			fprintf(stdout, "Status =%d\n", status);
		}
	} else if (!strcmp(api, "AddObject")){
		if ( argcnt != 1){
			fprintf(stderr, "AddObject requires 1 argument");
		} else {
			char *value;
			status = cpeAddObject(arg[0], &value );
			fprintf(stdout, "Status =%d arg=%s\n", status, value);
			free(value);
		}
	} else if (!strcmp(api, "DeleteObject")){
		if ( argcnt != 1){
			fprintf(stderr, "DeleteObject requires 1 argument");
		} else {
			status = cpeDeleteObject( arg[0] );
			fprintf(stdout, "Status =%d\n", status);
		}
	} else if (!strcmp(api, "delay")){
		if ( argcnt != 1){
			fprintf(stderr, "delay requires 1 argument");
		} else {
			sleep(atoi(arg[0]));
			fprintf(stdout, "  Completed\n");
		}
	} else if (!strcmp(api, "FindObjectPath")){
		if ( argcnt != 3){
			fprintf(stderr, "FindObjectPath requires 3 arguments");
		} else {
			char *value;
			status = cpeFindObjectPath(arg[0], arg[1], arg[2], &value );
			if ( status )
				fprintf(stdout, "Status =%d\n", status);
			else {
				fprintf(stdout, "Status =%d value=%s\n", status, value);
				free(value);
			}
		}
	} else if (!strcmp(api, "GetRPCMethods")){
		if ( argcnt != 0){
			fprintf(stderr, "GetRPCMethods has no arguments");
		} else {
			status = cpeGetRPCMethods();
			fprintf(stdout, "Status =%d\n", status);
		}
	} else if (!strcmp(api, "GetParameterNames")){
		if ( argcnt != 2){
			fprintf(stderr, "GetParameterNames requires 2 arguments");
		} else {
			int count;
			status = cpeGetParameterNames(arg[0], atoi(arg[1]), &count);
			if ( status )
				fprintf(stdout, "Status =%d\n", status);
			else {
				fprintf(stdout, "Status =%d Parameter Count=%d\n", status, count);
			}
		}
	} else if (!strcmp(api, "GetNextParameterName")){
		int getall = !strcasecmp( arg[0], "all");
		do {
			char *value;
			status = cpeGetNextParameterName( &value );
			if ( status )
				fprintf(stdout, "Status =%d\n", status);
			else if ( value ) {
				fprintf(stdout, "Status =%d name=%s\n", status, value);
				free(value);
			} else {
				fprintf(stdout, "Status =%d end-of-names\n", status);
				getall = 0;
			}
		} while ( getall );
	} else if (!strcmp(api, "CPEReboot")){
			status = cpeReboot();


	} else if (!strcmp(api, "CPEReset")){
			status = cpeReset();


	} else if (!strcmp(api, "StopCWMPc")){
		status = cpeStopCWMPc();
	}

	fprintf(stdout, "\n");
}

void usage(void) {
	printf("Use: testrun <host> \n");
	printf("   Script commands are input from stdin.\n");
}

int main( int argc, char *argv[] ) {

	char 	line[1024];
	char	*t;
//	char	*p;//-Werror=unused-but-set-variable
	char	*hostname;

	if (argc >= 2 ) {
		hostname = argv[1];
	} else {
		usage();
		exit(-1);
	}

	cpeLibInit( hostname, CPEAPI_PORT );
	while (fgets(line, sizeof(line), stdin ) != NULL ){
		//fprintf(stdout, "%s\n", line);
		//p = line;//-Werror=unused-but-set-variable
		argcnt = 0;
		memset( arg, 0, sizeof(arg));
		t = strtok(line, " \n\r");
		if ( t ) {
			strcpy(api, t);
			while ( (t = strtok(NULL, " \n\r\t"))) {
				strcpy(arg[argcnt], t);
				++argcnt;
			}
			runAPI();

		}
	}
	return 0;
}




