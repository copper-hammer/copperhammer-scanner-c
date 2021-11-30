CFLAGS=-g -Wall -pedantic -Wno-pointer-arith
LDFLAGS=
LDLIBS=-lm
LIBOBJS=lib/sockio.o lib/mcping.o lib/utils.o lib/mcproto.o lib/stb_ds.o lib/logging.o
EXTLIBS=src/extras/cjson/cJSON.o

all: main
	
run: all
	bin/main ${RUN_ARGS}

clean:
	${RM} ${LIBOBJS} bin/main || true
	( cd src/extras/cjson; make clean )

main: cjson ${LIBOBJS}
	${CC} ${CFLAGS} ${LIBOBJS} ${EXTLIBS} src/main.c -o bin/main ${LDFLAGS} ${LDLIBS}

lib/%.o: src/%.c
	${CC} ${CFLAGS} -c $< -o $@

cjson:
	( cd src/extras/cjson; make )
