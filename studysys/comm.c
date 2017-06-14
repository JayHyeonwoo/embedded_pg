#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

/* user defined header files */
#include <comm.h>
#include <devinfo.h>
#include <schedule.h>

static unsigned int self_idx;
static pthread_mutex_t information_list_lock = PTHREAD_MUTEX_INITIALIZER;
static struct information information_list[DEVINFO_MAX];
static size_t alive_info;

int comm_init(void)
{
	pthread_t tid;
	int err;

	/* create receiver, reognizer thread */
	err = pthread_create(&tid, NULL, thr_receiver, NULL);
	if (err) {
		return err;
	}

	err = pthread_detach(tid);
	if (err) {
		return err;
	}

	pthread_create(&tid, NULL, thr_recognizer, NULL);
	if (err) {
		return err;
	}

	pthread_detach(tid);
	if (err) {
		return err;
	}

	return 0;

}

void unpack_packet(const struct packet *packet, struct information *info)
{
	info->index = packet->index;
	info->time = ntohl(packet->time);
	strcpy(info->subject_name, packet->name);
}

void pack_information(const struct information *info, struct packet *packet)
{
	packet->index = info->index;
	packet->time = htonl(info->time);
	strcpy(packet->name, info->subject_name);
}

void update_infolist(const struct information *info)
{
	pthread_mutex_lock(&information_list_lock);

	information_list[info->index] = *info;

	pthread_mutex_unlock(&information_list_lock);
}

void get_other_info(struct information *info)
{
	pthread_mutex_lock(&information_list_lock);

	*info = information_list[info->index];

	pthread_mutex_unlock(&information_list_lock);
}

unsigned int self_index(void)
{
	return self_idx;
}

void self_info(struct information *info)
{
	static schedule scheds[1000];
	int nsched;

	info->index = self_index();
	strcpy(info->subject_name, "subject");
	info->time = 10;
	//nsched = txt_read(scheds);
	//cur_subject(scheds, nsched, packet.name);
	//packet.time = htonl(get_total_min());
}

