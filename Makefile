KDIR ?=	/usr/src/linux
PWD := $(shell pwd)
CFLAGS := -W -Wall
CPPFLAGS := -I$(PWD)/include -I$(PWD)/kernel

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

ARCH ?= $(shell uname -m | sed -e 's/i.86/x86/g' -e 's/x86_64/x86/g' \
		-e 's/arm.*/arm/g')
ifeq ($(ARCH),arm)
	CPPFLAGS += -DCONFIG_ARM
endif

ifeq ($(ARCH),x86)
	CPPFLAGS += -DCONFIG_X86
endif

ifeq ($(CONFIG_ULTRASONIC),y)
	CPPFLAGS += -DCONFIG_ULTRAWAVE
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

tags:
	ctags -R

clean:
	$(RM) tags
	$(MAKE) -C studysys clean
	$(MAKE) -C ultrasonic_wave clean
	$(MAKE) -C test clean

.PHONY: all clean $(SUBDIRS) tags
