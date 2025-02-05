#include <vector>
#include <iostream>
#include <windows.h>
#include <chrono>
#include <conio.h>

using namespace std;
using namespace std::chrono_literals;

#define snakeBodyASCII 186
#define snakeBodyHorizontalASCII 205
#define snakeTurningLeftUpASCII 187 
#define snakeTurningLeftDownASCII 188 
#define snakeTurningRightDownASCII 200
#define snakeTurningRightUpASCII 201
#define backGroudASCII 32
#define appleASCII 254

#ifdef _WIN32
    LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        switch (uMsg) {
            // Zablokuj zmianę rozmiaru okna
            case WM_SIZING:
            case WM_SIZE:
                return 0; // Ignoruj komunikaty zmiany rozmiaru

            // Zablokuj maksymalizację i minimalizację
            case WM_SYSCOMMAND:
                if (wParam == SC_MAXIMIZE || wParam == SC_MINIMIZE || wParam == SC_RESTORE) {
                    return 0; // Ignoruj komunikaty maksymalizacji/minimalizacji
                }
                break;
        }

        // Domyślna obsługa komunikatów
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }    
#endif

void setConsoleSize(int width, int height) {
    #ifdef _WIN32
        // TODO 
    #else 
        throw "Not implemented for this platform";
    #endif
}

bool isRunningAsAdmin() {
    #ifdef _WIN32
        bool isAdmin = false;
        HANDLE hToken = NULL;

        if ( OpenProcessToken( GetCurrentProcess(), TOKEN_QUERY, &hToken) ) {
            TOKEN_ELEVATION elevation;
            DWORD size;
            if ( GetTokenInformation(hToken, 
            TokenElevation, &elevation, sizeof(elevation), &size) ) {
                isAdmin = elevation.TokenIsElevated;
            }
        }

        if ( hToken )
            CloseHandle( hToken );
        
        return isAdmin;
    #else 
        throw "Not implemented for this platform";
    #endif
}

void restartAsAdmin() {
    #ifdef _WIN32
        char path[MAX_PATH];
        GetModuleFileName(NULL, path, MAX_PATH);

        SHELLEXECUTEINFO sei = { sizeof(sei) };
        sei.lpVerb = "runas";
        sei.lpFile = path;
        sei.hwnd = NULL;
        sei.nShow = SW_NORMAL;

        if (!ShellExecuteEx(&sei)) {
            std::cerr << "Can't run as an administrator !\n";
        }
    #else 
        throw "Not implemented for this platform";
    #endif
}   

void disableConsoleResizing() {
    #ifdef _WIN32
        HWND hwnd = GetConsoleWindow();
        if (hwnd == NULL) {
            return;
        }

        SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)WindowProc);

        LONG style = GetWindowLong(hwnd, GWL_STYLE);
        style &= ~(WS_THICKFRAME | WS_MAXIMIZEBOX | WS_MINIMIZEBOX);
        SetWindowLong(hwnd, GWL_STYLE, style);

        SetWindowPos(hwnd, NULL, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_FRAMECHANGED);
    #else 
        throw "Not implemented for this platform";
    #endif
}

bool getInput(char * key) { 
    #ifdef _WIN32
        if(kbhit()) {
            *key = getch();    
            return true;
        }

        return false;
    #else 
        throw "Not implemented for this platform";
    #endif
}

void getConsoleSize(int * width, int * height) {
    #ifdef _WIN32
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
        *width = csbi.dwSize.X;
        *height = csbi.dwSize.Y;
    #else 
        throw "Not implemented for this platform";
    #endif
}

void moveCursor(int x, int y) {
    #ifdef _WIN32
        HANDLE consoleHandle = GetStdHandle((DWORD)STD_OUTPUT_HANDLE);

        SetConsoleCursorPosition(
            consoleHandle,
            COORD{(short)x, (short)y}
        );
    #else 
        throw "Not implemented for this platform";
    #endif
}

void hideCursor() {
    #ifdef _WIN32
        HANDLE hStdOut = NULL;
        CONSOLE_CURSOR_INFO curInfo;

        hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
        GetConsoleCursorInfo(hStdOut, &curInfo);
        curInfo.bVisible = FALSE;
        SetConsoleCursorInfo(hStdOut, &curInfo);
    #else 
        throw "Not implemented for this platform";
    #endif
}

struct Position {
    int x;
    int y;
};

class SnakeBody {
public:
    vector<vector<char>>* gridPtr;
    Position position;
    char body;
    SnakeBody * next;
    SnakeBody * prev;
    int direction = 0; // 0 - down; 1 - left; 2 - up; 3 - right;

    SnakeBody(char body, SnakeBody * prev, vector<vector<char>> * grid, int y, int x) {
        this->body = body;
        this->prev = prev;
        this->next = nullptr;
        this->gridPtr = grid;
        this->position.x = x;
        this->position.y = y;

        // set char in grid
        this->gridPtr->data()[y][x] = this->body;
    }

    Position move(int x, int y) {
        Position _ret = this->position;
        this->gridPtr->data()[this->position.y][this->position.x] = backGroudASCII;

        this->position.x = x;
        this->position.y = y;
        
        if (this->prev != nullptr) {
            Position prevPos = this->prev->position;
            // if we go to the left or right normally
            if(prevPos.x - 1 == this->position.x || prevPos.x + 1 == this->position.x) {
                this->body = snakeBodyHorizontalASCII;
            }

            // if we go to the up or down normally
            if(prevPos.y - 1 == this->position.y || prevPos.y + 1 == this->position.y) {
                this->body = snakeBodyASCII;
            }
        }
        this->gridPtr->data()[this->position.y][this->position.x] = this->body;
        return _ret;
    }
};

void moveSnake(SnakeBody * head) {
    int prevX = head->position.x, prevY = head->position.y;
    switch(head->direction) {
        case 0: {
            prevY += 1;
        } break;

        case 1: {
            prevX -= 1;
        } break;

        case 2: {
            prevY -= 1;
        } break;

        case 3: {
            prevX += 1;
        }
    }

    while(head != nullptr) {
        Position prevPos = head->move(prevX, prevY);
        prevX = prevPos.x;
        prevY = prevPos.y;

        head = head->next;
    }
}

int main() {
    if (!isRunningAsAdmin()) {
        restartAsAdmin();
        return 0;
    }

    disableConsoleResizing();
    hideCursor();
    setConsoleSize(250, 250);
    system("cls");

    vector<vector<char>> grid 
        = vector<vector<char>>
        (21, vector<char>(36, char(backGroudASCII)));
    
    for (int i = 0; i < 21; i++)
        grid[i][35] = char(snakeBodyASCII);

    for (int i = 0; i < 36; i++)
        grid[20][i] = char(snakeBodyHorizontalASCII);
    
    SnakeBody head = SnakeBody(char(appleASCII), nullptr, &grid, 1, 0);
    SnakeBody * it = &head;
    for (int i = 0; i < 20; i++) {
        it->next = new SnakeBody(char(snakeBodyASCII), it, &grid, 1, 0);
        it = it->next;
    }

    auto start = chrono::high_resolution_clock::now();
    auto updateTimerStart = start;
    while (true)
    {
        // Check input
        char key = ' ';
        if (getInput(&key)) {
            if (tolower(key) == 'q' || key == 27) {
                break;
            } else if (tolower(key) == 'w') {
                head.direction = 2;
            } else if (tolower(key) == 'a') {
                head.direction = 1;
            } else if (tolower(key) == 's') {
                head.direction = 0;
            } else if (tolower(key) == 'd') {
                head.direction = 3;
            }
        }

        // Update timer
        auto currTime = chrono::high_resolution_clock::now();
        auto timePassed = currTime - updateTimerStart;
        if(chrono::duration_cast<chrono::milliseconds>(timePassed).count() >= ((head.direction == 0 || head.direction == 2) ? 300.f : 200.f)) {
            moveSnake(&head); 
            updateTimerStart = currTime;
        }

        // Draw
        moveCursor(0, 0);
        for (auto row : grid) {
            for (auto el : row) {
                cout << el;
            }
            cout << "\n";
        } 
    }
    
    return 0;
}
