#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <errno.h>
#include <fcntl.h>


// 서버와 통신할 포트 번호 설정
#define PORTNUM 12312
// 타임아웃 시간(초)
#define TIMEOUT_SEC 1


typedef struct {
	struct sockaddr_in sin;
	int sd;
	int client_num;
} network;

network network_connect() {

	network nt;

	int client_num;
	struct sockaddr_in sin;
	int sd;
	char buf[50];

	// 서버 주소 구조체 초기화
	memset((char *)&sin, '\0', sizeof(sin));
	sin.sin_family = AF_INET; // IPv4 체계
	sin.sin_port = htons(PORTNUM); // 포트 번호 설정 (네트워크 바이트 순서로 변환)
	sin.sin_addr.s_addr = inet_addr("127.0.0.1"); // 서버 IP 주소 설정 (접속할 때마다 확인 요망)

	// 소켓 생성
	if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		exit(1);
	}

	// 연결 시도 (비동기 모드에서 즉시 반환)
	if (connect(sd, (struct sockaddr *)&sin, sizeof(sin)) == -1) {

		if (errno == EINPROGRESS) { // 연결 진행 중인 상태
			struct timeval timeout; // 타임아웃 설정용 구조체
			fd_set writefds; // 쓰기 가능한 소켓을 탐지하기 위한 파일 디스크

			FD_ZERO(&writefds); // fd_set 추가
			FD_SET(sd, &writefds); // 소켓 디스크립터 추가

			timeout.tv_sec = TIMEOUT_SEC;   // 타임아웃 시간 설정 (1초)
			timeout.tv_usec = 0;
	        
			// select()로 연결 대기
			int res = select(sd + 1, NULL, &writefds, NULL, &timeout);
	        		
	    	if (res == 0) {  // 타임아웃 발생
	    		fprintf(stderr, "Connection timeout: Server IP does not match or server not reachable.\n");
	    		close(sd);
	    		exit(1);
			} else if (res < 0) { // 연결 대기(select)에서 오류 발생
	    		perror("select");
	    		close(sd);
	    		exit(1);
			}

	        		
			if (FD_ISSET(sd, &writefds)) { // 연결 성공
	    		printf("IP 주소 매칭 확인\n");
			}

		} else { // 연결 실패
			perror("connect");
	    		close(sd);
	    		exit(1);
		}

	} else { // 즉시 연결 성공
    	printf("Successfully connected to the server!\n");
	}

	// 소켓을 blocking 모드로 설정
	fcntl(sd, F_SETFL, F_LOCK);

	// 서버에게 연결되었으므로 클라이언트 번호를 요구한다
	memset(buf, '\0', sizeof(buf));
	sprintf(buf, "<<connect>>");
	if (send(sd, buf, sizeof(buf), 0) == -1) {
		perror("send");
		exit(1);
	}
	
	// 서버로부터 클라이언트 고유 번호 수신
	memset(buf, '\0', sizeof(buf));
	if (recv(sd, buf, sizeof(buf), 0) == -1) {
		perror("recv from server --1");
		exit(1);
	}

	// 클라이언트 고유 번호 저장
	client_num = atoi(buf); // 서버로부터 받은 데이터를 정수로 변환
	memset(buf, '\0', sizeof(buf)); // 버퍼 초기화

	nt.sin = sin;
	nt.sd = sd;
	nt.client_num = client_num;

	return nt;
}