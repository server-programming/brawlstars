#include <SDL2/SDL_mixer.h>
#include "background_music.h"

// 배경 음악 시작
void play_background_music(const char *music_file) {
    if (Mix_PlayingMusic() == 0) {
        Mix_Music *music = Mix_LoadMUS(music_file);
        if (music == NULL) {
            printf("Error loading music: %s\n", Mix_GetError());
            return;
        }
        Mix_PlayMusic(music, -1);  // 음악을 무한 반복
    }
}

// 배경 음악 멈추기
void stop_background_music() {
    Mix_HaltMusic();
}

