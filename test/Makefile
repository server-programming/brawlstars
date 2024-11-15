CC=gcc
CFLAGS=-Wall -g -DNCURSES_WIDECHAR   # 넓은 문자 지원을 위한 플래그 추가
LDFLAGS=-lncursesw                     # ncursesw 라이브러리 추가

SRC=main.c
OBJ=$(SRC:.c=.o)
EXEC=brawlstars

$(EXEC): $(OBJ)
	$(CC) $(OBJ) -o $(EXEC) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(OBJ) $(EXEC)

