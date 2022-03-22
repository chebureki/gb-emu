#include "../common/datatypes.h"

#ifndef GB_EMU_PPU_H
#define GB_EMU_PPU_H

#define VIDEO_WIDTH 160
#define VIDEO_HEIGHT 144

#define PPU_OAM_SEARCH_CLOCKS 20
#define PPU_PIXEL_TRANSFER_CLOCKS 43
#define PPU_HBLANK_CLOCKS 51

#define PPU_REG_LCD_CONTROL 0xFF40
#define PPU_REG_LCD_STATUS 0xFF41
#define PPU_REG_SCY 0xFF42
#define PPU_REG_SCX 0xFF43
#define PPU_REG_LY 0xFF44
#define PPU_REG_LYC 0xFF45
#define PPU_REG_WY 0xFF4A
#define PPU_REG_WX 0xFF4B
#define PPU_REG_BGP 0xFF47
#define PPU_REG_OBP0 0xFF48
#define PPU_REG_OBP1 0xFF49
#define PPU_REG_DMA 0xFF46

//V-Blank of 10 Lines i.e.:
#define PPU_VBLANK_CLOCKS = (10*(PPU_OAM_SEARCH_CLOCKS+PPU_PIXEL_TRANSFER_CLOCKS+PPU_HBLANK_CLOCKS))

//since we have 4 shades, we only need 2 bits
// 160*144*2 bits = 5760 bytes

enum PPUState{
    HBLANK=0,
    VBLANK=1,
    OAM_SEARCH=2,
    LCD_DATA_TRANSFER=3
};

typedef struct {
    u16 extra_cycles;
    int state;

    u8 vram[0x2000];
    u8 oam[0xA0];

    u8 lx;
    u8 ly;

    u8 lcd_control;
    u8 lcd_status;
    //display ready to be read :^), pls don't write to it okay?
    u8 display[(VIDEO_WIDTH*VIDEO_HEIGHT)>>2];
}PPU;

PPU *ppu_new();
void ppu_close(PPU *ppu);

void ppu_clock(PPU *ppu);

u8 vram_bus_read(void *_ppu, u16 addr, u16 abs_addr);
void vram_bus_write(void *_ppu, u16 addr, u16 abs_addr, u8 val);

u8 oam_bus_read(void *_ppu, u16 addr, u16 abs_addr);
void oam_bus_write(void *_ppu, u16 addr, u16 abs_addr, u8 val);

#endif //GB_EMU_PPU_H

u8 ppu_register_read(void *_ppu, u16 addr, u16 abs_addr);
void ppu_register_write(void *_ppu, u16 addr, u16 abs_addr, u8 val);