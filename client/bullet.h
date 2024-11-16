#ifndef BULLET_H
#define BULLET_H

// 한 번에 발생할 수 있는 총알 개수
#define MAX_BULLETS 10

// 총알 구조체 정의
typedef struct {
    int x, y; // 총알의 좌표
    int dx, dy; // 총알 이동 방향
} Bullet;

void shoot_bullet(int x, int y, int direction); // 총알 발사
void move_bullets(void); // 발사된 총알 이동
void draw_bullets(void); // 발사된 총알 그리기

#endif
