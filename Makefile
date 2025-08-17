CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -O2 -g -Iinclude
LDFLAGS =
SRC = $(wildcard src/*.c)
OBJ = $(SRC:src/%.c=build/%.o)
EXEC = build/nonogram

# Exclude main.c for tests (avoid duplicate main at link)
SRC_NOMAIN = $(filter-out src/main.c,$(SRC))
OBJ_NOMAIN = $(SRC_NOMAIN:src/%.c=build/%.o)

TEST_SRC = $(wildcard tests/*.c)
TEST_OBJ = $(TEST_SRC:tests/%.c=build/tests_%.o)
TEST_EXEC = build/test_runner

all: $(EXEC)

$(EXEC): $(OBJ) | build
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Objects from src
build/%.o: src/%.c | build
	$(CC) $(CFLAGS) -c $< -o $@

# Objects from tests (separate name pattern to avoid rule collision)
build/tests_%.o: tests/%.c | build
	$(CC) $(CFLAGS) -c $< -o $@

tests: $(TEST_EXEC)

# Link tests against app objects *without* main.o
$(TEST_EXEC): $(OBJ_NOMAIN) $(TEST_OBJ) | build
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

build:
	mkdir -p build

clean:
	rm -rf build

.PHONY: all clean tests
