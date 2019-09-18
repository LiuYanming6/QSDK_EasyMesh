#ifndef _GS__TIME_H_
#define _GS__TIME_H_

/**@obj Time **/
typedef struct GS_Time {
	char *	nTPServer1;
	char *	nTPServer2;
	time_t	currentLocalTime;
	char *	localTimeZone;
	char *	localTimeZoneName;
	unsigned char	daylightSavingsUsed;
	time_t	daylightSavingsStart;
	time_t	daylightSavingsEnd;
} GS_Time;
/**@endobj Time **/




#endif /* _GS__TIME_H_ */
