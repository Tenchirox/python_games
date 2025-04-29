CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g
LDFLAGS = $(shell sdl2-config --libs) -lSDL2_ttf -lSDL2_image -lm

# Source files
SRCS = main.c menu.c
OBJS = $(SRCS:.c=.o)

# Target executable
TARGET = game_menu

# Default target
all: $(TARGET)

# Link object files to create executable
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

# Compile source files to object files
%.o: %.c
	$(CC) $(CFLAGS) $(shell sdl2-config --cflags) -c $< -o $@

# Clean up
clean:
	rm -f $(OBJS) $(TARGET)

# Run the program
run: $(TARGET)
	./$(TARGET)

# Create fonts directory and download a free font if needed
setup:
	mkdir -p fonts
	if [ ! -f fonts/arial.ttf ]; then \
		echo "Downloading a free font..."; \
		curl -o fonts/arial.ttf https://github.com/matomo-org/travis-scripts/raw/master/fonts/Arial.ttf || \
		cp /usr/share/fonts/truetype/freefont/FreeSans.ttf fonts/arial.ttf || \
		echo "Could not download or copy a font. Please manually add a TTF font to the fonts directory."; \
	fi

.PHONY: all clean run setup

