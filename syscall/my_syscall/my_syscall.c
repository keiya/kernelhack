#include <linux/kernel.h>
#include <linux/syscalls.h>

#define MAX_BUFFER_LIMIT 256

SYSCALL_DEFINE2( my_syscall, char __user *, ubuf, int , size )
{
    char kbuf[MAX_BUFFER_LIMIT];
    char tmp;
    int len;
    int i;

    if ( ! access_ok( VERIFY_WRITE, ubuf, MAX_BUFFER_LIMIT )) {
      printk("<MY_SYSCALL>access_ok() failed\n");
      return (-EFAULT);
    }

    len = strlen_user(ubuf) - 1;
    if (len > size)
      len = size;
    if( MAX_BUFFER_LIMIT < len )
    {
        printk("<MY_SYSCALL>exeed MAX_BUFFER_LIMIT\n");
        return( -ENOMEM );
    }

    if ( ! strncpy_from_user(kbuf,ubuf,len)) {
      printk("<MY_SYSCALL>strncpy_from_user() failed\n");
      return (-EFAULT);
    }

    for (i=0; i<len/2; ++i) {
        tmp = kbuf[i];
        kbuf[i] = kbuf[len-1-i];
        kbuf[len-1-i] = tmp;
    }

    return copy_to_user( ubuf, kbuf, len );
}
