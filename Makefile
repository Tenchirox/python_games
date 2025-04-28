CC = gcc
CFLAGS = -Wall -Wextra -std=c99
LDFLAGS = 

# Target executable
SNAKE_TARGET = snake_c

# Source files
SNAKE_SRC = snake.c

# Object files
SNAKE_OBJ = $(SNAKE_SRC:.c=.o)

# Default target
all: $(SNAKE_TARGET)

# Snake game
$(SNAKE_TARGET): $(SNAKE_OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Compile source files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up
clean:
	rm -f $(SNAKE_TARGET) $(SNAKE_OBJ)

# Run the snake game
run_snake: $(SNAKE_TARGET)
	./$(SNAKE_TARGET)

.PHONY: all clean run_snake
