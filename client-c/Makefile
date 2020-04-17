# make start
# make -s start (slient)

CC = gcc
CFLAGS = -I.

SRC_DIR = src/
ENTRY = app.c

BUILD_DIR = build/
TARGET= app

start: build
	./build/app

build:
	mkdir -p build && gcc -o $(BUILD_DIR)${TARGET} ${SRC_DIR}${ENTRY}

clean:
	rm -rf ${BUILD_DIR}

.PHONY: all build run clean