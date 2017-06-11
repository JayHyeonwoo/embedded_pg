KDIR ?=	/usr/src/linux
CFLAGS := -W -Wall
CPPFLAGS := -I$(KDIR)/include -I$(shell pwd)/include
CROSS_COMPILE ?= 

CC := $(CROSS_COMPILE)gcc
AR := $(CROSS_COMPILE)ar
LD := $(CROSS_COMPILE)ld

SUBDIRS := kernel studysys test ultrasonic_wave

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

export ARCH
export CFLAGS
export CPPFLAGS
export CC
export AR
export LD

all: kernel studysys ultrasonic_wave

kernel:
	$(MAKE) -C kernel KDIR=$(KDIR)

studysys:
	$(MAKE) -C $@

ultrasonic_wave:
	$(MAKE) -C $@

test:
	$(MAKE) -C $@

clean:
	$(MAKE) -C studysys clean
	$(MAKE) -C ultrasonic_wave clean
	$(MAKE) -C test clean

.PHONY: all clean $(SUBDIRS)
