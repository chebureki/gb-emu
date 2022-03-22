#include "../bus/bus.h"
#include "../common/log.h"

#include <stdlib.h>

#include "ppu.h"

enum LCDControl{
    DISPLAY_ENABLE=1<<7,
    WINDOW_TILEMAP_9c00_9fff=1<<6,
    WINDOW_DISPLAY_ENABLE=1<<5,
    BG_WINDOW_TILE_DATA_SELECT_8000_8FFF=1<<4,
    BG_TILE_MAP_DISPLAY_SELECT_9C00_9FFF=1<<3,
    OBJ_SPRITE_SIZE_8x16=1<<2,
    OBJ_DISPLAY_ENABLE=1<<1,
    BG_WINDOW_PRIORITY=1<<0, //TODO: what does this even mean
};

enum LCDStatus{
    LYC_LY_STAT_INTERRUPT = 1<<6,
    MODE_2_OAM_STAT_INTERRUPT = 1<<5,
    MODE_1_VBLANK_STAT_INTERRUPT = 1<<4,
    MODE_0_HBLANK_STAT_INTERRUPT = 1<<3,
    LYC_LY_FLAG = 1<<2,
    MODE_FLAG = 0x03, //first two bits
};

PPU *ppu_new(){
    PPU *ppu = calloc(1,sizeof (PPU));
    ppu->extra_cycles=0;
    ppu->state=HBLANK;
    return ppu;
}
void ppu_close(PPU *ppu){
    free(ppu);
}

u8 vram_bus_read(void *_ppu, u16 addr, u16 abs_addr){
    PPU *ppu = (PPU*)_ppu;
    if (ppu->state == LCD_DATA_TRANSFER)
        return UNDEFINED_U8;

    return ppu->vram[addr];
}
void vram_bus_write(void *_ppu, u16 addr, u16 abs_addr, u8 val){
    //TODO: hinder if needed
    PPU *ppu = (PPU*)_ppu;
    if (ppu->state == LCD_DATA_TRANSFER)
        return;
    //log_error("write to vram at %04x",abs_addr);
    log_info("writing %02x to %04x",val,abs_addr);
    ppu->vram[addr] = val;
}

u8 oam_bus_read(void *_ppu, u16 addr, u16 abs_addr){
    PPU *ppu = (PPU*)_ppu;
    if(ppu->state == OAM_SEARCH)
        return UNDEFINED_U8;
    return ppu->oam[addr];
}
void oam_bus_write(void *_ppu, u16 addr, u16 abs_addr, u8 val){
    PPU *ppu = (PPU*)_ppu;
    if(ppu->state == OAM_SEARCH)
        return UNDEFINED_U8;
    ppu->oam[addr] = val;
}

u8 ppu_register_read(void *_ppu, u16 addr, u16 abs_addr){
    PPU *ppu = (PPU*)_ppu;
    switch (abs_addr) {
        //case PPU_LCD_CONTROL: return ppu->lcd_control;
        //case PPU_LCD_STATUS: return ppu->lcd_status; //TODO
        default:
            log_error("unimplemented ppu register read: %04x",abs_addr);
    }
}

void ppu_register_write(void *_ppu, u16 addr, u16 abs_addr, u8 val){
    PPU *ppu = (PPU*)_ppu;
    switch (abs_addr) {
        default:
            log_error("unimplemented ppu register write: %04x",abs_addr);
    }
}

void ppu_clock(PPU *ppu){
    if(ppu->extra_cycles)
        ppu->extra_cycles--;
    else{
        //TODO: this is just testing code
        int offset = 20*(16);
        ppu->extra_cycles = 1000;
        ppu->display[(0*(VIDEO_WIDTH>>2))+0] = ppu->vram[offset+0];
        ppu->display[(0*(VIDEO_WIDTH>>2))+1] = ppu->vram[offset+1];
        ppu->display[(1*(VIDEO_WIDTH>>2))+0] = ppu->vram[offset+2];
        ppu->display[(1*(VIDEO_WIDTH>>2))+1] = ppu->vram[offset+3];
        ppu->display[(2*(VIDEO_WIDTH>>2))+0] = ppu->vram[offset+4];
        ppu->display[(2*(VIDEO_WIDTH>>2))+1] = ppu->vram[offset+5];
        ppu->display[(3*(VIDEO_WIDTH>>2))+0] = ppu->vram[offset+6];
        ppu->display[(3*(VIDEO_WIDTH>>2))+1] = ppu->vram[offset+7];
        ppu->display[(4*(VIDEO_WIDTH>>2))+0] = ppu->vram[offset+8];
        ppu->display[(4*(VIDEO_WIDTH>>2))+1] = ppu->vram[offset+9];
        ppu->display[(5*(VIDEO_WIDTH>>2))+0] = ppu->vram[offset+10];
        ppu->display[(5*(VIDEO_WIDTH>>2))+1] = ppu->vram[offset+11];
        ppu->display[(6*(VIDEO_WIDTH>>2))+0] = ppu->vram[offset+12];
        ppu->display[(6*(VIDEO_WIDTH>>2))+1] = ppu->vram[offset+13];
        ppu->display[(7*(VIDEO_WIDTH>>2))+0] = ppu->vram[offset+14];
        ppu->display[(7*(VIDEO_WIDTH>>2))+1] = ppu->vram[offset+15];
        /*
        for(int i=0;i<VIDEO_WIDTH>>2;i++){
            for(int j=0;j<8;j++){
                ppu->display[(j*(VIDEO_WIDTH>>2)+(i*2))+0] = ppu->vram[(i*16)+(j*2)];
                ppu->display[(j*(VIDEO_WIDTH>>2)+(i*2))+1] = ppu->vram[(i*16)+(j*2)+1];
            }
        }
         */
        //memcpy(ppu->display,ppu->vram,(VIDEO_WIDTH*VIDEO_HEIGHT)>>2);
        //TODO: do stuff
    }
}