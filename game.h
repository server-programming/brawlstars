#ifndef GAME_H
#define GAME_H
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 게임 초기화 함수
void init_game(); 

// 플레이어 그리기 함수
void draw_player(int x, int y);

// 플레이어 이동 함수
void move_player(int* x, int* y, int ch, int* direction);

// 총알 발사 함수
void shoot_bullet(int x, int y, int direction);

// 총알 이동 함수
void move_bullets();

// 총알 그리기 함수
void draw_bullet(int x, int y);

// 총알 삭제 함수
void remove_bullet(int index);

#endif // GAME_H
