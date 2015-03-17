
CC=g++

all:
	$(CC) -o polargrapher polargrapher.cpp -lmuparser -lsfml-graphics -lsfml-window -lsfml-system -lm
