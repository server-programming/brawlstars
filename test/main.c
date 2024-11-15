#include <ncurses.h>
#include <locale.h>
#include <wchar.h>  // wchar_t 타입을 위한 헤더 추가
#include <unistd.h> // sleep 함수 사용을 위한 헤더 추가
#include <string.h>

// 아스키 아트를 출력하는 함수들

// 첫 번째 아스키 아트: 장식용 아스키 아트
void print_art1(int row, int col) {
    const wchar_t* art[] = {
        L"/̵͇̿̿/’̿’̿ ̿ ̿̿ ̿̿ ̿̿ ⠀.𖥔 ݁ ˖ ⌖ ₊    ",
        L"⠀    /̵͇̿̿/’̿’̿ ̿ ̿̿ ̿̿ ̿̿⠀⊹ 𖥔   ⌖"
    };
    for (int i = 0; i < 2; i++) {
        mvaddwstr(row + i, (col - wcslen(art[i])) / 2, art[i]);
    }
}

// 두 번째 아스키 아트: 전투용 아스키 아트
void print_art2(int row, int col) {
    const wchar_t* art[] = {
        L"  _/﹋\\_",
        L"  (҂`_´)",
        L"       <,︻╦╤─ ҉ - -",
        L"_/﹋\\"
    };
    for (int i = 0; i < 4; i++) {
        mvaddwstr(row + i, (col - wcslen(art[i])) / 2, art[i]);
    }
}

// 세 번째 아스키 아트: 상자에 들어간 텍스트와 캐릭터
void print_art3(int row, int col, const wchar_t* text) {
    const wchar_t* top_bottom = L"|￣￣￣￣￣￣￣￣￣￣￣￣￣￣|";
    const wchar_t* line1 = L"|＿＿＿＿＿＿＿＿＿＿＿＿＿＿|";
    const wchar_t* line2 = L"              \\ (•◡•) /";
    const wchar_t* line3 = L"              \\    /";

    mvaddwstr(row, (col - wcslen(top_bottom)) / 2, top_bottom);
    mvaddwstr(row + 1, (col - wcslen(text)) / 2, text);  // 사용자 텍스트 출력
    mvaddwstr(row + 2, (col - wcslen(line1)) / 2, line1);
    mvaddwstr(row + 3, (col - wcslen(line2)) / 2, line2);
    mvaddwstr(row + 4, (col - wcslen(line3)) / 2, line3);
}

// 아스키 아트를 출력하는 함수 선택
void print_ascii_art(int art_type, int row, int col, const wchar_t* text) {
    switch (art_type) {
        case 1:
            print_art1(row, col);
            break;
        case 2:
            print_art2(row, col);
            break;
        case 3:
            print_art3(row, col, text);
            break;
        default:
            break;
    }
}

int main() {
    setlocale(LC_ALL, ""); // 로케일 설정
    initscr(); // ncurses 초기화
    cbreak(); // 즉시 입력 모드 설정
    noecho(); // 입력된 문자를 화면에 표시하지 않음
    keypad(stdscr, TRUE); // 특수 키 입력 허용
    setlocale(LC_CTYPE, "");

    // 원하는 위치에서 아스키 아트를 출력
    print_ascii_art(1, LINES / 2 - 4, COLS, NULL);  // 첫 번째 아트
    print_ascii_art(2, LINES / 2 + 6, COLS, NULL);  // 두 번째 아트
    print_ascii_art(3, LINES / 2 + 12, COLS, L"Your text here");  // 세 번째 아트

    refresh(); // 화면 갱신
    sleep(5); // 5초 대기

    endwin(); // ncurses 종료
    return 0;
}

