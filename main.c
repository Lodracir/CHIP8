/////////////////////////////////////////////////
/// Includes
/////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "raylib.h"
#include "CHIP8/CHIP8.h"

#ifdef RAYGUI_IMPLEMENTATION
#include "raygui.h"
#endif

/////////////////////////////////////////////////
/// Defines
/////////////////////////////////////////////////

#define MAIN_WINDOW_NAME    "CHIP8"
#define MAIN_WINDOW_SCALE_FACTOR    10
#define MAIN_WINDOW_WIDTH   (CHIP8_WIDTH_SCREEN * MAIN_WINDOW_SCALE_FACTOR)
#define MAIN_WINDOW_HEIGHT  (CHIP8_HEIGHT_SCREEN * MAIN_WINDOW_SCALE_FACTOR)
#define MAIN_WINDOW_FPS     60

/////////////////////////////////////////////////
/// Local variables
/////////////////////////////////////////////////

static chip8_keymap_t keymap;
static uint8_t *buff;
static uint32_t size;

/////////////////////////////////////////////////
/// Local functions
/////////////////////////////////////////////////

static bool b_load_file(const char *filename);
static void keyboard_map(void);
static void keyboard_logic(void);

/////////////////////////////////////////////////
/// Main function
/////////////////////////////////////////////////

int main(int argc, char **argv)
{
    if( argc < 2 )
    {
        puts("You must provide a file to load.");
        return -1;
    }

    if( b_load_file(argv[1]) == false )
    {
        puts("Failed to load file");
        return -1;
    }

    ///Local variables
    chip8_t CHIP8;

    ///Map keyboard
    keyboard_map();

    ///Init CHIP8
    CHIP8_Init(&CHIP8, &keymap, buff, size);
    free(buff);

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

        if(CHIP8_GetDelayTimer(&CHIP8) > 0)
        {
            CHIP8_DecreaseDelayTimer(&CHIP8);
        }

        if(CHIP8_GetSoundTimer(&CHIP8) > 0)
        {
            puts("INFO: BEEP!");
            CHIP8_ResetSoundTimer(&CHIP8);
        }

        CHIP8_Run(&CHIP8);
    }

    CloseWindow();
    return 0;
}

static bool b_load_file(const char *filename) {
    printf("File name: %s\n", filename);

    FILE *f = fopen(filename, "rb");
    if (!f) {
        puts("Failed to open file.");
        return false;
    }

    /// Get file size
    fseek(f, 0, SEEK_END);
    size = ftell(f);
    fseek(f, 0, SEEK_SET);

    /// Allocate memory
    buff = (uint8_t *) malloc(size);
    if (buff == NULL)
    {
        puts("Failed to allocate memory.");
        return false;
    }

    memset(buff, 0, size);
    size_t res = fread(buff, size, 1, f);
    if(res != 1)
    {
        /// Clean memory and free it
        memset(buff, 0, size);
        free(buff);

        puts("Failed to read from file.");
        return false;
    }

    return true;
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