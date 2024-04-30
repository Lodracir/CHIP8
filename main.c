#include <stdio.h>
#include "raylib.h"

int main(void) {
    InitWindow(800, 450, "Test");

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
