#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <pthread.h>

#define PORTNUM 23042
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

network network_connection();

void *threadfunc(void *vargp) {
	network_player *np = (network_player *)vargp;
	char buf[50];
	char player_pos[1024];
	int network_status;

	// 1-- 클라이언트에게 고유 번호 매겨서 전달 서버:send
	int cur_client_num = np->cur_client;
	printf("cur_client_num %d\n", cur_client_num);
	sprintf(buf, "%d", cur_client_num);

	if (send(np->ns[cur_client_num], buf, strlen(buf), 0) == -1) {
		perror("send to client -- 1");
		exit(1);
	}

	// 2-- 클라이언트가 서버에게 연결되었음을 알려줄 부분, 서버:recv
	if (recv(np->ns[cur_client_num], buf, sizeof(buf), 0) == -1) {
		perror("recv to client -- 2");
		exit(1);
	}


	// 3-- 클라이언트가 서버에게 게임을 시작함을 전달하는 부분 서버:recv
	

	// 클라이언트가 서버에게 데이터를 전달하는 부분, 서버:recv
	
	// 서버가 클라이언트들에게 데이터를 전달하는 부분, 서버:recv


	if (send(np->ns[cur_client_num], buf, strlen(buf), 0) == -1) {
		printf("client : %d, ns = %d\n", cur_client_num, np->ns[cur_client_num]);
		perror("send");
		exit(1);
	}

	while(1) {
		memset(buf, '\0', sizeof(buf));
		network_status = recv(np->ns[cur_client_num], buf, sizeof(buf), 0);

		if (network_status == -1) {
			perror("recv");
			break;
		} else if (network_status == 0) {
			printf("** From Client %d : Client is offline\n", cur_client_num);
			break;
		} else {

			if (strstr(buf, "=") != NULL) {
				sscanf(buf, "x=%d,y=%d", &np->players[cur_client_num].x
						, &np->players[cur_client_num].y);

				for(int i=0; i<PLAYER; i++) {
					printf("*** From Client %d : x: %d y: %d\n", 
							i, np->players[i].x, np->players[i].y);
				}
				printf("\n");

				// 서버가 모든 클라이언트들의 위치를 모든 클라이언트들에게 전달한다
				memset(buf, '\0', sizeof(buf));
				memset(player_pos, '\0', sizeof(player_pos));

				for(int i=0; i<PLAYER; i++) {
					sprintf(buf, "%d,x=%d,y=%d\n", 
							i, np->players[i].x, np->players[i].y);
					strcat(player_pos, buf);
				}

				for(int i=0; i<PLAYER; i++) {
					if (np->ns[i] > 0) {
						if (send(np->ns[i], player_pos, strlen(player_pos), 0) == -1) {
							perror("send to client");
							exit(1);
						}
					}
				}
				
			} else {
				printf("** From Client : %s\n", buf);
			}

		}
	}

	close(np->ns[cur_client_num]);
	np->ns[cur_client_num] = 0;

	printf("ns->ns : %d\n", np->ns[cur_client_num]);

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

		for(int i=0; i<PLAYER; i++) {
			printf("** client %d  ns %d\n", i, np->ns[i]);
		}

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
			continue;
		}

		if (tid_count < MAX_THREAD) {
			if (pthread_create(&tid[tid_count], NULL, threadfunc, (void *)np) != 0) {
				perror("pthread_create");
				break;
			} else {
				tid_count++;
			}
		} else {
			fprintf(stderr, "Maximum number of clients reached.\n");
		}

		for(int i=0; i<PLAYER; i++) {
			printf("ns : %d\n", np->ns[i]);
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
