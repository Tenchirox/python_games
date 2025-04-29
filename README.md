# Game Menu in C with SDL2

This is a C implementation of the game menu system originally written in Python using Tkinter. The new implementation uses SDL2 for graphics and event handling.

## Prerequisites

To build and run this project, you need the following libraries installed:

- SDL2
- SDL2_ttf
- SDL2_image
- GCC or another C compiler
- Make

### Installing Dependencies on Ubuntu/Debian

```bash
sudo apt-get update
sudo apt-get install libsdl2-dev libsdl2-ttf-dev libsdl2-image-dev gcc make
```

### Installing Dependencies on macOS

```bash
brew install sdl2 sdl2_ttf sdl2_image
```

### Installing Dependencies on Windows

For Windows, you can use MSYS2 or download the development libraries from:
- [SDL2](https://www.libsdl.org/download-2.0.php)
- [SDL2_ttf](https://www.libsdl.org/projects/SDL_ttf/)
- [SDL2_image](https://www.libsdl.org/projects/SDL_image/)

## Building the Project

1. Clone the repository
2. Run the setup to create the fonts directory and download a free font:
   ```bash
   make setup
   ```
3. Build the project:
   ```bash
   make
   ```

## Running the Game Menu

After building, you can run the game menu with:

```bash
make run
```

or directly:

```bash
./game_menu
```

## Project Structure

- `main.c` - Main entry point for the application
- `menu.h` - Header file for the menu system
- `menu.c` - Implementation of the menu system
- `Makefile` - Build configuration

## Features

- Game selection menu with thumbnails
- Scrollable interface
- Play buttons for each game
- Similar look and feel to the original Python/Tkinter implementation

## Notes

This implementation provides the menu system only. The actual games are not implemented in C. When a game is selected, the program will print a message indicating which game would be launched.

To fully implement the games, each game would need to be ported from Python to C with SDL2.

