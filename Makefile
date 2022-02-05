CFLAGS=-Wall -Wextra -Werror -pedantic -Wno-pointer-arith
LDFLAGS=
LDLIBS=-lm -lpthread
LIBOBJS=lib/sockio.o lib/mcping.o lib/utils.o lib/mcproto.o lib/stb_ds.o lib/logging.o
EXTLIBS=src/extras/cjson/cJSON.o

all: main
	
run: all
	bin/main ${RUN_ARGS}

clean: clean_json clean_bin

clean_bin:
	${RM} ${LIBOBJS} bin/main || true

clean_json:
	( cd src/extras/cjson; make clean )

main: cjson ${LIBOBJS}
	${CC} ${CFLAGS} ${LIBOBJS} ${EXTLIBS} src/main.c -o bin/main ${LDFLAGS} ${LDLIBS}

lib/%.o: src/%.c
	${CC} ${CFLAGS} -c $< -o $@

cjson:
	( cd src/extras/cjson; make )

valgrind:
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes \
		./bin/main ${RUN_ARGS}
