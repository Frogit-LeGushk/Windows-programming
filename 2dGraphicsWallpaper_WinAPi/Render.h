#include "Declaration.h"


#ifndef RENDER_H
#define RENDER_H

#define MAX_VERTEXES 200

typedef struct {
    POINT       center;
    int         radius;
    float       corner;
    COLORREF    color;
} Vertex;

typedef struct {
    double K;
    double B;
} EquationLine;

void renderState(TState *state);

#endif
