// 기본 모듈
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

// ncurse와 다국어 지원
#include <ncursesw/ncurses.h>
#include <locale.h> 

// 네트워크 통신 모듈
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>

// 사용자 정의 모듈
#include "start_menu.h"
#include "game.h"
#include "help.h"

// 서버와 통신할 포트 번호 설정
#define PORTNUM 9001
// 타임아웃 시간(초)
#define TIMEOUT_SEC 1

int main() {
	int client_num; // 서버로부터 전달받은 클라이언트 고유 번호
	struct sockaddr_in sin; // 서버 주소 정보를 저장하는 구조체
	int sd; // 소켓 디스크립터
	char buf[50]; // 데이터 송수신 버퍼

    // 서버 주소 구조체 초기화
	memset((char *)&sin, '\0', sizeof(sin));
	sin.sin_family = AF_INET; // IPv4 체계
	sin.sin_port = htons(PORTNUM); // 포트 번호 설정 (네트워크 바이트 순서로 변환)
	sin.sin_addr.s_addr = inet_addr("172.17.14.162"); // 서버 IP 주소 설정 (접속할 때마다 확인 요망)
    
    // 소켓 생성
	if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket"); // 소켓 생성 실패 시 오류 출력
		exit(1);
	}

    // 소켓을 non-blocking 모드로 설정
    fcntl(sd, F_SETFL, O_NONBLOCK);

    // 연결 시도 (비동기 모드에서 즉시 반환)
    if (connect(sd, (struct sockaddr *)&sin, sizeof(sin)) == -1) {
		if (errno == EINPROGRESS) { // 연결 진행 중인 상태
            struct timeval timeout; // 타임아웃 설정용 구조체
            fd_set writefds; // 쓰기 가능한 소켓을 탐지하기 위한 파일 디스크

            FD_ZERO(&writefds); // fd_set 추가
            FD_SET(sd, &writefds); // 소켓 디스크립터 추가

            timeout.tv_sec = TIMEOUT_SEC;   // 타임아웃 시간 설정 (1초)
            timeout.tv_usec = 0;
                    
            // select()로 연결 대기
            int res = select(sd + 1, NULL, &writefds, NULL, &timeout);
            		
		    if (res == 0) {  // 타임아웃 발생
                fprintf(stderr, "Connection timeout: Server IP does not match or server not reachable.\n");
                close(sd);
                exit(1);
            } else if (res < 0) { // 연결 대기(select)에서 오류 발생
                perror("select");
                close(sd);
                exit(1);
            }

            		
            if (FD_ISSET(sd, &writefds)) { // 연결 성공
                printf("IP 주소 매칭 확인\n");
            }
        } else { // 연결 실패
            perror("connect");
            close(sd);
            exit(1);
        }
    } else { // 즉시 연결 성공
        printf("Successfully connected to the server!\n");
    }

    // 소켓을 blocking 모드로 설정
    fcntl(sd, F_SETFL, F_LOCK);

	// 1-- 서버로부터 클라이언트 고유 번호 수신
	if (recv(sd, buf, sizeof(buf), 0) == -1) {
		perror("recv from server --1");
		exit(1);
	}

	// 클라이언트 고유 번호 저장
	client_num = atoi(buf); // 서버로부터 받은 데이터를 정수로 변환
	memset(buf, '\0', sizeof(buf)); // 버퍼 초기화

	// 2-- 서버에게 연결되었음을 알림
	sprintf(buf, "client %d is online", client_num);
	if (send(sd, buf, sizeof(buf), 0) == -1) {
		perror("send to server --2");
		exit(1);
	}

	int selected_mode = 0; // 선택된 메뉴 모드
	basic_setting(); // 프로그램 기본 설정 (ncurses 초기화 등)
	
	while (1) {
		start_menu(&selected_mode); // 시작 메뉴 표시 및 모드 선택
		if (selected_mode == 1) { // 게임 시작
			init_game(sd, client_num);
		} else if (selected_mode == 2) { // 도움말 표시
			help();
		} else if (selected_mode == 3) { // 종료
			break;
		}
	}
	endwin(); // ncurses 종료
	
	close(sd); // 소켓 닫기

	return 0;
}
