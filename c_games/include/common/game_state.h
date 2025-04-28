#ifndef GAME_STATE_H
#define GAME_STATE_H

#include <stdbool.h>

/**
 * @brief Game state interface
 */
typedef struct GameState {
    void* data;                          // Game-specific data
    void (*init)(struct GameState* self);       // Initialize the game state
    void (*cleanup)(struct GameState* self);    // Clean up the game state
    void (*pause)(struct GameState* self);      // Pause the game state
    void (*resume)(struct GameState* self);     // Resume the game state
    void (*handleEvents)(struct GameState* self); // Handle events
    void (*update)(struct GameState* self);     // Update the game state
    void (*render)(struct GameState* self);     // Render the game state
} GameState;

/**
 * @brief Game state manager
 */
typedef struct {
    GameState* states[10];  // Stack of game states
    int numStates;          // Number of states on the stack
} GameStateManager;

/**
 * @brief Initialize the game state manager
 */
void GameStateManager_Init(void);

/**
 * @brief Clean up the game state manager
 */
void GameStateManager_Cleanup(void);

/**
 * @brief Change to a new game state
 * 
 * @param state New game state to change to
 */
void GameStateManager_ChangeState(GameState* state);

/**
 * @brief Push a new game state onto the stack
 * 
 * @param state New game state to push
 */
void GameStateManager_PushState(GameState* state);

/**
 * @brief Pop the top game state from the stack
 */
void GameStateManager_PopState(void);

/**
 * @brief Handle events for the current game state
 */
void GameStateManager_HandleEvents(void);

/**
 * @brief Update the current game state
 */
void GameStateManager_Update(void);

/**
 * @brief Render the current game state
 */
void GameStateManager_Render(void);

/**
 * @brief Get the current game state
 * 
 * @return GameState* Pointer to the current game state, or NULL if none
 */
GameState* GameStateManager_GetCurrentState(void);

#endif /* GAME_STATE_H */
