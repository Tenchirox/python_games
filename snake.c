#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>

#ifdef _WIN32
#include <conio.h>
#include <windows.h>
#else
#include <termios.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#endif

// Game constants
#define WIDTH 30
#define HEIGHT 20
#define INITIAL_SNAKE_LENGTH 3

// Direction constants
#define UP 0
#define RIGHT 1
#define DOWN 2
#define LEFT 3

// Game elements
#define EMPTY ' '
#define SNAKE_BODY 'o'
#define SNAKE_HEAD '@'
#define FOOD '*'
#define WALL '#'

// Game state
typedef struct {
    int x;
    int y;
} Point;

typedef struct {
    Point positions[WIDTH * HEIGHT]; // Maximum possible size
    int length;
    int direction;
} Snake;

typedef struct {
    char grid[HEIGHT][WIDTH];
    Snake snake;
    Point food;
    int score;
    bool game_over;
} GameState;

// Function prototypes
void initGame(GameState *game);
void drawGame(GameState *game);
void updateGame(GameState *game);
void placeFood(GameState *game);
bool isCollision(GameState *game, int x, int y);
void changeDirection(GameState *game, int new_direction);
int getInput();
void setupTerminal();
void resetTerminal();
void clearScreen();
void gotoxy(int x, int y);

#ifndef _WIN32
// Global to store original terminal settings
struct termios orig_termios;

// Non-blocking input for Unix-like systems
int kbhit() {
    struct termios term;
    tcgetattr(0, &term);

    struct termios term2 = term;
    term2.c_lflag &= ~ICANON;
    term2.c_lflag &= ~ECHO;
    term2.c_cc[VMIN] = 0;
    term2.c_cc[VTIME] = 0;
    tcsetattr(0, TCSANOW, &term2);

    int byteswaiting;
    ioctl(0, FIONREAD, &byteswaiting);

    tcsetattr(0, TCSANOW, &term);
    return byteswaiting;
}

int getch() {
    int ch;
    struct termios term;
    tcgetattr(0, &term);

    struct termios term2 = term;
    term2.c_lflag &= ~ICANON;
    term2.c_lflag &= ~ECHO;
    term2.c_cc[VMIN] = 1;
    term2.c_cc[VTIME] = 0;
    tcsetattr(0, TCSANOW, &term2);

    ch = getchar();

    tcsetattr(0, TCSANOW, &term);
    return ch;
}
#endif

void setupTerminal() {
#ifndef _WIN32
    // Save current terminal settings
    tcgetattr(STDIN_FILENO, &orig_termios);
    
    // Set terminal to raw mode
    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
#endif
}

void resetTerminal() {
#ifndef _WIN32
    // Restore original terminal settings
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
#endif
}

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void gotoxy(int x, int y) {
#ifdef _WIN32
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
#else
    printf("\033[%d;%dH", y + 1, x + 1);
#endif
}

void initGame(GameState *game) {
    // Initialize game state
    game->score = 0;
    game->game_over = false;
    
    // Initialize grid
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            if (x == 0 || x == WIDTH - 1 || y == 0 || y == HEIGHT - 1) {
                game->grid[y][x] = WALL;
            } else {
                game->grid[y][x] = EMPTY;
            }
        }
    }
    
    // Initialize snake in the middle of the grid
    game->snake.length = INITIAL_SNAKE_LENGTH;
    game->snake.direction = RIGHT;
    
    int middle_x = WIDTH / 2;
    int middle_y = HEIGHT / 2;
    
    for (int i = 0; i < game->snake.length; i++) {
        game->snake.positions[i].x = middle_x - i;
        game->snake.positions[i].y = middle_y;
    }
    
    // Place initial food
    placeFood(game);
}

void drawGame(GameState *game) {
    clearScreen();
    
    // Update grid with current game state
    // First clear the grid (except walls)
    for (int y = 1; y < HEIGHT - 1; y++) {
        for (int x = 1; x < WIDTH - 1; x++) {
            game->grid[y][x] = EMPTY;
        }
    }
    
    // Place snake on grid
    for (int i = 0; i < game->snake.length; i++) {
        int x = game->snake.positions[i].x;
        int y = game->snake.positions[i].y;
        
        if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {
            game->grid[y][x] = (i == 0) ? SNAKE_HEAD : SNAKE_BODY;
        }
    }
    
    // Place food on grid
    game->grid[game->food.y][game->food.x] = FOOD;
    
    // Draw the grid
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            printf("%c", game->grid[y][x]);
        }
        printf("\n");
    }
    
    // Display score
    printf("\nScore: %d\n", game->score);
    printf("Controls: WASD or Arrow Keys to move, Q to quit\n");
}

void placeFood(GameState *game) {
    int x, y;
    bool valid_position;
    
    do {
        valid_position = true;
        x = rand() % (WIDTH - 2) + 1;  // Avoid walls
        y = rand() % (HEIGHT - 2) + 1; // Avoid walls
        
        // Check if position is occupied by snake
        for (int i = 0; i < game->snake.length; i++) {
            if (game->snake.positions[i].x == x && game->snake.positions[i].y == y) {
                valid_position = false;
                break;
            }
        }
    } while (!valid_position);
    
    game->food.x = x;
    game->food.y = y;
}

bool isCollision(GameState *game, int x, int y) {
    // Check wall collision
    if (x == 0 || x == WIDTH - 1 || y == 0 || y == HEIGHT - 1) {
        return true;
    }
    
    // Check self collision (skip the head)
    for (int i = 1; i < game->snake.length; i++) {
        if (game->snake.positions[i].x == x && game->snake.positions[i].y == y) {
            return true;
        }
    }
    
    return false;
}

void updateGame(GameState *game) {
    // Calculate new head position based on current direction
    int new_x = game->snake.positions[0].x;
    int new_y = game->snake.positions[0].y;
    
    switch (game->snake.direction) {
        case UP:
            new_y--;
            break;
        case RIGHT:
            new_x++;
            break;
        case DOWN:
            new_y++;
            break;
        case LEFT:
            new_x--;
            break;
    }
    
    // Check for collisions
    if (isCollision(game, new_x, new_y)) {
        game->game_over = true;
        return;
    }
    
    // Check if food is eaten
    bool ate_food = (new_x == game->food.x && new_y == game->food.y);
    
    // Move snake (shift all positions)
    for (int i = game->snake.length - 1; i > 0; i--) {
        game->snake.positions[i] = game->snake.positions[i - 1];
    }
    
    // Update head position
    game->snake.positions[0].x = new_x;
    game->snake.positions[0].y = new_y;
    
    // Handle food consumption
    if (ate_food) {
        game->score += 10;
        game->snake.length++;
        
        // Copy the last segment to extend the snake
        game->snake.positions[game->snake.length - 1] = game->snake.positions[game->snake.length - 2];
        
        // Place new food
        placeFood(game);
    }
}

void changeDirection(GameState *game, int new_direction) {
    // Prevent 180-degree turns
    if ((new_direction == UP && game->snake.direction == DOWN) ||
        (new_direction == DOWN && game->snake.direction == UP) ||
        (new_direction == LEFT && game->snake.direction == RIGHT) ||
        (new_direction == RIGHT && game->snake.direction == LEFT)) {
        return;
    }
    
    game->snake.direction = new_direction;
}

int getInput() {
#ifdef _WIN32
    if (_kbhit()) {
        int ch = _getch();
        
        if (ch == 224) { // Arrow key prefix
            ch = _getch();
            switch (ch) {
                case 72: return UP;    // Up arrow
                case 80: return DOWN;  // Down arrow
                case 75: return LEFT;  // Left arrow
                case 77: return RIGHT; // Right arrow
            }
        } else {
            switch (ch) {
                case 'w': case 'W': return UP;
                case 's': case 'S': return DOWN;
                case 'a': case 'A': return LEFT;
                case 'd': case 'D': return RIGHT;
                case 'q': case 'Q': return -1; // Quit
            }
        }
    }
#else
    if (kbhit()) {
        int ch = getch();
        
        if (ch == 27) { // ESC key (arrow key prefix)
            if (getch() == 91) { // '[' character
                switch (getch()) {
                    case 65: return UP;    // Up arrow
                    case 66: return DOWN;  // Down arrow
                    case 68: return LEFT;  // Left arrow
                    case 67: return RIGHT; // Right arrow
                }
            }
        } else {
            switch (ch) {
                case 'w': case 'W': return UP;
                case 's': case 'S': return DOWN;
                case 'a': case 'A': return LEFT;
                case 'd': case 'D': return RIGHT;
                case 'q': case 'Q': return -1; // Quit
            }
        }
    }
#endif
    return -2; // No input
}

int main() {
    // Seed random number generator
    srand(time(NULL));
    
    // Set up terminal
    setupTerminal();
    
    // Initialize game
    GameState game;
    initGame(&game);
    
    // Game loop
    while (!game.game_over) {
        // Draw current game state
        drawGame(&game);
        
        // Process input
        int input = getInput();
        if (input == -1) { // Quit
            break;
        } else if (input >= 0) { // Direction change
            changeDirection(&game, input);
        }
        
        // Update game state
        updateGame(&game);
        
        // Sleep to control game speed
        usleep(150000); // 150ms delay
    }
    
    // Game over
    clearScreen();
    printf("Game Over!\n");
    printf("Final Score: %d\n", game.score);
    printf("Press any key to exit...\n");
    
    // Wait for key press
    getchar();
    
    // Reset terminal
    resetTerminal();
    
    return 0;
}
