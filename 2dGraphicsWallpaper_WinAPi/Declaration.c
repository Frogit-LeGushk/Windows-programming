#include "Declaration.h"

void SetNewCountVertexes(TState *state) {
    state->radius_dispersion = state->square_grid_size / 2 - 0.1 * state->square_grid_size;
    state->grid_vertexes.x = state->screen_rect.right / state->square_grid_size + 4;
    state->grid_vertexes.y = state->screen_rect.bottom / state->square_grid_size + 4;
}

void SetNewWallpaper(TState *state) {
    if(clock() - state->last_render > state->timeout_bmp_load) {
        CreateBMPFile(state);
        state->last_render = clock();
        SystemParametersInfoA(SPI_SETDESKWALLPAPER, 0, TEXT(state->path), SPIF_UPDATEINIFILE);
    }

    Sleep(state->sleep_delay);
}

void initState(TState *state) {
    state->filename[0] = '\0';
    strcat(state->filename, "bitmap.bmp");


    state->dirname[0] = '\0';
    _getcwd(state->dirname, FILENAME_MAX);
    state->dirname[strlen(state->dirname) + 1] = '\0';
    state->dirname[strlen(state->dirname)] = '\\';


    state->path[0] = '\0';
    strcat(state->path, state->dirname);
    strcat(state->path, state->filename);


    state->scale_factory = 1.25f;
    state->sleep_delay = 10;


    state->screen_handler = GetDesktopWindow();
    state->screen_context = GetWindowDC(state->screen_handler);

    state->console_handler = GetConsoleWindow();
    state->console_context = GetWindowDC(state->console_handler);


    GetClientRect(state->screen_handler, &(state->screen_rect));
    state->screen_rect.right *= state->scale_factory;
    state->screen_rect.bottom *= state->scale_factory;


    state->compatable_context = CreateCompatibleDC(state->screen_context);
    state->compatable_bitmap = CreateCompatibleBitmap(
        state->screen_context, state->screen_rect.right, state->screen_rect.bottom
    );


    state->last_render = clock();
    state->timeout_bmp_load = 1000;


    state->square_grid_size = 50;
    SetNewCountVertexes(state);


    state->background_color = RGB(0, 0, 0);
    state->line_width = 2;
    state->stepCurrentY = 1;


    SelectObject(state->compatable_context, state->compatable_bitmap);
    srand(time(NULL));
}
