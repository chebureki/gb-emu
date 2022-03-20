#include "input.h"
#include "../emulator/emulator.h"

#include <SDL2/SDL.h>


void inputs_fetch(Inputs *inputs){
    SDL_Event event;
    SDL_PollEvent(&event);
    switch (event.type) {
        case SDL_QUIT:
            inputs->quit=1;
            break;

        case SDL_KEYDOWN:
            switch(event.key.keysym.sym){
                case INPUT_KEY_UP: inputs->emulator_inputs|=INPUT_UP;break;
                case INPUT_KEY_DOWN: inputs->emulator_inputs|=INPUT_DOWN;break;
                case INPUT_KEY_LEFT: inputs->emulator_inputs|=INPUT_LEFT;break;
                case INPUT_KEY_RIGHT: inputs->emulator_inputs|=INPUT_RIGHT;break;
                case INPUT_KEY_B: inputs->emulator_inputs|=INPUT_B;break;
                case INPUT_KEY_A: inputs->emulator_inputs|=INPUT_A;break;
                case INPUT_KEY_START: inputs->emulator_inputs|=INPUT_START;break;
                case INPUT_KEY_SELECT: inputs->emulator_inputs|=INPUT_SELECT;break;

                case INPUT_KEY_TURBO: inputs->turbo=1;break;
            }
            break;
        case SDL_KEYUP:
            switch(event.key.keysym.sym){
                case INPUT_KEY_UP: inputs->emulator_inputs&=~(INPUT_UP);break;
                case INPUT_KEY_DOWN: inputs->emulator_inputs&=~(INPUT_DOWN);break;
                case INPUT_KEY_LEFT: inputs->emulator_inputs&=~(INPUT_LEFT);break;
                case INPUT_KEY_RIGHT: inputs->emulator_inputs&=~(INPUT_RIGHT);break;
                case INPUT_KEY_B: inputs->emulator_inputs&=~(INPUT_B);break;
                case INPUT_KEY_A: inputs->emulator_inputs&=~(INPUT_A);break;
                case INPUT_KEY_START: inputs->emulator_inputs&=~(INPUT_START);break;
                case INPUT_KEY_SELECT: inputs->emulator_inputs&=~(INPUT_SELECT);break;

                case INPUT_KEY_TURBO: inputs->turbo=0;break;
            }
            break;
    }
}