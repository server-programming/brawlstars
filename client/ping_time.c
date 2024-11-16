#include <sys/time.h>
#include <stdio.h>
#include "ping_time.h"

// 주어진 두 시간의 차이를 밀리초로 반환
long long get_ms(struct timeval start, struct timeval end) {
   return (end.tv_sec - start.tv_sec) * 1000 + (end.tv_usec - start.tv_usec) / 1000;
}
