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
- Pacman: Navigate a maze, eating dots while avoiding ghosts
- Space Invaders: Shoot down waves of descending aliens

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

## License

This project is open source and available under the MIT License.
