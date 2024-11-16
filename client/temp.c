// ncurses 모듈
#include <ncurses.h>
#include <wchar.h>

// 기본 모듈
#include <unistd.h>
#include <string.h>

// 네트워크 모듈
#include <sys/socket.h>

// 음악 재생 모듈
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

// 사용자 정의 모듈
#include "lobby.h"
#include "text_align.h"
#include "background_music.h"

void lobby(int sd, int client_num) {
    char ch;
    while (1) {
        clear(); // 화면 지우기
        print_concurrent_users(sd); // 동시 접속자 수 출력
        refresh();

        ch = getch();
        if (ch == 'q') {
            break; 
        }
    }
    return;
}
// 동시 접속자 수 출력
void print_concurrent_users(int sd) {
    int concurrent_users = get_concurrent_users(sd); // 동시 접속자 수 저장
    
    // 동시 접속자 수 출력 형식 지정
    wchar_t wstr[50];
    swprintf(wstr, sizeof(wstr) / sizeof(wchar_t), L"Current concurrent users: %d", concurrent_users);
    
    // 화면 중앙에 동시 접속자 수 출력 
    center_text(10, COLS / 2, wstr); 
    return;
}

// 동시 접속자 수 반환
int get_concurrent_users(int sd) {
    int concurrent_users = -1;

    const char* request = "GET_CONCURRENT_USERS"; // 동시 접속자 수 확인 요청
    if (send(sd, request, strlen(request), 0) < 0) {
        perror("Error sending request to server");
        return -1;
    }    

    char buffer[256]; // 서버로부터 동시 접속자 수 응답 수신
    if (recv(sd, buffer, sizeof(buffer) - 1, 0) < 0) {
        perror("Error receiving reposne from server");
        return -1;
    }
    
    // 문자열로 받은 동시 접속자 수를 정수로 변환
    concurrent_users = atoi(buffer);
    return concurrent_users; // 동시 접속자 수 반환
}
