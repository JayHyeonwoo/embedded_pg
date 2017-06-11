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

int arr[999] = {};

char* reference[5] = {"192.168.2.1", "192.168.2.2"};
int node_count = 2;

struct information {
    char index;
    int time;
    char subjuct_name[19];

};

struct information list[256 - 2] = {};
bool index_management[256 - 2] = {};

struct packet make_packet(u_int8_t action, u_int8_t index,
		u_int32_t time, const char *name) {

    struct packet packet;

    packet.action = action;
    packet.index = index;
    packet.time = time;
    strcpy(packet.name, name);

    return packet;

}

void *thr_receiver(void *arg)
{
    printf("thr_receive start\n");

    int sock;
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
            case '1':
                // 헬로패킷이 수신되었을 때 index_management 배열을 수정하고
		// 이후 mutex를 통해서 제어 신호 전달
                printf("case 1\n");
		update_netif_status(index, 1);
                break;
            case '2':
                // 데드패킷이 수신되었을 때 루틴 처리
                // unlock
                printf("case 2\n");
		update_netif_status(index, 0);
                break;
            case '3':
                // update information
                printf("case 3\n");
                list[index].time = time;
                list[index].index = index;
                strcpy(list[index].subjuct_name, name);
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
