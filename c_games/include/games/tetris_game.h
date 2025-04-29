#ifndef TETRIS_GAME_H
#define TETRIS_GAME_H

#include "game_state.h"
#include <SDL2/SDL.h>
#include <stdbool.h>

// Tetris board dimensions
#define TETRIS_BOARD_WIDTH 10
#define TETRIS_BOARD_HEIGHT 20

// Tetromino types
typedef enum {
    TETROMINO_I,
    TETROMINO_J,
    TETROMINO_L,
    TETROMINO_O,
    TETROMINO_S,
    TETROMINO_T,
    TETROMINO_Z,
    TETROMINO_COUNT
} TetrominoType;

// Tetromino rotation states
typedef enum {
    ROTATION_0,
    ROTATION_90,
    ROTATION_180,
    ROTATION_270,
    ROTATION_COUNT
} RotationState;

// Tetromino structure
typedef struct {
    TetrominoType type;
    RotationState rotation;
    int x, y;
} Tetromino;

// Tetris game state
typedef struct {
    // Game board
    int board[TETRIS_BOARD_HEIGHT][TETRIS_BOARD_WIDTH];
    
    // Current and next tetromino
    Tetromino currentTetromino;
    Tetromino nextTetromino;
    
    // Game state
    bool gameOver;
    int score;
    int level;
    int linesCleared;
    
    // Timing
    Uint32 lastMoveTime;
    Uint32 moveDelay;
    Uint32 lastFallTime;
    Uint32 fallDelay;
    
    // UI elements
    SDL_Rect boardRect;
    SDL_Rect nextTetrominoRect;
    SDL_Rect scoreRect;
    SDL_Rect levelRect;
    int cellSize;
    
    // Input handling
    bool keyRepeatEnabled;
    Uint32 lastKeyRepeatTime;
    Uint32 keyRepeatDelay;
    Uint32 keyRepeatInterval;
} TetrisGameData;

// Game state functions
void TetrisGame_Init(GameState* state);
void TetrisGame_Cleanup(GameState* state);
void TetrisGame_Pause(GameState* state);
void TetrisGame_Resume(GameState* state);
void TetrisGame_HandleEvents(GameState* state);
void TetrisGame_Update(GameState* state);
void TetrisGame_Render(GameState* state);

// Helper functions
void TetrisGame_Reset(TetrisGameData* data);
void TetrisGame_SpawnTetromino(TetrisGameData* data);
bool TetrisGame_CheckCollision(TetrisGameData* data, Tetromino* tetromino);
void TetrisGame_LockTetromino(TetrisGameData* data);
void TetrisGame_ClearLines(TetrisGameData* data);
void TetrisGame_RotateTetromino(TetrisGameData* data, int direction);
void TetrisGame_MoveTetromino(TetrisGameData* data, int dx, int dy);
void TetrisGame_HardDrop(TetrisGameData* data);
void TetrisGame_UpdateLevel(TetrisGameData* data);
void TetrisGame_RenderTetromino(SDL_Renderer* renderer, Tetromino* tetromino, int offsetX, int offsetY, int cellSize);
void TetrisGame_GetTetrominoShape(TetrominoType type, RotationState rotation, int shape[4][4]);
SDL_Color TetrisGame_GetTetrominoColor(TetrominoType type);

// Create a new tetris game state
GameState* TetrisGame_Create(void);

#endif /* TETRIS_GAME_H */

