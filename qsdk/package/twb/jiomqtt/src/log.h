#ifndef _APP_LOG_H
#define _APP_LOG_H

#include <errno.h>
#include <string.h>
#include <syslog.h>

#define LOGINIT(prog) openlog((prog), LOG_PID, LOG_DAEMON)

#define LOGINITCONSOLE(prog) openlog((prog),                          \
    LOG_CONS | LOG_NDELAY | LOG_PERROR | LOG_PID, LOG_DAEMON)

#define LOG_ENABLE_UPTO(level) setlogmask(LOG_UPTO(level))

//"[file %s] [line %d] [func %s] [NOTICE]:"

#define LDEBUG(fmt, ...)                                              \
  syslog(LOG_DEBUG,                                                   \
         "[%s:%d]: "                                                  \
         fmt, __FILE__, __LINE__, ##__VA_ARGS__)

#define LTRACE(fmt, ...)                                              \
  syslog(LOG_DEBUG,                                                   \
         "[file %s] [line %d] [func %s] [TRACE]: "                    \
         fmt, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)

#define LINFO(fmt, ...)                                               \
  syslog(LOG_INFO,                                                    \
         "[%s:%d]: "                                                  \
         fmt, __FILE__, __LINE__, ##__VA_ARGS__)

#define LEMERGENCY(fmt, ...)                                          \
  syslog(LOG_EMERGENCY,                                               \
         "[%s:%d]: "                                                  \
         fmt, __FILE__, __LINE__, ##__VA_ARGS__)

#define LALERT(fmt, ...)                                              \
  syslog(LOG_ALERT,                                                   \
         "[%s:%d]: "                                                  \
         fmt, __FILE__, __LINE__, ##__VA_ARGS__)

#define LCRITICAL(fmt, ...)                                           \
  syslog(LOG_CRITICAL,                                                \
         "[%s:%d]: "                                                  \
         fmt, __FILE__, __LINE__, ##__VA_ARGS__)

#define LERROR(fmt, ...)                                              \
  syslog(LOG_ERR,                                                     \
         "[%s:%d]: "                                                  \
         fmt, __FILE__, __LINE__, ##__VA_ARGS__)

#define LWARNING(fmt, ...)                                            \
  syslog(LOG_WARNING,                                                 \
         "[%s:%d]: "                                                  \
         fmt, __FILE__, __LINE__, ##__VA_ARGS__)

#define LNOTICE(fmt, ...)                                             \
  syslog(LOG_NOTICE,                                                  \
         "[%s:%d]: "                                                  \
         fmt, __FILE__, __LINE__, ##__VA_ARGS__)

#define LOGCLOSE() closelog()

#endif //_APP_LOG_H
