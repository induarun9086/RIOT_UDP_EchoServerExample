#include <stdio.h>
#include "net/sock/udp.h"

/* Structure to hold recieved UDP data and associated meta data required
   to echo the packet after a delay. */
struct udp_data
{
     uint8_t buffer[128];
     ssize_t size;
     sock_udp_t* sock;
     sock_udp_ep_t remote;
     xtimer_t* timer;
     msg_t* msg;
};

/* Buffer for the received UDP packet */
uint8_t buf[128];

/* Stack memory for the thread */
char timer_thread_stack[THREAD_STACKSIZE_MAIN];

/* Timer thread */
void *timer_thread(void *arg)
{
   struct udp_data* data;
   (void) arg;
 
    printf("Timer thread started, pid: %" PRIkernel_pid "\n", thread_getpid());
    msg_t m;
    xtimer_ticks32_t current_time;

    while (1) {
        ssize_t res;

        /* Wait until a message is recieved */
        msg_receive(&m);

        current_time = xtimer_now();
        data = (struct udp_data*) m.content.ptr; 
        printf("Timer: Got msg at %d ms and UDP msg \"%s\" of size %d sent\n",current_time.ticks32/1000, (char*)data->buffer, data->size);

        /* Send the UDP data recieved in the timer over UDP again */
        if ((res = sock_udp_send(data->sock, data->buffer, data->size, &data->remote)) < 0) {
             printf("Error sending reply %d\n",res);
        }

        /* Free the allocated strutcures */
        free(data->timer);
        free(data->msg);
        free(data);
    }

    return NULL;
}

/* Main thread */
int main(void)
{
    sock_udp_ep_t local = SOCK_IPV6_EP_ANY;
    sock_udp_t sock;
    struct udp_data* rcvd_udp_packet;
    xtimer_t* timer;
    msg_t* msg;

    /* Create a seperate thread to handle timer messages */
    kernel_pid_t pid = thread_create(timer_thread_stack, sizeof(timer_thread_stack),
                            THREAD_PRIORITY_MAIN - 1, THREAD_CREATE_STACKTEST,
                            timer_thread, NULL, "timer");

    /* Create a scoket at the given port */
    local.port = 12345;
    if (sock_udp_create(&sock, &local, NULL, 0) < 0) {
        puts("Error creating UDP sock");
        return 1;
    }

    while (1) {
        ssize_t res;
        sock_udp_ep_t remote;
        xtimer_ticks32_t current_time;

        /* Wait for an incoming UDP packet */
        if ((res = sock_udp_recv(&sock, buf, sizeof(buf), SOCK_NO_TIMEOUT, &remote)) >= 0) {
            current_time = xtimer_now();
            printf("Received a UDP message \"%s\" of size %d at %d ms\n",(char*)buf,res,current_time.ticks32/1000);

            /* Allocate the required structures for handle the current UDP packet */
            rcvd_udp_packet = (struct udp_data*)malloc(sizeof(struct udp_data));
            timer = (xtimer_t*)malloc(sizeof (xtimer_t));
            msg = (msg_t*)malloc(sizeof(msg_t));

            /* Initialize the data */
            memset(rcvd_udp_packet,0,sizeof(struct udp_data));
            memset(timer,0,sizeof(xtimer_t));
            memset(msg,0,sizeof(msg_t));

            /* Copy the UDP data and all other meta data to be sent to the timer */
            rcvd_udp_packet->size = res;
            memcpy(rcvd_udp_packet->buffer,buf,res);
            memcpy(&rcvd_udp_packet->remote,&remote,sizeof(sock_udp_ep_t));
            rcvd_udp_packet->sock = &sock;
            rcvd_udp_packet->timer= timer;
            rcvd_udp_packet->msg = msg;

            /* Copy the UDP data structure to the message */
	          msg->content.ptr = rcvd_udp_packet;

            /* Start the timer */
            xtimer_set_msg (timer,1 * US_PER_SEC,msg,pid);
        }
    }

    return 0;
}
