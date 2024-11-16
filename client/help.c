#include <unistd.h>
#include <ncurses.h>
#include <wchar.h>
#include "help.h"
#include "text_align.h"
// 게임의 기본 조작법을 화면 중앙에 출력
void print_manual() {
    // 메뉴 항목 정의
	const wchar_t* menu_items[] = {
		L"WASD: Move", 
		L"Enter: Shoot",
		L"Esc: Exit Game"
	};
    
    // 메뉴 항목을 한 줄씩 화면 중앙에 출력
	for (int i = 0; i < 3; i++) {
		center_text(LINES / 2 - 2 + i, COLS, menu_items[i]);
	}
}

// 도움말 화면을 표시하며, ESC 키를 누를 때까지 댁 
void help() {
	while(1) {
		clear(); // 화면 지우기
		print_manual(); // 매뉴얼 출력
		refresh(); // 화면 갱신

		int ch = getch(); // 사용자 입력 받기
		
		// ESC(27) 키가 눌리면 메인 메뉴로 돌아가기
		if (ch == 27) { // 
			break;
		}

		sleep(1); // 1초 대기
	}
}
