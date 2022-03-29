#include "../common/datatypes.h"
#include "../bus/bus.h"

#ifndef GB_EMU_CPU_H
#define GB_EMU_CPU_H


#define CPU_CARRYFLAG_POS 4
#define CPU_HALFCARRYFLAG_POS 5
#define CPU_SUBTRACTIONFLAG_POS 6
#define CPU_ZEROFLAG_POS 7

#define CPU_CARRYFLAG 1<<CPU_CARRYFLAG_POS
#define CPU_HALFCARRYFLAG 1<<CPU_HALFCARRYFLAG_POS
#define CPU_SUBTRACTIONFLAG 1<<CPU_SUBTRACTIONFLAG_POS
#define CPU_ZEROFLAG 1<<CPU_ZEROFLAG_POS

enum InterruptFlag{
    IF_VBLANK = 1<<0,
    IF_LCD_STAT = 1<<1,
    IF_TIMER = 1<<2,
    IF_SERIAL = 1<<3,
    IF_JOYPAD = 1<<4,
};

typedef struct {
    u16 extra_cycles;

    u8 IME; //interrupt master enable flag
    u8 IE; //interrupt enable
    u8 IF; //interrupt flags

    u8 A;
    u8 F;
    u8 B;
    u8 C;
    u8 D;
    u8 E;
    u8 H;
    u8 L;
    u16 SP;
    u16 PC;

    Bus *bus;
}CPU;

//returns cycles ran by instruction
int cpu_next_ins(CPU* cpu);

void cpu_clock(CPU *cpu);

CPU *cpu_new(Bus *bus);
void cpu_close(CPU *cpu);

#endif //GB_EMU_CPU_H