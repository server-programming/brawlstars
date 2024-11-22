#include <SDL2/SDL_mixer.h>
#include "background_music.h"

Mix_Chunk *shoot_sound = NULL;
Mix_Chunk *move_sound = NULL;

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

void init_sound_effects() {
    shoot_sound = Mix_LoadWAV("../audio_files/shoot_sound.wav");
    move_sound = Mix_LoadWAV("../audio_files/move_sound.wav");
}

void play_shoot_sound() {
    if (shoot_sound) {
        Mix_PlayChannel(-1, shoot_sound, 0);
    }
}

void play_move_sound() {
    if (move_sound) {
        Mix_PlayChannel(-1, move_sound, 0);
    }
}
