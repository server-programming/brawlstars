#include <ncurses.h>
#include <wchar.h>
#include <string.h>
#include <sys/socket.h>  // send/recv
#include "bullet.h"
#include "map.h"
#include "player.h"

// 색상 정의
#define BLUE_COLOR "\x1b[34m"
#define RESET_COLOR "\x1b[0m"

// 각 총알 구조체를 관리하기 위한 배열
Bullet local_bullets[MAX_LOCAL_BULLETS];
int local_bullet_count = 0; // 자신이 발사한 총알 개수

Bullet remote_bullets[MAX_REMOTE_BULLETS];
int remote_bullet_count = 0; // 서버로부터 받은 총알 개수

// 총알 이동 및 충돌 확인 함수들
int is_bullet_collision(int x, int y) {
    if (is_bullet_blocked(x, y)) {
        return 1; // 충돌
    }
    return 0; // 충돌 없음
}

void move_bullet(Bullet* b) {
    b->x += b->dx;
    b->y += b->dy;
}

int is_player_hit(int bullet_x, int bullet_y, Player* player) {
    int player_width = wcslen(player->skin);
    if (bullet_y == player->y && bullet_x >= player->x && bullet_x < player->x + player_width) {
        player->hp--;
        return 1; // 충돌
    }

    return 0; // 충돌 없음
}

// 로컬 총알 발사 처리
void shoot_bullet(int x, int y, int direction, wchar_t *player_shape, int ch) {
    if ((ch == '\n') && (local_bullet_count < MAX_LOCAL_BULLETS)) {
        Bullet *b = &local_bullets[local_bullet_count++];
        int player_length = wcslen(player_shape);

        // 방향에 따라 총알의 시작 위치 조정
        switch (direction) {
            case 0: b->x = x + player_length / 2; b->y = y - 1; break; // 위
            case 1: b->x = x + player_length; b->y = y; break; // 오른쪽
            case 2: b->x = x + player_length / 2; b->y = y + 1; break; // 아래
            case 3: b->x = x - 1; b->y = y; break; // 왼쪽
        }

        b->dx = (direction == 1) - (direction == 3);
        b->dy = (direction == 2) - (direction == 0);
        b->is_active = 1; // 총알을 활성화 상태로 설정
    }
}

// 총알 전송 함수
int send_local_bullets(int sd) {
    char buf[1024] = "";
    int buf_pos = 0;

    // 로컬 총알 처리
    for (int i = 0; i < local_bullet_count; i++) {
        Bullet* b = &local_bullets[i];
        if (!b->is_active) continue; // 비활성화된 총알은 무시

        if (is_bullet_collision(b->x, b->y)) {
            b->is_active = 0;  // 충돌 시 총알 비활성화
        } else {
            buf_pos += snprintf(
                buf + buf_pos, sizeof(buf) - buf_pos,
                "LOCAL_BULLET_INFO,x=%d,y=%d,dx=%d,dy=%d,is_active=%d\n",
                b->x, b->y, b->dx, b->dy, b->is_active
            );
        }
    }

    // 서버에 아무것도 보내지 않는 것을 방지하기 위한 코드
    if (strlen(buf) == 0) {
        snprintf(buf, sizeof(buf), "LOCAL_BULLET_INFO\n");
    }

    // 서버로 전송
    if (strlen(buf) > 0) {
        if (send(sd, buf, strlen(buf), 0) == -1) {
            perror("send bullets to server");
            return 1;
        }
    }
    return 0;
}

// 서버로부터 총알 정보 수신
int recv_remote_bullets(int sd) {
    char buf[1024];
    if (recv(sd, buf, sizeof(buf), 0) == -1) {
        perror("recv bullets from server");
        return 1;
    }

    // 서버에서 받은 총알 정보 처리
    char* line = strtok(buf, "\n");
    while (line != NULL) {
        int id, x, y, dx, dy, is_active;
        if (sscanf(line, "%d,x=%d,y=%d,dx=%d,dy=%d,is_active=%d", &id, &x, &y, &dx, &dy, &is_active) == 6) {
            Bullet* b = &remote_bullets[remote_bullet_count++];
            b->x = x;
            b->y = y;
            b->dx = dx;
            b->dy = dy;
            b->is_active = is_active;
        }
        line = strtok(NULL, "\n");
    }
    return 0;
}

// 총알 그리기
void draw_bullets(int sd, Player players[]) {

    // 서버로 총알 정보를 전송
    if (send_local_bullets(sd) != 0) {
        return;
    }

    // 서버로부터 총알 정보를 수신
    if (recv_remote_bullets(sd) != 0) {
        return;
    }

    update_bullets(players);

    // 로컬 총알 그리기
    for (int i = 0; i < local_bullet_count; i++) {
        if (local_bullets[i].is_active) {
            mvaddch(local_bullets[i].y, local_bullets[i].x, '*');
        }
    }

    // 원격 총알 그리기
    for (int i = 0; i < remote_bullet_count; i++) {
        if (remote_bullets[i].is_active) {
            mvaddch(remote_bullets[i].y, remote_bullets[i].x, '*');
        }
    }
}

void update_bullets(Player players[]) {
    // 로컬 총알 이동 및 충돌 처리
    for (int i = 0; i < local_bullet_count; i++) {
        Bullet* b = &local_bullets[i];
        if (!b->is_active) continue; // 비활성화된 총알은 무시

        move_bullet(b);

        // 로컬 총알 충돌 체크
        if (is_bullet_collision(b->x, b->y)) {
            b->is_active = 0;  // 충돌 시 총알 비활성화
        }

        for (int j = 0; j < 4; j++) {
            if (is_player_hit(b->x, b->y, &players[j])) {
                b->is_active = 0;  // 플레이어와 충돌 시 총알 비활성화
            }
        }
    }

    // 서버로부터 받은 원격 총알 처리
    for (int i = 0; i < remote_bullet_count; i++) {
        Bullet* b = &remote_bullets[i];
        if (!b->is_active) continue; // 비활성화된 총알은 무시

        move_bullet(b);

        // 원격 총알 충돌 체크
        if (is_bullet_collision(b->x, b->y)) {
            b->is_active = 0;  // 충돌 시 원격 총알 비활성화
        }

        for (int j = 0; j < 4; j++) {
            if (is_player_hit(b->x, b->y, &players[j])) {
                b->is_active = 0;  // 플레이어와 충돌 시 원격 총알 비활성화
            }
        }
    }
}
