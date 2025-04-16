CC=gcc
CFLAGS=-W -Wall -Wextra -g
LIBS=

BUILD_FOLDER=build
SRC_FOLDER=src

lexer: 	$(SRC_FOLDER)/lexer.c $(SRC_FOLDER)/lexer.h
	$(CC) $(CFLAGS) $(LIBS) $< -o $(BUILD_FOLDER)/$@

%.o: src/%.c
	$(CC) $(CLFAGS) -c $<  -o $(BUILD_FOLDER)/$@
