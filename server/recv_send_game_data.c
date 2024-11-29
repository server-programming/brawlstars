#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PLAYER 4

typedef struct player {
        int x;
        int y;
        int skin;
	int hp;
	int is_dead;
} player;

typedef struct network_player {
	int *ns;
	struct player *players;
	int cur_client;
} network_player;

int connect_to_client(int ns, int cur_client_num, char *buf, int flag);

int recv_send_game_data(network_player *np, char *buf, int cur_client_num) {
	char buf1[1024];
	char player_pos[1024];
	int client_x, client_y, client_skin, client_hp, client_is_dead;
	sscanf(buf, "ACCESS_TO_GAME,x=%d,y=%d,skin=%d,hp=%d,is_dead=%d\n",
		&client_x, &client_y, &client_skin, &client_hp, &client_is_dead);

	np->players[cur_client_num].x = client_x;
	np->players[cur_client_num].y = client_y;
	np->players[cur_client_num].skin = client_skin;
	np->players[cur_client_num].hp = client_hp;
	np->players[cur_client_num].is_dead = client_is_dead;

	memset(player_pos, '\0', sizeof(player_pos));
	for(int i=0; i<PLAYER; i++) {

		if (i != cur_client_num) {
			memset(buf1, '\0', sizeof(buf1));
			if (np->ns[i] > 0) {
				sprintf(buf1, "%d,x=%d,y=%d,skin=%d,hp=%d,is_dead=%d\n",
					i, np->players[i].x, np->players[i].y, np->players[i].skin,
					np->players[i].hp, np->players[i].is_dead);
				//strcat(player_pos, buf);
			} else {
				sprintf(buf1, "%d,x=-10,y=-10,skin=0,hp=0,is_dead=0\n", i);
				printf("else i %d\n", i);
			}
		} else {
			memset(buf1, '\0', sizeof(buf1));
			sprintf(buf1, "x=%d,y=%d,skin=%d,hp=%d,is_dead=%d\n",
					np->players[i].x, np->players[i].y, np->players[i].skin, 
					np->players[i].hp, np->players[i].is_dead);
			//strcat(player_pos, buf);
		}

		strncat(player_pos, buf1, strlen(buf1));
	}

	printf("%s\n", player_pos);

	if (connect_to_client(np->ns[cur_client_num], cur_client_num, player_pos, 2) == 0) {
		return 0;
	}

	return 1;
}
