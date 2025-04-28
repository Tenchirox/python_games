CC = gcc
CFLAGS = -Wall -Wextra -std=c99
LDFLAGS = -lm

# List of source files
SRCS = game_menu.c snake.c pacman.c

# Generate object file names
OBJS = $(SRCS:.c=.o)

# Main target
all: game_menu

# Compile the main game menu
game_menu: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Compile individual games for testing
snake_standalone: snake.c
	$(CC) $(CFLAGS) -DSNAKE_STANDALONE -o $@ $< $(LDFLAGS)

pacman_standalone: pacman.c
	$(CC) $(CFLAGS) -DPACMAN_STANDALONE -o $@ $< $(LDFLAGS)

# Pattern rule for object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up
clean:
	rm -f game_menu snake_standalone pacman_standalone $(OBJS)

.PHONY: all clean
