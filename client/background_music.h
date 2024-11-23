#ifndef BACKGROUND_MUSIC_H
#define BACKGROUND_MUSIC_H

void play_background_music(const char *music_file); // 특정 음악 파일을 배경 음악으로 재생
void stop_background_music(); // 현재 재생 중인 배경 음악을 정지
void play_shoot_sound();
void play_move_sound();
#endif
