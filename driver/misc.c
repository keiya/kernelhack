#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include "misc.h"

#define MAX_BUF_SZ 65536

MODULE_LICENSE("GPL");

char buf[MAX_BUF_SZ];
size_t buf_sz = 0;

void printm(const char *func, const char *msg)
{
    printk("<miscv2><%s>%s\n",func,msg);
}
void printmn(const char *func, const char *msg, int d)
{
    printk("<miscv2><%s>%s %d\n",func,msg,d);
}

static int misc_open(struct inode *inode, struct file *fp)
{
    printm(__func__,"open");
    return 0;
}

static ssize_t misc_read(struct file *fp, char __user *ubuf, size_t size, loff_t *ppos)
{
    int read_size;
    int offset;

    read_size = size;

    if (size > buf_sz)
      read_size = buf_sz;
    else
      read_size = size;

    if (unlikely(copy_to_user(ubuf, buf, read_size))) {
        return -EFAULT;
    }

    printmn(__func__,"size=",size);
    printk("<miscv2>>>>>>read_sz=%d,ptr=%p\n",read_size,ppos);

    *ppos += read_size;
    buf_sz -= read_size;

    printk("<miscv2><<<<<read_sz=%d,ptr=%p\n",read_size,ppos);
	return read_size;
}

static ssize_t misc_write(struct file *fp, const char __user *ubuf, size_t size, loff_t *ppos)
{
    if (size > MAX_BUF_SZ) {
        return -ENOMEM;
    }
    if (unlikely(copy_from_user(buf, ubuf, size))) {
        return -EFAULT;
    }
    printmn(__func__,"size=",size);
    printk("<miscv2>%s\n",buf);

    buf_sz += size;

    return size;
}

static int misc_ioctl(struct file *fp, unsigned int cmd, unsigned long arg)
{
    printm(__func__,"ioctl");
    printk("<miscv2>cmd=%d\n",cmd);
    query_arg_t q;
 
    switch (cmd)
    {
        case QUERY_GET_VARIABLES:
            q.status = status;
            if (copy_to_user((query_arg_t *)arg, &q, sizeof(query_arg_t)))
            {
                return -EACCES;
            }
            break;
        case QUERY_CLR_VARIABLES:
            status = 0;
            break;
        case QUERY_SET_VARIABLES:
            if (copy_from_user(&q, (query_arg_t *)arg, sizeof(query_arg_t)))
            {
                return -EACCES;
            }
            status = q.status;
            break;
        default:
            return -EINVAL;
    }
 
    return 0;
    return 0;
}

static int misc_release(struct inode *inode, struct file *filp)
{
    printk("%s\n", __func__);
    return 0;
}



static struct file_operations misc_fops = {
    .owner      = THIS_MODULE,
    .open       = misc_open,
    .read       = misc_read,
    .write      = misc_write,
    .unlocked_ioctl = misc_ioctl, // Linux >= 2.6.35
    .release    = misc_release,
    //.poll       = misc_poll,
};

static struct miscdevice misc_dev = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = "misc",
    .fops = &misc_fops,
};

static int __init misc_init(void)
{
    int rv;
    if ((rv = misc_register(&misc_dev))) {
        printm(__func__,"misc_register failed");
        return rv;
    }
    printm(__func__,"misc_register success");
    return 0;
}


static void __exit misc_exit(void)
{
    misc_deregister(&misc_dev);
    printm(__func__,"unload");
}


module_init(misc_init);
module_exit(misc_exit);

