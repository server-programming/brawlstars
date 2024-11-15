#ifndef START_MENU_H
#define START_MENU_H

#include <wchar.h>  // wchar_t 지원

void basic_setting();
void center_text(int row, int col, const wchar_t* text);
void print_menu();
int get_mode();
void start_menu(int* selected_mode);

#endif
