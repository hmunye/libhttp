BIN = httpc
SRCDIRS = . ./src
INCDIRS = . ./include
BUILDDIR = ./build

CC = gcc
CFLAGS = -Wall -Wextra -Werror -Wconversion --std=c99 -Wpedantic

INCLUDES = $(foreach DIR,$(INCDIRS),-I$(DIR))
DEPFLAGS = -MP -MD
DFLAGS = -D_DEBUG -g
LDFLAGS = -pthread

CFLAGS += $(INCLUDES) $(DEPFLAGS) $(LDFLAGS)

CFILES = $(foreach DIR,$(SRCDIRS),$(wildcard $(DIR)/*.c))

BIN_OBJDIR = $(BUILDDIR)/bin
BIN_OBJFILES = $(patsubst %.c,$(BIN_OBJDIR)/%.o,$(CFILES))
BIN_DEPFILES = $(patsubst %.c,$(BIN_OBJDIR)/%.d,$(CFILES))

.PHONY: all debug test clean help

all: $(BIN)

debug: CFLAGS += $(DFLAGS)
debug: all

$(BIN): $(BIN_OBJFILES)
	$(CC) -o $@ $^

$(BIN_OBJDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

test:
	@make --no-print-directory -C test

clean:
	@rm -rf $(BIN) $(BUILDDIR)

help:
	@echo "Available targets:"
	@echo "  all       - Build binary ($(BIN))"
	@echo "  debug     - Build binary with debugging symbols and _DEBUG macro ($(BIN))"
	@echo "  test      - Build test binary and run all tests"
	@echo "  clean     - Clean up generated files (binary, object files, dependencies)"
	@echo "  help      - Show this message"

memcheck: debug
	@valgrind --tool=memcheck --leak-check=full --track-origins=yes --show-leak-kinds=all ./$(BIN)

threadcheck: debug
	@valgrind --tool=helgrind --history-level=none -s ./$(BIN)

test-debug:
	@make --no-print-directory -C test debug

test-memcheck:
	@make --no-print-directory -C test memcheck

test-threadcheck:
	@make --no-print-directory -C test threadcheck

test-clean:
	@make --no-print-directory -C test clean

-include $(BIN_DEPFILES)
