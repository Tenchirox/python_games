#include "games/tic_tac_toe_game.h"
#include "common/sdl_framework.h"
#include "common/input_handler.h"
#include "common/texture_manager.h"
#include "common/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define WINDOW_WIDTH 600
#define WINDOW_HEIGHT 600
#define CELL_SIZE 120
#define BOARD_SIZE (CELL_SIZE * 3)
#define BOARD_OFFSET_X ((WINDOW_WIDTH - BOARD_SIZE) / 2)
#define BOARD_OFFSET_Y ((WINDOW_HEIGHT - BOARD_SIZE) / 2)
#define BUTTON_WIDTH 200
#define BUTTON_HEIGHT 50
#define BUTTON_PADDING 20

// Create a new tic-tac-toe game state
GameState* TicTacToeGame_Create(void) {
    GameState* state = (GameState*)malloc(sizeof(GameState));
    if (!state) {
        return NULL;
    }
    
    // Initialize function pointers
    state->init = TicTacToeGame_Init;
    state->cleanup = TicTacToeGame_Cleanup;
    state->pause = TicTacToeGame_Pause;
    state->resume = TicTacToeGame_Resume;
    state->handleEvents = TicTacToeGame_HandleEvents;
    state->update = TicTacToeGame_Update;
    state->render = TicTacToeGame_Render;
    
    // Create game data
    state->data = malloc(sizeof(TicTacToeGameData));
    if (!state->data) {
        free(state);
        return NULL;
    }
    
    return state;
}

void TicTacToeGame_Init(GameState* state) {
    TicTacToeGameData* data = (TicTacToeGameData*)state->data;
    
    // Initialize random seed
    srand((unsigned int)time(NULL));
    
    // Initialize board rectangle
    data->boardRect.x = BOARD_OFFSET_X;
    data->boardRect.y = BOARD_OFFSET_Y;
    data->boardRect.w = BOARD_SIZE;
    data->boardRect.h = BOARD_SIZE;
    
    // Initialize cell rectangles
    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 3; col++) {
            data->cellRects[row][col].x = BOARD_OFFSET_X + col * CELL_SIZE;
            data->cellRects[row][col].y = BOARD_OFFSET_Y + row * CELL_SIZE;
            data->cellRects[row][col].w = CELL_SIZE;
            data->cellRects[row][col].h = CELL_SIZE;
        }
    }
    
    // Initialize reset button
    data->resetButtonRect.x = (WINDOW_WIDTH - BUTTON_WIDTH) / 2;
    data->resetButtonRect.y = BOARD_OFFSET_Y + BOARD_SIZE + BUTTON_PADDING;
    data->resetButtonRect.w = BUTTON_WIDTH;
    data->resetButtonRect.h = BUTTON_HEIGHT;
    
    // Initialize mode button
    data->modeButtonRect.x = (WINDOW_WIDTH - BUTTON_WIDTH) / 2;
    data->modeButtonRect.y = data->resetButtonRect.y + BUTTON_HEIGHT + BUTTON_PADDING;
    data->modeButtonRect.w = BUTTON_WIDTH;
    data->modeButtonRect.h = BUTTON_HEIGHT;
    
    // Initialize game data
    data->mode = MODE_TWO_PLAYERS;
    data->computerMoveDelay = 500; // 500ms delay for computer move
    data->computerThinking = false;
    
    // Reset the game
    TicTacToeGame_Reset(data);
}

void TicTacToeGame_Cleanup(GameState* state) {
    // Free game data
    free(state->data);
    state->data = NULL;
}

void TicTacToeGame_Pause(GameState* state) {
    // Nothing to do
    (void)state;
}

void TicTacToeGame_Resume(GameState* state) {
    // Nothing to do
    (void)state;
}

void TicTacToeGame_HandleEvents(GameState* state) {
    TicTacToeGameData* data = (TicTacToeGameData*)state->data;
    
    // Don't handle events if computer is thinking
    if (data->computerThinking) {
        return;
    }
    
    // Handle mouse clicks
    if (Input_IsMouseButtonJustPressed(SDL_BUTTON_LEFT)) {
        int mouseX, mouseY;
        Input_GetMousePosition(&mouseX, &mouseY);
        
        // Check if reset button was clicked
        if (SDL_PointInRect(&(SDL_Point){mouseX, mouseY}, &data->resetButtonRect)) {
            TicTacToeGame_Reset(data);
            return;
        }
        
        // Check if mode button was clicked
        if (SDL_PointInRect(&(SDL_Point){mouseX, mouseY}, &data->modeButtonRect)) {
            TicTacToeGame_ToggleMode(data);
            return;
        }
        
        // Check if game is over
        if (data->gameOver) {
            return;
        }
        
        // Check if a cell was clicked
        for (int row = 0; row < 3; row++) {
            for (int col = 0; col < 3; col++) {
                if (SDL_PointInRect(&(SDL_Point){mouseX, mouseY}, &data->cellRects[row][col])) {
                    // Make move if cell is empty
                    if (data->board[row][col] == PLAYER_NONE) {
                        TicTacToeGame_MakeMove(data, row, col);
                    }
                    return;
                }
            }
        }
    }
}

void TicTacToeGame_Update(GameState* state) {
    TicTacToeGameData* data = (TicTacToeGameData*)state->data;
    
    // Check if game is over
    if (data->gameOver) {
        return;
    }
    
    // Check if it's computer's turn
    if (data->mode == MODE_VS_COMPUTER && data->currentPlayer == PLAYER_O) {
        // Check if computer is thinking
        if (data->computerThinking) {
            // Check if thinking time is over
            Uint32 currentTime = SDL_GetTicks();
            if (currentTime - data->lastComputerMoveTime >= data->computerMoveDelay) {
                // Make computer move
                TicTacToeGame_ComputerMove(data);
                data->computerThinking = false;
            }
        } else {
            // Start computer thinking
            data->computerThinking = true;
            data->lastComputerMoveTime = SDL_GetTicks();
        }
    }
}

void TicTacToeGame_Render(GameState* state) {
    TicTacToeGameData* data = (TicTacToeGameData*)state->data;
    SDL_Renderer* renderer = SDL_Framework_GetRenderer();
    
    // Clear screen
    SDL_Framework_ClearScreen(240, 240, 240, 255);
    
    // Draw board background
    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
    SDL_RenderFillRect(renderer, &data->boardRect);
    
    // Draw grid lines
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    
    // Vertical lines
    for (int i = 1; i < 3; i++) {
        SDL_RenderDrawLine(
            renderer,
            BOARD_OFFSET_X + i * CELL_SIZE,
            BOARD_OFFSET_Y,
            BOARD_OFFSET_X + i * CELL_SIZE,
            BOARD_OFFSET_Y + BOARD_SIZE
        );
    }
    
    // Horizontal lines
    for (int i = 1; i < 3; i++) {
        SDL_RenderDrawLine(
            renderer,
            BOARD_OFFSET_X,
            BOARD_OFFSET_Y + i * CELL_SIZE,
            BOARD_OFFSET_X + BOARD_SIZE,
            BOARD_OFFSET_Y + i * CELL_SIZE
        );
    }
    
    // Draw X's and O's
    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 3; col++) {
            SDL_Rect cellRect = data->cellRects[row][col];
            int padding = CELL_SIZE / 6;
            
            if (data->board[row][col] == PLAYER_X) {
                // Draw X
                SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
                SDL_RenderDrawLine(
                    renderer,
                    cellRect.x + padding,
                    cellRect.y + padding,
                    cellRect.x + cellRect.w - padding,
                    cellRect.y + cellRect.h - padding
                );
                SDL_RenderDrawLine(
                    renderer,
                    cellRect.x + cellRect.w - padding,
                    cellRect.y + padding,
                    cellRect.x + padding,
                    cellRect.y + cellRect.h - padding
                );
            } else if (data->board[row][col] == PLAYER_O) {
                // Draw O
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
                int centerX = cellRect.x + cellRect.w / 2;
                int centerY = cellRect.y + cellRect.h / 2;
                int radius = (cellRect.w - padding * 2) / 2;
                
                // Draw circle using multiple lines
                for (int i = 0; i < 360; i += 10) {
                    double angle1 = i * 3.14159265 / 180.0;
                    double angle2 = (i + 10) * 3.14159265 / 180.0;
                    
                    SDL_RenderDrawLine(
                        renderer,
                        centerX + (int)(radius * cos(angle1)),
                        centerY + (int)(radius * sin(angle1)),
                        centerX + (int)(radius * cos(angle2)),
                        centerY + (int)(radius * sin(angle2))
                    );
                }
            }
        }
    }
    
    // Draw reset button
    SDL_SetRenderDrawColor(renderer, 100, 100, 200, 255);
    SDL_RenderFillRect(renderer, &data->resetButtonRect);
    
    // Draw reset button text
    SDL_Color textColor = {255, 255, 255, 255};
    TextureManager_DrawText("menu_font", "New Game", 
                           data->resetButtonRect.x + 50, 
                           data->resetButtonRect.y + 15, 
                           textColor);
    
    // Draw mode button
    SDL_SetRenderDrawColor(renderer, 100, 200, 100, 255);
    SDL_RenderFillRect(renderer, &data->modeButtonRect);
    
    // Draw mode button text
    const char* modeText = (data->mode == MODE_TWO_PLAYERS) ? "2 Players" : "vs Computer";
    TextureManager_DrawText("menu_font", modeText, 
                           data->modeButtonRect.x + 50, 
                           data->modeButtonRect.y + 15, 
                           textColor);
    
    // Draw current player or game result
    char statusText[64];
    if (data->gameOver) {
        if (data->winner == PLAYER_NONE) {
            sprintf(statusText, "Game Over: Draw!");
        } else {
            sprintf(statusText, "Game Over: %s wins!", 
                   (data->winner == PLAYER_X) ? "X" : "O");
        }
    } else {
        sprintf(statusText, "Current Player: %s", 
               (data->currentPlayer == PLAYER_X) ? "X" : "O");
    }
    
    TextureManager_DrawText("menu_font", statusText, 
                           (WINDOW_WIDTH - 200) / 2, 
                           BOARD_OFFSET_Y - 40, 
                           (SDL_Color){0, 0, 0, 255});
}

void TicTacToeGame_Reset(TicTacToeGameData* data) {
    // Clear the board
    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 3; col++) {
            data->board[row][col] = PLAYER_NONE;
        }
    }
    
    // Reset game state
    data->currentPlayer = PLAYER_X;
    data->gameOver = false;
    data->winner = PLAYER_NONE;
    data->computerThinking = false;
}

bool TicTacToeGame_CheckWinner(TicTacToeGameData* data, Player* winner) {
    // Check rows
    for (int row = 0; row < 3; row++) {
        if (data->board[row][0] != PLAYER_NONE &&
            data->board[row][0] == data->board[row][1] &&
            data->board[row][1] == data->board[row][2]) {
            *winner = data->board[row][0];
            return true;
        }
    }
    
    // Check columns
    for (int col = 0; col < 3; col++) {
        if (data->board[0][col] != PLAYER_NONE &&
            data->board[0][col] == data->board[1][col] &&
            data->board[1][col] == data->board[2][col]) {
            *winner = data->board[0][col];
            return true;
        }
    }
    
    // Check diagonals
    if (data->board[0][0] != PLAYER_NONE &&
        data->board[0][0] == data->board[1][1] &&
        data->board[1][1] == data->board[2][2]) {
        *winner = data->board[0][0];
        return true;
    }
    
    if (data->board[0][2] != PLAYER_NONE &&
        data->board[0][2] == data->board[1][1] &&
        data->board[1][1] == data->board[2][0]) {
        *winner = data->board[0][2];
        return true;
    }
    
    // No winner
    *winner = PLAYER_NONE;
    return false;
}

bool TicTacToeGame_IsBoardFull(TicTacToeGameData* data) {
    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 3; col++) {
            if (data->board[row][col] == PLAYER_NONE) {
                return false;
            }
        }
    }
    return true;
}

void TicTacToeGame_MakeMove(TicTacToeGameData* data, int row, int col) {
    // Make the move
    data->board[row][col] = data->currentPlayer;
    
    // Check for winner
    if (TicTacToeGame_CheckWinner(data, &data->winner)) {
        data->gameOver = true;
        return;
    }
    
    // Check for draw
    if (TicTacToeGame_IsBoardFull(data)) {
        data->gameOver = true;
        data->winner = PLAYER_NONE;
        return;
    }
    
    // Switch player
    data->currentPlayer = (data->currentPlayer == PLAYER_X) ? PLAYER_O : PLAYER_X;
}

void TicTacToeGame_ComputerMove(TicTacToeGameData* data) {
    int row = -1, col = -1;
    
    // Try to win
    if (TicTacToeGame_CheckWinningMove(data, PLAYER_O, &row, &col)) {
        TicTacToeGame_MakeMove(data, row, col);
        return;
    }
    
    // Try to block
    if (TicTacToeGame_CheckWinningMove(data, PLAYER_X, &row, &col)) {
        TicTacToeGame_MakeMove(data, row, col);
        return;
    }
    
    // Try center
    if (data->board[1][1] == PLAYER_NONE) {
        TicTacToeGame_MakeMove(data, 1, 1);
        return;
    }
    
    // Try corners
    int corners[4][2] = {{0, 0}, {0, 2}, {2, 0}, {2, 2}};
    int availableCorners[4][2];
    int numAvailableCorners = 0;
    
    for (int i = 0; i < 4; i++) {
        row = corners[i][0];
        col = corners[i][1];
        if (data->board[row][col] == PLAYER_NONE) {
            availableCorners[numAvailableCorners][0] = row;
            availableCorners[numAvailableCorners][1] = col;
            numAvailableCorners++;
        }
    }
    
    if (numAvailableCorners > 0) {
        int randomIndex = rand() % numAvailableCorners;
        row = availableCorners[randomIndex][0];
        col = availableCorners[randomIndex][1];
        TicTacToeGame_MakeMove(data, row, col);
        return;
    }
    
    // Try any available move
    int availableMoves[9][2];
    int numAvailableMoves = 0;
    
    for (int r = 0; r < 3; r++) {
        for (int c = 0; c < 3; c++) {
            if (data->board[r][c] == PLAYER_NONE) {
                availableMoves[numAvailableMoves][0] = r;
                availableMoves[numAvailableMoves][1] = c;
                numAvailableMoves++;
            }
        }
    }
    
    if (numAvailableMoves > 0) {
        int randomIndex = rand() % numAvailableMoves;
        row = availableMoves[randomIndex][0];
        col = availableMoves[randomIndex][1];
        TicTacToeGame_MakeMove(data, row, col);
    }
}

bool TicTacToeGame_CheckWinningMove(TicTacToeGameData* data, Player player, int* row, int* col) {
    // Check each empty cell
    for (int r = 0; r < 3; r++) {
        for (int c = 0; c < 3; c++) {
            if (data->board[r][c] == PLAYER_NONE) {
                // Try the move
                data->board[r][c] = player;
                
                // Check if it's a winning move
                Player winner;
                bool isWinner = TicTacToeGame_CheckWinner(data, &winner);
                
                // Undo the move
                data->board[r][c] = PLAYER_NONE;
                
                // If it's a winning move, return the position
                if (isWinner && winner == player) {
                    *row = r;
                    *col = c;
                    return true;
                }
            }
        }
    }
    
    // No winning move found
    return false;
}

void TicTacToeGame_ToggleMode(TicTacToeGameData* data) {
    data->mode = (data->mode == MODE_TWO_PLAYERS) ? MODE_VS_COMPUTER : MODE_TWO_PLAYERS;
    TicTacToeGame_Reset(data);
}

int main(int argc, char* argv[]) {
    // Initialize SDL framework
    if (!SDL_Framework_Init("Tic-Tac-Toe", WINDOW_WIDTH, WINDOW_HEIGHT)) {
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
    
    // Create tic-tac-toe game state
    GameState* gameState = TicTacToeGame_Create();
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

