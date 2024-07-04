.SUFFIXES: .c .o

CC = gcc
FLAGS = -Wall -Werror -std=gnu11 -pedantic -g

EXEC = project2
SRCS := main.c queue.c pcb.c io.c schedulers.c
OBJDIR := bin
OBJS := $(addprefix $(OBJDIR)/, $(SRCS:.c=.o))

TEST_ARGS := -alg RR -input data/input-1.txt -quant 50

default:${EXEC}

$(OBJDIR)/%.o: %.c
	$(CC) $(FLAGS) -c -o $@ $<

$(OBJS): | $(OBJDIR)

$(OBJDIR):
	mkdir $(OBJDIR)

clean:
	rm -f ${EXEC}
	rm -rf bin

run: ${EXEC}
	./${EXEC}

test: $(EXEC)
	./$(EXEC) $(TEST_ARGS)

${EXEC}:${OBJS}
	${CC} ${FLAGS} -o ${EXEC} ${OBJS}

#.c.o:
#	${CC} ${FLAGS} -c $<

vpath %.c src
vpath %.h src
