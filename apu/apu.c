#include "apu.h"

u8 apu_reg_bus_read(void *_a, u16 addr, u16 abs_addr){
    log_error("ignoring apu reg read");
    return UNDEFINED_U8;
}

void apu_reg_bus_write(void *_a, u16 addr, u16 abs_addr, u8 val){
    log_error("ignoring apu reg write");
}


u8 apu_wave_ram_bus_read(void *_a, u16 addr, u16 abs_addr){
    APU *a = (APU*)_a;
    log_error("not reading wave ram");
    return UNDEFINED_U8;
}
void apu_wave_ram_bus_write(void *_a, u16 addr, u16 abs_addr, u8 val){
    APU *a = (APU*)_a;
    log_error("not writing wave ram");
}

APU *apu_new(){
    APU *apu = calloc(1, sizeof (APU));
    return apu;
}

void apu_close(APU* apu){
    free(apu);
}