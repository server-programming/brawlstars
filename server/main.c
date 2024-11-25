#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <pthread.h>

#define PORTNUM 12312
#define MAX_THREAD 100
#define PLAYER 4

typedef struct player {
        int x;
        int y;
} player;

typedef struct network_player {
	int *ns;
	struct player *players;
	int cur_client;
} network_player;

typedef struct {
	struct sockaddr_in cli;
	int sd;
} network;

//네트워크 연결이 잘 되었는지 확인하는 함수 선언
network network_connection();

int connection(int ns, int cur_client_num, char *buf, int flag);

// 현재 대기 중인 플레이어수를 저장하는 변수
int cur_player = 0;

void *threadfunc(void *vargp) {
	network_player *np = (network_player *)vargp;
	char buf[50];
	char player_pos[200];
	int network_status;
	int client_x;
	int client_y;
	int cur_client_num = np->cur_client;
	int ns = np->ns[cur_client_num];

	while(1) {
		memset(buf, '\0', sizeof(buf));
		network_status = recv(ns, buf, sizeof(buf), 0);

		if (network_status == -1) {
			perror("recv");
			break;
		} else if (network_status == 0) {
			break;
		}

		// 클라이언트가 서버와 연결될 경우
		if (strstr(buf, "<<connect>>") != NULL) {
			// 클라이언트 연결에 오류가 있거나 오프라인일 경우 0을 반환
			if (connection(np->ns[cur_client_num], cur_client_num, buf, 1) == 0) {
				break;
			}
			printf("Client %d is online\n", cur_client_num);
			cur_player++;
		}


		// 클라이언트가 로비에 접속하는 경우 동접자 수를 늘린다
		// 서버는 동접자 수를 전송한다
		if (strstr(buf, "<<concurrent_users>>") != NULL) {
			memset(buf, '\0', sizeof(buf));
			sprintf(buf, "%d", cur_player);
			if (connection(np->ns[cur_client_num], cur_client_num, buf, 1) == 0) {
				break;
			}
		}
		
		// 클라이언트가 게임에 접속하는 경우
		if (strstr(buf, "<<game>>") != NULL) {
			printf("%s\n", buf);
			sscanf(buf, "<<game>>x=%d,y=%d", &client_x, &client_y);
			np->players[cur_client_num].x = client_x;
			np->players[cur_client_num].y = client_y;
			
			memset(player_pos, '\0', sizeof(player_pos));
			for(int i=0; i<PLAYER; i++) {
				memset(buf, '\0', sizeof(buf));
				if (np->ns[cur_client_num] != 0) {
					sprintf(buf, "%d,x=%d,y=%d\n", i, np->players[i].x, np->players[i].y);
					strcat(player_pos, buf);
				} else {
					sprintf(buf, "%d,x=0,y=0", i);
				}
			}

			//for(int i=0; i<PLAYER; i++) {
			//	printf("client %d: x: %d y: %d\n", i, np->players[i].x, np->players[i].y);
			//}
			//printf("\n");

			printf("%s\n", player_pos);

			if (connection(np->ns[cur_client_num], cur_client_num, player_pos, 2) == 0) {
				break;
			}
		}
	}

	printf("client %d is offline\n", cur_client_num);
	cur_player--;
	close(np->ns[cur_client_num]);
	np->ns[cur_client_num] = 0;
	np->players[cur_client_num].x = 0;
	np->players[cur_client_num].y = 0;

	return NULL;
}

int main() {
	char buf[256];
	struct sockaddr_in cli;
	int sd;
	int *ns;
	int tid_count = 0;
	int clientlen;
	pthread_t tid[MAX_THREAD];
	int temp_ns;
	int ns_accept;
	
	// 서버의 소켓 준비 코드
	network nt;
	nt = network_connection();
	sd = nt.sd;
	cli = nt.cli;
	clientlen = sizeof(cli);

	// 클라이언트 정보 저장할 배열 준비
	player *p = (player *)malloc(sizeof(player) * PLAYER);
	network_player *np = (network_player *)malloc(sizeof(network_player));
	ns = (int *)malloc(sizeof(int) * PLAYER);

	for(int i=0; i<PLAYER; i++) {
		ns[i] = 0;
	}

	np->players = p;
	np->ns = ns;

	while(1) {

		// 서버는 클라이언트가 접속을 요청할 경우, ns 배열이 비었는지 보고 빈 곳이 있다면 그곳에 넣는다
		temp_ns = accept(sd, (struct sockaddr *)&cli, &clientlen);
		if (temp_ns == -1) {
			perror("accept");
			exit(1);
			break;
		}

		ns_accept = 0;

		for(int i=0; i<PLAYER; i++) {
			if (np->ns[i] == 0) {
				np->ns[i] = temp_ns;
				np->cur_client = i;
				ns_accept = 1;
				break;
			}
		}

		if (ns_accept == 0) {
			printf("FULL!!\n");
			break;
		}

		// 쓰레드 생성
		if (tid_count < MAX_THREAD) {
			if (pthread_create(&tid[tid_count], NULL, threadfunc, (void *)np) != 0) {
				perror("pthread_create");
				break;
			} else {
				tid_count++;
			}
		} else {
			printf("Maximum number of clients reached.\n");
		}
	}

	for(int i=0; i<tid_count; i++) {
		pthread_join(tid[i], NULL);
	}

	close(*ns);
	free(ns);
	free(p);
	free(np);

	return 0;
}

int connection(int ns, int cur_client_num, char *buf, int flag) {
	int network_status;
	char buf1[50];
	char buf2[200];

	if (flag == 1) {
		strcpy(buf1, buf);
		network_status = send(ns, buf1, sizeof(buf1), 0);
	}

	if (flag == 2) {
		strcpy(buf2, buf);
		network_status = send(ns, buf2, sizeof(buf2), 0);
	}

	if (network_status == -1 || network_status == 0) {
		return 0;
	} else {
		return 1;
	}
}


network network_connection() {
	network nt;
	struct sockaddr_in sin, cli;
	int sd, clientlen = sizeof(cli);
	
	if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		exit(1);
	}

	int optvalue = 1;
	setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &optvalue, sizeof(optvalue));
	memset((char *)&sin, '\0', sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(PORTNUM);
	sin.sin_addr.s_addr = inet_addr("0.0.0.0");

        if (bind(sd, (struct sockaddr *)&sin, sizeof(sin))) {
                perror("bind");
                exit(1);
        }

        if (listen(sd, 5)) {
                perror("listen");
                exit(1);
        }

	nt.cli = cli;
	nt.sd = sd;

	return nt;
}

