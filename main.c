/////////////////////////////////////////////////
/// Includes
/////////////////////////////////////////////////

#include <stdio.h>
#include "raylib.h"
#include "CHIP8/CHIP8.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

/////////////////////////////////////////////////
/// Defines
/////////////////////////////////////////////////

#define MAIN_WINDOW_NAME    "CHIP8"
#define MAIN_WINDOW_SCALE_FACTOR    10
#define MAIN_WINDOW_WIDTH   (CHIP8_WIDTH_SCREEN * MAIN_WINDOW_SCALE_FACTOR)
#define MAIN_WINDOW_HEIGHT  (CHIP8_HEIGHT_SCREEN * MAIN_WINDOW_SCALE_FACTOR)
#define MAIN_WINDOW_FPS     30

/////////////////////////////////////////////////
/// Local variables
/////////////////////////////////////////////////

static chip8_keymap_t keymap;

/////////////////////////////////////////////////
/// Local functions
/////////////////////////////////////////////////

static void keyboard_map(void);
static void keyboard_logic(void);

/////////////////////////////////////////////////
/// Main function
/////////////////////////////////////////////////

int main(void)
{
    ///Local variables
    chip8_t CHIP8;

    ///Map keyboard
    keyboard_map();

    ///Init CHIP8
    CHIP8_Init(&CHIP8, &keymap, NULL, 0);

    InitWindow(MAIN_WINDOW_WIDTH, MAIN_WINDOW_HEIGHT, MAIN_WINDOW_NAME);
    SetTargetFPS(MAIN_WINDOW_FPS);

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(BLACK);
        for(uint16_t y = 0; y < CHIP8_HEIGHT_SCREEN; y++)
        {
            for(uint16_t x = 0; x < CHIP8_WIDTH_SCREEN; x++)
            {
                if( CHIP8_IsPixelSet(&CHIP8, x, y) )
                {
                    DrawRectangle(x * 10, y * 10, 10, 10, WHITE);
                }
            }
        }
        EndDrawing();
    }

    CloseWindow();
    return 0;
}

static void keyboard_map(void)
{
    keymap.map[CHIP8_KEY_ID_0] = KEY_ZERO;
    keymap.map[CHIP8_KEY_ID_1] = KEY_ONE;
    keymap.map[CHIP8_KEY_ID_2] = KEY_TWO;
    keymap.map[CHIP8_KEY_ID_3] = KEY_THREE;
    keymap.map[CHIP8_KEY_ID_4] = KEY_FOUR;
    keymap.map[CHIP8_KEY_ID_5] = KEY_FIVE;
    keymap.map[CHIP8_KEY_ID_6] = KEY_SIX;
    keymap.map[CHIP8_KEY_ID_7] = KEY_SEVEN;
    keymap.map[CHIP8_KEY_ID_8] = KEY_EIGHT;
    keymap.map[CHIP8_KEY_ID_9] = KEY_NINE;
    keymap.map[CHIP8_KEY_ID_A] = KEY_Q;
    keymap.map[CHIP8_KEY_ID_B] = KEY_W;
    keymap.map[CHIP8_KEY_ID_C] = KEY_E;
    keymap.map[CHIP8_KEY_ID_D] = KEY_A;
    keymap.map[CHIP8_KEY_ID_E] = KEY_S;
    keymap.map[CHIP8_KEY_ID_F] = KEY_D;
}

static void keyboard_logic(void)
{
    uint32_t key_pressed = GetKeyPressed();
    if(key_pressed != 0)
    {

    }
}