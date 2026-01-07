cmake_minimum_required(VERSION 4.00)
project(PlatformerClone)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Compiler warnings
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Main executable (platform layer)
add_executable(PlatformerClone
        src/main.cpp
        src/Kor_Lib.h
        src/gl_renderer.h
        "3rd Party Lib/glcorearb.h"
        src/platform.h
        src/input.h
        src/render_interface.h
        src/assets.h
        src/game.h
        src/sound.h
)

if(WIN32)
    target_link_libraries(PlatformerClone PRIVATE opengl32 gdi32)
endif()

# Game DLL
add_library(game SHARED
        src/game.cpp
)

# Remove the "lib" prefix on Windows
set_target_properties(game PROPERTIES PREFIX "")

# Make sure the game DLL exports symbols properly
target_compile_definitions(game PRIVATE GAME_DLL_EXPORT)

# Copy the game DLL to the executable directory after build
add_custom_command(TARGET game POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        $<TARGET_FILE:game>
        $<TARGET_FILE_DIR:PlatformerClone>
)
add_dependencies(PlatformerClone game)

