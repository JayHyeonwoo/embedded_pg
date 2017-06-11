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
#include <fcntl.h>

#include "comm.h"
#include "devinfo.h"

char self = 3;

struct packet make_packet(u_int8_t action, u_int8_t index,
		u_int32_t time, const char *name) {

	struct packet packet;

	packet.action = action;
	packet.index = index;
	packet.time = time;
	strcpy(packet.name, name);

	return packet;
}

void broadcast_hello_packet(void)
{
	struct packet packet = make_packet(HELLO_PACKET, self, 0, "");
	struct devinfo devinfos[5];
	long ndev = getdevinfo(5, devinfos), i;
	int sock;

	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
	    return;
	}

	for(i = 0; i < ndev; i++) {

		struct sockaddr_in client_addr;
		memset(&client_addr, 0, sizeof(client_addr));
		client_addr.sin_family = AF_INET;
		client_addr.sin_port = htons(RX_PORT);
		client_addr.sin_addr.s_addr= inet_addr(devinfos[i].ip);

		sendto(sock, &packet, sizeof(packet) + 1, 0,
				(struct sockaddr *) &client_addr, sizeof(client_addr));
	}

	close(sock);
}

void braodcast_dead_packet(void) {
	struct packet packet = make_packet(DEAD_PACKET, self, 0, "");
	struct devinfo devinfos[5];
	long ndev = getdevinfo(5, devinfos), i = 0;
	int sock;

	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
	    return;
	}

	for(i = 0; i < ndev; i++) {
		struct sockaddr_in client_addr;
		memset(&client_addr, 0, sizeof(client_addr));
		client_addr.sin_family = AF_INET;
		client_addr.sin_port = htons(RX_PORT);
		client_addr.sin_addr.s_addr= inet_addr(devinfos[i].ip);

		sendto(sock, &packet, sizeof(packet) + 1, 0,
				(struct sockaddr *) &client_addr, sizeof(client_addr));
	}

	close(sock);
}
