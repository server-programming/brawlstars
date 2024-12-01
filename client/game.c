// 기본 모듈
#include <ncurses.h> // 게임 츨력
#include <stdlib.h>
#include <string.h>
// 시간 측정
#include <time.h>
// send/recv
#include <sys/socket.h>
// player 구조체, move_player, draw_player, draw_player_hp
#include "player.h"
// draw_map()
#include "map.h"
// bullet 구조체, move_bullet, draw_bullet, shoot_bullet
#include "bullet.h"
// player skin 구조체 정의
#include "player_shape.h"
// game.c 함수 프로토타입 정의
#include "game.h"

#define MAX_PLAYERS 4
#define KEY_INTERVAL 1 
// 색상 정의
#define BLUE_COLOR "\x1b[34m"
#define RESET_COLOR "\x1b[0m"

void draw_game_screen(Player* players, int selected_skin, int sd) {
    draw_map(); // 맵 그리기
    draw_player_hp(&players[0]);
    draw_players(players, MAX_PLAYERS, selected_skin, sd);
    draw_bullets(sd, players);
}

void process_game_input(int sd, Player* player) {
    static clock_t last_key_time = 0;  // 마지막 총알 발사 시간 추적
    clock_t current_time = clock();  // 현재 시간

    char ch = getch();
    move_player(player, ch);
    if (ch == 't') {
	    player->hp -= 1;
    }

    // 총알 발사 간격 확인
    if ((current_time - last_key_time) * 1000 / CLOCKS_PER_SEC >= KEY_INTERVAL) {
        shoot_bullet(player->x, player->y, player->dir, player->skin, ch);
        last_key_time = current_time;
    }
}

int send_player_dead(int sd) {
	char buf[1024] = "";
<<<<<<< HEAD
	int buf_pos = 0;

	buf = snprintf(buf, sizeof(buf), "PLAYER_IS_DEAD");
=======

	snprintf(buf, sizeof(buf), "PLAYER_IS_DEAD");
>>>>>>> 43ac2fd (총알 한발씩 나갈 수 있도록 수정 완료)
	if (send(sd, buf, strlen(buf), 0) == -1) {
		perror("send bullets to server");
		return 1;
	}
	return 0;
}

// 게임 초기화 및 루프
void init_game(int sd, int client_num, int selected_skin) {
    // 플레이어 정보 배열
    Player players[MAX_PLAYERS]; // 최대 4명의 플레이어를 관리할 수 있음
    init_players(players, MAX_PLAYERS, selected_skin); // 플레이어들 초기화    
    Player* player = &players[0];    
    init_map(); // 맵 초기화

    while (1) { // 게임 루프
        clear(); // 화면 지우기

        process_game_input(sd, player);       
        draw_game_screen(players, selected_skin, sd);
        if (player->hp <= 0){
	    send_player_dead(sd); // 플레이어 죽음 철;?
            draw_game_over_screen(); // 게임 오버 스크린 띄움
	    napms(30);
            break;
        }
        
        // 화면 업데이트
        refresh();
        napms(30); 
    }

    // lobby(sd, client_num); // 게임 루프 종료 시 로비 화면으로 돌아감
    clear();
    refresh();
    napms(1000);
}


void draw_game_over_screen() {
    clear();  // 사망 시 화면을 완전히 지움
    
    const char *game_over_art[] = {
        "                                                   ",
        "                                                   ",
        " ####   ##   #    # ####    ###  ##   # #### ####  ",
        "###     ##   ##  ## ##     ##  # ##   # ##   ##  # ",
        "##     # ##  ###### ##     ##  # ##  ## ##   ##  # ",
        "##  #  # ##  ###### ####   ##  #  ## #  #### ####  ",
        "##  # ###### # # ## ##     ##  #  ####  ##   ## #  ",
        "### # #   ## # # ## ##     ##  #   ##   ##   ##  # ",
        " #### #   ## #   ## ####    ###    ##   #### ##  # ",
        "                                                   ",
        "                                                   ",
        "",
        "Press 'r' to return to lobby",
        NULL
    };
    
    //여기쯤 게임 오버 화면에 플레이어 순위를 띄우면 좋을 것 같아요. -> 추후 구현

    int start_y = (LINES - 13) / 2;
    for (int i = 0; game_over_art[i] != NULL; i++) {
        mvprintw(start_y + i, (COLS - strlen(game_over_art[i])) / 2, "%s", game_over_art[i]);
    }

    refresh();  // 화면 갱신

    // 'r' 키 입력을 기다림
    int ch;
    while ((ch = getch()) != 'r') {
        // 'r' 키가 아닌 다른 키 입력은 무시
    }
}
