#include "../common/datatypes.h"

#ifndef GB_EMU_PPU_H
#define GB_EMU_PPU_H

#define VIDEO_WIDTH 160
#define VIDEO_HEIGHT 144

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

#define FIFO_CAP 16

enum PPUState{
    HBLANK=0,
    VBLANK=1,
    OAM_SEARCH=2,
    LCD_DATA_TRANSFER=3
};

typedef struct {
    u8 y_pos;
    u8 x_pos;
    u8 tile_number;
    u8 sprite_flags;
}OAMObject;

typedef struct {
    u8 color;
    u8 background_priority;
}FIFOPixel;

typedef struct {
    u16 extra_cycles;
    int state;

    u8 vram[0x2000];
    u8 oam[0xA0];

    u8 scx;
    u8 scy;

    u8 ly;

    //FIFOPixel background_fifo[FIFO_CAP];
    //int background_fifo_size;
    //FIFOPixel sprite_fifo[FIFO_CAP];
    //int sprite_fifo_size;

    u8 lcd_control;
    u8 lcd_status;
    //display ready to be read :^), pls don't write to it okay?
    u8 display[(VIDEO_WIDTH*VIDEO_HEIGHT)>>2];

    OAMObject eligible_objects[10]; //only up to ten
    u8 eligible_objects_count; //ditto

    u8 bg_palette;
    u8 obp0_palette;
    u8 obp1_palette;
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