#include "input_handler.h"
#include <string.h>

#define MAX_KEYS 512
#define MAX_MOUSE_BUTTONS 5

// Static variables
static bool s_currentKeyStates[MAX_KEYS];
static bool s_previousKeyStates[MAX_KEYS];
static bool s_currentMouseButtonStates[MAX_MOUSE_BUTTONS];
static bool s_previousMouseButtonStates[MAX_MOUSE_BUTTONS];
static int s_mouseX;
static int s_mouseY;

void Input_Init(void) {
    // Initialize key states
    memset(s_currentKeyStates, 0, sizeof(s_currentKeyStates));
    memset(s_previousKeyStates, 0, sizeof(s_previousKeyStates));
    
    // Initialize mouse button states
    memset(s_currentMouseButtonStates, 0, sizeof(s_currentMouseButtonStates));
    memset(s_previousMouseButtonStates, 0, sizeof(s_previousMouseButtonStates));
    
    // Initialize mouse position
    s_mouseX = 0;
    s_mouseY = 0;
}

bool Input_ProcessEvents(void) {
    SDL_Event event;
    
    // Update mouse position
    SDL_GetMouseState(&s_mouseX, &s_mouseY);
    
    // Process events
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                return false;
                
            case SDL_KEYDOWN:
                if (event.key.keysym.sym < MAX_KEYS) {
                    s_currentKeyStates[event.key.keysym.sym] = true;
                }
                break;
                
            case SDL_KEYUP:
                if (event.key.keysym.sym < MAX_KEYS) {
                    s_currentKeyStates[event.key.keysym.sym] = false;
                }
                break;
                
            case SDL_MOUSEBUTTONDOWN:
                if (event.button.button <= MAX_MOUSE_BUTTONS) {
                    s_currentMouseButtonStates[event.button.button - 1] = true;
                }
                break;
                
            case SDL_MOUSEBUTTONUP:
                if (event.button.button <= MAX_MOUSE_BUTTONS) {
                    s_currentMouseButtonStates[event.button.button - 1] = false;
                }
                break;
        }
    }
    
    return true;
}

bool Input_IsKeyPressed(SDL_Keycode key) {
    if (key < MAX_KEYS) {
        return s_currentKeyStates[key];
    }
    return false;
}

bool Input_IsKeyJustPressed(SDL_Keycode key) {
    if (key < MAX_KEYS) {
        return s_currentKeyStates[key] && !s_previousKeyStates[key];
    }
    return false;
}

bool Input_IsKeyJustReleased(SDL_Keycode key) {
    if (key < MAX_KEYS) {
        return !s_currentKeyStates[key] && s_previousKeyStates[key];
    }
    return false;
}

void Input_GetMousePosition(int* x, int* y) {
    if (x) *x = s_mouseX;
    if (y) *y = s_mouseY;
}

bool Input_IsMouseButtonPressed(Uint8 button) {
    if (button <= MAX_MOUSE_BUTTONS) {
        return s_currentMouseButtonStates[button - 1];
    }
    return false;
}

bool Input_IsMouseButtonJustClicked(Uint8 button) {
    if (button <= MAX_MOUSE_BUTTONS) {
        return s_currentMouseButtonStates[button - 1] && !s_previousMouseButtonStates[button - 1];
    }
    return false;
}

void Input_Update(void) {
    // Copy current states to previous states
    memcpy(s_previousKeyStates, s_currentKeyStates, sizeof(s_currentKeyStates));
    memcpy(s_previousMouseButtonStates, s_currentMouseButtonStates, sizeof(s_currentMouseButtonStates));
}
