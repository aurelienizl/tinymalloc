CC = gcc
CPPFLAGS = -D_DEFAULT_SOURCE
CFLAGS = -Wall -Wextra -Werror -std=c99 -Wvla
LDFLAGS = 
VPATH = src

TARGET_LIB = libmalloc.so
OBJS = my_malloc.o tools.o blk_allocator.o my_recycler.o

TEST_OBJS = tests/malloc.o
TEST_BIN = test

COV_DIR = coverage
COV_FLAGS = -fprofile-arcs -ftest-coverage

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
	$(RM) $(TARGET_LIB) $(OBJS) $(TEST_OBJS) $(TEST_BIN) *.gcda *.gcno *.gcov
	$(RM) -r $(COV_DIR)

# Check target
check: LDLIBS = -lcriterion
check: CFLAGS += -g
check: $(TEST_OBJS) $(filter-out malloc.o, $(OBJS))
	$(CC) $(LDFLAGS) $(LDLIBS) -o $(TEST_BIN) $^
	./$(TEST_BIN)

# Coverage target
coverage: CFLAGS += $(COV_FLAGS)
coverage: LDFLAGS += $(COV_FLAGS)
coverage: clean check
	mkdir -p $(COV_DIR)
	mv *.gcno *.gcda $(COV_DIR)
	lcov --directory $(COV_DIR) --capture --output-file $(COV_DIR)/coverage.info
	genhtml $(COV_DIR)/coverage.info --output-directory $(COV_DIR)

# Pattern rules for object files
%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

.PHONY: all library debug clean check coverage
