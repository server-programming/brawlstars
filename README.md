# Brawlstars — 터미널 탑다운 슈팅 게임

![title](https://assetsio.gnwcdn.com/brawl_R1bhBto.jpg?width=1200&height=1200&fit=bounds&quality=70&format=jpg&auto=webp)

C 언어와 TCP 소켓 통신 기반의 터미널 멀티플레이어 슈팅 게임입니다.  
최대 4명이 실시간으로 접속하여 최후의 1인을 가리는 배틀로얄 방식으로 진행됩니다.

---

## 게임 소개

| 키 | 동작 |
|---|---|
| `W` `A` `S` `D` | 이동 |
| `Enter` | 사격 |
| `Esc` | 게임 종료 |

---

## 팀원

| 이름 | 역할 |
|---|---|
| 본인 | 클라이언트 (게임 UI, 렌더링, 입력 처리) |
| 김윤태 | 서버 (소켓 통신, 매칭, 게임 데이터 동기화) |

---

## 기술 스택

| 분류 | 내용 |
|---|---|
| 언어 | C |
| UI | ncurses / ncursesw |
| 네트워크 | TCP 소켓 (POSIX) |
| 동시성 | pthread |
| 빌드 | Makefile |

---

## 서버 설계

### 1. 통신 프로토콜

클라이언트와 서버는 커스텀 텍스트 프로토콜로 통신합니다.  
모든 메시지는 문자열 기반이며 `\n`으로 구분됩니다.

| 방향 | 메시지 | 설명 |
|---|---|---|
| C → S | `GET_CLIENT_UNIQUE_NUM` | 접속 시 고유 번호 요청 |
| C → S | `GET_CONCURRENT_USER` | 동접자 수 요청 |
| C → S | `GET_READY_USER` | 매칭 대기 요청 |
| C → S | `ACCESS_TO_GAME,x=%d,y=%d,skin=%d,hp=%d,is_dead=%d` | 매 프레임 플레이어 상태 전송 |
| C → S | `LOCAL_BULLET_INFO,x=%d,y=%d,dx=%d,dy=%d,is_active=%d` | 총알 상태 전송 |
| C → S | `PLAYER_IS_DEAD` | 플레이어 사망 통보 |
| S → C | `GAME_MATCHED` | 매칭 성사 통보 |
| S → C | `%d,x=%d,y=%d,skin=%d,hp=%d,is_dead=%d,enemy=%d` | 다른 플레이어 상태 브로드캐스트 |

### 2. 매칭 알고리즘

4명이 모일 때까지 서버가 대기열을 관리하며, 정원이 채워지면 방을 배정합니다.

```
클라이언트 접속
      │
      ▼
고유 번호 발급 (client_num)
      │
      ▼
GET_READY_USER 수신 → 대기열 추가
      │
      ▼
대기열 인원 == 4 ?
   YES → room_info 생성 → GAME_MATCHED 브로드캐스트
   NO  → 대기
```

`room_info` 구조체로 방을 관리합니다.

```c
typedef struct room_info {
    int client_id[MATCHING_NUM]; // 방에 속한 클라이언트 번호
    int is_empty;                // 방 사용 여부
    int player_num;              // 현재 생존 플레이어 수
} room_info;
```

### 3. 게임 상태 동기화

매 프레임마다 아래 순서로 상태를 동기화합니다.

```
클라이언트                          서버
    │                               │
    │── ACCESS_TO_GAME (내 상태) ──▶│  players[cur_client] 갱신
    │                               │
    │◀─ 같은 방 전체 플레이어 상태 ──│  room 내 4명 순회하여 브로드캐스트
    │                               │
    │── LOCAL_BULLET_INFO ─────────▶│  총알 상태 수신
    │◀─ 원격 총알 상태 ─────────────│  다른 클라이언트 총알 전달
```

### 4. 총알 처리 방식

- **로컬 총알**: 클라이언트에서 이동 및 충돌 계산 후 서버로 전송
- **원격 총알**: 서버 경유로 수신하여 클라이언트에서 렌더링
- 총알-맵 충돌 및 총알-플레이어 충돌 모두 클라이언트에서 판정

### 5. 부하 테스트

`test/main.c`에서 pthread 127개를 생성하여 멀티플레이 환경을 자동 시뮬레이션합니다.  
각 스레드는 클라이언트 프로세스를 실행하고 WASD 및 Enter 입력을 자동으로 전송합니다.

```c
#define NUM_THREADS 127

for (i = 0; i < NUM_THREADS; i++) {
    pthread_create(&threads[i], NULL, run_process, NULL);
}
```

---

## 프로젝트 구조

```
brawlstars/
├── client/
│   ├── main.c                # 진입점, 메뉴 루프
│   ├── network_connect.c     # 서버 연결
│   ├── lobby.c               # 매칭 대기, 스킨 선택
│   ├── game.c                # 게임 루프
│   ├── player.c              # 플레이어 이동, 렌더링, 송수신
│   ├── bullet.c              # 총알 발사, 충돌, 송수신
│   ├── map.c                 # 맵 초기화, 충돌 판정
│   ├── player_shape.c        # 캐릭터 스킨 정의
│   ├── start_menu.c          # 시작 메뉴 UI
│   ├── help.c                # 조작법 안내
│   ├── ping_time.c           # 핑 계산
│   ├── text_align.c          # 텍스트 중앙 정렬
│   ├── ascii_art.c           # 아스키 아트
│   └── Makefile
├── server/
│   ├── main.c                # 클라이언트 수신, 매칭, 게임 진행
│   ├── network_connect.c     # 소켓 바인딩, 리슨
│   ├── connect_to_client.c   # 클라이언트 데이터 전송
│   ├── recv_send_game_data.c # 게임 상태 동기화
│   └── Makefile
├── test/
│   ├── main.c                # 멀티플레이 시뮬레이션
│   └── Makefile
└── audio_files/              # 효과음 및 배경음악
```

---

## 빌드 및 실행

### 요구 사항

- Linux 환경
- 터미널 크기 **120×40 이상** 권장

```bash
sudo apt install libncurses5-dev libncursesw5-dev
```

### 실행 순서

**1. 서버 실행**
```bash
cd server && make && ./server
```

**2. 클라이언트 실행 (4개 터미널에서 각각)**
```bash
cd client && make && ./brawlstars
```

4명이 모두 접속하면 자동으로 게임이 시작됩니다.

### 테스트

```bash
cd test && make && ./test
```
