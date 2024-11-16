#include <ncurses.h>
#include <locale.h>
#include <stdlib.h>
#include <time.h>
#include <sys/socket.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include "game.h"
#include "player_shape.h"
#include "map.h"
#include "background_music.h"

// 플레이어와 총알 관련 정의
#define MAX_BULLETS 999
#define PLAYER_CHAR 'O'

typedef struct {
    int x, y;
    int dx, dy;
} Bullet;

typedef struct {
    int x, y;
} player_loc;

Bullet bullets[MAX_BULLETS];
int bullet_count = 0;

// 총알 발사
void shoot_bullet(int x, int y, int direction) {
    if (bullet_count < MAX_BULLETS) {
        Bullet *b = &bullets[bullet_count++];
        b->x = x;
        b->y = y;
        b->dx = (direction == 1) - (direction == 3); // 오른쪽 1, 왼쪽 -1
        b->dy = (direction == 2) - (direction == 0); // 아래 1, 위 -1
    }
}

// 총알 이동
void move_bullets() {
    for (int i = 0; i < bullet_count; i++) {
        bullets[i].x += bullets[i].dx;
        bullets[i].y += bullets[i].dy;

        if (bullets[i].x < 0 || bullets[i].x >= COLS || bullets[i].y < 0 || bullets[i].y >= LINES) {
            for (int j = i; j < bullet_count - 1; j++) {
                bullets[j] = bullets[j + 1];
            }
            bullet_count--;
            i--;
        }
    }
}

// 총알 그리기
void draw_bullets() {
    for (int i = 0; i < bullet_count; i++) {
        mvaddch(bullets[i].y, bullets[i].x, '*');
    }
}

// 플레이어 그리기
void draw_player(int x, int y, wchar_t *player_shape) {
    mvaddwstr(y, x, player_shape);
}

// 플레이어 이동
void move_player(int *x, int *y, int ch, int *direction) {
    if (ch == 'w') (*y)--, *direction = 0; // 위로
    if (ch == 's') (*y)++, *direction = 2; // 아래로
    if (ch == 'a') (*x)--, *direction = 3; // 왼쪽으로
    if (ch == 'd') (*x)++, *direction = 1; // 오른쪽으로
}

// 게임 초기화 및 루프
void init_game(int sd, int client_num) {
    int x = COLS / 2, y = LINES / 2, player_dir;
    char buf[256], player_pos[1024];
    char ch;
    player_loc all_players[4];
    int id, x1, y1;

    initscr();
    cbreak();
    noecho();
    curs_set(0);
    nodelay(stdscr, TRUE);
    keypad(stdscr, TRUE);

    // 오디오 초기화
    if (SDL_Init(SDL_INIT_AUDIO) < 0 || Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        perror("SDL or Mix_OpenAudio failed");
        return;
    }
    play_background_music("../audio_files/planet_brainwave.mp3");

    PlayerShape *player_shapes = get_player_shape();
    int current_shape = 0;

    init_map();

    while (1) {
        clear();

        draw_map();
        draw_player(x, y, player_shapes->shapes[current_shape]);
        move_bullets();
        draw_bullets();

        refresh();

        ch = getch();

        // 플레이어 이동 및 방향 설정
        move_player(&x, &y, ch, &player_dir);

        // 서버에 위치 전달
        snprintf(buf, sizeof(buf), "x=%d,y=%d", x, y);
        if (send(sd, buf, sizeof(buf), 0) == -1) {
            perror("send to server");
            break;
        }

        // 서버에서 위치 정보 받기
        memset(player_pos, 0, sizeof(player_pos));
        if (recv(sd, player_pos, sizeof(player_pos), 0) == -1) {
            perror("recv from server");
            break;
        }

        // 다른 플레이어 그리기
        char *line = strtok(player_pos, "\n");
        while (line) {
            if (sscanf(line, "%d,x=%d,y=%d", &id, &x1, &y1) == 3) {
                draw_player(x1, y1, player_shapes->shapes[current_shape]);
            }
            line = strtok(NULL, "\n");
        }

        // 총알 발사
        if (ch == '\n') {
            shoot_bullet(x, y, player_dir);
        }

        // 플레이어 모양 변경 (디버깅용)
        // if (ch == 'c') {
        //    current_shape = (current_shape + 1) % MAX_SHAPES;
        // }

        // if (ch == 'q') break;

        napms(10);
    }

    stop_background_music();
    SDL_Quit();
    endwin();
}

