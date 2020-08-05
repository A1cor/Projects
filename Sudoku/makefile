CC = gcc
OBJS = main.o game.o solve.o stack.o linkedList.o fileParsing.o gurobi.o game_gurobi.o parser.o
EXEC = sudoku-console
COMP_FLAG = -ansi -Wall -Wextra -Werror -pedantic-errors
GUROBI_COMP = -I/usr/local/lib/gurobi563/include
GUROBI_LIB = -L/usr/local/lib/gurobi563/lib -lgurobi56

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(OBJS) $(GUROBI_LIB) -o $@ -lm
main.o: main.c game.h structs.h
	$(CC) $(COMP_FLAG) $(GUROBI_LIB) -c $*.c
game.o: game.c game.h
	$(CC) $(COMP_FLAG) $(GUROBI_LIB) -c $*.c
solve.o: solve.c solve.h
	$(CC) $(COMP_FLAG) $(GUROBI_LIB) -c $*.c
stack.o: stack.c stack.h
	$(CC) $(COMP_FLAG) $(GUROBI_LIB) -c $*.c
linkedList.o: linkedList.c linkedList.h
	$(CC) $(COMP_FLAG) $(GUROBI_LIB) -c $*.c
fileParsing.o: fileParsing.c fileParsing.h
	$(CC) $(COMP_FLAG) $(GUROBI_LIB) -c $*.c
gurobi.o: gurobi.c gurobi.h
	$(CC) $(COMP_FLAG) $(GUROBI_LIB) -c $*.c
game_gurobi.o: game_gurobi.c game_gurobi.h
	$(CC) $(COMP_FLAG) $(GUROBI_LIB) -c $*.c
parser.o: parser.c parser.h
	$(CC) $(COMP_FLAG) $(GUROBI_LIB) -c $*.c

clean:
	rm -f $(OBJS) $(EXEC)
