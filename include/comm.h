#ifndef _COMM_H
#define _COMM_H

#include <sys/types.h>
#include <stdint.h>
#include <syslog.h>

/* user defined header files */
#include <devinfo.h>
#include <schedule.h>

#define RX_PORT		9007

struct sender_arg
{
	int sockfd;
	char ip[IP_NAME_MAX];
	int quitflag;
	time_t delay_time;
};

struct packet {
    u_int8_t index;
    u_int32_t time;
    char name[SUBJECT_NAME_MAX];

};

struct information {
    unsigned int index;
    time_t time;
    time_t rx_time;
    char subject_name[SUBJECT_NAME_MAX];
};

extern int comm_init(void);

/* defined in recognizer.c */
extern void *thr_recognizer(void *);

extern void set_delay_time(time_t);

/* defined in sender.c */
extern void *thr_sender(void *);

/* defined in receiver.c */
extern void *thr_receiver(void *);

/* defined in comm.c */
extern void unpack_packet(const struct packet *, struct information *);

extern void pack_information(const struct information *, struct packet *);

extern void update_infolist(const struct information *);

extern void get_other_info(struct information *);

extern unsigned int self_index(void);

extern void self_info(struct information *);

#define func_syslog(PRIO, STR, ...) \
	syslog((PRIO), "%s: "STR, __func__, __VA_ARGS__)

#endif
