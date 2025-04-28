# Python Games Collection

This repository contains a collection of classic arcade games implemented in Python using Tkinter for the GUI.

## Games Included

- **Snake**: Guide the snake to eat food and grow without hitting walls or itself
- **Pac-Man**: Classic arcade game where you eat dots and avoid ghosts
- **Tetris**: Arrange falling blocks to create complete lines
- **Space Invaders**: Defend Earth from waves of alien invaders
- **Connect Four**: Drop discs to connect four of your color in a row
- **Brick Breaker**: Break all the bricks with a bouncing ball
- **Solitaire**: Classic card game of patience and strategy
- **Micro Machines**: Race tiny cars around various tracks

## Requirements

- Python 3.x
- Pillow (PIL Fork) for image processing

Install the required packages:

```bash
pip install pillow
```

## Running the Games

You can run the game menu to select any game:

```bash
python game_menu.py
```

Or run individual games directly:

```bash
python snake.py
python pacman.py
python tetris.py
# etc.
```

## C Implementation

A C version of the Snake game is also included for performance comparison. This version runs in the terminal and uses ASCII characters for display.

### Compiling the C Version

```bash
make
```

### Running the C Version

```bash
./snake_c
```

Or use the make target:

```bash
make run_snake
```

### Controls for C Version

- Arrow keys or WASD to move the snake
- Q to quit the game

## Game Controls

Most games use arrow keys for movement and spacebar for actions. See the in-game instructions for specific controls.

## Sprites

The games will automatically generate sprites if needed, but you can also place custom sprites in the `sprites` directory to customize the appearance of the games.

## Fixed Issues

- Fixed toggle_pause method in pacman.py that was incorrectly placed outside the class
- Created pacman2.py to match the reference in game_menu.py
- Fixed various minor bugs and improved code organization
- Added C implementation of Snake game for performance comparison

## License

This project is open source and available under the MIT License.
