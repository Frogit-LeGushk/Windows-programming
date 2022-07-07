#include "Declaration.h"
#include "BMPHandlers.h"
#include "Render.h"
#include "WAVHandlers.h"



int main(void) {
    TState state;
    initState(&state);

    while(true) {
        if(GetKeyState(VK_ESCAPE) < 0) break;

        // change scale
        if(GetKeyState('I') < 0) state.square_grid_size += 20;
        if(GetKeyState('D') < 0) state.square_grid_size -= 20;
        if(state.square_grid_size > 250) state.square_grid_size = 250;
        if(state.square_grid_size < 20) state.square_grid_size = 20;
        SetNewCountVertexes(&state);

        // change render speed
        if(GetKeyState('F') < 0) state.timeout_bmp_load -= 10;
        if(GetKeyState('S') < 0) state.timeout_bmp_load += 10;

        if(state.timeout_bmp_load < 20) state.timeout_bmp_load = 20;
        if(state.timeout_bmp_load > 4000) state.timeout_bmp_load = 4000;

        // change contrast
        for(int i = 1; i < 9; i++) {
            if(GetKeyState('0' + i) < 0) {
                state.stepCurrentY = i;
                break;
            }
        }

        // just "croak":)
        if(GetKeyState(VK_LBUTTON) < 0)
            playFrogCroak(&state);


        renderState(&state);
        SetNewWallpaper(&state);
    }

    return 0;
}
