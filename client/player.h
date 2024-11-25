#ifndef PLAYER_H
#define PLAYER_H

typedef struct {
    int x, y; // 플레이어 위치
    int dir; // 플레이어 방향
    wchar_t* skin; // 플레이어 스킨
    int hp; // 플레이어 체력
    int is_dead; // 사망 여부
    int id; // 플레이어 번호
    int rank; // 플레이어 체력
} Player;

Player* init_player(wchar_t *skin, int x, int y); // 플레이어 초기화
void draw_player(Player* player); // 플레이어 그리기
void move_player(Player* player, int ch); // 플레이어 이동

#endif
