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
    char buf[50], player_pos[200];
    char ch; // 플레이어가 입력하는 키
    char *line;
    player_loc all_players[4]; // 4명의 플레이어 위치를 저장
    
    // 플레이어 ID 및 위치를 저장할 변수
    int id, x1, y1; 

    // 오디오 초기화
    if (SDL_Init(SDL_INIT_AUDIO) < 0 || Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        perror("SDL or Mix_OpenAudio failed");
        return;
    }
    
    // 배경 음악 재생
    play_background_music("../audio_files/planet_brainwave.mp3");
    
    // 플레이어의 모양(스킨)을 가져옴
    PlayerShape *player_shapes = get_player_shape();
    int current_shape = 0; // 현재 플레이어 모양 인덱스

    init_map(); // 맵 초기화
    
    while (1) { // 게임 루프
        clear(); // 화면 지우기

        draw_map(); // 맵 그리기
        
        // 플레이어 그리기
        draw_player(x, y, player_shapes->shapes[current_shape]);
        // 서버에 플레이어 위치를 전달
        snprintf(buf, sizeof(buf), "<<game>>x=%d,y=%d", x, y);
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

        // 서버로투버 받은 위치 정보로 다른 플플레이어 그리기
        line = strtok(player_pos, "\n");
        while (line != NULL) {
            if (sscanf(line, "%d,x=%d,y=%d", &id, &x1, &y1) == 3) {
                draw_player(x1, y1, player_shapes->shapes[current_shape]);
            }
            line = strtok(NULL, "\n");
        }
        move_bullets(); // 발사된 총알 이동
        draw_bullets(); // 총알 그리기
        
        refresh();

        ch = getch(); // 키 입력

        // 플레이어 이동 및 방향 설정
        move_player(&x, &y, ch, &player_dir);

        // 총알 발사
        if (ch == '\n') {
            shoot_bullet(x, y, player_dir);
        }

        // 플레이어 모양 변경 (디버깅용)
        // if (ch == 'c') {
        //    current_shape = (current_shape + 1) % MAX_SHAPES;
        // }

        // if (ch == 'q') break;

        napms(10); // 게임의 프레임 레이트 조정 
    }
    
    stop_background_music(); // 배경 음악 중지
    SDL_Quit(); // SDL 종료 
    endwin(); // ncurses 종료
}
