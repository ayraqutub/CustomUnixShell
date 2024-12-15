CC = gcc
CFLAGS = -Wall -g

SRC = builtin.c clock.c dragonshell.c pipe.c

OBJ = $(SRC:.c=.o)

EXEC = dragonshell

$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(EXEC)

compile: $(SRC)
	$(CC) $(CFLAGS) -c $(SRC)

clean:
	rm -f $(OBJ) $(EXEC)

all: $(TARGET)
