cmake_minimum_required(VERSION 3.10)
project(C_Games C)

# Set C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Find SDL2 packages
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(SDL2_mixer REQUIRED)

# Include directories
include_directories(
    ${SDL2_INCLUDE_DIRS}
    ${SDL2_IMAGE_INCLUDE_DIRS}
    ${SDL2_TTF_INCLUDE_DIRS}
    ${SDL2_MIXER_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include/common
    ${CMAKE_SOURCE_DIR}/include/games
)

# Define the common framework library
add_subdirectory(src/common)

# Add games subdirectories
add_subdirectory(src/games)

# Add the main menu executable
add_executable(game_menu src/game_menu.c)
target_link_libraries(game_menu
    common_framework
    ${SDL2_LIBRARIES}
    ${SDL2_IMAGE_LIBRARIES}
    ${SDL2_TTF_LIBRARIES}
    ${SDL2_MIXER_LIBRARIES}
)

# Copy assets to build directory
file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})

# Installation rules
install(TARGETS game_menu DESTINATION bin)
install(DIRECTORY assets/ DESTINATION share/c_games/assets)
