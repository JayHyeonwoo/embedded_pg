#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <devinfo.h>

int main(void)
{
	long ndev, i;
	struct devinfo devs[DEVINFO_MAX];

	for (;;) {
		ndev = getdevinfo(DEVINFO_MAX, devs);
		if (ndev < 0) {
			perror("getdevinfo error");
			continue;
		}

		printf("ndev: %ld\n", ndev);
		for (i = 0; i < ndev; ++i) {
			printf("mac: %s, ip: %s\n", devs[i].mac, devs[i].ip);
		}

		sleep(1);
	}

	exit(0);
}
