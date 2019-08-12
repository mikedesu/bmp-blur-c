CC=gcc
CFLAGS=-g -Wall -Werror -std=c11
BINARY=main

DSYM_FOLDER=$(BINARY).dSYM

RM=rm -rfv

all: $(BINARY)

$(BINARY): main.c
	$(CC) $(CFLAGS) $^ -o $@ 

clean:
	$(RM) $(BINARY) $(DSYM_FOLDER)

