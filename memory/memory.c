#include "memory.h"
#include "../common/log.h"

#include <stdlib.h>

WorkRAM *workram_new(){
    WorkRAM *wr = calloc(1,sizeof(WorkRAM));
    return wr;
}
void workram_close(WorkRAM *wr){
    free(wr);
}

u8 workram_bus_read(void *_wr, u16 addr, u16 abs_addr){
    WorkRAM *wr = (WorkRAM*)_wr;
    return wr->memory[addr];
}



void workram_bus_write(void *_wr, u16 addr, u16 abs_addr, u8 val){
    WorkRAM *wr = (WorkRAM*)_wr;
    wr->memory[addr] = val;
}

HighRAM *highram_new(){
    HighRAM *hr = calloc(1, sizeof (HighRAM));
    return  hr;
}
void highram_close(HighRAM *hr){
    free(hr);
}

u8 highram_bus_read(void *_hr, u16 addr, u16 abs_addr){
    HighRAM *hr = (HighRAM*)_hr;
    return hr->memory[addr];
}
void highram_bus_write(void *_hr, u16 addr, u16 abs_addr, u8 val){
    HighRAM *hr = (HighRAM*)_hr;
    hr->memory[addr] = val;
}