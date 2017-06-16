#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <time.h>

/* user defined header files */
#include <comm.h>

void *thr_receiver(void *arg)
{
	struct sockaddr_in server_addr;
	struct packet packet;
	struct information info;
	int sock, status;

	syslog(LOG_DEBUG, "thr_receiver is created\n");
	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (-1 == sock) {
		func_syslog(LOG_ERR, "socket error: %s\n", strerror(errno));
		pthread_exit((void *)-1);
	}

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family      = AF_INET;
	server_addr.sin_port        = htons(RX_PORT);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	if( -1 == bind(sock, (struct sockaddr*)&server_addr, sizeof(server_addr) ) )
	{
		func_syslog(LOG_ERR, "bind error: %s\n", strerror(errno));
		pthread_exit((void *)-1);
	}

	while(1) {
		recvfrom(sock, &packet, sizeof(packet), 0, NULL, NULL);
		unpack_packet(&packet, &info);
		func_syslog(LOG_DEBUG, 
				"receive from %s: subject=%s, time=%lu\n",
			       	index2ip(info.index),
				info.subject_name,
				info.time);

		info.rx_time = time(NULL);
		update_infolist(&info);
	}

	pthread_exit(0);
}
