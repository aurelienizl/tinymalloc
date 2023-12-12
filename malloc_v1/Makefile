CC = gcc
CPPFLAGS = -D_DEFAULT_SOURCE
CFLAGS = -Wall -Wextra -Werror -std=c99 -Wvla -std=c99
VPATH = src

TARGET_LIB = libmalloc.so
OBJS = my_malloc.o tools.o allocator.o recycler.o

TEST_OBJS = tests/malloc.o
TEST_BIN = test

all: library

library: $(TARGET_LIB)
$(TARGET_LIB): CFLAGS += -pedantic -fvisibility=hidden -fPIC
$(TARGET_LIB): LDFLAGS += -Wl,--no-undefined -shared
$(TARGET_LIB): OBJS += malloc.o
$(TARGET_LIB): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

debug: CFLAGS += -g
debug: clean $(TARGET_LIB)

clean:
	$(RM) $(TARGET_LIB) $(OBJS) ${TEST_OBJS} ${TEST_BIN}

check: LDLIBS = -lcriterion
check: CFLAGS = -g
check: ${TEST_OBJS} ${OBJS}
	$(CC) $(LDFLAGS) ${LDLIBS} -o ${TEST_BIN} $^
	./${TEST_BIN}

.PHONY: all $(TARGET_LIB) clean check
