#include <stdio.h>
#include <stdlib.h>

#define _WIN32_WINNT 0x0A00
#include <windows.h>

int main()
{
    // 1) получаем дескриптор консольного окна
    HWND hwnd =  GetConsoleWindow();

    // 2) получаем Дескриптор рабочего стола
    //HWND hwnd = GetDesktopWindow();

    // 3) получаем дескриптор рабочего стола
    //HWND hwnd = FindWindow(NULL, "Калькулятор");

    // получаем контекст устройства
    HDC hdc = GetDC(hwnd);


    //Rectangle(hdc, 0, 0, 200, 200);
    //Ellipse(hdc, 1400, 110, 1500, 210);

    int i = 0;
    const char *message = "Frog ANd Bug";

    do {
        // Устанавливает объект в качестве текущего
        SelectObject(hdc, GetStockObject(DC_BRUSH));
        // Назначаем объекту цвет кисти
        SetDCBrushColor(hdc, RGB(10, 10, 10));
        // GetStockObject(DC_PEN) - обводка примитива
        SelectObject(hdc, GetStockObject(DC_PEN));
        SetDCPenColor(hdc, RGB(255, 0, 255));
        // Рисуем фон
        Rectangle(hdc, 0, 0, 640, 300);

        // Рисуем движущийся квадрат
        SelectObject(hdc, GetStockObject(DC_BRUSH));
        SetDCBrushColor(hdc, RGB(0, 255, 0));
        Rectangle(hdc, 0 + i, 0, 640 + i, 300);
        i++;

        TextOutA(hdc, 20, 50, message, strlen(message) + 1);


        Sleep(10);
    }
    while(GetKeyState(VK_ESCAPE) >= 0);

    return 0;
}
