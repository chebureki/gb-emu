#include "../common/datatypes.h"
#include "../ppu/ppu.h"

#include <SDL2/SDL.h>

#ifndef GB_EMU_RENDERER_H
#define GB_EMU_RENDERER_H

#define RENDERER_TITLE "cheb's gameboy emulator :^) | fps:%d"
#define RENDERER_SCALE 3

typedef struct {
    u32 pixels[VIDEO_WIDTH*VIDEO_HEIGHT];

    SDL_Window *sdl_window;
    SDL_Renderer *sdl_renderer;
    SDL_Texture *sdl_texture;

    u32 last_60_frames;
    u32 frame_counter;
    u32 frame_rate_average;
} Renderer;

Renderer *renderer_new();
void renderer_close(Renderer *renderer);

void renderer_update(Renderer *renderer, u8 *display);


#endif //GB_EMU_RENDERER_H
