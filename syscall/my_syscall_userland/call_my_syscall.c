#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>

#define SYS_my_syscall 546 


int main( int argc, char *argv[ ] )
{
        char buf[] = "The quick brown fox jumps over the lazy dog";
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

