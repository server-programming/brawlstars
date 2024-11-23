#ifndef PLAYER_H
#define PLAYER_H

typedef struct {
    int x, y; // 플레이어 좌표
} player_loc;

typedef struct {
    player_loc ploc;
    int hp;
} player;

void draw_player(int x, int y, wchar_t *player_shape); // 플레이어 그리기
void move_player(int *x, int *y, int ch, int *direction, wchar_t *player_shape); // 플레이어 이동

#endif
