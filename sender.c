#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <stdbool.h>

#include "comm.h"

int main(void)
{
    int sock;
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (-1 == sock)
    {
	printf( "socket 생성 실패");
	exit( 1) ;
    }

    broadcast_hello_packet(sock);

    getchar();

    braodcast_dead_packet(sock);

#if 0
    struct sockaddr_in client_addr;
    memset(&client_addr, 0, sizeof(client_addr));
    client_addr.sin_family = AF_INET;
    client_addr.sin_port = htons(RX_PORT);
    client_addr.sin_addr.s_addr= inet_addr("127.0.0.1");

    // time check
    struct packet packet;
    packet.action = '1';
    packet.index = '2';
    packet.time = 30; // little endian -> big endian
    strcpy(packet.name, "subject");

    //    while(1) {
    // send
    sendto(sock, &packet, sizeof(packet) + 1, 0,
	    (struct sockaddr *) &client_addr, sizeof(client_addr));

    //    }
#endif
    return 0;
}
