#include <linux/unistd.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <devinfo.h>

#ifdef ARM
#define __NR_parsebat	397
#else
#define __NR_parsebat	385
#endif

/*
 * 주어진 MAC주소와 IP주소
 * 고정적임
 */
static struct devinfo arptbl[] = {
	{ .mac = "90:9f:33:ec:04:8a", .ip = "192.168.2.1"},
	{ .mac = "90:9f:33:eb:63:e9", .ip = "192.168.2.2"},
	{ .mac = "b8:27:eb:12:ca:e0", .ip = "192.168.2.3"},
	{ .mac = "64:e5:99:fa:1d:61", .ip = "192.168.2.4"},
};

const char *batctl_path = "/usr/local/sbin/batctl";
char *const batctl_args[] = {
	"batctl",
	"n",
	(char *)0
};

/*
 * MAC주소에서 IP주소를 얻는 함수
 * MAC주소와 IP주소는 고정적임
 */
char *mac2ip(const char *mac)
{
	size_t i;
	for (i = 0; i < sizeof arptbl / sizeof arptbl[0]; ++i) {
		if (strcmp(mac, arptbl[i].mac) == 0) {
			return arptbl[i].ip;
		}
	}

	return NULL;
}

char *index2ip(int id)
{
	return arptbl[id - 1].ip;
}

int ip2index(const char *ip)
{
	return ip[IP_NAME_MAX - 2] - '0';
}

/*
 * batctl의 출력 결과를 파싱하여 struct devinfo에 최대 maxdev만큼 채우는 함수
 *
 * Return
 * 성공시 struct devinfo의 개수, 실패시 -1
 */
long parsebat(int fd, long maxdev, struct devinfo *devinfo)
{
	return syscall(__NR_parsebat, fd, maxdev, devinfo);
}


/*
 * 최대 maxdev만큼의 struct devinfo의 정보를 채운다
 *
 * Return
 * 성공시 struct devinfo의 개수, 실패시 -1
 */
long getdevinfo(long maxdev, struct devinfo *devinfo)
{
	const char *ip;
	int fds[2], stat;
	long ndev, i;
	pid_t pid;

	if (pipe(fds) < 0) {
		perror("pipe error");
		return -1;
	}

	if ((pid = fork()) < 0) {
		perror("fork error");
		return -1;
	} else if (pid == 0) {
		/* Child process */
		/* 파이프 출력을 stdout으로 */
		if (dup2(fds[1], STDOUT_FILENO) < 0) {
			perror("dup2 error");
			return -1;
		}
		close(fds[1]);

		/* batctl n 실행 */
		if (execv(batctl_path, batctl_args) < 0) {
			perror("execv error");
			return -1;
		}
	}

	/* Parent process */
	close(fds[1]);
	ndev = parsebat(fds[0], maxdev, devinfo);
	waitpid(pid, &stat, 0);

	/* MAC 주소에 따라 IP주소 찾기 */
	for (i = 0; i < ndev; ++i) {
		ip = mac2ip(devinfo[i].mac);
		if (ip == NULL) {
			return -1;
		}
		strcpy(devinfo[i].ip, ip);
	}

	return ndev;
}
