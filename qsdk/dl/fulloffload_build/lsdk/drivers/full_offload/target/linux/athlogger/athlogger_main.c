/*
Copyright © 2012 Qualcomm Atheros, Inc.
All Rights Reserved.
Qualcomm Atheros Confidential and Proprietary.
*/

#include <linux/types.h>
#include <linux/mm.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/console.h>
#include <linux/moduleparam.h>
#include <linux/string.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/wait.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/ioctl.h>
#include "athlogger.h"

/* Function Prototypes */
static void athcon_msg_to_athlog(struct console *con, const char *msg,
                                                     unsigned int len);
/* Global Definitions */
static athlogger_t* athlogger;
static char *mode = "none";
static unsigned int livelog = 0;

/* athlog console structure */
static struct console athlogger_console = {
        .name   = "athlog",
        .flags  = CON_ENABLED,
        .write  = athcon_msg_to_athlog,
};

static int athlog_common_init(athlogger_t *logger)
{
        int ret = 0;
        unsigned long flags;

        spin_lock_init(&logger->lock);

        /* Trasport Layer Init */
        spin_lock_irqsave(&logger->lock, flags);
        athlog_transport_register(logger);
        spin_unlock_irqrestore(&logger->lock, flags);

        logger->state = ATHLOGGER_STATE_BOOT;

        return ret;
}

static void athlog_common_clean(athlogger_t *logger)
{
        athlog_transport_unregister(logger);
}

/* console callback for data */
static void athcon_msg_to_athlog(struct console *con, const char *msg, 
                                                      unsigned int len)
{
        unsigned long flags;

        if (athlogger->console_log_enabled ) {
            spin_lock_irqsave(&athlogger->lock, flags);
            athlog_buf_write(athlogger->logbuf, msg, len);
            spin_unlock_irqrestore(&athlogger->lock, flags);
        }

        /* Any Sharing Alogo Enabled */
        if(athlogger->share_algo)
           schedule_work(&athlogger->share_worker);
}

/* Enable console log */
static void athlog_console_enable(void) 
{
        athlogger->console_log_enabled = 1;
        /* Dump existing printks on registration */
        athlogger_console.flags |= CON_PRINTBUFFER;
        register_console(&athlogger_console);
}

/* Disable console log */
static void athlog_console_disable(void)
{
        unregister_console(&athlogger_console);
        athlogger->console_log_enabled = 0;
}

athlogger_t *athlog_get_handle(void)
{
        return athlogger;
}

static inline void athlog_share_algo_set(athlogger_share_algo_t algo)
{
        athlogger->share_algo = algo;
}

static int athlog_share_all(athlogger_t *logger)
{
        unsigned long flags;
        unsigned int frag, send = 0;
        unsigned char *buf;
        athlog_share_status_t status;

        /* TODO: make sure we don't require locking while reading the buffer */
        while(athlog_buf_data_cnt(logger->logbuf) ) {
            spin_lock_irqsave(&logger->lock, flags); /* lock */
            buf = athlog_buf_read_addr(logger->logbuf);
            frag = athlog_buf_data_to_end(logger->logbuf);
            frag = min(frag, (unsigned int)ATHLOGGER_SHARE_MAX_PKT_SIZE);
            spin_unlock_irqrestore(&logger->lock, flags);/* unlock */

            status = athlog_send_log(logger, buf, frag);
            if(status != ATHLOGGER_SHARE_SUCESS) {
               ATHLOGGER_DBG_PRINT("\n%s: logbuf send error: %d", __func__, status);
               break;
            }

            spin_lock_irqsave(&logger->lock, flags); /* lock */
            athlog_buf_fwd_read(logger->logbuf, frag);
            send += frag;
            spin_unlock_irqrestore(&logger->lock, flags);/* unlock */
        }

        return send;
}

static void athlog_share_worker(struct work_struct *work)
{
        athlogger_t *logger = container_of(work, athlogger_t, share_worker);

        /* if immediate share required, share it */
        if(logger->share_algo & ATHLOGGER_SHARE_IMMEDIATE) {
            athlog_share_all(logger);
        }
        return;
}

/* share log to the Host */
static void athlog_target_reboot_log_share(athlogger_t *logger)
{  
        while(1) {
           athlog_share_all(logger);
           if (is_athlog_buf_empty(logger->logbuf)) {
             break;
           } else {
              msleep(100);
           }
        }
        return;
}

/* athlog_target_boot_worker :
 * 1. Init Target Mode Resorces.
 * 2. Send the remaining previous boot log to Host. 
 * 3. Enable new Logging after the complete log is shared. 
 */
static void athlog_target_boot_worker(struct work_struct *work)
{
        athlogger_t *logger = container_of(work, athlogger_t, boot_worker);

        /* Send Previous Log */
        athlog_target_reboot_log_share(logger);

        /* set sharing Algorithm */
        athlog_share_algo_set(livelog);

        /* start new logging */
        athlog_buf_reset(logger->logbuf);
        athlog_console_enable();
        athlog_buf_set_valid(logger->logbuf);

        logger->state = ATHLOGGER_STATE_NORMAL;

        return;
}

static int athlog_target_init(athlogger_t *logger)
{
        int ret = 0;

        logger->logbuf = (athlogger_buf_t *)ATHLOGGER_TARGET_BUF_SRAM_ADDRESS;
        if(logger->logbuf == NULL) {
            printk("%s: logbuf allocation falied", __func__);
            return (-1);
        }
        athlog_buf_init(logger->logbuf, ATHLOGGER_TARGET_BUF_MAX_SIZE);
        /* if logbuffer is invalid, reset before tarnsport layer init */
        if(!is_athlog_buf_valid(logger->logbuf)) {
           printk("%s: logbuf invalid. resetting \n", __func__);
           athlog_buf_reset(logger->logbuf);
           athlog_buf_set_valid(logger->logbuf);
        }

        /* create and start log share worker */
        INIT_WORK(&athlogger->share_worker, athlog_share_worker);
        INIT_WORK(&athlogger->boot_worker, athlog_target_boot_worker);

        return ret;
}

static void athlog_target_clean(athlogger_t *logger)
{
        /* Disable and unregister comsole logging */
        if(logger->console_log_enabled)
            athlog_console_disable();

        /* Target buffer is located in SRAM, we can't free it. */
        /* Before unload, send the remaining log to Host*/
        athlog_target_reboot_log_share(logger);

        /* Reset the buffer */
        athlog_buf_reset(logger->logbuf);

}

/* athlog_host_boot_worker :
 * Init Host Mode Resorces.
 */
static void athlog_host_boot_worker(struct work_struct *work)
{

}

static int athlog_host_init(athlogger_t *logger)
{
        int ret = 0;

        /* allocate logbuffer */
        athlog_buf_alloc(logger->logbuf, ATHLOGGER_HOST_LIVE_BUF_MAX_SIZE);
        if(logger->logbuf == NULL) {
            printk("%s: logbuf allocation falied", __func__);
            return (-1);
        }
        athlog_buf_init(logger->logbuf, ATHLOGGER_HOST_LIVE_BUF_MAX_SIZE);
        /* if logbuffer is invalid, reset before tarnsport layer init */
        if(!is_athlog_buf_valid(logger->logbuf)) {
           athlog_buf_reset(logger->logbuf);
           athlog_buf_set_valid(logger->logbuf);
        }

        /* allocate crash buffer */
        athlog_buf_alloc(logger->crashbuf, ATHLOGGER_HOST_CRASH_BUF_MAX_SIZE);
        if(logger->crashbuf == NULL) {
            printk("%s: crashbuf allocation falied", __func__);
            return (-1);
        }
        athlog_buf_init(logger->crashbuf, ATHLOGGER_HOST_CRASH_BUF_MAX_SIZE);
        /* if logbuffer is invalid, reset before tarnsport layer init */
        if(!is_athlog_buf_valid(logger->crashbuf)) {
           athlog_buf_reset(logger->crashbuf);
           athlog_buf_set_valid(logger->crashbuf);
        }

        /* init char interface */
        ret = athlog_char_iface_init(logger);

        INIT_WORK(&athlogger->boot_worker, athlog_host_boot_worker);

        return ret;
}

static void athlog_host_clean(athlogger_t *logger)
{

        /* Free the live buffer */
        athlog_buf_free(logger->logbuf);
        /* Free the crash buffer */
        athlog_buf_free(logger->crashbuf);

        /* init char interface */
        athlog_char_iface_clean(logger); 
}

/* Init module called during insmod */
static int __init init_athlog_module(void)
{
	int ret = 0;

        /* Init athlogger structure */
        athlogger = kzalloc(sizeof(athlogger_t), GFP_KERNEL);
        if (athlogger == NULL) {
          printk("\nCan't Allocate athlogger memory");
          return -1;
        }
        memset(athlogger, 0, sizeof(athlogger_t));

        if (strncmp(mode,"target", 6) == 0) {
            printk("\nathlogger mode: target");
            athlogger->mode = ATHLOGGER_MODE_TARGET;
            /* init boot work*/
            ret = athlog_target_init(athlogger);
        } else if (strncmp(mode,"host", 4) == 0) {
            printk("\nathlogger mode: host");
            athlogger->mode = ATHLOGGER_MODE_HOST;
            /* init boot work*/
            ret = athlog_host_init(athlogger);
        } else {
            printk("\nathlogger invalid mode defined.");
            printk("please use \"insmod athlogger.ko mode=[host/target]\"\n");
            ret = -1;
        }

        if(ret < 0) {
            if(athlogger->mode == ATHLOGGER_MODE_TARGET)
               athlog_target_clean(athlogger);
            else if (athlogger->mode == ATHLOGGER_MODE_HOST)
               athlog_host_clean(athlogger);
            kfree(athlogger);
            return ret;
        }

        /* init transport */
        ret = athlog_common_init(athlogger);

        return ret;
}

/* Cleanup module called during rmmod */
static void __exit exit_athlog_module(void)
{
        if (athlogger->mode & ATHLOGGER_MODE_TARGET) {
            athlog_target_clean(athlogger);
        } else if (athlogger->mode & ATHLOGGER_MODE_HOST) {
            athlog_host_clean(athlogger); 
        }

        athlog_common_clean(athlogger);

        kfree(athlogger);
}


module_init(init_athlog_module);
module_exit(exit_athlog_module);
module_param(mode, charp, 0600);
module_param(livelog, int, S_IRUGO);
MODULE_PARM_DESC(mode, "athlogger mode none/target/host");
MODULE_LICENSE("Dual BSD/GPL");


