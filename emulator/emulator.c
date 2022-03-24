#include "emulator.h"

#define REG_UNMAP_BOOT_ROM 0xFF50

//TODO: JESUS FUCKING CHRIST REMOVE DIS ILLEGAL SINNERY
const u8 boot_rom[256] = {
        0x31, 0xfe, 0xff, 0xaf, 0x21, 0xff, 0x9f, 0x32, 0xcb, 0x7c, 0x20, 0xfb, 0x21, 0x26, 0xff, 0xe,
        0x11, 0x3e, 0x80, 0x32, 0xe2, 0xc, 0x3e, 0xf3, 0xe2, 0x32, 0x3e, 0x77, 0x77, 0x3e, 0xfc, 0xe0,
        0x47, 0x11, 0x4, 0x1, 0x21, 0x10, 0x80, 0x1a, 0xcd, 0x95, 0x0, 0xcd, 0x96, 0x0, 0x13, 0x7b,
        0xfe, 0x34, 0x20, 0xf3, 0x11, 0xd8, 0x0, 0x6, 0x8, 0x1a, 0x13, 0x22, 0x23, 0x5, 0x20, 0xf9,
        0x3e, 0x19, 0xea, 0x10, 0x99, 0x21, 0x2f, 0x99, 0xe, 0xc, 0x3d, 0x28, 0x8, 0x32, 0xd, 0x20,
        0xf9, 0x2e, 0xf, 0x18, 0xf3, 0x67, 0x3e, 0x64, 0x57, 0xe0, 0x42, 0x3e, 0x91, 0xe0, 0x40, 0x4,
        0x1e, 0x2, 0xe, 0xc, 0xf0, 0x44, 0xfe, 0x90, 0x20, 0xfa, 0xd, 0x20, 0xf7, 0x1d, 0x20, 0xf2,
        0xe, 0x13, 0x24, 0x7c, 0x1e, 0x83, 0xfe, 0x62, 0x28, 0x6, 0x1e, 0xc1, 0xfe, 0x64, 0x20, 0x6,
        0x7b, 0xe2, 0xc, 0x3e, 0x87, 0xe2, 0xf0, 0x42, 0x90, 0xe0, 0x42, 0x15, 0x20, 0xd2, 0x5, 0x20,
        0x4f, 0x16, 0x20, 0x18, 0xcb, 0x4f, 0x6, 0x4, 0xc5, 0xcb, 0x11, 0x17, 0xc1, 0xcb, 0x11, 0x17,
        0x5, 0x20, 0xf5, 0x22, 0x23, 0x22, 0x23, 0xc9, 0xce, 0xed, 0x66, 0x66, 0xcc, 0xd, 0x0, 0xb,
        0x3, 0x73, 0x0, 0x83, 0x0, 0xc, 0x0, 0xd, 0x0, 0x8, 0x11, 0x1f, 0x88, 0x89, 0x0, 0xe,
        0xdc, 0xcc, 0x6e, 0xe6, 0xdd, 0xdd, 0xd9, 0x99, 0xbb, 0xbb, 0x67, 0x63, 0x6e, 0xe, 0xec, 0xcc,
        0xdd, 0xdc, 0x99, 0x9f, 0xbb, 0xb9, 0x33, 0x3e, 0x3c, 0x42, 0xb9, 0xa5, 0xb9, 0xa5, 0x42, 0x3c,
        0x21, 0x4, 0x1, 0x11, 0xa8, 0x0, 0x1a, 0x13, 0xbe, 0x20, 0xfe, 0x23, 0x7d, 0xfe, 0x34, 0x20,
        0xf5, 0x6, 0x19, 0x78, 0x86, 0x23, 0x5, 0x20, 0xfb, 0x86, 0x20, 0xfe, 0x3e, 0x1, 0xe0, 0x50,
};


u8 emulator_echo_ram_read(void *_bus, u16 addr, u16 abs_addr){
    Bus *bus = (Bus*)_bus;
    return bus_read(bus,0xC000+addr);
}

void emulator_echo_ram_write(void *_bus, u16 addr, u16 abs_addr, u8 val){
    Bus *bus = (Bus*)_bus;
    bus_write(bus,0xC000+addr,val);
}

u8 emulator_boot_rom_wrapper_read(void *_c, u16 addr, u16 abs_addr){
    if(addr < 0x100)
        return boot_rom[addr];

    return cartridge_bus_read(_c,addr,abs_addr);
}


void emulator_unmap_boot_rom(Emulator *emulator){
    log_debug("unmapping boot rom");
    bus_unmap(emulator->bus,0x0000,0x3FFF);
    bus_map(emulator->bus,0x0000,0x3FFF,emulator->cartridge,cartridge_bus_read,cartridge_bus_write);
}

u8 emulator_ioreg_bus_read(void *_emulator, u16 addr, u16 abs_addr){
    Emulator *e = (Emulator*)_emulator;
    switch (abs_addr) {
        case 0xFF00:{
            u8 d = e->io.joypad;
            if(e->io.joypad&0x2){
                //action buttons selected
                d |= ((e->input_state & INPUT_A)>0 )<<0;
                d |= ((e->input_state & INPUT_B)>0 )<<1;
                d |= ((e->input_state & INPUT_SELECT)>0 )<<2;
                d |= ((e->input_state & INPUT_START)>0 )<<3;
                return d;
            }else if(e->io.joypad&0x2){
                //direction buttons selected
                d |= ((e->input_state & INPUT_RIGHT)>0 )<<0;
                d |= ((e->input_state & INPUT_LEFT)>0 )<<1;
                d |= ((e->input_state & INPUT_UP)>0 )<<2;
                d |= ((e->input_state & INPUT_DOWN)>0 )<<3;
            } else{
                log_error("returning no input because no mode is selected");
            }
            return d;
        }
        case PPU_REG_LCD_CONTROL:
        case PPU_REG_LCD_STATUS:
        case PPU_REG_SCY:
        case PPU_REG_SCX:
        case PPU_REG_LY:
        case PPU_REG_LYC:
        case PPU_REG_WX:
        case PPU_REG_WY:
        case PPU_REG_BGP:
        case PPU_REG_OBP0:
        case PPU_REG_OBP1:
        case PPU_REG_DMA:
        {
            return ppu_register_read(e->ppu,addr,abs_addr);
        }
        case REG_UNMAP_BOOT_ROM: return UNDEFINED_U8; //TODO: is this ever read?
        default:{
            log_error("unimplemented io read to %04x", abs_addr);
            return UNDEFINED_U8;
        }
    }
}

void emulator_ioreg_bus_write(void *_emulator, u16 addr, u16 abs_addr, u8 val){
    Emulator *e = (Emulator*)_emulator;
    switch (abs_addr) {
        case 0xFF00:{
            //only write to select bits
            e->io.joypad = val&0x30;
            return;
        }

        case PPU_REG_LCD_CONTROL:
        case PPU_REG_LCD_STATUS:
        case PPU_REG_SCY:
        case PPU_REG_SCX:
        case PPU_REG_LY:
        case PPU_REG_LYC:
        case PPU_REG_WX:
        case PPU_REG_WY:
        case PPU_REG_BGP:
        case PPU_REG_OBP0:
        case PPU_REG_OBP1:
        case PPU_REG_DMA:
        {
            ppu_register_write(e->ppu,addr,abs_addr,val);
            break;
        }
        case REG_UNMAP_BOOT_ROM: {
            if(val)
                emulator_unmap_boot_rom(e);
            break;
        }
        default:{
            log_error("unimplemented io write to %04x", abs_addr);
        }
    }
}

void emulator_map_memory(Emulator *emu){
    //0000	3FFF	16 KiB ROM bank 00	From cartridge, usually a fixed bank. $0~$100 mapped to boot-rom initially
    bus_map(emu->bus,0x0000,0x3FFF,emu->cartridge,emulator_boot_rom_wrapper_read,cartridge_bus_write);

    //4000	7FFF	16 KiB ROM Bank 01~NN	From cartridge, switchable bank via mapper (if any)
    //TODO

    //8000	9FFF	8 KiB Video RAM (VRAM)	In CGB mode, switchable bank 0/1
    bus_map(emu->bus, 0x8000, 0x9FFF, emu->ppu, vram_bus_read, vram_bus_write);

    //A000	BFFF	8 KiB External RAM	From cartridge, switchable bank if any
    //TODO

    //C000	CFFF	4 KiB Work RAM (WRAM)
    bus_map(emu->bus, 0xC000, 0xCFFF, emu->workram_1, workram_bus_read, workram_bus_write);

    //D000	DFFF	4 KiB Work RAM (WRAM)	In CGB mode, switchable bank 1~7
    bus_map(emu->bus, 0xD000, 0xDFFF, emu->workram_2, workram_bus_read, workram_bus_write);

    //E000	FDFF	Mirror of C000~DDFF (ECHO RAM)	Nintendo says use of this area is prohibited.
    bus_map(emu->bus, 0xE000, 0xFDFF, emu->bus, emulator_echo_ram_read, emulator_echo_ram_write);

    //FE00	FE9F	Sprite attribute table (OAM)
    bus_map(emu->bus, 0xFE00, 0xFE9F, emu->ppu, oam_bus_read, oam_bus_write);

    //FEA0	FEFF	Not Usable	Nintendo says use of this area is prohibited
    //this area is simply not mapped lol

    //FF00	FF7F	I/O Registers
    bus_map(emu->bus, 0xFF00,0xFF7F, emu,emulator_ioreg_bus_read, emulator_ioreg_bus_write);

    //FF80	FFFE	High RAM (HRAM)
    bus_map(emu->bus,0xff80,0xfffe,emu->highram,highram_bus_read,highram_bus_write);

    //FFFF	FFFF	Interrupt Enable register (IE)
    //TODO

}

Emulator *emulator_new(Cartridge *cartridge){
    Emulator *emu = malloc(sizeof(Emulator));
    emu->bus = bus_new();
    emu->cpu = cpu_new(emu->bus);
    emu->ppu = ppu_new();
    emu->workram_1 = workram_new();
    emu->workram_2 = workram_new();
    emu->cartridge = cartridge;
    emu->highram = highram_new();

    emulator_map_memory(emu);
    return emu;
}

void emulator_close(Emulator *emu){
    workram_close(emu->workram_1);
    workram_close(emu->workram_2);
    bus_close(emu->bus);
    highram_close(emu->highram);
    cpu_close(emu->cpu);
    ppu_close(emu->ppu);
    //don't free cartridge!
    free(emu);
}

void emulator_clock(Emulator *emulator, u8 inputs){
    emulator->input_state = inputs;
    cpu_clock(emulator->cpu);
    ppu_clock(emulator->ppu);
}