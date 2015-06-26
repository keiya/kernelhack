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
#include "aes_ctr.h"

#define PKTSIZ 1500
#define DEBUG 1
struct options_s { int fd;int sock; };
struct sockaddr_in vpn_addr;

int server_mode = 0;
int server_bind_port;
struct sockaddr_in senderinfo;

unsigned char key[32];
/*
 * MyVPN, written by Keiya CHINEN <s1011420@coins.tsukuba.ac.jp>
 * USE FOR >>TESTING<< PURPOSES ONLY
 * USING THIS PROGRAM ON THE "REAL INTERNET" COULD BE DANGEROUS!!
 *                                                    ^^^^^^^^^
 * cited: ISBN 978-4-87311-501-6
 */

void parse_args (int argc, char *argv[], char *ifconfig)
{
    int command;
    while((command = getopt(argc, argv, "a:p:s:t:k:")) != -1){
        switch(command){
            case 'a':
                // [client] VPN peer to connect
                vpn_addr.sin_addr.s_addr = inet_addr(optarg);
                break;
            case 's':
                // [server] server mode (specify bind port)
                server_mode = 1;
                server_bind_port = atoi(optarg);
                break;
            case 'p':
                // [client] server's port
                vpn_addr.sin_port = htons(atoi(optarg));
                break;
            case 't':
                // [client&server] configuration (ifconfig) of local tun device
                strncpy(ifconfig,optarg,255);
                break;
            case 'k':
                // specify key
                strncpy(key,optarg,32);
                break;
            default:
                ;
        }
    }

}


int tun_open(char *ifconfig)
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
    
    sprintf(buf,ifconfig,dev);
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
    int sock = options->sock;

    vpn_addr.sin_family = AF_INET;

        int len;
        unsigned char pkt[PKTSIZ];
    while (1) {
        fd_set fds;

        FD_ZERO(&fds);
        FD_SET(fd,&fds);


        if (server_mode) {
            vpn_addr.sin_port = senderinfo.sin_port;
            vpn_addr.sin_addr.s_addr = senderinfo.sin_addr.s_addr;
        }

  unsigned char encrypted[PKTSIZ];
        //select(fd + 1, &fds, NULL, NULL, NULL);
        if (FD_ISSET(fd,&fds)) {
            len = read(fd,pkt,PKTSIZ);
            // encapsulate a packet and send to VPN server
  int encrypted_len = encrypt (pkt, len,aad ,strlen(aad), key, iv,
                            encrypted, tag);
#ifdef DEBUG
  printf("%32s %16s\n",key,iv);
  printf("[TUN>VPN] %d->%d\n",len,encrypted_len);
  evp_dump(&encrypted,encrypted_len);
  unsigned char decrypted[PKTSIZ];
  int decrypted_len = decrypt(&encrypted, encrypted_len, aad, strlen(aad), tag, key, iv,
    &decrypted);
  evp_dump(&decrypted,decrypted_len);
#endif
            //sendto(sock, pkt, len, 0, (struct sockaddr *)&vpn_addr, sizeof(vpn_addr));
            sendto(sock, encrypted, encrypted_len, 0, (struct sockaddr *)&vpn_addr, sizeof(vpn_addr));
        }
    }
}

// VPN監視、パケット着信でTUNに流す
void* vpnlisten(void *args)
{
    struct options_s *options = args;
    int fd = options->fd;
    int sock = options->sock;
    unsigned char buf[PKTSIZ];

    struct in_addr ip_src;
    struct in_addr ip_dst;

    size_t len;

    socklen_t addrlen;
        addrlen = sizeof(senderinfo);

        int byte;
int decrypted_len;
  unsigned char decrypted[PKTSIZ];
    while(1) {

        byte = recvfrom(sock, buf, sizeof(buf) - 1, 0,
        (struct sockaddr *)&senderinfo, &addrlen);


  decrypted_len = decrypt(buf, byte, aad, strlen(aad), tag, key, iv,
    decrypted);
#ifdef DEBUG
  printf("%32s %16s\n",key,iv);
  printf("[VPN>TUN] %d->%d\n",byte,decrypted_len);
  evp_dump(&buf,byte);
  evp_dump(&decrypted,decrypted_len);
#endif

        //write(fd,&buf,byte);
        if (decrypted_len < 0 )
          printf("decrypt failed?\n");
        write(fd,decrypted,decrypted_len);
    }
}

int main(int argc, char **argv)
{
    char ifconfig[256];
    parse_args(argc, argv,&ifconfig);

    struct options_s options;
    options.fd = tun_open(ifconfig);

    if ((options.sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
      perror("socket");
      exit(1);
    }
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    if (server_mode)
        addr.sin_port = htons(server_bind_port);
    else
        addr.sin_port = 0;
    addr.sin_addr.s_addr = INADDR_ANY;

    bind(options.sock, (struct sockaddr *)&addr, sizeof(addr));

struct sockaddr_in sin = {};
socklen_t slen;
int sock;
short unsigned int port;
slen = sizeof(sin);
getsockname(options.sock, (struct sockaddr *)&sin, &slen);
port = ntohs(sin.sin_port);
printf("client src = %d\n",port);

        if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }


      pthread_t thread_vpn;
    pthread_t thread_tun;


    // is it THREAD SAFE??????????????///
  ERR_load_crypto_strings();
  OpenSSL_add_all_algorithms();
  OPENSSL_config(NULL);



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
