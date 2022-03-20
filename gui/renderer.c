#include "renderer.h"

//from light to dark
const u32 color_tones[4]= {
0x009bbc0f,
0x008bac0f,
0x00306230,
0x000f380f
};

Renderer *renderer_new(){
    Renderer *renderer = malloc(sizeof(Renderer));
    renderer->sdl_window = SDL_CreateWindow(RENDERER_TITLE,SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,VIDEO_WIDTH*RENDERER_SCALE,VIDEO_HEIGHT*RENDERER_SCALE,SDL_WINDOW_RESIZABLE);
    renderer->sdl_renderer = SDL_CreateRenderer(renderer->sdl_window,-1,0);
    renderer->sdl_texture = SDL_CreateTexture(renderer->sdl_renderer,SDL_PIXELFORMAT_ARGB8888,SDL_TEXTUREACCESS_STREAMING, VIDEO_WIDTH, VIDEO_HEIGHT);

    renderer->last_60_frames = SDL_GetTicks();
    renderer->frame_counter = 0;
    return renderer;
}
void renderer_close(Renderer *renderer){
    SDL_DestroyTexture(renderer->sdl_texture);
    SDL_DestroyRenderer(renderer->sdl_renderer);
    SDL_DestroyWindow(renderer->sdl_window);
    free(renderer);
}

void renderer_copy_display_data(Renderer *renderer, const u8 *display){
    //there are 4 2-bit colors in a byte
    for(int i=0; i<(VIDEO_WIDTH*VIDEO_HEIGHT)>>2; i++){
        renderer->pixels[(i<<2)+0] = color_tones[(display[i]&0xC0)>>6];
        renderer->pixels[(i<<2)+1] = color_tones[(display[i]&0x30)>>4];
        renderer->pixels[(i<<2)+2] = color_tones[(display[i]&0x0C)>>2];
        renderer->pixels[(i<<2)+3] = color_tones[(display[i]&0x03)>>0];
    }
}

void renderer_update(Renderer *renderer, u8 *display){
    renderer->frame_counter++;
    if(renderer->frame_counter >=60){
        u32 now = SDL_GetTicks();
        int frame_rate = (int)(60.0/((double)(now-renderer->last_60_frames)/1000.0));
        char title[64];
        sprintf(title,RENDERER_TITLE,frame_rate);
        SDL_SetWindowTitle(renderer->sdl_window,title);
        renderer->last_60_frames = now;
        renderer->frame_counter = 0;

    }

    renderer_copy_display_data(renderer, display);
    SDL_UpdateTexture(renderer->sdl_texture, NULL, renderer->pixels, VIDEO_WIDTH*sizeof(u32));
    SDL_RenderCopy(renderer->sdl_renderer,renderer->sdl_texture, NULL, NULL);
    SDL_RenderPresent(renderer->sdl_renderer);
}
