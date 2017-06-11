TARGET := ultrasonic
OBJS := $(patsubst %.c, %.o, $(wildcard *.c))
LDFLAGS := -lwiringPi -lpthread

ultrasonic: $(OBJS)
	$(CC) -o $@ $^ -lwiringPi $(LDFLAGS)
