CC = gcc

CFLAGS  = -std=gnu11 -Wall -Wextra -pedantic

SOURCES := $(shell find ./src -name "*.c")
OBJS = ${SOURCES:.c=.o}

OUTPUT = sh13-client
OUTPUT_DIR = dist

all: build

build: ${OBJS}
	@${CC} $(CFLAGS) -o ./${OUTPUT_DIR}/${OUTPUT} ${OBJS} -lSDL2 -lSDL2_image -lSDL2_ttf -lm

%.o: %.c
	@${CC} -c $< -o $@

clean:
	@find . -type f -name '*.o' -exec rm {} +
	@rm -f ./${OUTPUT_DIR}/${OUTPUT}