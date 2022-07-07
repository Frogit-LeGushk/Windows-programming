#include <windows.h>
#include <gl/gl.h>

#define STB_IMAGE_IMPLEMENTATION
#include "../lib/stb-master/stb_image.h"

LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
void EnableOpenGL(HWND hwnd, HDC*, HGLRC*);
void DisableOpenGL(HWND, HDC, HGLRC);

/**
 * Create custom texture
 */
unsigned int texture;

void generate_castom_texture()
{
    int width, height;
    width = 2;
    height = 2;

    // 4 pixels - first [] -> axis Y; second [] -> axis X
    struct { unsigned char r,g,b,a; } map_colors[2][2];
    memset(map_colors, 0, sizeof(map_colors));
    map_colors[0][0].r = 255;
    map_colors[1][0].g = 255;
    map_colors[1][1].b = 255;
    map_colors[0][1].r = 255;
    map_colors[0][1].g = 255;

    // create texture on video card
    glGenTextures(1, &texture);
    // do texture is active for set some props
    glBindTexture(GL_TEXTURE_2D, texture);
        /**
         * template [type_texture, which_props_is_set, value_of_prop]
         */
        // GL_CLAMP - filling area by nearest color
        // GL_REPEAT - repeating texture
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // GL_TEXTURE_MIN_FILTER - decrease texture
        // GL_TEXTURE_MAG_FILTER - increase texture
        // GL_NEAREST - OpenGL get value from nearest vertex
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        // load texture from array in video card
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
                     GL_UNSIGNED_BYTE, map_colors);
    // after that how we sot props, do texture is not active
    // 0 --> no have active textures
    glBindTexture(GL_TEXTURE_2D, 0);
}
void load_texture()
{
    int width, height, cnt;
    const char *filename = "bug.png";
    unsigned char *data = stbi_load(filename, &width, &height, &cnt, 0);

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D( GL_TEXTURE_2D, 0, cnt == 3 ? GL_RGB : GL_RGBA,
                                        width, height, 0,
                                        cnt == 3 ? GL_RGB : GL_RGBA,
                                        GL_UNSIGNED_BYTE, data);
    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(data);
}


GLfloat array_vertexes_squeare[] = {
    -1,-1,0,  -1,1,0,  1,1,0,  1,-1,0
};
GLfloat array_coordinates_texture[] = {
    0,1,  0,0,  1,0,  1,1
};

void show_texture_square()
{
    static float yAlpha = 0;
    static float xAlpha = 0;
    static float zAlpha = 0;

    // allow work with texture
    glEnable(GL_TEXTURE_2D);
    // do texture is active
    glBindTexture(GL_TEXTURE_2D, texture);
    // for displaying all colors
    glColor3f(1,1,1);

    glPushMatrix();
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

        glVertexPointer(3, GL_FLOAT, 0, array_vertexes_squeare);
        glTexCoordPointer(2, GL_FLOAT, 0, array_coordinates_texture);


        glTranslatef(0, 0, -4);
        glRotatef(xAlpha, 1, 0, 0);
        glRotatef(yAlpha, 0, 1, 0);
        glRotatef(zAlpha, 0, 0, 1);



        glPushMatrix();
            glTranslatef(0,0,1);
            glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        glPopMatrix();

        glPushMatrix();
            glRotatef(90,0,1,0);
            glTranslatef(0,0,1);
            glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        glPopMatrix();

        glPushMatrix();
            glRotatef(180,0,1,0);
            glTranslatef(0,0,1);
            glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        glPopMatrix();

        glPushMatrix();
            glRotatef(270,0,1,0);
            glTranslatef(0,0,1);
            glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        glPopMatrix();

        glPushMatrix();
            glRotatef(90,1,0,0);
            glTranslatef(0,0,1);
            glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        glPopMatrix();

        glPushMatrix();
            glRotatef(270,1,0,0);
            glTranslatef(0,0,1);
            glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        glPopMatrix();


    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glPopMatrix();

    yAlpha += 0.5;
    xAlpha += 0.25;
    //zAlpha += 0.125;
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
                          1000,
                          1000,
                          NULL,
                          NULL,
                          hInstance,
                          NULL);

    ShowWindow(hwnd, nCmdShow);

    /* enable OpenGL for the window */
    EnableOpenGL(hwnd, &hDC, &hRC);

    //generate_castom_texture();
    load_texture();

    glFrustum(-1,1, -1,1, 1, 100);
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

            glClearColor(0.1f, 1.0f, 0.1f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                show_texture_square();
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

