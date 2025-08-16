cmake_minimum_required(VERSION 3.15)

set(PROJECT_NAME 3d_engine)
project(${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

include_directories("inc/")
include_directories("/usr/include/SFML")  # системный include

set(SOURCES
    main.cpp
    inc/scene.hpp
    src/scene.cpp
    inc/camera.hpp
    src/camera.cpp
    inc/matrix.hpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME}
    sfml-graphics
    sfml-window
    sfml-system
    sfml-audio
    freetype
    openal
    # flac
    vorbisenc
    vorbisfile
    vorbis
    ogg
    pthread
    X11
    GL
)
