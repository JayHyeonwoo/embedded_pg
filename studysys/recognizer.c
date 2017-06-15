#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <pthread.h>

/* user defined header files */
#include <comm.h>
#include <devinfo.h>
#include <schedule.h>

static time_t delay_time = 5;
static struct devinfo devinfos[DEVINFO_MAX];

void *thr_recognizer(void *arg)
{
	(void)arg;

	struct sockaddr_in client_addr;
	struct packet packet;
	struct information info;
	time_t retrieve_time;
	int sockfd;
	long ndev, i;

	syslog(LOG_DEBUG, "thr_recognizer is created\n");
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		func_syslog(LOG_ERR, "socket error: %s\n", strerror(errno));
		pthread_exit((void *)errno);
	}

	/* setup client_addr except ip */
	memset(&client_addr, 0, sizeof(client_addr));
	client_addr.sin_family = AF_INET;
	client_addr.sin_port = htons(RX_PORT);

	retrieve_time = 0;
	for ( ; ; ) {
		/* time check */
		if (retrieve_time + delay_time > time(NULL)) {
			continue;
		}
		retrieve_time = time(NULL);

		/* retrieve network interface informations */
		if ((ndev = getdevinfo(DEVINFO_MAX, devinfos)) < 0) {
			func_syslog(LOG_ERR, "getdevinfo error: %s\n",
				       	strerror(errno));
			pthread_exit((void *)errno);
		}

		self_info(&info);
		pack_information(&info, &packet);

		/* broadcast packet */
		for (i = 0; i < ndev; ++i) {
			func_syslog(LOG_DEBUG, "sendto: %s\n", devinfos[i].ip);
			client_addr.sin_addr.s_addr= inet_addr(devinfos[i].ip);
			/* send packet */
			sendto(sockfd, &packet, sizeof(packet), 0,
					(struct sockaddr *) &client_addr,
					sizeof(client_addr));
		}
	}

	pthread_exit(NULL);
}

void set_delay_time(time_t t)
{
	delay_time = t;
}
