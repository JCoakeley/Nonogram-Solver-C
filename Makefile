CC = gcc
CFLAGS = -ansi -pedantic -Wall -Wextra -O2 -pg -Iinclude
LDFLAGS =
SRC = $(wildcard src/*.c)
OBJ = $(SRC:src/%.c=build/%.o)
EXEC = build/nonogram

TEST_SRC = $(wildcard tests/*.c)
TEST_OBJ = $(TEST_SRC:tests/%.c=build/%.o)
TEST_EXEC = build/test_runner

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

build/%.o: src/%.c | build
	$(CC) $(CFLAGS) -c $< -o $@

tests: $(TEST_EXEC)

$(TEST_EXEC): $(OBJ) $(TEST_OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

build/%.o: tests/%.c | build
	$(CC) $(CFLAGS) -c $< -o $@

build:
	mkdir -p build

clean:
	rm -rf build

.PHONY: all clean tests
