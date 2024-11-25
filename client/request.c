#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

int get_concurrent_users(int sd) {
    int concurrent_users = 0;
    char buf[50];
    
    // 서버에게 동접자 수 요청
    memset(buf, '\0', sizeof(buf));
    sprintf(buf, "<<concurrent_users>>");
    if (send(sd, buf, sizeof(buf), 0) == -1) {
        perror("send");
        return -1;
    }
    
    // 서버로부터 동접자 수 수신
    memset(buf, '\0', sizeof(buf));
    if (recv(sd, buf, sizeof(buf), 0) == -1) {
        perror("recv");
        return -1;
    }    

    concurrent_users = atoi(buf);
    return concurrent_users;
}

int get_is_matched(int sd) {
    int is_matched = 0;
    char buf[50];
    
    // 서버에게 대기열 유저 수 요청
    memset(buf, '\0', sizeof(buf));
    sprintf(buf, "<<is_matched>>");
    if (send(sd, buf, sizeof(buf), 0) == -1) {
        perror("send");
        return -1;
    }

    // 서버로부터 대기열 유저 수 수신
    if (recv(sd, buf, sizeof(buf), 0) == -1) {
        perror("recv");
        return -1;
    }
    
    is_matched = atoi(buf);
    return is_matched;
}
