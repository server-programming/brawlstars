CC=gcc

SRC=main.c

OBJ=$(SRC:.c=.o)
EXEC=test

$(EXEC): $(OBJ)
	$(CC) $(OBJ) -o $(EXEC)
%.o: %.c
	$(CC) -c $<

clean:
	rm -f $(OBJ) $(EXEC)
