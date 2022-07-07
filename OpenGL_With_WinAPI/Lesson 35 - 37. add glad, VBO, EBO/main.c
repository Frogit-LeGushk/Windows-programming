#include <windows.h>
#include <glad/glad.h>
#include <stdlib.h>

LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
void EnableOpenGL(HWND hwnd, HDC*, HGLRC*);
void DisableOpenGL(HWND, HDC, HGLRC);


// array_vertexes_triangle contains in RAM
GLfloat array_vertexes_triangle[] = {0,0,  1,0,  0,1};
GLfloat array_colors_triangle[] = {1,0,0,  0,1,0,  0,0,1};
GLuint array_indexes_triangle[] = {0,1,2};

GLuint vertex_VBO;
GLuint color_VBO;
GLuint index_EBO;

/**
 * ПРОБЛЕМА ТАКОГО ПОДХОДА:
 * каждый раз при отрисовке массив вершин будет копироваться
 * из оперативной памяти в видеокарту
 * в таком случае скорость рендеринга
 * будет зависить от скорости оперативной памяти и шины
 * VBO - позволяет снизить эту зависимость
 * загружая массив сразу в видеокарту
 */

void show_triangle()
{
    // NULL --> then OpenGL take vertexes from VBO
    glBindBuffer(GL_ARRAY_BUFFER, vertex_VBO);
        glVertexPointer(2, GL_FLOAT, 0, NULL);
    glBindBuffer(GL_ARRAY_BUFFER, color_VBO);
        glColorPointer(3, GL_FLOAT, 0, NULL);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
        // copy array from RAM into Video Card
        // glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_EBO);
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, NULL);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
}
void init_OpenGL_functions()
{
    gladLoadGL();

    // generate 1 free buffer
    glGenBuffers(1, &vertex_VBO);
    glGenBuffers(1, &color_VBO);
    glGenBuffers(1, &index_EBO);
    // do buffer is active
    glBindBuffer(GL_ARRAY_BUFFER, vertex_VBO);
        // copy vertex array into video card
        // GL_STATIC_DRAW - type re-updating (very seldom)
        glBufferData(
            GL_ARRAY_BUFFER,
            sizeof(array_vertexes_triangle),
            array_vertexes_triangle,
            GL_STATIC_DRAW
        );
    // do buffer is not active
    glBindBuffer(GL_ARRAY_BUFFER, color_VBO);
        glBufferData(
            GL_ARRAY_BUFFER,
            sizeof(array_colors_triangle),
            array_colors_triangle,
            GL_STATIC_DRAW
        );
    glBindBuffer(GL_ARRAY_BUFFER, 0);


    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_EBO);
        glBufferData(
            GL_ELEMENT_ARRAY_BUFFER,
            sizeof(array_indexes_triangle),
            array_indexes_triangle,
            GL_STATIC_DRAW
        );
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
void change_array_vertex_triangle()
{
    GLfloat data[2];
    data[0] = (rand() % 11 - 5) / 10.0;
    data[1] = (rand() % 11 - 5) / 10.0;

    glBindBuffer(GL_ARRAY_BUFFER, vertex_VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(data), data);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
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
                          600,
                          600,
                          NULL,
                          NULL,
                          hInstance,
                          NULL);

    ShowWindow(hwnd, nCmdShow);

    /* enable OpenGL for the window */
    EnableOpenGL(hwnd, &hDC, &hRC);
    /**
     * initialization OpenGL via glad need only
     * after call "EnableOpenGL"
     */
    init_OpenGL_functions();

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
            glClear(GL_COLOR_BUFFER_BIT);

            glPushMatrix();
                glRotatef(theta, 0,0,1);
                show_triangle();
            glPopMatrix();

            SwapBuffers(hDC);
            theta += 1.0f;
            Sleep (1);
        }
    }

    glDeleteBuffers(1, &vertex_VBO);

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
                case '1':
                    change_array_vertex_triangle();
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

