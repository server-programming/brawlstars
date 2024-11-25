#include "player_shape.h"
#include <stdlib.h>
#include <time.h>

PlayerShape* get_player_shape() {
    // 플레이어의 스킨(모양) 리스트를 초기화
    static PlayerShape player = {{
    	L"@(^ㅇ ^)==@",
	    L"(ง •̀_•́)ง",
	    L"╭( ･ㅂ･)و",
	    L"╭( ･ㅂ･)ว",
    }};
 
    // 플레이어 모양 데이터를 변환
    return &player;
}