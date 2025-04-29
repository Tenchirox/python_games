#include "menu.h"
#include <stdio.h>

int main(int argc, char* argv[]) {
    // Create menu
    Menu menu;
    
    // Initialize menu
    if (!menu_init(&menu)) {
        printf("Failed to initialize menu!\n");
        return 1;
    }
    
    // Main loop
    while (!menu.quit) {
        // Handle events
        menu_handle_events(&menu);
        
        // Render menu
        menu_render(&menu);
        
        // Add a small delay to reduce CPU usage
        SDL_Delay(16); // ~60 FPS
    }
    
    // Clean up
    menu_cleanup(&menu);
    
    return 0;
}

