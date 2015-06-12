#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>

int main() {
    int vpnsock,tunsock;
    struct sockaddr_in addr;

    unsigned char *buf;
    //char buf[10];
    int bufsz=512;
    buf = calloc(bufsz, sizeof(unsigned char));
    if (!buf) {
        perror("calloc");
        return -1;
    }
    
  int on = 1;
    //vpnsock = socket(AF_INET, SOCK_DGRAM, 0);
    if ((vpnsock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
      perror("socket");
      exit(1);
    }
    if ((tunsock = socket(PF_INET, SOCK_RAW, IPPROTO_RAW)) < 0) {
      perror("socket");
      exit(1);
    }
    if (setsockopt(tunsock, IPPROTO_IP, IP_HDRINCL, &on, sizeof(int)) < 0) {
      perror("setsockopt");
      exit(1);
    }


    
    addr.sin_family = AF_INET;
    addr.sin_port = htons(12345);
    addr.sin_addr.s_addr = INADDR_ANY;
    
    bind(vpnsock, (struct sockaddr *)&addr, sizeof(addr));
    
    size_t len;
    struct in_addr ip_src;
    struct in_addr ip_dst;
    while (1) {
        int byte = recv(vpnsock, buf, bufsz, 0);
        int i = 0;
        printf("%d bytes received:\n",byte);
        for (i=0;i<byte;i++) {
            if (i % 4 == 0) printf("\n");
            printf("%02x ",buf[i]);
        }
        printf("\n");
        memcpy(&ip_src.s_addr,&buf[12],sizeof(struct in_addr));
        memcpy(&ip_dst.s_addr,&buf[16],sizeof(struct in_addr));

        printf("%x ",ntohl(ip_dst.s_addr));

        len = sendto(tunsock, &buf, byte, 0,
        (struct sockaddr *)&ip_dst, sizeof(struct sockaddr_in));
        if (len < 0) {
          perror("sendto");
          exit(1);
        }

        printf("\n");
    }
    
    close(vpnsock);
    
    return 0;
}


