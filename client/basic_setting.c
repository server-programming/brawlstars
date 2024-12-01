#include <locale.h>
#include <ncurses.h>
#include "basic_setting.h"

// 기본 설정
void basic_setting() {
    setlocale(LC_ALL, ""); // 로케일 설정
    initscr(); // ncurses 초기화
    cbreak(); // 즉시 입력 모드 설정
    noecho(); // 입력된 문자를 화면에 표시하지 않음
    curs_set(0); // 커서 숨기기
    nodelay(stdscr, TRUE); // getch()가 블로킹되지 않도록 설정
    keypad(stdscr, TRUE); // 특수 키 처리 활성화
    setlocale(LC_CTYPE, ""); // 문자셋 설정
}
