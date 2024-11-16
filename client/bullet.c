#include <ncurses.h>
#include "bullet.h"

// 각 총알 구조체를 관리하기 위한 배열
Bullet bullets[MAX_BULLETS];
int bullet_count = 0; // 발사(생성)되어 있는 총알 개수

// 총알 발사
void shoot_bullet(int x, int y, int direction) {
    if (bullet_count < MAX_BULLETS) { // 최대 총알 개수 제한
        Bullet *b = &bullets[bullet_count++]; // 새 총알 생성
        b->x = x;
        b->y = y;
        b->dx = (direction == 1) - (direction == 3); // 오른쪽 1, 왼쪽 -1
        b->dy = (direction == 2) - (direction == 0); // 아래 1, 위 -1
    }
}

// 발사된 총알 이동
void move_bullets() {
    for (int i = 0; i < bullet_count; i++) {
        bullets[i].x += bullets[i].dx;  // x 좌표 이동
        bullets[i].y += bullets[i].dy;  // y 좌표 이동

        // 화면을 벗어난 총알은 제거
        if (bullets[i].x < 0 || bullets[i].x >= COLS || bullets[i].y < 0 || bullets[i].y >= LINES) {
            for (int j = i; j < bullet_count - 1; j++) {
                bullets[j] = bullets[j + 1]; // 총알 배열에서 총알 삭제
            }
            bullet_count--; // 총알 수 감소
            i--; // 인덱스를 1 감소
        }
    }
}

// 발사된 총알 그리기
void draw_bullets() {
    // 총알 배열에 있는 총알들을 화면에 출력
    for (int i = 0; i < bullet_count; i++) {
        mvaddch(bullets[i].y, bullets[i].x, '*');  // 총알을 '*'로 표시
    }
}
