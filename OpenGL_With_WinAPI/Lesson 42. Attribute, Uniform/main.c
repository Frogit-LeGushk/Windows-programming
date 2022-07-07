#include <windows.h>
#include <glad/glad.h>
#include <stdio.h>

GLuint prog;
LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
void EnableOpenGL(HWND hwnd, HDC*, HGLRC*);
void DisableOpenGL(HWND, HDC, HGLRC);

void show_triangle_1()
{
    glUseProgram(prog);

    // return -1 if the variable exist in shader, but don't use
    // after compiling unusable variable will be delete
    int k = glGetAttribLocation(prog, "abc");
    int u = glGetUniformLocation(prog, "dxy");
    glUniform2f(u, -0.25, -0.25);

    glBegin(GL_TRIANGLES);
        // glVertexAttrib3f - send vec3 type in vertex shader
        glVertexAttrib3f(k, 1.0f, 0.0f, 0.0f);   glVertex2f(0.0f,   1.0f);
        glVertexAttrib3f(k, 0.0f, 1.0f, 0.0f);   glVertex2f(0.87f,  -0.5f);
        glVertexAttrib3f(k, 0.0f, 0.0f, 1.0f);   glVertex2f(-0.87f, -0.5f);
    glEnd();
}
GLfloat __vertexes[] = {0,0,  1,0,  0,1};
GLfloat __colors[] = {0,1,0,  1,0,0,  0,0,1};
GLuint __color_VBO;

void show_triangle_2()
{
    glUseProgram(prog);

    // uniform var send is the same method
    int u = glGetUniformLocation(prog, "dxy");
    glUniform2f(u, -0.5, -0.5);

    glVertexPointer(2, GL_FLOAT, 0, __vertexes);

    int k = glGetAttribLocation(prog, "abc");
    glBindBuffer(GL_ARRAY_BUFFER, __color_VBO);
        glVertexAttribPointer(k, 3, GL_FLOAT, 0, 0, NULL);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableVertexAttribArray(k);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableVertexAttribArray(k);
}

char *load_text_file(const char *filename)
{
    static char program_text[(1 << 13)];
    memset(program_text, 0, 1 << 13);
    FILE *fp = fopen(filename, "rb");
    fread(program_text, sizeof(char), (1 << 13) - 1, fp);
    return program_text;
}
void check_compile_status(GLuint shader)
{
    GLuint status_ok;
    GLchar status_log[2000];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status_ok);
    if(!status_ok)
    {
        glGetShaderInfoLog(shader, 2000, NULL, status_log);
        printf("COMPILE STATUS LOG: %s\n", status_log);
    }
}
void check_link_status(GLuint shader)
{
    GLuint status_ok;
    GLchar status_log[2000];
    glGetProgramiv(shader, GL_LINK_STATUS, &status_ok);
    if(!status_ok)
    {
        // NULL - addr var where write length of log
        glGetProgramInfoLog(shader, 2000, NULL, status_log);
        printf("LINK STATUS LOG: %s\n", status_log);
    }
}
GLuint load_shader(const char *filename, GLuint type)
{
    char *text = load_text_file(filename);
    // get descriptor of shader
    GLuint shader = glCreateShader(type);
    // load text into shader
    // [3,4] - array string's and array lengths of strings
    glShaderSource(shader, 1, &text, NULL);

    // compile shader
    // transform source code in executable code
    glCompileShader(shader);
    check_compile_status(shader);
    return shader;
}
void init_OpenGL_functions()
{
    gladLoadGL();
    GLuint status_ok;
    GLchar status_log[2000];
    GLuint f_shader = load_shader("shader_fragment.txt", GL_FRAGMENT_SHADER);
    GLuint v_shader = load_shader("shader_vertex.txt", GL_VERTEX_SHADER);

    // create program object (just program)
    prog = glCreateProgram();
    glAttachShader(prog, f_shader);
    glAttachShader(prog, v_shader);

    // link all shader in program
    glLinkProgram(prog);

    // check link status
    check_link_status(f_shader);
    check_link_status(v_shader);

    // apply the program for next calc
    /*  теперь для отрисовки будут вместо стандартных шейдеров
        исполняться те, которые есть в указанной программе
    */
    glUseProgram(prog);
    /**
     * если мы не хотим больше использовать шейдер то
     * glUseProgram(0);
     * в этом случае будут исполнятья стандартные шейдеры
     */
     glGenBuffers(1, &__color_VBO);
     glBindBuffer(GL_ARRAY_BUFFER, __color_VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(__colors), __colors, GL_STATIC_DRAW);
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
                          0,
                          0,
                          720,
                          720,
                          NULL,
                          NULL,
                          hInstance,
                          NULL);

    ShowWindow(hwnd, nCmdShow);

    /* enable OpenGL for the window */
    EnableOpenGL(hwnd, &hDC, &hRC);
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


            //show_triangle_1();
            show_triangle_2();

            SwapBuffers(hDC);
            theta += 1.0f;
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

