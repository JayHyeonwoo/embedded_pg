#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "devinfo.h"
#include "comm.h"

int main(int argc, char *argv[])
{
    (void)argc;

    int sockfd;
    struct sender_arg sender_arg;
    pthread_t tid;
    void *retval;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
	perror("socket error");
	exit(-1);
    }

    sender_arg.sockfd = sockfd;
    strcpy(sender_arg.ip, argv[1]);
    sender_arg.quitflag = 0;
    sender_arg.delay_time = 1;

    pthread_create(&tid, NULL, thr_sender, &sender_arg);
    pthread_join(tid, &retval);

    exit(0);
}
