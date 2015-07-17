#include <stdio.h>
#include <string.h>
#include <openssl/sha.h>

#define MAX_PASSPHRASE 32

int i=0;
inline void decrypt(char* dst, char* src, unsigned int size, char* key)
{
    for (i=0; i<size; ++i) {
        dst[i] = src[i] ^ key[i%(SHA512_DIGEST_LENGTH/sizeof(char))];
    }
}
inline void encrypt(char* dst, char* src, unsigned int size, char* key)
{
    for (i=0; i<size; ++i) {
        dst[i] = src[i] ^ key[i%(SHA512_DIGEST_LENGTH/sizeof(char))];
    }
}
void genkey(char* key, char* passphrase)
{
    SHA512(passphrase, strlen(passphrase), key);
}
#ifndef HEXDUMP_COLS
#define HEXDUMP_COLS 8
#endif
 
void hexdump(void *mem, unsigned int len)
{
        unsigned int i, j;
        
        for(i = 0; i < len + ((len % HEXDUMP_COLS) ? (HEXDUMP_COLS - len % HEXDUMP_COLS) : 0); i++)
        {
                /* print offset */
                if(i % HEXDUMP_COLS == 0)
                {
                        printf("0x%06x: ", i);
                }
 
                /* print hex data */
                if(i < len)
                {
                        printf("%02x ", 0xFF & ((char*)mem)[i]);
                }
                else /* end of block, just aligning for ASCII dump */
                {
                        printf("   ");
                }
                
                /* print ASCII dump */
                if(i % HEXDUMP_COLS == (HEXDUMP_COLS - 1))
                {
                        for(j = i - (HEXDUMP_COLS - 1); j <= i; j++)
                        {
                                if(j >= len) /* end of block, not really printing */
                                {
                                        putchar(' ');
                                }
                                else if(isprint(((char*)mem)[j])) /* printable char */
                                {
                                        putchar(0xFF & ((char*)mem)[j]);        
                                }
                                else /* other char */
                                {
                                        putchar('.');
                                }
                        }
                        putchar('\n');
                }
        }
}
 
#ifdef HEXDUMP_TEST
int main(int argc, char *argv[])
{
        hexdump(argv[0], 20);
 
        return 0;
}
#endif

#ifdef BOOTSTRAP_MAIN
int main (void)
{

char passphrase[MAX_PASSPHRASE] = "data to hash";
char key[SHA512_DIGEST_LENGTH];
size_t datalen;

char data[] = "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789";
datalen = sizeof(data);


char encrypted[1500];
char decrypted[1500];

genkey(key,passphrase);
hexdump(key,64);
encrypt(encrypted,data,datalen,key);

printf("\n");

//strncpy(passphrase,"data to hashe",32);
//genkey(key,passphrase);
hexdump(key,64);


decrypt(decrypted,encrypted,datalen,key);
hexdump(decrypted,datalen);

    //dst(&decrypted,&encrypted,1500,&key);


}
#endif
