#include <SDL2/SDL_mixer.h>
#include "background_music.h"

// 배경 음악 시작
void play_background_music(const char *music_file) {
    // 현재 음악이 재생 중이 아닌 경우에 실행
    if (Mix_PlayingMusic() == 0) {
        Mix_Music *music = Mix_LoadMUS(music_file); // 음악 파일을 로드
        if (music == NULL) {
            // 음악 로드 중 에러 발생 시 종료
            printf("Error loading music: %s\n", Mix_GetError());
            return;
        }
        Mix_PlayMusic(music, -1);  // 음악을 무한 반복
    }
}

// 배경 음악 정지
void stop_background_music() {
    // 현재 재생 중인 음악을 중지
    Mix_HaltMusic(); 
}

