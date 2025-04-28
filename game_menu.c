/**
 * Game Menu in C
 * Converted from Python Tkinter version
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Game function prototypes
void launch_pacman();
void launch_snake();
void launch_tetris();
void launch_space_invaders();
void launch_connect_four();
void launch_brick_breaker();
void launch_solitaire();
void launch_micromachines();

// Structure to represent a game
typedef struct {
    char name[50];
    char description[200];
    void (*launch_function)();
} Game;

// Array of available games
Game games[] = {
    {"Pac-Man", "Classic arcade game where you eat dots and avoid ghosts", launch_pacman},
    {"Snake", "Guide the snake to eat food and grow without hitting walls or itself", launch_snake},
    {"Tetris", "Arrange falling blocks to create complete lines", launch_tetris},
    {"Space Invaders", "Defend Earth from waves of alien invaders", launch_space_invaders},
    {"Connect Four", "Drop discs to connect four of your color in a row", launch_connect_four},
    {"Brick Breaker", "Break all the bricks with a bouncing ball", launch_brick_breaker},
    {"Solitaire", "Classic card game of patience and strategy", launch_solitaire},
    {"Micro Machines", "Race tiny cars around various tracks", launch_micromachines}
};

// Number of games
const int num_games = sizeof(games) / sizeof(games[0]);

// Function to display the menu
void display_menu() {
    int choice;
    int valid_input;
    
    while (1) {
        // Clear the screen
        system("clear"); // Use "cls" for Windows
        
        // Display header
        printf("\n");
        printf("╔══════════════════════════════════════════╗\n");
        printf("║             GAME SELECTION MENU          ║\n");
        printf("╚══════════════════════════════════════════╝\n\n");
        
        // Display games
        for (int i = 0; i < num_games; i++) {
            printf("  %d. %-15s - %s\n", i + 1, games[i].name, games[i].description);
        }
        
        // Display exit option
        printf("\n  0. Exit\n\n");
        
        // Get user choice
        printf("Enter your choice (0-%d): ", num_games);
        valid_input = scanf("%d", &choice);
        
        // Clear input buffer
        while (getchar() != '\n');
        
        // Validate input
        if (!valid_input || choice < 0 || choice > num_games) {
            printf("\nInvalid choice. Press Enter to try again...");
            getchar();
            continue;
        }
        
        // Exit if user chose 0
        if (choice == 0) {
            printf("\nThank you for playing! Goodbye.\n");
            break;
        }
        
        // Launch the selected game
        printf("\nLaunching %s...\n", games[choice - 1].name);
        games[choice - 1].launch_function();
        
        // Wait for user to press Enter before returning to menu
        printf("\nPress Enter to return to the menu...");
        getchar();
    }
}

// Main function
int main() {
    display_menu();
    return 0;
}

// Placeholder functions for game launches
// These would be replaced with actual implementations or calls to other C files

void launch_pacman() {
    printf("Pac-Man game would launch here.\n");
    // Implementation would be in pacman.c
}

void launch_snake() {
    printf("Snake game would launch here.\n");
    // Implementation would be in snake.c
}

void launch_tetris() {
    printf("Tetris game would launch here.\n");
    // Implementation would be in tetris.c
}

void launch_space_invaders() {
    printf("Space Invaders game would launch here.\n");
    // Implementation would be in space_invaders.c
}

void launch_connect_four() {
    printf("Connect Four game would launch here.\n");
    // Implementation would be in connect_four.c
}

void launch_brick_breaker() {
    printf("Brick Breaker game would launch here.\n");
    // Implementation would be in brick_breaker.c
}

void launch_solitaire() {
    printf("Solitaire game would launch here.\n");
    // Implementation would be in solitaire.c
}

void launch_micromachines() {
    printf("Micro Machines game would launch here.\n");
    // Implementation would be in micromachines.c
}
