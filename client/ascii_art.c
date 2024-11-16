#include <wchar.h>
#include <ncurses.h>
#include "ascii_art.h"

// 마피아 아스키 아트
void mafia_art(int row, int col) {
    const wchar_t* art[] = {
        L"_/﹋\\_",
        L"(҂`_´)",
        L"<,︻╦╤─ ҉ - -",
        L"_/﹋\\"
    };
    for (int i = 0; i < 4; i++) {
        mvaddwstr(row + i, col, art[i]);
    }
}

// 아스키 아트를 출력하는 함수 선택
void print_ascii_art(const wchar_t* type, int row, int col) {
    if (wcscmp(type, L"mafia") == 0) {
        mafia_art(row, col);
    }       
}
