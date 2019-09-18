#ifndef _CPELOG_H_
#define _CPELOG_H_

void cpeDbgLog( int mask, const char *fmt, ...);
void cpeLog(int level, const char* fmt, ...);
void cpeVlog(int level, const char* fmt, va_list ap);
void cpeInitLog(int flag, int mask);

#endif

