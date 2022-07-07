#include <stdio.h>
#include <stdlib.h>

#define _WIN32_WINNT 0x0A00
#include <windows.h>

void testFromClipboard(char **str) {
    // IsClipboardFormatAvailable - провер€ет €вл€ютс€ ли данные из буффера обмена
    // заданным типом данных
    // CF_TEXT - формат 1 байтовых символов ANSI
    if(IsClipboardFormatAvailable(CF_TEXT)) {

        if(OpenClipboard(0)) {
            // возвращает указатель на данные заданного типа в буффере обмена
            char *buffer = GetClipboardData(CF_TEXT);
            int length = strlen(buffer) + 1;

            *str = malloc(length * sizeof(char));
            sprintf(*str, "%s", buffer);

            // буффер обмена нужно закрыть, т.к. пока он не закрыт мы его не можем
            // использовать в других приложени€х
            CloseClipboard();

            printf("Clipboard data: %s\n", *str);
        }
    }
}



int main()
{
    HWND hwnd = GetConsoleWindow();
    char *string = NULL;

    while(1) {
        if(GetForegroundWindow() != hwnd) continue;
        if(GetKeyState(VK_ESCAPE) < 0) break;
        if(GetKeyState(VK_LCONTROL) < 0) {
            if(GetKeyState('V') < 0) {
                testFromClipboard(&string);
            }
        }

        Sleep(5);
    }

    return 0;
}
