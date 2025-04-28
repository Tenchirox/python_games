#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

#include <SDL2/SDL.h>
#include <stdbool.h>

/**
 * @brief Initialize the input handler
 */
void Input_Init(void);

/**
 * @brief Process SDL events
 * 
 * @return true if the game should continue, false if quit was requested
 */
bool Input_ProcessEvents(void);

/**
 * @brief Check if a key is currently pressed
 * 
 * @param key SDL keycode to check
 * @return true if the key is pressed, false otherwise
 */
bool Input_IsKeyPressed(SDL_Keycode key);

/**
 * @brief Check if a key was just pressed this frame
 * 
 * @param key SDL keycode to check
 * @return true if the key was just pressed, false otherwise
 */
bool Input_IsKeyJustPressed(SDL_Keycode key);

/**
 * @brief Check if a key was just released this frame
 * 
 * @param key SDL keycode to check
 * @return true if the key was just released, false otherwise
 */
bool Input_IsKeyJustReleased(SDL_Keycode key);

/**
 * @brief Get the mouse position
 * 
 * @param x Pointer to store the x coordinate
 * @param y Pointer to store the y coordinate
 */
void Input_GetMousePosition(int* x, int* y);

/**
 * @brief Check if a mouse button is currently pressed
 * 
 * @param button Mouse button to check (SDL_BUTTON_LEFT, SDL_BUTTON_RIGHT, etc.)
 * @return true if the button is pressed, false otherwise
 */
bool Input_IsMouseButtonPressed(Uint8 button);

/**
 * @brief Check if a mouse button was just clicked this frame
 * 
 * @param button Mouse button to check (SDL_BUTTON_LEFT, SDL_BUTTON_RIGHT, etc.)
 * @return true if the button was just clicked, false otherwise
 */
bool Input_IsMouseButtonJustClicked(Uint8 button);

/**
 * @brief Update the input state at the end of the frame
 */
void Input_Update(void);

#endif /* INPUT_HANDLER_H */
