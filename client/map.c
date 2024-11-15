#include "map.h"
#include <ncurses.h>

// 맵 배열 정의
char map[MAP_HEIGHT][MAP_WIDTH];

// 맵 초기화 함수
void init_map() {
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            if (y == 0 || y == MAP_HEIGHT - 1 || x == 0 || x == MAP_WIDTH - 1) {
                map[y][x] = '#';  // 벽
            } else {
                map[y][x] = ' ';  // 공터
            }
        }
    }
}

// 맵 그리기 함수
void draw_map() {
    // 화면 크기 계산
    int start_y = (LINES - MAP_HEIGHT) / 2;  // 세로 중앙 시작 위치
    int start_x = (COLS - MAP_WIDTH) / 2;    // 가로 중앙 시작 위치

    // 맵 그리기
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            mvaddch(start_y + y, start_x + x, map[y][x]);
        }
    }

    refresh(); // 화면 갱신
}

