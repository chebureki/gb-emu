#include "../bus/bus.h"
#include "../common/log.h"

#include <stdlib.h>

#include "ppu.h"

PPU *ppu_new(){
    //PPU *ppu = calloc(1,sizeof (PPU));
    PPU *ppu = malloc(sizeof (PPU));
    ppu->extra_cycles=0;
    ppu->state=HBLANK;
    return ppu;
}
void ppu_close(PPU *ppu){
    free(ppu);
}

u8 vram_bus_read(void *_ppu, u16 addr, u16 abs_addr){
    //TODO: hinder if needed
    PPU *ppu = (PPU*)_ppu;
    return ppu->vram[addr];
}
void vram_bus_write(void *_ppu, u16 addr, u16 abs_addr, u8 val){
    //TODO: hinder if needed
    PPU *ppu = (PPU*)_ppu;
    log_error("write to vram at %04x",addr);

    ppu->vram[addr] = val;
}

u8 oam_bus_read(void *_ppu, u16 addr, u16 abs_addr){
    //TODO: hinder if needed
    PPU *ppu = (PPU*)_ppu;
    return ppu->oam[addr];
}
void oam_bus_write(void *_ppu, u16 addr, u16 abs_addr, u8 val){
    //TODO: hinder if needed
    PPU *ppu = (PPU*)_ppu;
    ppu->oam[addr] = val;
}

void ppu_clock(PPU *ppu){
    if(ppu->extra_cycles)
        ppu->extra_cycles--;
    else{
        //TODO: do stuff
    }
}