KDIR ?=	/usr/src/linux
CFLAGS := -W -Wall
CPPFLAGS := -I$(KDIR)/include
LDFLAGS := -lpthread
CROSS_COMPILE ?= 

CC := $(CROSS_COMPILE)gcc
AR := $(CROSS_COMPILE)ar
LD := $(CROSS_COMPILE)ld

ifeq ($(DEBUG),y)
	CPPFLAGS += -DDEBUG
	CFLAGS += -g
else
	CFLAGS += -O2
endif

ifeq ($(ARCH),arm)
	CPPFLAGS += -DARM
else
	CPPFLAGS += -DX86
endif

export KDIR
export ARCH

all: kernel test

kernel:
	$(MAKE) -C kernel

test: test.o devinfo.o
	$(CC) -o $@ $?

clean:
	$(RM) devinfo.o test.o test tags *.o

.PHONY: all clean kernel
