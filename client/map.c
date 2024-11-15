#include "map.h"
#include <ncurses.h>

// 맵 배열 정의
// 2차원 배열로 맵을 표현
char map[MAP_HEIGHT][MAP_WIDTH];

// 맵 초기화 함수
void init_map() {
    // 반복문을 통해 초기값을 설정
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            if (y == 0 || y == MAP_HEIGHT - 1 || x == 0 || x == MAP_WIDTH - 1) {
                map[y][x] = '#';  // 맵 경계
            } else {
                map[y][x] = ' ';  // 내부 공간
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
            // 맵의 각 요소를 (start_ y, start_x) 기준으로 화면에 출력
            mvaddch(start_y + y, start_x + x, map[y][x]);
        }
    }

    refresh(); // 화면 갱신
}

