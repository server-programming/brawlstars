CC=gcc
CFLAGS=-Wall -g -DNCURSES_WIDECHAR   # 넓은 문자 지원을 위한 플래그 추가
LDFLAGS=-lncursesw -lSDL2 -lSDL2_mixer

SRC=main.c start_menu.c game.c help.c player_shape.c map.c background_music.c text_align.c ascii_art.c player.c bullet.c basic_setting.c lobby.c ping_time.c request.c network_connect.c

OBJ=$(SRC:.c=.o)
EXEC=brawlstars

$(EXEC): $(OBJ)
	$(CC) player.o game.o $(filter-out player.o game.o,$(OBJ)) -o $(EXEC) $(LDFLAGS)

player.o: player.c player.h
	$(CC) $(CFLAGS) -c player.c

game.o: game.c game.h player.h
	$(CC) $(CFLAGS) -c game.c

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(OBJ) $(EXEC)

