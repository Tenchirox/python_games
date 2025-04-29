# C Games with SDL

This project is a collection of classic games implemented in C using the SDL2 library. The games are converted from Python implementations to C.

## Project Structure

```
c_games/
├── assets/            # Game assets (fonts, images, sounds)
├── include/           # Header files
│   ├── common/        # Common framework headers
│   └── games/         # Game-specific headers
├── src/               # Source files
│   ├── common/        # Common framework implementation
│   └── games/         # Game implementations
└── CMakeLists.txt     # Main CMake build file
```

## Games Included

- Snake: Classic snake game where you control a snake to eat food and grow without hitting walls or yourself
- Tetris: Arrange falling blocks to create complete lines
- Tic-tac-toe (Morpion): Classic game of X's and O's with AI opponent
- Pacman: Navigate a maze, eating dots while avoiding ghosts
- Space Invaders: Shoot down waves of descending aliens
- Connect Four: Drop discs to create a line of four of your color

## Common SDL Framework

The project includes a common SDL framework that provides:

- Window and renderer management
- Input handling (keyboard and mouse)
- Texture and image loading
- Text rendering
- Audio playback
- Game state management
- Utility functions

## Building the Project

### Prerequisites

- CMake 3.10 or higher
- SDL2 development libraries
- SDL2_image development libraries
- SDL2_ttf development libraries
- SDL2_mixer development libraries
- C compiler with C11 support

### Installing Dependencies on Ubuntu/Debian

```bash
sudo apt-get update
sudo apt-get install libsdl2-dev libsdl2-ttf-dev libsdl2-image-dev libsdl2-mixer-dev cmake gcc make
```

### Installing Dependencies on macOS

```bash
brew install sdl2 sdl2_ttf sdl2_image sdl2_mixer cmake
```

### Build Instructions

1. Create a build directory:
   ```
   mkdir build
   cd build
   ```

2. Configure with CMake:
   ```
   cmake ..
   ```

3. Build the project:
   ```
   cmake --build .
   ```

4. Run the game menu:
   ```
   ./game_menu
   ```

## Game Menu

The game menu allows you to select and launch any of the available games. Each game can also be run directly by executing its binary.

## Converting from Python to C

The games in this project were originally written in Python using Tkinter and PIL. The conversion to C with SDL involved:

1. Creating a common SDL framework to replace Tkinter functionality
2. Implementing game logic in C using structs and functions
3. Converting object-oriented designs to procedural code
4. Implementing manual memory management
5. Creating a state-based game architecture

## Controls

### Snake
- Arrow keys to move
- R to restart

### Tetris
- Left/Right arrows to move
- Up arrow to rotate
- Down arrow for soft drop
- Space for hard drop
- R to restart

### Tic-tac-toe
- Mouse click to place X/O
- Button to switch between 2-player and vs computer modes
- Button to restart game

### Space Invaders
- Left/Right arrows to move
- Space to shoot
- R to restart

### Connect Four
- Mouse click to drop disc
- Button to switch between 2-player and vs computer modes
- Button to restart game

## License

This project is open source and available under the MIT License.

