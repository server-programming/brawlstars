// game.c
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/socket.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include "player.h"
#include "background_music.h"
#include "map.h"
#include "bullet.h"
#include "player_shape.h"
#include "game.h"

// 게임 초기화 및 루프
void init_game(int sd, int client_num, int selected_skin) {
    // 초기 플레이어 정보 설정
    int x = COLS / 2, y = LINES / 2;
    char buf[50], player_pos[200];
    char ch; // 플레이어가 입력하는 키

    // 오디오 초기화
    if (SDL_Init(SDL_INIT_AUDIO) < 0 || Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        perror("SDL or Mix_OpenAudio failed");
        return;
    }

    // 배경 음악 재생
    play_background_music("../audio_files/planet_brainwave.mp3");

    // 플레이어의 모양(스킨)을 가져옴
    PlayerShape *player_shapes = get_player_shape();

    // 플레이어 구조체 초기화
    Player* player = init_player(player_shapes->shapes[selected_skin], x, y);

    init_map(); // 맵 초기화

    while (1) { // 게임 루프
        clear(); // 화면 지우기

        draw_map(); // 맵 그리기

        ch = getch(); // 키 입력

        // 플레이어 이동 및 방향 설정
        move_player(player, ch);

        // 플레이어 그리기
        draw_player(player);

        // 서버에 플레이어 위치를 전달
        snprintf(buf, sizeof(buf), "<<game>>x=%d,y=%d", player->x, player->y);
        if (send(sd, buf, sizeof(buf), 0) == -1) {
            perror("send to server");
            break;
        }

        // 서버로부터 다른 플레이어의 위치 수신
        memset(player_pos, '\0', sizeof(player_pos));
        if (recv(sd, player_pos, sizeof(player_pos), 0) == -1) {
            perror("recv from server");
            break;
        }

        // 서버로부터 받은 위치 정보로 다른 플레이어 그리기
        char* line = strtok(player_pos, "\n");
        while (line != NULL) {
            int id, x1, y1;
            if (sscanf(line, "%d,x=%d,y=%d", &id, &x1, &y1) == 3) {
                Player* other_player = init_player(player_shapes->shapes[selected_skin], x1, y1);
                draw_player(other_player);
                free(other_player);
            }
            line = strtok(NULL, "\n");
        }

        move_bullets(player->x, player->y, player->skin); // 발사된 총알 이동
        draw_bullets(); // 총알 그리기

        refresh();

        // 총알 발사
        if (ch == '\n') {
            shoot_bullet(player->x, player->y, player->dir, player->skin);
            play_shoot_sound();
        }

        // 플레이어 모양 변경 (디버깅용)
        if (ch == 'c') {
            selected_skin = (selected_skin + 1) % MAX_SHAPES;
            mvprintw(LINES - 1, 0, "Shape changed to %d", selected_skin);
        }

        napms(10); // 게임의 프레임 레이트 조정
    }

    stop_background_music(); // 배경 음악 중지
    SDL_Quit(); // SDL 종료
    endwin(); // ncurses 종료
}

