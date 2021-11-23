CFLAGS=-g -Wall -pedantic -Wno-pointer-arith
LDLIBS=-lm
LIBOBJS=lib/sockio.o lib/mcping.o lib/utils.o lib/mcproto.o

all: main
	
run: all
	bin/main

clean:
	${RM} ${LIBOBJS} bin/main || true

main: ${LIBOBJS}
	${CC} ${CFLAGS} ${LDLIBS} ${LIBOBJS} src/main.c -o bin/main

lib/%.o: src/%.c
	${CC} ${CFLAGS} -c $< -o $@
