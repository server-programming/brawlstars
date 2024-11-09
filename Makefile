CC=gcc
CFLAGS=-Wall -g
LDFLAGS=-lncurses # ncurses 라이브러리 추기

SRC=main.c start_menu.c game.c help.c
OBJ = $(SRC:.c=.o)
EXEC=brawlstars

$(EXEC): $(OBJ)
	$(CC) $(OBJ) -o $(EXEC) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(OBJ) $(EXEC)
