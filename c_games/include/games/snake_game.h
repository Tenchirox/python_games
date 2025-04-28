#ifndef SNAKE_GAME_H
#define SNAKE_GAME_H

#include "game_state.h"
#include <SDL2/SDL.h>
#include <stdbool.h>

// Direction enum
typedef enum {
    DIR_UP,
    DIR_RIGHT,
    DIR_DOWN,
    DIR_LEFT
} Direction;

// Snake segment structure
typedef struct {
    int x;
    int y;
} SnakeSegment;

// Snake game state
typedef struct {
    int gridWidth;
    int gridHeight;
    int gridSize;
    int score;
    bool gameOver;
    
    SnakeSegment* segments;
    int numSegments;
    int maxSegments;
    
    Direction direction;
    Direction nextDirection;
    
    SnakeSegment food;
    
    Uint32 lastMoveTime;
    Uint32 moveDelay;
} SnakeGameData;

// Game state functions
void SnakeGame_Init(GameState* state);
void SnakeGame_Cleanup(GameState* state);
void SnakeGame_Pause(GameState* state);
void SnakeGame_Resume(GameState* state);
void SnakeGame_HandleEvents(GameState* state);
void SnakeGame_Update(GameState* state);
void SnakeGame_Render(GameState* state);

// Helper functions
void SnakeGame_Reset(SnakeGameData* data);
void SnakeGame_SpawnFood(SnakeGameData* data);
bool SnakeGame_CheckCollision(SnakeGameData* data);
void SnakeGame_GrowSnake(SnakeGameData* data);

// Create a new snake game state
GameState* SnakeGame_Create(void);

#endif /* SNAKE_GAME_H */
