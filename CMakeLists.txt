cmake_minimum_required(VERSION 3.28)
project(Proyecto_1 C)

set(CMAKE_C_STANDARD 23)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake_modules)

set(SDL2_PATH "D:/Dev/SDL/SDL2-2.30.6/x86_64-w64-mingw32")
set(SDL2_IMAGE_PATH "D:/Dev/SDL/SDL_Image/SDL2_image-2.8.2/x86_64-w64-mingw32")

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIR})

find_package(SDL2_image REQUIRED)
include_directories(${SDL2_IMAGE_INCLUDE_DIR})

add_executable(Proyecto_1 main.c
        figure.h
        explosion.h
)

target_link_libraries(Proyecto_1 ${SDL2_LIBRARY} ${SDL2MAIN_LIBRARY} ${SDL2_IMAGE_LIBRARY})
