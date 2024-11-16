#ifndef PING_TIME_H
#define PING_TIME_H

// 주어진 두 시간의 차이를 밀리초로 계산
long long get_ms(struct timeval start, struct timeval end);

#endif
