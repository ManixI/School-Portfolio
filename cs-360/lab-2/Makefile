.SUFFIXES: .c .o .zip .txt
CC = gcc
CCFLAGS =  -pedantic -Wall -std=c99 -g
EXEC = lab2
TEXEC = 
ZIP = 
OBJS =  main.o
TOBJS = 
ALLFILES = 

${EXEC}: ${OBJS}
	${CC} ${CCFLAGS} -o ${EXEC} ${OBJS} -lm

${TEXEC}: ${TOBJS}
	${CC} ${CCFLAGS} -o ${TEXEC} ${TOBJS} -lm

.c.o:
	${CC} ${CCFLAGS} -c $<

run: ${EXEC}
	./${EXEC}

test: ${TEXEC}
	./${TEXEC}

val: ${EXEC}
	valgrind --track-origins=yes ./${EXEC}

valTest: ${TEXEC}
	valgrind --track-origins=yes --leak-check=full ./${TEXEC} 

gdb: ${EXEC}
	gdb ${EXEC}

gdbTest: ${TEXEC}
	gdb ${TEXEC}

clean: 
	rm -f ${EXEC} ${OBJS} ${ZIP} ${TEXEC} ${TOBJS}

zip:
	zip -r ${ZIP} ${ALLFILES}

main.o: main.c
