KDIR ?=	/usr/src/linux
CFLAGS := -W -Wall
CPPFLAGS := -I$(KDIR)/include -I$(shell pwd) -I$(shell pwd)/app_dir
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
export CPPFLAGS
export LDFLAGS

all: kernel app_dir schedule 

kernel:
	$(MAKE) -C kernel

app_dir:
	$(MAKE) -C app_dir

schedule: devinfo.o recognizer.o receiver.o sender.o packet.o \
    schedule.o bright_control.o
	$(CC) -o $@ $^ $(LDFLAGS)

test: test.o devinfo.o recognizer.o receiver.o sender.o
	$(CC) -o $@ test.o devinfo.o recognizer.o receiver.o sender.o $(LDFLAGS)

test_sender: test_sender.o sender.o packet.o
	$(CC) -o $@ test_sender.o sender.o packet.o $(LDFLAGS)

clean:
	$(RM) devinfo.o test.o test tags *.o test_sender schedule
	$(MAKE) clean -C app_dir

.PHONY: all clean kernel app_dir
