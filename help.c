#include <ncurses.h>
#include "help.h"

void print_manual() {
	const char* menu_items[] = {
		"WASD: Move",
		"Enter: Shoot",
		"Esc: Exit Game"
	};

	for (int i = 0; i < 3; i++) {
		center_text(LINES / 2 - 2 + i, COLS, menu_items[i]);
	}
}

void help() {
	while(1) {
		clear(); // 화면 지우기
		print_manual();
		refresh();

		int ch = getch(); // 사용자 입력 받기
		
		// ESC(27) 키가 눌리면 메인 메뉴로 돌아가기
		if (ch == 27) { // 
			break;
		}

		sleep(1); // 1초 대기
	}
}
