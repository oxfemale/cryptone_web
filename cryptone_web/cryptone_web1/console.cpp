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
#include "console.h"

void ConsoleOutput(char* cLogFileName, char* cLogFuncName, int iCodeLine, char* cInfo, int flag)
{
	time_t rawtime;
	struct tm * timeinfo;

	time(&rawtime);
	timeinfo = localtime(&rawtime);

	gotoxy(0, 1);
	clear_screen(0, 0);
	gotoxy(0, 0);
	if (flag == 0)
	{
		printf("INFO: %s", cInfo);
		if (gLogFile != NULL)
		{
			fprintf(gLogFile, "TIME: %d:%.2d:%.2d %.2d:%.2d:%.2d INFO %s->%s() line %d: %s\r\n", 1900 + timeinfo->tm_year, 1+timeinfo->tm_mon, timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, cLogFileName, cLogFuncName, iCodeLine, cInfo);
		}
	}
	if (flag == 1)
	{
		printf( "ERROR: %s", cInfo);
		if (gLogFile != NULL)
		{
			fprintf(gLogFile, "TIME: %d:%.2d:%.2d %.2d:%.2d:%.2d ERROR: function: %s->%s() line %d: %s\r\n", 1900 + timeinfo->tm_year, 1+timeinfo->tm_mon, timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, asctime(timeinfo), cLogFileName, cLogFuncName, iCodeLine, cInfo);
		}
	}
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
    WritePos.X = x;
    WritePos.Y = y;
    SetConsoleCursorPosition(hStdOut, WritePos);
}
