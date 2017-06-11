#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <stdbool.h>

#include "comm.h"

int node_count = 2;

pthread_mutex_t information_list_lock = PTHREAD_MUTEX_INITIALIZER;
struct information information_list[256 - 2] = {};

void *thr_receiver(void *arg)
{
    printf("thr_receive start\n");

    int sock, status;
    sock = socket(AF_INET, SOCK_DGRAM, 0);

    if (-1 == sock) {
	printf("socket create fail");
	exit(1);
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family      = AF_INET;
    server_addr.sin_port        = htons(RX_PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if( -1 == bind(sock, (struct sockaddr*)&server_addr, sizeof(server_addr) ) )
    {
	printf( "bind() exe error \n");
	exit( 1);
    }

    while(1) {

	struct sockaddr_in client_addr;
	int client_addr_size;
	struct packet packet;

	// recvfrom(sock, &packet, sizeof(packet), 0,
	//          (struct sockaddr *) &client_addr, sizeof(client_addr));

	recvfrom(sock, &packet, sizeof(packet), 0,
		NULL, 0);

	char action = packet.action;
	char index = packet.index;
	int time = packet.time;
	char name[19];
	strcpy(name, packet.name);
	printf("packet %c %c %d %s\n", action, index, time, name);


	switch(action) {
	    case HELLO_PACKET:
		// 헬로패킷이 수신되었을 때 index_management 배열을 수정하고
		// 이후 mutex를 통해서 제어 신호 전달
		printf("case 1\n");
		update_netif_status(index, 1);
		break;
	    case DEAD_PACKET:
		// 데드패킷이 수신되었을 때 루틴 처리
		// unlock
		printf("case 2\n");
		update_netif_status(index, 0);
		break;
	    case DATA_PACKET:
		// update information
		printf("case 3\n");

		status = get_netif_status(index);
		if (!status) {
		    update_netif_status(index, 1);
		}
		// update application
		pthread_mutex_lock(&information_list_lock);
		information_list[index].time = time;
		information_list[index].index = index;
		strcpy(information_list[index].subject_name, name);
		pthread_mutex_unlock(&information_list_lock);

		break;
	}
    }
}

void write_others(int index) {

    pthread_mutex_t  mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_lock(&mutex);

    //write buffer to arr[index]

    pthread_mutex_unlock(&mutex);
}
