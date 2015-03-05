
CC=g++

polargraphermake: polar_grapher.cpp
	$(CC) -o polar_grapher polar_grapher.cpp -lmuparser -lsfml-graphics -lsfml-window -lsfml-system -lm
