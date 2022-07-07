#include <windows.h>
#include <gl/gl.h>
#include <math.h>
#include <stdbool.h>
#include <time.h>
#include <mmsystem.h>

LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
void EnableOpenGL(HWND hwnd, HDC*, HGLRC*);
void DisableOpenGL(HWND, HDC, HGLRC);

int width = 1080, height = 720;
HWND hwnd_horeground;
float xScaleFactory = 1;
POINT base_sursor;
bool show_mask = false;

typedef struct {
    float r,g,b;
} TColor;

typedef struct {
    TColor color;
} TCell;

#define pW 40
#define pH 40
TCell cell_map[pW][pH];

GLfloat array_vertexes_cube[] = {
    0,0,0,  0,1,0,  1,1,0,  1,0,0,
    0,0,1,  0,1,1,  1,1,1,  1,0,1,
};
GLuint array_indexes_cube[] = {
    0,1,2,  2,3,0,
    4,5,6,  6,7,4,
    0,4,7,  7,3,0,
    1,5,6,  6,2,1,
    3,7,6,  6,2,3,
    0,4,5,  5,1,0
};
struct {
    float x, y, z;
    float xAlpha, zAlpha;
} camera = {0,0,1.7,  75,-45};
const float base_camera_z = 1.7;

#define EN_CNT 80
struct {
    float x, y, z;
    bool is_active;
} array_enemies[EN_CNT];

void draw_cursor()
{
    glPushMatrix();
        glColor3f(0.1, 0.1, 0);
        glBegin(GL_LINES);
            glVertex3f(-0.05,0,-3);
            glVertex3f(0.05,0,-3);

            glVertex3f(0,-0.05,-3);
            glVertex3f(0,0.05,-3);
        glEnd();
    glPopMatrix();
}
void keyboard_handlers()
{
    if(GetKeyState('L') < 0)
    {
        for(int i = 0; i < EN_CNT; i++)
            array_enemies[i].is_active = true;
    }
}
void init_enemy()
{
    for(int i = 0; i < EN_CNT; i++)
    {
        array_enemies[i].is_active = true;
        array_enemies[i].x = rand() % pW;
        array_enemies[i].y = rand() % pH;
        array_enemies[i].z = rand() % 7;
    }
}
void show_enemy()
{
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, array_vertexes_cube);
    for(int i = 0; i < EN_CNT; i++)
    {
        if(!array_enemies[i].is_active) continue;
        glPushMatrix();
            glTranslatef(array_enemies[i].x, array_enemies[i].y, array_enemies[i].z);
            glColor3ub(150 + i, 60, 43);
            glDrawElements(
                GL_TRIANGLES,
                36,
                GL_UNSIGNED_INT,
                array_indexes_cube
            );
        glPopMatrix();
    }
    glDisableClientState(GL_VERTEX_ARRAY);
}
void set_base_cursor(int x, int y)
{
    base_sursor.x = x;
    base_sursor.y = y;
    SetCursorPos(base_sursor.x, base_sursor.y);
}
void move_camera()
{
    float speed = 0;
    float alpha = -camera.zAlpha / 180 * M_PI;

    if(GetKeyState('W') < 0) speed += 0.1;
    if(GetKeyState('S') < 0) speed -= 0.1;

    if(GetKeyState('D') < 0) {speed = 0.1; alpha += M_PI / 2;};
    if(GetKeyState('A') < 0) {speed = 0.1; alpha -= M_PI / 2;};

    camera.y += cos(alpha) * speed;
    camera.x += sin(alpha) * speed;


    if(GetKeyState(VK_SPACE) < 0)
        camera.z += 0.1;
    else
    {
        if(camera.z > base_camera_z)
            camera.z -= 0.05;
        else
            camera.z = base_camera_z;
    }

    if(GetForegroundWindow() != hwnd_horeground) return;
    if(GetKeyState(VK_LCONTROL) < 0) return;

    POINT mouse_pos;
    GetCursorPos(&mouse_pos);

    int dx = mouse_pos.x - base_sursor.x;
    int dy = mouse_pos.y - base_sursor.y;

    if(dx > 0) camera.zAlpha -= dx / 4.;
    else if(dx < 0) camera.zAlpha -= dx / 4.;

    if(dy > 0) camera.xAlpha -= dy / 4.;
    if(dy < 0) camera.xAlpha -= dy / 4.;

    set_base_cursor(base_sursor.x, base_sursor.y);
}
void apply_camera()
{
    glRotatef(-camera.xAlpha, 1, 0, 0);
    glRotatef(-camera.zAlpha, 0, 0, 1);
    glTranslatef(-camera.x,-camera.y,-camera.z);
}
void resize_projection(int new_w, int new_h)
{
    width = new_w;
    height = new_h;
    xScaleFactory = (float)width / height;

    glViewport(0, 0, width, height);
    float size = 0.1;

    glLoadIdentity();
    glFrustum(-xScaleFactory*size,xScaleFactory*size,  -size,size,  2*size, 100);
}
void init_map()
{
    for(int i = 0; i < pW; i++)
    {
        for(int j = 0; j < pH; j++)
        {
            float diff_c = (rand() % 20) * 0.015;
            cell_map[i][j].color.r = 0.31 + diff_c;
            cell_map[i][j].color.g = 0.5 + diff_c;
            cell_map[i][j].color.b = 0.13 + diff_c;
        }
    }
}
void init_game(HWND hwnd)
{
    hwnd_horeground = hwnd;

    RECT window_rect;
    GetClientRect(hwnd, &window_rect);
    resize_projection(
        window_rect.right - window_rect.left,
        window_rect.bottom - window_rect.top
    );
    set_base_cursor(300, 400);

    init_map();
    init_enemy();
    glEnable(GL_DEPTH_TEST);
}
void show_game()
{
    if(show_mask)
        glClearColor(0.f, 0.f, 0.0f, 0.0f);
    else
        glClearColor(0.6f, 0.8f, 1.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    keyboard_handlers();

    if(!show_mask)
        draw_cursor();

    glPushMatrix();
        move_camera();
        apply_camera();

        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_FLOAT, 0, array_vertexes_cube);
        for(int i = 0; i < pW; i++)
        {
            for(int j = 0; j < pH; j++)
            {
                glPushMatrix();
                    if(show_mask)
                        glColor3f(0, 0, 0);
                    else
                        glColor3f(
                            cell_map[i][j].color.r,
                            cell_map[i][j].color.g,
                            cell_map[i][j].color.b
                        );
                    glTranslatef(i,j,0);
                    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, array_indexes_cube);
                glPopMatrix();
            }
        }
        glDisableClientState(GL_VERTEX_ARRAY);

        show_enemy();
    glPopMatrix();
}
void player_shoot()
{
    show_mask = true;
    show_game();
    show_mask = false;

    RECT rect;
    GetClientRect(hwnd_horeground, &rect);

    GLubyte color[3];
    glReadPixels(rect.right / 2.0, rect.bottom / 2.0,
        1, 1, GL_RGB, GL_UNSIGNED_BYTE, color
    );

    if(color[0] > 0)
        array_enemies[color[0] - 150].is_active = false;

    PlaySound("shoot.wav", NULL, SND_ASYNC);
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
                          width,
                          height,
                          NULL,
                          NULL,
                          hInstance,
                          NULL);

    ShowWindow(hwnd, nCmdShow);

    /* enable OpenGL for the window */
    EnableOpenGL(hwnd, &hDC, &hRC);

    init_game(hwnd);

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
        break;

        case WM_LBUTTONDOWN:
            player_shoot();
        break;

        case WM_SETCURSOR:
            if(GetKeyState(VK_LCONTROL) < 0)
                ShowCursor(true);
            else
                ShowCursor(false);
        break;

        case WM_SIZE:
            resize_projection(LOWORD(lParam), HIWORD(lParam));
        break;

        case WM_KEYDOWN:
        {
            switch (wParam)
            {
                case VK_ESCAPE:
                    PostQuitMessage(0);

                default:
                {
                    if(GetKeyState('F') < 0)
                    {
                        static bool is_full = false;

                        // full screen + delete border
                        if(!is_full)
                        {
                            // процедура устанавливает атрибуты окна
                            // GWL_STYLE - изменяемый атрибут стили
                            SetWindowLongPtr(hwnd_horeground, GWL_STYLE, WS_POPUP | WS_VISIBLE);

                            static RECT screen;
                            if(screen.right == 0 && screen.bottom == 0)
                                GetWindowRect(GetDesktopWindow(), &screen);
                            // SetWindowPos - позволяет задать позицию и размер окна
                            // HWND_TOP - наше окно поверх других окон
                            // SWP_SHOWWINDOW - установить новые атрибуты и отобразить
                            SetWindowPos(hwnd_horeground, HWND_TOP, 0, 0, screen.right, screen.bottom, SWP_SHOWWINDOW);
                            is_full = true;
                        }
                        else
                        {
                            // возвращаем дефолтные стили с рамкой
                            SetWindowLongPtr(hwnd_horeground, GWL_STYLE, WS_OVERLAPPEDWINDOW | WS_VISIBLE);
                            SetWindowPos(hwnd_horeground, HWND_TOP, 0, 0, 1080, 720, SWP_SHOWWINDOW);
                            is_full = false;
                        }
                    }
                }
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

