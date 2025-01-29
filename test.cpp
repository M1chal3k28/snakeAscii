#include <vector>
#include <iostream>
#include <windows.h>

using namespace std;

#define snakeBodyASCII 186
#define snakeTurningLeftUpASCII 187 
#define snakeTurningLeftDownASCII 189 
#define snakeTurningRightDownASCII 200
#define snakeTurningRightUpASCII 201
#define backGroudASCII 32
#define appleASCII 254

void moveCursor(int x, int y) {
    HANDLE consoleHandle = GetStdHandle((DWORD)STD_OUTPUT_HANDLE);

    SetConsoleCursorPosition(
        consoleHandle,
        COORD{(short)x, (short)y}
    );
}
void hideCursor() {
    HANDLE hStdOut = NULL;
    CONSOLE_CURSOR_INFO curInfo;

    hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleCursorInfo(hStdOut, &curInfo);
    curInfo.bVisible = FALSE;
    SetConsoleCursorInfo(hStdOut, &curInfo);
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
    int direction; // 0 - down; 1 - left; 2 - up; 3 - right;

    SnakeBody(char body, SnakeBody * prev, vector<vector<char>> * grid, int x, int y) {
        this->body = body;
        this->prev = prev;
        this->next = nullptr;
        this->gridPtr = grid;
        this->position.x = x;
        this->position.y = y;

        // set char in grid
        this->gridPtr->data()[x][y] = this->body;
    }

    Position move(int x, int y) {
        Position _ret = this->position;
        this->gridPtr->data()[this->position.x][this->position.y] = backGroudASCII;

        this->position.x = x;
        this->position.y = y;
        
        this->gridPtr->data()[this->position.x][this->position.y] = snakeBodyASCII;
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
            prevY += 1;
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
    hideCursor();
    system("cls");

    vector<vector<char>> grid 
        = vector<vector<char>>
        (10, vector<char>(20, backGroudASCII));
    
    SnakeBody head = SnakeBody(char(snakeBodyASCII), nullptr, &grid, 0, 0);
    SnakeBody * it = &head;
    it->next = new SnakeBody(char(snakeBodyASCII), it, &grid, 1, 0);

    while (true)
    {
        moveCursor(0, 0);
        for (auto row : grid) {
            for (auto el : row) {
                cout << el;
            }
            cout << "\n";
        } 

        moveSnake(&head);
    }
    
    return 0;
}