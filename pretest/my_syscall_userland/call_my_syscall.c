#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>

#define SYS_my_syscall 546 

/* http://softwaretechnique.jp/Linux/SystemCall/sc_03.html */

int main( int argc, char *argv[ ] )
{
        char    buf[ 256 ];
        long    ret;

        ret = syscall( SYS_my_syscall, buf, sizeof( buf ) );

        if( ret < 0 )
        {
                fprintf( stderr, "erro : %ld\n", ret );
        }
        else
        {
                printf( "buf = %s\n", buf );
        }

        return( 0 );
}

