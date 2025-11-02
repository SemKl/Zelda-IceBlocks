CC = g++

Headers = game.h board.h gamenode.h field.h func.h gamestate.h

CompileParms = -std=c++14 -c -O3 -g -Wall -Wextra

OBJS = main.o game.o board.o gamenode.o field.o func.o gamestate.o

all: iceblocks latexgen

iceblocks: $(OBJS)
	$(CC) $(OBJS) -o iceblocks

main.o: main.cpp $(Headers)
	$(CC) $(CompileParms) main.cpp

game.o: game.cpp $(Headers)
	$(CC) $(CompileParms) game.cpp

board.o: board.cpp $(Headers)
	$(CC) $(CompileParms) board.cpp

gamenode.o: gamenode.cpp $(Headers)
	$(CC) $(CompileParms) gamenode.cpp

field.o: field.cpp $(Headers)
	$(CC) $(CompileParms) field.cpp

func.o: func.cpp $(Headers)
	$(CC) $(CompileParms) func.cpp

gamestate.o: gamestate.cpp $(Headers)
	$(CC) $(CompileParms) gamestate.cpp

latexgen: latexgen.cc
	$(CC) -Wall -Wextra -o latexgen latexgen.cc

clean:
	rm -f iceblocks latexgen *.o
