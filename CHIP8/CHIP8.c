/////////////////////////////////////////////////
/// Includes
/////////////////////////////////////////////////

#include "CHIP8.h"
#include <string.h>

/////////////////////////////////////////////////
/// Static variables
/////////////////////////////////////////////////

static const uint8_t chip8_default_character_set[] = {
        0xF0,0x90, 0x90, 0x90, 0xF0,    // 0
        0x20,0x60, 0x20, 0x20, 0x70,    // 1
        0xF0,0x10, 0xF0, 0x80, 0xF0,   // 2
        0xF0,0x10, 0xF0, 0x10, 0xF0,   // 3
        0x90,0x90, 0xF0, 0x10, 0x10,   // 4
        0xF0,0x80, 0xF0, 0x10, 0xF0,   // 5
        0xF0,0x80, 0xF0, 0x90, 0xF0,   // 6
        0xF0,0x10, 0x20, 0x40, 0x40,   // 7
        0xF0,0x90, 0xF0, 0x90, 0xF0,   // 8
        0xF0,0x90, 0xF0, 0x10, 0xF0,   // 9
        0xF0,0x90, 0xF0, 0x90, 0x90,   // A
        0xE0,0x90, 0xE0, 0x90, 0xE0,   // B
        0xF0,0x80, 0x80, 0x80, 0xF0,   // C
        0xE0,0x90, 0x90, 0x90, 0xE0,   // D
        0xF0,0x80, 0xF0, 0x80, 0xF0,   // E
        0xF0,0x80, 0xF0, 0x80,0x80  // F
};

/////////////////////////////////////////////////
/// Prototype static functions
/////////////////////////////////////////////////

static void move_character_set_to_virtual_ram(chip8_t *chip);

/////////////////////////////////////////////////
/// Public functions
/////////////////////////////////////////////////

chip8_error_t CHIP8_Init(chip8_t *chip, uint8_t *program_buff, uint32_t size)
{
    if(chip == NULL)
    {
        return CHIP8_ERROR_INIT;
    }

    /// Clean CHIP8 structure data
    memset((void *)chip, 0, sizeof(chip8_t));

    /// Copy default character set to CHIP8 virtual memory
    move_character_set_to_virtual_ram(chip);

    return CHIP8_ERROR_NO;
}

/////////////////////////////////////////////////
/// Prototype static functions
/////////////////////////////////////////////////

static void move_character_set_to_virtual_ram(chip8_t *chip)
{
    for(uint32_t itr = 0; itr < sizeof(chip8_default_character_set); itr++)
    {
        chip->memory[itr] = chip8_default_character_set[itr];
    }
}
