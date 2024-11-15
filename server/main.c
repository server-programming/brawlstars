#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <pthread.h>

#define PORTNUM 9001
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
	int client_x;
	int client_y;

	// 1-- 클라이언트에게 고유 번호 매겨서 전달 서버:send
	int cur_client_num = np->cur_client;
	sprintf(buf, "%d", cur_client_num);
	network_status = send(np->ns[cur_client_num], buf, sizeof(buf), 0);
	if (network_status  == -1) {
		perror("send to client --1");
		close(np->ns[cur_client_num]);
		np->ns[cur_client_num] = 0;
	}
	memset(buf, '\0', sizeof(buf));

	// 2-- 클라이언트가 서버에게 연결되었음을 알려준다
	if (recv(np->ns[cur_client_num], buf, sizeof(buf), 0) == -1) {
		perror("recv from client --2");
		close(np->ns[cur_client_num]);
		np->ns[cur_client_num] = 0;
	}
	printf("**(2) From Client %d: %s\n", cur_client_num, buf);
	memset(buf, '\0', sizeof(buf));

	// 3-- 클라이언트가 서버에게 게임을 시작함을 전달하는 부분 서버:recv
	network_status = recv(np->ns[cur_client_num], buf, sizeof(buf), 0);
	if (network_status == -1) {
		perror("recv from client --3");
		close(np->ns[cur_client_num]);
		np->ns[cur_client_num] = 0;
		return NULL;
	} else if (network_status == 0) {
		printf("**(3) From Client %d: client is offline\n", cur_client_num);
		close(np->ns[cur_client_num]);
		np->ns[cur_client_num] = 0;
		return NULL;
	}

	printf("**(3) From Client %d: %s\n", cur_client_num, buf);
	memset(buf, '\0', sizeof(buf));

	// 게임 속 서버와 클라이언트의 통신
	while(1) {
		// 4-- 서버는 클라이언트에게 지속적으로 위치 정보를 받는다
		memset(buf, '\0', sizeof(buf));
		network_status = recv(np->ns[cur_client_num], buf, sizeof(buf), 0);
		if (network_status == -1) {
			perror("recv from client --4");
			break;
		} else if (network_status == 0) {
			printf("**(4) From Client %d: client is offline\n", cur_client_num);
			break;
		}

		// 클라이언트의 위치를 갱신한다
		sscanf(buf, "x=%d,y=%d", &client_x, &client_y);
		np->players[cur_client_num].x = client_x;
		np->players[cur_client_num].y = client_y;
		memset(buf, '\0', sizeof(buf));
		memset(player_pos, '\0', sizeof(player_pos));
		
		// 클라이언트들의 위치 정보를 보낼 데이터를 준비한다
		for(int i=0; i<PLAYER; i++) {
			if (np->ns[cur_client_num] != 0) {
				sprintf(buf, "%d,x=%d,y=%d\n", i, np->players[i].x, np->players[i].y);
				strcat(player_pos, buf);
			} else {
				sprintf(buf, "%d,x=0,y=0\n", i);
			}
		}

		for (int i=0; i<PLAYER; i++) {
			printf("client %d: x: %d y:%d\n", i, np->players[i].x, np->players[i].y);
		}
		printf("\n");
		
		// 5-- 서버가 클라이언트들에게 위치 정보를 전달한다
		network_status = send(np->ns[cur_client_num], player_pos, sizeof(player_pos), 0);
		if (network_status == -1) {
			perror("send to client --5");
		} else if (network_status == 0) {
			printf("**(5) From Client %d: client is offline\n", cur_client_num);
			break;
		}
	}
	
	// 클라이언트가 오프라인일 경우 정보를 초기화한다
	np->players[cur_client_num].x = 0;
	np->players[cur_client_num].y = 0;
	close(np->ns[cur_client_num]);
	np->ns[cur_client_num] = 0;

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
			continue;
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
