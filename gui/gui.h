#include <SDL2/SDL.h>

#ifndef GB_EMU_GUI_H
#define GB_EMU_GUI_H

void gui_init(){
    SDL_Init(SDL_INIT_VIDEO);
}
void gui_close(){
    SDL_Quit();
}

#endif //GB_EMU_GUI_H
