#ifndef STUN_H_
#define STUN_H_
/* Original Header definitions from vovida.org with major modifications for C */
/* */
#define STUN_MAX_STRING 256
#define STUN_MAX_UNKNOWN_ATTRIBUTES 8
#define STUN_MAX_MESSAGE_SIZE 2048

#define STUN_PORT 3478

// define some basic types
typedef unsigned char  UInt8;
typedef unsigned short UInt16;
typedef unsigned int   UInt32;
typedef unsigned long long UInt64;
typedef struct { unsigned char octet[16]; }  UInt128;

/// define a structure to hold a stun address
#define IPV4FAMILY     0x01

// define  stun attribute
#define MAPPEDADDRESS         0x0001
#define RESPONSEADDRESS       0x0002
#define SOURCEADDRESS         0x0004
#define CHANGEDADDRESS        0x0005
#define USERNAME              0x0006
#define MESSAGEINTEGRITY      0x0008
#define ERRORCODE             0x0009
#define REFLECTEDFROM         0x000B


/* Optional STUN attributes for TR-111 */
#define CONNECTIONREQUESTBINDING 0xc001
#define BINDINGCHANGE            0xc002

// define types for a stun message
#define BINDREQUESTMSG          0x0001
#define BINDRESPONSEMSG         0x0101
#define BINDERRORRESPONSEMSG    0x0111

typedef struct
{
      UInt16 msgType;
      UInt16 msgLength;
      UInt128 tid;
} StunMsgHdr;


typedef struct
{
      UInt16 type;
      UInt16 length;
      UInt8  octet[0];
} StunAtrHdr;

typedef struct StunAddress4
{
      UInt16 port;
      UInt32 addr;
} StunAddress4;

typedef struct
{
      UInt8 pad;
      UInt8 family;
      StunAddress4 ipv4;
} StunAtrAddress4;

typedef struct
{
      UInt32 value;
} StunAtrChangeRequest;

typedef struct
{
      UInt16 pad; // all 0
      UInt8 errorClass;
      UInt8 number;
      char reason[STUN_MAX_STRING];
      UInt16 sizeReason;
} StunAtrError;

typedef struct
{
      UInt16 attrType[STUN_MAX_UNKNOWN_ATTRIBUTES];
      UInt16 numAttributes;
} StunAtrUnknown;

typedef struct
{
      char value[STUN_MAX_STRING];
      UInt16 sizeValue;
} StunAtrString;

typedef struct
{
      char hash[20];
} StunAtrIntegrity;

/*
 * Response to a Binding Request
 */
typedef struct StunResponse {
	UInt16   msgType;
	UInt16   msgLth;
	int      errorCode;
	StunAtrAddress4 mappedAddr;
	StunAtrAddress4 sourceAddr;
	StunAtrAddress4 changedAddr;
	StunAtrAddress4 reflectFrom;
} StunResponse;

#define ADD_USERNAME	 0x0001
#define ADD_RESPONSEADDR 0x0002
#define ADD_INTEGRITY	 0x0004
#define ADD_CONNREQ      0x0008
#define ADD_BINDINGCHANGE 0x0010
#define MAXKEEPALIVE         300   /* secs */
#define MIN_DISCOVERY_TIME   10
#define MIN_DISCOVERY_DELTA   5

int  startStun(void);
int  reStartStun(void);
void stopStun(void);

#endif /*STUN_H_*/
