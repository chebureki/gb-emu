#include "../common/datatypes.h"

#include <stdio.h>

#ifndef GB_EMU_CARTRIDGE_H
#define GB_EMU_CARTRIDGE_H

#define ROM_BANK_SIZE 0x4000
#define CARTRIDGE_TITLE_SIZE 11

typedef struct {
    u8 *data;
    int rom_size;
    int banks_amount;

    u8 enabled_ram;
    int selected_rom_bank;
    u8 **extra_ram;
    int extra_ram_size;
    char title[CARTRIDGE_TITLE_SIZE];
}Cartridge;

//Cartridge *cartridge_from_buffer(u8 *buf, size_t buf_length);
Cartridge *cartridge_from_file(char *file_path);

void cartridge_close(Cartridge *cartridge);

u8 cartridge_bank0_bus_read(void *_c, u16 addr, u16 abs_addr);
void cartridge_bank0_bus_write(void *_c, u16 addr, u16 abs_addr, u8 val);

u8 cartridge_bankn_bus_read(void *_c, u16 addr, u16 abs_addr);
void cartridge_bankn_bus_write(void *_c, u16 addr, u16 abs_addr, u8 val);

u8 cartridge_external_ram_bus_read(void *_c, u16 addr, u16 abs_addr);
void cartridge_external_ram_bus_write(void *_c, u16 addr, u16 abs_addr, u8 val);

#endif //GB_EMU_CARTRIDGE_H
