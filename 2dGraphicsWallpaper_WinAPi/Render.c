#include "Render.h"


static Vertex vertexes[MAX_VERTEXES][MAX_VERTEXES];

static void drawLine(TState *state, Vertex v1, Vertex v2) {
    MoveToEx(state->compatable_context, v1.center.x, v1.center.y, NULL);
    LineTo(state->compatable_context, v2.center.x, v2.center.y);
}

static EquationLine generateEquationLine(Vertex vstart, Vertex vend) {
    EquationLine line;
    line.K = (vstart.center.x - vend.center.x) / (double)(vstart.center.y - vend.center.y);
    line.B = vend.center.x - line.K * vend.center.y;
    return line;
}

static EquationLine generateColorEquationLine(Vertex vstart, Vertex vend, char Color) {
    EquationLine line;

    if(Color == 'R') {
        line.K = (GetRValue(vstart.color) - GetRValue(vend.color)) /
                (vstart.center.y - vend.center.y);
        line.B = GetRValue(vend.color) - line.K * vend.center.y;
    }
    else if(Color == 'G') {
        line.K = (GetGValue(vstart.color) - GetGValue(vend.color)) /
                (vstart.center.y - vend.center.y);
        line.B = GetGValue(vend.color) - line.K * vend.center.y;
    }
    else if(Color == 'B') {
        line.K = (GetBValue(vstart.color) - GetBValue(vend.color)) /
                (vstart.center.y - vend.center.y);
        line.B = GetBValue(vend.color) - line.K * vend.center.y;
    }
    else exit(0);


    return line;
}

static double EquationLineFunction(double y, EquationLine line) {
    return line.K * y + line.B;
}

static void drawThreePointsGradient(TState *state, Vertex v1, Vertex v2, Vertex v3, Vertex v4) {
    // R G B color
    int color_v1[3] = {GetRValue(v1.color), GetGValue(v1.color), GetBValue(v1.color)};
    int color_v4[3] = {GetRValue(v4.color), GetGValue(v4.color), GetBValue(v4.color)};


    // create boundary line between v1 and v4
    EquationLine boundary = generateEquationLine(v2, v3);
    // create color boundary line
    EquationLine color_boundary_red = generateColorEquationLine(v2, v3, 'R');
    EquationLine color_boundary_green = generateColorEquationLine(v2, v3, 'G');
    EquationLine color_boundary_blue = generateColorEquationLine(v2, v3, 'B');


    Vertex vstart_one = v1;
    Vertex vstart_two = v4;
    Vertex vend;
    for(long int currentY = v3.center.y; currentY < v2.center.y; currentY+=state->stepCurrentY) {
        vend.center.y = currentY;
        vend.center.x = EquationLineFunction(currentY, boundary);
        vend.color = RGB(
            EquationLineFunction(currentY, color_boundary_red),
            EquationLineFunction(currentY, color_boundary_green),
            EquationLineFunction(currentY, color_boundary_blue)
        );


        int mediumR_one = (color_v1[0] + GetRValue(vend.color)) / 2;
        int mediumG_one = (color_v1[1] + GetGValue(vend.color)) / 2;
        int mediumB_one = (color_v1[2] + GetBValue(vend.color)) / 2;

        int mediumR_two = (color_v4[0] + GetRValue(vend.color)) / 2;
        int mediumG_two = (color_v4[1] + GetGValue(vend.color)) / 2;
        int mediumB_two = (color_v4[2] + GetBValue(vend.color)) / 2;


        SelectObject(state->compatable_context, GetStockObject(DC_PEN));
        SetDCPenColor(state->compatable_context, RGB(mediumR_one, mediumG_one, mediumB_one));
        drawLine(state, vstart_one, vend);


        SelectObject(state->compatable_context, GetStockObject(DC_PEN));
        SetDCPenColor(state->compatable_context, RGB(mediumR_two, mediumG_two, mediumB_two));
        drawLine(state, vstart_two, vend);
    }
}


void renderState(TState *state) {
    if(clock() - state->last_render + 100 < state->timeout_bmp_load) return;

    // clear background
    SelectObject(state->compatable_context, GetStockObject(DC_BRUSH));
    SetDCBrushColor(state->compatable_context, state->background_color);
    Rectangle(
        state->compatable_context, -1, -1,
        state->screen_rect.right + 1,
        state->screen_rect.bottom + 1
    );


    // set start point for pseudo generation number
    int i;
    int j;


    // calculate vertexes
    for(i = 0; i < state->grid_vertexes.x; i++) {
        for(j = 0; j < state->grid_vertexes.y; j++) {
            // calc vertexes
            vertexes[i][j].center.x = state->square_grid_size * (i - 1);
            vertexes[i][j].center.y = state->square_grid_size * (j - 1);

            vertexes[i][j].radius = rand() % state->radius_dispersion;
            vertexes[i][j].corner = 2 * M_PI * (rand() % 360) / 360;

            vertexes[i][j].center.x += cos(vertexes[i][j].corner) * vertexes[i][j].radius;
            vertexes[i][j].center.y += sin(vertexes[i][j].corner) * vertexes[i][j].radius;

            vertexes[i][j].color = RGB((rand() % 256), (rand() % 256), (rand() % 256));
        }
    }

    // draw three point gradient
    for(i = 0; i < state->grid_vertexes.x - 1; i++) {
        for(j = 0; j < state->grid_vertexes.y - 1; j++) {
            drawThreePointsGradient(
                state,
                vertexes[i][j], vertexes[i][j + 1],
                vertexes[i + 1][j], vertexes[i + 1][j + 1]
            );
        }
    }

    // draw line connection
    SelectObject(state->compatable_context, GetStockObject(DC_PEN));
    SetDCPenColor(state->compatable_context, RGB(0, 0, 0));
    for(i = 0; i < state->grid_vertexes.x - 1; i++) {
        for(j = 0; j < state->grid_vertexes.y - 1; j++) {
            drawLine(state, vertexes[i][j], vertexes[i][j + 1]);
            drawLine(state, vertexes[i][j], vertexes[i + 1][j]);
            drawLine(state, vertexes[i + 1][j], vertexes[i][j + 1]);
        }
    }

    // generate new sequence numbers
    static int updateSequence = 1;
    if(updateSequence % (state->screen_rect.bottom / 20) == 0)
        srand(time(NULL));
    updateSequence++;
}
