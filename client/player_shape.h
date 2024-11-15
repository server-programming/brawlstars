#ifndef PLAYER_SHAPE_H
#define PLAYER_SHAPE_H

#include <wchar.h>

#define MAX_SHAPES 18

typedef struct {
    wchar_t* shapes[MAX_SHAPES];
} PlayerShape;

PlayerShape* get_player_shape();

#endif
