#ifndef PLAYER_SHAPE_H
#define PLAYER_SHAPE_H

#include <wchar.h>

// 플레이어가 가질 수 있는 스킨(모양)의 개수
#define MAX_SHAPES 18

// PlayerShape 구조체 정의
typedef struct {
    // 와이드 문자열 포인터 배열로, 각 요소는 플레이어 스킨(모양)
    wchar_t* shapes[MAX_SHAPES];
} PlayerShape;

// 플레이어 모양(스킨)을 반환하는 함수 선언
PlayerShape* get_player_shape();

#endif
