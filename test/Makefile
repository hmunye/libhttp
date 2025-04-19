BIN = test_runner
SRCDIRS = . ./src ../src
INCDIRS = . ./include ../include
BUILDDIR = ./build

CC = gcc
CFLAGS = -Wall -Wextra -Werror -Wconversion --std=c99 -Wpedantic

INCLUDES = $(foreach DIR,$(INCDIRS),-I$(DIR))
# DFLAGS = -D_DEBUG -g
DFLAGS = -g
LDFLAGS = -pthread

CFLAGS += $(INCLUDES) $(LDFLAGS)

CFILES = $(foreach DIR,$(SRCDIRS),$(wildcard $(DIR)/*.c))

BIN_OBJDIR = $(BUILDDIR)/bin
BIN_OBJFILES = $(patsubst %.c,$(BIN_OBJDIR)/%.o,$(CFILES))

.PHONY: all debug memcheck threadcheck clean

all: $(BIN)
	@./$(BIN)

debug: CFLAGS += $(DFLAGS)
debug: $(BIN)

$(BIN): $(BIN_OBJFILES)
	@$(CC) -o $@ $^

$(BIN_OBJDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -c -o $@ $<

memcheck: debug
	@valgrind --tool=memcheck --leak-check=full --track-origins=yes --show-leak-kinds=all ./$(BIN)

threadcheck: debug
	@valgrind --tool=helgrind --history-level=none -s ./$(BIN)

clean:
	@rm -rf $(BIN) $(BUILDDIR)
