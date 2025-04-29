#ifndef TIC_TAC_TOE_GAME_H
#define TIC_TAC_TOE_GAME_H

#include "game_state.h"
#include <SDL2/SDL.h>
#include <stdbool.h>

// Player enum
typedef enum {
    PLAYER_NONE,
    PLAYER_X,
    PLAYER_O
} Player;

// Game mode enum
typedef enum {
    MODE_TWO_PLAYERS,
    MODE_VS_COMPUTER
} GameMode;

// Board position
typedef struct {
    int row;
    int col;
} BoardPosition;

// Tic-tac-toe game state
typedef struct {
    // Game board (3x3)
    Player board[3][3];
    
    // Game state
    Player currentPlayer;
    bool gameOver;
    Player winner;
    GameMode mode;
    
    // UI elements
    SDL_Rect boardRect;
    SDL_Rect cellRects[3][3];
    SDL_Rect resetButtonRect;
    SDL_Rect modeButtonRect;
    
    // Timing
    Uint32 computerMoveDelay;
    Uint32 lastComputerMoveTime;
    bool computerThinking;
} TicTacToeGameData;

// Game state functions
void TicTacToeGame_Init(GameState* state);
void TicTacToeGame_Cleanup(GameState* state);
void TicTacToeGame_Pause(GameState* state);
void TicTacToeGame_Resume(GameState* state);
void TicTacToeGame_HandleEvents(GameState* state);
void TicTacToeGame_Update(GameState* state);
void TicTacToeGame_Render(GameState* state);

// Helper functions
void TicTacToeGame_Reset(TicTacToeGameData* data);
bool TicTacToeGame_CheckWinner(TicTacToeGameData* data, Player* winner);
bool TicTacToeGame_IsBoardFull(TicTacToeGameData* data);
void TicTacToeGame_MakeMove(TicTacToeGameData* data, int row, int col);
void TicTacToeGame_ComputerMove(TicTacToeGameData* data);
bool TicTacToeGame_CheckWinningMove(TicTacToeGameData* data, Player player, int* row, int* col);
void TicTacToeGame_ToggleMode(TicTacToeGameData* data);

// Create a new tic-tac-toe game state
GameState* TicTacToeGame_Create(void);

#endif /* TIC_TAC_TOE_GAME_H */

