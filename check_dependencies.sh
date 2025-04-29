#!/bin/bash

# Check for SDL2
if ! command -v sdl2-config &> /dev/null; then
    echo "SDL2 is not installed. Please install libsdl2-dev."
    MISSING=1
else
    echo "SDL2 is installed."
fi

# Check for SDL2_ttf
if ! pkg-config --exists SDL2_ttf; then
    echo "SDL2_ttf is not installed. Please install libsdl2-ttf-dev."
    MISSING=1
else
    echo "SDL2_ttf is installed."
fi

# Check for SDL2_image
if ! pkg-config --exists SDL2_image; then
    echo "SDL2_image is not installed. Please install libsdl2-image-dev."
    MISSING=1
else
    echo "SDL2_image is installed."
fi

# Check for GCC
if ! command -v gcc &> /dev/null; then
    echo "GCC is not installed. Please install gcc."
    MISSING=1
else
    echo "GCC is installed."
fi

# Check for Make
if ! command -v make &> /dev/null; then
    echo "Make is not installed. Please install make."
    MISSING=1
else
    echo "Make is installed."
fi

if [ "$MISSING" == "1" ]; then
    echo -e "\nSome dependencies are missing. Please install them before building the project."
    echo "On Ubuntu/Debian, you can run:"
    echo "sudo apt-get update"
    echo "sudo apt-get install libsdl2-dev libsdl2-ttf-dev libsdl2-image-dev gcc make"
    exit 1
else
    echo -e "\nAll dependencies are installed. You can build the project with 'make'."
    exit 0
fi

