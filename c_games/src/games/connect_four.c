#include "games/connect_four_game.h"
#include "sdl_framework.h"
#include "input_handler.h"
#include "texture_manager.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define WINDOW_WIDTH 700
#define WINDOW_HEIGHT 600
#define GRID_WIDTH 7
#define GRID_HEIGHT 6
#define CELL_SIZE 80
#define BOARD_OFFSET_X 50
#define BOARD_OFFSET_Y 100
#define ANIMATION_SPEED 15.0f
#define AI_THINKING_TIME 500  // milliseconds

// Create a new Connect Four game state
GameState* ConnectFourGame_Create(void) {
    GameState* state = (GameState*)malloc(sizeof(GameState));
    if (!state) {
        return NULL;
    }
    
    // Initialize function pointers
    state->init = ConnectFourGame_Init;
    state->cleanup = ConnectFourGame_Cleanup;
    state->pause = ConnectFourGame_Pause;
    state->resume = ConnectFourGame_Resume;
    state->handleEvents = ConnectFourGame_HandleEvents;
    state->update = ConnectFourGame_Update;
    state->render = ConnectFourGame_Render;
    
    // Create game data
    state->data = malloc(sizeof(ConnectFourGameData));
    if (!state->data) {
        free(state);
        return NULL;
    }
    
    return state;
}

void ConnectFourGame_Init(GameState* state) {
    ConnectFourGameData* data = (ConnectFourGameData*)state->data;
    
    // Initialize game data
    data->gridWidth = GRID_WIDTH;
    data->gridHeight = GRID_HEIGHT;
    data->cellSize = CELL_SIZE;
    data->score[0] = 0;  // Player 1 score
    data->score[1] = 0;  // Player 2 score
    data->gameOver = false;
    data->aiEnabled = false;  // AI disabled by default
    data->aiPlayer = 2;       // AI is player 2 by default
    data->hoverColumn = -1;
    
    // Initialize the game board
    data->board = (int**)malloc(data->gridHeight * sizeof(int*));
    if (!data->board) {
        fprintf(stderr, "Failed to allocate memory for game board\n");
        return;
    }
    
    for (int i = 0; i < data->gridHeight; i++) {
        data->board[i] = (int*)malloc(data->gridWidth * sizeof(int));
        if (!data->board[i]) {
            fprintf(stderr, "Failed to allocate memory for game board row\n");
            
            // Free previously allocated rows
            for (int j = 0; j < i; j++) {
                free(data->board[j]);
            }
            free(data->board);
            data->board = NULL;
            return;
        }
    }
    
    // Initialize random seed
    srand((unsigned int)time(NULL));
    
    // Reset the game
    ConnectFourGame_Reset(data);
}

void ConnectFourGame_Cleanup(GameState* state) {
    ConnectFourGameData* data = (ConnectFourGameData*)state->data;
    
    // Free the game board
    if (data->board) {
        for (int i = 0; i < data->gridHeight; i++) {
            free(data->board[i]);
        }
        free(data->board);
        data->board = NULL;
    }
    
    // Free game data
    free(data);
    state->data = NULL;
}

void ConnectFourGame_Pause(GameState* state) {
    // Nothing to do
    (void)state;
}

void ConnectFourGame_Resume(GameState* state) {
    // Nothing to do
    (void)state;
}

void ConnectFourGame_HandleEvents(GameState* state) {
    ConnectFourGameData* data = (ConnectFourGameData*)state->data;
    
    // Don't handle events if the game is over or animating
    if (data->gameOver || data->animating) {
        return;
    }
    
    // Get mouse position
    int mouseX, mouseY;
    Input_GetMousePosition(&mouseX, &mouseY);
    
    // Calculate which column the mouse is over
    if (mouseX >= BOARD_OFFSET_X && mouseX < BOARD_OFFSET_X + data->gridWidth * data->cellSize &&
        mouseY >= BOARD_OFFSET_Y && mouseY < BOARD_OFFSET_Y + data->gridHeight * data->cellSize) {
        data->hoverColumn = (mouseX - BOARD_OFFSET_X) / data->cellSize;
    } else {
        data->hoverColumn = -1;
    }
    
    // Handle mouse click to drop a piece
    if (Input_IsMouseButtonJustClicked(SDL_BUTTON_LEFT) && data->hoverColumn >= 0) {
        // Check if the column is not full
        if (data->board[0][data->hoverColumn] == 0) {
            ConnectFourGame_DropPiece(data, data->hoverColumn);
        }
    }
    
    // Toggle AI with 'A' key
    if (Input_IsKeyJustPressed(SDLK_a)) {
        data->aiEnabled = !data->aiEnabled;
        printf("AI %s\n", data->aiEnabled ? "enabled" : "disabled");
    }
    
    // Reset game with 'R' key
    if (Input_IsKeyJustPressed(SDLK_r)) {
        ConnectFourGame_Reset(data);
    }
}

void ConnectFourGame_Update(GameState* state) {
    ConnectFourGameData* data = (ConnectFourGameData*)state->data;
    
    // Don't update if the game is over
    if (data->gameOver) {
        return;
    }
    
    // Handle animation
    if (data->animating) {
        // Calculate target Y position for the piece
        float targetY = BOARD_OFFSET_Y + (data->animRow + 0.5f) * data->cellSize;
        
        // Update animation position
        data->animY += data->animSpeed;
        
        // Check if animation is complete
        if (data->animY >= targetY) {
            data->animY = targetY;
            data->animating = false;
            
            // Check for winner
            if (ConnectFourGame_CheckWinner(data, data->animRow, data->animCol)) {
                data->gameOver = true;
                data->score[data->currentPlayer - 1]++;
                printf("Player %d wins!\n", data->currentPlayer);
            } else if (ConnectFourGame_IsBoardFull(data)) {
                data->gameOver = true;
                printf("Game over! It's a draw.\n");
            } else {
                // Switch to the next player
                data->currentPlayer = 3 - data->currentPlayer;  // Toggle between 1 and 2
            }
        }
        
        return;
    }
    
    // AI turn
    if (data->aiEnabled && data->currentPlayer == data->aiPlayer) {
        Uint32 currentTime = SDL_GetTicks();
        
        // Add a small delay before AI makes a move
        if (currentTime - data->lastUpdateTime >= AI_THINKING_TIME) {
            int aiCol = ConnectFourGame_GetAIMove(data);
            if (aiCol >= 0) {
                ConnectFourGame_DropPiece(data, aiCol);
            }
            data->lastUpdateTime = currentTime;
        }
    }
}

void ConnectFourGame_Render(GameState* state) {
    ConnectFourGameData* data = (ConnectFourGameData*)state->data;
    SDL_Renderer* renderer = SDL_Framework_GetRenderer();
    
    // Clear screen with dark background
    SDL_Framework_ClearScreen(47, 47, 47, 255);
    
    // Draw game board background (blue)
    SDL_SetRenderDrawColor(renderer, 0, 0, 170, 255);
    SDL_Rect boardRect = {
        BOARD_OFFSET_X, 
        BOARD_OFFSET_Y, 
        data->gridWidth * data->cellSize, 
        data->gridHeight * data->cellSize
    };
    SDL_RenderFillRect(renderer, &boardRect);
    
    // Draw grid cells (empty spaces)
    for (int row = 0; row < data->gridHeight; row++) {
        for (int col = 0; col < data->gridWidth; col++) {
            // Calculate cell position
            int x = BOARD_OFFSET_X + col * data->cellSize;
            int y = BOARD_OFFSET_Y + row * data->cellSize;
            
            // Draw empty cell (dark circle)
            SDL_SetRenderDrawColor(renderer, 31, 31, 31, 255);
            int cellPadding = 5;
            int radius = (data->cellSize / 2) - cellPadding;
            
            // Draw filled circle for the empty cell
            for (int dy = -radius; dy <= radius; dy++) {
                for (int dx = -radius; dx <= radius; dx++) {
                    if (dx*dx + dy*dy <= radius*radius) {
                        SDL_RenderDrawPoint(renderer, 
                                           x + data->cellSize/2 + dx, 
                                           y + data->cellSize/2 + dy);
                    }
                }
            }
        }
    }
    
    // Draw pieces on the board
    for (int row = 0; row < data->gridHeight; row++) {
        for (int col = 0; col < data->gridWidth; col++) {
            if (data->board[row][col] != 0) {
                // Calculate piece position
                int x = BOARD_OFFSET_X + col * data->cellSize;
                int y = BOARD_OFFSET_Y + row * data->cellSize;
                
                // Set color based on player
                if (data->board[row][col] == 1) {
                    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);  // Red for player 1
                } else {
                    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);  // Yellow for player 2
                }
                
                // Draw filled circle for the piece
                int radius = (data->cellSize / 2) - 5;
                for (int dy = -radius; dy <= radius; dy++) {
                    for (int dx = -radius; dx <= radius; dx++) {
                        if (dx*dx + dy*dy <= radius*radius) {
                            SDL_RenderDrawPoint(renderer, 
                                               x + data->cellSize/2 + dx, 
                                               y + data->cellSize/2 + dy);
                        }
                    }
                }
                
                // Draw highlight effect (small white circle in top-left)
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                int highlightRadius = radius / 3;
                int highlightOffsetX = -radius / 2;
                int highlightOffsetY = -radius / 2;
                
                for (int dy = -highlightRadius; dy <= highlightRadius; dy++) {
                    for (int dx = -highlightRadius; dx <= highlightRadius; dx++) {
                        if (dx*dx + dy*dy <= highlightRadius*highlightRadius) {
                            SDL_RenderDrawPoint(renderer, 
                                               x + data->cellSize/2 + highlightOffsetX + dx, 
                                               y + data->cellSize/2 + highlightOffsetY + dy);
                        }
                    }
                }
            }
        }
    }
    
    // Draw the piece being animated
    if (data->animating) {
        int x = BOARD_OFFSET_X + data->animCol * data->cellSize + data->cellSize / 2;
        int y = (int)data->animY;
        
        // Set color based on player
        if (data->currentPlayer == 1) {
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);  // Red for player 1
        } else {
            SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);  // Yellow for player 2
        }
        
        // Draw filled circle for the piece
        int radius = (data->cellSize / 2) - 5;
        for (int dy = -radius; dy <= radius; dy++) {
            for (int dx = -radius; dx <= radius; dx++) {
                if (dx*dx + dy*dy <= radius*radius) {
                    SDL_RenderDrawPoint(renderer, x + dx, y + dy);
                }
            }
        }
        
        // Draw highlight effect
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        int highlightRadius = radius / 3;
        int highlightOffsetX = -radius / 2;
        int highlightOffsetY = -radius / 2;
        
        for (int dy = -highlightRadius; dy <= highlightRadius; dy++) {
            for (int dx = -highlightRadius; dx <= highlightRadius; dx++) {
                if (dx*dx + dy*dy <= highlightRadius*highlightRadius) {
                    SDL_RenderDrawPoint(renderer, 
                                       x + highlightOffsetX + dx, 
                                       y + highlightOffsetY + dy);
                }
            }
        }
    }
    
    // Draw hover indicator
    if (data->hoverColumn >= 0 && !data->animating && !data->gameOver) {
        int x = BOARD_OFFSET_X + data->hoverColumn * data->cellSize;
        int y = BOARD_OFFSET_Y - data->cellSize;
        
        // Set color based on current player
        if (data->currentPlayer == 1) {
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 128);  // Semi-transparent red
        } else {
            SDL_SetRenderDrawColor(renderer, 255, 255, 0, 128);  // Semi-transparent yellow
        }
        
        // Draw semi-transparent circle for hover indicator
        int radius = (data->cellSize / 2) - 5;
        for (int dy = -radius; dy <= radius; dy++) {
            for (int dx = -radius; dx <= radius; dx++) {
                if (dx*dx + dy*dy <= radius*radius) {
                    SDL_RenderDrawPoint(renderer, 
                                       x + data->cellSize/2 + dx, 
                                       y + data->cellSize/2 + dy);
                }
            }
        }
    }
    
    // Draw scores and current player
    SDL_Color textColor = {255, 255, 255, 255};
    char scoreText[100];
    sprintf(scoreText, "Player 1: %d   Player 2: %d", data->score[0], data->score[1]);
    TextureManager_DrawText("menu_font", scoreText, 20, 20, textColor);
    
    char turnText[50];
    sprintf(turnText, "Current Player: %d", data->currentPlayer);
    TextureManager_DrawText("menu_font", turnText, 20, 50, textColor);
    
    // Draw AI status
    char aiText[50];
    sprintf(aiText, "AI: %s (Press A to toggle)", data->aiEnabled ? "ON" : "OFF");
    TextureManager_DrawText("menu_font", aiText, WINDOW_WIDTH - 300, 20, textColor);
    
    // Draw game over message
    if (data->gameOver) {
        SDL_Color gameOverColor = {255, 255, 255, 255};
        char gameOverText[100];
        
        if (ConnectFourGame_IsBoardFull(data) && !ConnectFourGame_CheckWinner(data, data->animRow, data->animCol)) {
            sprintf(gameOverText, "Game Over! It's a draw!");
        } else {
            sprintf(gameOverText, "Player %d wins!", 3 - data->currentPlayer);
        }
        
        TextureManager_DrawText("menu_font", gameOverText, WINDOW_WIDTH / 2 - 100, WINDOW_HEIGHT - 50, gameOverColor);
        TextureManager_DrawText("menu_font", "Press R to play again", WINDOW_WIDTH / 2 - 100, WINDOW_HEIGHT - 25, gameOverColor);
    }
}

void ConnectFourGame_Reset(ConnectFourGameData* data) {
    // Reset the game board
    for (int row = 0; row < data->gridHeight; row++) {
        for (int col = 0; col < data->gridWidth; col++) {
            data->board[row][col] = 0;
        }
    }
    
    // Reset game state
    data->currentPlayer = 1;  // Player 1 starts
    data->gameOver = false;
    data->animating = false;
    data->lastUpdateTime = SDL_GetTicks();
}

bool ConnectFourGame_CheckWinner(ConnectFourGameData* data, int row, int col) {
    int player = data->board[row][col];
    if (player == 0) return false;
    
    // Check horizontally
    int count = 0;
    for (int c = 0; c < data->gridWidth; c++) {
        if (data->board[row][c] == player) {
            count++;
            if (count >= 4) return true;
        } else {
            count = 0;
        }
    }
    
    // Check vertically
    count = 0;
    for (int r = 0; r < data->gridHeight; r++) {
        if (data->board[r][col] == player) {
            count++;
            if (count >= 4) return true;
        } else {
            count = 0;
        }
    }
    
    // Check diagonal (top-left to bottom-right)
    int startRow = row;
    int startCol = col;
    
    // Find the top-left point of the diagonal
    while (startRow > 0 && startCol > 0) {
        startRow--;
        startCol--;
    }
    
    count = 0;
    int r = startRow;
    int c = startCol;
    
    while (r < data->gridHeight && c < data->gridWidth) {
        if (data->board[r][c] == player) {
            count++;
            if (count >= 4) return true;
        } else {
            count = 0;
        }
        r++;
        c++;
    }
    
    // Check diagonal (bottom-left to top-right)
    startRow = row;
    startCol = col;
    
    // Find the bottom-left point of the diagonal
    while (startRow < data->gridHeight - 1 && startCol > 0) {
        startRow++;
        startCol--;
    }
    
    count = 0;
    r = startRow;
    c = startCol;
    
    while (r >= 0 && c < data->gridWidth) {
        if (data->board[r][c] == player) {
            count++;
            if (count >= 4) return true;
        } else {
            count = 0;
        }
        r--;
        c++;
    }
    
    return false;
}

bool ConnectFourGame_IsBoardFull(ConnectFourGameData* data) {
    // Check if the top row is full
    for (int col = 0; col < data->gridWidth; col++) {
        if (data->board[0][col] == 0) {
            return false;
        }
    }
    return true;
}

void ConnectFourGame_DropPiece(ConnectFourGameData* data, int col) {
    // Find the lowest empty row in the column
    int row = data->gridHeight - 1;
    while (row >= 0 && data->board[row][col] != 0) {
        row--;
    }
    
    // If the column is full, do nothing
    if (row < 0) {
        return;
    }
    
    // Start the animation
    data->animating = true;
    data->animCol = col;
    data->animRow = row;
    data->animY = BOARD_OFFSET_Y - data->cellSize / 2;  // Start above the board
    data->animSpeed = ANIMATION_SPEED;
    
    // Place the piece on the board
    data->board[row][col] = data->currentPlayer;
}

int ConnectFourGame_GetAIMove(ConnectFourGameData* data) {
    int bestScore = -1000000;
    int bestCol = -1;
    int validMoves[GRID_WIDTH];
    int numValidMoves = 0;
    
    // Find all valid moves
    for (int col = 0; col < data->gridWidth; col++) {
        if (data->board[0][col] == 0) {  // If the column is not full
            validMoves[numValidMoves++] = col;
        }
    }
    
    // If no valid moves, return -1
    if (numValidMoves == 0) {
        return -1;
    }
    
    // Evaluate each valid move
    for (int i = 0; i < numValidMoves; i++) {
        int col = validMoves[i];
        
        // Find the row where the piece would land
        int row = data->gridHeight - 1;
        while (row >= 0 && data->board[row][col] != 0) {
            row--;
        }
        
        // Simulate the move
        data->board[row][col] = data->aiPlayer;
        
        // Evaluate the board after this move
        int score = ConnectFourGame_EvaluateBoard(data, row, col, data->aiPlayer);
        
        // Undo the move
        data->board[row][col] = 0;
        
        // Update best move if needed
        if (score > bestScore) {
            bestScore = score;
            bestCol = col;
        }
    }
    
    return bestCol;
}

int ConnectFourGame_EvaluateBoard(ConnectFourGameData* data, int row, int col, int player) {
    int score = 0;
    int opponent = 3 - player;  // The other player
    
    // Check if this move is a winning move
    if (ConnectFourGame_CheckWinner(data, row, col)) {
        return 1000000;  // Very high score for a winning move
    }
    
    // Check if the opponent can win in the next move
    for (int c = 0; c < data->gridWidth; c++) {
        if (data->board[0][c] != 0) continue;  // Column is full
        
        // Find the row where the opponent's piece would land
        int r = data->gridHeight - 1;
        while (r >= 0 && data->board[r][c] != 0) {
            r--;
        }
        
        // Simulate the opponent's move
        data->board[r][c] = opponent;
        
        // Check if the opponent would win
        if (ConnectFourGame_CheckWinner(data, r, c)) {
            // Undo the move
            data->board[r][c] = 0;
            
            // This is a move we must block
            if (c == col) {
                return 900000;  // High score for blocking a winning move
            } else {
                score -= 800000;  // Penalty for allowing a winning move elsewhere
            }
        }
        
        // Undo the move
        data->board[r][c] = 0;
    }
    
    // Evaluate horizontal windows
    int window[4];
    for (int c = 0; c <= data->gridWidth - 4; c++) {
        for (int i = 0; i < 4; i++) {
            window[i] = data->board[row][c + i];
        }
        score += ConnectFourGame_EvaluateWindow(window, 4, player);
    }
    
    // Evaluate vertical windows
    for (int r = 0; r <= data->gridHeight - 4; r++) {
        for (int i = 0; i < 4; i++) {
            window[i] = data->board[r + i][col];
        }
        score += ConnectFourGame_EvaluateWindow(window, 4, player);
    }
    
    // Evaluate diagonal windows (top-left to bottom-right)
    for (int r = 0; r <= data->gridHeight - 4; r++) {
        for (int c = 0; c <= data->gridWidth - 4; c++) {
            for (int i = 0; i < 4; i++) {
                window[i] = data->board[r + i][c + i];
            }
            score += ConnectFourGame_EvaluateWindow(window, 4, player);
        }
    }
    
    // Evaluate diagonal windows (bottom-left to top-right)
    for (int r = 3; r < data->gridHeight; r++) {
        for (int c = 0; c <= data->gridWidth - 4; c++) {
            for (int i = 0; i < 4; i++) {
                window[i] = data->board[r - i][c + i];
            }
            score += ConnectFourGame_EvaluateWindow(window, 4, player);
        }
    }
    
    // Prefer center columns
    int centerCol = data->gridWidth / 2;
    if (col == centerCol) {
        score += 30;
    } else if (abs(col - centerCol) == 1) {
        score += 20;
    } else if (abs(col - centerCol) == 2) {
        score += 10;
    }
    
    return score;
}

int ConnectFourGame_EvaluateWindow(int* window, int length, int player) {
    int score = 0;
    int opponent = 3 - player;
    
    int playerCount = 0;
    int emptyCount = 0;
    int opponentCount = 0;
    
    for (int i = 0; i < length; i++) {
        if (window[i] == player) {
            playerCount++;
        } else if (window[i] == 0) {
            emptyCount++;
        } else {
            opponentCount++;
        }
    }
    
    // Score the window
    if (playerCount == 4) {
        score += 100;  // Four in a row
    } else if (playerCount == 3 && emptyCount == 1) {
        score += 5;    // Three in a row with an empty space
    } else if (playerCount == 2 && emptyCount == 2) {
        score += 2;    // Two in a row with two empty spaces
    }
    
    // Penalize opponent's pieces
    if (opponentCount == 3 && emptyCount == 1) {
        score -= 4;    // Block opponent's three in a row
    }
    
    return score;
}

int main(int argc, char* argv[]) {
    // Initialize SDL framework
    if (!SDL_Framework_Init("Connect Four", WINDOW_WIDTH, WINDOW_HEIGHT)) {
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
    
    // Create Connect Four game state
    GameState* gameState = ConnectFourGame_Create();
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

