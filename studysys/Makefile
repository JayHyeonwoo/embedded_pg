TARGET := study_system
SRCS := $(wildcard *.c)
OBJS := $(patsubst %.c, %.o, $(SRCS))
LDFLAGS := -lpthread
DEP := .dep

all: dep $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)
	ln -sfr $(TARGET) ../$(TARGET)

dep: $(DEP)

$(DEP): $(SRCS)
	$(CC) -MM $(CPPFLAGS) $^ > $@

clean:
	$(RM) *.o $(TARGET) ../$(TARGET) $(DEP)

include $(DEP)

.PHONY: all clean dep
