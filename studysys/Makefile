TARGET := study_system
OBJS := $(patsubst %.c, %.o, $(wildcard *.c))
LDFLAGS := -lpthread

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)
	ln -s $$(pwd)/$(TARGET) $$(pwd)/../$(TARGET)

clean:
	$(RM) $(OBJS) $(TARGET) ../$(TARGET)

.PHONY: all clean
