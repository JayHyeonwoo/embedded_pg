KDIR ?=	/usr/src/linux

all: cpfiles

cpfiles: $(KDIR)/kernel/parsebat.c \
	$(KDIR)/include/linux/devinfo.h

$(KDIR)/kernel/parsebat.c: parsebat.c
	cp parsebat.c $(KDIR)/kernel

$(KDIR)/include/linux/devinfo.h: devinfo.h
	cp devinfo.h $(KDIR)/include/linux


.PHONY: all cpfiles
