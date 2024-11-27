// 기본 모듈
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
// ncurses 모듈
#include <ncurses.h>
#include <wchar.h>
// 네트워크 모듈
#include <sys/socket.h>
// 오디오 모듈
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
// 시스템 모듈
#include <sys/time.h>
// 사용자 정의 모듈
// play_background_music
#include "background_music.h"
// lobby.c 함수 프로토타입 선언
#include "lobby.h"
// init_game
#include "game.h"
// player skin 구조체
#include "player_shape.h"
// center_text (텍스트를 가운데 정렬하여 출력)
#include "text_align.h"
// get_concurrent_users, get_is_matched
#include "request.h"
// struct timeval 시간차를 ms로 반환
#include "ping_time.h"

// 입력을 받아서 캐릭터 선택, 게임 매칭 시작 또는 종료 처리
int get_input_and_process(int *selected_skin, int *is_matching) {
    int ch;

    ch = getch();

    // 숫자 입력 (캐릭터 선택)
    if (ch >= '1' && ch <= '9') {
        *selected_skin = ch - '1';  // 선택된 캐릭터 저장
        return 1;  // 캐릭터 선택 완료
    }
    // Enter 키 (게임 매칭 시작 또는 취소)
    else if (ch == 10) {
        if (*is_matching) {
            *is_matching = 0;  // 이미 매칭 중이라면 매칭 취소
            return 4;  // 매칭 취소
        } else {
            *is_matching = 1;  // 매칭 시작
            return 2;  // 매칭 시작
        }
    }
    // ESC 키 (게임 종료)
    else if (ch == 27) {
        return 3;  // 게임 종료
    }

    return 0;  // 다른 키 입력 시 아무 동작도 하지 않음
}

// 게임 대기 화면과 매칭 처리
void lobby(int sd, int client_num) {
    int concurrent_users = -1; // 동접자 수
    int is_matching = 0; // 매칭 중인지 여부
    int is_matched = 0; // 게임 매칭 여부
    struct timeval start, end; // 응답 측정 시작 시간, 응답 측정 종료 시간
    long long ping;
    int selected_skin = 0;

    // 오디오 초기화
    if (SDL_Init(SDL_INIT_AUDIO) < 0 || Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        perror("SDL or Mix_OpenAudio failed");
        return;
    }
    
    // 배경 음악 재생
    play_background_music("../audio_files/metallic_madness_zone_act_2.mp3");

    while (1) {
        // 응답 속도 측정
        gettimeofday(&start, NULL);
        concurrent_users = get_concurrent_users(sd); // 동접자 수 저장
        gettimeofday(&end, NULL); // 응답 속도 측정 종료
        ping = get_ms(start, end); // 핑 계산

        // 화면을 지우지 않고 갱신
        clear(); // 화면을 지운 후에 계속해서 출력되도록 할 수 있습니다.

        // 스킨 선택
        PlayerShape *player_shapes = get_player_shape();
        int x_position = COLS / 4;  // 시작 위치

        for (int i = 0; i < MAX_SHAPES; i++) {
            mvprintw(14, x_position + i * 8, "%d", i + 1);  // 숫자
            mvprintw(15, x_position + i * 8, "%ls", player_shapes->shapes[i]);  // 캐릭터 모양
        }

        mvprintw(LINES - 2, COLS / 2 - 10, "(1-%d)번을 눌러 선택", MAX_SHAPES);

        // 선택된 캐릭터 출력 (선택이 있을 때마다 갱신)
        print_selected_skin(selected_skin);

        // 입력 받아 처리
        int input_result = get_input_and_process(&selected_skin, &is_matching);

        // 캐릭터 선택된 경우
        if (input_result == 1) {
            // 캐릭터가 선택되면 화면에 계속 유지됨
            print_selected_skin(selected_skin);  // 선택된 캐릭터 출력
        }
        // 매칭 시작 또는 취소 처리
        else if (input_result == 2) {
            while (1) {
                gettimeofday(&start, NULL);
                is_matched = 1; // 서버에서 매칭이 아직 안 됨
                gettimeofday(&end, NULL);
                ping = get_ms(start, end);

                // 매칭 대기 중일 때 화면을 갱신하고 계속 입력받을 수 있도록 변경
                if (is_matched == 0) {
                    break; // 매칭 대기 상태에서 빠져나오기 위해 while문을 탈출
                }

                if (is_matched == 1) {                    
                    stop_background_music(); // 배경 음악 중지 
                    init_game(sd, client_num, selected_skin); // 게임 시작
                    break;
                }
            }
        }
        // 매칭 취소
        else if (input_result == 4) {
            is_matching = 0; // 매칭 취소 상태로 되돌리기
        }
        // 게임 종료
        else if (input_result == 3) {
            break; // 게임 종료 (ESC)
        }

        // 동접자 수와 매칭 여부 출력
        if (concurrent_users == -1) { // 동접자 수를 가져오지 못한 경우
            mvprintw(10, COLS / 2 - 10, "Failed to get concurrent users.");
        } else {
            print_lobby_status(concurrent_users, ping, is_matching); // 상태 출력
        }

        refresh(); // 화면 갱신
        sleep(1);
    }
}

// 게임 대기열 상태 출력 함수
void print_lobby_status(int concurrent_users, long long ping, int is_matching) {
    // 동시 접속자 수 출력
    wchar_t wstr[50];
    swprintf(wstr, sizeof(wstr) / sizeof(wchar_t), L"동접자 수: %d \t 핑: %ldms", concurrent_users, ping);
    center_text(10, COLS, wstr);

    // 매칭 상태 출력
    const wchar_t* matching_status = is_matching ? L"매칭 중... (Enter를 눌러 취소)" : L"매칭 시작 (Enter를 눌러 시작)";
    center_text(12, COLS, matching_status);
}

// 선택된 캐릭터 출력 함수
void print_selected_skin(int selected_skin) {
    PlayerShape *player_shapes = get_player_shape();
    if (selected_skin >= 0 && selected_skin < MAX_SHAPES) {
        // 선택된 캐릭터를 화면에 출력
        mvprintw(LINES - 4, COLS / 2 - 10, "선택한 캐릭터: ");
        mvprintw(LINES - 3, COLS / 2 - 10, "%ls", player_shapes->shapes[selected_skin]);
    }
}

