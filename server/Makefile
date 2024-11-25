CC=gcc

SRC=main.c network_connect.c connect_to_client.c

OBJ=$(SRC:.c=.o)
EXEC=server

$(EXEC): $(OBJ)
	$(CC) $(OBJ) -o $(EXEC)
%.o: %.c
	$(CC) -c $<

clean:
	rm -f $(OBJ) $(EXEC)
