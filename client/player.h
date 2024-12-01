#ifndef PLAYER_H
#define PLAYER_H

typedef struct {
    int x, y; // 플레이어 위치
    int dir; // 플레이어 방향
    wchar_t* skin; // 플레이어 스킨
    int hp; // 플레이어 체력
    int is_dead; // 사망 여부
    int id; // 플레이어 번호
    int rank; // 플레이어 체력
	int enemy; // 적의 수

    int is_local; //플레이어 색상 구분 추가
} Player;

Player* init_player(int x, int y, int dir, wchar_t* skin, int is_local);void init_players(Player players[], int players_num, int selected_skin);
void draw_player(Player* player); // 플레이어 그리기
void draw_players(Player players[], int players_num, int selected_skin, int sd);
void move_player(Player* player, int ch); // 플레이어 이동
void update_player_info(Player* player, int x, int y, int dir, int skin_index, int hp, int is_dead, int rank, int is_local, int enemy);
void player_hit(Player* player);
void draw_player_hp(Player* player); //플레이어 hp 표시

#endif
