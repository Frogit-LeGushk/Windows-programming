#include <windows.h>
#include <gl/gl.h>
#include <stdbool.h>
#include <math.h>
#include "./_OpenGL/camera.h"

LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
void EnableOpenGL(HWND hwnd, HDC*, HGLRC*);
void DisableOpenGL(HWND, HDC, HGLRC);

typedef struct {
    float x, y, z;
} TCell;
typedef struct {
    float r, g, b;
} TColor;

#define mapW 100
#define mapH 100
TCell map_vertexes[mapW][mapH];
TColor map_colors[mapW][mapH];
GLuint map_indexes[mapW - 1][mapH - 1][6];
#define mapIndSize (sizeof(map_indexes) / sizeof(GLuint))


bool coordinates_in_map(float x, float y)
{
    return (x > 0) && (x < mapW) && (y > 0) && (y < mapH);
}
float get_map_height(float x, float y)
{
    if(!coordinates_in_map(x, y)) return 0;
    int xc = x;
    int yc = y;
    float h1 =  (1 - (x - xc))  * map_vertexes[xc][yc].z +
                (x - xc) * map_vertexes[xc + 1][yc].z;
    float h2 =  (1 - (x - xc))  * map_vertexes[xc][yc + 1].z +
                (x - xc) * map_vertexes[xc + 1][yc + 1].z;
    return (1 - (y - yc)) * h1 + (y - yc) * h2;
}
void create_hill_in_map(float xc, float yc, float radius, float height)
{
    for(int i = xc - radius; i <= xc + radius; i++)
    {
        for(int j = yc - radius; j <= yc + radius; j++)
        {
            if(coordinates_in_map(i, j))
            {
                float length = sqrt(pow(xc - i, 2) + pow(yc - j, 2));
                float alpha = length / radius * M_PI_2;
                if(length < radius)
                {
                    map_vertexes[i][j].z += cos(alpha) * height;
                }
            }
        }
    }
}
void init_all_maps()
{
    int i, j, pos;
    for(i = 0; i < mapW; i++)
    {
        for(j = 0; j < mapH; j++)
        {
            map_vertexes[i][j].x = i;
            map_vertexes[i][j].y = j;
            map_vertexes[i][j].z = (rand() % 10) * 0.05;

            float diff_color = (rand() % 20) * 0.01;
            map_colors[i][j].r = 0.31 + diff_color;
            map_colors[i][j].g = 0.5 + diff_color;
            map_colors[i][j].b = 0.13 + diff_color;
        }
    }
    for(i = 0; i < mapW - 1; i++)
    {
        pos = i * mapH;
        for(j = 0; j < mapH - 1; j++)
        {
            map_indexes[i][j][0] = pos;
            map_indexes[i][j][1] = pos + 1;
            map_indexes[i][j][2] = pos + 1 + mapH;

            map_indexes[i][j][3] = pos + 1 + mapH;
            map_indexes[i][j][4] = pos + mapH;
            map_indexes[i][j][5] = pos;

            pos++;
        }
    }

    for(i = 0; i < 10; i++)
    {
        create_hill_in_map(rand() % mapW, rand() % mapH, rand() % 50, rand() % 10);
    }

    for(i = 0; i < mapW - 1; i++)
    {
        for(j = 0; j < mapH - 1; j++)
        {
            float diff_color = (map_vertexes[i + 1][j + 1].z - map_vertexes[i][j].z) * 0.3;
            map_colors[i][j].r += diff_color;
            map_colors[i][j].g += diff_color;
            map_colors[i][j].b += diff_color;
        }
    }
}
void show_map()
{
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
        glVertexPointer(3, GL_FLOAT, 0, map_vertexes);
        glColorPointer(3, GL_FLOAT, 0, map_colors);
        glDrawElements(GL_TRIANGLES, mapIndSize, GL_UNSIGNED_INT, map_indexes);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
}
void window_resize(int width, int height)
{
    float size = 0.1;
    float xScale = (float)width / height;

    glLoadIdentity();
    glViewport(0, 0, width, height);
    glFrustum(-xScale*size,xScale*size,  -size,size,  2*size, 100);
}
void player_move()
{
    rotation_camera_mouse(400, 400, 0.2);
    move_camera(
        GetKeyState('W') < 0 ? 1 : (GetKeyState('S') < 0 ? -1 : 0),
        GetKeyState('D') < 0 ? 1 : (GetKeyState('A') < 0 ? -1 : 0),
        0.1
    );
    camera.z = get_map_height(camera.x, camera.y) + 1.7;
}



int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow)
{
    WNDCLASSEX wcex;
    HWND hwnd;
    HDC hDC;
    HGLRC hRC;
    MSG msg;
    BOOL bQuit = FALSE;
    float theta = 0.0f;

    /* register window class */
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_OWNDC;
    wcex.lpfnWndProc = WindowProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = "GLSample";
    wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);;


    if (!RegisterClassEx(&wcex))
        return 0;

    /* create main window */
    hwnd = CreateWindowEx(0,
                          "GLSample",
                          "OpenGL Sample",
                          WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT,
                          CW_USEDEFAULT,
                          1080,
                          720,
                          NULL,
                          NULL,
                          hInstance,
                          NULL);

    ShowWindow(hwnd, nCmdShow);

    /* enable OpenGL for the window */
    EnableOpenGL(hwnd, &hDC, &hRC);

    RECT client_rect;
    GetClientRect(hwnd, &client_rect);
    window_resize(client_rect.right, client_rect.bottom);
    init_all_maps();
    glEnable(GL_DEPTH_TEST);

    /* program main loop */
    while (!bQuit)
    {
        /* check for messages */
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            /* handle or dispatch messages */
            if (msg.message == WM_QUIT)
            {
                bQuit = TRUE;
            }
            else
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else
        {
            /* OpenGL animation code goes here */

            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glPushMatrix();
                if(GetForegroundWindow() == hwnd) player_move();
                apply_camera();
                show_map();
            glPopMatrix();

            SwapBuffers(hDC);
            Sleep (1);
        }
    }

    /* shutdown OpenGL */
    DisableOpenGL(hwnd, hDC, hRC);

    /* destroy the window explicitly */
    DestroyWindow(hwnd);

    return msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_CLOSE:
            PostQuitMessage(0);
        break;

        case WM_DESTROY:
            return 0;

        case WM_SIZE:
            window_resize(LOWORD(lParam), HIWORD(lParam));
        break;

        case WM_SETCURSOR:
            ShowCursor(false);
        break;

        case WM_KEYDOWN:
        {
            switch (wParam)
            {
                case VK_ESCAPE:
                    PostQuitMessage(0);
                break;
            }
        }
        break;

        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return 0;
}

void EnableOpenGL(HWND hwnd, HDC* hDC, HGLRC* hRC)
{
    PIXELFORMATDESCRIPTOR pfd;

    int iFormat;

    /* get the device context (DC) */
    *hDC = GetDC(hwnd);

    /* set the pixel format for the DC */
    ZeroMemory(&pfd, sizeof(pfd));

    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW |
                  PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 16;
    pfd.iLayerType = PFD_MAIN_PLANE;

    iFormat = ChoosePixelFormat(*hDC, &pfd);

    SetPixelFormat(*hDC, iFormat, &pfd);

    /* create and enable the render context (RC) */
    *hRC = wglCreateContext(*hDC);

    wglMakeCurrent(*hDC, *hRC);
}

void DisableOpenGL (HWND hwnd, HDC hDC, HGLRC hRC)
{
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hRC);
    ReleaseDC(hwnd, hDC);
}

