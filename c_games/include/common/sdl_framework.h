#ifndef SDL_FRAMEWORK_H
#define SDL_FRAMEWORK_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <stdbool.h>

/**
 * @brief Initialize SDL and its subsystems
 * 
 * @param window_title Title of the window
 * @param width Window width
 * @param height Window height
 * @return true if initialization was successful, false otherwise
 */
bool SDL_Framework_Init(const char* window_title, int width, int height);

/**
 * @brief Clean up SDL resources
 */
void SDL_Framework_Cleanup(void);

/**
 * @brief Get the SDL window
 * 
 * @return SDL_Window* Pointer to the SDL window
 */
SDL_Window* SDL_Framework_GetWindow(void);

/**
 * @brief Get the SDL renderer
 * 
 * @return SDL_Renderer* Pointer to the SDL renderer
 */
SDL_Renderer* SDL_Framework_GetRenderer(void);

/**
 * @brief Clear the screen with a specific color
 * 
 * @param r Red component (0-255)
 * @param g Green component (0-255)
 * @param b Blue component (0-255)
 * @param a Alpha component (0-255)
 */
void SDL_Framework_ClearScreen(Uint8 r, Uint8 g, Uint8 b, Uint8 a);

/**
 * @brief Update the screen
 */
void SDL_Framework_UpdateScreen(void);

/**
 * @brief Get the time elapsed since the last frame in milliseconds
 * 
 * @return Uint32 Time elapsed in milliseconds
 */
Uint32 SDL_Framework_GetDeltaTime(void);

/**
 * @brief Start the frame timer
 */
void SDL_Framework_StartFrameTimer(void);

/**
 * @brief Limit the frame rate to a specific FPS
 * 
 * @param fps Target frames per second
 */
void SDL_Framework_LimitFPS(int fps);

#endif /* SDL_FRAMEWORK_H */
