#ifndef CONNECT_FOUR_GAME_H
#define CONNECT_FOUR_GAME_H

#include "game_state.h"
#include <SDL2/SDL.h>
#include <stdbool.h>

// Connect Four game state
typedef struct {
    int gridWidth;
    int gridHeight;
    int cellSize;
    int score[2];  // Score for player 1 and player 2
    bool gameOver;
    
    int** board;   // 2D array for the game board (0=empty, 1=player1, 2=player2)
    int currentPlayer;  // 1 or 2
    
    // Animation variables
    bool animating;
    int animCol;
    int animRow;
    float animY;
    float animSpeed;
    
    // AI variables
    bool aiEnabled;
    int aiPlayer;  // Which player the AI controls (usually 2)
    
    // UI variables
    int hoverColumn;  // Column the mouse is hovering over
    
    // Timing variables
    Uint32 lastUpdateTime;
} ConnectFourGameData;

// Game state functions
void ConnectFourGame_Init(GameState* state);
void ConnectFourGame_Cleanup(GameState* state);
void ConnectFourGame_Pause(GameState* state);
void ConnectFourGame_Resume(GameState* state);
void ConnectFourGame_HandleEvents(GameState* state);
void ConnectFourGame_Update(GameState* state);
void ConnectFourGame_Render(GameState* state);

// Helper functions
void ConnectFourGame_Reset(ConnectFourGameData* data);
bool ConnectFourGame_CheckWinner(ConnectFourGameData* data, int row, int col);
bool ConnectFourGame_IsBoardFull(ConnectFourGameData* data);
void ConnectFourGame_DropPiece(ConnectFourGameData* data, int col);
int ConnectFourGame_GetAIMove(ConnectFourGameData* data);
int ConnectFourGame_EvaluateBoard(ConnectFourGameData* data, int row, int col, int player);
int ConnectFourGame_EvaluateWindow(int* window, int length, int player);

// Create a new Connect Four game state
GameState* ConnectFourGame_Create(void);

#endif /* CONNECT_FOUR_GAME_H */

