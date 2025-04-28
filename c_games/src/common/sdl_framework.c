#include "sdl_framework.h"
#include <stdio.h>
#include <stdlib.h>

// Static variables
static SDL_Window* s_window = NULL;
static SDL_Renderer* s_renderer = NULL;
static Uint32 s_lastFrameTime = 0;
static Uint32 s_currentFrameTime = 0;
static Uint32 s_deltaTime = 0;

bool SDL_Framework_Init(const char* window_title, int width, int height) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) < 0) {
        printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
        return false;
    }

    // Create window
    s_window = SDL_CreateWindow(window_title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
                               width, height, SDL_WINDOW_SHOWN);
    if (s_window == NULL) {
        printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
        return false;
    }

    // Create renderer
    s_renderer = SDL_CreateRenderer(s_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (s_renderer == NULL) {
        printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
        return false;
    }

    // Initialize SDL_image
    int imgFlags = IMG_INIT_PNG | IMG_INIT_JPG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
        return false;
    }

    // Initialize SDL_ttf
    if (TTF_Init() == -1) {
        printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
        return false;
    }

    // Initialize SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
        return false;
    }

    // Initialize frame timer
    s_lastFrameTime = SDL_GetTicks();
    s_currentFrameTime = s_lastFrameTime;
    s_deltaTime = 0;

    return true;
}

void SDL_Framework_Cleanup(void) {
    // Destroy renderer and window
    if (s_renderer != NULL) {
        SDL_DestroyRenderer(s_renderer);
        s_renderer = NULL;
    }
    
    if (s_window != NULL) {
        SDL_DestroyWindow(s_window);
        s_window = NULL;
    }

    // Quit SDL subsystems
    Mix_Quit();
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

SDL_Window* SDL_Framework_GetWindow(void) {
    return s_window;
}

SDL_Renderer* SDL_Framework_GetRenderer(void) {
    return s_renderer;
}

void SDL_Framework_ClearScreen(Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    SDL_SetRenderDrawColor(s_renderer, r, g, b, a);
    SDL_RenderClear(s_renderer);
}

void SDL_Framework_UpdateScreen(void) {
    SDL_RenderPresent(s_renderer);
}

Uint32 SDL_Framework_GetDeltaTime(void) {
    return s_deltaTime;
}

void SDL_Framework_StartFrameTimer(void) {
    s_lastFrameTime = s_currentFrameTime;
    s_currentFrameTime = SDL_GetTicks();
    s_deltaTime = s_currentFrameTime - s_lastFrameTime;
}

void SDL_Framework_LimitFPS(int fps) {
    Uint32 frameTime = SDL_GetTicks() - s_currentFrameTime;
    Uint32 targetFrameTime = 1000 / fps;
    
    if (frameTime < targetFrameTime) {
        SDL_Delay(targetFrameTime - frameTime);
    }
}
