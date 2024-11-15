#ifndef GAME_H
#define GAME_H

#include <ncurses.h>

void init_game(int sd, int client_num);
void draw_player(int x, int y, wchar_t* player_shape);  // 플레이어 모양을 인자로 받도록 변경
void move_player(int* x, int* y, int ch, int* direction);
void shoot_bullet(int x, int y, int direction);
void move_bullets();
void draw_bullet(int x, int y);
void remove_bullet(int index);

#endif

