CC=gcc
CFLAGS=-Wall -g -DNCURSES_WIDECHAR   # 넓은 문자 지원을 위한 플래그 추가
LDFLAGS=-lncursesw -lSDL2 -lSDL2_mixer

SRC=main.c start_menu.c game.c help.c player_shape.c map.c background_music.c text_align.c ascii_art.c
OBJ=$(SRC:.c=.o)
EXEC=brawlstars

$(EXEC): $(OBJ)
	$(CC) $(OBJ) -o $(EXEC) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(OBJ) $(EXEC)

