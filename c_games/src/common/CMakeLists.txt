set(COMMON_SOURCES
    sdl_framework.c
    input_handler.c
    texture_manager.c
    audio_manager.c
    game_state.c
    utils.c
)

add_library(common_framework STATIC ${COMMON_SOURCES})

target_link_libraries(common_framework
    ${SDL2_LIBRARIES}
    ${SDL2_IMAGE_LIBRARIES}
    ${SDL2_TTF_LIBRARIES}
    ${SDL2_MIXER_LIBRARIES}
)

target_include_directories(common_framework PUBLIC
    ${CMAKE_SOURCE_DIR}/include/common
)
