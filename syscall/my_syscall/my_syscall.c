#include <linux/kernel.h>
#include <linux/syscalls.h>

#define MAX_BUFFER_LIMIT 256


SYSCALL_DEFINE2( my_syscall, char __user *, ubuf, int, count )
{
	long err;

	char kbuf[MAX_BUFFER_LIMIT];

    access_ok( VERIFY_WRITE, ubuf, MAX_BUFFER_LIMIT );
	copy_from_user( kbuf, ubuf, MAX_BUFFER_LIMIT  );

    char tmp;
    int len = strlen_user(kbuf/*,MAX_BUFFER_LIMIT*/);
    for (int i=0; i<len/2; ++i) {
        tmp = kbuf[i];
        kbuf[i] = kbuf[len-1-i];
        kbuf[len-1-i] = tmp;
    }

    printk( "<MY_SYSCALL>%s\n", kbuf );

    if( count < sizeof( text ) )
    {
        return( -ENOMEM );
    }

    err = copy_to_user( ubuf, kbuf, MAX_BUFFER_LIMIT );
    	
    return( err );
}
