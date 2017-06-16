#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>
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
	fd_set rfdset;
	int sock, reuse = 1;

	(void)arg;

	syslog(LOG_DEBUG, "thr_receiver is created\n");
	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (-1 == sock) {
		func_syslog(LOG_ERR, "socket error: %s\n", strerror(errno));
		pthread_exit(NULL);
	}

	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int)) < 0) {
		func_syslog(LOG_ERR, "setsockopt error: %s\n", strerror(errno));
		goto out;
	}

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family      = AF_INET;
	server_addr.sin_port        = htons(RX_PORT);
	inet_aton(index2ip(self_index()), &server_addr.sin_addr);
	if( -1 == bind(sock, (struct sockaddr*)&server_addr, sizeof(server_addr) ) )
	{
		func_syslog(LOG_ERR, "bind error: %s\n", strerror(errno));
		goto out;
	}

	while(1) {
		/* I/O multiplexing */
		FD_ZERO(&rfdset);
		FD_SET(sock, &rfdset);
		if (select(sock + 1, &rfdset, NULL, NULL, NULL) < 0) {
			func_syslog(LOG_ERR, "select error: %s", strerror(errno));
			goto out;
		}

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

out:
	close(sock);
	pthread_exit(0);

}
