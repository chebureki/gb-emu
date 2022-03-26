#include "ppu.h"
#include "../bus/bus.h"
#include "../common/log.h"


#define HBLANK_CLOCKS 204
#define LCD_TRANSFER_CLOCKS 172
#define OAM_SEARCH_CLOCKS 80

#define MAP_WIDTH 256
#define MAP_HEIGHT 256

//since we have 4 shades, we only need 2 bits
// 160*144*2 bits = 5760 bytes

u8 color_from_palette(u8 p, u8 i){
    return (p&(3<<(2*i)))>>(2*i);
}

enum SpriteFlags{
    OBJ_TO_BG_PRIORITY = 1<<7,
    Y_FLIP = 1<<6,
    X_FLIP = 1<<5,
    PALETTE_NUMBER = 1<<4,
};


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
    ppu->state=OAM_SEARCH;
    ppu->ly = 0;
    return ppu;
}
void ppu_close(PPU *ppu){
    free(ppu);
}
/*
void fifo_pop(FIFOPixel fifo[FIFO_CAP], FIFOPixel *dst, int *cap){
    dst->background_priority=fifo[0].background_priority;
    dst->color = fifo[0].color;
    for(int i=1;i<*cap; i++){
        fifo[i-1].background_priority = fifo[i].background_priority;
        fifo[i-1].color = fifo[i].color;
    }
    (*cap)--;
}

void fifo_push(FIFOPixel fifo[16], FIFOPixel *src, int *cap){
    if(*cap >= FIFO_CAP){
        log_fatal("exceeded fifo cap");
    }
    fifo[*cap].color = src->color;
    fifo[*cap].background_priority = src->background_priority;
    (*cap)++;
}
 */

u8 vram_bus_read(void *_ppu, u16 addr, u16 abs_addr){
    PPU *ppu = (PPU*)_ppu;
    //if (ppu->state == LCD_DATA_TRANSFER)
    //    return UNDEFINED_U8;

    return ppu->vram[addr];
}
void vram_bus_write(void *_ppu, u16 addr, u16 abs_addr, u8 val){
    //TODO: hinder if needed
    PPU *ppu = (PPU*)_ppu;
    //if (ppu->state == LCD_DATA_TRANSFER)
    //   return;
    //log_info("writing %02x to %04x / %04x",val,addr,abs_addr);
    ppu->vram[addr] = val;
}

u8 oam_bus_read(void *_ppu, u16 addr, u16 abs_addr){
    PPU *ppu = (PPU*)_ppu;
    //if(ppu->state == OAM_SEARCH)
    //    return UNDEFINED_U8;
    return ppu->oam[addr];
}
void oam_bus_write(void *_ppu, u16 addr, u16 abs_addr, u8 val){
    PPU *ppu = (PPU*)_ppu;
    //if(ppu->state == OAM_SEARCH)
    //    return;
    ppu->oam[addr] = val;
}

u8 ppu_register_read(void *_ppu, u16 addr, u16 abs_addr){
    PPU *ppu = (PPU*)_ppu;
    switch (abs_addr) {
        case PPU_REG_LY: return ppu->ly;
        case PPU_REG_SCY: return ppu->scy;break;
        //case PPU_LCD_CONTROL: return ppu->lcd_control;
        //case PPU_LCD_STATUS: return ppu->lcd_status; //TODO
        default:
           log_error("unimplemented ppu register read: %04x",abs_addr);
    }
}

void ppu_register_write(void *_ppu, u16 addr, u16 abs_addr, u8 val){
    PPU *ppu = (PPU*)_ppu;
    switch (abs_addr) {
        case PPU_REG_SCY: ppu->scy=val;break;
        case PPU_REG_SCX: ppu->scx=val;break;
        case PPU_REG_BGP: ppu->bg_palette=val; break;
        case PPU_REG_OBP0: ppu->obp0_palette=val; break;
        case PPU_REG_OBP1: ppu->obp1_palette=val; break;
        case PPU_REG_LCD_CONTROL: ppu->lcd_control=val; break;

        default:
            log_error("unimplemented ppu register write: %04x : %02x / %08b",abs_addr,val,val);
    }
}

int ppu_sprite_height(PPU *ppu){
    if(ppu->lcd_status & OBJ_SPRITE_SIZE_8x16)
        return 16;
    else
        return 8;
}

//TODO: perhaps make this a macro, yea I am somewhat obsessed with macros, I am well aware
void set_pixel(u8 *display, u8 x, u8 y, u8 v){
    v&=3; //only select two bits
    int offset = ((y*VIDEO_WIDTH)+x)>>2;
    int in_byte_pos = ((y*VIDEO_WIDTH) +x)&3;
    int v_patch = (v<<((3-in_byte_pos)*2));
    int pixel_patch = (3<<((3-in_byte_pos)*2));
    display[offset] =  (display[offset]&~pixel_patch) | v_patch;
}

//return a DMA vram address, don't use it in any bus reads!!!
u16 obj_tile_offset(PPU *ppu, u8 tile_id){
    //8000 to 8fff
    return tile_id*16; //id * sizeof(Tile)
}

//ditto, see above
u16 bg_win_tile_offset(PPU *ppu, u8 tile_id){
    //8000 to 8fff, just like for objs
    if(ppu->lcd_control & BG_WINDOW_TILE_DATA_SELECT_8000_8FFF)
        return tile_id*16;

    //9000 to 97FF
    if(tile_id <=127)
        return 0x1000+(tile_id*16);
    //8800 to 8FFF
    return 0x800+(tile_id*16); //id * sizeof(Tile)
}

void ppu_oam_search(PPU *ppu){
    ppu->eligible_objects_count=0;
    //TODO: make 40 a constant, as is the maximum amount of objects in oam
    for(int i=0;i<40;i++){
        OAMObject *obj = (OAMObject*)(ppu->vram + ((sizeof(OAMObject))*i));
        if((obj->x_pos > 0) && ((ppu->ly+16)>=obj->y_pos) && ((ppu->ly+16)<(obj->y_pos+ ppu_sprite_height(ppu))) ){
            //just copy it over
            ppu->eligible_objects[ppu->eligible_objects_count] = (OAMObject){.x_pos=obj->x_pos, .y_pos=obj->y_pos, .tile_number=obj->tile_number, .sprite_flags=obj->sprite_flags};
            ppu->eligible_objects_count++;
        }
        //a scanline only allows for up to 10 sprites
        if(ppu->eligible_objects_count >= 10)
            break;
    }

    //TODO: this creates +1 cycle
    //fuck it, no cycles for you
    ppu->extra_cycles=OAM_SEARCH_CLOCKS;
    ppu->state = LCD_DATA_TRANSFER;
}

u16 ppu_get_bg_map_start(PPU *ppu){
    if(ppu->lcd_control & BG_TILE_MAP_DISPLAY_SELECT_9C00_9FFF)
        return 0x1C00;
    else
        return 0x1800;
}


u8 pixel_from_tile_bytes(u8 low, u8 high, int x){
    int low_bit = (low>>(7-x))&1;
    int high_bit =  (x<7? (high>>(7-x-1)):((high<<1)))&2;
    return low_bit|high_bit;
}

u8 ppu_get_bg_pixel(PPU *ppu, int wx, int wy){
    int tile_pos = (wx/8)+((wy/8)*(MAP_WIDTH/8));
    //log_info("%04x start",ppu_get_bg_map_start(ppu));
    int tile_i = ppu->vram[ppu_get_bg_map_start(ppu)+tile_pos];
    int tile_offset = bg_win_tile_offset(ppu,tile_i);
    u8 low = ppu->vram[tile_offset + (2*(wy&7))+0];
    u8 high = ppu->vram[tile_offset + (2*(wy&7))+1];

    return pixel_from_tile_bytes(low,high,wx&7);
}

u8 ppu_get_win_pixel(PPU *ppu, int wx, int wy){
    u8 pixel;
    //TODO
    return pixel;
}

u8 ppu_get_obj_pixel(PPU *ppu, int wx, int wy){
    u8 pixel;
    //TODO
    return pixel;
}

u8 ppu_wx(PPU *ppu,int sx){
    return (ppu->scx + sx)%MAP_WIDTH;
}

u8 ppu_wy(PPU *ppu, int sy){
    return (ppu->ly+ppu->scy)%MAP_HEIGHT;
}

void ppu_lcd_data_transfer(PPU *ppu){
    int wy = ppu_wy(ppu,ppu->ly);
    for(int sx=0;sx<VIDEO_WIDTH;sx++){
        int wx = ppu_wx(ppu,sx);
        u8 bg_pixel = ppu_get_bg_pixel(ppu,wx,wy);

        u8 col = color_from_palette(ppu->bg_palette,bg_pixel);
        set_pixel(ppu->display,sx,ppu->ly,col);
    }

    ppu->ly++;
    ppu->extra_cycles=LCD_TRANSFER_CLOCKS;
    if(ppu->ly>=144) {
        ppu->state = VBLANK;
    }
}

void ppu_hblank(PPU *ppu){
    ppu->extra_cycles = HBLANK_CLOCKS;
    ppu->state = OAM_SEARCH;
}

void ppu_vblank(PPU *ppu){
    ppu->ly++;
    if(ppu->ly >153){
        ppu->state = OAM_SEARCH;
        ppu->ly = 0;

    }else{
        ppu->extra_cycles=OAM_SEARCH_CLOCKS+LCD_TRANSFER_CLOCKS+HBLANK_CLOCKS;
    }
}

void ppu_clock(PPU *ppu){
    if(ppu->extra_cycles) {
        ppu->extra_cycles--;
        return;
    }

    switch (ppu->state) {
        case OAM_SEARCH: ppu_oam_search(ppu);break;
        case LCD_DATA_TRANSFER: ppu_lcd_data_transfer(ppu); break;
        case HBLANK: ppu_hblank(ppu); break;
        case VBLANK: ppu_vblank(ppu); break;
        default:
            log_fatal("reached undefined state: %d", ppu->state);
    }
}