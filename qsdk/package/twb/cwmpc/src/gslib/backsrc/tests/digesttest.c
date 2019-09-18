/*
 * digestest.c
 *
 *  Created on: Aug 31, 2009
 *      Author: dmounday
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "../src/utils.h"

SessionAuth wwwSa;
SessionAuth authSa;
SessionAuth genSa;

char	realm[]="cwmp-device";
char	domain[]="/cwmp";
char	user[]="don";
char	pw[]="abc123";
char	uri[]="/creq";

static void dumpSA( SessionAuth *authSa){
	fprintf(stdout, "realm =    %s\n", authSa->realm);
	fprintf(stdout, "nonce =    %s\n", authSa->nonce);
	fprintf(stdout, "domain=    %s\n", authSa->domain);
	fprintf(stdout, "opaque=    %s\n",authSa->opaque);
	fprintf(stdout, "cnonce=    %s\n", authSa->cnonce);
	fprintf(stdout, "algorithm= %s\n", authSa->algorithm);
	fprintf(stdout, "qop=       %s\n",authSa->qop);
	fprintf(stdout, "uri:       %s\n",authSa->uri);
	fprintf(stdout, "response=  %s\n",authSa->response);
	fprintf(stdout, "Method:    %s\n",authSa->method);
	fprintf(stdout, "nc:        %d\n\n",authSa->nonceCnt);
}

int main(int argc, char** argv){

	int		i;
	eAuthentication ath;
	char	*wwwHdr;
	char	*authHdr;



	if ( wwwHdr=generateWWWAuthenticateHdr(&wwwSa, realm, domain, "GET")){
		fprintf(stdout, "Server: generateWWWAuthentication:\n%s\n", wwwHdr);
		dumpSA(&wwwSa);

		fprintf(stdout, "Client: parseWWWAuthentication:\n");
		ath = parseWWWAuthenticate( wwwHdr, &authSa	);
		fprintf(stdout, "\nAuthentication = %d\n", ath);
		dumpSA( &authSa);

		/* now try again using lib generate function */
		fprintf(stdout, "Client: generateAuthorization:\n");
		if ( authHdr=generateAuthorizationHdrValue( &genSa, wwwHdr, "GET", uri, user, pw )){

			fprintf(stdout, "%s\n", authHdr);
			dumpSA( &genSa);

			fprintf(stdout, "Server: ParseAuthorization:\n");
			i = parseAuthorizationHdr(authHdr, &wwwSa, user, pw);
			fprintf(stdout, "requestDigest=   %s\n", wwwSa.requestDigest);
			fprintf( stdout, " ************ %s\n", i==0? "Failed": "Passed");

			dumpSA( &wwwSa);
		}

	}

}
