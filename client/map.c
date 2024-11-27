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

    // 고정 장애물 추가
    // 중앙 장애물
    for (int y = 8; y < 13; y++) {
        for (int x = 27; x < 34; x++) {
            map[y][x] = OBSTACLE_CHAR;
        }
    }

    // 코너 장애물
    for (int i = 2; i < 6; i++) {
        map[i][i] = OBSTACLE_CHAR;
        map[i][MAP_WIDTH-1-i] = OBSTACLE_CHAR;
        map[MAP_HEIGHT-1-i][i] = OBSTACLE_CHAR;
        map[MAP_HEIGHT-1-i][MAP_WIDTH-1-i] = OBSTACLE_CHAR;
    }

    // 측면 장애물
    for (int y = 5; y < MAP_HEIGHT - 5; y++) {
        map[y][10] = OBSTACLE_CHAR;
        map[y][MAP_WIDTH-11] = OBSTACLE_CHAR;
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

// 플레이어-맵 충돌 확인 함수
int is_player_blocked(int x, int y, wchar_t* player_shape) {
    int length = wcslen(player_shape); // 플레이어 모양 길이 계산

    // 화면 좌표를 맵 좌표로 변환
    int map_x = x - (COLS - MAP_WIDTH) / 2;
    int map_y = y - (LINES - MAP_HEIGHT) / 2;

    // 플레이어가 차지하는 가로 영역 확인
    for (int i = 0; i < length; i++) {
        int check_x = map_x + i;

        // 맵 경계 초과 확인
        if (check_x < 0 || check_x >= MAP_WIDTH || map_y < 0 || map_y >= MAP_HEIGHT) {
            return 1; // 경계를 벗어나면 장애물로 간주
        }

        // 장애물 확인
        if (map[map_y][check_x] == OBSTACLE_CHAR) {
            return 1; // 장애물 발견
        }
    }

    return 0; // 충돌 없음
}

// 총알-맵 충돌 확인 함수
int is_bullet_blocked(int x, int y) {
    // 화면 좌표를 맵 좌표로 변환
    int map_x = x - (COLS - MAP_WIDTH) / 2;
    int map_y = y - (LINES - MAP_HEIGHT) / 2;

    // 맵 경계 초과 확인
    if (map_x < 0 || map_x >= MAP_WIDTH || map_y < 0 || map_y >= MAP_HEIGHT) {
        return 1; // 경계를 벗어나면 충돌로 간주
    }

    // 장애물 확인
    if (map[map_y][map_x] == OBSTACLE_CHAR) {
        return 1; // 장애물이 있으면 충돌
    }

    return 0; // 충돌 없음
}

