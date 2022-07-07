#include <stdio.h>
#include <stdlib.h>

#define _WIN32_WINNT 0x0A00
#include <windows.h>


int main()
{
    HWND hwnd =  GetConsoleWindow();
    HDC dc =  GetDC(hwnd);

    // Структура для записи координат мыши
    LPPOINT pPnt;
    pPnt = (LPPOINT)malloc(sizeof(*pPnt));

    // получение размеров клиентского окна
    LPRECT rct;
    rct = (LPRECT)malloc(sizeof(*rct));
    GetClientRect(hwnd, rct);

    // получение размеров рабочего стола
    LPRECT rctTable;
    rctTable =  (LPRECT)malloc(sizeof(*rctTable));
    GetClientRect(GetDesktopWindow(), rctTable);

    do {
        system("cls");

        // получание глобальных координат мыши
        GetCursorPos(pPnt);
        printf("GCoordinates: left = %d, top = %d\n", pPnt->x, pPnt->y);

        // преобразует глобальные координаты мыши в локальные
        ScreenToClient(hwnd, pPnt);
        printf("LCoordinates: left = %d, top = %d\n", pPnt->x, pPnt->y);

        printf("ClientWidth: %d, ClientHeight: %d\n", rct->right, rct->bottom);
        printf("TableWidth: %d, TableHeight: %d\n", rctTable->right, rctTable->bottom);

        // проверяем нажата ли левая кнопка мыши
        if(GetKeyState(VK_LBUTTON) < 0) {
            printf("\n Have been pressed LButton on mouse!\n");
        }


        Sleep(100);
    }
    while(GetKeyState(VK_ESCAPE) >= 0);


    printf("Hello world!\n");
    return 0;
}
