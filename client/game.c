// 기본 모듈
#include <ncurses.h> // 게임 츨력
#include <stdlib.h>
#include <string.h>
// 시간 측정
#include <time.h>
// send/recv
#include <sys/socket.h>
// 오디오 재생
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
// player 구조체, move_player, draw_player
#include "player.h"
// play_background_music
#include "background_music.h"
// draw_map()
#include "map.h"
// bullet 구조체, move_bullet, draw_bullet, shoot_bullet
#include "bullet.h"
// player skin 구조체 정의
#include "player_shape.h"
// game.c 함수 프로토타입 정의
#include "game.h"

#define MAX_PLAYERS 4

// 게임 초기화 및 루프
void init_game(int sd, int client_num, int selected_skin) {
    // 플레이어 정보 배열 (지역 변수로 선언)
    Player players[MAX_PLAYERS]; // 최대 4명의 플레이어를 관리할 수 있음

    // 초기 플레이어 정보 설정
    int x = COLS / 2, y = LINES / 2;
    char buf[1000];
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

    // 현재 플레이어 초기화
    Player* player = &players[0];
    player->x = x;
    player->y = y;
    player->skin = player_shapes->shapes[selected_skin];
    player->hp = 3;
    player->is_dead = 0;
    player->dir = 1;

    init_map(); // 맵 초기화

    while (1) { // 게임 루프
        clear(); // 화면 지우기

        draw_map(); // 맵 그리기

        ch = getch(); // 키 입력

        // 플레이어 이동 및 방향 설정
        move_player(player, ch);
        shoot_bullet(player->x, player->y, player->dir, player->skin, ch);
        // 서버에 플레이어 위치 및 상태 전달 (스킨을 정수형 인덱스로 보냄)
        snprintf(buf, sizeof(buf), "ACCESS_TO_GAME,x=%d,y=%d,skin=%d,hp=%d,is_dead=%d", player->x, player->y, selected_skin, player->hp, player->is_dead);
        if (send(sd, buf, sizeof(buf), 0) == -1) {
            perror("send to server");
            break;
        }

        // 서버로부터 다른 플레이어의 정보 수신
        memset(buf, '\0', sizeof(buf));
        if (recv(sd, buf, sizeof(buf), 0) == -1) {
            perror("recv from server");
            break;
        }

        // 서버로부터 받은 정보로 다른 플레이어 그리기
        char* line = strtok(buf, "\n");        
        
        int index = 1;
        while (line != NULL) {
            int id, x1, y1, skin_index, hp, is_dead;

            if (sscanf(line, "%d,x=%d,y=%d,skin=%d,hp=%d,is_dead=%d", &id, &x1, &y1, &skin_index, &hp, &is_dead) == 6) {
                Player* other_player = &players[index]; // 해당 플레이어의 정보 업데이트
                other_player->x = x1;
                other_player->y = y1;
                other_player->skin = player_shapes->shapes[skin_index]; // 정수형 인덱스를 통해 스킨 설정
                other_player->hp = hp;
                other_player->is_dead = is_dead;
            }
            line = strtok(NULL, "\n");
            index += 1;
        }

        for (int i = 0; i < MAX_PLAYERS; i++) {
            draw_player(&players[i]);
        }
        
        // 총알 처리
        move_bullets(player->x, player->y, player->skin, sd);
        draw_bullets(); // 총알 그리기

        // 화면 업데이트
        refresh();
        napms(10); 
    }
}

