#include <windows.h>
#include <gl/gl.h>
#include <string.h>

#define STB_IMAGE_IMPLEMENTATION
#include "../lib/stb-master/stb_image.h"

LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
void EnableOpenGL(HWND hwnd, HDC*, HGLRC*);
void DisableOpenGL(HWND, HDC, HGLRC);

int tex_id;
float *char_width_array;

void set_char_size(unsigned char *data, int width_height, int cnt,
                   float **cWidthArray, int check_byte)
{
    int pixPerChar = width_height / 16;
    for(int k = 0; k < 256; k++)
    {
        int x = (k % 16) * pixPerChar;
        int y = (k / 16) * pixPerChar;
        int i;
        int ind;
        unsigned char alpha;
        for(i = x + pixPerChar - 1; i > x; i--)
        {
            for(int j = y + pixPerChar - 1; j > y; j--)
            {
                alpha = data[(j * width_height + i) * cnt + check_byte];
                if(alpha > 0) break;
            }
            if(alpha > 0) break;
        }

        i += pixPerChar / 10.0;
        if(i > x + pixPerChar - 1) i = x + pixPerChar - 1;
        if(k == 32) i = x + pixPerChar / 2;

        (*cWidthArray)[k] = (i - x) / (float)pixPerChar;
    }
}

void load_texture(const char *filename, int *texture, float **cWidthArray, int check_byte)
{
    int width,height,cnt;
    unsigned char *data = stbi_load(filename, &width, &height, &cnt, 0);

    if(cWidthArray != NULL)
        set_char_size(data, width, cnt, cWidthArray, check_byte);

    glGenTextures(1, texture);
    glBindTexture(GL_TEXTURE_2D, *texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height,
                                    0, cnt == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, data);
    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(data);
}
void init_game()
{
    char_width_array = malloc(256 * sizeof(*char_width_array));
    for(int i = 0; i < 256; i++)
        char_width_array[i] = 1;

    load_texture("Verdana.png", &tex_id, &char_width_array, 0);
    glBindTexture(GL_TEXTURE_2D, tex_id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
}
void text_out(int texture, char *text, float *cWidthArray)
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);
    glPushMatrix();
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
            GLfloat array_vertexes[] = {0,0,  1,0,  1,1,  0,1};
            GLfloat array_tex_coord[] = {0,1,  1,1,  1,0,  0,0};
            glVertexPointer(2, GL_FLOAT, 0, array_vertexes);
            glTexCoordPointer(2, GL_FLOAT, 0, array_tex_coord);

            while(*text)
            {
                unsigned char c = *text;
                int x = c & 0b1111;
                int y = c >> 4;

                float char_size = 1 / 16.0;
                float char_width = cWidthArray[c];

                struct {float left,right,top,botton} rect;
                rect.left = x * char_size;
                rect.right = rect.left + char_size * char_width;
                rect.top = y * char_size;
                rect.botton = rect.top + char_size;

                array_tex_coord[0] = array_tex_coord[6] = rect.left;
                array_tex_coord[2] = array_tex_coord[4] = rect.right;
                array_tex_coord[1] = array_tex_coord[3] = rect.botton;
                array_tex_coord[5] = array_tex_coord[7] = rect.top;
                array_vertexes[2] = array_vertexes[4] = char_width;

                glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
                glTranslatef(char_width,0,0);
                text++;
            }

        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glPopMatrix();
    glBindTexture(GL_TEXTURE_2D, 0);
}
void show_game()
{
    glPushMatrix();
        glTranslatef(-0.8,0,0);
        glScalef(0.1,0.1,0.1);

        glColor3f(1,1,1);
        text_out(tex_id, "Всем привееет!", char_width_array);

        glTranslatef(0,-1,0);

        glColor3f(0,1,0);
        text_out(tex_id, "Output greeeen!", char_width_array);
    glPopMatrix();
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
                          512,
                          512,
                          NULL,
                          NULL,
                          hInstance,
                          NULL);

    ShowWindow(hwnd, nCmdShow);

    /* enable OpenGL for the window */
    EnableOpenGL(hwnd, &hDC, &hRC);
    init_game();

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

            glClearColor(0.1f, 0.2f, 0.1f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            show_game();
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

