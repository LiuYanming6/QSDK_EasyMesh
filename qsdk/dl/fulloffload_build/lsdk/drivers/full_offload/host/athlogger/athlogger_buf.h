#ifndef _ATHLOGGER_BUF_MGMT_
#define _ATHLOGGER_BUF_MGMT_

typedef struct _athlogger_buf {
    unsigned char* addr;       /* address of log buffer. Should not change between boots */
    unsigned int   total;      /* size of the total log buffer */
    unsigned int   signature;  /* To Know if Logging is ever started */
    unsigned char* start;      /* start address of log section */
    unsigned int   size;       /* max size of log section */
    unsigned int   head;       /* data write start index */
    unsigned int   tail;       /* data start index  */
} athlogger_buf_t;


#define athlog_buf_alloc(logbuf, size)          \
{                                               \
   logbuf = kzalloc(size, GFP_KERNEL);          \
}

#define athlog_buf_free(logbuf)      \
{                                    \
   if(logbuf)                        \
      kfree(logbuf);                 \
}

#define athlog_buf_init(logbuf, size)           \
{                                               \
   logbuf->addr = (unsigned char *)logbuf;      \
   logbuf->total = size;                        \
}

#define athlog_buf_reset(logbuf)                \
{                                               \
   logbuf->start = logbuf->addr + sizeof(athlogger_buf_t); \
   logbuf->size = logbuf->total - sizeof(athlogger_buf_t); \
   logbuf->head = 0;                            \
   logbuf->tail = 0;                            \
}

#define athlog_buf_set_valid(logbuf)            \
{                                               \
   logbuf->signature = ATHLOGGER_BUF_SIGNATURE; \
}

#define athlog_buf_total_size(logbuf)  (logbuf->total)

#define is_athlog_buf_valid(logbuf) (logbuf->signature == ATHLOGGER_BUF_SIGNATURE)
#define is_athlog_buf_empty(logbuf) (logbuf->tail == logbuf->head)

#define athlog_buf_write_addr(logbuf) (logbuf->start + logbuf->head)
#define athlog_buf_read_addr(logbuf)  (logbuf->start + logbuf->tail)

#define athlog_buf_fwd_read(logbuf, len)  \
{                                         \
    logbuf->tail += len;                  \
    if(logbuf->tail == logbuf->size)      \
      logbuf->tail = 0;                   \
}

#define athlog_buf_fwd_write(logbuf, len) \
{                                         \
    logbuf->head += len;                  \
    if(logbuf->head == logbuf->size)      \
      logbuf->head = 0;                   \
}

/* Returns data avaialable in the buffer */
#define athlog_buf_data_cnt(logbuf)       \
     (logbuf->head >= logbuf->tail ?      \
          (logbuf->head - logbuf->tail) : \
          (logbuf->head + (logbuf->size - logbuf->tail)))

/* Return space available. ranges from 0 ... size-1 */
#define athlog_buf_space(logbuf) {                        \
     (logbuf->head >= logbuf->tail ?                      \
          (logbuf->size - (logbuf->head - logbuf->tail)) : \
          (logbuf->tail - logbuf->head))

/* As we can overwrite the buffer, always return with reference to head */
#define athlog_buf_space_to_end(logbuf) (logbuf->size - logbuf->head)

/* Returns linear data available to the end of buffer */
#define athlog_buf_data_to_end(logbuf)    \
     (logbuf->head >= logbuf->tail ?      \
          (logbuf->head - logbuf->tail) : \
          (logbuf->size - logbuf->tail))


/* athlogger_buf_write : Dump the data to the Log buffer.
 * buffer is a circular buffer, so, buffer can overwrite
 * some of the previous data.
 */
#define athlog_buf_write(logbuf, msg, len )     \
{                                               \
   unsigned int left, frag;                     \
   unsigned int wrapped=0;                      \
   unsigned char *tmp;                          \
   tmp = (unsigned char *)msg;                  \
   for (left=len;left;) {                       \
        frag = athlog_buf_space_to_end(logbuf); \
        frag = min(left, frag);                 \
        memcpy((logbuf->start+logbuf->head), tmp, frag); \
        tmp += frag;                            \
        left -= frag;                           \
        athlog_buf_fwd_write(logbuf,frag);      \
        if(logbuf->head == 0) {                 \
           wrapped++;                           \
        }                                       \
   }                                            \
   if (wrapped > 1 )                            \
        logbuf->tail = logbuf->head + 1;        \
   else if (wrapped && (logbuf->head >= logbuf->tail)) \
        logbuf->tail = logbuf->head + 1;        \
}

#define athlog_buf_dump(logbuf)                 \
{                                               \
   printk("\n%s()++",__func__);                 \
   printk("\naddr:0x%x, total:%d, signature:0x%x",     \
       logbuf->addr,logbuf->total,logbuf->signature);    \
   printk("\nstart:0x%x size:%d, head:%d tail:%d", \
       logbuf->start, logbuf->size, logbuf->head, logbuf->tail); \
   printk("\n%s()--\n",__func__);               \
}


#define athlog_buf_dump_data(logbuf)            \
{                                               \
    unsigned char *ch;                          \
    unsigned int frag = 0;                      \
    athlogger_buf_t tmpbuf;                     \
    athlogger_buf_t *p_tmpbuf = &tmpbuf;        \
    memcpy(p_tmpbuf, logbuf, sizeof(athlogger_buf_t)); \
    printk("\n****************************\n"); \
    printk("\n%s Start \n", __func__);          \
    while(athlog_buf_data_cnt(p_tmpbuf)) {      \
        ch = athlog_buf_read_addr(p_tmpbuf);    \
        frag = athlog_buf_data_to_end(p_tmpbuf);\
        for(;frag;frag--)                       \
           printk("%c", *ch++);                 \
        athlog_buf_fwd_read(p_tmpbuf, frag)     \
    }                                           \
    printk("\n%s End ", __func__ );             \
    printk("\n****************************\n"); \
}

#endif /*_ATHLOGGER_BUF_MGMT_*/
