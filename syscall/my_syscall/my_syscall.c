#include <linux/kernel.h>
#include <linux/syscalls.h>

#define MAX_BUFFER_LIMIT 256


SYSCALL_DEFINE1( my_syscall, char __user *, ubuf )
{
	long err;

	char kbuf[MAX_BUFFER_LIMIT];
    char tmp;
    int len;
    int i;

    access_ok( VERIFY_WRITE, ubuf, MAX_BUFFER_LIMIT );

    len = strlen_user(ubuf) - 1;
    if( MAX_BUFFER_LIMIT < len )
    {
        return( -ENOMEM );
    }

    err = strncpy_from_user(kbuf,ubuf,len);

    for (i=0; i<len/2; ++i) {
        tmp = kbuf[i];
        kbuf[i] = kbuf[len-1-i];
        kbuf[len-1-i] = tmp;
    printk( "<MY_SYSCALL><LOOP>%d,%d, %s,%c\n", i,len-1-i,kbuf,kbuf[i] );
    }
    printk( "<MY_SYSCALL>%d, %s\n", len,kbuf );

    err = copy_to_user( ubuf, kbuf, len );
    	
    return( err );
}
