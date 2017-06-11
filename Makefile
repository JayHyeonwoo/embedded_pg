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

all: kernel test test_sender

kernel:
	$(MAKE) -C kernel

test: test.o devinfo.o recognizer.o receiver.o
	$(CC) -o $@ test.o devinfo.o recognizer.o receiver.o $(LDFLAGS)

test_sender: sender.o packet.o
	$(CC) -o $@ sender.o packet.o $(LDFLAGS)

clean:
	$(RM) devinfo.o test.o test tags *.o test_sender

.PHONY: all clean kernel
