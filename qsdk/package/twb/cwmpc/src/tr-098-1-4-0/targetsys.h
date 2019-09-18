#ifndef TARGETSYS_H_
#define TARGETSYS_H_
/*
* oemCWMPc compile time constant definitions.
*/
#define CWMP_SOFTWARE_VERSION "5.xxxxx"
#define CWMP_HW_VERSION		  "LinuxWS"
#define CWMP_PRODUCT_CLASS	  "IGD-TR-181-2-3-Test-Client"
#define CWMP_DEVICE_DESCRIPTION "Gatespace CWMP Test IGD"
#define CWMP_MANUFACTURER		"Gatespace Networks, Inc."
#define CWMP_MODELNAME			"TR-181 Device Test-Model"
#define CWMP_DEVICEDESCRIPTON  "Gatespace CWMP Quick Start Test Client for TR-181 SMM"

#ifdef CONFIG_RPCKICK
/*
 * Kick URL listener
 **/
#define LANKICK_PORT	30006
#define LANKICK_PATH	"/kick.html"
#endif

// comment following macro to use UDP socket for in-box IPC.
// UNIX domain sockets are used/required by SMM code.
#define USE_UNIX_DOMAIN_SOCKET
#ifdef USE_UNIX_DOMAIN_SOCKET
/*
* Unix domain socket path to listen to CPE events.
*/
#define CPELISTEN_NAME	"/tmp/CPEListener"
#else
/*
* UDP port to listen to CPE events.
* Port is used for cpelisten or for clientapi packets.
*/
#define CPELISTENPORT 	30006
#endif /* USE_UNIX_DOMAIN_SOCKET */

/*
 * UDP port to listen for CPE Client API messages
 * Required by top-level Makefile macro GENERATE_CLIENTAPI=true
 */
#define CPE_CLIENTAPI_PORT 30007
#ifdef CONFIG_CLIENTAPI
/*
 * If POLL_ACSACCESS_STATUS is defined the function
 * cpeGetACSAccessStatus() is called prior to attempting to
 * create a session with the ACS. If the return indicates that
 * the ACS is not accessible the session attempt is delayed.
 */
//#define POLL_ACSACCESS_STATUS
#define POLL_ACSACCESS_INTERVAL   20*1000  /* 20 seconds */
/*
 * Initial ACS session state. This is used to set the state if it is not
 * defined in the cpestate.xml file.
 */
#define ACS_INITIAL_SESSION_STATE	ACS_SESSION_ENABLED //ACS_SESSION_DISABLED
#else
#define ACS_INITIAL_SESSION_STATE	ACS_SESSION_ENABLED
#endif /*CONFIG_CLIENTAPI*/

/*
 * Alias based addressing support as defined in TR-069 amendment 4.
 * 1: Support Alias based addressing
 * 0: Disable alias based addressing.
 */
#define ALIAS_BASED_ADDRESSING	1	/* set to 0 to disable Alias based Addressing */

#ifdef USE_SSL
/* Include code to use server certificates for SSL */

#ifdef USE_CERTIFICATES
/* ACS Server Certificate File path */
#define SERVER_CERT     "/var/cert/server.pem"		 				 /* certificate file path */
/* Client certificate file chain path */
#define CLIENT_CERT		"/var/cert/client.pem"

/* Cipher list to use for SSL */
/* The SSLv3 and TLS prefixs are only used to initialize the interface to openSSl. */
/* They are ignored by openSSL when the cipher list is set */
//#define ACS_CIPHERS 	"RSA:DES:SHA+RSA:RC4:EDH-RSA-DES-CBC-SHA" 		 /* cypher list */
#define ACS_CIPHERS 	"TLSv1.2:AES128-SHA:AES256-SHA:DHE-RSA-AES128-SHA:DHE-RSA-AES256-SHA:" /* cypher list */
#else /* USE_CERTIFICATES */
#define ACS_CIPHERS 	"SSLv3:"
#define SERVER_CERT   	NULL
#define CLIENT_CERT		NULL
#endif /* USE_CERTIFICATES */

#else  /* ifdef USE_SSL*/
#define ACS_CIPHERS		NULL
#define SERVER_CERT		NULL
#define CLIENT_CERT		NULL

#endif /* ifdef USE_SSL*/
/*
* Timer values:
*/
#define SERVER_CONNECT_TIMEOUT	45*1000		/* in milliseconds */
/*
 * CWMPc state information files
 */
#define CPESTATE_FILENAME "/etc/cpestate.xml"
#define CPESTATE_FILENAME_DEFAULT "/etc/cpestate-default.xml"  /* Minimal default pre-configuration */
#define NOTIFYATTRIBFILE  "cpeAttributeSave"
#define CONFIG_FILENAME	  "config.save"         /* saved obj/parameter configuration*/
#define DEFAULT_CONFIG    "config.default"      /* used if not active config.save of obj/parameter data */
#define INTERFACECONFIG_FILENAME "ifconfig.save" /* saved interface configuration */

/*
 * Definitions for xmlWriter.c:xmlOpenWriter(bufsize, flags )
 */
#define SOAP_SENDBUFSZ		4096	/* Initial SOAP buffer, send data to ACS */
#define SOAP_SENDFAULTSZ	2048	/* Size of initial allocation for a SOAP fault msg */
#define	XML_WRITER_FLG		0x00		/* XML Writer flags.					*/
										/*          XML_SURPRESS_LF 0x02      - don't generate some of the LF */
										/*			XML_NOINDENT	0x04      - don't generate indentation */
										/*          XML_NOREALLOC	0x08      - don't realloc() on buffer full */


/*
 * Maximum size of message acceptable from ACS
 * Applied to total size of received ACS response.
 * This is the maximum size that a response buffer
 * can grow to.
 */
#define MAXWEBBUFSZ 128000
/*
* Maximum download file size
*/
#define MAXFILESIZE 100000000
/*
 * Download directory path:
 * The following macro defines the directory path for down loading files.
 * It is used in the CPETransfer.c file that contains the cpeDownloadSetup, and
 * cpeDownloadComplete.
 */
#define DOWNLOADDIRPATH   "/tmp"

/*
 * File to retrieve the vendor_encapsulated_options if using DHCP to retrieve
 * the ACS URL and Provisioning Code. Created by the /etc/dhclient-exit-hooks script if using
 * dhclient  (ISC DHCP implementation).
 *  See dhcpVendorsOptions.c for interfaces from cpeWriapper and framework.
 */
#define DHCP_VENDOR_OPT_FILE	"/tmp/vendor-options"
/*
 * Used to conditionally include some functions in common files.
 */
//#define IGD_DATAMODEL
/* */
#endif /*TARGETSYS_H_*/
