#ifndef _COMM_H
#define _COMM_H

#include "devinfo.h"

#define RX_PORT		9007
#define TX_PORT		9006

enum {
	HELLO_PACKET = '1',
	DEAD_PACKET = '2',
	DATA_PACKET = '3'
};

struct sender_arg
{
	int sockfd;
	char ip[IP_NAME_MAX];
};

struct packet {

    u_int8_t action;
    u_int8_t index;
    u_int32_t time;
    char name[19];

};

extern void *thr_recognizer(void *);

extern void *thr_sender(void *);

extern void *thr_receiver(void *);

extern void update_netif_status(int, int);

extern int get_netif_status(int);


#endif
