#include <linux/kernel.h>
#include <linux/syscalls.h>

/* cf. http://softwaretechnique.jp/Linux/SystemCall/sc_03.html */

SYSCALL_DEFINE2(my_syscall,char __user *,buf,int,count)
{
    long err;
    printk("<MY_SYSCALL>%s\n",text);
    if (count < sizeof(text))
      return -ENOMEM;

    err = copy_to_user(buf,text,sizeof(text));
    return err;
}
