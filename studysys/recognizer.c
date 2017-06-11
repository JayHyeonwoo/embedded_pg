#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <pthread.h>

#include "comm.h"
#include "devinfo.h"

static int netif_status[DEVINFO_MAX];
static int netif_recent_idx;
static pthread_mutex_t netif_status_lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t netif_status_cond = PTHREAD_COND_INITIALIZER;

static struct sender_arg sender_args[DEVINFO_MAX];

void update_netif_status(int index, int value)
{
	pthread_mutex_lock(&netif_status_lock);
	netif_status[index] = value;
	netif_recent_idx = index;
	pthread_mutex_unlock(&netif_status_lock);
	pthread_cond_signal(&netif_status_cond);
}

int get_netif_status(int index)
{
	int ret;

	pthread_mutex_lock(&netif_status_lock);
	ret = netif_status[index];
	pthread_mutex_unlock(&netif_status_lock);

	return ret;
}

void *thr_recognizer(void *arg)
{
	(void)arg;

	pthread_t tid;
	int sockfd, err, status, id;

	printf("thr_recognizer!\n");
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("socket error");
		pthread_exit((void *)-1);
	}

	for (;;) {
		pthread_mutex_lock(&netif_status_lock);
		
		pthread_cond_wait(&netif_status_cond, &netif_status_lock);

		id = netif_recent_idx;
		printf("thr_recognizer unlock! id = %d\n", id);

		pthread_mutex_unlock(&netif_status_lock);

		status = get_netif_status(id);
		printf("status = %d\n", status);

		if (status) {
			/* set thread argument */
			sender_args[id].sockfd = sockfd;
			strcpy(sender_args[id].ip, index2ip(id));
			sender_args[id].quitflag = 0;
			sender_args[id].delay_time = 10;

			err = pthread_create(&tid,
					NULL,
					thr_sender,
					&sender_args[id]);
			if (err) {
				/* error handling */
			}

			err = pthread_detach(tid);
			if (err) {
			    /* error handling */
			}

		} else {
			/* kill thread */
			sender_args[id].quitflag = 1;
			printf("id = %d, flag %d\n", id, 
				sender_args[id].quitflag);
		}
	}

	pthread_exit(NULL);
}
