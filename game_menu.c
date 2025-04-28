/**
 * Game Menu in C
 * Converted from Python Tkinter version
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <SDL2/SDL.h>

// Constants
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define MAX_GAMES 10
#define GAME_CARD_WIDTH 200
#define GAME_CARD_HEIGHT 250
#define GAMES_PER_ROW 3

// Game structure
typedef struct {
    char name[50];
    char description[200];
    char executable[50];
    SDL_Texture* thumbnail;
} Game;

// Function prototypes
void init_games(Game games[], int* num_games, SDL_Renderer* renderer);
void draw_menu(Game games[], int num_games, SDL_Renderer* renderer);
void handle_click(Game games[], int num_games, int x, int y);
SDL_Texture* create_pacman_thumbnail(SDL_Renderer* renderer);
SDL_Texture* create_snake_thumbnail(SDL_Renderer* renderer);
SDL_Texture* create_tetris_thumbnail(SDL_Renderer* renderer);
SDL_Texture* create_space_invaders_thumbnail(SDL_Renderer* renderer);
SDL_Texture* create_connect_four_thumbnail(SDL_Renderer* renderer);
SDL_Texture* create_brick_breaker_thumbnail(SDL_Renderer* renderer);
SDL_Texture* create_solitaire_thumbnail(SDL_Renderer* renderer);
SDL_Texture* create_micromachines_thumbnail(SDL_Renderer* renderer);
SDL_Texture* create_tictactoe_thumbnail(SDL_Renderer* renderer);

int main(int argc, char* argv[]) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    // Create window
    SDL_Window* window = SDL_CreateWindow("Game Selection Menu", 
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

    // Initialize games
    Game games[MAX_GAMES];
    int num_games = 0;
    init_games(games, &num_games, renderer);

    // Main loop
    bool quit = false;
    SDL_Event event;

    while (!quit) {
        // Handle events
        while (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_QUIT) {
                quit = true;
            } else if (event.type == SDL_MOUSEBUTTONDOWN) {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    handle_click(games, num_games, event.button.x, event.button.y);
                }
            }
        }

        // Draw menu
        draw_menu(games, num_games, renderer);

        // Small delay to prevent CPU hogging
        SDL_Delay(10);
    }

    // Free resources
    for (int i = 0; i < num_games; i++) {
        SDL_DestroyTexture(games[i].thumbnail);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

void init_games(Game games[], int* num_games, SDL_Renderer* renderer) {
    // Initialize Pac-Man
    strcpy(games[*num_games].name, "Pac-Man");
    strcpy(games[*num_games].description, "Classic arcade game where you eat dots and avoid ghosts");
    strcpy(games[*num_games].executable, "./pacman");
    games[*num_games].thumbnail = create_pacman_thumbnail(renderer);
    (*num_games)++;

    // Initialize Snake
    strcpy(games[*num_games].name, "Snake");
    strcpy(games[*num_games].description, "Guide the snake to eat food and grow without hitting walls or itself");
    strcpy(games[*num_games].executable, "./snake");
    games[*num_games].thumbnail = create_snake_thumbnail(renderer);
    (*num_games)++;

    // Initialize Tetris
    strcpy(games[*num_games].name, "Tetris");
    strcpy(games[*num_games].description, "Arrange falling blocks to create complete lines");
    strcpy(games[*num_games].executable, "./tetris");
    games[*num_games].thumbnail = create_tetris_thumbnail(renderer);
    (*num_games)++;

    // Initialize Space Invaders
    strcpy(games[*num_games].name, "Space Invaders");
    strcpy(games[*num_games].description, "Defend Earth from waves of alien invaders");
    strcpy(games[*num_games].executable, "./space_invaders");
    games[*num_games].thumbnail = create_space_invaders_thumbnail(renderer);
    (*num_games)++;

    // Initialize Connect Four
    strcpy(games[*num_games].name, "Connect Four");
    strcpy(games[*num_games].description, "Drop discs to connect four of your color in a row");
    strcpy(games[*num_games].executable, "./connect_four");
    games[*num_games].thumbnail = create_connect_four_thumbnail(renderer);
    (*num_games)++;

    // Initialize Brick Breaker
    strcpy(games[*num_games].name, "Brick Breaker");
    strcpy(games[*num_games].description, "Break all the bricks with a bouncing ball");
    strcpy(games[*num_games].executable, "./brick_breaker");
    games[*num_games].thumbnail = create_brick_breaker_thumbnail(renderer);
    (*num_games)++;

    // Initialize Solitaire
    strcpy(games[*num_games].name, "Solitaire");
    strcpy(games[*num_games].description, "Classic card game of patience and strategy");
    strcpy(games[*num_games].executable, "./solitaire");
    games[*num_games].thumbnail = create_solitaire_thumbnail(renderer);
    (*num_games)++;

    // Initialize Micro Machines
    strcpy(games[*num_games].name, "Micro Machines");
    strcpy(games[*num_games].description, "Race tiny cars around various tracks");
    strcpy(games[*num_games].executable, "./micromachines");
    games[*num_games].thumbnail = create_micromachines_thumbnail(renderer);
    (*num_games)++;

    // Initialize Tic-Tac-Toe
    strcpy(games[*num_games].name, "Tic-Tac-Toe");
    strcpy(games[*num_games].description, "Classic game of X's and O's on a 3x3 grid");
    strcpy(games[*num_games].executable, "./tictactoe");
    games[*num_games].thumbnail = create_tictactoe_thumbnail(renderer);
    (*num_games)++;
}

void draw_menu(Game games[], int num_games, SDL_Renderer* renderer) {
    // Clear screen
    SDL_SetRenderDrawColor(renderer, 47, 47, 47, 255); // Dark gray background
    SDL_RenderClear(renderer);

    // Draw title
    // In a real implementation, you would use SDL_ttf to render text
    // For simplicity, we're not implementing text rendering here

    // Draw game cards
    for (int i = 0; i < num_games; i++) {
        int row = i / GAMES_PER_ROW;
        int col = i % GAMES_PER_ROW;
        
        int x = 20 + col * (GAME_CARD_WIDTH + 40);
        int y = 80 + row * (GAME_CARD_HEIGHT + 40);
        
        // Draw card background
        SDL_Rect card_rect = {x, y, GAME_CARD_WIDTH, GAME_CARD_HEIGHT};
        SDL_SetRenderDrawColor(renderer, 63, 63, 63, 255); // Darker gray for cards
        SDL_RenderFillRect(renderer, &card_rect);
        
        // Draw thumbnail
        SDL_Rect thumbnail_rect = {x + 25, y + 20, 150, 150};
        SDL_RenderCopy(renderer, games[i].thumbnail, NULL, &thumbnail_rect);
        
        // In a real implementation, you would use SDL_ttf to render text
        // For simplicity, we're not implementing text rendering here
        
        // Draw play button
        SDL_Rect button_rect = {x + 50, y + GAME_CARD_HEIGHT - 40, 100, 30};
        SDL_SetRenderDrawColor(renderer, 76, 175, 80, 255); // Green button
        SDL_RenderFillRect(renderer, &button_rect);
    }
    
    // Present renderer
    SDL_RenderPresent(renderer);
}

void handle_click(Game games[], int num_games, int x, int y) {
    // Check if a play button was clicked
    for (int i = 0; i < num_games; i++) {
        int row = i / GAMES_PER_ROW;
        int col = i % GAMES_PER_ROW;
        
        int card_x = 20 + col * (GAME_CARD_WIDTH + 40);
        int card_y = 80 + row * (GAME_CARD_HEIGHT + 40);
        
        int button_x = card_x + 50;
        int button_y = card_y + GAME_CARD_HEIGHT - 40;
        int button_width = 100;
        int button_height = 30;
        
        if (x >= button_x && x <= button_x + button_width &&
            y >= button_y && y <= button_y + button_height) {
            // Launch the game
            printf("Launching %s: %s\n", games[i].name, games[i].executable);
            system(games[i].executable);
            return;
        }
    }
}

SDL_Texture* create_pacman_thumbnail(SDL_Renderer* renderer) {
    // Create a surface for the thumbnail
    SDL_Surface* surface = SDL_CreateRGBSurface(0, 150, 150, 32, 0, 0, 0, 0);
    if (surface == NULL) {
        printf("Failed to create surface: %s\n", SDL_GetError());
        return NULL;
    }
    
    // Fill with black background
    SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0, 0, 0));
    
    // Draw Pac-Man (yellow circle with mouth)
    SDL_Rect pacman_rect = {55, 55, 40, 40};
    SDL_FillRect(surface, &pacman_rect, SDL_MapRGB(surface->format, 255, 255, 0));
    
    // Draw ghosts
    SDL_Rect ghost1_rect = {20, 100, 20, 20};
    SDL_FillRect(surface, &ghost1_rect, SDL_MapRGB(surface->format, 255, 0, 0)); // Red ghost
    
    SDL_Rect ghost2_rect = {50, 100, 20, 20};
    SDL_FillRect(surface, &ghost2_rect, SDL_MapRGB(surface->format, 0, 255, 255)); // Cyan ghost
    
    SDL_Rect ghost3_rect = {80, 100, 20, 20};
    SDL_FillRect(surface, &ghost3_rect, SDL_MapRGB(surface->format, 255, 184, 255)); // Pink ghost
    
    SDL_Rect ghost4_rect = {110, 100, 20, 20};
    SDL_FillRect(surface, &ghost4_rect, SDL_MapRGB(surface->format, 255, 184, 82)); // Orange ghost
    
    // Create texture from surface
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    
    return texture;
}

SDL_Texture* create_snake_thumbnail(SDL_Renderer* renderer) {
    // Create a surface for the thumbnail
    SDL_Surface* surface = SDL_CreateRGBSurface(0, 150, 150, 32, 0, 0, 0, 0);
    if (surface == NULL) {
        printf("Failed to create surface: %s\n", SDL_GetError());
        return NULL;
    }
    
    // Fill with dark green background
    SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0, 51, 0));
    
    // Draw snake body segments
    for (int i = 0; i < 8; i++) {
        SDL_Rect segment_rect = {75 - i * 15, 75, 15, 15};
        SDL_FillRect(surface, &segment_rect, SDL_MapRGB(surface->format, 144, 238, 144));
    }
    
    // Draw snake head
    SDL_Rect head_rect = {75, 75, 15, 15};
    SDL_FillRect(surface, &head_rect, SDL_MapRGB(surface->format, 0, 100, 0));
    
    // Draw food (apple)
    SDL_Rect food_rect = {120, 75, 15, 15};
    SDL_FillRect(surface, &food_rect, SDL_MapRGB(surface->format, 255, 0, 0));
    
    // Create texture from surface
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    
    return texture;
}

SDL_Texture* create_tetris_thumbnail(SDL_Renderer* renderer) {
    // Create a surface for the thumbnail
    SDL_Surface* surface = SDL_CreateRGBSurface(0, 150, 150, 32, 0, 0, 0, 0);
    if (surface == NULL) {
        printf("Failed to create surface: %s\n", SDL_GetError());
        return NULL;
    }
    
    // Fill with black background
    SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0, 0, 0));
    
    // Draw tetris pieces
    SDL_Rect i_piece = {20, 20, 60, 15};
    SDL_FillRect(surface, &i_piece, SDL_MapRGB(surface->format, 0, 255, 255)); // Cyan
    
    SDL_Rect j_piece = {20, 40, 45, 30};
    SDL_FillRect(surface, &j_piece, SDL_MapRGB(surface->format, 0, 0, 255)); // Blue
    
    SDL_Rect l_piece = {70, 40, 45, 30};
    SDL_FillRect(surface, &l_piece, SDL_MapRGB(surface->format, 255, 127, 0)); // Orange
    
    SDL_Rect o_piece = {20, 75, 30, 30};
    SDL_FillRect(surface, &o_piece, SDL_MapRGB(surface->format, 255, 255, 0)); // Yellow
    
    SDL_Rect s_piece = {55, 75, 45, 30};
    SDL_FillRect(surface, &s_piece, SDL_MapRGB(surface->format, 0, 255, 0)); // Green
    
    SDL_Rect t_piece = {20, 110, 45, 30};
    SDL_FillRect(surface, &t_piece, SDL_MapRGB(surface->format, 128, 0, 128)); // Purple
    
    SDL_Rect z_piece = {70, 110, 45, 30};
    SDL_FillRect(surface, &z_piece, SDL_MapRGB(surface->format, 255, 0, 0)); // Red
    
    // Create texture from surface
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    
    return texture;
}

SDL_Texture* create_space_invaders_thumbnail(SDL_Renderer* renderer) {
    // Create a surface for the thumbnail
    SDL_Surface* surface = SDL_CreateRGBSurface(0, 150, 150, 32, 0, 0, 0, 0);
    if (surface == NULL) {
        printf("Failed to create surface: %s\n", SDL_GetError());
        return NULL;
    }
    
    // Fill with black background
    SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0, 0, 0));
    
    // Draw invaders
    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 5; col++) {
            SDL_Rect invader_rect = {20 + col * 25, 20 + row * 25, 20, 20};
            SDL_FillRect(surface, &invader_rect, SDL_MapRGB(surface->format, 0, 255, 0));
        }
    }
    
    // Draw player ship
    SDL_Rect ship_rect = {65, 120, 20, 10};
    SDL_FillRect(surface, &ship_rect, SDL_MapRGB(surface->format, 0, 255, 0));
    
    // Draw bullets
    SDL_Rect bullet1_rect = {75, 100, 2, 10};
    SDL_FillRect(surface, &bullet1_rect, SDL_MapRGB(surface->format, 255, 255, 255));
    
    SDL_Rect bullet2_rect = {45, 50, 2, 10};
    SDL_FillRect(surface, &bullet2_rect, SDL_MapRGB(surface->format, 255, 255, 255));
    
    // Create texture from surface
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    
    return texture;
}

SDL_Texture* create_connect_four_thumbnail(SDL_Renderer* renderer) {
    // Create a surface for the thumbnail
    SDL_Surface* surface = SDL_CreateRGBSurface(0, 150, 150, 32, 0, 0, 0, 0);
    if (surface == NULL) {
        printf("Failed to create surface: %s\n", SDL_GetError());
        return NULL;
    }
    
    // Fill with blue background (board)
    SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0, 0, 255));
    
    // Draw grid of circles
    for (int row = 0; row < 6; row++) {
        for (int col = 0; col < 7; col++) {
            SDL_Rect circle_rect = {10 + col * 20, 10 + row * 20, 15, 15};
            
            // Randomly fill some circles with red or yellow
            int random_val = rand() % 10;
            if (random_val < 3) {
                SDL_FillRect(surface, &circle_rect, SDL_MapRGB(surface->format, 255, 0, 0)); // Red
            } else if (random_val < 6) {
                SDL_FillRect(surface, &circle_rect, SDL_MapRGB(surface->format, 255, 255, 0)); // Yellow
            } else {
                SDL_FillRect(surface, &circle_rect, SDL_MapRGB(surface->format, 255, 255, 255)); // Empty
            }
        }
    }
    
    // Create texture from surface
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    
    return texture;
}

SDL_Texture* create_brick_breaker_thumbnail(SDL_Renderer* renderer) {
    // Create a surface for the thumbnail
    SDL_Surface* surface = SDL_CreateRGBSurface(0, 150, 150, 32, 0, 0, 0, 0);
    if (surface == NULL) {
        printf("Failed to create surface: %s\n", SDL_GetError());
        return NULL;
    }
    
    // Fill with black background
    SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0, 0, 0));
    
    // Draw bricks
    for (int row = 0; row < 5; row++) {
        for (int col = 0; col < 6; col++) {
            SDL_Rect brick_rect = {10 + col * 22, 10 + row * 15, 20, 10};
            
            // Different colors for different rows
            Uint32 color;
            switch (row) {
                case 0: color = SDL_MapRGB(surface->format, 255, 0, 0); break; // Red
                case 1: color = SDL_MapRGB(surface->format, 255, 165, 0); break; // Orange
                case 2: color = SDL_MapRGB(surface->format, 255, 255, 0); break; // Yellow
                case 3: color = SDL_MapRGB(surface->format, 0, 255, 0); break; // Green
                case 4: color = SDL_MapRGB(surface->format, 0, 0, 255); break; // Blue
                default: color = SDL_MapRGB(surface->format, 255, 255, 255); break; // White
            }
            
            SDL_FillRect(surface, &brick_rect, color);
        }
    }
    
    // Draw paddle
    SDL_Rect paddle_rect = {50, 130, 50, 10};
    SDL_FillRect(surface, &paddle_rect, SDL_MapRGB(surface->format, 200, 200, 200));
    
    // Draw ball
    SDL_Rect ball_rect = {75, 100, 10, 10};
    SDL_FillRect(surface, &ball_rect, SDL_MapRGB(surface->format, 255, 255, 255));
    
    // Create texture from surface
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    
    return texture;
}

SDL_Texture* create_solitaire_thumbnail(SDL_Renderer* renderer) {
    // Create a surface for the thumbnail
    SDL_Surface* surface = SDL_CreateRGBSurface(0, 150, 150, 32, 0, 0, 0, 0);
    if (surface == NULL) {
        printf("Failed to create surface: %s\n", SDL_GetError());
        return NULL;
    }
    
    // Fill with green background (table)
    SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0, 100, 0));
    
    // Draw card piles
    for (int i = 0; i < 7; i++) {
        for (int j = 0; j <= i && j < 3; j++) { // Limit to 3 cards per pile for simplicity
            SDL_Rect card_rect = {10 + i * 20, 50 + j * 15, 15, 20};
            SDL_FillRect(surface, &card_rect, SDL_MapRGB(surface->format, 255, 255, 255));
        }
    }
    
    // Draw foundation piles
    for (int i = 0; i < 4; i++) {
        SDL_Rect foundation_rect = {30 + i * 25, 10, 20, 30};
        SDL_FillRect(surface, &foundation_rect, SDL_MapRGB(surface->format, 220, 220, 220));
    }
    
    // Create texture from surface
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    
    return texture;
}

SDL_Texture* create_micromachines_thumbnail(SDL_Renderer* renderer) {
    // Create a surface for the thumbnail
    SDL_Surface* surface = SDL_CreateRGBSurface(0, 150, 150, 32, 0, 0, 0, 0);
    if (surface == NULL) {
        printf("Failed to create surface: %s\n", SDL_GetError());
        return NULL;
    }
    
    // Fill with green background (grass)
    SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0, 170, 0));
    
    // Draw race track (oval)
    SDL_Rect track_outer = {20, 20, 110, 110};
    SDL_FillRect(surface, &track_outer, SDL_MapRGB(surface->format, 100, 100, 100));
    
    SDL_Rect track_inner = {40, 40, 70, 70};
    SDL_FillRect(surface, &track_inner, SDL_MapRGB(surface->format, 0, 170, 0));
    
    // Draw cars
    SDL_Rect car1_rect = {75, 20, 10, 5};
    SDL_FillRect(surface, &car1_rect, SDL_MapRGB(surface->format, 255, 0, 0));
    
    SDL_Rect car2_rect = {20, 75, 5, 10};
    SDL_FillRect(surface, &car2_rect, SDL_MapRGB(surface->format, 0, 0, 255));
    
    SDL_Rect car3_rect = {75, 125, 10, 5};
    SDL_FillRect(surface, &car3_rect, SDL_MapRGB(surface->format, 255, 255, 0));
    
    SDL_Rect car4_rect = {125, 75, 5, 10};
    SDL_FillRect(surface, &car4_rect, SDL_MapRGB(surface->format, 0, 255, 0));
    
    // Create texture from surface
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    
    return texture;
}

SDL_Texture* create_tictactoe_thumbnail(SDL_Renderer* renderer) {
    // Create a surface for the thumbnail
    SDL_Surface* surface = SDL_CreateRGBSurface(0, 150, 150, 32, 0, 0, 0, 0);
    if (surface == NULL) {
        printf("Failed to create surface: %s\n", SDL_GetError());
        return NULL;
    }
    
    // Fill with dark gray background
    SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 63, 63, 63));
    
    // Draw grid lines
    SDL_Rect h_line1 = {25, 50, 100, 3};
    SDL_Rect h_line2 = {25, 100, 100, 3};
    SDL_Rect v_line1 = {50, 25, 3, 100};
    SDL_Rect v_line2 = {100, 25, 3, 100};
    
    SDL_FillRect(surface, &h_line1, SDL_MapRGB(surface->format, 255, 255, 255));
    SDL_FillRect(surface, &h_line2, SDL_MapRGB(surface->format, 255, 255, 255));
    SDL_FillRect(surface, &v_line1, SDL_MapRGB(surface->format, 255, 255, 255));
    SDL_FillRect(surface, &v_line2, SDL_MapRGB(surface->format, 255, 255, 255));
    
    // Draw X's and O's
    // X in top-left
    SDL_Rect x1_line1 = {30, 30, 15, 3};
    SDL_Rect x1_line2 = {30, 42, 15, 3};
    SDL_FillRect(surface, &x1_line1, SDL_MapRGB(surface->format, 255, 107, 107));
    SDL_FillRect(surface, &x1_line2, SDL_MapRGB(surface->format, 255, 107, 107));
    
    // O in center
    SDL_Rect o_outer = {70, 70, 20, 20};
    SDL_Rect o_inner = {75, 75, 10, 10};
    SDL_FillRect(surface, &o_outer, SDL_MapRGB(surface->format, 78, 205, 196));
    SDL_FillRect(surface, &o_inner, SDL_MapRGB(surface->format, 63, 63, 63));
    
    // X in bottom-right
    SDL_Rect x2_line1 = {105, 105, 15, 3};
    SDL_Rect x2_line2 = {105, 117, 15, 3};
    SDL_FillRect(surface, &x2_line1, SDL_MapRGB(surface->format, 255, 107, 107));
    SDL_FillRect(surface, &x2_line2, SDL_MapRGB(surface->format, 255, 107, 107));
    
    // Create texture from surface
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    
    return texture;
}
