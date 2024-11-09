#include <ncurses.h>
#include <locale.h>
#include <string.h>
#include "start_menu.h"

void basic_setting() {
	setlocale(LC_ALL, ""); // 로케일 설정
	initscr(); // ncurses 초기화
	cbreak(); // 즉시 입력 모드 설정
	noecho(); // 입력된 문자를 화면에 표시하지 않음
	keypad(stdscr, TRUE); // 특수 키 입력 허용
}

void center_text(int row, int col, const char* text) {
	mvprintw(row, (col - strlen(text)) / 2, "%s", text);
}

void print_menu() {
	const char* menu_items[] = {
		"Brawl Stars",
		"1. Game Start",
		"2. Help",
		"3. Exit"
	};

	for (int i = 0; i < 4; i++) {
		center_text(LINES / 2 - 2 + i, COLS, menu_items[i]);
	}
}

int get_mode() {
	print_menu();
	int selected_mode = 0;
	int ch = getch(); // 사용자 입력 받기
	if (ch == '1') {
		selected_mode = 1;
	} else if (ch == '2') {
		selected_mode = 2;
	} else if (ch == '3') {
		selected_mode = 3;
	} else {
		selected_mode = 0;
	}

	return selected_mode;
}

void start_menu(int* selected_mode) {
	while(1) {
		clear(); // 화면 지우기
		*selected_mode = get_mode();
		char mode_text[50]; // 선택된 모드를 안내하는 문자열
		
		if (*selected_mode == 1) {
			strcpy(mode_text, "Game Start");
		} else if (*selected_mode == 2) {
			strcpy(mode_text, "Help");
		} else if (*selected_mode == 3) {
			strcpy(mode_text, "Exiting...");
		} else {
			strcpy(mode_text, "");
		}

		center_text(LINES / 2 + 4, COLS, mode_text); // 선택된 모드 출력
		refresh(); // 화면 갱신
		sleep(1); // 1초 대기 후 다음 화면으로 넘어감
		
		if ((*selected_mode > 0) && (*selected_mode < 4)) {
			break;
		}
	}
}
