#include "../common/log.h"
#include "../bus/bus.h"
#include "../cpu/cpu.h"
#include "../memory/memory.h"
#include "../cartridge/cartridge.h"
#include "../ppu/ppu.h"
#include "../apu/apu.h"

#ifndef GB_EMU_EMULATOR_H
#define GB_EMU_EMULATOR_H

#define INPUT_DOWN 1<<0
#define INPUT_UP 1<<1
#define INPUT_LEFT 1<<2
#define INPUT_RIGHT 1<<3
#define INPUT_START 1<<4
#define INPUT_SELECT 1<<5
#define INPUT_B 1<<6
#define INPUT_A 1<<7

typedef struct {
    u8 joypad;
}IORegisters;

typedef struct {

    Bus *bus;
    CPU *cpu;
    PPU *ppu;
    APU *apu;
    WorkRAM *workram_1;
    WorkRAM *workram_2;
    HighRAM *highram;

    //we only have a ref, don't own it
    Cartridge *cartridge;

    //TODO: kinda cheap, buuuuut whatever
    u8 input_state;

    IORegisters io;
} Emulator;

//Emulator *emulator_new(u16 config_flags);
Emulator *emulator_new(Cartridge *cartridge);
void emulator_close(Emulator *emu);

void emulator_clock(Emulator *emulator, u8 inputs);

#endif //GB_EMU_EMULATOR_H
