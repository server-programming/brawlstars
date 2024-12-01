#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORTNUM 12312

typedef struct {
	struct sockaddr_in cli;
	int sd;
} network;

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
