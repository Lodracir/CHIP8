/////////////////////////////////////////////////
/// Includes
/////////////////////////////////////////////////

#include <stdio.h>
#include "raylib.h"
#include "CHIP8/CHIP8.h"

/////////////////////////////////////////////////
/// Defines
/////////////////////////////////////////////////

#define MAIN_WINDOW_NAME    "CHIP8"
#define MAIN_WINDOW_SCALE_FACTOR    10
#define MAIN_WINDOW_WIDTH   (CHIP8_WIDTH_SCREEN * MAIN_WINDOW_SCALE_FACTOR)
#define MAIN_WINDOW_HEIGHT  (CHIP8_HEIGHT_SCREEN * MAIN_WINDOW_SCALE_FACTOR)
#define MAIN_WINDOW_FPS     30

/////////////////////////////////////////////////
/// Main function
/////////////////////////////////////////////////

int main(void)
{
    ///Local variables
    chip8_t CHIP8;

    ///Init CHIP8
    CHIP8_Init(&CHIP8, NULL, 0);

    InitWindow(MAIN_WINDOW_WIDTH, MAIN_WINDOW_HEIGHT, MAIN_WINDOW_NAME);
    SetTargetFPS(MAIN_WINDOW_FPS);

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(BLACK);
        DrawText("Congrats! You created your first window!", 190, 200, 20, WHITE);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
