#include <stdio.h>
#include <stdlib.h>

#define _WIN32_WINNT 0x0A00
#include <windows.h>

#define IDBtn1 1
#define IDBtn2 2
#define IDInput 3
#define IDText 4
#define IDCombo 5

HWND button_quite;
HWND button_noop;
HWND input;
HWND text;
HWND combo;

int SELECTED_ITEM = 0;


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


    // при WM_COMMAND сообщении в переменнную wparam записывается сразу 2 значения
    else if(message == WM_COMMAND) {
        if(lparam == button_quite)
            PostQuitMessage(0);
        if(LOWORD(wparam) == IDBtn2)
            printf("Click noop!\n");


        // LOWORD - макрос извлекает первые 2 байта -> идентификатор объекта
        // wparam - 4-x байтовое число
        if(LOWORD(wparam) == IDInput) {
            printf("Input: printing...\n");

            char buffer[256];
            // копирует текст из заданного объекта в буффер
            GetWindowText(lparam, buffer, 256);

            // HIWORD - макрос извлекает последние 2 байта -> код уведомления
            int code =  HIWORD(wparam);

            if(code == EN_UPDATE) {
                SetWindowText(text, buffer);
                printf("Update code: %s\n", buffer);
            }
            if(code == EN_CHANGE) {
                printf("Change code: %s\n", buffer);
            }
        }


        if(LOWORD(wparam) == IDCombo) {
            // CBN_SELCHANGE - уведомление в случае выбора элемента из комбобокса
            if(HIWORD(wparam) == CBN_SELCHANGE) {
                // CB_GETCURSEL - позволяет получить текущий выбранный элемент
                SELECTED_ITEM = SendMessage(lparam, CB_GETCURSEL, 0, 0);

                printf("SELECTED_ITEM=%d\n", SELECTED_ITEM);
            }
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


    button_quite = CreateWindow("button", "quite",
                 WS_CHILD | WS_VISIBLE, 0, 0, 200, 100, hwnd, IDBtn1, NULL, NULL);

    button_noop = CreateWindow("button", "noop",
                 WS_CHILD | WS_VISIBLE, 200, 0, 200, 100, hwnd, IDBtn2, NULL, NULL);

    input = CreateWindow("Edit", "0",
                 WS_CHILD | WS_VISIBLE | ES_RIGHT | WS_BORDER, 0, 100, 200, 20, hwnd, IDInput, NULL, NULL);

    text = CreateWindow("Static", "start text noop",
                 WS_CHILD | WS_VISIBLE | WS_BORDER | SS_RIGHT , 200, 100, 200, 20, hwnd, IDText, NULL, NULL);

    combo = CreateWindow("COMBOBOX", "",
                 WS_CHILD | WS_VISIBLE, 0, 120, 200, 480 - 120, hwnd, IDCombo, NULL, NULL);


    // SendMessage - оправляет сообщение в указанное окно или визуальный элемент
    // CB_ADDSTRING - тип сообщения
    SendMessage(combo, CB_ADDSTRING, 0, "<undefined>");
    SendMessage(combo, CB_ADDSTRING, 0, "<undefined 2>");
    SendMessage(combo, CB_ADDSTRING, 0, "<undefined 3>");
    SendMessage(combo, CB_ADDSTRING, 0, "<undefined 4>");
    // CB_SETCURSEL, 0 - выбрать конкретный элемент и установить его
    SendMessage(combo, CB_SETCURSEL, 0, 0);

    MSG msg;
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
    return 0;
}
