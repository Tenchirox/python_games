/**
 * Pac-Man Game in C
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
#define WALL '#'
#define PACMAN 'C'
#define GHOST 'G'
#define DOT '.'
#define POWER_PELLET 'O'
#define GAME_SPEED 200000 // microseconds

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
    Point position;
    int direction;
    int next_direction;
} Pacman;

typedef struct {
    Point position;
    int direction;
    int is_vulnerable;
} Ghost;

typedef struct {
    char grid[HEIGHT][WIDTH];
    Pacman pacman;
    Ghost ghosts[4]; // 4 ghosts
    int score;
    int dots_remaining;
    int game_over;
    int level;
} GameState;

// Function prototypes
void initialize_game(GameState *game);
void draw_game(GameState *game);
int kbhit();
int getch_nonblock();
void update_direction(GameState *game, int key);
void move_pacman(GameState *game);
void move_ghosts(GameState *game);
void update_game(GameState *game);
int can_move(GameState *game, int x, int y);
void place_dots(GameState *game);

// Level layouts
const char *level_layouts[] = {
    // Level 1
    "####################"
    "#........#........#"
    "#.##.###.#.###.##.#"
    "#O##.###.#.###.##O#"
    "#.................#"
    "#.##.#.#####.#.##.#"
    "#....#...#...#....#"
    "####.### # ###.####"
    "   #.#       #.#   "
    "####.# ##-## #.####"
    "#....#       #....#"
    "#.##.# ##### #.##.#"
    "#O...#       #...O#"
    "#.#####.###.#####.#"
    "#.................#"
    "####################"
};

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
    
    // Load level layout
    const char *layout = level_layouts[0]; // Start with level 1
    
    for (i = 0; i < HEIGHT; i++) {
        for (j = 0; j < WIDTH; j++) {
            char cell = layout[i * WIDTH + j];
            game->grid[i][j] = cell;
        }
    }
    
    // Initialize Pac-Man
    game->pacman.position.x = WIDTH / 2;
    game->pacman.position.y = HEIGHT - 3;
    game->pacman.direction = LEFT;
    game->pacman.next_direction = LEFT;
    
    // Initialize ghosts
    game->ghosts[0].position.x = WIDTH / 2 - 1;
    game->ghosts[0].position.y = HEIGHT / 2;
    game->ghosts[0].direction = UP;
    game->ghosts[0].is_vulnerable = 0;
    
    game->ghosts[1].position.x = WIDTH / 2;
    game->ghosts[1].position.y = HEIGHT / 2;
    game->ghosts[1].direction = UP;
    game->ghosts[1].is_vulnerable = 0;
    
    game->ghosts[2].position.x = WIDTH / 2 + 1;
    game->ghosts[2].position.y = HEIGHT / 2;
    game->ghosts[2].direction = UP;
    game->ghosts[2].is_vulnerable = 0;
    
    game->ghosts[3].position.x = WIDTH / 2;
    game->ghosts[3].position.y = HEIGHT / 2 + 1;
    game->ghosts[3].direction = UP;
    game->ghosts[3].is_vulnerable = 0;
    
    // Initialize score and game state
    game->score = 0;
    game->game_over = 0;
    game->level = 1;
    
    // Place dots and count them
    place_dots(game);
}

// Place dots in empty spaces
void place_dots(GameState *game) {
    int i, j;
    game->dots_remaining = 0;
    
    for (i = 0; i < HEIGHT; i++) {
        for (j = 0; j < WIDTH; j++) {
            if (game->grid[i][j] == EMPTY) {
                game->grid[i][j] = DOT;
                game->dots_remaining++;
            }
        }
    }
}

// Check if a position is valid for movement
int can_move(GameState *game, int x, int y) {
    // Check bounds
    if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) {
        return 0;
    }
    
    // Check if position is a wall
    if (game->grid[y][x] == WALL) {
        return 0;
    }
    
    return 1;
}

// Draw the current game state
void draw_game(GameState *game) {
    int i, j;
    
    // Clear the screen
    system("clear"); // Use "cls" for Windows
    
    // Create a temporary grid with current state
    char temp_grid[HEIGHT][WIDTH];
    
    // Copy the base grid
    for (i = 0; i < HEIGHT; i++) {
        for (j = 0; j < WIDTH; j++) {
            temp_grid[i][j] = game->grid[i][j];
        }
    }
    
    // Place Pac-Man
    temp_grid[game->pacman.position.y][game->pacman.position.x] = PACMAN;
    
    // Place ghosts
    for (i = 0; i < 4; i++) {
        temp_grid[game->ghosts[i].position.y][game->ghosts[i].position.x] = 
            game->ghosts[i].is_vulnerable ? 'V' : GHOST;
    }
    
    // Draw top border
    printf("+");
    for (j = 0; j < WIDTH; j++) {
        printf("-");
    }
    printf("+\n");
    
    // Draw grid
    for (i = 0; i < HEIGHT; i++) {
        printf("|");
        for (j = 0; j < WIDTH; j++) {
            printf("%c", temp_grid[i][j]);
        }
        printf("|\n");
    }
    
    // Draw bottom border
    printf("+");
    for (j = 0; j < WIDTH; j++) {
        printf("-");
    }
    printf("+\n");
    
    // Display score and level
    printf("Score: %d | Level: %d | Dots: %d\n", game->score, game->level, game->dots_remaining);
    printf("Controls: W (up), A (left), S (down), D (right), Q (quit)\n");
}

// Update Pac-Man direction based on key input
void update_direction(GameState *game, int key) {
    switch (key) {
        case 'w': case 'W':
            game->pacman.next_direction = UP;
            break;
        case 'd': case 'D':
            game->pacman.next_direction = RIGHT;
            break;
        case 's': case 'S':
            game->pacman.next_direction = DOWN;
            break;
        case 'a': case 'A':
            game->pacman.next_direction = LEFT;
            break;
        case 'q': case 'Q':
            game->game_over = 1;
            break;
    }
}

// Move Pac-Man
void move_pacman(GameState *game) {
    int new_x = game->pacman.position.x;
    int new_y = game->pacman.position.y;
    
    // Try to move in the next direction
    switch (game->pacman.next_direction) {
        case UP:
            if (can_move(game, new_x, new_y - 1)) {
                game->pacman.direction = UP;
                new_y--;
            }
            break;
        case RIGHT:
            if (can_move(game, new_x + 1, new_y)) {
                game->pacman.direction = RIGHT;
                new_x++;
            }
            break;
        case DOWN:
            if (can_move(game, new_x, new_y + 1)) {
                game->pacman.direction = DOWN;
                new_y++;
            }
            break;
        case LEFT:
            if (can_move(game, new_x - 1, new_y)) {
                game->pacman.direction = LEFT;
                new_x--;
            }
            break;
    }
    
    // If can't move in next direction, try current direction
    if (new_x == game->pacman.position.x && new_y == game->pacman.position.y) {
        switch (game->pacman.direction) {
            case UP:
                if (can_move(game, new_x, new_y - 1)) {
                    new_y--;
                }
                break;
            case RIGHT:
                if (can_move(game, new_x + 1, new_y)) {
                    new_x++;
                }
                break;
            case DOWN:
                if (can_move(game, new_x, new_y + 1)) {
                    new_y++;
                }
                break;
            case LEFT:
                if (can_move(game, new_x - 1, new_y)) {
                    new_x--;
                }
                break;
        }
    }
    
    // Update position
    game->pacman.position.x = new_x;
    game->pacman.position.y = new_y;
    
    // Check what Pac-Man ate
    char cell = game->grid[new_y][new_x];
    
    if (cell == DOT) {
        game->score += 10;
        game->grid[new_y][new_x] = EMPTY;
        game->dots_remaining--;
        
        // Check if level is complete
        if (game->dots_remaining == 0) {
            game->level++;
            initialize_game(game);
        }
    } else if (cell == POWER_PELLET) {
        game->score += 50;
        game->grid[new_y][new_x] = EMPTY;
        
        // Make ghosts vulnerable
        for (int i = 0; i < 4; i++) {
            game->ghosts[i].is_vulnerable = 1;
        }
    }
    
    // Check for collision with ghosts
    for (int i = 0; i < 4; i++) {
        if (game->pacman.position.x == game->ghosts[i].position.x && 
            game->pacman.position.y == game->ghosts[i].position.y) {
            
            if (game->ghosts[i].is_vulnerable) {
                // Eat the ghost
                game->score += 200;
                
                // Reset ghost position
                game->ghosts[i].position.x = WIDTH / 2;
                game->ghosts[i].position.y = HEIGHT / 2;
                game->ghosts[i].is_vulnerable = 0;
            } else {
                // Game over
                game->game_over = 1;
                printf("\nGame Over! You were caught by a ghost.\n");
                return;
            }
        }
    }
}

// Move ghosts
void move_ghosts(GameState *game) {
    int i;
    
    for (i = 0; i < 4; i++) {
        int new_x = game->ghosts[i].position.x;
        int new_y = game->ghosts[i].position.y;
        int possible_dirs[4] = {0}; // UP, RIGHT, DOWN, LEFT
        int num_dirs = 0;
        
        // Find possible directions
        if (can_move(game, new_x, new_y - 1) && game->ghosts[i].direction != DOWN) {
            possible_dirs[num_dirs++] = UP;
        }
        if (can_move(game, new_x + 1, new_y) && game->ghosts[i].direction != LEFT) {
            possible_dirs[num_dirs++] = RIGHT;
        }
        if (can_move(game, new_x, new_y + 1) && game->ghosts[i].direction != UP) {
            possible_dirs[num_dirs++] = DOWN;
        }
        if (can_move(game, new_x - 1, new_y) && game->ghosts[i].direction != RIGHT) {
            possible_dirs[num_dirs++] = LEFT;
        }
        
        // If no valid directions, try all directions
        if (num_dirs == 0) {
            if (can_move(game, new_x, new_y - 1)) {
                possible_dirs[num_dirs++] = UP;
            }
            if (can_move(game, new_x + 1, new_y)) {
                possible_dirs[num_dirs++] = RIGHT;
            }
            if (can_move(game, new_x, new_y + 1)) {
                possible_dirs[num_dirs++] = DOWN;
            }
            if (can_move(game, new_x - 1, new_y)) {
                possible_dirs[num_dirs++] = LEFT;
            }
        }
        
        // Choose a random direction from possible ones
        if (num_dirs > 0) {
            int new_dir = possible_dirs[rand() % num_dirs];
            game->ghosts[i].direction = new_dir;
            
            // Move in the chosen direction
            switch (new_dir) {
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
        }
        
        // Update position
        game->ghosts[i].position.x = new_x;
        game->ghosts[i].position.y = new_y;
    }
    
    // Check for collision with Pac-Man
    for (i = 0; i < 4; i++) {
        if (game->pacman.position.x == game->ghosts[i].position.x && 
            game->pacman.position.y == game->ghosts[i].position.y) {
            
            if (game->ghosts[i].is_vulnerable) {
                // Eat the ghost
                game->score += 200;
                
                // Reset ghost position
                game->ghosts[i].position.x = WIDTH / 2;
                game->ghosts[i].position.y = HEIGHT / 2;
                game->ghosts[i].is_vulnerable = 0;
            } else {
                // Game over
                game->game_over = 1;
                printf("\nGame Over! You were caught by a ghost.\n");
                return;
            }
        }
    }
}

// Update game state
void update_game(GameState *game) {
    // Process keyboard input
    int key = getch_nonblock();
    if (key != -1) {
        update_direction(game, key);
    }
    
    // Move Pac-Man
    move_pacman(game);
    
    // Move ghosts
    move_ghosts(game);
    
    // Draw updated game state
    draw_game(game);
}

// Main Pac-Man game function
void launch_pacman() {
    GameState game;
    struct termios old_term, new_term;
    
    // Seed random number generator
    srand(time(NULL));
    
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
#ifdef PACMAN_STANDALONE
int main() {
    launch_pacman();
    return 0;
}
#endif
