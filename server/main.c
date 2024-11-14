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

int client_num = 1;

void *threadfunc(void *vargp) {
	int *ns = (int *)vargp;
	char buf[256];
	int network;
	int cur_client_num = client_num;

	sprintf(buf, "%d", client_num);

	client_num++;

	if (send(*ns, buf, strlen(buf), 0) == -1) {
		perror("send");
		exit(1);
	}

	while(1) {
		memset(buf, '\0', sizeof(buf));
		network = recv(*ns, buf, sizeof(buf), 0);

		if (network == -1) {
			perror("recv");
			exit(1);
			break;
		} else if (network == 0) {
			printf("** From Client %d : Client is offline\n", cur_client_num);
			break;
		}

		printf("** From Client %d : %s\n", cur_client_num, buf);
	}
}

int main() {
	int client_num = 1;
	char buf[256];
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

	while(1) {
		ns = (int *)malloc(sizeof(int));
		if ((*ns = accept(sd, (struct sockaddr *)&cli, &clientlen)) == -1) {
			perror("accept");
			exit(1);
			free(ns);
			continue;
		}

		if (tid_count < MAX_THREAD) {
			if (pthread_create(&tid[tid_count], NULL, threadfunc, (void *)ns) != 0) {
				perror("pthread_create");
				free(ns);
			} else {
				tid_count++;
				client_num++;
			}
		} else {
			fprintf(stderr, "Maximum number of clients reached.\n");
		}
	}

	for(int i=0; i<tid_count; i++) {
		pthread_join(tid[i], NULL);
	}

	close(*ns);
	free(ns);

	return 0;
}
