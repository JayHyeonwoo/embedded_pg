TARGET := ultrasonic
SRCS := $(wildcard *.c)
OBJS := $(patsubst %.c, %.o, $(SRCS))
LDFLAGS := -lwiringPi -lpthread

DEP := .dep

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)
	ln -sfr $@ ../$@

dep: $(DEP)

$(DEP): $(SRCS)
	$(CC) -MM $(CPPFLAGS) $^ > $@
clean:
	$(RM) $(TARGET) $(OBJS) $(DEP) ../$(TARGET)

include $(DEP)

.PHONY: all clean dep
