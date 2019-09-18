/*
 * dnstest.c
 *
 *  Created on: Feb 17, 2009
 *      Author: dmounday
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <syslog.h>
#include <arpa/inet.h>
#include "../src/utils.h"
#include "../auxsrc/dns_lookup.h"

int main(int argc, char** argv){
	int		i;
	char	*host;
	InAddr	ip;
	InAddr	ip2;
	int		random = 0;
	int		nextIptest = 0;
	int		searchList = 0;
	int		iterations = 3;

	host = "acs.gatespace.net";
	extern int optind;
	extern char *optarg;
	char	c;
	while ((c=getopt(argc, argv, "rnhli:")) != -1) {
		switch (c) {

		case 'h':
			printf("Use: dnstest <options> \n");
			printf("  Tests the dns cacheing and sticky functions\n");
			printf("  -r Random selection test. Sets and removes cache host on each iteration.\n");
			printf("  -n Get next ip from cache test. Should return all of the possible IP addresses.\n");
			printf("  -l search host list test\n");
			printf("  -i iterations: number of iterations\n");

			exit(0);
			break;
		case 'r':
			random = 1;;
			break;
		case 'n':
			nextIptest = 1;
			break;
		case 'i':
			iterations = atoi(optarg);
			break;
		case 'l':
			searchList =1;
			break;
		default:
			break;
		}
	}
	if ( optind < argc ){
		host = argv[optind++];
	}


	if ( nextIptest ){
		dns_set_cache_host( host );
		for ( i=0; i<iterations; ++i){
			fprintf(stdout, "repeat - %d\n", i);
			if ( dns_lookup(host, SOCK_STREAM, &ip ) ){
				fprintf(stdout, "dns_lookup=%s\n", writeInIPAddr(&ip));
				while ( dns_get_next_ip(host, &ip) ){
					fprintf(stdout, "    next ip: %s\n", writeInIPAddr(&ip));
				}
				fprintf(stdout, "End of IP list, should be null -- ip=%s\n", writeInIPAddr(&ip));
			}
		}
	}

	if ( random ){
		for ( i=0; i<iterations; ++i ){
			dns_set_cache_host( host );
			if ( dns_lookup(host, SOCK_STREAM, &ip) ){
				fprintf(stdout, "%d Random lookup: %s\n", i, writeInIPAddr(&ip));

			} else {
				fprintf(stdout, "dns lookup failed\n");
			}
			dns_remove_cache_host(host);
		}
	}

	if ( searchList ){
		dns_set_cache_host(host);
		if ( dns_lookup(host, SOCK_STREAM, &ip ) ){
			fprintf(stdout, "dns_lookup=%s\n", writeInIPAddr(&ip));
			for ( i=0; i<iterations; ++i ){
				if ( dns_lookup(host, SOCK_STREAM, &ip2 ) ){
					fprintf(stdout, "cached lookup =%s\n", writeInIPAddr(&ip2));
					if ( !eqInIPAddr(&ip, &ip2 ))
						fprintf(stdout, "Cache lookup returned different address\n");
				}
			}
		} else {
			fprintf(stdout, "dns_lookup failed\n");
		}

	}

}

