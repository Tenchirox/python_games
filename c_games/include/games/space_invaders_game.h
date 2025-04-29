#ifndef SPACE_INVADERS_GAME_H
#define SPACE_INVADERS_GAME_H

#include "game_state.h"
#include <SDL2/SDL.h>
#include <stdbool.h>

#define MAX_INVADERS 55  // 5 rows of 11 invaders
#define MAX_BULLETS 30   // Maximum number of bullets on screen
#define MAX_BARRIERS 4   // Number of defensive barriers

// Entity types
typedef enum {
    ENTITY_PLAYER,
    ENTITY_INVADER,
    ENTITY_BULLET,
    ENTITY_BARRIER
} EntityType;

// Bullet types
typedef enum {
    BULLET_PLAYER,
    BULLET_INVADER
} BulletType;

// Invader types
typedef enum {
    INVADER_TYPE_1,  // Top row
    INVADER_TYPE_2,  // Middle rows
    INVADER_TYPE_3   // Bottom row
} InvaderType;

// Game entity structure
typedef struct {
    EntityType type;
    float x, y;
    float width, height;
    float velocityX, velocityY;
    bool active;
    
    // Type-specific data
    union {
        struct {
            int lives;
            int score;
            float shootCooldown;
            float lastShotTime;
        } player;
        
        struct {
            InvaderType invaderType;
            int row, col;
            float animationFrame;
        } invader;
        
        struct {
            BulletType bulletType;
        } bullet;
        
        struct {
            int health;
            int segments[4][8];  // 4x8 grid of segments for each barrier
        } barrier;
    };
} Entity;

// Space Invaders game state
typedef struct {
    // Game entities
    Entity player;
    Entity invaders[MAX_INVADERS];
    Entity bullets[MAX_BULLETS];
    Entity barriers[MAX_BARRIERS];
    
    // Game state
    bool gameOver;
    bool gameWon;
    int score;
    int level;
    int invadersRemaining;
    
    // Invader movement
    float invaderMoveTimer;
    float invaderMoveDelay;
    float invaderMoveSpeed;
    int invaderMoveDirection;  // 1 = right, -1 = left
    bool invaderDropFlag;
    
    // Invader shooting
    float invaderShootTimer;
    float invaderShootDelay;
    
    // Timing
    Uint32 lastUpdateTime;
    float deltaTime;
    
    // Screen dimensions
    int screenWidth;
    int screenHeight;
} SpaceInvadersGameData;

// Game state functions
void SpaceInvadersGame_Init(GameState* state);
void SpaceInvadersGame_Cleanup(GameState* state);
void SpaceInvadersGame_Pause(GameState* state);
void SpaceInvadersGame_Resume(GameState* state);
void SpaceInvadersGame_HandleEvents(GameState* state);
void SpaceInvadersGame_Update(GameState* state);
void SpaceInvadersGame_Render(GameState* state);

// Helper functions
void SpaceInvadersGame_Reset(SpaceInvadersGameData* data);
void SpaceInvadersGame_InitPlayer(SpaceInvadersGameData* data);
void SpaceInvadersGame_InitInvaders(SpaceInvadersGameData* data);
void SpaceInvadersGame_InitBarriers(SpaceInvadersGameData* data);
void SpaceInvadersGame_UpdatePlayer(SpaceInvadersGameData* data);
void SpaceInvadersGame_UpdateInvaders(SpaceInvadersGameData* data);
void SpaceInvadersGame_UpdateBullets(SpaceInvadersGameData* data);
void SpaceInvadersGame_CheckCollisions(SpaceInvadersGameData* data);
void SpaceInvadersGame_FirePlayerBullet(SpaceInvadersGameData* data);
void SpaceInvadersGame_FireInvaderBullet(SpaceInvadersGameData* data, int invaderIndex);
Entity* SpaceInvadersGame_GetFreeBullet(SpaceInvadersGameData* data);
void SpaceInvadersGame_DamageBarrier(SpaceInvadersGameData* data, int barrierIndex, float x, float y);
void SpaceInvadersGame_NextLevel(SpaceInvadersGameData* data);

// Create a new space invaders game state
GameState* SpaceInvadersGame_Create(void);

#endif /* SPACE_INVADERS_GAME_H */

