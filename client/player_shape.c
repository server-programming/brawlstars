#include "player_shape.h"
#include <stdlib.h>
#include <time.h>

PlayerShape* get_player_shape() {
    // 플레이어의 스킨(모양) 리스트를 초기화
    static PlayerShape player = {{
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
    }};

    // 시드 설정 (현재 시간 기준)
    srand(time(NULL));

    // 모양들을 랜덤하게 섞기
    for (int i = 0; i < MAX_SHAPES; i++) {
        // 0 ~ MAX-SHAPES-1 사이의 난수를 설정
        int j = rand() % MAX_SHAPES; 

        // 모양을 서로 교환 (Fisher-Yates Shuffle)
        wchar_t* temp = player.shapes[i];
        player.shapes[i] = player.shapes[j];
        player.shapes[j] = temp;
    }
    
    // 플레이어 모양 데이터를 변환
    return &player;
}

