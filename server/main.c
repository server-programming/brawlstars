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
#define MAX_PLAYER 1000
#define MATCHING_NUM 10

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

typedef struct {
	struct sockaddr_in cli;
	int sd;
} network;

// 매칭 대기 중인 클라이언트가 10명인지 확인하기 위한 배열
int ready_client[MATCHING_NUM] = {0, };
int ready_client_num = 0;

// 네트워크 연결이 잘 되었는지 확인하는 함수 network_connec.c
network network_connection();

// 서버가 클라이언트에게 데이터를 전송하는 함수 connect_to_client.c
int connect_to_client(int ns, int cur_client_num, char *buf, int flag);

// 현재 접속 중인 플레이어수를 저장하는 변수
int cur_player = 0;

// 게임에 접속하면 클라이언트와 서버의 데이터 전송을 이 함수가 관여함 recv_send_game_data.c
int recv_send_game_data(network_player *np, char *buf, int cur_client_num);

void *threadfunc(void *vargp) {
	network_player *np = (network_player *)vargp;
	char buf[50];
	char player_pos[200];
	int network_status;
	int client_x;
	int client_y;
	int client_skin;
	int client_hp;
	int client_is_dead;
	int cur_client_num = np->cur_client;
	int ns = np->ns[cur_client_num];

	// 클라이언트가 로비에 처음 접속하는지 확인하기 위한 변수
	int access_to_lobby = 1;

	while(1) {
		printf("현재 대기 중인 클라이언트 수 %d\n", ready_client_num);

		memset(buf, '\0', sizeof(buf));
		network_status = recv(ns, buf, sizeof(buf), 0);

		if (network_status == -1) {
			perror("recv");
			break;
		} else if (network_status == 0) {
			break;
		}

		// 클라이언트가 서버와 연결될 경우 고유번호를 전송한다
		if (strstr(buf, "GET_CLIENT_UNIQUE_NUM") != NULL) {
			// 클라이언트 연결에 오류가 있거나 오프라인일 경우 0을 반환
			if (connect_to_client(np->ns[cur_client_num], cur_client_num, buf, 1) == 0) {
				break;
			}
			printf("Client %d start game\n", cur_client_num);
		}

		// 클라이언트가 로비에 접속하는 경우 동접자 수를 늘린다
		// 서버는 동접자 수를 전송한다
		if (strstr(buf, "GET_CONCURRENT_USER") != NULL) {

			// 클라이언트가 로비에 접속한 경우
			if (access_to_lobby) {

				// 동접자를 1 늘린다
				cur_player++;

				// 동접자가 계속 늘어나는 것을 막기 위해 0으로 설정
				access_to_lobby  = 0;
			}

			// 동접자 정보를 클라이언트에게 전송
			memset(buf, '\0', sizeof(buf));
			sprintf(buf, "%d", cur_player);
			if (connect_to_client(np->ns[cur_client_num], cur_client_num, buf, 1) == 0) {
				break;
			}
		}

		// 클라이언트가 매칭을 요청하면 요청 인원이 10명이 될때까지 대기한다
		if (strstr(buf, "GET_READY_USER") != NULL) {

			int status = 1;
			int index = 0;

			// 전역 배열에 접근하므로 락을 걸어야 한다
			for(int i=0; i<MATCHING_NUM; i++) {
				if (ready_client[i] == 0) {
					ready_client[i] = np->ns[cur_client_num];
					ready_client_num += 1;
					index = i;
				}
			}
			
			memset(buf, '\0', sizeof(buf));
			sprintf(buf, "WAIT_FOR_MATCH");
			while(1) {
				if (ready_client_num == MATCHING_NUM) {
					break;
				}

				// 서버는 클라이언트에게 WAIT_FOR_MATCHING 메시지를 보내 클라이언트가 기다리게 한다
				// 클라이언트는 서버로부터 계속 WAIT_FOR_MATCH 메시지를 받으면서 대기한다
				if (connect_to_client(np->ns[cur_client_num], cur_client_num, buf, 1) == 0) {
					// 클라이언트가 도중에 연결이 끊길 수 있으므로
					status = 0;
					break;
				}
			}

			// 서버는 매칭이 완료되면 클라이언트에게 GAME_MATCHED 메시지를 보내 클라이언트가 대기상태를 벗어나 게임을 실행하도록 한다
			if (status) {
				memset(buf, '\0', sizeof(buf));
				sprintf(buf, "GAME_MATCHED\n");

				if (connect_to_client(np->ns[cur_client_num], cur_client_num, buf, 1) == 0) {
					break;
				}
			} else {
				ready_client[index] = 0;
				ready_client_num -= 1;
				break;
			}
		}
	


		// 클라이언트가 게임에 접속하는 경우
		if (strstr(buf, "ACCESS_TO_GAME") != NULL) {

			if (recv_send_game_data(np, buf, cur_client_num) == 0) {
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
	pthread_t tid[MAX_PLAYER];
	int temp_ns;
	int ns_accept;
	
	// 서버의 소켓 준비 코드
	network nt;
	nt = network_connection();
	sd = nt.sd;
	cli = nt.cli;
	clientlen = sizeof(cli);

	// 클라이언트 정보 저장할 배열 준비
	player *p = (player *)malloc(sizeof(player) * MAX_PLAYER);
	network_player *np = (network_player *)malloc(sizeof(network_player));
	ns = (int *)malloc(sizeof(int) * MAX_PLAYER);

	for(int i=0; i<MAX_PLAYER; i++) {
		ns[i] = 0;
	}

	np->players = p;
	np->ns = ns;

	while(1) {

		// 서버는 클라이언트가 접속을 요청할 경우
		// ns 배열이 비었는지 보고 빈 곳이 있다면 그곳에 넣는다
		temp_ns = accept(sd, (struct sockaddr *)&cli, &clientlen);
		if (temp_ns == -1) {
			perror("accept");
			exit(1);
			break;
		}

		ns_accept = 0;

		for(int i=0; i<MAX_PLAYER; i++) {
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
		if (tid_count < MAX_PLAYER) {
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


