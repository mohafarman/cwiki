##
# cwiki
#
# @file
# @version 0.1

EXECUTABLE = cwiki

CC = gcc
CFLAGS=-g -Wall -Werror -pedantic
LDLIBS = -lm -lncurses -lzlog
COMPILE = $(CC) $(CFLAGS) $(LDLIBS)

BUILD_DIR := ./bin
SRC_DIR := ./src
HDR_DIR := ./include

SRCS := $(shell find $(SRC_DIR) -name '*.c')
HDRS := $(shell find $(HDR_DIR) -name '*.h')
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)

dir_guard=@mkdir -p $(@D)

default: $(BUILD_DIR)/$(EXECUTABLE)

print:
	@echo $(SRCS)
	@echo $(HDRS)

run:
	./$(BUILD_DIR)/cwiki

$(BUILD_DIR)/$(EXECUTABLE): $(BUILD_DIR)/cwiki.o $(BUILD_DIR)/cwiki_tui.o $(BUILD_DIR)/cwiki_log.o
	$(COMPILE) $^ -o $@

# Define a pattern rule that compiles every .c file into a .o file
$(BUILD_DIR)/%.o : ./src/%.c $(HDRS)
	$(dir_guard)
	$(COMPILE) -c $< -o $@

# Previous. Back up plan.
# $(BUILD_DIR)/cwiki.o: src/cwiki.c include/cwiki_tui.h
# 	$(dir_guard)
# 	$(COMPILE) -c src/cwiki.c -o $@
#
# $(BUILD_DIR)/cwiki_tui.o: src/cwiki_tui.c include/cwiki_tui.h
# 	$(dir_guard)
# 	$(COMPILE) -c src/cwiki_tui.c -o $@

clean:
	$(RM) ./$(BUILD_DIR)/$(EXECUTABLE) ./$(BUILD_DIR)/*.o ./$(BUILD_DIR)/*~

# end
