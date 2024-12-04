#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/time.h>

#define PORTNUM 12312
#define MAX_PLAYER 200
#define MATCHING_NUM 4
#define BULLET_NUM 2

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

typedef struct player {
        int x;
        int y;
        int skin;
	int hp;
	int is_dead;
} player;

typedef struct bullet {
	int x, y;
	int dx, dy;
    int is_active;
} bullet;

typedef struct {
	bullet *bullet_info;
} bullet_array;

// 모든 클라이언트들의 정보를 담은 구조체
typedef struct network_player {

	// 클라이언트들의 소켓 번호를 저장하는 배열
	int *ns;

	// 플레이어 정보를 저장하는 배열
	struct player *players;
	int cur_client;

	// 입장한 게임 방의 번호를 저장하는 배열
	int *room_index;

	// 클라이언트들의 총알 정보를 저장하는 배열
	bullet_array *bullets;
} network_player;

typedef struct {
	struct sockaddr_in cli;
	int sd;
} network;

// 게임 방 구조체
typedef struct {
	// 게임에 참여하는 10명의 정보를 담을 구조체 배열
	int client_id[MATCHING_NUM];
	int is_empty;
	int player_num;
} room_info;

pthread_mutex_t m_lock;

// 모든 클라이언트들의 정보를 담은 배열에 접근할 수 있는 포인터 선언
network_player *np;

// 모든 게임방에 대한 정보를 담는 전역 배열 선언
room_info *room;

// 매칭 대기 중인 클라이언트가 10명인지 확인하기 위한 배열
// ready_client 배열 - 클라이언트 고유 번호를 입력받는다
int ready_client[MATCHING_NUM];
int ready_client_num = 0;

// 네트워크 연결이 잘 되었는지 확인하는 함수 network_connec.c
network network_connection();

// 서버가 클라이언트에게 데이터를 전송하는 함수 connect_to_client.c
int connect_to_client(int ns, int cur_client_num, char *buf, int flag);

// 현재 접속 중인 플레이어수를 저장하는 변수
int cur_player = 0;

// 게임에 접속하면 클라이언트와 서버의 데이터 전송을 이 함수가 관여함 recv_send_game_data.c
int recv_send_game_data(network_player *np, char *buf, int cur_client_num, room_info *room, int player_in_room);


// 게임 방 정보를 관리할 쓰레드 함수
void *manage_room(void *vargp) {

	// 우선은 만들 수 있는 방을 전부 다 초기화하고 거기에 매칭된 플레이어들을 할당하는 방식으로 구현
	// 추후에 동적으로 할당하도록 수정할 예정
	int room_num = MAX_PLAYER / MATCHING_NUM;
	int find_room = 0;

	room = (room_info *)malloc(sizeof(room_info) * room_num);

	// 모든 게임 방에 10명의 플레이어 정보를 받을 수 있도록 공간 할당
	for(int i=0; i<room_num; i++) {
		
		for(int j=0; j<MATCHING_NUM; j++) {
			room[i].client_id[j] = -1;
		}
		room[i].is_empty = 1;
		room[i].player_num = 0;
	}

	printf("모든 게임 방 공간 할당 완료, 방 개수 %d\n", room_num);

	while(1) {
		if (ready_client_num == MATCHING_NUM) {
			
			printf("매칭 완료\n");
			
			// 게임 방에 10명의 클라이언트 정보를 보낸다
			
			// 반복문을 돌면서 빈 방이 있는지 찾는다
			for(int i=0; i<room_num; i++) {

				// 빈 방이 있다면 
				if (room[i].is_empty) {

					find_room = 1;
					room[i].is_empty = 0;
					room[i].player_num = MATCHING_NUM;

					pthread_mutex_lock(&m_lock);

					// 해당 방에 대기 중이었던 유저들의 정보를 저장한다
					// ready_client에 인덱스로 접근하면 현재 대기 중인 클라이언트의 고유 번호를 확인할 수 있다
					for (int j = 0; j < MATCHING_NUM; j++) {

						room[i].client_id[j] = ready_client[j];
						np->room_index[ready_client[j]] = i;
						ready_client[j] = -1;
					}

					// 매칭된 유저들의 정보 확인
					for(int j=0; j<MATCHING_NUM; j++) {
						printf("%d번 방에 들어간 클라이언트:%d\n", i, room[i].client_id[j]);
					}

					// 대기열 정보를 초기화하기 전에 우선 대기중인 클라이언트들이 무한반복에서 빠져나와야 한다
					ready_client_num = 0;

					pthread_mutex_unlock(&m_lock);
					break;
				}
			}

			// 방이 꽉 차서 빈 방을 찾지 못했다면
			if (find_room == 0) {
				break;
			} else {
				continue;
			}	
		}
		usleep(10000);
	}

	printf("방이 꽉 찼음\n");
} 




void *threadfunc(void *vargp) {
	char buf[1024];
	char bullet_location[1024];
	int network_status;
	int client_x;
	int client_y;
	int client_skin;
	int client_hp;
	int client_is_dead;
	int cur_client_num = np->cur_client;
	int ns = np->ns[cur_client_num];
	int fd;
	struct timeval t1, t2;
	float check[1000];
	int file_index = 0;

	int bullet_x;
	int bullet_y;
	int bullet_dx;
	int bullet_dy;
    	int bullet_is_active;

	// 클라이언트가 로비에 처음 접속하는지 확인하기 위한 변수
	int access_to_lobby = 1;

	// 클라이언트가 대기열에 있었는지 확인하기 위한 변수
	int is_matching = 0;
	int ready_index;
	

	int recorded_cnt = 0;
	double min = DBL_MAX, max = DBL_MIN, sum = 0.0, avg = 0.0; // 초기값 설정
								   //
	if (cur_client_num == 0) {
		fd = open("log.txt", O_CREAT | O_WRONLY | O_APPEND, 0644);
	}

	while(1) {

		gettimeofday(&t1, NULL);

		memset(buf, '\0', sizeof(buf));
		network_status = recv(ns, buf, sizeof(buf), 0);

		if (network_status == -1) {
			perror("recv");
			break;
		} else if (network_status == 0) {
			break;
		}

		//printf("클라이언트 %d의 메시지: %s\n", cur_client_num, buf);

		// 클라이언트가 서버와 연결될 경우 고유번호를 전송한다
		if (strstr(buf, "GET_CLIENT_UNIQUE_NUM") != NULL) {
			// 클라이언트 연결에 오류가 있거나 오프라인일 경우 0을 반환
			if (connect_to_client(np->ns[cur_client_num], cur_client_num, buf, 1) == 0) {
				break;
			}
			printf("클라이언트  %d이(가) 접속함\n", cur_client_num);
		}

		// 클라이언트가 로비에 접속하는 경우 동접자 수를 늘린다
		// 서버는 동접자 수를 전송한다
		if (strstr(buf, "GET_CONCURRENT_USER") != NULL) {

			// 클라이언트가 로비에 접속한 경우
			if (access_to_lobby) {

				// 동접자를 1 늘린다
				cur_player++;

				// 동접자가 계속 늘어나는 것을 막기 위해 0으로 설정
				access_to_lobby  = 0;
			}

			// 동접자 정보를 클라이언트에게 전송
			memset(buf, '\0', sizeof(buf));
			sprintf(buf, "%d", cur_player);
			if (connect_to_client(np->ns[cur_client_num], cur_client_num, buf, 1) == 0) {
				break;
			}
		}

		// 클라이언트가 매칭을 요청하면 요청 인원이 10명이 될때까지 대기한다
		if (strstr(buf, "GET_READY_USER") != NULL) {

			int status = 1;

			is_matching = 1;

			// 전역 배열에 접근하므로 락을 걸어야 한다
			for(int i=0; i<MATCHING_NUM; i++) {
				if (ready_client[i] == -1) {
					
					pthread_mutex_lock(&m_lock);

					printf("대기열 %d번 위치에 클라이언트 %d의 정보 저장\n", i, cur_client_num);
					// 대기열에 클라이언트 고유 번호 저장
					ready_client[i] = cur_client_num;
					ready_client_num += 1;
					ready_index = i;

					pthread_mutex_unlock(&m_lock);

					break;
				}
			}

			printf("현재 대기 중인 클라이언트의 수 %d\n", ready_client_num);
			
			memset(buf, '\0', sizeof(buf));
			sprintf(buf, "WAIT_FOR_MATCH");
			while(1) {

				if (ready_client_num == MATCHING_NUM) {
					break;
				}
				usleep(3000);
			}

			usleep(3000);

			is_matching = 0;

			// 서버는 매칭이 완료되면 클라이언트에게 GAME_MATCHED 메시지를 보내 클라이언트가 대기상태를 벗어나 게임을 실행하도록 한다
			if (status) {
				memset(buf, '\0', sizeof(buf));
				sprintf(buf, "GAME_MATCHED\n");

				printf("클라이언트 %d가 %d번 방에 매칭되었음\n", cur_client_num, np->room_index[cur_client_num]);
				
				// 클라이언트에게 매칭이 되었으므로 게임으로 넘어가라는 메시지를 전송한다
				if (connect_to_client(np->ns[cur_client_num], cur_client_num, buf, 1) == 0) {
					break;
				}

			} else {

				pthread_mutex_lock(&m_lock);

				// 클라이언트가 매칭 중에 연결을 취소한 경우
				ready_client[ready_index] = -1;
				ready_client_num -= 1;
				printf("매칭 취소 현재 대기 중인 클라이언트의 수 %d\n", ready_client_num);
				
				pthread_mutex_unlock(&m_lock);

				break;
			}
		}
	

		// 클라이언트가 게임에 접속하는 경우
		if (strstr(buf, "ACCESS_TO_GAME") != NULL) {

			if (recv_send_game_data(np, buf, cur_client_num, &room[np->room_index[cur_client_num]], MATCHING_NUM) == 0) {
				break;
			}
		}
        
		// 클라이언트가 총알 정보를 보내는 경우
		if (strstr(buf, "LOCAL_BULLET_INFO") != NULL) {
                // 총알 정보를 초기화 (클라이언트 번호에 해당하는 총알 정보 초기화)
            	//memset(np->bullets[cur_client_num].bullet_info, 0, sizeof(np->bullets[cur_client_num].bullet_info));
            
			// 클라이언트로부터 총알 정보들을 받아서 갱신한다
			int bullet_index = 0;
			char *line = strtok(buf, "\n");
			while(line != NULL) {
				if (sscanf(buf, "LOCAL_BULLET_INFO,x=%d,y=%d,dx=%d,dy=%d,is_active=%d",
					&bullet_x, &bullet_y, &bullet_dx, &bullet_dy, &bullet_is_active) == 5) {
					
					if (bullet_is_active) {
						np->bullets[cur_client_num].bullet_info[bullet_index].x = bullet_x;
						np->bullets[cur_client_num].bullet_info[bullet_index].y = bullet_y;
						np->bullets[cur_client_num].bullet_info[bullet_index].dx = bullet_dx;
						np->bullets[cur_client_num].bullet_info[bullet_index].dy = bullet_dy;
                    				np->bullets[cur_client_num].bullet_info[bullet_index].is_active = bullet_is_active;
					} else {
						np->bullets[cur_client_num].bullet_info[bullet_index].x = -1;
						np->bullets[cur_client_num].bullet_info[bullet_index].y = -1;
						np->bullets[cur_client_num].bullet_info[bullet_index].dx = 0;
						np->bullets[cur_client_num].bullet_info[bullet_index].dy = 0;
                    				np->bullets[cur_client_num].bullet_info[bullet_index].is_active = bullet_is_active;
					}

					bullet_index++;
				}
				line = strtok(NULL, "\n");
			}
			
			memset(bullet_location, '\0', sizeof(bullet_location));
			for(int i=0; i<MATCHING_NUM; i++) {
	
				// 네트워크가 연결되어있다면
				if (np->ns[room[np->room_index[cur_client_num]].client_id[i]] > 0) {
					// 해당 클라이언트의 총알 정보를 읽어서 하나의 배열에 저장한다
					// 너무 변수 인덱스들이 많아져서 수정 좀 해야할듯
					for (int j=0; j<BULLET_NUM; j++) {
						memset(buf, '\0', sizeof(buf));
						sprintf(buf, "%d,x=%d,y=%d,dx=%d,dy=%d,is_active=%d\n", 
							room[np->room_index[cur_client_num]].client_id[i], 
							np->bullets[room[np->room_index[cur_client_num]].client_id[i]].bullet_info[j].x,
							np->bullets[room[np->room_index[cur_client_num]].client_id[i]].bullet_info[j].y,
							np->bullets[room[np->room_index[cur_client_num]].client_id[i]].bullet_info[j].dx,
							np->bullets[room[np->room_index[cur_client_num]].client_id[i]].bullet_info[j].dy,
							np->bullets[room[np->room_index[cur_client_num]].client_id[i]].bullet_info[j].is_active
						);

						strncat(bullet_location, buf, strlen(buf));
					}
				}
				
			}

			//printf("<<bullet>>\n%s\n", bullet_location);

			// 서버와 통신 중인 클라이언트를 제외한 나머지 클라이언트들의 총알 정보를 전송
			if (connect_to_client(np->ns[cur_client_num], cur_client_num, bullet_location, 2) == 0) {
				break;
			}
		}


		// 클라이언트가 사망했을 경우
		if (strstr(buf, "PLAYER_IS_DEAD") != NULL) {

			// 게임 방에서 해당 클라이언트의 정보를 찾는다
			for(int i=0; i<MATCHING_NUM; i++) {
				if (room[np->room_index[cur_client_num]].client_id[i] == cur_client_num) {
					
					// 게임 방에서 해당 플레이어를 제거한다
					room[np->room_index[cur_client_num]].client_id[i] = -1;
					
					// 게임 방에서 플레이 중인 유저 수도 감소시킨다
					
					pthread_mutex_lock(&m_lock);

					room[np->room_index[cur_client_num]].player_num -= 1;

					pthread_mutex_unlock(&m_lock);

					// 플레이어 정보도 초기화한다
					np->players[cur_client_num].x = -1;
					np->players[cur_client_num].y = -1;
					np->players[cur_client_num].skin = 0;
					np->players[cur_client_num].hp = 0;
					np->players[cur_client_num].is_dead = 0;

					// 해당 플레이어가 발사한 총알 정보도 초기화한다 
					for(int j=0; j<BULLET_NUM; j++) {
						np->bullets[cur_client_num].bullet_info[j].x = -1;
						np->bullets[cur_client_num].bullet_info[j].y = -1;
						np->bullets[cur_client_num].bullet_info[j].dx = 0;
						np->bullets[cur_client_num].bullet_info[j].dy = 0;
						np->bullets[cur_client_num].bullet_info[j].is_active = 0;
					}
				}
			}

			// 해당 게임방이 비었다면
			if (room[np->room_index[cur_client_num]].player_num == 0) {
				printf("클라이언트 %d가 %d방에서 마지막으로 사망함\n", cur_client_num, np->room_index[cur_client_num]);
				room[np->room_index[cur_client_num]].is_empty = 1;
			}

			// 해당 플레이어에게 배정된 게임 방 번호도 초기화
			np->room_index[cur_client_num] = -1;
		}

		gettimeofday(&t2, NULL);
		/*	
		if (cur_client_num == 0) {
    			if (file_index == 1000) {
        			// 마지막 처리를 위해 파일에 기록
        			avg = sum / recorded_cnt;

        			char buf_file1[50];
        			sprintf(buf_file1, "min=%lf,max=%lf,avg=%lf\n", min, max, avg);

        			write(fd, buf_file1, strlen(buf_file1));

        			close(fd);
			} else {
        			// 새 데이터를 기록
        			double current_time = (t2.tv_sec + (t2.tv_usec * 0.000001)) - (t1.tv_sec + (t1.tv_usec * 0.000001));
        			check[file_index] = current_time;
        			file_index++;

        			// 실시간 업데이트
        			if (current_time < min) {
            				min = current_time;
        			}
        			if (current_time > max) {
            				max = current_time;
        			}
        			sum += current_time;
        			recorded_cnt++;

        			avg = sum / recorded_cnt;

        			// 결과 출력
        			printf("파일 인덱스 %d %lf %s\n", file_index, current_time, buf);
        			char buf_file[200];
        			sprintf(buf_file, "time=%lf, min=%lf, max=%lf, avg=%lf\n", current_time, min, max, avg);

        			write(fd, buf_file, strlen(buf_file));
				fsync(fd);
    			}
		}
		*/
	}

	
	// 클라이언트가 매칭 도중 오프라인 상태가 되었다면
	if (is_matching) {
		ready_client[ready_index] = -1;

		pthread_mutex_lock(&m_lock);
		ready_client_num -= 1;
		pthread_mutex_unlock(&m_lock);
		printf("매칭 취소 현재 대기 중인 클라이언트의 수 %d\n", ready_client_num);
		is_matching = 0;
	}

	// 클라이언트가 동접자 수에 포함됐었다면 동접자 수를 줄이고, 그렇지 않다면 동접자 수를 그대로 유지한다
	if (access_to_lobby != 1) {

		pthread_mutex_lock(&m_lock);
		cur_player -= 1;
		pthread_mutex_unlock(&m_lock);
	}

	// 클라이언트가 게임 중에 접속을 끊었다면 해당 게임 방의 플레이어 정보를 초기화한다
	for(int i=0; i<MATCHING_NUM; i++) {

		// 클라이언트가 플레이하던 게임 방에서 자신의 정보를 찾을 경우
		if (room[np->room_index[cur_client_num]].client_id[i] == cur_client_num) {
			room[np->room_index[cur_client_num]].client_id[i] = -1;
			room[np->room_index[cur_client_num]].player_num -= 1;
		}

		for(int j=0; j<BULLET_NUM; j++) {
			np->bullets[cur_client_num].bullet_info[j].x = -1;
			np->bullets[cur_client_num].bullet_info[j].y = -1;
			np->bullets[cur_client_num].bullet_info[j].dx = 0;
			np->bullets[cur_client_num].bullet_info[j].dy = 0;
			np->bullets[cur_client_num].bullet_info[j].is_active = 0;
		}
	}

	// 클라이언트가 마지막으로 방에서 나간다면 방 정보를 초기화한다
	if (room[np->room_index[cur_client_num]].player_num == 0) {
		printf("클라이언트 %d가 %d번 방에서 마지막으로 나감\n", cur_client_num, np->room_index[cur_client_num]);

		pthread_mutex_lock(&m_lock);
		room[np->room_index[cur_client_num]].is_empty = 1;
		pthread_mutex_unlock(&m_lock);

	}


	// 클라이언트가 오프라인 상태이므로 해당 클라이언트가 쓰던 정보들을 초기화 
	printf("클라이언트 %d가 접속을 종료함\n", cur_client_num);
	close(np->ns[cur_client_num]);
	np->ns[cur_client_num] = 0;
	np->players[cur_client_num].x = -10;
	np->players[cur_client_num].y = -10;

	return NULL;
}

int main() {
	struct sockaddr_in cli;
	int sd;
	int *ns;
	int tid_count = 0;
	int clientlen;
	pthread_t tid[MAX_PLAYER];
	pthread_t room_manager;
	int temp_ns;
	int ns_accept;

	pthread_mutex_init(&m_lock, NULL);
	
	// 서버의 소켓 준비 코드
	network nt;
	nt = network_connection();
	sd = nt.sd;
	cli = nt.cli;
	clientlen = sizeof(cli);

	// 클라이언트 정보 저장할 배열 준비
	player *p = (player *)malloc(sizeof(player) * MAX_PLAYER);
	np = (network_player *)malloc(sizeof(network_player));
	ns = (int *)malloc(sizeof(int) * MAX_PLAYER);
	int *room_index = (int *)malloc(sizeof(int) * MAX_PLAYER);
	bullet_array *bullets = (bullet_array *)malloc(sizeof(bullet_array) * MAX_PLAYER);

	// 클라이언트들의 총알 정보를 저장할 배열 준비
	for(int i=0; i<MAX_PLAYER; i++) {
		bullet *bullet_info;
		bullet_info = (bullet *)malloc(sizeof(bullet) * BULLET_NUM);

		for(int j=0; j<BULLET_NUM; j++) {
			bullet_info[j].x = -1;
			bullet_info[j].y = -1;
			bullet_info[j].dx = -1;
			bullet_info[j].dy = -1;
		}

		bullets[i].bullet_info = bullet_info;
	}

	// 처음에는 모든 클라이언트가 배정된 게임방이 없으므로 -1로 초기화한다
	for(int i=0; i<MAX_PLAYER; i++) {
		room_index[i] = -1;
	}

	for(int i=0; i<MAX_PLAYER; i++) {
		ns[i] = 0;
	}

	np->players = p;
	np->ns = ns;
	np->room_index = room_index;
	np->bullets = bullets;

	// 대기열 초기화
	for(int i=0; i<MATCHING_NUM; i++) {
		ready_client[i] = -1;
	}

	pthread_create(&room_manager, NULL, manage_room, NULL);

	while(1) {

		// 서버는 클라이언트가 접속을 요청할 경우
		// ns 배열이 비었는지 보고 빈 곳이 있다면 그곳에 넣는다
		temp_ns = accept(sd, (struct sockaddr *)&cli, &clientlen);
		if (temp_ns == -1) {
			perror("accept");
			exit(1);
			break;
		}

		ns_accept = 0;

		for(int i=0; i<MAX_PLAYER; i++) {
			if (np->ns[i] == 0) {
				pthread_mutex_lock(&m_lock);
				np->ns[i] = temp_ns;
				np->cur_client = i;
				ns_accept = 1;
				pthread_mutex_unlock(&m_lock);
				break;
			}
		}

		if (ns_accept == 0) {
			printf("FULL!!\n");
			break;
		}

		// 쓰레드 생성
		if (tid_count < MAX_PLAYER) {
			if (pthread_create(&tid[tid_count], NULL, threadfunc, NULL) != 0) {
				perror("pthread_create");
				break;
			} else {
				tid_count++;
			}
		} else {
			printf("Maximum number of clients reached.\n");
		}
	}

	for(int i=0; i<tid_count; i++) {
		pthread_join(tid[i], NULL);
	}

	close(*ns);
	free(ns);
	free(p);
	free(np);

	return 0;
}
