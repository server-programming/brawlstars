#include <stdio.h> 
#include <locale.h> 
#include <ncursesw/ncurses.h>
#include "start_menu.h"
#include "help.h"
#include "game.h"

int main() {
	int selected_mode = 0;
	basic_setting();
	
	while (1) {
		start_menu(&selected_mode);
		if (selected_mode == 1) {
			init_game();
		} else if (selected_mode == 2) {
			help();
		} else if (selected_mode == 3) {
			break;
		}
	}
	endwin(); // ncurses 종료
	return 0;
}
