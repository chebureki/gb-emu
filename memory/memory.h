#include "../bus/bus.h"

#ifndef GB_EMU_MEMORY_H
#define GB_EMU_MEMORY_H

#define WORKRAMSIZE 4096
typedef struct {
    u8 memory[WORKRAMSIZE];
}WorkRAM;

WorkRAM *workram_new();
void workram_close(WorkRAM *wr);

u8 workram_bus_read(void *_wr, u16 addr, u16 abs_addr);
void workram_bus_write(void *_wr, u16 addr, u16 abs_addr, u8 val);

#define HIGHRAMSIZE 0x7f
typedef struct {
    u8 memory[HIGHRAMSIZE];
}HighRAM;

HighRAM *highram_new();
void highram_close(HighRAM *hr);

u8 highram_bus_read(void *_hr, u16 addr, u16 abs_addr);
void highram_bus_write(void *_hr, u16 addr, u16 abs_addr, u8 val);

#endif //GB_EMU_MEMORY_H
