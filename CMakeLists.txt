cmake_minimum_required(VERSION 4.0)
project(PlatformerClone)

set(CMAKE_CXX_STANDARD 20)

add_executable(PlatformerClone main.cpp
        Kor_Lib.h
        "gl_renderer.h"
        "3rd Party Lib/glcorearb.h"
        platform.h
        input.h
)
if(WIN32)
    target_link_libraries(PlatformerClone PRIVATE opengl32 gdi32)
endif()
