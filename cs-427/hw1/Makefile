CPP = gcc
FLAGS = -Wall -Werror -std=c99 -pedantic -g

EXEC= decypher
OBJS= decypher.o


default:${EXEC}

clean:
	rm -f ${EXEC}
	rm -f *.o

run: ${EXEC}
	./${EXEC}

${EXEC}:${OBJS}
	${CPP} ${FLAGS} -o ${EXEC} ${OBJS}

.c.o:
	${CPP} ${FLAGS} -c $<

decyper.o: decypher.c