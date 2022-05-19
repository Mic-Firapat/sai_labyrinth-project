CC = gcc
OPTIONS = -W -Wall -pedantic -O3
NOM_EXE = executable
all : main.c
	$(CC) $(OPTIONS) main.c -o $(NOM_EXE) -lglut -lGLU -lGL -lm

clean :
	rm executable

.PHONY : clean all