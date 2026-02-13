CC      := gcc
CFLAGS  := -O2 -Wall -Wextra -MMD -MP

SRCS := $(wildcard src/*.c)
OBJS := $(SRCS:src/%.c=build/%.o)

build:
	mkdir -p build

bin:
	mkdir -p bin

all: bin/normalize

bin/normalize: $(OBJS) | bin
	$(CC) $(CFLAGS) -o $@ $(OBJS)

build/%.o: src/%.c | build
	$(CC) $(CFLAGS) -c $< -o $@

-include $(OBJS:.o=.d)

clean:
	rm -r build bin

.PHONY: all clean
