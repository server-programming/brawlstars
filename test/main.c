#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <time.h>

#define NUM_THREADS 127 // 동시 실행할 프로세스 개수

// 스레드 종료 플래그
volatile int keep_running = 1;

// 무작위 선택 함수
char random_choice(const char *choices, int size) {
    return choices[rand() % size];
}

// SIGINT 신호 처리 함수
void handle_sigint(int sig) {
    printf("\nSIGINT(Ctrl+C) 감지, 모든 스레드 종료 중...\n");
    keep_running = 0; // 실행 중단 플래그 설정
}

// 스레드에서 실행할 함수
void *run_process(void *arg) {
    // 실행 파일 경로
    const char *command = "../client/brawlstars";

    // 프로세스를 실행하고 쓰기 모드로 열기
    FILE *fp = popen(command, "w");
    if (fp == NULL) {
        perror("popen 실패");
        pthread_exit(NULL);
    }

    // 첫 번째 입력: 1 -> [1, 2, 3, 4] 중 하나 -> Enter
    const char choices_1[] = {'1', '2', '3', '4'};
    fprintf(fp, "1\n"); // 첫 번째 입력: 1
    fflush(fp);
    usleep(100000);

    char choice1 = random_choice(choices_1, sizeof(choices_1)); // 두 번째 입력: [1, 2, 3, 4] 중 하나
    fprintf(fp, "%c\n", choice1);
    fflush(fp);
    usleep(100000);

    fprintf(fp, "\n"); // Enter
    fflush(fp);
    usleep(100000);

    // 두 번째 입력: [w, a, s, d, Enter] 반복 100회
    const char choices_2[] = {'w', 'a', 's', 'd', '\n'};
    for (int i = 0; i < 100; i++) {
        if (!keep_running) { // 종료 신호 감지 시 중단
            break;
        }
        char choice2 = random_choice(choices_2, sizeof(choices_2));
        if (choice2 == '\n') {
            fprintf(fp, "\n"); // Enter 처리
        } else {
            fprintf(fp, "%c\n", choice2);
        }
        fflush(fp);
        usleep(100000); // 0.1초 대기
    }

    // 프로세스 닫기
    if (pclose(fp) == -1) {
        perror("pclose 실패");
    }

    printf("프로세스 종료\n");
    pthread_exit(NULL);
}

int main() {
    pthread_t threads[NUM_THREADS];
    int i;

    // 난수 초기화
    srand(time(NULL));

    // SIGINT 신호 처리기 설정
    signal(SIGINT, handle_sigint);

    // 스레드 생성
    for (i = 0; i < NUM_THREADS; i++) {
        if (pthread_create(&threads[i], NULL, run_process, NULL) != 0) {
            perror("pthread_create 실패");
            return 1;
        }
    }

    // 모든 스레드가 종료되길 기다림
    for (i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("모든 프로세스 실행 완료\n");
    return 0;
}
