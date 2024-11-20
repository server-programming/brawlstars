#ifndef LOBBY_H
#define LOBBY_H

void lobby(int sd, int client_num);
// 동시 접속자 수를 반환
int get_concurrent_users(int sd);
// 동시 접속자 수를 출력
void print_concurrent_users(int concurrent_users, long long ping);

#endif
