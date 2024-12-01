#ifndef LOBBY_H
#define LOBBY_H

int get_skin_choice();
void print_selected_skin();
void print_lobby_status(int concurrent_users, long long ping, int is_matching);
int get_game_mode();
void lobby(int sd, int client_num);


#endif
