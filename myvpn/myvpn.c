#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/if.h>
#include <linux/if_tun.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <pthread.h>
#include <arpa/inet.h>

// normal ethernet frame 1500bytes - Smallest IP header 20bytes = 1480
#define PKTSIZ 1480
struct options_s { int fd; };
struct sockaddr_in vpn_addr;
int connect_port;

/*
 * MyVPN, written by Keiya CHINEN <s1011420@coins.tsukuba.ac.jp>
 * USE FOR >>TESTING<< PURPOSES ONLY
 * USING THIS PROGRAM ON THE "REAL INTERNET" COULD BE DANGEROUS!!
 *                                                    ^^^^^^^^^
 * cited: ISBN 978-4-87311-501-6
 */

void parse_args (int argc, char *argv[])
{
    int command;
    while((command = getopt(argc, argv, "h:p:")) != -1){
        switch(command){
            case 'a':
                vpn_addr.sin_addr.s_addr = inet_addr(optarg);
                break;
            case 'p':
                vpn_addr.sin_port = htons(atoi(optarg));
                break;
            default:
                ;
        }
    }

}


int tun_open(void)
{
    struct ifreq ifr;
    int fd;
    char dev[IFNAMSIZ];
    char buf[512];

    fd = open("/dev/net/tun", O_RDWR);
    
    memset(&ifr,0,sizeof(ifr));
    ifr.ifr_flags = IFF_TUN | IFF_NO_PI;
    strncpy(ifr.ifr_name,"tun%d",IFNAMSIZ);
    ioctl(fd,TUNSETIFF,&ifr);
    
    strncpy(dev,ifr.ifr_name,IFNAMSIZ);
    
    sprintf(buf,"ifconfig %s 192.168.1.1 pointopoint 192.168.1.2",dev);
    system(buf);
    
    return fd;
}

void dump_pkt(unsigned char *pkt, int len)
{
    int i;

    for (i = 0; i < len; i++)
    {
        printf("%02x ",pkt[i]);
    }
    printf("\n");
}

// TUN監視、パケット着信でVPNに流す
void* tunlisten(void *args)
{
    struct options_s *options = args;
    int fd = options->fd;
    int sock;

    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }
    vpn_addr.sin_family = AF_INET;

    while (1) {
        fd_set fds;
        int len;
        unsigned char pkt[PKTSIZ];

        FD_ZERO(&fds);
        FD_SET(fd,&fds);

        select(fd + 1, &fds, NULL, NULL, NULL);
        if (FD_ISSET(fd,&fds)) {
            len = read(fd,pkt,PKTSIZ);
            if (len <= 0) return;
            dump_pkt(pkt,len);
            printf("%d\n",len);

            // encapsulate a packet and send to VPN server
            sendto(sock, pkt, len, 0, (struct sockaddr *)&vpn_addr, sizeof(vpn_addr));
        }
    }
}

// VPN監視、パケット着信でTUNに流す
void* vpnlisten(void *args)
{
    struct options_s *options = args;
    int fd = options->fd;
    int sock;
    unsigned char buf[PKTSIZ];

    //buf = calloc(PKTSZ, sizeof(unsigned char));

    struct in_addr ip_src;
    struct in_addr ip_dst;

    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
      perror("socket");
      exit(1);
    }
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(12345);
    addr.sin_addr.s_addr = INADDR_ANY;

    bind(sock, (struct sockaddr *)&addr, sizeof(addr));
    size_t len;
    while(1) {
        int byte = recv(sock, &buf, PKTSIZ, 0);
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

        //len = sendto(tunsock, &buf, byte, 0,
        //(struct sockaddr *)&ip_dst, sizeof(struct sockaddr_in));
        len = write(fd,&buf,byte);
        if (len < 0) {
          perror("sendto");
          exit(1);
        }
        printf("\n");
    }
}

int main(int argc, char **argv)
{
    parse_args(argc, argv);

    struct options_s options;
    options.fd = tun_open();


    pthread_t thread_vpn;
    pthread_t thread_tun;

    if (pthread_create(&thread_vpn,NULL,vpnlisten,(void *)&options) != 0) {
      fprintf(stderr,"vpn pthread err.\n");
      exit (EXIT_FAILURE);
    }

    if (pthread_create(&thread_tun,NULL,tunlisten,(void *)&options) != 0) {
      fprintf(stderr,"tun pthread err.\n");
      exit (EXIT_FAILURE);
    }

    pthread_join(thread_vpn ,NULL);
    pthread_join(thread_tun ,NULL);

    return 0;
}
