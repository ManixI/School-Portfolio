.SUFFIXES: .c .o .zip .txt
CC = gcc
CCFLAGS =  -pedantic -Wall -g
EXECC = myftp  		# client size
EXECS = myftpserver # server side
TEXEC = finalTest
ZIP = myftp.zip
OBJSC = myftp.o
OBJSS = myftpserver.o
TOBJS = 
ALLFILES = myftp.c myftpserver.c Makefile README.md
1 = myftp.c
2 = myftpserver.c

all: client server

: ${OBJS}
	${CC} ${CCFLAGS} -o ${EXECC} myftp.c -lm
	${CC} ${CCFLAGS} -o ${EXECS} myftpserver.c -lm

${EXEC}: ${OBJS}
	${CC} ${CCFLAGS} -o ${EXEC} ${OBJS} -lm

client: ${OBJSC}
	${CC} ${CCFLAGS} -o ${EXECC} ${1} -lm

server: ${OBJS}
	${CC} ${CCFLAGS} -o ${EXECS} ${2} -lm

.PHONY: all

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

valText: ${EXEC}
	valgrind --track-origins=yes --leak-check=full --show-leak-kinds=all ./${EXEC} 

valTest: ${TEXEC}
	valgrind --track-origins=yes --leak-check=full ./${TEXEC} 

gdb: ${EXEC}
	gdb ${EXEC}

gdbText: ${EXEC}
	gdb ${EXEC} gettysburg.txt

gdbTest: ${TEXEC}
	gdb ${TEXEC}

clean: 
	rm -f ${EXEC} ${OBJS} ${ZIP} ${TEXEC} ${TOBJS} ${OBJSS} ${OBJSC} ${EXECS} ${EXECC}

zip:
	zip -r ${ZIP} ${ALLFILES}

myftp.o: myftp.c myftp.h
myftpserver.o: myftpserver.c myftp.h