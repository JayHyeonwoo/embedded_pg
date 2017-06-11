#TARGETS = ultrasonic_wave

SCHEDULE_OBJS = schedule.o bright_control.o
ULTRASONIC_OBJS = ultrasonic_wave.o

OBJS = $(SCHEDULE_OBJS) $(ULTRASONIC_OBJS)

CC = gcc

CFLAGS = -Wall -W
CFLAGS += -g

%.o: %.c
	$(CC) -o $*.o $< -c $(CFLAGS)  	$(CPPFLAGS) $(LDFLAGS)
	mv $@ ../

all: $(TARGETS) $(SCHEDULE_OBJS)

ultrasonic_wave: $(ULTRASONIC_OBJS)
	$(CC) -o $@ $^ -lwiringPi $(LDFLAGS)

clean:
	-rm -f $(TARGETS) $(OBJS)
