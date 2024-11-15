#include <ncurses.h>
#include <locale.h>
#include <wchar.h>  // wchar_t 타입을 위한 헤더 추가
#include <unistd.h> // sleep 함수 사용을 위한 헤더 추가
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include "start_menu.h"
#include "background_music.h"

// 아스키 아트를 출력하는 함수들

// 첫 번째 아스키 아트: 상자에 들어간 텍스트와 캐릭터
void print_art1(int row, int col, const wchar_t* text) {
    const wchar_t* top_bottom = L"|￣￣￣￣￣￣￣￣￣￣￣￣￣￣|";
    const wchar_t* line1 = L"|＿＿＿＿＿＿＿＿＿＿＿＿＿＿|";

    // 화면의 가로 길이를 기준으로 중앙 정렬
    int center_col = (col - wcslen(top_bottom)) / 2 - 4;
    mvaddwstr(row, center_col, top_bottom);  // 상단 테두리
    mvaddwstr(row + 1, (col - wcslen(text)) / 2, text);  // 사용자 텍스트 출력
    mvaddwstr(row + 2, center_col, line1);  // 하단 테두리
}

// 두 번째 아스키 아트: 전투용 아스키 아트
void print_art2(int row, int col) {
    const wchar_t* art[] = {
        L"_/﹋\\_",
        L"(҂`_´)",
        L"<,︻╦╤─ ҉ - -",
        L"_/﹋\\"
    };
    for (int i = 0; i < 4; i++) {
        mvaddwstr(row + i, col, art[i]);  // 주어진 열 위치에서 바로 출력
    }
}



// 아스키 아트를 출력하는 함수 선택
void print_ascii_art(int art_type, int row, int col, const wchar_t* text) {
    switch (art_type) {
        case 1:
            print_art1(row, col, text);
            break;
        case 2:
            print_art2(row, col);
            break;
        default:
            break;
    }
}

// 기본 설정 함수
void basic_setting() {
    setlocale(LC_ALL, ""); // 로케일 설정
    initscr(); // ncurses 초기화
    cbreak(); // 즉시 입력 모드 설정
    noecho(); // 입력된 문자를 화면에 표시하지 않음
    keypad(stdscr, TRUE); // 특수 키 입력 허용
    setlocale(LC_CTYPE, "");
}

// 텍스트 중앙 정렬 출력 함수
void center_text(int row, int col, const wchar_t* text) {
    mvaddwstr(row, (col - wcslen(text)) / 2, text);  // mvaddwstr 사용
}

// 메뉴 출력 함수
void print_menu() {
    const wchar_t* menu_items[] = {
        L"브롤스타즈",  // 유니코드 문자열
        L"1. 게임 시작",
        L"2. 도움말",
        L"3. 게임 종료"
    };

    for (int i = 0; i < 4; i++) {
        center_text(LINES / 2 - 2 + i, COLS, menu_items[i]);
    }
}

// 모드 선택 함수
int get_mode() {
    int selected_mode = 0;
    int ch = getch(); // 사용자 입력 받기
    if (ch == '1') {
        selected_mode = 1;
    } else if (ch == '2') {
        selected_mode = 2;
    } else if (ch == '3') {
        selected_mode = 3;
    } else {
        selected_mode = 0;
    }

    return selected_mode;
}

// 시작 메뉴 함수
void start_menu(int* selected_mode) {
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        printf("SDL_Init failed: %s\n", SDL_GetError());
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("Mix_OpenAudio failed: %s\n", Mix_GetError());
    }

    // play_background_music("../audio_files/lights_camera_action.mp3");

    while(1) {
        clear(); // 화면 지우기

        print_ascii_art(1, 15, COLS, L"브롤스타즈");  // 첫 번째 아트
        print_ascii_art(2, 6, 4, NULL);  // 두 번째 아트 1개: 왼쪽 상단
        print_ascii_art(2, 1, 12, NULL);  // 두 번째 아트 2개: 왼쪽 상단 바로 아래

        // 메뉴 출력
        print_menu();

        // 선택된 모드를 안내하는 유니코드 문자열
        wchar_t mode_text[50]; 

        *selected_mode = get_mode(); // 사용자 모드 입력 받기

        if (*selected_mode == 1) {
            wcscpy(mode_text, L"게임 시작");
        } else if (*selected_mode == 2) {
            wcscpy(mode_text, L"도움말");
        } else if (*selected_mode == 3) {
            wcscpy(mode_text, L"종료 중...");
        } else {
            wcscpy(mode_text, L"");
        }

        center_text(LINES / 2 + 4, COLS, mode_text); // 선택된 모드 출력
        refresh(); // 화면 갱신
        sleep(1); // 1초 대기 후 다음 화면으로 넘어감

        if ((*selected_mode > 0) && (*selected_mode < 4)) {
            break;
        }
    }
    stop_background_music();
    Mix_CloseAudio(); // Mixer 종료
    SDL_Quit(); // SDL 종료
}

