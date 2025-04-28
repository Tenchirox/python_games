#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <SDL2/SDL.h>

// Constants
#define WINDOW_WIDTH 600
#define WINDOW_HEIGHT 600
#define GRID_SIZE 30
#define GRID_WIDTH (WINDOW_WIDTH / GRID_SIZE)
#define GRID_HEIGHT (WINDOW_HEIGHT / GRID_SIZE)
#define GAME_SPEED 150 // Milliseconds between updates

// Directions
typedef enum {
    UP,
    DOWN,
    LEFT,
    RIGHT
} Direction;

// Snake segment structure
typedef struct {
    int x;
    int y;
} Segment;

// Game state structure
typedef struct {
    Segment* snake;
    int snake_length;
    int snake_capacity;
    Direction direction;
    Direction next_direction;
    Segment food;
    int score;
    bool game_over;
    bool game_started;
    SDL_Texture* head_texture;
    SDL_Texture* body_texture;
    SDL_Texture* food_texture;
} GameState;

// Function prototypes
void init_game(GameState* game);
void reset_game(GameState* game);
void place_food(GameState* game);
void move_snake(GameState* game, SDL_Renderer* renderer);
void draw_game(GameState* game, SDL_Renderer* renderer);
void draw_welcome_screen(SDL_Renderer* renderer);
void handle_input(GameState* game, SDL_Event* event);
SDL_Texture* create_head_texture(SDL_Renderer* renderer);
SDL_Texture* create_body_texture(SDL_Renderer* renderer);
SDL_Texture* create_food_texture(SDL_Renderer* renderer);

int main(int argc, char* argv[]) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    // Create window
    SDL_Window* window = SDL_CreateWindow("Snake Game", 
                                         SDL_WINDOWPOS_UNDEFINED, 
                                         SDL_WINDOWPOS_UNDEFINED, 
                                         WINDOW_WIDTH, 
                                         WINDOW_HEIGHT, 
                                         SDL_WINDOW_SHOWN);
    if (window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Create renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Initialize game state
    GameState game;
    game.snake = NULL;
    game.snake_length = 0;
    game.snake_capacity = 0;
    game.game_over = false;
    game.game_started = false;
    
    // Create textures
    game.head_texture = create_head_texture(renderer);
    game.body_texture = create_body_texture(renderer);
    game.food_texture = create_food_texture(renderer);
    
    // Initialize game
    init_game(&game);
    
    // Show welcome screen
    draw_welcome_screen(renderer);
    
    // Main game loop
    bool quit = false;
    SDL_Event event;
    Uint32 last_update_time = SDL_GetTicks();
    
    while (!quit) {
        // Handle events
        while (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_QUIT) {
                quit = true;
            } else if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    quit = true;
                } else if (event.key.keysym.sym == SDLK_RETURN && !game.game_started) {
                    // Start game when Enter is pressed
                    game.game_started = true;
                    reset_game(&game);
                } else if (event.key.keysym.sym == SDLK_r && game.game_over) {
                    // Restart game when R is pressed and game is over
                    reset_game(&game);
                } else {
                    handle_input(&game, &event);
                }
            }
        }
        
        // Update game state
        Uint32 current_time = SDL_GetTicks();
        if (game.game_started && !game.game_over && current_time - last_update_time >= GAME_SPEED) {
            move_snake(&game, renderer);
            last_update_time = current_time;
        }
        
        // Draw game
        if (game.game_started) {
            draw_game(&game, renderer);
        }
        
        // Small delay to prevent CPU hogging
        SDL_Delay(10);
    }
    
    // Free resources
    SDL_DestroyTexture(game.head_texture);
    SDL_DestroyTexture(game.body_texture);
    SDL_DestroyTexture(game.food_texture);
    free(game.snake);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    return 0;
}

void init_game(GameState* game) {
    // Initialize snake
    game->snake_capacity = 100;  // Initial capacity
    game->snake = (Segment*)malloc(game->snake_capacity * sizeof(Segment));
    if (game->snake == NULL) {
        printf("Failed to allocate memory for snake\n");
        exit(1);
    }
    
    // Initialize game state
    game->direction = RIGHT;
    game->next_direction = RIGHT;
    game->score = 0;
    game->game_over = false;
    
    // Initialize snake position (center of screen)
    int center_x = GRID_WIDTH / 2;
    int center_y = GRID_HEIGHT / 2;
    
    game->snake_length = 3;
    game->snake[0].x = center_x;
    game->snake[0].y = center_y;
    game->snake[1].x = center_x - 1;
    game->snake[1].y = center_y;
    game->snake[2].x = center_x - 2;
    game->snake[2].y = center_y;
    
    // Place initial food
    place_food(game);
}

void reset_game(GameState* game) {
    // Reset game state
    game->direction = RIGHT;
    game->next_direction = RIGHT;
    game->score = 0;
    game->game_over = false;
    
    // Reset snake position
    int center_x = GRID_WIDTH / 2;
    int center_y = GRID_HEIGHT / 2;
    
    game->snake_length = 3;
    game->snake[0].x = center_x;
    game->snake[0].y = center_y;
    game->snake[1].x = center_x - 1;
    game->snake[1].y = center_y;
    game->snake[2].x = center_x - 2;
    game->snake[2].y = center_y;
    
    // Place new food
    place_food(game);
}

void place_food(GameState* game) {
    // Seed random number generator
    static bool seeded = false;
    if (!seeded) {
        srand(time(NULL));
        seeded = true;
    }
    
    // Find a position not occupied by the snake
    bool valid_position;
    do {
        valid_position = true;
        game->food.x = rand() % GRID_WIDTH;
        game->food.y = rand() % GRID_HEIGHT;
        
        // Check if position is occupied by snake
        for (int i = 0; i < game->snake_length; i++) {
            if (game->snake[i].x == game->food.x && game->snake[i].y == game->food.y) {
                valid_position = false;
                break;
            }
        }
    } while (!valid_position);
}

void move_snake(GameState* game, SDL_Renderer* renderer) {
    // Update direction
    game->direction = game->next_direction;
    
    // Get head position
    int head_x = game->snake[0].x;
    int head_y = game->snake[0].y;
    
    // Calculate new head position
    int new_head_x = head_x;
    int new_head_y = head_y;
    
    switch (game->direction) {
        case UP:
            new_head_y--;
            break;
        case DOWN:
            new_head_y++;
            break;
        case LEFT:
            new_head_x--;
            break;
        case RIGHT:
            new_head_x++;
            break;
    }
    
    // Check for collisions with walls
    if (new_head_x < 0 || new_head_x >= GRID_WIDTH || 
        new_head_y < 0 || new_head_y >= GRID_HEIGHT) {
        game->game_over = true;
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, 
                                "Game Over", 
                                "You hit a wall!", 
                                SDL_GetWindowFromID(SDL_GetWindowID(SDL_RenderGetWindow(renderer))));
        return;
    }
    
    // Check for collisions with self
    for (int i = 0; i < game->snake_length; i++) {
        if (new_head_x == game->snake[i].x && new_head_y == game->snake[i].y) {
            game->game_over = true;
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, 
                                    "Game Over", 
                                    "You bit yourself!", 
                                    SDL_GetWindowFromID(SDL_GetWindowID(SDL_RenderGetWindow(renderer))));
            return;
        }
    }
    
    // Check if snake needs to grow
    bool grow = false;
    if (new_head_x == game->food.x && new_head_y == game->food.y) {
        grow = true;
        game->score += 10;
        
        // Place new food
        place_food(game);
    }
    
    // Resize snake array if needed
    if (grow && game->snake_length >= game->snake_capacity) {
        game->snake_capacity *= 2;
        game->snake = (Segment*)realloc(game->snake, game->snake_capacity * sizeof(Segment));
        if (game->snake == NULL) {
            printf("Failed to reallocate memory for snake\n");
            exit(1);
        }
    }
    
    // Move snake body
    if (grow) {
        game->snake_length++;
    }
    
    // Shift all segments
    for (int i = game->snake_length - 1; i > 0; i--) {
        game->snake[i].x = game->snake[i - 1].x;
        game->snake[i].y = game->snake[i - 1].y;
    }
    
    // Update head position
    game->snake[0].x = new_head_x;
    game->snake[0].y = new_head_y;
}

void draw_game(GameState* game, SDL_Renderer* renderer) {
    // Clear screen
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    
    // Draw snake
    for (int i = 0; i < game->snake_length; i++) {
        SDL_Rect rect = {
            game->snake[i].x * GRID_SIZE,
            game->snake[i].y * GRID_SIZE,
            GRID_SIZE,
            GRID_SIZE
        };
        
        if (i == 0) {
            // Draw head
            SDL_RenderCopy(renderer, game->head_texture, NULL, &rect);
        } else {
            // Draw body
            SDL_RenderCopy(renderer, game->body_texture, NULL, &rect);
        }
    }
    
    // Draw food
    SDL_Rect food_rect = {
        game->food.x * GRID_SIZE,
        game->food.y * GRID_SIZE,
        GRID_SIZE,
        GRID_SIZE
    };
    SDL_RenderCopy(renderer, game->food_texture, NULL, &food_rect);
    
    // Draw score
    char score_text[20];
    sprintf(score_text, "Score: %d", game->score);
    
    // In a real implementation, you would use SDL_ttf to render text
    // For simplicity, we're not implementing text rendering here
    
    // Present renderer
    SDL_RenderPresent(renderer);
}

void draw_welcome_screen(SDL_Renderer* renderer) {
    // Clear screen
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    
    // In a real implementation, you would use SDL_ttf to render text
    // For simplicity, we're not implementing text rendering here
    
    // Draw a simple welcome screen
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    
    // Draw "SNAKE" text (simplified as rectangles)
    int text_x = WINDOW_WIDTH / 2 - 100;
    int text_y = WINDOW_HEIGHT / 4;
    
    // S
    SDL_Rect s_rect1 = {text_x, text_y, 40, 10};
    SDL_Rect s_rect2 = {text_x, text_y, 10, 30};
    SDL_Rect s_rect3 = {text_x, text_y + 30, 40, 10};
    SDL_Rect s_rect4 = {text_x + 30, text_y + 30, 10, 30};
    SDL_Rect s_rect5 = {text_x, text_y + 60, 40, 10};
    
    SDL_RenderFillRect(renderer, &s_rect1);
    SDL_RenderFillRect(renderer, &s_rect2);
    SDL_RenderFillRect(renderer, &s_rect3);
    SDL_RenderFillRect(renderer, &s_rect4);
    SDL_RenderFillRect(renderer, &s_rect5);
    
    // Draw instructions
    SDL_Rect instr_rect = {WINDOW_WIDTH / 2 - 150, WINDOW_HEIGHT / 2, 300, 100};
    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
    SDL_RenderFillRect(renderer, &instr_rect);
    
    // Present renderer
    SDL_RenderPresent(renderer);
}

void handle_input(GameState* game, SDL_Event* event) {
    // Change direction based on key press
    switch (event->key.keysym.sym) {
        case SDLK_UP:
            if (game->direction != DOWN) {
                game->next_direction = UP;
            }
            break;
        case SDLK_DOWN:
            if (game->direction != UP) {
                game->next_direction = DOWN;
            }
            break;
        case SDLK_LEFT:
            if (game->direction != RIGHT) {
                game->next_direction = LEFT;
            }
            break;
        case SDLK_RIGHT:
            if (game->direction != LEFT) {
                game->next_direction = RIGHT;
            }
            break;
    }
}

SDL_Texture* create_head_texture(SDL_Renderer* renderer) {
    // Create a surface for the head
    SDL_Surface* surface = SDL_CreateRGBSurface(0, GRID_SIZE, GRID_SIZE, 32, 0, 0, 0, 0);
    if (surface == NULL) {
        printf("Failed to create surface: %s\n", SDL_GetError());
        return NULL;
    }
    
    // Fill with dark green
    SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0, 100, 0));
    
    // Draw eyes (simplified)
    SDL_Rect left_eye = {GRID_SIZE / 3 - 2, GRID_SIZE / 3 - 2, 4, 4};
    SDL_Rect right_eye = {2 * GRID_SIZE / 3 - 2, GRID_SIZE / 3 - 2, 4, 4};
    
    SDL_FillRect(surface, &left_eye, SDL_MapRGB(surface->format, 255, 255, 255));
    SDL_FillRect(surface, &right_eye, SDL_MapRGB(surface->format, 255, 255, 255));
    
    // Create texture from surface
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    
    return texture;
}

SDL_Texture* create_body_texture(SDL_Renderer* renderer) {
    // Create a surface for the body
    SDL_Surface* surface = SDL_CreateRGBSurface(0, GRID_SIZE, GRID_SIZE, 32, 0, 0, 0, 0);
    if (surface == NULL) {
        printf("Failed to create surface: %s\n", SDL_GetError());
        return NULL;
    }
    
    // Fill with light green
    SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 144, 238, 144));
    
    // Create texture from surface
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    
    return texture;
}

SDL_Texture* create_food_texture(SDL_Renderer* renderer) {
    // Create a surface for the food
    SDL_Surface* surface = SDL_CreateRGBSurface(0, GRID_SIZE, GRID_SIZE, 32, 0, 0, 0, 0);
    if (surface == NULL) {
        printf("Failed to create surface: %s\n", SDL_GetError());
        return NULL;
    }
    
    // Fill with red (apple)
    SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 255, 0, 0));
    
    // Draw stem (simplified)
    SDL_Rect stem = {GRID_SIZE / 2 - 1, 0, 2, 5};
    SDL_FillRect(surface, &stem, SDL_MapRGB(surface->format, 139, 69, 19));
    
    // Create texture from surface
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    
    return texture;
}
