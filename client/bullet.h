#ifndef BULLET_H
#define BULLET_H

#include "player.h"

// 한 번에 발생할 수 있는 총알 개수
#define MAX_LOCAL_BULLETS 10
#define MAX_REMOTE_BULLETS 1000
// 총알 구조체 정의
typedef struct {
    int x, y; // 총알의 좌표
    int dx, dy; // 총알 이동 방향
} Bullet;

void shoot_bullet(int x, int y, int direction, wchar_t* player_shape, int ch); // 총알 발사
// void move_bullets(int player_x, int player_y, wchar_t* player_shape, int sd); // 발사된 총알 이동
void move_bullets(Player* player, int sd); //수정된 move_bullets
void draw_bullets(void); // 발사된 총알 그리기
int is_bullet_collision(int x, int y); // 총알 충돌 검사

#endif

