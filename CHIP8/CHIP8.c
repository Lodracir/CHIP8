/////////////////////////////////////////////////
/// Includes
/////////////////////////////////////////////////

#include "CHIP8.h"
#include <string.h>
#include <stdlib.h>
#include <time.h>

/////////////////////////////////////////////////
/// Static typedef structures
/////////////////////////////////////////////////



/////////////////////////////////////////////////
/// Static variables
/////////////////////////////////////////////////

static const uint8_t chip8_default_character_set[] = {
        0xF0,0x90, 0x90, 0x90, 0xF0,    // 0
        0x20,0x60, 0x20, 0x20, 0x70,    // 1
        0xF0,0x10, 0xF0, 0x80, 0xF0,    // 2
        0xF0,0x10, 0xF0, 0x10, 0xF0,    // 3
        0x90,0x90, 0xF0, 0x10, 0x10,    // 4
        0xF0,0x80, 0xF0, 0x10, 0xF0,    // 5
        0xF0,0x80, 0xF0, 0x90, 0xF0,    // 6
        0xF0,0x10, 0x20, 0x40, 0x40,    // 7
        0xF0,0x90, 0xF0, 0x90, 0xF0,    // 8
        0xF0,0x90, 0xF0, 0x10, 0xF0,    // 9
        0xF0,0x90, 0xF0, 0x90, 0x90,    // A
        0xE0,0x90, 0xE0, 0x90, 0xE0,    // B
        0xF0,0x80, 0x80, 0x80, 0xF0,    // C
        0xE0,0x90, 0x90, 0x90, 0xE0,    // D
        0xF0,0x80, 0xF0, 0x80, 0xF0,    // E
        0xF0,0x80, 0xF0, 0x80, 0x80     // F
};

/////////////////////////////////////////////////
/// Prototype static functions
/////////////////////////////////////////////////

static void move_character_set_to_virtual_ram(chip8_t *chip);
static bool chip_draw_sprite(chip8_t *chip, uint16_t x, uint16_t y, uint8_t *sprite, uint32_t num);
static void chip_screen_clean(chip8_t *chip);
static chip8_error_t chip_set_pixel(chip8_t *chip, uint16_t x, uint16_t y);
static chip8_error_t chip_stack_push(chip8_t *chip, uint16_t data);
static chip8_error_t chip_stack_pop(chip8_t *chip, uint16_t *pdata);
static chip8_error_t chip_execute_opcode(chip8_t *chip, uint8_t opcode);

/////////////////////////////////////////////////
/// Public functions
/////////////////////////////////////////////////

chip8_error_t CHIP8_Init(chip8_t *chip, chip8_keymap_t *keymap, uint8_t *program_buff, uint32_t size)
{
    if(chip == NULL)
    {
        return CHIP8_ERROR_INIT;
    }

    /// Clean CHIP8 structure data
    memset((void *)chip, 0, sizeof(chip8_t));

    /// Copy default character set to CHIP8 virtual memory
    move_character_set_to_virtual_ram(chip);

    /// Load program to virtual memory and set PC to 0x200
    chip->registers.PC = CHIP8_PROGRAM_START_ADDR;

    return CHIP8_ERROR_NO;
}

bool CHIP8_DrawSprite(chip8_t *chip, uint16_t x, uint16_t y, uint8_t *sprite, uint32_t num)
{
    return chip_draw_sprite(chip, x, y, sprite, num);
}

bool CHIP8_IsPixelSet(chip8_t *chip, uint16_t x, uint16_t y)
{
    if(( x < 0 && CHIP8_HEIGHT_SCREEN >= x ) && ( y < 0 && CHIP8_WIDTH_SCREEN >= y ))
    {
        return false;
    }

    return chip->screen.buffer[y][x];
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

static bool chip_draw_sprite(chip8_t *chip, uint16_t x, uint16_t y, uint8_t *sprite, uint32_t num)
{
    //Local variables
    bool pixel_collision = false;

    for(uint32_t ly = 0; ly < num; ly++)
    {
        uint8_t c = sprite[ly];
        for(uint32_t lx = 0; lx < 8; lx++)
        {
            if( (c & (0b10000000 >> lx)) == 0 )
            {
                continue;
            }

            if(chip->screen.buffer[(ly+y)%CHIP8_HEIGHT_SCREEN][(lx+x)%CHIP8_WIDTH_SCREEN] == true)
            {
                pixel_collision = true;
            }

            chip->screen.buffer[(ly+y)%CHIP8_HEIGHT_SCREEN][(lx+x)%CHIP8_WIDTH_SCREEN] ^= true;
        }
    }

    return pixel_collision;
}

static void chip_screen_clean(chip8_t *chip)
{
    memset((void *)chip->screen.buffer, 0, sizeof(chip->screen.buffer));
}

static chip8_error_t chip_set_pixel(chip8_t *chip, uint16_t x, uint16_t y)
{
    if((y >= CHIP8_HEIGHT_SCREEN && y < 0) && (x >= CHIP8_WIDTH_SCREEN) && x < 0)
    {
        return CHIP8_ERROR_SCREEN_INVALID_COORDINATES;
    }

    chip->screen.buffer[y][x] = true;
    return CHIP8_ERROR_NO;
}

static chip8_error_t chip_stack_push(chip8_t *chip, uint16_t data)
{
    if( chip->registers.SP >= CHIP8_STACK_DEPTH_TOTAL )
    {
        return CHIP8_ERROR_STACK_FULL;
    }

    chip->stack[chip->registers.SP] = data;
    chip->registers.SP++;

    return CHIP8_ERROR_NO;
}

static chip8_error_t chip_stack_pop(chip8_t *chip, uint16_t *pdata)
{
    if( chip->registers.SP <= 0 )
    {
        return CHIP8_ERROR_STACK_EMPTY;
    }

    chip->registers.SP--;
    *(pdata) = chip->stack[chip->registers.SP];

    return CHIP8_ERROR_NO;
}

static chip8_error_t chip_execute_opcode(chip8_t *chip, uint8_t opcode)
{
    ///Local variables
    uint8_t code = (opcode & 0x0F00) >> 8;
    uint8_t x, y, temp_code;
    uint16_t n = 0, kk = 0;
    uint8_t *sprite = NULL;
    chip8_error_t err = CHIP8_ERROR_NO;

    switch(code)
    {
        case 0x0:
            switch(opcode)
            {
                case 0x00E0: /// Clear screen.
                    chip_screen_clean(chip);
                    break;

                case 0x00EE: /// Return from subroutine.
                    err = chip_stack_pop(chip, &chip->registers.PC);
                    break;

                default:
                    err = CHIP8_ERROR_INVALID_OPCODE;
                    break;
            }
            break;

        case 0x1:   /// Jump to address NNN.
            chip->registers.PC = opcode & 0x0FFF;
            break;

        case 0x2:   /// Call subroutine at NNN.
            chip_stack_push(chip, chip->registers.PC);
            chip->registers.PC = opcode & 0x0FFF;
            break;

        case 0x3:   /// Skip next instruction if Vx = NN.
            n = (opcode & 0x0F00) >> 8;
            kk = opcode & 0x00FF;
            if(chip->registers.V[n] == kk)
            {
                chip->registers.PC += 2;
            }
            break;

        case 0x4:   /// Skip next instruction if Vx != NN.
            n = (opcode & 0x0F00) >> 8;
            kk = opcode & 0x00FF;
            if(chip->registers.V[n] != kk)
            {
                chip->registers.PC += 2;
            }
            break;

        case 0x5:   /// Skip next instruction if Vx = Vy.
            x = (opcode & 0x0F00) >> 8;
            y = (opcode & 0x00F0) >> 4;
            if(chip->registers.V[x] == chip->registers.V[y])
            {
                chip->registers.PC += 2;
            }
            break;

        case 0x6:   /// Set Vx = NN.
            n = (opcode & 0x0F00) >> 8;
            kk = opcode & 0x00FF;
            chip->registers.V[n] = kk;
            break;

        case 0x7:   /// Set Vx = Vx + NN.
            n = (opcode & 0x0F00) >> 8;
            kk = opcode & 0x00FF;
            chip->registers.V[n] += kk;
            break;

        case 0x8:   /// 8XXX opcode instruction
            temp_code = opcode & 0x000F;
            switch(temp_code)
            {
                case 0x0:   /// Set Vx = Vy.
                    x = (opcode & 0x0F00) >> 8;
                    y = (opcode & 0x00F0) >> 4;
                    chip->registers.V[x] = chip->registers.V[y];
                    break;

                case 0x1:   /// Set Vx = Vx OR Vy.
                    x = (opcode & 0x0F00) >> 8;
                    y = (opcode & 0x00F0) >> 4;
                    chip->registers.V[x] |= chip->registers.V[y];
                    break;

                case 0x2:   /// Set Vx = Vx AND Vy.
                    x = (opcode & 0x0F00) >> 8;
                    y = (opcode & 0x00F0) >> 4;
                    chip->registers.V[x] &= chip->registers.V[y];
                    break;

                case 0x3:   /// Set Vx = Vx XOR Vy.
                    x = (opcode & 0x0F00) >> 8;
                    y = (opcode & 0x00F0) >> 4;
                    chip->registers.V[x] ^= chip->registers.V[y];
                    break;

                case 0x4:   /// Set Vx = Vx + Vy, set VF = carry.
                    x = (opcode & 0x0F00) >> 8;
                    y = (opcode & 0x00F0) >> 4;
                    temp_code = chip->registers.V[x] + chip->registers.V[y];
                    if(temp_code > 0xFF)
                    {
                        chip->registers.V[0xF] = 1;
                    }
                    else
                    {
                        chip->registers.V[0xF] = 0;
                    }
                    chip->registers.V[x] = temp_code;
                    break;

                case 0x5:   /// Set Vx = Vx - Vy, set VF = NOT borrow.
                    x = (opcode & 0x0F00) >> 8;
                    y = (opcode & 0x00F0) >> 4;
                    if(chip->registers.V[x] > chip->registers.V[y])
                    {
                        chip->registers.V[0xF] = 1;
                    }
                    else
                    {
                        chip->registers.V[0xF] = 0;
                    }
                    chip->registers.V[x] -= chip->registers.V[y];
                    break;

                case 0x6:   /// Set Vx = Vx SHR 1.
                    x = (opcode & 0x0F00) >> 8;
                    chip->registers.V[0xF] = chip->registers.V[x] & 0x1;
                    chip->registers.V[x] /= 2;
                    break;

                case 0x7:   /// Set Vx = Vy - Vx, set VF = NOT borrow.
                    x = (opcode & 0x0F00) >> 8;
                    y = (opcode & 0x00F0) >> 4;
                    if(chip->registers.V[y] > chip->registers.V[x])
                    {
                        chip->registers.V[0xF] = 1;
                    }
                    else
                    {
                        chip->registers.V[0xF] = 0;
                    }
                    chip->registers.V[x] = chip->registers.V[y] - chip->registers.V[x];
                    break;

                case 0xE:   /// Set Vx = Vx SHL 1.
                    x = (opcode & 0x0F00) >> 8;
                    chip->registers.V[0xF] = chip->registers.V[x] & 0x80;
                    chip->registers.V[x] *= 2;

                default:
                    err = CHIP8_ERROR_INVALID_OPCODE;
                    break;
            }

        case 0x9:  /// Skip next instruction if Vx != Vy.
            x = (opcode & 0x0F00) >> 8;
            y = (opcode & 0x00F0) >> 4;
            if(chip->registers.V[x] != chip->registers.V[y])
            {
                chip->registers.PC += 2;
            }
            break;

        case 0xA:   /// Set I = NNN
            chip->registers.I = opcode & 0x0FFF;
            break;

        case 0xB:   /// Jump to location NNN + V0.
            chip->registers.PC = (opcode & 0x0FFF) + chip->registers.V[0];
            break;

        case 0xC:   /// Set Vx = random byte AND NN.
            x = (opcode & 0x0F00) >> 8;
            kk = opcode & 0x00FF;
            srand(clock());
            chip->registers.V[x] = (rand() % 0xFF) & kk;
            break;

        case 0xD:   /// Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision.
            x = (opcode & 0x0F00) >> 8;
            y = (opcode & 0x00F0) >> 4;
            n = opcode & 0x000F;
            sprite = &chip->memory[chip->registers.I];
            chip->registers.V[0xF] = chip_draw_sprite(chip, chip->registers.V[x], chip->registers.V[y], sprite, n);
            break;

        case 0xE:
        {
            temp_code = opcode & 0x00FF;
            switch(temp_code)
            {
                case 0x9E: // Skip next instruction if key with the value of Vx is pressed.

                    break;

                default:
                    err = CHIP8_ERROR_INVALID_OPCODE;
                    break;
            }
        }

        default:
            err = CHIP8_ERROR_INVALID_OPCODE;
            break;
    }

    return err;
}
