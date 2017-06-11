#ifndef _DEVINFO_H
#define _DEVINFO_H

#include <linux/devinfo.h>

long parsebat(int fd, long maxdev, struct devinfo *devinfo);

long getdevinfo(long maxdev, struct devinfo *devinfo);

char *mac2ip(const char *);

char *index2ip(int);

int ip2index(const char *);

#endif
