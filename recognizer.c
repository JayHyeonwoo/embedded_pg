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

void update_netif_status(int index, int value)
{
	pthread_mutex_lock(&netif_status_lock);
	printf("update_netif_status!\n");
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

	struct sender_arg sender_args[DEVINFO_MAX];
	int sockfd, err, status, id;
	pthread_t tids[DEVINFO_MAX];

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
			sender_args[id - '1'].sockfd = sockfd;
			strcpy(sender_args[id].ip, index2ip(id));

			err = pthread_create(&tids[id - '1'],
					NULL,
					thr_sender,
					&sender_args[id - '1']);
			if (err) {
				/* error handling */
			}
		} else {
			/* kill thread */
			if (tids[id - '1'] == 0) {
				continue;
			}

			err = pthread_kill(tids[id - '1'], SIGQUIT);
			if (err) {
				/* error handling */
			}
		}
	}

	pthread_exit(NULL);
}

void *thr_sender(void *arg)
{
	/* stub */
	printf("thr_sender!\n");
}
