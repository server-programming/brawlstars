#ifndef GAME_H
#define GAME_H

#include <ncurses.h>
#include "player.h"
void draw_game_screen(Player* players, int selected_skin, int sd);
void process_game_input(int sd, Player* player);
void init_game(int sd, int client_num, int selected_skin);
void remove_bullet(int index);
void draw_game_over_screen(); //게임 오버 화면

#endif

