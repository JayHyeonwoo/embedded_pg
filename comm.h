#ifndef _COMM_H
#define _COMM_H

#include <stdint.h>
#include <sys/types.h>
#include "devinfo.h"

#define RX_PORT		9007
#define TX_PORT		9006

enum
{
	HELLO_PACKET,
	DEAD_PACKET,
	DATA_PACKET
};

struct sender_arg
{
	int sockfd;
	char ip[IP_NAME_MAX];
	int quitflag;
	time_t delay_time;
};

struct packet {
    u_int8_t action;
    u_int8_t index;
    u_int32_t time;
    char name[19];

};

struct information {
    char index;
    int time;
    char subject_name[19];
};

/* global variables in receive.c */
extern pthread_mutex_t information_list_lock;

extern struct information information_list[256 - 2]; 

extern void *thr_recognizer(void *);

extern void *thr_sender(void *);

extern void *thr_receiver(void *);

extern void update_netif_status(int, int);

extern int get_netif_status(int);

extern void broadcast_hello_packet(void);

extern void braodcast_dead_packet(void);

struct packet make_packet(u_int8_t, u_int8_t, u_int32_t,
		const char *);

#endif
