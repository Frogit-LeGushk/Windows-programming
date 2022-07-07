#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <direct.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>


#define _WIN32_WINNT 0x0A00
#include <windows.h>
#include <mmsystem.h>


#ifndef DECLARATION_H
#define DECLARATION_H


typedef struct {
    // window supported values
    char        filename[FILENAME_MAX];
    char        dirname[FILENAME_MAX];
    char        path[FILENAME_MAX];

    float       scale_factory;
    int         sleep_delay;

    HWND        screen_handler;
    HDC         screen_context;
    RECT        screen_rect;

    HWND        console_handler;
    HDC         console_context;

    HDC         compatable_context;
    HBITMAP     compatable_bitmap;


    // render time state
    clock_t     last_render;
    clock_t     timeout_bmp_load;


    // render drawing state
    int         square_grid_size;
    int         radius_dispersion;
    POINT       grid_vertexes;
    COLORREF    background_color;
    int         line_width;
    int         stepCurrentY;
} TState;

void SetNewCountVertexes(TState *state);
void SetNewWallpaper(TState *state);

void initState(TState *state);


#endif // DECLARATION_H


