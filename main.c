#include <stdio.h> 
#include <locale.h> 
#include <ncursesw/ncurses.h>
#include "start_menu.h"
#include "help.h"
#include "game.h"
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORTNUM 9001

int main() {
	int client_num;
	struct sockaddr_in sin;
	int sd;
	char buf[256];

	memset((char *)&sin, '\0', sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(PORTNUM);
	sin.sin_addr.s_addr = inet_addr("172.27.65.48");

	if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		exit(1);
	}

	if (connect(sd, (struct sockaddr *)&sin, sizeof(sin))) {
	 	perror("connect");
	 	exit(1);
	}

	if (recv(sd, buf, sizeof(buf), 0) == -1) {
		perror("recv");
		exit(1);
	}

	client_num = atoi(buf);

	memset(buf, '\0', sizeof(buf));

	sprintf(buf, "Client %d is online", client_num);

	if (send(sd, buf, sizeof(buf), 0) == -1) {
		perror("send");
		exit(1);
	}

	printf("server connect!");

	int selected_mode = 0;
	basic_setting();
	
	while (1) {
		start_menu(&selected_mode);
		if (selected_mode == 1) {
			init_game(sd);
		} else if (selected_mode == 2) {
			help();
		} else if (selected_mode == 3) {
			break;
		}
	}
	endwin(); // ncurses 종료
	
	close(sd);

	return 0;
}
