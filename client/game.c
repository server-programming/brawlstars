#include <ncurses.h>
#include <locale.h>
#include <stdlib.h>
#include <time.h>
#include <sys/socket.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
// 사용자 정의 모듈
#include "game.h"
#include "player_shape.h"
#include "map.h"
#include "background_music.h"
#include "player.h"
#include "bullet.h"

// 게임 초기화 및 루프
void init_game(int sd, int client_num) {
    // 플레이어의 초기 위치 및 방향
    int x = COLS / 2, y = LINES / 2, player_dir;
    // 데이터 송신을 위한 버퍼
    char buf[256], player_pos[1024];
    char ch; // 플레이어가 입력하는 키
    player_loc all_players[4]; // 4명의 플레이어 위치를 저장
    int id, x1, y1;
    
    initscr(); // ncurses 초기화
    cbreak(); // 입력 버퍼링 비활성화
    noecho(); // 키 입력을 화면에 표시하지 않음
    curs_set(0); // 커서 숨기기
    nodelay(stdscr, TRUE); // getch()가 블로킹되지 않도록 설정
    keypad(stdscr, TRUE); // 특수 키 처리 활성화

    // 오디오 초기화
    if (SDL_Init(SDL_INIT_AUDIO) < 0 || Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        perror("SDL or Mix_OpenAudio failed");
        return;
    }
    play_background_music("../audio_files/planet_brainwave.mp3");

    PlayerShape *player_shapes = get_player_shape();
    int current_shape = 0;

    init_map();

    while (1) {
        clear();

        draw_map();
        draw_player(x, y, player_shapes->shapes[current_shape]);
        move_bullets();
        draw_bullets();

        refresh();

        ch = getch();

        // 플레이어 이동 및 방향 설정
        move_player(&x, &y, ch, &player_dir);

        // 서버에 위치 전달
        snprintf(buf, sizeof(buf), "x=%d,y=%d", x, y);
        if (send(sd, buf, sizeof(buf), 0) == -1) {
            perror("send to server");
            break;
        }

        // 서버에서 위치 정보 받기
        memset(player_pos, 0, sizeof(player_pos));
        if (recv(sd, player_pos, sizeof(player_pos), 0) == -1) {
            perror("recv from server");
            break;
        }

        // 다른 플레이어 그리기
        char *line = strtok(player_pos, "\n");
        while (line) {
            if (sscanf(line, "%d,x=%d,y=%d", &id, &x1, &y1) == 3) {
                draw_player(x1, y1, player_shapes->shapes[current_shape]);
            }
            line = strtok(NULL, "\n");
        }

        // 총알 발사
        if (ch == '\n') {
            shoot_bullet(x, y, player_dir);
        }

        // 플레이어 모양 변경 (디버깅용)
        // if (ch == 'c') {
        //    current_shape = (current_shape + 1) % MAX_SHAPES;
        // }

        // if (ch == 'q') break;

        napms(10);
    }

    stop_background_music();
    SDL_Quit();
    endwin();
}

