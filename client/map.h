#ifndef MAP_H
#define MAP_H

#include <ncurses.h>

// 맵의 너비와 높이
#define MAP_WIDTH 120
#define MAP_HEIGHT 40

void init_map(); // 맵 초기화
void draw_map(); // 맵을 화면에 출력
void clear_map(); // 맵을 화면에서 삭제

#endif
