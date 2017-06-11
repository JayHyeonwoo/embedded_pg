TARGETS = schedule ultrasonic_wave

SCHEDULE_OBJS = schedule.o bright_control.o
ULTRASONIC_OBJS = ultrasonic_wave.o

OBJS = $(SCHEDULE_OBJS) $(ULTRASONIC_OBJS)

CC = gcc

CFLAGS = -Wall -W
CFLAGS += -g

%.o: %.c
	$(CC) -o $*.o $< -c $(CFLAGS)  	

all: $(TARGETS)


schedule: $(SCHEDULE_OBJS)
	$(CC) -o $@ $^ 

ultrasonic_wave: $(ULTRASONIC_OBJS)
	$(CC) -o $@ $^ -lwiringPi

clean:
	-rm -f $(TARGETS) $(OBJS)
