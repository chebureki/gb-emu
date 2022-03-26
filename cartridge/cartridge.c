#include "cartridge.h"
#include "../common/log.h"
#include "../bus/bus.h"

#include <stdio.h>

#define HEADER_SIZE 0x150
#define TITLE_POS 0x134
#define ROM_CODE_POS 0x148
#define RAM_SIZE_POS 0x149

int cartridge_rom_size(int code){
    if(0x00<= code && code <= 0x08)
        return 1<<(15+code); // 1<<15: 32kb, 1<<16: 64kb, ...
    log_error("did not recognize rom-code %01x",code);
    return -1;
}

int cartridge_bank_amount(int code){
    if(0x00<= code && code <= 0x08)
        return 1<<(1+code); // 1<<1: 2, 1<<2: 4kb, ...
    log_error("did not recognize rom-code %02x",code);
    return -1;
}

int cartridge_ram_size(int code){
    switch (code) {
        case 0: return 0;
        case 2: return 1;
        case 3: return 4;
        case 4: return 16;
        case 5: return 8;
        default: log_error("unknown ram code %d",code);return 0;
    }
}

//Cartridge *cartridge_from_buffer(u8 *buf, size_t buf_length);
Cartridge *cartridge_from_file(char *file_path){
    FILE *file = fopen(file_path, "rb");
    if(!file){
        log_error("failed to open cartridge at %s",file_path);
        return NULL;
    }

    u8 header_data[HEADER_SIZE];
    size_t read_bytes = fread(header_data,1,HEADER_SIZE,file);
    if(read_bytes != HEADER_SIZE){
        fclose(file);
        log_error("file too small to be a cartridge");
        return NULL;
    }

    Cartridge *cartridge = calloc(1,sizeof (Cartridge));

    fseek(file,TITLE_POS,SEEK_SET);
    fread(cartridge->title, 1, CARTRIDGE_TITLE_SIZE, file);
    u8 rom_code;
    fseek(file, ROM_CODE_POS, SEEK_SET);
    fread(&rom_code, 1, 1,file);
    if((cartridge->rom_size = cartridge_rom_size(rom_code))<0){
        fclose(file);
        free(cartridge);
        return NULL;
    }
    //rom_code implied to be valid
    cartridge->banks_amount = cartridge_bank_amount(rom_code);

    fseek(file,RAM_SIZE_POS,SEEK_SET);
    u8 ram_size_code;
    fread(&ram_size_code,1,1,file);
    cartridge->extra_ram_size = cartridge_ram_size(ram_size_code);

    fseek(file,0,SEEK_END);
    if(ftell(file) != cartridge->rom_size){
        log_error("file does not conform to indicated size");
        fclose(file);
        free(cartridge);
        return NULL;
    }
    cartridge->data = malloc(cartridge->rom_size);
    fseek(file,0,SEEK_SET);
    fread(cartridge->data,1,cartridge->rom_size,file);
    fclose(file);
    return cartridge;
}

void cartridge_close(Cartridge *cartridge){
    free(cartridge->data);
    free(cartridge);
}

//TODO: rom banks!!!
u8 cartridge_bank0_bus_read(void *_c, u16 addr, u16 abs_addr){
    Cartridge *cartridge = (Cartridge*)_c;
    return cartridge->data[addr];
}
void cartridge_bank0_bus_write(void *_c, u16 addr, u16 abs_addr, u8 val){
    Cartridge *c = (Cartridge*)_c;
    if(0x2000<=abs_addr && abs_addr<=0x3fff ){
        c->selected_rom_bank = val;
        return;
    }
    log_error("rom0 write?: %04x %2x 0a:%d",abs_addr,val,(val&0x0f)==0x0a);

    //cartridge->data[addr];
}

u8 cartridge_bankn_bus_read(void *_c, u16 addr, u16 abs_addr){
    Cartridge *c = (Cartridge*)_c;
    uint offset = (c->selected_rom_bank*ROM_BANK_SIZE)+addr;
    //if (c->data_size <= offset){
    //    log_fatal("out of bounce rom read at %04x",abs_addr);
    //}
    return c->data[offset];
}
void cartridge_bankn_bus_write(void *_c, u16 addr, u16 abs_addr, u8 val){
    Cartridge *c = (Cartridge*)_c;
    if(0x6000<=abs_addr && abs_addr<=0x7fff ){
        log_error("did not switch banking mode for testing purposes");
        return;
    }
    //log_fatal("unknown write to rom: %04x %02x",abs_addr,val);
}

u8 cartridge_external_ram_bus_read(void *_c, u16 addr, u16 abs_addr){
    log_fatal("cartridge ram read");
}
void cartridge_external_ram_bus_write(void *_c, u16 addr, u16 abs_addr, u8 val){
    log_error("cartridge ram write");
}