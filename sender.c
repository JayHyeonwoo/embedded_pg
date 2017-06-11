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
#include <time.h>

#include "comm.h"

static int self = 3;

void *thr_sender(void *arg)
{
    struct sender_arg *sender_arg = arg;
    struct sockaddr_in client_addr;
    time_t send_time;
    int sock;

    printf("thr_sender!\n");
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (-1 == sock)
    {
	perror("socket error");
	pthread_exit(NULL);
    }

    memset(&client_addr, 0, sizeof(client_addr));
    client_addr.sin_family = AF_INET;
    client_addr.sin_port = htons(RX_PORT);
    client_addr.sin_addr.s_addr= inet_addr(sender_arg->ip);

    send_time = 0;
    while (!sender_arg->quitflag) {
	/* time check */
	if (send_time + sender_arg->delay_time > time(NULL)) {
	    continue;
	}

	send_time = time(NULL);

	/* make packet */
	struct packet packet;
	packet.action = DATA_PACKET;
	packet.index = self;
	packet.time = htonl(30);
	strcpy(packet.name, "subject");

	/* send packet */
	sendto(sock, &packet, sizeof(packet) + 1, 0,
		(struct sockaddr *) &client_addr, sizeof(client_addr));
    }

    pthread_exit(NULL);
}
