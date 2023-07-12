CC = gcc
CFLAGS = -Wall -Wextra -g -Iinclude
SRC = src/main.c src/lexer.c src/ast.c src/parser.c src/value.c src/eval.c
OBJ = $(SRC:.c=.o)
TARGET = lofy.exe

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	del /Q src\*.o $(TARGET)
