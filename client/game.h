#ifndef GAME_H
#define GAME_H

#include <ncurses.h>

void init_game(int sd, int client_num, int selected_skin);
void remove_bullet(int index);

#endif

