#include <windows.h>
#include <glad/glad.h>
#include <stdio.h>

LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
void EnableOpenGL(HWND hwnd, HDC*, HGLRC*);
void DisableOpenGL(HWND, HDC, HGLRC);

GLuint prog;
typedef struct {
    float x,y,z;
} Tftp;
#define array_flakes_vertexes_count 2000
Tftp array_flakes_vertexes[array_flakes_vertexes_count];
GLuint flakes_VBO;

void init_array_flakes()
{
    for(int i = 0; i < array_flakes_vertexes_count; i++)
    {
        array_flakes_vertexes[i].x = (rand() % 2001) * 0.001 - 1;
        array_flakes_vertexes[i].y = (rand() % 2001) * 0.001 - 1;
        array_flakes_vertexes[i].z = (rand() % 2001) * 0.001 - 1;
    }
    glGenBuffers(1, &flakes_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, flakes_VBO);
        glBufferData(
            GL_ARRAY_BUFFER,
            sizeof(array_flakes_vertexes),
            array_flakes_vertexes,
            GL_STATIC_DRAW
        );
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
void show_array_flakes()
{
    static float y = 0;
    y -= 0.005;
    if(y < -2) y = 0;

    glBindBuffer(GL_ARRAY_BUFFER, flakes_VBO);
        glVertexPointer(3, GL_FLOAT, 0, NULL);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glUseProgram(prog);
    glPointSize(2);
    glColor3f(2,y,0);
    glEnableClientState(GL_VERTEX_ARRAY);
        glDrawArrays(GL_POINTS, 0, array_flakes_vertexes_count);
    glDisableClientState(GL_VERTEX_ARRAY);
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
    init_array_flakes();
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
                          1280,
                          720,
                          NULL,
                          NULL,
                          hInstance,
                          NULL);

    ShowWindow(hwnd, nCmdShow);

    RECT rectangle;
    GetClientRect(GetDesktopWindow(), &rectangle);
    SetWindowLongPtr(hwnd, GWL_STYLE, WS_POPUP | WS_VISIBLE);
    SetWindowPos(hwnd, HWND_TOP,
        rectangle.left, rectangle.top, rectangle.right, rectangle.bottom,
        SWP_SHOWWINDOW
    );


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

            /*
            glPushMatrix();
            glRotatef(theta, 0.0f, 0.0f, 1.0f);

            glUseProgram(prog);
            glBegin(GL_TRIANGLES);
                glColor3f(1.0f, 0.0f, 0.0f);   glVertex2f(0.0f,   1.0f);
                glColor3f(0.0f, 1.0f, 0.0f);   glVertex2f(0.87f,  -0.5f);
                glColor3f(0.0f, 0.0f, 1.0f);   glVertex2f(-0.87f, -0.5f);
            glEnd();
            glPopMatrix();
            */
            show_array_flakes();

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

