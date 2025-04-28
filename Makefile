CC = gcc
CFLAGS = -Wall -Wextra -std=c99
LDFLAGS = -lSDL2

# Main targets
all: snake game_menu

# Snake game
snake: snake.c
	$(CC) $(CFLAGS) -o snake snake.c $(LDFLAGS)

# Game menu
game_menu: game_menu.c
	$(CC) $(CFLAGS) -o game_menu game_menu.c $(LDFLAGS)

# Clean up
clean:
	rm -f snake game_menu *.o

.PHONY: all clean
