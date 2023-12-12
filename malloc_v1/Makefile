CC = gcc
CPPFLAGS = -D_DEFAULT_SOURCE
CFLAGS = -Wall -Wextra -Werror -std=c99 -Wvla
VPATH = src

TARGET_LIB = libmalloc.so
OBJS = my_malloc.o tools.o blk_allocator.o my_recycler.o

TEST_OBJS = tests/malloc.o
TEST_BIN = test

# Default target
all: library

# Library target
library: $(TARGET_LIB)

# Building the shared library
$(TARGET_LIB): CFLAGS += -pedantic -fvisibility=hidden -fPIC
$(TARGET_LIB): LDFLAGS += -Wl,--no-undefined -shared
$(TARGET_LIB): OBJS += malloc.o
$(TARGET_LIB): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

# Debug target
debug: CFLAGS += -g
debug: clean $(TARGET_LIB)

# Clean target
clean:
	$(RM) $(TARGET_LIB) $(OBJS) $(TEST_OBJS) $(TEST_BIN)

# Check target
check: LDLIBS = -lcriterion
check: CFLAGS += -g
check: $(TEST_OBJS) $(filter-out malloc.o, $(OBJS))
	$(CC) $(LDFLAGS) $(LDLIBS) -o $(TEST_BIN) $^
	./$(TEST_BIN)

# Pattern rules for object files
%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

.PHONY: all library debug clean check
