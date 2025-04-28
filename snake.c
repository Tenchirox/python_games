/**
 * Snake Game in C
 * Converted from Python Tkinter version
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>

// Game constants
#define WIDTH 20
#define HEIGHT 15
#define EMPTY ' '
#define SNAKE_BODY 'o'
#define SNAKE_HEAD '@'
#define FOOD '*'
#define WALL '#'
#define GAME_SPEED 150000 // microseconds

// Direction constants
#define UP 0
#define RIGHT 1
#define DOWN 2
#define LEFT 3

// Game state
typedef struct {
    int x;
    int y;
} Point;

typedef struct {
    Point positions[WIDTH * HEIGHT]; // Maximum possible snake length
    int length;
    int direction;
    int next_direction;
} Snake;

typedef struct {
    Point position;
} Food;

typedef struct {
    char grid[HEIGHT][WIDTH];
    Snake snake;
    Food food;
    int score;
    int game_over;
} GameState;

// Function prototypes
void initialize_game(GameState *game);
void draw_game(GameState *game);
void place_food(GameState *game);
int kbhit();
int getch_nonblock();
void update_direction(GameState *game, int key);
void move_snake(GameState *game);
void update_game(GameState *game);

// Non-blocking keyboard input functions
int kbhit() {
    struct termios oldt, newt;
    int ch;
    int oldf;
    
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
    
    ch = getchar();
    
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);
    
    if(ch != EOF) {
        ungetc(ch, stdin);
        return 1;
    }
    
    return 0;
}

int getch_nonblock() {
    if (kbhit()) {
        return getchar();
    }
    return -1;
}

// Initialize the game state
void initialize_game(GameState *game) {
    int i, j;
    
    // Clear the grid
    for (i = 0; i < HEIGHT; i++) {
        for (j = 0; j < WIDTH; j++) {
            game->grid[i][j] = EMPTY;
        }
    }
    
    // Initialize snake
    game->snake.length = 3;
    game->snake.direction = RIGHT;
    game->snake.next_direction = RIGHT;
    
    // Place snake in the middle
    int center_x = WIDTH / 2;
    int center_y = HEIGHT / 2;
    
    for (i = 0; i < game->snake.length; i++) {
        game->snake.positions[i].x = center_x - i;
        game->snake.positions[i].y = center_y;
    }
    
    // Initialize score and game state
    game->score = 0;
    game->game_over = 0;
    
    // Place initial food
    place_food(game);
}

// Place food at a random empty position
void place_food(GameState *game) {
    int x, y;
    int is_valid;
    
    // Seed the random number generator
    srand(time(NULL));
    
    do {
        is_valid = 1;
        x = rand() % WIDTH;
        y = rand() % HEIGHT;
        
        // Check if position is occupied by snake
        for (int i = 0; i < game->snake.length; i++) {
            if (game->snake.positions[i].x == x && game->snake.positions[i].y == y) {
                is_valid = 0;
                break;
            }
        }
    } while (!is_valid);
    
    game->food.position.x = x;
    game->food.position.y = y;
}

// Draw the current game state
void draw_game(GameState *game) {
    int i, j;
    
    // Clear the screen
    system("clear"); // Use "cls" for Windows
    
    // Draw top border
    printf("+");
    for (j = 0; j < WIDTH; j++) {
        printf("-");
    }
    printf("+\n");
    
    // Update grid with current snake and food positions
    // Clear the grid first
    for (i = 0; i < HEIGHT; i++) {
        for (j = 0; j < WIDTH; j++) {
            game->grid[i][j] = EMPTY;
        }
    }
    
    // Place food on grid
    game->grid[game->food.position.y][game->food.position.x] = FOOD;
    
    // Place snake on grid
    for (i = 0; i < game->snake.length; i++) {
        int x = game->snake.positions[i].x;
        int y = game->snake.positions[i].y;
        
        // Check if position is valid
        if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {
            game->grid[y][x] = (i == 0) ? SNAKE_HEAD : SNAKE_BODY;
        }
    }
    
    // Draw grid
    for (i = 0; i < HEIGHT; i++) {
        printf("|");
        for (j = 0; j < WIDTH; j++) {
            printf("%c", game->grid[i][j]);
        }
        printf("|\n");
    }
    
    // Draw bottom border
    printf("+");
    for (j = 0; j < WIDTH; j++) {
        printf("-");
    }
    printf("+\n");
    
    // Display score
    printf("Score: %d\n", game->score);
    printf("Controls: W (up), A (left), S (down), D (right), Q (quit)\n");
}

// Update snake direction based on key input
void update_direction(GameState *game, int key) {
    switch (key) {
        case 'w': case 'W':
            if (game->snake.direction != DOWN) {
                game->snake.next_direction = UP;
            }
            break;
        case 'd': case 'D':
            if (game->snake.direction != LEFT) {
                game->snake.next_direction = RIGHT;
            }
            break;
        case 's': case 'S':
            if (game->snake.direction != UP) {
                game->snake.next_direction = DOWN;
            }
            break;
        case 'a': case 'A':
            if (game->snake.direction != RIGHT) {
                game->snake.next_direction = LEFT;
            }
            break;
        case 'q': case 'Q':
            game->game_over = 1;
            break;
    }
}

// Move the snake
void move_snake(GameState *game) {
    int i;
    Point new_head;
    
    // Update direction
    game->snake.direction = game->snake.next_direction;
    
    // Get current head position
    Point head = game->snake.positions[0];
    
    // Calculate new head position based on direction
    switch (game->snake.direction) {
        case UP:
            new_head.x = head.x;
            new_head.y = head.y - 1;
            break;
        case RIGHT:
            new_head.x = head.x + 1;
            new_head.y = head.y;
            break;
        case DOWN:
            new_head.x = head.x;
            new_head.y = head.y + 1;
            break;
        case LEFT:
            new_head.x = head.x - 1;
            new_head.y = head.y;
            break;
    }
    
    // Check for collision with walls
    if (new_head.x < 0 || new_head.x >= WIDTH || new_head.y < 0 || new_head.y >= HEIGHT) {
        game->game_over = 1;
        printf("\nGame Over! You hit a wall.\n");
        return;
    }
    
    // Check for collision with self
    for (i = 0; i < game->snake.length; i++) {
        if (new_head.x == game->snake.positions[i].x && new_head.y == game->snake.positions[i].y) {
            game->game_over = 1;
            printf("\nGame Over! You hit yourself.\n");
            return;
        }
    }
    
    // Check if snake ate food
    if (new_head.x == game->food.position.x && new_head.y == game->food.position.y) {
        // Increase score
        game->score += 10;
        
        // Increase snake length
        game->snake.length++;
        
        // Place new food
        place_food(game);
    } else {
        // Move the rest of the snake body
        for (i = game->snake.length - 1; i > 0; i--) {
            game->snake.positions[i] = game->snake.positions[i - 1];
        }
    }
    
    // Update head position
    game->snake.positions[0] = new_head;
}

// Update game state
void update_game(GameState *game) {
    // Process keyboard input
    int key = getch_nonblock();
    if (key != -1) {
        update_direction(game, key);
    }
    
    // Move snake
    move_snake(game);
    
    // Draw updated game state
    draw_game(game);
}

// Main snake game function
void launch_snake() {
    GameState game;
    struct termios old_term, new_term;
    
    // Set up terminal for non-canonical mode
    tcgetattr(STDIN_FILENO, &old_term);
    new_term = old_term;
    new_term.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &new_term);
    
    // Initialize game
    initialize_game(&game);
    
    // Game loop
    while (!game.game_over) {
        update_game(&game);
        usleep(GAME_SPEED); // Sleep to control game speed
    }
    
    // Restore terminal settings
    tcsetattr(STDIN_FILENO, TCSANOW, &old_term);
    
    printf("\nFinal Score: %d\n", game.score);
    printf("Press Enter to return to menu...");
    getchar(); // Wait for Enter key
}

// Standalone main function for testing
#ifdef SNAKE_STANDALONE
int main() {
    launch_snake();
    return 0;
}
#endif
