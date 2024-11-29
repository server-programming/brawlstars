#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include <wchar.h>
#include <sys/socket.h>
// player.c 함수 프로토타입 선언
#include "player.h"
// is_player_blocked (플레이어가 벽에 충돌하였는가 여부 반환)
#include "map.h"
// 추후 수정 예정
#include "bullet.h"
#include "player_shape.h"

// 색상 정의
#define BLUE_COLOR "\x1b[34m"
#define RESET_COLOR "\x1b[0m"

// 플레이어 초기화 함수
Player* init_player(int x, int y, int dir, wchar_t* skin, int is_local) {
    Player* new_player = (Player*)malloc(sizeof(Player));
    new_player->x = x; // 플레이어 x 좌표
    new_player->y = y; // 플레이어 y 좌표
    new_player->dir = 1; // 플레이어 방향
    new_player->skin = skin; // 플레이어 스킨
    new_player->hp = 3; // 플레이어 체력
    new_player->is_dead = 0; // 플레이어 사망 여부
    new_player->rank = -1; // 플레이어 체력
    new_player->is_local = is_local;

    return new_player;
}

// 플레이어들을 초기화하는 함수
void init_players(Player players[], int players_num, int selected_skin) {
    // 플레이어의 모양(스킨)을 가져옴
    PlayerShape* player_shape = get_player_shape();

    // 첫 번째 플레이어 초기화 (자기 자신)
    players[0] = *init_player(COLS / 2, LINES / 2, 1, player_shape->shapes[selected_skin], 1);

    // 다른 플레이어들 초기화 (player[1] ~ player[3]) -> 좌표는 (-1, -1)
    for (int i = 1; i < players_num; i++) {
        players[i] = *init_player(-1, -1, 1, player_shape->shapes[selected_skin], 0);  // 초기 위치 (-1, -1), 로컬이 아님
    }
}

// 서버에 플레이어 정보 전송
int send_player_info(int sd, Player* player, int selected_skin) {
    char buf[1024];
    // 서버에 플레이어 위치 및 상태 전달
    snprintf(buf, sizeof(buf), "ACCESS_TO_GAME,x=%d,y=%d,skin=%d,hp=%d,is_dead=%d", player->x, player->y, selected_skin, player->hp, player->is_dead);

    if (send(sd, buf, sizeof(buf), 0) == -1) {
        perror("send to server");
        return 1;
    }
    return 0;
}

// 서버에서 다른 플레이어 정보 수신
int recv_other_players_info(int sd, Player players[]) {
    char buf[1024];
    memset(buf, 0, sizeof(buf));  // 수신하기 전에 버퍼 초기화

    if (recv(sd, buf, sizeof(buf), 0) == -1) {
        perror("recv from server");
        return 1;
    }

    // 서버로부터 받은 정보로 다른 플레이어 그리기
    char* line = strtok(buf, "\n");  // line 변수는 여기에 선언되어야 함

    int index = 1;
    while (line != NULL) {
        int id, x1, y1, skin_index, hp, is_dead;
        if (sscanf(line, "x=%d,y=%d,skin=%d,hp=%d,is_dead=%d", &x1, &y1, &skin_index, &hp, &is_dead) == 5) {
            update_player_info(&players[0], x1, y1, players[0].dir, skin_index, hp, is_dead, -1, 1);
        }
        else if (sscanf(line, "%d,x=%d,y=%d,skin=%d,hp=%d,is_dead=%d", &id, &x1, &y1, &skin_index, &hp, &is_dead) == 6) {
            Player* other_player = &players[index]; // 해당 플레이어의  정보 업데이트
            update_player_info(other_player, x1, y1, other_player->dir, skin_index, hp, is_dead, -1, 0);
            index += 1;
        }
        line = strtok(NULL, "\n");  // strtok 호출을 반복
    }
    return 0;
}

void update_player_info(Player* player, int x, int y, int dir, int skin_index, int hp, int is_dead, int rank, int is_local) {
    // 플레이어의 모양(스킨)을 가져옴
    PlayerShape* player_shape = get_player_shape();

    player->x = x; // 플레이어 x 좌표
    player->y = y; // 플레이어 y 좌표
    player->dir = dir; // 플레이어 방향
    player->skin = player_shape->shapes[skin_index]; // 플레이어 스킨
    player->hp = hp; // 플레이어 체력
    player->is_dead = is_dead; // 플레이어 사망 여부
    player->rank = rank; // 플레이어 순위
    player->is_local = is_local; // 플레이어 색상?
}

// 색상 구분된 플레이어 그리기 함수
void draw_player(Player* player) {   
    if (player->is_local) {
        mvaddwstr(player->y, player->x, player->skin);
    } else {
        mvaddwstr(player->y, player->x, player->skin);
    }
}

void draw_players(Player players[], int players_num, int selected_skin, int sd) {
    if (send_player_info(sd, &players[0], selected_skin) == 1) {
        return;
    }
    if (recv_other_players_info(sd, players) == 1) {
        return;
    }
    for (int i = 0; i < players_num; i++) {
        draw_player(&players[i]);
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

