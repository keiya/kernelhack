#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/proc_fs.h> // proc_create
#include <asm/uaccess.h> // copy_from_user
#include <linux/time.h>

// /proc/PROCNAMEにインタフェースを作る
#define PROCNAME "driver/uptimetest"
#define MAXBUF 64

MODULE_DESCRIPTION("uptimetest");
MODULE_LICENSE("GPL");

static int buflen = 0;

// 読み込み
static ssize_t proc_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
    struct timespec uptime;
    get_monotonic_boottime(&uptime);
    char modtest_buf[MAXBUF];

    buflen = sprintf(modtest_buf,"%lu\n",(unsigned long) uptime.tv_sec);

    if (*f_pos >= buflen)
        return 0;

    if (copy_to_user(buf, modtest_buf, buflen) > 0)
        return -EFAULT;
    *f_pos += buflen;

    printk(KERN_INFO "proc_read len = %d\n", buflen);

    return buflen;
}

struct file_operations proc_fops = {
    .read = proc_read,
};

// モジュール初期化
static int __init modtest_module_init(void)
{
    struct proc_dir_entry *entry;

//  /proc/PROCNAME にインタフェース作成
    entry = proc_create(PROCNAME, 0, NULL, &proc_fops);

    if (!entry) {
        printk(KERN_ERR "proc_create failed\n");
        return -EBUSY;
    }
    printk(KERN_INFO "modtest is loaded\n");
    return 0;
}

// モジュール解放
static void __exit modtest_module_exit(void)
{
//  インタフェース削除
    remove_proc_entry(PROCNAME, NULL);

    printk(KERN_INFO "modtest is removed\n");
}

module_init(modtest_module_init);
module_exit(modtest_module_exit);
