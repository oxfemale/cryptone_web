#include "stdafx.h"
#include "globalvars.h"
#include "UserRegistration.h"
#include "Randoms.h"
#include "Container.h"
#include "AskUsername.h"
#include "AskPasswords.h"
#include "http.h"
#include "PacketFactory.h"
#include "AddNewClient.h"
#include "ClientFunctions.h"
#include "SystemInfo.h"

void ConsoleOutput( char* Info, int flag )
{
    gotoxy(0, 1);
    clear_screen(0, 0);
    gotoxy(0, 0);
    if (flag == 0 )printf("INFO: %s", Info);
    if (flag == 1) printf("ERROR: %s", Info);
    gotoxy(0, 14);
    return;
}

void clear_screen(int x, int y)
{
    HANDLE hndl = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hndl, &csbi);
    DWORD written;
    DWORD n = csbi.dwSize.X * csbi.dwCursorPosition.Y + csbi.dwCursorPosition.X + 1;
    COORD curhome = { x,y };
    FillConsoleOutputCharacter(hndl, ' ', n, curhome, &written);
    csbi.srWindow.Bottom -= csbi.srWindow.Top;
    csbi.srWindow.Top = 0;
    SetConsoleWindowInfo(hndl, TRUE, &csbi.srWindow);
    SetConsoleCursorPosition(hndl, curhome);
}

void gotoxy(int x, int y)
{
    static HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    static COORD WritePos;
    WritePos.X = x; //-1 for borland compatibility??
    WritePos.Y = y;
    SetConsoleCursorPosition(hStdOut, WritePos);
}
