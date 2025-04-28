# Add each game as a separate executable
# Snake game
add_executable(snake snake.c)
target_link_libraries(snake
    common_framework
    ${SDL2_LIBRARIES}
    ${SDL2_IMAGE_LIBRARIES}
    ${SDL2_TTF_LIBRARIES}
    ${SDL2_MIXER_LIBRARIES}
)

# Tetris game
add_executable(tetris tetris.c)
target_link_libraries(tetris
    common_framework
    ${SDL2_LIBRARIES}
    ${SDL2_IMAGE_LIBRARIES}
    ${SDL2_TTF_LIBRARIES}
    ${SDL2_MIXER_LIBRARIES}
)

# Pacman game
add_executable(pacman pacman.c)
target_link_libraries(pacman
    common_framework
    ${SDL2_LIBRARIES}
    ${SDL2_IMAGE_LIBRARIES}
    ${SDL2_TTF_LIBRARIES}
    ${SDL2_MIXER_LIBRARIES}
)

# Space Invaders game
add_executable(space_invaders space_invaders.c)
target_link_libraries(space_invaders
    common_framework
    ${SDL2_LIBRARIES}
    ${SDL2_IMAGE_LIBRARIES}
    ${SDL2_TTF_LIBRARIES}
    ${SDL2_MIXER_LIBRARIES}
)

# Installation rules
install(TARGETS snake tetris pacman space_invaders DESTINATION bin)
