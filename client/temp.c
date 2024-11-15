#include <ncurses.h>
#include <locale.h>
#include <stdlib.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include "game.h"
#include "player_shape.h"  // player_shape.h 추가
#include "map.h"  // map.h 추가
#include "background_music.h"

// 최대 총알 수 설정
#define MAX_BULLETS 10

typedef struct {
    int x, y;
} player_loc;

typedef struct {
    int x, y;
    int dx, dy;  // x, y 방향
} Bullet;

Bullet bullets[MAX_BULLETS];  // 총알 배열
int bullet_count = 0;  // 총알 개수

// 총알 발사 함수
void fire_bullet(int x, int y, int direction) {
    if (bullet_count < MAX_BULLETS) {
        // 총알 초기화
        Bullet *b = &bullets[bullet_count++];
        b->x = x;
        b->y = y;

        // 방향에 따라 총알의 dx, dy 설정
        if (direction == 0) {  // 위쪽
            b->dx = 0;
            b->dy = -1;
        } else if (direction == 2) {  // 아래쪽
            b->dx = 0;
            b->dy = 1;
        } else if (direction == 1) {  // 오른쪽
            b->dx = 1;
            b->dy = 0;
        } else if (direction == 3) {  // 왼쪽
            b->dx = -1;
            b->dy = 0;
        }
    }
}

// 총알 이동 처리 함수
void move_bullets() {
    for (int i = 0; i < bullet_count; i++) {
        Bullet *b = &bullets[i];
        b->x += b->dx;
        b->y += b->dy;

        // 화면 밖으로 나갔을 경우, 총알 제거
        if (b->x < 0 || b->x >= COLS || b->y < 0 || b->y >= LINES) {
            for (int j = i; j < bullet_count - 1; j++) {
                bullets[j] = bullets[j + 1];  // 배열 이동
            }
            bullet_count--;  // 총알 수 감소
            i--;  // 현재 인덱스 다시 확인
        }
    }
}

// 총알 그리기 함수
void draw_bullets() {
    for (int i = 0; i < bullet_count; i++) {
        Bullet *b = &bullets[i];
        mvaddch(b->y, b->x, '*');  // 총알을 '*'로 그리기
    }
}

// 플레이어 이동 처리 함수
int move_player(int* x, int* y, int ch, int* direction) {
    if (ch == 'w') (*y)--, *direction = 0; // 위로
    if (ch == 's') (*y)++, *direction = 2; // 아래로
    if (ch == 'a') (*x)--, *direction = 3; // 왼쪽으로
    if (ch == 'd') (*x)++, *direction = 1; // 오른쪽으로

    return 1;
}

// 플레이어 그리기 함수
void draw_player(int x, int y, wchar_t* player_shape) {
    mvaddwstr(y, x, player_shape); // 유니코드 아스키 아트를 화면에 출력
}

// 게임 초기화 함수
void init_game(int sd) {
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        printf("SDL_Init failed: %s\n", SDL_GetError());
        return;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("Mix_OpenAudio failed: %s\n", Mix_GetError());
        return;
    }

    play_background_music("../audio_files/planet_brainwave.mp3");

    int x = COLS / 2;
    int y = LINES / 2;
    int ch;
    int player_dir = 0;
    player_loc *pl = (player_loc *)malloc(sizeof(player_loc));
    char buf[256];

    // 랜덤한 플레이어 모양 가져오기
    PlayerShape* player_shapes = get_player_shape();
    int current_shape = 0;

    // 맵 초기화
    init_map();

    while(1) {
        clear(); // 화면 지우기

        // 맵 그리기
        draw_map();  // 맵 그리기 함수 호출

        // 플레이어 그리기
        draw_player(x, y, player_shapes->shapes[current_shape]);
        
        // 총알 이동 및 그리기
        move_bullets();
        draw_bullets();

        refresh(); // 화면 갱신

        ch = getch(); // 사용자 입력 받기

        // 플레이어 이동 처리
        if (move_player(&x, &y, ch, &player_dir)) {
            sprintf(buf, "%d,%d", x, y);

            if (send(sd, buf, sizeof(buf), 0) == -1) {
                perror("player send");
                exit(1);
            }
        }

        // 플레이어 모양 변경
        if (ch == 'c') {  // 'c' 키를 눌러서 플레이어 모양을 변경
            current_shape = (current_shape + 1) % MAX_SHAPES;  // 다음 모양으로 변경
        }

        // 방향키를 눌렀을 때 총알 발사
        if (ch == KEY_UP) {
            fire_bullet(x, y - 1, 0);  // 위쪽 방향으로 총알 발사
        } else if (ch == KEY_DOWN) {
            fire_bullet(x, y + 1, 2);  // 아래쪽 방향으로 총알 발사
        } else if (ch == KEY_RIGHT) {
            fire_bullet(x + 1, y, 1);  // 오른쪽 방향으로 총알 발사
        } else if (ch == KEY_LEFT) {
            fire_bullet(x - 1, y, 3);  // 왼쪽 방향으로 총알 발사
        }

        if (ch == 'q') {
            break;
        }

        napms(10); // 10ms 대기
    }
    // stop_background_music();
    // SDL_Quit();
}
