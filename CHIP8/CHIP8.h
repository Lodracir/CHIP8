#ifndef CHIP8_CHIP8_H
#define CHIP8_CHIP8_H

/////////////////////////////////////////////////
/// Includes
/////////////////////////////////////////////////

#include <stdint.h>
#include <stdbool.h>

/////////////////////////////////////////////////
/// Defines
/////////////////////////////////////////////////

#define CHIP8_DATA_REGISTERS_TOTAL 16
#define CHIP8_STACK_DEPTH_TOTAL 16
#define CHIP8_MEMORY_SIZE 4096

#define CHIP8_WIDTH_SCREEN  64
#define CHIP8_HEIGHT_SCREEN 32

/////////////////////////////////////////////////
/// Typedef enumerations
/////////////////////////////////////////////////

typedef enum CHIP8_ERROR_TYPE
{
    CHIP8_ERROR_NO = 0,
    CHIP8_ERROR_INIT,
    CHIP8_ERROR_INVALID_INDEX,
    CHIP8_ERROR_STACK_FULL,
    CHIP8_ERROR_STACK_EMPTY,
    CHIP8_ERROR_INVALID_KEYBOARD_INDEX,
    CHIP8_ERROR_DATA_OVERSIZE,

} chip8_error_t;

typedef enum CHIP8_KEYBOARD_INDEX_TYPE
{
    CHIP8_KEY_ID_0 = 0,
    CHIP8_KEY_ID_1,
    CHIP8_KEY_ID_2,
    CHIP8_KEY_ID_3,
    CHIP8_KEY_ID_4,
    CHIP8_KEY_ID_5,
    CHIP8_KEY_ID_6,
    CHIP8_KEY_ID_7,
    CHIP8_KEY_ID_8,
    CHIP8_KEY_ID_9,
    CHIP8_KEY_ID_A,
    CHIP8_KEY_ID_B,
    CHIP8_KEY_ID_C,
    CHIP8_KEY_ID_D,
    CHIP8_KEY_ID_E,
    CHIP8_KEY_ID_F,

    CHIP8_KEY_ID_TOTAL
} chip8_key_id_t;

/////////////////////////////////////////////////
/// Typedef variables
/////////////////////////////////////////////////

typedef uint8_t chip8_mem_t[CHIP8_MEMORY_SIZE];
typedef uint16_t chip8_stack_t[CHIP8_STACK_DEPTH_TOTAL];
typedef bool chip8_keyboard_t[CHIP8_KEY_ID_TOTAL];

/////////////////////////////////////////////////
/// Typedef structures
/////////////////////////////////////////////////

typedef struct CHIP8_REGISTERS_STRUCT
{
    uint8_t     V[CHIP8_DATA_REGISTERS_TOTAL];
    uint16_t    I;
    uint8_t     delayTimer;
    uint8_t     soundTimer;
    uint16_t    PC; /// Program Counter
    uint8_t     SP; /// Stack Pointer

} chip8_registers_t;

typedef struct CHIP8_SCREEN_STRUCT
{
    bool buffer[CHIP8_HEIGHT_SCREEN][CHIP8_WIDTH_SCREEN];
} chip8_screen_t;

typedef struct CHIP8_STRUCT
{
    chip8_mem_t         memory;
    chip8_registers_t   registers;
    chip8_stack_t       stack;
    chip8_screen_t      screen;
    chip8_keyboard_t    key;
} chip8_t;

/////////////////////////////////////////////////
/// Public Prototype Functions
/////////////////////////////////////////////////

chip8_error_t CHIP8_Init(chip8_t *chip, uint8_t *program_buff, uint32_t size);

#endif //CHIP8_CHIP8_H
