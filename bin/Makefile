# FileManager Makefile
# Author: Lauri Westerholm

SRC = ../src
INC = ../include
CC = gcc
CFLAGS = -Wall -pedantic -Wextra -g $(shell pkg-config --cflags gtk+-3.0)
LDFLAGS = -lssh -rdynamic $(shell pkg-config --libs gtk+-3.0)
OBJ = ssh.o UI.o str_messages.o fs.o assets.o
EXE = FileManager

.PHONY: run clean clean-objects

all:	$(EXE)

%.o:	$(SRC)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

FileManager:	$(OBJ) $(SRC)/main.c
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	$(RM) ${EXE} ${OBJ}

clean-objects:
	$(RM) ${OBJ}

run: ${EXE}
	./$<

leak-check: main
	valgrind ./$< --leak-check=yes
