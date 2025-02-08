#include <vector>
#include <iostream>
#include <windows.h>
#include <chrono>
#include <conio.h>
#include <cstdlib>

using namespace std;
using namespace std::chrono_literals;

#define GRID_COLLS 37
#define GRID_ROWS 22

#define snakeBodyASCII 186
#define snakeBodyHorizontalASCII 205
#define snakeTurningLeftUpASCII 187 
#define snakeTurningLeftDownASCII 188 
#define snakeTurningRightDownASCII 200
#define snakeTurningRightUpASCII 201
#define backGroudASCII 32
#define appleASCII 254

#define redColor "\033[31m"
#define greenColor "\033[32m"
#define defaultColor "\033[0m"

#ifdef _WIN32
    // catch the windows events
    LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        switch (uMsg) {
            case WM_SIZING:
            case WM_SIZE:
                return 0;

            case WM_SYSCOMMAND:
                if (wParam == SC_MAXIMIZE || wParam == SC_MINIMIZE || wParam == SC_RESTORE) {
                    return 0;
                }
                break;
        }
        
        // pass unhandled messages to DefWindowProc - Default Window Procedure
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }    
#endif

class CellColor {
public:
    CellColor() = delete;
    CellColor(CellColor&) = delete;

    // Enum representing different cell colors
    enum Value {
        Red = 0,   ///< Red color
        Green,     ///< Green color
        Default    ///< Default color (no color)
    };

    // Static function to draw a cell with the specified color
    /**
     * @param color The color to use for drawing the cell
     * @param toDraw The character to draw
     */
    static void drawWithColor(CellColor::Value& color, char& toDraw) {
        switch (color)
        {
        case CellColor::Red:
            cout << redColor << toDraw << defaultColor;
            break;
        
        case CellColor::Green:
            cout << greenColor << toDraw << defaultColor;
            break;

        default:
            cout << toDraw;
            break;
        }
    }
};

// Constructor to create a GridCell with the specified data and color
/**
 * @param data The character to store in the cell
 * @param color The color of the cell
 */
class GridCell {
public:
    // character of the cell
    char data;
    // color of the cell
    CellColor::Value color;
    
    GridCell(char data, CellColor::Value color)
        : data(data), color(color) {}
    
    // Assignment operator to set the data of the cell
    /**
     * @param data The new data to store in the cell
     * @return A reference to the GridCell object
     */
    GridCell& operator=(char data) {
        this->data = data;
        return *this;
    }

    // Assignment operator to set the color of the cell
    /**
     * @param color The new color of the cell
     * @return A reference to the GridCell object
     */
    GridCell& operator=(CellColor::Value color) {
        this->color = color;
        return *this;
    }

    // Assignment operator to copy the data and color from another GridCell
    /**
     * @param cell The GridCell object to copy from
     * @return A reference to the GridCell object
     */
    GridCell& operator=(GridCell cell) {
        this->data = cell.data;
        this->color = cell.color;
        return *this;
    }

    // Not equal operator to check if the cell data is not equal to another cell data
    /**
     * @param data The data to compare with
     * @return True if the cell data is not equal to the specified data
     */
    bool operator!=(char data) {
        return this->data != data;
    }

    // Equal operator to check if the cell data is equal to another cell data
    /**
     * @param data The data to compare with
     * @return True if the cell data is equal to the specified data
     */
    bool operator==(char data) {
        return this->data == data;
    }

    // Function to draw the cell with the specified color
    /**
     * Draws the cell using the CellColor::drawWithColor function
     */
    void draw() {
        CellColor::drawWithColor(this->color, this->data);
    }
};

// Function to set the console size
/**
 * @param columns The new width of the console
 * @param rows The new height of the console
 */
void setConsoleSize(int columns /* witdh */, int rows /* height */) {
    #ifdef _WIN32
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        if (hConsole == INVALID_HANDLE_VALUE) {
            MessageBox(NULL, "Could not get console handle", "Console", MB_ICONWARNING);
            return;
        }

        CONSOLE_SCREEN_BUFFER_INFO csbi;
        if (!GetConsoleScreenBufferInfo(hConsole, &csbi)) {
            MessageBox(NULL, "Failed to get console buffer info", "Console", MB_ICONWARNING);
            return;
        }

        SMALL_RECT tempRect = { 0, 0, 1, 1 }; 
        SetConsoleWindowInfo(hConsole, TRUE, &tempRect);

        COORD bufferSize = { static_cast<SHORT>(columns), static_cast<SHORT>(rows) };
        if (!SetConsoleScreenBufferSize(hConsole, bufferSize)) {
            MessageBox(NULL, "Failed to set console buffer size", "Console", MB_ICONWARNING);
            return;
        }

        SMALL_RECT windowSize = { 0, 0, static_cast<SHORT>(columns - 1), static_cast<SHORT>(rows - 1) };
        if (!SetConsoleWindowInfo(hConsole, TRUE, &windowSize)) {
            MessageBox(NULL, "Failed to set console window size", "Console", MB_ICONWARNING);
            return;
        }
    #else 
        throw "Not implemented for this platform";
    #endif
}

// Function to check if the program is running as an administrator
/**
 * @param void
 * @return True if the program is running as an administrator, false otherwise
 */
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

// Restart the program as an administrator
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

// Disable console resizing by negating the window styles WS_MAXIMIZEBOX, WS_MINIMIZEBOX, WS_HSCROLL, and WS_VSCROLL
void disableConsoleResizing() {
    #ifdef _WIN32
        HWND hwnd = GetConsoleWindow();
        if (hwnd == NULL) {
            return;
        }

        SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)WindowProc);

        LONG style = GetWindowLong(hwnd, GWL_STYLE);
        style &= ~(WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_HSCROLL | WS_VSCROLL);
        SetWindowLong(hwnd, GWL_STYLE, style);

        SetWindowPos(hwnd, NULL, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_FRAMECHANGED);
    #else 
        throw "Not implemented for this platform";
    #endif
}

// Get key press event
/**
 * @param key key to store the pressed key
 * @return True if a key was pressed, false otherwise
 */
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

// Move the cursor to the specified position
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

// Hide the console cursor
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

// Class to represent a snake body
/**
 * @class SnakeBody
 * @brief Class to represent a snake body
 * @param data The character to be drawn on the console
 * @param color The color of the character
 * @param position The position of the character on the console
 * @param gridPtr The grid on which the character is drawn
 * @param prev The previous snake body in the snake
 */
class SnakeBody : public GridCell {
public:
    vector<vector<GridCell>>* gridPtr;
    Position position;
    SnakeBody * next;
    SnakeBody * prev;
    int direction = 0; // 0 - down; 1 - left; 2 - up; 3 - right;

    SnakeBody(GridCell body, SnakeBody * prev, vector<vector<GridCell>> * grid, int y, int x)
        : GridCell(body) {
        this->prev = prev;
        this->next = nullptr;
        this->gridPtr = grid;
        this->position.x = x;
        this->position.y = y;

        // set char in grid
        this->gridPtr->data()[y][x] = this->data;
    }

    // Function to move the snake body
    /**
     * @param x The new x position of the snake body
     * @param y The new y position of the snake body
     * @return The previous position of the snake body
     */
    Position move(int x, int y) {
        Position _ret = this->position;
        (this->gridPtr->data()[this->position.y][this->position.x] = char(backGroudASCII)) = CellColor::Default;

        this->position.x = x;
        this->position.y = y;
        
        if (this->prev != nullptr) {
            Position prevPos = this->prev->position;
            // if we go to the left or right normally
            if(prevPos.x - 1 == this->position.x || prevPos.x + 1 == this->position.x) {
                this->data = snakeBodyHorizontalASCII;
            }

            // if we go to the up or down normally
            if(prevPos.y - 1 == this->position.y || prevPos.y + 1 == this->position.y) {
                this->data = snakeBodyASCII;
            }
        }
        (this->gridPtr->data()[this->position.y][this->position.x] = this->data) = this->color;
        return _ret;
    }
};

// Function to place an apple on the grid
/**
 * @param grid The grid to place the apple on
 * @brief looks for free spaces and places an apple there
 * @return True if an apple was placed, false otherwise
 */
bool placeApple(vector<vector<GridCell>> * grid) {
    // loop through the grid for free spaces
    vector<Position> freePos;
    for(int i = 0; i < grid->size(); i++)
        for(int j = 0; j < grid->data()[i].size(); j++)
            if(grid->data()[i][j].data == backGroudASCII) 
                freePos.push_back({j, i});
    
    // if there are free spaces, place an apple there
    if (freePos.size() > 0) {
        int index = rand() % freePos.size();
        (grid->data()[freePos[index].y][freePos[index].x] = char(appleASCII)) = CellColor::Red;
        return true;
    } 

    return false;
}

// Function to add a body part to the snake
void addBody(SnakeBody ** tail) {
    (*tail)->next = new SnakeBody(
        GridCell((*tail)->data, 
        CellColor::Default), 
        *tail, 
        (*tail)->gridPtr, 
        (*tail)->position.y, 
        (*tail)->position.x
    );
    (*tail) = (*tail)->next;
}

// Function to restart the game
/**
 * @param head The head of the snake
 * @param tail The tail of the snake
 * @param grid The grid on which the snake is drawn
 * @brief restarts the game
 */
void restartGame(SnakeBody & head, SnakeBody ** tail,vector<vector<GridCell>> * grid) {
    // iterate over all elements of snake
    SnakeBody * it = head.next;
    while (it != nullptr) {
        // set temporary pointer
        SnakeBody * next = it->next;

        // clear grid on this position
        (grid->data()[it->position.y][it->position.x] = char(backGroudASCII)) = CellColor::Default;

        // delete element
        delete it;

        // iterate
        it = next;
    }

    // set head position to start position
    // reset head
    (grid->data()[head.position.y][head.position.x] = char(backGroudASCII)) = CellColor::Default;
    head.position.x = grid->size() / 2;
    head.position.y = grid->size() / 2;
    head.next = nullptr;
    head.prev = nullptr;
    head.direction = 0;
    
    // reset tail
    (*tail) = &head;
}

// Function to end the game if the player dies
void gameOver(SnakeBody & head, SnakeBody ** tail, vector<vector<GridCell>> * grid) {
    // Game over
    #ifdef _WIN32
        // ask user if they want to play again
        int choice = MessageBox(NULL, "Game Over !", "Console", MB_RETRYCANCEL | MB_ICONHAND);
        if (choice == IDRETRY) {
            restartGame(head, tail, head.gridPtr);
            return;
        }
    #endif

    // exit game if user chooses to quit
    exit(0);
}

// Function to move the snake
/** 
 * @brief moves the snake
 * @param body The head of the snake
 * @param tail The tail of the snake
 */
void moveSnake(SnakeBody * body, SnakeBody ** tail) {
    // Get previous position
    int prevX = body->position.x, prevY = body->position.y;

    // Adjust head position based on direction
    switch(body->direction) {
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

    // Check if player should eat apple
    if(body->gridPtr->data()[prevY][prevX] == char(appleASCII)) {
        // Add body part to snake
       addBody(tail);

       if (!placeApple(body->gridPtr)) {
           gameOver(*body, tail, body->gridPtr);
           return;
       }
    } // Check if player should die 
    else if(body->gridPtr->data()[prevY][prevX] != char(backGroudASCII)) {
        gameOver(*body, tail, body->gridPtr);
        return;
    }

    // Move snake parts
    // body is iterator
    while(body != nullptr) {
        // get prev position of element before the next one to move properly
        Position prevPos = body->move(prevX, prevY);
        prevX = prevPos.x;
        prevY = prevPos.y;

        body = body->next;
    }
}

int main() {
    if (!isRunningAsAdmin()) {
        restartAsAdmin();
        return 0;
    }

    setConsoleSize(37, 23); 
    disableConsoleResizing();
    hideCursor();
    system("cls");
    system("title C++ Snake in console");

    srand(time(0));

    // Set up grid
    vector<vector<GridCell>> grid 
        = vector<vector<GridCell>>
        (GRID_ROWS, vector<GridCell>(GRID_COLLS, GridCell(char(backGroudASCII), CellColor::Default)));
    
    for (int i = 0; i < GRID_ROWS; i++) {
        (grid[i][GRID_COLLS - 1] = char(snakeBodyASCII)) = CellColor::Green;
        (grid[i][0] = char(snakeBodyASCII)) = CellColor::Green;
    }

    for (int i = 0; i < GRID_COLLS; i++) {
        (grid[GRID_ROWS - 1][i] = char(snakeBodyHorizontalASCII)) = CellColor::Green;
        (grid[0][i] = char(snakeBodyHorizontalASCII)) = CellColor::Green;
    }
    
    (grid[GRID_ROWS - 1][GRID_COLLS - 1] = char(snakeTurningLeftDownASCII)) = CellColor::Green;
    (grid[GRID_ROWS - 1][0] = char(snakeTurningRightDownASCII)) = CellColor::Green;
    (grid[0][0] = char(snakeTurningRightUpASCII)) = CellColor::Green;
    (grid[0][GRID_COLLS - 1] = char(snakeTurningLeftUpASCII)) = CellColor::Green;

    // Create snake object
    SnakeBody head = SnakeBody(GridCell(char(appleASCII), CellColor::Red), nullptr, &grid, 1, 1);
    SnakeBody * tail = &head;

    // place apple
    placeApple(&grid);

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
            moveSnake(&head, &tail); 
            // Draw only when snake has moved
            moveCursor(0, 0);
            for (auto row : grid) {
                for (auto el : row) {
                    el.draw();
                }
                cout << "\n";
            } 
            updateTimerStart = currTime;
        }
    }
    
    return 0;
}
