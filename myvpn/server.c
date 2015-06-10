#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>

int main() {
    int sock;
    struct sockaddr_in addr;

    unsigned char *buf;
    //char buf[10];
int bufsz=512;
    buf = calloc(bufsz, sizeof(unsigned char));
    if (!buf) {
        perror("calloc");
        return -1;
    }
    
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    
    addr.sin_family = AF_INET;
    addr.sin_port = htons(12345);
    addr.sin_addr.s_addr = INADDR_ANY;
    
    bind(sock, (struct sockaddr *)&addr, sizeof(addr));
    
    while (1) {
    //  memset(buf, 0, sizeof(buf));
        int byte = recv(sock, buf, bufsz, 0);
        int i = 0;
        printf("%d bytes received:\n",byte);
        for (i=0;i<byte;i++) {
            printf("%02x ",buf[i]);
        }
        printf("\n");
        
    }
    
    close(sock);
    
    return 0;
}
