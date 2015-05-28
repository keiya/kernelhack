#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>

#define SYS_my_syscall 546

int main( int argc, char *argv[ ] )
{
        char buf[] = "The quick brown fox jumps over the lazy dog";
        
        // should be failed with "exeed MAX_BUFFER_LIMIT" error
        char buf2[] = "The quick brown fox jumps over the lazy dog.The quick brown fox jumps over the lazy dog.\
                      The quick brown fox jumps over the lazy dog.The quick brown fox jumps over the lazy dog.\
                      The quick brown fox jumps over the lazy dog.The quick brown fox jumps over the lazy dog.\
                      The quick brown fox jumps over the lazy dog.The quick brown fox jumps over the lazy dog.\
                      The quick brown fox jumps over the lazy dog.The quick brown fox jumps over the lazy dog.\
                      The quick brown fox jumps over the lazy dog.The quick brown fox jumps over the lazy dog.\
                      The quick brown fox jumps over the lazy dog.The quick brown fox jumps over the lazy dog.\
                      The quick brown fox jumps over the lazy dog.The quick brown fox jumps over the lazy dog.";

        char buf3[] = "";

        long    ret;

        // these cases should be failed
        if (syscall( SYS_my_syscall, NULL, sizeof( buf )) >= 0)
            printf("test case 1: assertion failed\n");
        if (syscall( SYS_my_syscall, buf2, sizeof( buf2 )) >= 0)
            printf("test case 2: assertion failed\n");
        if (syscall( SYS_my_syscall, buf3, 200) >= 0)
            printf("test case 3: assertion failed\n");
        if (syscall( SYS_my_syscall, 0xffffffffffffffff, 200) >= 0)
            printf("test case 4: assertion failed\n");

        // this case will success
        if ((ret = syscall( SYS_my_syscall, buf, sizeof( buf ))) < 0)
            printf("test case 5: assertion failed\n");

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

