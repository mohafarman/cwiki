##
# cwiki
#
# @file
# @version 0.1

EXECUTABLE = cwiki

CC = gcc
CFLAGS=-g3 -O0 -Wall -Werror -pedantic -std=c11
LDLIBS = -lm -lncurses -lmenu -lcurl -lcjson -lpanel -lxml2
COMPILE = $(CC) $(CFLAGS) $(LDLIBS)

BUILD_DIR := ./bin
SRC_DIR := ./src
HDR_DIR := ./include

# SRCS_BAK := $(wildcard $(SRC_DIR)/*.c)
SRCS := $(shell find $(SRC_DIR) -name '*.c')
HDRS := $(shell find $(HDR_DIR) -name '*.h')
OBJS := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))
# OBJS_BAK := $(BUILD_DIR)/cwiki.o $(BUILD_DIR)/cwiki_tui.o $(BUILD_DIR)/cwiki_log.o $(BUILD_DIR)/cwiki_curl.o $(BUILD_DIR)/cwiki_utils.o

dir_guard=@mkdir -p $(@D)

default: $(BUILD_DIR)/$(EXECUTABLE)

print:
	@echo $(SRCS)
	@echo $(OBJS)

run:
	$(BUILD_DIR)/cwiki

dir:
	@mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/$(EXECUTABLE): $(OBJS)
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
	$(RM) $(BUILD_DIR)/$(EXECUTABLE) $(OBJS) $(BUILD_DIR)/*~

# end
