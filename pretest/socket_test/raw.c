#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
int
main(void)
{
  //struct in_addr ip_src = { .s_addr = htonl(0x0a000001) }; /* 10.0.0.1 */
  struct in_addr ip_src = { .s_addr = htonl(0x829e5701) }; /* 10.0.0.1 */
  //struct in_addr ip_dst = { .s_addr = htonl() }; /* 10.0.0.2 */
  struct in_addr ip_dst = { .s_addr = htonl(0x829e5701) }; /* 10.0.0.1 */
  struct sockaddr_in sa_dst;
  struct ip ip;
  int fd;
  int on = 1;
  size_t len;
  if ((fd = socket(PF_INET, SOCK_RAW, IPPROTO_RAW)) < 0) {
    perror("socket");
    exit(1);
  }
  if (setsockopt(fd, IPPROTO_IP, IP_HDRINCL, &on, sizeof(int)) < 0) {
    perror("setsockopt");
    exit(1);
  }
  memset(&ip, 0, sizeof(struct ip));
  ip.ip_v = IPVERSION;
  ip.ip_hl = sizeof(struct ip) >> 2;
  ip.ip_tos = 0;
  ip.ip_len = sizeof(struct ip);
  ip.ip_id = 0;
  ip.ip_off = IP_DF|0;
  ip.ip_ttl = 255;
  //ip.ip_p = IPPROTO_RAW;
  ip.ip_p = 1;
  ip.ip_sum = 0;
  memcpy(&ip.ip_src, &ip_src, sizeof(struct in_addr));
  memcpy(&ip.ip_dst, &ip_dst, sizeof(struct in_addr));
  sa_dst.sin_family = AF_INET;
  //sa_dst.sin_len = sizeof(struct sockaddr_in);
  sa_dst.sin_port = htons(0);
  memcpy(&sa_dst.sin_addr, &ip_dst, sizeof(struct sockaddr_in));
  len = sendto(fd, &ip, sizeof(struct ip), 0,
  (struct sockaddr *)&sa_dst, sizeof(struct sockaddr_in));
  if (len < 0) {
    perror("sendto");
    exit(1);
  }
  return 0;
}
