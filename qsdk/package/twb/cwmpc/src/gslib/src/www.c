#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#include "www.h"

#ifdef DMALLOC
#include "dmalloc.h"
#endif
/* #define DEBUG 1   */

/*----------------------------------------------------------------------*
 * Converts hexadecimal to decimal (character):
 */
static char hexToDec(char *what)
{
   char digit;

   digit = (what[0] >= 'A' ? ((what[0] & 0xdf) - 'A')+10 : (what[0] - '0'));
   digit *= 16;
   digit += (what[1] >= 'A' ? ((what[1] & 0xdf) - 'A')+10 : (what[1] - '0'));

   return (digit);
}

/*----------------------------------------------------------------------*
 * Unescapes "%"-escaped characters in a query:
 */
static void unescapeUrl(char *url)
{
   int x, y, len;

   len = strlen(url);
   for (x = 0, y = 0; url[y]; x++, y++) {
      if ((url[x] = url[y]) == '%' && y < (len - 2)) {
	  url[x] = hexToDec(&url[y+1]);
	  y += 2;
      }
   }
   url[x] = '\0';
}

/*----------------------------------------------------------------------*/
void www_UrlDecode(char *s)
{
   char *pstr = s;

   /* convert plus (+) to space (' ') */
   for (pstr = s; pstr != NULL && *pstr != '\0'; pstr++) {
       if (*pstr == '+')
	   *pstr = ' ';
   }
   unescapeUrl(s);
}

/*----------------------------------------------------------------------*/
void www_UrlEncode(const char *s, char *t)
{
  while (*s) {
    if (*s == ' ') {
      *t++ = '+';
    } else if (isalnum(*s)) {
      *t++ = *s;
    } else {
      /* hex it */
      *t++ = '%';
      sprintf(t, "%2x", *s);
      t += 2;
    }
    s++;
  }
  *t = '\0';
}

/*----------------------------------------------------------------------*
 * parse url on form:
 *  "<proto>://<host>[:<port>][<uri>]"
 *  or the IPv6 forms of:
 *  "http://[xxxx::xx:xx:xx:xx]:8080/acsadmin/cpeserver"
 *     port and uri are both optional.
 *
 *  host and uri are limited to a 256 null terminated string length.
 *  returns
 *    0 Hostname is specified: Address family not known.
 *    2 Literal IP address specified: Address family is AF_INET
 *   10 Literal IP address specified: Address family is AF_INET6.
 *   -1 if parse failed
 *  port sets to 0 if no port is specified in URL
 *  uri is set to "" if no URI is specified
 */
int www_ParseUrl(const char *url, char *proto, char *host, int *port, char *uri) {
	int n;
	char *p;
	int res = -1;

	*port = 0;
	host[0]='\0';
	uri[0]='\0';
	proto[0]='\0';
	strcpy(uri, "");

	if(url == NULL) //Segmentation fault
		return -1;

	/* proto */
	p = (char *) url;
	if ((p = strchr(url, ':')) == NULL) {
		return -1;
	}
	n = p - url;
	if ( n < PROTOCOL_SZ ){
		strncpy(proto, url, n);
		proto[n] = '\0';
	} else
		return -1;

	/* skip "://" */
	if (*p++ != ':')
		return -1;
	if (*p++ != '/')
		return -1;
	if (*p++ != '/')
		return -1;
	if (*p == '[') {
		/* chk if http://[x:...  Its a literal IPv6 address */
		char *hp = host;
		int  l = 0;
		++p;
		while (*p && *p != ']') {
			*hp++ = *p++;
			if ( l > HOSTNAME_SZ-1 ) return -1;
			++l;
		}
		if (*p && *p != ']')
			return -1; /* missing closing ] */
		++p;           /* move past ] */
		res = AF_INET6;
		*hp= '\0';
	} else {
		/* host */
		char *hp = host;
		int l = 0;
		while (*p && *p != ':' && *p != '/' ) {
			*hp++ = *p++;
			if ( l>HOSTNAME_SZ-1) return -1;
			++l;
		}
		*hp = '\0';
		if (strspn(host, "0123456789.") == strlen(host)) {
			/* if digits then check if it was an IPv4 literal address */
			res = AF_INET;
		} else
			res = 0;
	}
	if (strlen(host) == 0)
		return -1;

	/* end */
	if (*p == '\0') {
		return res;
	}

	/* port */
	if (*p == ':') {
		p++;
		if ( isdigit(*p))
			*port = atoi(p);
		else
			return -1;
		while (isdigit(*p)) ++p;
	}
	/* uri */
	if (*p == '/') {
		strncpy(uri, p, URI_SZ-2);
		*(uri+URI_SZ-1) = '\0';
	} else
		*uri = '\0';
	return res;
}

/*----------------------------------------------------------------------*
 * returns
 *  0   if ok  (fd contains descriptor for connection)
 *  -1  if socket couldn't be created
 *  -2  if connection function could not be started.
 *
 * sock_fd will hold the socket.
 * The caller of www_Establishconnection must wait until write is possible
 * i.e. setListenerType(sockfd, ..., iListener_Write)
 * this to avoid blocking.
 */
int www_EstablishConnection(InAddr *host_addr, int port, int *sock_fd)
{
  int fd;

  SockAddrStorage sa;
  struct sockaddr_in *sp = (struct sockaddr_in *)&sa;
  long flags;
  //int res; //-Werror=unused-but-set-variable

  memset(sp, 0, sizeof(sa));

  SET_SockADDR(sp, htons(port), host_addr);

  if ((fd = socket(sp->sin_family, SOCK_STREAM, 0)) < 0) {
    return -1;
  }

  /* set non-blocking */
  flags = (long) fcntl(fd, F_GETFL);
  flags |= O_NONBLOCK;
  //res = fcntl(fd, F_SETFL, flags); //-Werror=unused-but-set-variable
  fcntl(fd, F_SETFL, flags);

  errno = 0;
  if (connect(fd, (struct sockaddr*)sp, SockADDRSZ(sp)) < 0) {
    if (errno != EINPROGRESS) {
      /* connect failed */
      close(fd);
      return -2;
    }
  }

  *sock_fd = fd;
  return 0;
}

/*----------------------------------------------------------------------
 * removes any trailing whitespaces, \r and \n
 * it destroys its argument...
 */
void www_StripTail(char *s)
{
  if (*s != '\0') {
    while(*s) s++;
    s--;
    while(*s == '\r' || *s == '\n' || *s == ' ' || *s == '\t') {
      *s = '\0';
      s--;
    }
  }
}


/*======================================================================*
 * module test code
 *======================================================================*/
#ifdef TEST
int main(int argc, char **argv)
{
  int res;
  char proto[256];
  char host[256];
  int port;
  char uri[256];

  strcpy(proto, "");

  res = www_ParseUrl(argv[1], proto, host, &port, uri);

  printf("result=%d\n", res);
  printf("proto= \"%s\"\n", proto);
  printf("host=  \"%s\"\n", host);
  printf("port=  %d\n", port);
  printf("uri=   \"%s\"\n", uri);
  return 0;
}
#endif
