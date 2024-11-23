#include <ncurses.h>
#include <wchar.h>
#include "bullet.h"
#include "map.h"

// 각 총알 구조체를 관리하기 위한 배열
Bullet bullets[MAX_BULLETS];
int bullet_count = 0; // 발사(생성)되어 있는 총알 개수

// 총알의 충돌을 확인
int is_bullet_collision(int x, int y) {
    if (is_bullet_blocked(x, y)) { // 총알이 벽에 가로막혔을 경우
        return 1; // 1을 반환
    }
    return 0; // 총알의 충돌이 없었을 경우 0을 반환
}


// 플레이어의 크기를 고려한 총알 발사
void shoot_bullet(int x, int y, int direction, wchar_t *player_shape) {
    if (bullet_count < MAX_BULLETS) {
        Bullet *b = &bullets[bullet_count++];
        
        // 플레이어 모양의 길이 계산
        int player_length = wcslen(player_shape);
        
        // 방향에 따라 총알의 시작 위치 조정
        switch (direction) {
            case 0: // 위
                b->x = x + player_length / 2;
                b->y = y - 1;
                break;
            case 1: // 오른쪽
                b->x = x + player_length;
                b->y = y;
                break;
            case 2: // 아래
                b->x = x + player_length / 2;
                b->y = y + 1;
                break;
            case 3: // 왼쪽
                b->x = x - 1;
                b->y = y;
                break;
        }
        
        b->dx = (direction == 1) - (direction == 3);
        b->dy = (direction == 2) - (direction == 0);
    }
}

void move_bullets(int player_x, int player_y, wchar_t *player_shape) {
    for (int i = 0; i < bullet_count; i++) {
        int new_x = bullets[i].x + bullets[i].dx;
        int new_y = bullets[i].y + bullets[i].dy;

        int collision = is_bullet_collision(new_x, new_y);

        if (collision == 1) { // 장애물 충돌
            for (int j = i; j < bullet_count - 1; j++) {
                bullets[j] = bullets[j + 1];
            }
            bullet_count--;
            i--;
        } else if (new_x < 0 || new_x >= COLS || new_y < 0 || new_y >= LINES) {
            // 화면 밖으로 나간 총알 제거
            for (int j = i; j < bullet_count - 1; j++) {
                bullets[j] = bullets[j + 1];
            }
            bullet_count--;
            i--;
        } else {
            // 총알 이동
            bullets[i].x = new_x;
            bullets[i].y = new_y;
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
