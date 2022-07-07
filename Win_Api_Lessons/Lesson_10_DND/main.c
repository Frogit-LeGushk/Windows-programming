#include <stdio.h>
#include <stdlib.h>

#define _WIN32_WINNT 0x0A00
#include <windows.h>


void proccessDnD(HDROP hdr) {
    // координаты мыши
    POINT pt;
    DragQueryPoint(hdr, &pt);
    printf("Coordinates: [%d, %d]\n", pt.x, pt.y);


    char path[MAX_PATH];
    // DragQueryFile - позвол€ет узнать имена файлов и их количество
    // 0xffffffff - указать количество файлов
    int cnt = DragQueryFile(hdr, 0xffffffff, NULL, 0);
    printf("cnt=%d\n", cnt);


    // узнаем пути до фалов
    for(int i = 0; i < cnt; i++) {
        DragQueryFile(hdr, i, path, MAX_PATH);
        printf("Name[%d]=%s\n", i, path);
    }

    DragFinish(hdr);
}

LRESULT castomHandler(HWND hwnd ,UINT message, WPARAM wparam, LPARAM lparam) {
    if(message == WM_DESTROY) {
        PostQuitMessage(0);
    }
    else if(message == WM_KEYDOWN) {
        printf("Code symbol: %d\n", wparam);
    }
    else if(message == WM_DROPFILES) {
        printf("Drop!\n");
        proccessDnD(wparam);
    }
    else if(message == WM_MOUSEMOVE) {
        int X = LOWORD(lparam);
        int Y = HIWORD(lparam);
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

    HWND hwnd = CreateWindow(
                             wndclassa.lpszClassName, // им€ класса окна, зарегестрированного ранее
                             "Name of window's",        // «аголовок окна дл€ пользовател€
                             WS_OVERLAPPEDWINDOW, // стиль окна
                             10, 10, // координаты верхнего левого окна при его создании
                             640, 480, // размеры окна
                             NULL, NULL, NULL, NULL
                             );


    ShowWindow(hwnd, SW_SHOWNORMAL);

    // –азрешаем ƒЌƒ
    DragAcceptFiles(hwnd, TRUE);

    MSG msg;
    while(GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }


    return 0;
}
