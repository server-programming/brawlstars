#ifndef GAME_H
#define GAME_H

#include <ncurses.h>

void init_game(int sd, int client_num, int selected_skin);
void remove_bullet(int index);
void draw_game_over_screen(); //게임 오버 화면

#endif

