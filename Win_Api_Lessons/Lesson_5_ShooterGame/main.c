#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <math.h>

#define _WIN32_WINNT 0x0A00
#include <windows.h>

#define WSize 800
#define HSize 600
#define PS 6
#define ENEMIES_SIZE 1000
#define BULLETS_SIZE 1000
#define MAX_RANGE_BULLET (HSize / 2)

#define ENEMIES_GENERATE 50


// DECLARATIONS
typedef struct {
    float x;
    float y;
} TPoint;

typedef struct SObject {
    TPoint pos;
    TPoint size;
    TPoint speed;
    TPoint fixed;

    COLORREF brush;
    COLORREF pen;

    bool type;
    bool deleted;
} TObject;


TObject player;
TObject enemies[ENEMIES_SIZE];
TObject bullets[BULLETS_SIZE];
int topEnemies = 0;
float fxyNoop = -1;
TPoint cam;


// HANDLERS
TPoint point(float x, float y) {
    TPoint tp;
    tp.x = x;
    tp.y = y;
    return tp;
}

void setFocusToPlayer(void) {
    cam.x = player.pos.x - WSize / 2;
    cam.y = player.pos.y - HSize / 2;
}


// OBJECT'S FUNCTIONS
void objectInit(TObject *obj, float x, float y, float w, float h, bool type, int fx, int fy) {
    assert(obj && w > 0 && h > 0);

    obj->pos= point(x, y);
    obj->size = point(w, h);
    obj->speed = point(0, 0);
    obj->fixed = point(fx, fy);

    if(type) {
        obj->brush = RGB(0, 255, 0);
        obj->pen = RGB(0, 0, 0);
    }
    else {
        obj->brush = RGB(255, 0, 0);
        obj->pen = RGB(0, 0, 0);
    }

    obj->type = type;
    obj->deleted = false;
}

void objectControl(TObject *obj) {
    assert(obj);

    static float playerSpeed = PS;
    static float enemySpeed = PS * 0.8;
    static float prSpeed = sqrt(2) / 2;

    if(obj->type == true && obj->fixed.x == fxyNoop && obj->fixed.y == fxyNoop) {
        obj->speed.x = 0;
        obj->speed.y = 0;

        if(GetKeyState('W') < 0) obj->speed.y = -playerSpeed;
        if(GetKeyState('S') < 0) obj->speed.y = playerSpeed;

        if(GetKeyState('A') < 0) obj->speed.x = -playerSpeed;
        if(GetKeyState('D') < 0) obj->speed.x = playerSpeed;

        if(obj->speed.x != 0 && obj->speed.y != 0)
            obj->speed = point(obj->speed.x * prSpeed, obj->speed.y * prSpeed);
    }
    else if(obj->type == false && obj->fixed.x == fxyNoop && obj->fixed.y == fxyNoop) {
        for(int i = 0; i < topEnemies; i++) {
            if(rand() % 25 == 1) {
                float dx = player.pos.x - enemies[i].pos.x;
                float dy = player.pos.y - enemies[i].pos.y;

                if(dx > 0) enemies[i].speed.x = enemySpeed;
                else enemies[i].speed.x = -enemySpeed;

                if(dy > 0) enemies[i].speed.y = enemySpeed;
                else enemies[i].speed.y = -enemySpeed;

                if(enemies[i].speed.x != 0 && enemies[i].speed.y != 0)
                    enemies[i].speed = point(enemies[i].speed.x * prSpeed, enemies[i].speed.y * prSpeed);
            }
        }
    }
}

void objectCalc(TObject *obj) {
    assert(obj);

    objectControl(obj);
    obj->pos.x += obj->speed.x;
    obj->pos.y += obj->speed.y;
}

void objectRender(TObject *obj, HDC hdc) {
        assert(obj && hdc);

        SelectObject(hdc, GetStockObject(DC_BRUSH));
        SetDCBrushColor(hdc, obj->brush);
        SelectObject(hdc, GetStockObject(DC_PEN));
        SetDCPenColor(hdc, obj->pen);

        TPoint start = point(obj->pos.x - (obj->size.x) / 2, obj->pos.y - (obj->size.y) / 2);
        TPoint end = point(obj->pos.x + (obj->size.x) / 2, obj->pos.y + (obj->size.y) / 2);

        if(obj->type)
            Rectangle(hdc, start.x - cam.x, start.y - cam.y, end.x - cam.x, end.y - cam.y);
        else
            Ellipse(hdc, start.x - cam.x, start.y - cam.y, end.x - cam.x, end.y - cam.y);

}





// RENDER FUNCTIONS
void initState(void) {
    cam = point(player.pos.x, player.pos.y);
    objectInit(&player, 100, 100, 40, 40, true, fxyNoop, fxyNoop);
    srand(time(NULL));

    for(int i = 0; i < ENEMIES_GENERATE; i++) {
        int x = rand() % 5000;
        int y = rand() % 5000;

        int sx = rand() % 10;
        int sy = rand() % 10;

        if(sx >= 5) x = abs(x);
        else x = -abs(x);

        if(sy >= 5) y = abs(y);
        else y = -abs(y);


        float dx = x - 100;
        float dy = y - 100;

        float length = sqrt(dx * dx + dy * dy);

        if(length > 400)
            objectInit(enemies + (topEnemies ++), x, y, 30, 30, false, fxyNoop, fxyNoop);
        else
            i--;
    }

    for(int i = 0; i < BULLETS_SIZE; i++)
        bullets[i].deleted = true;
}

void calcState(void) {
    if(!player.deleted)
        objectCalc(&player);

    setFocusToPlayer();

    for(int i = 0; i < topEnemies; i++)
        if(!enemies[i].deleted)
            objectCalc(enemies + i);

    for(int i = 0; i < BULLETS_SIZE; i++) {
        if(!bullets[i].deleted) {

            for(int j = 0; j < topEnemies; j++) {
                if(!enemies[j].deleted) {
                    float dx = bullets[i].pos.x - enemies[j].pos.x;
                    float dy = bullets[i].pos.y - enemies[j].pos.y;
                    float length = sqrt(dx * dx + dy * dx);

                    if(length < enemies[j].size.x / 2 || length < enemies[j].size.y / 2) {
                        enemies[j].deleted = true;
                        bullets[i].deleted = true;
                    }
                }
            }

            if(!bullets[i].deleted) {
                objectCalc(bullets + i);

                float dx = bullets[i].pos.x - bullets[i].fixed.x;
                float dy = bullets[i].pos.y - bullets[i].fixed.y;
                float length = sqrt(dx * dx + dy * dx);

                if(length < 10.0f || length > HSize)
                    bullets[i].deleted = true;
            }
        }
    }
}

void renderState(HDC hdc) {
    // виртуальный контекст для рисования в памяти
    HDC memDc = CreateCompatibleDC(hdc);
    // создаем картинку на которой будем рисовать через виртуальный контекст
    HBITMAP memBm = CreateCompatibleBitmap(hdc, WSize, HSize);
    // выбираем нашу картинку в качестве контекста для рисования
    SelectObject(memDc, memBm);


    // закрашиваем фон
    SelectObject(memDc, GetStockObject(DC_PEN));
    SetDCBrushColor(memDc, RGB(0, 0, 0));

    SelectObject(memDc, GetStockObject(DC_BRUSH));
    SetDCBrushColor(memDc, RGB(55, 255, 255));

    int squareSize = 200;
    int dx = (int)(cam.x) % squareSize;
    int dy = (int)(cam.y) % squareSize;

    for(int i = -1; i < (WSize / squareSize) + 2; i++) {
        for(int k = -1; k < (HSize / squareSize) + 2; k++) {
            Rectangle(memDc, -dx + i * squareSize, -dy + k * squareSize, -dx + (i + 1) * squareSize, -dy + (k + 1) * squareSize);
        }
    }



    // рисуем игрока
    if(!player.deleted)
        objectRender(&player, memDc);
    // рисуем врагов
    for(int i = 0; i < topEnemies; i++)
        if(!enemies[i].deleted)
            objectRender(enemies + i, memDc);
    // рисуем пули если они есть
    for(int i = 0; i < BULLETS_SIZE; i++)
        if(!bullets[i].deleted)
            objectRender(bullets + i, memDc);





    // копируем изображение из контекста memDc --to--> hdc
    BitBlt(hdc, 0, 0, WSize, HSize, memDc, 0, 0, SRCCOPY);
    // очищаем выделенную память
    DeleteDC(memDc);
    DeleteObject(memBm);
}



LRESULT castomHandler(HWND hwnd ,UINT message, WPARAM wparam, LPARAM lparam) {
    if(message == WM_DESTROY) {
        PostQuitMessage(0);
    }

    // KEYBOARD EVENTS
    if(message == WM_KEYDOWN) {
        //printf("Keyboard code symbol: %d\n", wparam);
    }
    if(message == WM_CHAR) {
        //printf("Keyboard symbol: %c\n", wparam);
    }

    // MOUSE EVENTS
    if(message == WM_MOUSEMOVE) {

    }
    if(message == WM_LBUTTONDOWN) {
        int x = LOWORD(lparam);
        int y = HIWORD(lparam);
        printf("Mouse [%d, %d]\n", x, y);

        for(int i = 0; i < BULLETS_SIZE; i++) {
            if(bullets[i].deleted) {
                objectInit(bullets + i, player.pos.x, player.pos.y, player.size.x / 4, player.size.y / 4, player.type, x, y);
                float dx = x - player.pos.x + cam.x;
                float dy = y - player.pos.y + cam.y;
                float length = sqrt(dx * dx + dy * dy);
                float cosX = dx / length;
                float cosY = dy / length;

                bullets[i].speed = point(PS * 3 * cosX, PS * 3 * cosY);
                break;
            }
        }
    }
    if(message == WM_RBUTTONDOWN) {
        //printf("Right button click!\n");
    }

    // Other events process default handler
    return DefWindowProcA(hwnd, message, wparam, lparam);
}

int main()
{
    WNDCLASSA wndclassa;
    memset(&wndclassa, 0, sizeof(WNDCLASSA));
    wndclassa.lpszClassName = "Window class A";
    wndclassa.lpfnWndProc = castomHandler;
    // LoadCursorA - возвращает указатель выбранного курсора
    // IDC_CROSS - курсор в виде крестика
    wndclassa.hCursor = LoadCursorA(NULL, IDC_CROSS);

    RegisterClassA(&wndclassa);

    HWND hwnd = CreateWindow(
                             wndclassa.lpszClassName,
                             "Name of window's",
                             WS_OVERLAPPEDWINDOW,
                             0, 0,
                             WSize, HSize,
                             NULL, NULL, NULL, NULL
                             );


    HDC hdc = GetDC(hwnd);
    ShowWindow(hwnd, SW_SHOWNORMAL);

    initState();

    MSG msg;
    while(true) {
        if(PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE)) {
            if(msg.message == WM_QUIT) break;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else {
            calcState();
            renderState(hdc);
            Sleep(10);
        }
    }


    return 0;
}
