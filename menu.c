#include "menu.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool menu_init(Menu* menu) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return false;
    }

    // Initialize SDL_ttf
    if (TTF_Init() < 0) {
        printf("SDL_ttf could not initialize! TTF_Error: %s\n", TTF_GetError());
        SDL_Quit();
        return false;
    }

    // Initialize SDL_image
    int img_flags = IMG_INIT_PNG;
    if (!(IMG_Init(img_flags) & img_flags)) {
        printf("SDL_image could not initialize! IMG_Error: %s\n", IMG_GetError());
        TTF_Quit();
        SDL_Quit();
        return false;
    }

    // Create window
    menu->window = SDL_CreateWindow("Game Selection Menu", 
                                   SDL_WINDOWPOS_UNDEFINED, 
                                   SDL_WINDOWPOS_UNDEFINED, 
                                   WINDOW_WIDTH, 
                                   WINDOW_HEIGHT, 
                                   SDL_WINDOW_SHOWN);
    if (menu->window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        IMG_Quit();
        TTF_Quit();
        SDL_Quit();
        return false;
    }

    // Create renderer
    menu->renderer = SDL_CreateRenderer(menu->window, -1, SDL_RENDERER_ACCELERATED);
    if (menu->renderer == NULL) {
        printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(menu->window);
        IMG_Quit();
        TTF_Quit();
        SDL_Quit();
        return false;
    }

    // Load fonts
    menu->title_font = TTF_OpenFont("fonts/arial.ttf", 24);
    menu->normal_font = TTF_OpenFont("fonts/arial.ttf", 14);
    menu->button_font = TTF_OpenFont("fonts/arial.ttf", 16);
    
    if (menu->title_font == NULL || menu->normal_font == NULL || menu->button_font == NULL) {
        printf("Failed to load font! TTF_Error: %s\n", TTF_GetError());
        // Try to load system fonts if the specified fonts are not available
        menu->title_font = TTF_OpenFont("/usr/share/fonts/truetype/freefont/FreeSans.ttf", 24);
        menu->normal_font = TTF_OpenFont("/usr/share/fonts/truetype/freefont/FreeSans.ttf", 14);
        menu->button_font = TTF_OpenFont("/usr/share/fonts/truetype/freefont/FreeSans.ttf", 16);
        
        if (menu->title_font == NULL || menu->normal_font == NULL || menu->button_font == NULL) {
            printf("Failed to load system fonts! TTF_Error: %s\n", TTF_GetError());
            SDL_DestroyRenderer(menu->renderer);
            SDL_DestroyWindow(menu->window);
            IMG_Quit();
            TTF_Quit();
            SDL_Quit();
            return false;
        }
    }

    // Initialize menu properties
    menu->scroll_offset = 0;
    menu->quit = false;
    menu->num_games = 0;

    // Initialize games
    menu_create_game_cards(menu);

    return true;
}

void menu_cleanup(Menu* menu) {
    // Free game resources
    for (int i = 0; i < menu->num_games; i++) {
        SDL_DestroyTexture(menu->games[i].thumbnail);
        free(menu->games[i].name);
        free(menu->games[i].description);
        free(menu->games[i].module);
        free(menu->games[i].class);
    }

    // Free fonts
    TTF_CloseFont(menu->title_font);
    TTF_CloseFont(menu->normal_font);
    TTF_CloseFont(menu->button_font);

    // Destroy renderer and window
    SDL_DestroyRenderer(menu->renderer);
    SDL_DestroyWindow(menu->window);

    // Quit SDL subsystems
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
}

void menu_create_game_cards(Menu* menu) {
    // Initialize games array
    menu->num_games = 9;

    // Pac-Man
    menu->games[0].name = strdup("Pac-Man");
    menu->games[0].description = strdup("Classic arcade game where you eat dots and avoid ghosts");
    menu->games[0].module = strdup("pacman2");
    menu->games[0].class = strdup("PacManGame");
    menu->games[0].thumbnail = create_pacman_thumbnail(menu->renderer);

    // Snake
    menu->games[1].name = strdup("Snake");
    menu->games[1].description = strdup("Guide the snake to eat food and grow without hitting walls or itself");
    menu->games[1].module = strdup("snake");
    menu->games[1].class = strdup("SnakeGame");
    menu->games[1].thumbnail = create_snake_thumbnail(menu->renderer);

    // Tetris
    menu->games[2].name = strdup("Tetris");
    menu->games[2].description = strdup("Arrange falling blocks to create complete lines");
    menu->games[2].module = strdup("tetris");
    menu->games[2].class = strdup("TetrisGame");
    menu->games[2].thumbnail = create_tetris_thumbnail(menu->renderer);

    // Space Invaders
    menu->games[3].name = strdup("Space Invaders");
    menu->games[3].description = strdup("Defend Earth from waves of alien invaders");
    menu->games[3].module = strdup("space_invaders");
    menu->games[3].class = strdup("SpaceInvadersGame");
    menu->games[3].thumbnail = create_space_invaders_thumbnail(menu->renderer);

    // Connect Four
    menu->games[4].name = strdup("Connect Four");
    menu->games[4].description = strdup("Drop discs to connect four of your color in a row");
    menu->games[4].module = strdup("puissance4");
    menu->games[4].class = strdup("Puissance4Game");
    menu->games[4].thumbnail = create_connect_four_thumbnail(menu->renderer);

    // Brick Breaker
    menu->games[5].name = strdup("Brick Breaker");
    menu->games[5].description = strdup("Break all the bricks with a bouncing ball");
    menu->games[5].module = strdup("casse_briques");
    menu->games[5].class = strdup("BrickGame");
    menu->games[5].thumbnail = create_brick_breaker_thumbnail(menu->renderer);

    // Solitaire
    menu->games[6].name = strdup("Solitaire");
    menu->games[6].description = strdup("Classic card game of patience and strategy");
    menu->games[6].module = strdup("solitaire");
    menu->games[6].class = strdup("SolitaireGame");
    menu->games[6].thumbnail = create_solitaire_thumbnail(menu->renderer);

    // Micro Machines
    menu->games[7].name = strdup("Micro Machines");
    menu->games[7].description = strdup("Race tiny cars around various tracks");
    menu->games[7].module = strdup("micromachines");
    menu->games[7].class = strdup("Car");
    menu->games[7].thumbnail = create_micromachines_thumbnail(menu->renderer);

    // Tic-Tac-Toe
    menu->games[8].name = strdup("Tic-Tac-Toe");
    menu->games[8].description = strdup("Classic game of X's and O's on a 3x3 grid");
    menu->games[8].module = strdup("morpion");
    menu->games[8].class = strdup("MorpionGame");
    menu->games[8].thumbnail = create_tictactoe_thumbnail(menu->renderer);

    // Calculate positions for each game card
    for (int i = 0; i < menu->num_games; i++) {
        int row = i / GAMES_PER_ROW;
        int col = i % GAMES_PER_ROW;
        
        int x = col * (CARD_WIDTH + CARD_PADDING) + CARD_PADDING;
        int y = row * (CARD_HEIGHT + CARD_PADDING) + CARD_PADDING + 60; // 60px for title
        
        menu->games[i].position.x = x;
        menu->games[i].position.y = y;
        menu->games[i].position.w = CARD_WIDTH;
        menu->games[i].position.h = CARD_HEIGHT;
        
        // Button position (centered at the bottom of the card)
        menu->games[i].button_position.x = x + CARD_WIDTH/2 - 50;
        menu->games[i].button_position.y = y + CARD_HEIGHT - 40;
        menu->games[i].button_position.w = 100;
        menu->games[i].button_position.h = 30;
    }
}

void menu_render(Menu* menu) {
    // Clear screen
    SDL_SetRenderDrawColor(menu->renderer, 47, 47, 47, 255); // #2F2F2F
    SDL_RenderClear(menu->renderer);
    
    // Render title
    SDL_Color white = {255, 255, 255, 255};
    SDL_Surface* title_surface = TTF_RenderText_Blended(menu->title_font, "Select a Game", white);
    SDL_Texture* title_texture = SDL_CreateTextureFromSurface(menu->renderer, title_surface);
    
    SDL_Rect title_rect = {
        WINDOW_WIDTH / 2 - title_surface->w / 2,
        20,
        title_surface->w,
        title_surface->h
    };
    
    SDL_RenderCopy(menu->renderer, title_texture, NULL, &title_rect);
    SDL_FreeSurface(title_surface);
    SDL_DestroyTexture(title_texture);
    
    // Render game cards
    for (int i = 0; i < menu->num_games; i++) {
        // Adjust position based on scroll offset
        SDL_Rect adjusted_position = menu->games[i].position;
        adjusted_position.y -= menu->scroll_offset;
        
        // Only render if the card is visible
        if (adjusted_position.y + adjusted_position.h > 60 && 
            adjusted_position.y < WINDOW_HEIGHT) {
            
            // Card background
            SDL_SetRenderDrawColor(menu->renderer, 63, 63, 63, 255); // #3F3F3F
            SDL_RenderFillRect(menu->renderer, &adjusted_position);
            
            // Card border
            SDL_SetRenderDrawColor(menu->renderer, 100, 100, 100, 255);
            SDL_RenderDrawRect(menu->renderer, &adjusted_position);
            
            // Thumbnail
            SDL_Rect thumbnail_rect = {
                adjusted_position.x + adjusted_position.w/2 - 75,
                adjusted_position.y + 20,
                150,
                150
            };
            SDL_RenderCopy(menu->renderer, menu->games[i].thumbnail, NULL, &thumbnail_rect);
            
            // Game name
            SDL_Surface* name_surface = TTF_RenderText_Blended(menu->normal_font, menu->games[i].name, white);
            SDL_Texture* name_texture = SDL_CreateTextureFromSurface(menu->renderer, name_surface);
            
            SDL_Rect name_rect = {
                adjusted_position.x + adjusted_position.w/2 - name_surface->w/2,
                adjusted_position.y + 180,
                name_surface->w,
                name_surface->h
            };
            
            SDL_RenderCopy(menu->renderer, name_texture, NULL, &name_rect);
            SDL_FreeSurface(name_surface);
            SDL_DestroyTexture(name_texture);
            
            // Play button
            SDL_Rect adjusted_button = menu->games[i].button_position;
            adjusted_button.y -= menu->scroll_offset;
            
            SDL_SetRenderDrawColor(menu->renderer, 76, 175, 80, 255); // #4CAF50
            SDL_RenderFillRect(menu->renderer, &adjusted_button);
            
            // Button text
            SDL_Surface* button_surface = TTF_RenderText_Blended(menu->button_font, "Play", white);
            SDL_Texture* button_texture = SDL_CreateTextureFromSurface(menu->renderer, button_surface);
            
            SDL_Rect button_text_rect = {
                adjusted_button.x + adjusted_button.w/2 - button_surface->w/2,
                adjusted_button.y + adjusted_button.h/2 - button_surface->h/2,
                button_surface->w,
                button_surface->h
            };
            
            SDL_RenderCopy(menu->renderer, button_texture, NULL, &button_text_rect);
            SDL_FreeSurface(button_surface);
            SDL_DestroyTexture(button_texture);
        }
    }
    
    // Present renderer
    SDL_RenderPresent(menu->renderer);
}

void menu_handle_events(Menu* menu) {
    SDL_Event e;
    
    while (SDL_PollEvent(&e) != 0) {
        if (e.type == SDL_QUIT) {
            menu->quit = true;
        }
        else if (e.type == SDL_MOUSEWHEEL) {
            // Handle scrolling
            if (e.wheel.y > 0) {
                // Scroll up
                menu->scroll_offset -= 30;
                if (menu->scroll_offset < 0) {
                    menu->scroll_offset = 0;
                }
            }
            else if (e.wheel.y < 0) {
                // Scroll down
                int max_scroll = (menu->num_games / GAMES_PER_ROW + 1) * (CARD_HEIGHT + CARD_PADDING) - WINDOW_HEIGHT + 100;
                menu->scroll_offset += 30;
                if (menu->scroll_offset > max_scroll) {
                    menu->scroll_offset = max_scroll;
                }
                if (menu->scroll_offset < 0) {
                    menu->scroll_offset = 0;
                }
            }
        }
        else if (e.type == SDL_MOUSEBUTTONDOWN) {
            if (e.button.button == SDL_BUTTON_LEFT) {
                int mouse_x = e.button.x;
                int mouse_y = e.button.y;
                
                // Check if a play button was clicked
                for (int i = 0; i < menu->num_games; i++) {
                    SDL_Rect adjusted_button = menu->games[i].button_position;
                    adjusted_button.y -= menu->scroll_offset;
                    
                    if (mouse_x >= adjusted_button.x && mouse_x <= adjusted_button.x + adjusted_button.w &&
                        mouse_y >= adjusted_button.y && mouse_y <= adjusted_button.y + adjusted_button.h) {
                        launch_game(menu, i);
                        break;
                    }
                }
            }
        }
    }
}

void launch_game(Menu* menu, int game_index) {
    if (game_index < 0 || game_index >= menu->num_games) {
        return;
    }
    
    printf("Launching game: %s\n", menu->games[game_index].name);
    printf("Module: %s, Class: %s\n", menu->games[game_index].module, menu->games[game_index].class);
    
    // In a real implementation, we would launch the game here
    // For now, we'll just print a message
    printf("Game launched!\n");
}

// Thumbnail creation functions
SDL_Texture* create_pacman_thumbnail(SDL_Renderer* renderer) {
    // Create a surface for the thumbnail
    SDL_Surface* surface = SDL_CreateRGBSurface(0, 150, 150, 32, 0, 0, 0, 0);
    
    // Fill with black background
    SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0, 0, 0));
    
    // Draw maze-like background (simplified)
    SDL_Rect rect;
    for (int y = 0; y < 150; y += 15) {
        for (int x = 0; x < 150; x += 15) {
            if ((x / 15 + y / 15) % 2 == 0) {
                rect.x = x;
                rect.y = y;
                rect.w = 15;
                rect.h = 15;
                SDL_FillRect(surface, &rect, SDL_MapRGB(surface->format, 0, 0, 170)); // #0000AA
            }
        }
    }
    
    // Create a texture from the surface
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    
    return texture;
}

SDL_Texture* create_snake_thumbnail(SDL_Renderer* renderer) {
    // Create a surface for the thumbnail
    SDL_Surface* surface = SDL_CreateRGBSurface(0, 150, 150, 32, 0, 0, 0, 0);
    
    // Fill with dark green background
    SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0, 51, 0)); // #003300
    
    // Create a texture from the surface
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    
    return texture;
}

SDL_Texture* create_tetris_thumbnail(SDL_Renderer* renderer) {
    // Create a surface for the thumbnail
    SDL_Surface* surface = SDL_CreateRGBSurface(0, 150, 150, 32, 0, 0, 0, 0);
    
    // Fill with black background
    SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0, 0, 0));
    
    // Create a texture from the surface
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    
    return texture;
}

SDL_Texture* create_space_invaders_thumbnail(SDL_Renderer* renderer) {
    // Create a surface for the thumbnail
    SDL_Surface* surface = SDL_CreateRGBSurface(0, 150, 150, 32, 0, 0, 0, 0);
    
    // Fill with black background
    SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0, 0, 0));
    
    // Create a texture from the surface
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    
    return texture;
}

SDL_Texture* create_connect_four_thumbnail(SDL_Renderer* renderer) {
    // Create a surface for the thumbnail
    SDL_Surface* surface = SDL_CreateRGBSurface(0, 150, 150, 32, 0, 0, 0, 0);
    
    // Fill with blue background
    SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0, 0, 128)); // #000080
    
    // Create a texture from the surface
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    
    return texture;
}

SDL_Texture* create_brick_breaker_thumbnail(SDL_Renderer* renderer) {
    // Create a surface for the thumbnail
    SDL_Surface* surface = SDL_CreateRGBSurface(0, 150, 150, 32, 0, 0, 0, 0);
    
    // Fill with dark background
    SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 32, 32, 32));
    
    // Create a texture from the surface
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    
    return texture;
}

SDL_Texture* create_solitaire_thumbnail(SDL_Renderer* renderer) {
    // Create a surface for the thumbnail
    SDL_Surface* surface = SDL_CreateRGBSurface(0, 150, 150, 32, 0, 0, 0, 0);
    
    // Fill with green background (card table)
    SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0, 100, 0)); // #006400
    
    // Create a texture from the surface
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    
    return texture;
}

SDL_Texture* create_micromachines_thumbnail(SDL_Renderer* renderer) {
    // Create a surface for the thumbnail
    SDL_Surface* surface = SDL_CreateRGBSurface(0, 150, 150, 32, 0, 0, 0, 0);
    
    // Fill with green background (grass)
    SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0, 170, 0)); // #00AA00
    
    // Create a texture from the surface
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    
    return texture;
}

SDL_Texture* create_tictactoe_thumbnail(SDL_Renderer* renderer) {
    // Create a surface for the thumbnail
    SDL_Surface* surface = SDL_CreateRGBSurface(0, 150, 150, 32, 0, 0, 0, 0);
    
    // Fill with dark gray background
    SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 63, 63, 63)); // #3F3F3F
    
    // Create a texture from the surface
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    
    return texture;
}

