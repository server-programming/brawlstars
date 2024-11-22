#include "map.h"
#include <ncurses.h>

#define OBSTACLE_CHAR '#' //맵 경계
#define EMPTY_CHAR ' ' //내부 공간

// 맵 배열 정의
// 2차원 배열로 맵을 표현
char map[MAP_HEIGHT][MAP_WIDTH];

// 맵 초기화 함수
void init_map() {
    // 반복문을 통해 초기값을 설정
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            if (y == 0 || y == MAP_HEIGHT - 1 || x == 0 || x == MAP_WIDTH - 1) {
                map[y][x] = OBSTACLE_CHAR; // 맵 경계
            } else {
                map[y][x] = EMPTY_CHAR;  // 내부 공간
            }
        }
    }

   // 내부 장애물 추가
    int num_obstacles = (MAP_HEIGHT * MAP_WIDTH) / 20; // 맵 크기의 5% 정도를 장애물로 채움
    for (int i = 0; i < num_obstacles; i++) {
        int x, y;
        do {
            x = rand() % (MAP_WIDTH - 2) + 1;
            y = rand() % (MAP_HEIGHT - 2) + 1;
        } while (map[y][x] != EMPTY_CHAR);
        map[y][x] = OBSTACLE_CHAR;
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

//장애물 충돌 검사: 외곽선, 장애물을 obstacle로 처리
int is_obstacle(int x, int y) {
    return map[y][x] == OBSTACLE_CHAR;
}