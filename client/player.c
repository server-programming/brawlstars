#include <ncurses.h>
#include <wchar.h>
#include "player.h"
#include "background_music.h"
#include "map.h"

// 플레이어 그리기 함수
void draw_player(int x, int y, wchar_t *player_shape) {
    mvaddwstr(y, x, player_shape); // 플레이어는 정해진 모양(스킨)으로 표시
}

// 플레이어 이동 함수
void move_player(int *x, int *y, int ch, int *direction, wchar_t *player_shape) {
    int old_x = *x, old_y = *y;

    if (ch == 'w') (*y)--, *direction = 0; // 위로
    if (ch == 's') (*y)++, *direction = 2; // 아래로
    if (ch == 'a') (*x)--, *direction = 3; // 왼쪽으로
    if (ch == 'd') (*x)++, *direction = 1; // 오른쪽으로
    
    if (is_player_blocked(*x, *y, player_shape)) {
        *x = old_x;
        *y = old_y;
    } else {
        play_move_sound();
    }
}
