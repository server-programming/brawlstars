// 기본 모듈
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

// ncurse와 다국어 지원
#include <ncursesw/ncurses.h>
#include <locale.h> 

// 네트워크 통신 모듈
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>

// 사용자 정의 모듈
#include "basic_setting.h"
#include "start_menu.h"
#include "lobby.h"
#include "game.h"
#include "help.h"

// 서버와 통신할 포트 번호 설정
#define PORTNUM 12312
// 타임아웃 시간(초)
#define TIMEOUT_SEC 1

typedef struct {
	struct sockaddr_in sin;
	int sd;
	int client_num;
} network;

network network_connect();


int main() {

	network nt;

	int selected_mode = 0; // 선택된 메뉴 모드
	basic_setting(); // 프로그램 기본 설정 (ncurses 초기화 등)

	nt = network_connect();
	
	while (1) {
		start_menu(&selected_mode); // 시작 메뉴 표시 및 모드 선택
		if (selected_mode == 1) { // 게임 시작
			lobby(nt.sd, nt.client_num);
            // init_game(sd, client_num);
		} else if (selected_mode == 2) { // 도움말 표시
			help();
		} else if (selected_mode == 3) { // 종료
			break;
		}
	}
	endwin(); // ncurses 종료
	
	close(nt.sd); // 소켓 닫기

	return 0;
}
