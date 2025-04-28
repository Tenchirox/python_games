#ifndef UTILS_H
#define UTILS_H

#include <SDL2/SDL.h>
#include <stdbool.h>

/**
 * @brief Check if a point is inside a rectangle
 * 
 * @param x X coordinate of the point
 * @param y Y coordinate of the point
 * @param rect Rectangle to check against
 * @return true if the point is inside the rectangle, false otherwise
 */
bool Utils_PointInRect(int x, int y, SDL_Rect rect);

/**
 * @brief Check if two rectangles intersect
 * 
 * @param rect1 First rectangle
 * @param rect2 Second rectangle
 * @return true if the rectangles intersect, false otherwise
 */
bool Utils_RectIntersect(SDL_Rect rect1, SDL_Rect rect2);

/**
 * @brief Generate a random integer within a range
 * 
 * @param min Minimum value (inclusive)
 * @param max Maximum value (inclusive)
 * @return int Random integer between min and max
 */
int Utils_RandomInt(int min, int max);

/**
 * @brief Generate a random float within a range
 * 
 * @param min Minimum value (inclusive)
 * @param max Maximum value (inclusive)
 * @return float Random float between min and max
 */
float Utils_RandomFloat(float min, float max);

/**
 * @brief Calculate the distance between two points
 * 
 * @param x1 X coordinate of the first point
 * @param y1 Y coordinate of the first point
 * @param x2 X coordinate of the second point
 * @param y2 Y coordinate of the second point
 * @return float Distance between the points
 */
float Utils_Distance(float x1, float y1, float x2, float y2);

/**
 * @brief Clamp a value between a minimum and maximum
 * 
 * @param value Value to clamp
 * @param min Minimum value
 * @param max Maximum value
 * @return float Clamped value
 */
float Utils_Clamp(float value, float min, float max);

/**
 * @brief Linear interpolation between two values
 * 
 * @param a Start value
 * @param b End value
 * @param t Interpolation factor (0.0 to 1.0)
 * @return float Interpolated value
 */
float Utils_Lerp(float a, float b, float t);

#endif /* UTILS_H */
