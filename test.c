#include <linux/unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "devinfo.h"
#include "comm.h"

int main(int argc, char *argv[])
{
	pthread_t receive_tid, recognizer_tid;
	void *retval;


	pthread_create(&receive_tid, NULL, thr_receiver, NULL);
	pthread_create(&recognizer_tid, NULL, thr_recognizer, NULL);

	pthread_join(receive_tid, &retval);
	pthread_join(recognizer_tid, &retval);


	exit(0);
}
