#ifndef _ATH_LOGGER_H_
#define _ATH_LOGGER_H_

#ifdef ATHLOGGER_DEBUG
#define ATHLOGGER_DBG_PRINT(_fmt, ...)  printk((_fmt), ## __VA_ARGS__)
#else
#define ATHLOGGER_DBG_PRINT(_fmt, ...) 
#endif

#include "athlogger_buf.h"

#define ATHLOGGER_TARGET_BUF_SRAM_ADDRESS    (0xbd005000) /* SRAM location */
#define ATHLOGGER_TARGET_BUF_MAX_SIZE        (12*1024)    /* 12 KB */
#define ATHLOGGER_BUF_SIGNATURE              (0xDEADBEEF) /* Signature */
#define ATHLOGGER_SHARE_MAX_PKT_SIZE         (1000)

#define ATHLOGGER_HOST_LIVE_BUF_MAX_SIZE     (4*1024)     /* 4 KB */
#define ATHLOGGER_HOST_CRASH_BUF_MAX_SIZE    (12*1024)    /* 12 KB */

/****************** athlogger definitions ***********************/
typedef enum {
    ATHLOGGER_MODE_NONE = 0,
    ATHLOGGER_MODE_TARGET = 1,
    ATHLOGGER_MODE_HOST = 2,
}athlogger_mode_t;

typedef enum {
    ATHLOGGER_SHARE_NONE=0x0,      /* Don't Share */
    ATHLOGGER_SHARE_IMMEDIATE=0x1, /* Imediate */
    ATHLOGGER_SHARE_THRESHOLD=0x2, /* Share Log after certain buffer Threshold */
    ATHLOGGER_SHARE_TIMER=0x4,     /* Share Log after certain time expiry */
}athlogger_share_algo_t;

typedef enum {
    ATHLOGGER_SHARE_SUCESS = 0,
    ATHLOGGER_SHARE_FAILED = -1,
    ATHLOGGER_SHARE_INVALID = -2,
}athlog_share_status_t;

typedef enum {
    ATHLOGGER_LOG_TYPE_CRASH = 0,
    ATHLOGGER_LOG_TYPE_LIVE = 1,
}athlogger_log_type_t;

typedef enum {
    ATHLOGGER_STATE_BOOT = 0,
    ATHLOGGER_STATE_NORMAL = 1,
}athlogger_state_t;

typedef struct _athlogger_t {
    athlogger_mode_t     mode;
    athlogger_state_t    state;
    unsigned int         console_log_enabled; /* Console Log is Enabled or Disabled */
    athlogger_share_algo_t  share_algo; /* Log Sharing Algorithm */
    spinlock_t           lock;
    athlogger_buf_t      *logbuf;     /* actual buffer to maintain the live log */
    athlogger_buf_t      *crashbuf;   /* actual buffer to maintain the crash log */
    struct work_struct   boot_worker;
    void *share_handle;         /* Pinter to the share handle */
    struct work_struct   share_worker;
    void *char_iface;           /* Pinter to the char interface handle */
} athlogger_t;

athlogger_t *athlog_get_handle(void);

typedef struct _athlog_hdr{
    uint8_t   type;
    uint8_t   flags;
    uint16_t  len;
} athlog_hdr_t;

/********************* Log Sharing definitions **********************/
void athlog_transport_register(athlogger_t *logger);
void athlog_transport_unregister(athlogger_t *logger);
athlog_share_status_t
athlog_send_log(athlogger_t *logger, const char *msg, int len);


/******************** Char interface definitions ********************/
#define ATHLOGGER_CHAR_IFACE_MAJOR    (140)
#define ATHLOGGER_CHAR_IFACE_DEVNAME  "athlogger"
#define ATHLOGGER_MINOR_LIVE          (0)
#define ATHLOGGER_MINOR_CRASH         (1)
#define ATHLOGGER_MINOR_MAX           (1)

typedef struct {
    unsigned int status;    /* usage status */ 
}athlog_chrdev_t;

typedef struct athlog_char_iface {
    unsigned int major;     /* char device major number */
    athlog_chrdev_t devlist[ATHLOGGER_MINOR_MAX + 1];     /* char device major number */
    athlogger_t *logger;    /* reference to logger */
}athlog_char_iface_t;

int athlog_char_iface_init(athlogger_t *logger);
void athlog_char_iface_clean(athlogger_t *logger);

#endif /* _ATH_LOGGER_H_ */

