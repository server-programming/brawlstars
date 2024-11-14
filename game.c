#include <ncurses.h>
#include "game.h"

// 플레이어 초기 위치
#define PLAYER_CHAR '🚖'

// 총알 구조체
typedef struct {
	int x, y;
	int direction; // 0: 위, 1: 오른쪽, 2: 아래, 3: 왼쪽
} Bullet;

typedef struct {
	int x, y;
} player_loc;

#define MAX_BULLETS 999
Bullet bullets[MAX_BULLETS]; // 최대 999개의 총알
int shot_count = 0; // 발사된 총알 수

void init_game(int sd) {
	int x = COLS / 2;
	int y = LINES / 2;
	int ch;
	int player_dir = 0;
	player_loc *pl = (player_loc *)malloc(sizeof(player_loc));

	while(1) {
		clear(); // 화면 지우기
			 //
		draw_player(x, y); // 플레이어 그리기
		move_bullets(); // 총알 이동 처리
		
		refresh(); // 화면 갱신
		
		ch = getch(); // 사용자 입력 받기
		
		// 플레이어 이동 처리
		if (move_player(&x, &y, ch, &player_dir)) {
			pl->x = x;
			pl->y = y;
			if (send(sd, pl, sizeof(player_loc), 0) == -1) {
				perror("player send");
				exit(1);
			}
		}

		// 총알 발사 처리
		if (ch == '\n') {
			shoot_bullet(x, y, player_dir);
		}
		
		if (ch == 'q') {
			break;
		}

		napms(10); // 10ms 대기
	}
}

// 플레이어 그리기
void draw_player(int x, int y) {
	mvaddch(y, x, PLAYER_CHAR); // 플레이어 그리기
}

// 플레이어 이동
int move_player(int* x, int* y, int ch, int* direction) {
	if (ch == 'w') (*y)--, *direction = 0; // 위로
	if (ch == 's') (*y)++, *direction = 2; // 아래로
	if (ch == 'a') (*x)--, *direction = 3; // 왼쪽으로
	if (ch == 'd') (*x)++, *direction = 1; // 오른쪽으로
	
	return 1;
}

// 총알 발사
void shoot_bullet(int x, int y, int direction) {
	if (shot_count < MAX_BULLETS) {
		bullets[shot_count].x = x;
		bullets[shot_count].y = y;
		bullets[shot_count].direction = direction;
		shot_count++;
	}
}

// 총알 이동
void move_bullets() {
	int i = 0;
	for (i = 0; i < shot_count; i++) {
		if (bullets[i].direction == 0) { // 위로
			bullets[i].y--;
		} else if (bullets[i].direction == 1) { // 오른쪽으로
			bullets[i].x++;
		} else if (bullets[i].direction == 2) { // 아래로
			bullets[i].y++;
		} else if (bullets[i].direction == 3) { // 왼쪽으로
			bullets[i].x--;
		}
	}

	// 총알이 화면을 벗어나면 삭제
	if (bullets[i].x < 0 || bullets[i].x >= COLS || bullets[i].y < 0 || bullets[i].y >= LINES) {
		remove_bullet(i);
		i--; // 삭제된 총알 뒤로 밀기
	} else {
		draw_bullet(bullets[i].x, bullets[i].y); // 총알 그리기
	}
}

// 총알 그리기
void draw_bullet(int x, int y) {
	mvaddch(y, x, '*'); // 총알을'*'로 표시
}

// 총알 삭제
void remove_bullet(int index) {
	for (int i = index; i < shot_count - 1; i++) {
		bullets[i] = bullets[i + 1];
	}
	shot_count--; // 총알 수 감소
}
