CC = gcc
CFLAGS = -Wall -Wextra -g

SRC = main.c input.c parser.c executor.c builtins.c redirection.c
TARGET = mini-shell

all:
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)