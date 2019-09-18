/* UDP connection request test */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/signal.h>
#include <sys/time.h>
#include <signal.h>
#include <ctype.h>
#include <openssl/hmac.h>

static void toHex(const char* buffer, int size, char* output) 
{	int i;
	static char hexmap[] = "0123456789abcdef";
	const char* p = buffer;
	char* r = output;
	for (i=0; i < size; i++) {
		unsigned char temp = *p++;
		int hi = (temp & 0xf0)>>4;
		int low = (temp & 0xf);
			
		*r++ = hexmap[hi];
		*r++ = hexmap[low];
	}
	*r = 0;
}
/* 
* 
*/
char    *hostname;
int     port;

main(int argc, char *argv[])
{
    int fd;
    int i;
    struct hostent *netent;
    size_t insize;
    struct sockaddr_in addr;
    int ip;
    int outsz =0;
    char *username;
    char *password;
    char *cnonce="12345678";
    char hmac[20];
    char digBuf[41];
    char buf[512];
    
    int   hmacSize;
	int   ts = time(NULL);
	int   id = rand();
	int   lth;
	
	memset( buf, 0, sizeof(buf));
    if(argc ==1)
    {
        hostname="localhost";
    }else if (argc >= 2) {
	if  (strcmp(argv[1],"-h")==0 ) {
		printf("Use: cpereq <host> <port> <username> <password>\n"
			);
	} else
        	hostname=argv[1];
    }
    if (argc >=3 ) {
        port = atoi(argv[2]);
    }
    if (argc >=5) {
		username=strdup(argv[3]);
		password=strdup(argv[4]);
    } else {
    	exit(-1);
    }
     
    if ( (netent = gethostbyname(hostname))){
        ip = ntohl((int)*(int *)*netent->h_addr_list);
    }
    fprintf(stderr, "IP address = %x:%d", ip, port);
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(ip);
    if((fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
        abort();
    
	
	memset(buf, 0, sizeof(buf));
	memset(digBuf, 0, sizeof(digBuf));
	lth = snprintf(buf, sizeof(buf), "%d%d%s%s", ts, id, username, cnonce);
	/* stunMsg is zeroed prior to building so just use as is for padded 64 bytes*/
	HMAC(EVP_sha1(), password, strlen(password),
	     (const unsigned char*)buf, lth, 
                          hmac, &hmacSize);
	toHex( hmac, 20, digBuf);
	
    lth = snprintf(buf, sizeof(buf),
       "GET http://%s:%d?ts=%d&id=%d&un=%s&cn=%s&sig=%s HTTP/1.1\r\n",
    	hostname, port, ts, id, username, cnonce, digBuf);
    fprintf(stdout, "Request-URI:%s\n", buf);
    
    for (i=1; i<=4; ++i){
	    outsz = sendto(fd,&buf, lth, 0, (struct sockaddr *)&addr,
	    			 sizeof (struct sockaddr_in));
	    fprintf(stderr, "Sent %d bytes %s\n", outsz, strerror(errno));
    }
    return insize!=outsz;
    
    
    

}
                                                                                                                                                           
                                                                                                                                                           
                                                                                                                                                           
