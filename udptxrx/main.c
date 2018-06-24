#include <stdio.h>
#include "net/af.h"
#include "net/protnum.h"
#include "net/ipv6/addr.h"
#include "net/sock/udp.h"
#include "xtimer.h"

/* Buffer for sent received UDP data */
uint8_t buf[128];

/* Main thread */
int main(void)
{
    sock_udp_ep_t local = SOCK_IPV6_EP_ANY;
    sock_udp_t sock;
    int msg_count = 0;

    /* Create a socket for the given port */    
    local.port = 12345;
    if (sock_udp_create(&sock, &local, NULL, 0) < 0) {
        puts("Error creating UDP sock");
        return 1;
    }
    
    while (1) {
        sock_udp_ep_t remote = { .family = AF_INET6 };
        ssize_t res;
        
        /* Create a multicase remote address for the given port */
        remote.port = 12345;
        ipv6_addr_set_all_nodes_multicast((ipv6_addr_t *)&remote.addr.ipv6,
                                          IPV6_ADDR_MCAST_SCP_LINK_LOCAL);
                                          
        /* Generate and send the next UDP message */
        sprintf((char*)buf, "Hello! - %d", msg_count++);
        if (sock_udp_send(&sock, buf, (strlen((char*)buf) + 1), &remote) < 0) {
            puts("Error sending message");
            sock_udp_close(&sock);
            return 1;
        }
        printf("Sent message: \"%s\"\n", (char*)buf);
        
        /* Wait for the UDP reply */
        if ((res = sock_udp_recv(&sock, buf, sizeof(buf), 1 * US_PER_SEC,
                                NULL)) < 0) {
            if (res == -ETIMEDOUT) {
                puts("Timed out");
            }
            else {
                puts("Error receiving message");
            }
        }
        else {
            printf("Received message: \"");
            for (int i = 0; i < res; i++) {
                printf("%c", buf[i]);
            }
            printf("\"\n");
        }
        
        /* Wait until the next send7receive cycle */
        xtimer_sleep(3);
    }
    
    return 0;
}
