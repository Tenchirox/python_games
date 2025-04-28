# C Game Collection

This is a collection of classic games implemented in C, converted from the original Python versions.

## Games Included

1. **Snake** - Guide the snake to eat food and grow without hitting walls or itself
2. **Pac-Man** - Classic arcade game where you eat dots and avoid ghosts
3. **Tetris** - Arrange falling blocks to create complete lines (placeholder)
4. **Space Invaders** - Defend Earth from waves of alien invaders (placeholder)
5. **Connect Four** - Drop discs to connect four of your color in a row (placeholder)
6. **Brick Breaker** - Break all the bricks with a bouncing ball (placeholder)
7. **Solitaire** - Classic card game of patience and strategy (placeholder)
8. **Micro Machines** - Race tiny cars around various tracks (placeholder)

## Requirements

- GCC compiler
- Standard C libraries
- Terminal that supports ANSI escape sequences (for better display)

## Compilation

To compile all games and the menu system:

```bash
make
```

This will create an executable called `game_menu`.

To compile individual games for standalone testing:

```bash
make snake_standalone
make pacman_standalone
```

## Running the Games

To run the game menu:

```bash
./game_menu
```

To run individual games directly:

```bash
./snake_standalone
./pacman_standalone
```

## Controls

### Menu
- Use number keys to select a game
- Press 0 to exit

### Snake
- W: Move Up
- A: Move Left
- S: Move Down
- D: Move Right
- Q: Quit

### Pac-Man
- W: Move Up
- A: Move Left
- S: Move Down
- D: Move Right
- Q: Quit

## Implementation Notes

These games are terminal-based implementations using ASCII characters for display. They use non-blocking keyboard input to provide a responsive gaming experience.

The games have been converted from Python Tkinter GUI versions to C terminal versions, maintaining the core gameplay while adapting to the terminal interface.

## Future Improvements

- Add color support using ncurses
- Implement the remaining games
- Add sound effects
- Improve AI for ghost movement in Pac-Man
- Add high score tracking
