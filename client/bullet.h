#ifndef BULLET_H
#define BULLET_H

#include "player.h"

// 한 번에 발생할 수 있는 총알 개수
#define MAX_LOCAL_BULLETS 1
#define MAX_REMOTE_BULLETS 1000

// 총알 구조체 정의
typedef struct {
    int x, y;  // 총알의 좌표
    int dx, dy; // 총알 이동 방향
    int is_active; // 총알의 비활성화 상태
} Bullet;

// 총알 발사 함수
void shoot_bullet(int x, int y, int direction, wchar_t* player_shape, int ch); 

// 총알 이동 함수
void move_bullets(Player* player, int sd); 

// 총알 그리기 함수
void draw_bullets(int sd, Player players[]); 

// 총알 이동 & 그리기
void update_bullets(Player players[]);

// 총알 충돌 검사 함수
int is_bullet_collision(int x, int y); 

// 서버와의 총알 송수신 관련 함수
int send_local_bullets(int sd); // 로컬 총알을 서버로 전송
int recv_remote_bullets(int sd); // 서버로부터 원격 총알 수신
#endif

