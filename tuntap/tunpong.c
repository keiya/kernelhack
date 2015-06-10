#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/if.h>
#include <linux/if_tun.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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

void pingpong(int fd)
{
    fd_set fds;
    int len;
    unsigned char pkt[512];

    FD_ZERO(&fds);
    FD_SET(fd,&fds);

    select(fd + 1, &fds, NULL, NULL, NULL);
    if (FD_ISSET(fd,&fds)) {
        len = read(fd,pkt,512);
        dump_pkt(pkt,len);

        if (pkt[20] != 0x08)
          return;

        pkt[15] = 0x02;
        pkt[19] = 0x01;
        pkt[20] = 0x00;
        write(fd,pkt,len);
    }
}

int main(int argc, char **argv)
{
    int fd;

    fd = tun_open();
    for(;;)
      pingpong(fd);

    return 0;
}
