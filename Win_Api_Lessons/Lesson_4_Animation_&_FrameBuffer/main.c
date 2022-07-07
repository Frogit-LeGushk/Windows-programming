#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define _WIN32_WINNT 0x0A00
#include <windows.h>

#define WSize 800
#define HSize 600
#define sizeSquare 150

#define mapSize 50
RECT map[mapSize];


void initMap(void) {
    srand(time(NULL));

    for(int i = 0; i < mapSize; i++) {
        map[i].left = rand() % (WSize - sizeSquare); // x start
        map[i].top = rand() % (HSize - sizeSquare); // y start
        map[i].right = map[i].left + sizeSquare;
        map[i].bottom = map[i].top + sizeSquare;
    }
}

void winMove(void) {
    const int dx = 3;

    for(int i = 0; i < mapSize; i++) {
        map[i].left += dx;
        if(map[i].left >= WSize) map[i].left = -sizeSquare;
        map[i].right = map[i].left + sizeSquare;
    }
}

void printMap(void) {
    for(int i = 0; i < mapSize; i++)
        printf("x1=%d, y1=%d, x2=%d, y2=%d\n", map[i].left, map[i].top, map[i].right, map[i].bottom);
}

void showSquaresInWindow(HDC hdc) {
    // виртуальный контекст для рисования в памяти
    HDC memDc = CreateCompatibleDC(hdc);
    // создаем картинку на которой будем рисовать через виртуальный контекст
    HBITMAP memBm = CreateCompatibleBitmap(hdc, WSize, HSize);

    // выбираем нашу картинку в качестве контекста для рисования
    SelectObject(memDc, memBm);

    // закрашиваем фон
    SelectObject(memDc, GetStockObject(DC_BRUSH));
    SetDCBrushColor(memDc, RGB(255, 255, 255));
    Rectangle(memDc, 0, 0, WSize, HSize);

    // опции для квадратов
    SelectObject(memDc, GetStockObject(DC_BRUSH));
    SetDCBrushColor(memDc, RGB(10, 255, 10));
    SelectObject(memDc, GetStockObject(DC_PEN));
    SetDCPenColor(memDc, RGB(10, 10, 255));

    // рисуем квадраты
    for(int i = 0; i < mapSize; i++)
        Rectangle(memDc, map[i].left, map[i].top, map[i].right, map[i].bottom);

    // копируем изображение из нашей виртуальной памяти в наше окно
    // т.е. из контекста memDc --to--> hdc

    // область вывода на контекст hdc
    //                        |        |                             |хотим скопировать все
    BitBlt(hdc, 0, 0, WSize, HSize, memDc, 0, 0, SRCCOPY);
    //             ^^                                    ^^
    // точка вывода изображ. на контексте hdc
    //
    // точка копирования изображ. на контексте memDc

    // очищаем выделенную память
    DeleteDC(memDc);
    DeleteObject(memBm);
}



LRESULT castomHandler(HWND hwnd ,UINT message, WPARAM wparam, LPARAM lparam) {
    // сообщение приходящее в ходе уничтожения окна
    if(message == WM_DESTROY) {
        // процедура создает сообщение в очереди сообщений о закрытии окна
        PostQuitMessage(0);
        // после такого сообщения "GetMessage" вернет false
    }
    // сообщение приходящее в ходе нажатия кнопки на клавиатуре
    else if(message == WM_KEYDOWN) {
        printf("Code symbol: %d\n", wparam);
    }
    // когда поступает символ
    else if(message == WM_CHAR) {
        printf("Symbol: %c\n", wparam);
    }
    // сообщение об изменении координат мыши внутри окна
    else if(message == WM_MOUSEMOVE) {
        int X = LOWORD(lparam);
        int Y = HIWORD(lparam);
        printf("[%d, %d]\n", X, Y);
    }

    else if(message == WM_LBUTTONDOWN) {
        printf("Left button mouse's have been pressed!\n");
    }

    else return DefWindowProcA(hwnd, message, wparam, lparam);
}

int main()
{
    WNDCLASSA wndclassa;
        memset(&wndclassa, 0, sizeof(WNDCLASSA));
        wndclassa.lpszClassName = "Window class A";
        wndclassa.lpfnWndProc = castomHandler;
    RegisterClassA(&wndclassa);

    // создает окно и возвращает его дескриптор
    HWND hwnd = CreateWindow(
                             wndclassa.lpszClassName, // имя класса окна, зарегестрированного ранее
                             "Name of window's",        // Заголовок окна для пользователя
                             WS_OVERLAPPEDWINDOW, // стиль окна
                             0, 0, // координаты верхнего левого окна при его создании
                             WSize, HSize, // размеры окна
                             NULL, NULL, NULL, NULL
                             );

    // создаем контекст устройства, чтобы рисовать в нем
    HDC hdc = GetDC(hwnd);

    // показывает окно, SW_SHOWNORMAL - при первом показе окна
    ShowWindow(hwnd, SW_SHOWNORMAL);

    initMap();
    printMap();

    MSG msg;
    while(1) {
        // проверяет наличие сообщений в очереди, если они есть - записывает в MSG
        // иначе возвращает false
        // PM_REMOVE - после получения сообщения оно будет удалено
        if(PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE)) {

            if(msg.message == WM_QUIT) break;

            // если приходит сообщение нажатия символа, преобразует код сообщения в символ
            // и генерирует новое сообщение
            TranslateMessage(&msg);
            // передаем полученное сообщение нашему окну
            DispatchMessage(&msg);
        }
        else {
            winMove();
            showSquaresInWindow(hdc);
            Sleep(10);
        }
    }


    /*
        Чтобы отключить консольное окно заходим Project/properties/build target/type
        Меняем console application to GUI
        Жмем иконку "rebuild"
    */

    printf("Hello world!\n");
    return 0;
}
