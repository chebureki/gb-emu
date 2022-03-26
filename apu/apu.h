#include "../bus/bus.h"
#include "../common/log.h"

#ifndef GB_EMU_APU_H
#define GB_EMU_APU_H

#define APU_REG_NR10 0xFF10
#define APU_REG_NR11 0xFF11
#define APU_REG_NR12 0xFF12
#define APU_REG_NR13 0xFF13
#define APU_REG_NR14 0xFF14

#define APU_REG_NR21 0xFF16
#define APU_REG_NR22 0xFF17
#define APU_REG_NR23 0xFF18
#define APU_REG_NR24 0xFF19

#define APU_REG_NR30 0xFF1A
#define APU_REG_NR31 0xFF1B
#define APU_REG_NR32 0xFF1C
#define APU_REG_NR33 0xFF1D
#define APU_REG_NR34 0xFF1E

#define APU_WAVEPATTERN_RAM_FROM 0xff30
#define APU_WAVEPATTERN_RAM_TO 0xff3F

#define APU_REG_NR50 0xFF24
#define APU_REG_NR51 0xFF25
#define APU_REG_NR52 0xFF26

typedef struct {
    u8 wave_pattern_ram[16];
}APU;

APU *apu_new();

void apu_close(APU *apu);

u8 apu_wave_ram_bus_read(void *_a, u16 addr, u16 abs_addr);
void apu_wave_ram_bus_write(void *_a, u16 addr, u16 abs_addr, u8 val);

u8 apu_reg_bus_read(void *_a, u16 addr, u16 abs_addr);
void apu_reg_bus_write(void *_a, u16 addr, u16 abs_addr, u8 val);

#endif //GB_EMU_APU_H
