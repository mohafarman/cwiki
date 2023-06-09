PROGRAM=cwiki
COMPILER = gcc

LIBS = -lm

###
CFLAGS  = -std=c99
CFLAGS += -g
CFLAGS += -Wall
CFLAGS += -Wextra
CFLAGS += -pedantic
CFLAGS += -Werror
CFLAGS += -Wmissing-declarations
CFLAGS += -DUNITY_SUPPORT_64 -DUNITY_OUTPUT_COLOR

ASANFLAGS  = -fsanitize=address
ASANFLAGS += -fno-common
ASANFLAGS += -fno-omit-frame-pointer

D_SRC = src
D_BIN = bin

COMPILE=$(COMPILER) $(ASANFLAGS) $(CFLAGS) $(LDLIBS)

$(PROGRAM): $(D_SRC)/$(PROGRAM).c
	@mkdir -p bin
	$(COMPILE) $(D_SRC)/$(PROGRAM).c -o $(D_BIN)/$(PROGRAM)
	@echo "Running cwiki"; echo ""
	@./$(D_BIN)/$(PROGRAM)
