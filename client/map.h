#ifndef MAP_H
#define MAP_H

#include <ncurses.h>
#include <wchar.h>

// 맵의 너비와 높이
#define MAP_WIDTH 120
#define MAP_HEIGHT 40

void init_map(); // 맵 초기화
void draw_map(); // 맵을 화면에 출력
void clear_map(); // 맵을 화면에서 삭제
int is_player_blocked(int x, int y, wchar_t* player_shape); // 플레이어-벽 충돌 여부 확인
int is_bullet_blocked(int x, int y); // 총알-벽 충돌 여부 확인
#endif
