#include <linux/mm.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/console.h>
#include <linux/moduleparam.h>
#include <linux/string.h>
#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/wait.h>
#include <linux/cdev.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/ioctl.h>
#include <asm/uaccess.h>
#include "athlogger.h"

static athlog_char_iface_t *athlog_char_iface = NULL;


static int athlog_open(struct inode *inode, struct file *filp)
{
        unsigned int minor = 0;

        minor = iminor(inode);
 
        if(minor > ATHLOGGER_MINOR_MAX ) {
           printk("\n athlogger minor %d not supported ", minor);
           return -ENXIO;
        }
 
        /* open() called only if device existed. so no need to check for NULL*/
        if(athlog_char_iface->devlist[minor].status) {
            printk("\n athlogger char iface already in use.");
            return -EBUSY;
        }

        filp->private_data = minor;
        athlog_char_iface->devlist[minor].status = 1;

        return 0;
}

static int athlog_release(struct inode *inode, struct file *filp)
{
        unsigned int minor = 0;

        minor =  iminor(inode);
        if(minor > ATHLOGGER_MINOR_MAX )
           return -ENXIO;

        athlog_char_iface->devlist[minor].status = 0;

        return 0;
}


static ssize_t athlog_char_buf_read(athlogger_buf_t *buf,  
                          char __user *user_buf, size_t count)
{
        unsigned int frag, read = 0;
        unsigned char *ptr;

        while(count && athlog_buf_data_cnt(buf) ) {
            ptr = athlog_buf_read_addr(buf);
            frag = athlog_buf_data_to_end(buf);
            frag = min(frag, count);
            if (copy_to_user(user_buf, ptr, frag)) {
                return -EINVAL;
            }
            athlog_buf_fwd_read(buf, frag);
            read += frag;
            count -= frag;
        }

        return read;

}

/* Read function */
ssize_t athlog_read(struct file * filp, char __user *user_buf,
                         size_t count, loff_t * f_pos)
{
        unsigned long flags;
        unsigned int minor;
        athlogger_buf_t *buf =  NULL;
        athlogger_t *logger = athlog_char_iface->logger;
        unsigned int read;

        /* validate read parameters from user */
        if(logger == NULL)
            return 0;
 
        minor = (unsigned int)filp->private_data;
        if(minor == ATHLOGGER_MINOR_LIVE)
            buf = logger->logbuf;
        else if (minor == ATHLOGGER_MINOR_CRASH)
            buf = logger->crashbuf;
        else {
            printk("\n athlogger minor %d not supported ", minor);
            return -EINVAL;
        }
        if(buf == NULL)
            return 0;

        spin_lock_irqsave(&logger->lock, flags); /* lock */
        read = athlog_char_buf_read(buf, user_buf, count);
        spin_unlock_irqrestore(&logger->lock, flags);/* unlock */

        return read;
}

/* Write function */
ssize_t athlog_write(struct file * filp, const char __user * buf,
                         size_t count, loff_t * f_pos)
{
       ATHLOGGER_DBG_PRINT("%s not supported.\n", __func__);
       return -EINVAL;
}

/* Control of the device */
static int athlog_ioctl(struct inode *inode, struct file *filp,
                         unsigned int cmd, unsigned long arg)
{
       ATHLOGGER_DBG_PRINT("%s(): cmd 0x%x not supported.\n", __func__, cmd);
       return -EINVAL;
}

/* File operations structure */
struct file_operations  athlog_char_iface_fops = {
        .owner   = THIS_MODULE,
        .read    = athlog_read,
        .write   = athlog_write,
        .ioctl   = athlog_ioctl,
        .open    = athlog_open,
        .release = athlog_release,
};

/* Init Char Interface Details */
int athlog_char_iface_init(athlogger_t *logger)
{
        int ret = 0;

        athlog_char_iface = kzalloc(sizeof(athlog_char_iface_t), GFP_KERNEL);
        if(athlog_char_iface == NULL) {
            printk(KERN_WARNING "Can't allocate memory for Char Interface\n");
            return -1;
        }

        athlog_char_iface->major = ATHLOGGER_CHAR_IFACE_MAJOR;
        /* Crate a Char Interface */
        ret = register_chrdev(athlog_char_iface->major, 
                              ATHLOGGER_CHAR_IFACE_DEVNAME,
                              &athlog_char_iface_fops);
        if (ret < 0) {
            printk(KERN_WARNING "athlog: can't get major %d\n",
                                athlog_char_iface->major);
            kfree(athlog_char_iface);
            athlog_char_iface = NULL;
            return ret;
        }

        athlog_char_iface->logger = logger;
        logger->char_iface = athlog_char_iface;
        return ret;
}

/* Clean Char Interface Details */
void athlog_char_iface_clean(athlogger_t *logger)
{
        unregister_chrdev(athlog_char_iface->major,
                          ATHLOGGER_CHAR_IFACE_DEVNAME);
        kfree(athlog_char_iface);
        athlog_char_iface = NULL;
        logger->char_iface = NULL;
}


