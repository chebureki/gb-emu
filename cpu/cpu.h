#include "../common/datatypes.h"
#include "../bus/bus.h"

#ifndef GB_EMU_CPU_H
#define GB_EMU_CPU_H

#define LO_GET(a) (u8)((a&0xff00)>>8)
#define HI_GET(a) (u8)(a&0x00ff)

#define LO_SET(a,v) a = (a&0x00ff) | (((u16)v)<<8)
#define HI_SET(a,v) a = (a&0xff00) | ((u16)v)

#define CPU_CARRYFLAG_POS 4
#define CPU_HALFCARRYFLAG_POS 5
#define CPU_SUBTRACTIONFLAG_POS 6
#define CPU_ZEROFLAG_POS 7

#define CPU_CARRYFLAG 1<<CPU_CARRYFLAG_POS
#define CPU_HALFCARRYFLAG 1<<CPU_HALFCARRYFLAG_POS
#define CPU_SUBTRACTIONFLAG 1<<CPU_SUBTRACTIONFLAG_POS
#define CPU_ZEROFLAG 1<<CPU_ZEROFLAG_POS

typedef struct {
    u16 extra_cycles;

    u16 AF;
    u16 BC;
    u16 DE;
    u16 HL;
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