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
#include <fcntl.h>
#include <errno.h>

#define PORTNUM 9001
#define TIMEOUT_SEC 3

int main() {
	int client_num;
	struct sockaddr_in sin;
	int sd;
	char buf[256];

	memset((char *)&sin, '\0', sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(PORTNUM);
	sin.sin_addr.s_addr = inet_addr("192.168.219.203");

	if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		exit(1);
	}

    // 소켓을 non-blocking 모드로 설정
    fcntl(sd, F_SETFL, O_NONBLOCK);

    // 연결 시도 (비동기 모드에서 즉시 반환)
    if (connect(sd, (struct sockaddr *)&sin, sizeof(sin)) == -1) {
        if (errno == EINPROGRESS) {
            // 연결 대기 상태: select()로 연결 완료를 기다림
            struct timeval timeout;
            fd_set writefds;

            FD_ZERO(&writefds);
            FD_SET(sd, &writefds);

            timeout.tv_sec = TIMEOUT_SEC;   // 타임아웃 설정
            timeout.tv_usec = 0;

            int res = select(sd + 1, NULL, &writefds, NULL, &timeout);
            if (res == 0) {  // 타임아웃 발생
                fprintf(stderr, "Connection timeout: Server IP does not match or server not reachable.\n");
                close(sd);
                exit(1);
            } else if (res < 0) {
                perror("select");
                close(sd);
                exit(1);
            }

            // 연결 성공
            if (FD_ISSET(sd, &writefds)) {
                printf("Successfully connected to the server!\n");
            }
        } else {
            perror("connect");
            close(sd);
            exit(1);
        }
    } else {
        // 즉시 연결이 성공한 경우
        printf("Successfully connected to the server!\n");
    }

    // 소켓을 non-blocking 모드로 설정
    fcntl(sd, F_SETFL, F_LOCK);


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
