# FileManager test Makefile
# Author: Lauri Westerholm

SRC = ../src
CC = gcc
CFLAGS = -Wall -pedantic -Wextra -g $(shell pkg-config --cflags gtk+-3.0)
LDFLAGS = -lssh -rdynamic $(shell pkg-config --libs gtk+-3.0)
OBJ = fs.o
EXE = fs_test

.PHONY: clean clean-objects

%.o:	$(SRC)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

fs_test: fs.o test_fs.c
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	$(RM) ${EXE} ${OBJ}

clean-objects:
	$(RM) ${OBJ}
