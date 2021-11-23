CC=gcc
CFLAGS=-g -lm -Wall -pedantic -Wno-pointer-arith
LIBRARIES=lib/sockio.o lib/mcping.o lib/utils.o lib/mcproto.o

all: main
	
run: all
	bin/main

clean:
	rm ${LIBRARIES} bin/main || true

main: ${LIBRARIES} Makefile
	${CC} ${CFLAGS} ${LIBRARIES} src/main.c -o bin/main

lib/%.o: src/%.c
	${CC} ${CFLAGS} -c $< -o $@
