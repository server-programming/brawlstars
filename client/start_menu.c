#include <ncurses.h>
#include <wchar.h>  
#include <unistd.h>
#include <string.h>
#include "start_menu.h"
#include "text_align.h"
#include "ascii_art.h"

// 메뉴 출력 함수
void print_menu() {
    const wchar_t* menu_items[] = {
        L"브롤스타즈",  // 유니코드 문자열
        L"1. 게임 시작",
        L"2. 도움말",
        L"3. 게임 종료"
    };

    for (int i = 0; i < 4; i++) {
        center_text(LINES / 2 - 2 + i, COLS, menu_items[i]);
    }
}

// 모드 선택 함수
int get_mode() {
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

// 시작 메뉴 함수
void start_menu(int* selected_mode) {
    while(1) {
        clear(); // 화면 지우기

        print_ascii_art(L"mafia", 6, 4);  // 두 번째 아트 1개: 왼쪽 상단
        print_ascii_art(L"mafia", 1, 12);  // 두 번째 아트 2개: 왼쪽 상단 바로 아래

        // 메뉴 출력
        print_menu();

        // 선택된 모드를 안내하는 유니코드 문자열
        wchar_t mode_text[50]; 

        *selected_mode = get_mode(); // 사용자 모드 입력 받기

        if (*selected_mode == 1) {
            wcscpy(mode_text, L"게임 시작");
        } else if (*selected_mode == 2) {
            wcscpy(mode_text, L"도움말");
        } else if (*selected_mode == 3) {
            wcscpy(mode_text, L"종료 중...");
        } else {
            wcscpy(mode_text, L"");
        }

        center_text(LINES / 2 + 4, COLS, mode_text); // 선택된 모드 출력
        refresh(); // 화면 갱신
        sleep(1); // 1초 대기 후 다음 화면으로 넘어감

        if ((*selected_mode > 0) && (*selected_mode < 4)) {
            break;
        }
    }
}

