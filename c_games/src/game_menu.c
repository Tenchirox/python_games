#include "sdl_framework.h"
#include "input_handler.h"
#include "texture_manager.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define BUTTON_WIDTH 200
#define BUTTON_HEIGHT 50
#define BUTTON_PADDING 20
#define NUM_GAMES 5  // Updated to include Connect Four

typedef struct {
    char* title;
    char* executable;
    SDL_Rect rect;
    bool hover;
} GameButton;

// Function prototypes
void initButtons(GameButton* buttons, int numButtons);
void renderButtons(GameButton* buttons, int numButtons);
void launchGame(const char* executable);
void cleanup(GameButton* buttons, int numButtons);

int main(int argc, char* argv[]) {
    // Initialize SDL framework
    if (!SDL_Framework_Init("Game Menu", WINDOW_WIDTH, WINDOW_HEIGHT)) {
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
    
    // Create game buttons
    GameButton buttons[NUM_GAMES];
    initButtons(buttons, NUM_GAMES);
    
    // Main loop
    bool running = true;
    while (running) {
        // Start frame timer
        SDL_Framework_StartFrameTimer();
        
        // Process input
        running = Input_ProcessEvents();
        
        // Get mouse position
        int mouseX, mouseY;
        Input_GetMousePosition(&mouseX, &mouseY);
        
        // Update button hover states
        for (int i = 0; i < NUM_GAMES; i++) {
            buttons[i].hover = Utils_PointInRect(mouseX, mouseY, buttons[i].rect);
            
            // Check for button click
            if (buttons[i].hover && Input_IsMouseButtonJustClicked(SDL_BUTTON_LEFT)) {
                launchGame(buttons[i].executable);
            }
        }
        
        // Clear screen
        SDL_Framework_ClearScreen(45, 45, 45, 255);
        
        // Render title
        SDL_Color titleColor = {255, 255, 255, 255};
        TextureManager_DrawText("menu_font", "Select a Game", WINDOW_WIDTH / 2 - 100, 50, titleColor);
        
        // Render buttons
        renderButtons(buttons, NUM_GAMES);
        
        // Update screen
        SDL_Framework_UpdateScreen();
        
        // Update input state
        Input_Update();
        
        // Limit FPS
        SDL_Framework_LimitFPS(60);
    }
    
    // Cleanup
    cleanup(buttons, NUM_GAMES);
    TextureManager_Cleanup();
    SDL_Framework_Cleanup();
    
    return 0;
}

void initButtons(GameButton* buttons, int numButtons) {
    // Define game titles and executables
    const char* titles[NUM_GAMES] = {"Snake", "Tetris", "Pacman", "Space Invaders", "Connect Four"};
    const char* executables[NUM_GAMES] = {"snake", "tetris", "pacman", "space_invaders", "connect_four"};
    
    // Calculate button positions
    int startY = WINDOW_HEIGHT / 2 - ((BUTTON_HEIGHT + BUTTON_PADDING) * numButtons) / 2;
    
    for (int i = 0; i < numButtons; i++) {
        buttons[i].title = strdup(titles[i]);
        buttons[i].executable = strdup(executables[i]);
        buttons[i].rect.x = WINDOW_WIDTH / 2 - BUTTON_WIDTH / 2;
        buttons[i].rect.y = startY + i * (BUTTON_HEIGHT + BUTTON_PADDING);
        buttons[i].rect.w = BUTTON_WIDTH;
        buttons[i].rect.h = BUTTON_HEIGHT;
        buttons[i].hover = false;
    }
}

void renderButtons(GameButton* buttons, int numButtons) {
    SDL_Renderer* renderer = SDL_Framework_GetRenderer();
    
    for (int i = 0; i < numButtons; i++) {
        // Draw button background
        if (buttons[i].hover) {
            SDL_SetRenderDrawColor(renderer, 100, 100, 200, 255);
        } else {
            SDL_SetRenderDrawColor(renderer, 70, 70, 150, 255);
        }
        SDL_RenderFillRect(renderer, &buttons[i].rect);
        
        // Draw button border
        SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
        SDL_RenderDrawRect(renderer, &buttons[i].rect);
        
        // Draw button text
        SDL_Color textColor = {255, 255, 255, 255};
        TextureManager_DrawText("menu_font", buttons[i].title, 
                               buttons[i].rect.x + buttons[i].rect.w / 2 - 40, 
                               buttons[i].rect.y + buttons[i].rect.h / 2 - 12, 
                               textColor);
    }
}

void launchGame(const char* executable) {
    char command[256];
    
#ifdef _WIN32
    sprintf(command, "start %s.exe", executable);
#else
    sprintf(command, "./%s &", executable);
#endif
    
    system(command);
}

void cleanup(GameButton* buttons, int numButtons) {
    for (int i = 0; i < numButtons; i++) {
        free(buttons[i].title);
        free(buttons[i].executable);
    }
}
