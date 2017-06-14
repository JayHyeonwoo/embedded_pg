#ifndef _KERNEL_DEVINFO_H
#define _KERNEL_DEVINFO_H

#define DEVINFO_MAX	256
#define MAC_NAME_MAX	18
#define IP_NAME_MAX	16

struct devinfo {
	char	mac[MAC_NAME_MAX];
	char	ip[IP_NAME_MAX];
};

#endif
