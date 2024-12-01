#include <ncurses.h>
#include <wchar.h>
#include "text_align.h"

// 텍스트 중앙 정렬 출력
// row: 출력할 행, col: 출력할 열, text: 출력할 텍스트
void center_text(int row, int col, const wchar_t* text) {
    mvaddwstr(row, (col - wcslen(text)) / 2, text);
}
