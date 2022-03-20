#include "../common/datatypes.h"

#ifndef GB_EMU_INPUT_H
#define GB_EMU_INPUT_H

//set your controls here
//funny that I refer in the second person, eventhough the only person ever to read this is is me: cheb

#define INPUT_KEY_DOWN SDLK_UP
#define INPUT_KEY_UP SDLK_DOWN
#define INPUT_KEY_LEFT SDLK_LEFT
#define INPUT_KEY_RIGHT SDLK_RIGHT
#define INPUT_KEY_START SDLK_f
#define INPUT_KEY_SELECT SDLK_d
#define INPUT_KEY_B SDLK_s
#define INPUT_KEY_A SDLK_a

#define INPUT_KEY_TURBO SDLK_TAB


typedef struct {
    u8 emulator_inputs;
    u8 quit;
    u8 turbo;
}Inputs;

void inputs_fetch(Inputs *inputs);

#endif //GB_EMU_INPUT_H