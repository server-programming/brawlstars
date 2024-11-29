#include <ncurses.h>
#include <wchar.h>
#include <string.h>
// send/recv
#include <sys/socket.h>
// bullet.c 함수 프로토타입 정의
#include "bullet.h"
// is_bullet_blocked
#include "map.h"
// player 구조체
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

// 플레이어 피격 처리 함수
int is_player_hit(int bullet_x, int bullet_y, int player_x, int player_y, wchar_t *player_shape) {
    int player_width = wcslen(player_shape);
    return (bullet_y == player_y && bullet_x >= player_x && bullet_x < player_x + player_width);
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
    }
}

// 총알 전송 함수
int send_local_bullets(int sd) {
    char buf[1024] = "";
    int buf_pos = 0;

    // 로컬 총알 처리
    for (int i = 0; i < local_bullet_count; i++) {
        Bullet* b = &local_bullets[i];
        if (is_bullet_collision(b->x, b->y)) {
            // 충돌 시 총알 제거
            for (int j = i; j < local_bullet_count - 1; j++) {
                local_bullets[j] = local_bullets[j + 1];
            }
            local_bullet_count--;
            i--;
        } else {
            buf_pos += snprintf(
                buf + buf_pos, sizeof(buf) - buf_pos,
                "LOCAL_BULLET_INFO,x=%d,y=%d,dx=%d,dy=%d\n",
                b->x, b->y, b->dx, b->dy
            );
        }
    }
    
    // 서버에 아무것도 보내지 않는 것을 방지하기 위한 코드    
    if (strlen(buf) == 0) {
        snprintf(buf, sizeof(buf), "LOCAL_BULLET_INFO\n");
    }        

    // 서버로 전송 (서버 단 구현 시 주석 해제)
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
        int id, x, y, dx, dy;
        if (sscanf(line, "%d,x=%d,y=%d,dx=%d,dy=%d", &id, &x, &y, &dx, &dy) == 5) {
            Bullet* b = &remote_bullets[remote_bullet_count++];
            b->x = x;
            b->y = y;
            b->dx = dx;
            b->dy = dy;
        }
        line = strtok(NULL, "\n");
    }
    return 0;
}

// 총알 그리기
void draw_bullets(int sd) {
    
    // 서버로 총알 정보를 전송
    if (send_local_bullets(sd) != 0) {
        return;
    }    
        
    // 서버로부터 총알 정보를 수신
    if (recv_remote_bullets(sd) != 0) {
        return;
    }
    
    update_bullets();
    // 로컬 총알 그리기
    for (int i = 0; i < local_bullet_count; i++) {
        mvaddch(local_bullets[i].y, local_bullets[i].x, '*');
    }
    
    // 원격 총알 그리기
    for (int i = 0; i < remote_bullet_count; i++) {
        mvaddch(remote_bullets[i].y, remote_bullets[i].x, '*');
    }
}

// 총알 이동
void update_bullets() {
    // 로컬 총알 이동
    for (int i = 0; i < local_bullet_count; i++) {
        move_bullet(&local_bullets[i]);
    }
    // 서버로부터 받은 원격 총알 이동
    for (int i = 0; i < remote_bullet_count; i++) { 
        move_bullet(&remote_bullets[i]);
    }
}

// 임의의 총알 생성 (디버그 용)
void debug_bullets(char ch, int sd) {
    char buf[10000];
    int len = 0; // 현재 버퍼에 저장된 총알의 개수
    
    // x가 1부터 120까지 dy=-1인 총알 생성
    for (int x = 1; x <= 120; x++) {
        // 총알 데이터 포맷팅
        len += snprintf(buf + len, sizeof(buf) - len, "1,x=%d,y=5,dx=0,dy=1\n", x);
    }

    // 'c' 키가 눌리면 buf에 있는 총알 데이터를 remote_bullets 배열에 추가
    if (ch == 'c') {
        // buf를 줄 단위로 나누어서 처리
        char* line = strtok(buf, "\n");
        while (line != NULL) {
            int id, x, y, dx, dy;
            // 총알 데이터 파싱
            if (sscanf(line, "%d,x=%d,y=%d,dx=%d,dy=%d", &id, &x, &y, &dx, &dy) == 5) {
                // remote_bullets 배열에 새로운 총알 추가
                Bullet* b = &remote_bullets[remote_bullet_count++];
                b->x = x;
                b->y = y;
                b->dx = dx;
                b->dy = dy;
            }
            line = strtok(NULL, "\n");  // 다음 줄로 이동
        }
    }
}

