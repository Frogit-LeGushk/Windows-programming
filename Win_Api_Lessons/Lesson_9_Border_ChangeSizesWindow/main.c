#include <stdio.h>
#include <stdlib.h>

#define _WIN32_WINNT 0x0A00
#include <windows.h>

LRESULT castomHandler(HWND hwnd ,UINT message, WPARAM wparam, LPARAM lparam) {
    if(message == WM_DESTROY) {
        PostQuitMessage(0);
    }
    else if(message == WM_KEYDOWN) {

        printf("Code symbol: %d\n", wparam);
        if(wparam == VK_ESCAPE) PostQuitMessage(0);

        // full screen + delete border
        if(wparam == '1') {
            // процедура устанавливает атрибуты окна
            // GWL_STYLE - изменяемый атрибут стили
            SetWindowLongPtr(hwnd, GWL_STYLE, WS_POPUP | WS_VISIBLE);

            // SetWindowPos - позволяет задать позицию и размер окна
            // HWND_TOP - наше окно поверх других окон
            // SWP_SHOWWINDOW - установить новые атрибуты и отобразить
            SetWindowPos(hwnd, HWND_TOP, 0, 0, 1280, 720, SWP_SHOWWINDOW);
        }

        if(wparam == '2') {
            // возвращаем дефолтные стили с рамкой
            SetWindowLongPtr(hwnd, GWL_STYLE, WS_OVERLAPPEDWINDOW | WS_VISIBLE);
            SetWindowPos(hwnd, HWND_TOP, 0, 0, 640, 480, SWP_SHOWWINDOW);
        }
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
                             wndclassa.lpszClassName,
                             "Name of window's",
                             WS_OVERLAPPEDWINDOW,
                             10, 10,
                             640, 480,
                             NULL, NULL, NULL, NULL
                             );


    ShowWindow(hwnd, SW_SHOWNORMAL);

    MSG msg;
    while(GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }


    /*
        Чтобы отключить консольное окно заходим Project/properties/build target/type
        Меняем console application to GUI
        Жмем иконку "rebuild"
    */
    return 0;
}
