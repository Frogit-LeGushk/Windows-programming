#include <windows.h>
#include <gl/gl.h>

#ifndef HANDLERS_H_INCLUDED
#define HANDLERS_H_INCLUDED

void EnableOpenGL(HWND hwnd, HDC*, HGLRC*);
void DisableOpenGL(HWND, HDC, HGLRC);

#endif // HANDLERS_H_INCLUDED
