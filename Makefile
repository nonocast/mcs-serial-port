BUILD_DIR = build/
SRC_DIR= src/
ENTRY=app-a.c
OUTPUT=app-a.ihx
PROTOCOL=stc89
DEVICE=/dev/tty.usbserial-14110

build: ${OUTPUT}

flash: build
	stcgal -P ${PROTOCOL} -p ${DEVICE} ${BUILD_DIR}${OUTPUT} 

${OUTPUT}:
	mkdir -p build && sdcc -o $(BUILD_DIR) ${SRC_DIR}${ENTRY}

clean:
	rm -f ${BUILD_DIR}*

test-compile:
	gcc test/test.c -o build/test

test: test-compile
	./build/test

.PHONY: all test clean