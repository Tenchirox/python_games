#ifndef TEXTURE_MANAGER_H
#define TEXTURE_MANAGER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>

/**
 * @brief Initialize the texture manager
 * 
 * @param renderer SDL renderer to use
 * @return true if initialization was successful, false otherwise
 */
bool TextureManager_Init(SDL_Renderer* renderer);

/**
 * @brief Clean up texture manager resources
 */
void TextureManager_Cleanup(void);

/**
 * @brief Load a texture from a file
 * 
 * @param filename Path to the image file
 * @param id Identifier to associate with the texture
 * @return true if the texture was loaded successfully, false otherwise
 */
bool TextureManager_LoadTexture(const char* filename, const char* id);

/**
 * @brief Draw a texture
 * 
 * @param id Identifier of the texture to draw
 * @param x X coordinate to draw at
 * @param y Y coordinate to draw at
 * @param width Width to draw the texture
 * @param height Height to draw the texture
 * @param flip SDL_RendererFlip value for flipping the texture
 */
void TextureManager_DrawTexture(const char* id, int x, int y, int width, int height, SDL_RendererFlip flip);

/**
 * @brief Draw a portion of a texture (sprite)
 * 
 * @param id Identifier of the texture to draw
 * @param x X coordinate to draw at
 * @param y Y coordinate to draw at
 * @param width Width to draw the texture
 * @param height Height to draw the texture
 * @param srcX X coordinate in the source texture
 * @param srcY Y coordinate in the source texture
 * @param srcWidth Width in the source texture
 * @param srcHeight Height in the source texture
 * @param flip SDL_RendererFlip value for flipping the texture
 */
void TextureManager_DrawFrame(const char* id, int x, int y, int width, int height, 
                             int srcX, int srcY, int srcWidth, int srcHeight, 
                             SDL_RendererFlip flip);

/**
 * @brief Load a font
 * 
 * @param filename Path to the font file
 * @param id Identifier to associate with the font
 * @param size Font size
 * @return true if the font was loaded successfully, false otherwise
 */
bool TextureManager_LoadFont(const char* filename, const char* id, int size);

/**
 * @brief Draw text using a loaded font
 * 
 * @param fontId Identifier of the font to use
 * @param text Text to draw
 * @param x X coordinate to draw at
 * @param y Y coordinate to draw at
 * @param color SDL_Color for the text
 */
void TextureManager_DrawText(const char* fontId, const char* text, int x, int y, SDL_Color color);

/**
 * @brief Get the width of a texture
 * 
 * @param id Identifier of the texture
 * @return int Width of the texture, or 0 if not found
 */
int TextureManager_GetTextureWidth(const char* id);

/**
 * @brief Get the height of a texture
 * 
 * @param id Identifier of the texture
 * @return int Height of the texture, or 0 if not found
 */
int TextureManager_GetTextureHeight(const char* id);

#endif /* TEXTURE_MANAGER_H */
