KDIR ?=	/usr/src/linux

all: install 

install: $(KDIR)/kernel/parsebat.c \
	$(KDIR)/include/linux/parsebat.h

$(KDIR)/kernel/parsebat.c: parsebat.c
	cp parsebat.c $(KDIR)/kernel

$(KDIR)/include/linux/parsebat.h: parsebat.h
	cp parsebat.h $(KDIR)/include/linux


.PHONY: all install
