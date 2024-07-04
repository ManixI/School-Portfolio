.SUFFIXES: .c .o

CC = gcc
FLAGS = -Wall -Werror -std=gnu11 -pedantic -g

EXEC = project4
SRCS := main.c 
OBJDIR := bin
OBJS := $(addprefix $(OBJDIR)/, $(SRCS:.c=.o))

TEST_ARGS := MIN 10 data/pageref-1.txt

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
