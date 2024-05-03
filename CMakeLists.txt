cmake_minimum_required(VERSION 3.28)
project(CHIP8 C)

find_package(raylib 4.0 REQUIRED) # Requires at least version 3.0
set(CMAKE_C_STANDARD 11)

include_directories(
        Inc
)

add_executable(CHIP8
        main.c
        CHIP8/CHIP8.c
        CHIP8/CHIP8.h
)

target_link_libraries(${PROJECT_NAME} raylib)

# Copy required files to build directory
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/CHIP8/char_set.bin
        ${CMAKE_BINARY_DIR}/bin/char_set.bin
)

# Checks if OSX and links appropriate frameworks (only required on MacOS)
if (APPLE)
    target_link_libraries(${PROJECT_NAME} "-framework IOKit")
    target_link_libraries(${PROJECT_NAME} "-framework Cocoa")
    target_link_libraries(${PROJECT_NAME} "-framework OpenGL")
endif()
