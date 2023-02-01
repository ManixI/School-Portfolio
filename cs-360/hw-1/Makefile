.SUFFIXES: .c .o .zip .txt
CC = gcc
CCFLAGS =  -pedantic -Wall -g
EXEC = wordpairs
TEXEC = TableTest
ZIP = wordpairs.zip
OBJS = wordPairs.o hashTable.o crc64.o getWord.o
TOBJS = hashTable.o hashTableTest.o crc64.o 
ALLFILES = wordPairs.c wordPairs.h hashTable.c hashTable.h crc64.c crc64.h hashTableTest.c getWord.c getWord.h README.txt Makefile gettysburg.txt

${EXEC}: ${OBJS}
	${CC} ${CCFLAGS} -o ${EXEC} ${OBJS} -lm

${TEXEC}: ${TOBJS}
	${CC} ${CCFLAGS} -o ${TEXEC} ${TOBJS} -lm

.c.o:
	${CC} ${CCFLAGS} -c $<

run: ${EXEC}
	./${EXEC}

runText: ${EXEC}
	./${EXEC} gettysburg.txt

test: ${TEXEC}
	./${TEXEC}

val: ${EXEC}
	valgrind --track-origins=yes ./${EXEC}

valText: ${EXEC}
	valgrind --track-origins=yes --leak-check=full --show-leak-kinds=all ./${EXEC} 10 gettysburg.txt

valTest: ${TEXEC}
	valgrind --track-origins=yes --leak-check=full ./${TEXEC} 

gdb: ${EXEC}
	gdb ${EXEC}

gdbText: ${EXEC}
	gdb ${EXEC} gettysburg.txt

gdbTest: ${TEXEC}
	gdb ${TEXEC}

clean: 
	rm -f ${EXEC} ${OBJS} ${ZIP} ${TEXEC} ${TOBJS}

zip:
	zip -r ${ZIP} ${ALLFILES}

wordPairs.o: wordPairs.c wordPairs.h
hashTable.o: hashTable.c hashTable.h
hashTableTest.o: hashTableTest.c
crc64.o: crc64.c crc64.h
getWord.o: getWord.c getWord.h
