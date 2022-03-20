#include "../common/datatypes.h"

#include <stdio.h>

#ifndef GB_EMU_CARTRIDGE_H
#define GB_EMU_CARTRIDGE_H

#define CARTRIDGE_TITLE_SIZE 11

typedef struct {
    u8 *data;
    int rom_size;
    int banks_amount;
    char title[CARTRIDGE_TITLE_SIZE];
}Cartridge;

//Cartridge *cartridge_from_buffer(u8 *buf, size_t buf_length);
Cartridge *cartridge_from_file(char *file_path);

void cartridge_close(Cartridge *cartridge);

u8 cartridge_bus_read(void *_c, u16 addr, u16 abs_addr);
void cartridge_bus_write(void *_c, u16 addr, u16 abs_addr, u8 val);

#endif //GB_EMU_CARTRIDGE_H
