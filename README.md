# Python Games Converted to C

This repository contains classic arcade games implemented in C using SDL2. The games were originally written in Python using Tkinter and have been converted to C for better performance and portability.

## Games Included

- **Snake**: Guide the snake to eat food and grow without hitting walls or itself
- **Pac-Man**: Classic arcade game where you eat dots and avoid ghosts
- **Tetris**: Arrange falling blocks to create complete lines
- **Space Invaders**: Defend Earth from waves of alien invaders
- **Connect Four**: Drop discs to connect four of your color in a row
- **Brick Breaker**: Break all the bricks with a bouncing ball
- **Solitaire**: Classic card game of patience and strategy
- **Micro Machines**: Race tiny cars around various tracks
- **Tic-Tac-Toe**: Classic game of X's and O's on a 3x3 grid

## Requirements

- C compiler (gcc recommended)
- SDL2 library

## Installation

### Linux

```bash
# Install SDL2
sudo apt-get install libsdl2-dev

# Clone the repository
git clone https://github.com/Tenchirox/python_games.git
cd python_games

# Compile the games
make
```

### macOS

```bash
# Install SDL2 using Homebrew
brew install sdl2

# Clone the repository
git clone https://github.com/Tenchirox/python_games.git
cd python_games

# Compile the games
make
```

### Windows

1. Install MinGW or another C compiler
2. Download and install SDL2 development libraries for MinGW
3. Clone the repository
4. Compile using the Makefile or manually with gcc

## Running the Games

You can run the game menu to select a game:

```bash
./game_menu
```

Or run individual games directly:

```bash
./snake
```

## Controls

### Snake
- Arrow keys to change direction
- Enter to start the game
- R to restart after game over
- Escape to quit

## Implementation Details

The games use SDL2 for graphics and input handling. Each game is implemented as a standalone C file that can be compiled and run independently. The game menu provides a unified interface to launch any of the games.

## Original Python Implementation

The original Python implementations using Tkinter are still available in the repository with `.py` extensions. The C versions aim to maintain the same gameplay while improving performance.

## License

This project is open source and available under the MIT License.
