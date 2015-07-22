#include <linux/module.h>
#include <linux/kthread.h>
#include <linux/netdevice.h>
#include <linux/ip.h>
#include <linux/in.h>

MODULE_DESCRIPTION("periodic_task");
MODULE_LICENSE("GPL");

#define CONNECT_PORT ((unsigned short)12345)
#define SEND_ADDR ((unsigned long int)0xAC100501) //172.16.5.1

struct socket *sock_send;
struct sockaddr_in addr_send;

struct task_struct *th;

// ksocket_send
// http://kernelnewbies.org/Simple_UDP_Server
/*
  Rodrigo Rubira Branco <rodrigo@kernelhacking.com>
  Toni Garcia-Navarro <topi@phreaker.net>
*/
// licensed under Creative-commons
int ksocket_send(struct socket *sock, struct sockaddr_in *addr, unsigned char *buf, int len)
{
	struct msghdr msg;
        struct iovec iov;
        mm_segment_t oldfs;
        int size = 0;

        if (sock->sk==NULL)
           return 0;

        iov.iov_base = buf;
        iov.iov_len = len;

        msg.msg_flags = 0;
        msg.msg_name = addr;
        msg.msg_namelen  = sizeof(struct sockaddr_in);
        msg.msg_control = NULL;
        msg.msg_controllen = 0;
        //msg.msg_iov = (struct iovec*)&iov;
        //msg.msg_iovlen = 1;
        msg.msg_control = NULL;

	iov_iter_init(&msg.msg_iter, WRITE, &iov, 1,len );

        oldfs = get_fs();
        set_fs(KERNEL_DS);

        size = sock_sendmsg(sock,&msg,len);
	printk("wrote %d bytes\n",size);

        set_fs(oldfs);

        return size;
}


static int perio_thr(void * num)
{
	int err;
        /* create a socket */
        if (( (err = sock_create(AF_INET, SOCK_DGRAM, IPPROTO_UDP, &sock_send)) < 0 ))
        {
                printk(": Could not create a datagram socket, error = %d\n", -ENXIO);
                goto out;
        }
	addr_send.sin_family = AF_INET;
	addr_send.sin_addr.s_addr = htonl(SEND_ADDR);
	addr_send.sin_port = htons(CONNECT_PORT);
/*
        if ((err = sock_send->ops->connect(sock_send, (struct sockaddr *)&addr_send, sizeof(struct sockaddr), 0) < 0 ))
        {
                printk(": Could not bind or connect to socket, error = %d\n", -err);
                goto close_and_out;
        }
*/
	while(!kthread_should_stop()){
		ksocket_send(sock_send, &addr_send,"Heartbeat" ,9);
		current->state = TASK_INTERRUPTIBLE;
		schedule_timeout(1000);
	}
close_and_out:
        sock_release(sock_send);
	sock_send = NULL;
	return 0;
out:
	return -1;
}



static int __init modperiodic_init(void)
{
	printk("module_init\n");
	th = kthread_run(perio_thr, NULL, "perio_thr");
	if (IS_ERR(th)) {
		printk("kthread_run error\n");
		return -EBUSY;
	}
	return 0;
}

static void __exit modperiodic_exit(void)
{
	printk("module_exit\n");
	kthread_stop(th);
}

module_init(modperiodic_init);
module_exit(modperiodic_exit);
