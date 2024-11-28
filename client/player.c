// player.c
#include <stdlib.h>
#include <ncurses.h>
#include <wchar.h>
// player.c 함수 프로토타입 선언
#include "player.h"
// is_player_blocked (플레이어가 벽에 충돌하였는가 여부 반환)
#include "map.h"
// 추후 수정 예정
#include "bullet.h"

// 색상 정의
#define BLUE_COLOR "\x1b[34m"
#define RESET_COLOR "\x1b[0m"

// 플레이어 초기화 함수
Player* init_player(wchar_t* skin, int x, int y) {
    Player* new_player = (Player*)malloc(sizeof(Player));
    new_player->x = x;
    new_player->y = y;
    new_player->dir = 1;
    new_player->skin = skin;
    new_player->hp = 3;
    new_player->is_dead = 0;
    new_player->rank = -1;

    return new_player;
}

// 플레이어 그리기 함수
// void draw_player(Player* player) {
//     mvaddwstr(player->y, player->x, player->skin); // 플레이어는 정해진 모양(스킨)으로 표시
// }

//색상 구분된 플레이어 그리기 함수
void draw_player(Player* player) {
    if (player->is_local) {
        
        mvaddwstr(player->y, player->x, player->skin);
        // mvprintw(player->y, player->x, "%s%ls%s", BLUE_COLOR, player->skin, RESET_COLOR);
    } else {
        
        mvaddwstr(player->y, player->x, player->skin);
    }
}

// 플레이어 이동 함수
void move_player(Player* player, int ch) {
    int old_x = player->x, old_y = player->y;

    if (ch == 'w') { player->y--; player->dir = 0; } // 위로
    if (ch == 's') { player->y++; player->dir = 2; } // 아래로
    if (ch == 'a') { player->x--; player->dir = 3; } // 왼쪽으로
    if (ch == 'd') { player->x++; player->dir = 1; } // 오른쪽으로

    if (is_player_blocked(player->x, player->y, player->skin)) {
        player->x = old_x;
        player->y = old_y;
    } else {
        // play_move_sound();
    }
}

void player_hit(Player* player) {
    player->hp--;
    if (player->hp <= 0) {
        player->is_dead = 1;
        // 사망 화면 전환
    }
}

void draw_player_hp(Player* player) {
    mvprintw(0, 0, "HP: %d", player->hp);
}
