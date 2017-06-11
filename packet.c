#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>

#include "comm.h"

char* reference[5] = {"","192.168.2.1", "192.168.2.2"};
char self = '3';

struct packet make_packet(u_int8_t action, u_int8_t index,
		u_int32_t time, const char *name) {

	struct packet packet;

	packet.action = action;
	packet.index = index;
	packet.time = time;
	strcpy(packet.name, name);

	return packet;
}

void broadcast_hello_packet(int sock)
{
	//invoke system call to update ip address
	struct packet packet = make_packet('1', self, 0, "");

	int i = 0;

	struct sockaddr_in client_addr;
	memset(&client_addr, 0, sizeof(client_addr));
	client_addr.sin_family = AF_INET;
	client_addr.sin_port = htons(RX_PORT);
	client_addr.sin_addr.s_addr= inet_addr("127.0.0.1");


	sendto(sock, &packet, sizeof(packet) + 1, 0,
			(struct sockaddr *) &client_addr, sizeof(client_addr));
}

void braodcast_dead_packet(int sock) {

	//invoke system call to update ip address
	struct packet packet = make_packet('2', self, 0, "");

	int i = 0;

	struct sockaddr_in client_addr;
	memset(&client_addr, 0, sizeof(client_addr));
	client_addr.sin_family = AF_INET;
	client_addr.sin_port = htons(RX_PORT);
	client_addr.sin_addr.s_addr= inet_addr("127.0.0.1");


	sendto(sock, &packet, sizeof(packet) + 1, 0,
			(struct sockaddr *) &client_addr,
			sizeof(client_addr));
}
