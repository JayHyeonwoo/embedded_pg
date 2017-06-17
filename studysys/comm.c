#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <ifaddrs.h>
#include <pthread.h>
#include <string.h>

/* user defined header files */
#include <comm.h>
#include <devinfo.h>
#include <schedule.h>

static unsigned int self_idx;
static pthread_mutex_t information_list_lock = PTHREAD_MUTEX_INITIALIZER;
static struct information information_list[DEVINFO_MAX];

static int self_index_init(void);

int comm_init(void)
{
	pthread_t tid;
	int err;

	if (self_index_init() < 0) {
		return -1;
	}

	func_syslog(LOG_INFO, "%s", index2ip(self_index()));

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

static int self_index_init(void)
{
	struct ifaddrs *ifap, *ifa;
	struct sockaddr_in *sa;
	char *addr;

	getifaddrs(&ifap);
	for (ifa = ifap; ifa != NULL; ifa = ifa->ifa_next) {
		if (ifa->ifa_addr->sa_family == AF_INET &&
				strcmp(ifa->ifa_name, "bat0") == 0) {
			sa = (struct sockaddr_in *)ifa->ifa_addr;
			addr = inet_ntoa(sa->sin_addr);
			self_idx = ip2index(addr);
			freeifaddrs(ifap);
			return 0;
		}
	}

	freeifaddrs(ifap);
	return -1;
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
	int index = info->index;

	pthread_mutex_lock(&information_list_lock);

	information_list[index] = *info;

	pthread_mutex_unlock(&information_list_lock);
}

void get_other_info(struct information *info)
{
	int index = info->index;

	pthread_mutex_lock(&information_list_lock);

	*info = information_list[index];

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
	nsched = txt_read(scheds);
	cur_subject(scheds, nsched, &info->subject_name);
#ifdef CONFIG_ULTRAWAVE
	info->time = get_total_min();
#else
	info->time = 10;
#endif
}

