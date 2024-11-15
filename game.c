#include <ncurses.h>
#include "game.h"

// 플레이어 초기 위치
#define PLAYER_CHAR 'O'

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

void init_game(int sd, int client_num) {
	int x = COLS / 2;
	int y = LINES / 2;
	int ch;
	int player_dir = 0;
	char buf[50];
	char *line;
	char player_pos[1024];
	player_loc all_players[4];
	int num_players = 0;
	int id, x1, y1;

	initscr(); // ncurses 초기화
	cbreak(); // 즉시 입력 모드
	noecho(); // 입력값 출력하지 않음
	curs_set(0); // 커서 숨기기
	nodelay(stdscr, TRUE); // 비차단 모드 설정
	keypad(stdscr, TRUE); // 특수 키 입력 활성화

	// 3-- 서버에게 게임을 시작함을 전달하는 부분
	memset(buf, '\0', sizeof(buf));
	sprintf(buf, "client is starting game", client_num);
	if (send(sd, buf, sizeof(buf), 0) == -1) {
		perror("send to server --3");
	}

	while(1) {
		clear(); // 화면 지우기
			
		draw_player(x, y); // 플레이어 그리기

		// 4-- 클라이언트는 무조건 서버로 위치 정보를 보낸다
		memset(buf, '\0', sizeof(buf));
		sprintf(buf, "x=%d,y=%d", x, y);
		if (send(sd, buf, sizeof(buf), 0) == -1) {
			perror("send to server --4");
			exit(1);
		}

		// 5-- 클라이언트는 무조건 서버로부터 위치 정보들을 받는다
		memset(player_pos, '\0', sizeof(player_pos));
		if (recv(sd, player_pos, sizeof(player_pos), 0) == -1) {
			perror("recv from server --5");
			exit(1);
		}

		// 모든 클라이언트들 그리기
		line = strtok(player_pos, "\n");
		while(line != NULL) {
			if (sscanf(line, "%d,x=%d,y=%d", &id, &x1, &y1) == 3) {
				draw_player(x1, y1);
			}
			line = strtok(NULL, "\n");
		}

		move_bullets(); // 총알 이동 처리
		
		refresh(); // 화면 갱신
		
		ch = getch(); // 사용자 입력 받기
		
		if (ch != ERR) { // 입력이 없으면 넘어감

			// 플레이어 이동 처리
			move_player(&x, &y, ch, &player_dir);

			// 총알 발사 처리
			if (ch == '\n') {
				shoot_bullet(x, y, player_dir);
			}

			// 메뉴 화면으로 
			if (ch == 'q') {
				break;
			}
		}

		napms(10); // 10ms 대기
	}
}

// 플레이어 그리기
void draw_player(int x, int y) {
	mvaddch(y, x, PLAYER_CHAR); // 플레이어 그리기
}

// 플레이어 이동
void move_player(int* x, int* y, int ch, int* direction) {
	if (ch == 'w') (*y)--, *direction = 0; // 위로
	if (ch == 's') (*y)++, *direction = 2; // 아래로
	if (ch == 'a') (*x)--, *direction = 3;
	if (ch == 'd') (*x)++, *direction = 1;
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
