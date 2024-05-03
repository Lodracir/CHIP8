/////////////////////////////////////////////////
/// Includes
/////////////////////////////////////////////////

#include "CHIP8.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

/////////////////////////////////////////////////
/// Defines
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Static variables
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Prototype static functions
/////////////////////////////////////////////////

static bool move_character_set_to_virtual_ram(chip8_t *chip);
static void move_data_to_virtual_ram(chip8_t *chip, uint8_t *buff, uint32_t size);
static bool chip_draw_sprite(chip8_t *chip, uint16_t x, uint16_t y, uint8_t *sprite, uint32_t num);
static void chip_screen_clean(chip8_t *chip);
static chip8_error_t chip_set_pixel(chip8_t *chip, uint16_t x, uint16_t y);
static chip8_error_t chip_memory_write(chip8_t *chip, uint16_t index, uint8_t data);
static chip8_error_t chip_memory_read(chip8_t *chip, uint16_t index, uint8_t *data);
static chip8_error_t chip_stack_push(chip8_t *chip, uint16_t data);
static chip8_error_t chip_stack_pop(chip8_t *chip, uint16_t *pdata);
static chip8_error_t chip_execute_opcode(chip8_t *chip, uint16_t opcode);
static uint16_t chip_get_opcode(chip8_t *chip, uint16_t index);

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
    if( move_character_set_to_virtual_ram(chip) == false )
    {
        return CHIP8_ERROR_INIT;
    }

    /// Load program to virtual memory and set PC to 0x200
    move_data_to_virtual_ram(chip, program_buff, size);
    chip->registers.PC = CHIP8_PROGRAM_START_ADDR;

    return CHIP8_ERROR_NO;
}

chip8_error_t CHIP8_Run(chip8_t *chip)
{
    uint16_t opcode = chip_get_opcode(chip, chip->registers.PC);
    chip->registers.PC += 2;

    printf("INFO: OPCODE %04X | PC %04X\n", opcode, chip->registers.PC);

    chip_execute_opcode(chip, opcode);
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

uint8_t CHIP8_GetDelayTimer(chip8_t *chip)
{
    return chip->registers.delayTimer;
}

uint8_t CHIP8_GetSoundTimer(chip8_t *chip)
{
    return chip->registers.soundTimer;
}

void CHIP8_DecreaseDelayTimer(chip8_t *chip)
{
    chip->registers.delayTimer--;
}

void CHIP8_ResetSoundTimer(chip8_t *chip)
{
    chip->registers.soundTimer = 0;
}

/////////////////////////////////////////////////
/// Prototype static functions
/////////////////////////////////////////////////

static bool move_character_set_to_virtual_ram(chip8_t *chip)
{
    const char char_set_path[] = "./bin/char_set.bin";
    uint8_t *buff = NULL;
    uint16_t size = 0;

    FILE *f = fopen(char_set_path, "rb");
    if (!f)
    {
        puts("Failed to open file.");
        return false;
    }

    /// Get file size
    fseek(f, 0, SEEK_END);
    size = ftell(f);
    fseek(f, 0, SEEK_SET);

    /// Allocate memory
    buff = (uint8_t *)malloc(size);
    if(buff == NULL)
    {
        puts("Failed to allocate memory");
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

    for(uint32_t itr = 0; itr < size; itr++)
    {
        chip->memory[itr] = buff[itr];
    }

    /// Clean memory and free it
    memset(buff, 0, size);
    free(buff);

    return true;
}

static void move_data_to_virtual_ram(chip8_t *chip, uint8_t *buff, uint32_t size)
{
    for(uint32_t itr = 0; itr < size; itr++)
    {
        chip->memory[itr + CHIP8_PROGRAM_START_ADDR] = buff[itr];
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
static chip8_error_t chip_memory_write(chip8_t *chip, uint16_t index, uint8_t data)
{
    if(index > CHIP8_MEMORY_SIZE)
    {
        return CHIP8_ERROR_INVALID_INDEX;
    }

    chip->memory[index] = data;
    return CHIP8_ERROR_NO;
}

static chip8_error_t chip_memory_read(chip8_t *chip, uint16_t index, uint8_t *data)
{
    if(index > CHIP8_MEMORY_SIZE)
    {
        return CHIP8_ERROR_INVALID_INDEX;
    }

    (*data) = chip->memory[index];
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

static chip8_error_t chip_execute_opcode(chip8_t *chip, uint16_t opcode)
{
    ///Local variables
    uint8_t code = (opcode & 0xF000) >> 12;
    uint8_t x, y, temp_code, hundreds, tens, units;
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
                case 0x9E: /// Skip next instruction if key with the value of Vx is pressed.
                    ///TODO: Implement keyboard logic
                    break;

                case 0xA1: /// Skip next instruction if key with the value of Vx is not pressed.
                    ///TODO: Implement keyboard logic
                    break;

                default:
                    err = CHIP8_ERROR_INVALID_OPCODE;
                    break;
            }
        }

        case 0xF:
        {
            temp_code = opcode & 0x00FF;
            switch(temp_code)
            {
                case 0x07:  /// Set Vx = delay timer value.
                    x = (opcode & 0x0F00) >> 8;
                    chip->registers.V[x] = chip->registers.delayTimer;
                    break;

                case 0x0A:  /// Wait for a key press, store the value of the key in Vx.
                    ///TODO: Implement keyboard logic
                    break;

                case 0x15:  /// Set delay timer = Vx.
                    x = (opcode & 0x0F00) >> 8;
                    chip->registers.delayTimer = chip->registers.V[x];
                    break;

                case 0x18:  /// Set sound timer = Vx.
                    x = (opcode & 0x0F00) >> 8;
                    chip->registers.soundTimer = chip->registers.V[x];
                    break;

                case 0x1E:  /// Set I = I + Vx.
                    x = (opcode & 0x0F00) >> 8;
                    chip->registers.I = chip->registers.I + chip->registers.V[x];
                    break;

                case  0x29: /// Set I = location of sprite for digit Vx
                    x = (opcode & 0x0F00) >> 8;
                    chip->registers.I = chip->registers.V[x] * 5;
                    break;

                case 0x33:  /// Store BCD representation of Vx in memory locations I, I+1, and I+2.
                    x = (opcode & 0x0F00) >> 8;
                    hundreds = chip->registers.V[x] / 100;
                    tens = chip->registers.V[x] / 10 % 10;
                    units = chip->registers.V[x] % 10;
                    chip_memory_write(chip, chip->registers.I, hundreds);
                    chip_memory_write(chip, chip->registers.I+1, hundreds);
                    chip_memory_write(chip, chip->registers.I+2, hundreds);
                    break;

                case 0x55:  /// Store registers V0 through Vx in memory starting at location I.
                    x = (opcode & 0x0F00) >> 8;
                    for(uint32_t itr = 0; itr < x; itr++)
                    {
                        chip_memory_write(chip, chip->registers.I+itr, chip->registers.V[itr]);
                    }
                    break;

                case 0x65:  /// Read registers V0 through Vx from memory starting at location I.
                    x = (opcode & 0x0F00) >> 8;
                    for(uint32_t itr = 0; itr < x; itr++)
                    {
                        chip_memory_read(chip, chip->registers.I+itr, &chip->registers.V[itr]);
                    }
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

static uint16_t chip_get_opcode(chip8_t *chip, uint16_t index)
{
    uint8_t byte[2] = {0};

    /// Get U16 value
    chip_memory_read(chip, index, &byte[0]);
    chip_memory_read(chip, index + 1, &byte[1]);

    return (uint16_t)( byte[0] << 8 | byte[1] );
}