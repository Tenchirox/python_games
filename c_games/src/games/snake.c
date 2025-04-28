#include "games/snake_game.h"
#include "sdl_framework.h"
#include "input_handler.h"
#include "texture_manager.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define WINDOW_WIDTH 600
#define WINDOW_HEIGHT 600
#define GRID_SIZE 30
#define GRID_WIDTH (WINDOW_WIDTH / GRID_SIZE)
#define GRID_HEIGHT (WINDOW_HEIGHT / GRID_SIZE)
#define INITIAL_SNAKE_LENGTH 3
#define MAX_SNAKE_LENGTH (GRID_WIDTH * GRID_HEIGHT)
#define INITIAL_MOVE_DELAY 150

// Create a new snake game state
GameState* SnakeGame_Create(void) {
    GameState* state = (GameState*)malloc(sizeof(GameState));
    if (!state) {
        return NULL;
    }
    
    // Initialize function pointers
    state->init = SnakeGame_Init;
    state->cleanup = SnakeGame_Cleanup;
    state->pause = SnakeGame_Pause;
    state->resume = SnakeGame_Resume;
    state->handleEvents = SnakeGame_HandleEvents;
    state->update = SnakeGame_Update;
    state->render = SnakeGame_Render;
    
    // Create game data
    state->data = malloc(sizeof(SnakeGameData));
    if (!state->data) {
        free(state);
        return NULL;
    }
    
    return state;
}

void SnakeGame_Init(GameState* state) {
    SnakeGameData* data = (SnakeGameData*)state->data;
    
    // Initialize game data
    data->gridWidth = GRID_WIDTH;
    data->gridHeight = GRID_HEIGHT;
    data->gridSize = GRID_SIZE;
    data->score = 0;
    data->gameOver = false;
    
    // Initialize snake segments
    data->maxSegments = MAX_SNAKE_LENGTH;
    data->segments = (SnakeSegment*)malloc(data->maxSegments * sizeof(SnakeSegment));
    if (!data->segments) {
        fprintf(stderr, "Failed to allocate memory for snake segments\n");
        return;
    }
    
    // Initialize random seed
    srand((unsigned int)time(NULL));
    
    // Reset the game
    SnakeGame_Reset(data);
}

void SnakeGame_Cleanup(GameState* state) {
    SnakeGameData* data = (SnakeGameData*)state->data;
    
    // Free snake segments
    if (data->segments) {
        free(data->segments);
        data->segments = NULL;
    }
    
    // Free game data
    free(data);
    state->data = NULL;
}

void SnakeGame_Pause(GameState* state) {
    // Nothing to do
    (void)state;
}

void SnakeGame_Resume(GameState* state) {
    // Nothing to do
    (void)state;
}

void SnakeGame_HandleEvents(GameState* state) {
    SnakeGameData* data = (SnakeGameData*)state->data;
    
    // Handle key presses for direction changes
    if (Input_IsKeyJustPressed(SDLK_UP) && data->direction != DIR_DOWN) {
        data->nextDirection = DIR_UP;
    } else if (Input_IsKeyJustPressed(SDLK_RIGHT) && data->direction != DIR_LEFT) {
        data->nextDirection = DIR_RIGHT;
    } else if (Input_IsKeyJustPressed(SDLK_DOWN) && data->direction != DIR_UP) {
        data->nextDirection = DIR_DOWN;
    } else if (Input_IsKeyJustPressed(SDLK_LEFT) && data->direction != DIR_RIGHT) {
        data->nextDirection = DIR_LEFT;
    }
    
    // Reset game if R is pressed
    if (Input_IsKeyJustPressed(SDLK_r)) {
        SnakeGame_Reset(data);
    }
}

void SnakeGame_Update(GameState* state) {
    SnakeGameData* data = (SnakeGameData*)state->data;
    
    // Don't update if game is over
    if (data->gameOver) {
        return;
    }
    
    // Check if it's time to move the snake
    Uint32 currentTime = SDL_GetTicks();
    if (currentTime - data->lastMoveTime < data->moveDelay) {
        return;
    }
    
    // Update last move time
    data->lastMoveTime = currentTime;
    
    // Update direction
    data->direction = data->nextDirection;
    
    // Move snake segments (start from the tail)
    for (int i = data->numSegments - 1; i > 0; i--) {
        data->segments[i] = data->segments[i - 1];
    }
    
    // Move head based on direction
    switch (data->direction) {
        case DIR_UP:
            data->segments[0].y--;
            break;
        case DIR_RIGHT:
            data->segments[0].x++;
            break;
        case DIR_DOWN:
            data->segments[0].y++;
            break;
        case DIR_LEFT:
            data->segments[0].x--;
            break;
    }
    
    // Check for collisions
    if (SnakeGame_CheckCollision(data)) {
        data->gameOver = true;
        return;
    }
    
    // Check if snake ate food
    if (data->segments[0].x == data->food.x && data->segments[0].y == data->food.y) {
        // Increase score
        data->score++;
        
        // Grow snake
        SnakeGame_GrowSnake(data);
        
        // Spawn new food
        SnakeGame_SpawnFood(data);
        
        // Increase speed (decrease move delay)
        if (data->moveDelay > 50) {
            data->moveDelay -= 5;
        }
    }
}

void SnakeGame_Render(GameState* state) {
    SnakeGameData* data = (SnakeGameData*)state->data;
    SDL_Renderer* renderer = SDL_Framework_GetRenderer();
    
    // Clear screen
    SDL_Framework_ClearScreen(0, 0, 0, 255);
    
    // Draw grid lines
    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
    for (int x = 0; x <= data->gridWidth; x++) {
        SDL_RenderDrawLine(renderer, x * data->gridSize, 0, x * data->gridSize, WINDOW_HEIGHT);
    }
    for (int y = 0; y <= data->gridHeight; y++) {
        SDL_RenderDrawLine(renderer, 0, y * data->gridSize, WINDOW_WIDTH, y * data->gridSize);
    }
    
    // Draw snake
    for (int i = 0; i < data->numSegments; i++) {
        SDL_Rect segmentRect = {
            data->segments[i].x * data->gridSize,
            data->segments[i].y * data->gridSize,
            data->gridSize,
            data->gridSize
        };
        
        // Head is a different color
        if (i == 0) {
            SDL_SetRenderDrawColor(renderer, 0, 200, 0, 255);
        } else {
            SDL_SetRenderDrawColor(renderer, 0, 150, 0, 255);
        }
        
        SDL_RenderFillRect(renderer, &segmentRect);
    }
    
    // Draw food
    SDL_Rect foodRect = {
        data->food.x * data->gridSize,
        data->food.y * data->gridSize,
        data->gridSize,
        data->gridSize
    };
    SDL_SetRenderDrawColor(renderer, 200, 0, 0, 255);
    SDL_RenderFillRect(renderer, &foodRect);
    
    // Draw score
    char scoreText[32];
    sprintf(scoreText, "Score: %d", data->score);
    SDL_Color textColor = {255, 255, 255, 255};
    TextureManager_DrawText("menu_font", scoreText, 10, 10, textColor);
    
    // Draw game over message
    if (data->gameOver) {
        TextureManager_DrawText("menu_font", "Game Over! Press R to restart", 
                               WINDOW_WIDTH / 2 - 150, WINDOW_HEIGHT / 2 - 12, textColor);
    }
}

void SnakeGame_Reset(SnakeGameData* data) {
    // Reset game state
    data->score = 0;
    data->gameOver = false;
    data->moveDelay = INITIAL_MOVE_DELAY;
    
    // Initialize snake
    data->numSegments = INITIAL_SNAKE_LENGTH;
    data->direction = DIR_RIGHT;
    data->nextDirection = DIR_RIGHT;
    
    // Place snake in the middle of the grid
    int startX = data->gridWidth / 2;
    int startY = data->gridHeight / 2;
    
    for (int i = 0; i < data->numSegments; i++) {
        data->segments[i].x = startX - i;
        data->segments[i].y = startY;
    }
    
    // Spawn initial food
    SnakeGame_SpawnFood(data);
    
    // Reset timer
    data->lastMoveTime = SDL_GetTicks();
}

void SnakeGame_SpawnFood(SnakeGameData* data) {
    bool validPosition = false;
    
    while (!validPosition) {
        // Generate random position
        data->food.x = rand() % data->gridWidth;
        data->food.y = rand() % data->gridHeight;
        
        // Check if position is not occupied by snake
        validPosition = true;
        for (int i = 0; i < data->numSegments; i++) {
            if (data->segments[i].x == data->food.x && data->segments[i].y == data->food.y) {
                validPosition = false;
                break;
            }
        }
    }
}

bool SnakeGame_CheckCollision(SnakeGameData* data) {
    // Get head position
    int headX = data->segments[0].x;
    int headY = data->segments[0].y;
    
    // Check wall collision
    if (headX < 0 || headX >= data->gridWidth || headY < 0 || headY >= data->gridHeight) {
        return true;
    }
    
    // Check self collision (start from 1 to skip the head)
    for (int i = 1; i < data->numSegments; i++) {
        if (headX == data->segments[i].x && headY == data->segments[i].y) {
            return true;
        }
    }
    
    return false;
}

void SnakeGame_GrowSnake(SnakeGameData* data) {
    // Check if we have room to grow
    if (data->numSegments >= data->maxSegments) {
        return;
    }
    
    // Add a new segment at the end (same position as the last segment)
    data->segments[data->numSegments] = data->segments[data->numSegments - 1];
    data->numSegments++;
}

int main(int argc, char* argv[]) {
    // Initialize SDL framework
    if (!SDL_Framework_Init("Snake Game", WINDOW_WIDTH, WINDOW_HEIGHT)) {
        printf("Failed to initialize SDL framework!\n");
        return 1;
    }
    
    // Initialize input handler
    Input_Init();
    
    // Initialize texture manager
    if (!TextureManager_Init(SDL_Framework_GetRenderer())) {
        printf("Failed to initialize texture manager!\n");
        SDL_Framework_Cleanup();
        return 1;
    }
    
    // Load font
    if (!TextureManager_LoadFont("assets/fonts/arial.ttf", "menu_font", 24)) {
        printf("Failed to load font!\n");
        TextureManager_Cleanup();
        SDL_Framework_Cleanup();
        return 1;
    }
    
    // Create snake game state
    GameState* gameState = SnakeGame_Create();
    if (!gameState) {
        printf("Failed to create game state!\n");
        TextureManager_Cleanup();
        SDL_Framework_Cleanup();
        return 1;
    }
    
    // Initialize game
    gameState->init(gameState);
    
    // Main game loop
    bool running = true;
    while (running) {
        // Start frame timer
        SDL_Framework_StartFrameTimer();
        
        // Process input
        running = Input_ProcessEvents();
        
        // Handle game events
        gameState->handleEvents(gameState);
        
        // Update game state
        gameState->update(gameState);
        
        // Render game
        gameState->render(gameState);
        
        // Update screen
        SDL_Framework_UpdateScreen();
        
        // Update input state
        Input_Update();
        
        // Limit FPS
        SDL_Framework_LimitFPS(60);
    }
    
    // Cleanup
    gameState->cleanup(gameState);
    free(gameState);
    TextureManager_Cleanup();
    SDL_Framework_Cleanup();
    
    return 0;
}
