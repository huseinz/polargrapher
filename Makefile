
CC=g++

all:
	$(CC) -O2 -o polargrapher polargrapher.cpp -lmuparser -lsfml-graphics -lsfml-window -lsfml-system -lm
