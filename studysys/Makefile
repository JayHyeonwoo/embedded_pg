TARGET := study_system
SRCS := $(wildcard *.c)
OBJS := $(patsubst %.c, %.o, $(SRCS))
LDFLAGS := -lpthread
DEP := .dep

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)
	ln -sf $$(pwd)/$(TARGET) $$(pwd)/../$(TARGET)

dep: $(DEP)

$(DEP): $(SRCS)
	$(CC) -MM $(CPPFLAGS) $^ > $@

clean:
	$(RM) $(OBJS) $(TARGET) ../$(TARGET) $(DEP)

include $(DEP)

.PHONY: all clean dep
