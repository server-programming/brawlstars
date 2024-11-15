#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <pthread.h>

#define PORTNUM 14023
#define MAX_THREAD 100
#define PLAYER 4

int client_num = 0;

typedef struct player {
        int x;
        int y;
} player;

typedef struct network_player {
        int *ns;
        player *players;
} network_player;

void *threadfunc(void *vargp) {
        network_player *np = (network_player *)vargp;
        char buf[256];
        int network_status;
        int cur_client_num = client_num; // 정확하게 값을 추적

        client_num++; // 클라이언트 수 증가

        sprintf(buf, "%d", cur_client_num);

        if (send(*(np->ns), buf, strlen(buf), 0) == -1) {
                perror("send");
                exit(1);
        }

        while(1) {
                memset(buf, '\0', sizeof(buf));
                network_status = recv(*(np->ns), buf, sizeof(buf), 0);

                if (network_status == -1) {
                        perror("recv");
                        exit(1);
                        break;
                } else if (network_status == 0) {
                        printf("** From Client %d : Client is offline\n", cur_client_num);
                        break;
                } else if (network_status > 0) {
                        sscanf(buf, "%d,%d", &np->players[cur_client_num].x, &np->players[cur_client_num].y);

                        for (int i = 0; i < PLAYER; i++) {
                                printf("** From Client : Client: %d x: %d y: %d\n", i, np->players[i].x, np->players[i].y);
                        }
                }
        }

        free(np);  // 메모리 해제
        return NULL;
}

int main() {
        struct sockaddr_in sin, cli;
        int sd, clientlen = sizeof(cli);
        int* ns;
        int tid_count = 0;
        pthread_t tid[MAX_THREAD];

        if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
                perror("socket");
                exit(1);
        }

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

        // 플레이어 배열을 동적으로 할당
        player *p = (player *)malloc(sizeof(player) * PLAYER);
        network_player *np = (network_player *)malloc(sizeof(network_player));

        while(1) {
                ns = (int *)malloc(sizeof(int));
                if ((*ns = accept(sd, (struct sockaddr *)&cli, &clientlen)) == -1) {
                        perror("accept");
                        exit(1);
                        free(ns);
                        continue;
                }

                np->ns = ns;
                np->players = p;

                if (tid_count < MAX_THREAD) {
                        if (pthread_create(&tid[tid_count], NULL, threadfunc, (void *)np) != 0) {
                                perror("pthread_create");
                                free(np->ns);
                                free(np);
                        } else {
                                tid_count++;
                        }
                } else {
                        fprintf(stderr, "Maximum number of clients reached.\n");
                }
        }

        for(int i = 0; i < tid_count; i++) {
                pthread_join(tid[i], NULL);
        }

        close(*ns);
        free(p);  // 메모리 해제
        free(np); // 메모리 해제
        free(ns); // 메모리 해제

        return 0;
}

