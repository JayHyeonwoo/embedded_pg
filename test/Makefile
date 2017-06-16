$(DEP) := .dep


%: %.c
	$(CC) -o $@ $^ $(CFLAGS) $(CPPFLAGS) $(LDFLAGS)

all: getdevinfo_test

dep: $(DEP)

$(DEP): $(SRCS)
	$(CC) -MM $(CPPFLAGS) $^ > $@

clean:
	$(RM) $(OBJS) $(DEP) $(TARGETS) *.o

include $(DEP)

.PHONY: all clean dep
