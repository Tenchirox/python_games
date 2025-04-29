#ifndef MENU_H
#define MENU_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>

// Constants
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define MAX_GAMES 9
#define GAMES_PER_ROW 3
#define CARD_WIDTH 200
#define CARD_HEIGHT 250
#define CARD_PADDING 20

// Game structure
typedef struct {
    char* name;
    char* description;
    char* module;
    char* class;
    SDL_Texture* thumbnail;
    SDL_Rect position;
    SDL_Rect button_position;
} Game;

// Menu structure
typedef struct {
    SDL_Window* window;
    SDL_Renderer* renderer;
    TTF_Font* title_font;
    TTF_Font* normal_font;
    TTF_Font* button_font;
    Game games[MAX_GAMES];
    int num_games;
    int scroll_offset;
    bool quit;
} Menu;

// Function prototypes
bool menu_init(Menu* menu);
void menu_cleanup(Menu* menu);
void menu_render(Menu* menu);
void menu_handle_events(Menu* menu);
void menu_create_game_cards(Menu* menu);
SDL_Texture* create_pacman_thumbnail(SDL_Renderer* renderer);
SDL_Texture* create_snake_thumbnail(SDL_Renderer* renderer);
SDL_Texture* create_tetris_thumbnail(SDL_Renderer* renderer);
SDL_Texture* create_space_invaders_thumbnail(SDL_Renderer* renderer);
SDL_Texture* create_connect_four_thumbnail(SDL_Renderer* renderer);
SDL_Texture* create_brick_breaker_thumbnail(SDL_Renderer* renderer);
SDL_Texture* create_solitaire_thumbnail(SDL_Renderer* renderer);
SDL_Texture* create_micromachines_thumbnail(SDL_Renderer* renderer);
SDL_Texture* create_tictactoe_thumbnail(SDL_Renderer* renderer);
void launch_game(Menu* menu, int game_index);

#endif /* MENU_H */

