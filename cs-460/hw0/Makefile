CPP = gcc
FLAGS = -Wall -Werror -std=c99 -pedantic -g

EXEC_CAT = my-cat
OBJS_CAT = my-cat.o

EXEC_SED = my-sed
OBJS_SED = my-sed.o

EXEC_UNIQ = my-uniq
OBJS_UNIQ = my-uniq.o

default:${EXEC_CAT}

cat:${EXEC_CAT}

sed:${EXEC_SED}

uniq:${EXEC_UNIQ}

clean:
	rm -f ${EXEC_CAT} ${EXEC_SED} ${EXEC_UNIQ}
	rm -f *.o

runCat: ${EXEC_CAT}
	./${EXEC_CAT}

runSed: ${EXEC_SED}
	./${EXEC_SED}

runUniq: ${EXEC_UNIQ}
	./${EXEC_UNIQ}

${EXEC_CAT}:${OBJS_CAT}
	${CPP} ${FLAGS} -o ${EXEC_CAT} ${OBJS_CAT}

${EXEC_SED}:${OBJS_SED}
	${CPP} ${FLAGS} -o ${EXEC_SED} ${OBJS_SED}

${EXEC_UNIQ}:${OBJS_UNIQ}
	${CPP} ${FLAGS} -o ${EXEC_UNIQ} ${OBJS_UNIQ}

.c.o:
	${CPP} ${FLAGS} -c $<

my-cat.o: my-cat.c
my-sed.o: my-sed.c my-sed.h
my-uniq.o: my-uniq.c my-uniq.h