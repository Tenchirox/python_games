#include "games/space_invaders_game.h"
#include "common/sdl_framework.h"
#include "common/input_handler.h"
#include "common/texture_manager.h"
#include "common/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define PLAYER_WIDTH 50
#define PLAYER_HEIGHT 30
#define PLAYER_SPEED 300.0f
#define PLAYER_BULLET_SPEED 400.0f
#define INVADER_WIDTH 40
#define INVADER_HEIGHT 30
#define INVADER_HORIZONTAL_SPACING 15
#define INVADER_VERTICAL_SPACING 15
#define INVADER_ROWS 5
#define INVADER_COLS 11
#define INVADER_INITIAL_Y 80
#define INVADER_DROP_AMOUNT 20
#define INVADER_BULLET_SPEED 200.0f
#define BULLET_WIDTH 5
#define BULLET_HEIGHT 15
#define BARRIER_WIDTH 70
#define BARRIER_HEIGHT 50
#define BARRIER_Y 450
#define BARRIER_SEGMENT_SIZE 8
#define PLAYER_SHOOT_COOLDOWN 0.5f
#define INVADER_INITIAL_MOVE_DELAY 1.0f
#define INVADER_MIN_MOVE_DELAY 0.1f
#define INVADER_SHOOT_DELAY 1.0f

// Create a new space invaders game state
GameState* SpaceInvadersGame_Create(void) {
    GameState* state = (GameState*)malloc(sizeof(GameState));
    if (!state) {
        return NULL;
    }
    
    // Initialize function pointers
    state->init = SpaceInvadersGame_Init;
    state->cleanup = SpaceInvadersGame_Cleanup;
    state->pause = SpaceInvadersGame_Pause;
    state->resume = SpaceInvadersGame_Resume;
    state->handleEvents = SpaceInvadersGame_HandleEvents;
    state->update = SpaceInvadersGame_Update;
    state->render = SpaceInvadersGame_Render;
    
    // Create game data
    state->data = malloc(sizeof(SpaceInvadersGameData));
    if (!state->data) {
        free(state);
        return NULL;
    }
    
    return state;
}

void SpaceInvadersGame_Init(GameState* state) {
    SpaceInvadersGameData* data = (SpaceInvadersGameData*)state->data;
    
    // Initialize random seed
    srand((unsigned int)time(NULL));
    
    // Initialize screen dimensions
    data->screenWidth = WINDOW_WIDTH;
    data->screenHeight = WINDOW_HEIGHT;
    
    // Initialize timing
    data->lastUpdateTime = SDL_GetTicks();
    data->deltaTime = 0.0f;
    
    // Reset the game
    SpaceInvadersGame_Reset(data);
}

void SpaceInvadersGame_Cleanup(GameState* state) {
    // Free game data
    free(state->data);
    state->data = NULL;
}

void SpaceInvadersGame_Pause(GameState* state) {
    // Nothing to do
    (void)state;
}

void SpaceInvadersGame_Resume(GameState* state) {
    // Nothing to do
    (void)state;
}

void SpaceInvadersGame_HandleEvents(GameState* state) {
    SpaceInvadersGameData* data = (SpaceInvadersGameData*)state->data;
    
    // Don't handle events if game is over
    if (data->gameOver || data->gameWon) {
        // Reset game if R is pressed
        if (Input_IsKeyJustPressed(SDLK_r)) {
            SpaceInvadersGame_Reset(data);
        }
        return;
    }
    
    // Handle player movement
    if (Input_IsKeyPressed(SDLK_LEFT)) {
        data->player.velocityX = -PLAYER_SPEED;
    } else if (Input_IsKeyPressed(SDLK_RIGHT)) {
        data->player.velocityX = PLAYER_SPEED;
    } else {
        data->player.velocityX = 0;
    }
    
    // Handle player shooting
    if (Input_IsKeyJustPressed(SDLK_SPACE)) {
        SpaceInvadersGame_FirePlayerBullet(data);
    }
}

void SpaceInvadersGame_Update(GameState* state) {
    SpaceInvadersGameData* data = (SpaceInvadersGameData*)state->data;
    
    // Calculate delta time
    Uint32 currentTime = SDL_GetTicks();
    data->deltaTime = (currentTime - data->lastUpdateTime) / 1000.0f;
    data->lastUpdateTime = currentTime;
    
    // Don't update if game is over
    if (data->gameOver || data->gameWon) {
        return;
    }
    
    // Update player
    SpaceInvadersGame_UpdatePlayer(data);
    
    // Update invaders
    SpaceInvadersGame_UpdateInvaders(data);
    
    // Update bullets
    SpaceInvadersGame_UpdateBullets(data);
    
    // Check collisions
    SpaceInvadersGame_CheckCollisions(data);
    
    // Check if all invaders are destroyed
    if (data->invadersRemaining == 0) {
        data->gameWon = true;
    }
}

void SpaceInvadersGame_Render(GameState* state) {
    SpaceInvadersGameData* data = (SpaceInvadersGameData*)state->data;
    SDL_Renderer* renderer = SDL_Framework_GetRenderer();
    
    // Clear screen
    SDL_Framework_ClearScreen(0, 0, 0, 255);
    
    // Draw player
    if (data->player.active) {
        SDL_Rect playerRect = {
            (int)data->player.x,
            (int)data->player.y,
            (int)data->player.width,
            (int)data->player.height
        };
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        SDL_RenderFillRect(renderer, &playerRect);
    }
    
    // Draw invaders
    for (int i = 0; i < MAX_INVADERS; i++) {
        if (data->invaders[i].active) {
            SDL_Rect invaderRect = {
                (int)data->invaders[i].x,
                (int)data->invaders[i].y,
                (int)data->invaders[i].width,
                (int)data->invaders[i].height
            };
            
            // Different colors for different invader types
            switch (data->invaders[i].invader.invaderType) {
                case INVADER_TYPE_1:
                    SDL_SetRenderDrawColor(renderer, 255, 50, 50, 255);
                    break;
                case INVADER_TYPE_2:
                    SDL_SetRenderDrawColor(renderer, 50, 255, 50, 255);
                    break;
                case INVADER_TYPE_3:
                    SDL_SetRenderDrawColor(renderer, 50, 50, 255, 255);
                    break;
            }
            
            SDL_RenderFillRect(renderer, &invaderRect);
        }
    }
    
    // Draw bullets
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (data->bullets[i].active) {
            SDL_Rect bulletRect = {
                (int)data->bullets[i].x,
                (int)data->bullets[i].y,
                (int)data->bullets[i].width,
                (int)data->bullets[i].height
            };
            
            // Different colors for different bullet types
            if (data->bullets[i].bullet.bulletType == BULLET_PLAYER) {
                SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
            } else {
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            }
            
            SDL_RenderFillRect(renderer, &bulletRect);
        }
    }
    
    // Draw barriers
    for (int i = 0; i < MAX_BARRIERS; i++) {
        if (data->barriers[i].active) {
            // Draw each segment of the barrier
            for (int row = 0; row < 4; row++) {
                for (int col = 0; col < 8; col++) {
                    if (data->barriers[i].barrier.segments[row][col] > 0) {
                        SDL_Rect segmentRect = {
                            (int)(data->barriers[i].x + col * BARRIER_SEGMENT_SIZE),
                            (int)(data->barriers[i].y + row * BARRIER_SEGMENT_SIZE),
                            BARRIER_SEGMENT_SIZE,
                            BARRIER_SEGMENT_SIZE
                        };
                        
                        // Color based on health
                        int health = data->barriers[i].barrier.segments[row][col];
                        int green = (health * 255) / 3;  // Max health is 3
                        SDL_SetRenderDrawColor(renderer, 0, green, 255, 255);
                        SDL_RenderFillRect(renderer, &segmentRect);
                    }
                }
            }
        }
    }
    
    // Draw score
    char scoreText[32];
    sprintf(scoreText, "Score: %d", data->score);
    SDL_Color textColor = {255, 255, 255, 255};
    TextureManager_DrawText("menu_font", scoreText, 10, 10, textColor);
    
    // Draw level
    char levelText[32];
    sprintf(levelText, "Level: %d", data->level);
    TextureManager_DrawText("menu_font", levelText, 10, 40, textColor);
    
    // Draw lives
    char livesText[32];
    sprintf(livesText, "Lives: %d", data->player.player.lives);
    TextureManager_DrawText("menu_font", livesText, 10, 70, textColor);
    
    // Draw game over message
    if (data->gameOver) {
        TextureManager_DrawText("menu_font", "Game Over! Press R to restart", 
                               WINDOW_WIDTH / 2 - 150, WINDOW_HEIGHT / 2 - 12, textColor);
    }
    
    // Draw game won message
    if (data->gameWon) {
        TextureManager_DrawText("menu_font", "You Won! Press R to play next level", 
                               WINDOW_WIDTH / 2 - 170, WINDOW_HEIGHT / 2 - 12, textColor);
    }
}

void SpaceInvadersGame_Reset(SpaceInvadersGameData* data) {
    // Reset game state
    data->gameOver = false;
    data->gameWon = false;
    data->score = 0;
    data->level = 1;
    
    // Initialize player
    SpaceInvadersGame_InitPlayer(data);
    
    // Initialize invaders
    SpaceInvadersGame_InitInvaders(data);
    
    // Initialize barriers
    SpaceInvadersGame_InitBarriers(data);
    
    // Clear bullets
    for (int i = 0; i < MAX_BULLETS; i++) {
        data->bullets[i].active = false;
    }
    
    // Initialize invader movement
    data->invaderMoveTimer = 0.0f;
    data->invaderMoveDelay = INVADER_INITIAL_MOVE_DELAY;
    data->invaderMoveSpeed = 10.0f;
    data->invaderMoveDirection = 1;
    data->invaderDropFlag = false;
    
    // Initialize invader shooting
    data->invaderShootTimer = 0.0f;
    data->invaderShootDelay = INVADER_SHOOT_DELAY;
}

void SpaceInvadersGame_InitPlayer(SpaceInvadersGameData* data) {
    data->player.type = ENTITY_PLAYER;
    data->player.x = (data->screenWidth - PLAYER_WIDTH) / 2;
    data->player.y = data->screenHeight - PLAYER_HEIGHT - 20;
    data->player.width = PLAYER_WIDTH;
    data->player.height = PLAYER_HEIGHT;
    data->player.velocityX = 0;
    data->player.velocityY = 0;
    data->player.active = true;
    data->player.player.lives = 3;
    data->player.player.score = 0;
    data->player.player.shootCooldown = PLAYER_SHOOT_COOLDOWN;
    data->player.player.lastShotTime = 0;
}

void SpaceInvadersGame_InitInvaders(SpaceInvadersGameData* data) {
    data->invadersRemaining = 0;
    
    for (int row = 0; row < INVADER_ROWS; row++) {
        for (int col = 0; col < INVADER_COLS; col++) {
            int index = row * INVADER_COLS + col;
            
            data->invaders[index].type = ENTITY_INVADER;
            data->invaders[index].x = col * (INVADER_WIDTH + INVADER_HORIZONTAL_SPACING) + 50;
            data->invaders[index].y = row * (INVADER_HEIGHT + INVADER_VERTICAL_SPACING) + INVADER_INITIAL_Y;
            data->invaders[index].width = INVADER_WIDTH;
            data->invaders[index].height = INVADER_HEIGHT;
            data->invaders[index].velocityX = 0;
            data->invaders[index].velocityY = 0;
            data->invaders[index].active = true;
            
            // Set invader type based on row
            if (row == 0) {
                data->invaders[index].invader.invaderType = INVADER_TYPE_1;
            } else if (row < 3) {
                data->invaders[index].invader.invaderType = INVADER_TYPE_2;
            } else {
                data->invaders[index].invader.invaderType = INVADER_TYPE_3;
            }
            
            data->invaders[index].invader.row = row;
            data->invaders[index].invader.col = col;
            data->invaders[index].invader.animationFrame = 0;
            
            data->invadersRemaining++;
        }
    }
}

void SpaceInvadersGame_InitBarriers(SpaceInvadersGameData* data) {
    for (int i = 0; i < MAX_BARRIERS; i++) {
        data->barriers[i].type = ENTITY_BARRIER;
        data->barriers[i].width = BARRIER_WIDTH;
        data->barriers[i].height = BARRIER_HEIGHT;
        data->barriers[i].x = i * (BARRIER_WIDTH + 60) + 120;
        data->barriers[i].y = BARRIER_Y;
        data->barriers[i].velocityX = 0;
        data->barriers[i].velocityY = 0;
        data->barriers[i].active = true;
        data->barriers[i].barrier.health = 3;
        
        // Initialize barrier segments
        for (int row = 0; row < 4; row++) {
            for (int col = 0; col < 8; col++) {
                // Create a barrier shape with a gap in the middle bottom
                if (row == 3 && (col == 3 || col == 4)) {
                    data->barriers[i].barrier.segments[row][col] = 0;
                } else {
                    data->barriers[i].barrier.segments[row][col] = 3;  // Full health
                }
            }
        }
    }
}

void SpaceInvadersGame_UpdatePlayer(SpaceInvadersGameData* data) {
    // Update player position
    data->player.x += data->player.velocityX * data->deltaTime;
    
    // Keep player within screen bounds
    if (data->player.x < 0) {
        data->player.x = 0;
    } else if (data->player.x > data->screenWidth - data->player.width) {
        data->player.x = data->screenWidth - data->player.width;
    }
    
    // Update shoot cooldown
    data->player.player.lastShotTime += data->deltaTime;
}

void SpaceInvadersGame_UpdateInvaders(SpaceInvadersGameData* data) {
    // Update invader movement timer
    data->invaderMoveTimer += data->deltaTime;
    
    // Move invaders when timer expires
    if (data->invaderMoveTimer >= data->invaderMoveDelay) {
        data->invaderMoveTimer = 0;
        
        // Check if invaders need to change direction and move down
        bool changeDirection = false;
        
        for (int i = 0; i < MAX_INVADERS; i++) {
            if (data->invaders[i].active) {
                // Check if invader is at screen edge
                if ((data->invaderMoveDirection > 0 && data->invaders[i].x + data->invaders[i].width + data->invaderMoveSpeed > data->screenWidth) ||
                    (data->invaderMoveDirection < 0 && data->invaders[i].x - data->invaderMoveSpeed < 0)) {
                    changeDirection = true;
                    break;
                }
            }
        }
        
        // Move invaders
        for (int i = 0; i < MAX_INVADERS; i++) {
            if (data->invaders[i].active) {
                if (data->invaderDropFlag) {
                    // Move down
                    data->invaders[i].y += INVADER_DROP_AMOUNT;
                    
                    // Check if invader reached the player or barriers
                    if (data->invaders[i].y + data->invaders[i].height >= BARRIER_Y) {
                        data->gameOver = true;
                    }
                } else {
                    // Move horizontally
                    data->invaders[i].x += data->invaderMoveSpeed * data->invaderMoveDirection;
                }
                
                // Update animation frame
                data->invaders[i].invader.animationFrame += 0.5f;
                if (data->invaders[i].invader.animationFrame >= 2.0f) {
                    data->invaders[i].invader.animationFrame = 0.0f;
                }
            }
        }
        
        // Handle direction change
        if (changeDirection) {
            data->invaderMoveDirection *= -1;
            data->invaderDropFlag = true;
        } else {
            data->invaderDropFlag = false;
        }
    }
    
    // Update invader shooting timer
    data->invaderShootTimer += data->deltaTime;
    
    // Invader shooting
    if (data->invaderShootTimer >= data->invaderShootDelay) {
        data->invaderShootTimer = 0;
        
        // Find bottom-most invader in a random column
        int col = rand() % INVADER_COLS;
        int bottomInvaderIndex = -1;
        
        for (int row = INVADER_ROWS - 1; row >= 0; row--) {
            int index = row * INVADER_COLS + col;
            if (index < MAX_INVADERS && data->invaders[index].active) {
                bottomInvaderIndex = index;
                break;
            }
        }
        
        // Fire bullet from bottom-most invader
        if (bottomInvaderIndex != -1) {
            SpaceInvadersGame_FireInvaderBullet(data, bottomInvaderIndex);
        }
    }
}

void SpaceInvadersGame_UpdateBullets(SpaceInvadersGameData* data) {
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (data->bullets[i].active) {
            // Update bullet position
            data->bullets[i].y += data->bullets[i].velocityY * data->deltaTime;
            
            // Check if bullet is out of screen
            if (data->bullets[i].y < 0 || data->bullets[i].y > data->screenHeight) {
                data->bullets[i].active = false;
            }
        }
    }
}

void SpaceInvadersGame_CheckCollisions(SpaceInvadersGameData* data) {
    // Check bullet collisions
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (data->bullets[i].active) {
            SDL_Rect bulletRect = {
                (int)data->bullets[i].x,
                (int)data->bullets[i].y,
                (int)data->bullets[i].width,
                (int)data->bullets[i].height
            };
            
            // Player bullet
            if (data->bullets[i].bullet.bulletType == BULLET_PLAYER) {
                // Check collision with invaders
                for (int j = 0; j < MAX_INVADERS; j++) {
                    if (data->invaders[j].active) {
                        SDL_Rect invaderRect = {
                            (int)data->invaders[j].x,
                            (int)data->invaders[j].y,
                            (int)data->invaders[j].width,
                            (int)data->invaders[j].height
                        };
                        
                        if (SDL_HasIntersection(&bulletRect, &invaderRect)) {
                            // Destroy invader
                            data->invaders[j].active = false;
                            data->bullets[i].active = false;
                            data->invadersRemaining--;
                            
                            // Add score based on invader type
                            switch (data->invaders[j].invader.invaderType) {
                                case INVADER_TYPE_1:
                                    data->score += 30;
                                    break;
                                case INVADER_TYPE_2:
                                    data->score += 20;
                                    break;
                                case INVADER_TYPE_3:
                                    data->score += 10;
                                    break;
                            }
                            
                            // Increase invader speed as fewer remain
                            data->invaderMoveDelay = INVADER_INITIAL_MOVE_DELAY * 
                                                    ((float)data->invadersRemaining / (INVADER_ROWS * INVADER_COLS));
                            
                            // Ensure minimum delay
                            if (data->invaderMoveDelay < INVADER_MIN_MOVE_DELAY) {
                                data->invaderMoveDelay = INVADER_MIN_MOVE_DELAY;
                            }
                            
                            break;
                        }
                    }
                }
            }
            // Invader bullet
            else if (data->bullets[i].bullet.bulletType == BULLET_INVADER) {
                // Check collision with player
                if (data->player.active) {
                    SDL_Rect playerRect = {
                        (int)data->player.x,
                        (int)data->player.y,
                        (int)data->player.width,
                        (int)data->player.height
                    };
                    
                    if (SDL_HasIntersection(&bulletRect, &playerRect)) {
                        // Damage player
                        data->player.player.lives--;
                        data->bullets[i].active = false;
                        
                        // Check if game over
                        if (data->player.player.lives <= 0) {
                            data->gameOver = true;
                        }
                    }
                }
            }
            
            // Check collision with barriers
            for (int j = 0; j < MAX_BARRIERS; j++) {
                if (data->barriers[j].active) {
                    SDL_Rect barrierRect = {
                        (int)data->barriers[j].x,
                        (int)data->barriers[j].y,
                        (int)data->barriers[j].width,
                        (int)data->barriers[j].height
                    };
                    
                    if (SDL_HasIntersection(&bulletRect, &barrierRect)) {
                        // Calculate which segment was hit
                        float relX = data->bullets[i].x - data->barriers[j].x;
                        float relY = data->bullets[i].y - data->barriers[j].y;
                        
                        // Damage barrier segment
                        SpaceInvadersGame_DamageBarrier(data, j, relX, relY);
                        
                        // Destroy bullet
                        data->bullets[i].active = false;
                        break;
                    }
                }
            }
        }
    }
}

void SpaceInvadersGame_FirePlayerBullet(SpaceInvadersGameData* data) {
    // Check cooldown
    if (data->player.player.lastShotTime < data->player.player.shootCooldown) {
        return;
    }
    
    // Reset cooldown
    data->player.player.lastShotTime = 0;
    
    // Get free bullet
    Entity* bullet = SpaceInvadersGame_GetFreeBullet(data);
    if (!bullet) {
        return;
    }
    
    // Initialize bullet
    bullet->type = ENTITY_BULLET;
    bullet->x = data->player.x + (data->player.width - BULLET_WIDTH) / 2;
    bullet->y = data->player.y - BULLET_HEIGHT;
    bullet->width = BULLET_WIDTH;
    bullet->height = BULLET_HEIGHT;
    bullet->velocityX = 0;
    bullet->velocityY = -PLAYER_BULLET_SPEED;
    bullet->active = true;
    bullet->bullet.bulletType = BULLET_PLAYER;
}

void SpaceInvadersGame_FireInvaderBullet(SpaceInvadersGameData* data, int invaderIndex) {
    // Get free bullet
    Entity* bullet = SpaceInvadersGame_GetFreeBullet(data);
    if (!bullet) {
        return;
    }
    
    // Initialize bullet
    bullet->type = ENTITY_BULLET;
    bullet->x = data->invaders[invaderIndex].x + (data->invaders[invaderIndex].width - BULLET_WIDTH) / 2;
    bullet->y = data->invaders[invaderIndex].y + data->invaders[invaderIndex].height;
    bullet->width = BULLET_WIDTH;
    bullet->height = BULLET_HEIGHT;
    bullet->velocityX = 0;
    bullet->velocityY = INVADER_BULLET_SPEED;
    bullet->active = true;
    bullet->bullet.bulletType = BULLET_INVADER;
}

Entity* SpaceInvadersGame_GetFreeBullet(SpaceInvadersGameData* data) {
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (!data->bullets[i].active) {
            return &data->bullets[i];
        }
    }
    return NULL;
}

void SpaceInvadersGame_DamageBarrier(SpaceInvadersGameData* data, int barrierIndex, float relX, float relY) {
    // Calculate segment indices
    int col = (int)(relX / BARRIER_SEGMENT_SIZE);
    int row = (int)(relY / BARRIER_SEGMENT_SIZE);
    
    // Ensure indices are in bounds
    if (row >= 0 && row < 4 && col >= 0 && col < 8) {
        // Damage segment
        if (data->barriers[barrierIndex].barrier.segments[row][col] > 0) {
            data->barriers[barrierIndex].barrier.segments[row][col]--;
        }
    }
}

void SpaceInvadersGame_NextLevel(SpaceInvadersGameData* data) {
    // Increase level
    data->level++;
    
    // Reset game state but keep score and lives
    int score = data->score;
    int lives = data->player.player.lives;
    
    // Reset game
    SpaceInvadersGame_Reset(data);
    
    // Restore score and lives
    data->score = score;
    data->player.player.lives = lives;
    
    // Make game harder
    data->invaderMoveDelay = INVADER_INITIAL_MOVE_DELAY - (data->level - 1) * 0.1f;
    if (data->invaderMoveDelay < INVADER_MIN_MOVE_DELAY) {
        data->invaderMoveDelay = INVADER_MIN_MOVE_DELAY;
    }
    
    data->invaderShootDelay = INVADER_SHOOT_DELAY - (data->level - 1) * 0.1f;
    if (data->invaderShootDelay < 0.2f) {
        data->invaderShootDelay = 0.2f;
    }
}

int main(int argc, char* argv[]) {
    // Initialize SDL framework
    if (!SDL_Framework_Init("Space Invaders", WINDOW_WIDTH, WINDOW_HEIGHT)) {
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
    
    // Create space invaders game state
    GameState* gameState = SpaceInvadersGame_Create();
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

