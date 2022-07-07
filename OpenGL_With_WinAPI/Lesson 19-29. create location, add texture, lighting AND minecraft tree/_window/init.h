#include <windows.h>

#ifndef INIT_H_INCLUDED
#define INIT_H_INCLUDED

LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
void init_and_show_window(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nCmdShow,
    HWND *hwnd
    );

#endif // INIT_H_INCLUDED
