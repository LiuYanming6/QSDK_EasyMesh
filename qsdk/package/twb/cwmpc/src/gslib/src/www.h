#ifndef _WWW_H_
#define _WWW_H_

/*----------------------------------------------------------------------*
 * Utility Library for web related stuff
 *----------------------------------------------------------------------*/
#define PROTOCOL_SZ	10
#define HOSTNAME_SZ 257
#define URI_SZ		257

void www_StripTail(char *s);
void www_UrlDecode(char *s);
void www_UrlEncode(const char *s, char *t);

/*----------------------------------------------------------------------*
 * parse url on form:
 *  "<proto>://<host>[:<port>][<uri>]"
 *  returns
 *    0 if parse ok
 *   -1 if parse failed
 *  port sets to 0 if no port is specified in URL
 *  uri is set to "" if no URI is specified
 */
#include "utils.h"
int www_ParseUrl(const char *url, char *proto, char *host, int *port, char *uri);
int www_EstablishConnection(InAddr *host_addr, int port, int *sock_fd);

#endif
