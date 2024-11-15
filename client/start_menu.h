#ifndef START_MENU_H
#define START_MENU_H

#include <wchar.h>  // wchar_t 지원

// center_text 함수 수정: wchar_t* 타입을 받도록 변경
void center_text(int row, int col, const wchar_t* text);
void print_menu();
int get_mode();
void start_menu(int* selected_mode);

#endif
