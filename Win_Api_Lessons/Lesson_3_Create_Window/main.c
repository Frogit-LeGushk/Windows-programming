#include <stdio.h>
#include <stdlib.h>

#define _WIN32_WINNT 0x0A00
#include <windows.h>

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
                             10, 10, // координаты верхнего левого окна при его создании
                             640, 480, // размеры окна
                             NULL, NULL, NULL, NULL
                             );

    // показывает окно, SW_SHOWNORMAL - при первом показе окна
    ShowWindow(hwnd, SW_SHOWNORMAL);

    MSG msg;
    // GetMessage - извлекает сообщение из очереди сообщений и сохраняет в "msg"
    // если сообщений нет, то функция будет ожидать, т.е. тело цикла будет исполняться, когда будет получено сообщение
    // если было получено сообщение о закрытии приложения - return false;
    while(GetMessage(&msg, NULL, 0, 0)) {
        // если приходит сообщение нажатия символа, преобразует код сообщения в символ
        // и генерирует новое сообщение
        TranslateMessage(&msg);
        // передаем полученное сообщение нашему окну
        DispatchMessage(&msg);
    }


    /*
        Чтобы отключить консольное окно заходим Project/properties/build target/type
        Меняем console application to GUI
        Жмем иконку "rebuild"
    */

    printf("Hello world!\n");
    return 0;
}
