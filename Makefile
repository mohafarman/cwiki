##
# cwiki
#
# @file
# @version 0.1

EXECUTABLE = cwiki

CC = gcc
CFLAGS  = -g -Wall
LDLIBS = -lm -lncurses

COMPILE := $(CC) $(CFLAGS) $(LDLIBS)

dir_guard=@mkdir -p $(@D)

default: $(EXECUTABLE)

$(EXECUTABLE): bin/cwiki.o bin/tui.o
	$(COMPILE) -o ./bin/$@ ./bin/cwiki.o ./bin/tui.o

bin/cwiki.o: src/cwiki.c include/tui.h
	$(dir_guard)
	$(COMPILE) -c src/cwiki.c -o $@

bin/tui.o: src/tui.c include/tui.h
	$(dir_guard)
	$(COMPILE) -c src/tui.c -o $@

clean:
	$(RM) ./bin/$(EXECUTABLE) ./bin/*.o ./bin/*~

# end
