#include <ncurses.h>
#include <wchar.h>
#include <string.h>
#include <sys/socket.h>
#include "bullet.h"
#include "map.h"

// 각 총알 구조체를 관리하기 위한 배열
Bullet local_bullets[MAX_LOCAL_BULLETS];
int local_bullet_count = 0; // 자신이 발사한 총알 개수

Bullet remote_bullets[MAX_REMOTE_BULLETS];
int remote_bullet_count = 0; // 서버로부터 받은 총알 개수

// 총알의 충돌을 확인
int is_bullet_collision(int x, int y) {
    if (is_bullet_blocked(x, y)) { // 총알이 벽에 가로막혔을 경우
        return 1; // 1을 반환
    }
    return 0; // 총알의 충돌이 없었을 경우 0을 반환
}

// 플레이어의 크기를 고려한 총알 발사
void shoot_bullet(int x, int y, int direction, wchar_t *player_shape, int ch) {
    if ((ch == '\n') && (local_bullet_count < MAX_LOCAL_BULLETS)) {
        Bullet *b = &local_bullets[local_bullet_count++];

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

// 총알 이동 및 충돌 처리
void move_bullets(int player_x, int player_y, wchar_t *player_shape, int sd) {
    char bullet_buffer[1024] = ""; // 서버로 전송할 총알 정보
    int buffer_pos = 0;

    // 로컬 총알 처리
    for (int i = 0; i < local_bullet_count; i++) {
        int new_x = local_bullets[i].x + local_bullets[i].dx;
        int new_y = local_bullets[i].y + local_bullets[i].dy;

        if (is_bullet_collision(new_x, new_y) || new_x < 0 || new_x >= COLS || new_y < 0 || new_y >= LINES) {
            // 충돌 시 총알 제거
            for (int j = i; j < local_bullet_count - 1; j++) {
                local_bullets[j] = local_bullets[j + 1];
            }
            local_bullet_count--;
            i--;
        } else {
            // 총알 이동
            local_bullets[i].x = new_x;
            local_bullets[i].y = new_y;

            // 서버로 전송할 데이터 추가
            buffer_pos += snprintf(
                bullet_buffer + buffer_pos, sizeof(bullet_buffer) - buffer_pos,
                "<<bullet>>x=%d,y=%d,dx=%d,dy=%d\n",
                local_bullets[i].x, local_bullets[i].y, local_bullets[i].dx, local_bullets[i].dy
            );
        }
    }

    // 서버로 전송
    if (strlen(bullet_buffer) > 0) {
        if (send(sd, bullet_buffer, strlen(bullet_buffer), 0) == -1) {
            perror("send bullets to server");
        }
    }

    // 원격 총알 처리
    for (int i = 0; i < remote_bullet_count; i++) {
        int new_x = remote_bullets[i].x + remote_bullets[i].dx;
        int new_y = remote_bullets[i].y + remote_bullets[i].dy;

        if (is_bullet_collision(new_x, new_y) || new_x < 0 || new_x >= COLS || new_y < 0 || new_y >= LINES) {
            // 충돌 시 총알 제거
            for (int j = i; j < remote_bullet_count - 1; j++) {
                remote_bullets[j] = remote_bullets[j + 1];
            }
            remote_bullet_count--;
            i--;
        } else {
            // 총알 이동
            remote_bullets[i].x = new_x;
            remote_bullets[i].y = new_y;
        }
    }
}

// 발사된 총알 그리기
void draw_bullets() {
    // 로컬 총알 그리기
    for (int i = 0; i < local_bullet_count; i++) {
        mvaddch(local_bullets[i].y, local_bullets[i].x, '*');
    }

    // 원격 총알 그리기
    for (int i = 0; i < remote_bullet_count; i++) {
        mvaddch(remote_bullets[i].y, remote_bullets[i].x, '*');
    }
}

