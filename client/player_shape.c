#include "player_shape.h"
#include <stdlib.h>
#include <time.h>

PlayerShape* get_player_shape() {
    static PlayerShape player = {
        L"▄︻字═━──",
        L"︻̷═━一",
        L"▄︻デ气═━─",
        L"︻╦╤─ ҉ - - -",
        L"⌐╦╦═─",
        L"▄▄︻┳═──＊ˋ",
        L"▄▄︻══──＊ˋ",
        L"︻╦══╤─",
        L"▄︻̷═━一",
        L"︻̷═━一",
        L"︻╦══╤─",
        L"▄︻▇▇〓",
	L"@(^ㅇ ^)==@",
	L"(ง •̀_•́)ง",
	L"╭( ･ㅂ･)و",
	L"╭( ･ㅂ･)ว",
    };

    // 시드 설정 (한번만 실행되어야 함)
    srand(time(NULL));

    // 모양들을 랜덤하게 섞기
    for (int i = 0; i < MAX_SHAPES; i++) {
        int j = rand() % MAX_SHAPES;
        // 모양을 서로 교환 (Fisher-Yates Shuffle)
        wchar_t* temp = player.shapes[i];
        player.shapes[i] = player.shapes[j];
        player.shapes[j] = temp;
    }

    return &player;
}

