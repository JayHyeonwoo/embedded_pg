TARGET := ultrasonic
OBJS := $(patsubst %.c, %.o, $(wildcard *.c))
LDFLAGS := -lwiringPi -lpthread

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

clean:
	$(RM) $(TARGET)

.PHONY: all clean
