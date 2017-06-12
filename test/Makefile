$(DEP) := .dep

all:

dep: $(DEP)

$(DEP): $(SRCS)
	$(CC) -MM $(CPPFLAGS) $^ > $@

clean:
	$(RM) $(OBJS) $(DEP) $(TARGETS)

include $(DEP)

.PHONY: all clean dep
