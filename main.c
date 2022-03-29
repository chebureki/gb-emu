#include "emulator/emulator.h"
#include "cartridge/cartridge.h"
#include "gui/renderer.h"
#include "gui/gui.h"
#include "gui/input.h"

#define CYCLES_IN_MS 4100

int main(int argc, char **argv) {
    if(argc < 2){
        log_info("usage: gb_emu <path-to-rom>");
        return 0;
    }
    //TODO: read from from cli options, e.g. gb-emu -i foo.gb
    char *rom_path = argv[1];
    gui_init();
    Cartridge *cartridge = cartridge_from_file(rom_path);

    if(!cartridge){
        log_fatal("failed to open cartridge");
    }
    log_info("title: %s", cartridge->title);
    log_info("banks: %d", cartridge->banks_amount);
    log_info("rom-size: %d bytes", cartridge->rom_size);
    log_info("extra ram-banks: %d",cartridge->extra_ram_size);

    Emulator *emu = emulator_new(cartridge);
    Renderer *renderer = renderer_new();

    Inputs inputs= {.quit=0,.turbo=0,.emulator_inputs=0};

    while (!inputs.quit){
        u32 prev_ticks = SDL_GetTicks();
        inputs_fetch(&inputs);

        for (int i=0; i<70000; ++i)
            emulator_clock(emu,inputs.emulator_inputs);
        renderer_update(renderer,emu->ppu->display);
        u32 exec_delta = SDL_GetTicks()-prev_ticks;

        //TODO: shorten this
        if(inputs.turbo){
            SDL_Delay(1);
        }else{
            if (exec_delta < 17)
                SDL_Delay(17-exec_delta); //60fps
            else
                SDL_Delay(1);
        }
    }
    emulator_close(emu);
    cartridge_close(cartridge);
    renderer_close(renderer);
    gui_close();
    return 0;
}
