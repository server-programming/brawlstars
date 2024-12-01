#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MATCHING_NUM 4

typedef struct player {
        int x;
        int y;
        int skin;
	int hp;
	int is_dead;
} player;

typedef struct bullet {
	int x, y, dx, dy;
} bullet;

typedef struct bullet_array {
	struct ullet *bullet_info;
} bullet_array;

typedef struct network_player {
	int *ns;
	struct player *players;
	int cur_client;
	int *room_index;
	struct bullet_array *bullets;
} network_player;

typedef struct room_info {
	int client_id[MATCHING_NUM];
	int is_empty;
	int player_num;
} room_info;

int connect_to_client(int ns, int cur_client_num, char *buf, int flag);

int recv_send_game_data(network_player *np, char *buf, int cur_client_num, room_info *room, int player_in_room) {
	char buf1[1024];
	char player_pos[1024];
	int room_num = np->room_index[cur_client_num];
	int client_x, client_y, client_skin, client_hp, client_is_dead;
	sscanf(buf, "ACCESS_TO_GAME,x=%d,y=%d,skin=%d,hp=%d,is_dead=%d\n",
		&client_x, &client_y, &client_skin, &client_hp, &client_is_dead);

	np->players[cur_client_num].x = client_x;
	np->players[cur_client_num].y = client_y;
	np->players[cur_client_num].skin = client_skin;
	np->players[cur_client_num].hp = client_hp;
	np->players[cur_client_num].is_dead = client_is_dead;

	memset(player_pos, '\0', sizeof(player_pos));
	for(int i=0; i<MATCHING_NUM; i++) {
		memset(buf1, '\0', sizeof(buf1));
		if (room->client_id[i] != cur_client_num) {
			if (np->ns[room->client_id[i]] > 0) {
				sprintf(buf1, "%d,x=%d,y=%d,skin=%d,hp=%d,is_dead=%d,enemy=%d\n",
					room->client_id[i],
					np->players[room->client_id[i]].x,
					np->players[room->client_id[i]].y,
					np->players[room->client_id[i]].skin,
					np->players[room->client_id[i]].hp,
					np->players[room->client_id[i]].is_dead,
					room->player_num);
			} else {
				sprintf(buf1, "%d,x=-10,y=-10,skin=0,hp=0,is_dead=0,enemy=%d\n", room->client_id[i], room->player_num);
			}
		} else {
			sprintf(buf1, "x=%d,y=%d,skin=%d,hp=%d,is_dead=%d,enemy=%d\n",
					np->players[room->client_id[i]].x,
					np->players[room->client_id[i]].y,
					np->players[room->client_id[i]].skin,
					np->players[room->client_id[i]].hp,
					np->players[room->client_id[i]].is_dead,
					room->player_num);
		}

		strncat(player_pos, buf1, strlen(buf1));
	}

	printf("player pos\n%s\n", player_pos);

	if (connect_to_client(np->ns[cur_client_num], cur_client_num, player_pos, 2) == 0) {
		return 0;
	}

	return 1;
}
