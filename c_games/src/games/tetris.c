#include "games/tetris_game.h"
#include "common/sdl_framework.h"
#include "common/input_handler.h"
#include "common/texture_manager.h"
#include "common/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define CELL_SIZE 25
#define BOARD_OFFSET_X 200
#define BOARD_OFFSET_Y 50
#define NEXT_TETROMINO_OFFSET_X 550
#define NEXT_TETROMINO_OFFSET_Y 100
#define INITIAL_FALL_DELAY 1000
#define LEVEL_LINES_THRESHOLD 10
#define MAX_LEVEL 10

// Tetromino shapes (4x4 grid for each rotation state)
static const int TETROMINO_SHAPES[TETROMINO_COUNT][ROTATION_COUNT][4][4] = {
    // I
    {
        {{0, 0, 0, 0}, {1, 1, 1, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}},
        {{0, 0, 1, 0}, {0, 0, 1, 0}, {0, 0, 1, 0}, {0, 0, 1, 0}},
        {{0, 0, 0, 0}, {0, 0, 0, 0}, {1, 1, 1, 1}, {0, 0, 0, 0}},
        {{0, 1, 0, 0}, {0, 1, 0, 0}, {0, 1, 0, 0}, {0, 1, 0, 0}}
    },
    // J
    {
        {{1, 0, 0, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
        {{0, 1, 1, 0}, {0, 1, 0, 0}, {0, 1, 0, 0}, {0, 0, 0, 0}},
        {{0, 0, 0, 0}, {1, 1, 1, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}},
        {{0, 1, 0, 0}, {0, 1, 0, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}}
    },
    // L
    {
        {{0, 0, 1, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
        {{0, 1, 0, 0}, {0, 1, 0, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}},
        {{0, 0, 0, 0}, {1, 1, 1, 0}, {1, 0, 0, 0}, {0, 0, 0, 0}},
        {{1, 1, 0, 0}, {0, 1, 0, 0}, {0, 1, 0, 0}, {0, 0, 0, 0}}
    },
    // O
    {
        {{0, 1, 1, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
        {{0, 1, 1, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
        {{0, 1, 1, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
        {{0, 1, 1, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}
    },
    // S
    {
        {{0, 1, 1, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
        {{0, 1, 0, 0}, {0, 1, 1, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}},
        {{0, 0, 0, 0}, {0, 1, 1, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}},
        {{1, 0, 0, 0}, {1, 1, 0, 0}, {0, 1, 0, 0}, {0, 0, 0, 0}}
    },
    // T
    {
        {{0, 1, 0, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
        {{0, 1, 0, 0}, {0, 1, 1, 0}, {0, 1, 0, 0}, {0, 0, 0, 0}},
        {{0, 0, 0, 0}, {1, 1, 1, 0}, {0, 1, 0, 0}, {0, 0, 0, 0}},
        {{0, 1, 0, 0}, {1, 1, 0, 0}, {0, 1, 0, 0}, {0, 0, 0, 0}}
    },
    // Z
    {
        {{1, 1, 0, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
        {{0, 0, 1, 0}, {0, 1, 1, 0}, {0, 1, 0, 0}, {0, 0, 0, 0}},
        {{0, 0, 0, 0}, {1, 1, 0, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}},
        {{0, 1, 0, 0}, {1, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 0, 0}}
    }
};

// Tetromino colors
static const SDL_Color TETROMINO_COLORS[TETROMINO_COUNT] = {
    {0, 255, 255, 255},   // I - Cyan
    {0, 0, 255, 255},     // J - Blue
    {255, 165, 0, 255},   // L - Orange
    {255, 255, 0, 255},   // O - Yellow
    {0, 255, 0, 255},     // S - Green
    {128, 0, 128, 255},   // T - Purple
    {255, 0, 0, 255}      // Z - Red
};

// Create a new tetris game state
GameState* TetrisGame_Create(void) {
    GameState* state = (GameState*)malloc(sizeof(GameState));
    if (!state) {
        return NULL;
    }
    
    // Initialize function pointers
    state->init = TetrisGame_Init;
    state->cleanup = TetrisGame_Cleanup;
    state->pause = TetrisGame_Pause;
    state->resume = TetrisGame_Resume;
    state->handleEvents = TetrisGame_HandleEvents;
    state->update = TetrisGame_Update;
    state->render = TetrisGame_Render;
    
    // Create game data
    state->data = malloc(sizeof(TetrisGameData));
    if (!state->data) {
        free(state);
        return NULL;
    }
    
    return state;
}

void TetrisGame_Init(GameState* state) {
    TetrisGameData* data = (TetrisGameData*)state->data;
    
    // Initialize random seed
    srand((unsigned int)time(NULL));
    
    // Initialize game data
    data->cellSize = CELL_SIZE;
    
    // Initialize board rectangle
    data->boardRect.x = BOARD_OFFSET_X;
    data->boardRect.y = BOARD_OFFSET_Y;
    data->boardRect.w = TETRIS_BOARD_WIDTH * data->cellSize;
    data->boardRect.h = TETRIS_BOARD_HEIGHT * data->cellSize;
    
    // Initialize next tetromino rectangle
    data->nextTetrominoRect.x = NEXT_TETROMINO_OFFSET_X;
    data->nextTetrominoRect.y = NEXT_TETROMINO_OFFSET_Y;
    data->nextTetrominoRect.w = 4 * data->cellSize;
    data->nextTetrominoRect.h = 4 * data->cellSize;
    
    // Initialize score rectangle
    data->scoreRect.x = NEXT_TETROMINO_OFFSET_X;
    data->scoreRect.y = NEXT_TETROMINO_OFFSET_Y + data->nextTetrominoRect.h + 50;
    data->scoreRect.w = 150;
    data->scoreRect.h = 30;
    
    // Initialize level rectangle
    data->levelRect.x = NEXT_TETROMINO_OFFSET_X;
    data->levelRect.y = data->scoreRect.y + data->scoreRect.h + 20;
    data->levelRect.w = 150;
    data->levelRect.h = 30;
    
    // Initialize input handling
    data->keyRepeatEnabled = true;
    data->keyRepeatDelay = 200;    // Initial delay before repeating
    data->keyRepeatInterval = 50;  // Interval between repeats
    
    // Reset the game
    TetrisGame_Reset(data);
}

void TetrisGame_Cleanup(GameState* state) {
    // Free game data
    free(state->data);
    state->data = NULL;
}

void TetrisGame_Pause(GameState* state) {
    // Nothing to do
    (void)state;
}

void TetrisGame_Resume(GameState* state) {
    // Nothing to do
    (void)state;
}

void TetrisGame_HandleEvents(GameState* state) {
    TetrisGameData* data = (TetrisGameData*)state->data;
    
    // Don't handle events if game is over
    if (data->gameOver) {
        // Reset game if R is pressed
        if (Input_IsKeyJustPressed(SDLK_r)) {
            TetrisGame_Reset(data);
        }
        return;
    }
    
    // Handle key presses
    Uint32 currentTime = SDL_GetTicks();
    
    // Rotate tetromino
    if (Input_IsKeyJustPressed(SDLK_UP)) {
        TetrisGame_RotateTetromino(data, 1);
    }
    
    // Move tetromino left
    if (Input_IsKeyJustPressed(SDLK_LEFT) || 
        (data->keyRepeatEnabled && Input_IsKeyPressed(SDLK_LEFT) && 
         currentTime - data->lastKeyRepeatTime > data->keyRepeatInterval)) {
        TetrisGame_MoveTetromino(data, -1, 0);
        data->lastKeyRepeatTime = currentTime;
    }
    
    // Move tetromino right
    if (Input_IsKeyJustPressed(SDLK_RIGHT) || 
        (data->keyRepeatEnabled && Input_IsKeyPressed(SDLK_RIGHT) && 
         currentTime - data->lastKeyRepeatTime > data->keyRepeatInterval)) {
        TetrisGame_MoveTetromino(data, 1, 0);
        data->lastKeyRepeatTime = currentTime;
    }
    
    // Soft drop (move down faster)
    if (Input_IsKeyJustPressed(SDLK_DOWN) || 
        (data->keyRepeatEnabled && Input_IsKeyPressed(SDLK_DOWN) && 
         currentTime - data->lastKeyRepeatTime > data->keyRepeatInterval)) {
        TetrisGame_MoveTetromino(data, 0, 1);
        data->lastKeyRepeatTime = currentTime;
    }
    
    // Hard drop (drop to bottom)
    if (Input_IsKeyJustPressed(SDLK_SPACE)) {
        TetrisGame_HardDrop(data);
    }
}

void TetrisGame_Update(GameState* state) {
    TetrisGameData* data = (TetrisGameData*)state->data;
    
    // Don't update if game is over
    if (data->gameOver) {
        return;
    }
    
    // Check if it's time to move the tetromino down
    Uint32 currentTime = SDL_GetTicks();
    if (currentTime - data->lastFallTime >= data->fallDelay) {
        // Try to move down
        if (!TetrisGame_MoveTetromino(data, 0, 1)) {
            // If can't move down, lock the tetromino
            TetrisGame_LockTetromino(data);
            
            // Clear completed lines
            TetrisGame_ClearLines(data);
            
            // Update level
            TetrisGame_UpdateLevel(data);
            
            // Spawn new tetromino
            TetrisGame_SpawnTetromino(data);
            
            // Check if game is over
            if (TetrisGame_CheckCollision(data, &data->currentTetromino)) {
                data->gameOver = true;
            }
        }
        
        // Update last fall time
        data->lastFallTime = currentTime;
    }
}

void TetrisGame_Render(GameState* state) {
    TetrisGameData* data = (TetrisGameData*)state->data;
    SDL_Renderer* renderer = SDL_Framework_GetRenderer();
    
    // Clear screen
    SDL_Framework_ClearScreen(240, 240, 240, 255);
    
    // Draw board background
    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
    SDL_RenderFillRect(renderer, &data->boardRect);
    
    // Draw board grid
    SDL_SetRenderDrawColor(renderer, 70, 70, 70, 255);
    for (int x = 0; x <= TETRIS_BOARD_WIDTH; x++) {
        SDL_RenderDrawLine(
            renderer,
            data->boardRect.x + x * data->cellSize,
            data->boardRect.y,
            data->boardRect.x + x * data->cellSize,
            data->boardRect.y + data->boardRect.h
        );
    }
    for (int y = 0; y <= TETRIS_BOARD_HEIGHT; y++) {
        SDL_RenderDrawLine(
            renderer,
            data->boardRect.x,
            data->boardRect.y + y * data->cellSize,
            data->boardRect.x + data->boardRect.w,
            data->boardRect.y + y * data->cellSize
        );
    }
    
    // Draw board cells
    for (int y = 0; y < TETRIS_BOARD_HEIGHT; y++) {
        for (int x = 0; x < TETRIS_BOARD_WIDTH; x++) {
            if (data->board[y][x] > 0) {
                SDL_Rect cellRect = {
                    data->boardRect.x + x * data->cellSize,
                    data->boardRect.y + y * data->cellSize,
                    data->cellSize,
                    data->cellSize
                };
                
                // Get color based on tetromino type (stored in board)
                SDL_Color color = TETROMINO_COLORS[data->board[y][x] - 1];
                SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
                SDL_RenderFillRect(renderer, &cellRect);
                
                // Draw cell border
                SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
                SDL_RenderDrawRect(renderer, &cellRect);
            }
        }
    }
    
    // Draw current tetromino
    if (!data->gameOver) {
        TetrisGame_RenderTetromino(
            renderer,
            &data->currentTetromino,
            data->boardRect.x,
            data->boardRect.y,
            data->cellSize
        );
    }
    
    // Draw next tetromino background
    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
    SDL_RenderFillRect(renderer, &data->nextTetrominoRect);
    
    // Draw next tetromino
    TetrisGame_RenderTetromino(
        renderer,
        &data->nextTetromino,
        data->nextTetrominoRect.x - data->cellSize,
        data->nextTetrominoRect.y,
        data->cellSize
    );
    
    // Draw score and level
    SDL_Color textColor = {0, 0, 0, 255};
    char scoreText[32];
    sprintf(scoreText, "Score: %d", data->score);
    TextureManager_DrawText("menu_font", scoreText, data->scoreRect.x, data->scoreRect.y, textColor);
    
    char levelText[32];
    sprintf(levelText, "Level: %d", data->level);
    TextureManager_DrawText("menu_font", levelText, data->levelRect.x, data->levelRect.y, textColor);
    
    // Draw next tetromino label
    TextureManager_DrawText("menu_font", "Next:", data->nextTetrominoRect.x, data->nextTetrominoRect.y - 30, textColor);
    
    // Draw game over message
    if (data->gameOver) {
        TextureManager_DrawText("menu_font", "Game Over! Press R to restart", 
                               WINDOW_WIDTH / 2 - 150, WINDOW_HEIGHT / 2 - 12, textColor);
    }
}

void TetrisGame_Reset(TetrisGameData* data) {
    // Clear the board
    memset(data->board, 0, sizeof(data->board));
    
    // Reset game state
    data->gameOver = false;
    data->score = 0;
    data->level = 1;
    data->linesCleared = 0;
    data->fallDelay = INITIAL_FALL_DELAY;
    
    // Reset timing
    data->lastFallTime = SDL_GetTicks();
    data->lastKeyRepeatTime = SDL_GetTicks();
    
    // Generate next tetromino
    data->nextTetromino.type = rand() % TETROMINO_COUNT;
    data->nextTetromino.rotation = ROTATION_0;
    data->nextTetromino.x = 3;
    data->nextTetromino.y = 0;
    
    // Spawn first tetromino
    TetrisGame_SpawnTetromino(data);
}

void TetrisGame_SpawnTetromino(TetrisGameData* data) {
    // Set current tetromino to next tetromino
    data->currentTetromino = data->nextTetromino;
    
    // Generate new next tetromino
    data->nextTetromino.type = rand() % TETROMINO_COUNT;
    data->nextTetromino.rotation = ROTATION_0;
    data->nextTetromino.x = 3;
    data->nextTetromino.y = 0;
}

bool TetrisGame_CheckCollision(TetrisGameData* data, Tetromino* tetromino) {
    int shape[4][4];
    TetrisGame_GetTetrominoShape(tetromino->type, tetromino->rotation, shape);
    
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            if (shape[y][x]) {
                int boardX = tetromino->x + x;
                int boardY = tetromino->y + y;
                
                // Check if out of bounds
                if (boardX < 0 || boardX >= TETRIS_BOARD_WIDTH || boardY >= TETRIS_BOARD_HEIGHT) {
                    return true;
                }
                
                // Check if collides with existing blocks (but ignore if above the board)
                if (boardY >= 0 && data->board[boardY][boardX]) {
                    return true;
                }
            }
        }
    }
    
    return false;
}

void TetrisGame_LockTetromino(TetrisGameData* data) {
    int shape[4][4];
    TetrisGame_GetTetrominoShape(data->currentTetromino.type, data->currentTetromino.rotation, shape);
    
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            if (shape[y][x]) {
                int boardX = data->currentTetromino.x + x;
                int boardY = data->currentTetromino.y + y;
                
                // Only place on board if within bounds
                if (boardX >= 0 && boardX < TETRIS_BOARD_WIDTH && boardY >= 0 && boardY < TETRIS_BOARD_HEIGHT) {
                    // Store tetromino type + 1 (0 means empty)
                    data->board[boardY][boardX] = data->currentTetromino.type + 1;
                }
            }
        }
    }
}

void TetrisGame_ClearLines(TetrisGameData* data) {
    int linesCleared = 0;
    
    for (int y = TETRIS_BOARD_HEIGHT - 1; y >= 0; y--) {
        bool lineComplete = true;
        
        // Check if line is complete
        for (int x = 0; x < TETRIS_BOARD_WIDTH; x++) {
            if (data->board[y][x] == 0) {
                lineComplete = false;
                break;
            }
        }
        
        if (lineComplete) {
            linesCleared++;
            
            // Move all lines above down
            for (int moveY = y; moveY > 0; moveY--) {
                for (int x = 0; x < TETRIS_BOARD_WIDTH; x++) {
                    data->board[moveY][x] = data->board[moveY - 1][x];
                }
            }
            
            // Clear top line
            for (int x = 0; x < TETRIS_BOARD_WIDTH; x++) {
                data->board[0][x] = 0;
            }
            
            // Check the same line again (since we moved everything down)
            y++;
        }
    }
    
    // Update score and lines cleared
    if (linesCleared > 0) {
        // Score calculation: 100 * level * (lines^2)
        data->score += 100 * data->level * (linesCleared * linesCleared);
        data->linesCleared += linesCleared;
    }
}

void TetrisGame_RotateTetromino(TetrisGameData* data, int direction) {
    // Save original rotation
    RotationState originalRotation = data->currentTetromino.rotation;
    
    // Calculate new rotation
    data->currentTetromino.rotation = (data->currentTetromino.rotation + direction) % ROTATION_COUNT;
    
    // Check if new rotation causes collision
    if (TetrisGame_CheckCollision(data, &data->currentTetromino)) {
        // Try wall kicks (move left/right to avoid collision)
        for (int offset = 1; offset <= 2; offset++) {
            // Try moving right
            data->currentTetromino.x += offset;
            if (!TetrisGame_CheckCollision(data, &data->currentTetromino)) {
                return;
            }
            
            // Try moving left
            data->currentTetromino.x -= 2 * offset;
            if (!TetrisGame_CheckCollision(data, &data->currentTetromino)) {
                return;
            }
            
            // Reset x position
            data->currentTetromino.x += offset;
        }
        
        // If all wall kicks fail, revert rotation
        data->currentTetromino.rotation = originalRotation;
    }
}

bool TetrisGame_MoveTetromino(TetrisGameData* data, int dx, int dy) {
    // Save original position
    int originalX = data->currentTetromino.x;
    int originalY = data->currentTetromino.y;
    
    // Move tetromino
    data->currentTetromino.x += dx;
    data->currentTetromino.y += dy;
    
    // Check if new position causes collision
    if (TetrisGame_CheckCollision(data, &data->currentTetromino)) {
        // Revert position
        data->currentTetromino.x = originalX;
        data->currentTetromino.y = originalY;
        return false;
    }
    
    return true;
}

void TetrisGame_HardDrop(TetrisGameData* data) {
    // Move down until collision
    while (TetrisGame_MoveTetromino(data, 0, 1)) {
        // Add points for each cell dropped
        data->score += 2;
    }
    
    // Lock tetromino
    TetrisGame_LockTetromino(data);
    
    // Clear completed lines
    TetrisGame_ClearLines(data);
    
    // Update level
    TetrisGame_UpdateLevel(data);
    
    // Spawn new tetromino
    TetrisGame_SpawnTetromino(data);
    
    // Check if game is over
    if (TetrisGame_CheckCollision(data, &data->currentTetromino)) {
        data->gameOver = true;
    }
    
    // Reset fall timer
    data->lastFallTime = SDL_GetTicks();
}

void TetrisGame_UpdateLevel(TetrisGameData* data) {
    // Calculate level based on lines cleared
    int newLevel = (data->linesCleared / LEVEL_LINES_THRESHOLD) + 1;
    
    // Cap level
    if (newLevel > MAX_LEVEL) {
        newLevel = MAX_LEVEL;
    }
    
    // Update level if changed
    if (newLevel != data->level) {
        data->level = newLevel;
        
        // Update fall delay based on level (gets faster as level increases)
        data->fallDelay = INITIAL_FALL_DELAY * (1.0f - ((data->level - 1) * 0.1f));
        
        // Ensure minimum delay
        if (data->fallDelay < 100) {
            data->fallDelay = 100;
        }
    }
}

void TetrisGame_RenderTetromino(SDL_Renderer* renderer, Tetromino* tetromino, int offsetX, int offsetY, int cellSize) {
    int shape[4][4];
    TetrisGame_GetTetrominoShape(tetromino->type, tetromino->rotation, shape);
    SDL_Color color = TetrisGame_GetTetrominoColor(tetromino->type);
    
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            if (shape[y][x]) {
                SDL_Rect cellRect = {
                    offsetX + (tetromino->x + x) * cellSize,
                    offsetY + (tetromino->y + y) * cellSize,
                    cellSize,
                    cellSize
                };
                
                SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
                SDL_RenderFillRect(renderer, &cellRect);
                
                // Draw cell border
                SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
                SDL_RenderDrawRect(renderer, &cellRect);
            }
        }
    }
}

void TetrisGame_GetTetrominoShape(TetrominoType type, RotationState rotation, int shape[4][4]) {
    // Copy shape from predefined shapes
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            shape[y][x] = TETROMINO_SHAPES[type][rotation][y][x];
        }
    }
}

SDL_Color TetrisGame_GetTetrominoColor(TetrominoType type) {
    return TETROMINO_COLORS[type];
}

int main(int argc, char* argv[]) {
    // Initialize SDL framework
    if (!SDL_Framework_Init("Tetris", WINDOW_WIDTH, WINDOW_HEIGHT)) {
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
    
    // Create tetris game state
    GameState* gameState = TetrisGame_Create();
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

