CC=gcc
CFLAGS=-g -Wall -Werror -std=c11
BINARY=main
OBJ_FILES=boxblur.o

DSYM_FOLDER=$(BINARY).dSYM

RM=rm -rfv

all: $(BINARY)

$(BINARY): main.c boxblur.o
	$(CC) $(CFLAGS) $^ -o $@ 

boxblur.o: boxblur.c
	$(CC) $(CFLAGS) $^ -c 

clean:
	$(RM) $(BINARY) $(DSYM_FOLDER) $(OBJ_FILES)

