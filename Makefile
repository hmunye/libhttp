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

.PHONY: bin debug clean help

bin: $(BIN)

debug: CFLAGS += $(DFLAGS)
debug: bin

$(BIN): $(BIN_OBJFILES)
	$(CC) -o $@ $^

$(BIN_OBJDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	@rm -rf $(BIN) $(BUILDDIR)

help:
	@echo "Available targets:"
	@echo "  bin       - Build binary executable ($(BIN))"
	@echo "  debug     - Build binary with debugging symbols and _DEBUG macro ($(BIN))"
	@echo "  clean     - Clean up generated files (binary, object files, dependencies)"
	@echo "  help      - Show this message"

-include $(BIN_DEPFILES)
