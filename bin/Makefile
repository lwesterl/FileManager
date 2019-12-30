# FileManager Makefile
# Author: Lauri Westerholm

SRC = ../src
INC = ../include
CC = gcc
CFLAGS = -Wall -pedantic -Wextra -g $(shell pkg-config --cflags gtk+-3.0)
LDFLAGS = -lssh $(shell pkg-config --libs gtk+-3.0)
OBJ = ssh.o UI.o
EXE = main

.PHONY: run clean clean-objects

all:	$(EXE)

%.o:	$(SRC)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

main:	$(OBJ) $(SRC)/main.c
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	$(RM) ${EXE} ${OBJ}

clean-objects:
	$(RM) ${OBJ}

run: main
	./$<

leak-check: main
	valgrind ./$< --leak-check=yes
