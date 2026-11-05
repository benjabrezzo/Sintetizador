# Tests de lógica del firmware en la PC (gcc + make, sin frameworks).
# Cada test_*.c es un programa que devuelve 0 si pasa y distinto de 0 si falla.

CC      := gcc
CFLAGS  := -std=c99 -Wall -Wextra -Werror
BUILD   := build
SOURCES := $(wildcard test_*.c)
TESTS   := $(SOURCES:%.c=$(BUILD)/%)

.PHONY: test clean

test: $(TESTS)
	@for t in $(TESTS); do echo "== $$t"; $$t || exit 1; done

$(BUILD)/%: %.c | $(BUILD)
	$(CC) $(CFLAGS) -o $@ $<

$(BUILD):
	mkdir -p $(BUILD)

clean:
	rm -rf $(BUILD)
