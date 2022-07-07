#include <windows.h>
#include <glad/glad.h>
#include <stdio.h>

#define STB_IMAGE_IMPLEMENTATION
#include "../lib/stb-master/stb_image.h"
#include "../Lesson 14. module camera/_OpenGL/camera.h"

LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
void EnableOpenGL(HWND hwnd, HDC*, HGLRC*);
void DisableOpenGL(HWND, HDC, HGLRC);

GLuint prog;
int tex_1, tex_2, tex_glass_2, tex_sand, tex_stone_2, tex_Cube;
static int __width = 1200, __height = 700;
char *skybox[6] = {
    "textures/right.png",
    "textures/left.png",
    "textures/top.png",
    "textures/bottom.png",
    "textures/front.png",
    "textures/back.png"
};
GLfloat skybox_coord[] = {
    -1,-1,1,  1,-1,1,  1,1,1,  -1,1,1,
    -1,-1,-1,  1,-1,-1,  1,1,-1,  -1,1,-1,
};
GLuint skybox_indexes[] = {
    0,1,2, 2,3,0,  4,5,6, 6,7,4,
    1,5,6, 6,2,1,  0,4,7, 7,3,0,
    3,2,6, 6,7,3,  0,1,5, 5,4,0
};
#define skybox_indexes_cnt sizeof(skybox_indexes) / sizeof(GLuint)

void show_cube()
{
    glUseProgram(prog);
    //glEnable(GL_TEXTURE_CUBE_MAP);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, tex_Cube);

        glVertexPointer(3, GL_FLOAT, 0, skybox_coord);
        glTexCoordPointer(3, GL_FLOAT, 0, skybox_coord);

        int t = glGetUniformLocation(prog, "skybox");
        glUniform1i(t, 0);

        glPushMatrix();
        glRotatef(90, 1,0,0);
        glDisable(GL_DEPTH_TEST);
        glEnableClientState(GL_VERTEX_ARRAY);
        //glEnableClientState(GL_TEXTURE_COORD_ARRAY);
            glDrawElements(GL_TRIANGLES, skybox_indexes_cnt, GL_UNSIGNED_INT, skybox_indexes);
        glDisableClientState(GL_VERTEX_ARRAY);
        //glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        glEnable(GL_DEPTH_TEST);
        glPopMatrix();

    //glDisable(GL_TEXTURE_CUBE_MAP);
}
void load_texture_cube(char *filename[6], int *target)
{
    glGenTextures(1, target);
    glBindTexture(GL_TEXTURE_CUBE_MAP, *target);
        /**
         * появляется артефакт связанный с взятием цвета за границей текстуры
         *
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
         */
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


        int width, height, cnt;
        unsigned char *data;
        for(int i = 0; i < 6; i++)
        {
            data = stbi_load(filename[i], &width, &height, &cnt, 0);
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, width, height, 0,
                        cnt == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}
void show_triangle()
{
    glUseProgram(prog);
    /**
     * do not write glEnable(GL_TEXTRURE_2D)
     * because using define by shader code
     */
    // do active GL_TEXTURE0 - texture block
    glActiveTexture(GL_TEXTURE0);
    // for current texture block take tex_1 texture
    glBindTexture(GL_TEXTURE_2D, tex_1);

    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_2D, tex_2);


    int tex_A = glGetUniformLocation(prog, "texA");
    int tex_B = glGetUniformLocation(prog, "texB");
    // send texture (tex_1) contain in texture block GL_TEXTURE0
    // in shader into uniform variable "texA"
    // 0 - texture block
    glUniform1i(tex_A, 0);
    glUniform1i(tex_B, 1);

    int tc = glGetAttribLocation(prog, "userTC");
    glBegin(GL_TRIANGLE_FAN);
        glVertexAttrib2f(tc, 1.0f, 0.0f);   glVertex2f(1.0f,   1.0f);
        glVertexAttrib2f(tc, 1.0f, 1.0f);   glVertex2f(1.0f,  -1.0f);
        glVertexAttrib2f(tc, 0.0f, 1.0f);   glVertex2f(-1.0f, -1.0f);
        glVertexAttrib2f(tc, 0.0f, 0.0f);   glVertex2f(-1.0f, 1.0f);
    glEnd();
}
void load_texture(const char *filename, int *texture)
{
    int width,height,cnt;
    unsigned char *data = stbi_load(filename, &width, &height, &cnt, 0);

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
    camera.z = 0;
    camera.xAlpha = 90;

    gladLoadGL();
    GLuint status_ok;
    GLchar status_log[2000];
    GLuint f_shader = load_shader("shader_fragment.txt", GL_FRAGMENT_SHADER);
    GLuint v_shader = load_shader("shader_vertex.txt", GL_VERTEX_SHADER);

    prog = glCreateProgram();
    glAttachShader(prog, f_shader);
    glAttachShader(prog, v_shader);

    glLinkProgram(prog);

    check_link_status(f_shader);
    check_link_status(v_shader);

    glUseProgram(prog);
    load_texture("./textures/01.jpg", &tex_1);
    load_texture("./textures/02.jpg", &tex_2);
    load_texture("./textures/grass_2.jpg", &tex_glass_2);
    load_texture("./textures/sand.jpg", &tex_sand);
    load_texture("./textures/stone_2.jpg", &tex_stone_2);

    load_texture_cube(skybox, &tex_Cube);
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
                          __width,
                          __height,
                          NULL,
                          NULL,
                          hInstance,
                          NULL);

    ShowWindow(hwnd, nCmdShow);


    /* enable OpenGL for the window */
    EnableOpenGL(hwnd, &hDC, &hRC);
    init_OpenGL_functions();

    glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        float k = (float)__width / __height;
        glFrustum(-0.1*k,0.1*k,  -0.1,0.1,  0.2, 1000);
    glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

    glEnable(GL_DEPTH_TEST);

    // сращивание кубических текстур (для удаления артефактов на границах между частями)
    // т.е. пропадет черные ребра кубов
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
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
                //glRotatef(theta, 0.0f, 0.0f, 1.0f);
                //show_triangle();
                rotation_camera_mouse(500, 500, 0.1);
                apply_camera();

                show_cube();
            glPopMatrix();

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

