// 기본 모듈
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
// ncurses 모듈
#include <ncurses.h>
#include <wchar.h>
// 네트워크 모듈
#include <sys/socket.h>
// 시스템 모듈
#include <sys/time.h>
// 사용자 정의 모듈
#include "lobby.h"
#include "text_align.h"
#include "ping_time.h"

// 동시 접속자 수 출력
void print_concurrent_users(int concurrent_users, long long ping) {
    // int concurrent_users = 1;
    // struct timeval start, end;
    
    // 응답 시간 측정 시작
    // gettimeofday(&start, NULL);   
    // int concurrent_users = get_concurrent_users(sd); // 동시 접속자 수 저장
    // 응답 시간 측정 종료
    // gettimeofday(&end, NULL);
    
    // long long ping = get_ms(start, end);     
    
    // 동시 접속자 수 출력 형식 지정
    wchar_t wstr[50];
    swprintf(wstr, sizeof(wstr) / sizeof(wchar_t), L"동접자 수: %d \t 핑: %ldms", concurrent_users, ping);

    // 화면 중앙에 동시 접속자 수 출력 
    center_text(10, COLS, wstr); 
    return;
}

// 동시 접속자 수 반환
// int get_concurrent_users(int sd) {
   //  int concurrent_users = -1;

    // const char* request = "GET_CONCURRENT_USERS"; // 동시 접속자 수 확인 요청
    // if (send(sd, request, strlen(request), 0) < 0) {
    //    perror("Error sending request to server");
    //    return -1;
    //}    

    //char buffer[256]; // 서버로부터 동시 접속자 수 응답 수신
    //if (recv(sd, buffer, sizeof(buffer) - 1, 0) < 0) {
    //    perror("Error receiving reposne from server");
    //    return -1;
    //}
    
    // 문자열로 받은 동시 접속자 수를 정수로 변환
    //concurrent_users = atoi(buffer);
    //return concurrent_users; // 동시 접속자 수 반환
//}

// 도움말 화면을 표시하며, ESC 키를 누를 때까지 대기
void lobby(int sd, int client_num) {
	
	int concurrent_users;
	char buf[50];
	struct timeval start, end;

	while(1) {
		memset(buf, '\0', sizeof(buf));
		sprintf(buf, "<<lobby>>");

		// 응답 속도 측정
		gettimeofday(&start, NULL);
		if (send(sd, buf, sizeof(buf), 0) == -1) {
			perror("send");
			exit(1);
		}
		memset(buf, '\0', sizeof(buf));
		if (recv(sd, buf, sizeof(buf), 0) == -1) {
			perror("recv");
			exit(1);
		}

		// 응답 속도 측정 종료
		gettimeofday(&end, NULL);

		concurrent_users = atoi(buf);
		long long ping = get_ms(start, end);

		clear(); // 화면 지우기
		print_concurrent_users(concurrent_users, ping); // 동접자 수 출력
		refresh(); // 화면 갱신

		int ch = getch(); // 사용자 입력 받기
		
		// ESC(27) 키가 눌리면 메인 메뉴로 돌아가기
		if (ch == 27) { // 
			break;
		}

		sleep(1); // 1초 대기
	}
}
